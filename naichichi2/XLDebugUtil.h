#pragma once
//デバッグ系ユーティリティ
class XLDebugUtil
{
public:
	//メインスレッドIDを取得する.
	static thread::id GetMainThreadID()
	{
		static auto threadid = this_thread::get_id();
		return threadid;
	}
	//MemoryToFile:		メモリをファイルに
	static void MemoryToFile(const string& inFilename ,const void* inMemory , int inSize);
	//MemoryToFile:		メモリをファイルに
	static void MemoryToFile(const string& inFilename ,const char* inMemory );
	//MemoryToFile:		メモリをファイルに
	static void MemoryToFile(const string& inFilename ,string& inMemory );
	//FileCheck:			二つのファイルが同じかどうかチェック.
	static bool FileVerify(const string& inSrc,const string& inDest);
	//16進数dump
	static string HexDump(const void* data,int len);

	static void SetThreadName(const std::string& name);
};
#if _DEBUG
//メインスレッドでしか動きません
	#define ASSERT_IS_MAIN_THREAD_RUNNING()			(assert(XLDebugUtil::GetMainThreadID() == this_thread::get_id()))
//メインスレッド以外で動きます
	#define ASSERT___IS_WORKER_THREAD_RUNNING()		(assert(XLDebugUtil::GetMainThreadID() != this_thread::get_id()))
//このスレッドでのみ動きます
	#define ASSERT___IS_WORKER_THREAD_RUNNING_OF(X)	(assert(((X).get_id()) != this_thread::get_id()))
//どのスレッドでも動きます
	#define ASSERT___IS_THREADFREE()				
	#define ASSERT(x)                               (assert(x))
#else
	#define ASSERT_IS_MAIN_THREAD_RUNNING()	
	#define ASSERT___IS_WORKER_THREAD_RUNNING()		
	#define ASSERT___IS_WORKER_THREAD_RUNNING_OF(X)
	#define ASSERT___IS_THREADFREE()				
	#define ASSERT(x)
#endif //_DEBUG
