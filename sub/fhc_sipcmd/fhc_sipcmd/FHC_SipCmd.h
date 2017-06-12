// FHC_SipCmd.h: FHC_SipCmd クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FHC_SipCmd_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_FHC_SipCmd_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common.h"
#include <pjsua-lib/pjsua.h>

class FHC_SipCmd
{
public:

	FHC_SipCmd();
	virtual ~FHC_SipCmd();
	bool Create();
	void Stop();

	//認証
	void Auth(const std::string& id,const std::string& domain,const std::string& password);
	//有効かどうか
	bool IsEnable() const;
	//SIP URLの妥当性のチェック
	bool URLCheck(const std::string& sipurl) const;
	//電話を掛ける.
	void Call(const std::string& sipurl);
	//電話がかかってきているか？
	bool IsIncoming();
	//電話で話しているか？
	bool IsTalkingNow();
	//電話に出る
	void Answer();
	//電話を切る
	void Hangup();
	//SIP URLの抽出
	static std::string ParseSipAddress(const std::string& msg);

	void OnIncomingEvent(pjsua_call_id call_id,const pjsua_call_info& ci);
	void OnCallStateEvent(pjsua_call_id call_id,const pjsua_call_info& ci);
	void OnCallMediaStateEvent(pjsua_call_id call_id,const pjsua_call_info& ci);

private:
	//pjsip アカウントID
	pjsua_acc_id  SipAccID;
	//電話を受けたり、かけたりしたときの、通話ID
	pjsua_call_id SipCallID;
	//電話がかかってきて呼び出し中のID
	pjsua_call_id SipRingCallID;
	//ユーザのSIPアカウントのディフォルトドメイン.
	std::string SipDomain;
	//有効かどうか.
	bool isEnable;

	//sipの初期化
	bool SetupSIP();
};

#endif // !defined(AFX_FHC_SipCmd_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
