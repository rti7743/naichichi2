#pragma once
#include "Callbackable.h"

//Web画面を実装するうえでの、合成音声関係の動作
class ScriptWebRunnerSpeak : public CallbackPP
{
public:
	ScriptWebRunnerSpeak();
	virtual ~ScriptWebRunnerSpeak();

	//組み込みlinuxだと合成音が遅いので、あらかじめわかっている分は読み込ませてキャッシュを作らせておく
	void ReloadSpeak(bool sync);

	void ClearCallback();
private:
	CallbackPP NewCallback(std::function<void (void) > f);

	list<CallbackPP> CallbackList;
};
