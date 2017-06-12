// SAPISpeechRecognition.h: SAPISpeechRecognition クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Speak_OpenJTalk_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_Speak_OpenJTalk_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
struct OpenJTalk;

#include "SpeakCacheDB.h"

class Speak_OpenJTalk : public ISpeechSpeakInterface
{
public:
	Speak_OpenJTalk(const string& VoiceType);
	virtual ~Speak_OpenJTalk	();

	//音声のためのオブジェクトの構築.
	virtual bool Create();
	virtual void PushQueeOnly(const CallbackPP& callback,const string & str,bool noplay = false);
	virtual void FireQuee();
	virtual void SpeakSync(const string & str,bool noplay);
	virtual void Cancel();
	virtual void RemoveCallback(const CallbackPP& callback , bool is_unrefCallback) ;
	virtual void DeleteSpeakCacheDB(const string& str);
	virtual void ClearSpeakCacheDB();
	static list<string> getSpeakModelList() ;

private:
	void Run();
	void PickupRun();
	bool CreateOpenJtalk(const string& voiceType,OpenJTalk** openJTalkHandle , vector<string>* options) const;

	list<SpeakTask> SpeakQueue;

	//openjtalkを再生するスレッド
	thread* Thread;
	OpenJTalk*     OpenJTalkHandle;
	vector<string> Options;

	mutex   Lock;
	condition_variable EventObject;
	bool           StopFlag;
	bool		   CancelFlag;

	string VoiceType;

	SpeakCacheDB			CacheDB;
};

#endif // !defined(AFX_Speak_OpenJTalk_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
