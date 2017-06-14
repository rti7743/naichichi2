//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include "Fire.h"
#include "MainWindow.h"
#include "SystemMisc.h"
#include "XLStringUtil.h"
#include "ScriptRemoconWeb.h"
#include "XLFileUtil.h"
#include "MultiRoomUtil.h"
#include "NetDevice.h"
#include "SipServer.h"
#include "ActionImplement.h"

//循環参照で無限ループにならないように、マイクロの呼び出し回数に上限を設ける
const int NAICHICHI_MACRO_MAX_CALLCOUNT = 30;

Fire::Fire()
{
}
Fire::~Fire()
{
}

bool Fire::FireAction(int key1,int key2)
{
	try
	{
		int nestcount = 0;
		fireAction(key1,key2,&nestcount);
	}
	catch(XLException & e)
	{
		this->ErrorMessage = e.what();
		ERRORLOG("ERR:" << e.what() );
		return false;
	}
	return true;
}

void Fire::fireAction(int key1,int key2,int* nestcount)  
{
	const auto configmap = MainWindow::m()->Config.FindGetsToMap("elec_",false);
	const string prefix = string("elec_") + num2str(key1) + "_action_" + num2str(key2);

	
	bool updateStatus = true;	//ステータスを更新するかどうか
	bool isSync = true;			//configを鯖と同期するかどうか？

	if (*nestcount >= 1)
	{
		//マクロ内実行なのでステータスを更新しない
		//		updateStatus = false;
		//マクロ内実行なのでconfigを鯖と同期しない
		isSync = false;
	}
	if ( stringbool( mapfind(configmap , prefix + "_nostateremocon") ) )
	{//ステータス変更を禁止されている場合
		updateStatus = false;
	}

	const string execflag = mapfind(configmap , prefix + "_execflag");
	if ( execflag == "ir" )
	{
		const string exec_ir = mapfind(configmap , prefix + "_exec_ir");
		const int r = remocon_ir_fire(exec_ir);
		if (r <= 0)
		{
			if (r == -1)
			{
				throw XLEXCEPTION( "赤外線データがありません。最初にリモコンの記録を行なってください。");
			}
			else if (r == -2)
			{
				throw XLEXCEPTION( "赤外線を送信できるデバイスがありません。");
			}
			else if (r == -3)
			{
				throw XLEXCEPTION("赤外線データをロードできませんでした。もう一度、リモコンから学習させ直してください。");
			}
			else if (r == -4)
			{
				throw XLEXCEPTION( "赤外線データを送信出来ませんでした");
			}
			throw XLEXCEPTION( "不明なエラーにより赤外線を送信出来ませんでした。エラーコード:" <<r);
		}
	}
	else if ( execflag == "command" )
	{
		const string execcommand = mapfind(configmap , prefix + "_exec_command");
		const string execargs1 = mapfind(configmap , prefix + "_exec_command_args1");
		const string execargs2 = mapfind(configmap , prefix + "_exec_command_args2");
		const string execargs3 = mapfind(configmap , prefix + "_exec_command_args3");
		const string execargs4 = mapfind(configmap , prefix + "_exec_command_args4");
		const string execargs5 = mapfind(configmap , prefix + "_exec_command_args5");

		string stdout_;
		run_commandE(execcommand,execargs1,execargs2,execargs3,execargs4,execargs5,&stdout_); //エラー時 例外
	}
	else if ( execflag == "macro" )
	{
		run_macroE(configmap, prefix + "_exec_macro_" , nestcount);
	}
	else if ( execflag == "timer" )
	{
		const string timer_type = mapfind(configmap , prefix + "_timer_type");
		run_set_electimer(timer_type,key1,key2);
	}
	else if ( execflag == "recongpause" )
	{
		const string recongpause_type = mapfind(configmap , prefix + "_recongpause_type");
		run_recongpause(recongpause_type);
	}
	else if ( execflag == "multiroom" )
	{
		const string multiroom_room = mapfind(configmap , prefix + "_multiroom_room");
		const string multiroom_elec = mapfind(configmap , prefix + "_multiroom_elec");
		const string multiroom_action = mapfind(configmap , prefix + "_multiroom_action");
		string stdout_;
		run_multiroom(multiroom_room,multiroom_elec,multiroom_action,&stdout_);
	}
	else if ( execflag == "netdevice" )
	{
		const string netdevice_elec = mapfind(configmap , prefix + "_netdevice_elec");
		const string netdevice_action = mapfind(configmap , prefix + "_netdevice_action");
		const string netdevice_value = mapfind(configmap , prefix + "_netdevice_value");
		run_netdev(netdevice_elec,netdevice_action,netdevice_value);
	}
	else if ( execflag == "sip" )
	{
		const string sip_action_type = mapfind(configmap , prefix + "_sip_action_type");
		const string sip_call_number = mapfind(configmap , prefix + "_sip_call_number");
		run_sip(sip_action_type,sip_call_number);
	}

	//読み上げ処理
	const string tospeak_select = mapfind(configmap , prefix + "_tospeak_select");
	if (tospeak_select == "string")
	{
		const string tospeak_string = mapfind(configmap , prefix + "_tospeak_string");
		MainWindow::m()->SyncInvoke( [&](){
			MainWindow::m()->Speak.SpeakAsync(CallbackPP::NoCallback() ,tospeak_string);
		});
	}
	else if (tospeak_select == "musicfile")
	{
		const string tospeak_mp3 = mapfind(configmap , prefix + "_tospeak_mp3");

		MainWindow::m()->MusicPlayAsync.Play(CallbackPP::NoCallback() ,tospeak_mp3 ,1);
	}

	//ステータスの変更
	if (updateStatus)
	{
		const string actiontype = mapfind(configmap , prefix + "_actiontype");
		MainWindow::m()->Config.Set(string("elec_") + num2str(key1) + "_status" , actiontype );
		DEBUGLOG("機材 " << key1 << " のステータスを " << actiontype << " に変更します。");
		MainWindow::m()->HomeKitServer.NotifyAccessory();
	}
	//sync
	if (isSync)
	{
		//温度と明るさのセンサーの値を更新する
		MainWindow::m()->Sensor.getSensorNow();

		const string actioncolor = mapfind(configmap , prefix + "_actioncolor");
		MainWindow::m()->Config.Set(string("elec_") + num2str(key1) + "_statuscolor" , actioncolor );
		ConfigUpdated();
	} 
}
//Configを変更した場合
void Fire::ConfigUpdated()
{
	//configの保存
	MainWindow::m()->SyncInvoke([=](){
		MainWindow::m()->Config.overrideSave();
	});
	//configの同期を行う
	MainWindow::m()->ColudSyncPooling.WakeupSyncConfigByconfigOnly();
}

//家電タイマーのセット(おやす○タイマー)
void Fire::run_set_electimer(const string& timer_type,unsigned int key1,unsigned int key2)
{
	const time_t now = time(NULL);
	const string elecPrefix       = string("elec_") + num2str(key1) ;
	const string elecActionPrefix = string("elec_") + num2str(key1) + "_action_" + num2str(key2);

	if (timer_type == "reset")
	{//トリガーの取り消し
		MainWindow::m()->Config.Remove(elecPrefix + "_timer_firetime" );
		MainWindow::m()->Config.Remove(elecPrefix + "_timer_firekey" );

		NOTIFYLOG("家電タイマー(" << elecActionPrefix << ")を取り消します");
	}
	else
	{
		const time_t fireTime = atoi(timer_type) + (now/60);
		MainWindow::m()->Config.Set(elecPrefix + "_timer_firetime", num2str(fireTime) );
		MainWindow::m()->Config.Set(elecPrefix + "_timer_firekey", num2str(key2) );

		NOTIFYLOG("家電タイマー(" << elecActionPrefix << ")を登録します");
	}

	//リロードする.
	MainWindow::m()->ScriptManager.ReloadTrigger(now);
}



bool Fire::FireByString(const string& str)
{
	if (str.empty()) return false;

	//音素文字列に変換します.
	const string yomi = MainWindow::m()->Recognition.convertYomi(str);

	//呼びかけの読みを取得して、すべて読みに変換し直します
	list<string> yobikakeListArray = MainWindow::m()->Config.FindGets("recong__yobikake__");
	for(auto yit = yobikakeListArray.begin() ; yit != yobikakeListArray.end() ; ++yit )
	{
		if (!yit->empty())
		{
			*yit = MainWindow::m()->Recognition.convertYomi(*yit) + " "; //次にコマンドが続くのでスペースが必須
		}
	}

	//メインスレッドでしか触れない領域なので、コピーする。 そのコストは支払う
	list<RecongTask> recongtask;
	MainWindow::m()->SyncInvoke( [&](){
		recongtask = *MainWindow::m()->Recognition.getAllCommandRecongTask();
	});

	//電気消してー
	CallbackPP callback;
	for(auto it = recongtask.begin() ; it != recongtask.end() ; ++it )
	{
		for(auto yit = yobikakeListArray.begin() ; yit != yobikakeListArray.end() ; ++yit )
		{//よびかけ + よみで検索
			if ( (!yit->empty()) && yomi == *yit + it->yomiString )
			{
				callback = it->callback;
				break;
			}
		}

		if (!callback.Empty())
		{//よびかけ + よみでヒットしたら終了
			break;
		}
		if ( yomi == it->yomiString )
		{//呼びかけをいれない部分でも検索してみる
			callback = it->callback;
			break;
		}
	}

	if (callback.Empty())
	{
		return false;
	}

	MainWindow::m()->SyncInvoke( [&](){
		MainWindow::m()->ScriptManager.VoiceRecogntion( callback );
	});
	DEBUGLOG(str << "音声認識完了しました。");

	return true;
}

//実行!!
bool Fire::FireByType(const string& type1,const string& type2) 
{
	try
	{
		int nestcount = 0;
		return fireStringAction(type1,type2,&nestcount);
	}
	catch(XLException & e)
	{
		this->ErrorMessage = e.what();
		ERRORLOG("ERR:" << e.what() );
		return false;
	}
}

bool Fire::fireStringAction(const string& type1,const string& type2 , int* nestcount) 
{
	//システムで予約されているものはfireできません。
	if (type1 == "" || type1 == "none" || type1 == "SPACE" ) return false;
	if (type2 == "" || type2 == "none" || type2 == "SPACE" ) return false;

	assert(nestcount != NULL);
	const auto configmap = MainWindow::m()->Config.FindGetsToMap("elec_",false);

	int key1 ,key2 ;
	if( ! ScriptRemoconWeb::type2key(configmap,type1,type2,&key1,&key2) )
	{
		return false;
	}

	fireAction(key1,key2,  nestcount);
	return true;
}

//コマンドを実効する
bool Fire::run_command(const string& execcommand,const string& execargs1,const string& execargs2,const string& execargs3,const string& execargs4,const string& execargs5,string* appendSTDOUT)
{
	try
	{
		run_commandE(execcommand,execargs1,execargs2,execargs3,execargs4,execargs5,appendSTDOUT);
	}
	catch(XLException & e)
	{
		this->ErrorMessage = e.what();
		ERRORLOG("ERR:" << e.what() );
		return false;
	}
	return true;
}

//コマンドを実効する
string Fire::run_commandE(const string& execcommand,const string& execargs1,const string& execargs2,const string& execargs3,const string& execargs4,const string& execargs5,string* appendSTDOUT)
{
	if ( ! XLStringUtil::checkSafePath(execcommand) )
	{
		throw XLEXCEPTION("ファイル名がセキュアではありません");
	}
	NOTIFYLOG("コマンド " << execcommand << "(" << execargs1 << ","<< execargs2 << ","<< execargs3 << ","<< execargs4 << ","<< execargs5 << ")" );

	
	const auto fullpath = MainWindow::m()->GetConfigBasePath("./webroot/user/script_command/" + execcommand);

	list<string> args;
	if (! (execargs1.empty() && execargs2.empty() && execargs3.empty() && execargs4.empty() && execargs5.empty())  )
	{
		args.push_back(execargs1);
		if (! (execargs2.empty() && execargs3.empty() && execargs4.empty() && execargs5.empty())  )
		{
			args.push_back(execargs2);
			if (! (execargs3.empty() && execargs4.empty() && execargs5.empty())  )
			{
				args.push_back(execargs3);
				if (! (execargs4.empty() && execargs5.empty())  )
				{
					args.push_back(execargs4);
					if (! execargs5.empty()  )
					{
						args.push_back(execargs5);
					}
				}
			}
		}
	}

	string result;
	string exceptionMessage;
	MainWindow::m()->SyncInvoke([&](){
		try
		{
			ScriptRunner runner( false );
			runner.LoadScript(fullpath);
			result = runner.callFunction("main",args);
			*appendSTDOUT += runner.getSTDOUT();
		}
		catch(XLException&e)
		{
			exceptionMessage = e.what();
			ERRORLOG(exceptionMessage );
		}
	});

	if (!exceptionMessage.empty())
	{
		this->ErrorMessage = exceptionMessage;
		throw XLEXCEPTION("実行に失敗しました。" << exceptionMessage );
	}
	return result;
}

//マクロを実行する
bool Fire::run_macro(const map<string,string>& request , const string& prefix)
{
	try
	{
		run_macroE(request ,prefix);
	}
	catch(XLException & e)
	{
		this->ErrorMessage = e.what();
		ERRORLOG("ERR:" << e.what() );
		return false;
	}
	return true;
}

//マクロを実行する
void Fire::run_macroE(const map<string,string>& request , const string& prefix)
{
	int nestcount = 1;
	run_macroE(request ,prefix,&nestcount);
}
void Fire::run_macroE(const map<string,string>& request , const string& prefix,int* nestcount)
{
	DEBUGLOG("マクロを実行します prefix:" << prefix);

	for (int macrocount = 1 ; macrocount <= 9 ; macrocount++ )
	{
		const string exec_macro_elec = mapfind(request,  prefix + num2str(macrocount) + "_elec");
		const string exec_macro_action = mapfind(request,prefix + num2str(macrocount) + "_action");
		const string exec_macro_after = mapfind(request, prefix + num2str(macrocount) + "_after");

		DEBUGLOG("マクロ評価 elec:" << exec_macro_elec << " action:" << exec_macro_action << " after:" << exec_macro_after);
		if (exec_macro_elec == "none" || exec_macro_action == "none")
		{
			DEBUGLOG("マクロ評価 none何もしない elec:" << exec_macro_elec << " action:" << exec_macro_action << " after:" << exec_macro_after);
			continue;
		}
		
		bool runfire = true;
		if (exec_macro_after == "ifnext")
		{//状態が同じなら動かずに次へ
			if ( ScriptRemoconWeb::getElecStatus(exec_macro_elec) == exec_macro_action )
			{
				DEBUGLOG("マクロ評価 ifnextにより何もしない elec:" << exec_macro_elec << " action:" << exec_macro_action << " after:" << exec_macro_after);
				runfire = false;
			}
		}

		if (runfire)
		{
			DEBUGLOG("マクロ評価 命令実行 elec:" << exec_macro_elec << " action:" << exec_macro_action << " after:" << exec_macro_after);
			fireStringAction(exec_macro_elec,exec_macro_action,nestcount);

			if (exec_macro_after.substr(0,5) == "wait_")
			{//待機命令？
				size_t waittimes = atoi( exec_macro_after.c_str() + (sizeof("wait_") - 1) );
				if (waittimes >= 60) waittimes = 10;
				if (waittimes <= 0)  waittimes = 1;

				SecSleep( waittimes + 1 );
			}
			else
			{//それ以外の命令
				//連続して赤外線を放射できないのでちょっと待ちます。
				SecSleep( 1 );
			}
		}
		
		*nestcount = *nestcount + 1;
		if (*nestcount > NAICHICHI_MACRO_MAX_CALLCOUNT)
		{//エラーマクロ多すぎ
			throw XLEXCEPTION("マクロのネストが深すぎます");
		}
	}
}


//循環参照になって無限ループにならないようにマクロの上限をチェックする
bool Fire::check_macroE(const string& type1,const string& type2 , int* nestcount)  const
{
	int key1 ,key2 ;
	const auto configmap = MainWindow::m()->Config.FindGetsToMap("elec_",false);
	if(! ScriptRemoconWeb::type2key(configmap,type1,type2,&key1,&key2) )
	{
		return true;
	}

	//呼び出し回数が増える
	*nestcount += 1;
	if (*nestcount > NAICHICHI_MACRO_MAX_CALLCOUNT)
	{
		return false;
	}

	const string prefix = string("elec_") + num2str(key1) + "_action_" + num2str(key2);

	const string execflag = mapfind(configmap , prefix + "_execflag");
	if ( execflag != "macro" )
	{
		return true;
	}
	for (int macrocount = 1 ; macrocount <= 9 ; macrocount++ )
	{
		const string exec_macro_elec = mapfind(configmap,prefix + "_exec_macro_" + num2str(macrocount) + "_elec");
		const string exec_macro_action = mapfind(configmap,prefix + "_exec_macro_" + num2str(macrocount) + "_action");

		if (exec_macro_elec == "none" || exec_macro_action == "none")
		{
			continue;
		}

		*nestcount = *nestcount + 1;
		if (*nestcount > NAICHICHI_MACRO_MAX_CALLCOUNT)
		{//エラーマクロ多すぎ
			return false;
		}
		if (!  check_macroE(exec_macro_elec,exec_macro_action , nestcount)  )
		{//再度呼び出したマクロの中で呼び出し回数をオーバーした
			return false;
		}
	}
	return true;
}

void Fire::run_recongpause(const string& recongpause_type)
{
	if (recongpause_type == "stop")
	{//一時停止
		NOTIFYLOG("音声認識を一時停止にします");
		MainWindow::m()->UpdateRecongpause(true);
	}
	else if (recongpause_type == "start")
	{//再開
		NOTIFYLOG("音声認識を再開します");
		MainWindow::m()->UpdateRecongpause(false);
	}
	else
	{
		NOTIFYLOG("音声認識一時停止のrecongpause_typeが不明("<<recongpause_type<<")です");
	}
	//わかりづらいので、LEDを点灯させる.
	MainWindow::m()->CapeBord.SleepAndBrightLED(5,200);
}

void Fire::run_multiroom(const string& uuid,const string& elec,const string& action,string* outStdout)
{
	

	const auto configmap = MainWindow::m()->Config.FindGetsToMap("multiroom_",false);
	const int key = MultiRoomUtil::FindRoom(uuid,configmap);
	if(key < 0 )
	{
		throw XLEXCEPTION("UUIDが正しくありません");
	}
	const string your_authkey = MainWindow::m()->Config.Get("multiroom_"+num2str(key)+"_authkey", "" );
	const string your_url = MainWindow::m()->Config.Get("multiroom_"+num2str(key)+"_url", "" );

	const string get = 
	"?webapi_apikey=" + XLStringUtil::urlencode(your_authkey)
	+ "&elec=" + XLStringUtil::urlencode(_A2U(elec))
	+ "&action=" + XLStringUtil::urlencode(_A2U(action))
	;

	const string url = XLStringUtil::urlcombine(your_url , "/api/elec/action");
	string your_result;
	map<string,string> header;
	*outStdout = ActionImplement::HttpGet(url + get,header,3);
}

bool Fire::run_netdev(const string& elec,const string& action,const string& value)
{
	NetDevice netdev;
	return netdev.Fire(elec,action,value);
}

bool Fire::run_sip(const string& sip_action_type,const string& sip_call_number)
{
	if (sip_action_type == "call")
	{
		MainWindow::m()->SipServer.Call(sip_call_number);
		return true;
	}
	else if (sip_action_type == "answer")
	{
		MainWindow::m()->SipServer.Answer();
		MainWindow::m()->SipServer.WaitForRequest(5);
		return true;
	}
	else if (sip_action_type == "hangup")
	{
		MainWindow::m()->SipServer.Hangup();
		MainWindow::m()->SipServer.WaitForRequest(5);
		return true;
	}
	else if (sip_action_type == "numpad")
	{
		const time_t now = time(NULL);
		MainWindow::m()->SipServer.ShowNumpad(now);
		return true;
	}
	return false;
}

int Fire::remocon_ir_fire(const string& exec_ir)
{
	if ( ! XLStringUtil::checkSafePath(exec_ir) )
	{
		ERRORLOG("IRファイルのパスは危険です " << exec_ir  );
		return -1;
	}

	auto irfilename = MainWindow::m()->GetConfigBasePath("./ir/user/" + exec_ir + ".dat");
	if (! XLFileUtil::Exist(irfilename) )
	{
		ERRORLOG("IRファイルがありません " << irfilename  );
		return -1;
	}

	if (! MainWindow::m()->IRDevice.Radiate(irfilename ) )
	{
		ERRORLOG("IR Radiate ERROR! " << irfilename  );
		return -4;
	}
	NOTIFYLOG("IR 送信成功 " << irfilename  );

	return 1;
}
bool Fire::remocon_ir_leaning(const string& exec_ir)
{
	if ( ! XLStringUtil::checkSafePath(exec_ir) )
	{
		return false;
	}
	auto irfilename = MainWindow::m()->GetConfigBasePath("./ir/user/" + exec_ir + ".dat");

	if ( ! MainWindow::m()->IRDevice.Capture(irfilename) )
	{
		DEBUGLOG("学習に失敗しました" );
		return false;
	}

	NOTIFYLOG("IR 学習データの保存完了 " << irfilename  );
	return true;
}



#endif //WITH_CLIENT_ONLY_CODE
