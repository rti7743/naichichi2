//結局使いませんでした そりゃそうだろう
//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
// Speak_GoogleTranslate.cpp: Speak_GoogleTranslate クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "ScriptRunner.h"
#include "Speak_Factory.h"
#include "Speak_GoogleTranslate.h"
#include "MainWindow.h"
#include "XLHttpSocket.h"
#include "XLFileUtil.h"
#include "XLStringUtil.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Speak_GoogleTranslate::Speak_GoogleTranslate()
{
	this->Thread = NULL;
	this->StopFlag = false;
	this->CancelFlag = false;
}

Speak_GoogleTranslate::~Speak_GoogleTranslate()
{
	this->StopFlag = true;
	this->EventObject.notify_one();
	this->Thread->join();
	delete this->Thread;
}

//音声認識のためのオブジェクトの構築.
bool Speak_GoogleTranslate::Create()
{
	assert(this->Thread == NULL);

	this->CacheDB.Create("mp3");

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

void Speak_GoogleTranslate::Run()
{
	while(!this->StopFlag)
	{
		{
			unique_lock<mutex> al(this->Lock);
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
			this->CancelFlag = false;

			if (this->SpeakQueue.size() <= 0)
			{
				continue;
			}
			task = *(this->SpeakQueue.begin());
			this->SpeakQueue.pop_front();
		}

		if (this->StopFlag)
		{
			return;
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

void Speak_GoogleTranslate::PushQueeOnly(const CallbackPP& callback,const string & str,bool noplay)
{
	lock_guard<mutex> al(this->Lock);

	//キューに積んで、読み上げスレッドに通知する.
	this->SpeakQueue.push_back(SpeakTask(callback,str,noplay));
}
void Speak_GoogleTranslate::FireQuee()
{
	this->EventObject.notify_one();
}


void Speak_GoogleTranslate::SpeakSync(const string & str,bool noplay)
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
		//ダウンロードの準備
		string lang = "ja";
		string encoding = "utf_8";	

		map<string , string> param;
		param["q"]      = XLStringUtil::urlencode(_A2U(str.c_str()));
		param["ie"]		= "UTF-8";
		param["tl"]		= "ja";
		param["total"]	= "1";
		param["idx"]	= "0";

		map<string,string> header;
		vector<char> retBinary;
		XLHttpHeader retHeader;

		header["host"] = "translate.google.co.jp";
		header["connection"] = "close";
		header["accept-encoding"] = "identity;q=1, *;q=0";
		header["user-agent"] = "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/536.11 (KHTML, like Gecko) Chrome/20.0.1132.57 Safari/536.11";
		header["accept"] = "*/*";
//			header["x-chrome-variations"] = "CL21yQEIiLbJAQibtskBCKO2yQEIqLbJAQi5g8oBCMaDygE=";
		header["referer"] = "http://translate.google.co.jp/";
		header["accept-language"] = "ja,en-US;q=0.8,en;q=0.6";
		header["accept-charset"] = "Shift_JIS,utf-8;q=0.7,*;q=0.3";
		header["range"] = "bytes=0-";

		XLHttpSocket::GetBinary("http://translate.google.com/translate_tts?" + XLStringUtil::crossjoin("=","&",param),header,60,&retBinary,&retHeader);

		//結果を保存する
		XLFileUtil::write(wavfilename,retBinary);

		if (!noplay)
		{
			MainWindow::m()->MusicPlayAsync.PlaySyncLow(wavfilename);
		}
	}
}

void Speak_GoogleTranslate::Cancel()
{
	lock_guard<mutex> al(this->Lock);

	this->SpeakQueue.clear();
	this->CancelFlag = true;
}

void Speak_GoogleTranslate::RemoveCallback(const CallbackPP& callback , bool is_unrefCallback) 
{
	lock_guard<mutex> al(this->Lock);

}

//このモデルを実行できるspeakmodelをすべて取得する (static)
list<string> Speak_GoogleTranslate::getSpeakModelList() 
{
	list<string> arr;
	arr.push_back("google_ja");
	arr.push_back("google_en");
	return arr;
}

void Speak_GoogleTranslate::DeleteSpeakCacheDB(const string& str)
{
	lock_guard<mutex> al(this->Lock);
	this->CacheDB.Delete(str);
}
void Speak_GoogleTranslate::ClearSpeakCacheDB()
{
	lock_guard<mutex> al(this->Lock);
	this->CacheDB.Clear();
}

#endif //WITH_CLIENT_ONLY_CODE==1
