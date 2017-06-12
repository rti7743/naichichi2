// NetDeviceHUE.h: NetDeviceHUE クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NetDeviceHUE_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_NetDeviceHUE_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "XLSocket.h"
#include "XLHttpHeader.h"
#include "UPNPServer.h"

//HUEもっていないからつくれないよ未実装

class NetDeviceHUE
{
public:
	bool IsThisDevice(const string& name);
	list<string> GetAll(const list<UPNPMap>& servers);
	list<string> GetActionAll(const string& name);
	list<string> GetValueAll(const string& name,const string& action);
	bool Fire(const string& name,const string& action,const string& value,string* outSTD);
private:
	string ResolveName(const string& name);
	unsigned int ResolveAction(const string& action);
	string GetNameDisp(const UPNPMap &upnp,const string& xml);

};


#endif // !defined(AFX_NetDeviceHUE_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
