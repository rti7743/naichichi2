//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#include "common.h"
#include "Config.h"
#include "ScriptManager.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"
#include "SystemMisc.h"
#include "ScriptRemoconWeb.h"
#include "MainWindow.h"

#if _MSC_VER
	#include "resource1.h"
	#include "methodcallback.h"

	const char* HIDDEN_WINDOW_NAME = "hidden";
#endif	//_MSC_VER
const int WM_THREAD_CALLBACK		 = WM_USER + 1;
const int WM_USER_TASKTRAY_CLICK	 = WM_USER + 2;
const int WM_TASKBAR_CREATED		 = WM_USER + 3;

MainWindow* MainWindow::SingletonPoolMainWindow = NULL;

MainWindow::MainWindow()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	ASSERT(MainWindow::SingletonPoolMainWindow == NULL); //シングルトンです
	MainWindow::SingletonPoolMainWindow = this; //まーいいだろう。 (スレッド競合は気にせんでいい)

	this->WindowInstance = NULL;
	this->MainWindowHandle = NULL;
	this->IsOpenConsole = false;
#if WITH_CLIENT_ONLY_CODE==1
	this->isRecongpauseMode = false;
	this->isWhenTakingStopRecogn = false;
#endif

	//config読み込み
	this->ConfigBaseDirectory = XLStringUtil::pathcombine( XLFileUtil::getSelfExeDirectory() , "config" );
	std::string configfilename = GetConfigBasePath("config.conf");
	if (! this->Config.Create( configfilename ) )
	{
		//だめなら、カレントパスから・・・
		this->ConfigBaseDirectory = XLStringUtil::pathcombine( XLFileUtil::pwd() , "config" );
		configfilename = GetConfigBasePath("config.conf");
		if (! this->Config.Create( configfilename ) )
		{
			//configが読み込めない!
			puts("致命的エラー configが読み込めない!");
		}
	}

	const std::string logfilename = XLStringUtil::pathseparator( this->Config.Get("log__filename", GetConfigBasePath("log") + "/log.txt") );
	XLFileUtil::touch( logfilename );

	//loggerの準備
	sexylog::m()->SetLogFile( 
		 logfilename
		,this->Config.GetInt("log__bufferbyte",65535)
		,this->Config.GetInt("log__rotatebyte",1024*1024*1)
		);
	sexylog::m()->SetCanStdout(this->IsOpenConsole);

	SEXYLOG_LEVEL loglevel = (SEXYLOG_LEVEL) this->Config.GetInt("log__level",(int)SEXYLOG_LEVEL_NOTIFY);
	#if _DEBUG
	loglevel = SEXYLOG_LEVEL_DEBUG;
	#endif
	sexylog::m()->SetLogLevel( loglevel );

	//翻訳層の読み込み
	ReTranslateFile();
}
MainWindow::~MainWindow()
{

}


bool MainWindow::Create(bool isdebug)
{
	this->MainThreadID = ::this_thread::get_id();
	this->StopFlag = false;
	this->ExitLevel = EXITCODE_LEVEL_ERROR;

#if _MSC_VER
	this->WindowInstance = GetModuleHandle(0);
	WNDCLASS myProg;
	myProg.style            =CS_HREDRAW | CS_VREDRAW;
	myProg.lpfnWndProc      =methodcallback::registstdcall<struct _mainwindow_winproc,WNDPROC>(this, &MainWindow::WndProc);
	myProg.cbClsExtra       =0;
	myProg.cbWndExtra       =0;
	myProg.hInstance        =this->WindowInstance ;
	myProg.hIcon            =NULL;
	myProg.hCursor          =LoadCursor(NULL, IDC_ARROW);
	myProg.hbrBackground    = (HBRUSH)GetStockObject(WHITE_BRUSH);
	myProg.lpszMenuName     =NULL;
	myProg.lpszClassName    = HIDDEN_WINDOW_NAME;
	if (!RegisterClass(&myProg))
	{
		throw XLException("windowクラスの登録に失敗!");
	}

	this->MainWindowHandle = CreateWindow(HIDDEN_WINDOW_NAME
		,HIDDEN_WINDOW_NAME
		,WS_OVERLAPPEDWINDOW
		,CW_USEDEFAULT
		,CW_USEDEFAULT
		,400
		,400
		,NULL
		,NULL
		,this->WindowInstance 
		,NULL);
	if (isdebug)
	{
//		ShowWindow(this->MainWindowHandle,SW_SHOW);
//		UpdateWindow(this->MainWindowHandle);
	}

	this->MCIPlayStatus = 0;
#else
	this->MainWindowHandle = NULL;
	this->WindowInstance = NULL;
#endif

	if (isdebug)
	{
		OpenLogWindow();
	}
	else
	{
		CloseLogWindow();
	}

	if (! OnInit() )
	{
		ERRORLOG("OnInit に失敗しました.");
		return false;
	}
	return true;
}


EXITCODE_LEVEL MainWindow::BlockMessageLoop()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

#if _MSC_VER
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
#else
	while(!this->StopFlag)
	{
		{
			unique_lock<mutex> al(this->Lock);
			if (this->Queue.empty())
			{
				this->EventObject.wait(al);
			}
		}
		//寝起きかもしれないので終了条件の確認.
		if (this->StopFlag)
		{
			return EXITCODE_LEVEL_NORMALEND; //0
		}
		//メッセージをすべて再生
		RunMessagePump();
	}
#endif
	return this->ExitLevel;
}
LRESULT MainWindow::WndProc(HWND hWnd, unsigned int msg, WPARAM wParam, LPARAM lParam)
{
	//メッセージによる分岐
	switch (msg) {
		case WM_CREATE:
			if (! OnCreate() )
			{
				return -1;
			}
			break;
		case WM_DESTROY:
			OnDestory((EXITCODE_LEVEL)(int)wParam);
			break;
		case WM_THREAD_CALLBACK:
			if (wParam && lParam)
			{
				((_THREAD_CALLBACK)wParam)(lParam);
			}
			break;
#if _MSC_VER
		case WM_USER_TASKTRAY_CLICK:
			OnTaskTrayClick(wParam, lParam);
			break;
		case WM_TASKBAR_CREATED:
			OnTaskbarCreated(wParam, lParam);
			break;
		case WM_COMMAND:
			OnCommand(wParam, lParam);
			break;
		case MM_MCINOTIFY:
			this->MCIPlayStatus = 0;
			break;
		default:
			return(DefWindowProc(hWnd, msg, wParam, lParam));
#endif
	}
	return (0L);
}

void MainWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
#if _MSC_VER
	if (wParam == ID_MENU_QUIT)
	{//終了
		SendMessage(this->MainWindowHandle,WM_DESTROY,0,0);
	}
	else if (wParam == ID_MENU_DEBUG)
	{//デバッグメニューを開く
		if ( this->IsOpenConsole )
		{
			CloseLogWindow();
		}
		else
		{
			OpenLogWindow();
		}
	}
	else if (wParam == ID_MENU_WEB)
	{//web画面を開く
		string weburl = this->Httpd.getWebURL("/remocon/");
		ShellExecute(NULL,NULL,weburl.c_str() , NULL,NULL,0);
	}
#endif
}


//windows依存部
#if _MSC_VER
//通知領域に追加
bool MainWindow::AddNotifyIcon()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	//タスクトレイにアイコンを出す
	this->NotifyIcon.cbSize = sizeof(NOTIFYICONDATA);
	this->NotifyIcon.uID = 0;
	this->NotifyIcon.hWnd = this->MainWindowHandle;
	this->NotifyIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	this->NotifyIcon.hIcon = LoadIcon(this->WindowInstance,MAKEINTRESOURCE(IDI_ICON1));
	this->NotifyIcon.uCallbackMessage = WM_USER_TASKTRAY_CLICK;
	lstrcpyA( this->NotifyIcon.szTip, "ないちちボイスコマンダー by rti\r\nコマンドー。繰り返します。コマンドー。" );

	// アイコンを登録できるかエラーになるまで繰り返す
	while (true)
	{
		if (Shell_NotifyIcon(NIM_ADD, &this->NotifyIcon))
		{
			// 登録できたら終わり
			break;
		}
		else
		{
			// タイムアウトかどうか調べる
			if (::GetLastError() != ERROR_TIMEOUT)
			{
				// アイコン登録エラー
				return false;
			}

			// 登録できていないことを確認する
			if (Shell_NotifyIcon(NIM_MODIFY, &this->NotifyIcon))
			{
				// 登録できていた
				break;
			}
			else
			{
				// 登録できていなかった
				SecSleep(1);
			}
		}
	}
	return true;
}

//タスクトレイからアイコンを消す
bool MainWindow::DelNotifyIcon()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	::Shell_NotifyIcon( NIM_DELETE, &this->NotifyIcon );
	return true;
}

//タスクトレイのアイコンがクリックされたとき
LRESULT MainWindow::OnTaskTrayClick(WPARAM wParam, LPARAM lParam)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	// カーソルの現在位置を取得
	POINT point;
	::GetCursorPos(&point);

	switch(lParam) 
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		SetForegroundWindow(this->MainWindowHandle);
		{
			HMENU menu = LoadMenu( this->WindowInstance , MAKEINTRESOURCE(IDR_MENU2) );
			if (!menu)
			{
				break;
			}
			HMENU menu2 = GetSubMenu(menu,0);
			if (!menu2)
			{
				break;
			}
			TrackPopupMenu(menu2,TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,0,this->MainWindowHandle,NULL);
			PostMessage(this->MainWindowHandle,WM_NULL,0,0);
		}
	}
	return 0;
}

//シェルが死んだ時に飛んでくるメッセージ
LRESULT MainWindow::OnTaskbarCreated(WPARAM wParam, LPARAM lParam)
{
	this->AddNotifyIcon();
	return 0;
}

//MCIで音楽を再生する。再生が終わるまで待つ
void MainWindow::PlayMCISync(const string& filename )
{
#if WITH_CLIENT_ONLY_CODE==1
	MCI_OPEN_PARMS openParms = {0};
	MCI_PLAY_PARMS playParms = {0};
	MCIERROR  r;

	const string ext = XLStringUtil::baseext_nodot(filename);
	if (ext == "mp3")
	{
		openParms.lpstrElementName = filename.c_str();
		openParms.lpstrDeviceType = (LPTSTR)"MPEGVideo";

		r = mciSendCommand(0, MCI_OPEN,
			MCI_OPEN_TYPE | MCI_OPEN_ELEMENT,
			(DWORD)(LPVOID)&openParms);
		if (r != 0)
		{
			char errorMessage[MAX_PATH];
			mciGetErrorString(r,errorMessage,MAX_PATH);
			throw XLException("音楽を再生できません MCI_OPENに失敗(" + num2str(r) + " / " + errorMessage + ") filename:" + filename);
		}

	}
	else if (ext == "wav")
	{
		openParms.lpstrElementName = filename.c_str();
		openParms.lpstrDeviceType = (LPTSTR)MCI_DEVTYPE_WAVEFORM_AUDIO;

		r = mciSendCommand(0, MCI_OPEN,
			MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID | MCI_OPEN_ELEMENT,
			(DWORD)(LPVOID)&openParms);
		if (r != 0)
		{
			char errorMessage[MAX_PATH];
			mciGetErrorString(r,errorMessage,MAX_PATH);
			throw XLException("音楽を再生できません MCI_OPENに失敗(" + num2str(r) + " / " + errorMessage + ") filename:" + filename);
		}
	}
	else
	{
		throw XLException("サポートしていないファイル形式です。 filename:" + filename);
	}

	//再生中だと終わるまで待つ
	while (this->MCIPlayStatus == 1)
	{
		MCI_STATUS_PARMS mciStatus;
		mciStatus.dwItem = MCI_STATUS_MODE;
		mciSendCommand(openParms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&mciStatus);
		if (mciStatus.dwReturn != MCI_MODE_PLAY)
		{//メッセージは送られて来ないが実際の再生は終わっている
			break;
		}
		SecSleep(1); 
	}
	this->MCIPlayStatus = 1;
	
	playParms.dwFrom = 0;
	playParms.dwCallback = (DWORD_PTR) this->MainWindowHandle;
	r = mciSendCommand(openParms.wDeviceID, MCI_PLAY , MCI_NOTIFY ,
				(DWORD)(LPVOID)&playParms);
	if (r != 0)
	{
		char errorMessage[MAX_PATH];
		mciGetErrorString(r,errorMessage,MAX_PATH);
		throw XLException("音楽を再生できません MCI_PLAYに失敗(" + num2str(r) + "/ " + errorMessage + ") filename:" + filename);
	}

	if (::this_thread::get_id() == this->MainThreadID) 
	{
		ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドですね

		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (this->MCIPlayStatus == 0) break;

			MCI_STATUS_PARMS mciStatus;
			mciStatus.dwItem = MCI_STATUS_MODE;
			mciSendCommand(openParms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&mciStatus);
			if (mciStatus.dwReturn != MCI_MODE_PLAY)
			{
				break;
			}
			SecSleep(1); 
		}
	}
	else
	{
		while(1) 
		{
			if (this->MCIPlayStatus == 0) break;

			MCI_STATUS_PARMS mciStatus;
			mciStatus.dwItem = MCI_STATUS_MODE;
			mciSendCommand(openParms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&mciStatus);
			if (mciStatus.dwReturn != MCI_MODE_PLAY)
			{
				break;
			}
			SecSleep(1); 
		}
	}

	MCI_GENERIC_PARMS genericParam = {0};
	r = mciSendCommand(openParms.wDeviceID, MCI_CLOSE , 0 ,
				(DWORD)(LPVOID)&genericParam);

	return ;
#endif //WITH_CLIENT_ONLY_CODE
}

//MCIで音楽停止
void MainWindow::StopMCISync()
{
#if WITH_CLIENT_ONLY_CODE==1
	if (this->MCIPlayStatus == 1)
	{//再生中だと停止する
		mciSendCommand(0, MCI_STOP , 0 ,0);
	}
#endif //WITH_CLIENT_ONLY_CODE
}


#else
//linux依存部

//メッセージを処理する
void MainWindow::RunMessagePump()
{
	lock_guard<mutex> al(this->Lock);
	for(auto it = this->Queue.begin() ; it != this->Queue.end() ; ++it )
	{
		LRESULT r = WndProc(0, it->message,it->wParam,it->lParam);
		if (it->result)
		{
			it->result->set_value(r);
		}
	}
	this->Queue.clear();
}
LRESULT MainWindow::SendMessage(HWND dummyWindowHandle,unsigned int message,WPARAM wParam,LPARAM lParam)
{
	promise<LRESULT> resultP;
	future<LRESULT> future = resultP.get_future();

	struct Message m = {0};
	m.message = message;
	m.wParam = wParam;
	m.lParam = lParam;
	m.result = &resultP;
	
	//仕事を積む
	{
		lock_guard<mutex> al(this->Lock);
		this->Queue.push_back(m);
	}
	//スレッドwakeup
	{
		this->EventObject.notify_one();
	}

	if (this->MainThreadID == ::this_thread::get_id())
	{
		//メインスレッドであれば、メッセージポンプを回す。 そうじゃないと詰まってしまう。
		RunMessagePump();
	}
	
	//値を取得する(値を取得するまでブロック).
	return future.get();
}
void MainWindow::PostMessage(HWND dummyWindowHandle,unsigned int message,WPARAM wParam,LPARAM lParam)
{
	struct Message m = {0};
	m.message = message;
	m.wParam = wParam;
	m.lParam = lParam;
	m.result = NULL;

	//仕事を積む
	{
		lock_guard<mutex> al(this->Lock);
		this->Queue.push_back(m);
	}
	//スレッドwakeup
	{
		this->EventObject.notify_one();
	}
}


#endif

//終了時
void MainWindow::OnDestory(EXITCODE_LEVEL level)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	DEBUGLOG("プログラムを終了させます。");
	SyncFiles();
	DEBUGLOG("configセーブ完了。");

	//停止フラグを立てる.
	this->StopFlag = true;
	//終了レベルを記録
	this->ExitLevel = level;


#if _MSC_VER
	//今あるメッセージ・キューを空にする.
	{
		MSG msg;
		while(PeekMessage (&msg,NULL,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	DEBUGLOG("メッセージループ完了。");
#endif

	DEBUGLOG("HTTPD Stop...");
	this->Httpd.Stop();
	DEBUGLOG("HTTPD Stop");
#if WITH_SERVER_ONLY_CODE==1
	DEBUGLOG("ServerScript Stop...");
	this->ServerScript.DestoryScript();
	DEBUGLOG("ServerScript Stop");
#endif

#if WITH_CLIENT_ONLY_CODE==1
	DEBUGLOG("ScriptManager Stop...");
	this->ScriptManager.DestoryScript();
	DEBUGLOG("ScriptManager Stop");
	DEBUGLOG("SipServer Stop...");
	this->SipServer.Stop();
	DEBUGLOG("SipServer Stop");
#endif

#if _MSC_VER
	DEBUGLOG("DelNotifyIcon");
	//タスクトレイからアイコンを消す.
	DelNotifyIcon();
	DEBUGLOG("PostQuitMessage");
	//自滅用のメッセージを送信.
	PostQuitMessage(0);
#endif
	//ウィンドウハンドルのクリア.
	this->MainWindowHandle = NULL;
	DEBUGLOG("OnDestory Complate");
}

//作成された時
bool MainWindow::OnCreate()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	return true;
}



//Popupメッセージを出す.
void MainWindow::PopupMessage(const string & title,const string & message )
{
#if _MSC_VER
	//バルーンを出す.
	NOTIFYICONDATA	sData;
	ZeroMemory(&sData, sizeof(NOTIFYICONDATA));
	sData.cbSize		= sizeof(NOTIFYICONDATA);
	sData.uFlags		= NIF_INFO;
	sData.hWnd			= this->MainWindowHandle;
	sData.dwInfoFlags	= NIIF_INFO;
	sData.uTimeout		= 2000;

	lstrcpyA(sData.szInfoTitle,title.c_str() );
	lstrcpyA(sData.szInfo,  message.c_str() );

	Shell_NotifyIcon(NIM_MODIFY,&sData);
#endif
}

#include "XLHttpSocket.h"
#include "XLGZip.h"
#include "XLFileUtil.h"

//初期化を行う所
bool MainWindow::OnInit()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	
#if _MSC_VER
	bool ret = this->AddNotifyIcon();
	if (!ret)
	{
		return false;
	}
#endif

	NOTIFYLOG("naichichi2起動初期化シーケンススタート");


#if WITH_CLIENT_ONLY_CODE==1
	//前回の積み残しをやっていきます。
	{
		NOTIFYLOG("OnInit System..");

		//UUIDが作られていない場合は作ります。
		ScriptRemoconWeb::SettingUUIDOverriade();

		//webapiキーが設定されていなければ設定しなおします
		ScriptRemoconWeb::SettingWebAPIKeyOverriade();

		//FHC BOXとしての基礎設定を行います。
		if ( ! SystemMisc::SettingFHCBoxEnvSetup() )
		{
			NOTIFYLOG("SettingFHCBoxEnvSetupに失敗しました。システムを再起動します。");
			Shutdown(EXITCODE_LEVEL_REBOOT , true);
			return false;
		}

		#if _MSC_VER
		#else
				//configの状態をリセットするファイルの監視をします。
				NOTIFYLOG("OnInit ResetCoing..");
				ResetConfig.Create();
		#endif //_MSC_VER

		//CapeBordの構築
		NOTIFYLOG("OnInit CapeBord..");
		CapeBord.Create();
		//赤外線デバイスの構築
		NOTIFYLOG("OnInit CapeBord..");
		IRDevice.Create();
		//音源のボリューム設定
		VolumeControll.Create();
	}
#endif //WITH_CLIENT_ONLY_CODE

	//mecab
	{
		NOTIFYLOG("OnInit MecabControl..");
		const string mecabdir = this->Config.Get("mecab__mecabdir", GetConfigBasePath("mecab") );
		const string cmudictfilename = this->Config.Get("mecab__cmudict_filename", GetConfigBasePath("mecab/cmudict.0.7a") );
		MecabControl.Create(mecabdir,cmudictfilename);
	}

#if WITH_CLIENT_ONLY_CODE==1
	//GIS
	{
		NOTIFYLOG("OnInit GISAddress..");
		const string prefTSV = this->Config.Get("gisaddress__preftsv", GetConfigBasePath("gis/pref.tsv") );
		GISAddress.Create(prefTSV);
	}
	//認識エンジンの初期化(v8初期より前にやらないとダメ)
	try
	{
		NOTIFYLOG("OnInit ReCreateRecognitionEngine..");
		ReCreateRecognitionEngine();
	}
	catch(XLException &e)
	{
		ERRORLOG("音声認識の構築に失敗しました。　音声認識は利用できません。" << e.what() );
	}

	//合成音声
	{
		NOTIFYLOG("OnInit ReCreateSpeakEngine..");
		ReCreateSpeakEngine();
	}

	//センサー
	{
		NOTIFYLOG("OnInit ReCreateSensorEngine..");
		ReCreateSensorEngine();
	}

	//非同期音声読上げ
	{
		NOTIFYLOG("OnInit MusicPlayAsync..");
		this->MusicPlayAsync.Create();
	}

	//トリガー
	{
		NOTIFYLOG("OnInit Trigger..");
		this->Trigger.Create();
	}

	//シナリオスクリプト管理
	{
		NOTIFYLOG("OnInit ScriptManager..");
		this->ScriptManager.Create();
	}

	//音声認識エンジンへコミットする
	{
		DEBUGLOG("音声認識エンジンコミット開始");
		try
		{
			MainWindow::m()->Recognition.CommitRule();
		}
		catch(XLException& e)
		{//音声認識エンジンへコミットできませんでした
			ERRORLOG("致命的なエラー 音声認識エンジンへコミットできませんでした " << e.what() );
			//ここで例外を外に投げてしまうと、音声認識エンジンの設定を間違える無限再起動してしまうので、それはやらない。
			//ログに書いて保留する.
		}
		DEBUGLOG("音声認識エンジンコミット終了");
	}
	//後で音楽をかける機能を初期化します
	{
		DEBUGLOG("BackgroundMusic開始");
		if ( ! BackgroundMusic.Create() )
		{
			DEBUGLOG("後で音楽かける機能は利用できません。");
		}
	}

#endif //WITH_CLIENT_ONLY_CODE
	//Httpd
	{
		NOTIFYLOG("OnInit Httpd..");
		this->Httpd.Create();
	}

	//server
	{
#if WITH_SERVER_ONLY_CODE==1
		NOTIFYLOG("OnInit ServerScript..");
		this->ServerScript.Create();
#endif
	}
#if WITH_CLIENT_ONLY_CODE==1
	//unpn
	{
		NOTIFYLOG("OnInit UPNP Server..");
		this->UPNPServer.Create();
	}
	//econet lite
	{
		NOTIFYLOG("OnInit EconetLite Server..");
		this->EcoNetLiteServer.Create();
	}
	//homekit
	{
		NOTIFYLOG("OnInit HomeKit Server..");
		this->HomeKitServer.Create();
	}
	//sip
	{
		NOTIFYLOG("OnInit Sip Server..");
		this->SipServer.Create();
	}

	//クラウド
	{
		NOTIFYLOG("OnInit ColudSyncPooling..");
		const string server_url = this->Config.Get("server_url","https://fhc.rti-giken.jp/");
		const string id = this->Config.Get("account_usermail","");
		const string pass = this->Config.Get("account_password","");
		const string uuid = this->Httpd.GetUUID();
		const int sync_server = MainWindow::m()->Config.GetInt("account_sync_server",1);
		const string linkid = this->Config.Get("linksession","");

		this->ColudSyncPooling.Create(server_url,id,pass,uuid,(sync_server != 0) ,linkid);
	}
	//複数台連動
	{
		MultiRoom.Create();
	}


	//音声認識のon/off
	{
		this->UpdateRecongpause( stringbool( Config.Get("recongpause_mode","0") ) );
	}

	//起動音を鳴らす
	{
		if(	Config.GetInt("is_adin_alsa_fragment_error",0) )
		{//みっともないから起動音を鳴らさない,
			Config.Set("is_adin_alsa_fragment_error","0");
		}
		else
		{
			const string system_fhcboot_mp3 = MainWindow::m()->Config.Get("system_fhcboot_mp3", "fhcboot_tyaran.mp3" );
			MainWindow::m()->MusicPlayAsync.Play(CallbackPP::NoCallback() ,system_fhcboot_mp3 ,1);
		}
	}
	
	//バージョン番号が何かの理由がかかれていなければ書いて上げる
	{
		const string version_number = Config.Get("version_number","100");
		if (version_number == "100")
		{
			Config.Set("version_number",version_number);
		}
	}
#endif //WITH_CLIENT_ONLY_CODE
	NOTIFYLOG("naichichi2起動初期化シーケンス完了");


	return true;
}
void MainWindow::ReTranslateFile()
{
	this->TranslateFile.Create(GetConfigBasePath("trans") , this->Config.Get("system_lang","") );
}

#if WITH_CLIENT_ONLY_CODE==1

void MainWindow::ReCreateRecognitionEngine()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	this->Recognition.Create();
}

void MainWindow::ReCreateSpeakEngine()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	this->Speak.Create();
}

void MainWindow::ReCreateSensorEngine()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	//合成音声
	this->Sensor.Create();
}
#endif //WITH_CLIENT_ONLY_CODE

//バッファリングしているものをsyncする
void MainWindow::SyncFiles()
{
	DEBUGLOG("データをsyncします");

	this->Config.overrideSave();
	DEBUGLOG("configセーブ完了。");
#if WITH_CLIENT_ONLY_CODE==1
	this->Sensor.Flush();
	DEBUGLOG("sensorセーブ完了。");
#endif
	sexylog::m()->Flush();
	DEBUGLOG("logセーブ完了。");
}


//終了させる.
void MainWindow::Shutdown(EXITCODE_LEVEL level,bool isForce)
{
	//ファイルをすべて保存する
	SyncFiles();

	if (isForce)
	{//今すぐ叩き殺す!! メモリは OSが開放してくれるでしょうwww
	 //行儀は悪くても確実性を私は取りたい.
#if WITH_CLIENT_ONLY_CODE==1
		this->SipServer.Stop();
#endif
		exit((int)level);
	}
	else
	{//自滅するメッセージを送信
		PostMessage(
			 this->MainWindowHandle
			,WM_DESTROY
			,(WPARAM)level
			,(LPARAM)isForce
		);
	}
}


void MainWindow::SyncInvoke(std::function<void (void) > func)
{
	//停止フラグが有効な場合は無視する。
	if (this->StopFlag)
	{
		return ;
	}

	if (::this_thread::get_id() == this->MainThreadID) 
	{
		ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドですね
		//即実行
		func();
		return ;
	}

	struct callbackClass
	{
		callbackClass(std::function<void (void) > func) : func(func)
		{
			
		}
		static void callback(LPARAM p)
		{
			ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

			callbackClass* _this = (callbackClass*)p;
			_this->func();

			delete _this;
		}

		std::function<void (void) > func;
	};

	callbackClass *call = new callbackClass(func);
	SendMessage(
		 this->MainWindowHandle
		,WM_THREAD_CALLBACK
		,(WPARAM)callbackClass::callback
		,(LPARAM)call
	);
}

void MainWindow::AsyncInvoke(std::function<void (void) > func)
{
	//停止フラグが有効な場合は無視する。
	if (this->StopFlag)
	{
		return ;
	}

	struct callbackClass
	{
		callbackClass(std::function<void (void) > func) : func(func)
		{
			
		}
		static void callback(LPARAM p)
		{
			ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

			callbackClass* _this = (callbackClass*)p;
			_this->func();

			delete _this;
		}

		std::function<void (void) > func;
	};
	callbackClass *call = new callbackClass(func);
	PostMessage(
		 this->MainWindowHandle
		,WM_THREAD_CALLBACK
		,(WPARAM)callbackClass::callback
		,(LPARAM)call
	);
}

#if WITH_CLIENT_ONLY_CODE==1
void MainWindow::TopLevelInvoke(std::function<void (void) > func)
{
	//停止フラグが有効な場合は無視する。
	if (this->StopFlag)
	{
		return ;
	}

	MainWindow::m()->SyncInvoke([&](){
		try
		{
			//重い音声認識を止めます
			this->Recognition.Free();
			DEBUGLOG("this->Recognition.Free()");
			//常時内部LANのUPNPを受け止めているスレッドは止めます.
			this->UPNPServer.Stop();
			DEBUGLOG("this->UPNPServer.Stop()");
			//常時内部LANのEcoNetLiteを受け止めているスレッドは止めます.
			this->EcoNetLiteServer.Stop();
			DEBUGLOG("this->EcoNetLiteServer.Stop()");
			//SIPも止めます.
			this->SipServer.Stop();
			DEBUGLOG("this->SipServer.Stop()");

			func();
		}
		catch(XLException& e)
		{
			ERRORLOG("最優先実行できませんでした。例外 " << e.what() );
		}

		try
		{
			ReCreateRecognitionEngine();
			this->Recognition.CommitRule();
		}
		catch(XLException& e)
		{
			ERRORLOG("最優先実行後に音声認識エンジンを構築できませんでした。例外 " << e.what() );
		}
		
		try
		{
			this->UPNPServer.Create();
		}
		catch(XLException& e)
		{
			ERRORLOG("最優先実行後にUPNP SERVER を起動できません。例外 " << e.what() );
		}
		
		try
		{
			this->EcoNetLiteServer.Create();
		}
		catch(XLException& e)
		{
			ERRORLOG("最優先実行後にEcoNetLite SERVER を起動できません。例外 " << e.what() );
		}

		try
		{
			this->SipServer.Create();
		}
		catch(XLException& e)
		{
			ERRORLOG("最優先実行後にSip SERVER を起動できません。例外 " << e.what() );
		}
	});
}
#endif


//ログ用のコンソールを開きます。
bool MainWindow::OpenLogWindow()
{
#if _MSC_VER
	if ( this->IsOpenConsole )
	{
		return false;
	}

	//http://eternalwindows.jp/windevelop/console/console05.html
	//コンソールを作り、閉じるボタンを無効にする.
	if ( ! AllocConsole() )
	{
		throw XLException(XLException::StringWindows() + "コンソールを確保できません");
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

	//閉じるボタンを消します。
	HMENU hmenu = GetSystemMenu(GetConsoleWindow(), FALSE);
	RemoveMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);

	//開きました!
	this->IsOpenConsole = true;

	struct _ref
	{
		static void logger(const std::string& log)
		{
			HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

			//本文 長すぎる文章を一気に書くと表示されないので、一行づつくぎって表示する.
			std::list<std::string> l = XLStringUtil::split("\r\n",log);
			for(std::list<string>::iterator it = l.begin() ; it != l.end() ; ++it)
			{
				WriteConsole(handle , it->c_str() , it->size(), NULL, NULL);
				WriteConsole(handle , "\r\n" , 2 , NULL, NULL);
			}
		}
	};
	sexylog::m()->SetCustomLoggerFunc(_ref::logger);

#else
	this->IsOpenConsole = true;
#endif
	sexylog::m()->SetCanStdout(true);

	return true;
}

//ログ用のコンソールを閉じます.
void MainWindow::CloseLogWindow()
{
	sexylog::m()->SetCanStdout(false);
	sexylog::m()->SetCustomLoggerFunc(NULL);
#if _MSC_VER
	if (! this->IsOpenConsole )
	{
		return ;
	}

	FreeConsole();
#else
#endif
	this->IsOpenConsole = false;
	return ;
}

#if WITH_CLIENT_ONLY_CODE==1
//音声認識の一時停止のon/off (true->停止)
void MainWindow::UpdateRecongpause(bool isStop)
{
	Config.Set("recongpause_mode",isStop ? "1":"0");
	this->isRecongpauseMode = isStop;
	if (this->isRecongpauseMode)
	{
		NOTIFYLOG("音声認識は一時停止中です (recongpause_mode=1) ");
	}
}
//通話中は音声認識を一時停止するべきかどうか
void MainWindow::UpdateWhenTakingStopRecogn(int updateWhenTakingStopRecogn,bool isStop)
{
	if (updateWhenTakingStopRecogn == 0)
	{//通話中でも全部有効にする
		if (isStop)
		{
			NOTIFYLOG("通話中ですが、音声認識は停止しません");
		}
		return ;
	}
	else if (updateWhenTakingStopRecogn == 1)
	{//電話機能以外有効
		this->isWhenTakingStopRecogn = isStop;
		if (this->isWhenTakingStopRecogn)
		{
			NOTIFYLOG("通話中なので音声認識を一時停止中します");
		}
	}
	else 
	{//通話中では音声認識を停止する.
		this->isWhenTakingStopRecogn = isStop;
		if (isStop)
		{
			NOTIFYLOG("通話中なので音声認識を停止します");
			MainWindow::m()->SyncInvoke([&](){
				//音声認識を止めます.
				try
				{
					this->Recognition.Free();
				}
				catch(XLException& e)
				{
					ERRORLOG("通話中なので音声認識を止めようとしましたが、失敗しました。 例外 " << e.what() );
				}
			});
		}
		else
		{
			MainWindow::m()->SyncInvoke([&](){
				//音声認識を開始します
				try
				{
					if(! this->Recognition.isCretated() )
					{
						ReCreateRecognitionEngine();
						this->Recognition.CommitRule();
					}
				}
				catch(XLException& e)
				{
					ERRORLOG("通話完了後に音声認識エンジンを構築できませんでした。例外 " << e.what() );
				}
			});
		}
	}


}
#endif

//config の中のパスを作る
string MainWindow::GetConfigBasePath(const string& path) const
{
	return XLStringUtil::pathcombine( this->ConfigBaseDirectory , path);
}

