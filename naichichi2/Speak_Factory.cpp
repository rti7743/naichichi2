//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include "ScriptRunner.h"
#include "Speak_Factory.h"
#include "Speak_OpenJTalk.h"
#include "Speak_Festival.h"
#include "Speak_None.h"
#include "MainWindow.h"
#include "XLFileUtil.h"
#include "FHCCommander.h"

Speak_Factory::Speak_Factory()
{
	this->Engine = NULL;
}

Speak_Factory::~Speak_Factory()
{
	DEBUGLOG("stop...");

	delete this->Engine;

	DEBUGLOG("done");
}


bool Speak_Factory::Create()
{
	struct _inner
	{
		static bool isuse(const list<string>& a ,const string& need)
		{
			return find(a.begin() , a.end() , need ) != a.end();
		}

	};


	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	if (this->Engine != NULL)
	{
		delete this->Engine;
	}

	//合成音声
	const string name = MainWindow::m()->Config.Get("speak_type","");


	list<string> arr;
	if ( _inner::isuse(Speak_OpenJTalk::getSpeakModelList() , name) )
	{
		this->Engine = new Speak_OpenJTalk(name);
	}
	else if ( _inner::isuse(Speak_Festival::getSpeakModelList() , name) )
	{
		this->Engine = new Speak_Festival(name);
	}
#if _MSC_VER
//	else if ( _inner::isuse(Speak_SpeechPlatform::getSpeakModelList() , name) )
//	{
//		this->Engine = new Speak_SpeechPlatform(name);
//	}
#endif
	else if (name == "none")
	{
		this->Engine = new Speak_None();
	}
//	else if ( isuse(Speak_SpeechPlatform::Speak_AquesTalk() , name) )
//	{ //ライセンス料が高すぎまするぞ陛下・・・
//		this->Engine = new Speak_AquesTalk(name);
//	}
	else if (name == "")
	{//名無しの場合は、meiちゃんで.
		this->Engine = new Speak_OpenJTalk("mei_normal");
	}
	else
	{
		throw XLException("合成音声エンジン" + name + "がありません");
	}

	this->Engine->Create();

	return true;
}

void Speak_Factory::PushQueeOnly(const CallbackPP& callback,const string & str,bool noplay)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	if (str.empty())
	{
		return ;
	}

	this->Engine->PushQueeOnly(callback,str,noplay);
}

void Speak_Factory::FireQuee()
{
	this->Engine->FireQuee();
}


bool Speak_Factory::SpeakAsync(const CallbackPP& callback,const string & str,bool noplay)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	if (str.empty())
	{
		return false;
	}

	this->Engine->PushQueeOnly(callback,str,noplay);
	this->Engine->FireQuee();
	return true;
}

bool Speak_Factory::SpeakSync(const string & str,bool noplay)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	if (str.empty())
	{
		return false;
	}

	this->Engine->SpeakSync(str,noplay);
	return true;
}
void Speak_Factory::Cancel()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	this->Engine->Cancel();
}

//このコールバックに関連付けられているものをすべて消す
bool Speak_Factory::RemoveCallback(const CallbackPP& callback , bool is_unrefCallback)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	this->Engine->RemoveCallback(callback,is_unrefCallback);
	return true;
}

void Speak_Factory::TryDeleteSpeakCacheDB(const string& oldstr,const string& newstr) 
{
	if (oldstr.empty())
	{//昔の文字列がない場合は何もしない.
		return ;
	}
	if (oldstr == newstr)
	{//古いのと新しいのが同じなら何もしない
		return ;
	}
	this->DeleteSpeakCacheDB(oldstr);
}

void Speak_Factory::DeleteSpeakCacheDB(const string& str)
{
	if (str.empty())
	{//ない文字列は消さない.
		return ;
	}
	this->Engine->DeleteSpeakCacheDB(str);
}

void Speak_Factory::ClearSpeakCacheDB()
{
	this->Engine->ClearSpeakCacheDB();
}

list<string> Speak_Factory::getSpeakModelList() const
{
	list<string> arr;
//	arr =  XLStringUtil::merge(arr,Speak_AquesTalk::getSpeakModelList(),true);
//	arr =  XLStringUtil::merge(arr,Speak_SpeechPlatform::getSpeakModelList(),true);
//	arr =  XLStringUtil::merge(arr,Speak_GoogleTranslate::getSpeakModelList(),true);
	arr =  XLStringUtil::merge(arr,Speak_Festival::getSpeakModelList());
	arr =  XLStringUtil::merge(arr,Speak_OpenJTalk::getSpeakModelList());

	return arr;
}

#endif //WITH_CLIENT_ONLY_CODE==1
