// NetDeviceEcoNetLite.h: NetDeviceEcoNetLite クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NetDeviceEcoNetLite_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_NetDeviceEcoNetLite_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "XLSocket.h"
#include "XLHttpHeader.h"
#include "EcoNetLiteServer.h"

class NetDeviceEcoNetLite
{
public:

	//すべての端末名を取得
	list<string> GetAll();
	//端末名の動作を取得
	list<string> GetSetActionAll(const string& name);
	list<string> GetSetValueAll(const string& name,const string& action);
	list<string> GetGetActionAll(const string& name);
	bool IsThisDevice(const string& name);
	bool Fire(const string& name,const string& action,const string& value);
	string Pickup(const string& name,const string& action);

	string MakeName(const string& identification,const EcoNetLiteObjCode& deoj);
	static string CodeToNameDisp(const EcoNetLiteObjCode& deoj);

private:
	string MakeActionName(const EcoNetLiteObjCode& deoj,const unsigned char& prop);
	string CodeToActionNameDisp(const EcoNetLiteObjCode& deoj,const unsigned char& prop);
	unsigned char ResolveActionName(const string& name);
	unsigned char ResolveValueName(const string& name);

	string ResolveName(const string& name);
	void GetValueTemp50(list<string>* outRet);
	void GetValueParsent100(list<string>* outRet);
	void GetLevel8(bool enableAuto41,list<string>* outRet);
	bool GetValue(const EcoNetLiteObjCode& deoj,const unsigned char& prop,list<string>* outRet);
};



#endif // !defined(AFX_NetDeviceEcoNetLite_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
