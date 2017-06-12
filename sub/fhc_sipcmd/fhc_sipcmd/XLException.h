#pragma once

#ifdef _MSC_VER
#endif
#include <string>
#include <typeinfo>
#include <iostream>
#include <sstream>

//UTF-8 BOMいれた...
//
class XLException : public std::exception {
protected:
	std::string message; //
	int code;			 //std::exceptionがなぜか保持してくれないので自分で保持する.
public:

	XLException() : code(0)
	{
	}
	XLException(const std::string & message , int code)
		: std::exception() , message(message) , code(code)
	{
	}
	XLException(const std::string & message)
		: std::exception() , message(message) , code(0)
	{
	}
	XLException(const XLException& error)
		: std::exception() 
	{
		this->message = error.message;
		this->code = error.code;
	}
	XLException(const XLException& error,const std::string & message)
		: std::exception()
	{
		this->message = message + error.message;
		this->code = error.code;
	}
	XLException(const XLException& error,const std::string & message,int code)
		: std::exception()
	{
		this->message = message + error.message;
		this->code = code;
	}
	virtual ~XLException() throw()
	{
	}

	virtual int getErrorCode() const
	{
		return this->code;
	}
	virtual std::string getErrorMessage() const
	{
		return this->message;
	}
	virtual std::string getFullErrorMessage() const
	{
		std::stringstream out;
		out << this->code << " " << this->message;

		return out.str();
	}

	static std::string StringErrNo();
#if _MSC_VER
	static std::string StringWindows();
#endif

	static std::string StringErrNo(int errorno);

#if _MSC_VER
	static std::string StringWindows(unsigned long errorno );
#endif
};

