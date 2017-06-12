// SipServer.h: SipServer クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SipServer_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_SipServer_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XLPOpen2.h"


class SipServer
{
public:

	SipServer();
	virtual ~SipServer();
	bool Create();
	void Stop();

	//電話を掛ける.
	void Call(const string& tel);
	//電話がかかってきているか？
	bool IsIncoming();
	//電話で話しているか？
	bool IsTalkingNow();
	//電話に出る
	void Answer();
	//電話を切る
	void Hangup();

	//認証
	enum AUTH
	{
		 AUTH_PROCESSING_NOW
		,AUTH_BAD
		,AUTH_OK
	};
	AUTH getSipAuth() const;
	//SIP URLの構築
	string MakeSIPURL(const string& tel) const;
	//SIP URLの抽出
	string ParseSipAddress(const string& msg) const;
	//SIP URLをできるだけ簡略表記.
	string EllipsisSipURL(const string& msg) const;

	//通話中には音声認識を止めるかどうか
	int SipWhenTakingStopRecogn;

	//認証の是非が出るまで待つ.
	SipServer::AUTH WaitForAuth(int timeoutSec);
	//リクエストの応答がくるまで待つ
	bool WaitForRequest(int timeoutSec);

	//キーパッドを表示,非表示.
	void ShowNumpad(time_t now);
	void HideNumpad();
private:
	void SipThreadMain();
	void RecvRequest();
	void SendRequest(const string& cmd);

	//かけた電話に返事があった
	void OnCall(const string& msg);
	//電話がかかってきたと通知された
	void OnIncoming(const string& msg);
	//電話がかかってきた電話に出た
	void OnAnswer(const string& msg);
	//電話が切られた
	void OnHangup(const string& msg);
	//認証の結果
	void OnAuth(const string& msg);

	//認証
	void AuthRequest();

	//エラーが帰ってきたか？
	bool isErrorReslt(const string& msg) const;

	//sipはGPLのためlinkできない。
	//子プロセスとして起動する.
	XLPOpen2 sip;

	//ユーザのSIPアカウントのディフォルトドメイン.
	string SipDomain;
	//話し中
	bool isTalkingNow;
	//電話をかけて呼び出し中
	bool isCalling;
	//電話がかかってきている
	bool isIncoming;
	//電話がかかってきたアドレス
	string IncomingFrom;
	//電話をかけたアドレス
	string CallTo;
	//リクエスト処理中である
	bool isSendRequest;


	//エラーの理由
	string Error;
	//認証の状態
	AUTH Auth;

	bool StopFlag;
    thread*					SipThread;
	mutable mutex lock;
	mutable condition_variable	EventObject;
};

#endif // !defined(AFX_SipServer_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
