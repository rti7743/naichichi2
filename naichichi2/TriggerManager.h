// TriggerManager.h: TriggerManager クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TriggerManager_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_TriggerManager_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class TriggerManager
{
public:
	TriggerManager();
	virtual ~TriggerManager();
	
	bool Create();
	bool Regist(const CallbackDataStruct* callback , const string & menuName, const string & triggername );
	string Call( const string & triggername ,const map<string,string>& args);
	bool IsExist(const string & triggerName) const;
	map<string,string> GetAllMenuname() const;
private:
	struct triggerStruct
	{
		string menuname;
		const CallbackDataStruct* callback;

		triggerStruct(const string& menuname,const CallbackDataStruct* callback) : menuname(menuname) , callback(callback)
		{
		};
	};
	map<string,const triggerStruct*> TriggerMap;
	
};



#endif // !defined(AFX_TriggerManager_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
