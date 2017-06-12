//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include "CapeBord_IRR.h"
#include "MainWindow.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "XLFileUtil.h"
#include "SystemMisc.h"

CapeBord_IRR::CapeBord_IRR()
{
	this->CapeBord_IRRBIN = MainWindow::m()->GetConfigBasePath("linuxboot/irr/irr");

	const string beaglebone_ocp_dir = XLFileUtil::FindOneDirecotry("/sys/devices","ocp");
	if (beaglebone_ocp_dir.empty())
	{
		DEBUGLOG("/sys/devices/ocp*/ ディレクトリが見つかりません");
	}
	else
	{
		const string beaglebone_helper_dir = XLFileUtil::FindOneDirecotry(beaglebone_ocp_dir,"helper");
		if (beaglebone_helper_dir.empty())
		{
			DEBUGLOG("/sys/devices/ocp*/helper*/ ディレクトリが見つかりません");
		}
		else
		{
			this->TempAIN = beaglebone_helper_dir + "/" + "AIN1";
			this->LumiAIN = beaglebone_helper_dir + "/" + "AIN0";
		}
	}

	//Beaglebone LED OFF.
	TurnAllOffLED();
}

CapeBord_IRR::~CapeBord_IRR()
{
}

int CapeBord_IRR::getLumi()
{
	unique_lock<mutex> al(this->updatelock);

	const string lumiStr = XLFileUtil::cat(this->LumiAIN);
	int lumi = atoi(lumiStr);
	if (lumi < 0) lumi = 0;
	if (lumi > 300) lumi = 300;

	return lumi;
}

float CapeBord_IRR::getTemp()
{
	unique_lock<mutex> al(this->updatelock);

	const string tempStr = XLFileUtil::cat(this->TempAIN);
	int temp = atoi(tempStr);

	return (float)((1.0/(log((10000.f*(1023.f/(temp/1800.0f*1000.f)) - 10000.f) /10000.f)/3380.f+1.f/298.15f))+(-273.15f));
}

bool CapeBord_IRR::Capture(const string& path)
{
	unique_lock<mutex> al(this->updatelock);

	//優先度とかいろいろあるので別途バイナリを作ってそいつを呼び出す.
	//pathは安全なファイル名が渡されることを前提としている。

	//音声認識処理を止めて、トッププライオリティで。
	//IRQ割り込みを利用て、10usオーダーぐらいの処理をします。CPUが忙しいとダメです。
	const string command = this->CapeBord_IRRBIN + " save " + path;
	string out;
	MainWindow::m()->TopLevelInvoke([&](){
		out = SystemMisc::PipeExec(command);
	});
	DEBUGLOG("IRR  " << command << " @@OUT: " << out );
	
	if (! strstr(out.c_str(),"!OK!") )
	{
		ERRORLOG("IRR学習に失敗しました。" );
		return false;
	}

	DEBUGLOG("IRR学習できました。保存しました。 ");

	return true;
}

bool CapeBord_IRR::Radiate(const string& path)
{
	unique_lock<mutex> al(this->updatelock);

	//優先度とかいろいろあるので別途バイナリを作ってそいつを呼び出す.
	//pathは安全なファイル名が渡されることを前提としている。
	const string command = this->CapeBord_IRRBIN + " load " + path;
	const string out = SystemMisc::PipeExec(command);
	DEBUGLOG("IRR  " << command << " @@OUT: " << out );
	
	if (! strstr(out.c_str(),"!OK!") )
	{
		ERRORLOG("IRR送信に失敗しました。" );
		return false;
	}

	DEBUGLOG("IRR送信できました。" );

	return true;
}

void CapeBord_IRR::TurnOnLED()
{
	if (XLFileUtil::Exist("/sys/class/leds/beaglebone:green:usr0/brightness") )
	{//beaglebone black
		XLFileUtil::write("/sys/class/leds/beaglebone:green:usr0/brightness","1");
	}
	else if (XLFileUtil::Exist("/sys/class/leds/beaglebone::usr0/brightness") )
	{//beaglebone
//		XLFileUtil::write("/sys/class/leds/beaglebone::usr0/brightness","1");
	}
	else
	{
	}
}


void CapeBord_IRR::TurnOffLED()
{
	if (XLFileUtil::Exist("/sys/class/leds/beaglebone:green:usr0/brightness") )
	{//beaglebone black
		XLFileUtil::write("/sys/class/leds/beaglebone:green:usr0/brightness","0");
	}
	else if (XLFileUtil::Exist("/sys/class/leds/beaglebone::usr0/brightness") )
	{//beaglebone
//		XLFileUtil::write("/sys/class/leds/beaglebone::usr0/brightness","0");
	}
	else
	{
	}
}

void CapeBord_IRR::TurnAllOffLED()
{
	if (XLFileUtil::Exist("/sys/class/leds/beaglebone:green:usr0/brightness") )
	{//beaglebone black
		XLFileUtil::write("/sys/class/leds/beaglebone:green:usr0/brightness","0");
		XLFileUtil::write("/sys/class/leds/beaglebone:green:usr1/brightness","0");
		XLFileUtil::write("/sys/class/leds/beaglebone:green:usr2/brightness","0");
		XLFileUtil::write("/sys/class/leds/beaglebone:green:usr3/brightness","0");
	}
	else if (XLFileUtil::Exist("/sys/class/leds/beaglebone::usr0/brightness") )
	{//beaglebone
//		XLFileUtil::write("/sys/class/leds/beaglebone::usr0/brightness","0");
//		XLFileUtil::write("/sys/class/leds/beaglebone::usr1/brightness","0");
//		XLFileUtil::write("/sys/class/leds/beaglebone::usr2/brightness","0");
//		XLFileUtil::write("/sys/class/leds/beaglebone::usr3/brightness","0");
	}
	else
	{
	}
}
#endif