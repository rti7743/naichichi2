#include "common.h"
#include "XLDebugUtil.h"
#include "sexylog.h"

/**
 * MemoryToFile:		メモリをファイルに
 *
 * @param inFilename	ファイル名
 * @param inMemory		メモリ
 * @param inSize		メモリサイズ
 * @return void 
 */
void XLDebugUtil::MemoryToFile(const string& inFilename ,const void* inMemory , int inSize)
{
	FILE * fp = fopen( inFilename.c_str() ,"wb");
	if (fp == NULL)
	{
		assert(0);
		return;
	}

	fwrite( inMemory  ,sizeof(char) , inSize , fp);

	fclose(fp);
}

/**
 * MemoryToFile:		メモリをファイルに
 *
 * @param inFilename	ファイル名
 * @param inMemory		メモリ
 * @return void 
 */
void XLDebugUtil::MemoryToFile(const string& inFilename ,const char* inMemory )
{
	XLDebugUtil::MemoryToFile(inFilename , inMemory , strlen(inMemory) );
}

/**
 * MemoryToFile:		メモリをファイルに
 *
 * @param inFilename	ファイル名
 * @param inMemory		メモリ
 * @return void 
 */
void XLDebugUtil::MemoryToFile(const string& inFilename ,string& inMemory )
{
	XLDebugUtil::MemoryToFile(inFilename , inMemory.c_str() , inMemory.size() );
}

/**
 * FileCheck:			二つのファイルが同じかどうかチェック.
 *
 * @param inSrc			ファイル名1
 * @param inDest		ファイル名2
 * @return bool 
 */
bool XLDebugUtil::FileVerify(const string& inSrc,const string& inDest)
{
	FILE* src = fopen(inSrc.c_str() , "rb" );
	assert(src);
	FILE* dest = fopen(inDest.c_str() , "rb" );
 	assert(dest);
	
	bool ret = true;

	char* srcBuffer = new char[1024];
	char* destBuffer = new char[1024];
	while(1)
	{
		int srclen = fread( srcBuffer , 1 , 1024 , src);
		int destlen = fread( destBuffer , 1 , 1024 , dest);

		if (srclen == 0 && destlen == 0) break;

		if (srclen != destlen)
		{
			fclose(src);			fclose(dest);
			//サイズ不一致
			ret = false;
			break;
		}
		if ( memcmp(srcBuffer ,  destBuffer ,srclen) != 0)
		{
			fclose(src);			fclose(dest);
			//内容不一致
			ret = false;
			break;
		}
	}
	fclose(src);			fclose(dest);
	delete []  srcBuffer ;
	delete [] destBuffer ;

	return ret;
}

/**
 * HexDump:		16進数ダンプ
 *
 * @return string
 */
string XLDebugUtil::HexDump(const void* data,int len)
{
	string ret ;
	ret.reserve(len * 4 );

	for(int i = 0 ; i < len ; i++ )
	{
		char buf[10];
		snprintf(buf,10,"%02x",((const unsigned char*)data)[i]);
		ret += buf;
		if ( i > 0  && i % 16 == 0)
		{
			ret += "\r\n";
		}
		else
		{
			ret += " ";
		}
	}
	return ret;
}

#if _MSC_VER
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")
#else
#include <execinfo.h>
#include <signal.h>
#endif

std::string XLDebugUtil::BackTraceNoSymbol(void** stack,int length)
{
	std::stringstream out;
	out << "can not symbols" << std::endl;
	for(int i = 0; i < length; i++)
	{
		out << " address:0x" << stack[i] << std::endl;
	}
	return out.str();
}

std::string XLDebugUtil::BackTrace(void** stack,int length)
{
	std::stringstream out;
#if _MSC_VER
	HANDLE process = GetCurrentProcess();
	if ( ! SymInitialize(process, NULL, TRUE) )
	{
		return BackTraceNoSymbol(stack,length);
	}

	SYMBOL_INFO *symbol = (SYMBOL_INFO *)malloc(sizeof(SYMBOL_INFO)+(MAX_PATH) );
	if(!symbol)
	{
		return BackTraceNoSymbol(stack,length);
	}

	for(int i = 0; i < length; i++)
	{
		if ( SymFromAddr(process,(DWORD64) stack[i], NULL, symbol) )
		{
			out << symbol->Name << " ";

			IMAGEHLP_LINE line;
			if ( SymGetLineFromAddr(process, (DWORD64) stack[i],NULL,&line) )
			{
				out << line.FileName << ":" << line.LineNumber;
			}
			else
			{
				out << "???" ;
			}
		}
		else
		{
			out << "???" << " " << "???";
		}
		out << " address:0x" << stack[i] << std::endl;
	}
	free(symbol);
#else
	//自プロセスへのパス
	char myprocess[256];
	int r = readlink("/proc/self/exe", myprocess, sizeof(myprocess));  
	if(r<0)
	{
		return BackTraceNoSymbol(stack,length);
	}

	//see 
	//https://www.gnu.org/software/libc/manual/html_node/Backtraces.html
	char **strings = backtrace_symbols (stack, length);
	if (!strings)
	{
		return BackTraceNoSymbol(stack,length);
	}

	char add2line[256];
	char buffer[256];
	for (size_t i = 0; i < length; i++)
	{
		//addr2lineでアドレスから関数名へ変換可能
		//see
		//https://gist.github.com/jvranish/4441299
		snprintf(add2line,256,"addr2line -f -p -C -e %s %p",myprocess,stack[i]);
		FILE * fp = popen(add2line , "r");
		if (fp)
		{
			fgets(buffer, 256, fp);
			pclose(fp);	
			if (buffer[0] != '?' && buffer[0] != '\0')
			{//addr2lineで名前が判明すれば表示
				char * sep = strstr(buffer," at ??");
				if ( sep )
				{//aaaaa(int) at ??:?? の、 at 以降を消す.
					*sep = 0;
				}
				else
				{//末尾の改行を消す.
					buffer[ strlen(buffer) - 1] = 0; 
				}
				out << buffer << " ";
			}
		}
		out << strings[i] << std::endl;
	}

	free (strings);
#endif
	return out.str();
}


std::string XLDebugUtil::BackTrace()
{
	std::stringstream out;
#if _MSC_VER
	///see
	//https://stackoverflow.com/questions/590160/how-to-log-stack-frames-with-windows-x64
	//https://stackoverflow.com/questions/22467604/how-can-you-use-capturestackbacktrace-to-capture-the-exception-stack-not-the-ca
	typedef USHORT (WINAPI *CaptureStackBackTraceType)(__in ULONG, __in ULONG, __out PVOID*, __out_opt PULONG);
	CaptureStackBackTraceType func = (CaptureStackBackTraceType)(GetProcAddress(LoadLibrary("kernel32.dll"), "RtlCaptureStackBackTrace"));

	if(func == NULL)
	{// WOE 29.SEP.2010
		return "";
	}

	// Quote from Microsoft Documentation:
	// ## Windows Server 2003 and Windows XP:  
	// ## The sum of the FramesToSkip and FramesToCapture parameters must be less than 63.
	const int kMaxCallers = 62; 

	void* callers[kMaxCallers];
	int count = (func)(0, kMaxCallers, callers, NULL);

	out << XLDebugUtil::BackTrace(callers,count);
#else
	//see 
	//https://www.gnu.org/software/libc/manual/html_node/Backtraces.html
	void *array[62];
	size_t size;

	size = backtrace (array, 62);
	out << XLDebugUtil::BackTrace(array,size);
#endif
	return out.str();
}




//クラッシュした時呼ばれるハンドルを設定します.
#if _MSC_VER
static LONG WINAPI OnHaikuFunction( PEXCEPTION_POINTERS pep )
{
	PEXCEPTION_RECORD per = pep->ExceptionRecord;

	const int STACK_TRACE_MAX = 63;
	int count;
	void* stack[STACK_TRACE_MAX];
	STACKFRAME64 stack_frame = {0};
#if defined(_WIN64)
	int machine_type = IMAGE_FILE_MACHINE_AMD64;
	stack_frame.AddrPC.Offset = pep->ContextRecord->Rip;
	stack_frame.AddrFrame.Offset = pep->ContextRecord->Rbp;
	stack_frame.AddrStack.Offset = pep->ContextRecord->Rsp;
#else
	int machine_type = IMAGE_FILE_MACHINE_I386;
	stack_frame.AddrPC.Offset = pep->ContextRecord->Eip;
	stack_frame.AddrFrame.Offset = pep->ContextRecord->Ebp;
	stack_frame.AddrStack.Offset = pep->ContextRecord->Esp;
#endif
	stack_frame.AddrPC.Mode = AddrModeFlat;
	stack_frame.AddrFrame.Mode = AddrModeFlat;
	stack_frame.AddrStack.Mode = AddrModeFlat;


	while (StackWalk64(machine_type,
		GetCurrentProcess(),
		GetCurrentThread(),
		&stack_frame,
		pep->ContextRecord,
		NULL,
		&SymFunctionTableAccess64,
		&SymGetModuleBase64,
		NULL) ) 
	{
		stack[count++] = reinterpret_cast<void*>(stack_frame.AddrPC.Offset);
		if (count >= STACK_TRACE_MAX)
		{
			break;
		}
	}
	ERRORLOG("=HAIKU=============================================");
	ERRORLOG("backtrace:" << std::endl << XLDebugUtil::BackTrace(stack,count));
	ERRORLOG("Exception Code:"<<per->ExceptionCode<< " Exception Address:" <<per->ExceptionAddress); 
	sexylog::m()->Flush();

	//C++11だとこうすることで例外をとれるらしい. なんて変な仕様だw
	try 
	{
		auto unknown = std::current_exception();
		if (unknown == NULL) 
		{//例外ではない
			ERRORLOG("exception: none");
		}
		else
		{//例外ならば再送する.
			std::rethrow_exception(unknown);
		}
	}
	catch (const XLException& e) 
	{
		ERRORLOG("exception: XLException " << e.what());
	}
	catch (const std::exception& e) 
	{
		ERRORLOG("exception: " << e.what());
	}
	catch (...)
	{
		ERRORLOG("exception: unknown exception");
	}
	
	ERRORLOG("===================================================");
	sexylog::m()->Flush();

	//exceptionを上に投げる DEBUGGERがあればストップし、 なければ落ちてくれる.
	//間違っても握りつぶしてはいけない.
	return EXCEPTION_EXECUTE_HANDLER;
}
#else
#include <dlfcn.h>

//Linuxだと SetUnhandledExceptionFilter が利用できないので、
//例外をフックして、スタックトレースを記録する。
//ちょっと重いかもしれんが気にしない方針で。
const int LAST_STACK_LENGTH = 10; //あまり重くないように忖度して10個ぐらい
static void* g_last_stack[LAST_STACK_LENGTH] = {0};
static int g_last_stack_size = 0;

typedef void (*cxa_throw_type)(void *, void *, void (*) (void *));
static cxa_throw_type g_orig_cxa_throw = NULL;


extern "C" {
	void __cxa_throw (void *thrown_exception, void *pvtinfo, void (*dest)(void *)) 
	{
		if (!g_orig_cxa_throw)
		{
			g_orig_cxa_throw = (cxa_throw_type) dlsym(RTLD_NEXT, "__cxa_throw");
		}
		g_last_stack_size = backtrace(g_last_stack,LAST_STACK_LENGTH);
		g_orig_cxa_throw(thrown_exception, pvtinfo, dest);
	}
};

static void OnHaikuFunction()
{
	ERRORLOG("=HAIKU=============================================");
	ERRORLOG("backtrace:" <<  std::endl << XLDebugUtil::BackTrace(g_last_stack,g_last_stack_size));
	sexylog::m()->Flush();

	//C++11だとこうすることで例外をとれるらしい. なんて変な仕様だw
	try 
	{
		auto unknown = std::current_exception();
		if (unknown == NULL) 
		{//例外ではない
			ERRORLOG("exception: none");
		}
		else
		{//例外ならば再送する.
			std::rethrow_exception(unknown);
		}
	}
	catch (const XLException& e) 
	{
		ERRORLOG("exception: XLException " << e.what());
	}
	catch (const std::exception& e) 
	{
		ERRORLOG("exception: " << e.what());
	}
	catch (...)
	{
		ERRORLOG("exception: unknown exception");
	}

	ERRORLOG("===================================================");
	sexylog::m()->Flush();

	//終了!
	std::abort();
}

#endif

//ハイクを詠め。カイシャクしてやる。
//未キャッチ例外などの異常事態が起きたときのフック
void XLDebugUtil::HaikuWoYome()
{
#if _MSC_VER
	//例外を受け取った時にログを書いて死ぬようにする.
	SetUnhandledExceptionFilter(OnHaikuFunction);
#else
	std::set_terminate(OnHaikuFunction);
#endif
}
