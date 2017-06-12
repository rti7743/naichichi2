//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include "ScriptWebRunnerTrigger.h"
#include "Fire.h"
#include "MainWindow.h"
#include "XLHttpSocket.h"
#include "ScriptRemoconWeb.h"
#include "ActionImplement.h"

ScriptWebRunnerTrigger::ScriptWebRunnerTrigger()
{
}
ScriptWebRunnerTrigger::~ScriptWebRunnerTrigger()
{
	ClearCallback();
}

void ScriptWebRunnerTrigger::ClearCallback()
{
	for(auto it = this->CallbackList.begin() ; it != this->CallbackList.end() ; ++it)
	{
		MainWindow::m()->Trigger.RemoveCallbackDelay(*it);
	}
	this->CallbackList.clear();
}

CallbackPP ScriptWebRunnerTrigger::NewCallback(std::function<void (void) > f)
{
	CallbackPP pp = CallbackPP::New(f);
	this->CallbackList.push_back(pp);

	return pp;
}

//トリガーの再設定
void ScriptWebRunnerTrigger::ReloadTrigger(const time_t& now)
{
	//現在のコールバックを停止する.
	for(auto it = this->CallbackList.begin() ; it != this->CallbackList.end() ; ++it)
	{
		MainWindow::m()->Trigger.RemoveCallbackDelay(*it);
	}

	//トリガーの登録
	const auto triggermap = MainWindow::m()->Config.FindGetsToMap("trigger_",false);
	for(auto it = triggermap.begin() ; it != triggermap.end() ; ++it )
	{
		if (! (strstr(it->first.c_str(),"_enable") && !strstr(it->first.c_str(),"_if_") && !strstr(it->first.c_str(),"_then_")) )
		{
			continue;
		}
		int key = 0;
		sscanf(it->first.c_str(),"trigger_%d_name",&key);
		if (key <= 0)
		{
			continue;
		}
		string prefix = "trigger_" + num2str(key) ;
		bool enable = MainWindow::m()->Config.GetBool(prefix + "_enable",false);
		if (!enable)
		{//無効なイベント
			continue;
		}
		const string type = MainWindow::m()->Config.Get(prefix + "_if_type","");
		if (type == "date")
		{
			const int hour = MainWindow::m()->Config.GetInt(prefix + "_if_date_hour",0);
			const int minute = MainWindow::m()->Config.GetInt(prefix + "_if_date_minute",0);
			const int if_date_loop_hourly = MainWindow::m()->Config.GetInt(prefix + "_if_date_loop_hourly",0);

			const time_t fireTime = convertTriggerDate(now,hour,minute,if_date_loop_hourly);
			NOTIFYLOG("時間トリガー" << key << "は、次は" << XLStringUtil::timetostr(fireTime) << "に動きます." );
			
			CallbackPP callback = NewCallback( [=](){ this->triggerFireTimeCallbackFunction(key,time(NULL) ); }  );
			MainWindow::m()->Trigger.AppendNextTimeTrigger(callback,fireTime  );
		}
		else if (type == "sensor")
		{
			const string sensorType = MainWindow::m()->Config.Get(prefix + "_if_sensor_type","");
			const float sensorValue = MainWindow::m()->Config.GetDouble(prefix + "_if_sensor_value",0.0f);
			const string sensorExpr = MainWindow::m()->Config.Get(prefix + "_if_sensor_expr","");

			auto sensorTypeEnum = Trigger::convertStringToSensorType(sensorType);
			auto sensorExprEnum = Trigger::convertStringToExpr(sensorExpr);

			if (sensorTypeEnum == Trigger::SensorType_None)
			{
				NOTIFYLOG("センサートリガー" << key << "は、種類(" << sensorType << ") が不明です" );
				continue;
			}
			if (sensorExprEnum == Trigger::Expr_None)
			{
				NOTIFYLOG("センサートリガー" << key << "は、条件(" << sensorExpr << ") が不明です" );
				continue;
			}
			NOTIFYLOG("センサートリガー" << key << "は、種類(" << sensorType << ") 値(" << sensorValue << ") 条件(" << sensorExpr << ")で動きます" );

			CallbackPP callback = NewCallback( [=](){ this->triggerFireSensorCallbackFunction(key,time(NULL) ); }  );
			MainWindow::m()->Trigger.AppendSensorCallbackableTrigger(callback, sensorTypeEnum, sensorValue , sensorExprEnum );
		}
		else if (type == "command")
		{
			const int pooingMinute = MainWindow::m()->Config.GetInt(prefix + "_if_command_pooling",false);
			unsigned int nextLoop = MAX(pooingMinute * 60 , 60);
			NOTIFYLOG("コマンドトリガー" << key << "は、" << nextLoop << "秒ごとに動きます" );

			CallbackPP callback = NewCallback( [=](){ this->triggerFireCommandCallbackFunction(key,time(NULL) ); }  );
			MainWindow::m()->Trigger.AppendNextSecondTrigger(callback, nextLoop, true );
		}
		else if (type == "web")
		{
			const int pooingMinute = MainWindow::m()->Config.GetInt(prefix + "_if_web_pooling",false);
			unsigned int nextLoop = MAX(pooingMinute * 60 , 60);
			NOTIFYLOG("コマンドトリガー" << key << "は、" << nextLoop << "秒ごとに動きます" );

			CallbackPP callback = NewCallback( [=](){ this->triggerFireWebCallbackFunction(key,time(NULL) ); }  );
			MainWindow::m()->Trigger.AppendNextSecondTrigger(callback,nextLoop , true );
		}
	}

	//おや○みタイマーの登録
	const auto configmap = MainWindow::m()->Config.FindGetsToMap("elec_",false);
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		//音声認識項目をすべて登録する
		if ( ! strstr(it->first.c_str() , "_timer_firetime") )
		{
			continue;
		}
		int key1 = -1;
		sscanf(it->first.c_str() , "elec_%d_timer_firetime",&key1);
		if (key1 < 0)
		{
			continue;
		}
		if (it->second.empty())
		{
			continue;
		}
		time_t firetime = ((time_t)atou( it->second.c_str() ))*60;
		if (firetime <= 0)
		{
			continue;
		}

		//firekeyの確認
		const int key2 = MainWindow::m()->Config.GetInt("elec_" + num2str(key1) +  "_timer_firekey",-1);
		if (key2 < 0)
		{
			continue;
		}
		NOTIFYLOG("家電タイマー" << key1 << " 実行キー" << key2 << "は、次は" << XLStringUtil::timetostr(firetime) << "に動きます." );

		CallbackPP callback = NewCallback( [=](){ this->triggerFireElecTimerCallbackFunction(key1,key2,time(NULL) ); }  );
		MainWindow::m()->Trigger.AppendNextTimeTrigger(callback,firetime  );
	}
}

void ScriptWebRunnerTrigger::triggerFireTimeCallbackFunction(int triggerkey,const time_t& now)
{
	const string prefix = "trigger_" + num2str(triggerkey) ;
	const bool loop_use = MainWindow::m()->Config.GetBool(prefix + "_if_date_loop_use",false);
	if (loop_use)
	{//繰り返し実行するなら、曜日を見る
		const int loop_dayofweek = MainWindow::m()->Config.GetInt(prefix + "_if_date_loop_dayofweek",0);
		const int loop_holiday = MainWindow::m()->Config.GetInt(prefix + "_if_date_loop_holiday",0);

		const string loop_holiday_dataof = MainWindow::m()->Config.Get(prefix + "_if_date_loop_holiday_dataof","");
		if ( ! convertTriggerDateWeekDay( now,loop_dayofweek,loop_holiday,loop_holiday_dataof) )
		{//今は動くべき曜日ではない
			NOTIFYLOG("トリガー" << triggerkey << "は、今の曜日には動きません 現在:" << XLStringUtil::timetostr(now) );

			//次の実行を仕込む
			const int hour = MainWindow::m()->Config.GetInt(prefix + "_if_date_hour",0);
			const int minute = MainWindow::m()->Config.GetInt(prefix + "_if_date_minute",0);
			const int if_date_loop_hourly = MainWindow::m()->Config.GetInt(prefix + "_if_date_loop_hourly",0);

			const time_t fireTime = convertTriggerDate(now,hour,minute,if_date_loop_hourly);
			NOTIFYLOG("トリガー" << triggerkey << "は、次は " << XLStringUtil::timetostr(fireTime) << " に起動します。" );

			//次の呼び出しを仕込む
			CallbackPP callback = NewCallback( [=](){ this->triggerFireTimeCallbackFunction(triggerkey,time(NULL) ); }  );
			MainWindow::m()->Trigger.AppendNextTimeTrigger(callback,fireTime);

			return ;
		}
	}

	if (!triggerCheckExIf(prefix,now))
	{//無効なイベント
		NOTIFYLOG("トリガー" << triggerkey << "は、現在動作する設定ではありません" );
	}
	else
	{
		//trigger実行
		triggerFire(triggerkey,now);
	}
	
	//次のトリガーの仕込み
	if (!loop_use)
	{//定期実行しないならそこでおしまい
		MainWindow::m()->Config.Set(prefix + "_enable","0");
		NOTIFYLOG("トリガー" << triggerkey << "は、連続して動作しませんので、無効にします。" );
		return ;
	}
	{
		//定期実行するならば次の仕込みをする
		const int hour = MainWindow::m()->Config.GetInt(prefix + "_if_date_hour",0);
		const int minute = MainWindow::m()->Config.GetInt(prefix + "_if_date_minute",0);
		const int if_date_loop_hourly = MainWindow::m()->Config.GetInt(prefix + "_if_date_loop_hourly",0);

		const time_t fireTime = convertTriggerDate(now,hour,minute,if_date_loop_hourly);

		//次の呼び出しを仕込む
		CallbackPP callback = NewCallback( [=](){ this->triggerFireTimeCallbackFunction(triggerkey,time(NULL) ); }  );
		MainWindow::m()->Trigger.AppendNextTimeTrigger(callback,fireTime);

		NOTIFYLOG("トリガー" << triggerkey << "は、次は " << XLStringUtil::timetostr(fireTime) << " に起動します。" );
	}
}

//トリガー/センサー
void ScriptWebRunnerTrigger::triggerFireSensorCallbackFunction(int triggerkey,const time_t& now)
{
	const string prefix = "trigger_" + num2str(triggerkey) ;
	if (!triggerCheckExIf(prefix,now))
	{//無効なイベント
		NOTIFYLOG("トリガー" << triggerkey << "は、現在動作する設定ではありません" );
		return;
	}

	//trigger実行
	triggerFire(triggerkey,now);
}

//トリガー/コマンド
void ScriptWebRunnerTrigger::triggerFireCommandCallbackFunction(int triggerkey,const time_t& now)
{
	const string prefix = "trigger_" + num2str(triggerkey) ;
	if (!triggerCheckExIf(prefix,now))
	{//無効なイベント
		NOTIFYLOG("トリガー" << triggerkey << "は、現在動作する設定ではありません" );
		return;
	}

	//コマンドを調べる
	const string name = MainWindow::m()->Config.Get(prefix + "_if_command","");
	const string args1 = MainWindow::m()->Config.Get(prefix + "_if_command_select_args1","");
	const string args2 = MainWindow::m()->Config.Get(prefix + "_if_command_select_args2","");
	const string args3 = MainWindow::m()->Config.Get(prefix + "_if_command_select_args3","");
	const string args4 = MainWindow::m()->Config.Get(prefix + "_if_command_select_args4","");
	const string args5 = MainWindow::m()->Config.Get(prefix + "_if_command_select_args5","");
	string stdout_;
	Fire f;
	const string result = f.run_commandE(name,args1,args2,args3,args4,args5,&stdout_);
	NOTIFYLOG("トリガー" << triggerkey << "の定期実行コマンドを実行しました。結果「" << result << "」" );

	if (! (stringbool(result) || atoi( result ) ) )
	{//NG
		NOTIFYLOG("トリガー" << triggerkey << "の定期実行コマンドを実行しました。" << "結果「" << result << "」だと実行しません" );
		return ;
	}

	//trigger実行
	triggerFire(triggerkey,now);
}

//トリガー/web
void ScriptWebRunnerTrigger::triggerFireWebCallbackFunction(int triggerkey,const time_t& now)
{
	const string prefix = "trigger_" + num2str(triggerkey) ;
	if (!triggerCheckExIf(prefix,now))
	{//無効なイベント
		NOTIFYLOG("トリガー" << triggerkey << "は、現在動作する設定ではありません" );
		return;
	}

	//コマンドを調べる
	const string url = MainWindow::m()->Config.Get(prefix + "_if_web_url","");
	const string str = MainWindow::m()->Config.Get(prefix + "_if_web_string","");
	if (str.empty() || url.empty() )
	{
		DEBUGLOG("トリガー" << triggerkey << "の定期webチェックは、URLまたは文字列が空なので実行出来ません URL(" << url << ") str(" << str << ")" );
		return ;
	}

	const string html = XLStringUtil::mb_convert_utf8( XLHttpSocket::Get(url , 2 ) );

	if (str[0] == '/' && str[str.size() - 1 ] == '/' )
	{
		if (! XLStringUtil::regex_check( html, 	str.substr(1,str.size() - 2) ) )
		{
			DEBUGLOG("トリガー" << triggerkey << "の定期webチェックを実行しました。URL(" << url << ") 正規表現 str(" << str << ")" << " 結果:マッチしませんでした" );
			return ;
		}
		DEBUGLOG("トリガー" << triggerkey << "の定期webチェックを実行しました。URL(" << url << ") 正規表現 str(" << str << ")" << " 結果:マッチしました" );
	}
	else
	{
		if ( XLStringUtil::stristr(html , str) == NULL )
		{
			DEBUGLOG("トリガー" << triggerkey << "の定期webチェックを実行しました。URL(" << url << ") 文字列 str(" << str << ")" << " 結果:マッチしませんでした" );
			return ;
		}
		DEBUGLOG("トリガー" << triggerkey << "の定期webチェックを実行しました。URL(" << url << ") 文字列 str(" << str << ")" << " 結果:マッチしました" );
	}

	//trigger実行
	triggerFire(triggerkey,now);
}


//トリガーの追加条件を検証する
bool ScriptWebRunnerTrigger::triggerCheckExIf(const string& prefix,const time_t& now)
{
	bool enable = MainWindow::m()->Config.GetBool(prefix + "_enable",false);
	if (!enable)
	{//無効なイベント
		DEBUGLOG("トリガー(" << prefix << ")は無効です");
		return false;
	}

	//家電を条件にするか？
	bool elec_use = MainWindow::m()->Config.GetBool(prefix + "_if_elec_use",false);
	if (elec_use)
	{
		const string elec_type   = MainWindow::m()->Config.Get(prefix + "_if_elec_type","");
		const string elec_action = MainWindow::m()->Config.Get(prefix + "_if_elec_action","");
		const string elec_expr   = MainWindow::m()->Config.Get(prefix + "_if_elec_expr","");
		const string now_status = ScriptRemoconWeb::getElecStatus(elec_type);

		DEBUGLOG("トリガー(" << prefix << ")は家電を条件に使います 家電(" << elec_type << ")がステータス(" << elec_action << ")に("<<elec_expr<<")であるが、今は (" << now_status << ")です");
		if ( elec_expr == "equal" )
		{
			if(elec_action != now_status)
			{
				DEBUGLOG("トリガー(" << prefix << ")で、家電(" << elec_type <<")のステータス(" << elec_action << ")になっていることが期待されますが、今は (" << now_status << ")です");
				return false;
			}
		}
		else
		{
			if(elec_action == now_status)
			{
				DEBUGLOG("トリガー(" << prefix << ")で、家電(" << elec_type <<")のステータス(" << elec_action << ")になっていないことが期待されますが、今は (" << now_status << ")です");
				return false;
			}
		}
	}
	//前回発動からN分離れていること
	bool stooper_use = MainWindow::m()->Config.GetBool(prefix + "_if_stopper_use",false);
	if (stooper_use)
	{
		const string lastFireTime     = MainWindow::m()->Config.Get(prefix + "_lastfire","");
		const string stopper_minute   = MainWindow::m()->Config.Get(prefix + "_if_stopper_minute","");
		time_t value = now + MIN(atoi(stopper_minute)*60,60);
		time_t lastfire = atou(lastFireTime);
		DEBUGLOG("トリガー(" << prefix << ")は次回からN(" << stopper_minute<< ")分たっている必要がります 前回(" << XLStringUtil::timetostr( lastfire ) << ")動作していて、今回は(" << XLStringUtil::timetostr( value ) << ")でないとダメです");

		if ( lastfire > value )
		{//まだ発動したらダメ
			DEBUGLOG("トリガー(" << prefix << ")は、前回(" << XLStringUtil::timetostr( lastfire ) << ")に発動しているので、次の発動は(" << XLStringUtil::timetostr( value ) << ")です。まだです。");
			return false;
		}
	}

	//近くに人がいる
	bool sound_use = MainWindow::m()->Config.GetBool(prefix + "_if_sound_use",false);
	if (sound_use)
	{
		const unsigned int trigger_if_sound_value = MainWindow::m()->Config.GetInt(prefix + "_if_sound_value",2);
		const string trigger_if_sound_expr = MainWindow::m()->Config.Get(prefix + "_if_sound_expr","equal");
		const float sensor_sound = MainWindow::m()->Config.GetDouble("sensor_sound_value",0.0f);
		DEBUGLOG("トリガー(" << prefix << ")は、近くに人がいるセンサー(" << sensor_sound << ")が、基準値(" << trigger_if_sound_value << ")で "<<trigger_if_sound_expr<<" ある必要があります。");

		if ( trigger_if_sound_expr == "equal" )
		{
			if (sensor_sound < trigger_if_sound_value)
			{
				DEBUGLOG("トリガー(" << prefix << ")は、近くに人がいるセンサー(" << sensor_sound << ")が、基準値(" << trigger_if_sound_value << ")以下なので起動しません。");
				return false;
			}
		}
		else
		{
			if (sensor_sound > trigger_if_sound_value)
			{
				DEBUGLOG("トリガー(" << prefix << ")は、近くに人がいるセンサー(" << sensor_sound << ")が、基準値(" << trigger_if_sound_value << ")以上なので起動しません。");
				return false;
			}
		}


	}

	//天気
	bool weather_use = MainWindow::m()->Config.GetBool(prefix + "_if_weather_use",false);
	if (weather_use)
	{
		const string weather_script_command = MainWindow::m()->Config.Get("weather_script_command","weather_openweather.js");
		const string trigger_if_weather_address = MainWindow::m()->Config.Get(prefix + "_if_weather_address","");
		const string trigger_if_weather_value = MainWindow::m()->Config.Get(prefix + "_if_weather_value","");
		
		string stdout_;
		map<string,string> retmap;
		const string weatherResult = run_command_resultjson(weather_script_command,trigger_if_weather_address,"","","","",&stdout_,&retmap);

		const string weather = retmap["weather"];

		DEBUGLOG("トリガー(" << prefix << ")は、天気(" << trigger_if_weather_value << ")が 値(" << weather << ")であるかチェックします。コマンド戻り値("<<weatherResult<<")  stdout:("<< stdout_ <<")");
		if (trigger_if_weather_value == "rain")
		{
			if ( (weather == "rain" || weather == "snow") )
			{
			}
			else
			{
				DEBUGLOG("トリガー(" << prefix << ")は、天気(" << trigger_if_weather_value << ")が 値(" << weather << ")ではありませんでした");
				return false;
			}
		}
		else if (trigger_if_weather_value == "not_rain")
		{
			if (! (weather == "rain" || weather == "snow") )
			{
			}
			else
			{
				DEBUGLOG("トリガー(" << prefix << ")は、天気(" << trigger_if_weather_value << ")が 値(" << weather << ")ではありませんでした");
				return false;
			}
		}
		else if (trigger_if_weather_value == "soon_rain" || trigger_if_weather_value == "soon_clear")
		{
			const string next = retmap["next"];
			if (trigger_if_weather_value == "soon_rain")
			{
				if ( trigger_if_weather_IsRain(weather) || trigger_if_weather_IsRain(next) )
				{
				}
				else
				{
					DEBUGLOG("トリガー(" << prefix << ")は、天気(" << trigger_if_weather_value << ") 、値(" << next << ")ではありません");
					return false;
				}
			}
			else if (trigger_if_weather_value == "soon_clear")
			{
				if (! ( trigger_if_weather_IsRain(weather) || trigger_if_weather_IsRain(next) ) )
				{
				}
				else
				{
					DEBUGLOG("トリガー(" << prefix << ")は、天気(" << trigger_if_weather_value << ") 、値(" << next << ")ではありません");
					return false;
				}
			}
		}
	}

	DEBUGLOG("トリガー(" << prefix << ")追加発動条件はOKです。発動出来ます。");
	return true;
}

string ScriptWebRunnerTrigger::run_command_resultjson(const string& execcommand,const string& execargs1,const string& execargs2,const string& execargs3,const string& execargs4,const string& execargs5,string* appendSTDOUT,map<string,string>* resultmap)
{
	if ( ! XLStringUtil::checkSafePath(execcommand) )
	{
		throw XLException("ファイル名がセキュアではありません");
	}
	NOTIFYLOG("コマンド " << execcommand << "(" << execargs1 << ","<< execargs2 << ","<< execargs3 << ","<< execargs4 << ","<< execargs5 << ")" );

	
	const auto fullpath = MainWindow::m()->GetConfigBasePath("./webroot/user/script_command/" + execcommand);

	list<string> args;
	if (! (execargs1.empty() && execargs2.empty() && execargs3.empty() && execargs4.empty() && execargs5.empty())  )
	{
		args.push_back(execargs1);
		if (! (execargs2.empty() && execargs3.empty() && execargs4.empty() && execargs5.empty())  )
		{
			args.push_back(execargs2);
			if (! (execargs3.empty() && execargs4.empty() && execargs5.empty())  )
			{
				args.push_back(execargs3);
				if (! (execargs4.empty() && execargs5.empty())  )
				{
					args.push_back(execargs4);
					if (! execargs5.empty()  )
					{
						args.push_back(execargs5);
					}
				}
			}
		}
	}

	string result;
	string exceptionMessage;
	MainWindow::m()->SyncInvoke([&](){
		try
		{
			ScriptRunner runner( false );
			runner.LoadScript(fullpath);
			result = runner.callFunctionResultJson("main",args,resultmap);
			*appendSTDOUT += runner.getSTDOUT();
		}
		catch(XLException&e)
		{
			exceptionMessage = e.what();
			ERRORLOG(exceptionMessage );
		}
	});

	if (!exceptionMessage.empty())
	{
		throw XLException("実行に失敗しました。" + exceptionMessage );
	}
	return result;
}


void ScriptWebRunnerTrigger::triggerFire(int triggerkey,const time_t& now)
{
	const string prefix = "trigger_" + num2str(triggerkey) ;
	//実行した時刻を記録
	MainWindow::m()->Config.Set(prefix + "_lastfire",num2str(now) );
	DEBUGLOG("トリガー " << triggerkey << " を実行します");

	//読み上げ処理
	const string tospeak_select = MainWindow::m()->Config.Get(prefix + "_then_tospeak_select","");
	if (tospeak_select == "string")
	{
		const string tospeak_string = MainWindow::m()->Config.Get(prefix + "_then_tospeak_string","");
		MainWindow::m()->SyncInvoke( [tospeak_string](){
			MainWindow::m()->Speak.SpeakAsync(CallbackPP::NoCallback() ,tospeak_string,false);
		});
	}
	else if (tospeak_select == "musicfile")
	{
		const string tospeak_mp3 = MainWindow::m()->Config.Get(prefix + "_then_tospeak_mp3","");

		MainWindow::m()->MusicPlayAsync.Play(CallbackPP::NoCallback() ,tospeak_mp3,1);
	}
	
	if (  MainWindow::m()->Config.GetBool(prefix + "_then_macro_use",false) )
	{
		const auto configmap = MainWindow::m()->Config.FindGetsToMap(prefix + "_then_macro_",false);
		Fire f;
		f.run_macro(configmap,prefix + "_then_macro_");
	}
	if (  MainWindow::m()->Config.GetBool(prefix + "_then_command_use",false) )
	{
		const string name = MainWindow::m()->Config.Get(prefix + "_then_command","");
		const string args1 = MainWindow::m()->Config.Get(prefix + "_then_command_select_args1","");
		const string args2 = MainWindow::m()->Config.Get(prefix + "_then_command_select_args2","");
		const string args3 = MainWindow::m()->Config.Get(prefix + "_then_command_select_args3","");
		const string args4 = MainWindow::m()->Config.Get(prefix + "_then_command_select_args4","");
		const string args5 = MainWindow::m()->Config.Get(prefix + "_then_command_select_args5","");

		string stdout_;
		Fire f;
		f.run_command(name,args1,args2,args3,args4,args5,&stdout_);
	}

}


//トリガーの次の曜日
time_t ScriptWebRunnerTrigger::convertTriggerDate(const time_t& now,int hour,int minute,int if_date_loop_hourly) const
{
#if _MSC_VER
	struct tm *date = localtime(&now);
#else
	struct tm datetmp;
	struct tm *date = &datetmp;
	localtime_r(&now,&datetmp);
#endif
	//分・秒とかをタイマーの日付にする
	if (if_date_loop_hourly <= 0)
	{//毎時ではなくて当時
		date->tm_hour = hour;
	}
	else
	{//毎時実行
		int hour = 0;
		for(int i = 0 ; i < 24 ; i += if_date_loop_hourly)
		{
			if ((i == date->tm_hour && minute > date->tm_min) || i > date->tm_hour) 
			{
				hour = i;
				break;
			}
		}
		date->tm_hour = hour;
	}
	date->tm_min = minute;
	date->tm_sec = 0;
	time_t plainTime = mktime(date);
	
	//本日か？
	bool isToday = now < plainTime;

	if (!isToday)
	{//もう過ぎているなら翌日に
		return plainTime + (24 * 60 * 60);
	}
	return plainTime;
}

//トリガーの曜日
bool ScriptWebRunnerTrigger::convertTriggerDateWeekDay(const time_t& now,int dayofweek,int holiday,const string& holiday_dataof) const
{
#if _MSC_VER
	struct tm *date = localtime(&now);
#else
	struct tm datetmp;
	struct tm *date = &datetmp;
	localtime_r(&now,&datetmp);
#endif

	if (holiday <= 1)
	{//祝日関係なく
	}
	else 
	{
		bool is_holiday = XLStringUtil::is_japan_holiday(now);
		if (holiday == 2)
		{//祝日を除く
			if (is_holiday)
			{
				DEBUGLOG("祝日を除く日に動作するオプションで、今日は祝日です");
				return false;
			}
		}
		else if (holiday == 3)
		{//祝日のみ
			if (!is_holiday)
			{
				DEBUGLOG("祝日のみ動作するオプションで、今は祝日ではありません");
				return false;
			}
		}
	}

	switch (date->tm_wday)
	{
	case 0:	//日
		return (bool)(dayofweek & 1);
	case 1:	//月
		return (bool)(dayofweek & 2);
	case 2:	//火
		return (bool)(dayofweek & 4);
	case 3:	//水
		return (bool)(dayofweek & 8);
	case 4:	//木
		return (bool)(dayofweek & 16);
	case 5:	//金
		return (bool)(dayofweek & 32);
	case 6:	//土
		return (bool)(dayofweek & 64);
	}
	return false;
}


//家電タイマーの実行(おや○みタイマー)
void ScriptWebRunnerTrigger::triggerFireElecTimerCallbackFunction(unsigned int key1,unsigned int key2,const time_t& now)
{
	const string elecPrefix       = string("elec_") + num2str(key1) ;
	const string elecActionPrefix = string("elec_") + num2str(key1) + "_action_" + num2str(key2);

	const string elec      = MainWindow::m()->Config.Get(elecActionPrefix + "_timer_exec_elec","none");
	const string action    = MainWindow::m()->Config.Get(elecActionPrefix + "_timer_exec_action","none");
	const int checktime = MainWindow::m()->Config.GetInt(elecPrefix + "_timer_firetime", 0 );
	if (checktime <= 0)
	{//もう既に動作しているか、途中で停止されて行き違いが有るらしい
		NOTIFYLOG("家電タイマー(" << elec << " " << action << " " << elecActionPrefix << ")は既に実行されているか、キャンセルされています。実行しません");
		return ;
	}
	NOTIFYLOG("家電タイマー(" << elec << " " << action << " " << elecActionPrefix << ")の時刻になりました。");
	//もう動作するから、二重に動かないように停止させる.
	MainWindow::m()->Config.Remove(elecPrefix + "_timer_firetime");
	MainWindow::m()->Config.Remove(elecPrefix + "_timer_firekey");

	try
	{
		//読み上げ
		ActionImplement::ToSpeak(elecActionPrefix + "_timer");

		//実行
		if (elec == "none" || action == "none")
		{
			NOTIFYLOG("家電タイマー(" << elec << " " << action << " " << elecActionPrefix << ")の実行項目がありません。");
			return;
		}

		//すでにその状態なら命令を送らない
		const bool timer_no_status_overraide = MainWindow::m()->Config.GetBool( elecActionPrefix + "_timer_no_status_overraide" , false);
		if ( timer_no_status_overraide )
		{
			const string nowStatus = MainWindow::m()->Config.Get(elecPrefix + "_status","");
			if (action == nowStatus)
			{
				NOTIFYLOG("家電タイマー(" << elec << " " << action << " " << elecActionPrefix << ")の実行時間ですが、すでに状態が " << nowStatus << " のため命令を追加で送りません。");
				return ;
			}
			NOTIFYLOG("家電タイマー(" << elec << " " << action << " " << elecActionPrefix << ")の実行時間です。状態は " << nowStatus << " なので発動します。");
		}
		else
		{
			//nop
		}

		Fire f;
		f.FireByType(elec,action);
	}
	catch(XLException &e )
	{
		NOTIFYLOG("家電タイマー(" << elec << " " << action << " " << elecActionPrefix << ")の実行に失敗しました。 "<< e.what());
	}
}

//雨か雪が降るか？
bool ScriptWebRunnerTrigger::trigger_if_weather_IsRain(const string weather) const
{
	if ( (weather == "rain" || weather == "snow") )
	{
		return true;
	}
	return false;
}

//トリガーの新規作成するキーをもとめる.
int ScriptWebRunnerTrigger::NewTriggerID() const
{
	int max = 1;
	const auto configmap = MainWindow::m()->Config.FindGetsToMap("trigger_",false);
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		const int key = atoi( it->first.c_str() + (sizeof("trigger_") - 1) );
		if (key > max)
		{
			max = key;
		}
	}
	return max + 1;
}

#endif //WITH_CLIENT_ONLY_CODE==1
