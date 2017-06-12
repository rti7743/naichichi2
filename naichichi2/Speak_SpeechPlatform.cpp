//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
// Speak_SpeechPlatform.cpp: Speak_SpeechPlatform クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "ScriptRunner.h"
#include "Speak_Factory.h"
#include "Speak_SpeechPlatform.h"
#include "MainWindow.h"
//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
Speak_SpeechPlatform::Speak_SpeechPlatform()
{
	this->Thread = NULL;
	this->StopFlag = false;
	this->CancelFlag = false;
}

Speak_SpeechPlatform::~Speak_SpeechPlatform()
{
	this->StopFlag = true;
	this->EventObject.notify_one();
	this->Thread->join();
	delete this->Thread;
}

//音声認識のためのオブジェクトの構築.
bool Speak_SpeechPlatform::Create()
{
	assert(this->Thread == NULL);
	
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


bool Speak_SpeechPlatform::Run()
{
	

	//comの初期化
	COMInit cominit;

	//エンジンの構築
	HRESULT hr;

	hr = this->Engine.CoCreateInstance(CLSID_SpVoice);
	if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

	//ボットを登録する
	this->RegistVoiceBot("");

	//http://msdn.microsoft.com/en-us/library/ms720164(v=vs.85).aspx
	hr = this->Engine->SetInterest(SPFEI(SPEI_END_INPUT_STREAM), SPFEI(SPEI_END_INPUT_STREAM));
	if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

	//文章が来たら読み上げる
	while(!this->StopFlag)
	{
		{
			unique_lock<mutex> al(this->Lock);
			this->CancelFlag = false;
			if (this->SpeakQueue.empty())
			{
				this->EventObject.wait(al);
			}
		}
		//寝起きかもしれないので終了条件の確認.
		if (this->StopFlag)
		{
			return true;
		}

		//読み上げる文字列をキューから取得.
		SpeakTask task;
		{
			lock_guard<mutex> al(this->Lock);

			if (this->SpeakQueue.empty())
			{
				continue;
			}
			task = *(this->SpeakQueue.begin());
			this->SpeakQueue.pop_front();
		}
		if (!task.noplay)
		{
			hr = this->Engine->Speak( _A2W(task.text.c_str()) ,  SVSFIsXML,NULL);
			if(FAILED(hr))	throw XLException(XLException::StringWindows(hr));
		}
		
		if (this->CancelFlag)
		{

		}
		else
		{
			//コールバックする.
			MainWindow::m()->ScriptManager.SpeakEnd(task.callback,task.text);
		}
	}
	return true;
}

bool Speak_SpeechPlatform::RegistVoiceBot(const string & botname)
{
	//see http://msdn.microsoft.com/en-us/library/ms719807(v=vs.85).aspxs
	
	HRESULT hr;

	CComPtr<IEnumSpObjectTokens>   cpEnum;
	hr = SpEnumTokens(SPCAT_VOICES, NULL, NULL, &cpEnum);
	if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

	ULONG ulCount;
	hr = cpEnum->GetCount(&ulCount);
	if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

	string foundBotName = "";
	while (SUCCEEDED(hr) && ulCount--)
	{
		CComPtr<ISpObjectToken>        cpVoiceToken;
		hr = cpEnum->Next(1, &cpVoiceToken, NULL);
		if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

		CSpDynamicString curDesc;
		hr = SpGetDescription(cpVoiceToken, &curDesc);
		if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

		if (botname.empty() || strstr(_W2A(curDesc),botname.c_str()) != NULL )
		{
			hr = this->Engine->SetVoice(cpVoiceToken);
			if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

			return true;
		}
		else
		{
			foundBotName = foundBotName + _W2A(curDesc) + " ";
		}
	}
	throw XLException("読み上げるボット" + botname + "がみつかりません。見つかったボット:" + foundBotName);
}

void Speak_SpeechPlatform::PushQueeOnly(const CallbackPP& callback,const string & str,bool noplay)
{
	lock_guard<mutex> al(this->Lock);

	//キューに積んで、読み上げスレッドに通知する.
	this->SpeakQueue.push_back(SpeakTask(callback,str,noplay));
}

void Speak_SpeechPlatform::FireQuee()
{
	this->EventObject.notify_one();
}

void Speak_SpeechPlatform::SpeakSync(const string & str,bool noplay)
{
	
	HRESULT hr;

	lock_guard<mutex> al(this->Lock);

	if (!noplay)
	{
		hr = this->Engine->Speak( _A2W(str.c_str()) ,  SVSFIsXML,NULL);
		if(FAILED(hr))	throw XLException(XLException::StringWindows(hr));
	}

}


void Speak_SpeechPlatform::Cancel()
{
	lock_guard<mutex> al(this->Lock);

	this->SpeakQueue.clear();
	this->CancelFlag = true;
}

void Speak_SpeechPlatform::RemoveCallback(const CallbackPP& callback , bool is_unrefCallback) 
{
	lock_guard<mutex> al(this->Lock);

}

//このモデルを実行できるspeakmodelをすべて取得する (static)
list<string> Speak_SpeechPlatform::getSpeakModelList() 
{
	list<string> arr;
	arr.push_back("mssp_haruka");
	return arr;
}

void Speak_SpeechPlatform::DeleteSpeakCacheDB(const string& str)
{
	lock_guard<mutex> al(this->Lock);
	this->CacheDB.Delete(str);
}
void Speak_SpeechPlatform::ClearSpeakCacheDB()
{
	lock_guard<mutex> al(this->Lock);
	this->CacheDB.Clear();
}


#endif // WITH_CLIENT_ONLY_CODE==1
