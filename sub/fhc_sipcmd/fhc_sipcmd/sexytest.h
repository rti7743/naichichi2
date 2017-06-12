#pragma once

//C++ simple test
//LICENSE: NYSL
//AUTHOR: rti

#define SEXYTEST_LINE_STRCAT(x,y) SEXYTEST_LINE_STRCAT_(x,y)
#define SEXYTEST_LINE_STRCAT_(x,y) x##y

#ifdef _DEBUG

	#ifdef _MSC_VER
		//msvc
		#define SEXYTEST_BREAKPOINT()			{ \
				MSG msg;	\
				BOOL bQuit = PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);	\
				if (bQuit)	PostQuitMessage((int)msg.wParam);	\
				DebugBreak(); \
			} 
	#else
		#define SEXYTEST_BREAKPOINT()	{ asm ("int $3") ; }
	#endif

	//assertを上書きする
	#ifdef SEXYTEST_ASSERT_OVERRAIDE
		#ifdef assert
		#undef assert
		#endif
		#define assert(X)	SEXYTEST_ASSERT(X)
	#endif

	#ifndef SEXYTEST_FAIL_DO
		//ブレークポイント
		#define SEXYTEST_FAIL_DO(msg)	{ SEXYTEST_BREAKPOINT(); }
	#endif

	//各種チェック用
	#define SEXYTEST_EQ(x,y) {\
			if ( (x) != (y) ){\
				SEXYTEST_FAIL_DO(out.str().c_str());\
			}\
		}
	#define SEXYTEST_ASSERT(x) {\
			if ( ! (x) ){\
				SEXYTEST_FAIL_DO(out.str().c_str());\
			}\
		}


	#define _SEXYTEST_LINE_STRCAT(x,y,z) _SEXYTEST_LINE_STRCAT_(x,y,z)
	#define _SEXYTEST_LINE_STRCAT_(x,y,z) x##y##z

	typedef void( *SEXYTEST_TESTFUNCTION_TYPE )();

	enum SEXYTEST_TYPE_ENUM
	{
		 SEXYTEST_TYPE_NORMAL  = 1//普通のテスト　実行順はコンパイラ任せ
		,SEXYTEST_TYPE_NOW     = 0//真っ先に実行するテスト 複数個の優先があったらコンパイラ任せ.
		,SEXYTEST_TYPE_HEAVY   = 99//重たいテスト 通常は実行しない 
	};

	//STLにも依存したくないのでリストを自分で作る.
	struct SEXYTEST_LIST_ST;
	struct SEXYTEST_LIST_ST
	{
		SEXYTEST_TESTFUNCTION_TYPE func;
		SEXYTEST_TYPE_ENUM    type;
		struct SEXYTEST_LIST_ST* next;
	};

	class SEXYTEST_DATASTORE
	{
	public:
		static SEXYTEST_DATASTORE* s()
		{
			static SEXYTEST_DATASTORE s;
			return &s;
		}
		SEXYTEST_DATASTORE()
		{
			this->TestStore = NULL;
		}
		virtual ~SEXYTEST_DATASTORE()
		{
			Clear();
		}
		void Clear()
		{
			SEXYTEST_LIST_ST* p = this->TestStore;
			while( p )
			{
				SEXYTEST_LIST_ST* next = p->next;
				delete p;
				p = next;
			}
			this->TestStore = NULL;
		}


		SEXYTEST_LIST_ST* TestStore;
	};

	
	//テストを末尾に追加.
	class SexyTestUnitJoin
	{
	public:
		SexyTestUnitJoin( SEXYTEST_TESTFUNCTION_TYPE func ,SEXYTEST_TYPE_ENUM type)
		{
			SEXYTEST_LIST_ST** p = &SEXYTEST_DATASTORE::s()->TestStore;
			while(*p )
			{
				p = &((*p)->next);
			}
			SEXYTEST_LIST_ST* test = new SEXYTEST_LIST_ST();
			test->next = NULL;
			test->type = type;
			test->func = func;

			*p = test;
		}
	};

	static void SEXYTEST_RUNNER(SEXYTEST_TYPE_ENUM type = SEXYTEST_TYPE_NORMAL )
	{
		//優先実行のものをまず実行
		int types[] = {SEXYTEST_TYPE_NOW,SEXYTEST_TYPE_NORMAL,SEXYTEST_TYPE_HEAVY};
		for(int i = 0 ; i < sizeof(types); i++ )
		{
			if (types[i] > type)
			{//思いテストなどの所定レベル以上のテストはやらない.
				break;
			}

			SEXYTEST_LIST_ST* p = SEXYTEST_DATASTORE::s()->TestStore;
			while( p )
			{
				if (p->type == types[i] )
				{
					p->func();
				}
				p = p->next;
			}
		}
		//メモリをあとあと解放するとメモリーリークチェッカーが誤動作するといけないので、先に開放しておく.
		SEXYTEST_DATASTORE::s()->Clear();
	}


	//テストを書く
	#define SEXYTEST()  \
		static void SEXYTEST_LINE_STRCAT(SexyTestUnitTest,__LINE__)(); \
		static SexyTestUnitJoin SEXYTEST_LINE_STRCAT(SexyTestUnitJoin,__LINE__)(SEXYTEST_LINE_STRCAT(SexyTestUnitTest,__LINE__),SEXYTEST_TYPE_NORMAL); \
		static void SEXYTEST_LINE_STRCAT(SexyTestUnitTest,__LINE__)()

	#define SEXYTEST_NOW()  \
		static void SEXYTEST_LINE_STRCAT(SexyTestUnitTest,__LINE__)(); \
		static SexyTestUnitJoin SEXYTEST_LINE_STRCAT(SexyTestUnitJoin,__LINE__)(SEXYTEST_LINE_STRCAT(SexyTestUnitTest,__LINE__),SEXYTEST_TYPE_NOW); \
		static void SEXYTEST_LINE_STRCAT(SexyTestUnitTest,__LINE__)()

	#define SEXYTEST_HEAVY()  \
		static void SEXYTEST_LINE_STRCAT(SexyTestUnitTest,__LINE__)(); \
		static SexyTestUnitJoin SEXYTEST_LINE_STRCAT(SexyTestUnitJoin,__LINE__)(SEXYTEST_LINE_STRCAT(SexyTestUnitTest,__LINE__),SEXYTEST_TYPE_HEAVY); \
		static void SEXYTEST_LINE_STRCAT(SexyTestUnitTest,__LINE__)()


#else  //_DEBUG
	#define SEXYTEST_BREAKPOINT()
	#define SEXYTEST_TRACE(msg)

	//各種チェック用
	#define SEXYTEST_EQ(x,y)
	#define SEXYTEST_ASSERT(X)

	static void SEXYTEST_RUNNER(){}
	#define SEXYTEST()  \
		static void SEXYTEST_LINE_STRCAT(SexyTestUnitTest,__LINE__)()

	#define SEXYTEST_NOW()  \
		static void SEXYTEST_LINE_STRCAT(SexyTestUnitTest,__LINE__)()

	#define SEXYTEST_HEAVY()  \
		static void SEXYTEST_LINE_STRCAT(SexyTestUnitTest,__LINE__)()

#endif //_DEBUG