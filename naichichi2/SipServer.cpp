//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include "SipServer.h"
#include "MainWindow.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"
#include "XLGZip.h"
#include "XLSocket.h"
#include "ScriptRemoconWeb.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
SipServer::SipServer()
{
	this->SipThread = NULL;
	this->StopFlag = false;
	this->isTalkingNow = false;
	this->isCalling = false;
	this->isIncoming = false;
	this->isSendRequest = false;
	this->SipWhenTakingStopRecogn = 2;
}
SipServer::~SipServer()
{
	DEBUGLOG("stop...");

	Stop();

	DEBUGLOG("done");
}

bool SipServer::Create()
{
	if ( ! MainWindow::m()->Config.GetBool("sip_enable", false ) )
	{
		NOTIFYLOG("sip server is disable");
		return false;
	}
	Stop();
	this->Auth = AUTH_PROCESSING_NOW;
	this->StopFlag = false;
	this->IncomingFrom = "";
	MainWindow::m()->Config.Set("sipevent_incoming","");

	//econetliteスレッド作成.
	DEBUGLOG("sip server...");
	this->SipThread = new thread([=](){
#if WITH_CLIENT_ONLY_CODE==1
		XLDebugUtil::SetThreadName("SipServer");
		this->SipThreadMain(); 
#endif //WITH_CLIENT_ONLY_CODE==1
	});

	return true;
}

void SipServer::Stop()
{
	this->StopFlag = true;
	if ( ! MainWindow::m()->Config.GetBool("sip_enable", false ) )
	{
		NOTIFYLOG("sip server is disable");
		return;
	}

	if (! sip.IsExiteProcess() )
	{
		SendRequest("QUIT");
		sip.Join();
		sip.Stop();

		this->SipThread->join();

		delete this->SipThread;
		this->SipThread = NULL;
	}
}

//SIP URLの構築
string SipServer::MakeSIPURL(const string& tel) const
{
	if( strncmp(tel.c_str() , "sip:",4)==0 )
	{//sip:1234@domain.local
		if (tel.find("@") != string::npos)
		{//1234@domain.local
			return tel;
		}
		return tel + "@" + this->SipDomain;
	}
	if (tel.find("@") != string::npos)
	{//1234@domain.local
		return "sip:" + tel;
	}
	//1234
	return "sip:" + tel + "@" + this->SipDomain;
}

//SIP URLの抽出
string SipServer::ParseSipAddress(const string& msg) const
{
	//sip:123123@rti-giken.jp --> sip:123123@rti-giken.jp
	const char* p = msg.c_str();
	const char* start = strstr(p,"<sip:");
	if(start == NULL)
	{//番号不明
		return "";
	}
	start = start +1; //skip <
	const char* end = strstr(start,">");
	if(end == NULL)
	{
		return start;
	}
	return string(start,0,end - start );
}

//SIP URLをできるだけ簡略表記.
string SipServer::EllipsisSipURL(const string& msg) const
{
	//sip:123123@rti-giken.jp --> 123123
	const char* p = msg.c_str();
	const char* start = strstr(p,"sip:");
	if(start == NULL)
	{//sip:がない？
		start = p;
	}
	else
	{
		start += sizeof("sip:") - 1;
	}

	const string domain = "@"+this->SipDomain;
	const char* end = strstr(start, domain.c_str() );
	if(end == NULL)
	{//ドメイン名が自ドメインではないのでそのまま.
		return start;
	}
	//ドメイン名が、自ドメインならば、ドメイン名を省略する.
	return string(start,0,end - start );
}
void SipServer::ShowNumpad(time_t now)
{
	MainWindow::m()->Config.Set("sipevent_numpad",num2str(now+120));
	MainWindow::m()->ColudSyncPooling.ConfigUpdated();
}
void SipServer::HideNumpad()
{
	MainWindow::m()->Config.Remove("sipevent_numpad");
	MainWindow::m()->ColudSyncPooling.ConfigUpdated();
}

//電話をかける.
void SipServer::Call(const string& tel)
{
	string sipurl = MakeSIPURL(tel);
	if ( IsTalkingNow() )
	{
		ERRORLOG("現在、あなたが話し中のため、指定されたSIPURL(" + sipurl + ")に電話をかけることができません");
		return;
	}
	if ( IsIncoming() )
	{
		ERRORLOG("電話がかかってきているので、電話できません。");
		return;
	}

	SendRequest("CALL\t" + sipurl);
	DEBUGLOG(sipurl);

	//アドレスの簡易表記
	this->CallTo = EllipsisSipURL(sipurl);

	const string mp3 = MainWindow::m()->Config.Get("sip_calling_mp3","sip_calling_ring.mp3");
	MainWindow::m()->MusicPlayAsync.Play(mp3,50);

	MainWindow::m()->Config.Remove("sipevent_numpad");
	MainWindow::m()->Config.Set("sipevent_incoming","");
	MainWindow::m()->Config.Set("sipevent_call",this->CallTo);
	MainWindow::m()->ColudSyncPooling.ConfigUpdated();
}
//かけた電話に返事があった
void SipServer::OnCall(const string& msg)
{
	MainWindow::m()->MusicPlayAsync.StopPlay();
	if(isErrorReslt(msg))
	{
		this->isIncoming = false;
		this->isTalkingNow = false;
		ERRORLOG("SIP CALL ERROR" << msg);
		NOTIFYLOG("@SIPTEL CALL_ERROR " << this->CallTo);

		MainWindow::m()->Config.Set("sipevent_incoming","");
		MainWindow::m()->Config.Set("sipevent_call","");
		MainWindow::m()->ColudSyncPooling.ConfigUpdated();
		return ;
	}

	this->isIncoming = false;
	this->isTalkingNow = true;
	NOTIFYLOG("@SIPTEL CALL " << this->CallTo);
	DEBUGLOG(msg);

	MainWindow::m()->Config.Set("sipevent_incoming","");
	MainWindow::m()->Config.Set("sipevent_call",this->CallTo);
	MainWindow::m()->ColudSyncPooling.ConfigUpdated();

	//通話中は音声認識を停止するべきかどうか
	MainWindow::m()->UpdateWhenTakingStopRecogn(this->SipWhenTakingStopRecogn,true);
}

//電話がかかってきたと通知された
void SipServer::OnIncoming(const string& msg)
{
	if(isErrorReslt(msg))
	{
		ERRORLOG("SIP INCOMING ERROR" << msg);
		return ;
	}

	//相手のアドレス
	const string sipurl = msg;
	//アドレスの簡易表記
	const string tel = EllipsisSipURL(sipurl);

	if ( ! MainWindow::m()->Config.GetBool("sip_enable_incoming", false ) )
	{
		NOTIFYLOG("電話(" << sipurl << ")がかかってきましたが、着信は許可されていません。");
		this->Hangup();
		return ;
	}
	//着信音を鳴らす.
	const string mp3 = MainWindow::m()->Config.Get("sip_incoming_mp3","sip_incoming_ring.mp3");
	MainWindow::m()->MusicPlayAsync.Play(mp3,50);

	this->isIncoming = true;
	this->isTalkingNow = false;
	this->IncomingFrom = tel;
	this->CallTo = "";
	DEBUGLOG(msg);

	//かかってきた番号を入れることで、着信通知をします.
	MainWindow::m()->Config.Set("sipevent_incoming",tel);
	MainWindow::m()->ColudSyncPooling.ConfigUpdated();
}
//電話がかかってきて、電話に出た
void SipServer::OnAnswer(const string& msg)
{
	MainWindow::m()->MusicPlayAsync.StopPlay();
	if(isErrorReslt(msg))
	{
		NOTIFYLOG("@SIPTEL ANSWER_ERROR " << this->IncomingFrom);

		this->isIncoming = false;
		this->isTalkingNow = false;
		this->IncomingFrom = "";
		this->CallTo = "";
		MainWindow::m()->Config.Set("sipevent_incoming","");
		MainWindow::m()->Config.Set("sipevent_call","");
		MainWindow::m()->ColudSyncPooling.ConfigUpdated();
		return ;
	}
	NOTIFYLOG("@SIPTEL ANSWER " << this->IncomingFrom);

	this->isIncoming = false;
	this->isTalkingNow = true;

	MainWindow::m()->Config.Set("sipevent_incoming","");
	MainWindow::m()->Config.Set("sipevent_call",this->IncomingFrom);
	MainWindow::m()->ColudSyncPooling.ConfigUpdated();

	MainWindow::m()->UpdateWhenTakingStopRecogn(this->SipWhenTakingStopRecogn,true);
}
//電話が切られた
void SipServer::OnHangup(const string& msg)
{
	this->isIncoming = false;
	this->isTalkingNow = false;
	this->IncomingFrom = "";
	this->CallTo = "";

	MainWindow::m()->Config.Set("sipevent_incoming","");
	MainWindow::m()->Config.Set("sipevent_call","");
	MainWindow::m()->ColudSyncPooling.ConfigUpdated();

	MainWindow::m()->MusicPlayAsync.StopPlay();

	MainWindow::m()->UpdateWhenTakingStopRecogn(this->SipWhenTakingStopRecogn,false);
}
//認証の結果
void SipServer::OnAuth(const string& msg)
{
	this->isIncoming = false;
	this->isTalkingNow = false;
	if(isErrorReslt(msg))
	{
		this->Auth = AUTH_BAD;
		ERRORLOG("認証に失敗しました。" + msg);
		return ;
	}
	this->Auth = AUTH_OK;
}

//エラーが帰ってきたか？
bool SipServer::isErrorReslt(const string& msg) const
{
	if (strncmp(msg.c_str(),"200",3) == 0 ) 
	{//認証成功
		return false;
	}
	//エラー
	return true;
}

//電話がかかってきているか？
bool SipServer::IsIncoming()
{
	return this->isIncoming;
}

//電話で話しているか？
bool SipServer::IsTalkingNow()
{
	return this->isTalkingNow;
}

//電話に出る
void SipServer::Answer()
{
	SendRequest("ANSWER");
	MainWindow::m()->MusicPlayAsync.StopPlay();
}

//電話を切る
void SipServer::Hangup()
{
	SendRequest("HANGUP");
	MainWindow::m()->MusicPlayAsync.StopPlay();
	MainWindow::m()->UpdateWhenTakingStopRecogn(this->SipWhenTakingStopRecogn,false);
}

SipServer::AUTH SipServer::getSipAuth() const
{
	return this->Auth;
}

void SipServer::AuthRequest()
{
	const string id = MainWindow::m()->Config.Get("sip_id");
	const string password = MainWindow::m()->Config.Get("sip_password");
	const string domain = MainWindow::m()->Config.Get("sip_domain");

	//通話中には音声認識を止めるかどうか
	this->SipWhenTakingStopRecogn = MainWindow::m()->Config.GetInt("sip_when_taking_stop_recogn",2);

	this->Auth = AUTH_PROCESSING_NOW;
	this->SipDomain = domain;

	SendRequest("AUTH\t" + id + "\t" + domain + "\t" + password);
}

//認証の是非が出るまで待つ.
SipServer::AUTH SipServer::WaitForAuth(int timeoutSec)
{
	//認証の是非のチェック
	volatile SipServer::AUTH auth = SipServer::AUTH_PROCESSING_NOW;
	for(int i = 0 ; i < timeoutSec; i ++)
	{
		auth = this->getSipAuth();
		if (auth != SipServer::AUTH_PROCESSING_NOW)
		{
			return auth;
		}
		SecSleep(1);
	}
	return this->getSipAuth();
}

//リクエストの応答がくるまで待つ
bool SipServer::WaitForRequest(int timeoutSec)
{
	//認証の是非のチェック
	volatile bool is = this->isSendRequest;
	for(int i = 0 ; i < timeoutSec; i ++)
	{
		is = this->isSendRequest;
		if (!is)
		{
			return true;
		}
		SecSleep(1);
	}

	return !is;
}

void SipServer::SipThreadMain()
{
#if _MSC_VER
	const string sipcmdEXE = MainWindow::m()->GetConfigBasePath("fhc_sipcmd\\fhc_sipcmd.exe");
#else
	const string sipcmdEXE = MainWindow::m()->GetConfigBasePath("fhc_sipcmd/fhc_sipcmd");
#endif
	sip.Open(sipcmdEXE);
	SecSleep(1);

	AuthRequest();

	while(! this->StopFlag )
	{
		RecvRequest();
		this->isSendRequest = false;
	}
}

void SipServer::RecvRequest()
{
	const int BUFFER_SIZE = 2048;
	char buffer[BUFFER_SIZE] = {0};
	int i = 0;

	int loop = 0;
	const int LOOP_OVER = 10;
	for( ; loop < LOOP_OVER ; loop++ )
	{
		int rr = this->sip.Read(buffer+i,BUFFER_SIZE-i);
		if (rr <= -1)
		{
			return ;
		}
		if(this->StopFlag)
		{
			return ;
		}

		i += rr;
		if ( i > 0 && buffer[i-1] == '\n' )
		{
			break;
		}
		SecSleep(1);
	}

	vector<string> vec1 = XLStringUtil::split_vector("\r\n",buffer);
	for(auto it = vec1.begin(); it != vec1.end() ; it++)
	{
		NOTIFYLOG("応答:" << it->c_str() );
		if ( strncmp(it->c_str() , ">CALL\t",sizeof(">CALL\t")-1) == 0)
		{//電話をかけて応答を得られた
			OnCall(it->c_str() + sizeof(">CALL\t")-1);
		}
		else if ( strncmp(it->c_str() , ">AUTH\t",sizeof(">AUTH\t")-1) == 0)
		{//認証
			OnAuth(it->c_str() + sizeof(">AUTH\t")-1);
		}
		else if ( strncmp(it->c_str() , ">ANSWER\t",sizeof(">ANSWER\t")-1) == 0)
		{//電話がかかってきて、それに応答した
			OnAnswer(it->c_str() + sizeof(">ANSWER\t")-1);
		}
		else if ( strncmp(it->c_str() , ">HANGUP\t",sizeof(">HANGUP\t")-1) == 0)
		{//電話終了
			OnHangup(it->c_str() + sizeof(">HANGUP\t")-1);
		}
		else if ( strncmp(it->c_str() , ">INCOMING\t",sizeof(">INCOMING\t")-1) == 0)
		{//電話がかかってきた。
			OnIncoming(it->c_str() + sizeof(">INCOMING\t")-1);
		}
		else if ( (it->c_str())[0] == '>' )
		{
			ERRORLOG("不明な応答:" << it->c_str() );
		}
	}
}

void SipServer::SendRequest(const string& cmd)
{
	NOTIFYLOG(cmd);
	this->isSendRequest = true;
	this->sip.Write(cmd+"\r\n");
}


#endif