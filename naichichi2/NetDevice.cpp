//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)

#include "common.h"
#include "NetDevice.h"
#include "MainWindow.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"
#include "XLGZip.h"
#include "SystemMisc.h"


//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

//すべての端末名を取得
list<string> NetDevice::GetAll(unsigned int searchtime)
{
	MainWindow::m()->EcoNetLiteServer.sendSearchRequest();
	MainWindow::m()->UPNPServer.upnpMSearch(searchtime);

	list<string> ret;
	ret = EcoNetLite.GetAll();

	list<UPNPMap> upnpServers;
	MainWindow::m()->UPNPServer.GetAll(&upnpServers);
	list<string> t = Wemo.GetAll(upnpServers);
	if(ret.size() <= 0)
	{
		return t;
	}

	ret.insert(ret.end(),t.begin(),t.end());
	return ret;
}
list<string> NetDevice::GetSetActionAll(const string& name)
{
	if ( EcoNetLite.IsThisDevice(name) )
	{
		return EcoNetLite.GetSetActionAll(name);
	}
	if ( Wemo.IsThisDevice(name) )
	{
		return Wemo.GetSetActionAll(name);
	}

	list<string> ret;
	return ret;
}

list<string> NetDevice::GetSetValueAll(const string& name,const string& action)
{
	if ( EcoNetLite.IsThisDevice(name) )
	{
		return EcoNetLite.GetSetValueAll(name,action);
	}
	if ( Wemo.IsThisDevice(name) )
	{
		return Wemo.GetSetValueAll(name,action);
	}

	list<string> ret;
	return ret;
}

bool NetDevice::Fire(const string& name,const string& action,const string& value)
{
	if ( EcoNetLite.IsThisDevice(name) )
	{
		return EcoNetLite.Fire(name,action,value);
	}
	if ( Wemo.IsThisDevice(name) )
	{
		return Wemo.Fire(name,action,value);
	}

	return false;
}

string NetDevice::Pickup(const string& name,const string& action)
{
	if ( EcoNetLite.IsThisDevice(name) )
	{
		return EcoNetLite.Pickup(name,action);
	}
	if ( Wemo.IsThisDevice(name) )
	{
		return Wemo.Pickup(name,action);
	}

	return false;
}

list<string> NetDevice::GetGetActionAll(const string& name)
{
	if ( EcoNetLite.IsThisDevice(name) )
	{
		return EcoNetLite.GetGetActionAll(name);
	}
	if ( Wemo.IsThisDevice(name) )
	{
		return Wemo.GetGetActionAll(name);
	}

	list<string> ret;
	return ret;
}


unsigned char NetDevice::ResolveValueName(const string& name)
{
	unsigned int i = 0;
	string a = XLStringUtil::cut(name,"(#",")",NULL);
	if(a.empty())
	{
		a = name;
	}
	sscanf(a.c_str(),"%02x",&i);
	return (unsigned char)i;
}
