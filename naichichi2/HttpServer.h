// HttpServer.h: HttpServer クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HttpServer_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_HttpServer_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "XLSocket.h"
#include "XLHttpHeader.h"

//インターフェース用にHTTPサーバを作ります.

enum WEBSERVER_RESULT_TYPE
{
	 WEBSERVER_RESULT_TYPE_OK_HTML
	,WEBSERVER_RESULT_TYPE_OK_JSON
	,WEBSERVER_RESULT_TYPE_OK_JSONP
	,WEBSERVER_RESULT_TYPE_OK_DATA
	,WEBSERVER_RESULT_TYPE_TRASMITFILE
	,WEBSERVER_RESULT_TYPE_DOWNLOADFILE
	,WEBSERVER_RESULT_TYPE_ERROR
	,WEBSERVER_RESULT_TYPE_NOT_FOUND
	,WEBSERVER_RESULT_TYPE_LOCATION
	,WEBSERVER_RESULT_TYPE_FORBIDDEN
};
class HttpLongPoolingInterface
{
public:
	HttpLongPoolingInterface(){};
	virtual ~HttpLongPoolingInterface(){};
	virtual bool Pooling(int count);
	virtual string getFulltext() const;
};


//仕事をするHTTP WORKER
class HttpWorker
{
	XLSocket* ConnectSocket;
    thread*					Thread;
	mutable mutex lock;
	mutable condition_variable	EventObject;
	bool	StopFlag;

public:
	HttpWorker();
	virtual ~HttpWorker();

	void Create();
	bool Post(XLSocket* socket);
	bool isEmpty();
	void Stop();
	void StopRequest();

private:
	void WorkThreadMain();
	void WorkHTTP1_1();
	void HTTP500();
	void HTTP404();
	void HTTP403();
	void HTTP403(const string& message);
	void HTTP302(const string& url,const string& cookieString);
	void HTTP200(const string& contents,const string& appendheaders,const XLHttpHeader& httpHeaders,const string& cookieString,bool isKeepAlive);
	void HTTP200SendFileContent(const string& urlpath,const string& mime,const XLHttpHeader& httpHeaders,const string& cookieString,bool isKeepAlive,bool isDownload);
	bool HttpTrans();
	bool HTTPRecvServerMode(XLSocket * socket , vector<char>* retBinary,XLHttpHeader* retHeader);
	
	bool IsResponsGZIP(size_t size,const XLHttpHeader& httpHeaders) const;
};


//HTTPサーバ
class HttpServer
{
    thread*					Thread;
public:

	HttpServer();
	virtual ~HttpServer();
	void Create();
	void Stop();

	string getAllowExtAndMime(const string& ext) const;
	string getWebURL(const string& path) const;

	//サーバのトップアドレス.
	string getServerTop() const;

	string getWebroot() const
	{
		return this->Webroot;
	}
	//cookie を処理する
	string procCookie(const XLHttpHeader& httpHeaders, const string& cookieNameID ,const string& host, map<string,string>* data ) ;
	//cookieのデータを上書きする.
	void updateCookie(const string& cookie , const string& host,const map<string,string>& data) ;
	//cookieを消す
	void delCookie(const string& cookie,const string& host) ;

	//cookie名を新規作成する
	string genCookie() const;

	string GetUUID() const;

private:
	//HTTPDサーバのメインスレッド
	void acceptThreadMain(int threadcount);

	XLSocket Socket;
	bool StopFlag;
	string Webroot;
	int Port;
	string SessionDir;
	time_t CookieGCTime;

	mutable mutex lock;
};


#endif // !defined(AFX_HttpServer_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
