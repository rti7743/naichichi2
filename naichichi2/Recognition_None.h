#pragma once;
#include "common.h"

class Recognition_None : public ISpeechRecognitionInterface
{
public:
	Recognition_None(){}
	virtual ~Recognition_None(){}

	virtual bool Create() { return true; }
	//呼びかけを設定します。
	//設定したあと、 CommitRule() てしてね。
	virtual bool SetYobikake(const list<string> & yobikakeList) { return true; }
	//構築したルールを音声認識エンジンにコミットします。
	virtual bool CommitRule(const list<RecongTask>* allCommandRecongTask)  { return true; }
	//このコールバックに関連付けられているものをすべて消す
	virtual bool RemoveCallback(const CallbackPP& callback , bool is_unrefCallback)  { return true; }
	//音声認識が利用できる音素を作成する
	virtual string ConvertYomi(const string& word) const { return word; }
	//音声認識が利用できる音素を作成する(スペースを入れない)
	virtual string ConvertNoSpaceYomi(const string& word) const { return word; }
	//音声認識が利用できる音素を作成する
	virtual bool CheckYomi(const string& word) const { return true; }
	//waveファイルデータから直接音声認識させます。
	virtual bool RedirectWave(const vector<char>& wavedata) { return false; }
};
