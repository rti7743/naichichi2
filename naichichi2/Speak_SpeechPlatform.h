// SAPISpeechRecognition.h: SAPISpeechRecognition クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAPISpeechRecognition_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_SAPISpeechRecognition_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlbase.h>

//Speech Platform SDK 入れてね http://www.microsoft.com/download/en/details.aspx?id=27226
#include <../../../Speech/v11.0/Include/sapi.h>
#include <../../../Speech/v11.0/Include/sphelper.h>

#include "SpeakCacheDB.h"


class Speak_SpeechPlatform : public ISpeechSpeakInterface
{
public:
	Speak_SpeechPlatform();
	virtual ~Speak_SpeechPlatform	();

	//音声のためのオブジェクトの構築.
	virtual bool Create();
	virtual void PushQueeOnly(const CallbackPP& callback,const string & str,bool noplay = false) = 0;
	virtual void FireQuee() = 0;
	virtual void SpeakSync(const string & str,bool noplay);
	virtual void Cancel();
	virtual void RemoveCallback(const CallbackPP& callback , bool is_unrefCallback) ;
	virtual void DeleteSpeakCacheDB(const string& str);
	virtual void ClearSpeakCacheDB();
	static list<string> getSpeakModelList() ;

private:
	bool Run();
	bool RegistVoiceBot(const string & botname);

	list<SpeakTask> SpeakQueue;

	CComPtr<ISpVoice>				Engine;

	thread* Thread;
	mutex   Lock;
	condition_variable EventObject;
	bool           StopFlag;
	bool		   CancelFlag;

	SpeakCacheDB	CacheDB;
};

#endif // !defined(AFX_SAPISpeechRecognition_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
