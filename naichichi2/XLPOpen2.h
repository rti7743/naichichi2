#if !defined(AFX_XLPOpen2_H__137F6EB5_32A1_46CD_9CA7_EC6E9C6A6E6A__INCLUDED_)
#define AFX_XLPOpen2_H__137F6EB5_32A1_46CD_9CA7_EC6E9C6A6E6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//LinuxとWindows両対応な peopn2を実現する.
class XLPOpen2
{
public:
	XLPOpen2();
	virtual ~XLPOpen2();
	void Open(const string& commandLine);
	int Read(char* buffer,int size);
	int Write(const char* buffer,int size);
	void Stop();

	int Write(const string& str);
	string Read(int size);
	//読めるようになるまで待機
	void WaitForReadble();

	bool IsExiteProcess();
	void Join();
private:
#if _MSC_VER
	HANDLE Process;
	HANDLE ReadPipe;
	HANDLE WritePipe;

	HANDLE ChildReadPipe;
	HANDLE ChildWritePipe;
#else
	int Process;
	int ReadPipe;
	int WritePipe;
#endif
};
#endif // !defined(AFX_XLPOpen2_H__137F6EB5_32A1_46CD_9CA7_EC6E9C6A6E6A__INCLUDED_)
