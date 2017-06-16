//未キャッチ例外が発生したら、スタックトレースを生成します.
//
//main関数のどこかで以下のように初期化してください。
//初期化後に未キャッチ例外が発生すると、スタックトレースが生成されます.
//ディフォルトは stderrに出力します. HAIKU_WO_YOME_OUTPUT_STDERR で変更可能です.
//
//HaikuWoYome::KaisyakuShiteYaru();
//
//ライセンス:NYSL
//作った人:  rti (rti@rti-giken.jp)
//

//エラーをどこに吐き出すのか.
//ディフォルトは stderr です.
#ifndef HAIKU_WO_YOME_OUTPUT_STDERR
#define HAIKU_WO_YOME_OUTPUT_STDERR(MSG) fputs(MSG,stderr)
#endif

//あなたの独自例外クラスがあれば定義してください.
// class.what() で const char* か std::string を返す必要あり.
#ifndef HAIKU_WO_YOME_YOUR_EXECPTION_CLASS
#define HAIKU_WO_YOME_YOUR_EXECPTION_CLASS std::runtime_error
#endif


#if _MSC_VER
#include <windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")
#include <time.h>

#else
#include <execinfo.h>
#include <signal.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <cxxabi.h> //でまんぐる

//addr2lineに頼らない dumpを行う.
#include <elf.h>    //山田エルフ先生

#ifdef __x86_64__  
typedef uint64_t Elf_Addr;
typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Shdr Elf_Shdr;
typedef Elf64_Sym  Elf_Sym;
#else
typedef uint32_t Elf_Addr;
typedef Elf32_Ehdr Elf_Ehdr;
typedef Elf32_Shdr Elf_Shdr;
typedef Elf32_Sym  Elf_Sym;
#endif //__x86_64__

#endif


#include <iostream>
#include <string>
#include <sstream>

#if __cplusplus>=201103L || _MSC_VER  >= 1600
#include <exception>
#include <stdexcept>
#endif


namespace HaikuWoYome
{
#if _MSC_VER
#else

//Linuxだと SetUnhandledExceptionFilter が利用できないので、
//例外をフックして、スタックトレースを記録する。
//ちょっと重いかもしれんが気にしない方針で。
const int LAST_STACK_LENGTH = 10; //あまり重くないように忖度して10個ぐらい
static void* g_last_stack[LAST_STACK_LENGTH] = {0};
static int g_last_stack_size = 0;

typedef void (*cxa_throw_type)(void *, void *, void (*) (void *));
static cxa_throw_type g_orig_cxa_throw = NULL;

#endif //_MSC_VER

//symbollを付与しないスタックトレースを返します.
static std::string BackTraceNoSymbol(void** stack,int length)
{
	std::stringstream out;
	out << "can not symbols" << std::endl;
	for(int i = 0; i < length; i++)
	{
		out << " address:0x" << stack[i] << std::endl;
	}
	return out.str();
}

static std::string demangle(const std::string& name)
{
	char buf[256];
#if _MSC_VER
    if (UnDecorateSymbolName(name.c_str(), buf, 256, UNDNAME_COMPLETE))
    {
		return buf;
	}
#else
	int status;
	size_t length = 256;
	if ( abi::__cxa_demangle(name.c_str(),buf,&length,&status))
	{
		return buf;
	}
#endif
	return name;
}


#if _MSC_VER
#else
//elf形式の実行ファイルのシンボル情報からアドレス位置にある関数名の取得を行う.
static bool ElfToSymbol(const std::string& filename,Elf_Addr addr,std::string* outSymbol)
{
	int fd = open(filename.c_str(),O_RDONLY);
	if (fd < 0)
	{
		return false;
	}

	Elf_Ehdr ehdr;
	Elf_Shdr shdr;
	Elf_Shdr shdr_linksecsion;
	Elf_Sym  sym;
	int r = read(fd,&ehdr,sizeof(ehdr));
	if (r < 0)
	{
		close(fd);
		return false;
	}
	if ( memcmp(ehdr.e_ident,ELFMAG,SELFMAG) != 0 )
	{//can not elf
		close(fd);
		return false;
	}

	//find SHDRテーブルの探索.
	for(int i = 0 ; i < ehdr.e_shnum ; i++ )
	{
		lseek(fd,ehdr.e_shoff + (i * sizeof(shdr)),SEEK_SET);
		r = read(fd,&shdr,sizeof(shdr));
		if ( r < sizeof(shdr) )
		{
			continue;
		}
		if ( ! (shdr.sh_type == SHT_SYMTAB || shdr.sh_type == SHT_DYNSYM) )
		{//シンボルが書かれているテーブルではないっぽい.
			continue;
		}

		//sh_link番目にあるデータに文字列テーブルがあるらしい.
		lseek(fd,ehdr.e_shoff + (shdr.sh_link * sizeof(shdr)),SEEK_SET);
		r = read(fd,&shdr_linksecsion,sizeof(shdr_linksecsion));
		if ( r < sizeof(shdr_linksecsion) )
		{
			continue;
		}

		//現在のSHDRテーブルを読む
		const unsigned int nloop_count = shdr.sh_size / sizeof(sym);
		for(int n = 0 ; n < nloop_count; n++ )
		{
			lseek(fd,shdr.sh_offset + (n*sizeof(sym)),SEEK_SET);
			r = read(fd,&sym,sizeof(sym));
			if ( r < sizeof(sym) )
			{
				continue;
			}

			if (addr < sym.st_value || addr >= sym.st_value  + sym.st_size )
			{//探しているアドレスではない
				continue;
			}
			//found.
			char buf[256];
			if (sym.st_name != 0)
			{
				//名前がある場合、[sh_link].sh_offset + sym.st_name に名前がある. 0終端
				lseek(fd,shdr_linksecsion.sh_offset + sym.st_name,SEEK_SET);
				r = read(fd,buf,255);
				if ( r < 0 )
				{
					continue;
				}
				buf[r] = 0; //終端

				*outSymbol = demangle(buf);
			}
			close(fd);
			return true;
		}
	}

	close(fd);
	return false;
}
#endif

//自分自身へのパス
static std::string getSelfEXEPath()
{
#if _MSC_VER
	char buffer[MAX_PATH] = {0};
	if(!GetModuleFileName(NULL, buffer, MAX_PATH))
	{
		return "";
	}
#else
	char buffer[256] = {0};
	int r = readlink("/proc/self/exe", buffer, 255); 
	if (r < 0)
	{
		return "";
	}
#endif
	return buffer;
}

//スタックトレースにsymboll情報を付与して文字列で返します.
static std::string BackTrace(void** stack,int length)
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
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	symbol->MaxNameLen = MAX_SYM_NAME;

	for(int i = 0; i < length; i++)
	{
		DWORD64 displacement = 0;
		if ( SymFromAddr(process,(DWORD64) stack[i], &displacement, symbol) )
		{
			out << symbol->Name << " ";

			IMAGEHLP_LINE64 line = { sizeof(IMAGEHLP_LINE64) };
			if ( SymGetLineFromAddr64(process, (DWORD64) stack[i],(PDWORD)&displacement,&line) )
			{
				out << line.FileName << ":" << line.LineNumber;
			}
			else
			{
				out << "???";
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
	const std::string myprocess = getSelfEXEPath();

	//see 
	//https://www.gnu.org/software/libc/manual/html_node/Backtraces.html
	char **strings = backtrace_symbols (stack, length);
	if (!strings)
	{
		return BackTraceNoSymbol(stack,length);
	}

	for (size_t i = 0; i < length; i++)
	{
		std::string symbol;
		if ( ElfToSymbol(myprocess,(Elf_Addr)(stack[i]),&symbol) )
		{
			out << symbol << " ";
		}
		out << strings[i] << std::endl;
	}
	free (strings);
#endif
	return out.str();
}

static void ExceptionToRecord(std::stringstream& out)
{
#if __cplusplus>=201103L || _MSC_VER  >= 1600
	//C++11だとこうすることで例外をとれるらしい. なんて変な仕様だw
	try 
	{
		auto unknown = std::current_exception();
		if (unknown == NULL) 
		{//例外ではない
			out << "exception: none" << std::endl;
		}
		else
		{//例外ならば再送する.
			std::rethrow_exception(unknown);
		}
	}
	catch (const HAIKU_WO_YOME_YOUR_EXECPTION_CLASS& e) 
	{
		out << "exception: " << demangle(typeid(e).name()) << " " << e.what() << std::endl;
	}
	catch (const std::exception& e) 
	{
		out << "exception: " << demangle(typeid(e).name()) << " " << e.what() << std::endl;
	}
	catch (...)
	{
		out << "exception: unknown exception" << std::endl;
	}
#else
#endif
}


//スタックトレースを取得します.
static std::string BackTrace()
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

	out << BackTrace(callers,count);
#else
	//see 
	//https://www.gnu.org/software/libc/manual/html_node/Backtraces.html
	void *array[62];
	size_t size;

	size = backtrace (array, 62);
	out << BackTrace(array,size);
#endif
	return out.str();
}


//クラッシュした時呼ばれるハンドルを設定します.
#if _MSC_VER
static LONG WINAPI OnHaikuFunction( PEXCEPTION_POINTERS pep )
{
	const int STACK_TRACE_MAX = 63;
	int count = 0;
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

	std::stringstream out;
	out << "=HAIKU=============================================" << std::endl;
	out << "backtrace:" << std::endl;
	out << BackTrace(stack,count) << std::endl;
	out << "Exception Code:"<<pep->ExceptionRecord->ExceptionCode<< " Exception Address:" <<pep->ExceptionRecord->ExceptionAddress << std::endl; 

	//例外の記録
	ExceptionToRecord(out);

	out << "process_id: " << GetCurrentProcessId() << " thread_id:" << GetCurrentThreadId() << std::endl;
	out << "===================================================" << std::endl;
	HAIKU_WO_YOME_OUTPUT_STDERR( out.str().c_str() );

	//exceptionを上に投げる DEBUGGERがあればストップし、 なければ落ちてくれる.
	//間違っても握りつぶしてはいけない.
	return EXCEPTION_EXECUTE_HANDLER;
}

//minidumpを取得する.
static std::string StoreMiniDump(PEXCEPTION_POINTERS pep = NULL,const std::string& filename = "")
{
	std::string fullname = filename;
	if (fullname.empty())
	{
		const std::string myprocess = getSelfEXEPath();
		time_t time = ::time(NULL);
		struct tm date ;
		localtime_s(&date, &time);

		char buf[MAX_PATH];
		_snprintf_s(buf, MAX_PATH, _TRUNCATE, "%s.%04d%02d%02d_%02d%02d%02d.dmp",myprocess.c_str(),
			date.tm_year + 1900,date.tm_mon + 1,date.tm_mday,date.tm_hour,date.tm_min,date.tm_sec);
		fullname = buf;
	}

	HANDLE han = CreateFile(fullname.c_str(), GENERIC_READ|GENERIC_WRITE, 
                FILE_SHARE_WRITE|FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	if (han == INVALID_HANDLE_VALUE)
	{
		return "ERROR CAN NOT OPEN: " + fullname;
	}

	MINIDUMP_TYPE dumpflag = MiniDumpNormal;
	if ( pep == NULL )
	{
		MiniDumpWriteDump(
					GetCurrentProcess(), 
					GetCurrentProcessId(), 
					han, 
					dumpflag, 
					NULL,
					NULL,
					NULL
				);
	}
	else
	{
		MINIDUMP_EXCEPTION_INFORMATION   stExInfo ;

		stExInfo.ThreadId = GetCurrentThreadId ( ) ;
		stExInfo.ClientPointers = TRUE ;
		stExInfo.ExceptionPointers = pep ;
		MiniDumpWriteDump(
					GetCurrentProcess(), 
					GetCurrentProcessId(), 
					han, 
					dumpflag, 
					&stExInfo,
					NULL,
					NULL
				);
	}
	CloseHandle(han);
	return fullname;
}


#else
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

//スレッドIDの取得
static pid_t gettid(void)
{
    return syscall(SYS_gettid);
}

static void aexit()
{
#if __cplusplus>=201103L
	std::abort();
#else
	abort();
#endif
}

static void OnHaikuFunction()
{
	std::stringstream out;
	out << "=HAIKU=============================================" << std::endl;
	out << "backtrace:" <<  std::endl;
	out << BackTrace(g_last_stack,g_last_stack_size) << std::endl;

	ExceptionToRecord(out);

	out << "process_id: " << getpid() << " thread_id:" << gettid() << std::endl;
	out << "===================================================" << std::endl;
	HAIKU_WO_YOME_OUTPUT_STDERR( out.str().c_str() );

	//終了!
	aexit();
}

static const char* ConvertSigToString(int sig)
{
	switch(sig)
	{
	case SIGSEGV: return "SIGSEGV";
	case SIGILL:  return "SIGILL";
	case SIGFPE:  return "SIGFPE";
	case SIGBUS:  return "SIGBUS";
	}
	return "NAZO";
}

static void OnSignalFunction(int sig)
{
	std::stringstream out;
	out << "=HAIKU=============================================" << std::endl;
	out << "backtrace:" <<  std::endl;
	out << BackTrace() << std::endl;

	out << "signal: " << sig << " (" << ConvertSigToString(sig) << ")" << std::endl;
	out << "process_id: " << getpid() << " thread_id:" << gettid() << std::endl;
	out << "===================================================" << std::endl;
	HAIKU_WO_YOME_OUTPUT_STDERR( out.str().c_str() );

	//終了!
	aexit();
}

static std::string StoreMiniDump(void *pep = NULL,const std::string& filename = "")
{
	return "";
}

#endif

//ハイクを詠め。カイシャクしてやる。
//未キャッチ例外などの異常事態が起きたときのフック
static void KaisyakuShiteYaru()
{
#if _MSC_VER
	//例外を受け取った時にログを書いて死ぬようにする.
	SetUnhandledExceptionFilter(OnHaikuFunction);
#else
	std::set_terminate(OnHaikuFunction);
	signal(SIGSEGV, &OnSignalFunction);
	signal(SIGILL,  &OnSignalFunction);
	signal(SIGFPE,  &OnSignalFunction);
	signal(SIGBUS,  &OnSignalFunction);
#endif
}

};
