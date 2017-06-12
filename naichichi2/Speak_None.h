#pragma once;
#include "common.h"

class Speak_None : public ISpeechSpeakInterface
{
public:
	Speak_None(){}
	virtual ~Speak_None(){}

	//音声のためのオブジェクトの構築.
	virtual bool Create()  { return true; }
	virtual void PushQueeOnly(const CallbackPP& callback,const string & str,bool noplay = false) { }
	virtual void FireQuee() { } 
	virtual void SpeakSync(const string & str,bool noplay) {  }
	virtual void Cancel() { }
	virtual void RemoveCallback(const CallbackPP& callback , bool is_unrefCallback) {  }
	virtual void DeleteSpeakCacheDB(const string& str) { }
	virtual void ClearSpeakCacheDB() { }
};
