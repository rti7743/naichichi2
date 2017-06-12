#pragma once
struct SpeakTask
{
	SpeakTask()
	{
	}
	SpeakTask(const CallbackPP& callback,const string & str,bool noplay) : text(str) , callback(callback) , noplay(noplay)
	{
	}

	string text;
	CallbackPP	callback;
	bool noplay;
};

struct ISpeechSpeakInterface
{
	ISpeechSpeakInterface(){}
	virtual ~ISpeechSpeakInterface(){}

	//音声のためのオブジェクトの構築.
	virtual bool Create() = 0;
	virtual void PushQueeOnly(const CallbackPP& callback,const string & str,bool noplay = false) = 0;
	virtual void FireQuee() = 0;
	virtual void SpeakSync(const string & str,bool noplay) = 0;
	virtual void Cancel() = 0;
	virtual void RemoveCallback(const CallbackPP& callback , bool is_unrefCallback) = 0;

	virtual void DeleteSpeakCacheDB(const string& str) = 0;
	virtual void ClearSpeakCacheDB() = 0;
};

class Speak_Factory
{
public:
	Speak_Factory();
	virtual ~Speak_Factory();

	bool Create();
	void PushQueeOnly(const CallbackPP& callback,const string & str,bool noplay = false);
	void FireQuee();
	bool SpeakAsync(const CallbackPP& callback,const string & str,bool noplay = false);
	bool SpeakSync(const string & str,bool noplay);
	void Cancel();
	bool RemoveCallback(const CallbackPP& callback , bool is_unrefCallback);

	void TryDeleteSpeakCacheDB(const string& oldstr,const string& newstr) ;
	void DeleteSpeakCacheDB(const string& str) ;
	void ClearSpeakCacheDB() ;
	list<string> getSpeakModelList() const;
private:

	ISpeechSpeakInterface* Engine;
};
