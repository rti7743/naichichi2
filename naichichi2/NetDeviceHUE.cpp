//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)

//HUEもっていないからつくれないよ未実装

#include "common.h"
#include "NetDeviceHUE.h"
#include "MainWindow.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"
#include "XLGZip.h"
#include "SystemMisc.h"
#include "XLHttpSocket.h"


//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
bool NetDeviceHUE::IsThisDevice(const string& name)
{
	return name.find("hue://")!=string::npos;
}
string NetDeviceHUE::ResolveName(const string& name)
{
	//(hue://x.x.x.x@123)ではなければ、 hue://@123 かな・・・？
	const string id = XLStringUtil::cut(name,"hue://","",NULL);
	if (! id.empty())
	{
		return id;
	}
	//それでもダメならあきらめる
	return "";
}
unsigned int NetDeviceHUE::ResolveAction(const string& action)
{
	unsigned int i=0;
	string a = XLStringUtil::cut(action,"(#",")",NULL);
	if(a.empty())
	{
		a = action;
	}
	sscanf(a.c_str(),"%02x",&i);
	return i;
}

string NetDeviceHUE::GetNameDisp(const UPNPMap &upnp,const string& xml)
{
	string name = XLStringUtil::cut(xml,"<modelName>","</modelName>");
	if(name.empty())
	{
		name = XLStringUtil::cut(xml,"<friendlyName>","</friendlyName>");
	}
	return name + "(hue://" + upnp.uuid + ")";
}

//すべての端末名を取得
list<string> NetDeviceHUE::GetAll(const list<UPNPMap>& servers)
{
	list<string> ret;
	const string findst = "urn:schemas-upnp-org:device:basic:1";

	for(auto it = servers.begin() ; it != servers.end() ; it++ )
	{
		if (! UPNPServer::STSMatch(*it,findst)  )
		{
			continue;
		}
		if( it->server.find("IpBridge") == string::npos)
		{
			continue;
		}
		string xml = XLHttpSocket::Get(it->location,5);
		ret.push_back( GetNameDisp(*it,xml) );
	}
	return ret;
}
list<string> NetDeviceHUE::GetActionAll(const string& name)
{
	list<string> ret;
	ret.push_back("POWER(#00)");
	return ret;
}

list<string> NetDeviceHUE::GetValueAll(const string& name,const string& action)
{
	list<string> ret;
	ret.push_back("ON(#01)");
	ret.push_back("OFF(#00)");
	return ret;
}

bool NetDeviceHUE::Fire(const string& name,const string& action,const string& value,string* outSTD)
{
	return false;
}
