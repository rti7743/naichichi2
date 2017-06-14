//安いだけじゃないソケット!!
// XLSocket.cpp: XLSocket クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////
#include "common.h"
#include <string>
#include <assert.h>
#include <time.h>
#include "XLException.h"
#include "XLSocket.h"
#include "SystemMisc.h"
#include "MainWindow.h"


#if _MSC_VER
	//IPアドレスを取得するのに使う
	#include <iptypes.h>
	#include <iphlpapi.h>
	#pragma comment(lib, "ws2_32.lib")
	#pragma comment(lib, "IPHLPAPI.lib")
	#pragma comment(lib, "openssl.lib")
#else
	#include <sys/ioctl.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <net/if.h>
#endif


//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

XLSocket::XLSocket()
{
	this->Socket = INVALID_SOCKET;
	this->Connected = false;
	//SSL
	this->UseSSL = false;
	this->SSLContext = NULL;
	this->SSLhandle = NULL;
}

XLSocket::~XLSocket()
{
	if (this->Connected)
	{
		Shutdown();
	}
	if (this->Socket != INVALID_SOCKET)
	{
		Close();
	}
}

XLSocket::XLSocket(SOCKET inSocket)
{
	this->Socket = inSocket;
	this->Connected = true;
	this->UseSSL = false;

	this->SSLhandle = NULL;
	this->SSLContext = NULL;

}


void XLSocket::CreateLow( int type , int protocol ,int timeout)
{
	Close();
	 
	//ソケットを開く
	this->Socket = ::socket(AF_INET,type,protocol);
	if (this->Socket == INVALID_SOCKET)
	{
		throw XLEXCEPTION( XLException::StringWindows( WSAGetLastError() ) );
	}
	SetTimeout(timeout);
} 

void XLSocket::SetTimeout(int timeout)
{
#if _MSC_VER
    int32_t _timeout = timeout * 1000;
    setsockopt(this->Socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&_timeout, sizeof(_timeout));
    setsockopt(this->Socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&_timeout, sizeof(_timeout));
#else
    struct timeval tv;
    tv.tv_sec  = timeout; 
    tv.tv_usec = 0;         
    setsockopt(this->Socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(this->Socket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
#endif
} 

void XLSocket::CreateTCP(int timeout )
{
	CreateLow(SOCK_STREAM , 0,timeout);
}
void XLSocket::CreateUDP(int timeout  )
{
	CreateLow(SOCK_DGRAM , 0,timeout);
}
void XLSocket::CreateSSL(int timeout  )
{
	CreateLow(SOCK_STREAM , 0,timeout);
	this->UseSSL = true;
}


void XLSocket::Close()
{
	if (this->Socket == INVALID_SOCKET)
	{
		return ;
	}


	if (this->UseSSL)
	{

		if (this->SSLhandle)
		{
//				SSL_shutdown (this->SSLhandle);
				SSL_free (this->SSLhandle);
		}
		if (this->SSLContext)
		{
				SSL_CTX_free (this->SSLContext);
		}

	}

	::closesocket(this->Socket);
	this->Socket = INVALID_SOCKET;

}


int XLSocket::IoctlSocket(long inCmd , unsigned long * ioArgp)
{
	assert(this->Socket != INVALID_SOCKET);

	int ret =  ::ioctlsocket(this->Socket , inCmd , ioArgp);
	if (ret == SOCKET_ERROR )
	{
		throw XLEXCEPTION( XLException::StringWindows( WSAGetLastError() ) );
	}
	return ret;
}


int XLSocket::SetSockopt( int level, int optname,  const char * optval,  int optlen )
{
	assert(this->Socket != INVALID_SOCKET);

	return ::setsockopt(this->Socket , level , optname , optval , optlen);
}
int XLSocket::SetSockopt( int level, int optname,  int optval )
{
	assert(this->Socket != INVALID_SOCKET);

	return ::setsockopt(this->Socket , level , optname , (char *)&optval , sizeof(int));
}
int XLSocket::GetSockopt( int level, int optname,  char * optval,  int * optlen )
{
	assert(this->Socket != INVALID_SOCKET);
	
	return ::getsockopt(this->Socket , level , optname , optval , optlen);
}
int XLSocket::GetSockopt( int level, int optname,  int * optval )
{
	assert(this->Socket != INVALID_SOCKET);
	char buffer[20]={0};
	int optlen = sizeof(buffer)-1;
	int r = ::getsockopt(this->Socket , level , optname , buffer , &optlen);

	*optval = atoi(buffer);
	return r;
}



SOCKADDR_IN XLSocket::ToSockAddrIn(const string &inHost , int inPort, int sin_family) 
{
	SOCKADDR_IN	sai = {0};
	
	//IP?
	unsigned long connectIP = inet_addr( inHost.c_str() );
	if(connectIP==INADDR_NONE)
	{
		//名前のようなので解決してみる.
		HOSTENT	* hostEnt;
		hostEnt=gethostbyname(inHost.c_str() );
		if(!hostEnt)
		{
			throw XLEXCEPTION("ホスト名を解決できません" << XLException::StringWindows(WSAGetLastError() ) );
		}
		sai.sin_addr=*((IN_ADDR*)*hostEnt->h_addr_list);
	}
	else
	{
#if _MSC_VER
		sai.sin_addr.S_un.S_addr = connectIP;
#else
		sai.sin_addr.s_addr = connectIP;
#endif
	}

	sai.sin_port=htons((unsigned short)inPort);
	sai.sin_family=sin_family;

	return sai;
}

SOCKADDR_IN XLSocket::ToANYSockAddrIn(int inPort, int sin_family) 
{
	struct sockaddr_in server_address;
	server_address.sin_family=sin_family;
	server_address.sin_addr.s_addr=htonl(INADDR_ANY);
	server_address.sin_port=htons(inPort);
	
	return server_address;
}

SOCKADDR_IN XLSocket::ToSockAddrIn(unsigned long ip , int inPort, int sin_family) 
{
	SOCKADDR_IN	sai = {0};
	sai.sin_family			= sin_family;
	sai.sin_addr.s_addr	=   htonl(ip);
	sai.sin_port			= htons((unsigned short)inPort);

	return sai;
}

SOCKADDR XLSocket::ToSockAddr(const string &inHost , int inPort, int sin_family) 
{
	SOCKADDR_IN sai =  ToSockAddrIn(inHost,inPort,sin_family);
	return *((SOCKADDR*)(&sai));
}

SOCKADDR XLSocket::ToSockAddr(unsigned long ip , int inPort, int sin_family) 
{
	SOCKADDR_IN sai =  ToSockAddrIn(ip,inPort,sin_family);
	return *((SOCKADDR*)(&sai));
}

void XLSocket::Connect(const string &inHost , int inPort )
{
	assert(this->Socket != INVALID_SOCKET);
	assert(this->Connected == false);

	SOCKADDR_IN	sai = ToSockAddrIn(inHost,inPort);

	//接続を試みる.
	const int ret = connect(Socket,(SOCKADDR *)&sai,sizeof(SOCKADDR_IN));
	if( ret == SOCKET_ERROR )
	{	
		throw XLEXCEPTION("接続に失敗しました" << XLException::StringWindows( WSAGetLastError() ) );
	}

	this->Connected = true;

	if (this->UseSSL)
	{

		// Register the error strings for libcrypto & libssl
		//SSL_load_error_strings ();
		// Register the available ciphers and digests
		SSL_library_init ();

		// New context saying we are a client, and using SSL 2 or 3
		this->SSLContext = SSL_CTX_new (SSLv23_client_method ());
		if (this->SSLContext == NULL)
		{
			throw XLEXCEPTION("SSL_CTX_newに失敗しました");
		}

		// Create an SSL struct for the connection
		this->SSLhandle = SSL_new (this->SSLContext);
		if (this->SSLhandle  == NULL)
		{
			throw XLEXCEPTION("SSL_newに失敗しました");
		}

		// Connect the SSL struct to our connection
		if (!SSL_set_fd (this->SSLhandle, this->Socket))
		{
			throw XLEXCEPTION("SSL_set_fdに失敗しました");
		}

		// Initiate SSL handshake
		int r;
		r=SSL_connect (this->SSLhandle);
		if (r != 1)
		{
			throw XLEXCEPTION("SSL_connectに失敗しました");
		}

	}
}


void XLSocket::Bind(unsigned long inBindIP , int inPort)
{
	assert(this->Socket != INVALID_SOCKET);
	assert(this->Connected == false);

	SOCKADDR	sa = ToSockAddr(inBindIP,inPort);
	this->Bind( &sa );
}

void XLSocket::Bind(const string& inHost , int inPort)
{
	assert(this->Socket != INVALID_SOCKET);
	assert(this->Connected == false);

	SOCKADDR	sa = ToSockAddr(inHost,inPort);
	this->Bind( &sa );
}

void XLSocket::Bind(const SOCKADDR_IN * inSai)
{
	assert(this->Socket != INVALID_SOCKET);
	assert(this->Connected == false);

	Bind((const SOCKADDR*)inSai);
}

void XLSocket::Bind(const SOCKADDR * inSa)
{
	assert(this->Socket != INVALID_SOCKET);
	assert(this->Connected == false);

	int	Opt = 1;
	this->SetSockopt(SOL_SOCKET, SO_REUSEADDR, (const char*)&Opt , sizeof(int) );

	int ret = ::bind( this->Socket ,inSa , sizeof(SOCKADDR) );
	if (ret < 0)
	{
		throw XLEXCEPTION("bind に失敗しました" << XLException::StringWindows( WSAGetLastError() ) );
	}

	this->Connected  = true;
}

void XLSocket::Listen(int inBacklog )
{
	int ret = ::listen( this->Socket , inBacklog );
	if (ret < 0)
	{
		throw XLEXCEPTION("listen に失敗しました" << XLException::StringWindows( WSAGetLastError() ) );
	}
}

XLSocket* XLSocket::Accept()
{
	assert(Socket != INVALID_SOCKET);

	SOCKET  newSock = ::accept( this->Socket , (struct sockaddr*) NULL , NULL);
	if (newSock <= 0 )
	{
		throw XLEXCEPTION("accept に失敗しました" << XLException::StringWindows( WSAGetLastError() ) );
	}
	if (this->UseSSL)
	{

		SSL_accept(this->SSLhandle);

	}

	//新規に作成して返す
	return new XLSocket(newSock);
}

void XLSocket::Shutdown()
{
	if ( this->Socket == INVALID_SOCKET) return ;
	if ( this->Connected == false)	return ;

	::shutdown( this->Socket , 1 );

	//この時間になってもつないでいる場合は、即切る.
	time_t TimeOutTime = time(NULL) + 10; //10秒待つ.

	//保留中のすべてのデータを受け取る.
	int		ret;
	char	buffer[1024];
	while(1)
	{
		ret = ::recv( this->Socket , buffer , 1024 , 0 );
		
		if ( ret <= 0 || ret == SOCKET_ERROR)	break;

		//タイムアウトチェック
		if ( TimeOutTime < time(NULL) ) 
		{
			break;
		}
	}
	//データをこれ以上受信しないことを相手に伝える.
	::shutdown( this->Socket , 2);

	this->Connected = false;
}

int XLSocket::Send( const string&  str )
{
	return this->Send(str.c_str() , str.size() );
}

int XLSocket::Send( const char* inBuffer ,unsigned int inBufferLen  )
{
	unsigned int sended = 0;
	if (this->UseSSL)
	{

		while(1)
		{
			int ret = SSL_write( this->SSLhandle ,(void*) (inBuffer + sended), inBufferLen - sended);
			if (ret < 0)
			{
				return ret;
			}
			sended += ret;
//			printf("SSL_write %d\n",ret);

			if (sended >= inBufferLen) return (int)sended;
		}

	}
	else
	{
		while(1)
		{
			int ret = ::send( this->Socket , inBuffer + sended, inBufferLen - sended, 0);
			if (ret < 0)
			{
				return ret;
			}
			sended += ret;

			if (sended >= inBufferLen) return (int)sended;
		}
	}
	return (int)sended;
}

int XLSocket::Recv( char* outBuffer ,unsigned int inBufferLen )
{
	if (this->UseSSL)
	{
		return SSL_read( this->SSLhandle ,(void*) outBuffer, inBufferLen);
	}
	else
	{
		return ::recv( this->Socket , outBuffer , inBufferLen, 0);
	}
}
int XLSocket::SendTo(const char* inBuffer ,unsigned int inBufferLen , int inFlags ,struct sockaddr_in *senderinfo,int fromlen)
{
	return ::sendto(this->Socket,inBuffer,inBufferLen,inFlags,(struct sockaddr *)senderinfo, fromlen);
}

int XLSocket::RecvFrom(char* outBuffer ,unsigned int inBufferLen , int inFlags,struct sockaddr_in *senderinfo,int* fromlen )
{
	*fromlen = sizeof(struct sockaddr_in);
	return ::recvfrom(this->Socket,outBuffer,inBufferLen,inFlags,(struct sockaddr *)senderinfo, fromlen );
}

int XLSocket::getErrorCode() const
{
	return WSAGetLastError();
}
string XLSocket::what(int errorcode) const
{
	return XLException::StringWindows( errorcode );
}
string XLSocket::what() const
{
	return XLException::StringWindows( WSAGetLastError() );
}

//インターフェースがマルチキャストを受け取れるように設定する.
bool XLSocket::AddMulticastMembersip(const string& address) 
{
	bool ret = true;

	const auto interfaceMap = SystemMisc::GetIPAddressMap();
	for(auto it = interfaceMap.begin(); it != interfaceMap.end() ; ++it )
	{
		struct ip_mreq ssdpMcastAddr = {0};
		ssdpMcastAddr.imr_interface.s_addr = inet_addr( it->second.c_str() );
		ssdpMcastAddr.imr_multiaddr.s_addr = inet_addr( address.c_str() );
		if( setsockopt( this->getSocketHandle(), IPPROTO_IP, IP_ADD_MEMBERSHIP,
						( char * )&ssdpMcastAddr,
						sizeof( struct ip_mreq ) ) != 0 ) 
		{
			ERRORLOG("マルチキャスト " << address << " に interface " << it->second.c_str() << " を追加できません");
			ret = false;
			continue;
		}
		NOTIFYLOG("マルチキャスト " << address << " に interface " << it->second.c_str() << " を追加");
	}
	return ret;
}
