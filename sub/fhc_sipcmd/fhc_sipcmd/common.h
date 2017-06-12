#pragma once

#ifdef _MSC_VER // This check can be removed if you only build for Windows
#include <Winsock2.h>
#include <mswsock.h>
#include <Ws2tcpip.h>

#include <windows.h>

#pragma warning (push)
#pragma warning (disable : 4005)
#include <intsafe.h>
#include <stdint.h>
#pragma warning (push)
#pragma warning (default : 4005)
#include <atlbase.h>

#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
#endif

#ifndef snprintf
#define snprintf(s, n, format, ...) _snprintf_s(s, n, _TRUNCATE, format, __VA_ARGS__)
#endif

#define SecSleep(sec) ::Sleep((sec)*1000)
#define MiriSleep(miri) ::Sleep((miri))
#define INT64 __int64
#define PATHSEP "\\"
#define STRICMP(x,y)	_stricmp((x),(y))
#define STRINCMP(x,y,n)	strncmpi((x),(y),(n))
#define STRINCMPNN(x,y)	strncmpi((x),(y),sizeof(y)-1)
static const void* memmem(const void* l, size_t l_len, const void* s, size_t s_len)
{
	if (l_len < s_len)
	{
		return NULL;
	}
	if (l_len == 0 || s_len == 0)
	{
		return NULL;
	}
	const char *p = (const char*)l;
	const char *e = p + l_len - s_len;
	for( ; p < e; p++ )
	{
		if(memcmp(p,s,s_len)==0)
		{
			return (const void*)p;
		}
	}

	return NULL;
}


#else
#include <stdint.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <dirent.h>
#include <climits>
#include <cstring>
#define SecSleep(sec) sleep((sec))
#define MiriSleep(miri) usleep((miri)*1000)
#define INT64 long long
#define PATHSEP "/"
typedef unsigned int  LRESULT;
typedef unsigned int  WPARAM;
typedef unsigned int  LPARAM;
typedef unsigned int* HINSTANCE;
typedef unsigned int* HWND;
#define STRICMP(x,y)	strcasecmp((x),(y))
#define STRINCMP(x,y,n)	strncasecmp((x),(y),(n))
const int MAX_PATH = 255;

const int WM_CREATE                                             = 0x0001;
const int WM_DESTROY									        = 0x0002;
const int WM_USER                                               = 0x0400;

#endif

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <functional>
#include <assert.h>
#include <sstream>

template<typename _INT> static const std::string num2str(_INT i)
{
	std::stringstream out;
	out << i;
	return out.str();
}
template<typename _INT> static const std::wstring num2wstr(_INT i)
{
	std::wstringstream out;
	out << i;
	return out.str();
}
template<typename _INT> static const std::string num2str(_INT i,const std::string& format)
{
	char buf[64]={0};
	snprintf(buf,64,format.c_str(),i);
	return buf;
}

static int atoi(const std::string& str)
{
	return atoi(str.c_str());
}
static unsigned int atou(const std::string& str)
{
	char* e;
	return strtoul( str.c_str() ,&e , 10); 
}
static unsigned int atou(const char* str)
{
	char * e;
	return strtoul( str ,&e, 10); 
}
static float atof(const std::string& str)
{
	return (float) atof(str.c_str());
}

//strcmpみたいに数字も比較
#define INTCMP(x,y) ((x) > (y) ? 1 : ((x)==(y) ? 0 : -1))

//TRUE を true に
#define Btob(x)	((x) != FALSE)
//true を TRUE に
#define btoB(x)	((DWORD) x)

//linux系で定義されていないことがあるので、されていなければ定義する
#ifndef MAX
#define MAX(a,b)	((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b)	((a) < (b) ? (a) : (b))
#endif
#ifndef MINMAX
#define MINMAX(a,x,b)	()
#endif


//メインウィンドウ
class MainWindow;

//#include "windows_encoding.h"
#include "Logger.h"

static std::string mapfind(const std::map<std::string,std::string>& stringmap ,const std::string& name ,const std::string& def = "" )
{
	auto it = stringmap.find(name);
	if (it == stringmap.end()) return def;
	return it->second;
}
static std::string mapfind(const std::map<std::string,std::string>* stringmap ,const std::string& name ,const std::string& def = "" )
{
	auto it = stringmap->find(name);
	if (it == stringmap->end()) return def;
	return it->second;
}


// std::list<void*> みたいなのの解放
template<typename _T> static void DeleteList(_T* t)
{
	for(auto it= t->begin() ; it != t->end() ; it++)
	{
		delete (*it);
	}
	t->clear();
}
// std::map<std::string,void*> みたいなのの解放
template<typename _T> static void DeleteMapValue(_T* t)
{
	for(auto it= t->begin() ; it != t->end() ; it++)
	{
		delete (*it)->second;
	}
	t->clear();
}


class COMInit
{
public:
	COMInit()
	{
#if _MSC_VER
		::CoInitialize(NULL);
#else
#endif
	}
	virtual ~COMInit()
	{
#if _MSC_VER
		::CoUninitialize();
#else
#endif
	}
};

class WinSockInit
{
public:
	WinSockInit()
	{
#if _MSC_VER
		WSADATA wsaData;
		::WSAStartup(2 , &wsaData);
#else
#endif
	}
	virtual ~WinSockInit()
	{
#if _MSC_VER
		::WSACleanup();
#else
#endif
	}
};
#include "sexytest.h"
