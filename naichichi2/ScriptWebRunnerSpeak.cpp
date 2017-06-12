//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include "ScriptWebRunnerSpeak.h"
#include "Fire.h"
#include "MainWindow.h"

ScriptWebRunnerSpeak::ScriptWebRunnerSpeak()
{
}
ScriptWebRunnerSpeak::~ScriptWebRunnerSpeak()
{
}
void ScriptWebRunnerSpeak::ClearCallback()
{
	for(auto it = this->CallbackList.begin() ; it != this->CallbackList.end() ; ++it)
	{
		MainWindow::m()->Speak.RemoveCallback(*it,false);
	}
	this->CallbackList.clear();
}

CallbackPP ScriptWebRunnerSpeak::NewCallback(std::function<void (void) > f)
{
	CallbackPP pp = CallbackPP::New(f);
	this->CallbackList.push_back(pp);

	return pp;
}


//組み込みlinuxだと合成音が遅いので、あらかじめわかっている分は読み込ませてキャッシュを作らせておく
void ScriptWebRunnerSpeak::ReloadSpeak(bool sync)
{
	MainWindow::m()->SyncInvoke([&](){
		for(auto it = this->CallbackList.begin() ; it != this->CallbackList.end() ; ++it)
		{
			MainWindow::m()->Speak.RemoveCallback(*it,false);
		}

		const auto configmap = MainWindow::m()->Config.ToMap();
		for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
		{
			if (! strstr( it->first.c_str() , "tospeak_string" ) )
			{
				continue;
			}
			//読み上げないモードでキャッシュを作らせます。
			if (sync)
			{
				MainWindow::m()->Speak.SpeakSync( it->second , true );
			}
			else
			{
				MainWindow::m()->Speak.PushQueeOnly( CallbackPP::NoCallback() ,it->second , true );
			}
		}

		//非同期の場合、キューに乗っているのを実行開始を指示する。
		if (!sync)
		{
			MainWindow::m()->Speak.FireQuee(  );
		}

	});
}


#endif //WITH_CLIENT_ONLY_CODE==1
