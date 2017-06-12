// EcoNetLiteServer.h: EcoNetLiteServer クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EcoNetLiteServer_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_EcoNetLiteServer_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "XLSocket.h"
#include "XLHttpHeader.h"

#pragma pack(push)
#pragma pack(1)     
struct EcoNetLiteObjCode{
	unsigned char code[3];

	bool operator==(const EcoNetLiteObjCode& a) const
	{
		return 
			a.code[0] == this->code[0]
		&&	a.code[1] == this->code[1]
		&&	a.code[2] == this->code[2]
		;
	}
};

//単一データを送信するよう
struct EcoNetLiteData{
	unsigned char	header[2];	//0x10 0x81
	unsigned char	tid[2];	// 0x00 0x00
	EcoNetLiteObjCode	seoj;// 0x05 0xff 0x01 //送り先の機材名
	EcoNetLiteObjCode	deoj;// ?? ?? ?? //相手の機材名
	unsigned char	esv;	//サービス名 set1:60 setC:61 Get:62
	unsigned char	epc;	//01
	unsigned char	epc_1;	//サービス名 動作状態:80 
	unsigned char	pdc_1;	//01
	unsigned char	edt_1;	//動作 ON:30 OFF:31

	//1081 0000 013501 05ff01 72 01 80 01 30
};

#pragma pack(pop)

typedef std::function<void (size_t size,const char* buffer) > ECONETLITESERVER_TIDCALLBACK;

struct TIDWatch{
	unsigned short tid;
	ECONETLITESERVER_TIDCALLBACK callback;
};
struct EcoNetLiteMap{
	string ip;
	string identification; //識別番号
	bool is_anonymous;			//識別番号がとれないので適当なIDを降った場合
	EcoNetLiteObjCode deoj;

	list<unsigned char> setProp;
	list<unsigned char> getProp;
	list<TIDWatch> tidWatch;
};


class EcoNetLiteServer
{
    thread*					EcoNetLiteThread;
public:

	EcoNetLiteServer();
	virtual ~EcoNetLiteServer();
	void Create();
	void Stop();

	//ほかのノードを探すリクエスト送信.
	void sendSearchRequest();
	//すべての端末の取得
	void GetAll(list<EcoNetLiteMap>* outRet);
	//端末の取得 ID
	bool Get(const string& id,EcoNetLiteMap* outMap);
	bool Get(const string& id,EcoNetLiteMap* outMap,unsigned int timeoutSec);
	//端末の取得 ipと機器分類
	bool Get(const string& ip,const EcoNetLiteObjCode& deoj,EcoNetLiteMap* outMap);
	bool Get(const string& ip,const EcoNetLiteObjCode& deoj,EcoNetLiteMap* outMap,unsigned int timeoutSec);

	//リクエストの送信.UDPなので結果は不定.
	void sendSetRequest(const string& ip,const EcoNetLiteObjCode& deoj,unsigned char propUC,unsigned char valueUC);
	//値の取得 値が取れたらcallbackする
	void sendGetRequest(const string& ip,const EcoNetLiteObjCode& deoj,unsigned char propUC,ECONETLITESERVER_TIDCALLBACK& callback);
private:
	//受信のスレッドメイン
	void EcoNetLiteThreadMain();
	//端末の追加
	bool Add(const string& ip,const EcoNetLiteObjCode& deoj);
	bool UpdateSetProp(const string& ip,const EcoNetLiteObjCode& deoj,const list<unsigned char>& setprop);
	bool UpdateGetProp(const string& ip,const EcoNetLiteObjCode& deoj,const list<unsigned char>& getprop);
	bool UpdateIdentification(const string& ip,const EcoNetLiteObjCode& deoj,const string& id);

	//setできるプロパティを聞く
	void sendFindSetPropMapRequest(const string& ip,const EcoNetLiteObjCode& deoj);
	//getできるプロパティを聞く
	void sendFindGetPropMapRequest(const string& ip,const EcoNetLiteObjCode& deoj);
	//固有IDの取得
	void sendGetIdentificationRequest(const string& ip,const EcoNetLiteObjCode& deoj);

	void ParseResponsD6(const string& ip,const EcoNetLiteData* data,const char* buffer,int size);
	void ParseRespons9D(const string& ip,const EcoNetLiteData* data,const char* buffer,int size);
	void ParseRespons9E(const string& ip,const EcoNetLiteData* data,const char* buffer,int size);
	void ParseRespons83(const string& ip,const EcoNetLiteData* data,const char* buffer,int size);

	//固有IDが取れない場合、IPとかを使って適当なIDを作ります.
	string MakeAnonymousID(const string& ip,const EcoNetLiteObjCode& deoj);

	bool FindLow(const string& ip,const EcoNetLiteObjCode& deoj,EcoNetLiteMap** outM);

	unsigned short NextTID();
	bool AppendTIDCallback(const string& ip,const EcoNetLiteObjCode& deoj,unsigned short tid,ECONETLITESERVER_TIDCALLBACK& callback);


	XLSocket		EcoNetLiteSocket;

	bool StopFlag;

	//発見した機材
	list<EcoNetLiteMap*> MappingList;

	//次のTIDコード
	unsigned short TIDTotal;

	mutable mutex lock;
};



#endif // !defined(AFX_EcoNetLiteServer_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
