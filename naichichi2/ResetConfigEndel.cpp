//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#include "common.h"
#include "MainWindow.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"
#include "ResetConfig.h"
#include "ResetConfigEndel.h"
#include "SystemMisc.h"
#include "ScriptRemoconWeb.h"

ResetConfigEndel::ResetConfigEndel()
{
	this->StopFlag = false;
	this->Thread = NULL;
}

ResetConfigEndel::~ResetConfigEndel()
{
	Stop();
}

void ResetConfigEndel::Create()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	DEBUGLOG("ResetConfigEndelを構築します.");

	this->Thread = new thread([=](){
		XLDebugUtil::SetThreadName("ResetConfigEndel");
		this->ThreadMain(); 
	});
}

void ResetConfigEndel::Stop()
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

void ResetConfigEndel::ThreadMain()
{
#if WITH_CLIENT_ONLY_CODE==1
#ifdef _MSC_VER
#else
	while(! this->StopFlag)
	{
		bool found = false;

		//stat では見えないバグを引く時があるので、findfirstで調べる
		XLFileUtil::findfile("/mnt/sd/" , [&](const string& filename,const string& fullfilename) -> bool {
			if (filename == "networksetting.reset")
			{
				found = true;
				return false;
			}
			return true;
		});
		if (found)
		{//あるらしい
			NOTIFYLOG(string() + "ResetConfigEndel: findfirstの結果それらしいものがありました。remountして確認します");

			//mount しなおす。これで statでも確認できるようになる。わけわかめ
			SystemMisc::PipeExec("umount /mnt/sd");
			SystemMisc::PipeExec("mount /mnt/sd");
			if ( XLFileUtil::Exist("/mnt/sd/networksetting.reset") )
			{//あるらしい
				NOTIFYLOG(string() + "ResetConfigEndel: remountしても大丈夫でした。ファイルを読みます" );

				//networksetting.reset がある時は、読み込む
				try
				{
					if ( loadConfig("/mnt/sd/networksetting.reset") <= 0)
					{
						ERRORLOG("ResetConfigEndel: 設定が正しくありません" );
					}
					else
					{
						NOTIFYLOG("ResetConfigEndel: 設定をリセットしました");
					}
				}
				catch(XLException &e )
				{
					ERRORLOG(e.what());
				}
				XLFileUtil::del("/mnt/sd/networksetting.reset");
				ERRORLOG(string() + "ResetConfigEndel: networksetting.resetを削除しました。");
				MainWindow::m()->Shutdown(EXITCODE_LEVEL_REBOOT,true); //危険なので再起動します.
				return ;
			}
		}
		SecSleepEx(this->lock,this->EventObject,5);
	}
#endif //_MSC_VER
#endif //WITH_CLIENT_ONLY_CODE==1
}


#if WITH_CLIENT_ONLY_CODE==1
int ResetConfigEndel::loadConfig(const string& filename)
{
	const string base64config = XLFileUtil::cat(filename);
	const string str = XLStringUtil::base64decode(base64config);
	const auto vec = XLStringUtil::split_vector("\t",str);
	if ( vec.size () < 16 )
	{
		ERRORLOG("ResetConfigEndel: リセットファイルの要素数がありません" << vec[0]);
		return -1;
	}
	if ("NAICHICHI2_RESET" != vec[0])
	{
		ERRORLOG("ResetConfigEndel: 正しいprefixがありません。" << vec[0] );
		return -2;
	}
	for(unsigned int i = 0 ; i < vec.size() ; ++i )
	{
		DEBUGLOG("ResetConfigEndel: vec[" << i << "]:" << vec[i] );
	}

	string account_usermail = vec[1];
	string account_password_sha1 = vec[2];
	
	string network_ipaddress_type = vec[3];
	string network_ipaddress_ip = vec[4];
	string network_ipaddress_mask = vec[5];
	string network_ipaddress_gateway = vec[6];
	string network_ipaddress_dns = vec[7];

	string network_w_ipaddress_type = vec[8];
	string network_w_ipaddress_ip = vec[9];
	string network_w_ipaddress_mask = vec[10];
	string network_w_ipaddress_gateway = vec[11];
	string network_w_ipaddress_dns = vec[12];
	string network_w_ipaddress_ssid = vec[13];
	string network_w_ipaddress_password = vec[14];
	string network_w_ipaddress_wkeymgmt = vec[15];

	//cabel
	if (network_ipaddress_type == "DHCP")
	{
		network_ipaddress_ip = "DHCP";
		network_ipaddress_mask = "";
		network_ipaddress_gateway = "";
		network_ipaddress_dns = "";
	}

	//wifi
	network_w_ipaddress_type = "NONE";
	if (network_w_ipaddress_type == "DHCP")
	{
		network_w_ipaddress_ip = "DHCP";
		network_w_ipaddress_mask = "";
		network_w_ipaddress_gateway = "";
		network_w_ipaddress_dns = "";
	}
	else if (network_w_ipaddress_type == "NONE")
	{
		network_w_ipaddress_ip = "NONE";
		network_w_ipaddress_mask = "";
		network_w_ipaddress_gateway = "";
		network_w_ipaddress_dns = "";
		network_w_ipaddress_ssid = "";
		network_w_ipaddress_password = "";
		network_w_ipaddress_wkeymgmt = "";
	}

	//ネットワークの設定更新
	SystemMisc::SetIPAddress(network_ipaddress_ip,network_ipaddress_mask,network_ipaddress_gateway,network_ipaddress_dns,network_w_ipaddress_ip,network_w_ipaddress_mask,network_w_ipaddress_gateway,network_w_ipaddress_dns,network_w_ipaddress_ssid,network_w_ipaddress_password,network_w_ipaddress_wkeymgmt);
	SystemMisc::NetworkRestart(network_ipaddress_ip,network_w_ipaddress_ip);

	//アカウントの同期
	const string uuid = MainWindow::m()->Httpd.GetUUID();
	unsigned int retry = 3;
	for(; retry > 0 ; retry --)
	{
		const string serverresult = MainWindow::m()->ColudSyncPooling.OpeartionCheckRegist(account_usermail,account_password_sha1,uuid,"1");
		if (serverresult == "OK")
		{
			break;
		}
		DEBUGLOG("ResetConfigEndel: アカウントをリセットしようとしたら、サーバから拒絶されました。retry:" << retry << " name:" << account_usermail  );
		SecSleep(1);
	}
	//サーバと同期がとれなかった?
	if (retry <= 0)
	{
		ERRORLOG("ResetConfigEndel: アカウントをリセットしようとしたら、サーバから拒絶されました。 name:" << account_usermail  );
//		return -3;
	}

	//configの上書き
	MainWindow::m()->Config.Set("network_ipaddress_type",network_ipaddress_type);
	MainWindow::m()->Config.Set("network_ipaddress_ip",network_ipaddress_ip);
	MainWindow::m()->Config.Set("network_ipaddress_mask",network_ipaddress_mask);
	MainWindow::m()->Config.Set("network_ipaddress_gateway",network_ipaddress_gateway);
	MainWindow::m()->Config.Set("network_ipaddress_dns",network_ipaddress_dns);

	MainWindow::m()->Config.Set("network_w_ipaddress_type",network_w_ipaddress_type);
	MainWindow::m()->Config.Set("network_w_ipaddress_ip",network_w_ipaddress_ip);
	MainWindow::m()->Config.Set("network_w_ipaddress_mask",network_w_ipaddress_mask);
	MainWindow::m()->Config.Set("network_w_ipaddress_gateway",network_w_ipaddress_gateway);
	MainWindow::m()->Config.Set("network_w_ipaddress_dns",network_w_ipaddress_dns);
	MainWindow::m()->Config.Set("network_w_ipaddress_ssid",network_w_ipaddress_ssid);
	MainWindow::m()->Config.Set("network_w_ipaddress_password",network_w_ipaddress_password);
	MainWindow::m()->Config.Set("network_w_ipaddress_wkeymgmt",network_w_ipaddress_wkeymgmt);


	MainWindow::m()->Config.Set("account_usermail",account_usermail);
	MainWindow::m()->Config.Set("account_password",account_password_sha1);
	MainWindow::m()->Config.Set("account_sync_server" , "1"); //同期させる

	//configの保存
	MainWindow::m()->SyncInvoke([=](){
		ScriptRemoconWeb::SettingPageIPAddressOverriade();
		MainWindow::m()->Config.overrideSave();
	});


	//configの同期を行う
	MainWindow::m()->ColudSyncPooling.WakeupByNewSyncID(account_usermail,account_password_sha1,uuid,true  );
	MainWindow::m()->ColudSyncPooling.WakeupSyncConfigByconfig();
	
	DEBUGLOG("config等のsyncを待ちます...");
	//syncが完了するまで待つ
	MainWindow::m()->ColudSyncPooling.SyncJoin(15);

	return 1;
}
#endif //WITH_CLIENT_ONLY_CODE==1

string ResetConfigEndel::make(
	 const string& user_mail
	,const string& user_password_sha1
	,const string& network_ipaddress_type
	,const string& network_ipaddress_ip
	,const string& network_ipaddress_mask
	,const string& network_ipaddress_gateway
	,const string& network_ipaddress_dns
	,const string& network_w_ipaddress_type
	,const string& network_w_ipaddress_ip
	,const string& network_w_ipaddress_mask
	,const string& network_w_ipaddress_gateway
	,const string& network_w_ipaddress_dns
	,const string& network_w_ipaddress_ssid
	,const string& network_w_ipaddress_password
	,const string& network_w_ipaddress_wkeymgmt)
{
	const string str = string() + "NAICHICHI2_RESET"
	+ "\t" + user_mail
	+ "\t" + user_password_sha1
	+ "\t" + network_ipaddress_type
	+ "\t" + network_ipaddress_ip
	+ "\t" + network_ipaddress_mask
	+ "\t" + network_ipaddress_gateway
	+ "\t" + network_ipaddress_dns
	+ "\t" + network_w_ipaddress_type
	+ "\t" + network_w_ipaddress_ip
	+ "\t" + network_w_ipaddress_mask
	+ "\t" + network_w_ipaddress_gateway
	+ "\t" + network_w_ipaddress_dns
	+ "\t" + network_w_ipaddress_ssid
	+ "\t" + network_w_ipaddress_password
	+ "\t" + network_w_ipaddress_wkeymgmt
	;
	
	return XLStringUtil::base64encode(str);
}
