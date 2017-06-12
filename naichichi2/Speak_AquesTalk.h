// SAPISpeechRecognition.h: SAPISpeechRecognition クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Speak_AquesTalk_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_Speak_AquesTalk_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XLLoadLibraryHelper.h"
#include "SpeakCacheDB.h"

class Speak_AquesTalk : public ISpeechSpeakInterface
{
public:
	Speak_AquesTalk(const string& botname);
	virtual ~Speak_AquesTalk();

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

	list<SpeakTask> SpeakQueue;
	
	vector<char> PhontDat; //aques talk2 の場合音声データを選べる。　emptyならディフォルト音声

	string BotName;

	thread* Thread;
	mutex   Lock;
	condition_variable EventObject;
	bool           StopFlag;
	bool			CancelFlag;

	//AquesTalkを何とかして読み込む
	bool LoadAquesTalk();
	//ぼっとを選択する.
	void SelectBot();
	//バージョンを取得.
	int getVersion() const;

	XLLoadLibraryHelper Lib;

	//for aques talk2
	typedef unsigned char *  ( __stdcall *AquesTalk2_SyntheDef)(const char *koe, int iSpeed, int *pSize, void *phontDat);
	typedef void  ( __stdcall *AquesTalk2_FreeDef)(unsigned char *wav);
	AquesTalk2_SyntheDef	AquesTalk2_Synthe;
	AquesTalk2_FreeDef		AquesTalk2_Free;

	//for aques talk1
	typedef unsigned char *  ( __stdcall *AquesTalk_SyntheDef)(const char *koe, int iSpeed, int *pSize);
	typedef void  ( __stdcall *AquesTalk_FreeWaveDef)(unsigned char *wav);
	AquesTalk_SyntheDef		AquesTalk_Synthe;
	AquesTalk_FreeWaveDef	AquesTalk_FreeWave;

	SpeakCacheDB			CacheDB;
};

#endif // !defined(AFX_Speak_AquesTalk_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
