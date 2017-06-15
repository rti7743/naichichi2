#pragma once

#include "common.h"



#define XLEXCEPTION(MSG) XLException(static_cast<std::ostringstream&>(std::ostringstream().seekp(0, std::ios_base::cur) << MSG << " " << __FUNCTION__ << ":" << __LINE__).str())
//throw XLEXCEPTION("foo" << "bar" << 123)
//↓	
//throw XLEXCEPTION("foobar123 function:456");
//see
//https://stackoverflow.com/questions/492475/inline-ostringstream-macro-reloaded

class XLException : public std::runtime_error {
public:
	XLException() : std::runtime_error("")
	{
	}
	XLException(const string & msg) : std::runtime_error(msg)
	{
	}
	virtual ~XLException() throw()
	{
	}

	static std::string StringErrNo(int errorno);
	static std::string StringErrNo();
#if _MSC_VER
	static std::string StringWindows(unsigned long errorno);
	static std::string StringWindows();
#endif
};

