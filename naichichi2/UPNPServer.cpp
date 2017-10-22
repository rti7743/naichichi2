//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)

#include "common.h"
#include "UPNPServer.h"
#include "MainWindow.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"
#include "XLGZip.h"
#include "SystemMisc.h"


//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
UPNPServer::UPNPServer()
{
	this->UPNPThread = NULL;
	this->StopFlag = false;
}
UPNPServer::~UPNPServer()
{
	DEBUGLOG("stop...");

	Stop();
	DeleteList(&this->MappingList);

	DEBUGLOG("done");
}


void UPNPServer::Create()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	//upnpスレッド作成.
	DEBUGLOG("upnp server...");
	this->UPNPThread = new thread([=](){
#if WITH_CLIENT_ONLY_CODE==1
		XLDebugUtil::SetThreadName("UPNPServer");
		this->upnpThreadMain(); 
#endif //WITH_CLIENT_ONLY_CODE==1
	});
}

void UPNPServer::Stop()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	this->StopFlag = true;

	if (this->UPNPThread)
	{
		this->UpnpSocket.Close();
		this->UPNPThread->join();

		delete this->UPNPThread;
		this->UPNPThread = NULL;
	}
}

void UPNPServer::upnpThreadMain()
{
	this->UpnpSocket.CreateUDP(60);

	//1900ポートを利用します.
	this->UpnpSocket.Bind(INADDR_ANY,1900);

	//インターフェースがマルチキャストを受け取れるように設定する.
	this->UpnpSocket.AddMulticastMembersip("239.255.255.250");

	//recvfrom で読み込む前に、まずはNOTIFYで自己紹介
	this->upnpNotify();
	//ついでにほかのノードの探索
	this->upnpMSearch(2);

	//UPNPサーバー
	while(! this->StopFlag )
	{
		struct sockaddr_in senderinfo;

		char buffer[8192];
		int fromlen = sizeof(senderinfo);
		int size = this->UpnpSocket.RecvFrom(buffer , sizeof(buffer)-1,0 ,&senderinfo,&fromlen);
		if (size < 0)
		{
			continue;
		}
		buffer[size] = 0;
		if (this->StopFlag) break;

		XLHttpHeader header;
		if ( ! header.Parse( buffer ,size ) )
		{
			ERRORLOG("UPNPヘッダをパースできませんでした " << buffer );
			continue;
		}

		const string ip = inet_ntoa(senderinfo.sin_addr);
		if ( header.getFirstParam(0) == "M-SEARCH" )
		{//M-SEARCHへ応答
			const string deviceURL = MainWindow::m()->Httpd.getWebURL("/api/device.xml");
			const string uuid = MainWindow::m()->Httpd.GetUUID();
			string respons = 
				"HTTP/1.1 200 OK\r\n"
				"EXT:\r\n"
				"LOCATION: " + deviceURL + "\r\n"
				"SERVER: linux UPnP/1.0 naichichi2\r\n"
				"ST: upnp:rootdevice:rti-giken:naichichi2\r\n"
				"USN: uuid:" + uuid + "::upnp:rootdevice:rti-giken:naichichi2\r\n"
				"\r\n"
				;
			this->UpnpSocket.SendTo(respons.c_str(),respons.size(),0 ,&senderinfo,fromlen);

			//DEBUGLOG("UPNPヘッダ応答を返します " << respons << "  リクエスト " << buffer );
		}
		else if ( header.getFirstParam(0) == "NOTIFY" )
		{//NOTIFYの観測.
			Add(ip,header);
		}
		else if ( header.getFirstParam(1) == "200" && header.getFirstParam(2) == "OK" )
		{//M-SEARCHへの応答結果？
			Add(ip,header);
		}
		else
		{
			ERRORLOG("不明なUPNPヘッダです " << buffer );
			continue;
		}
	}
}

string UPNPServer::parseUSNtoUUID(const string& usn) const
{
	//uuidのパース
	const char * uuidStart = strstr(usn.c_str() , "uuid:");
	if (!uuidStart)
	{//uuidがない
		return "";
	}

	uuidStart = uuidStart + sizeof("uuid:") - 1;
	const char * uuidEnd = strstr(uuidStart,":");
	if (uuidEnd)
	{
		return string(uuidStart,0,uuidEnd-uuidStart);
	}
	else
	{
		return uuidStart;
	}
}
//すべての端末の取得
//ロックが面倒なのでコピーして返す. コストは負担する.
void UPNPServer::GetAll(list<UPNPMap>* outRet)
{
	volatile lock_guard<mutex> al(this->lock);
	for(auto it = this->MappingList.begin() ; it!=this->MappingList.end() ; it++)
	{
		NOTIFYLOG("UPNP FOUND :" << (*it)->ip << " " << (*it)->uuid );
		outRet->push_back( *(*it) );
	}
}

//UPNP で NOTIFYを送ります
void UPNPServer::upnpNotify()
{
	const string deviceURL = MainWindow::m()->Httpd.getWebURL("/api/device.xml");
	const string uuid = MainWindow::m()->Httpd.GetUUID();
	const string msg = 
		"NOTIFY * HTTP/1.1\r\n"
		"HOST: 239.255.255.250:1900\r\n" 
//		"CHACHE-CONTROL: max-age\r\n"
		"LOCATION: " + deviceURL + "\r\n"
		"ST: upnp:rootdevice:rti-giken:naichichi2\r\n"
		"STS: ssdp:alive\r\n"
		"SERVER: linux UPnP/1.0 naichichi2\r\n"
		"USN: uuid:" + uuid + "::upnp:rootdevice:rti-giken:naichichi2\r\n"
		"\r\n"
		;

	XLSocket socket;
	socket.CreateUDP(5); 
	socket.SetSockopt(SOL_SOCKET,SO_BROADCAST, 1);
	struct sockaddr_in addr = 
		XLSocket::ToSockAddrIn("239.255.255.250",1900);
	socket.SendTo(msg.c_str() , msg.size() , 0 , &addr , sizeof(addr) );
}

//UPNP で M-SEARCHを送ります
void UPNPServer::upnpMSearch(unsigned int timeoutS)
{
	const string msg = 
		"M-SEARCH * HTTP/1.1\r\n" 
		"HOST: 239.255.255.250:1900\r\n" 
		"MAN: \"ssdp:discover\"\r\n" 
		"ST: upnp:rootdevices\r\n"
		"MX: 3\r\n"
		"\r\n"
		;
	XLSocket socket;
	socket.CreateUDP(timeoutS); 
	socket.SetSockopt(SOL_SOCKET,SO_BROADCAST, 1);
	struct sockaddr_in addr = 
		XLSocket::ToSockAddrIn("239.255.255.250",1900);
	socket.SendTo(msg.c_str() , msg.size() , 0 , &addr , sizeof(addr) );

	//なぜか この M-Searchは、upnpThreadMainの方で補足できないらしい
	while(1)
	{
		struct sockaddr_in senderinfo;
		char buffer[8192];
		int fromlen = sizeof(senderinfo);
		int size = socket.RecvFrom(buffer, sizeof(buffer)-1,0,&senderinfo,&fromlen);
		if (size < 0)
		{
			break;
		}
		buffer[size] = 0;
		const string ip = inet_ntoa(senderinfo.sin_addr);

		XLHttpHeader header;
		if ( ! header.Parse( buffer ,size ) )
		{
			DEBUGLOG("HTTPDヘッダーとしてパース出来ません " << buffer );
			continue;
		}

		Add(ip,header);
	}
}

void UPNPServer::Add(const string& ip,const XLHttpHeader& header )
{
	const string usn = header.getAt("usn");
	const string uuid = parseUSNtoUUID(usn);
	if (uuid.empty())
	{
		DEBUGLOG("UPNP UUIDがありません USN:" << usn);
		return ;
	}

	{
		volatile lock_guard<mutex> al(this->lock);

		for(auto it = MappingList.begin(); it != MappingList.end() ; it++)
		{
			if ( (*it)->ip == ip )
			{
				if ( (*it)->uuid == uuid )
				{
					return ;
				}
			}
		}

		//新規追加
		UPNPMap * m = new UPNPMap;
		m->ip = ip;
		m->usn = usn;
		m->uuid = uuid;
		m->server = header.getAt("server");
		m->st = header.getAt("st");
		m->location = header.getAt("location");

		this->MappingList.push_back(m);
	}
}

//端末の取得
bool UPNPServer::Get(const string& uuid,UPNPMap* outMap)
{
	volatile lock_guard<mutex> al(this->lock);

	for(auto it = MappingList.begin(); it != MappingList.end() ; it++)
	{
		if ( (*it)->uuid == uuid )
		{
			if (outMap)
			{
				*outMap = *(*it);
			}
			return true;
		}
	}
	return false;
}


bool UPNPServer::STSMatch(const UPNPMap& m,const string& findst) 
{
	if (   m.usn.find(findst) != string::npos 
		|| m.st.find(findst) != string::npos )
	{
		return true;
	}

	return false;
}

//端末の取得
bool UPNPServer::GetST(const string& findst,UPNPMap* outMap)
{
	volatile lock_guard<mutex> al(this->lock);

	for(auto it = MappingList.begin(); it != MappingList.end() ; it++)
	{
		if ( STSMatch( *(*it),findst)  )
		{
			if (outMap)
			{
				*outMap = *(*it);
			}
			return true;
		}
	}
	return false;
}


#include "XLHttpSocket.h"

//端末の取得
bool UPNPServer::GetSTAndXML(const string& findst,const string& findxml,UPNPMap* outMap,string* outXML)
{
	//同じlocationを何回も調べるのは無駄なので.
	list<string> findlocations;

	list<UPNPMap> servers;
	GetAll(&servers);
	for(auto it = servers.begin() ; it != servers.end() ; it++ )
	{
		if ( STSMatch(*it,findst)  )
		{//ノードがあったのでxmlを取得して目的のものがあるか見る
			auto isAlready = find(findlocations.begin(),findlocations.end(),it->location );
			if (isAlready == findlocations.end() )
			{//既に調べた.
				continue;
			}
			findlocations.push_back( it->location);

			string xml = XLHttpSocket::Get(it->location);
			if ( xml.find(findxml) != string::npos)
			{
				if (outMap)
				{
					*outMap = *it;
				}
				if (outXML)
				{
					*outXML = xml;
				}
				return true;
			}
		}
	}
	return false;
}
