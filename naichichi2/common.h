#pragma once

#ifdef _MSC_VER
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
#include <ctime>
#include <time.h>  
#include <sys/types.h>  
#include <sys/timeb.h>  
#include <string.h>  

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

#if _MSC_VER < 1800
//VS2013までは thread使えないので、boostで代用
#include <boost/thread.hpp>
#include <boost/ref.hpp>

//std:: や boost::は常につけたい。だがしかし
//define は :: に対応していないんだ。 #define std::thread boost::thread はうごかない。
using namespace boost;
using namespace std;

#else
//VS2013以降
#include <thread>
#include <mutex>
using namespace std;

#endif //_MSC_VER < 1800


#else 
//linux

#include <stdint.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <limits>
#include <climits>
#include <cstring>
#include <thread>
#include <mutex>
#include <future>
using namespace std;

#define SecSleep(sec) sleep((sec))
#define MiriSleep(miri) usleep((miri)*1000)
#define INT64 long long
#define PATHSEP "/"
typedef unsigned int  LRESULT;
typedef unsigned int* WPARAM;
typedef unsigned int* LPARAM;
typedef unsigned int* HINSTANCE;
typedef unsigned int* HWND;
#define STRICMP(x,y)	strcasecmp((x),(y))
#define STRINCMP(x,y,n)	strncasecmp((x),(y),(n))
const int MAX_PATH = 512;

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
#include <algorithm>

template<typename _INT> static const string num2str(_INT i)
{
	stringstream out;
	out << i;
	return out.str();
}
template<typename _INT> static const wstring num2wstr(_INT i)
{
	wstringstream out;
	out << i;
	return out.str();
}
template<typename _INT> static const string num2str(_INT i,const string& format)
{
	char buf[64]={0};
	snprintf(buf,64,format.c_str(),i);
	return buf;
}

static int atoi(const string& str)
{
	return atoi(str.c_str());
}
static unsigned int atou(const string& str)
{
	char* e;
	return strtoul( str.c_str() ,&e , 10); 
}
static unsigned int atou(const char* str)
{
	char * e;
	return strtoul( str ,&e, 10); 
}
static float atof(const string& str)
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

#include "XLException.h"
#include "windows_encoding.h"
#include "XLDebugUtil.h"
#include "sexytest.h"

static string mapfind(const map<string,string>& stringmap ,const string& name ,const string& def = "" )
{
	auto it = stringmap.find(name);
	if (it == stringmap.end()) return def;
	return it->second;
}
static string mapfind(const map<string,string>* stringmap ,const string& name ,const string& def = "" )
{
	auto it = stringmap->find(name);
	if (it == stringmap->end()) return def;
	return it->second;
}

static bool stringbool(const string& str )
{
	if ( str.empty() ) return false;
	if ( str == "0" || STRICMP(str.c_str() , "false") == 0 || STRICMP(str.c_str() , "no") == 0 || STRICMP(str.c_str() , "off") == 0  || STRICMP(str.c_str() , "none") == 0)
	{
		return false;
	}
	return true;
}
template<typename _INT> static _INT sizehosei(_INT x, _INT min,_INT max)
{
	if (x > max) return max;
	if (x < min) return min;
	return x;
}

//スレッドの秒単位スリープ
static void SecSleepEx(mutex& lock,condition_variable& EventObject,unsigned int sec)
{
	//愚かな仕様の this_thread::sleep はこれじゃない. 俺たちのほしいのはこれじゃない。はっきりわかんだね。
	//終了割り込みがちゃんとかからない sleep なんて、 ::Sleep() と同じやん。
	//ついでに chronoもとても使いずらいので改善を希望。なんでも型でやろうとするのは理想主義だと思うんだよな。
	const auto timeout = chrono::seconds(sec);

	unique_lock<mutex> al(lock);
	EventObject.wait_for( al , timeout );
}
//スレッドのミリ秒単位スリープ
static void MiriSleepEx(mutex& lock,condition_variable& EventObject,unsigned int miri)
{
	const auto timeout = chrono::milliseconds(miri);

	unique_lock<mutex> al(lock);
	EventObject.wait_for( al , timeout );
}

// list<void*> みたいなのの解放
template<typename _T> static void DeleteList(_T* t)
{
	for(auto it= t->begin() ; it != t->end() ; it++)
	{
		delete (*it);
	}
	t->clear();
}
// map<string,void*> みたいなのの解放
template<typename _T> static void DeleteMapValue(_T* t)
{
	for(auto it= t->begin() ; it != t->end() ; it++)
	{
		delete (*it)->second;
	}
	t->clear();
}

//ログ出力とかを有利にするためのトリック
template<class _T>ostream& operator<<(ostream& os, const std::vector<_T>& a)  
{
	os << "[";
	for(auto it = a.begin() ; it != a.end() ; it++)
	{
		os << *it << "\t";
	}
	os << "]";
	return os;  
}
template<class _T>ostream& operator<<(ostream& os, const std::list<_T>& a)  
{
	os << "{";
	for(auto it = a.begin() ; it != a.end() ; it++)
	{
		os << *it << "\t";
	}
	os << "}";
	return os;  
}
template<class _A,class _B>ostream& operator<<(ostream& os, const std::map<_A,_B>& a)  
{
	os << "{";
	for(auto it = a.begin() ; it != a.end() ; it++)
	{
		os << it->first << "=" << it->second << "\t";
	}
	os << "}";
	return os;  
}

#include <random>
//乱数を発行する
static int xlrandom(int end=0)
{
	ASSERT(end != 1); //(start,end] で endを含まないので1はありえない.
/* //rand()バージョン
#ifdef _MSC_VER
	//windowsだと srandはスレッドごとなので thread_local指定にする.
	static __declspec( thread )  bool s_init = false;
#else
	static bool s_init = false;
#endif

	//std::random使いずらいからrandでいいや・・・

	if( s_init == false)
	{
		srand((unsigned int)time(NULL));
		s_init = true;
	}
	if(end == 0)
	{
		return rand();
	}
	return rand() % end;

*/
//mt19937 バージョン
#if _MSC_VER
	//windows の __declspec( thread ) は、コンストラクタがあるとダメなので
	struct mt19937_rapper{
		bool	init;
		std::mt19937 mt;
	};

	static __declspec( thread )  mt19937_rapper mt;
	if( !mt.init)
	{
		mt.init = true;
		mt.mt.seed((unsigned int) time(NULL));
	}
	if (end == 0)
	{
		return mt.mt();
	}
    std::uniform_int_distribution<int> distribution(0,end - 1);
    return distribution(mt.mt);
#elif __GNUC__ < 5
	//残念ながら古いgccは thread_localをサポートしない.
	//__threadはオブジェクトをサポートしない
	//ポインタならサポートするが、この方法は、メモリリークがある.
	//まあ、ごく少量だからきにしない方向でいく
	struct mt19937_rapper{
		std::mt19937 mt;
	};
	static __thread mt19937_rapper* mt = NULL;
	if( mt == NULL )
	{
		mt = new mt19937_rapper;
		mt->mt.seed((unsigned int) time(NULL));
	}
	if (end == 0)
	{
		return mt->mt();
	}
    std::uniform_int_distribution<int> distribution(0,end - 1);
    return distribution(mt->mt);
#else
	//C++11 の thread_local が使える場合
    static thread_local std::mt19937 mt(time(NULL));
	if (end == 0)
	{
		return mt();
	}
    std::uniform_int_distribution<int> distribution(0,end - 1);
    return distribution(mt);
#endif
}
