#include "common.h"
#include "SoundPlay.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"
#include "MainWindow.h"


#ifdef _MSC_VER
#pragma comment(lib,"winmm.lib")
#else
#endif


SoundPlay::SoundPlay()
{
}
SoundPlay::~SoundPlay()
{
}

void SoundPlay::Init()
{
}

void SoundPlay::Free()
{
}
void SoundPlay::Play(const string & filename)
{
#ifdef _MSC_VER
	MainWindow::m()->PlayMCISync(filename);
#else
	//Linuxだと とりあえず mplayer にまかせる
	//要検討箇所
	const string ext = XLStringUtil::strtolower(XLStringUtil::baseext_nodot(filename));
	if (ext == "mp3")
	{
		const string cmd = "nice -n -1 /usr/bin/mplayer -vo null \"" + filename + "\" > /dev/null ";
		system(cmd.c_str() );
	}
	else if (ext == "wav")
	{
		const string cmd = "nice -n -1 /usr/bin/mplayer -vo null \"" + filename + "\" > /dev/null ";
		system(cmd.c_str() );
	}
#endif
}

