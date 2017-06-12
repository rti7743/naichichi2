//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
// Speak_Festival.cpp: Speak_Festival クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "ScriptRunner.h"
#include "Speak_Factory.h"
#include "Speak_Festival.h"
#include "MainWindow.h"
#include "XLHttpSocket.h"
#include "XLFileUtil.h"
#include "XLStringUtil.h"
#include "SystemMisc.h"
#if _MSC_VER
#endif

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Speak_Festival::Speak_Festival(const string& voiceType)
{
	this->Thread = NULL;
	this->StopFlag = false;
	this->CancelFlag = false;
	this->VoiceType = voiceType;
}

Speak_Festival::~Speak_Festival()
{
	this->StopFlag = true;
	this->EventObject.notify_one();
	if (this->Thread)
	{
		this->Thread->join();
		delete this->Thread;
	}
	
	if ((!this->SpeakTextFilename.empty()) && XLFileUtil::Exist(this->SpeakTextFilename) )
	{
		XLFileUtil::del(this->SpeakTextFilename);
	}

}

//音声認識のためのオブジェクトの構築.
bool Speak_Festival::Create()
{
	assert(this->Thread == NULL);

	this->StopFlag = false;
	this->SpeakTextFilename = XLFileUtil::getTempDirectory("naichichi2") + "/festival_text";
	{
		const string voiceTypeLow = XLStringUtil::replace(this->VoiceType,"festival_","");

		//festivalはlispとかいろいろ搭載しているので、外部exeとして動かすわ・・・統合するのはめげる.
#if _MSC_VER
		const auto festivalEXE = MainWindow::m()->GetConfigBasePath("festival\\festival.exe");
#else
		const auto festivalEXE = MainWindow::m()->GetConfigBasePath("festival/festival.bin");
#endif
		const auto festivalScript = voiceTypeLow + ".scm";
		const auto festivalTempText = this->SpeakTextFilename;

		this->FestivalCMDLine = festivalEXE + " --script " + festivalScript + " " + festivalTempText + " -o " ;
	}

	this->CacheDB.Create("wav");

	this->Thread = new thread([=](){
		this->Run(); 
	} );

	return true;
}

void Speak_Festival::Run()
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
void Speak_Festival::PushQueeOnly(const CallbackPP& callback,const string & str,bool noplay)
{
	lock_guard<mutex> al(this->Lock);

	//キューに積んで、読み上げスレッドに通知する.
	this->SpeakQueue.push_back(SpeakTask(callback,str,noplay));
}
void Speak_Festival::FireQuee()
{
	this->EventObject.notify_one();
}
void Speak_Festival::SpeakSync(const string & str,bool noplay)
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
		//festivalはlispとかいろいろ搭載しているので、外部exeとして動かすわ・・・統合するのはめげる.

		//読み上げる内容をファイルに書く
		const string romaji = MainWindow::m()->MecabControl.KanjiAndKanaToRomaji(str,false);
		XLFileUtil::write(this->SpeakTextFilename,romaji  );

		//festival実行 wavを生成する
		const string cmd = this->FestivalCMDLine + wavfilename;
		const string stdout_ = SystemMisc::PipeExec(cmd);
		if (! XLFileUtil::Exist(wavfilename) )
		{
			ERRORLOG( "読み上げることができませんでした " << romaji << " " <<  stdout_ << " " << cmd );
		}

		if (! noplay )
		{
			MainWindow::m()->MusicPlayAsync.PlaySyncLow(wavfilename);
		}
	}

}


void Speak_Festival::Cancel()
{
	lock_guard<mutex> al(this->Lock);

	this->SpeakQueue.clear();
	this->CancelFlag = true;
}

void Speak_Festival::RemoveCallback(const CallbackPP& callback , bool is_unrefCallback) 
{
	lock_guard<mutex> al(this->Lock);

}

//このモデルを実行できるspeakmodelをすべて取得する (static)
list<string> Speak_Festival::getSpeakModelList() 
{
	list<string> arr;
	const auto dir = MainWindow::m()->GetConfigBasePath("festival/");
	XLFileUtil::findfile( dir , [&](const string& filename,const string& fullfilename) -> bool {

		if (XLStringUtil::baseext(filename) != ".scm")
		{
			return true;
		}
		arr.push_back("festival_" + XLStringUtil::basenameonly(filename) );

		return true;
	});

	return arr;
}

void Speak_Festival::DeleteSpeakCacheDB(const string& str)
{
	lock_guard<mutex> al(this->Lock);
	this->CacheDB.Delete(str);
}
void Speak_Festival::ClearSpeakCacheDB()
{
	lock_guard<mutex> al(this->Lock);
	this->CacheDB.Clear();
}


#endif //WITH_CLIENT_ONLY_CODE==1
