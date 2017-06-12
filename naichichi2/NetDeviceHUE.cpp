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
	return XLStringUtil::cut(name,"(hue://",")",NULL);
}
unsigned int NetDeviceHUE::ResolveAction(const string& action)
{
	unsigned int i=0;
	string a = XLStringUtil::cut(action,"(#",")",NULL);
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
	string uuid = ResolveName(name);

	UPNPMap m;
	bool r = MainWindow::m()->UPNPServer.GetSimple(uuid,&m);
	if (!r)
	{
		return false;
	}

	unsigned int actionU = ResolveAction(action);
	unsigned int valueU = ResolveAction(value);
	
	if (actionU == 0x00)
	{//POWER
		string sw;
		if (valueU == 0x01)
		{//ON
			sw = "1";
		}
		else
		{//OFF
			sw = "0";
		}
		string url = XLStringUtil::hosturl(m.location) 
			+ "/upnp/control/basicevent1";
		string postdata = 
			string() +
			"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" +
			"<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\n" +
			" <s:Body>\n" +
			"  <u:SetBinaryState xmlns:u=\"urn:Belkin:service:basicevent:1\">\n" + // 対象の Action を指定
			"   <BinaryState>" + sw + "</BinaryState>\n" + // ON/OFF を 1/0 で指定
			"  </u:SetBinaryState>\n" +
			" </s:Body>\n" +
			"</s:Envelope>\n";

		map<string,string> header;
		header["User-Agent"] = "CyberGarage-HTTP/1.0"; // iPhone App と同じ User-Agent
		header["SOAPACTION"] = "\"urn:Belkin:service:basicevent:1#SetBinaryState\"";
		header["Content-Type"] = "text/xml; charset=\"utf-8\"";

		string ret = XLHttpSocket::Post(url,header,5,postdata.c_str() , postdata.size() );
		
		return true;
	}
	return false;
}
