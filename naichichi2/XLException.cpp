#include "common.h"
#include "XLException.h"

string XLException::StringErrNo()
{
	return StringErrNo(errno);
}

#if _MSC_VER
string XLException::StringWindows()
{
	return StringWindows(::GetLastError());
}
#endif


string XLException::StringErrNo(int errorno)
{
	stringstream out;
	out << " errno:" << errorno << " info:";
#ifdef __STDC_WANT_SECURE_LIB__
		char buffer[1024];
		strerror_s( buffer , sizeof(buffer)-1, errorno);
		buffer[sizeof(buffer)-1] = '\0';
		out << buffer;
#else
		out << strerror( errorno );
#endif
		return out.str();
}

#if _MSC_VER

string XLException::StringWindows(unsigned long errorno )
{
	stringstream out;
	out << " GetLastError:" << errorno << " info:";

	void* msgBuf;
	::FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorno,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // デフォルト言語
		(LPTSTR) &msgBuf,
		0,
		NULL 
	);
	if (msgBuf == NULL)
	{
		out << "can not found error!";
	}
	else
	{
		out << msgBuf;
	}
	return out.str();
}

#endif
