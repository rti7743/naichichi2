//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include "ColudSyncPooling.h"
#include "MainWindow.h"
#include "SystemMisc.h"
#include "XLFileUtil.h"
#include "XLHttpSocket.h"
#include "XLGZip.h"
#include "ScriptRemoconWeb.h"

ColudSyncPooling::ColudSyncPooling()
{
	this->StopFlag = false;
	this->Thread = NULL;
	this->isSync = false;
	this->isSyncServerOK = false;
	this->isWakeupFlag = false;
}

ColudSyncPooling::~ColudSyncPooling()
{
	DEBUGLOG("stop...");

	Stop();

	DEBUGLOG("done");
}

void ColudSyncPooling::Create(const string& serverurl,const string& id,const string& pass,const string& uuid,bool isSyncServerOK,const string& linkid)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	this->id = id;
	this->pass = pass;
	this->uuid = uuid;
	this->linkid = linkid;
	this->isSync = false;
	this->StopFlag = false;
	this->isSyncServerOK = isSyncServerOK;
	this->serverurl = serverurl;
	this->isWakeupFlag = false;
	
	this->Thread = new thread([=](){
		XLDebugUtil::SetThreadName("ColudSyncPooling");
		this->ThreadMain(); 
	});

	this->SyncConfig.Create(serverurl);
}

void ColudSyncPooling::Stop()
{
	this->SyncConfig.Stop();

	if (this->Thread)
	{
		this->StopFlag = true;
		this->EventObject.notify_one();
		this->Thread->join();
		delete this->Thread;
		this->Thread = NULL;
	}
}

//Configを変更した場合
void ColudSyncPooling::ConfigUpdated()
{
	//configの保存
	MainWindow::m()->SyncInvoke([=](){
		MainWindow::m()->Config.overrideSave();
	});

	WakeupSyncConfigByconfigOnly();
}

//Configを変更した場合 画像も同期する場合
void ColudSyncPooling::ConfigUpdatedFull()
{
	//configの保存
	MainWindow::m()->SyncInvoke([=](){
		MainWindow::m()->Config.overrideSave();
	});
	WakeupSyncConfigByconfig();
}

void ColudSyncPooling::WakeupByNewSyncID(const string& id,const string& pass,const string& uuid,bool isSyncServerOK)
{
	lock_guard<mutex> al(this->lock);

	this->id = id;
	this->pass = pass;
	this->uuid = uuid;
	this->linkid = "";
	this->isSync = false;
	this->isSyncServerOK = isSyncServerOK;
	this->isWakeupFlag = true;
}

void ColudSyncPooling::WakeupSyncConfigByconfig()
{
	if (!this->isSyncServerOK)
	{//sync禁止!
		DEBUGLOG("サーバーとのsyncはユーザによって禁止されています");
		return ;
	}
	if (this->uuid.empty())
	{
		DEBUGLOG("UUIDがありません");
		return ;
	}


	this->SyncConfig.WakeupByconfig();
}

void ColudSyncPooling::WakeupSyncConfigByconfigOnly()
{
	if (!this->isSyncServerOK)
	{//sync禁止!
		DEBUGLOG("サーバーとのsyncはユーザによって禁止されています");
		return ;
	}
	if (this->uuid.empty())
	{
		DEBUGLOG("UUIDがありません");
		return ;
	}
	this->SyncConfig.WakeupByconfigOnly();
}

string ColudSyncPooling::getLinkID() const
{
	lock_guard<mutex> al(this->lock);

	if (!this->isSync) return "";
	return this->linkid;
}

//syncが完了するまで待ちます.
bool ColudSyncPooling::SyncJoin(unsigned int waitSec) const
{
	if (!this->isSyncServerOK)
	{//sync禁止!
		DEBUGLOG("サーバーとのsyncはユーザによって禁止されています");
		return true;
	}
	//まずlinkを構築できているかどうか？
	unsigned int i ;
	for(i = 0 ; i < waitSec ; ++i )
	{
		if ( this->isSync )
		{
			DEBUGLOG("link完了しています");
			break;
		}
		SecSleepEx(this->lock,this->EventObject,1);
	}
	if (i >= waitSec)
	{//timeout
		DEBUGLOG("syncjoinがタイムアウトしました。linkは完了していません.");
		return false;
	}

	//次にconfigとかのsyncができているかどうか？
	for( ; i < waitSec ; ++i )
	{
		if ( this->SyncConfig.IsSync() )
		{
			DEBUGLOG("configの同期も完了しています");
			break;
		}
		SecSleepEx(this->lock,this->EventObject,1);
	}
	if (i >= waitSec)
	{//timeout
		DEBUGLOG("syncjoinがタイムアウトしました。config同期は完了していません.");
		return false;
	}
	
	//OK!
	DEBUGLOG("SyncJoin: true");
	return true;
}


void ColudSyncPooling::ThreadMain()
{
	bool error;

	{
		//まず自分はここにいることを発信.
		const string uuid = MainWindow::m()->Httpd.GetUUID();
		const string url = MainWindow::m()->Httpd.getServerTop();
		OpeartionWeAreHere(uuid,url);
	}

	while(! this->StopFlag)
	{
		if (! this->isSyncServerOK)
		{//サーバーとsyncしてはいけない設定
			Wait(120);
		}
		else
		{//サーバーとsyncしてもいい設定
			//ランデブー
			while(! this->StopFlag)
			{
				{
					lock_guard<mutex> al(this->lock);
					this->isSync = false;
				}
				error  = true;
				try
				{
					const string newid = OpeartionGetLink(this->id,this->pass,this->uuid,linkid);
					if (newid.empty())
					{
						error  = true;
					}
					else
					{
						lock_guard<mutex> al(this->lock);
						this->linkid = newid;
						this->isSync = true;
						error = false;
					}
				}
				catch(XLException& e)
				{
					DEBUGLOG("ColudSyncPooling::ThreadMainのOpeartionGetLinkでエラー:" << e.what() );
				}

				if (!error)
				{
					NOTIFYLOG("getlinkできました。linkid:"+this->linkid );
					break;
				}
				DEBUGLOG("getlinkできませんでした。30秒後にリトライします" );
			
				//エラーが発生している可能性もあるので、30秒程度待つ
				Wait(30);
			}

			//IDがとれたので、他のスレッドに教えてあげる
			this->SyncConfig.WakeupByNewSyncID(linkid);

			//命令ループ
			while(! this->StopFlag)
			{
				error  = true;
				try
				{
					const string opline = OpeartionPooling(linkid);
//					DEBUGLOG("poolingしました opline:"+opline);

					if (! RunOpline(opline) )
					{//命令実行失敗
						ERRORLOG("poolingした命令に失敗しました opline:" << opline);
						break;
					}
					error = false;
				}
				catch(XLException& e)
				{
					ERRORLOG("ColudSyncPooling::ThreadMainのOpeartionPoolingでエラー:" << e.what());
					break;
				}

				//エラーが起きていたら命令ループを抜けて、再度ランデブーからやり直す
				if (error)
				{
					break;
				}
				if (! this->isSync) break;
			}
		}
	}
}

bool ColudSyncPooling::RunOpline(const string& opline)
{
	const auto opcode = XLStringUtil::split_vector("\t" , _U2A(opline) );
	if (opcode[0] == "wait")
	{
		MiriSleepEx(this->lock,this->EventObject,500);
		return true;
	}
	else if (opcode[0] == "action")
	{
		if (opcode.size() <= 2)
		{
			return false;
		}

		//家電操作実行
		MainWindow::m()->ScriptManager.FireElec(CallbackPP::NoCallback(),opcode[1],opcode[2]);
		return true;
	}
	else if (opcode[0] == "actionstr")
	{
		if (opcode.size() <= 1)
		{
			return false;
		}

		//家電操作実行
		MainWindow::m()->ScriptManager.FireElecString(CallbackPP::NoCallback(),opcode[1]);
		return true;
	}
	else if (opcode[0] == "reconfig")
	{
		//温度と明るさのセンサーの値を更新する
		MainWindow::m()->Sensor.getSensorNow();
		ScriptRemoconWeb::SettingPageIPAddressOverriade(true);

		//configの保存
		MainWindow::m()->SyncInvoke([=](){
			MainWindow::m()->Config.overrideSave();
		});

		MainWindow::m()->ColudSyncPooling.WakeupSyncConfigByconfig();
		return true;
	}
/*
	else if (opcode[0] == "sip_call")
	{
		if (opcode.size() <= 1)
		{
			return false;
		}

		//電話をかける
		MainWindow::m()->SipServer.Call(opcode[1]);
		return true;
	}
	else if (opcode[0] == "sip_answer")
	{
		//かかってきた電話に出る
		MainWindow::m()->SipServer.Answer();
		MainWindow::m()->SipServer.WaitForRequest(5);
		return true;
	}
	else if (opcode[0] == "sip_hangup")
	{
		//電話を切る
		MainWindow::m()->SipServer.Hangup();
		MainWindow::m()->SipServer.WaitForRequest(5);
		return true;
	}
	else if (opcode[0] == "sip_numpad_close")
	{//numpadを閉じるイベント
		MainWindow::m()->SipServer.HideNumpad();
		return true;
	}
*/
	else if (opcode[0] == "BAD")
	{
		//エラーが発生
		ERRORLOG("サーバーとのpoolingでエラーが返されました。" << _U2A(opline) );
		return false;
	}
	else
	{
		Wait(30);
	}
	return true;
}

void ColudSyncPooling::Wait(unsigned int second) 
{
	if (second == 0)
	{
		MiriSleepEx(this->lock,this->EventObject,300);
		return ;
	}

	for(unsigned int t = 0 ; t < second ; ++t)
	{
		if (this->StopFlag || this->isWakeupFlag) break;

		SecSleepEx(this->lock,this->EventObject,1);
	}
	//起きたのでフラグを下ろす
	this->isWakeupFlag = false;
}


string ColudSyncPooling::OpeartionGetLink(const string& id,const string& pass,const string& uuid,const string& linkid) const
{
	if (uuid.empty())
	{
		ERRORLOG("UUIDがありません");
		return "";
	}

	map<string,string> header;
	if (!linkid.empty())	header["Cookie"] = linkid;
	header["Cache-Control"] = "no-cache";

	XLHttpHeader retheader;
	const string getparam = 
		"?name=" + XLStringUtil::urlencode(id) 
		;
	const string postparam = 
		"name=" + XLStringUtil::urlencode(id) 
		+ "&password=" + XLStringUtil::urlencode(pass) 
		+ "&uuid=" + XLStringUtil::urlencode(uuid) 
		;

	const string r = XLHttpSocket::Post(this->serverurl + "auto/getlink" + getparam , header ,60, &retheader,postparam);
	if (r != "OK")
	{
		DEBUGLOG("getlinkに失敗しました:" << r);
		return "";
	}
	
	const string setCookieHeader = retheader.getAt("set-cookie");
	const char * startP = strstr(setCookieHeader.c_str() , "naichichi2=" );
	if (!startP)
	{
		DEBUGLOG("getlinkに失敗しました クッキーがありません:" << setCookieHeader);
		return "";
	}

	const char * endP = strstr(startP + sizeof("naichichi2=") - 1, ";" );
	if (!endP)
	{
		DEBUGLOG("getlinkに失敗しました クッキーの終端がありません:" << setCookieHeader);
		return "";
	}
	endP ++; // 最後の ; を含める
	return string( startP , 0 , (int)(endP - startP) ) ;
}

string ColudSyncPooling::OpeartionPooling(const string& linkid) const
{
	//クッキーが必要
	map<string,string> header;
	header["Cookie"] = linkid;
	header["Cache-Control"] = "no-cache";

	return XLHttpSocket::Get(this->serverurl + "auto/pooling" ,header,15);
}

string ColudSyncPooling::OpeartionChangeRegist(const string& old_id,const string& old_password,const string& new_id,const string& new_password,const string& uuid,const string& is_uuid_overriade) const
{
	if (uuid.empty())
	{
		ERRORLOG("UUIDがありません");
		return "BAD\tEXCEPTION";
	}

	//ユーザー系なのでクッキーなくてもOK
	const string postparam = 
		"old_name=" + XLStringUtil::urlencode(old_id) 
		+ "&old_password=" + XLStringUtil::urlencode(old_password) 
		+ "&new_name=" + XLStringUtil::urlencode(new_id) 
		+ "&new_password=" + XLStringUtil::urlencode(new_password) 
		+ "&uuid=" + XLStringUtil::urlencode(uuid) 
		+ "&is_uuid_overriade=" + XLStringUtil::urlencode(is_uuid_overriade) 
		;

	DEBUGLOG("POST " << this->serverurl + "auto/changeregist" << "@@" << postparam );

	try
	{
		const string r =  XLHttpSocket::Post(this->serverurl + "auto/changeregist", postparam.c_str() , postparam.size() );
		DEBUGLOG("RESULT " << r );

		return r;
	}
	catch(XLException &e)
	{
		DEBUGLOG("EXCEPTION! " << e.what() );

		return "BAD\tEXCEPTION";

	}

}

string ColudSyncPooling::OpeartionCheckRegist(const string& id,const string& password,const string& uuid,const string& is_uuid_overriade) const
{
	if (uuid.empty())
	{
		ERRORLOG("UUIDがありません");
		return "BAD\tEXCEPTION";
	}

	//ユーザー系なのでクッキーなくてもOK
	const string postparam = 
		"name=" + XLStringUtil::urlencode(id) 
		+ "&password=" + XLStringUtil::urlencode(password) 
		+ "&uuid=" + XLStringUtil::urlencode(uuid) 
		+ "&is_uuid_overriade=" + XLStringUtil::urlencode(is_uuid_overriade) 
		;

	DEBUGLOG("POST " << this->serverurl + "auto/changeregist" << "@@" << postparam );

	try
	{
		const string r = XLHttpSocket::Post(this->serverurl + "auto/checkregist", postparam.c_str() , postparam.size() );
		DEBUGLOG("RESULT " << r );

		return r;
	}
	catch(XLException &e)
	{
		DEBUGLOG("EXCEPTION! " << e.what() );

		return "BAD\tEXCEPTION";
	}
}

string ColudSyncPooling::OpeartionWeAreHere(const string& uuid,const string& url) const
{
	if (uuid.empty())
	{
		ERRORLOG("UUIDがありません");
		return "BAD\tEXCEPTION";
	}


	//ユーザー系なのでクッキーなくてもOK
	const string postparam = 
		+ "uuid=" + XLStringUtil::urlencode(uuid) 
		+ "&url=" + XLStringUtil::urlencode(url) 
		;

	DEBUGLOG("POST " << this->serverurl + "auto/wearehere" << "@@" << postparam );

	try
	{
		const string r = XLHttpSocket::Post(this->serverurl + "auto/wearehere", postparam.c_str() , postparam.size() );
		DEBUGLOG("RESULT " << r );

		return r;
	}
	catch(XLException &e)
	{
		DEBUGLOG("EXCEPTION! " << e.what() );

		return "BAD\tEXCEPTION";
	}
}

string ColudSyncPooling::OpeartionGetURLByPassword(const string id,const string password) const
{
	const string postparam = 
		"yourname=" + XLStringUtil::urlencode(id) 
		+ "&yourpassword=" + XLStringUtil::urlencode(password) 
		;

	//クッキーが必要
	map<string,string> header;
	header["Cookie"] = linkid;
	header["Cache-Control"] = "no-cache";

	return XLHttpSocket::Post(this->serverurl + "auto/get_url_by_password" ,header,15, postparam.c_str() , postparam.size());
}

string ColudSyncPooling::OpeartionGetURLByUUID(const string id,const string uuid) const
{
	const string postparam = 
		"yourname=" + XLStringUtil::urlencode(id) 
		+ "&youruuid=" + XLStringUtil::urlencode(uuid) 
		;

	//クッキーが必要
	map<string,string> header;
	header["Cookie"] = linkid;
	header["Cache-Control"] = "no-cache";

	return XLHttpSocket::Post(this->serverurl + "auto/get_url_by_uuid" ,header,15, postparam.c_str() , postparam.size());
}

#endif //WITH_CLIENT_ONLY_CODE