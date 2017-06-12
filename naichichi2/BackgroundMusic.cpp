//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include "BackgroundMusic.h"
#include "MainWindow.h"
#include "SystemMisc.h"
#include "XLStringUtil.h"
#ifdef _MSC_VER
#else
#include <sys/wait.h>
#endif

BackgroundMusic::BackgroundMusic()
{
#ifdef _MSC_VER
	this->Player = NULL;
	this->Controls = NULL;
#else
#endif
}
BackgroundMusic::~BackgroundMusic()
{
	DEBUGLOG("stop...");
	Stop();
#ifdef _MSC_VER
#else
	this->StopFlag = true;
	this->EventObject.notify_one();
	if (this->Thread)
	{
		this->Thread->join();
		delete this->Thread;
	}
#endif
	DEBUGLOG("done");
}
	
bool BackgroundMusic::Create()
{
#ifdef _MSC_VER
	HRESULT hr = S_OK;

	hr = Player.CoCreateInstance( __uuidof(WindowsMediaPlayer), 0, CLSCTX_INPROC_SERVER );
	if(!SUCCEEDED(hr))
	{
		return false;
	}
	hr = Player->get_controls(&this->Controls);
	if(!SUCCEEDED(hr))
	{
		return false;
	}

#else
	this->StopFlag = false;
	this->Thread = new thread([=](){
		this->ThreadMain(); 
	} );

#endif
	return true;
}

void BackgroundMusic::ThreadMain()
{
#ifdef _MSC_VER
#else
	while(!this->StopFlag)
	{
		{
			unique_lock<mutex> al(this->Lock);
			this->EventObject.wait(al);
		}
		DEBUGLOG("wakeup");

		//寝起きかもしれないので終了条件の確認.
		if (this->StopFlag)
		{
			DEBUGLOG("this->StopFlag" );
			return;
		}
		
		string command;
		{
			lock_guard<mutex> al(this->Lock);
			command = this->RunCommand;
		}
		if (command.empty())
		{
			DEBUGLOG("command.empty()" );
			continue;
		}

		const string str = "/usr/bin/mplayer -vo null " + XLStringUtil::escapeshellarg_single(command) + " < /dev/null > /dev/null 2>&1";
		DEBUGLOG("system " << str );
		system(str.c_str());

		DEBUGLOG("wakeup");
	}
#endif
}

bool BackgroundMusic::Play(const string& command)
{
	DEBUGLOG("BackgroundMusic::Play");
#ifdef _MSC_VER

	if(Player && Controls)
	{
		Player->put_URL( BSTR(_A2W(command)) );
		Controls->play();
	}
	this->RunCommand = command;
#else
	Stop();
	
	DEBUGLOG("setup command");
	{
		lock_guard<mutex> al(this->Lock);
		this->RunCommand = command;
	}

	DEBUGLOG("fire play..");
	this->EventObject.notify_one();
#endif
	return true;
}
void BackgroundMusic::Stop()
{
	DEBUGLOG("BackgroundMusic::Stop");
#ifdef _MSC_VER
	if(Controls)
	{
		Controls->stop();
	}
	this->RunCommand = "";
#else

	string command;
	{
		lock_guard<mutex> al(this->Lock);
		command = this->RunCommand;
		//必ずここでクリアすること。
		this->RunCommand = "";
	}

	DEBUGLOG("search kill process..");
	auto plist = SystemMisc::ProcessList();
	for(auto it = plist.rbegin() ; it != plist.rend() ; ++it )
	{
		if ( strstr(it->processname.c_str(),"mplayer") != NULL)
		{
			if ( strstr(it->args.c_str(),command.c_str() ) != NULL)
			{
//				SystemMisc::PipeExec("kill "+num2str(it->pid));
				DEBUGLOG("kill " << it->pid);
				SystemMisc::KillProces(it->pid);
			}
		}
	}
	SecSleepEx(this->Lock,this->EventObject,1);
#endif
}

#endif //WITH_CLIENT_ONLY_CODE
