//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
// MusicPlayAsync.cpp: MusicPlayAsync クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "ScriptRunner.h"
#include "Speak_Factory.h"
#include "MusicPlayAsync.h"
#include "MainWindow.h"
#include "XLHttpSocket.h"
#include "XLFileUtil.h"
#include "XLStringUtil.h"
#include "FHCCommander.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

MusicPlayAsync::MusicPlayAsync()
{
	this->Thread = NULL;
	this->StopFlag = false;
	this->StopPlayFlag = false;
}

MusicPlayAsync::~MusicPlayAsync()
{
	DEBUGLOG("stop...");

	this->StopFlag = true;
	this->EventObject.notify_one();
	if (this->Thread)
	{
		this->Thread->join();
		delete this->Thread;
	}

	DEBUGLOG("done");
}

bool MusicPlayAsync::Create()
{
	assert(this->Thread == NULL);
	
	
	this->StopFlag = false;
	this->StopPlayFlag = false;
	this->Thread = new thread([=](){
		XLDebugUtil::SetThreadName("MusicPlayAsync");
		this->Run(); 
	} );
	return true;
}

void MusicPlayAsync::Run()
{
	

	while(!this->StopFlag)
	{
		{
			unique_lock<mutex> al(this->Lock);
			if (this->PlayQueue.size() <= 0)
			{
				this->EventObject.wait(al);
			}
		}
		//寝起きかもしれないので終了条件の確認.
		if (this->StopFlag)
		{
			return;
		}

		//読み上げるファイルをキューから取得.
		MusicPlayAsyncTask task;
		{
			lock_guard<mutex> al(this->Lock);

			if (this->PlayQueue.size() <= 0)
			{
				continue;
			}
			task = *(this->PlayQueue.begin());
			this->PlayQueue.pop_front();
		}

		//再生
		for(int i = 0 ; i < task.playcount ; i++ )
		{
			if (this->StopFlag)
			{//停止フラグ
				break;
			}
			if (this->StopPlayFlag)
			{//再生停止フラグ
				break;
			}

			this->PlaySync(task.filename);
		}

		if ( ! task.callback.Empty() )
		{
			//コールバックする.
			MainWindow::m()->ScriptManager.SpeakEnd(task.callback,task.filename);
		}

		this->StopPlayFlag = false;
	}
}

bool MusicPlayAsync::IsExist(const string & filename)
{
	if (filename.empty()) return false;
	const string f = MainWindow::m()->GetConfigBasePath("./webroot/user/tospeak_mp3/" + filename);
	return XLFileUtil::Exist(f);
}

bool MusicPlayAsync::Play(const CallbackPP& callback,const string & filename,unsigned int loop=1)
{
	ASSERT___IS_THREADFREE();
	if ( ! IsExist(filename) ) return false;

	lock_guard<mutex> al(this->Lock);

	//キューに積んで、読み上げスレッドに通知する.
	this->PlayQueue.push_back(MusicPlayAsyncTask(callback,filename,loop));
	this->EventObject.notify_one();

	return true;
}

bool MusicPlayAsync::Play(const string & filename,unsigned int loop=1)
{
	ASSERT___IS_THREADFREE();
	if (filename.empty()) return false;
	return Play(CallbackPP::NoCallback(),filename,loop);
}

bool MusicPlayAsync::PlaySync(const string & filename)
{
	ASSERT___IS_THREADFREE();
	if (filename.empty()) return false;
	const string fullpath = MainWindow::m()->GetConfigBasePath("./webroot/user/tospeak_mp3/" + filename);
	return PlaySyncLow(fullpath);
}
bool MusicPlayAsync::PlaySyncLow(const string & fullpath)
{
	ASSERT___IS_THREADFREE();
	if (!XLFileUtil::Exist(fullpath) )
	{//ファイルがない!!
		return false;
	}

	MainWindow::m()->VolumeControll.ChangeSpeakerVolume();
	if ( MainWindow::m()->VolumeControll.GetSpeakerVolume() == 0)
	{
		NOTIFYLOG("スピーカーボリュームがゼロなので何もしません");
		return false;
	}

	//リモートスピーカーで音を再生する場合は、時間もったいないからスレッドでやる
	thread* playthread = NULL;
	if ( MainWindow::m()->Config.Get("speak_use_remote","") == "1" )
	{//リモートスピーカーを使う
		playthread = new thread([=](){
			XLDebugUtil::SetThreadName("MusicPlayAsync::RemoteSpaker");
			try
			{//リモートで再生する
				const string host = MainWindow::m()->Config.Get("speak_remote1_ip","");
				const string secret = MainWindow::m()->Config.Get("speak_remote1_secretkey","");
				FHCCommander::Play(host,secret,fullpath);
			}
			catch(XLException & e)
			{
				ERRORLOG(e.what());
				//通信できないエラーは無視
			}
		});
	}

	//実際に音を鳴らす
	{
		//ネイティブで音を鳴らすから順次ならせるようにロックする
		lock_guard<mutex> al(this->PlayLock);
		try
		{
			MainWindow::m()->Play(fullpath);
		}
		catch(XLException& e)
		{
			ERRORLOG("音楽ファイル " << fullpath << " 再生中にエラーが発生しました。" << e.what() );
		}
	}

	//リモートスピーカーの終了待ち
	if (playthread)
	{
		//リモート再生の終了確認.
		playthread->join();
		delete playthread;
	}

	return true;
}

bool MusicPlayAsync::RemoveCallback(const CallbackPP& callback , bool is_unrefCallback) 
{
	lock_guard<mutex> al(this->Lock);

	return true;
}

void MusicPlayAsync::StopPlay()
{
	ASSERT___IS_THREADFREE();

	this->StopPlayFlag = true;
	MainWindow::m()->StopPlay();
}


#endif //WITH_CLIENT_ONLY_CODE==1
