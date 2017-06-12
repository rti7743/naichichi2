// HttpServer.h: HttpServer クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HomeKitServer_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_HomeKitServer_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "XLSocket.h"
#include "XLHttpHeader.h"

class PHKNetworkIP;

//Homekitエミュレーションの実装
class HomeKitServer
{
    thread* HomeKitThread;
	string    PinCode;
	string    DeviceIdentity;

	PHKNetworkIP*  PHKNetwork;
public:

	HomeKitServer();
	virtual ~HomeKitServer();
	bool Create();
	void Stop();


	//PINCODEの再読み込み
	void RegeneratePinCode();
	//アクセサリーの再読み込み
	void ReloadAccessory(); 
	//アクセサリーの状態変更通知
	void NotifyAccessory();
	//Homekit内で家電状態を変えたときに、ブロードキャストしないようにバリアを立てる
	void setHomekitUpdateSelf(bool flag);

private:

	void HomeKitThreadMain();
	string MakeNewPinCode() const;
	string MakeNewDeviceIdentity() const;

	//アクセサリーの構築
	void BuildAccessory(); 
	//アクセサリーの追加
	void AddOneAccessory(int elecID,const string& type,const vector<string>& actions) ;

	bool StopFlag;
	bool HomekitUpdateSelf;
};


#endif // !defined(AFX_HomeKitServer_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
