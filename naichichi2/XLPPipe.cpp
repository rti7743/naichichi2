#include "common.h"
#include "XLPipe.h"
#include "MainWindow.h"
#if _MSC_VER
#else
#include <sys/types.h> 
#include <sys/wait.h>
#include <poll.h>
#include <sys/stat.h>
#endif

//windows see http://www.ne.jp/asahi/hishidama/home/tech/c/windows/CreatePipe.html
//linux   see http://qiita.com/edo_m18/items/989d2907d4627e19ff65
XLPipe::XLPipe()
{
	this->Pipe = NULL;
}
XLPipe::~XLPipe()
{
	Stop();
}
string XLPipe::Read(int size = 65535)
{
	vector<char> buf(size);
	int readsize = Read(&buf[0],size);
	if (readsize <= -1)
	{
		return "";
	}
	return string(&buf[0],0,readsize);
}
int XLPipe::Write(const string& str)
{
	return Write(str.c_str(),str.size());
}

void XLPipe::Create(const string& name)
{
	Stop();

#if _MSC_VER
	const string pipename = "\\\\.\\pipe\\" + name;
	this->Pipe = CreateNamedPipe(pipename.c_str(), PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE, 1, 256, 256, 1000, NULL);
	if (this->Pipe == INVALID_HANDLE_VALUE) 
	{
		this->Pipe = NULL;
		throw XLException("Create" + XLException::StringWindows());
	}
#else
	const string pipename = "/tmp/" + name;
	int r = mkfifo(pipename.c_str(),0666);
	if (r < 0)
	{
		throw XLException("Create" + XLException::StringErrNo());
	}

	this->Pipe = open(pipename.c_str(),O_WRONLY);
	if (r < 0)
	{
		throw XLException("Create" + XLException::StringErrNo());
	}
#endif
}

void XLPipe::Open(const string& name)
{
	Stop();

#if _MSC_VER
	const string pipename = "\\\\.\\pipe\\" + name;
	this->Pipe = CreateFile(pipename.c_str() , GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (this->Pipe == INVALID_HANDLE_VALUE)
	{
		this->Pipe = NULL;
		throw XLException("Open" + XLException::StringWindows());
		return 0;
	}
#else
	this->Pipe = open(pipename.c_str(),O_RDONLY);
	if (r < 0)
	{
		throw XLException("Open" + XLException::StringErrNo());
	}
#endif
}

int XLPipe::Read(char* buffer,int size)
{
	ASSERT(this->Process != NULL );
#if _MSC_VER
	DWORD numberOfBytesRead;
	if (!ReadFile(this->ReadPipe, buffer, size, &numberOfBytesRead, NULL)) {
		return -1;
	}
	return (int)numberOfBytesRead;
#else
	return read(this->ReadPipe, buffer, size);
#endif
}

int XLPipe::Write(const char* buffer,int size)
{
	ASSERT(this->Process != NULL );
#if _MSC_VER
	DWORD numberOfBytesWritten;
	if (!WriteFile(this->WritePipe, buffer, size, &numberOfBytesWritten, NULL)) {
		return -1;
	}
	return (int)numberOfBytesWritten;
#else
	return write(this->WritePipe, buffer, size);
#endif
}

//読めるようになるまで待機
void XLPipe::WaitForReadble()
{
#if _MSC_VER
	DWORD dwRead = 0;
	for(;;)
	{
		if ( ::PeekNamedPipe(this->ReadPipe, NULL, 0, NULL, &dwRead, NULL) == 0 )
		{//プロセスの強制断
			break;
		}
		if (dwRead!=0)
		{//データが来た
			break;
		}
		//windowsのOVERLAPPEDはわかりにくいので、 CPUを使って待ちます.
		SecSleep(1000);
	}
#else
	fd_set fds, readfds;
	FD_ZERO(&readfds);
	FD_SET(this->ReadPipe, &readfds);

	select(this->ReadPipe+1, &fds, NULL, NULL, NULL);
#endif

}

void XLPipe::Stop()
{
#if _MSC_VER
	if (this->Pipe != NULL) {
		CloseHandle(this->Pipe);
		this->Pipe = NULL;
	}
#else
	if (this->Pipe != NULL) {
		close(this->Pipe);
		this->Pipe = NULL;
	}
#endif
}

// 子プロセスの終了待ち
void XLPipe::Join()
{
#if _MSC_VER
	WaitForSingleObject(this->Process, INFINITE);
#else
	int status;
	waitpid(this->Process, &status, WNOHANG);
#endif
}
