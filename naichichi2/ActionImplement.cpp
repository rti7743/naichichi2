//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include "ActionImplement.h"
#include "XLHttpSocket.h"
#include "XLUrlParser.h"
#include "XLStringUtil.h"
#include "SystemMisc.h"
#include "FHCCommander.h"
#include "TwitterOAuth.h"
#include "MainWindow.h"

ActionImplement::ActionImplement(void)
{

}


ActionImplement::~ActionImplement(void)
{
}

string ActionImplement::Telnet(const string& host,const string& wait,const string& send,const string& recv)
{
	XLUrlParser urlparser;
	urlparser.Parse("telnet://"+host);

	int port = urlparser.getPort();
	if (port == -1) port = 23;

	vector<char> buffermalloc(2048*10);
	const unsigned int buffersize = buffermalloc.size() - 1;
	char * buffer = &buffermalloc[0];

	XLSocket socket;
	//タイムアウト5秒
	socket.SetTimeout(5);
	socket.Connect( urlparser.getHost() , port );

	//まずは相手の言い分を聞きます.
	if (!wait.empty())
	{
		unsigned int total_recv_size = 0;
		while(1)
		{
			int size = socket.Recv(buffer+total_recv_size,buffersize-total_recv_size);
			if (size <= 0)
			{//エラー
				break;
			}
			buffer[size] = '\0';
			total_recv_size += size;
			if(total_recv_size >= buffersize)
			{
				throw XLException("waitまでにあまりに長いデータを受信したので終了します。");
			}
			if ( strstr(buffer , wait.c_str() ) != NULL )
			{
				break;
			}
		}
	}

	//送信
	if (!send.empty() )
	{
		socket.Send(send);
	}

	//最後に会いての言い分を聞いてから通信を切ります。
	if (!recv.empty())
	{
		unsigned int total_recv_size = 0;
		while(1)
		{
			int size = socket.Recv(buffer+total_recv_size,buffersize-total_recv_size);
			if (size <= 0)
			{//エラー
				break;
			}
			buffer[size] = '\0';
			total_recv_size += size;
			if(total_recv_size >= buffersize)
			{
				throw XLException("waitまでにあまりに長いデータを受信したので終了します。");
			}
			if ( strstr(buffer , recv.c_str() ) != NULL )
			{
				break;
			}
		}
		return buffer;
	}
	return "";
}

string ActionImplement::HttpGet(const string& url,const map<string,string>& header,unsigned int timeoutS)
{
	return XLHttpSocket::Get(url,header,timeoutS);
}

string ActionImplement::HttpPost(const string& url,const string& postdata,const map<string,string>& header,unsigned int timeoutS)
{
	return XLHttpSocket::Post(url,header,timeoutS,postdata.c_str() , postdata.size() );
}


string ActionImplement::ExecuteLocal(const string& command)
{
	string _command = XLStringUtil::pathseparator(command);

	return SystemMisc::PipeExec(_command);
}
bool ActionImplement::ExecuteRemote(const string& host,const string& key,const string& command,const string& args)
{
	return FHCCommander::Execute(host , key , command,args);
}

bool ActionImplement::OpenWeb(const string& targetpc,const string& command)
{
	ASSERT ( XLStringUtil::strpos(command,"http://") == 0 || XLStringUtil::strpos(command,"https://") == 0 );

#if _MSC_VER
	::ShellExecute(NULL,"open",command.c_str(),NULL,NULL,SW_MAXIMIZE);
#endif
	return true;
}

bool ActionImplement::SendKeydown(const string& targetpc,const string& windowname,int key , int keyoption)
{
#if _MSC_VER
	HWND hwnd = ::GetTopWindow(::GetDesktopWindow());
	do
	{
		char title[ MAX_PATH ]= {0};
		::GetWindowText( hwnd, title, sizeof(title) );

		if ( strstr(title , windowname.c_str() ) == NULL )
		{
			continue;
		}
		
		//keybd_event
		if (keyoption & 1)  ::SendMessage(hwnd , WM_KEYDOWN ,VK_CONTROL ,   0 );
		if (keyoption & 2)  ::SendMessage(hwnd , WM_KEYDOWN ,VK_LMENU     ,   0 );
		if (keyoption & 4)  ::SendMessage(hwnd , WM_KEYDOWN ,VK_LSHIFT  ,   0 );
		::SendMessage(hwnd , WM_KEYDOWN ,key, 0 );
		::SendMessage(hwnd , WM_CHAR    ,key, 0 );
		::SendMessage(hwnd , WM_KEYUP   ,key, 0 );
		if (keyoption & 4)  ::SendMessage(hwnd , WM_KEYUP ,VK_LSHIFT  ,   0 );
		if (keyoption & 2)  ::SendMessage(hwnd , WM_KEYUP ,VK_LMENU     ,   0 );
		if (keyoption & 1)  ::SendMessage(hwnd , WM_KEYUP ,VK_CONTROL ,   0 );
	}
	while( (hwnd = ::GetNextWindow(hwnd,GW_HWNDNEXT)) !=NULL );
#else
#endif

	return true;
}

bool ActionImplement::SendMessage(const string& targetpc,const string& windowname,int message,int wparam,int  lparam)
{
#if _MSC_VER
	HWND hwnd = ::GetTopWindow(::GetDesktopWindow());
	do
	{
		char title[ MAX_PATH ]= {0};
		::GetWindowText( hwnd, title, sizeof(title) );

		if ( strstr(title , windowname.c_str() ) == NULL )
		{
			continue;
		}
		
		//keybd_event
		::SendMessage(hwnd , message , wparam,lparam );
	}
	while( (hwnd = ::GetNextWindow(hwnd,GW_HWNDNEXT)) !=NULL );

#else
#endif
	return true;
}

bool ActionImplement::MSleep(const string& targetpc,unsigned int mtime )
{
	const unsigned int sleep_poolingtime = 100;
	unsigned int loop = mtime / sleep_poolingtime;
	if (loop <= 0) loop ++;
	//この実装はおろかだけど、とりあえずこれで。
	//main thread に対する停止や linux対応、などを考えるとまずはこれで様子を見る.
	for(unsigned int t = 0 ; t < loop ; ++t)
	{
		MiriSleep(sleep_poolingtime);
	}
	return true;
}



string ActionImplement::SSTPSend11(const string& host,const string& message)
{
	
	XLUrlParser urlparser;
	urlparser.Parse("sstp://"+host);

	int port = urlparser.getPort();
	if (port == -1) port = 9801;


	XLSocket socket;
	socket.CreateTCP(60);
	socket.Connect( urlparser.getHost() , port );

	//送信ヘッダーの準備
	string sendstring =
					   string("SEND SSTP/1.1\r\n")
					+ "Sender: naichichi2\r\n"
					+ "Charset: UTF-8\r\n"
					+ "Script: " + _A2U(message) + "\r\n"
					+ "\r\n"
	;
	int size = socket.Send(sendstring.c_str() , sendstring.size() ) ;
	if (size <= 0)
	{
		int err = socket.getErrorCode();
		throw XLException("送信中にエラー " + num2str(err) );
	}

	vector<char> buffermalloc(65535);
	const unsigned int buffersize = buffermalloc.size() - 1;
	char * buffer = &buffermalloc[0];

	//まずヘッダーのパース
	size = socket.Recv(buffer,buffersize);
	if (size <= 0)
	{
		int err = socket.getErrorCode();
		throw XLException("受信中にエラー " + num2str(err) );
	}
	buffer[size] = '\0';

	return buffer;
}

bool ActionImplement::WolSend(const string& macaddress,unsigned int port,const string& boadcastip)
{
	//:または- を抜いて小文字化する
	string macplain = macaddress;
	macplain = XLStringUtil::replace(macplain,":","");
	macplain = XLStringUtil::replace(macplain,"-","");
	macplain = XLStringUtil::replace(macplain,".","");
	macplain = XLStringUtil::replace(macplain," ","");
	macplain = XLStringUtil::strtolower(macplain);
	if ( ! XLStringUtil::isnumlic16(macplain) )
	{//16進数ではない
		return false;
	}
	if ( macplain.size() != 12 )
	{//6バイトの16進数ではない
		return false;
	}
	unsigned char wolpacket[102] = {0};
	wolpacket[0] = 0xff;
	wolpacket[1] = 0xff;
	wolpacket[2] = 0xff;
	wolpacket[3] = 0xff;
	wolpacket[4] = 0xff;
	wolpacket[5] = 0xff;
	for(unsigned int i  = 0 ;i < 16 ; i ++)
	{//以下MACアドレスを16個繰り返す
		const char * macPtr = macplain.c_str(); //必ず12バイトある。上のifで保障している
		const unsigned int pos = 6 + (i * 6);
		for( unsigned int n = 0 ; n < 6 ; n ++ )
		{
			const unsigned macPtrN = n * 2;
			const unsigned char a1 = macPtr[macPtrN     ];
			const unsigned char a2 = macPtr[macPtrN + 1 ];
			wolpacket[pos + n] = ((a1 >= 'a' ? a1 - 'a'+10 : a1 - '0')<<4) 
								| (a2 >= 'a' ? a2 - 'a'+10 : a2 - '0');
		}
	}

	//利用するポート
	unsigned int useport = 2304;
	if (port > 0 && port < 65535)
	{
		useport = port;
	}
	//利用するブロードキャスト先
	string useboadcast = "255.255.255.255";
	if (!boadcastip.empty())
	{//ダイレクトブロードキャスト先
		useboadcast = boadcastip;
	}
	
	//ブロードキャストで送信します。
	struct sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(useport);
	addr.sin_addr.s_addr = inet_addr( useboadcast.c_str() );

	//wolパケットを飛ばします
	XLSocket socket;
	socket.CreateUDP(5);
	socket.SetSockopt(SOL_SOCKET,SO_BROADCAST, 1);
	socket.SendTo((char*)wolpacket , 102 , 0 , &addr , sizeof(addr) );

	return true;
}


bool ActionImplement::UpnpSearchAll(vector<ActionImplement::SSDPStruct>* outRet,const string& findstr,unsigned int timeoutS)
{
	string find = findstr;
	if (find == "ssdp:all")
	{
		find = "";
	}

	//検索リクエスト送信
	MainWindow::m()->UPNPServer.upnpMSearch(timeoutS);

	//発見した端末一覧の取得.
	list<UPNPMap> servers;
	MainWindow::m()->UPNPServer.GetAll(&servers);
	for(auto it = servers.begin() ; it != servers.end() ; it++ )
	{
		if ( UPNPServer::STSMatch(*it,find) )
		{
			ActionImplement::SSDPStruct ssdp;
			ssdp.location = it->location;
			ssdp.usn = it->usn;
			ssdp.uuid = it->uuid;
			ssdp.st = it->st;
			outRet->push_back(ssdp);
		}
	}
	return outRet->size() >= 1;
}

bool ActionImplement::UpnpSearchOne(ActionImplement::SSDPStruct* outRet,const string& findstr,unsigned int timeoutS)
{
	string find = findstr;
	if (find == "ssdp:all")
	{
		find = "";
	}

	UPNPMap m;
	bool r = MainWindow::m()->UPNPServer.GetST(find,&m);
	if (!r)
	{
		MainWindow::m()->UPNPServer.upnpMSearch(timeoutS);
		r = MainWindow::m()->UPNPServer.GetST(find,&m);
		if (!r)
		{
			return false;
		}
	}
	outRet->location = m.location;
	outRet->usn = m.usn;
	outRet->uuid = m.uuid;
	outRet->st = m.st;
	return true;
}

bool ActionImplement::UpnpSearchUUID(ActionImplement::SSDPStruct* outRet,const string& finduuid,unsigned int timeoutS)
{
	UPNPMap m;
	bool r = MainWindow::m()->UPNPServer.Get(finduuid,&m);
	if (!r)
	{
		MainWindow::m()->UPNPServer.upnpMSearch(timeoutS);
		r = MainWindow::m()->UPNPServer.Get(finduuid,&m);
		if (!r)
		{
			return false;
		}
	}
	outRet->location = m.location;
	outRet->usn = m.usn;
	outRet->uuid = m.uuid;
	outRet->st = m.st;
	return true;
}

bool ActionImplement::UpnpSearchXML(ActionImplement::SSDPStruct* outSSDP,string* outXML,const string& headerstr,const string& xmlstr,unsigned int timeoutS)
{
	string find = headerstr;
	if (find == "ssdp:all")
	{
		find = "";
	}

	UPNPMap m;
	bool r = MainWindow::m()->UPNPServer.GetSTAndXML(find,xmlstr,&m,outXML);
	if (!r)
	{
		MainWindow::m()->UPNPServer.upnpMSearch(timeoutS);
		r = MainWindow::m()->UPNPServer.GetSTAndXML(find,xmlstr,&m,outXML);
		if (!r)
		{
			return false;
		}
	}

	outSSDP->location = m.location;
	outSSDP->usn = m.usn;
	outSSDP->uuid = m.uuid;
	outSSDP->st = m.st;
	return true;
}

bool ActionImplement::TwitterTweet(const string& messages)
{
	const string oauth_access_token = MainWindow::m()->Config.Get( "twitter_oauth_token");
	const string oauth_access_token_secret = MainWindow::m()->Config.Get( "twitter_oauth_token_secret" );

	TwitterOAuth tw;
	bool r = tw.Tweet(messages
		,oauth_access_token
		,oauth_access_token_secret
		);
	return r;
}

void ActionImplement::TwitterHomeTimeLine(const string& screen_name,vector< map<string,string>* >* outTimelineMapVec)
{
	const string oauth_access_token = MainWindow::m()->Config.Get( "twitter_oauth_token");
	const string oauth_access_token_secret = MainWindow::m()->Config.Get( "twitter_oauth_token_secret" );

	TwitterOAuth tw;
	string r = tw.HomeTimeLine(
		screen_name
		,oauth_access_token
		,oauth_access_token_secret
		);
	tw.ParseTwitterTimelineResultToStringMapVec(r,outTimelineMapVec);
	return ;
}

void ActionImplement::TwitterUserTimeLine(const string& screen_name,vector< map<string,string>* >* outTimelineMapVec)
{
	const string oauth_access_token = MainWindow::m()->Config.Get( "twitter_oauth_token");
	const string oauth_access_token_secret = MainWindow::m()->Config.Get( "twitter_oauth_token_secret" );

	TwitterOAuth tw;
	string r = tw.UserTimeLine(
		screen_name
		,oauth_access_token
		,oauth_access_token_secret
		);
	tw.ParseTwitterTimelineResultToStringMapVec(r,outTimelineMapVec);
	return ;
}

//読み上げ処理
void ActionImplement::ToSpeak(const string& tospeakPrefix )
{
	const string tospeak_select = MainWindow::m()->Config.Get(tospeakPrefix + "_tospeak_select","");
	const string tospeak_string = MainWindow::m()->Config.Get(tospeakPrefix + "_tospeak_string","");
	const string tospeak_mp3 = MainWindow::m()->Config.Get(tospeakPrefix + "_tospeak_mp3","");
	ToSpeak(tospeak_select,tospeak_string,tospeak_mp3);
}

//読み上げ処理
void ActionImplement::ToSpeak(const string& tospeak_select
	,const string& tospeak_string
	,const string& tospeak_mp3 )
{
	if (tospeak_select == "string")
	{
		//文字列読み上げ
		MainWindow::m()->SyncInvoke( [&](){
			MainWindow::m()->Speak.SpeakAsync(CallbackPP::NoCallback() ,tospeak_string);
		});
	}
	else if (tospeak_select == "musicfile")
	{
		//mp3読み上げ
		MainWindow::m()->MusicPlayAsync.Play(CallbackPP::NoCallback() ,tospeak_mp3 ,1);
	}
}

#endif //WITH_CLIENT_ONLY_CODE==1