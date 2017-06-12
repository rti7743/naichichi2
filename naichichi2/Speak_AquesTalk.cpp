//結局使いませんでした
#if WITH_CLIENT_ONLY_CODE==1
// Speak_AquesTalk.cpp: Speak_AquesTalk クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "ScriptRunner.h"
#include "Speak_Factory.h"
#include "Speak_AquesTalk.h"
#include "MainWindow.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Speak_AquesTalk::Speak_AquesTalk(const string& botname)
{
	this->Thread = NULL;
	this->StopFlag = false;
	this->CancelFlag = false;

	this->BotName = botname;
	this->AquesTalk2_Free = NULL;
	this->AquesTalk2_Synthe = NULL;
	this->AquesTalk_FreeWave = NULL;
	this->AquesTalk_Synthe = NULL;
}

Speak_AquesTalk::~Speak_AquesTalk()
{
	this->StopFlag = true;
	this->EventObject.notify_one();
	this->Thread->join();
	delete this->Thread;
}

//音声認識のためのオブジェクトの構築.
bool Speak_AquesTalk::Create()
{
	assert(this->Thread == NULL);
	
	
	this->LoadAquesTalk();
	this->SelectBot();

	this->CacheDB.Create("wav");

	this->StopFlag = false;
	this->Thread = new thread([=](){
		try
		{
			this->Run(); 
		}
		catch(XLException &e)
		{
			ERRORLOG( e.what() );
			MainWindow::m()->Shutdown(EXITCODE_LEVEL_ERROR,true); //危険なので強制終了
		}
	} );
	return true;
}

//ぼっとを選択する.
void Speak_AquesTalk::SelectBot()
{
	if (getVersion() == 2)
	{
		if (! this->BotName.empty())
		{
			if ( XLStringUtil::baseext(this->BotName) != ".phont" )
			{
				this->BotName += ".phont";
			}
			
			if (! XLFileUtil::cat_b( MainWindow::m()->GetConfigBasePath("./phont/" + this->BotName) , &this->PhontDat) )
			{
				if (! XLFileUtil::cat_b( MainWindow::m()->GetConfigBasePath("./" + this->BotName) , &this->PhontDat) )
				{
					if (! XLFileUtil::cat_b(this->BotName , &this->PhontDat) )
					{
						//ないので、ディフォルトを使おう!
						this->PhontDat.clear();
						ERRORLOG("Speak_AquesTalk 指定された、音声データ " + this->BotName + "を読み込むことができませんでした" );
					}
				}
			}
		}
	}
}


//AquesTalkを何とかして読み込む
bool Speak_AquesTalk::LoadAquesTalk()
{
	this->AquesTalk_FreeWave = NULL;
	this->AquesTalk_Synthe = NULL;
	this->AquesTalk2_Synthe = NULL;
	this->AquesTalk2_Free = NULL;

#if _MSC_VER
	string aquesTalk = "AquesTalk2.dll";
	string aquesTalk2 = "AquesTalk.dll";
#else
	string aquesTalk = "AquesTalk2.so";
	string aquesTalk2 = "AquesTalk.so";
#endif
	if (!  this->Lib.Load( MainWindow::m()->GetConfigBasePath("./" + aquesTalk2) ) )
	{
		this->AquesTalk2_Synthe =  (AquesTalk2_SyntheDef) this->Lib.GetProcAddress("AquesTalk2_Synthe");
		this->AquesTalk2_Free = (AquesTalk2_FreeDef) this->Lib.GetProcAddress("AquesTalk2_Free");
		return true;
	}
	if (!  this->Lib.Load(aquesTalk2) )
	{
		this->AquesTalk2_Synthe = (AquesTalk2_SyntheDef) this->Lib.GetProcAddress("AquesTalk2_Synthe");
		this->AquesTalk2_Free = (AquesTalk2_FreeDef) this->Lib.GetProcAddress("AquesTalk2_Free");
		return true;
	}

	if (!  this->Lib.Load( MainWindow::m()->GetConfigBasePath("./" + aquesTalk) ) )
	{
		this->AquesTalk_Synthe = (AquesTalk_SyntheDef) this->Lib.GetProcAddress("AquesTalk_Synthe");
		this->AquesTalk_FreeWave = (AquesTalk_FreeWaveDef) this->Lib.GetProcAddress("AquesTalk_FreeWave");
		return true;
	}
	if (!  this->Lib.Load(aquesTalk) )
	{
		this->AquesTalk_Synthe = (AquesTalk_SyntheDef) this->Lib.GetProcAddress("AquesTalk_Synthe");
		this->AquesTalk_FreeWave = (AquesTalk_FreeWaveDef) this->Lib.GetProcAddress("AquesTalk_FreeWave");
		return true;
	}


	throw XLException(aquesTalk2 + " または " + aquesTalk + " が読み込めません。" );
}


void Speak_AquesTalk::Run()
{
	while(!this->StopFlag)
	{
/*
		bool istask = false;
		//キューがあるかどうか確認.
		{
			lock_guard<mutex> al(this->Lock);
			istask = (this->SpeakQueue.size() >= 1);
		}
		//タスクがないならば、タスクが入るまで寝る.
		if (!istask)
		{
			
		}
*/
		{
			unique_lock<mutex> al(this->Lock);
			this->CancelFlag = false;
			if (this->SpeakQueue.size() <= 0)
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

			if (this->SpeakQueue.size() <= 0)
			{
				continue;
			}
			task = *(this->SpeakQueue.begin());
			this->SpeakQueue.pop_front();
		}

		SpeakSync(task.text,task.noplay);
		if (this->StopFlag)
		{
			return;
		}

		if (!this->CancelFlag)
		{

		}
		else
		{
			//コールバックする.
			MainWindow::m()->ScriptManager.SpeakEnd(task.callback,task.text);
		}
	}
}

void Speak_AquesTalk::PushQueeOnly(const CallbackPP& callback,const string & str,bool noplay)
{
	lock_guard<mutex> al(this->Lock);

	//キューに積んで、読み上げスレッドに通知する.
	this->SpeakQueue.push_back(SpeakTask(callback,str,noplay));
}
void Speak_AquesTalk::FireQuee()
{
	this->EventObject.notify_one();
}

void Speak_AquesTalk::SpeakSync(const string & str,bool noplay)
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
		//aques talkにwaveを生成させる。
		int size;
		int speed = 1;
		unsigned char * wavData = NULL;
		if (getVersion() == 1)
		{
			wavData = this->AquesTalk_Synthe(str.c_str(), speed ,&size  );
		}
		else 
		{
			wavData = this->AquesTalk2_Synthe(str.c_str(), speed ,&size , (void*) (this->PhontDat.empty() ? NULL : &this->PhontDat[0])  );
		}
		if (wavData == NULL)
		{//エラー
			return;
		}

		//結果を保存するファイル名
		XLFileUtil::write(wavfilename,(const char*)wavData,size);

		//AquesTalkのデータを開放する.
		if (getVersion() == 1)
		{
			this->AquesTalk2_Free(wavData);
		}
		else 
		{
			this->AquesTalk_FreeWave(wavData);
		}
		wavData = NULL;

		if (! noplay )
		{
			MainWindow::m()->MusicPlayAsync.PlaySyncLow(wavfilename);
		}
	}
}
void Speak_AquesTalk::Cancel()
{
	lock_guard<mutex> al(this->Lock);

	this->SpeakQueue.clear();
	this->CancelFlag = true;
}

int Speak_AquesTalk::getVersion() const
{
	return this->AquesTalk2_Synthe == NULL ? 1 : 2;
}

void Speak_AquesTalk::RemoveCallback(const CallbackPP& callback , bool is_unrefCallback) 
{
}
//このモデルを実行できるspeakmodelをすべて取得する (static)
list<string> Speak_AquesTalk::getSpeakModelList() 
{
	list<string> arr;
	return arr;
}
void Speak_AquesTalk::DeleteSpeakCacheDB(const string& str)
{
	lock_guard<mutex> al(this->Lock);
	this->CacheDB.Delete(str);
}
void Speak_AquesTalk::ClearSpeakCacheDB()
{
	lock_guard<mutex> al(this->Lock);
	this->CacheDB.Clear();
}

#endif //WITH_CLIENT_ONLY_CODE==1
