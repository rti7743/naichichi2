#pragma once

#include "common.h"



#define XLEXCEPTION(MSG) XLException(static_cast<std::ostringstream&>(std::ostringstream().seekp(0, std::ios_base::cur) << MSG << " " << __FUNCTION__ << ":" << __LINE__).str())
//throw XLEXCEPTION("foo" << "bar" << 123)
//↓
//throw XLEXCEPTION("foobar123 function:456");
//see
//https://stackoverflow.com/questions/492475/inline-ostringstream-macro-reloaded

class XLException {
protected:
	string message; //メッセージ
public:
	XLException()
	{
	}
	XLException(const string & msg)
	{
		this->message = msg;
	}
	XLException(const XLException& error)
	{
		this->message = error.message;
	}
	virtual ~XLException() throw()
	{
	}

	virtual const char* what() const
	{
		return this->message.c_str();
	}

	static string StringErrNo();
#if _MSC_VER
	static string StringWindows();
#endif

	static string StringErrNo(int errorno);

#if _MSC_VER
	static string StringWindows(unsigned long errorno );
#endif
};

