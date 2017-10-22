// NetDeviceWemo.h: NetDeviceWemo クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NetDeviceWemo_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_NetDeviceWemo_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "XLSocket.h"
#include "XLHttpHeader.h"
#include "UPNPServer.h"


class NetDeviceWemo
{
public:
	bool IsThisDevice(const string& name);
	list<string> GetAll(const list<UPNPMap>& servers);
	list<string> GetSetActionAll(const string& name);
	list<string> GetSetValueAll(const string& name,const string& action);
	list<string> GetGetActionAll(const string& name);
	bool Fire(const string& name,const string& action,const string& value);
	string Pickup(const string& name,const string& action);

private:
	string ResolveName(const string& name);
	string GetNameDisp(const UPNPMap &upnp,const string& xml);

};


#endif // !defined(AFX_NetDeviceWemo_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
