// XLSocket.h: XLSocket クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XLSOCKET_H__137F6EB5_32A1_46CD_9CA7_EC6E9C6A6E6A__INCLUDED_)
#define AFX_XLSOCKET_H__137F6EB5_32A1_46CD_9CA7_EC6E9C6A6E6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if _MSC_VER
	#include <Winsock2.h>
	#include <mswsock.h>

#else

	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
    #include <sys/ioctl.h>

	typedef int SOCKET;
	typedef struct sockaddr_in SOCKADDR_IN;
	typedef struct sockaddr    SOCKADDR;
	typedef struct hostent     HOSTENT;
	typedef struct in_addr     IN_ADDR;
	const int INVALID_SOCKET = -1;
	const int SOCKET_ERROR = -1;
	#define WSAGetLastError() (errno)
	#define closesocket(p)    close(p)
	#define ioctlsocket(p,a,b)    ioctl((p),(a),(b))


#endif
	#include <openssl/crypto.h>
	#include <openssl/ssl.h>
	#include <openssl/err.h>
	#include <openssl/rand.h>



class XLSocket  
{
public:
	/**
	 * XLSocket:	ソケット作成
	 *
	 * @return  
	 */
	XLSocket();
	virtual ~XLSocket();

	/**
	 * XLSocket:		作成済みソケットの割り当て.
	 *
	 * @param inSocket 
	 * @return  
	 */
	XLSocket(SOCKET inSocket);
	void SetTimeout(int timeout);


	/**
	 * Create:	ソケットの作成
	 *
	 * @param type	ソケットの種類	SOCK_STREAM : TCP
	 *								SOCK_DGRAM : UDP
	 * @return void 
	 */
	void CreateLow( int type , int protocol ,int timeout = 60 );
	void CreateTCP(int timeout = 60 );
	void CreateUDP(int timeout = 60  );
	void CreateSSL(int timeout = 60  );
	
	/**
	 * Close:	ソケットを閉じる
	 *
	 * @return void 
	 */
	void Close();

	/**
	 * ioctlsocket:
	 *
	 * @param inCmd 
	 * @param ioArgp 
	 * @return  int	
	 */
	int IoctlSocket(long inCmd , unsigned long * ioArgp);

	int SetSockopt( int level, int optname,  const char * optval,  int optlen );
	int SetSockopt( int level, int optname,  int optval );

	int GetSockopt( int level, int optname,  char * optval,  int * optlen );
	int GetSockopt( int level, int optname,  int * optval );

	void Connect(const string &inHost , int inPort);
	void Connect(const SOCKADDR_IN * inSai);
	void Connect(const SOCKADDR * inSai);

	void Bind(unsigned long inBindIP , int inPort);
	void Bind(const string& inHost , int inPort);
	void Bind(const SOCKADDR_IN * inSai);
	void Bind(const SOCKADDR * inSa);


	void Listen(int inBacklog = SOMAXCONN);

	XLSocket* Accept();

	void Shutdown();

	int Send( const string&  str );
	int Send( const char* inBuffer ,unsigned int inBufferLen );
	int Recv( char* outBuffer ,unsigned int inBufferLen );

	int SendTo(const char* inBuffer ,unsigned int inBufferLen , int inFlags ,struct sockaddr_in *senderinfo,int fromlen);
	int RecvFrom(char* outBuffer ,unsigned int inBufferLen , int inFlags,struct sockaddr_in *senderinfo,int* fromlen);

	static SOCKADDR_IN ToSockAddrIn(const string &inHost , int inPort, int sin_family = AF_INET) ;
	static SOCKADDR_IN ToSockAddrIn(unsigned long ip , int inPort, int sin_family = AF_INET) ;
	static SOCKADDR_IN ToANYSockAddrIn(int inPort, int sin_family = AF_INET) ;
	static SOCKADDR ToSockAddr(const string &inHost , int inPort, int sin_family = AF_INET) ;
	static SOCKADDR ToSockAddr(unsigned long ip , int inPort, int sin_family = AF_INET) ;

	int getErrorCode() const;
	string what(int errorcode) const;
	string what() const;

	bool AddMulticastMembersip(const string& address);

	SOCKET getSocketHandle() const 
	{
		return this->Socket;
	}
protected:

	/**
	 * Socket:	ソケットハンドル
	 */
	SOCKET	Socket;
	
	
	/**
	 * Connected:	接続しているかどうか？
	 */
	bool Connected;

	bool UseSSL;
	//OpenSSL
	SSL_CTX*	SSLContext;
	SSL*		SSLhandle;
};

#endif // !defined(AFX_XLSOCKET_H__137F6EB5_32A1_46CD_9CA7_EC6E9C6A6E6A__INCLUDED_)
