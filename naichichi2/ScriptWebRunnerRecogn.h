#pragma once
#include "Callbackable.h"

//Web画面を実装するうえでの、音声認識関係の動作
class ScriptWebRunnerRecogn : public CallbackPP
{
public:
	ScriptWebRunnerRecogn();
	virtual ~ScriptWebRunnerRecogn();

	//音声認識イベントのリセット
	void ReloadRecong(bool doAutoCommit);

	void ClearCallback();
private:
	CallbackPP NewCallback(std::function<void (void) > f);

	bool isrecongpauseMode(int key1,int key2) const;
	void recongFireCallbackFunction(int key1,int key2);
	void run_recongpause(const string& recongpause_type);
	
	
	list<CallbackPP> CallbackList;
};
