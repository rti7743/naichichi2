//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include "CapeBord.h"
#include "MainWindow.h"
#include "SystemMisc.h"
#if _MSC_VER
#else
#include "CapeBord_EndelS.h"
#include "CapeBord_IRR.h"
#endif

AutoLEDOn::AutoLEDOn()
{
	MainWindow::m()->CapeBord.TurnOnLED();
}
AutoLEDOn::~AutoLEDOn()
{
	MainWindow::m()->CapeBord.TurnOffLED();
}


CapeBord::CapeBord()
{
	this->Cape = NULL;
}
CapeBord::~CapeBord()
{
	delete this->Cape;
	this->Cape = NULL;
}
int CapeBord::getLumi()
{
	return this->Cape->getLumi();
}
float CapeBord::getTemp()
{
	return this->Cape->getTemp();
}
bool CapeBord::Capture(const string& path)
{
	return this->Cape->Capture(path);
}
bool CapeBord::Radiate(const string& path)
{
	return this->Cape->Radiate(path);
}

void CapeBord::TurnOnLED() 
{
	this->Cape->TurnOnLED();
}

void CapeBord::TurnOffLED() 
{
	this->Cape->TurnOffLED();
}

void CapeBord::Create()
{
	delete this->Cape;

#if _MSC_VER
	{
		this->Cape = new CapeBord_None;
		NOTIFYLOG("CapeBord_None が選択されました");
	}
#else
	if ( SystemMisc::IsIRR() )
	{
		this->Cape = new CapeBord_IRR();
		NOTIFYLOG("CapeBord_IRR が選択されました");
	}
	else if ( SystemMisc::IsEndel() )
	{
		this->Cape = new CapeBord_EndelS();
		NOTIFYLOG("CapeBord_EndelS が選択されました");
	}
	else
	{
		this->Cape = new CapeBord_None;
		NOTIFYLOG("CapeBord_None が選択されました");
	}
#endif
}

void CapeBord::SleepAndBrightLED(int patternCount,int miriSec)
{
	MainWindow::m()->CapeBord.TurnOnLED();
	for(int i = 0; i < patternCount ; i ++)
	{
		MiriSleep(miriSec/2);
		MainWindow::m()->CapeBord.TurnOffLED();
		MiriSleep(miriSec/2);
		MainWindow::m()->CapeBord.TurnOnLED();
	}
	MainWindow::m()->CapeBord.TurnOffLED();
}


#endif