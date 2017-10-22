#pragma once

#include "common.h"
#include "Config.h"
#include "ScriptManager.h"
#include "Recognition_Factory.h"
#include "Speak_Factory.h"
#include "Sensor_Factory.h"
#include "HttpServer.h"
#include "MecabControl.h"
#include "ColudSyncPooling.h"
#include "ServerScript.h"
#include "ResetConfig.h"
#include "sexylog.h"
#include "MusicPlayAsync.h"
#include "TranslateFile.h"
#include "Trigger.h"
#include "XLGISAddress.h"
#include "BackgroundMusic.h"
#include "CapeBord.h"
#include "IRDevice.h"
#include "VolumeControll.h"
#include "UPNPServer.h"
#include "HomeKitServer.h"
#include "MultiRoom.h"
#include "EcoNetLiteServer.h"
//#include "SipServer.h"
#include "SystemMisc.h"

enum EXITCODE_LEVEL
{
	 EXITCODE_LEVEL_REBOOT   = 10 //プログラムを終了して、OSごと再起動します
	,EXITCODE_LEVEL_SHUTDOWN = 20 //プログラムを終了して、システムをシャットダウンします
	,EXITCODE_LEVEL_UPGRADE  = 30 //プログラムを終了して、アップデートを実行します
	,EXITCODE_LEVEL_ERROR    = 5  //プログラムはエラーで死にました。ザオリクします。 
	,EXITCODE_LEVEL_NORMALEND= 0  //プログラムは正常に終了しました。
};

class MainWindow
{
public:
	MainWindow();
	virtual ~MainWindow();

	bool Create(bool isdebug);
	void Shutdown(EXITCODE_LEVEL level,bool isForce);
	void ReCreateRecognitionEngine();
	void ReCreateSpeakEngine();
	void ReCreateSensorEngine();
	void ReTranslateFile();


	EXITCODE_LEVEL BlockMessageLoop();

	//初期化を行う所
	typedef void (*_THREAD_CALLBACK)(int p);
	//メインスレッドコールバック 関数オブジェクト
	void SyncInvoke(std::function<void (void) > func);
	//メインスレッドで後で実行するコールバック
	void AsyncInvoke(std::function<void (void) > func);

	//config の中のパスを作る
	string GetConfigBasePath(const string& path) const;
	//バッファリングしているものをsyncする
	void SyncFiles();
	//Popupメッセージを出す.
	void PopupMessage(const string & title,const string & message);
	//音楽再生
	void Play(const string& filename )
	{
#if WITH_CLIENT_ONLY_CODE==1
		SystemMisc::Play(filename);
#endif 
	}
	//音楽停止
	void StopPlay()
	{
#if WITH_CLIENT_ONLY_CODE==1
		SystemMisc::StopPlay();
#endif 
	}

#if WITH_CLIENT_ONLY_CODE==1
	//音声認識を一時停止しているかどうか
	bool IsRecongpause() const
	{
		return this->isRecongpauseMode || this->isWhenTakingStopRecogn;
	}
	//音声認識一時停止中モードかどうか
	bool IsRecongpauseMode() const
	{
		return this->isRecongpauseMode;
	}
	//通話中は音声認識を一時停止するかどうか
	bool IsWhenTakingStopRecogn() const
	{
		return this->isWhenTakingStopRecogn;
	}
	//音声認識の一時停止のon/off (true->停止)
	void UpdateRecongpause(bool isStop);
	//通話中は音声認識を一時停止するべきかどうか
	void UpdateWhenTakingStopRecogn(int updateWhenTakingStopRecogn,bool isStop);

#endif



	static MainWindow* m()
	{
		return MainWindow::SingletonPoolMainWindow;
	}

#if _MSC_VER
	//MCIで音楽を再生する。再生が終わるまで待つ
	void PlayMCISync(const string& filename );
	//MCIで音楽停止
	void StopMCISync();
#endif

private:
	virtual LRESULT WndProc(HWND hWnd, unsigned int msg, WPARAM wParam, LPARAM lParam);
	bool OnInit();

	//終了時
	void OnDestory(EXITCODE_LEVEL level);
	//作成された時
	bool OnCreate();
	//ログ用のコンソールを開きます。
	bool OpenLogWindow();
	//ログ用のコンソールを閉じます.
	void CloseLogWindow();
	void OnCommand(WPARAM wParam, LPARAM lParam);


#if _MSC_VER
//windows依存部
	//通知領域に追加
	bool AddNotifyIcon();
	//タスクトレイからアイコンを消す
	bool DelNotifyIcon();
	//タスクトレイのアイコンがクリックされたとき
	LRESULT OnTaskTrayClick(WPARAM wParam, LPARAM lParam);
	//シェルが死んだ時に飛んでくるメッセージ
	LRESULT OnTaskbarCreated(WPARAM wParam, LPARAM lParam);

#else
//linux依存部
	//windowsの SendMessage相当のことをする
	LRESULT SendMessage(HWND dummyWindowHandle,unsigned int message,WPARAM wParam,LPARAM lParam);
	//windowsの PostMessage相当のことをする
	void PostMessage(HWND dummyWindowHandle,unsigned int message,WPARAM wParam,LPARAM lParam);
	//メッセージを処理する
	void RunMessagePump();
#endif

public:
	class Config					Config;				//configオプション.
	class HttpServer				Httpd;				//httpdサーバー.
	class MecabControl				MecabControl;		//mecabを全体で使えるように.
	class ResetConfig				ResetConfig;		//ファイル監視してconfigをリセットする初期化回り.
	class TranslateFile				TranslateFile;		//翻訳層
	class VolumeControll			VolumeControll;		//音源のボリューム調整
#if WITH_CLIENT_ONLY_CODE==1
	class UPNPServer				UPNPServer;			//UPNPの受け答え
	class EcoNetLiteServer			EcoNetLiteServer;	//EcoNetLite関係
	class HomeKitServer				HomeKitServer;		//Homekitサポート
//	class SipServer					SipServer;			//sip電話サポート
	class MultiRoom					MultiRoom;			//複数台連携 同期処理
	class ScriptManager				ScriptManager;		//内蔵スクリプトとかwebとかの管理
	class Recognition_Factory		Recognition;		//音声認識.
	class Speak_Factory				Speak;				//合成音声.
	class Sensor_Factory            Sensor;				//センサー.
	class ColudSyncPooling			ColudSyncPooling;	//config同期とリモート操作
	class MusicPlayAsync			MusicPlayAsync;		//非同期音楽再生
	class Trigger					Trigger;			//トリガー
	class XLGISAddress				GISAddress;			//住所変換ライブラリ
	class BackgroundMusic			BackgroundMusic;	//後で音楽やニュースを再生するインターフェース
	class CapeBord					CapeBord;			//拡張ボード.
	class IRDevice					IRDevice;			//赤外線デバイス
#endif

#if WITH_SERVER_ONLY_CODE
	class ServerScript				ServerScript;
#endif

private:
	::thread::id MainThreadID;
	bool IsOpenConsole;
	bool StopFlag;
	HINSTANCE WindowInstance;
	HWND MainWindowHandle;
	EXITCODE_LEVEL ExitLevel;
	string ConfigBaseDirectory;
	static MainWindow* SingletonPoolMainWindow;

#if WITH_CLIENT_ONLY_CODE==1
	//音声認識を一時停止しているかどうか
	bool isRecongpauseMode;
	//通話中は音声認識を一時停止するべきかどうか
	bool isWhenTakingStopRecogn;
#endif

#if _MSC_VER
//windows依存部
	NOTIFYICONDATA NotifyIcon;
	int MCIPlayStatus;
#else
//linuxでもwindowsみたいなメッセージループを擬似的に作る
	mutex   Lock;
	condition_variable EventObject;
	struct Message
	{
		unsigned int	message;
		WPARAM	wParam;
		LPARAM	lParam;
		promise<LRESULT>*	result;
	};
	list<Message> Queue;
#endif
};

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

class V8Init
{
public:
	V8Init()
	{
#if WITH_CLIENT_ONLY_CODE==1
	v8::V8::Initialize();
#endif
	}
	virtual ~V8Init()
	{
#if WITH_CLIENT_ONLY_CODE==1
	v8::V8::Dispose();
#endif
	}
};
