//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)

#include "common.h"
#include "HttpServer.h"
#include "MainWindow.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"
#include "XLGZip.h"
#include "XLHttpSocket.h"

HttpWorker::HttpWorker()
{
	this->ConnectSocket = NULL;
	this->Thread = NULL;
	this->StopFlag = false;
}
HttpWorker::~HttpWorker()
{
	this->Stop();
}

void HttpWorker::Create()
{
	this->StopFlag = false;

	this->Thread = new thread([=](){
		this->WorkThreadMain(); 
	});
}
bool HttpWorker::Post(XLSocket* socket)
{
	ASSERT(this->ConnectSocket == NULL);
	if (this->StopFlag)
	{//終了しようというのに仕事を積むのかよ
		return false;
	}

	{
		lock_guard<mutex> al(this->lock);
		if ( this->ConnectSocket)
		{//別の仕事が急きょ入った場合
			return false;
		}

		//仕事を与える.
		this->ConnectSocket = socket;
		
		//起きろ!
		this->EventObject.notify_one();
	}
	return true;
}
void HttpWorker::StopRequest()
{
	lock_guard<mutex> al(this->lock);

	this->StopFlag = true;
	if (this->ConnectSocket)
	{
		this->ConnectSocket->Close();
	}


	//起きろ!
	this->EventObject.notify_one();
}


bool HttpWorker::isEmpty()
{//NULLなら仕事なしで暇.
	return this->ConnectSocket == NULL;
}

void HttpWorker::Stop()
{
	if (this->Thread)
	{
		this->StopRequest();
		this->Thread->join();
		
		delete this->Thread;
		this->Thread = NULL;
	}
}


bool HttpWorker::IsResponsGZIP(size_t size,const XLHttpHeader& httpHeaders) const
{
	if (size<=512) return false; //短すぎる応答は圧縮しない.

	//圧縮していいよとクライアントはいっているか？
	const string accept_encoding = httpHeaders.getAt("accept-encoding");	
	if (accept_encoding.find("gzip") != string::npos )
	{//クライアントは圧縮してもいいと言っている
		return true;
	}

	//クライアントは圧縮を望んでいない!
	return false;
}

void HttpWorker::HTTP500()
{
	const string header = 
		"HTTP/1.0 500 Internal Server Error\r\n"
		"Server:naichichi2\r\n"
		"Connection: close\r\n"
		"Content-Length: 0\r\n"
		"Cache-Control: no-cache\r\n"
		"\r\n";
	this->ConnectSocket->Send(header);
}
void HttpWorker::HTTP404()
{
	const string header = 
		"HTTP/1.0 404 Not Found\r\n"
		"Server:naichichi2\r\n"
		"Connection: close\r\n"
		"Content-Length: 0\r\n"
		"Cache-Control: no-cache\r\n"
		"\r\n";
	this->ConnectSocket->Send(header);
}

void HttpWorker::HTTP403()
{
	const string header = 
		"HTTP/1.0 403 Forbidden\r\n"
		"Server:naichichi2\r\n"
		"Connection: close\r\n"
		"Content-Length: 0\r\n"
		"Cache-Control: no-cache\r\n"
		"\r\n";
	this->ConnectSocket->Send(header);
}

void HttpWorker::HTTP403(const string& message)
{
	const string header = 
		"HTTP/1.0 403 Forbidden\r\n"
		"Server:naichichi2\r\n"
		"Connection: close\r\n"
		"Content-Length: " + num2str(message.size()) + "\r\n"
		"Cache-Control: no-cache\r\n"
		"\r\n";
	this->ConnectSocket->Send(header);
	this->ConnectSocket->Send(message);
}



void HttpWorker::HTTP302(const string& url,const string& cookieString)
{
	string header = 
		"HTTP/1.0 302 Found\r\n"
		"Server:naichichi2\r\n"
		"Connection: close\r\n"
		"Content-Length: 0\r\n"
		"Cache-Control: no-cache\r\n";
	if (!cookieString.empty())
	{
		header += "Set-Cookie: naichichi2=" + cookieString + "; expires=Fri, 01-Jan-2038 00:00:00 GMT; path=/\r\n" ;
	}
	header += "Location: " + XLStringUtil::clearcontrollcode(url) + "\r\n"
			  "\r\n";
	this->ConnectSocket->Send(header);
}

void HttpWorker::HTTP200(const string& contents,const string& appendheaders,const XLHttpHeader& httpHeaders,const string& cookieString,bool isKeepAlive)
{
	string sheader = 
		"HTTP/1.0 200 OK\r\n"
		"Server:naichichi2\r\n"
		"X-Frame-Options: SAMEORIGIN\r\n"
		;

	if (!cookieString.empty())
	{
		sheader += "Set-Cookie: naichichi2=" + cookieString + "; expires=Fri, 01-Jan-2038 00:00:00 GMT; path=/\r\n" ;
	}
	if (isKeepAlive)
	{//keepaliveする
		sheader +=	"Connection: keep-alive\r\n"
					"Keep-Alive: timeout=5, max=100\r\n"
					;
	}
	else
	{//keepaliveしない
		sheader +=	"Connection: close\r\n";
	}
	if (!appendheaders.empty())
	{
		sheader += appendheaders ;
	}

	if ( IsResponsGZIP(contents.size(),httpHeaders) )
	{
		XLGZip gzip;
		if ( gzip.gzip(&contents[0],contents.size()) )
		{
			sheader += "Content-Length: " + num2str(gzip.getSize()) + "\r\n"
					  "Content-Encoding: gzip\r\n"
					  "\r\n";
			this->ConnectSocket->Send(sheader);
			this->ConnectSocket->Send(gzip.getData(),gzip.getSize());
			return ;
		}
	}

	sheader += "Content-Length: " + num2str(contents.size()) + "\r\n"
			  "\r\n";

	this->ConnectSocket->Send(sheader);
	this->ConnectSocket->Send(contents);
}

	
void HttpWorker::HTTP200SendFileContent(const string& realpath,const string& mime,const XLHttpHeader& httpHeaders,const string& cookieString,bool isKeepAlive,bool isDownload)
{
	//アクセスが許可されている拡張子か？
	ASSERT(!mime.empty());
	ASSERT(XLFileUtil::Exist(realpath));

	//更新されているか？
	const time_t unixtime = XLFileUtil::getfiletime(realpath);
	const string ifmodifiedSince = httpHeaders.getAt("if-modified-since");
	if (! ifmodifiedSince.empty())
	{
		time_t ifmodifiedSinceTime = XLStringUtil::HttpTimeToUnixTime( ifmodifiedSince );
		if (ifmodifiedSinceTime >= unixtime)
		{
			string header =	"HTTP/1.0 304 Not Modified\r\n";
			if (isKeepAlive)
			{//keepaliveする
				header +=	"Connection: keep-alive\r\n"
							"Keep-Alive: timeout=5, max=100\r\n"
							;
			}
			else
			{//keepaliveしない
				header +=	"Connection: close\r\n";
			}
			if (isDownload)
			{
				const string filename = XLStringUtil::basename(realpath);
				if ( !filename.empty() && XLStringUtil::checkSafePath(filename) )
				{
					header +=	"Content-Disposition: inline; filename=" + filename + "\r\n";
				}
			}
			header +=	"Server:naichichi2\r\n"
						"X-Frame-Options: SAMEORIGIN\r\n"
						"Content-Length: 0\r\n"
						"\r\n";
			this->ConnectSocket->Send(header);
			return ;
		}
	}
	//ファイルを読み込む
	vector<char> filebinary = XLFileUtil::cat_b(realpath);

	//とりあえずさきっちょだけ
	string sheader =	"HTTP/1.0 200 OK\r\n";

	if (isKeepAlive)
	{//keepaliveする
		sheader +=	"Connection: keep-alive\r\n"
					"Keep-Alive: timeout=5, max=100\r\n"
					;
	}
	else
	{//keepaliveしない
		sheader +=	"Connection: close\r\n";
	}

	sheader +=	"Server:naichichi2\r\n"
				"Last-Modified: " + XLStringUtil::UnixTimeToHttpTime( unixtime ) + "\r\n" +
				"Content-Type: " + mime + "\r\n"
				;

	//ダウンロードするの？
	if (isDownload)
	{
		const string filename = XLStringUtil::basename(realpath);
		if ( !filename.empty() && XLStringUtil::checkSafePath(filename) )
		{
			sheader += "Content-Disposition: inline; filename=" + filename + "\r\n";
		}
	}

	//圧縮してもいい？
	if ( IsResponsGZIP(filebinary.size(),httpHeaders) )
	{//圧縮する
		XLGZip gzip;
		if ( gzip.gzip(&filebinary[0],filebinary.size()) )
		{
			sheader +=	"Content-Length: " + num2str(gzip.getSize()) + "\r\n"
						"Content-Encoding: gzip\r\n"
						"\r\n";

			this->ConnectSocket->Send(sheader);
			this->ConnectSocket->Send(gzip.getData(),gzip.getSize());
			return ;
		}
	}

	//圧縮しない場合
	sheader +=	"Content-Length: " + num2str(filebinary.size()) + "\r\n"
				"\r\n";

	this->ConnectSocket->Send(sheader);
	this->ConnectSocket->Send(&filebinary[0],filebinary.size());
	return ;
}

void HttpWorker::WorkThreadMain()
{
	while(!this->StopFlag)
	{
		//寝る
		{
			unique_lock<mutex> al(this->lock);
			this->EventObject.wait(al);
		}
		if (this->StopFlag)
		{//停止命令らしい.
			break;
		}

		//具体的な処理 HTTP/1.1 を実装する
		WorkHTTP1_1();

		//切断と解放
		{
			lock_guard<mutex> al(this->lock);

			this->ConnectSocket->Close();
			delete this->ConnectSocket;
			this->ConnectSocket = NULL;
		}
	}
}

void HttpWorker::WorkHTTP1_1()
{
	ASSERT(this->ConnectSocket);
	//タイムアウト10秒
	this->ConnectSocket->SetTimeout(10);

	bool doContinue = false;

	do
	{
		if( this->StopFlag)
		{//サーバ停止命令
			break;
		}

		try
		{
			doContinue = HttpTrans();
		}
		catch(XLException& e)
		{
			doContinue = false;
			ERRORLOG("HTTPワーカー実行中に例外が投げられました。 " << e.what());
		}

		//パイプライン転送を行います
	}
	while(doContinue);

}

bool HttpWorker::HTTPRecvServerMode(XLSocket * socket , vector<char>* retBinary,XLHttpHeader* retHeader)
{
	vector<char> buffermalloc(2048);
	const unsigned int buffersize = buffermalloc.size() - 1;
	char * buffer = &buffermalloc[0];

	//まずヘッダーのパース
	{
		unsigned int total_recv_size = 0;

		const int HEADER_RETRY_MAX = 10;
		int i = 0;
		for( ; i < HEADER_RETRY_MAX ; i++)
		{
			int size = socket->Recv(buffer+total_recv_size,buffersize-total_recv_size);
			if (size < 0)
			{
				return false;
			}
			buffer[size] = '\0';
			total_recv_size += size;
			if ( retHeader->Parse(buffer) )
			{
				break;
			}
			if(total_recv_size >= buffersize)
			{
				throw XLException("ヘッダがあまりにも長すぎます" );
			}
		}
		if (i >= HEADER_RETRY_MAX)
		{
			throw XLException("ヘッダーパース中にエラーが発生しました");
		}
		
		//ヘッダーとボディを分離する
		retBinary->insert(retBinary->end() ,buffer + retHeader->getHeaderSize() ,buffer + total_recv_size);
	}

	//まだボディが続く場合は読み込む
	const map<string,string>* header = retHeader->getHeaderPointer();
	map<string,string>::const_iterator it = header->find("content-length");
	if (it != header->end())
	{//content lengthがある
		unsigned int content_length = atoi(it->second);
		while(1)
		{
			if (retBinary->size() >= content_length)
			{//content-lengthだけ受信したら終了
				break;
			}
			int size = socket->Recv(buffer,buffersize);
			if (size == 0)
			{
				break;
			}
			else if (size < 0)
			{
				int err = socket->getErrorCode();
				throw XLException("受信中にエラーが発生しました err:"+ num2str(err) );
			}

			retBinary->insert(retBinary->end() ,buffer,buffer +size );
		}
	}

	if ( retHeader->getAt("content-encoding").find("gzip") != string::npos )
	{//gzip圧縮されているらしいよ!
		XLGZip zip;
		
		if ( zip.gunzip(&((*retBinary)[0]),retBinary->size() ) )
		{//gzip解凍できたから、バッファを入れ替える
			retBinary->clear();
			retBinary->insert(retBinary->end() ,zip.getData() , zip.getData() + zip.getSize() );
		}
	}
	return true;
}

bool HttpWorker::HttpTrans()
{
	XLHttpHeader httpHeaders;
	vector<char> body;

	//ヘッダとボディの取得.
	bool r = HTTPRecvServerMode(this->ConnectSocket,&body,&httpHeaders);
	if(!r)
	{//timeout
		DEBUGLOG("socket timeout");
		return false;
	}

	//パスはよく使うので変数に入れるよ
	const string path = httpHeaders.getRequestPath();
	if ( strstr(path.c_str() , "/../") ) 
	{//..がくることはないっ
		HTTP403();
		NOTIFYLOG("403\t" << path << "\t\tパスに .. が入ってはいけません");
		return false;
	}

	//コンテンツを受け取る場合
	if (httpHeaders.getRequestMethod() == "POST" && body.size() >= 1)
	{
		httpHeaders.PostParse(&body[0],body.size() );
	}

	//http/1.0 keep aliveできかな？
	bool isKeepAlive = false;
	if ( "keep-alive" == httpHeaders.getAt("connection") )
	{
		isKeepAlive = true;
	}

	//host を見る
	const string host = httpHeaders.getHostHeader();
	//実ファイルがあって、アクセスが許可されているならば、そいつを優先して転送する
	const string realpath = MainWindow::m()->GetConfigBasePath("webroot" + path);

	//存在してアクセスが許可されている拡張子か？
	if ( XLFileUtil::Exist(realpath) )
	{
		string mime = MainWindow::m()->Httpd.getAllowExtAndMime( XLStringUtil::strtolower(XLStringUtil::baseext_nodot(path)) );
		if (!mime.empty())
		{//実ファイルを転送する
			HTTP200SendFileContent(realpath,mime,httpHeaders,"",isKeepAlive,false);
			return isKeepAlive;
		}
		else
		{
			DEBUGLOG("mimeが許可されていません\t" << path );
		}
	}
	//cookieの読み込み
	map<string,string> cookieArray,orignalCookieArray;
	string cookieString = MainWindow::m()->Httpd.procCookie(httpHeaders,"naichichi2", host,&cookieArray);
	orignalCookieArray = cookieArray;

//	DEBUGLOG("httpd 処理系\t" << path << "\tにアクセスします");
	
	//処理系に投げる
	string responsString;
	WEBSERVER_RESULT_TYPE result = WEBSERVER_RESULT_TYPE_NOT_FOUND;
	if (0)
	{
	}
#if WITH_SERVER_ONLY_CODE==1
#endif //WITH_SERVER_ONLY_CODE
#if WITH_CLIENT_ONLY_CODE==1
	else
	{
		//ユーザープロセス
		if (!  MainWindow::m()->ScriptManager.WebAccess(path,httpHeaders,&cookieArray,&result,&responsString) )
		{//存在しないので 404
				HTTP404();
				NOTIFYLOG("404\t" << path << "\t" << cookieString << "\tuser処理系に存在しないパスです");
				return false;
		}
	}
#endif //WITH_CLIENT_ONLY_CODE
//	DEBUGLOG("httpd 処理系\t" << path << "\tにアクセスしました result:" << result);

	//セッションをリネームする指示が出ていたら適応する.
	if ( cookieArray.find("__regen__") != cookieArray.end() )
	{
		NOTIFYLOG("cookie __regen__! " << cookieString);

		//消して
		MainWindow::m()->Httpd.delCookie(cookieString,host);

		//作りなおす
		cookieString = MainWindow::m()->Httpd.genCookie();
		MainWindow::m()->Httpd.updateCookie(cookieString,host,cookieArray);

		NOTIFYLOG("cookie __regen__! new -> " << cookieString);
	}
	else if ( cookieArray.find("__delete__") != cookieArray.end() )
	{//クッキー削除
		NOTIFYLOG("cookie __delete__!" << cookieString);

		MainWindow::m()->Httpd.delCookie(cookieString,host);
		cookieString = "";
	}
	else
	{
		if (cookieArray.empty() || !XLStringUtil::CompareStringMap(cookieArray,orignalCookieArray) )
		{//データがない場合はクッキーを作らない
		}
		else
		{//データがあるからクッキーを作る
			if (cookieString.empty())
			{//クッキーの名前がないなら作る
				cookieString = MainWindow::m()->Httpd.genCookie();
			}
			//セッションを上書きする.
			MainWindow::m()->Httpd.updateCookie(cookieString,host,cookieArray);
		}
	}

	switch(result)
	{
	case WEBSERVER_RESULT_TYPE_OK_HTML:
		HTTP200(responsString,"Content-type: text/html; charset=utf-8\r\n",httpHeaders,cookieString,isKeepAlive);
		return false;
	case WEBSERVER_RESULT_TYPE_OK_JSON:
		HTTP200(responsString,"Content-Type: application/json; charset=utf-8\r\nCache-Control: no-cache\r\n",httpHeaders,cookieString,isKeepAlive);
		return false;
	case WEBSERVER_RESULT_TYPE_OK_JSONP:
		{
			const auto request = httpHeaders.getRequest();
			responsString = mapfind(request,"callback","callback") + "(" +  responsString + ");";
			HTTP200(responsString,"Content-Type: text/javascript; charset=utf-8\r\nCache-Control: no-cache\r\nAccess-Control-Allow-Origin: *\r\n",httpHeaders,cookieString,isKeepAlive);
		}
		return false;
	case WEBSERVER_RESULT_TYPE_OK_DATA:
		HTTP200(responsString,"Cache-Control: no-cache\r\n",httpHeaders,cookieString,isKeepAlive);
		return false;
	case WEBSERVER_RESULT_TYPE_TRASMITFILE:
	case WEBSERVER_RESULT_TYPE_DOWNLOADFILE:
		{
			responsString = XLStringUtil::pathseparator(responsString);
			//存在してアクセスが許可されている拡張子か？
			if (responsString.empty() || !XLFileUtil::Exist(responsString) )
			{
				HTTP404();
				NOTIFYLOG("404\t" << path << "\t" << cookieString << "\tコンテンツ転送(transmit)しようとしたがファイルがありません。" << responsString);
				return false;
			}
			const string mime = MainWindow::m()->Httpd.getAllowExtAndMime( XLStringUtil::strtolower(XLStringUtil::baseext_nodot(responsString)) );
			if (mime.empty())
			{
				HTTP403();
				NOTIFYLOG("403\t" << path << "\t" << cookieString << "\tコンテンツ転送(transmit)しようとしたが許可されていないmimeでした。" << responsString);
				return false;
			}

			//実ファイルを転送する
			const bool isDownload = (result == WEBSERVER_RESULT_TYPE_DOWNLOADFILE);
			HTTP200SendFileContent(responsString,mime,httpHeaders,cookieString,isKeepAlive,isDownload);
		}
		break;
	case WEBSERVER_RESULT_TYPE_ERROR:
		HTTP500();
		return false;
	case WEBSERVER_RESULT_TYPE_LOCATION:
		HTTP302(responsString,cookieString);
		return false;
	case WEBSERVER_RESULT_TYPE_NOT_FOUND:
		HTTP404();
		return false;
	case WEBSERVER_RESULT_TYPE_FORBIDDEN:
		HTTP403(responsString);
		return false;
	default:
		return false;
	};

	return isKeepAlive;
}

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
HttpServer::HttpServer()
{
	this->Thread = NULL;
	this->StopFlag = false;
}
HttpServer::~HttpServer()
{
	DEBUGLOG("stop...");

	Stop();

	DEBUGLOG("done");
}


void HttpServer::Create()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	this->Webroot = MainWindow::m()->Config.Get("httpd__webroot", MainWindow::m()->GetConfigBasePath("webroot") );
	this->Port = MainWindow::m()->Config.GetInt("httpd__port",80);
	this->SessionDir = MainWindow::m()->Config.Get("httpd__sessiondir",MainWindow::m()->GetConfigBasePath("session"));
	this->CookieGCTime = MainWindow::m()->Config.GetInt("httpd__cookie_gctime",(24*60*60*90));
	int threadcount = MainWindow::m()->Config.GetInt("httpd__workerthread",50);

	DEBUGLOG("httpd acceptor...");

	//acceptスレッド作成.
	DEBUGLOG("httpd worker thread...");
	this->Thread = new thread([=](){
		this->acceptThreadMain(threadcount); 
	});
	
}

void HttpServer::Stop()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	this->StopFlag = true;
	this->Socket.Close();

	if (this->Thread)
	{
		this->Thread->join();
		delete this->Thread;
		this->Thread = NULL;
	}
}

void HttpServer::acceptThreadMain(int threadcount)
{
	this->Socket.CreateTCP(0); //タイムアウトしない
	this->Socket.Bind(INADDR_ANY,this->Port);
	this->Socket.Listen();

	HttpWorker* HttpWorkers = new HttpWorker[threadcount];
	for(int i = 0 ; i < threadcount ; i++)
	{
		HttpWorkers[i].Create();
	}
	while (!this->StopFlag)
	{
		//accept のエラーをチェックしていないで、 終了時の close()すると例外で死ぬ。
		XLSocket* socket = this->Socket.Accept();

		if (this->StopFlag)
		{
			delete socket;
			break;
		}

		int i ;
		for(i = 0 ; i < threadcount ; i ++)
		{
			if( HttpWorkers[i].isEmpty() )
			{
				//仕事を依頼
				if ( ! HttpWorkers[i].Post(socket) )
				{//ごめん、別の仕事が急きょ入った! 他を当たって!
					continue;
				}
				break;
			}
		}
		if (i >= threadcount)
		{//スレッドが満杯!
			delete socket;
			ERRORLOG("スレッドが満杯で受け取れませんでした");
			continue;
		}
	}
	for(int i = 0 ; i < threadcount ; i++)
	{//停止命令
		HttpWorkers[i].StopRequest();
	}
	for(int i = 0 ; i < threadcount ; i++)
	{//止まらさせる.
		HttpWorkers[i].Stop();
	}
}


string HttpServer::GetUUID() const
{
	return MainWindow::m()->Config.Get("httpd__uuid", "" );
}

string HttpServer::getAllowExtAndMime(const string& ext) const
{
	if (ext == "jpg") return "image/jpeg";
	if (ext == "png") return "image/png";
	if (ext == "gif") return "image/gif";
	if (ext == "ico") return "image/ico";
	if (ext == "css") return "text/css";
	if (ext == "html") return "text/html";
	if (ext == "js") return "text/javascript";
	if (ext == "swf") return "application/x-shockwave-flash";
	if (ext == "mp3") return "audio/mp3";
	if (ext == "ogg") return "audio/ogg";
	if (ext == "wav") return "audio/wave";
	if (ext == "reset") return "application/x-naichichi2";
	if (ext == "zip") return "application/zip";
	if (ext == "gz") return "application/gz";
	return "";
}

//続きはwebで
string HttpServer::getWebURL(const string& path) const
{
	string url = this->getServerTop() ;

	if (path.empty())        url += "/";
	else if (path[0] != '/') url += "/" + path;
	else url += path;

	return url;
}

//サーバのトップアドレス.
string HttpServer::getServerTop() const
{
	return MainWindow::m()->Config.Get("httpd__url", "http://127.0.0.1:" + num2str(this->Port ) );
}

//cookie を処理する
string HttpServer::procCookie(const XLHttpHeader& httpHeaders, const string& cookieNameID , const string& host, map<string,string>* data ) 
{
	volatile lock_guard<mutex> al(this->lock);
	 
	const string cookie = httpHeaders.getCookieHeader(cookieNameID);
	if (cookie.empty() )
	{
//		DEBUGLOG("クッキー:" << cookie << "が存在しません");
//		DEBUGLOG("ヘッダー情報" << httpHeaders.Build() );

		return "";
	}


	//データを消す
	data->clear();

	const time_t now = time(NULL);

	// 1/4096 のタイミングで GCを動かす
	if ( (now % 4096) == 0)
	{
		DEBUGLOG("クッキーのGCを動かします");

		time_t gctime = now;
		//指定時間より前のものを消す
		if (gctime > this->CookieGCTime) gctime = gctime - this->CookieGCTime;
		else						     gctime = 0;

		XLFileUtil::findfile(this->SessionDir , [&](const string& filename,const string& fullfilename) -> bool {
			const time_t t = XLFileUtil::getfiletime(fullfilename);
			if (t < gctime )
			{
				DEBUGLOG("GCでクッキーを消します: " << fullfilename );
				XLFileUtil::del(fullfilename);
			}

			return true;
		});
	}
	//cookie名の検証
	if ( ! (XLStringUtil::checkSafePath(cookie) && XLStringUtil::checkSafePath(host)) )
	{
		DEBUGLOG("クッキー:" << cookie << "は危険な名前です");
		DEBUGLOG("ヘッダー情報" << httpHeaders.Build() );
		return "";
	}

	const auto filename = host + "_" + cookie;
	const auto fullname = XLStringUtil::pathseparator(this->SessionDir + "/" + filename);
	auto fp = fopen(fullname.c_str(),"rb");
	if (!fp)
	{
		DEBUGLOG("クッキー:" << cookie << " file:(" << filename << ")が存在しません");
		DEBUGLOG("ヘッダー情報" << httpHeaders.Build() );
		return "";
	}

	vector<char> key(10000);
	vector<char> value(10000);
	while(!feof(fp))
	{
		unsigned short keysize = 0;
		(void) fread(&keysize,sizeof(unsigned short),1,fp);
		if (keysize<= 0||keysize>=10000) break;
		(void) fread(&key[0],sizeof(char),keysize,fp);
		key[keysize] = 0;

		unsigned short valuesize = 0;
		(void) fread(&valuesize,sizeof(unsigned short),1,fp);
		if (valuesize>=10000) break;
		(void) fread(&value[0],sizeof(char),valuesize,fp);
		value[valuesize] = 0;

		const string kk = &key[0];
		const string vv = &value[0];

		data->insert( pair<string,string>(kk,vv));
	}
	fclose(fp);

	//クッキーの時間管理をアップデート.
	XLFileUtil::setfiletime(fullname,now);

	return cookie;
}

//cookieのデータを上書きする.
void HttpServer::updateCookie(const string& cookie , const string& host,const map<string,string>& data) 
{
	volatile lock_guard<mutex> al(this->lock);

	//cookie名の検証
	ASSERT( XLStringUtil::checkSafePath(cookie) );  //cookie取るときに確認しているはず
	ASSERT( XLStringUtil::checkSafePath(host) );    //host取るときに確認しているはず


	const auto filename = host + "_" + cookie;
	const auto fullname = XLStringUtil::pathseparator(this->SessionDir + "/" + filename);
	const auto fullnameTemp = fullname + ".tmp";
	{
		auto fp = fopen(fullnameTemp.c_str(),"wb");
		if (!fp)
		{
			ERRORLOG("クッキー:" << fullnameTemp << "を保存出来ません");
			return ;
		}

		for(auto it = data.begin() ; it != data.end() ; ++it)
		{
			if (it->first == "__regen__") continue;
			if (it->first == "__delete__") continue;

			unsigned short keysize = it->first.size();
			unsigned short valuesize = it->second.size();
			if (keysize <= 0 || valuesize <= 0) continue;

			fwrite(&keysize,sizeof(unsigned short),1,fp);
			fwrite(it->first.c_str(),sizeof(char),keysize,fp);
			fwrite(&valuesize,sizeof(unsigned short),1,fp);
			fwrite(it->second.c_str(),sizeof(char),valuesize,fp);
		}
		fclose(fp);

		//正規の名前にリネームする
		XLFileUtil::copy(fullnameTemp,fullname);
		XLFileUtil::del( fullnameTemp );
	}
}

//cookieを消す
void HttpServer::delCookie(const string& cookie,const string& host) 
{
	volatile lock_guard<mutex> al(this->lock);

	if (cookie.empty() ) return ;
	ASSERT( XLStringUtil::checkSafePath(cookie) );  //cookie取るときに確認しているはず
	ASSERT( XLStringUtil::checkSafePath(host) );    //host取るときに確認しているはず

	const auto filename = host + "_" + cookie;
	const auto fullname = XLStringUtil::pathseparator(this->SessionDir + "/" + filename);
	{
		XLFileUtil::del(fullname);
	}
}

//cookie名を新規作成する
string HttpServer::genCookie() const
{
	return XLStringUtil::randsting(32) + "_" + num2str(time(NULL));
}

