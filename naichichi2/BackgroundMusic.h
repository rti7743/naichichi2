//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#pragma once


#ifdef _MSC_VER
#include "atlbase.h"
#include "atlwin.h"
#include "wmp.h"
#else
#endif

//裏側で音楽を再生する
class BackgroundMusic
{
public:
	BackgroundMusic();
	virtual ~BackgroundMusic();
	
	bool Create();
	bool Play(const string& command);
	void Stop();
	
	
private:
	void ThreadMain();

#ifdef _MSC_VER
	CComPtr<IWMPPlayer> Player;
	CComPtr<IWMPControls> Controls;
#else
	//後で音楽を再生するスレッド
	thread* Thread;

	mutex   Lock;
	condition_variable EventObject;
	bool           StopFlag;
#endif
	string RunCommand;
};

#endif //WITH_CLIENT_ONLY_CODE