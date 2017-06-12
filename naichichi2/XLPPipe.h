#if !defined(AFX_XLPipe_H__137F6EB5_32A1_46CD_9CA7_EC6E9C6A6E6A__INCLUDED_)
#define AFX_XLPipe_H__137F6EB5_32A1_46CD_9CA7_EC6E9C6A6E6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class XLPipe
{
public:
	XLPipe();
	virtual ~XLPipe();
	//server
	void Create(const string& name);
	//client
	void Open(const string& name);
	int Read(char* buffer,int size);
	int Write(const char* buffer,int size);
	void Stop();

	int Write(const string& str);
	string Read(int size);
	//読めるようになるまで待機
	void WaitForReadble();
private:
#if _MSC_VER
	HANDLE Pipe;
#else
	int Pipe;
#endif
};
#endif // !defined(AFX_XLPipe_H__137F6EB5_32A1_46CD_9CA7_EC6E9C6A6E6A__INCLUDED_)
