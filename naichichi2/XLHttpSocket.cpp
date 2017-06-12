#include "common.h"
#include "XLException.h"
#include "XLHttpSocket.h"
#include "XLUrlParser.h"
#include "XLGZip.h"

string XLHttpSocket::Get(const string& url)
{
	const map<string,string> header;
	return Get(url,header,60);
}

string XLHttpSocket::Get(const string& url,unsigned int timeout)
{
	const map<string,string> header;
	return Get(url,header,timeout);
}

string XLHttpSocket::Get(const string& url,const map<string,string> & header)
{
	return Get(url,header,60);
}

string XLHttpSocket::Get(const string& url,const map<string,string> & header,unsigned int timeout)
{
	vector<char> retBinary;
	XLHttpHeader retHeader;
	GetBinary(url,header,timeout,&retBinary,&retHeader);

	if (retBinary.empty()) return "";
	return string( &retBinary[0] , 0 , retBinary.size() );
}

string XLHttpSocket::Get(const string& url,const map<string,string> & header,unsigned int timeout,XLHttpHeader * retheader)
{
	vector<char> retBinary;
	GetBinary(url,header,timeout,&retBinary,retheader);

	if (retBinary.empty()) return "";
	return string( &retBinary[0] , 0 , retBinary.size() );
}



void XLHttpSocket::GetBinary(const string& url,const map<string,string> & header,unsigned int timeout,vector<char>* retBinary,	XLHttpHeader* retHeader)
{
	retBinary->clear();

	//URLパース
	XLUrlParser urlparse;
	urlparse.Parse(url);

	XLSocket socket;
	if ((urlparse.getProtocol() == "https"))
	{
		socket.CreateSSL(timeout);
	}
	else
	{
		socket.CreateTCP(timeout);
	}
	socket.Connect( urlparse.getHost() , urlparse.getPort() );

	//送信ヘッダーの準備
	string sendstring;
	sendstring = "GET " + urlparse.getPath() + " HTTP/1.0\r\n";
	for(auto headerit = header.begin() ; header.end() != headerit ; ++headerit)
	{
		sendstring += XLStringUtil::HeaderUpperCamel(headerit->first) + ": " + headerit->second + "\r\n";
	}
	if ( header.find("host") == header.end())
	{
		sendstring += "Host: " + urlparse.getHost() + "\r\n";
	}
	if ( header.find("connection") == header.end())
	{
		sendstring += "Connection: close\r\n";
	}
	if ( urlparse.IsAuthParam() && header.find("authorization") == header.end())
	{
		sendstring += "Authorization: Basic " + XLStringUtil::base64encode(urlparse.getUserName() + ":" + urlparse.getPassword()) + "\r\n";
	}
	sendstring += "\r\n";

	int ret = socket.Send(sendstring.c_str() ,sendstring.size() );
	if (ret < 0)
	{
		int err = socket.getErrorCode();
		throw XLException("ヘッダー送信中にエラー " + num2str(err) );
	}
	//結果の受信
	HTTPRecv( &socket , retBinary,retHeader);
}


string XLHttpSocket::Post(const string& url,const char * postBinaryData,unsigned int postBinaryLength)
{
	const map<string,string> header;
	return Post(url,header,60,postBinaryData,postBinaryLength);
}

string XLHttpSocket::Post(const string& url,unsigned int timeout,const char * postBinaryData,unsigned int postBinaryLength)
{
	const map<string,string> header;
	return Post(url,header,timeout,postBinaryData,postBinaryLength);
}

string XLHttpSocket::Post(const string& url,const map<string,string> & header,const char * postBinaryData,unsigned int postBinaryLength)
{
	return Post(url,header,60,postBinaryData,postBinaryLength);
}

string XLHttpSocket::Post(const string& url,const map<string,string> & header,unsigned int timeout,const char * postBinaryData,unsigned int postBinaryLength)
{
	vector<char> retBinary;
	XLHttpHeader retHeader;
	PostBinary(url,header,timeout,&retBinary,&retHeader,postBinaryData,postBinaryLength);

	if (retBinary.empty()) return "";
	return string( &retBinary[0] , 0 , retBinary.size() );
}

string XLHttpSocket::Post(const string& url,const map<string,string> & header,unsigned int timeout,const vector<char>& postBinaryData)
{
	vector<char> retBinary;
	XLHttpHeader retHeader;
	PostBinary(url,header,timeout,&retBinary,&retHeader,&postBinaryData[0],postBinaryData.size() );

	if (retBinary.empty()) return "";
	return string( &retBinary[0] , 0 , retBinary.size() );
}

string XLHttpSocket::Post(const string& url,const map<string,string> & header,unsigned int timeout,XLHttpHeader* retHeader,const vector<char>& postBinaryData)
{
	vector<char> retBinary;
	PostBinary(url,header,timeout,&retBinary,retHeader,&postBinaryData[0],postBinaryData.size() );

	if (retBinary.empty()) return "";
	return string( &retBinary[0] , 0 , retBinary.size() );
}

string XLHttpSocket::Post(const string& url,const map<string,string> & header,unsigned int timeout,XLHttpHeader* retHeader,const string& postString)
{
	vector<char> retBinary;
	PostBinary(url,header,timeout,&retBinary,retHeader,postString.c_str(),postString.size() );

	if (retBinary.empty()) return "";
	return string( &retBinary[0] , 0 , retBinary.size() );
}



void XLHttpSocket::PostBinary(const string& url,const map<string,string> & header,unsigned int timeout,vector<char>* retBinary,XLHttpHeader* retHeader,const char * postBinaryData,unsigned int postBinaryLength)
{
	retBinary->clear();

	//URLパース
	XLUrlParser urlparse;
	urlparse.Parse(url);

	XLSocket socket;
	if ( (urlparse.getProtocol() == "https") )
	{
		socket.CreateSSL(timeout);
	}
	else
	{
		socket.CreateTCP(timeout);
	}
	socket.Connect( urlparse.getHost() , urlparse.getPort() );

	//送信ヘッダーの準備
	string sendstring;
	sendstring = "POST " + urlparse.getPath() + " HTTP/1.0\r\n";
	for(auto headerit = header.begin() ; header.end() != headerit ; ++headerit)
	{
		sendstring += XLStringUtil::HeaderUpperCamel(headerit->first) + ": " + headerit->second + "\r\n";
	}
	if ( header.find("host") == header.end())
	{
		sendstring += "Host: " + urlparse.getHost() + "\r\n";
	}
	if ( header.find("connection") == header.end())
	{
		sendstring += "Connection: close\r\n";
	}
	if ( header.find("content-length") == header.end())
	{
		sendstring += "Content-Length: " + num2str(postBinaryLength) + "\r\n";
	}
	if ( header.find("content-type") == header.end())
	{
		sendstring += "Content-Type: application/x-www-form-urlencoded\r\n";
	}
	if ( urlparse.IsAuthParam() && header.find("authorization") == header.end())
	{
		sendstring += "Authorization: " + XLStringUtil::base64encode(urlparse.getUserName() + ":" + urlparse.getPassword()) + "\r\n";
	}
	sendstring += "\r\n";

	int ret;
	ret = socket.Send(sendstring.c_str() ,sendstring.size()   );
	if (ret < 0)
	{
		int err = socket.getErrorCode();
		throw XLException("ヘッダー送信中にエラー " + num2str(err) );
	}
	ret = socket.Send(postBinaryData     ,postBinaryLength   );
	if (ret < 0)
	{
		int err = socket.getErrorCode();
		throw XLException("POSTコンテンツ送信中にエラーー " + num2str(err) );
	}
	//結果の受信
	HTTPRecv( &socket , retBinary,retHeader);
}



void XLHttpSocket::HTTPRecv(XLSocket * socket , vector<char>* retBinary,XLHttpHeader* retHeader)
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
				int err = socket->getErrorCode();
				throw XLException("受信中にエラー " + num2str(err) );
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
	else
	{//content lengthがない
		while(1)
		{
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
}

