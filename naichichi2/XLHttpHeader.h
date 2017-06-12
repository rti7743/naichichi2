// XLHttpHeader.h: XLHttpHeader クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XLHTTPHEADER_H__3569AE9A_5235_4741_B71B_1FB2ABE4CD70__INCLUDED_)
#define AFX_XLHTTPHEADER_H__3569AE9A_5235_4741_B71B_1FB2ABE4CD70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XLStringUtil.h"

class XLHttpHeader  
{
public:
	struct __file_struct
	{
		vector<char> data;
		string filename;
		string mime;
	};


	XLHttpHeader();
	virtual ~XLHttpHeader();

	bool Parse(const char * inHeader ,unsigned int size);
	bool Parse(const string& inHeader )
	{
		return this->Parse(inHeader.c_str()  ,inHeader.size() );
	}
	void PostParse( const char * inBody ,unsigned int size );

	string getRequestMethod() const
	{
		return this->getFirstParam(0);
	}
	string getRequestPath() const
	{
		return this->getFirstParam(1);
	}
	string getRequestProtocol() const
	{
		return this->getFirstParam(2);
	}
	bool isRespons() const
	{
		//2つ目のヘッダが数字ならばレスポンス
		//HTTP/1.1 200 OK
		//GET / HTTP/1.1
		const char c = FirstHeader[1][0] ;
		return (bool)(isdigit(c));
	}

	string getResponsProtocol() const
	{
		return this->getFirstParam(0);	//HTTP/1.0
	}
	string getResponsResultCode() const
	{
		return this->getFirstParam(1);	//200
	}
	string getResponsResultState() const
	{
		return this->getFirstParam(2);	//OK
	}

	string getFirstParam(int inNumebr) const;

	void setFirstParam(int inNumebr, const string & inStr)
	{
		assert(inNumebr >= 0 && inNumebr <= 2);
		this->FirstHeader[inNumebr] = inStr;
	}
	void setFirstParams( const string & inStr1, const string & inStr2, const string & inStr3)
	{
		this->FirstHeader[0] = inStr1;
		this->FirstHeader[1] = inStr2;
		this->FirstHeader[2] = inStr3;
	}
	unsigned int getHeaderSize() const
	{
		return this->HeaderSize;
	}

	void setAt(const string& inKey , const string & inValue);
	string getAt(const string& inKey) const;
	string Build() const;
	const map<string,string>	getHeader() const
	{
		return this->Header;
	}
	const map<string,string>	getGet() const
	{
		return this->Get;
	}
	const map<string,string>	getPost()const
	{
		return this->Post;
	}
	const map<string,string>*	getHeaderPointer() const
	{
		return &this->Header;
	}
	const map<string,string>*	getGetPointer() const
	{
		return &this->Get;
	}
	const map<string,string>*	getPostPointer()const
	{
		return &this->Post;
	}
	const map<string,string>	getRequest() const
	{
		return XLStringUtil::merge(this->Get,this->Post,true);
	}
	const map<string,XLHttpHeader::__file_struct*>* getFilesPointer()const
	{
		return &this->Files;
	}
	string getCookieHeader(const string& cookiename) const;
	string getHostHeader() const;

	bool FullfileUploadParse(const char * inBody ,unsigned int size);


private:
	bool MultipartBounderParse( const char * inBody ,unsigned int size );

private:
	string							FirstHeader[3];
	map<string,string>	Header;
	map<string,string>	Get;
	map<string,string>	Post;

	//添付ファイル
	map<string,__file_struct*>	Files;
	unsigned int HeaderSize;
};

#endif // !defined(AFX_XLHTTPHEADER_H__3569AE9A_5235_4741_B71B_1FB2ABE4CD70__INCLUDED_)
