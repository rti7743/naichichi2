// Device.h: Device クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Device_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_Device_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "XLSocket.h"
#include "XLHttpHeader.h"
#include "NetDeviceEcoNetLite.h"
#include "NetDeviceWemo.h"

class NetDevice
{
public:
	list<string> GetAll(unsigned int searchtime);
	list<string> GetSetActionAll(const string& name);
	list<string> GetSetValueAll(const string& name,const string& action);
	list<string> GetGetActionAll(const string& name);
	bool Fire(const string& name,const string& action,const string& value);
	string Pickup(const string& name,const string& action);


	static unsigned char ResolveValueName(const string& name);
private:
	NetDeviceEcoNetLite EcoNetLite;
	NetDeviceWemo Wemo;

};


#endif // !defined(AFX_Device_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
