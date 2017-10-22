//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#include "common.h"
#include "MainWindow.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"
#include "ResetConfig.h"
#include "ResetConfigIRR.h"
#include "SystemMisc.h"
#include "ScriptRemoconWeb.h"
#include "CapeBord.h"
#ifdef _MSC_VER
#else
#include <poll.h>
#include <sys/stat.h>
#endif

#define GPIONUMBER 45

ResetConfigIRR::ResetConfigIRR()
{
	this->StopFlag = false;
	this->Thread = NULL;
	this->GPIOFD = -1;
}

ResetConfigIRR::~ResetConfigIRR()
{
	Stop();
}

void ResetConfigIRR::Create()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	
	DEBUGLOG("ResetConfigIRRを構築します.");
	
#ifdef _MSC_VER
#else
	//GPIO export.
	XLFileUtil::write("/sys/class/gpio/export",num2str(GPIONUMBER));
	
	//少し待ちましょう.
	usleep(1000);
	
	//入力方向 in 
	const string gpioDirection = "/sys/class/gpio/gpio" + num2str(GPIONUMBER) + "/direction";
	XLFileUtil::write(gpioDirection,"in");

	const string gpioEdge = "/sys/class/gpio/gpio" + num2str(GPIONUMBER) + "/edge";
	XLFileUtil::write(gpioEdge,"rising");
	
	//GPIO valueのハンドルをつかむ
	const string gpioValue = "/sys/class/gpio/gpio" + num2str(GPIONUMBER) + "/value";

	this->GPIOFD = open(gpioValue.c_str() , O_RDONLY| O_NONBLOCK );
	if (this->GPIOFD < 0)
	{
		DEBUGLOG("ResetConfigIRRを構築に必要なGPIOファイル " << gpioValue << "を開けません。エラー(" << this->GPIOFD << ")");
		return ;
	}

	//一発目空打ち
	char buf[5];
	int ret = read(this->GPIOFD, buf, sizeof(buf));
	if (ret < 0) 
	{
		DEBUGLOG("ResetConfigIRRを構築に必要なGPIOファイル " << gpioValue << "を読み出せません。エラー(" << ret << ")");
		return ;
	}

	DEBUGLOG("polling thread boot...");
	
	
	this->Thread = new thread([=](){
		XLDebugUtil::SetThreadName("ResetConfigIRR");
		this->ThreadMain(); 
	});
#endif
}

void ResetConfigIRR::Stop()
{
	this->StopFlag = true;
#ifdef _MSC_VER
#else
	if (this->Thread)
	{
		if (this->GPIOFD >= 0)
		{
			close(this->GPIOFD);
		}
		this->GPIOFD = -1;

		this->Thread->join();
		delete this->Thread;
		this->Thread = NULL;
	}

	XLFileUtil::write("/sys/class/gpio/unexport",num2str(GPIONUMBER));
#endif
}

void ResetConfigIRR::ThreadMain()
{
	DEBUGLOG("ThreadMain");
#if WITH_CLIENT_ONLY_CODE==1
#ifdef _MSC_VER
#else
	while(! this->StopFlag)
	{
		DEBUGLOG("poll wait...");
		
		char buf[5];
		struct pollfd pfd;
		pfd.fd = this->GPIOFD;
		pfd.events = POLLPRI;

		lseek(this->GPIOFD, 0, SEEK_SET);
		int ret = poll(&pfd, 1, -1);
		DEBUGLOG("poll break...");
		if( this->StopFlag )
		{
			DEBUGLOG(" 停止フラグが入りました。");
			break;
		}
		
		if (ret < 0)
		{
			DEBUGLOG("poll error :" << ret);
			continue;
		}
		if (ret == 0)
		{
			DEBUGLOG("poll timeout");
			continue;
		}

		DEBUGLOG("read...");
		ret = read(this->GPIOFD, buf, sizeof(buf));
		if (ret < 0) 
		{
			DEBUGLOG("pollで反応があったのでreadしたらエラーになりました エラー(" << ret << ")");
			continue;
		}
		buf[ret] = '\0';
		
		if ( atoi(buf) )
		{
			DEBUGLOG("FResetスイッチが押されました。" << buf );
			AutoLEDOn ledon;	//LED光らせる.
			FireFactoryReset();
		}
		else
		{
			DEBUGLOG("FResetスイッチが離されました。 "  << buf );
		}
	}
#endif //_MSC_VER
#endif //WITH_CLIENT_ONLY_CODE==1
	DEBUGLOG("ThreadMain down");
}


#if WITH_CLIENT_ONLY_CODE==1
int ResetConfigIRR::FireFactoryReset()
{
	//configの上書き
	MainWindow::m()->Config.Set("network_ipaddress_type","DHCP");
	MainWindow::m()->Config.Set("network_ipaddress_ip","");
	MainWindow::m()->Config.Set("network_ipaddress_mask","");
	MainWindow::m()->Config.Set("network_ipaddress_gateway","");
	MainWindow::m()->Config.Set("network_ipaddress_dns","");

	MainWindow::m()->Config.Set("network_w_ipaddress_type","NONE");
	MainWindow::m()->Config.Set("network_w_ipaddress_ip","");
	MainWindow::m()->Config.Set("network_w_ipaddress_mask","");
	MainWindow::m()->Config.Set("network_w_ipaddress_gateway","");
	MainWindow::m()->Config.Set("network_w_ipaddress_dns","");
	MainWindow::m()->Config.Set("network_w_ipaddress_ssid","");
	MainWindow::m()->Config.Set("network_w_ipaddress_password","");
	MainWindow::m()->Config.Set("network_w_ipaddress_wkeymgmt","");


	MainWindow::m()->Config.Set("account_usermail","");
	MainWindow::m()->Config.Set("account_password","");
	MainWindow::m()->Config.Set("account_sync_server" , "1"); //同期させる

	//configの保存
	MainWindow::m()->SyncInvoke([=](){
		ScriptRemoconWeb::SettingPageIPAddressOverriade();
		MainWindow::m()->Config.overrideSave();
	});

	//configの同期を行う(失敗するかもしれんけど)
	MainWindow::m()->ColudSyncPooling.WakeupSyncConfigByconfig();
	SystemMisc::WaitForIPAddress(120);

	//サーバーに、私はここにいる、と伝える。
	const string uuid = MainWindow::m()->Httpd.GetUUID();
	const string url = MainWindow::m()->Httpd.getServerTop();

	unsigned int retry = 8;
	for(; retry > 0 ; retry --)
	{
		const string serverresult = MainWindow::m()->ColudSyncPooling.OpeartionWeAreHere(uuid,url);
		if (serverresult != "OK")
		{
			DEBUGLOG("ResetConfigIRR: WeAreHereしたら、サーバから拒絶されました。retry:" << retry << " uuid:" << uuid  );
			SecSleep(1);
			continue;
		}

		break;
	}

	//サーバと同期がとれなかった?
	if (retry <= 0)
	{
		ERRORLOG("ResetConfigIRR: WeAreHereしたら、サーバから拒絶されました。"  );
	}
	
	return 1;
}
#endif //WITH_CLIENT_ONLY_CODE==1
