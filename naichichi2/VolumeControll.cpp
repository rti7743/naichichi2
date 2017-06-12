#include "common.h"
#include "MainWindow.h"
#include "XLFileUtil.h"
#include "XLStringUtil.h"
#include "SystemMisc.h"

VolumeControll::VolumeControll()
{
	this->NowSpeakerVolume = 0xff;
	this->NowMicVolume = 0xff;
}
VolumeControll::~VolumeControll()
{
}

void VolumeControll::Create()
{
	unsigned int recong_volume = MainWindow::m()->Config.GetInt("recong_volume", 100 );
	unsigned int speak_volume  = MainWindow::m()->Config.GetInt("speak_volume", 100 );

	this->NowSpeakerVolume = recong_volume;
	this->NowMicVolume = speak_volume;

	ChangeMicVolume();
	ChangeSpeakerVolume();
}

unsigned int VolumeControll::GetSpeakerVolume() const
{
	return this->NowSpeakerVolume;
}
unsigned int VolumeControll::GetMicVolume() const
{
	return this->NowMicVolume;
}

void VolumeControll::ChangeMicVolume() 
{
	unsigned int vol = MainWindow::m()->Config.GetInt("recong_volume", 100 );
	if (vol <= 0) vol = 0;
	if (vol >= 100) vol = 100;
	
	if (this->NowMicVolume == vol)
	{
		//既にそのボリュームなので何もしない
		return ;
	}
	DEBUGLOG("マイク音量変更(mic) 音量:" << vol)

	this->NowMicVolume = vol;
	SystemMisc::SetMicVolume(vol);
}

void VolumeControll::ChangeSpeakerVolume() 
{
	unsigned int vol;
	const time_t now = time(NULL);
	string logmessage;

	if ( VolumeControll::IsNightSpeakerVolume(now) )
	{
		vol = MainWindow::m()->Config.GetInt("speak_nightvolume_volume", 50 );
		logmessage = "夜間時間帯ですので、 vol(" + num2str(vol) + ")を適応します";
	}
	else
	{
		vol = MainWindow::m()->Config.GetInt("speak_volume", 100 );
		logmessage = "通常時間帯ですので、 vol(" + num2str(vol) + ")を適応します";
	}
	if (vol <= 0) vol = 0;
	if (vol >= 100) vol = 100;
	
	if (this->NowSpeakerVolume == vol)
	{
		//既にそのボリュームなので何もしない
		return ;
	}
	DEBUGLOG("スピーカー音量変更(speaker) " << logmessage)

	this->NowMicVolume = vol;
	SystemMisc::SetSpeakerVolume(vol);
}

bool VolumeControll::IsNightSpeakerVolume(const time_t& now) const
{
	if ( MainWindow::m()->Config.Get("speak_use_nightvolume","") != "1")
	{
		return false;
	}

#if _MSC_VER
	struct tm *date = localtime(&now);
#else
	struct tm datetmp;
	struct tm *date = &datetmp;
	localtime_r(&now,&datetmp);
#endif
	const int start_hour = MainWindow::m()->Config.GetInt("speak_nightvolume_start_hour",23);
	const int start_minute = MainWindow::m()->Config.GetInt("speak_nightvolume_start_minute",0);
	const int end_hour = MainWindow::m()->Config.GetInt("speak_nightvolume_end_hour",6);
	const int end_minute = MainWindow::m()->Config.GetInt("speak_nightvolume_end_minute",0);

	return XLStringUtil::IsTimeOver(date,start_hour,start_minute,end_hour,end_minute);
}
