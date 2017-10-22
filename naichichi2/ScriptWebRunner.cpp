//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include "MainWindow.h"
#include "ScriptWebRunner.h"
#include "ScriptRemoconWeb.h"
#include "XLFileUtil.h"
#include "XLStringUtil.h"
#include "ActionImplement.h"
#include "XLHttpSocket.h"
#include "Recognition_Factory.h"
#include "SystemMisc.h"
#include "FHCCommander.h"
#include "XLUrlParser.h"
#include "TwitterOAuth.h"
#include "Fire.h"
#include "MultiRoomUtil.h"
#include "NetDevice.h"

ScriptWebRunner::ScriptWebRunner()
{
}

ScriptWebRunner::~ScriptWebRunner()
{
}


//Scriptの終了
void ScriptWebRunner::DestoryScript()
{
	Recogn.ClearCallback();
	Speak.ClearCallback();
	Trigger.ClearCallback();
}

void ScriptWebRunner::Create()
{
	//configのなかに出来たゴミデータを消す
	ScriptRemoconWeb::DeflagConfig("elec_","type");
	ScriptRemoconWeb::DeflagConfig("trigger_","enable");
	ScriptRemoconWeb::DeflagConfig("multiroom_","name");

	Recogn.ReloadRecong(false);
	Speak.ReloadSpeak(true);
	Trigger.ReloadTrigger(time(NULL));
	SystemMisc::MakeClientWelcome();
}


//変更画面全部
string ScriptWebRunner::EditIndex(const XLHttpHeader& httpHeaders) const
{
	string html = MainWindow::m()->TranslateFile.TransEdit( ScriptRemoconWeb::OpenTPL(httpHeaders,"edit.tpl") );

	html = XLStringUtil::replace_low(html,"%TRIGGER%", TriggerStatus() );
	html = XLStringUtil::replace_low(html,"%MULTIROOM%", MultiRoomStatus() );
	html = XLStringUtil::replace_low(html,"%SETTING%", SettingStatus() );
	html = XLStringUtil::replace_low(html,"%REMOCON%", RemoconStatus("") );
	return html;
}

//ステータスを jsonで
string ScriptWebRunner::RemoconStatus(const string& appendOption)  const
{
	const auto configmap = MainWindow::m()->Config.FindGetsToMap("elec_",false);
	return ScriptRemoconWeb::DrawJsonStatus(configmap,appendOption );
}
string ScriptWebRunner::GetSensorJson() 
{
	float sensor_temp,sensor_lumi,sensor_sound;
	MainWindow::m()->Sensor.getSensorNow(&sensor_temp,&sensor_lumi,&sensor_sound);
	
	//今の時刻を取得
	const time_t now = time(NULL);

	string jsonstring;
	jsonstring  = ",\"temp\": "+ XLStringUtil::jsonescape( num2str(sensor_temp) ) ;
	jsonstring += ",\"lumi\": "+ XLStringUtil::jsonescape( num2str(sensor_lumi) ) ;
	jsonstring += ",\"sound\": "+ XLStringUtil::jsonescape( num2str(sensor_sound) ) ;

	map<time_t,short> tempmap,lumimap,soundmap;
	MainWindow::m()->Sensor.ReadDataMapDay( now , &tempmap , &lumimap,&soundmap);
	jsonstring += ",\"tempday\": " + ScriptRemoconWeb::sensorMaptoJsonScale(tempmap,100) ;	//100倍された値が入っているのでもとに戻す
	jsonstring += ",\"lumiday\": " + ScriptRemoconWeb::sensorMaptoJson(lumimap) ;
	jsonstring += ",\"soundday\": " + ScriptRemoconWeb::sensorMaptoJson(soundmap) ;
	MainWindow::m()->Sensor.ReadDataMapWeek( now , &tempmap , &lumimap,&soundmap);
	jsonstring += ",\"tempweek\": " + ScriptRemoconWeb::sensorMaptoJsonScale(tempmap,100) ;	//100倍された値が入っているのでもとに戻す
	jsonstring += ",\"lumiweek\": " + ScriptRemoconWeb::sensorMaptoJson(lumimap) ;
	jsonstring += ",\"soundweek\": " + ScriptRemoconWeb::sensorMaptoJson(soundmap) ;

	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return _A2U(jsonstring.c_str());
}

string ScriptWebRunner::remocon_fire_bystring(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	{
		auto str = mapfind(request,"str");
		Fire f;
		if ( ! f.FireByString(str) )
		{
			return ScriptRemoconWeb::ResultError(10100 , "webからの音声認識に失敗しました" +f.getError());
		}
	}
	return RemoconStatus("");;
}

string ScriptWebRunner::remocon_preview_tospeak_string(const XLHttpHeader& httpHeaders) 
{
	

	const map<string,string> request = httpHeaders.getPost();
	string jsonstring;
	{
		//type1=hogehoge&type2=hogehoge
		auto tospeak_string = mapfind(request,"tospeak_string");
		if (tospeak_string == "")
		{
			return ScriptRemoconWeb::ResultError(10200 , "tospeak_stringが見つかりません");
		}

		MainWindow::m()->SyncInvoke( [&](){
			MainWindow::m()->Speak.SpeakSync(tospeak_string,false);
		});
	}
	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return _A2U(jsonstring.c_str());
}

string ScriptWebRunner::remocon_preview_elec_mp3(const XLHttpHeader& httpHeaders) 
{
	

	const map<string,string> request = httpHeaders.getPost();
	string jsonstring;
	{
		//type1=hogehoge&type2=hogehoge
		auto tospeak_mp3 = mapfind(request,"tospeak_mp3");
		if (tospeak_mp3 == "")
		{
			return ScriptRemoconWeb::ResultError(10300 , "tospeak_mp3が見つかりません");
		}

		if (! XLStringUtil::checkSafePath(tospeak_mp3) )
		{
			return ScriptRemoconWeb::ResultError(10301 , "tospeak_mp3は安全ではありません");
		}

		MainWindow::m()->MusicPlayAsync.PlaySync(tospeak_mp3);
	}
	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return _A2U(jsonstring.c_str());
}

string ScriptWebRunner::remocon_preview_elec_macro(const XLHttpHeader& httpHeaders) 
{
	

	AutoLEDOn autoLEDOn;
	const map<string,string> request = httpHeaders.getPost();
	string jsonstring;
	{
		try
		{
			Fire f;
			f.run_macroE(request, "exec_macro_");
		}
		catch(XLException &e)
		{
			return ScriptRemoconWeb::ResultError(10401 , e.what());
		}
	}
	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return _A2U(jsonstring.c_str());
}
//SIPで発信する
string ScriptWebRunner::remocon_sip_call(const XLHttpHeader& httpHeaders) 
{
/*
	string jsonstring;
	const map<string,string> request = httpHeaders.getPost();
	const string call = mapfind(request,"call");

	MainWindow::m()->SipServer.Call(call);

	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return _A2U(jsonstring.c_str());
*/
	return "";
}

//SIPでかかってきた電話にこたえる
string ScriptWebRunner::remocon_sip_answer(const XLHttpHeader& httpHeaders) 
{
/*
	string jsonstring;

	MainWindow::m()->SipServer.Answer();
	MainWindow::m()->SipServer.WaitForRequest(5);

	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return _A2U(jsonstring.c_str());
*/
	return "";
}

//SIPで電話を切る
string ScriptWebRunner::remocon_sip_hangup(const XLHttpHeader& httpHeaders) 
{
/*
	string jsonstring;

	MainWindow::m()->SipServer.Hangup();
	MainWindow::m()->SipServer.WaitForRequest(5);

	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return _A2U(jsonstring.c_str());
*/
	return "";
}

//SIP numpadを閉じるイベント
string ScriptWebRunner::remocon_sip_numpad_close(const XLHttpHeader& httpHeaders) 
{
/*
	string jsonstring;

	MainWindow::m()->SipServer.HideNumpad();

	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return _A2U(jsonstring.c_str());
*/
	return "";
}


//リモコンの副次的情報の取得
string ScriptWebRunner::remocon_get_append_info(const XLHttpHeader& httpHeaders)
{
	const auto configmap = MainWindow::m()->Config.ToMap();
	const time_t now = time(NULL);

	return ScriptRemoconWeb::remocon_get_append_info(configmap,now);
}

string ScriptWebRunner::remocon_recogn_reload(const XLHttpHeader& httpHeaders)
{
	const map<string,string> request = httpHeaders.getPost();

	auto c = mapfind(request,"c");
	auto args1 = mapfind(request,"arg1");
	if (c == "recogn")
	{
		Recogn.ReloadRecong(true);
	}

	string jsonstring = ",\"c\": " + XLStringUtil::jsonescape(c) ;
	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return _A2U(jsonstring.c_str());
}

string ScriptWebRunner::remocon_toplevel_invoke(const XLHttpHeader& httpHeaders)
{
	const map<string,string> request = httpHeaders.getPost();

	auto c = mapfind(request,"c");
	if (c == "start")
	{
		//音声認識処理を止めて、トッププライオリティで。
		//IRQ割り込みを利用て、10usオーダーぐらいの処理をします。CPUが忙しいとダメです。

		//すべてを一つでやると、linuxだとまれに落ちる時がある？ 意味がよくわからないが、安定性を取るため処理を分割します.
		MainWindow::m()->SyncInvoke([&](){
			try
			{
				//重い音声認識を止めます
				MainWindow::m()->Recognition.Free();
				NOTIFYLOG("this->Recognition.Free()");
			}
			catch(XLException& e)
			{
				ERRORLOG("最優先実行できませんでした。例外 " << e.what() );
			}
		});
	}
	else if (c == "end")
	{
		MainWindow::m()->SyncInvoke([&](){
			try
			{
				NOTIFYLOG("this->Recognition.Rebuild()");
				MainWindow::m()->ReCreateRecognitionEngine();
				MainWindow::m()->Recognition.CommitRule();
			}
			catch(XLException& e)
			{
				ERRORLOG("最優先実行後に音声認識エンジンを構築できませんでした。例外 " << e.what() );
			}
		});
	}

	string jsonstring = ",\"c\": " + XLStringUtil::jsonescape(c) ;
	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return _A2U(jsonstring.c_str());
}

string ScriptWebRunner::remocon_ir_leaning(const XLHttpHeader& httpHeaders)
{
	const map<string,string> request = httpHeaders.getPost();

	auto exec_ir = mapfind(request,"exec_ir");
	if (exec_ir == "")
	{
		exec_ir = num2str(time(NULL)) + "_" + num2str( xlrandom() ) ;
	}

	Fire f;
	if ( ! f.remocon_ir_leaning(exec_ir) )
	{
		return ScriptRemoconWeb::ResultError(10500 , "赤外線を記録できませんでした");
	}

	NOTIFYLOG("remocon_ir_leaning -> OK 1");
	NOTIFYLOG(exec_ir);
	NOTIFYLOG(XLStringUtil::jsonescape(exec_ir));
	
	string jsonstring = ",\"exec_ir\": " + XLStringUtil::jsonescape(exec_ir) ;
	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";

	NOTIFYLOG("remocon_ir_leaning -> OK 2");
	return _A2U(jsonstring.c_str());
}

string ScriptWebRunner::remocon_ir_fire(const XLHttpHeader& httpHeaders)
{
	const map<string,string> request = httpHeaders.getPost();

	auto exec_ir = mapfind(request,"exec_ir");

	AutoLEDOn autoLEDOn;
	Fire f;
	int r = f.remocon_ir_fire(exec_ir);
	if (r <= 0)
	{
		if (r == -1)
		{
			return ScriptRemoconWeb::ResultError(10601 , "赤外線データがありません。最初にリモコンの記録を行なってください。");
		}
		else if (r == -4)
		{
			return ScriptRemoconWeb::ResultError(10604 , "赤外線データを送信出来ませんでした");
		}
		return ScriptRemoconWeb::ResultError(10609 , "不明なエラーにより赤外線を送信出来ませんでした。 エラーコード:" + num2str(r));
	}
	
	return RemoconStatus("");
}



//アイコンの並び順の更新
string ScriptWebRunner::remocon_update_icon_order(const XLHttpHeader& httpHeaders)
{
	const map<string,string> request = httpHeaders.getPost();
	DEBUGLOG("request: " << XLStringUtil::crossjoin("=","&",request) );
	{
		//elec_ID_order=hogehoge&elec_ID_order=hogehoge&elec_ID_order=hogehoge&elec_ID_order=hogehoge&elec_ID_order=hogehoge&elec_ID_order=hogehoge&
		for(auto it = request.begin() ; it != request.end() ; ++it )
		{
			int key = -1;
			sscanf(it->first.c_str(),"elec_%d_order" , &key );
			if (key == -1)
			{
				continue;
			}
			if ( atoi( it->second.c_str() ) <= 0 )
			{
				continue;
			}

			MainWindow::m()->Config.Set( it->first.c_str() , it->second.c_str() );
		}
	}
	ConfigUpdated(false);
	return this->RemoconStatus("");
}

//アクションアイコンの並び順の更新
string ScriptWebRunner::remocon_update_elec_action_order(const XLHttpHeader& httpHeaders)
{
	const map<string,string> request = httpHeaders.getPost();
	DEBUGLOG("request: " << XLStringUtil::crossjoin("=","&",request) );
	{
		int target_key = -1;
		//elec_ID1_action_ID2_order=hogehoge&elec_ID1_action_ID2_order=hogehoge&elec_ID1_action_ID2_order=hogehoge&elec_ID1_action_ID2_order=hogehoge
		for(auto it = request.begin() ; it != request.end() ; ++it )
		{
			int key1 = -1;
			int key2 = -1;
			sscanf(it->first.c_str(),"elec_%d_action_%d_order" , &key1 , &key2 );
			if (key1 == -1 || key2 == -1)
			{
				continue;
			}
			if (target_key == -1)
			{
				target_key = key1;
			}
			if (key1 != target_key)
			{
				//ふつーは複数の 機材の actionを同時に更新したりはしないはず。
				//とりあえずこれで。
				continue;
			}
			if ( atoi( it->second.c_str() ) <= 0 )
			{
				continue;
			}

			MainWindow::m()->Config.Set( it->first.c_str() , it->second.c_str() );
		}
	}
	ConfigUpdated(false);
	return this->RemoconStatus("");
}

//家電を消す
string ScriptWebRunner::remocon_delete_elec(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	DEBUGLOG("request: " << XLStringUtil::crossjoin("=","&",request) );
	{
		//key=ID
		auto keyIT = request.find("key");
		if (keyIT == request.end() )
		{
			return ScriptRemoconWeb::ResultError(10700,"keyがありません。");
		}
		int key = atoi(keyIT->second.c_str());
		if (key <= 0)
		{
			return ScriptRemoconWeb::ResultError(10701,"keyが数字ではありません。");
		}

		const string prefix = string("elec_") + num2str(key) + "_";
		const auto configmap = MainWindow::m()->Config.FindGetsToMap(prefix,false);
		for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
		{
			MainWindow::m()->Config.Remove( it->first.c_str() );
		}

	}
	ConfigUpdated(false);
//	Recogn.ReloadRecong(true);
//	MainWindow::m()->HomeKitServer.NotifyAccessory();
	return this->RemoconStatus("");
}

//家電の操作を消す
string ScriptWebRunner::remocon_delete_elec_action(const XLHttpHeader& httpHeaders)
{
	const map<string,string> request = httpHeaders.getPost();
	DEBUGLOG("request: " << XLStringUtil::crossjoin("=","&",request) );

	{
		//key1=ID&key2=ID
		auto key1IT = request.find("key1");
		if (key1IT == request.end() )
		{
			return ScriptRemoconWeb::ResultError(10800,"key1がありません。");
		}
		int key1 = atoi(key1IT->second.c_str());
		if (key1 <= 0)
		{
			return ScriptRemoconWeb::ResultError(10801,"key1が数字ではありません");
		}

		auto key2IT = request.find("key2");
		if (key2IT == request.end() )
		{
			return ScriptRemoconWeb::ResultError(10802,"key2がありません。");
		}
		int key2 = atoi(key2IT->second.c_str());
		if (key2 <= 0)
		{
			return ScriptRemoconWeb::ResultError(10803,"key2が数字ではありません");
		}

		const string prefix = string("elec_") + num2str(key1) + "_action_" +  num2str(key2) + "_";
		const auto configmap = MainWindow::m()->Config.FindGetsToMap(prefix,false);
		for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
		{
			MainWindow::m()->Config.Remove( it->first.c_str() );
		}

	}

	ConfigUpdated(false);
//	Recogn.ReloadRecong(true);
	return this->RemoconStatus("");
}




//機材のアップデート
//elec_key=123&type=タイプ&type_other=タイプその他
string ScriptWebRunner::remocon_update_elec(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	DEBUGLOG("request: " << XLStringUtil::crossjoin("=","&",request) );

	const string elec_key = mapfind(request,"elec_key");

	string appendOption;
	{
		int key;
		if (elec_key == "new")
		{//家電の新しいIDを降り出す
			key = ScriptRemoconWeb::NewElecID();
		}
		else
		{
			key = atoi(elec_key.c_str());
		}
		if (key <= 0) 
		{
			return ScriptRemoconWeb::ResultError(10900 , "elec_keyの値が数字ではありません");
		}
		const auto prefix = string("elec_") + num2str(key)  ;

		string type = mapfind(request,"type");
		if (type == "" || type == "other")
		{
			type = mapfind(request,"type_other");
			if (type == "SPACE")
			{//イタヅラ？でspace と入力された場合はエラー
				return ScriptRemoconWeb::ResultError(10923 , "typeで設定しようとしている操作種類はシステムで予約されています");
			}
		}
		if (type == "")
		{
			return ScriptRemoconWeb::ResultError(10921 , "その他が設定されていません");
		}
		if (! ScriptRemoconWeb::checkUniqElecType(key,type) )
		{
			return ScriptRemoconWeb::ResultError(10922 , "typeで設定しようとしている操作種類は既に使われています");
		}
		if (type == "none")
		{
			return ScriptRemoconWeb::ResultError(10923 , "typeで設定しようとしている操作種類はシステムで予約されています");
		}
		const string elecicon = mapfind(request,"elecicon");
		const string showremocon = mapfind(request,"showremocon");
		const string ignore_recogn = mapfind(request,"ignore_recogn");
		const string click_to_menu = mapfind(request,"click_to_menu");

		MainWindow::m()->Config.Set( prefix + "_type" , type );
		MainWindow::m()->Config.Set( prefix + "_elecicon" , elecicon );
		MainWindow::m()->Config.Set( prefix + "_showremocon" , showremocon );
		MainWindow::m()->Config.Set( prefix + "_ignore_recogn" , ignore_recogn );
		MainWindow::m()->Config.Set( prefix + "_click_to_menu" , click_to_menu );

		if (elec_key == "new")
		{
			MainWindow::m()->Config.Set( prefix + "_status" , "" );
			MainWindow::m()->Config.Set( prefix + "_statuscolor" , "#000000" );
			MainWindow::m()->Config.Set( prefix + "_order" , num2str( ScriptRemoconWeb::newOrderElec() ) );
		}

		appendOption = "\"updatekey\": \"" + num2str(key) + "\"";
	}

	ConfigUpdated(true);
//	if (elec_key == "new")
//	{//家電の新しいがあったので、通知
//		MainWindow::m()->HomeKitServer.NotifyAccessory();
//	}
	return this->RemoconStatus(appendOption);
}

//機材のアクション項目のアップデート
string ScriptWebRunner::remocon_update_elec_action(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	DEBUGLOG("request: " << XLStringUtil::crossjoin("=","&",request) );

	string appendOption;
	{
		const string action_key1 = mapfind(request,"action_key1");
		int key1 = atoi(action_key1.c_str());
		if (key1 <= 0) 
		{
			return ScriptRemoconWeb::ResultError(20002 , "action_key1の値が数字ではありません");
		}

		const string action_key2 = mapfind(request,"action_key2");
		int key2;
		if (action_key2 == "new")
		{//家電操作の新しいIDを降り出す
			key2 = ScriptRemoconWeb::NewElecActionID(key1);
		}
		else
		{
			key2 = atoi(action_key2.c_str());
		}
		if (key2 <= 0) 
		{
			return ScriptRemoconWeb::ResultError(20004 , "action_key2の値が数字ではありません");
		}
		const auto prefix = string("elec_") + num2str(key1) + "_action_" + num2str(key2) ;

		string actiontype = mapfind(request,"actiontype");
		if (actiontype == "" || actiontype == "other")
		{
			actiontype = mapfind(request,"actiontype_other");
			if (actiontype == "SPACE")
			{//イタヅラ？でspace と入力された場合はエラー
				return ScriptRemoconWeb::ResultError(20021 , "typeで設定しようとしている操作種類はシステムで予約されています");
			}
		}
		if (actiontype == "")
		{
			return ScriptRemoconWeb::ResultError(20021 , "種類で設定しようとしている操作名称は既に使われています");
		}
		if (! ScriptRemoconWeb::checkUniqElecActionType(key1,key2,actiontype) )
		{
			return ScriptRemoconWeb::ResultError(20022 , "種類で設定しようとしている操作名称は既に使われています");
		}
		if (actiontype == "none")
		{
			return ScriptRemoconWeb::ResultError(20023 , "種類で設定しようとしている操作名称はシステムで予約されています");
		}

		const string actionvoice_command_1 = mapfind(request,"actionvoice_command_1");
		const string actionvoice_command_2 = mapfind(request,"actionvoice_command_2");
		const string actionvoice_command_3 = mapfind(request,"actionvoice_command_3");
		const string actionvoice_command_4 = mapfind(request,"actionvoice_command_4");
		const string actionvoice_command_5 = mapfind(request,"actionvoice_command_5");
		const string showremocon = mapfind(request,"showremocon");
		const string nostateremocon = mapfind(request,"nostateremocon");
		const string actioncolor = mapfind(request,"actioncolor");
		const string tospeak_select = mapfind(request,"tospeak_select");
		const string tospeak_string = mapfind(request,"tospeak_string");
		const string tospeak_mp3 = mapfind(request,"tospeak_mp3");
		const string execflag = mapfind(request,"execflag");
		const string exec_ir = mapfind(request,"exec_ir");
		const string exec_command = mapfind(request,"exec_command");
		const string exec_command_args1 = mapfind(request,"exec_command_args1");
		const string exec_command_args2 = mapfind(request,"exec_command_args2");
		const string exec_command_args3 = mapfind(request,"exec_command_args3");
		const string exec_command_args4 = mapfind(request,"exec_command_args4");
		const string exec_command_args5 = mapfind(request,"exec_command_args5");
		const string exec_macro_1_elec = mapfind(request,"exec_macro_1_elec");
		const string exec_macro_1_action = mapfind(request,"exec_macro_1_action");
		const string exec_macro_1_after = mapfind(request,"exec_macro_1_after");
		const string exec_macro_2_elec = mapfind(request,"exec_macro_2_elec");
		const string exec_macro_2_action = mapfind(request,"exec_macro_2_action");
		const string exec_macro_2_after = mapfind(request,"exec_macro_2_after");
		const string exec_macro_3_elec = mapfind(request,"exec_macro_3_elec");
		const string exec_macro_3_action = mapfind(request,"exec_macro_3_action");
		const string exec_macro_3_after = mapfind(request,"exec_macro_3_after");
		const string exec_macro_4_elec = mapfind(request,"exec_macro_4_elec");
		const string exec_macro_4_action = mapfind(request,"exec_macro_4_action");
		const string exec_macro_4_after = mapfind(request,"exec_macro_4_after");
		const string exec_macro_5_elec = mapfind(request,"exec_macro_5_elec");
		const string exec_macro_5_action = mapfind(request,"exec_macro_5_action");
		const string exec_macro_5_after = mapfind(request,"exec_macro_5_after");
		const string exec_macro_6_elec = mapfind(request,"exec_macro_6_elec");
		const string exec_macro_6_action = mapfind(request,"exec_macro_6_action");
		const string exec_macro_6_after = mapfind(request,"exec_macro_6_after");
		const string exec_macro_7_elec = mapfind(request,"exec_macro_7_elec");
		const string exec_macro_7_action = mapfind(request,"exec_macro_7_action");
		const string exec_macro_7_after = mapfind(request,"exec_macro_7_after");
		const string exec_macro_8_elec = mapfind(request,"exec_macro_8_elec");
		const string exec_macro_8_action = mapfind(request,"exec_macro_8_action");
		const string exec_macro_8_after = mapfind(request,"exec_macro_8_after");
		const string exec_macro_9_elec = mapfind(request,"exec_macro_9_elec");
		const string exec_macro_9_action = mapfind(request,"exec_macro_9_action");
		const string exec_macro_9_after = mapfind(request,"exec_macro_9_after");
		const string timer_type = mapfind(request,"timer_type");
		const string timer_exec_elec = mapfind(request,"timer_exec_elec");
		const string timer_exec_action = mapfind(request,"timer_exec_action");
		const string timer_tospeak_select = mapfind(request,"timer_tospeak_select");
		const string timer_tospeak_string = mapfind(request,"timer_tospeak_string");
		const string timer_tospeak_mp3 = mapfind(request,"timer_tospeak_mp3");
		const string timer_no_status_overraide = mapfind(request,"timer_no_status_overraide");
		const string recongpause_type = mapfind(request,"recongpause_type");
		const string multiroom_room = mapfind(request,"multiroom_room");
		const string multiroom_elec = mapfind(request,"multiroom_elec");
		const string multiroom_action = mapfind(request,"multiroom_action");
		const string netdevice_elec = mapfind(request,"netdevice_elec");
		const string netdevice_action = mapfind(request,"netdevice_action");
		const string netdevice_value = mapfind(request,"netdevice_value");
		const string sip_action_type = mapfind(request,"sip_action_type");
		const string sip_call_number = mapfind(request,"sip_call_number");
		const string actionicon_use = mapfind(request,"actionicon_use");
		const string actionicon = mapfind(request,"actionicon");

		if (!MainWindow::m()->Recognition.checkExprYomi(actionvoice_command_1))
		{
			return ScriptRemoconWeb::ResultError(20031 , "音声認識のコマンド1に読み上げできない文字が含まれています");
		}
		if (!MainWindow::m()->Recognition.checkExprYomi(actionvoice_command_2))
		{
			return ScriptRemoconWeb::ResultError(20032 , "音声認識のコマンド2に読み上げできない文字が含まれています");
		}
		if (!MainWindow::m()->Recognition.checkExprYomi(actionvoice_command_3))
		{
			return ScriptRemoconWeb::ResultError(20033 , "音声認識のコマンド3に読み上げできない文字が含まれています");
		}
		if (!MainWindow::m()->Recognition.checkExprYomi(actionvoice_command_4))
		{
			return ScriptRemoconWeb::ResultError(20034 , "音声認識のコマンド4に読み上げできない文字が含まれています");
		}
		if (!MainWindow::m()->Recognition.checkExprYomi(actionvoice_command_5))
		{
			return ScriptRemoconWeb::ResultError(20035 , "音声認識のコマンド5に読み上げできない文字が含まれています");
		}

		const string old_timer_type = MainWindow::m()->Config.Get( prefix + "_timer_type","");

		const string prefixActionvoice1 = prefix + "_actionvoice_command_1";
		const string prefixActionvoice2 = prefix + "_actionvoice_command_2";
		const string prefixActionvoice3 = prefix + "_actionvoice_command_3";
		const string prefixActionvoice4 = prefix + "_actionvoice_command_4";
		const string prefixActionvoice5 = prefix + "_actionvoice_command_5";
		
		if (MainWindow::m()->Recognition.isAlreadyRegist5(actionvoice_command_1,prefixActionvoice1,prefixActionvoice2,prefixActionvoice3,prefixActionvoice4,prefixActionvoice5))
		{
			return ScriptRemoconWeb::ResultError(20061 , "音声認識のコマンド1は既に別のコマンドに割り振られています");
		}
		//先頭のコマンドだけは、同一項目内でのチェックをスキップする。
//		if (MainWindow::m()->Recognition.isAlreadyRegistOther5(actionvoice_command_1,"","","","")
//		{
//			return ScriptRemoconWeb::ResultError(20061 , "音声認識のコマンド1は既に別のコマンド(同一項目内)に割り振られています");
//		}

		if (MainWindow::m()->Recognition.isAlreadyRegist5(actionvoice_command_2,prefixActionvoice2,prefixActionvoice1,prefixActionvoice3,prefixActionvoice4,prefixActionvoice5))
		{
			return ScriptRemoconWeb::ResultError(20062 , "音声認識のコマンド2は既に別のコマンドに割り振られています");
		}
		if (MainWindow::m()->Recognition.isAlreadyRegistOther5(actionvoice_command_2,actionvoice_command_1,actionvoice_command_3,actionvoice_command_4,actionvoice_command_5))
		{
			return ScriptRemoconWeb::ResultError(20062 , "音声認識のコマンド2は既に別のコマンド(同一項目内)に割り振られています");
		}

		if (MainWindow::m()->Recognition.isAlreadyRegist5(actionvoice_command_3,prefixActionvoice3,prefixActionvoice1,prefixActionvoice2,prefixActionvoice4,prefixActionvoice5))
		{
			return ScriptRemoconWeb::ResultError(20063 , "音声認識のコマンド3は既に別のコマンドに割り振られています");
		}
		if (MainWindow::m()->Recognition.isAlreadyRegistOther5(actionvoice_command_3,actionvoice_command_1,actionvoice_command_2,actionvoice_command_4,actionvoice_command_5))
		{
			return ScriptRemoconWeb::ResultError(20063 , "音声認識のコマンド3は既に別のコマンド(同一項目内)に割り振られています");
		}

		if (MainWindow::m()->Recognition.isAlreadyRegist5(actionvoice_command_4,prefixActionvoice4,prefixActionvoice1,prefixActionvoice2,prefixActionvoice3,prefixActionvoice5))
		{
			return ScriptRemoconWeb::ResultError(20064 , "音声認識のコマンド4は既に別のコマンドに割り振られています");
		}
		if (MainWindow::m()->Recognition.isAlreadyRegistOther5(actionvoice_command_4,actionvoice_command_1,actionvoice_command_2,actionvoice_command_3,actionvoice_command_5))
		{
			return ScriptRemoconWeb::ResultError(20064 , "音声認識のコマンド4は既に別のコマンド(同一項目内)に割り振られています");
		}

		if (MainWindow::m()->Recognition.isAlreadyRegist5(actionvoice_command_5,prefixActionvoice5,prefixActionvoice1,prefixActionvoice2,prefixActionvoice3,prefixActionvoice4))
		{
			return ScriptRemoconWeb::ResultError(20065 , "音声認識のコマンド5は既に別のコマンドに割り振られています");
		}
		if (MainWindow::m()->Recognition.isAlreadyRegistOther5(actionvoice_command_5,actionvoice_command_1,actionvoice_command_2,actionvoice_command_3,actionvoice_command_4))
		{
			return ScriptRemoconWeb::ResultError(20065 , "音声認識のコマンド5は既に別のコマンド(同一項目内)に割り振られています");
		}

		if ( tospeak_select == "string" )
		{
			if (! MainWindow::m()->Recognition.checkYomi(tospeak_string))
			{
				return ScriptRemoconWeb::ResultError(20041 , "読み上げできない文字列が含まれています");
			}
			//読み上げないモードでキャッシュを作らせます。
			MainWindow::m()->SyncInvoke([&](){
				MainWindow::m()->Speak.SpeakAsync( CallbackPP::NoCallback() , tospeak_string , true );
			});
		}
		else if ( tospeak_select == "musicfile" )
		{
			if ( ! XLStringUtil::checkSafePath(tospeak_mp3) )
			{
				return ScriptRemoconWeb::ResultError(20042 , "読み上げるファイル名が正しくありません");
			}
		}

		if ( execflag != "ir" )
		{//赤外線でなくて、赤外線の設定が含まれている場合はゴミデータになるので削除してあげる
			if (XLStringUtil::checkSafePath(exec_ir) )
			{
				auto irdat = MainWindow::m()->GetConfigBasePath("./ir/user/" + exec_ir + ".dat");
				if (XLFileUtil::Exist(irdat))
				{
					XLFileUtil::del(irdat);
				}
			}
		}

		if ( execflag != "timer" )
		{//タイマーじゃないなら消す
			MainWindow::m()->Config.Set( prefix + "_timer_firetime" , "" );
		}
		else
		{//タイマーなら
			if ( timer_tospeak_select == "string" )
			{
				if (! MainWindow::m()->Recognition.checkYomi(timer_tospeak_string))
				{
					return ScriptRemoconWeb::ResultError(20141 , "読み上げできない文字列が含まれています");
				}
				//読み上げないモードでキャッシュを作らせます。
				MainWindow::m()->SyncInvoke([&](){
					MainWindow::m()->Speak.SpeakAsync( CallbackPP::NoCallback() , timer_tospeak_string , true );
				});
			}
			else if ( timer_tospeak_select == "musicfile" )
			{
				if ( ! XLStringUtil::checkSafePath(timer_tospeak_mp3) )
				{
					return ScriptRemoconWeb::ResultError(20142 , "読み上げるファイル名が正しくありません");
				}
			}
		}

		if ( execflag == "macro" )
		{//マクロのチェック
			//マクロが循環参照になっていないか求めます
			int nestcount = 0;
			//循環参照になって無限ループにならないようにマクロの上限をチェックする
			Fire f;
			if (  ! f.check_macroE(exec_macro_1_elec,exec_macro_1_action , &nestcount) )
			{
				return ScriptRemoconWeb::ResultError(20051 , "マクロ1で呼び出し回数の上限を超えました。マクロの中で無限ループに入っている可能性があります。");
			}
			if (  ! f.check_macroE(exec_macro_2_elec,exec_macro_2_action , &nestcount) )
			{
				return ScriptRemoconWeb::ResultError(20052 , "マクロ2で呼び出し回数の上限を超えました。マクロの中で無限ループに入っている可能性があります。");
			}
			if (  ! f.check_macroE(exec_macro_3_elec,exec_macro_3_action , &nestcount) )
			{
				return ScriptRemoconWeb::ResultError(20053 , "マクロ3で呼び出し回数の上限を超えました。マクロの中で無限ループに入っている可能性があります。");
			}
			if (  ! f.check_macroE(exec_macro_4_elec,exec_macro_4_action , &nestcount) )
			{
				return ScriptRemoconWeb::ResultError(20054 , "マクロ4で呼び出し回数の上限を超えました。マクロの中で無限ループに入っている可能性があります。");
			}
			if (  ! f.check_macroE(exec_macro_5_elec,exec_macro_5_action , &nestcount) )
			{
				return ScriptRemoconWeb::ResultError(20055 , "マクロ5で呼び出し回数の上限を超えました。マクロの中で無限ループに入っている可能性があります。");
			}
			if (  ! f.check_macroE(exec_macro_6_elec,exec_macro_6_action , &nestcount) )
			{
				return ScriptRemoconWeb::ResultError(20056 , "マクロ6で呼び出し回数の上限を超えました。マクロの中で無限ループに入っている可能性があります。");
			}
			if (  ! f.check_macroE(exec_macro_7_elec,exec_macro_7_action , &nestcount) )
			{
				return ScriptRemoconWeb::ResultError(20057 , "マクロ7で呼び出し回数の上限を超えました。マクロの中で無限ループに入っている可能性があります。");
			}
			if (  ! f.check_macroE(exec_macro_8_elec,exec_macro_8_action , &nestcount) )
			{
				return ScriptRemoconWeb::ResultError(20058 , "マクロ8で呼び出し回数の上限を超えました。マクロの中で無限ループに入っている可能性があります。");
			}
			if (  ! f.check_macroE(exec_macro_9_elec,exec_macro_9_action , &nestcount) )
			{
				return ScriptRemoconWeb::ResultError(20059 , "マクロ9で呼び出し回数の上限を超えました。マクロの中で無限ループに入っている可能性があります。");
			}
		}

		NOTIFYLOG("speakcache reload");
		
		//スピーチキャッシュ関係の処理.
		MainWindow::m()->Speak.TryDeleteSpeakCacheDB(
			 MainWindow::m()->Config.Get( prefix + "_tospeak_string","")
			,tospeak_string );
		MainWindow::m()->Speak.TryDeleteSpeakCacheDB(
			 MainWindow::m()->Config.Get( prefix + "_timer_tospeak_string","")
			,timer_tospeak_string );

		MainWindow::m()->Config.Set( prefix + "_actiontype" , actiontype );
		MainWindow::m()->Config.Set( prefix + "_actionvoice_command_1" , actionvoice_command_1 );
		MainWindow::m()->Config.Set( prefix + "_actionvoice_command_2" , actionvoice_command_2 );
		MainWindow::m()->Config.Set( prefix + "_actionvoice_command_3" , actionvoice_command_3 );
		MainWindow::m()->Config.Set( prefix + "_actionvoice_command_4" , actionvoice_command_4 );
		MainWindow::m()->Config.Set( prefix + "_actionvoice_command_5" , actionvoice_command_5 );
		MainWindow::m()->Config.Set( prefix + "_showremocon" , showremocon );
		MainWindow::m()->Config.Set( prefix + "_nostateremocon" , nostateremocon );
		MainWindow::m()->Config.Set( prefix + "_actioncolor" , actioncolor );
		MainWindow::m()->Config.Set( prefix + "_tospeak_select" , tospeak_select );
		MainWindow::m()->Config.Set( prefix + "_tospeak_string" , tospeak_string );
		MainWindow::m()->Config.Set( prefix + "_tospeak_mp3" , tospeak_mp3 );
		MainWindow::m()->Config.Set( prefix + "_execflag" , execflag );
		MainWindow::m()->Config.Set( prefix + "_exec_ir" , exec_ir );
		MainWindow::m()->Config.Set( prefix + "_exec_command" , exec_command );
		MainWindow::m()->Config.Set( prefix + "_exec_command_args1" , exec_command_args1 );
		MainWindow::m()->Config.Set( prefix + "_exec_command_args2" , exec_command_args2 );
		MainWindow::m()->Config.Set( prefix + "_exec_command_args3" , exec_command_args3 );
		MainWindow::m()->Config.Set( prefix + "_exec_command_args4" , exec_command_args4 );
		MainWindow::m()->Config.Set( prefix + "_exec_command_args5" , exec_command_args5 );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_1_elec" , exec_macro_1_elec );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_1_action" , exec_macro_1_action );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_1_after" , exec_macro_1_after );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_2_elec" , exec_macro_2_elec );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_2_action" , exec_macro_2_action );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_2_after" , exec_macro_2_after );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_3_elec" , exec_macro_3_elec );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_3_action" , exec_macro_3_action );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_3_after" , exec_macro_3_after );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_4_elec" , exec_macro_4_elec );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_4_action" , exec_macro_4_action );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_4_after" , exec_macro_4_after );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_5_elec" , exec_macro_5_elec );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_5_action" , exec_macro_5_action );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_5_after" , exec_macro_5_after );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_6_elec" , exec_macro_6_elec );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_6_action" , exec_macro_6_action );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_6_after" , exec_macro_6_after );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_7_elec" , exec_macro_7_elec );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_7_action" , exec_macro_7_action );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_7_after" , exec_macro_7_after );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_8_elec" , exec_macro_8_elec );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_8_action" , exec_macro_8_action );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_8_after" , exec_macro_8_after );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_9_elec" , exec_macro_9_elec );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_9_action" , exec_macro_9_action );
		MainWindow::m()->Config.Set( prefix + "_exec_macro_9_after" , exec_macro_9_after );
		MainWindow::m()->Config.Set( prefix + "_timer_type" , timer_type );
		MainWindow::m()->Config.Set( prefix + "_timer_exec_elec" , timer_exec_elec );
		MainWindow::m()->Config.Set( prefix + "_timer_exec_action" , timer_exec_action );
		MainWindow::m()->Config.Set( prefix + "_timer_tospeak_select" , timer_tospeak_select );
		MainWindow::m()->Config.Set( prefix + "_timer_tospeak_string" , timer_tospeak_string );
		MainWindow::m()->Config.Set( prefix + "_timer_tospeak_mp3" , timer_tospeak_mp3 );
		MainWindow::m()->Config.Set( prefix + "_timer_no_status_overraide" , timer_no_status_overraide );
		MainWindow::m()->Config.Set( prefix + "_recongpause_type" , recongpause_type );
		MainWindow::m()->Config.Set( prefix + "_multiroom_room" , multiroom_room );
		MainWindow::m()->Config.Set( prefix + "_multiroom_elec" , multiroom_elec );
		MainWindow::m()->Config.Set( prefix + "_multiroom_action" , multiroom_action );
		MainWindow::m()->Config.Set( prefix + "_netdevice_elec" , netdevice_elec );
		MainWindow::m()->Config.Set( prefix + "_netdevice_action" , netdevice_action );
		MainWindow::m()->Config.Set( prefix + "_netdevice_value" , netdevice_value );
		MainWindow::m()->Config.Set( prefix + "_sip_action_type"  , sip_action_type );
		MainWindow::m()->Config.Set( prefix + "_sip_call_number" , sip_call_number );
		MainWindow::m()->Config.Set( prefix + "_actionicon_use" , actionicon_use );
		MainWindow::m()->Config.Set( prefix + "_actionicon" , actionicon );

		if (action_key2 == "new")
		{
			MainWindow::m()->Config.Set( prefix + "_order" , num2str(ScriptRemoconWeb::newOrderElecAction(key1)) );
		}

//		//リロード処理
//		Recogn.ReloadRecong(true);

		appendOption = "\"updatekey\": \"" + num2str(key2) + "\"";
	}

	NOTIFYLOG("ConfigUpdated.");
	ConfigUpdated(true);

	NOTIFYLOG("Result.");
	return this->RemoconStatus(appendOption);
}




//設定のステータスを jsonで
string ScriptWebRunner::SettingStatus()  const
{
	
	int i = 0;

	string jsonstring;
	jsonstring += string(",") + XLStringUtil::jsonescape("account_usermail") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("account_usermail",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("account_sync_server") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("account_sync_server","1"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("network_ipaddress_type") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("network_ipaddress_type",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("network_ipaddress_ip") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("network_ipaddress_ip",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("network_ipaddress_mask") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("network_ipaddress_mask",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("network_ipaddress_gateway") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("network_ipaddress_gateway",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("network_ipaddress_dns") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("network_ipaddress_dns",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("network_w_ipaddress_type") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("network_w_ipaddress_type",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("network_w_ipaddress_ip") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("network_w_ipaddress_ip",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("network_w_ipaddress_mask") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("network_w_ipaddress_mask",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("network_vipaddress_gateway") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("network_w_ipaddress_gateway",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("network_vipaddress_dns") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("network_w_ipaddressvfixed_dns",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("network_w_ipaddress_ssid") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("network_w_ipaddress_ssid",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("network_w_ipaddress_password") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("network_w_ipaddress_password",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("network_w_ipaddress_wkeymgmt") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("network_w_ipaddress_wkeymgmt",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("recong_type") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("recong_type",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("recong_julius_gomi_y") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("recong_julius_gomi_y","6"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("recong_julius_gomi_e") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("recong_julius_gomi_e","3"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("recong_julius_gomi_d") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("recong_julius_gomi_d","5"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("recong_julius_filter1") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("recong_julius_filter1","45"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("recong_julius_filter2") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("recong_julius_filter2","25"))) ;

	jsonstring += string(",") + XLStringUtil::jsonescape("recong_julius_lv_base") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("recong_julius_lv_base","20"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("recong_julius_lv_max") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("recong_julius_lv_max","150"))) ;

	jsonstring += string(",") + XLStringUtil::jsonescape("recong_ok_mp3") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("recong_ok_mp3","recong_ok_tyariri.mp3"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("recong_ng_mp3") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("recong_ng_mp3","recong_ng_pyuin.mp3"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("recong_pause_mp3") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("recong_pause_mp3","recong_pause_syun.mp3"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("recong_volume") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("recong_volume","100"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("recong__yobikake__1") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("recong__yobikake__1","コンピューター"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("recong__yobikake__2") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("recong__yobikake__2","コンピュータ"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("recong__yobikake__3") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("recong__yobikake__3",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("recong__yobikake__4") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("recong__yobikake__4",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("recong__yobikake__5") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("recong__yobikake__5",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("speak_type") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("speak_type",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("speak_volume") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("speak_volume","100"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("speak_nightvolume_volume") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("speak_nightvolume_volume","0"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("speak_option") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("speak_option",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("speak_use_remote") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("speak_use_remote",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("speak_remote1_ip") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("speak_remote1_ip",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("speak_remote1_secretkey") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("speak_remote1_secretkey",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("speak_use_nightvolume") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("speak_use_nightvolume",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("speak_nightvolume_start_hour") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("speak_nightvolume_start_hour","23"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("speak_nightvolume_start_minute") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("speak_nightvolume_start_minute","0"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("speak_nightvolume_end_hour") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("speak_nightvolume_end_hour","6"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("speak_nightvolume_end_minute") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("speak_nightvolume_end_minute","0"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("system_lang") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("system_lang",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("weather_script_command") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("weather_script_command","weather_openweather.js"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("train_script_command") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("train_script_command","train_tetsudocom.js"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("system_fhcboot_mp3") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("system_fhcboot_mp3","fhcboot_tyaran.mp3"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("version_number") + ": " 
		+ _A2U(XLStringUtil::jsonescape(num2str(MainWindow::m()->Config.GetInt("version_number",100) / 100.f ,"%.2f") )) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("httpd__uuid") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Httpd.GetUUID())) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("version_updateurl") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("version_updateurl","http://rti-giken.jp/fhc/update/stable/"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("version_osversion") + ": " 
		+ _A2U(XLStringUtil::jsonescape( SystemMisc::GetOSVersion() )) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("love") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("love",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("network_is_wifi_device") + ": " 
		+ (SystemMisc::IsWifiDevice() ? "1" : "0") ;
	jsonstring += string(",") + XLStringUtil::jsonescape("webapi_apikey") + ": " 
		+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("webapi_apikey",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("system_room_string") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("system_room_string","リモコン"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("system_room_color") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("system_room_color","#000000"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("system_room_icon") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("system_room_icon","room_icon1.png"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_default_font_color") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_default_font_color","#666666"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_default_background_color") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_default_background_color","#FFFFFF"))) ;

	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_background_image") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_background_image","bodybg_1.gif"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_menu_icon") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_menu_icon","list_1.png"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_sensor_font_color") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_sensor_font_color","#000000"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_sensor_temp_string") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_sensor_temp_string","温度"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_sensor_lumi_string") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_sensor_lumi_string","明るさ"))) ;

	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_footermenu_remoconedit_string") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_footermenu_remoconedit_string","リモコンを編集"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_footermenu_remoconedit_icon") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_footermenu_remoconedit_icon","remoconedit_icon1.png"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_footermenu_setting_string") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_footermenu_setting_string","設定画面に行く"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_footermenu_setting_icon") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_footermenu_setting_icon","setting_icon1.png"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_footermenu_font_color") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_footermenu_font_color","#000000"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_footermenu_background_color") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_footermenu_background_color","#FFFFDD"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_footermenu_border_color") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_footermenu_border_color","#CCCCCC"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_footermenu_hover_color") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_footermenu_hover_color","#FFF070"))) ;

	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_dialog_close_string") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_dialog_close_string","close"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_dialog_background_color") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_dialog_background_color","#FFFFFF"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_dialog_border_color") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_dialog_border_color","#AAAAAA"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_dialog_donot_autoclose") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_dialog_donot_autoclose","0"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_dialog_iconcount_pc") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_dialog_iconcount_pc","4"))) ;

	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_button_font_color") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_button_font_color","#000000"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_button_background_color1") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_button_background_color1","#F4F5F5"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_button_background_color2") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_button_background_color2","#DDDDDD"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_button_border_color") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_button_border_color","#BFC4C4"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_button_hover_color1") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_button_hover_color1","#F4F5F5"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_button_hover_color2") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_button_hover_color2","#C6C3C3"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_button_hover_border_color") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_button_hover_border_color","#BFC4C4"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_badges_timer_icon") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("remoconsetting_badges_timer_icon","timer.png"))) ;

	jsonstring += string(",") + XLStringUtil::jsonescape("twitter_screen_name") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("twitter_screen_name",""))) ;

	jsonstring += string(",") + XLStringUtil::jsonescape("homekit_boot") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("homekit_boot","0"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("homekit_pin") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("homekit_pin",""))) ;

	jsonstring += string(",") + XLStringUtil::jsonescape("sip_enable_incoming") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("sip_enable_incoming","0"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("sip_domain") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("sip_domain",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("sip_password") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("sip_password",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("sip_id") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("sip_id",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("sip_enable") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("sip_enable",""))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("sip_incoming_mp3") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("sip_incoming_mp3","sip_incoming_ring.mp3"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("sip_calling_mp3") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("sip_calling_mp3","sip_calling_ring.mp3"))) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("sip_when_taking_stop_recogn") + ": " 
			+ _A2U(XLStringUtil::jsonescape(MainWindow::m()->Config.Get("sip_when_taking_stop_recogn","1"))) ;

	//読み上げエンジンを列挙します.
	auto speakList = MainWindow::m()->Speak.getSpeakModelList();
	i = 1;
	for(auto it = speakList.begin() ; it != speakList.end() ; ++it , ++i)
	{
		jsonstring += string(",") + XLStringUtil::jsonescape("setting_speak_type_list_" + num2str(i) ) + ": " 
			+ _A2U(XLStringUtil::jsonescape(*it)) ;
	}


	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return jsonstring.c_str();
}

//トリガー設定のステータスを jsonで
string ScriptWebRunner::TriggerStatus()  const
{
	

	string jsonstring;
	const auto triggermap = MainWindow::m()->Config.FindGetsToMap("trigger_",false);
	for(auto it = triggermap.begin() ; it != triggermap.end() ; ++it )
	{
		jsonstring += string(",") + XLStringUtil::jsonescape(it->first) + ": " 
			+ _A2U(XLStringUtil::jsonescape(it->second)) ;
	}

	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return jsonstring;
}

//マルチルーム設定のステータスを jsonで
string ScriptWebRunner::MultiRoomStatus()  const
{
	const auto configmap = MainWindow::m()->Config.FindGetsToMap("multiroom_",false);
	const string my_uuid = MainWindow::m()->Httpd.GetUUID();

	string jsonstring 
		= MultiRoomUtil::DrawJsonStatus(my_uuid,configmap,false);

	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return jsonstring;
}

string ScriptWebRunner::setting_account_update(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray) 
{
	const map<string,string> request = httpHeaders.getPost();
	const string account_usermail = mapfind(request,"account_usermail");
	const string account_password = mapfind(request,"account_password");
	const string account_sync_server = mapfind(request,"account_sync_server");

	if (account_usermail.empty() || account_password.empty() )
	{
		return ScriptRemoconWeb::ResultError(39020 , "IDとパスワードを入力してください" );
	}

	{
		if (account_password == "<%PASSWORD NO CHANGE%>")
		{
			const string old_id = MainWindow::m()->Config.Get( "account_usermail" , "");;
			const string old_password = MainWindow::m()->Config.Get( "account_password" , "");
			const string new_id = account_usermail;
			const string new_password = "<%PASSWORD NO CHANGE%>";
			const string uuid = MainWindow::m()->Httpd.GetUUID();

			//サーバーに変更を告げる
			string serverResult = MainWindow::m()->ColudSyncPooling.OpeartionChangeRegist(old_id,old_password,new_id,new_password,uuid,"0");
			if ( strstr(serverResult.c_str(),"OK") != serverResult.c_str() )
			{
				if ( serverResult.find("uuid already use") != string::npos )
				{
					return ScriptRemoconWeb::ResultError(39091 , "このUUIDは別の端末で使われています" );
				}
				return ScriptRemoconWeb::ResultError(39020 , "サーバーから拒絶されました" );
			}

			//ユーザーIDだけ変更
			MainWindow::m()->Config.Set( "account_usermail" , new_id );
			//cookieにログインの情報を書き込みます。
			(*cookieArray)["account_usermail"] = new_id;
			(*cookieArray)["__regen__"] = "1";
			//マルチルームへも通知
			MultiRoomUtil::changeAccount(new_id);
		}
		else
		{
			const string old_id = MainWindow::m()->Config.Get( "account_usermail" , "");;
			const string old_password = MainWindow::m()->Config.Get( "account_password" , "");
			const string new_id = account_usermail;
			const string new_password = SystemMisc::MakePassword( account_usermail , account_password);
			const string uuid = MainWindow::m()->Httpd.GetUUID();

			//サーバーに変更を告げる
			string serverResult = MainWindow::m()->ColudSyncPooling.OpeartionChangeRegist(old_id,old_password,new_id,new_password,uuid,"0");
			if ( strstr(serverResult.c_str(),"OK") != serverResult.c_str() )
			{
				if ( serverResult.find("uuid already use") != string::npos )
				{
					return ScriptRemoconWeb::ResultError(39091 , "このUUIDは別の端末で使われています" );
				}
				return ScriptRemoconWeb::ResultError(39020 , "サーバーから拒絶されました" );
			}

			//ユーザーIDとパスワードを変更
			MainWindow::m()->Config.Set( "account_usermail" , new_id );
			MainWindow::m()->Config.Set( "account_password" , new_password);
			//SSHのパスワードの変更
			SystemMisc::SetSSHUserPassword(account_password);
			//cookieにログインの情報を書き込みます。
			(*cookieArray)["account_usermail"] = new_id;
			(*cookieArray)["account_password"] = new_password;
			(*cookieArray)["__regen__"] = "1";
			
			MultiRoomUtil::changeAccount(new_id);
		}

		MainWindow::m()->Config.Set( "account_sync_server" , account_sync_server);
	}

	IDPasswordUpdated();
	ConfigUpdated(false);
	return this->SettingStatus();
}

//ネットワークの設定を変更する通信 (設定を変えるだけ)
string ScriptWebRunner::setting_network_update(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	DEBUGLOG("request: " << XLStringUtil::crossjoin("=","&",request) );
	{
		string network_ipaddress_type = mapfind(request,"network_ipaddress_type");
		string network_ipaddress_ip = mapfind(request,"network_ipaddress_ip");
		string network_ipaddress_mask = mapfind(request,"network_ipaddress_mask");
		string network_ipaddress_gateway = mapfind(request,"network_ipaddress_gateway");
		string network_ipaddress_dns = mapfind(request,"network_ipaddress_dns");

		string network_w_ipaddress_type = mapfind(request,"network_w_ipaddress_type");
		string network_w_ipaddress_ip = mapfind(request,"network_w_ipaddress_ip");
		string network_w_ipaddress_mask = mapfind(request,"network_w_ipaddress_mask");
		string network_w_ipaddress_gateway = mapfind(request,"network_w_ipaddress_gateway");
		string network_w_ipaddress_dns = mapfind(request,"network_w_ipaddress_dns");
		string network_w_ipaddress_ssid = mapfind(request,"network_w_ipaddress_ssid");
		string network_w_ipaddress_password = mapfind(request,"network_w_ipaddress_password");
		string network_w_ipaddress_wkeymgmt = mapfind(request,"network_w_ipaddress_wkeymgmt");

		if(network_ipaddress_type == "DHCP")
		{
		}
		else if (network_ipaddress_type == "STATIC")
		{
			if (! XLStringUtil::checkIPAddressV4(network_ipaddress_ip) )return ScriptRemoconWeb::ResultError(40001,"bad network_ipaddress_ip");
			if (! XLStringUtil::checkIPAddressV4(network_ipaddress_mask) )return ScriptRemoconWeb::ResultError(40002,"bad network_ipaddress_mask");
			if (! XLStringUtil::checkIPAddressV4(network_ipaddress_gateway) )return ScriptRemoconWeb::ResultError(40003,"bad network_ipaddress_gateway");
			if (! XLStringUtil::checkIPAddressV4(network_ipaddress_dns) )return ScriptRemoconWeb::ResultError(40004,"bad network_ipaddress_dns");
		}
		else
		{
			return ScriptRemoconWeb::ResultError(40010,"bad network_ipaddress_type");
		}
		if(network_w_ipaddress_type == "DHCP" || network_w_ipaddress_type == "NONE")
		{
		}
		else if (network_ipaddress_type == "STATIC")
		{
			if (! XLStringUtil::checkIPAddressV4(network_w_ipaddress_ip) )return ScriptRemoconWeb::ResultError(40021,"bad network_w_ipaddress_ip");
			if (! XLStringUtil::checkIPAddressV4(network_w_ipaddress_mask) )return ScriptRemoconWeb::ResultError(40022,"bad network_w_ipaddress_mask");
			if (! XLStringUtil::checkIPAddressV4(network_w_ipaddress_gateway) )return ScriptRemoconWeb::ResultError(40023,"bad network_w_ipaddress_gateway");
			if (! XLStringUtil::checkIPAddressV4(network_w_ipaddress_dns) )return ScriptRemoconWeb::ResultError(40024,"bad network_w_ipaddress_dns");
		}
		else
		{
			network_w_ipaddress_type = "NONE";
		}

		if(network_w_ipaddress_type != "NONE")
		{
			if ( ! network_w_ipaddress_ssid.empty() )return ScriptRemoconWeb::ResultError(40040,"bad network_w_ipaddress_ssid");
		}

		//cable
		if (network_ipaddress_type == "DHCP")
		{
			network_ipaddress_ip = "DHCP";
			network_ipaddress_mask = "";
			network_ipaddress_gateway = "";
			network_ipaddress_dns = "";
		}
		else if (network_ipaddress_type == "NONE")
		{
			network_ipaddress_ip = "NONE";
			network_ipaddress_mask = "";
			network_ipaddress_gateway = "";
			network_ipaddress_dns = "";
		}

		//wifi
		if (network_w_ipaddress_type == "DHCP")
		{
			network_w_ipaddress_ip = "DHCP";
			network_w_ipaddress_mask = "";
			network_w_ipaddress_gateway = "";
			network_w_ipaddress_dns = "";
		}
		else if (network_w_ipaddress_type == "NONE")
		{
			network_w_ipaddress_ip = "NONE";
			network_w_ipaddress_mask = "";
			network_w_ipaddress_gateway = "";
			network_w_ipaddress_dns = "";
			network_w_ipaddress_ssid = "";
			network_w_ipaddress_password = "";
			network_w_ipaddress_wkeymgmt = "";
		}

		MainWindow::m()->Config.Set( "network_ipaddress_type" , network_ipaddress_type );
		MainWindow::m()->Config.Set( "network_ipaddress_ip" , network_ipaddress_ip );
		MainWindow::m()->Config.Set( "network_ipaddress_mask" , network_ipaddress_mask );
		MainWindow::m()->Config.Set( "network_ipaddress_gateway" , network_ipaddress_gateway );
		MainWindow::m()->Config.Set( "network_ipaddress_dns" , network_ipaddress_dns );

		MainWindow::m()->Config.Set( "network_w_ipaddress_type" , network_w_ipaddress_type );
		MainWindow::m()->Config.Set( "network_w_ipaddress_ip" , network_w_ipaddress_ip );
		MainWindow::m()->Config.Set( "network_w_ipaddress_mask" , network_w_ipaddress_mask );
		MainWindow::m()->Config.Set( "network_w_ipaddress_gateway" , network_w_ipaddress_gateway );
		MainWindow::m()->Config.Set( "network_w_ipaddress_dns" , network_w_ipaddress_dns );
		MainWindow::m()->Config.Set( "network_w_ipaddress_ssid" , network_w_ipaddress_ssid );
		MainWindow::m()->Config.Set( "network_w_ipaddress_password" , network_w_ipaddress_password );
		MainWindow::m()->Config.Set( "network_w_ipaddress_wkeymgmt" , network_w_ipaddress_wkeymgmt );

		ConfigUpdated(false);

		//ネットワークの設定更新
		SystemMisc::SetIPAddress(network_ipaddress_ip,network_ipaddress_mask,network_ipaddress_gateway,network_ipaddress_dns,network_w_ipaddress_ip,network_w_ipaddress_mask,network_w_ipaddress_gateway,network_w_ipaddress_dns,network_w_ipaddress_ssid,network_w_ipaddress_password,network_w_ipaddress_wkeymgmt);
	}

	return this->SettingStatus();
}


string ScriptWebRunner::setting_recong_update(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	DEBUGLOG("request: " << XLStringUtil::crossjoin("=","&",request) );
	{
		const string recong_type_old = MainWindow::m()->Config.Get( "recong_type" , "" );

		const string recong_type = mapfind(request,"recong_type");
		const string recong_julius_gomi_y = mapfind(request,"recong_julius_gomi_y");
		const string recong_julius_gomi_e = mapfind(request,"recong_julius_gomi_e");
		const string recong_julius_gomi_d = mapfind(request,"recong_julius_gomi_d");
		const string recong_julius_filter1 = mapfind(request,"recong_julius_filter1");
		const string recong_julius_filter2 = mapfind(request,"recong_julius_filter2");
		const string recong_volume = mapfind(request,"recong_volume");
		const string recong_julius_lv_base = mapfind(request,"recong_julius_lv_base");
		const string recong_julius_lv_max = mapfind(request,"recong_julius_lv_max");
		const string recong_ok_mp3 = mapfind(request,"recong_ok_mp3");
		const string recong_ng_mp3 = mapfind(request,"recong_ng_mp3");
		const string recong_pause_mp3 = mapfind(request,"recong_pause_mp3");
		const string recong__yobikake__1 = mapfind(request,"recong__yobikake__1");
		const string recong__yobikake__2 = mapfind(request,"recong__yobikake__2");
		const string recong__yobikake__3 = mapfind(request,"recong__yobikake__3");
		const string recong__yobikake__4 = mapfind(request,"recong__yobikake__4");
		const string recong__yobikake__5 = mapfind(request,"recong__yobikake__5");

		if (!MainWindow::m()->Recognition.checkYomi(recong__yobikake__1))
		{
			return ScriptRemoconWeb::ResultError(39051 , "読み上げできない文字列が含まれています");
		}
		if (XLStringUtil::mb_strlen(recong__yobikake__1) < 3)
		{
			return ScriptRemoconWeb::ResultError(39061 , "呼びかけが短すぎます");
		}
		const string firstChar = XLStringUtil::mb_substr(recong__yobikake__1,0,2);
		if (recong__yobikake__2 != "")
		{
			if (!MainWindow::m()->Recognition.checkYomi(recong__yobikake__2))
			{
				return ScriptRemoconWeb::ResultError(39052 , "読み上げできない文字列が含まれています");
			}
			if (XLStringUtil::mb_strlen(recong__yobikake__2) < 3)
			{
				return ScriptRemoconWeb::ResultError(39062 , "呼びかけが短すぎます");
			}
			if (firstChar != XLStringUtil::mb_substr(recong__yobikake__2,0,2) )
			{
				return ScriptRemoconWeb::ResultError(39072 , "呼びかけはすべて同じ2文字から始まる必要があります");
			}
		}
		if (recong__yobikake__3 != "")
		{
			if (!MainWindow::m()->Recognition.checkYomi(recong__yobikake__3))
			{
				return ScriptRemoconWeb::ResultError(39053 , "読み上げできない文字列が含まれています");
			}
			if (XLStringUtil::mb_strlen(recong__yobikake__3) < 3)
			{
				return ScriptRemoconWeb::ResultError(39063 , "呼びかけが短すぎます");
			}
			if (firstChar != XLStringUtil::mb_substr(recong__yobikake__3,0,2) )
			{
				return ScriptRemoconWeb::ResultError(39073 , "呼びかけはすべて同じ2文字から始まる必要があります");
			}
		}
		if (recong__yobikake__4 != "")
		{
			if (!MainWindow::m()->Recognition.checkYomi(recong__yobikake__4))
			{
				return ScriptRemoconWeb::ResultError(39054 , "読み上げできない文字列が含まれています");
			}
			if (XLStringUtil::mb_strlen(recong__yobikake__4) < 3)
			{
				return ScriptRemoconWeb::ResultError(39064 , "呼びかけが短すぎます");
			}
			if (firstChar != XLStringUtil::mb_substr(recong__yobikake__4,0,2) )
			{
				return ScriptRemoconWeb::ResultError(39074 , "呼びかけはすべて同じ2文字から始まる必要があります");
			}
		}
		if (recong__yobikake__5 != "")
		{
			if (!MainWindow::m()->Recognition.checkYomi(recong__yobikake__5))
			{
				return ScriptRemoconWeb::ResultError(39055 , "読み上げできない文字列が含まれています");
			}
			if (XLStringUtil::mb_strlen(recong__yobikake__5) < 3)
			{
				return ScriptRemoconWeb::ResultError(39065 , "呼びかけが短すぎます");
			}
			if (firstChar != XLStringUtil::mb_substr(recong__yobikake__5,0,2) )
			{
				return ScriptRemoconWeb::ResultError(39075 , "呼びかけはすべて同じ2文字から始まる必要があります");
			}
		}

		MainWindow::m()->Config.Set( "recong_type" , recong_type );
		MainWindow::m()->Config.Set( "recong_julius_gomi_y" , recong_julius_gomi_y );
		MainWindow::m()->Config.Set( "recong_julius_gomi_e" , recong_julius_gomi_e );
		MainWindow::m()->Config.Set( "recong_julius_gomi_d" , recong_julius_gomi_d );
		MainWindow::m()->Config.Set( "recong_julius_filter1" , recong_julius_filter1 );
		MainWindow::m()->Config.Set( "recong_julius_filter2" , recong_julius_filter2 );
		MainWindow::m()->Config.Set( "recong_julius_lv_base" , recong_julius_lv_base );
		MainWindow::m()->Config.Set( "recong_julius_lv_max"  , recong_julius_lv_max );
		MainWindow::m()->Config.Set( "recong_volume" , recong_volume );
		MainWindow::m()->Config.Set( "recong_ok_mp3" , recong_ok_mp3 );
		MainWindow::m()->Config.Set( "recong_ng_mp3" , recong_ng_mp3 );
		MainWindow::m()->Config.Set( "recong_pause_mp3" , recong_pause_mp3 );
		MainWindow::m()->Config.Set( "recong__yobikake__1" , recong__yobikake__1 );
		MainWindow::m()->Config.Set( "recong__yobikake__2" , recong__yobikake__2 );
		MainWindow::m()->Config.Set( "recong__yobikake__3" , recong__yobikake__3 );
		MainWindow::m()->Config.Set( "recong__yobikake__4" , recong__yobikake__4 );
		MainWindow::m()->Config.Set( "recong__yobikake__5" , recong__yobikake__5 );
		//音量を変える
		MainWindow::m()->VolumeControll.ChangeMicVolume();

		{
			//音声認識エンジンを再構築する
			int errortype = 0;
			string errorMessage;
			MainWindow::m()->SyncInvoke([&](){
				try
				{
					MainWindow::m()->ReCreateRecognitionEngine();
				}
				catch(XLException& e)
				{
					errortype = 1;
					errorMessage = e.what();
					ERRORLOG("webエラー " << errorMessage);
				}
			});

			if (errortype != 0)
			{
				return ScriptRemoconWeb::ResultError(39020 , errorMessage);
			}
		}
	}

	ConfigUpdated(false);
	return this->SettingStatus();
}

string ScriptWebRunner::setting_sip_update(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	{
		const string sip_enable_incoming  = mapfind(request,"sip_enable_incoming");
		const string sip_domain = mapfind(request,"sip_domain");
		const string sip_password = mapfind(request,"sip_password");
		const string sip_id = mapfind(request,"sip_id");
		const string sip_enable = mapfind(request,"sip_enable");
		const string sip_incoming_mp3 = mapfind(request,"sip_incoming_mp3");
		const string sip_calling_mp3 = mapfind(request,"sip_calling_mp3");
		const string sip_when_taking_stop_recogn = mapfind(request,"sip_when_taking_stop_recogn");

		MainWindow::m()->Config.Set( "sip_enable_incoming" , sip_enable_incoming );
		MainWindow::m()->Config.Set( "sip_domain" , sip_domain );
		MainWindow::m()->Config.Set( "sip_password" , sip_password );
		MainWindow::m()->Config.Set( "sip_id" , sip_id );
		MainWindow::m()->Config.Set( "sip_enable" , sip_enable );
		MainWindow::m()->Config.Set( "sip_incoming_mp3" , sip_incoming_mp3 );
		MainWindow::m()->Config.Set( "sip_calling_mp3" , sip_calling_mp3 );
		MainWindow::m()->Config.Set( "sip_when_taking_stop_recogn" , sip_when_taking_stop_recogn );
/*
		if ( stringbool(sip_enable) )
		{//SIPを有効にする.
			MainWindow::m()->SipServer.Create();
			
			//認証の是非のチェック
			SipServer::AUTH auth = MainWindow::m()->SipServer.WaitForAuth(10);
			if (auth != SipServer::AUTH_OK )
			{
				return ScriptRemoconWeb::ResultError(39021 , "認証に失敗ました。");
			}
		}
		else
		{//SIPを無効にする.
			MainWindow::m()->SipServer.Stop();
		}
*/
	}

	ConfigUpdated(false);
	return this->SettingStatus();
}


string ScriptWebRunner::setting_speak_update(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	DEBUGLOG("request: " << XLStringUtil::crossjoin("=","&",request) );
	{
		const string speak_type_old  = MainWindow::m()->Config.Get( "speak_type" , "" );
		const string speak_type = mapfind(request,"speak_type");
		const string speak_volume = mapfind(request,"speak_volume");
		const string speak_use_remote = mapfind(request,"speak_use_remote");
		const string speak_remote1_ip = mapfind(request,"speak_remote1_ip");
		const string speak_remote1_secretkey = mapfind(request,"speak_remote1_secretkey");

		const string speak_use_nightvolume = mapfind(request,"speak_use_nightvolume");
		const string speak_nightvolume_start_hour = mapfind(request,"speak_nightvolume_start_hour");
		const string speak_nightvolume_start_minute = mapfind(request,"speak_nightvolume_start_minute");
		const string speak_nightvolume_end_hour = mapfind(request,"speak_nightvolume_end_hour");
		const string speak_nightvolume_end_minute = mapfind(request,"speak_nightvolume_end_minute");
		const string speak_nightvolume_volume = mapfind(request,"speak_nightvolume_volume");

		if (speak_use_remote == "1")
		{
			//FHCコマンダーとの接続テスト
			if (! FHCCommander::checkAuth(speak_remote1_ip,speak_remote1_secretkey))
			{//接続エラー
				return ScriptRemoconWeb::ResultError(39030 , "FHCコマンダーに接続できません。");
			}
		}

		MainWindow::m()->Config.Set( "speak_type" , speak_type );
		MainWindow::m()->Config.Set( "speak_volume" , speak_volume );
		MainWindow::m()->Config.Set( "speak_nightvolume_volume" , speak_nightvolume_volume );
		MainWindow::m()->Config.Set( "speak_use_remote" , speak_use_remote );
		if (speak_use_remote == "1")
		{
			MainWindow::m()->Config.Set( "speak_remote1_ip" , speak_remote1_ip );
			MainWindow::m()->Config.Set( "speak_remote1_secretkey" , speak_remote1_secretkey );
		}
		else
		{
			MainWindow::m()->Config.Remove( "speak_remote1_ip"  );
			MainWindow::m()->Config.Remove( "speak_remote1_secretkey");
		}

		MainWindow::m()->Config.Set( "speak_use_nightvolume" , speak_use_nightvolume );
		if (speak_use_nightvolume == "1")
		{
			MainWindow::m()->Config.Set( "speak_nightvolume_start_hour" , speak_nightvolume_start_hour );
			MainWindow::m()->Config.Set( "speak_nightvolume_start_minute" , speak_nightvolume_start_minute );
			MainWindow::m()->Config.Set( "speak_nightvolume_end_hour" , speak_nightvolume_end_hour );
			MainWindow::m()->Config.Set( "speak_nightvolume_end_minute" , speak_nightvolume_end_minute );
		}
		else
		{
			MainWindow::m()->Config.Remove( "speak_nightvolume_start_hour" );
			MainWindow::m()->Config.Remove( "speak_nightvolume_start_minute");
			MainWindow::m()->Config.Remove( "speak_nightvolume_end_hour" );
			MainWindow::m()->Config.Remove( "speak_nightvolume_end_minute" );
		}

		//音量を変える
		MainWindow::m()->VolumeControll.ChangeSpeakerVolume();

		//合成音声エンジンを再構築する
		if (speak_type != speak_type_old)
		{
			int errortype = 0;
			string errorMessage;
			MainWindow::m()->SyncInvoke([&](){
				try
				{
					//スピーチキャッシュをすべてクリアする.
					MainWindow::m()->Speak.ClearSpeakCacheDB();

					//スピーチエンジンの再構築.
					MainWindow::m()->ReCreateSpeakEngine();
					Speak.ReloadSpeak(false);
				}
				catch(XLException& e)
				{
					errortype = 1;
					errorMessage = e.what();
					ERRORLOG("webエラー " << errorMessage);
				}
			});

			if (errortype != 0)
			{
				return ScriptRemoconWeb::ResultError(39020 , errorMessage);
			}
		}

	}

	ConfigUpdated(false);
	return this->SettingStatus();
}

//アップグレードチェック
string ScriptWebRunner::setting_version_upgradecheck(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	DEBUGLOG("request: " << XLStringUtil::crossjoin("=","&",request) );
	{
		const string version_updateurl = mapfind(request,"version_updateurl");
		const string version = num2str(MainWindow::m()->Config.GetInt("version_number",100) / 100.f);
		const string model = SystemMisc::GetModel();
#if _MSC_VER
		const string os = "windows";
#else
		const string os = "linux";
#endif
		const string url = version_updateurl + "?version=" + version + "&os=" + os + "&model=" + model + "&type=check";

		try
		{
			const string checkvertion = XLHttpSocket::Get(url , 5 );
			DEBUGLOG("バージョンチェックの戻り値:" + checkvertion );

			const auto vec = XLStringUtil::split_vector("\t",checkvertion);
			if (vec.empty() || vec[0] != "OK")
			{
				DEBUGLOG("アップデートの必要はありません:" + checkvertion );
				return ScriptRemoconWeb::ResultError(20003 , "アップデートの必要はありません");
			}
		}
		catch(XLException &e)
		{
			DEBUGLOG("webサーバに接続出来ません: " << e.what() );
			return ScriptRemoconWeb::ResultError(20003 , "アップデートの必要はありません");
		}

		return this->SettingStatus();
	}

}

//アップグレード実行
string ScriptWebRunner::setting_version_upgraderun(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	DEBUGLOG("request: " << XLStringUtil::crossjoin("=","&",request) );
	{
		const string version_updateurl = mapfind(request,"version_updateurl");
		const string version = num2str(MainWindow::m()->Config.GetInt("version_number",100) / 100.f);
		const string model = SystemMisc::GetModel();
#if _MSC_VER
		const string os = "windows";
#else
		const string os = "linux";
#endif
		const string url = version_updateurl + "?version=" + version + "&os=" + os + "&model=" + model + "&type=download";

		try
		{
#if _MSC_VER
			{//何もできないんだなこれが
			}
#else
			{
				//サイズが巨大になってしまうのでアップデーターは裏で実行する
				XLFileUtil::write( "/tmp/naichichi2_updater_url" , url );
				XLFileUtil::write( "/tmp/naichichi2_import_dir" , "" );
			}
#endif
		}
		catch(XLException &e)
		{
			return ScriptRemoconWeb::ResultError(20003 , e.what() );
		}

		MainWindow::m()->Shutdown(EXITCODE_LEVEL_UPGRADE , true);
		return this->SettingStatus();
	}
}

//再起動させる
string ScriptWebRunner::setting_version_reboot(const XLHttpHeader& httpHeaders) 
{
	ConfigUpdated(true);

	const map<string,string> request = httpHeaders.getPost();
	DEBUGLOG("request: " << XLStringUtil::crossjoin("=","&",request) );
	{
		MainWindow::m()->Shutdown(EXITCODE_LEVEL_REBOOT , true);
	}

	return this->SettingStatus();
}

//終了させる
string ScriptWebRunner::setting_version_shutdown(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	DEBUGLOG("request: " << XLStringUtil::crossjoin("=","&",request) );
	{
		MainWindow::m()->Shutdown(EXITCODE_LEVEL_NORMALEND , true);
	}

	return this->SettingStatus();
}


//ログを取得する
string ScriptWebRunner::setting_version_log(const XLHttpHeader& httpHeaders)  const
{
	
	return _A2U( sexylog::m()->Tail(300) );
}

//サポート用のtar玉の取得
string ScriptWebRunner::setting_version_download_supportfile(const XLHttpHeader& httpHeaders)  const
{
	MainWindow::m()->SyncFiles();
	return SystemMisc::SupportDump();
}


string ScriptWebRunner::setting_twitter_regist(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result)  const
{
	TwitterOAuth tw;
	string oauth_token;
	string oauth_token_secret;
	string auth_url;
	const string callback_url = MainWindow::m()->Httpd.getWebURL("/setting/twitter/callback");
	if (!tw.Auth(callback_url,&oauth_token,&oauth_token_secret,&auth_url) )
	{
		ERRORLOG("twitter認証失敗:" << tw.what() );
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		return string("twitter auth error! ") + tw.what();
	}
	MainWindow::m()->Config.Set( "twitter_oauth_token" , oauth_token );
	MainWindow::m()->Config.Set( "twitter_oauth_token_secret" , oauth_token_secret );

	*result = WEBSERVER_RESULT_TYPE_LOCATION;
	return auth_url;
}

string ScriptWebRunner::setting_twitter_callback(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result)  
{
	TwitterOAuth tw;
	const map<string,string> request = httpHeaders.getRequest();

	const string oauth_token =	MainWindow::m()->Config.Get( "twitter_oauth_token" );
	const string oauth_token_secret = MainWindow::m()->Config.Get( "twitter_oauth_token_secret" );
	const string oauth_verifier = mapfind(request,"oauth_verifier","");
	if (oauth_token.empty())
	{
		ERRORLOG("セッションにoauth_tokenがない");
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		return string("セッションにoauth_tokenがない");
	}
	if (oauth_token_secret.empty())
	{
		ERRORLOG("セッションにoauth_token_secretがない");
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		return string("セッションにoauth_token_secretがない");
	}
	if (oauth_verifier.empty())
	{
		ERRORLOG("コールバックパラメータにoauth_verifierがない");
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		return string("コールバックパラメータにoauth_verifierがない");
	}

	string oauth_access_token;
	string oauth_access_token_secret;
	string user_id;
	string screen_name;
	if (!tw.AuthCallback(oauth_token
		,oauth_token_secret
		,oauth_verifier
		,&oauth_access_token
		,&oauth_access_token_secret
		,&user_id
		,&screen_name
		) )
	{
		ERRORLOG("twitter認証失敗:" << tw.what() );
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		return string("twitter auth error! ") + tw.what();
	}

	MainWindow::m()->Config.Set( "twitter_oauth_token" , oauth_access_token );
	MainWindow::m()->Config.Set( "twitter_oauth_token_secret" , oauth_access_token_secret );
	MainWindow::m()->Config.Set( "twitter_user_id" , user_id );
	MainWindow::m()->Config.Set( "twitter_screen_name" , screen_name );

	ConfigUpdated(false);

	*result = WEBSERVER_RESULT_TYPE_LOCATION;
	const string setting_url = MainWindow::m()->Httpd.getWebURL("/edit/#system");
	return setting_url;
}
string ScriptWebRunner::setting_twitter_cancel(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result)  
{
	MainWindow::m()->Config.Set( "twitter_oauth_token" , "" );
	MainWindow::m()->Config.Set( "twitter_oauth_token_secret" , "" );
	MainWindow::m()->Config.Set( "twitter_user_id" , "" );
	MainWindow::m()->Config.Set( "twitter_screen_name" , "" );

	ConfigUpdated(false);

	*result = WEBSERVER_RESULT_TYPE_LOCATION;
	const string setting_url = MainWindow::m()->Httpd.getWebURL("/edit/#system");
	return setting_url;
}
//最新の音声認識のログを取得する
string ScriptWebRunner::mictest_log(const XLHttpHeader& httpHeaders) const
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string lasttimeString = mapfind(request,"lasttime","0");
	const string limitString = mapfind(request,"limit","5");
	const time_t lasttime = atou(lasttimeString);
	const unsigned int limit = atoi(limitString) ;

	string lastlog;
	if (lasttime == 0 )
	{
		lastlog = sexylog::m()->Tail(300);
	}
	else
	{
		lastlog = sexylog::m()->TailByte(1024*10);
	}

	vector<string> pickerVec;
	
	//改行で分離して見ていきます。
	const auto lines = XLStringUtil::split_vector("\n",lastlog);
	for(auto lineIT = lines.begin() ; lineIT != lines.end() ; lineIT++ )
	{
		string rrr = *lineIT;
		//ログデータは\tでゆるく切られているので、それでばらしていきます。
		const auto parts = XLStringUtil::split_vector("\t",*lineIT);
		if (parts.size() <= 4)
		{
			continue;
		}

//		2013/9/14 8:32:9	3	OnOutputResult_2Pass:363	認識結果:gomi (koka) file://aaaa.wav GM
		if ( parts[2].find("OutputResult") == string::npos )
		{
			continue;
		}

		//最後に符号がつきます。2バイト固定です
		string code = parts[ parts.size()-1 ];
		if (code.size() != 2+1 || (('A' >= code[0] && 'Z' <= code[0]) && ('A' >= code[1] && 'Z' <= code[1]) ) )
		{
			continue;
		}
		code = code.substr(0,2);

		//時間が今より新しいかどうか
		const time_t logtime = XLStringUtil::strtotime(parts[0]);
		if (lasttime != 0 )
		{
			if (lasttime >= logtime)
			{
				continue;
			}
		}

		//ログ本文を取得する.
		string body;
		for(unsigned int i = 3 ; i < parts.size()-1 ; i ++)
		{
			body += parts[i];
		}
		
		const string filename = XLStringUtil::cut(body," file:","");
		const string match    = XLStringUtil::cut(body," match:(",")");
		const string matchd   = XLStringUtil::cut(body,"[matchD:(",")");

		pickerVec.push_back(
		              string("{") + XLStringUtil::jsonescape("time") + ": "   + XLStringUtil::jsonescape(num2str(logtime)) 
				   += string(",") + XLStringUtil::jsonescape("file") + ": "   + XLStringUtil::jsonescape(filename) 
				   += string(",") + XLStringUtil::jsonescape("code") + ": "   + XLStringUtil::jsonescape(code) 
				   += string(",") + XLStringUtil::jsonescape("match") + ": "  + XLStringUtil::jsonescape(match) 
				   += string(",") + XLStringUtil::jsonescape("matchd") + ": " + XLStringUtil::jsonescape(matchd) 
				   += string(",") + XLStringUtil::jsonescape("timef") + ": "   + XLStringUtil::jsonescape(parts[0]) 
				   += string(",") + XLStringUtil::jsonescape("body") + ": "   + XLStringUtil::jsonescape(body) 
				   += string("}")
	   );
	}
	string jsonstring;
	{
		unsigned int i = 0;
		if (pickerVec.size() > limit)
		{
			i = pickerVec.size() - limit;
		}
		for( ; i < pickerVec.size() ; i++ )
		{
			if (!jsonstring.empty()) jsonstring += ",";
			jsonstring += pickerVec[i];
		}
	}
	jsonstring = string("{ \"result\": \"ok\", \"log\": [") + jsonstring + "]}";
	return _A2U(jsonstring.c_str());
}

//録音されたファイルをダウンロード
string ScriptWebRunner::mictest_wavdownload(const XLHttpHeader& httpHeaders) const
{
	
	const map<string,string> request = httpHeaders.getGet();
	const string name = mapfind(request,"name");

	if (! XLStringUtil::checkSafePath(name) )
	{
		return "";
	}

	if (name[0] != 'F')
	{
		return "";
	}


	const string filepath = XLFileUtil::getTempDirectory("naichichi2") + "/"+name + ".wav";
	return filepath;
}

string ScriptWebRunner::setting_sensor_getall(const XLHttpHeader& httpHeaders) 
{
	return GetSensorJson();
}


string ScriptWebRunner::setting_sensor_update(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	DEBUGLOG("request: " << XLStringUtil::crossjoin("=","&",request) );
	{
		const string sensor_temp_type = mapfind(request,"sensor_temp_type");
		const string sensor_lumi_type = mapfind(request,"sensor_lumi_type");
		const string sensor_sound_type = mapfind(request,"sensor_sound_type");

		MainWindow::m()->Config.Set( "sensor_temp_type" , sensor_temp_type );
		MainWindow::m()->Config.Set( "sensor_lumi_type" , sensor_lumi_type );
		MainWindow::m()->Config.Set( "sensor_sound_type" , sensor_sound_type );
	}

	ConfigUpdated(true);
	return this->SettingStatus();
}

string ScriptWebRunner::setting_system_update(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	DEBUGLOG("request: " << XLStringUtil::crossjoin("=","&",request) );
	{
		const string system_fhcboot_mp3 = mapfind(request,"system_fhcboot_mp3");
		const string weather_script_command = mapfind(request,"weather_script_command");
		const string train_script_command = mapfind(request,"train_script_command");

		MainWindow::m()->Config.Set( "system_fhcboot_mp3" , system_fhcboot_mp3 );
		MainWindow::m()->Config.Set( "weather_script_command" , weather_script_command );
		MainWindow::m()->Config.Set( "train_script_command" , train_script_command );
	}

	ConfigUpdated(true);
	return this->SettingStatus();
}

string ScriptWebRunner::setting_changelang_update(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	DEBUGLOG("request: " << XLStringUtil::crossjoin("=","&",request) );
	{
		const string system_lang = mapfind(request,"system_lang");

		const string system_lang_old = MainWindow::m()->Config.Get( "system_lang" , "japanese" );

		MainWindow::m()->Config.Set( "system_lang" , system_lang );

		//言語設定が更新されている？
		if (system_lang_old != system_lang)
		{
			//設定の変更.
			ScriptRemoconWeb::ChangeLangSetting(system_lang);

			int errortype = 0;
			string errorMessage;

			if (errortype != 0)
			{
				ERRORLOG("webエラー " << errorMessage);
				return ScriptRemoconWeb::ResultError(39020 , errorMessage);
			}
		}
	}

	ConfigUpdated(true);
	return this->SettingStatus();
}

string ScriptWebRunner::setting_remoconsetting_update(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	DEBUGLOG("request: " << XLStringUtil::crossjoin("=","&",request) );
	{
		const string old_system_room_string = MainWindow::m()->Config.Get("system_room_string");

		const string system_room_string = mapfind(request,"system_room_string");
		const string system_room_color = mapfind(request,"system_room_color");
		const string system_room_icon = mapfind(request,"system_room_icon");
		const string remoconsetting_default_font_color = mapfind(request,"remoconsetting_default_font_color","#666666");
		const string remoconsetting_default_background_color = mapfind(request,"remoconsetting_default_background_color");

		const string remoconsetting_background_image = mapfind(request,"remoconsetting_background_image");
		const string remoconsetting_menu_icon = mapfind(request,"remoconsetting_menu_icon");
		const string remoconsetting_sensor_font_color = mapfind(request,"remoconsetting_sensor_font_color");
		const string remoconsetting_sensor_temp_string = mapfind(request,"remoconsetting_sensor_temp_string");
		const string remoconsetting_sensor_lumi_string = mapfind(request,"remoconsetting_sensor_lumi_string");

		const string remoconsetting_footermenu_remoconedit_string = mapfind(request,"remoconsetting_footermenu_remoconedit_string");
		const string remoconsetting_footermenu_remoconedit_icon = mapfind(request,"remoconsetting_footermenu_remoconedit_icon");
		const string remoconsetting_footermenu_setting_string = mapfind(request,"remoconsetting_footermenu_setting_string");
		const string remoconsetting_footermenu_setting_icon = mapfind(request,"remoconsetting_footermenu_setting_icon");
		const string remoconsetting_footermenu_font_color = mapfind(request,"remoconsetting_footermenu_font_color");
		const string remoconsetting_footermenu_background_color = mapfind(request,"remoconsetting_footermenu_background_color");
		const string remoconsetting_footermenu_border_color = mapfind(request,"remoconsetting_footermenu_border_color");
		const string remoconsetting_footermenu_hover_color = mapfind(request,"remoconsetting_footermenu_hover_color");

		const string remoconsetting_dialog_close_string = mapfind(request,"remoconsetting_dialog_close_string");
		const string remoconsetting_dialog_font_color = mapfind(request,"remoconsetting_dialog_font_color","#000000");
		const string remoconsetting_dialog_background_color = mapfind(request,"remoconsetting_dialog_background_color");
		const string remoconsetting_dialog_border_color = mapfind(request,"remoconsetting_dialog_border_color");
		const string remoconsetting_dialog_donot_autoclose = mapfind(request,"remoconsetting_dialog_donot_autoclose");
		const string remoconsetting_dialog_iconcount_pc = mapfind(request,"remoconsetting_dialog_iconcount_pc");

		const string remoconsetting_button_font_color = mapfind(request,"remoconsetting_button_font_color","#000000");
		const string remoconsetting_button_background_color1 = mapfind(request,"remoconsetting_button_background_color1","#f4f5f5");
		const string remoconsetting_button_background_color2 = mapfind(request,"remoconsetting_button_background_color2","#dddddd");
		const string remoconsetting_button_border_color = mapfind(request,"remoconsetting_button_border_color","#d7dada");
		const string remoconsetting_button_hover_color1 = mapfind(request,"remoconsetting_button_hover_color1","#f4f5f5");
		const string remoconsetting_button_hover_color2 = mapfind(request,"remoconsetting_button_hover_color2","#c6c3c3");
		const string remoconsetting_button_hover_border_color = mapfind(request,"remoconsetting_button_hover_border_color","#bfc4c4");
		const string remoconsetting_badges_timer_icon = mapfind(request,"remoconsetting_badges_timer_icon","timer.png");
	
		MainWindow::m()->Config.Set("system_room_string",system_room_string);
		MainWindow::m()->Config.Set("system_room_color",system_room_color);
		MainWindow::m()->Config.Set("system_room_icon",system_room_icon);
		MainWindow::m()->Config.Set("remoconsetting_default_font_color",remoconsetting_default_font_color);
		MainWindow::m()->Config.Set("remoconsetting_default_background_color",remoconsetting_default_background_color);

		MainWindow::m()->Config.Set("remoconsetting_background_image",remoconsetting_background_image);
		MainWindow::m()->Config.Set("remoconsetting_menu_icon",remoconsetting_menu_icon);
		
		MainWindow::m()->Config.Set("remoconsetting_sensor_font_color",remoconsetting_sensor_font_color);
		MainWindow::m()->Config.Set("remoconsetting_sensor_temp_string",remoconsetting_sensor_temp_string);
		MainWindow::m()->Config.Set("remoconsetting_sensor_lumi_string",remoconsetting_sensor_lumi_string);

		MainWindow::m()->Config.Set("remoconsetting_footermenu_remoconedit_string",remoconsetting_footermenu_remoconedit_string);
		MainWindow::m()->Config.Set("remoconsetting_footermenu_remoconedit_icon",remoconsetting_footermenu_remoconedit_icon);
		MainWindow::m()->Config.Set("remoconsetting_footermenu_setting_string",remoconsetting_footermenu_setting_string);
		MainWindow::m()->Config.Set("remoconsetting_footermenu_setting_icon",remoconsetting_footermenu_setting_icon);
		MainWindow::m()->Config.Set("remoconsetting_footermenu_font_color",remoconsetting_footermenu_font_color);
		MainWindow::m()->Config.Set("remoconsetting_footermenu_background_color",remoconsetting_footermenu_background_color);
		MainWindow::m()->Config.Set("remoconsetting_footermenu_border_color",remoconsetting_footermenu_border_color);
		MainWindow::m()->Config.Set("remoconsetting_footermenu_hover_color",remoconsetting_footermenu_hover_color);

		MainWindow::m()->Config.Set("remoconsetting_dialog_close_string",remoconsetting_dialog_close_string);
		MainWindow::m()->Config.Set("remoconsetting_dialog_font_color",remoconsetting_dialog_font_color);
		MainWindow::m()->Config.Set("remoconsetting_dialog_background_color",remoconsetting_dialog_background_color);
		MainWindow::m()->Config.Set("remoconsetting_dialog_border_color",remoconsetting_dialog_border_color);
		MainWindow::m()->Config.Set("remoconsetting_dialog_donot_autoclose",remoconsetting_dialog_donot_autoclose);
		MainWindow::m()->Config.Set("remoconsetting_dialog_iconcount_pc",remoconsetting_dialog_iconcount_pc);

		MainWindow::m()->Config.Set("remoconsetting_button_font_color",remoconsetting_button_font_color);
		MainWindow::m()->Config.Set("remoconsetting_button_background_color1",remoconsetting_button_background_color1);
		MainWindow::m()->Config.Set("remoconsetting_button_background_color2",remoconsetting_button_background_color2);
		MainWindow::m()->Config.Set("remoconsetting_button_border_color",remoconsetting_button_border_color);
		MainWindow::m()->Config.Set("remoconsetting_button_hover_color1",remoconsetting_button_hover_color1);
		MainWindow::m()->Config.Set("remoconsetting_button_hover_color2",remoconsetting_button_hover_color2);
		MainWindow::m()->Config.Set("remoconsetting_button_hover_border_color",remoconsetting_button_hover_border_color);
		MainWindow::m()->Config.Set("remoconsetting_button_background_color1",remoconsetting_button_background_color1);
		MainWindow::m()->Config.Set("remoconsetting_badges_timer_icon",remoconsetting_badges_timer_icon);

		if(old_system_room_string != system_room_string)
		{//部屋の名前を変えたら・・・
			MultiRoomUtil::changeName(system_room_string);
		}
	}

	ConfigUpdated(false);
	return this->SettingStatus();
}


string ScriptWebRunner::setting_trigger_enable_change(const XLHttpHeader& httpHeaders)
{
	const map<string,string> request = httpHeaders.getPost();
	DEBUGLOG("request: " << XLStringUtil::crossjoin("=","&",request) );
	{
		const string key = mapfind(request,"key");
		const string enable = mapfind(request,"enable");

		string prefix = "trigger_" + num2str( atoi(key.c_str()) );
		MainWindow::m()->Config.Set(prefix + "_enable", num2str( atoi(enable.c_str() ) ) );
	}
	ConfigUpdated(false);
	return this->TriggerStatus();
}

string ScriptWebRunner::setting_trigger_update(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	DEBUGLOG("request: " << XLStringUtil::crossjoin("=","&",request) );
	{
		const string trigger_key = mapfind(request,"trigger_key");
		const string trigger_if_type = mapfind(request,"trigger_if_type");
		const string trigger_if_date_hour = mapfind(request,"trigger_if_date_hour");
		const string trigger_if_date_minute = mapfind(request,"trigger_if_date_minute");
		const string trigger_if_date_loop_use = mapfind(request,"trigger_if_date_loop_use");
		const string trigger_if_date_loop_dayofweek = mapfind(request,"trigger_if_date_loop_dayofweek");
		const string trigger_if_date_loop_holiday = mapfind(request,"trigger_if_date_loop_holiday");
		const string trigger_if_date_loop_holiday_dataof = mapfind(request,"trigger_if_date_loop_holiday_dataof");
		const string trigger_if_date_loop_hourly = mapfind(request,"trigger_if_date_loop_hourly");

		const string trigger_if_sensor_type = mapfind(request,"trigger_if_sensor_type");
		const string trigger_if_sensor_value = mapfind(request,"trigger_if_sensor_value");
		const string trigger_if_sensor_expr = mapfind(request,"trigger_if_sensor_expr");

		const string trigger_if_command = mapfind(request,"trigger_if_command");
		const string trigger_if_command_select_args1 = mapfind(request,"trigger_if_command_select_args1");
		const string trigger_if_command_select_args2 = mapfind(request,"trigger_if_command_select_args2");
		const string trigger_if_command_select_args3 = mapfind(request,"trigger_if_command_select_args3");
		const string trigger_if_command_select_args4 = mapfind(request,"trigger_if_command_select_args4");
		const string trigger_if_command_select_args5 = mapfind(request,"trigger_if_command_select_args5");
		const string trigger_if_command_pooling = mapfind(request,"trigger_if_command_pooling");

		const string trigger_if_web_url = mapfind(request,"trigger_if_web_url");
		const string trigger_if_web_string = mapfind(request,"trigger_if_web_string");
		const string trigger_if_web_pooling = mapfind(request,"trigger_if_web_pooling");

		const string trigger_if_elec_use = mapfind(request,"trigger_if_elec_use");
		const string trigger_if_elec_type = mapfind(request,"trigger_if_elec_type");
		const string trigger_if_elec_action = mapfind(request,"trigger_if_elec_action");
		const string trigger_if_elec_expr = mapfind(request,"trigger_if_elec_expr");

		const string trigger_if_stopper_use = mapfind(request,"trigger_if_stopper_use");
		const string trigger_if_stopper_minute = mapfind(request,"trigger_if_stopper_minute");

		const string trigger_if_sound_use = mapfind(request,"trigger_if_sound_use");
		const string trigger_if_sound_expr = mapfind(request,"trigger_if_sound_expr");
		const string trigger_if_sound_value = mapfind(request,"trigger_if_sound_value");

		const string trigger_if_weather_use = mapfind(request,"trigger_if_weather_use");
		const string trigger_if_weather_address = mapfind(request,"trigger_if_weather_address");
		const string trigger_if_weather_value = mapfind(request,"trigger_if_weather_value");
		const string trigger_if_weather_rain_value = mapfind(request,"trigger_if_weather_rain_value");

		const string trigger_then_tospeak_select = mapfind(request,"trigger_then_tospeak_select");
		const string trigger_then_tospeak_string = mapfind(request,"trigger_then_tospeak_string");
		const string trigger_then_tospeak_mp3 = mapfind(request,"trigger_then_tospeak_mp3");

		const string trigger_then_macro_use = mapfind(request,"trigger_then_macro_use");
		const string trigger_then_macro_1_elec   = mapfind(request,"trigger_then_macro_1_elec");
		const string trigger_then_macro_1_action = mapfind(request,"trigger_then_macro_1_action");
		const string trigger_then_macro_1_after  = mapfind(request,"trigger_then_macro_1_after");
		const string trigger_then_macro_2_elec   = mapfind(request,"trigger_then_macro_2_elec");
		const string trigger_then_macro_2_action = mapfind(request,"trigger_then_macro_2_action");
		const string trigger_then_macro_2_after  = mapfind(request,"trigger_then_macro_2_after");
		const string trigger_then_macro_3_elec   = mapfind(request,"trigger_then_macro_3_elec");
		const string trigger_then_macro_3_action = mapfind(request,"trigger_then_macro_3_action");
		const string trigger_then_macro_3_after  = mapfind(request,"trigger_then_macro_3_after");
		const string trigger_then_macro_4_elec   = mapfind(request,"trigger_then_macro_4_elec");
		const string trigger_then_macro_4_action = mapfind(request,"trigger_then_macro_4_action");
		const string trigger_then_macro_4_after  = mapfind(request,"trigger_then_macro_4_after");
		const string trigger_then_macro_5_elec   = mapfind(request,"trigger_then_macro_5_elec");
		const string trigger_then_macro_5_action = mapfind(request,"trigger_then_macro_5_action");
		const string trigger_then_macro_5_after  = mapfind(request,"trigger_then_macro_5_after");
		const string trigger_then_macro_6_elec   = mapfind(request,"trigger_then_macro_6_elec");
		const string trigger_then_macro_6_action = mapfind(request,"trigger_then_macro_6_action");
		const string trigger_then_macro_6_after  = mapfind(request,"trigger_then_macro_6_after");
		const string trigger_then_macro_7_elec   = mapfind(request,"trigger_then_macro_7_elec");
		const string trigger_then_macro_7_action = mapfind(request,"trigger_then_macro_7_action");
		const string trigger_then_macro_7_after  = mapfind(request,"trigger_then_macro_7_after");
		const string trigger_then_macro_8_elec   = mapfind(request,"trigger_then_macro_8_elec");
		const string trigger_then_macro_8_action = mapfind(request,"trigger_then_macro_8_action");
		const string trigger_then_macro_8_after  = mapfind(request,"trigger_then_macro_8_after");
		const string trigger_then_macro_9_elec   = mapfind(request,"trigger_then_macro_9_elec");
		const string trigger_then_macro_9_action = mapfind(request,"trigger_then_macro_9_action");
		const string trigger_then_macro_9_after  = mapfind(request,"trigger_then_macro_9_after");

		const string trigger_then_command_use = mapfind(request,"trigger_then_command_use");
		const string trigger_then_command = mapfind(request,"trigger_then_command");
		const string trigger_then_command_select_args1 = mapfind(request,"trigger_then_command_select_args1");
		const string trigger_then_command_select_args2 = mapfind(request,"trigger_then_command_select_args2");
		const string trigger_then_command_select_args3 = mapfind(request,"trigger_then_command_select_args3");
		const string trigger_then_command_select_args4 = mapfind(request,"trigger_then_command_select_args4");
		const string trigger_then_command_select_args5 = mapfind(request,"trigger_then_command_select_args5");

		const string trigger_display_name = mapfind(request,"trigger_display_name");
		const string trigger_display_color = mapfind(request,"trigger_display_color");

		int key;
		if (trigger_key == "new")
		{//家電の新しいIDを降り出す
			key = Trigger.NewTriggerID();
		}
		else
		{
			key = atoi(trigger_key.c_str());
		}
		string prefix = "trigger_" + num2str(key);

		if (trigger_if_type == "web")
		{
			if (trigger_if_web_url.empty())
			{
				return ScriptRemoconWeb::ResultError(20080 , "URLを入れてください");
			}
			if (trigger_if_web_string.empty())
			{
				return ScriptRemoconWeb::ResultError(20081 , "検索文字列を入れてください");
			}
		}

		if (stringbool(trigger_then_macro_use))
		{
			//マクロが循環参照になっていないか求めます
			int nestcount = 0;
			//循環参照になって無限ループにならないようにマクロの上限をチェックする
			Fire f;
			if (  ! f.check_macroE(trigger_then_macro_1_elec,trigger_then_macro_1_action , &nestcount) )
			{
				return ScriptRemoconWeb::ResultError(20051 , "マクロ1で呼び出し回数の上限を超えました。マクロの中で無限ループに入っている可能性があります。");
			}
			if (  ! f.check_macroE(trigger_then_macro_2_elec,trigger_then_macro_2_action , &nestcount) )
			{
				return ScriptRemoconWeb::ResultError(20052 , "マクロ2で呼び出し回数の上限を超えました。マクロの中で無限ループに入っている可能性があります。");
			}
			if (  ! f.check_macroE(trigger_then_macro_3_elec,trigger_then_macro_3_action , &nestcount) )
			{
				return ScriptRemoconWeb::ResultError(20053 , "マクロ3で呼び出し回数の上限を超えました。マクロの中で無限ループに入っている可能性があります。");
			}
			if (  ! f.check_macroE(trigger_then_macro_4_elec,trigger_then_macro_4_action , &nestcount) )
			{
				return ScriptRemoconWeb::ResultError(20054 , "マクロ4で呼び出し回数の上限を超えました。マクロの中で無限ループに入っている可能性があります。");
			}
			if (  ! f.check_macroE(trigger_then_macro_5_elec,trigger_then_macro_5_action , &nestcount) )
			{
				return ScriptRemoconWeb::ResultError(20055 , "マクロ5で呼び出し回数の上限を超えました。マクロの中で無限ループに入っている可能性があります。");
			}
			if (  ! f.check_macroE(trigger_then_macro_6_elec,trigger_then_macro_6_action , &nestcount) )
			{
				return ScriptRemoconWeb::ResultError(20056 , "マクロ6で呼び出し回数の上限を超えました。マクロの中で無限ループに入っている可能性があります。");
			}
			if (  ! f.check_macroE(trigger_then_macro_7_elec,trigger_then_macro_7_action , &nestcount) )
			{
				return ScriptRemoconWeb::ResultError(20057 , "マクロ7で呼び出し回数の上限を超えました。マクロの中で無限ループに入っている可能性があります。");
			}
			if (  ! f.check_macroE(trigger_then_macro_8_elec,trigger_then_macro_8_action , &nestcount) )
			{
				return ScriptRemoconWeb::ResultError(20058 , "マクロ8で呼び出し回数の上限を超えました。マクロの中で無限ループに入っている可能性があります。");
			}
			if (  ! f.check_macroE(trigger_then_macro_9_elec,trigger_then_macro_9_action , &nestcount) )
			{
				return ScriptRemoconWeb::ResultError(20059 , "マクロ9で呼び出し回数の上限を超えました。マクロの中で無限ループに入っている可能性があります。");
			}
		}

		//結果を読み上げる場合は、キャッシュを作ります。
		if (trigger_then_tospeak_select == "string")
		{
			//読み上げないモードでキャッシュを作らせます。
			MainWindow::m()->SyncInvoke([&](){
				MainWindow::m()->Speak.SpeakAsync( CallbackPP::NoCallback() , trigger_then_tospeak_string , true );
			});
		}

		MainWindow::m()->Config.Set(prefix + "_if_type",trigger_if_type);
		MainWindow::m()->Config.Set(prefix + "_if_date_hour",trigger_if_date_hour);
		MainWindow::m()->Config.Set(prefix + "_if_date_minute",trigger_if_date_minute);
		MainWindow::m()->Config.Set(prefix + "_if_date_loop_use",trigger_if_date_loop_use);
		MainWindow::m()->Config.Set(prefix + "_if_date_loop_dayofweek",trigger_if_date_loop_dayofweek);
		MainWindow::m()->Config.Set(prefix + "_if_date_loop_holiday",trigger_if_date_loop_holiday);
		MainWindow::m()->Config.Set(prefix + "_if_date_loop_holiday_dataof",trigger_if_date_loop_holiday_dataof);
		MainWindow::m()->Config.Set(prefix + "_if_date_loop_hourly",trigger_if_date_loop_hourly);

		MainWindow::m()->Config.Set(prefix + "_if_sensor_type",trigger_if_sensor_type);
		MainWindow::m()->Config.Set(prefix + "_if_sensor_value",trigger_if_sensor_value);
		MainWindow::m()->Config.Set(prefix + "_if_sensor_expr",trigger_if_sensor_expr);

		MainWindow::m()->Config.Set(prefix + "_if_command",trigger_if_command);
		MainWindow::m()->Config.Set(prefix + "_if_command_select_args1",trigger_if_command_select_args1);
		MainWindow::m()->Config.Set(prefix + "_if_command_select_args2",trigger_if_command_select_args2);
		MainWindow::m()->Config.Set(prefix + "_if_command_select_args3",trigger_if_command_select_args3);
		MainWindow::m()->Config.Set(prefix + "_if_command_select_args4",trigger_if_command_select_args4);
		MainWindow::m()->Config.Set(prefix + "_if_command_select_args5",trigger_if_command_select_args5);
		MainWindow::m()->Config.Set(prefix + "_if_command_pooling",trigger_if_command_pooling);

		MainWindow::m()->Config.Set(prefix + "_if_web_url",trigger_if_web_url);
		MainWindow::m()->Config.Set(prefix + "_if_web_string",trigger_if_web_string);
		MainWindow::m()->Config.Set(prefix + "_if_web_pooling",trigger_if_web_pooling);

		MainWindow::m()->Config.Set(prefix + "_if_elec_use",trigger_if_elec_use);
		MainWindow::m()->Config.Set(prefix + "_if_elec_type",trigger_if_elec_type);
		MainWindow::m()->Config.Set(prefix + "_if_elec_action",trigger_if_elec_action);
		MainWindow::m()->Config.Set(prefix + "_if_elec_expr",trigger_if_elec_expr);

		MainWindow::m()->Config.Set(prefix + "_if_stopper_use",trigger_if_stopper_use);
		MainWindow::m()->Config.Set(prefix + "_if_stopper_minute",trigger_if_stopper_minute);

		MainWindow::m()->Config.Set(prefix + "_if_sound_use",trigger_if_sound_use);
		MainWindow::m()->Config.Set(prefix + "_if_sound_expr",trigger_if_sound_expr);
		MainWindow::m()->Config.Set(prefix + "_if_sound_value",trigger_if_sound_value);

		MainWindow::m()->Config.Set(prefix + "_if_weather_use",trigger_if_weather_use);
		MainWindow::m()->Config.Set(prefix + "_if_weather_address",trigger_if_weather_address);
		MainWindow::m()->Config.Set(prefix + "_if_weather_value",trigger_if_weather_value);
		MainWindow::m()->Config.Set(prefix + "_if_weather_rain_value",trigger_if_weather_rain_value);

		MainWindow::m()->Config.Set(prefix + "_then_tospeak_select",trigger_then_tospeak_select);
		MainWindow::m()->Config.Set(prefix + "_then_tospeak_string",trigger_then_tospeak_string);
		MainWindow::m()->Config.Set(prefix + "_then_tospeak_mp3",trigger_then_tospeak_mp3);

		MainWindow::m()->Config.Set(prefix + "_then_macro_use",trigger_then_macro_use);
		MainWindow::m()->Config.Set(prefix + "_then_macro_1_elec",trigger_then_macro_1_elec);
		MainWindow::m()->Config.Set(prefix + "_then_macro_1_action",trigger_then_macro_1_action);
		MainWindow::m()->Config.Set(prefix + "_then_macro_1_after",trigger_then_macro_1_after);
		MainWindow::m()->Config.Set(prefix + "_then_macro_2_elec",trigger_then_macro_2_elec);
		MainWindow::m()->Config.Set(prefix + "_then_macro_2_action",trigger_then_macro_2_action);
		MainWindow::m()->Config.Set(prefix + "_then_macro_2_after",trigger_then_macro_2_after);
		MainWindow::m()->Config.Set(prefix + "_then_macro_3_elec",trigger_then_macro_3_elec);
		MainWindow::m()->Config.Set(prefix + "_then_macro_3_action",trigger_then_macro_3_action);
		MainWindow::m()->Config.Set(prefix + "_then_macro_3_after",trigger_then_macro_3_after);
		MainWindow::m()->Config.Set(prefix + "_then_macro_4_elec",trigger_then_macro_4_elec);
		MainWindow::m()->Config.Set(prefix + "_then_macro_4_action",trigger_then_macro_4_action);
		MainWindow::m()->Config.Set(prefix + "_then_macro_4_after",trigger_then_macro_4_after);
		MainWindow::m()->Config.Set(prefix + "_then_macro_5_elec",trigger_then_macro_5_elec);
		MainWindow::m()->Config.Set(prefix + "_then_macro_5_action",trigger_then_macro_5_action);
		MainWindow::m()->Config.Set(prefix + "_then_macro_5_after",trigger_then_macro_5_after);
		MainWindow::m()->Config.Set(prefix + "_then_macro_6_elec",trigger_then_macro_6_elec);
		MainWindow::m()->Config.Set(prefix + "_then_macro_6_action",trigger_then_macro_6_action);
		MainWindow::m()->Config.Set(prefix + "_then_macro_6_after",trigger_then_macro_6_after);
		MainWindow::m()->Config.Set(prefix + "_then_macro_7_elec",trigger_then_macro_7_elec);
		MainWindow::m()->Config.Set(prefix + "_then_macro_7_action",trigger_then_macro_7_action);
		MainWindow::m()->Config.Set(prefix + "_then_macro_7_after",trigger_then_macro_7_after);
		MainWindow::m()->Config.Set(prefix + "_then_macro_8_elec",trigger_then_macro_8_elec);
		MainWindow::m()->Config.Set(prefix + "_then_macro_8_action",trigger_then_macro_8_action);
		MainWindow::m()->Config.Set(prefix + "_then_macro_8_after",trigger_then_macro_8_after);
		MainWindow::m()->Config.Set(prefix + "_then_macro_9_elec",trigger_then_macro_9_elec);
		MainWindow::m()->Config.Set(prefix + "_then_macro_9_action",trigger_then_macro_9_action);
		MainWindow::m()->Config.Set(prefix + "_then_macro_9_after",trigger_then_macro_9_after);
		
		MainWindow::m()->Config.Set(prefix + "_then_command_use",trigger_then_command_use);
		MainWindow::m()->Config.Set(prefix + "_then_command",trigger_then_command);
		MainWindow::m()->Config.Set(prefix + "_then_command_select_args1",trigger_then_command_select_args1);
		MainWindow::m()->Config.Set(prefix + "_then_command_select_args2",trigger_then_command_select_args2);
		MainWindow::m()->Config.Set(prefix + "_then_command_select_args3",trigger_then_command_select_args3);
		MainWindow::m()->Config.Set(prefix + "_then_command_select_args4",trigger_then_command_select_args4);
		MainWindow::m()->Config.Set(prefix + "_then_command_select_args5",trigger_then_command_select_args5);

		MainWindow::m()->Config.Set(prefix + "_display_name" ,trigger_display_name);
		MainWindow::m()->Config.Set(prefix + "_display_color",trigger_display_color);

		MainWindow::m()->Config.Set(prefix + "_enable","1");

		//リロードする.
		ReloadTrigger(time(NULL));
	}

	ConfigUpdated(true);
	return this->TriggerStatus();
}

//webapiのキーリセット
string ScriptWebRunner::setting_webapi_resetkey(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();

	const int temp = 0;
	const string apikey = SystemMisc::MakeWebAPIKey();

	{
		MainWindow::m()->Config.Set( "webapi_apikey" , apikey );

		MultiRoomUtil::changeAuthkey(apikey);

		//Sync開始(多少時間がかかるので非同期)
		MainWindow::m()->MultiRoom.startSync();
	}

	ConfigUpdated(false);
	return this->SettingStatus();
}


//homekitの起動と停止
string ScriptWebRunner::setting_homekit_boot(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	const string homekit_boot = mapfind(request,"homekit_boot");
	if( stringbool(homekit_boot) )
	{
		MainWindow::m()->Config.Set("homekit_boot","1");
		MainWindow::m()->HomeKitServer.Stop();
		if( ! MainWindow::m()->HomeKitServer.Create() )
		{
			MainWindow::m()->Config.Set("homekit_boot","0");
			return ScriptRemoconWeb::ResultError(40030,"homekit can not start");
		}
	}
	else
	{
		MainWindow::m()->Config.Set("homekit_boot","0");
		MainWindow::m()->HomeKitServer.Stop();
	}

	return this->SettingStatus();
}


//homekitのキーリセット
string ScriptWebRunner::setting_homekit_resetkey(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();

	MainWindow::m()->HomeKitServer.RegeneratePinCode();
	MainWindow::m()->HomeKitServer.Stop();
	MainWindow::m()->HomeKitServer.Create();

	return this->SettingStatus();
}



string ScriptWebRunner::remocon_fileselectpage_find(const XLHttpHeader& httpHeaders) const
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string typepath = mapfind(request,"typepath");
	const string search = mapfind(request,"search");

	string path = "";
	if (typepath == "tospeak_mp3")
	{
		path = "./webroot/user/tospeak_mp3/";
	}
	else if (typepath == "elecicon")
	{
		path = "./webroot/user/elecicon/";
	}
	else if (typepath == "script_scenario")
	{
		path = "./webroot/user/script_scenario/";
	}
	else if (typepath == "script_command")
	{
		path = "./webroot/user/script_command/";
	}
	else if (typepath == "remocon")
	{
		path = "./webroot/user/remocon/";
	}
	else if (typepath == "actionicon")
	{
		path = "./webroot/user/actionicon/";
	}
	else
	{
		return ScriptRemoconWeb::ResultError(20005 , "typepathが正しくありません");
	}
	string jsonstring;

	int fileid = 1;
	bool ret = XLFileUtil::findfile_orderby( MainWindow::m()->GetConfigBasePath(path) ,XLFileUtil::findfile_orderby_name_ascending, [&](const string& filename,const string& fullfilename) -> bool {
		
		if (filename == "." || filename == "..")
		{
			return true;
		}
		if (!search.empty())
		{
			if ( ! XLStringUtil::stristr(filename,search) )
			{
				return true;
			}
		}
		const string prefix = string("file_") + num2str(fileid) ;

		string icon;
		if (typepath == "tospeak_mp3")
		{
			icon = "/jscss/images/icon_audio.png";
		}
		else if (typepath == "elecicon")
		{
			icon = string("/user/elecicon/") + filename;
		}
		else if (typepath == "script_scenario")
		{
			icon = "/jscss/images/icon_script_scenario.png";
		}
		else if (typepath == "script_command")
		{
			icon = "/jscss/images/icon_script_command.png";
		}
		else if (typepath == "remocon")
		{
			icon = string("/user/remocon/") + filename;
		}
		else if (typepath == "actionicon")
		{
			icon = string("/user/actionicon/") + filename;
		}
		
		jsonstring += string(",") + XLStringUtil::jsonescape(prefix + "_name") + ": " + XLStringUtil::jsonescape(filename) 
			+ string(",") + XLStringUtil::jsonescape(prefix + "_size") + ": " + XLStringUtil::jsonescape(num2str(XLFileUtil::getfilesize(fullfilename) )) 
			+ string(",") + XLStringUtil::jsonescape(prefix + "_time") + ": " + XLStringUtil::jsonescape(num2str(XLFileUtil::getfiletime(fullfilename) )) 
			+ string(",") + XLStringUtil::jsonescape(prefix + "_date") + ": " + XLStringUtil::jsonescape(XLStringUtil::timetostr(XLFileUtil::getfiletime(fullfilename),"%Y/%m/%d %H:%M:%S" )) 
			+ string(",") + XLStringUtil::jsonescape(prefix + "_icon") + ": " + XLStringUtil::jsonescape(icon) 
		;
		fileid++;
		return true;
	});

	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return _A2U(jsonstring.c_str());
}

string ScriptWebRunner::remocon_fileselectpage_upload(const XLHttpHeader& httpHeaders) 
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string typepath = mapfind(request,"typepath");

	string path = "";
	if (typepath == "tospeak_mp3")
	{
		path = MainWindow::m()->GetConfigBasePath("./webroot/user/tospeak_mp3");
	}
	else if (typepath == "elecicon")
	{
		path = MainWindow::m()->GetConfigBasePath("./webroot/user/elecicon");
	}
	else if (typepath == "remocon")
	{
		path = MainWindow::m()->GetConfigBasePath("./webroot/user/remocon");
	}
	else if (typepath == "actionicon")
	{
		path = MainWindow::m()->GetConfigBasePath("./webroot/user/actionicon");
	}
	else
	{
		return ScriptRemoconWeb::ResultError(20005 , "typepathが正しくありません");
	}

	auto files = httpHeaders.getFilesPointer();
	for(auto it = files->begin() ; it != files->end() ; ++ it )
	{

		if (! XLStringUtil::checkSafePath(it->second->filename) )
		{
			return ScriptRemoconWeb::ResultError(20006 , "このファイル名(" + it->second->filename + ")は危険です");
		}

		const string savepath = XLStringUtil::strtolower( XLStringUtil::pathcombine( path , it->second->filename) );
		const string ext = XLStringUtil::baseext_nodot(savepath);
		if (typepath == "tospeak_mp3")
		{
			if ( ! ( ext == "mp3" || ext == "wav" ) )
			{
				return ScriptRemoconWeb::ResultError(20005 , "未サポートのファイル形式です。filename:" + it->second->filename);
			}
		}
		else if (typepath == "elecicon" || typepath == "remocon")
		{
			if ( ! ( ext == "png" || ext == "jpg" ||  ext == "gif" ) )
			{
				return ScriptRemoconWeb::ResultError(20005 , "未サポートのファイル形式です。filename:" + it->second->filename);
			}
		}
		
		XLFileUtil::write(savepath , it->second->data);
	}

	ConfigUpdated(true);
	return remocon_fileselectpage_find(httpHeaders);
}


string ScriptWebRunner::remocon_fileselectpage_delete(const XLHttpHeader& httpHeaders) 
{
	
	const map<string,string> request = httpHeaders.getPost();
	const string typepath = mapfind(request,"typepath");
	const string filename = mapfind(request,"filename");

	if (! XLStringUtil::checkSafePath(filename) )
	{
		return ScriptRemoconWeb::ResultError(20005 , "filenameが安全ではありません");
	}

	string path = "";
	if (typepath == "tospeak_mp3")
	{
		path = MainWindow::m()->GetConfigBasePath("./webroot/user/tospeak_mp3");
	}
	else if (typepath == "elecicon")
	{
		path = MainWindow::m()->GetConfigBasePath("./webroot/user/elecicon");
	}
	else if (typepath == "script_scenario")
	{
		path = MainWindow::m()->GetConfigBasePath("./webroot/user/script_scenario");
	}
	else if (typepath == "script_command")
	{
		path = MainWindow::m()->GetConfigBasePath("./webroot/user/script_command");
	}
	else if (typepath == "remocon")
	{
		path = MainWindow::m()->GetConfigBasePath("./webroot/user/remocon");
	}
	else if (typepath == "actionicon")
	{
		path = MainWindow::m()->GetConfigBasePath("./webroot/user/actionicon");
	}
	else
	{
		return ScriptRemoconWeb::ResultError(20005 , "typepathが正しくありません");
	}
	
	const string fullpath = XLStringUtil::pathcombine( path ,filename);
	if ( XLFileUtil::Exist(fullpath) )
	{
		XLFileUtil::del(fullpath);
	}

	string jsonstring;
	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";

	ConfigUpdated(true);
	return _A2U(jsonstring.c_str());
}

string ScriptWebRunner::script_scripteditor_load(const XLHttpHeader& httpHeaders) const
{
	
	const map<string,string> request = httpHeaders.getPost();
	const string typepath = mapfind(request,"typepath");
	const string filename = mapfind(request,"filename");

	if (! XLStringUtil::checkSafePath(filename) )
	{
		return ScriptRemoconWeb::ResultError(20005 , "filenameが安全ではありません");
	}

	string path;
	if (typepath == "script_scenario")
	{
		path = MainWindow::m()->GetConfigBasePath("./webroot/user/script_scenario/" + filename);
	}
	else if (typepath == "script_command")
	{
		path = MainWindow::m()->GetConfigBasePath("./webroot/user/script_command/" + filename);
	}
	else
	{
		return ScriptRemoconWeb::ResultError(20005 , "typepathが正しくありません");
	}

	if (!XLFileUtil::Exist(path))
	{//ファイルがない場合は、雛形を返す
		if (typepath == "script_scenario")
		{
			path = MainWindow::m()->GetConfigBasePath("./help/script_example/script_scenario.js");
		}
		else if (typepath == "script_command")
		{
			path = MainWindow::m()->GetConfigBasePath("./help/script_example/script_command.js");
		}
	}

	return XLFileUtil::cat(path);
}



//会話シナリオの実行
string ScriptWebRunner::script_scripteditor_save(const XLHttpHeader& httpHeaders)
{
	
	const map<string,string> request = httpHeaders.getPost();
	const string typepath = mapfind(request,"typepath");
	const string filename = mapfind(request,"filename");
	const string filecontexnt = _A2U(mapfind(request,"filecontexnt"));
	string stdout_;

	int errortype = 0;
	string errorMessage;
	string jsonstring;

	if (! XLStringUtil::checkSafePath(filename) )
	{
		return ScriptRemoconWeb::ResultError(39010 , "ファイル名(" + filename + ")はセキュアではありません。");
	}

	if (typepath == "script_scenario")
	{
		const string path = MainWindow::m()->GetConfigBasePath("./webroot/user/script_scenario/" + filename);

		//jsの再ロード
		MainWindow::m()->SyncInvoke([&](){
			try
			{
				//内容の書き込み
				if ( ! XLFileUtil::write(path,filecontexnt) )
				{
					errortype = 1;
					return ;
				}
				string r = MainWindow::m()->ScriptManager.ReloadScript(path);
				stdout_ += r;

				DEBUGLOG("音声認識エンジンコミット開始" );
				MainWindow::m()->Recognition.CommitRule();
				DEBUGLOG("音声認識エンジンコミット終了" );
			}
			catch(XLException& e)
			{
				errortype = 2;
				errorMessage = e.what();
				ERRORLOG("webエラー " << errorMessage);
			}
		});
	}
	else if (typepath == "script_command")
	{
		//アクションはチェックできないので保存するだけ
		const string path = MainWindow::m()->GetConfigBasePath("./webroot/user/script_command/" + filename);
		//内容の書き込み
		if ( ! XLFileUtil::write(path,filecontexnt) )
		{
			errortype = 1;
		}
	}
	else
	{
  		return ScriptRemoconWeb::ResultError(39001 , "typepathが正しくありません");
	}

	if (errortype == 1)
	{
		return ScriptRemoconWeb::ResultError(39010 , "ファイルに書き込み出来ません");
	}
	else if (errortype == 2)
	{
		ERRORLOG("webエラー " << errorMessage);
		return ScriptRemoconWeb::ResultError(39020 , errorMessage);
	}

	DEBUGLOG("stdout: " << stdout_ );
	jsonstring += string(",") + XLStringUtil::jsonescape("stdout") + ": " 
		+ XLStringUtil::jsonescape(stdout_) ;

	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";

	return _A2U(jsonstring.c_str());
}


string ScriptWebRunner::version_import(const XLHttpHeader& httpHeaders) const
{
	
	const map<string,string> request = httpHeaders.getRequest();

	auto files = httpHeaders.getFilesPointer();
	if(files->empty())
	{
		return ScriptRemoconWeb::ResultError(20008 , "ファイルが送信されてきていません。");
	}

	auto it = files->begin();
	
	const string savepath = XLFileUtil::getTempDirectory("naichichi2") + "/import" + ".tar.gz";
	XLFileUtil::write(savepath , it->second->data);
	try
	{
		MainWindow::m()->SyncFiles();
		SystemMisc::Import(savepath);

		return this->SettingStatus();
	}
	catch(XLException &e)
	{
		return ScriptRemoconWeb::ResultError(20008 , std::string("ファイルが破損しています") + e.what() );
	}
}

string ScriptWebRunner::version_export(const XLHttpHeader& httpHeaders) 
{
	MainWindow::m()->SyncFiles();
	return SystemMisc::Export();
}



//認証しているかどうかのチェック
bool ScriptWebRunner::checkAuth(map<string,string>* cookieArray) const
{
	const string account_usermail = MainWindow::m()->Config.Get("account_usermail","");
	const string account_password = MainWindow::m()->Config.Get("account_password","");
	if (account_usermail.empty() || account_password.empty())
	{//そもそも登録されていないっぽいので、エラーにする.
		return false;
	}

	if (account_usermail != mapfind(cookieArray,"account_usermail") )
	{
		return false;
	}
	if (account_password != mapfind(cookieArray,"account_password") )
	{
		return false;
	}
	
	return true;
}

//認証
string ScriptWebRunner::AuthIndex(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result,map<string,string>* cookieArray)
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const auto headers = httpHeaders.getHeaderPointer();
	const string id = mapfind(request,"id");
	const string password = mapfind(request,"password");
	const string backurl = mapfind(request,"backurl");
	const string uuidoverraide = mapfind(request,"uuidoverraide");

	string html = MainWindow::m()->TranslateFile.TransAuth( ScriptRemoconWeb::OpenTPL(httpHeaders,"auth.tpl") );
	html = XLStringUtil::replace_low(html,"%ID%", XLStringUtil::htmlspecialchars_low(_A2U(id) ) );
//	html = XLStringUtil::replace_low(html,"%PASSWORD%",  XLStringUtil::htmlspecialchars_low(_A2U(password) ) ); //エラーの時だけ差し込む
	html = XLStringUtil::replace_low(html,"%BACKURL%", XLStringUtil::htmlspecialchars_low(_A2U(backurl) ) );

	if ( !id.empty() )
	{
		html = XLStringUtil::replace(html,"%PASSWORD%",  XLStringUtil::htmlspecialchars_low(_A2U(password) ) );
		html = XLStringUtil::replace_low(html,"%FIRST_CALL%", "0" );

		//パスワードは sha1して保持しているので、比較するときは sha1 で比較するよ
		const string password_sha1 =  SystemMisc::MakePassword( id , password);
	
		//現在設定されている内容と比較
		const string account_usermail = MainWindow::m()->Config.Get("account_usermail","");
		const string account_password = MainWindow::m()->Config.Get("account_password","");
		bool is_logined = false;
		if (account_usermail.empty() || account_password.empty())
		{//そもそも登録されていないっす
			NOTIFYLOG("このID(" << account_usermail << ")はそもそも登録されていません");
			is_logined = false;
		}
		if (account_usermail == id && account_password == password_sha1 )
		{//OK
			NOTIFYLOG("このID(" << account_usermail << ")の認証OKです");
			is_logined = true;
		}
	
		const string uuid = MainWindow::m()->Httpd.GetUUID();
		const string serverresult = MainWindow::m()->ColudSyncPooling.OpeartionCheckRegist(id,password_sha1,uuid,uuidoverraide);

		bool isPay = true;
		
		enum SERVER_SAY{ SERVER_SAY_OK,SERVER_SAY_NG,SERVER_SAY_NULL };
		SERVER_SAY server_say;
		if ( serverresult.find("NG") == 0 || serverresult.find("BAD") == 0)
		{//サーバーもダメといったのでダメ
			if ( serverresult == "BAD\tEXCEPTION")
			{//サーバーがダウンしている
				NOTIFYLOG("サーバー接続がエラーになりました。認証サーバがダウンしているか、ネットにつながっていません" << serverresult);
				server_say = SERVER_SAY_NULL;
			}
			else
			{
				NOTIFYLOG("サーバー接続がエラーを返しました。" << serverresult);
				server_say = SERVER_SAY_NG;
			}
		}
		else if (serverresult.find("OK") == 0)
		{//サーバーがOKを返した
			NOTIFYLOG("サーバーがOKを返しました" << serverresult);
			server_say = SERVER_SAY_OK;
			isPay = (serverresult.find("\tpayed") == string::npos);
		}
		else 
		{//サーバーが意味不明な応答を返した
			NOTIFYLOG("サーバーが意味不明な応答を返しました" << serverresult);
			server_say = SERVER_SAY_NULL;
		}

		if (server_say == SERVER_SAY_NG)
		{//サーバーもダメといったのでダメ
			if ( serverresult.find("uuid already use") != string::npos )
			{
				NOTIFYLOG("IDは別のUUIDに関連付けられています." << serverresult);

				html = XLStringUtil::replace_low(html,"%LOGIN_ERROR_DISPLAY%", "none" );
				html = XLStringUtil::replace_low(html,"%UUID_ERROR_DISPLAY%", "1" );
				*result = WEBSERVER_RESULT_TYPE_OK_HTML;
				return html;
			}
			else
			{
				NOTIFYLOG("IDとパスワードが正しくない." << serverresult);

				html = XLStringUtil::replace_low(html,"%LOGIN_ERROR_DISPLAY%", "block" );
				html = XLStringUtil::replace_low(html,"%UUID_ERROR_DISPLAY%", "0" );
				*result = WEBSERVER_RESULT_TYPE_OK_HTML;
				return html;
			}
		}
		else if (server_say == SERVER_SAY_OK)
		{//サーバーがいいと言ったのでOK
			NOTIFYLOG("このID(" << account_usermail << ")は、サーバがOKといったので利用できます。");
		}
		else
		{
			NOTIFYLOG("サーバーが不明な応答を返しました。応答(" << serverresult << ")");

			//サーバーが落ちてる？
			if (!is_logined)
			{//ローカルの情報でログインできない
				//ダメ
				NOTIFYLOG("ローカルでもIDとパスワードが正しくない.");

				html = XLStringUtil::replace_low(html,"%LOGIN_ERROR_DISPLAY%", "block" );
				html = XLStringUtil::replace_low(html,"%UUID_ERROR_DISPLAY%", "0" );
				*result = WEBSERVER_RESULT_TYPE_OK_HTML;
				return html;
			}
			else
			{
				NOTIFYLOG("サーバーが落ちているのでローカルの情報でログインさせます.");
			}
		}
		
		//この情報でローカルの設定ファイルを上書きする
		MainWindow::m()->Config.Set("account_usermail",id);
		MainWindow::m()->Config.Set("account_password",password_sha1);
		//SSHのパスワードの変更
		SystemMisc::SetSSHUserPassword(password);
		//マルチルームへも通知
		MultiRoomUtil::changeAccount(id);
		//更新通知
		IDPasswordUpdated();
		ConfigUpdated(true);

		//OKログインできる。

		//cookieにログインの情報を書き込みます。
		(*cookieArray)["account_usermail"] = id;
		(*cookieArray)["account_password"] = password_sha1;
//		(*cookieArray)["is_pay"] = (isPay ? "1" : "0");
		(*cookieArray)["is_pay"] = "1";
		(*cookieArray)["__regen__"] = "1";

		*result = WEBSERVER_RESULT_TYPE_LOCATION;
		//明示的な戻り先がある場合以外は、リモコン操作に戻す
		if (backurl.empty())
		{
			return "/remocon/";
		}
		
		return backurl;
	}

	//ログイン画面を出す	
	html = XLStringUtil::replace_low(html,"%PASSWORD%",  "" );
	html = XLStringUtil::replace_low(html,"%LOGIN_ERROR_DISPLAY%", "none" );
	html = XLStringUtil::replace_low(html,"%UUID_ERROR_DISPLAY%", "0" );
	html = XLStringUtil::replace_low(html,"%FIRST_CALL%", "1" );
	*result = WEBSERVER_RESULT_TYPE_OK_HTML;
	return html;
}

//リモコン
string ScriptWebRunner::RemoconIndex(const XLHttpHeader& httpHeaders,const map<string,string>& cookieArray) 
{
	MainWindow::m()->Sensor.getSensorNow();

	const auto headers = httpHeaders.getHeaderPointer();
	const auto configmap = MainWindow::m()->Config.ToMap();
	const bool ispay = stringbool( mapfind(cookieArray,"is_pay","1") );
	const time_t now = time(NULL);

	const string htmlsrc = MainWindow::m()->TranslateFile.TransRemocon( ScriptRemoconWeb::OpenTPL(httpHeaders,"remocon.tpl"));
	return ScriptRemoconWeb::DrawHTML(configmap,htmlsrc, now, true , true  ,MultiRoomUtil::isMultiRoomEnable(configmap) , !ispay );
}

//認証されているか？
string ScriptWebRunner::auth_is_auth(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	

	string jsonstring;
	if (checkAuth(cookieArray))
	{
		jsonstring = ",\"auth\": \"ok\"";
	}
	else
	{
		jsonstring = ",\"auth\": \"bad\"";
	}
	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";

	return _A2U(jsonstring.c_str());
}


//ID Passwordに変更があった時
void ScriptWebRunner::IDPasswordUpdated()
{
	const string& id          = MainWindow::m()->Config.Get("account_usermail","");
	const string& password    = MainWindow::m()->Config.Get("account_password","");
	const string& uuid        = MainWindow::m()->Httpd.GetUUID();
	const int sync_server = MainWindow::m()->Config.GetInt("account_sync_server",1);
	MainWindow::m()->ColudSyncPooling.WakeupByNewSyncID(id,password,uuid,(sync_server != 0)  );
}


//Configを変更した場合
void ScriptWebRunner::ConfigUpdated(bool fullsync)
{
	//configの同期を行う
	if (fullsync)
	{
		MainWindow::m()->ColudSyncPooling.ConfigUpdatedFull();
	}
	else
	{
		MainWindow::m()->ColudSyncPooling.ConfigUpdated();
	}
}

//トリガーリロード
void ScriptWebRunner::ReloadTrigger(const time_t& now)
{
	return this->Trigger.ReloadTrigger(now);
}


bool ScriptWebRunner::checkPasswordAuth(const string& id,const string& password)
{
	
	//パスワードは sha1して保持しているので、比較するときは sha1 で比較するよ
	const string password_sha1 = SystemMisc::MakePassword( id , password);

	//現在設定されている内容と比較
	const string account_usermail = MainWindow::m()->Config.Get("account_usermail","");
	const string account_password = MainWindow::m()->Config.Get("account_password","");
	bool is_logined = false;
	if (account_usermail.empty() || account_password.empty())
	{//そもそも登録されていないっす
		is_logined = false;
	}
	if (account_usermail == id && account_password == password_sha1 )
	{//OK
		is_logined = true;
	}
	return is_logined;
}

string ScriptWebRunner::api_upnp_xml(const XLHttpHeader& httpHeaders) 
{
	
	auto upnpxml = ScriptRemoconWeb::OpenTPL(httpHeaders,"upnp_xml.tpl");
	upnpxml = XLStringUtil::replace_low(upnpxml,"%MODEL%", _A2U(SystemMisc::GetModel()) );
	upnpxml = XLStringUtil::replace_low(upnpxml,"%VERSION%", _A2U( num2str(MainWindow::m()->Config.GetInt("version_number",100) / 100.f) ) );
	upnpxml = XLStringUtil::replace_low(upnpxml,"%UUID%", _A2U(MainWindow::m()->Httpd.GetUUID()) );
	upnpxml = XLStringUtil::replace_low(upnpxml,"%WEBURL%", _A2U(MainWindow::m()->Httpd.getWebURL("/remocon/")) );
	
	return upnpxml;
}


string ScriptWebRunner::api_elec_action(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) 
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string apikey = mapfind(request,"webapi_apikey");
	*result = ScriptRemoconWeb::SelectJsonPResult(request);

	if (MainWindow::m()->Config.Get("webapi_apikey","") != apikey )
	{
		return ScriptRemoconWeb::ResultError(40002 , "apikeyが正しくない");
	}

	const string elec = mapfind(request,"elec");
	const string action = mapfind(request,"action");

	{
		AutoLEDOn autoLEDOn;
		Fire f;
		if (! f.FireByType(elec,action) )
		{
			return ScriptRemoconWeb::ResultError(20004 , "ERR: "+f.getError());
		}
	}

	string jsonstring;
	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return _A2U(jsonstring);
}

string ScriptWebRunner::api_elec_actionstr(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) 
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string apikey = mapfind(request,"webapi_apikey");
	*result = ScriptRemoconWeb::SelectJsonPResult(request);

	if (MainWindow::m()->Config.Get("webapi_apikey","") != apikey )
	{
		return ScriptRemoconWeb::ResultError(40002 , "apikeyが正しくない");
	}

	AutoLEDOn autoLEDOn;
	Fire f;
	const string actionstr = mapfind(request,"actionstr");
	if (! f.FireByString(actionstr) )
	{
		return ScriptRemoconWeb::ResultError(40002 , "登録された家電操作はありません" + f.getError());
	}

	string jsonstring;
	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return _A2U(jsonstring);
}


//すごく詳しいけどわかりづらいリストを返す
string ScriptWebRunner::api_elec_detaillist(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) 
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string apikey = mapfind(request,"webapi_apikey");
	*result = ScriptRemoconWeb::SelectJsonPResult(request);
	
	if (MainWindow::m()->Config.Get("webapi_apikey","") != apikey )
	{
		return ScriptRemoconWeb::ResultError(40002 , "apikeyが正しくない");
	}

	return RemoconStatus("") ;
}

//家電一覧だけ取得する
string ScriptWebRunner::api_elec_getlist(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) 
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string apikey = mapfind(request,"webapi_apikey");
	*result = ScriptRemoconWeb::SelectJsonPResult(request);
	
	if (MainWindow::m()->Config.Get("webapi_apikey","") != apikey )
	{
		return ScriptRemoconWeb::ResultError(40002 , "apikeyが正しくない");
	}

	//隠しアイコンも出す？
	bool showall = false;
	if ( stringbool(mapfind(request,"showall")) )
	{
		showall = true;
	}

	//ソート済みのIDを取得
	const auto configmap = MainWindow::m()->Config.FindGetsToMap("elec_",false);
	const vector<unsigned int> arr = ScriptRemoconWeb::getElecID_Array(configmap,showall);

	string jsonstring;
	for(auto it = arr.begin() ; it != arr.end() ; ++it )
	{
		const auto typeIT = configmap.find("elec_" + num2str(*it) + "_type");
		if (typeIT == configmap.end())
		{
			continue;
		}
		const auto showremoconIT = configmap.find("elec_" + num2str(*it) + "_showremocon");
		if (showremoconIT == configmap.end())
		{
			continue;
		}
		if (!showall)
		{
			if (! stringbool(showremoconIT->second) )
			{//非表示は表示しない時は、非表示家電だったら表示しない
				continue;
			}
		}
		if (typeIT->second == "SPACE")
		{//余白
			continue;
		}

		jsonstring += "," + XLStringUtil::jsonescape(typeIT->second) ;
	}
	if (! jsonstring.empty() )
	{
		jsonstring = jsonstring.substr(1);
	}
	return _A2U("{\"result\": \"ok\" , \"list\": [" + jsonstring + "]}");
}


//機材のアクション一覧を取得する
string ScriptWebRunner::api_elec_getactionlist(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) 
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string apikey = mapfind(request,"webapi_apikey");
	*result = ScriptRemoconWeb::SelectJsonPResult(request);
	
	if (MainWindow::m()->Config.Get("webapi_apikey","") != apikey )
	{
		return ScriptRemoconWeb::ResultError(40002 , "apikeyが正しくない");
	}

	const string p1 = mapfind(request,"elec");
	//隠しアイコンも出す？
	bool showall = false;
	if ( stringbool(mapfind(request,"showall")) )
	{
		showall = true;
	}

	//ソート済みのIDを取得
	const auto configmap = MainWindow::m()->Config.FindGetsToMap("elec_",false);
	unsigned int elecID = 0;
	const vector<unsigned int> arr = ScriptRemoconWeb::getElecActionID_Array(configmap,p1,&elecID,showall);

	string jsonstring;
	for(auto it = arr.begin() ; it != arr.end() ; ++it )
	{
		const auto typeIT = configmap.find("elec_" + num2str(elecID)  + "_action_" + num2str(*it) + "_actiontype");
		if (typeIT == configmap.end())
		{
			continue;
		}
		const auto showremoconIT = configmap.find("elec_" + num2str(elecID)  + "_action_" + num2str(*it) + "_showremocon");
		if (showremoconIT == configmap.end())
		{
			continue;
		}
		if (!showall)
		{
			if (! stringbool(showremoconIT->second) )
			{//非表示は表示しない時は、非表示家電だったら表示しない
				continue;
			}
		}
		if (typeIT->second == "SPACE")
		{//余白
			continue;
		}

		jsonstring += "," + XLStringUtil::jsonescape(typeIT->second) ;
	}
	if (! jsonstring.empty() )
	{
		jsonstring = jsonstring.substr(1);
	}
	return _A2U("{\"result\": \"ok\" , \"list\": [" + jsonstring + "]}");
}

//家電の情報を取得する
string ScriptWebRunner::api_elec_getinfo(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) 
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string apikey = mapfind(request,"webapi_apikey");
	*result = ScriptRemoconWeb::SelectJsonPResult(request);
	
	if (MainWindow::m()->Config.Get("webapi_apikey","") != apikey )
	{
		return ScriptRemoconWeb::ResultError(40002 , "apikeyが正しくない");
	}

	const string p1 = mapfind(request,"elec");
	const auto configmap = MainWindow::m()->Config.FindGetsToMap("elec_",false);
	int key1 = 0;
	if ( ! ScriptRemoconWeb::type2key(configmap ,p1 , &key1) )
	{
		return ScriptRemoconWeb::ResultError(40003 , "対応する機材が存在しない");
	}

	string jsonstring;
	const string prefix = "elec_" + num2str(key1) + "_";
	const auto elecInfomap = MainWindow::m()->Config.FindGetsToMap(prefix ,false);
	for(auto it = elecInfomap.begin() ; it != elecInfomap.end() ; ++it )
	{
		if ( strstr( it->first.c_str() , "_action_" ) )
		{
			continue;
		}
		
		jsonstring += "," + XLStringUtil::jsonescape(it->first.c_str() + prefix.size() ) + ": " + XLStringUtil::jsonescape(it->second ) ;
	}
	return _A2U("{\"result\": \"ok\"" + jsonstring + "}");
}

string ScriptWebRunner::api_sensor_get(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) 
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string apikey = mapfind(request,"webapi_apikey");
	*result = ScriptRemoconWeb::SelectJsonPResult(request);
	
	if (MainWindow::m()->Config.Get("webapi_apikey","") != apikey )
	{
		return ScriptRemoconWeb::ResultError(40002 , "apikeyが正しくない");
	}

	return GetSensorJson();
}

string ScriptWebRunner::api_speak(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) 
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string apikey = mapfind(request,"webapi_apikey");
	*result = ScriptRemoconWeb::SelectJsonPResult(request);
	
	if (MainWindow::m()->Config.Get("webapi_apikey","") != apikey )
	{
		return ScriptRemoconWeb::ResultError(40002 , "apikeyが正しくない");
	}
	const string str = mapfind(request,"str");
	const bool noplay = stringbool(mapfind(request,"noplay","0"));
	const bool async = stringbool(mapfind(request,"async","0"));

	MainWindow::m()->SyncInvoke( [&](){
		if (async)
		{//非同期
			MainWindow::m()->Speak.SpeakAsync( CallbackPP::NoCallback(),str , noplay );
		}
		else
		{//同期
			MainWindow::m()->Speak.SpeakSync( str , noplay );
		}
	});
	return _A2U("{\"result\": \"ok\" }");
}

//環境情報の取得
string ScriptWebRunner::api_env(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) 
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string apikey = mapfind(request,"webapi_apikey");
	*result = ScriptRemoconWeb::SelectJsonPResult(request);

	if (MainWindow::m()->Config.Get("webapi_apikey","") != apikey )
	{
		return ScriptRemoconWeb::ResultError(40002 , "apikeyが正しくない");
	}

	const string version = num2str(MainWindow::m()->Config.GetInt("version_number",100) / 100.f);
	const string model = SystemMisc::GetModel();
#if _MSC_VER
	const string os = "windows";
#else
	const string os = "linux";
#endif

	return _A2U(string("{\"result\": \"ok\"")
		+ ", \"version\": " + XLStringUtil::jsonescape(version) 
		+ ", \"model\": " + XLStringUtil::jsonescape(model) 
		+ ", \"os\": " + XLStringUtil::jsonescape(os) 
		+ "}");
}

string ScriptWebRunner::api_play(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) 
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string apikey = mapfind(request,"webapi_apikey");
	*result = ScriptRemoconWeb::SelectJsonPResult(request);

	if (MainWindow::m()->Config.Get("webapi_apikey","") != apikey )
	{
		return ScriptRemoconWeb::ResultError(40002 , "apikeyが正しくない");
	}
	const string name = mapfind(request,"name");
	const bool noplay = stringbool(mapfind(request,"noplay","0"));
	const bool async = stringbool(mapfind(request,"async","0"));

	if (! XLStringUtil::checkSafePath(name) )
	{
		return ScriptRemoconWeb::ResultError(40003 , "指定されたファイル(" + name + ")に危険な文字列があるので再生出来ません");
	}

	string filepath = MainWindow::m()->GetConfigBasePath("./webroot/user/tospeak_mp3/" + name) ;
	if (! XLFileUtil::Exist(filepath) )
	{
		return ScriptRemoconWeb::ResultError(40004 , "指定されたファイル(" + name + ")が存在しません");
	}

	if (async)
	{//非同期
		MainWindow::m()->MusicPlayAsync.Play(name,1);
	}
	else
	{//同期
		MainWindow::m()->MusicPlayAsync.PlaySync(name);
	}
	return _A2U("{\"result\": \"ok\" }");
}


string ScriptWebRunner::api_recong_list(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) 
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string apikey = mapfind(request,"webapi_apikey");
	*result = ScriptRemoconWeb::SelectJsonPResult(request);

	if (MainWindow::m()->Config.Get("webapi_apikey","") != apikey )
	{
		return ScriptRemoconWeb::ResultError(40002 , "apikeyが正しくない");
	}
	//メインスレッドでしか触れない領域なので、コピーする。 そのコストは支払う
	list<RecongTask> recongtask;
	MainWindow::m()->SyncInvoke( [&](){
		recongtask = *MainWindow::m()->Recognition.getAllCommandRecongTask();
	});

	string jsonstring;
	for(auto it = recongtask.begin() ; it != recongtask.end() ; ++it )
	{
		jsonstring += "," + XLStringUtil::jsonescape(it->orignalString) ;
	}
	if (! jsonstring.empty() )
	{
		jsonstring = jsonstring.substr(1);
	}
	return _A2U("{\"result\": \"ok\" , \"list\": [" + jsonstring + "]}");
}

string ScriptWebRunner::api_recong_firebystring(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) 
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string apikey = mapfind(request,"webapi_apikey");
	*result = ScriptRemoconWeb::SelectJsonPResult(request);

	if (MainWindow::m()->Config.Get("webapi_apikey","") != apikey )
	{
		return ScriptRemoconWeb::ResultError(40002 , "apikeyが正しくない");
	}
	const string str = mapfind(request,"str");
	Fire f;
	if (! f.FireByString(str) )
	{
		return ScriptRemoconWeb::ResultError(40003 , "指定されたコマンド(" + str + ")はありません");
	}

	return _A2U("{\"result\": \"ok\"}");
}

//実行!!
string ScriptWebRunner::remocon_fire_bytype(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	//type1=hogehoge&type2=hogehoge
	auto type1 = mapfind(request,"type1");
	if (type1 == "")
	{
		return ScriptRemoconWeb::ResultError(10001 , "type1が見つかりません");
	}

	auto type2 = mapfind(request,"type2");
	if (type2 == "")
	{
		return ScriptRemoconWeb::ResultError(10002 , "type2が見つかりません");
	}


	{
		AutoLEDOn autoLEDOn;
		Fire f;
		if (! f.FireByType(type1,type2) )
		{
			return ScriptRemoconWeb::ResultError(10003 ,"ERR: "+f.getError());
		}
	}

	return "{\"result\": \"ok\"}";
}

//コマンドの実行
string ScriptWebRunner::script_scripteditor_command_run(const XLHttpHeader& httpHeaders)
{
	
	const map<string,string> request = httpHeaders.getPost();
	const string execcommand = mapfind(request,"filename");
	const string execargs1 = mapfind(request,"args1");
	const string execargs2 = mapfind(request,"args2");
	const string execargs3 = mapfind(request,"args3");
	const string execargs4 = mapfind(request,"args4");
	const string execargs5 = mapfind(request,"args5");

	string jsonstring;
	string stdout_;
	string returnValue;
	try
	{
		AutoLEDOn autoLEDOn;
		Fire f;
		returnValue = f.run_commandE(execcommand,execargs1,execargs2,execargs3,execargs4,execargs5,&stdout_) ;
	}
	catch(XLException &e)
	{
		return ScriptRemoconWeb::ResultError(38020 , e.what() );
	}

	jsonstring += string(",") + XLStringUtil::jsonescape("stdout") + ": " 
		+ XLStringUtil::jsonescape(stdout_) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("returnValue") + ": " 
		+ XLStringUtil::jsonescape( returnValue ) ;
	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";

	return _A2U(jsonstring.c_str());
}
string ScriptWebRunner::api_multiroom_elec_action(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) 
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string apikey = mapfind(request,"webapi_apikey");
	*result = ScriptRemoconWeb::SelectJsonPResult(request);

	if (MainWindow::m()->Config.Get("webapi_apikey","") != apikey )
	{
		return ScriptRemoconWeb::ResultError(40002 , "apikeyが正しくない");
	}

	const string uuid = mapfind(request,"uuid");
	const string elec = mapfind(request,"elec");
	const string action = mapfind(request,"action");

	string your_result;
	try
	{
		Fire fire;
		fire.run_multiroom(uuid,elec,action,&your_result);
	}
	catch(XLException &e)
	{
		return ScriptRemoconWeb::ResultError(40004 , std::string("命令送信エラー ") + e.what() + " STDOUT:" + your_result);
	}
	return your_result;
}


string ScriptWebRunner::api_multiroom_elec_getlist(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) 
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string apikey = mapfind(request,"webapi_apikey");
	*result = ScriptRemoconWeb::SelectJsonPResult(request);

	if (MainWindow::m()->Config.Get("webapi_apikey","") != apikey )
	{
		return ScriptRemoconWeb::ResultError(40002 , "apikeyが正しくない");
	}

	const string uuid = mapfind(request,"uuid");
	const auto configmap = MainWindow::m()->Config.FindGetsToMap("multiroom_",false);
	const int key = MultiRoomUtil::FindRoom(uuid,configmap);
	if(key < 0 )
	{
		return ScriptRemoconWeb::ResultError(40003 , "UUIDが正しくありません");
	}
	const string your_authkey = MainWindow::m()->Config.Get("multiroom_"+num2str(key)+"_authkey", "" );
	const string your_url = MainWindow::m()->Config.Get("multiroom_"+num2str(key)+"_url", "" );

	const string get = 
	"?webapi_apikey=" + XLStringUtil::urlencode(your_authkey)
	;

	const string url = XLStringUtil::urlcombine(your_url , "/api/elec/getlist");
	string your_result;
	try
	{
		map<string,string> header;
		your_result = ActionImplement::HttpGet(url + get,header,3);
	}
	catch(XLException &e)
	{
		return ScriptRemoconWeb::ResultError(40004 , std::string("命令送信エラー ")+ e.what());
	}
	return your_result;
}


string ScriptWebRunner::api_multiroom_elec_getactionlist(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) 
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string apikey = mapfind(request,"webapi_apikey");
	*result = ScriptRemoconWeb::SelectJsonPResult(request);

	if (MainWindow::m()->Config.Get("webapi_apikey","") != apikey )
	{
		return ScriptRemoconWeb::ResultError(40002 , "apikeyが正しくない");
	}

	const string uuid = mapfind(request,"uuid");
	const string elec = mapfind(request,"elec");


	const auto configmap = MainWindow::m()->Config.FindGetsToMap("multiroom_",false);
	const int key = MultiRoomUtil::FindRoom(uuid,configmap);
	if(key < 0 )
	{
		return ScriptRemoconWeb::ResultError(40003 , "UUIDが正しくありません");
	}
	const string your_authkey = MainWindow::m()->Config.Get("multiroom_"+num2str(key)+"_authkey", "" );
	const string your_url = MainWindow::m()->Config.Get("multiroom_"+num2str(key)+"_url", "" );

	const string get = 
	"?webapi_apikey=" + XLStringUtil::urlencode(your_authkey)
	+ "&elec=" + XLStringUtil::urlencode(_A2U(elec))
	;

	const string url = XLStringUtil::urlcombine(your_url , "/api/elec/getactionlist");
	string your_result;
	try
	{
		map<string,string> header;
		your_result = ActionImplement::HttpGet(url + get,header,3);
	}
	catch(XLException &e)
	{
		return ScriptRemoconWeb::ResultError(40004 , std::string("命令送信エラー ")+ e.what());
	}
	return your_result;
}
string ScriptWebRunner::remocon_netdevice_fire(const XLHttpHeader& httpHeaders)
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string elec = mapfind(request,"elec");
	const string action = mapfind(request,"action");
	const string value = mapfind(request,"value");

	string jsonstring;
	Fire fire;
	bool r = fire.run_netdev(elec,action,value);
	if(!r)
	{
		return ScriptRemoconWeb::ResultError(20005 , "実行に失敗しました");
	}
	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return _A2U(jsonstring.c_str());
}

string ScriptWebRunner::remocon_sip_fire(const XLHttpHeader& httpHeaders)
{
	
	const map<string,string> request = httpHeaders.getRequest();

	const string sip_action_type = mapfind(request,"sip_action_type");
	const string sip_call_number = mapfind(request,"sip_call_number");

	string jsonstring;
	Fire fire;
	bool r = fire.run_sip(sip_action_type,sip_call_number);
	if(!r)
	{
		return ScriptRemoconWeb::ResultError(20005 , "実行に失敗しました");
	}
	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return _A2U(jsonstring.c_str());
}


string ScriptWebRunner::remocon_netdevice_elec_getlist(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getRequest();

	unsigned int searchtime = atoi(mapfind(request,"searchtime","0"));

	string jsonstring;
	NetDevice netdev;
	list<string> netdevlist = netdev.GetAll(searchtime);
	for(auto it = netdevlist.begin() ; it != netdevlist.end() ; ++it )
	{
		jsonstring += "," + XLStringUtil::jsonescape(*it) ;
	}
	if (! jsonstring.empty() )
	{
		jsonstring = jsonstring.substr(1);
	}
	NOTIFYLOG("ネット家電一覧を返します.");
	return _A2U("{\"result\": \"ok\" , \"list\": [" + jsonstring + "]}");
}

string ScriptWebRunner::remocon_netdevice_action_getlist(const XLHttpHeader& httpHeaders) 
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string elec = mapfind(request,"elec");

	string jsonstring;
	NetDevice netdev;
	list<string> netdevlist = netdev.GetSetActionAll(elec);
	for(auto it = netdevlist.begin() ; it != netdevlist.end() ; ++it )
	{
		jsonstring += "," + XLStringUtil::jsonescape(*it) ;
	}
	if (! jsonstring.empty() )
	{
		jsonstring = jsonstring.substr(1);
	}
	return _A2U("{\"result\": \"ok\" , \"list\": [" + jsonstring + "]}");
}
string ScriptWebRunner::remocon_netdevice_value_getlist(const XLHttpHeader& httpHeaders) 
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string elec = mapfind(request,"elec");
	const string action = mapfind(request,"action");

	string jsonstring;
	NetDevice netdev;
	list<string> netdevlist = netdev.GetSetValueAll(elec,action);
	for(auto it = netdevlist.begin() ; it != netdevlist.end() ; ++it )
	{
		jsonstring += "," + XLStringUtil::jsonescape(*it) ;
	}
	if (! jsonstring.empty() )
	{
		jsonstring = jsonstring.substr(1);
	}
	return _A2U("{\"result\": \"ok\" , \"list\": [" + jsonstring + "]}");
}


//スクリプト内のコメントに書いてあるアノテーションから引数情報を取得する.
string ScriptWebRunner::script_scripteditor_annotation(const XLHttpHeader& httpHeaders) const
{
	
	const map<string,string> request = httpHeaders.getPost();
	const string typepath = mapfind(request,"typepath");
	const string filename = mapfind(request,"filename");

	if (! XLStringUtil::checkSafePath(filename) )
	{
		return "";
	}

	string path;
	if (typepath == "script_scenario")
	{
		path = MainWindow::m()->GetConfigBasePath("./webroot/user/script_scenario/" + filename);
	}
	else if (typepath == "script_command")
	{
		path = MainWindow::m()->GetConfigBasePath("./webroot/user/script_command/" + filename);
	}
	else
	{
		return ScriptRemoconWeb::ResultError(20005 , "typepathが正しくありません");
	}

	
	string jsonstring;

	//ファイルの中身
#if _MSC_VER
	auto lines = XLStringUtil::split("\n",_U2A(XLFileUtil::cat(path)));
#else
	auto lines = XLStringUtil::split("\n",XLFileUtil::cat(path));
#endif
	for(auto it = lines.begin() ; it != lines.end() ; ++it )
	{
		const char * p;
		p = strstr(it->c_str() , "///@args"); 
		if (!p)
		{
			p = strstr(it->c_str() , "---@args"); 
			if (!p)
			{
				continue;
			}
		}
		auto parts = XLStringUtil::split_vector("@",p+sizeof("///@args")-1);
		if ( parts.size() <= 3 )
		{
			continue;
		}
		string argsname = "args_" + num2str( atoi( parts[0].c_str() ) );
		string mustFlag = parts[1];
		string name;
		if (parts.size() >= 3 ) name = parts[2];
		string def;
		if (parts.size() >= 4 ) def = parts[3];
		string type;
		if (parts.size() >= 5 ) type = parts[4];

		jsonstring  += ",\"" + argsname + "_must\": " + XLStringUtil::jsonescape(mustFlag) 
                    +  ",\"" + argsname + "_name\": " + XLStringUtil::jsonescape(name) 
                    +  ",\"" + argsname + "_def\":  " + XLStringUtil::jsonescape(def) 
                    +  ",\"" + argsname + "_type\": " + XLStringUtil::jsonescape(type) 
					;
	}
	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";

	return _A2U(jsonstring.c_str());
}

//トリガーを消す
string ScriptWebRunner::setting_trigger_delete(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	DEBUGLOG("request: " << XLStringUtil::crossjoin("=","&",request) );
	{
		//key=ID
		auto keyIT = request.find("key");
		if (keyIT == request.end() )
		{
			return ScriptRemoconWeb::ResultError(10700,"keyがありません。");
		}
		int key = atoi(keyIT->second.c_str());
		if (key <= 0)
		{
			return ScriptRemoconWeb::ResultError(10701,"keyが数字ではありません。");
		}

		const string prefix = string("trigger_") + num2str(key) + "_";
		const auto configmap = MainWindow::m()->Config.FindGetsToMap(prefix,false);
		for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
		{
			MainWindow::m()->Config.Remove( it->first.c_str() );
		}

	}
	ConfigUpdated(true);
	ReloadTrigger(time(NULL));
	return this->TriggerStatus();
}

//トリガーのweb検索のプレビュー
string ScriptWebRunner::trigger_preview_if_web(const XLHttpHeader& httpHeaders) 
{
	

	const map<string,string> request = httpHeaders.getPost();
	string jsonstring;
	{
		//type1=hogehoge&type2=hogehoge
		auto url = mapfind(request,"url");
		auto str = mapfind(request,"string");
		if (url == "")
		{
			return ScriptRemoconWeb::ResultError(10200 , "urlが見つかりません");
		}
		if (str == "")
		{
			return ScriptRemoconWeb::ResultError(10201 , "stringが見つかりません");
		}

		string srchtml;
		string error;
		try
		{
			srchtml = XLHttpSocket::Get(url , 2 );
		}
		catch(XLException& e)
		{
			error += e.what();
		}
		const string encoding = XLStringUtil::mb_detect_encoding(srchtml);
		const string html = XLStringUtil::mb_convert_utf8(srchtml,encoding);

		bool regex = false;
		bool match = false;
		if (str[0] == '/' && str[str.size() - 1 ] == '/' )
		{
			regex = true;
			try
			{
				string regexString = str.substr(1,str.size() - 2);
				match =  XLStringUtil::regex_checkE( html, 	_A2U(regexString) );
			}
			catch(XLException& e)
			{
				error += e.what();
			}
		}
		else
		{
			match = XLStringUtil::stristr( _U2A(html) , str ) != NULL;
		}
		jsonstring += string(",") + XLStringUtil::jsonescape("match") + ": " 
			+ XLStringUtil::jsonescape( match ? "1" : "0" ) ;
		jsonstring += string(",") + XLStringUtil::jsonescape("regex") + ": " 
			+ XLStringUtil::jsonescape( regex ? "1" : "0" ) ;
		jsonstring += string(",") + XLStringUtil::jsonescape("error") + ": " 
			+ XLStringUtil::jsonescape( error ) ;
		jsonstring += string(",") + XLStringUtil::jsonescape("encoding") + ": " 
			+ XLStringUtil::jsonescape( encoding ) ;
		jsonstring += string(",") + XLStringUtil::jsonescape("html") + ": " 
			+ XLStringUtil::jsonescape( _U2A(html) ) ;
	}
	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return _A2U(jsonstring.c_str());
}

//トリガーのコマンドの実行
string ScriptWebRunner::trigger_preview_if_command(const XLHttpHeader& httpHeaders)
{
	
	const map<string,string> request = httpHeaders.getPost();
	const string execcommand = mapfind(request,"filename");
	const string execargs1 = mapfind(request,"args1");
	const string execargs2 = mapfind(request,"args2");
	const string execargs3 = mapfind(request,"args3");
	const string execargs4 = mapfind(request,"args4");
	const string execargs5 = mapfind(request,"args5");

	string jsonstring;
	string returnValue;
	string stdout_;
	try
	{
		Fire f;
		returnValue = f.run_commandE(execcommand,execargs1,execargs2,execargs3,execargs4,execargs5,&stdout_) ;
	}
	catch(XLException &e)
	{
		return ScriptRemoconWeb::ResultError(38020 , e.what() );
	}

	bool match  = (stringbool(returnValue) || atoi( returnValue ) );
	jsonstring += string(",") + XLStringUtil::jsonescape("match") + ": " 
		+ XLStringUtil::jsonescape( match ? "1" : "0" ) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("returnValue") + ": " 
		+ XLStringUtil::jsonescape( returnValue ) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("stdout") + ": " 
		+ XLStringUtil::jsonescape( stdout_ ) ;

	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";

	return _A2U(jsonstring.c_str());
}

//天気のトリガ条件のテスト
string ScriptWebRunner::trigger_preview_if_weather(const XLHttpHeader& httpHeaders)
{
	
	const map<string,string> request = httpHeaders.getPost();
	const string trigger_if_weather_address = mapfind(request,"address");
	const string trigger_if_weather_value = mapfind(request,"value");
	const string trigger_if_weather_rain_value = mapfind(request,"rain_value");

	const string weather_script_command = MainWindow::m()->Config.Get("weather_script_command","weather_openweather.js");

	string jsonstring;
	string returnValue;
	string stdout_;
	map<string,string> retmap;
	try
	{
		returnValue = Trigger.run_command_resultjson(weather_script_command,trigger_if_weather_address,"","","","",&stdout_,&retmap);
	}
	catch(XLException &e)
	{
		return ScriptRemoconWeb::ResultError(38030 , e.what() );
	}

	const string weather = retmap["weather"];
	bool match  = false;

	if (trigger_if_weather_value == "rain")
	{
		if (weather == "rain" || weather == "snow")
		{
			match  = true;
		}
	}
	else if (trigger_if_weather_value == "not_rain")
	{
		if (! (weather == "rain" || weather == "snow") )
		{
			match  = true;
		}
	}
	else if (trigger_if_weather_value == "soon_rain" || trigger_if_weather_value == "soon_clear")
	{
		const string next = retmap["next"];
		if (trigger_if_weather_value == "soon_rain")
		{
			if ( Trigger.trigger_if_weather_IsRain(weather) || Trigger.trigger_if_weather_IsRain(next) )
			{
				match  = true;
			}
		}
		else if (trigger_if_weather_value == "soon_clear")
		{
			if (! (Trigger.trigger_if_weather_IsRain(weather) || Trigger.trigger_if_weather_IsRain(next) ) )
			{
				match  = true;
			}
		}
	}

	jsonstring += string(",") + XLStringUtil::jsonescape("match") + ": " 
		+ XLStringUtil::jsonescape( match ? "1" : "0" ) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("returnValue") + ": " 
		+ XLStringUtil::jsonescape( returnValue ) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("stdout") + ": " 
		+ XLStringUtil::jsonescape( stdout_ ) ;

	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";

	return _A2U(jsonstring.c_str());
}


//部屋の並び順の更新
string ScriptWebRunner::setting_multiroom_order(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	DEBUGLOG("request: " << XLStringUtil::crossjoin("=","&",request) );
	{
		//multiroom_ID_order=hogehoge&multiroom_ID_order=hogehoge&multiroom_ID_order=hogehoge&multiroom_ID_order=hogehoge&multiroom_ID_order=hogehoge&multiroom_ID_order=hogehoge&
		for(auto it = request.begin() ; it != request.end() ; ++it )
		{
			int key = -1;
			sscanf(it->first.c_str(),"multiroom_%d_order" , &key );
			if (key == -1)
			{
				continue;
			}
			if ( atoi( it->second.c_str() ) <= 0 )
			{
				continue;
			}

			MainWindow::m()->Config.Set( it->first.c_str() , it->second.c_str() );
		}
		//Sync開始(多少時間がかかるので非同期)
		MainWindow::m()->MultiRoom.startSync();
	}
	ConfigUpdated(true);
	return MultiRoomStatus();
}

//検索
string ScriptWebRunner::setting_multiroom_search(const XLHttpHeader& httpHeaders) 
{
	

	map<string,string> locationMap;
	const string my_servertop = MainWindow::m()->Httpd.getServerTop();

	//検索リクエスト送信
	//3秒待機
	MainWindow::m()->UPNPServer.upnpMSearch(3);

	//発見した端末一覧の取得.
	list<UPNPMap> servers;
	MainWindow::m()->UPNPServer.GetAll(&servers);
	for(auto it = servers.begin() ; it != servers.end() ; it++ )
	{
		//server ヘッダに naichichi2 と入っていたら俺です。
		if ( it->server.find("naichichi2") == string::npos )
		{
			DEBUGLOG("SERVERアトリビュートが naichichi2 ではありません 値:" << it->server);
			continue;
		}

		const string location = it->location;
		if(location.empty())
		{
			DEBUGLOG("UPNP応答のlocationが空です 値:" << location);
			continue;
		}

		if ( location.find(my_servertop) == 0 )
		{//自分自身
			continue;
		}

		locationMap[location] = location;
	}

	const auto configmap = MainWindow::m()->Config.FindGetsToMap("multiroom_",false);
	string jsonstring;
	int i = 1;
	for(auto it = locationMap.begin(); it != locationMap.end() ; it++, i++ )
	{
		const string xml = XLHttpSocket::Get( it->second ,2);
		const string my_uuid = XLStringUtil::cut(xml,"<UDN>","</UDN>");
		const string my_url = XLStringUtil::replace( XLStringUtil::cut(xml,"<presentationURL>","</presentationURL>"),"/remocon/","/");
		const int key = MultiRoomUtil::FindRoom(my_uuid,configmap);

		jsonstring += string(",") + XLStringUtil::jsonescape("multiroom_"+num2str(i) + "_url") + ": " 
			+ XLStringUtil::jsonescape(my_url) ;
		jsonstring += string(",") + XLStringUtil::jsonescape("multiroom_"+num2str(i) + "_uuid") + ": " 
			+ XLStringUtil::jsonescape(my_uuid) ;
		jsonstring += string(",") + XLStringUtil::jsonescape("multiroom_"+num2str(i) + "_key") + ": " 
			+ XLStringUtil::jsonescape(num2str(key)) ;
	}

	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return _A2U(jsonstring);
}


//削除
string ScriptWebRunner::setting_multiroom_delete(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	{
		const int key = atoi(mapfind(request,"key"));
		if(key <= 0)
		{
			return ScriptRemoconWeb::ResultError(40021 , "can not delete , key is zero!");
		}

		const string prefix = "multiroom_" + num2str(key) ;
		const string your_uuid = MainWindow::m()->Config.Get(prefix+"_uuid","");
		const string your_url = MainWindow::m()->Config.Get(prefix+"_url","");
		const string your_authkey = MainWindow::m()->Config.Get(prefix+"_authkey","");
		if(your_uuid == MainWindow::m()->Httpd.GetUUID())
		{//自分自身は消せません
			return ScriptRemoconWeb::ResultError(40021 , "can not delete , it is myself!");
		}
		MainWindow::m()->Config.RemoveTree(prefix + "_");

		string post = 
		  "our_uuid=" + XLStringUtil::urlencode(MainWindow::m()->Httpd.GetUUID())
		+ "&your_authkey=" + XLStringUtil::urlencode(your_authkey)
		;

		//相手ホストへ削除を打診.
		const string url = XLStringUtil::urlcombine(your_url , "/multiroom/registdel");
		string your_result;
		try
		{
			map<string,string> header;
			XLHttpHeader retheader;
			your_result = XLHttpSocket::Post(url, header ,3, &retheader,post);
		}
		catch(XLException& e)
		{
			ERRORLOG("can not connect " << url << "  message:" << e.what());
			//ただし削除は続行する.
			//相手のホストが取り除かれ方ら削除するかもしれないので.
		}

		//Sync開始(多少時間がかかるので非同期)
		MainWindow::m()->MultiRoom.startSync();
	}
	ConfigUpdated(true);
	return MultiRoomStatus();
}

//追加処理
string ScriptWebRunner::setting_multiroom_append(const XLHttpHeader& httpHeaders) 
{
	
	const map<string,string> request = httpHeaders.getPost();
	{
		const string your_user_usermail = mapfind(request,"multiroomappend_usermail");
		const string your_user_password = mapfind(request,"multiroomappend_password");
		const string your_url = mapfind(request,"multiroomappend_url");

		string post = 
		"our_url=" + XLStringUtil::urlencode( MainWindow::m()->Httpd.getServerTop())
		+ "&our_uuid=" + XLStringUtil::urlencode(MainWindow::m()->Httpd.GetUUID())
		+ "&our_authkey=" + XLStringUtil::urlencode(MainWindow::m()->Config.Get("webapi_apikey",""))
		+ "&our_name=" + XLStringUtil::urlencode(_A2U(MainWindow::m()->Config.Get("system_room_string","")))
		+ "&our_account=" + XLStringUtil::urlencode(_A2U(MainWindow::m()->Config.Get("account_usermail","")))
		+ "&your_user_usermail=" + XLStringUtil::urlencode(your_user_usermail)
		+ "&your_user_password=" + XLStringUtil::urlencode(your_user_password)
		;

		//相手ホストへ追加を打診する.
		const string url = XLStringUtil::urlcombine(your_url , "/multiroom/registadd");
		string your_result;
		try
		{
			map<string,string> header;
			XLHttpHeader retheader;
			your_result = XLHttpSocket::Post(url, header ,3, &retheader,post);
			your_result = _U2A(your_result);
		}
		catch(XLException& e)
		{
			return ScriptRemoconWeb::ResultError(40000 , "can not connect " +  url + "  message:" + e.what() );
		}

		//結果をパース
		map<string,string> json = XLStringUtil::parsejson(your_result);
		if( mapfind(json,"result") != "ok")
		{//相手から拒絶された.
			return ScriptRemoconWeb::ResultError(40001 , "can not append " + mapfind(json,"error")  );
		}

		//相手が知っている部屋情報をマージ
		MultiRoomUtil::Merge(json);

		//Sync開始(多少時間がかかるので非同期)
		MainWindow::m()->MultiRoom.startSync();
	}
	ConfigUpdated(true);
	return MultiRoomStatus();
}

//ほかの端末からマルチルームの追加時に呼ばれます. 機械が勝手にコール.
string ScriptWebRunner::multiroom_registadd(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	{
		const string our_url = mapfind(request,"our_url");
		const string our_uuid = mapfind(request,"our_uuid");
		const string our_authkey = mapfind(request,"our_authkey");
		const string our_name = mapfind(request,"our_name");
		const string our_account = mapfind(request,"our_account");
		const string your_user_usermail = mapfind(request,"your_user_usermail");
		const string your_user_password = mapfind(request,"your_user_password");
		string your_order;

		if (! SystemMisc::checkUUID(our_uuid))
		{
			return ScriptRemoconWeb::ResultError(40001 , "bad your uuid");
		}
		const string myself_uuid = MainWindow::m()->Httpd.GetUUID();
		if (! SystemMisc::checkUUID(myself_uuid))
		{
			return ScriptRemoconWeb::ResultError(40002 , "bad myself uuid");
		}

		//自分自身をさしていてはだめです.
		const string myself_url = MainWindow::m()->Httpd.getServerTop();
		if (myself_url == our_url)
		{
			return ScriptRemoconWeb::ResultError(40003 , "bad. you select myself");
		}
		if (myself_uuid == our_uuid)
		{
			return ScriptRemoconWeb::ResultError(40003 , "bad. you select myself");
		}

		//認証する.
		if( ! ScriptRemoconWeb::LocalAuth(your_user_usermail,your_user_password))
		{
			return ScriptRemoconWeb::ResultError(40003 , "bad auth");
		}

		const auto configmap = MainWindow::m()->Config.FindGetsToMap("multiroom_",false);
		int key = MultiRoomUtil::FindRoom(our_uuid,configmap);
		if(key <= 0)
		{
			//新しく追加する.
			key = MultiRoomUtil::NewMultiRoomID(configmap);
			your_order = num2str(MultiRoomUtil::NewMultiRoomOrder(configmap));
		}

		string prefix = "multiroom_" + num2str(key);
		MainWindow::m()->Config.Set(prefix + "_url",our_url);
		MainWindow::m()->Config.Set(prefix + "_name",our_name);
		MainWindow::m()->Config.Set(prefix + "_account",our_account);
		MainWindow::m()->Config.Set(prefix + "_uuid",our_uuid);
		MainWindow::m()->Config.Set(prefix + "_authkey",our_authkey);
		if(!your_order.empty())
		{
			MainWindow::m()->Config.Set(prefix + "_order",your_order);
		}
	}
	ConfigUpdated(true);

	{
		const auto configmap = MainWindow::m()->Config.FindGetsToMap("multiroom_",false);
		const string my_uuid = MainWindow::m()->Httpd.GetUUID();

		string jsonstring 
			= MultiRoomUtil::DrawJsonStatus(my_uuid,configmap,true);

		jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
		return jsonstring;
	}
}

//ほかの端末からマルチルームの削除時に呼ばれます. 機械が勝手にコール.
string ScriptWebRunner::multiroom_registdel(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	{
		const string our_uuid = mapfind(request,"our_uuid");
		const string your_authkey = mapfind(request,"your_authkey");

		const auto configmap = MainWindow::m()->Config.FindGetsToMap("multiroom_",false);
		int key = MultiRoomUtil::FindRoom(our_uuid,configmap);
		if (key <= 0)
		{
			return ScriptRemoconWeb::ResultError(40001 , "bad your uuid (who are you)");
		}

		const string webapi_apikey = MainWindow::m()->Config.Get("webapi_apikey","");
		if (your_authkey != webapi_apikey)
		{
			return ScriptRemoconWeb::ResultError(40002 , "bad authkey");
		}

		const string myself_uuid = MainWindow::m()->Httpd.GetUUID();
		if (our_uuid == myself_uuid)
		{
			return ScriptRemoconWeb::ResultError(40003 , "bad your uuid (who are you. why my uuid.)");
		}

		const string prefix = "multiroom_" + num2str(key);
		MainWindow::m()->Config.RemoveTree(prefix + "_");
	}
	ConfigUpdated(true);

	{
		string jsonstring ;
		jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
		return jsonstring;
	}
}

//sync POSTされたMultiRoomデータで上書きます. 機械が勝手にコール.
string ScriptWebRunner::multiroom_sync(const XLHttpHeader& httpHeaders) 
{
	const map<string,string> request = httpHeaders.getPost();
	{
		const string your_authkey = mapfind(request,"your_authkey");
		const string our_uuid = mapfind(request,"our_uuid");
		const string webapi_apikey = MainWindow::m()->Config.Get("webapi_apikey","");

		if(your_authkey.empty())
		{
			return ScriptRemoconWeb::ResultError(40011 , "bad authkey (empty)");
		}
		//自分のauthkeyの確認
		if(your_authkey != webapi_apikey)
		{
			return ScriptRemoconWeb::ResultError(40012 , "bad authkey (not match)");
		}
		//命令を出した人を知っていますか？
		const auto configmap = MainWindow::m()->Config.FindGetsToMap("multiroom_",false);
		int key = MultiRoomUtil::FindRoom(our_uuid,configmap);
		if (key <= 0)
		{
			return ScriptRemoconWeb::ResultError(40013 , "bad uuid (who are you)");
		}
		
		//自分のマルチルームの設定が全部消して、もらったものに置き換える.
		const string prefix = "multiroom_";
		MainWindow::m()->Config.RemoveTree(prefix);
		for(auto postit = request.begin(); postit != request.end() ; postit++ )
		{
			if ( postit->first.find(prefix) == 0 )
			{
				MainWindow::m()->Config.Set(postit->first,postit->second);
			}
		}
	}
	ConfigUpdated(true);
	return MultiRoomStatus();
}

//リモコン画面からの部屋変更.
string ScriptWebRunner::remocon_multiroom_change(const XLHttpHeader& httpHeaders) 
{
	
	const map<string,string> request = httpHeaders.getPost();

	const string my_uuid = MainWindow::m()->Httpd.GetUUID();
	if (! SystemMisc::checkUUID(my_uuid))
	{
		return ScriptRemoconWeb::ResultError(40002 , "bad myself uuid");
	}

	int key;
	const auto configmap = MainWindow::m()->Config.FindGetsToMap("multiroom_",false);
	const auto movetype = mapfind(request,"movetype");
	if(movetype=="n")
	{//next
		key = MultiRoomUtil::getNextRoom(my_uuid,configmap,false);
	}
	else 
	{//back
		key = MultiRoomUtil::getNextRoom(my_uuid,configmap,true);
	}
	
	if (key <= 0)
	{
		return ScriptRemoconWeb::ResultError(10001 , "no room to switch");
	}
	const string webapi_apikey = MainWindow::m()->Config.Get("webapi_apikey","");

	const string prefix = "multiroom_"+num2str(key);
	const string your_authkey = MainWindow::m()->Config.Get(prefix+"_authkey","");
	const string your_uuid = MainWindow::m()->Config.Get(prefix+"_uuid","");
	const string your_url = MainWindow::m()->Config.Get(prefix+"_url","");

	const string post = 
		"our_authkey="+ XLStringUtil::urlencode(webapi_apikey)
		+"&our_uuid="+ XLStringUtil::urlencode(my_uuid)
		+"&your_authkey="+ XLStringUtil::urlencode(your_authkey)
		+"&your_uuid="+ XLStringUtil::urlencode(your_uuid)
	;

	//相手ホストへ認証を打診する.
	const string url = XLStringUtil::urlcombine(your_url , "/auth/multiroom/challenge/generation");
	string your_result;
	try
	{
		map<string,string> header;
		XLHttpHeader retheader;
		your_result = XLHttpSocket::Post(url, header ,3, &retheader,post);
		your_result = _U2A(your_result);
	}
	catch(XLException& e)
	{
		return ScriptRemoconWeb::ResultError(40000 , "can not connect " +  url + "  message:" + e.what() );
	}

	const string authcode = XLStringUtil::findjsonvalue(your_result,"authcode");
	if (authcode.empty())
	{
		return ScriptRemoconWeb::ResultError(40011 , "bad authkey your_result:" + your_result);
	}

	const string location = 
	XLStringUtil::urlcombine(your_url , string("/auth/multiroom/challenge/try")
	+"?authcode=" + XLStringUtil::urlencode(authcode)
	+"&our_uuid=" + XLStringUtil::urlencode(my_uuid)
	);

	string jsonstring;
	jsonstring += string(",") + XLStringUtil::jsonescape("location") + ": " 
		+'"'+ location + '"';

	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return _A2U(jsonstring.c_str());
}

//ワンタイムな認証コードの生成
string ScriptWebRunner::auth_multiroom_challenge_generation(const XLHttpHeader& httpHeaders) 
{
	
	const map<string,string> request = httpHeaders.getPost();

	const string our_authkey = mapfind(request,"our_authkey");
	const string our_uuid = mapfind(request,"our_uuid");
	const string your_authkey = mapfind(request,"your_authkey");
	const string your_uuid = mapfind(request,"your_uuid");

	const auto configmap = MainWindow::m()->Config.FindGetsToMap("multiroom_",false);
	const int key = MultiRoomUtil::FindRoom(our_uuid,configmap);
	if (key <= 0)
	{
		return ScriptRemoconWeb::ResultError(10001 , "There is no room with the specified UUID(" + our_uuid + ").  Who are you?");
	}

	const string webapi_apikey = MainWindow::m()->Config.Get("webapi_apikey","");
	if(your_authkey != webapi_apikey)
	{
		return ScriptRemoconWeb::ResultError(10002 , "your authkey or uuid mismatch");
	}
	const string my_uuid = MainWindow::m()->Httpd.GetUUID();
	if(your_uuid != my_uuid)
	{
		return ScriptRemoconWeb::ResultError(10002 , "your authkey or uuid mismatch");
	}

	const string authcode = MultiRoomUtil::makeOnetimeAuthcode(time(NULL),webapi_apikey,our_uuid);

	string jsonstring;
	jsonstring += string(",") + XLStringUtil::jsonescape("authcode") + ": " 
		+ XLStringUtil::jsonescape(authcode) ;
	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return _A2U(jsonstring);
}



//ワンタイムな認証コードを入れて認証する.
string ScriptWebRunner::auth_multiroom_challenge_try(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray,WEBSERVER_RESULT_TYPE* result) 
{
	
	const map<string,string> request = httpHeaders.getGet();

	const string our_uuid = mapfind(request,"our_uuid");
	const string authcode = mapfind(request,"authcode");

	const auto configmap = MainWindow::m()->Config.FindGetsToMap("multiroom_",false);
	const int key = MultiRoomUtil::FindRoom(our_uuid,configmap);
	if (key <= 0)
	{
		*result = WEBSERVER_RESULT_TYPE_FORBIDDEN;
		return ScriptRemoconWeb::ResultError(10001 , "There is no room with the specified UUID(" + our_uuid + ").  Who are you?");
	}
	const string webapi_apikey = MainWindow::m()->Config.Get("webapi_apikey","");

	time_t now = time(NULL);
	const string check_authcode = MultiRoomUtil::makeOnetimeAuthcode(now,webapi_apikey,our_uuid);
	if(authcode != check_authcode)
	{//念のためちょい前の時間で再チャレンジ
		const string check_authcode_1 = MultiRoomUtil::makeOnetimeAuthcode(now-10,webapi_apikey,our_uuid);
		if(authcode != check_authcode_1)
		{
			*result = WEBSERVER_RESULT_TYPE_FORBIDDEN;
			return ScriptRemoconWeb::ResultError(10002 , "authcode missmatch!");
		}
	}

	//OKログインできる。
	//cookieにログインの情報を書き込みます。
	(*cookieArray)["account_usermail"] = MainWindow::m()->Config.Get("account_usermail","");
	(*cookieArray)["account_password"] = MainWindow::m()->Config.Get("account_password","");
	(*cookieArray)["__regen__"] = "1";

	const string myself_url = MainWindow::m()->Httpd.getServerTop();

	*result = WEBSERVER_RESULT_TYPE_LOCATION;
	return "/remocon/";
}






//webからアクセスがあったときに呼ばれます。
bool ScriptWebRunner::WebAccess(const string& path,const XLHttpHeader& httpHeaders,map<string,string>* cookieArray,WEBSERVER_RESULT_TYPE* result,string* responsString)
{
	ASSERT___IS_WORKER_THREAD_RUNNING(); //メインスレッド以外で動きます。

	if ( path == "/auth/")
	{
		//これに refererチェックしてはいけない.
		*responsString = AuthIndex(httpHeaders,result,cookieArray);
		return true;
	}
	else if ( path == "/auth/is_auth_json" )
	{//現在認証されているか？ jsonで返す
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = auth_is_auth(httpHeaders,cookieArray);
		return true;
	}
	else if ( path == "/auth/is_auth" )
	{//現在認証されているか？ 帰りはjsonpで
		*result = WEBSERVER_RESULT_TYPE_OK_JSONP;
		*responsString = auth_is_auth(httpHeaders,cookieArray);
		return true;
	}
	else if ( path == "/auth/multiroom/challenge/generation" )
	{//ワンタイムな認証コードの生成
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = auth_multiroom_challenge_generation(httpHeaders);
		return true;
	}
	else if ( path == "/auth/multiroom/challenge/try" )
	{//ワンタイムな認証コードを入れて認証する.
		*responsString = auth_multiroom_challenge_try(httpHeaders,cookieArray,result);
		return true;
	}
	else if ( path == "/api/device.xml" )
	{
		*result = WEBSERVER_RESULT_TYPE_OK_DATA;
		*responsString = api_upnp_xml(httpHeaders);
		return true;
	}
	else if ( path == "/api/elec/action" )
	{
		*responsString = api_elec_action(httpHeaders,result);
		return true;
	}
	else if ( path == "/api/multiroom/elec/action" )
	{
		*responsString = api_multiroom_elec_action(httpHeaders,result);
		return true;
	}
	else if ( path == "/api/elec/actionstr" )
	{
		*responsString = api_elec_actionstr(httpHeaders,result);
		return true;
	}
	else if ( path == "/api/elec/detaillist" )
	{
		*responsString = api_elec_detaillist(httpHeaders,result);
		return true;
	}
	else if ( path == "/api/elec/getlist" )
	{
		*responsString = api_elec_getlist(httpHeaders,result);
		return true;
	}
	else if ( path == "/api/elec/getactionlist" )
	{
		*responsString = api_elec_getactionlist(httpHeaders,result);
		return true;
	}
	else if ( path == "/api/multiroom/elec/getlist" )
	{
		*responsString = api_multiroom_elec_getlist(httpHeaders,result);
		return true;
	}
	else if ( path == "/api/multiroom/elec/getactionlist" )
	{
		*responsString = api_multiroom_elec_getactionlist(httpHeaders,result);
		return true;
	}
	else if ( path == "/api/elec/getinfo" )
	{
		*responsString = api_elec_getinfo(httpHeaders,result);
		return true;
	}
	else if ( path == "/api/sensor/get" )
	{
		*responsString = api_sensor_get(httpHeaders,result);
		return true;
	}
	else if ( path == "/api/speak" )
	{
		*responsString = api_speak(httpHeaders,result);
		return true;
	}
	else if ( path == "/api/env" )
	{
		*responsString = api_env(httpHeaders,result);
		return true;
	}
	else if ( path == "/api/recong/list" || path == "/api/recog/list")
	{//recong は typo で recog が正しいです。
		*responsString = api_recong_list(httpHeaders,result);
		return true;
	}
	else if ( path == "/api/recong/firebystring" || path == "/api/recog/firebystring" )
	{//recong は typo で recog が正しいです。
		*responsString = api_recong_firebystring(httpHeaders,result);
		return true;
	}
	else if ( path == "/api/play" )
	{
		*responsString = api_play(httpHeaders,result);
		return true;
	}
	else if ( path == "/multiroom/registadd" )
	{
		*responsString = multiroom_registadd(httpHeaders);
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		return true;
	}
	else if ( path == "/multiroom/registdel" )
	{
		*responsString = multiroom_registdel(httpHeaders);
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		return true;
	}
	else if ( path == "/multiroom/sync" )
	{
		*responsString = multiroom_sync(httpHeaders);
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		return true;
	}

	//認証を確認する.
	if (! checkAuth(cookieArray) )
	{//認証されていない場合、認証させる.
		*result = WEBSERVER_RESULT_TYPE_LOCATION;
		*responsString = "/auth/?gotourl=" + path;
		return true;
	}
	//これより下認証済みのエリア

	if (path == "/remocon/") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = RemoconIndex(httpHeaders,*cookieArray);

		return true;
	}
	else if (path == "/") 
	{
		*result = WEBSERVER_RESULT_TYPE_LOCATION;
		*responsString = "/remocon/";
		return true;
	}
	else if (path == "/edit/")
	{
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = EditIndex(httpHeaders);
		return true;
	}
	else if (path == "/remocon/get/status") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = RemoconStatus("");
		return true;
	}
	else if ( path == "/remocon/get/append_info" )
	{//リモコン画面の副次的情報を取得
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_get_append_info(httpHeaders);
		return true;
	}
	else if (path == "/remocon/ir/leaning") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_ir_leaning(httpHeaders);
		return true;
	}
	else if (path == "/remocon/toplevel/invoke") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_toplevel_invoke(httpHeaders);
		return true;
	}
	else if (path == "/remocon/recogn/reload") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_recogn_reload(httpHeaders);
		return true;
	}
	else if (path == "/recong/mictest/wavdownload") 
	{
		*result = WEBSERVER_RESULT_TYPE_DOWNLOADFILE;
		*responsString = mictest_wavdownload(httpHeaders);
		return true;
	}
	else if (path == "/setting/twitter/regist") 
	{
		*responsString = setting_twitter_regist(httpHeaders,result);
		return true;
	}
	else if (path == "/setting/twitter/callback") 
	{
		*result = WEBSERVER_RESULT_TYPE_LOCATION;
		*responsString = setting_twitter_callback(httpHeaders,result);
		return true;
	}
	else if (path == "/setting/twitter/cancel") 
	{
		*result = WEBSERVER_RESULT_TYPE_LOCATION;
		*responsString = setting_twitter_cancel(httpHeaders,result);
		return true;
	}
	else if (path == "/setting/get/status") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = SettingStatus();
		return true;
	}
	else if( path == "/%ICON%" )
	{
		*result = WEBSERVER_RESULT_TYPE_TRASMITFILE;
		*responsString = ScriptRemoconWeb::GetWebRootPath("jscss/images/ajax-loader.gif");

		return true;
	}

	//これより下、値設定 CSRFトーク必須
	if (!ScriptRemoconWeb::checkCSRFToken(httpHeaders,result,responsString))
	{
		return true;
	}
	//これより下 認証と CSRFトークで守られた最重要エリア


	else if (path == "/remocon/ir/fire") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_ir_fire(httpHeaders);
		return true;
	}
	else if (path == "/remocon/update/icon_order") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_update_icon_order(httpHeaders);
		return true;
	}
	else if (path == "/remocon/update/elec") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_update_elec(httpHeaders);
		return true;
	}
	else if (path == "/remocon/update/elec_action") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_update_elec_action(httpHeaders);
		return true;
	}
	else if (path == "/remocon/update/elec_action_order") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_update_elec_action_order(httpHeaders);
		return true;
	}
	else if (path == "/remocon/delete/elec") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_delete_elec(httpHeaders);
		return true;
	}
	else if (path == "/remocon/delete/elec_action") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_delete_elec_action(httpHeaders);
		return true;
	}
	else if (path == "/remocon/fire/bytype") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_fire_bytype(httpHeaders);
		return true;
	}
	else if (path == "/remocon/fire/bystring") 
	{//xspeechでの入力
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_fire_bystring(httpHeaders);
		return true;
	}
	else if (path == "/remocon/preview/tospeak_string") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_preview_tospeak_string(httpHeaders);
		return true;
	}
	else if (path == "/remocon/preview/elec_mp3") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_preview_elec_mp3(httpHeaders);
		return true;
	}
	else if (path == "/remocon/preview/elec_command") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = script_scripteditor_command_run(httpHeaders);
		return true;
	}
	else if (path == "/remocon/preview/elec_macro") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_preview_elec_macro(httpHeaders);
		return true;
	}
	else if (path == "/remocon/get/append/info") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_get_append_info(httpHeaders);
		return true;
	}
	else if (path == "/remocon/sip/call") 
	{//SIPで発信する
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_sip_call(httpHeaders);
		return true;
	}
	else if (path == "/remocon/sip/answer") 
	{//SIPでかかってきた電話にこたえる
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_sip_answer(httpHeaders);
		return true;
	}
	else if (path == "/remocon/sip/hangup") 
	{//SIPで電話を切る
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_sip_hangup(httpHeaders);
		return true;
	}
	else if (path == "/remocon/sip/numpad/close") 
	{//numpadを閉じるイベント
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_sip_numpad_close(httpHeaders);
		return true;
	}
	else if (path == "/remocon/multiroom/change") 
	{//リモコン画面からの部屋変更.
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_multiroom_change(httpHeaders);
		return true;
	}
	else if (path == "/setting/account/update") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_account_update(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/setting/network/update") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_network_update(httpHeaders);
		return true;
	}
	else if (path == "/setting/recong/update") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_recong_update(httpHeaders);
		return true;
	}
	else if (path == "/setting/speak/update") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_speak_update(httpHeaders);
		return true;
	}
	else if (path == "/setting/version/upgradecheck") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_version_upgradecheck(httpHeaders);
		return true;
	}
	else if (path == "/setting/version/upgraderun") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_version_upgraderun(httpHeaders);
		return true;
	}
	else if (path == "/setting/version/reboot") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_version_reboot(httpHeaders);
		return true;
	}
	else if (path == "/setting/version/shutdown") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_version_shutdown(httpHeaders);
		return true;
	}
	else if (path == "/setting/version/log") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_DATA;
		*responsString = setting_version_log(httpHeaders);
		return true;
	}
	else if (path == "/recong/mictest/log") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = mictest_log(httpHeaders);
		return true;
	}
	else if (path == "/setting/version/download/supportfile") 
	{
		*result = WEBSERVER_RESULT_TYPE_TRASMITFILE;
		*responsString = setting_version_download_supportfile(httpHeaders);
		return true;
	}
	else if (path == "/setting/version/export") 
	{
		*result = WEBSERVER_RESULT_TYPE_DOWNLOADFILE;
		*responsString = version_export(httpHeaders);
		return true;
	}
	else if (path == "/setting/version/import") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = version_import(httpHeaders);
		return true;
	}
	else if (path == "/setting/sensor/getall") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_sensor_getall(httpHeaders);
		return true;
	}
	else if (path == "/setting/sensor/update") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_sensor_update(httpHeaders);
		return true;
	}
	else if (path == "/setting/system/update") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_system_update(httpHeaders);
		return true;
	}
	else if (path == "/setting/changelang/update") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_changelang_update(httpHeaders);
		return true;
	}
	else if (path == "/setting/remoconsetting/update") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_remoconsetting_update(httpHeaders);
		return true;
	}
	else if (path == "/setting/trigger/update") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_trigger_update(httpHeaders);
		return true;
	}
	else if (path == "/setting/trigger/delete") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_trigger_delete(httpHeaders);
		return true;
	}
	else if (path == "/setting/trigger/update") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_trigger_update(httpHeaders);
		return true;
	}
	else if (path == "/setting/trigger/delete") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_trigger_delete(httpHeaders);
		return true;
	}
	else if (path == "/setting/trigger/enable_change") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_trigger_enable_change(httpHeaders);
		return true;
	}
	else if (path == "/trigger/preview/if_web") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = trigger_preview_if_web(httpHeaders);
		return true;
	}
	else if (path == "/trigger/preview/if_weather") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = trigger_preview_if_weather(httpHeaders);
		return true;
	}
	else if (path == "/trigger/preview/if_command") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = trigger_preview_if_command(httpHeaders);
		return true;
	}
	else if (path == "/setting/sip/update") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_sip_update(httpHeaders);
		return true;
	}
	else if ( path == "/setting/multiroom/search" )
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_multiroom_search(httpHeaders);
		return true;
	}
	else if (path == "/setting/multiroom/delete") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_multiroom_delete(httpHeaders);
		return true;
	}
	else if (path == "/setting/multiroom/order") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_multiroom_order(httpHeaders);
		return true;
	}
	else if (path == "/setting/multiroom/append") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_multiroom_append(httpHeaders);
		return true;
	}
	else if (path == "/setting/webapi/resetkey") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_webapi_resetkey(httpHeaders);
		return true;
	}
	else if (path == "/setting/homekit/boot") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_homekit_boot(httpHeaders);
		return true;
	}
	else if (path == "/setting/homekit/resetkey") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_homekit_resetkey(httpHeaders);
		return true;
	}
	else if (path == "/remocon/fileselectpage/find") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_fileselectpage_find(httpHeaders);
		return true;
	}
	else if (path == "/remocon/fileselectpage/upload") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_fileselectpage_upload(httpHeaders);
		return true;
	}
	else if (path == "/remocon/fileselectpage/delete") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_fileselectpage_delete(httpHeaders);
		return true;
	}
	else if (path == "/script/scripteditor/load") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_DATA;
		*responsString = script_scripteditor_load(httpHeaders);
		return true;
	}
	else if (path == "/script/scripteditor/save") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = script_scripteditor_save(httpHeaders);
		return true;
	}
	else if (path == "/script/scripteditor/command_run") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = script_scripteditor_command_run(httpHeaders);
		return true;
	}
	else if (path == "/script/scripteditor/annotation") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = script_scripteditor_annotation(httpHeaders);

		return true;
	}
	else if ( path == "/remocon/netdevice/elec/getlist" )
	{
		*responsString = remocon_netdevice_elec_getlist(httpHeaders);
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		return true;
	}
	else if ( path == "/remocon/netdevice/action/getlist" )
	{
		*responsString = remocon_netdevice_action_getlist(httpHeaders);
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		return true;
	}
	else if ( path == "/remocon/netdevice/value/getlist" )
	{
		*responsString = remocon_netdevice_value_getlist(httpHeaders);
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		return true;
	}
	else if ( path == "/remocon/netdevice/fire" )
	{

		*responsString = remocon_netdevice_fire(httpHeaders);
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		return true;
	}
	else if ( path == "/remocon/sip/fire" )
	{
		*responsString = remocon_sip_fire(httpHeaders);
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		return true;
	}
	else
	{
		return false;
	}
}

#endif //WITH_CLIENT_ONLY_CODE