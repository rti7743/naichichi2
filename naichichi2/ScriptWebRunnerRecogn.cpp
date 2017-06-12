//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include "ScriptWebRunnerRecogn.h"
#include "Fire.h"
#include "MainWindow.h"


ScriptWebRunnerRecogn::ScriptWebRunnerRecogn()
{
}

ScriptWebRunnerRecogn::~ScriptWebRunnerRecogn()
{
	ClearCallback();
}

void ScriptWebRunnerRecogn::ClearCallback()
{
	for(auto it = this->CallbackList.begin() ; it != this->CallbackList.end() ; ++it)
	{
		MainWindow::m()->Recognition.RemoveCallback(*it,false);
	}
	this->CallbackList.clear();
}

CallbackPP ScriptWebRunnerRecogn::NewCallback(std::function<void (void) > f)
{
	CallbackPP pp = CallbackPP::New(f);
	this->CallbackList.push_back(pp);

	return pp;
}


//音声認識イベントのリセット
void ScriptWebRunnerRecogn::ReloadRecong(bool doAutoCommit)
{
	MainWindow::m()->SyncInvoke([&](){
		for(auto it = this->CallbackList.begin() ; it != this->CallbackList.end() ; ++it)
		{
			MainWindow::m()->Recognition.RemoveCallback(*it,false);
		}
	});

	const auto configmap = MainWindow::m()->Config.FindGetsToMap("elec_",false);
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		//音声認識項目をすべて登録する
		if ( ! strstr(it->first.c_str() , "actionvoice_command") )
		{
			continue;
		}
		int key1 = -1;
		int key2 = -1;
		int key3 = -1;
		sscanf(it->first.c_str() , "elec_%d_action_%d_actionvoice_command_%d",&key1,&key2,&key3);
		if (key1 < 0 || key2 < 0 || key3 < 0 )
		{//不明なキー
			continue;
		}
		if (it->second.empty())
		{//音声認識文字列が空です
			continue;
		}

		//機材prefix
		const string prefix = "elec_" + num2str(key1);

		if ( stringbool( mapfind(configmap,prefix + "_ignore_recogn","") ) )
		{//音声認識を無効にする機材
			DEBUGLOG("機材 " << mapfind(configmap,prefix + "_name","") << " (" << key1 << ") は音声認識を無効にするフラグが立っています。音声命令("<<it->second<<")は無視されます。" );
			continue;
		}


		CallbackPP callback = NewCallback( [=](){ this->recongFireCallbackFunction(key1,key2); }  );
		MainWindow::m()->SyncInvoke([=](){
			auto xr = MainWindow::m()->Recognition.AddCommandRegexp( callback , it->second,it->first);
			if(!xr)
			{
				ERRORLOG("音声認識項目" << it->first << ":" << it->second << "を登録できません" );
			}
		});
	}

	if (doAutoCommit)
	{
		DEBUGLOG("音声認識エンジンコミット開始" );
		MainWindow::m()->SyncInvoke([](){
			MainWindow::m()->Recognition.CommitRule();
		});
		DEBUGLOG("音声認識エンジンコミット終了" );
	}
}

bool ScriptWebRunnerRecogn::isrecongpauseMode(int key1,int key2) const
{
	if (!MainWindow::m()->IsRecongpause() )
	{
		//音声認識停止中ではありません.
		return false;
	}

	//音声認識停止中
	const auto configmap = MainWindow::m()->Config.FindGetsToMap("elec_",false);
	const string prefix = string("elec_") + num2str(key1) + "_action_" + num2str(key2);

	const string execflag = MainWindow::m()->Config.Get(prefix+"_execflag","");
	if (execflag == "recongpause")
	{
		//停止中を解除させる動作のようです. 実行を許可します.
		return false;
	}
	if(MainWindow::m()->IsWhenTakingStopRecogn())
	{//通話中だから静かにしている場合
		if (execflag == "sip")
		{
			const string sip_action_type = MainWindow::m()->Config.Get(prefix+"_sip_action_type","");
			if (sip_action_type == "hangup" || sip_action_type == "answer")
			{//電話をきると、電話を取るだけ許可.
				return true;
			}
		}
	}
	//音声認識停止中なのに、音声認識で動作が呼ばれたので停止させる.
	return true;
}

void ScriptWebRunnerRecogn::recongFireCallbackFunction(int key1,int key2)
{
	AutoLEDOn autoLEDOn;

	if ( isrecongpauseMode(key1,key2) )
	{
		NOTIFYLOG("現在音声認識停止中です。 elec:" << key1 << " action:"<< key2 << "が呼ばれましたが無視します.");

		//音を流す.
		const string recong_pause_mp3 = MainWindow::m()->Config.Get("recong_pause_mp3", "recong_pause_syun.mp3" );
		MainWindow::m()->MusicPlayAsync.Play(recong_pause_mp3,1);

		//停止がわかりづらいので、LEDを点灯させる.
		MainWindow::m()->CapeBord.SleepAndBrightLED(3,500);
		return ;
	}

	if ( MainWindow::m()->IsRecongpause() )
	{//現在音声認識停止中なのに、許可されたということは、音声認識停止解除コマンド
	 //ちゃりりんの音を鳴らす
		MainWindow::m()->Recognition.PlayRecongOKSoundForce();
	}

	Fire f;
	f.FireAction( key1 , key2);
}




void ScriptWebRunnerRecogn::run_recongpause(const string& recongpause_type)
{
	if (recongpause_type == "stop")
	{//一時停止
		NOTIFYLOG("音声認識を一時停止にします");
		MainWindow::m()->UpdateRecongpause(true);
	}
	else if (recongpause_type == "start")
	{//再開
		NOTIFYLOG("音声認識を再開します");
		MainWindow::m()->UpdateRecongpause(false);
	}
	else
	{
		NOTIFYLOG("音声認識一時停止のrecongpause_typeが不明("<<recongpause_type<<")です");
	}
	//わかりづらいので、LEDを点灯させる.
	MainWindow::m()->CapeBord.SleepAndBrightLED(5,200);
}


#endif //WITH_CLIENT_ONLY_CODE==1
