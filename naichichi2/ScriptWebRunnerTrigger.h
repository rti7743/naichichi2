#pragma once
#include "Callbackable.h"

class ScriptWebRunnerTrigger : public CallbackPP
{
public:
	ScriptWebRunnerTrigger();
	virtual ~ScriptWebRunnerTrigger();

	//トリガーの再設定
	void ReloadTrigger(const time_t& now);
	//トリガーの新規作成するキーをもとめる.
	int NewTriggerID() const;

	string run_command_resultjson(const string& execcommand,const string& execargs1,const string& execargs2,const string& execargs3,const string& execargs4,const string& execargs5,string* appendSTDOUT,map<string,string>* resultmap);
	//雨か雪が降るか？
	bool trigger_if_weather_IsRain(const string weather) const;
	void ClearCallback();
private:
	CallbackPP NewCallback(std::function<void (void) > f);
	void triggerFireTimeCallbackFunction(int triggerkey,const time_t& now);
	//トリガー/センサー
	void triggerFireSensorCallbackFunction(int triggerkey,const time_t& now);
	//トリガー/コマンド
	void triggerFireCommandCallbackFunction(int triggerkey,const time_t& now);
	//トリガー/web
	void triggerFireWebCallbackFunction(int triggerkey,const time_t& now);
	//トリガーの追加条件を検証する
	bool triggerCheckExIf(const string& prefix,const time_t& now);
	void triggerFire(int triggerkey,const time_t& now);
	//トリガーの次の曜日
	time_t convertTriggerDate(const time_t& now,int hour,int minute,int if_date_loop_hourly) const;
	//トリガーの曜日
	bool convertTriggerDateWeekDay(const time_t& now,int dayofweek,int holiday,const string& holiday_dataof) const;
	//家電タイマーの実行(おや○みタイマー)
	void triggerFireElecTimerCallbackFunction(unsigned int key1,unsigned int key2,const time_t& now);

	list<CallbackPP> CallbackList;
};
