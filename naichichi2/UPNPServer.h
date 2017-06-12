// UPNPServer.h: UPNPServer クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UpnpServer_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_UpnpServer_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "XLSocket.h"
#include "XLHttpHeader.h"


struct UPNPMap{
	string ip;
	string uuid;
	string st;
	string usn;
	string server;
	string location;
};


class UPNPServer
{
    thread*					UPNPThread;
public:

	UPNPServer();
	virtual ~UPNPServer();
	void Create();
	void Stop();

	//UPNP で M-SEARCHを送ります
	void upnpMSearch(unsigned int timeoutS);
	//端末の取得
	bool Get(const string& uuid,UPNPMap* outMap);
	//端末の取得
	bool GetST(const string& findst,UPNPMap* outMap);
	//端末の取得
	bool GetSTAndXML(const string& findst,const string& findxml,UPNPMap* outMap,string* outXML);

	//すべての端末の取得
	void GetAll(list<UPNPMap>* outRet);
	static bool STSMatch(const UPNPMap& m,const string& findst) ;

private:

	//UPNP送受信のスレッドメイン
	void upnpThreadMain();

	//UPNP で NOTIFYを送ります
	void upnpNotify();

	string uuid;
	string server;

	//端末の追加
	void Add(const string& ip,const XLHttpHeader& header );

	//USNからUUIDを取得する.
	string parseUSNtoUUID(const string& usn) const;


	XLSocket		UpnpSocket;

	bool StopFlag;

	//発見した機材
	list<UPNPMap*> MappingList;

	mutable mutex lock;
};


#endif // !defined(AFX_UpnpServer_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
