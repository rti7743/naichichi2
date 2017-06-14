// TriggerManager.cpp: TriggerManager クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "ScriptRunner.h"
#include "TriggerManager.h"
#include "MainWindow.h"
#include "XLStringUtil.h"


//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////


TriggerManager::TriggerManager()
{
}
TriggerManager::~TriggerManager()
{
	for(auto it = this->TriggerMap.begin() ; it != this->TriggerMap.end() ; ++it )
	{
		delete it->second;
	}
}

bool TriggerManager::Create()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	return true;
}

bool TriggerManager::Regist(const CallbackDataStruct* callback , const string & menuName,const string & triggerName )
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	//すでにあるならメモリ解放しといてね
	auto alreadyIT = this->TriggerMap.find(triggerName);
	if ( alreadyIT != this->TriggerMap.end() )
	{
		delete alreadyIT->second;

		NOTIFYLOG(string() + "Trigger " + triggerName + " は、既に存在しますが、新しい値で上書きされます。" );
	}

	//追加(または上書き)
	this->TriggerMap[triggerName] = new triggerStruct(menuName,callback);
	return true;
}

string TriggerManager::Call(const string & triggerName ,const map<string,string>& args)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	for(auto it = this->TriggerMap.begin() ; it != this->TriggerMap.end() ; ++it )
	{
		if ( it->first == triggerName )
		{
			string respons;
			MainWindow::m()->ScriptManager.TriggerCall(it->second->callback,args,&respons);
			return respons;
		}
	}

	throw XLEXCEPTION("指定された" + triggerName + "は存在しません");
}

bool TriggerManager::IsExist(const string & triggerName) const
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	auto alreadyIT = this->TriggerMap.find(triggerName);
	return alreadyIT != this->TriggerMap.end();
}

map<string,string> TriggerManager::GetAllMenuname() const
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	map<string,string> d;
	for(auto it = this->TriggerMap.begin() ; it != this->TriggerMap.end() ; ++it )
	{
		if (! it->second->menuname.empty() )
		{
			d.insert( pair<string,string>(it->first,it->second->menuname) );
		}
	}
	return d;
}