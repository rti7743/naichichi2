//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
// Speak_OpenJTalk.cpp: Speak_OpenJTalk クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "ScriptRunner.h"
#include "Speak_Factory.h"
#include "Speak_OpenJTalk.h"
#include "MainWindow.h"
#include "XLHttpSocket.h"
#include "XLFileUtil.h"
#include "XLStringUtil.h"
#include "MecabControl.h"
#include "../openjtalk/open_jtalk/bin/open_jtalkdll.h"
#if _MSC_VER
	#pragma comment(lib, "winmm.lib")
	#pragma comment(lib, "openjtalk.lib")
#endif

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Speak_OpenJTalk::Speak_OpenJTalk(const string& VoiceType): VoiceType(VoiceType)
{
	this->Thread = NULL;
	this->StopFlag = false;
	this->CancelFlag = false;
	this->OpenJTalkHandle = NULL;
}

Speak_OpenJTalk::~Speak_OpenJTalk()
{
	this->StopFlag = true;
	this->EventObject.notify_one();
	if (this->Thread)
	{
		this->Thread->join();
		delete this->Thread;
	}

	if (this->OpenJTalkHandle)
	{
		OpenJTalk_Delete(&this->OpenJTalkHandle);
		this->OpenJTalkHandle = NULL;
	}
}

//音声認識のためのオブジェクトの構築.
bool Speak_OpenJTalk::Create()
{
	assert(this->Thread == NULL);

	this->StopFlag = false;
	if ( ! CreateOpenJtalk(this->VoiceType,&this->OpenJTalkHandle , &this->Options ) )
	{
		throw XLEXCEPTION(string("openjtalk構築失敗") << OpenJTalk_GetLastError(&this->OpenJTalkHandle));
	}

	this->CacheDB.Create("wav");
	
	this->Thread = new thread([=](){
		XLDebugUtil::SetThreadName("Speak_OpenJTalk");
		this->Run(); 
	} );

	return true;
}

bool Speak_OpenJTalk::CreateOpenJtalk(const string& voiceType,OpenJTalk** openJTalkHandle ,vector<string>* options ) const
{
	const string voiceTypeLow = XLStringUtil::replace(voiceType,"openjtalk_","");
	const string openjtalkpath = MainWindow::m()->GetConfigBasePath("openjtalk/" + voiceTypeLow) + XLStringUtil::pathseparator("/");
	
	*options = XLStringUtil::split_vector(" ", XLFileUtil::cat( openjtalkpath + "option.txt" ) );
	if ( XLFileUtil::Exist(openjtalkpath + "gv-switch.inf") ){ options->push_back("-k"); options->push_back(openjtalkpath + "gv-switch.inf"); } 

	if ( XLFileUtil::Exist(openjtalkpath + "tree-mgc.inf") ){ options->push_back("-tm"); options->push_back(openjtalkpath + "tree-mgc.inf"); } 
	if ( XLFileUtil::Exist(openjtalkpath + "tree-lf0.inf") ){ options->push_back("-tf"); options->push_back(openjtalkpath + "tree-lf0.inf"); } 
	if ( XLFileUtil::Exist(openjtalkpath + "tree-lpf.inf") ){ options->push_back("-tl"); options->push_back(openjtalkpath + "tree-lpf.inf"); } 
	if ( XLFileUtil::Exist(openjtalkpath + "tree-dur.inf") ){ options->push_back("-td"); options->push_back(openjtalkpath + "tree-dur.inf"); } 

	if ( XLFileUtil::Exist(openjtalkpath + "dur.pdf") ){ options->push_back("-md"); options->push_back(openjtalkpath + "dur.pdf"); } 
	if ( XLFileUtil::Exist(openjtalkpath + "mgc.pdf") ){ options->push_back("-mm"); options->push_back(openjtalkpath + "mgc.pdf"); } 
	if ( XLFileUtil::Exist(openjtalkpath + "lf0.pdf") ){ options->push_back("-mf"); options->push_back(openjtalkpath + "lf0.pdf"); } 
	if ( XLFileUtil::Exist(openjtalkpath + "lpf.pdf") ){ options->push_back("-ml"); options->push_back(openjtalkpath + "lpf.pdf"); } 

	if ( XLFileUtil::Exist(openjtalkpath + "tree-gv-mgc.inf") ){ options->push_back("-em"); options->push_back(openjtalkpath + "tree-gv-mgc.inf"); } 
	if ( XLFileUtil::Exist(openjtalkpath + "tree-gv-lf0.inf") ){ options->push_back("-ef"); options->push_back(openjtalkpath + "tree-gv-lf0.inf"); } 
	if ( XLFileUtil::Exist(openjtalkpath + "gv-mgc.pdf") ){ options->push_back("-cm"); options->push_back(openjtalkpath + "gv-mgc.pdf"); } 
	if ( XLFileUtil::Exist(openjtalkpath + "gv-lf0.pdf") ){ options->push_back("-cf"); options->push_back(openjtalkpath + "gv-lf0.pdf"); } 

	if ( XLFileUtil::Exist(openjtalkpath + "mgc.win1") ){ options->push_back("-dm"); options->push_back(openjtalkpath + "mgc.win1"); } 
	if ( XLFileUtil::Exist(openjtalkpath + "mgc.win2") ){ options->push_back("-dm"); options->push_back(openjtalkpath + "mgc.win2"); } 
	if ( XLFileUtil::Exist(openjtalkpath + "mgc.win3") ){ options->push_back("-dm"); options->push_back(openjtalkpath + "mgc.win3"); } 
	if ( XLFileUtil::Exist(openjtalkpath + "lf0.win1") ){ options->push_back("-df"); options->push_back(openjtalkpath + "lf0.win1"); } 
	if ( XLFileUtil::Exist(openjtalkpath + "lf0.win2") ){ options->push_back("-df"); options->push_back(openjtalkpath + "lf0.win2"); } 
	if ( XLFileUtil::Exist(openjtalkpath + "lf0.win3") ){ options->push_back("-df"); options->push_back(openjtalkpath + "lf0.win3"); } 
	if ( XLFileUtil::Exist(openjtalkpath + "lpf.win1") ){ options->push_back("-dl"); options->push_back(openjtalkpath + "lpf.win1"); } 
	if ( XLFileUtil::Exist(openjtalkpath + "lpf.win2") ){ options->push_back("-dl"); options->push_back(openjtalkpath + "lpf.win2"); } 
	if ( XLFileUtil::Exist(openjtalkpath + "lpf.win3") ){ options->push_back("-dl"); options->push_back(openjtalkpath + "lpf.win3"); } 
	options->push_back("-z"); options->push_back("0");

	const char* argv[100] = { NULL };
	argv[0] = "openjtalk";

	int argc = 1;
	for(auto it = options->begin() ; it != options->end() ; ++it)
	{
		argv[argc++] = it->c_str();
	}

	if (! OpenJTalk_Create(openJTalkHandle ,argc , argv)	)
	{
		throw XLEXCEPTION(string("openjtalk構築失敗") << OpenJTalk_GetLastError(openJTalkHandle));
	}
	return true;
}

void Speak_OpenJTalk::Run()
{
	

	while(!this->StopFlag)
	{
		{
			unique_lock<mutex> al(this->Lock);
			if (this->SpeakQueue.empty() )
			{
				this->EventObject.wait(al);
			}
		}
		//寝起きかもしれないので終了条件の確認.
		if (this->StopFlag)
		{
			return;
		}

		//読み上げる文字列をキューから取得.
		SpeakTask task;
		{
			lock_guard<mutex> al(this->Lock);
			this->CancelFlag = false;

			if (this->SpeakQueue.empty() )
			{
				continue;
			}
			task = *(this->SpeakQueue.begin());
			this->SpeakQueue.pop_front();
		}

		SpeakSync(task.text,task.noplay);
		if (this->CancelFlag)
		{

		}
		else
		{
			//コールバックする.
			MainWindow::m()->ScriptManager.SpeakEnd(task.callback,task.text);
		}
	}
}
void Speak_OpenJTalk::PushQueeOnly(const CallbackPP& callback,const string & str,bool noplay)
{
	lock_guard<mutex> al(this->Lock);

	//キューに積んで、読み上げスレッドに通知する.
	this->SpeakQueue.push_back(SpeakTask(callback,str,noplay));
}
void Speak_OpenJTalk::FireQuee()
{
	this->EventObject.notify_one();
}


void Speak_OpenJTalk::SpeakSync(const string & str,bool noplay)
{
	lock_guard<mutex> al(this->Lock);

	//キャッシュにあるかな？
	string wavfilename;
	if ( this->CacheDB.Find(str,&wavfilename) )
	{
		if (! noplay )
		{
			MainWindow::m()->MusicPlayAsync.PlaySyncLow(wavfilename);
		}
	}
	else
	{
		DEBUGLOG("speak cache no hit " << str);

		//データを取り直す.
		auto mecabFeatures = MainWindow::m()->MecabControl.mallocFeatures(str.c_str());
		if(mecabFeatures)
		{
			OpenJTalk_synthesis_toplay_savefile(&this->OpenJTalkHandle,mecabFeatures , wavfilename.c_str() );

			MainWindow::m()->MecabControl.FreeFeatures(mecabFeatures);
			if (! noplay )
			{
				MainWindow::m()->MusicPlayAsync.PlaySyncLow(wavfilename);
			}
		}
	}

}


void Speak_OpenJTalk::Cancel()
{
	lock_guard<mutex> al(this->Lock);

	this->SpeakQueue.clear();
	this->CancelFlag = true;
}

void Speak_OpenJTalk::RemoveCallback(const CallbackPP& callback , bool is_unrefCallback) 
{
	lock_guard<mutex> al(this->Lock);

}

list<string> Speak_OpenJTalk::getSpeakModelList() 
{	
	list<string> arr;

	const auto dir = MainWindow::m()->GetConfigBasePath("openjtalk/");
	XLFileUtil::findfile( dir , [&](const string& filename,const string& fullfilename) -> bool {

		if (! XLFileUtil::Exist(fullfilename + XLStringUtil::pathseparator("/") + "option.txt" ) )
		{
			return true;
		}
		arr.push_back("openjtalk_" + filename);

		return true;
	});

	return arr;
}

void Speak_OpenJTalk::DeleteSpeakCacheDB(const string& str)
{
	lock_guard<mutex> al(this->Lock);
	this->CacheDB.Delete(str);
}
void Speak_OpenJTalk::ClearSpeakCacheDB()
{
	lock_guard<mutex> al(this->Lock);
	this->CacheDB.Clear();
}

#endif //WITH_CLIENT_ONLY_CODE==1
