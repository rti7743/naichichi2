// XLSocket.h: XLSocket クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GZIP_H__137F6EB5_32A1_46CD_9CA7_EC6E9C6A6E6A__INCLUDED_)
#define AFX_GZIP_H__137F6EB5_32A1_46CD_9CA7_EC6E9C6A6E6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class XLGZip
{
public:
	XLGZip();
	virtual ~XLGZip();

	bool gzip(const char* data ,unsigned int len, int compression_level = 6);
	bool gunzip(const char* data ,unsigned int len);
	//bool lz4(const char* data ,unsigned int len , int compression_level );
	//bool unlz4(const char* data ,unsigned int len);
	bool wavpack(const char* data ,unsigned int len , long sfreq );
	bool wavunpack(const char* data ,unsigned int len);
	bool rtiwavpack(const char* data ,unsigned int len , int compression_level );
	bool rtiwavunpack(const char* data ,unsigned int len );
	bool rtiwavpack2(const char* data ,unsigned int len , int compression_level );
	bool rtiwavpack3(const char* data ,unsigned int len , int compression_level );

	bool rtiwavunpack2(const char* data ,unsigned int len);
	bool rtiwavunpack3(const char* data ,unsigned int len);

	char* getData() const { return this->Data; }
	unsigned int getSize() const { return this->Size; }
private:
	void Free();

	char* Data;
	unsigned int Size;
};


#endif // !defined(AFX_GZIP_H__137F6EB5_32A1_46CD_9CA7_EC6E9C6A6E6A__INCLUDED_)
