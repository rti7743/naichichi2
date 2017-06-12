#pragma once

//C++ simple test
//LICENSE: NYSL
//AUTHOR: rti
/*
ヘッダだけで使えるテストライブラリです。
ヘッダだけなので、コピペすれば即使えます。

テストとテスト対象の近くにあるべきということと、
テストは意識しないでも毎回実行されるべき、
IDEでブレークポイントとか置きたいよね、
と、いう設計思想です。

//テストしたい関数
int add(int a,int b)
{
	return a + b;
}

//テストしたい関数のすぐ下に書くことを推奨する
//なぜなら、テストがドキュメントになるからだ。
SEXYTEST()
{
	//1+2=3であるべき.
	int r = add(1,2);
	assert(r==3);
}

SEXYTEST()
{
	//3+2=5であるべき.
	int r = add(3,2);
	assert(r==5);
}

こんな風に書いていきます。
pythonとかのテストに気持ち的に近い感じです。

そんで、main()に、テストを実行する魔法を書きます。

int main()
{
	//初期化などをする
	winsockInit init;

	//テストを実行する
	SEXYTEST_RUNNER();
	
	//アプリを書く
	app.mainloop();

	return 0;
}

つまり、アプリを実行するということは、テストを必ず動かすことになります。
プログラム描いたのにテストランをしない人はいないですから、
テストは常に実行されます。
テストをしていることすら忘れるぐらいテストが実行されます。

もちろん、これは _DEBUGモードの話。
リリースビルドでは、テストは取り除かれますのでご安心ください。


ただ、これには2つ問題があります。
1つは、重たいテストが実行できないことです。
10分かかるテストなんて作られたら、テストランするたびに毎回10分まつことになりますからね。
さすがにそれはできません。
テストは、1秒以下で終わることが大原則です。

それでも、重たいテストを書きたいときもあるでしょう。
そういうときは、SEXYTEST_HEAVYを使います。


//重たいテストしたい関数
int add_sleep(int a,int b)
{
	::Sleep(60);
	return a + b;
}

SEXYTEST_HEAVY()
{
	//3+2=5であるべき.
	int r = add_sleep(3,2);
	assert(r==5);
}

SEXYTEST_HEAVYでかかれたテストは、通常のテストでは実行されません。
ヘビーなテストを実行するには、以下のようにします。

int main()
{
	//初期化などをする
	winsockInit init;

	//重たいテストを含めて、テストを実行する
	SEXYTEST_RUNNER(SEXYTEST_TYPE_HEAVY);
	
	//アプリを書く
	app.mainloop();

	return 0;
}


引数などで切り分けるといいでしょう。

int main()
{
	//初期化などをする
	winsockInit init;

	if (argc[1][0] == 'h')
	{
		//重たいテストを含めて、テストを実行する
		SEXYTEST_RUNNER(SEXYTEST_TYPE_HEAVY);
	}
	else
	{	//ふつー
		SEXYTEST_RUNNER();
	}
	
	//アプリを書く
	app.mainloop();

	return 0;
}

テストが実行する順番はコンパイラの気まぐれになります。
ですが、現在自分がデバッグしている関数のテストを優先してほしい時があります。
その場合、SEXYTEST() を SEXYTEST_NOW() と、します。


SEXYTEST()
{
	//1+2=3であるべき.
	int r = add(1,2);
	assert(r==3);
}

SEXYTEST_NOW()   //最優先でこのテストを実行する
{
	//3+2=5であるべき.
	int r = add(3,2);
	assert(r==5);
}

SEXYTEST_NOWと書いておくと、最優先で実行されます。
ブレークポイントとかを立ててIDEで挙動を観察したりできます。
ただし、ひとつだけ注意点があります。
SEXYTEST_NOWが複数あると、どの最優先が最優先されるかは不定になります。
そのため、デバッグが終わったら、
ソースコードを commit する前に、 SEXYTEST_NOW() から SEXYTEST() に、戻してください。


これで、あなたのプロジェクトにもテストが導入されました。
*/
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
		#if _ARM
			#define SEXYTEST_BREAKPOINT()	{ asm ("trap") ; }
		#else
			#define SEXYTEST_BREAKPOINT()	{ asm ("int $3") ; }
		#endif
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

	//今、デバッグしているこのテストを真っ先に実行してくれ!
	#define SEXYTEST_NOW()  \
		static void SEXYTEST_LINE_STRCAT(SexyTestUnitTest,__LINE__)(); \
		static SexyTestUnitJoin SEXYTEST_LINE_STRCAT(SexyTestUnitJoin,__LINE__)(SEXYTEST_LINE_STRCAT(SexyTestUnitTest,__LINE__),SEXYTEST_TYPE_NOW); \
		static void SEXYTEST_LINE_STRCAT(SexyTestUnitTest,__LINE__)()

	//このテストは重いので、フルテストモードしかやれない. 1秒で終わらないものはこれにするべき
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