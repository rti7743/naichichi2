//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include "MultiRoomUtil.h"
#include "MainWindow.h"
#include "XLHttpSocket.h"
#include "MultiRoomUtil.h"

MultiRoom::MultiRoom()
{
	this->Thread = NULL;
	this->StopFlag = false;
}

MultiRoom::~MultiRoom()
{
	DEBUGLOG("stop...");

	Stop();

	DEBUGLOG("done");
}

void MultiRoom::Create()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	this->StopFlag = false;

	this->Thread = new thread([=](){
		XLDebugUtil::SetThreadName("MultiRoom");
		this->ThreadMain(); 
	});
	
}
void MultiRoom::Stop()
{
	if (this->Thread)
	{
		this->StopFlag = true;
		this->EventObject.notify_one();
		this->Thread->join();
		delete this->Thread;
		this->Thread = NULL;
	}
}

void MultiRoom::startSync()
{
//	lock_guard<mutex> al(this->lock);	
	this->EventObject.notify_one();
}

void MultiRoom::ThreadMain()
{
	if( Touch() )
	{
		Sync();
	}

	while(! this->StopFlag)
	{
		//寝る
		{
			unique_lock<mutex> al(this->lock);
			this->EventObject.wait(al);
		}
		if ( this->StopFlag )
		{
			break;
		}

		try
		{
			Touch();
			Sync();
		}
		catch(XLException& e)
		{
			ERRORLOG("マルチルームの同期中にエラー:" << e.what() );
		}
	}
}

//同期処理
void MultiRoom::Sync()
{
	
	const string myself_uuid = MainWindow::m()->Httpd.GetUUID();
	if (! SystemMisc::checkUUID(myself_uuid))
	{
		DEBUGLOG("UUIDがありません!");
		return ;
	}

	const string myself_url = MainWindow::m()->Httpd.getServerTop();

	const auto configmap = MainWindow::m()->Config.FindGetsToMap("multiroom_",false);

	string post_multiroom;
	for(auto it = configmap.begin(); it != configmap.end(); it++)
	{
		post_multiroom += "&"+it->first + "=" + XLStringUtil::urlencode(_A2U(it->second));
	}

	for(auto it = configmap.begin(); it != configmap.end(); it++)
	{
		if(it->first.find("multiroom_") != 0)
		{
			continue;
		}
		if( it->first.find("_uuid") == string::npos)
		{
			continue;
		}
		int key=0;
		sscanf(it->first.c_str(),"multiroom_%d_uuid",&key);
		if (key <= 0)
		{
			continue;
		}

		const string prefix = "multiroom_" + num2str(key) ;
		const string your_uuid = mapfind(configmap,prefix + "_uuid");
		if(myself_uuid == your_uuid)
		{//自分自身なのでsyncしてはいけない
			continue;
		}
		
		const string your_url = mapfind(configmap,prefix + "_url");
		if (your_url == myself_url)
		{//UUIDが違うのに、自分自身をさしている
			ERRORLOG("UUID(" << myself_uuid << "!=" << your_uuid << ")が違うのに、自己URL(" << your_url << ")さしているMULTIROOM設定があります");
			continue;
		}
		const string your_authkey = mapfind(configmap,prefix + "_authkey");

		const string post = 
			"your_authkey=" + XLStringUtil::urlencode(your_authkey)
			+ "&our_uuid=" + XLStringUtil::urlencode(myself_uuid)
			+ post_multiroom;

		const string url = XLStringUtil::urlcombine(your_url , "/multiroom/sync");
		string your_result;
		map<string,string> header;
		XLHttpHeader retheader;
		try
		{
			your_result = XLHttpSocket::Post(url, header ,3, &retheader,post);
			your_result = _U2A(your_result);
		}
		catch(XLException &e)
		{
			ERRORLOG("can not connect " +  url + "message:" << e.what() );
			continue;
		}

		//結果をパース
		map<string,string> json = XLStringUtil::parsejson(your_result);
		if( mapfind(json,"result") != "ok")
		{//相手から拒絶された.
			ERRORLOG("can not sync " +  url + "message:" + your_result );
			continue;
		}

	}
}

//自分の部屋がなければ作るし、
//あって情報が更新されていた場合、
//syncしなおす
bool MultiRoom::Touch()
{
	const auto configmap = MainWindow::m()->Config.FindGetsToMap("multiroom_",false);
	const string myself_uuid = MainWindow::m()->Httpd.GetUUID();
	if (! SystemMisc::checkUUID(myself_uuid))
	{
		DEBUGLOG("UUIDがありません!");
		return false;
	}

	const string our_url = MainWindow::m()->Httpd.getServerTop();
	const string our_name = MainWindow::m()->Config.Get("system_room_string","リモコン");
	const string our_account = MainWindow::m()->Config.Get("account_usermail","");
	string our_authkey = MainWindow::m()->Config.Get("webapi_apikey","");
	if(our_authkey.empty())
	{
		our_authkey = SystemMisc::MakeWebAPIKey();
		MainWindow::m()->Config.Set("webapi_apikey",our_authkey);
	}

	for(auto it = configmap.begin(); it != configmap.end(); it++)
	{
		if(it->first.find("multiroom_") != 0)
		{
			continue;
		}
		if( it->first.find("_uuid") == string::npos)
		{
			continue;
		}
		int key=0;
		sscanf(it->first.c_str(),"multiroom_%d_uuid",&key);
		if (key <= 0)
		{
			continue;
		}
		const string my_prefix = "multiroom_" + num2str(key) ;
		const string my_uuid = mapfind(configmap,my_prefix + "_uuid");
		if(my_uuid != myself_uuid)
		{
			continue;
		}

		bool isUpdate = false;
		const string my_url = mapfind(configmap,my_prefix + "_url");
		if(my_url != our_url)
		{
			MainWindow::m()->Config.Set(my_prefix + "_url",our_url);
			DEBUGLOG("update " << my_prefix + "_url=" << our_url);
			isUpdate = true;
		}

		string my_authkey = mapfind(configmap,my_prefix + "_authkey");
		if(my_authkey != our_authkey)
		{
			MainWindow::m()->Config.Set(my_prefix + "_authkey",our_authkey);
			DEBUGLOG("update " << my_prefix + "_authkey=" << our_authkey);
			isUpdate = true;
		}

		string my_name = mapfind(configmap,my_prefix + "_name");
		if(my_name != our_name)
		{
			MainWindow::m()->Config.Set(my_prefix + "_name",our_name);
			DEBUGLOG("update " << my_prefix + "_name=" << our_name);
			isUpdate = true;
		}

		string my_account = mapfind(configmap,my_prefix + "_account");
		if(my_account != our_account)
		{
			MainWindow::m()->Config.Set(my_prefix + "_account",our_account);
			DEBUGLOG("update " << my_prefix + "_account=" << our_account);
			isUpdate = true;
		}

		if(isUpdate)
		{
			MainWindow::m()->ColudSyncPooling.ConfigUpdated();
		}
		return isUpdate;
	}

	//どうやら自分がないらしいので新規追加.
	int our_key = MultiRoomUtil::NewMultiRoomID(configmap);
	int our_order = MultiRoomUtil::NewMultiRoomOrder(configmap);

	string my_prefix = "multiroom_" + num2str(our_key) ;
	MainWindow::m()->Config.Set(my_prefix+"_uuid",   myself_uuid);
	MainWindow::m()->Config.Set(my_prefix+"_url" ,   our_url);
	MainWindow::m()->Config.Set(my_prefix+"_authkey",our_authkey);
	MainWindow::m()->Config.Set(my_prefix+"_name",   our_name);
	MainWindow::m()->Config.Set(my_prefix+"_account",our_account);
	MainWindow::m()->Config.Set(my_prefix+"_order",  num2str(our_order));

	DEBUGLOG("update_new: " << my_prefix + "_uuid=" << myself_uuid);
	DEBUGLOG("update_new: " << my_prefix + "_url=" << our_url);
	DEBUGLOG("update_new: " << my_prefix + "_authkey=" << our_authkey);
	DEBUGLOG("update_new: " << my_prefix + "_name=" << our_name);
	DEBUGLOG("update_new: " << my_prefix + "_account=" << our_account);
	
	MainWindow::m()->ColudSyncPooling.ConfigUpdated();
	return false;
}


#endif //WITH_CLIENT_ONLY_CODE