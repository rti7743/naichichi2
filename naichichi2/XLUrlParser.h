// XLUrlParser.h: XLUrlParser クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XLURLPARSER_H__3E362401_E16F_401B_A2CB_9B409C4F79D8__INCLUDED_)
#define AFX_XLURLPARSER_H__3E362401_E16F_401B_A2CB_9B409C4F79D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//URL形式をパースする
class XLUrlParser  
{
public:
	XLUrlParser();
	virtual ~XLUrlParser();

	void Parse(const string & inURL);

	string getProtocol() const{	return this->Protocol;	}
	string getUserName() const{	return this->UserName;	}
	string getPassword() const{	return this->Password;	}
	string getHost() const{	return this->Host;	}
	int getPort() const{	return this->Port;	}
	string getPath() const{	return this->Path;	}
	bool IsAuthParam() const{	return !this->UserName.empty();	}

	static void test();

private:
	string		Protocol;
	string		UserName;
	string		Password;
	string		Host;
	int				Port;
	string		Path;
};

#endif // !defined(AFX_XLURLPARSER_H__3E362401_E16F_401B_A2CB_9B409C4F79D8__INCLUDED_)
