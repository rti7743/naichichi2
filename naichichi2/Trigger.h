#pragma once
#include "Callbackable.h"

class Trigger
{
public:
	Trigger();
	virtual ~Trigger();

	enum Expr
	{
		 Expr_None	//不明 常に false を返す
		,Expr_Equal
		,Expr_Not
		,Expr_Greater
		,Expr_Less
		,Expr_GreaterEq
		,Expr_LessEq
	};
	enum SensorType
	{
		 SensorType_None
		,SensorType_Temp
		,SensorType_Lumi
	};
	void Create() ;

	void UpdateConfig();
	void AppendNextSecondTrigger(const CallbackPP callback, unsigned int nextSecond ,bool isAutoInterval );
	void AppendNextTimeTrigger(const CallbackPP callback, const time_t fireTime );

	void AppendSensorCallbackableTrigger(const CallbackPP callback,SensorType type, float value,Expr expr);
	//遅延して削除します。どこで呼び出しても安全です。フリーズしません。ただし、リソースの開放は遅延して実行されます。
	void RemoveCallbackDelay(const CallbackPP callback) ;

	void OnSensorUpdate(float temp,float lumi);


	static Trigger::Expr convertStringToExpr(const string& s);
	static Trigger::SensorType convertStringToSensorType(const string& s);

	struct CronCallbackableTriggerSt
	{
		CallbackPP callback;
		unsigned int nextSecond;
		time_t fireTime;
	};
	struct AtCallbackableTriggerSt
	{
		CallbackPP callback;
		unsigned int nextSecond;
		time_t fireTime;
	};
	struct SensorCallbackableTriggerSt
	{
		CallbackPP callback;
		Trigger::SensorType type;
		float value;
		Expr expr;
		bool lastResult;
	};

	list<CronCallbackableTriggerSt>   CronCallbackableTriggerList;
	list<AtCallbackableTriggerSt>     AtCallbackableTriggerList;
	list<SensorCallbackableTriggerSt> SensorCallbackableTriggerList;

	list< std::function<void (void) > > ASyncFunctionList;

private:
	void Stop();
	void ThreadMain();
	bool checkExpr(Expr expr,float a,float b) const;
	void RemoveCallbackLow(const CallbackPP& callback) ;

	thread*					Thread;
	mutable mutex updatelock;
	condition_variable	EventObject;
	
	bool StopFlag;

	float SensorTemp;
	float SensorLumi;
};
