// XLSocket.h: XLSocket クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XLHTTPSOCKET_H__137F6EB5_32A1_46CD_9CA7_EC6E9C6A6E6A__INCLUDED_)
#define AFX_XLHTTPSOCKET_H__137F6EB5_32A1_46CD_9CA7_EC6E9C6A6E6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <functional>
#include <vector>
#include <map>
#include <list>
#include "XLSocket.h"
#include "XLHttpHeader.h"


class XLHttpSocket  
{
public:
	static string Get(const string& url);
	static string Get(const string& url,unsigned int timeout);
	static string Get(const string& url,const map<string,string> & header);
	static string Get(const string& url,const map<string,string> & header,unsigned int timeout);
	static string Get(const string& url,const map<string,string> & header,unsigned int timeout,XLHttpHeader* retHeader);
	static void GetBinary(const string& url,const map<string,string> & header,unsigned int timeout,vector<char>* retBinary,XLHttpHeader* retHeader);

	static string Post(const string& url,const char * postBinaryData,unsigned int postBinaryLength);
	static string Post(const string& url,unsigned int timeout,const char * postBinaryData,unsigned int postBinaryLength);
	static string Post(const string& url,const map<string,string> & header,const char * postBinaryData,unsigned int postBinaryLength);
	static string Post(const string& url,const map<string,string> & header,unsigned int timeout,const char * postBinaryData,unsigned int postBinaryLength);
	static string Post(const string& url,const map<string,string> & header,unsigned int timeout,const vector<char>& postBinaryData);
	static string Post(const string& url,const map<string,string> & header,unsigned int timeout,XLHttpHeader* retHeader,const vector<char>& postBinaryData);
	static string Post(const string& url,const map<string,string> & header,unsigned int timeout,XLHttpHeader* retHeader,const string& postString);
	static void PostBinary(const string& url,const map<string,string> & header,unsigned int timeout,vector<char>* retBinary,XLHttpHeader* retHeader,const char * postBinaryData,unsigned int postBinaryLength);

	static void HTTPRecv(XLSocket * socket ,vector<char>* retBinary,XLHttpHeader* retHeader);
};

#endif // !defined(AFX_XLHTTPSOCKET_H__137F6EB5_32A1_46CD_9CA7_EC6E9C6A6E6A__INCLUDED_)
