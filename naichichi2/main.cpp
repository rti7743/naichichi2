//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#include "common.h"
#include "MainWindow.h"
#include "SystemMisc.h"

#if _MSC_VER
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst,
                   LPSTR lpszCmdLine, int nCmdShow)
#else
int main(int argc, const char **argv)
#endif
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません


#if _MSC_VER && _DEBUG
//	_CrtSetBreakAlloc(144);
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//	_CrtDumpMemoryLeaks();
#endif
	EXITCODE_LEVEL ret = EXITCODE_LEVEL_ERROR;
	if ( SystemMisc::IsDoubleBoot())
	{
		ERRORLOG( "多重起動です。" );
		return -30;
	}

	{
		COMInit cominit;
		WinSockInit winsockinit;
		V8Init v8init;

		{
			MainWindow win;

			#if _DEBUG
				//デバッグビルドだと、テストをすべて実行する.
				//必ずテストをやって通らないコードは実行しない方針.
				SEXYTEST_RUNNER();
			#endif //#if _DEBUG


			bool isdebug = true;
#if _MSC_VER && !_DEBUG
			//windowsでデバッグビルドでなければコンソールを出さない.
			isdebug = false;
#endif

			//これ以降、例外を受け取った時にログを書いて死ぬようにする.
			XLDebugUtil::HaikuWoYome();

			//引数 --nostdout がなければ debug モードとしてログを吐きまくる
			{
#if _MSC_VER
				const auto argsMap = SystemMisc::ArgsToMap(lpszCmdLine);
#else
				const auto argsMap = SystemMisc::ArgsToMap(argc,argv);
#endif
				if (argsMap.find("--nostdout") != argsMap.end())
				{
					isdebug = false;
				}
			}

			//メインウィンドウの生成.
			if ( win.Create(isdebug) )
			{
				//メインループ
				ret = win.BlockMessageLoop();
			}
		}
	}

	return (int)ret;
}
