//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include "Trigger.h"
#include "XLHttpSocket.h"

#include "MainWindow.h"

Trigger::Trigger()
{
	this->Thread = NULL;
	this->StopFlag = false;
}

Trigger::~Trigger()
{
	DEBUGLOG("stop...");

	Stop();

	DEBUGLOG("done");
}

void Trigger::Create()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	this->StopFlag = false;

	this->Thread = new thread([=](){
		XLDebugUtil::SetThreadName("Trigger");
		this->ThreadMain(); 
	});
}

void Trigger::Stop()
{
	if (this->Thread)
	{
		this->StopFlag = true;
		this->EventObject.notify_one();
		this->Thread->join();
		delete this->Thread;
		this->Thread = NULL;
	}
}

void Trigger::AppendNextSecondTrigger(const CallbackPP callback, unsigned int nextSecond ,bool isAutoInterval )
{
	lock_guard<mutex> al(this->updatelock);	

	//ここですぐにthis->CronCallbackableTriggerListリストに突っ込んではいけない.
	//javascriptのタイマーは、実行中に、自分自身のもう一度タイマーをタイマーを追加できたり、削除できたりと、
	//非常にフリーダムで、下手にロックをかけると、フリーズしてしまう。
	//実行する内容を関数オブジェクトにして、実行キューに入れて、あとで Tiggerスレッド自身に実行させる。
	//そうしないとスレッド競合でフリーズする可能性がある.
	if (isAutoInterval)
	{
		ASyncFunctionList.push_back([=](){
			CronCallbackableTriggerSt s ;
			s.callback = callback;
			s.nextSecond = nextSecond;
			s.fireTime = time(NULL) + (nextSecond);
			this->CronCallbackableTriggerList.push_back(s);
		});
	}
	else
	{
		ASyncFunctionList.push_back([=](){
			AtCallbackableTriggerSt s;
			s.callback = callback;
			s.nextSecond = nextSecond;
			s.fireTime = time(NULL) + (nextSecond);
			this->AtCallbackableTriggerList.push_back(s);
		});
	}
}

void Trigger::AppendNextTimeTrigger(const CallbackPP callback, const time_t fireTime )
{
	lock_guard<mutex> al(this->updatelock);	

	//ここですぐにthis->CronCallbackableTriggerListリストに突っ込んではいけない.
	//javascriptのタイマーは、実行中に、自分自身のもう一度タイマーをタイマーを追加できたり、削除できたりと、
	//非常にフリーダムで、下手にロックをかけると、フリーズしてしまう。
	//実行する内容を関数オブジェクトにして、実行キューに入れて、あとで Tiggerスレッド自身に実行させる。
	//そうしないとスレッド競合でフリーズする可能性がある.
	ASyncFunctionList.push_back([=](){
		AtCallbackableTriggerSt s;
		s.callback = callback;
		s.nextSecond = 0;
		s.fireTime = fireTime;
		this->AtCallbackableTriggerList.push_back(s);

		DEBUGLOG("aa " << s.fireTime << " " << XLStringUtil::timetostr(s.fireTime) << " " );
	});
}

void Trigger::AppendSensorCallbackableTrigger(const CallbackPP callback,SensorType type, float value,Expr expr)
{
	lock_guard<mutex> al(this->updatelock);	

	//ここですぐにthis->ASyncFunctionListリストに突っ込んではいけない.
	//javascriptのタイマーは、実行中に、自分自身のもう一度タイマーをタイマーを追加できたり、削除できたりと、
	//非常にフリーダムで、下手にロックをかけると、フリーズしてしまう。
	//実行する内容を関数オブジェクトにして、実行キューに入れて、あとで Tiggerスレッド自身に実行させる。
	//そうしないとスレッド競合でフリーズする可能性がある.
	ASyncFunctionList.push_back([=](){
		SensorCallbackableTriggerSt s;
		s.type = type;
		s.callback = callback;
		s.value = value;
		s.expr = expr;
		s.lastResult = false;

		this->SensorCallbackableTriggerList.push_back(s);
	});
}

void Trigger::OnSensorUpdate(float temp,float lumi)
{
	lock_guard<mutex> al(this->updatelock);	

	//ここですぐにthis->ASyncFunctionListリストに突っ込んではいけない.
	//javascriptのタイマーは、実行中に、自分自身のもう一度タイマーをタイマーを追加できたり、削除できたりと、
	//非常にフリーダムで、下手にロックをかけると、フリーズしてしまう。
	//実行する内容を関数オブジェクトにして、実行キューに入れて、あとで Tiggerスレッド自身に実行させる。
	//そうしないとスレッド競合でフリーズする可能性がある.
	ASyncFunctionList.push_back([=](){
		this->SensorTemp = temp;
		this->SensorLumi = lumi;
	});
}


void Trigger::ThreadMain()
{
	const time_t BAD_TIME = 1000000000;
	
	this->SensorTemp = 0;
	this->SensorLumi = 0;
	while(1)
	{
		if (this->StopFlag) break;

		{
			{	
				//リストに積んであるキューを実行する.
				lock_guard<mutex> al(this->updatelock);

				for(auto it = ASyncFunctionList.begin() ; it != ASyncFunctionList.end() ; ++ it)
				{
					(*it)();
				}
				ASyncFunctionList.clear();
			}

			time_t now = time(NULL);

			for(auto it = this->CronCallbackableTriggerList.begin() ; it != this->CronCallbackableTriggerList.end() ; ++it )
			{
				if (now < BAD_TIME)
				{//時計が正しい時刻補正されていないので、トリガーを動かさない
					NOTIFYLOG("時刻が補正されていないので、トリガーを動かせません now:" << now);
					continue;
				}
				if ( now >= it->fireTime )
				{
					MainWindow::m()->ScriptManager.FireTrigger( it->callback);
					//基本的に自動延長
					it->fireTime = time(NULL) + (it->nextSecond);
				}
			}

			for(auto it = this->AtCallbackableTriggerList.begin() ; it != this->AtCallbackableTriggerList.end() ;  )
			{
				if (now < BAD_TIME)
				{//時計が正しい時刻補正されていないので、トリガーを動かさない
					NOTIFYLOG("時刻が補正されていないので、トリガーを動かせません now:" << now);
					++it;
					continue;
				}

				if ( now >= it->fireTime )
				{
					MainWindow::m()->ScriptManager.FireTrigger(it->callback);
					//一度fireしたらリストから消す
					auto ite = it++;
					this->AtCallbackableTriggerList.erase(ite);
				}
				else
				{
					++it;
				}
			}

			for(auto it = this->SensorCallbackableTriggerList.begin() ; it != this->SensorCallbackableTriggerList.end() ; ++it )
			{
				if (it->lastResult)
				{
					continue;
				}
				if (it->type == SensorType_Temp)
				{
					if ( checkExpr(it->expr,this->SensorTemp,it->value) )
					{
						it->lastResult = true;
						MainWindow::m()->ScriptManager.FireTrigger(it->callback);
					}
				}
				else if (it->type == SensorType_Lumi)
				{
					if ( checkExpr(it->expr,this->SensorLumi,it->value) )
					{
						it->lastResult = true;
						MainWindow::m()->ScriptManager.FireTrigger(it->callback);
					}
				}
			}
		}

		SecSleepEx(this->updatelock,this->EventObject,1);
	}
}
bool Trigger::checkExpr(Expr expr,float a,float b) const
{
	switch(expr)
	{
	case Expr_Equal:
		return a == b;
	case Expr_Not:
		return a != b;
	case Expr_Greater:
		return a > b;
	case Expr_Less:
		return a < b;
	case Expr_GreaterEq:
		return a >= b;
	case Expr_LessEq:
		return a <= b;
	};
	return false;
}

//staticメソッド
Trigger::Expr Trigger::convertStringToExpr(const string& s)
{
	if (s == "equal") return Expr_Equal;
	if (s == "not") return Expr_Not;
	if (s == "greater") return Expr_Greater;
	if (s == "less") return Expr_Less;
	if (s == "greatereq") return Expr_GreaterEq;
	if (s == "lesseq") return Expr_LessEq;

	return Expr_None;
}

void Trigger::RemoveCallbackDelay(const CallbackPP callback) 
{
	lock_guard<mutex> al(this->updatelock);	

	ASyncFunctionList.push_back([=](){
		RemoveCallbackLow(callback);
	});
}



//遅延して削除します。どこで呼び出しても安全です。フリーズしません。ただし、リソースの開放は遅延して実行されます。
void Trigger::RemoveCallbackLow(const CallbackPP& callback) 
{
	for(auto it = CronCallbackableTriggerList.begin() ; it != CronCallbackableTriggerList.end() ; )
	{
		if ( it->callback == callback )
		{
			auto ite = it++;
			CronCallbackableTriggerList.erase(ite);
		}
		else
		{
			++it;
		}
	}

	for(auto it = AtCallbackableTriggerList.begin() ; it != AtCallbackableTriggerList.end() ; )
	{
		if ( it->callback == callback )
		{
			auto ite = it++;
			AtCallbackableTriggerList.erase(ite);
		}
		else
		{
			++it;
		}
	}

	for(auto it = SensorCallbackableTriggerList.begin() ; it != SensorCallbackableTriggerList.end() ; )
	{
		if ( it->callback == callback )
		{
			auto ite = it++;
			SensorCallbackableTriggerList.erase(ite);
		}
		else
		{
			++it;
		}
	}
}

Trigger::SensorType Trigger::convertStringToSensorType(const string& s)
{
	if (s == "temp") return SensorType_Temp;
	else if (s == "lumi") return SensorType_Lumi;
	else return SensorType_None;
}





#endif // WITH_CLIENT_ONLY_CODE==1
