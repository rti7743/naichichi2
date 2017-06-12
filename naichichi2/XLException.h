#pragma once

#ifdef _MSC_VER
#endif
#include "common.h"

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

