//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)

#include "common.h"
#include "EcoNetLiteServer.h"
#include "MainWindow.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"
#include "XLGZip.h"
#include "SystemMisc.h"
#include "NetDeviceEcoNetLite.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
EcoNetLiteServer::EcoNetLiteServer()
{
	this->EcoNetLiteThread = NULL;
	this->StopFlag = false;
	this->TIDTotal = 0;
}
EcoNetLiteServer::~EcoNetLiteServer()
{
	DEBUGLOG("stop...");

	Stop();
	DeleteList(&this->MappingList);

	DEBUGLOG("done");
}


void EcoNetLiteServer::Create()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	//econetliteスレッド作成.
	DEBUGLOG("econetlite server...");
	this->EcoNetLiteThread = new thread([=](){
#if WITH_CLIENT_ONLY_CODE==1
		this->EcoNetLiteThreadMain(); 
#endif //WITH_CLIENT_ONLY_CODE==1
	});
}

void EcoNetLiteServer::Stop()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	this->StopFlag = true;

	if (this->EcoNetLiteThread)
	{
		this->EcoNetLiteSocket.Close();
		this->EcoNetLiteThread->join();

		delete this->EcoNetLiteThread;
		this->EcoNetLiteThread = NULL;
	}
}

void EcoNetLiteServer::EcoNetLiteThreadMain()
{
	this->EcoNetLiteSocket.CreateUDP(60);

	//3610ポートを利用します.
	this->EcoNetLiteSocket.Bind(INADDR_ANY,3610);

	//インターフェースがマルチキャストを受け取れるように設定する.
	this->EcoNetLiteSocket.AddMulticastMembersip("224.0.23.0");

	//recvfrom で読み込む前に、各家電に情報を得る.
	this->sendSearchRequest();

	while(! this->StopFlag )
	{
		struct sockaddr_in senderinfo;

		char buffer[8192] = {0};
		int fromlen = sizeof(senderinfo);
		int size = this->EcoNetLiteSocket.RecvFrom(buffer , sizeof(buffer)-1,0 ,&senderinfo,&fromlen);
		if (size < 0)
		{
			continue;
		}
		buffer[size] = 0;
		if (this->StopFlag) break;

		NOTIFYLOG("ECONETLITE " << XLDebugUtil::HexDump(buffer,size) );

		const string ip = inet_ntoa(senderinfo.sin_addr);
		if (size < sizeof(EcoNetLiteData)-1 )
		{
			continue;
		}

		const EcoNetLiteData* data = (EcoNetLiteData*)buffer;
		if (!(data->header[0] == 0x10 && data->header[1] == 0x81))
		{
			continue;
		}

		if(data->epc_1 == 0xd6 || data->epc_1 == 0xd5)
		{//機材一覧
			ParseResponsD6(ip,data,buffer,size);
		}
		else if(data->epc_1 == 0x9D)
		{//機材のgetできるプロパティ
			ParseRespons9D(ip,data,buffer,size);
		}
		else if(data->epc_1 == 0x9E)
		{//機材のsetできるプロパティ
			ParseRespons9E(ip,data,buffer,size);
		}
		else if(data->epc_1 == 0x83)
		{//固有ID
			ParseRespons83(ip,data,buffer,size);
		}

		DoCallback(data,buffer,size);
	}
}


void EcoNetLiteServer::ParseResponsD6(const string& ip,const EcoNetLiteData* data,const char* buffer,int size)
{
	const char *p = buffer + sizeof(EcoNetLiteData);
	size_t n = MIN( (size - sizeof(EcoNetLiteData))/3 , data->edt_1);
	for(size_t i = 0 ; i < n ; i ++)
	{
		EcoNetLiteObjCode deoj;
		deoj.code[0] = p[i*3+0];
		deoj.code[1] = p[i*3+1];
		deoj.code[2] = p[i*3+2];

		//取り合えず追加.
		bool isallready =  Add(ip,deoj);
		if (isallready)
		{//新規に発見した機材の場合・・・
			//固有IDを聞く
			sendGetIdentificationRequest(ip,deoj);
			//setプロパティを問い合わせる.
			sendFindSetPropMapRequest(ip,deoj);
			//getプロパティを問い合わせる.
			sendFindGetPropMapRequest(ip,deoj);
		}
	}
}

void EcoNetLiteServer::ParseDataList(const EcoNetLiteData* data,const char* buffer,int size,list<unsigned char>* outValues)
{
	if (data->edt_1 < 16)
	{
		const size_t n = MIN(data->edt_1,size-sizeof(EcoNetLiteData));
		const unsigned char *p = (unsigned char*) buffer + sizeof(EcoNetLiteData);
		for(size_t i = 0 ; i < n ; i ++)
		{
			outValues->push_back(p[i]);
		}
	}
	else
	{//16個を超えるので、ビット表記されている
	 //see http://qiita.com/miyazawa_shi/items/4f11493079b5191bd5d5#_reference-3c80b969ad147c95ecb7
 	 //see http://qiita.com/katsumin/items/14970c340c6dc65dbd93
		const unsigned char *p = (unsigned char*) buffer + sizeof(EcoNetLiteData);
		const size_t n = MIN(data->pdc_1,size-sizeof(EcoNetLiteData));
		for(size_t i = 0 ; i < n ; i ++)
		{
			for(int bit = 0 ; bit < 8 ; bit ++)
			{
				if (p[i] & (0x01 << bit) )
				{
					unsigned char code = 0x80 + (bit*16) + i;
					outValues->push_back(code);
				}
			}
		}
	}
}

//setプロパティの検索
void EcoNetLiteServer::ParseRespons9E(const string& ip,const EcoNetLiteData* data,const char* buffer,int size)
{
	list<unsigned char> setprop;
	EcoNetLiteServer::ParseDataList(data,buffer,size,&setprop);
	UpdateSetProp(ip,data->seoj,setprop);
}

//getプロパティの検索
void EcoNetLiteServer::ParseRespons9D(const string& ip,const EcoNetLiteData* data,const char* buffer,int size)
{
	list<unsigned char> getprop;
	EcoNetLiteServer::ParseDataList(data,buffer,size,&getprop);
	UpdateGetProp(ip,data->seoj,getprop);
}

void EcoNetLiteServer::ParseRespons83(const string& ip,const EcoNetLiteData* data,const char* buffer,int size)
{
	const unsigned char *p = (unsigned char*) buffer + sizeof(EcoNetLiteData);
	const size_t n = MIN(data->pdc_1,size-sizeof(EcoNetLiteData));
	string id = string((const char*)p,0,n);
	UpdateIdentification(ip,data->seoj,id);
}

//固有IDが取れない場合、IPとかを使って適当なIDを作ります.
//SSDPは固有ID UUIDが取れるが econetはそうじゃないので適当に作る
string EcoNetLiteServer::MakeAnonymousID(const string& ip,const EcoNetLiteObjCode& deoj)
{
	char buf[1024] = {0};
	snprintf(buf,1024,"%s@%02x%02x%02x",ip.c_str() , (unsigned int)deoj.code[0] , (unsigned int)deoj.code[1] , (unsigned int)deoj.code[2]);
	return buf;
}

//ほかの機材を探す
void EcoNetLiteServer::sendSearchRequest()
{
	XLSocket socket;
	socket.CreateUDP(1); //返事は期待していないので1秒だけ聞いてやる
	socket.SetSockopt(SOL_SOCKET,SO_BROADCAST, 1);

	struct sockaddr_in addr = 
		XLSocket::ToSockAddrIn("224.0.23.0",3610);

	unsigned short tid = NextTID();

	EcoNetLiteData data;
	data.header[0] = 0x10; //eco net lite ヘッダー
	data.header[1] = 0x81;
	data.tid[0] = (tid >> 8) & 0xFF;
	data.tid[1] = (tid & 0xFF);
	data.seoj.code[0] = 0x05; //コントローラーは 05 FF 01
	data.seoj.code[1] = 0xFF;
	data.seoj.code[2] = 0x01;
	data.deoj.code[0] = 0x0E; //相手は 0E F0 00
	data.deoj.code[1] = 0xF0; // Profile class group, Node profile class
	data.deoj.code[2] = 0x00;
	data.esv = 0x62;	//サービス名 set1:60 setC:61 Get:62
	data.epc = 0x01;
	data.epc_1 = 0xD6; //多分 取得する命令？// Self-node instance list S
	data.pdc_1 = 0x00;
	data.edt_1 = 0x00;

	socket.SendTo( (const char*) &data , sizeof(data) - 1 , 0 , &addr , sizeof(addr) );
}
//Setできるプロパティを探す.
//see http://qiita.com/miyazawa_shi/items/4f11493079b5191bd5d5#_reference-3c80b969ad147c95ecb7
void EcoNetLiteServer::sendFindSetPropMapRequest(const string& ip,const EcoNetLiteObjCode& deoj)
{
	XLSocket socket;
	socket.CreateUDP(1); //返事は期待していないので1秒だけ聞いてやる

	struct sockaddr_in addr = 
		XLSocket::ToSockAddrIn(ip,3610);

	unsigned short tid = NextTID();

	EcoNetLiteData data;
	data.header[0] = 0x10; //eco net lite ヘッダー
	data.header[1] = 0x81;
	data.tid[0] = (tid >> 8) & 0xFF;
	data.tid[1] = (tid & 0xFF);
	data.seoj.code[0] = 0x05; //コントローラーは 05 FF 01
	data.seoj.code[1] = 0xFF;
	data.seoj.code[2] = 0x01;
	data.deoj = deoj;
	data.esv = 0x62;	//サービス名 set1:60 setC:61 Get:62
	data.epc = 0x01;
	data.epc_1 = 0x9E; //Setプロパティマップの取得命令
	data.pdc_1 = 0x00;
	data.edt_1 = 0x00;

	socket.SendTo( (const char*) &data , sizeof(data) - 1 , 0 , &addr , sizeof(addr) );
}

//Getできるプロパティを探す.
void EcoNetLiteServer::sendFindGetPropMapRequest(const string& ip,const EcoNetLiteObjCode& deoj)
{
	XLSocket socket;
	socket.CreateUDP(1); //返事は期待していないので1秒だけ聞いてやる

	struct sockaddr_in addr = 
		XLSocket::ToSockAddrIn(ip,3610);

	unsigned short tid = NextTID();

	EcoNetLiteData data;
	data.header[0] = 0x10; //eco net lite ヘッダー
	data.header[1] = 0x81;
	data.tid[0] = (tid >> 8) & 0xFF;
	data.tid[1] = (tid & 0xFF);
	data.seoj.code[0] = 0x05; //コントローラーは 05 FF 01
	data.seoj.code[1] = 0xFF;
	data.seoj.code[2] = 0x01;
	data.deoj = deoj;
	data.esv = 0x62;	//サービス名 set1:60 setC:61 Get:62
	data.epc = 0x01;
	data.epc_1 = 0x9D; //Getプロパティマップの取得命令
	data.pdc_1 = 0x00;
	data.edt_1 = 0x00;

	socket.SendTo( (const char*) &data , sizeof(data) - 1 , 0 , &addr , sizeof(addr) );
}


//固有IDの取得
void EcoNetLiteServer::sendGetIdentificationRequest(const string& ip,const EcoNetLiteObjCode& deoj)
{
	XLSocket socket;
	socket.CreateUDP(1); //返事は期待していないので1秒だけ聞いてやる

	struct sockaddr_in addr = 
		XLSocket::ToSockAddrIn(ip,3610);

	unsigned short tid = NextTID();

	EcoNetLiteData data;
	data.header[0] = 0x10; //eco net lite ヘッダー
	data.header[1] = 0x81;
	data.tid[0] = (tid >> 8) & 0xFF;
	data.tid[1] = (tid & 0xFF);
	data.seoj.code[0] = 0x05; //コントローラーは 05 FF 01
	data.seoj.code[1] = 0xFF;
	data.seoj.code[2] = 0x01;
	data.deoj = deoj;
	data.esv = 0x62;	//サービス名 set1:60 setC:61 Get:62
	data.epc = 0x01;
	data.epc_1 = 0x83; //固有IDの取得命令
	data.pdc_1 = 0x00;
	data.edt_1 = 0x00;

	socket.SendTo( (const char*) &data , sizeof(data) - 1 , 0 , &addr , sizeof(addr) );
}

//リクエストの送信.UDPなので結果は不定.
void EcoNetLiteServer::sendSetRequest(const string& ip,const EcoNetLiteObjCode& deoj,unsigned char propUC,unsigned char valueUC)
{
	XLSocket socket;
	socket.CreateUDP(1); //返事は期待していないので1秒だけ聞いてやる

	struct sockaddr_in addr = 
		XLSocket::ToSockAddrIn(ip,3610);

	unsigned short tid = NextTID();

	EcoNetLiteData data;
	data.header[0] = 0x10; //eco net lite ヘッダー
	data.header[1] = 0x81;
	data.tid[0] = (tid >> 8) & 0xFF;
	data.tid[1] = (tid & 0xFF);
	data.seoj.code[0] = 0x05; //コントローラーは 05 FF 01
	data.seoj.code[1] = 0xFF;
	data.seoj.code[2] = 0x01;
	data.deoj = deoj;
	data.esv = 0x60;	//サービス名 set1:60 setC:61 Get:62
	data.epc = 0x01;
	data.epc_1 = propUC;
	data.pdc_1 = 0x01;
	data.edt_1 = valueUC;

	socket.SendTo( (const char*) &data , sizeof(data), 0 , &addr , sizeof(addr) );
}

//値の取得 値が取れたらcallbackする
void EcoNetLiteServer::sendGetRequest(const string& ip,const EcoNetLiteObjCode& deoj,unsigned char propUC,ECONETLITESERVER_TIDCALLBACK& callback)
{
	XLSocket socket;
	socket.CreateUDP(1); //返事は期待していないので1秒だけ聞いてやる

	struct sockaddr_in addr = 
		XLSocket::ToSockAddrIn(ip,3610);

	unsigned short tid = NextTID();
	AppendTIDCallback(tid,callback);

	EcoNetLiteData data;
	data.header[0] = 0x10; //eco net lite ヘッダー
	data.header[1] = 0x81;
	data.tid[0] = (tid >> 8) & 0xFF;
	data.tid[1] = (tid & 0xFF);
	data.seoj.code[0] = 0x05; //コントローラーは 05 FF 01
	data.seoj.code[1] = 0xFF;
	data.seoj.code[2] = 0x01;
	data.deoj = deoj;
	data.esv = 0x62;	//サービス名 set1:60 setC:61 Get:62
	data.epc = 0x01;
	data.epc_1 = propUC;
	data.pdc_1 = 0x01;
	data.edt_1 = 0;

	socket.SendTo( (const char*) &data , sizeof(data) - 1, 0 , &addr , sizeof(addr) );
}

bool EcoNetLiteServer::FindLow(const string& ip,const EcoNetLiteObjCode& deoj,EcoNetLiteMap** outM)
{
	//かならずロックして呼ぶこと.
	ASSERT( !this->lock.try_lock() );

	for(auto it = this->MappingList.begin(); it != this->MappingList.end() ; it++)
	{
		if ( (*it)->ip == ip && deoj == (*it)->deoj )
		{
			*outM = (*it);
			return true;
		}
	}
	return false;
}

bool EcoNetLiteServer::Add(const string& ip,const EcoNetLiteObjCode& deoj)
{
	const string anonymous_identification = MakeAnonymousID(ip,deoj);
	{
		volatile lock_guard<mutex> al(this->lock);

		EcoNetLiteMap* m;
		if ( FindLow(ip,deoj,&m) )
		{//既にあるよ.
			return false;
		}

		//新規追加
		EcoNetLiteMap* newM = new EcoNetLiteMap;
		newM->ip = ip;
		newM->deoj = deoj;
		newM->identification = anonymous_identification;
		newM->is_anonymous = true;

		MappingList.push_back(newM);
	}
	NOTIFYLOG("econetlite add " << "IP:" << ip << " disp:" << NetDeviceEcoNetLite::CodeToNameDisp(deoj) );
	return true;
}

bool EcoNetLiteServer::UpdateSetProp(const string& ip,const EcoNetLiteObjCode& deoj,const list<unsigned char>& setprop)
{
	{
		volatile lock_guard<mutex> al(this->lock);

		EcoNetLiteMap* m;
		if (! FindLow(ip,deoj,&m) )
		{//未知の端末
			return false;
		}

		//プロパティ更新
		m->setProp = setprop;
	}

	return true;
}

bool EcoNetLiteServer::UpdateGetProp(const string& ip,const EcoNetLiteObjCode& deoj,const list<unsigned char>& getprop)
{
	{
		volatile lock_guard<mutex> al(this->lock);

		EcoNetLiteMap* m;
		if (! FindLow(ip,deoj,&m) )
		{//未知の端末
			return false;
		}

		//プロパティ更新
		m->getProp = getprop;
	}

	return true;
}

bool EcoNetLiteServer::UpdateIdentification(const string& ip,const EcoNetLiteObjCode& deoj,const string& id)
{
	if ( id.size() <= 8)
	{
		NOTIFYLOG("econetlite id " << "固有IDが小さすぎ" << "ID:" << id << " IP:" << ip << " disp:" << NetDeviceEcoNetLite::CodeToNameDisp(deoj) );
		return false;
	}

	{
		volatile lock_guard<mutex> al(this->lock);

		EcoNetLiteMap* m;
		if (! FindLow(ip,deoj,&m) )
		{//未知の端末
			return false;
		}

		//固有ID更新
		m->identification = id;
		m->is_anonymous = false;
	}
	NOTIFYLOG("econetlite id " << "ID:" << id << " IP:" << ip << " disp:" << NetDeviceEcoNetLite::CodeToNameDisp(deoj) );

	return true;
}
bool EcoNetLiteServer::Get(const string& id,EcoNetLiteMap* outMap)
{
	volatile lock_guard<mutex> al(this->lock);

	for(auto it = MappingList.begin(); it != MappingList.end() ; it++)
	{
		if ( id == (*it)->identification )
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

bool EcoNetLiteServer::Get(const string& id,EcoNetLiteMap* outMap,unsigned int timeoutSec)
{
	if ( Get(id,outMap) )
	{
		return true;
	}

	//データがないらしいので検索する.
	sendSearchRequest();

	//検索できましたか？
	const int timeoutLoopCount = timeoutSec * 5;
	for(int i = 0 ; i < timeoutLoopCount ; i++)
	{
		MiriSleep(200);
		if ( Get(id,outMap) )
		{
			return true;
		}
	}

	return false;
}

bool EcoNetLiteServer::Get(const string& ip,const EcoNetLiteObjCode& deoj,EcoNetLiteMap* outMap)
{
	volatile lock_guard<mutex> al(this->lock);

	for(auto it = MappingList.begin(); it != MappingList.end() ; it++)
	{
		if ( ip == (*it)->ip && deoj == (*it)->deoj )
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

bool EcoNetLiteServer::Get(const string& ip,const EcoNetLiteObjCode& deoj,EcoNetLiteMap* outMap,unsigned int timeoutSec)
{
	if ( Get(ip,deoj,outMap) )
	{
		return true;
	}

	//データがないらしいので検索する.
	sendSearchRequest();

	//検索できましたか？
	const int timeoutLoopCount = timeoutSec * 5;
	for(int i = 0 ; i < timeoutLoopCount ; i++)
	{
		MiriSleep(200);
		if ( Get(ip,deoj,outMap) )
		{
			return true;
		}
	}

	return false;
}

unsigned short EcoNetLiteServer::NextTID()
{
	volatile lock_guard<mutex> al(this->lock);
	return this->TIDTotal++;
}
bool EcoNetLiteServer::AppendTIDCallback(unsigned short tid,ECONETLITESERVER_TIDCALLBACK& callback)
{
	TIDWatch* tidwatch = new TIDWatch;
	tidwatch->callback = callback;
	tidwatch->tid = tid;

	volatile lock_guard<mutex> al(this->lock);
	this->TidWatch.push_back(tidwatch);
	return true;
}


//すべての端末名を取得
void EcoNetLiteServer::GetAll(list<EcoNetLiteMap>* outRet)
{
	volatile lock_guard<mutex> al(this->lock);
	for(auto it = this->MappingList.begin() ; it!=this->MappingList.end() ; it++)
	{
		outRet->push_back( *(*it) );
	}
}

unsigned short EcoNetLiteServer::ParseTID(const EcoNetLiteData* data) const
{
	unsigned short r = data->tid[0] ;
	r = r << 8;
	r = r + data->tid[1];
	return r;
}
void EcoNetLiteServer::DoCallback(const EcoNetLiteData* data,const char* buffer,int size)
{
	const unsigned short tid = ParseTID(data);

	volatile lock_guard<mutex> al(this->lock);
	{
		for(auto it = this->TidWatch.begin() ; it != this->TidWatch.end(); )
		{
			if ( (*it)->tid == tid )
			{
				auto nextIT = it;
				nextIT++;
				(*it)->callback(data,buffer,size);
				this->TidWatch.erase(it);
				delete (*it);

				it = nextIT;
			}
			else
			{
				it++;
			}
		}
	}
}
