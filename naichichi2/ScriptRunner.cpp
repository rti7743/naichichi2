//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include "ScriptRunner.h"
#include "MainWindow.h"
#include "ActionImplement.h"
#include <fstream>
#include "XLStringUtil.h"
#include "XLFileUtil.h"
#include "ScriptRemoconWeb.h"
#include "SystemMisc.h"
#include "XLGISAddress.h"
#include "MultiRoomUtil.h"

#if _MSC_VER
	#pragma comment(lib, "v8.lib")
#endif
//config をそのまま触るとシステムを壊すことがあるので、通常利用する部分は user__ で jailします。
const string USERPREFIX_JAIL = "user__";

//thisの引き回しがめんどいのでこれで逃げますww
//どうせ、この部分は 特定のスレッドが一本で処理するのでスレッド同期の問題は発生しない!!
static ScriptRunner* g_ScriptRunner_This = NULL;
//実行している個数
static int g_ScriptRunner_RunCount = 0;
//stdout
static string g_STDOUT;

ScriptRunner::~ScriptRunner()
{
	this->V8Context.Dispose();
	this->V8Global.Clear();

	//ワーカーリソースの解放
	WorkResourceClear();

//	ClearCallback();

}

void ScriptRunner::ClearCallback()
{
	for(auto it = this->CallbackList.begin() ; it != this->CallbackList.end() ; ++it)
	{
		MainWindow::m()->Recognition.RemoveCallback(*it,false);
		MainWindow::m()->Speak.RemoveCallback(*it,false);
		MainWindow::m()->Trigger.RemoveCallbackDelay(*it);
	}
	this->CallbackList.clear();
}

CallbackPP ScriptRunner::NewCallback(std::function<void (void) > f)
{
	CallbackPP pp = CallbackPP::New(f);
	this->CallbackList.push_back(pp);

	return pp;
}


//Scriptインスタンスを解放し初期化します。 変更を加えたコールバックを破棄します。
void ScriptRunner::Reset()
{
	ClearCallback();

	this->V8Context.Dispose();
	this->V8Global.Clear();

	//ワーカーリソースの解放
	WorkResourceClear();
}


bool ScriptRunner::LoadScript(const string & filename)
{
	ASSERT_IS_MAIN_THREAD_RUNNING();
	// V8を開く
	this->filename = filename;
	CreateV8Instance(XLFileUtil::cat(filename));

	return true;
}


bool ScriptRunner::EvalScript(const string & script)
{
	ASSERT_IS_MAIN_THREAD_RUNNING();
	// V8を開く
	this->filename = "<EVAL>";
	CreateV8Instance(script);

	return true;
}

bool ScriptRunner::CreateV8Instance(const string& source)
{
	ASSERT_IS_MAIN_THREAD_RUNNING();
	

	//これがグローバルなテンプレートだだだだ!!
	this->V8Global = v8::ObjectTemplate::New();

	//関数をセット
	this->V8Global->Set(v8::String::New("onvoice"),				 v8::FunctionTemplate::New(v8_onvoice));
	this->V8Global->Set(v8::String::New("action"),				 v8::FunctionTemplate::New(v8_action));
	this->V8Global->Set(v8::String::New("callcommand"),			 v8::FunctionTemplate::New(v8_callcommand));
	this->V8Global->Set(v8::String::New("speak"),				 v8::FunctionTemplate::New(v8_speak));
	this->V8Global->Set(v8::String::New("speak_cache"),			 v8::FunctionTemplate::New(v8_speak_cache));
	this->V8Global->Set(v8::String::New("play"),				 v8::FunctionTemplate::New(v8_play));
	this->V8Global->Set(v8::String::New("config_get"),			 v8::FunctionTemplate::New(v8_config_get));
	this->V8Global->Set(v8::String::New("config_set"),			 v8::FunctionTemplate::New(v8_config_set));
	this->V8Global->Set(v8::String::New("kernel_config_get"),	 v8::FunctionTemplate::New(v8_kernel_config_get));
	this->V8Global->Set(v8::String::New("kernel_config_set"),	 v8::FunctionTemplate::New(v8_kernel_config_set));
	this->V8Global->Set(v8::String::New("telnet"),				 v8::FunctionTemplate::New(v8_telnet));
	this->V8Global->Set(v8::String::New("http_get"),			 v8::FunctionTemplate::New(v8_http_get));
	this->V8Global->Set(v8::String::New("http_post"),			 v8::FunctionTemplate::New(v8_http_post));
	this->V8Global->Set(v8::String::New("kernel_execute_local"), v8::FunctionTemplate::New(v8_kernel_execute_local));
	this->V8Global->Set(v8::String::New("execute_remote"),		 v8::FunctionTemplate::New(v8_execute_remote));
	this->V8Global->Set(v8::String::New("msleep"),				 v8::FunctionTemplate::New(v8_msleep));
	this->V8Global->Set(v8::String::New("dump"),				 v8::FunctionTemplate::New(v8_dump));
	this->V8Global->Set(v8::String::New("print"),				 v8::FunctionTemplate::New(v8_dump));
	this->V8Global->Set(v8::String::New("var_dump"),			 v8::FunctionTemplate::New(v8_dump));
	this->V8Global->Set(v8::String::New("alert"),				 v8::FunctionTemplate::New(v8_dump));
	this->V8Global->Set(v8::String::New("echo"),				 v8::FunctionTemplate::New(v8_dump));
	this->V8Global->Set(v8::String::New("sensor_lumi"),			 v8::FunctionTemplate::New(v8_sensor_lumi));
	this->V8Global->Set(v8::String::New("sensor_temp"),			 v8::FunctionTemplate::New(v8_sensor_temp));
	this->V8Global->Set(v8::String::New("sensor_sound"),		 v8::FunctionTemplate::New(v8_sensor_sound));
	this->V8Global->Set(v8::String::New("config_find"),			 v8::FunctionTemplate::New(v8_config_find));
	this->V8Global->Set(v8::String::New("kernel_config_find"),	 v8::FunctionTemplate::New(v8_kernel_config_find));

#if _WITH_USB
	this->V8Global->Set(v8::String::New("usb_open"),			 v8::FunctionTemplate::New(v8_usb_open));
	this->V8Global->Set(v8::String::New("usb_close"),			 v8::FunctionTemplate::New(v8_usb_close));
	this->V8Global->Set(v8::String::New("usb_read"),			 v8::FunctionTemplate::New(v8_usb_read));
	this->V8Global->Set(v8::String::New("usb_write"),			 v8::FunctionTemplate::New(v8_usb_write));
	this->V8Global->Set(v8::String::New("usb_messageread"),		 v8::FunctionTemplate::New(v8_usb_messageread));
	this->V8Global->Set(v8::String::New("usb_messagewrite"),	 v8::FunctionTemplate::New(v8_usb_messagewrite));
#endif //_WITH_USB

	this->V8Global->Set(v8::String::New("elec_getlist"),		 v8::FunctionTemplate::New(v8_elec_getlist));
	this->V8Global->Set(v8::String::New("elec_getactionlist"),	 v8::FunctionTemplate::New(v8_elec_getactionlist));
	this->V8Global->Set(v8::String::New("elec_getinfo"),	     v8::FunctionTemplate::New(v8_elec_getinfo));
	this->V8Global->Set(v8::String::New("elec_setstatus"),		 v8::FunctionTemplate::New(v8_elec_setstatus));
	this->V8Global->Set(v8::String::New("elec_getstatus"),		 v8::FunctionTemplate::New(v8_elec_getstatus));
	this->V8Global->Set(v8::String::New("sstp_send11"),			 v8::FunctionTemplate::New(v8_sstp_send11));
	this->V8Global->Set(v8::String::New("soap_action"),			 v8::FunctionTemplate::New(v8_soap_action));
	this->V8Global->Set(v8::String::New("wol_send"),			 v8::FunctionTemplate::New(v8_wol_send));
	this->V8Global->Set(v8::String::New("env_version"),			 v8::FunctionTemplate::New(v8_env_version));
	this->V8Global->Set(v8::String::New("env_model"),			 v8::FunctionTemplate::New(v8_env_model));
	this->V8Global->Set(v8::String::New("env_os"),				 v8::FunctionTemplate::New(v8_env_os));
	this->V8Global->Set(v8::String::New("env_mic_volume"),		 v8::FunctionTemplate::New(v8_env_mic_volume));
	this->V8Global->Set(v8::String::New("env_speaker_volume"),	 v8::FunctionTemplate::New(v8_env_speaker_volume));
	this->V8Global->Set(v8::String::New("upnp_search_all"),		 v8::FunctionTemplate::New(v8_upnp_search_all));
	this->V8Global->Set(v8::String::New("upnp_search_one"),		 v8::FunctionTemplate::New(v8_upnp_search_one));
	this->V8Global->Set(v8::String::New("upnp_search_uuid"),	 v8::FunctionTemplate::New(v8_upnp_search_uuid));
	this->V8Global->Set(v8::String::New("upnp_search_xml"),		 v8::FunctionTemplate::New(v8_upnp_search_xml));
	this->V8Global->Set(v8::String::New("url_hosturl"),			 v8::FunctionTemplate::New(v8_url_hosturl));
	this->V8Global->Set(v8::String::New("url_baseurl"),			 v8::FunctionTemplate::New(v8_url_baseurl));
	this->V8Global->Set(v8::String::New("url_changeprotocol"),	 v8::FunctionTemplate::New(v8_url_changeprotocol));
	this->V8Global->Set(v8::String::New("url_appendparam"),		 v8::FunctionTemplate::New(v8_url_appendparam));
	this->V8Global->Set(v8::String::New("base64encode"),		 v8::FunctionTemplate::New(v8_base64encode));
	this->V8Global->Set(v8::String::New("base64decode"),		 v8::FunctionTemplate::New(v8_base64decode));
	this->V8Global->Set(v8::String::New("urlencode"),			 v8::FunctionTemplate::New(v8_urlencode));
	this->V8Global->Set(v8::String::New("urldecode"),			 v8::FunctionTemplate::New(v8_urldecode));
	this->V8Global->Set(v8::String::New("md5"),					 v8::FunctionTemplate::New(v8_md5));
	this->V8Global->Set(v8::String::New("sha1"),				 v8::FunctionTemplate::New(v8_sha1));
	this->V8Global->Set(v8::String::New("escapeshellarg"),		 v8::FunctionTemplate::New(v8_escapeshellarg));
	this->V8Global->Set(v8::String::New("mb_convert_kana"),		 v8::FunctionTemplate::New(v8_mb_convert_kana));
	this->V8Global->Set(v8::String::New("mb_convert_yomi"),		 v8::FunctionTemplate::New(v8_mb_convert_yomi));
	
	this->V8Global->Set(v8::String::New("date"),				 v8::FunctionTemplate::New(v8_date));
	this->V8Global->Set(v8::String::New("time"),				 v8::FunctionTemplate::New(v8_time));
	this->V8Global->Set(v8::String::New("time_dayofweek"),		 v8::FunctionTemplate::New(v8_time_dayofweek));
	this->V8Global->Set(v8::String::New("uuid"),				 v8::FunctionTemplate::New(v8_uuid));
	this->V8Global->Set(v8::String::New("trim"),				 v8::FunctionTemplate::New(v8_trim));
	this->V8Global->Set(v8::String::New("html_selector"),		 v8::FunctionTemplate::New(v8_html_selector));
	this->V8Global->Set(v8::String::New("html_inner"),			 v8::FunctionTemplate::New(v8_html_inner));
	this->V8Global->Set(v8::String::New("html_text"),			 v8::FunctionTemplate::New(v8_html_text));
	this->V8Global->Set(v8::String::New("innerHTML"),			 v8::FunctionTemplate::New(v8_html_inner));
	this->V8Global->Set(v8::String::New("innerText"),			 v8::FunctionTemplate::New(v8_html_text));
	this->V8Global->Set(v8::String::New("innerhtml"),			 v8::FunctionTemplate::New(v8_html_inner));
	this->V8Global->Set(v8::String::New("innertext"),			 v8::FunctionTemplate::New(v8_html_text));
	this->V8Global->Set(v8::String::New("html_attr"),			 v8::FunctionTemplate::New(v8_html_attr));
	this->V8Global->Set(v8::String::New("setInterval"),			 v8::FunctionTemplate::New(v8_setInterval));
	this->V8Global->Set(v8::String::New("setTimeout"),			 v8::FunctionTemplate::New(v8_setTimeout));
	this->V8Global->Set(v8::String::New("clearTimeout"),		 v8::FunctionTemplate::New(v8_clearTimeout));
	this->V8Global->Set(v8::String::New("clearInterval"),		 v8::FunctionTemplate::New(v8_clearTimeout));
	this->V8Global->Set(v8::String::New("setinterval"),			 v8::FunctionTemplate::New(v8_setInterval));	//小文字バージョン
	this->V8Global->Set(v8::String::New("settimeout"),			 v8::FunctionTemplate::New(v8_setTimeout));		//小文字バージョン
	this->V8Global->Set(v8::String::New("cleartimeout"),		 v8::FunctionTemplate::New(v8_clearTimeout));	//小文字バージョン
	this->V8Global->Set(v8::String::New("clearinterval"),		 v8::FunctionTemplate::New(v8_clearTimeout));	//小文字バージョン

	this->V8Global->Set(v8::String::New("time_gmt_to_local"),	 v8::FunctionTemplate::New(v8_time_gmt_to_local));
	this->V8Global->Set(v8::String::New("time_local_to_gmt"),	 v8::FunctionTemplate::New(v8_time_local_to_gmt));
	this->V8Global->Set(v8::String::New("time_is_holiday"),		 v8::FunctionTemplate::New(v8_time_is_holiday));
	this->V8Global->Set(v8::String::New("time_day_info"),		 v8::FunctionTemplate::New(v8_time_day_info));
	this->V8Global->Set(v8::String::New("address_parse"),		 v8::FunctionTemplate::New(v8_address_parse));

	this->V8Global->Set(v8::String::New("kernel_kill"),			 v8::FunctionTemplate::New(v8_kernel_kill));
	this->V8Global->Set(v8::String::New("kernel_system_reboot"), v8::FunctionTemplate::New(v8_kernel_system_reboot));
	this->V8Global->Set(v8::String::New("kernel_process_list"),	 v8::FunctionTemplate::New(v8_kernel_process_list));

	this->V8Global->Set(v8::String::New("weather_info"),		 v8::FunctionTemplate::New(v8_weather_info));
	this->V8Global->Set(v8::String::New("wearher_is_rain"),		 v8::FunctionTemplate::New(v8_wearher_is_rain));
	this->V8Global->Set(v8::String::New("wearher_is_rain_soon"), v8::FunctionTemplate::New(v8_wearher_is_rain_soon));
	this->V8Global->Set(v8::String::New("train_info"),			 v8::FunctionTemplate::New(v8_train_info));
	this->V8Global->Set(v8::String::New("train_is_delay"),		 v8::FunctionTemplate::New(v8_train_is_delay));
	this->V8Global->Set(v8::String::New("background_music_play"),v8::FunctionTemplate::New(v8_background_music_play));
	this->V8Global->Set(v8::String::New("background_music_stop"),v8::FunctionTemplate::New(v8_background_music_stop));

	this->V8Global->Set(v8::String::New("twitter_tweet"),v8::FunctionTemplate::New(v8_twitter_tweet));
	this->V8Global->Set(v8::String::New("twitter_home_timeline"),v8::FunctionTemplate::New(v8_twitter_home_timeline));
	this->V8Global->Set(v8::String::New("twitter_user_timeline"),v8::FunctionTemplate::New(v8_twitter_user_timeline));

	this->V8Global->Set(v8::String::New("multiroom_list"),v8::FunctionTemplate::New(v8_multiroom_list));
	this->V8Global->Set(v8::String::New("multiroom_fire"),v8::FunctionTemplate::New(v8_multiroom_fire));

	// Create a new context.
	//Persistent<Context> context = Context::New();
	this->V8Context = v8::Context::New(NULL,this->V8Global); //ここで globalを渡す.
  
	// Enter the created context for compiling and
	// running the hello world script. 
	v8::Context::Scope context_scope(this->V8Context);

	v8::TryCatch try_catch;

	// Create a string containing the JavaScript source code.
	v8::Handle<v8::String> jssource = v8::String::New(source.c_str());

	// Compile the source code.
	v8::Handle<v8::Script> compiled_script = v8::Script::Compile(jssource);
	if (compiled_script.IsEmpty())
	{
		throw XLException("スクリプトの読み込みに失敗しました ファイル名:"+ this->filename + " V8error:"  + ReportException(&try_catch) );
	}

	const auto backup_g_ScriptRunner_This = g_ScriptRunner_This;
	g_ScriptRunner_This = this;

	// Run the script to get the result.
	v8::Handle<v8::Value> result;
	bool isComplate = RunScriptWithTimeout([&](){
		result = compiled_script->Run();
	} );
	if (!isComplate)
	{
		throw XLException("スクリプトの実行がタイムアウトしました ファイル名:"+ this->filename  );
	}
	if (result.IsEmpty())
	{
		throw XLException("スクリプトの実行に失敗しました ファイル名:"+ this->filename + " V8error:"  + ReportException(&try_catch) );
	}

	g_ScriptRunner_This = backup_g_ScriptRunner_This;

	return true;
}


//V8の関数を実行する
v8::Handle<v8::Value> ScriptRunner::RunV8Function(v8::Handle<v8::Function>& function,unsigned int argCount,v8::Handle<v8::Value> args[])
{
	ASSERT_IS_MAIN_THREAD_RUNNING();

	const auto backup_g_ScriptRunner_This = g_ScriptRunner_This;
	g_ScriptRunner_This = this;
	g_ScriptRunner_RunCount++;

	v8::Handle<v8::Value> result;
	if (g_ScriptRunner_RunCount >= 2)
	{//実行する. タイムアウト監視は、ネスト元が呼び出しているはず
		result = function->Call(function, argCount , args);
	}
	else
	{//ネストされていないトップレベル呼び出します。
		//STDOUTを消します。
		g_STDOUT = "";
		RunScriptWithTimeout([&](){
			result = function->Call(function, argCount , args);
		} );
	}
	

	g_ScriptRunner_RunCount--;
	g_ScriptRunner_This = backup_g_ScriptRunner_This;

	//ワーカーリソースの解放
	WorkResourceClear();
	
	return result;
}

bool ScriptRunner::RunScriptWithTimeout(const std::function<void(void)>& func )
{
	//タイムアウトを監視するスレッドを作ります.
	bool isTimeoutAbort = false;
	bool stopFlag = false;
	const int TIMEOUT_MS = 60000; //60秒.
	const int SLEEP_SPAN = 100; //100ミリ秒.
	auto isolate = v8::Isolate::GetCurrent();
	auto timeoutTread = new thread([&](){
		for( int i = 0 ; i < TIMEOUT_MS / SLEEP_SPAN ; i++ )
		{
			if (stopFlag) return;
			MiriSleep(SLEEP_SPAN);
		}

		NOTIFYLOG( "V8処理がタイムアウトしました。 " << TIMEOUT_MS << "ミリ秒までに処理が終わりませんでした" );
		//V8を強制終了させる
		v8::V8::TerminateExecution(isolate);
		isTimeoutAbort = true;
	});
	func();

	const bool complate = (stopFlag == false);

	//タイムアウトスレッドを止める
	stopFlag = true;
	timeoutTread->join();
	delete timeoutTread;

	return ! isTimeoutAbort;
}


string ScriptRunner::callFunction(const string& name)
{
	ASSERT_IS_MAIN_THREAD_RUNNING();
	
	DEBUGLOG(string() + "callFunction:" + name + "()");

	v8::Context::Scope context_scope(this->V8Context);

	//関数を取得.
	v8::Local<v8::Function> function = v8::Local<v8::Function>::Cast( this->V8Context->Global()->Get(v8::String::New( _A2U(name.c_str()) )) );

	v8::TryCatch try_catch;
	//引数を作成する
	v8::Handle<v8::Value> args[1] = {};

	//V8の関数を実行する
	v8::Handle<v8::Value> result = RunV8Function(function,0,args);

	if (result.IsEmpty())
	{
		ERRORLOG(string() + "callFunction:" + name + "()" + " エラー:" + "スクリプトの内関数" + name + "の実行に失敗しました ファイル名:"+ this->filename + " V8error:"  + ReportException(&try_catch));
		throw XLException("スクリプトの内関数" + name + "の実行に失敗しました ファイル名:"+ this->filename + " V8error:"  + ReportException(&try_catch) );
	}
	if (result->IsObject() || result->IsArray() )
	{
		result = JsonSerialize(result);
	}

	v8::String::AsciiValue ascii(result);
	string r =  _U2A(*ascii);
	if (r == "undefined")
	{
		DEBUGLOG(string() + "callFunction:" + name + "()" + " 正常終了" + "(undefined)");
		return "";
	}
	DEBUGLOG(string() + "callFunction:" + name + "()" + " 正常終了" + "(" + r + ")");
	return r;
}


string ScriptRunner::callFunction(const string& name,const list<string> & list)
{
	ASSERT_IS_MAIN_THREAD_RUNNING();

	
	DEBUGLOG(string() + "callFunction:" + name + "(!list!)");
	if (list.size() >= 16)
	{
		ERRORLOG("エラー: 引数は16個までです");
		return "";
	}

	v8::Context::Scope context_scope(this->V8Context);

	//関数を取得.
	v8::Local<v8::Function> function = v8::Local<v8::Function>::Cast( this->V8Context->Global()->Get(v8::String::New( _A2U(name.c_str()) )) );

	//引数を作成する
	v8::Handle<v8::Value> args[16] ;
	v8::TryCatch try_catch;

	unsigned int size = list.size();
	auto it = list.begin();
	for(unsigned int n = 0 ; it != list.end() ; ++it , ++ n )
	{
		args[n] = v8::String::New(_A2U(it->c_str()) );
	}

	//V8の関数を実行する
	v8::Handle<v8::Value> result = RunV8Function(function,size,args);

	if (result.IsEmpty())
	{
		DEBUGLOG(string() + "callFunction:" + name + "(!list!)" + " エラー" + "スクリプトの内関数" + name + "の実行に失敗しました ファイル名:"+ this->filename + " V8error:"  + ReportException(&try_catch));
		throw XLException("スクリプトの内関数" + name + "の実行に失敗しました ファイル名:"+ this->filename + " V8error:"  + ReportException(&try_catch) );
	}
	if (result->IsObject() || result->IsArray() )
	{
		result = JsonSerialize(result);
	}
	;

	const string r =  ToStdString(result->ToString());
	if (r == "undefined")
	{
		DEBUGLOG(string() + "callFunction:" + name + "(!list!)" + " 正常終了" + "(undefined)");
		return "";
	}
	DEBUGLOG(string() + "callFunction:" + name + "(!list!)" + " 正常終了" + "(" + r + ")");
	return r;
}

string ScriptRunner::callFunctionResultJson(const string& name,const list<string> & list,map<string,string>* outResult)
{
	ASSERT_IS_MAIN_THREAD_RUNNING();

	string result = callFunction(name,list);
	StringToJson(result,outResult);

	return result;
}

string ScriptRunner::StringToJson(const string result,map<string,string>* outResult)
{
	ASSERT_IS_MAIN_THREAD_RUNNING();

	v8::Handle<v8::Value> retobj = this->JsonUnSerialize(result);
	if ( IsbadObject(retobj) )
	{//結果をjsonとして解析できませんでした
		outResult->clear();
	}
	else
	{//返還できたので、mapに変換する.
		*outResult = ToStdStringMap(retobj);
	}
	return result;
}


void ScriptRunner::callbackFunction(v8::Persistent<v8::Function> callback)
{
	

	MainWindow::m()->SyncInvoke([&](){
		v8::Context::Scope context_scope(this->V8Context);

		//引数を作成する
		v8::Handle<v8::Value> args[1] = {};

		//V8の関数を実行する
		v8::TryCatch try_catch;
		v8::Handle<v8::Value> result = RunV8Function(callback,0,args);

		if (result.IsEmpty())
		{
			ERRORLOG("スクリプト実行時にエラー:ファイル名:" << this->filename << " V8error:" << ReportException(&try_catch) );
			return ;
		}
		if (result->IsObject() || result->IsArray() )
		{
			result = JsonSerialize(result);
		}

		v8::String::AsciiValue ascii(result);
		string functionResult =  _U2A(*ascii);
		if (functionResult == "undefined")
		{
			DEBUGLOG("callback正常終了" << "(undefined)" );
			return ;
		}
		DEBUGLOG("callback正常終了" << "(" << functionResult << ")" );
		return ;
	});
}

v8::Handle<v8::Value> ScriptRunner::v8_onvoice(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:onvoice" );
	ScriptRunner* _this = g_ScriptRunner_This;

	if (!_this->IsScenario)
	{
		return v8::ThrowException(v8::String::New(_A2U("onvoiceには会話シナリオだけしか使えません。")));
	}

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("onvoiceには1つ以上の引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("onvoiceの第一引数が文字列ではありません")));
	}

	const string p1 = ToStdString(args[0]->ToString());
	if (!MainWindow::m()->Recognition.checkExprYomi(p1))
	{
		string msg = "音声認識に対応していない文字列(" + p1 + ")が含まれています";
		return v8::ThrowException(v8::String::New(_A2U(msg.c_str())));
	}
	const string fromSetting = "script:" + _this->filename;
	
	if (MainWindow::m()->Recognition.isAlreadyRegist5(p1, fromSetting,"","","",""))
	{
		string msg = "音声認識コマンド(" + p1 + ")は既に使われています";
		return v8::ThrowException(v8::String::New(_A2U(msg.c_str())));
	}

	CallbackPP callback ;
	if (args.Length() >= 2 )
	{
		if (! args[1]->IsFunction() )
		{
			return v8::ThrowException(v8::String::New(_A2U("onvoiceの第二引数がコールバックではありません")));
		}
		
		auto callbackFunc = v8::Persistent<v8::Function>::New( args[1].As<v8::Function>() );
		callback = _this->NewCallback([=](){
			_this->OnVoiceCallback(callbackFunc);
		});
	}
	
	try
	{
		MainWindow::m()->Recognition.AddCommandRegexp( callback , p1  , fromSetting);
	}
	catch(XLException &e)
	{
		const string ee = std::string("onvice/") + e.what();
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}

	return v8::Boolean::New(true);
}

void ScriptRunner::OnVoiceCallback(v8::Persistent<v8::Function> callback)
{
	if (MainWindow::m()->IsRecongpause() )
	{
		NOTIFYLOG("現在音声認識は停止中です。");

		//音を流す.
		const string recong_pause_mp3 = MainWindow::m()->Config.Get("recong_pause_mp3", "recong_pause_syun.mp3" );
		MainWindow::m()->MusicPlayAsync.Play(recong_pause_mp3 ,1);

		return ;
	}

	callbackFunction(callback);
}


v8::Handle<v8::Value> ScriptRunner::v8_setInterval(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:setInterval" );
	ScriptRunner* _this = g_ScriptRunner_This;

	if (!_this->IsScenario)
	{
		return v8::ThrowException(v8::String::New(_A2U("setIntervalには会話シナリオだけしか使えません。")));
	}

	if (args.Length() < 2)
	{
		return v8::ThrowException(v8::String::New(_A2U("setIntervalには2つ以上の引数が必要です")));
	}

	if (! args[0]->IsFunction() )
	{
		return v8::ThrowException(v8::String::New(_A2U("setIntervalの第一引数がコールバックではありません")));
	}

	unsigned int timeMS = 0;
	if ( args[1]->IsInt32() )
	{
		timeMS = args[1]->ToInt32()->Value();
	}
	else if ( args[1]->IsString() )
	{
		timeMS = ToStringToI(args[1]->ToString());
	}
	else
	{
		return v8::ThrowException(v8::String::New(_A2U("setIntervalの第二引数が数字(定期的に呼び出すマイクロ秒数)ではありません")));
	}

	int resourceID = _this->TimerCallbackList.size();
	if( resourceID >= INT_MAX)
	{
		return v8::ThrowException(v8::String::New(_A2U("タイマーリソースがもうありません。")));
	}

	CallbackPP callback;
	auto callbackFunc = v8::Persistent<v8::Function>::New( args[0].As<v8::Function>() );
	callback = _this->NewCallback([=](){
		_this->callbackFunction(callbackFunc);
	});

	//clearTimeoutとかで削除できるようにリソース番号を作り出します。(めんどい)
	_this->TimerCallbackList.push_back(callback);
	
	try
	{
		MainWindow::m()->Trigger.AppendNextSecondTrigger(callback , MAX(timeMS / 1000,1) , true );
	}
	catch(XLException &e)
	{
		const string ee = std::string("setInterval/") + e.what();
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}

	return v8::Integer::New(resourceID);
}

v8::Handle<v8::Value> ScriptRunner::v8_setTimeout(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:setTimeout" );
	ScriptRunner* _this = g_ScriptRunner_This;

	if (!_this->IsScenario)
	{
		return v8::ThrowException(v8::String::New(_A2U("setTimeoutには会話シナリオだけしか使えません。")));
	}

	if (args.Length() < 2)
	{
		return v8::ThrowException(v8::String::New(_A2U("setTimeoutには2つ以上の引数が必要です")));
	}

	if (! args[0]->IsFunction() )
	{
		return v8::ThrowException(v8::String::New(_A2U("setTimeoutの第一引数がコールバックではありません")));
	}

	unsigned int timeMS = 0;
	if ( args[1]->IsInt32() )
	{
		timeMS = args[1]->ToInt32()->Value();
	}
	else if ( args[1]->IsString() )
	{
		timeMS = ToStringToI(args[1]->ToString());
	}
	else
	{
		return v8::ThrowException(v8::String::New(_A2U("setTimeoutの第二引数が数字(定期的に呼び出すマイクロ秒数)ではありません")));
	}
	int resourceID = _this->TimerCallbackList.size();
	if( resourceID >= INT_MAX)
	{
		return v8::ThrowException(v8::String::New(_A2U("タイマーリソースがもうありません。")));
	}

		
	CallbackPP callback;
	auto callbackFunc = v8::Persistent<v8::Function>::New( args[0].As<v8::Function>() );
	callback = _this->NewCallback([=](){
		_this->callbackFunction(callbackFunc);
	});
	//clearTimeoutとかで削除できるようにリソース番号を作り出します。(めんどい)
	_this->TimerCallbackList.push_back(callback);
	
	try
	{
		MainWindow::m()->Trigger.AppendNextSecondTrigger(callback , MAX(timeMS / 1000,1) , false );
	}
	catch(XLException &e)
	{
		const string ee = std::string("setTimeout/") + e.what();
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}

	return v8::Integer::New(resourceID);
}

v8::Handle<v8::Value> ScriptRunner::v8_clearTimeout(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:clearTimeout/clearInterval" );
	ScriptRunner* _this = g_ScriptRunner_This;

	if (!_this->IsScenario)
	{
		return v8::ThrowException(v8::String::New(_A2U("clearTimeout/clearIntervalには会話シナリオだけしか使えません。")));
	}

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("clearTimeout/clearIntervalには1つ以上の引数が必要です")));
	}

	unsigned int resorceID = 0;
	if ( args[0]->IsInt32() )
	{
		resorceID = args[0]->ToInt32()->Value();
	}
	else if ( args[0]->IsString() )
	{
		resorceID = ToStringToI(args[0]->ToString());
	}
	else
	{
		return v8::ThrowException(v8::String::New(_A2U("clearTimeout/clearIntervalの第一引数がタイマーのリソースIDではありません")));
	}
		
	if (resorceID < 0 || resorceID >= _this->TimerCallbackList.size() ) 
	{//指定されたリソースIDはないです
		return v8::Boolean::New(false);
	}
	CallbackPP callback = _this->TimerCallbackList[resorceID];
	if (callback.Empty())
	{//指定したコールバックは既に削除されています
		return v8::Boolean::New(true);
	}
	_this->TimerCallbackList[resorceID] = CallbackPP::NoCallback();

	try
	{
		MainWindow::m()->Trigger.RemoveCallbackDelay(callback);
	}
	catch(XLException &e)
	{
		const string ee = std::string("clearTimeouts/clearInterval") + e.what();
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}

	return v8::Boolean::New(true);
}

v8::Handle<v8::Value> ScriptRunner::v8_time_gmt_to_local(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:time_gmt_to_local" );
	ScriptRunner* _this = g_ScriptRunner_This;

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("time_gmt_to_localには1つ以上の引数が必要です")));
	}

	time_t time = 0;
	if ( args[0]->IsInt32() )
	{
		time = args[0]->ToInt32()->Value();
	}
	else if ( args[0]->IsString() )
	{
		time = ToStringToI(args[0]->ToString());
	}
	else if ( args[0]->IsNumber() )
	{
		time = args[0]->ToInteger()->Value();
	}
	else
	{
		return v8::ThrowException(v8::String::New(_A2U("time_gmt_to_localの第一引数が数字(unixtime)ではありません")));
	}
		
	if (time == 0  ) 
	{//時刻ではありません
		return v8::Boolean::New(false);
	}
	time = XLStringUtil::GMTtoLocalTime(time);
	
	return v8::Number::New( (double) time );
}

v8::Handle<v8::Value> ScriptRunner::v8_time_local_to_gmt(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:time_local_to_gmt" );
	ScriptRunner* _this = g_ScriptRunner_This;

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("time_local_to_gmtには1つ以上の引数が必要です")));
	}

	time_t time = 0;
	if ( args[0]->IsInt32() )
	{
		time = args[0]->ToInt32()->Value();
	}
	else if ( args[0]->IsString() )
	{
		time = ToStringToI(args[0]->ToString());
	}
	else if ( args[0]->IsNumber() )
	{
		time = args[0]->ToInteger()->Value();
	}
	else
	{
		return v8::ThrowException(v8::String::New(_A2U("time_local_to_gmtの第一引数が数字(unixtime)ではありません")));
	}
		
	if (time == 0  ) 
	{//時刻ではありません
		return v8::Boolean::New(false);
	}
	time = XLStringUtil::LocalTimetoGMT(time);
	
	return v8::Number::New( (double) time );
}

v8::Handle<v8::Value> ScriptRunner::v8_time_is_holiday(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:time_is_holiday" );
	ScriptRunner* _this = g_ScriptRunner_This;

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("time_is_holidayには1つ以上の引数(unixtime)が必要です")));
	}

	time_t time = 0;
	if ( args[0]->IsInt32() )
	{
		time = args[0]->ToInt32()->Value();
	}
	else if ( args[0]->IsString() )
	{
		time = ToStringToI(args[0]->ToString());
	}
	else if ( args[0]->IsNumber() )
	{
		time = args[0]->ToInteger()->Value();
	}
	else
	{
		return v8::ThrowException(v8::String::New(_A2U("time_is_holidayの第一引数が数字(unixtime)ではありません")));
	}
		
	bool r = XLStringUtil::is_japan_holiday(time);
	return v8::Boolean::New(r);
}

v8::Handle<v8::Value> ScriptRunner::v8_time_day_info(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:time_day_info" );
	ScriptRunner* _this = g_ScriptRunner_This;

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("time_day_infoには1つ以上の引数(unixtime)が必要です")));
	}

	time_t time = 0;
	if ( args[0]->IsInt32() )
	{
		time = args[0]->ToInt32()->Value();
	}
	else if ( args[0]->IsString() )
	{
		time = ToStringToI(args[0]->ToString());
	}
	else if ( args[0]->IsNumber() )
	{
		time = args[0]->ToInteger()->Value();
	}
	else
	{
		return v8::ThrowException(v8::String::New(_A2U("time_day_infoの第一引数が数字(unixtime)ではありません")));
	}

#if _MSC_VER
	struct tm *dateTM = localtime(&time);
#else
	struct tm datetmp;
	struct tm *dateTM = &datetmp;
	localtime_r(&time,&datetmp);
#endif
	string holidayName;
	const bool isHoliday = XLStringUtil::is_japan_holiday(time,&holidayName);
	const string _monthToJapanese = XLStringUtil::MonthToJapanese(dateTM->tm_mon);
	const string _dayofweekToJapanese = XLStringUtil::DayofweekToJapanese(dateTM->tm_wday);
	v8::Local<v8::Object> objectV8 = v8::Object::New();
	objectV8->Set( v8::String::New( "year" ) , v8::Integer::New( dateTM->tm_year + 1900) );
	objectV8->Set( v8::String::New( "month" ) , v8::String::New( _A2U(_monthToJapanese.c_str() )  ) );
	objectV8->Set( v8::String::New( "day" ) , v8::Integer::New( dateTM->tm_mday) );
	objectV8->Set( v8::String::New( "weekofday" ) , v8::String::New( _A2U(_dayofweekToJapanese.c_str() )  ) );
	objectV8->Set( v8::String::New( "is_holiday" ) , v8::Boolean::New( isHoliday ) );
	objectV8->Set( v8::String::New( "holiday_name" ) , v8::String::New( _A2U(holidayName.c_str() ) ) );
//	objectV8->Set( v8::String::New( "gomi" ) , v8::Integer::New( _A2U("予約されています。(todo:今日は燃えるゴミの日ですとか出したいねえ)") ) );

	return objectV8;
}


v8::Handle<v8::Value> ScriptRunner::v8_action(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:action"  );
	ScriptRunner* _this = g_ScriptRunner_This;

	if (args.Length() < 2)
	{
		return v8::ThrowException(v8::String::New(_A2U("actionには2つ以上の引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("actionの第一引数が文字列ではありません")));
	}
	if (! args[1]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("actionの第二引数が文字列ではありません")));
	}

	string p1 = ToStdString(args[0]->ToString());
	string p2 = ToStdString(args[1]->ToString());

	CallbackPP callback ;
	if (args.Length() >= 3)
	{//コールバックあり
		if (!_this->IsScenario)
		{
			return v8::ThrowException(v8::String::New(_A2U("actionのコールバックは会話シナリオだけしか使えません。")));
		}
		if (! args[2]->IsFunction() )
		{
			return v8::ThrowException(v8::String::New(_A2U("actionの第三引数がコールバックではありません")));
		}
		auto callbackFunc = v8::Persistent<v8::Function>::New( args[2].As<v8::Function>() );
		callback = _this->NewCallback([=](){
			_this->callbackFunction(callbackFunc);
		});
	}

	try
	{
		MainWindow::m()->ScriptManager.FireElec(callback ,p1,p2);
	}
	catch(XLException &e)
	{
		const string ee = std::string("action/") + e.what();
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}

	return v8::Boolean::New(true);
}

v8::Handle<v8::Value> ScriptRunner::v8_address_parse(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:address_parse"  );
	ScriptRunner* _this = g_ScriptRunner_This;

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("address_parseには1つ以上の引数が必要です")));
	}

	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("address_parseの第一引数が文字列ではありません")));
	}

	string p1 = ToStdString(args[0]->ToString());

	XLGISAddress::GISResult gisResult;
	if (! MainWindow::m()->GISAddress.Parse(p1,&gisResult) )
	{
		return v8::Boolean::New(false);
	}

	v8::Local<v8::Object> objectV8 = v8::Object::New();

	objectV8->Set( v8::String::New("src") , v8::String::New( p1.c_str() ) );
	objectV8->Set( v8::String::New("lat") , v8::String::New( num2str(gisResult.Lat).c_str() ) );
	objectV8->Set( v8::String::New("lon") , v8::String::New( num2str(gisResult.Lon).c_str() ) );
	objectV8->Set( v8::String::New("pref"), v8::String::New( gisResult.Pref.c_str() ));
	objectV8->Set( v8::String::New("country"), v8::String::New( gisResult.Country.c_str() ));
	return objectV8;
}



v8::Handle<v8::Value> ScriptRunner::v8_weather_info(const v8::Arguments& args) 
{
	ScriptRunner* _this = g_ScriptRunner_This;

	v8::Handle<v8::Value> obj = 
		_this->SimpleAPICaller("weather_info","weather_script_command","weather_openweather.js",args);
	if ( IsbadObject(obj) )
	{
		return obj;
	}
	return obj;
}

v8::Handle<v8::Value> ScriptRunner::v8_wearher_is_rain(const v8::Arguments& args) 
{
	
	ScriptRunner* _this = g_ScriptRunner_This;

	v8::Handle<v8::Value> obj = 
		_this->SimpleAPICaller("wearher_is_rain","weather_script_command","weather_openweather.js",args);
	if ( IsbadObject(obj) )
	{
		return obj;
	}

	auto jsonMap = ToStdStringMap(obj);
	const string weather =  jsonMap["weather"];
	if ( (weather == "rain" || weather == "snow") )
	{
		return v8::Boolean::New(true);
	}
	return v8::Boolean::New(false);
}


v8::Handle<v8::Value> ScriptRunner::v8_wearher_is_rain_soon(const v8::Arguments& args) 
{
	
	ScriptRunner* _this = g_ScriptRunner_This;

	v8::Handle<v8::Value> obj = 
		_this->SimpleAPICaller("wearher_is_rain_soon","weather_script_command","weather_openweather.js",args);
	if ( IsbadObject(obj) )
	{
		return obj;
	}

	auto jsonMap = ToStdStringMap(obj);
	const string weather =  jsonMap["weather"];
	if ( (weather == "rain" || weather == "snow") )
	{
		return v8::Boolean::New(true);
	}
	const string next =  jsonMap["next"];
	if ( (next == "rain" || next == "snow") )
	{
		return v8::Boolean::New(true);
	}
	return v8::Boolean::New(false);
}


v8::Handle<v8::Value> ScriptRunner::v8_train_info(const v8::Arguments& args) 
{
	
	ScriptRunner* _this = g_ScriptRunner_This;

	v8::Handle<v8::Value> obj = 
		_this->SimpleAPICaller("train_info","train_script_command","train_tetsudocom.js",args);
	return obj;
}
v8::Handle<v8::Value> ScriptRunner::v8_train_is_delay(const v8::Arguments& args) 
{
	
	ScriptRunner* _this = g_ScriptRunner_This;

	v8::Handle<v8::Value> obj = 
		_this->SimpleAPICaller("train_is_delay","train_script_command","train_tetsudocom.js",args);
	if ( IsbadObject(obj) )
	{
		return obj;
	}

	auto jsonMap = ToStdStringMap(obj);
	const string is_delay =  jsonMap["is_delay"];
	if (stringbool(is_delay) )
	{
		return v8::Boolean::New(true);
	}
	return v8::Boolean::New(false);
}

v8::Handle<v8::Value> ScriptRunner::v8_background_music_play(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:background_music_play"  );
	ScriptRunner* _this = g_ScriptRunner_This;

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("background_music_playには1つ以上の引数が必要です")));
	}

	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("background_music_playの第一引数が文字列ではありません")));
	}

	string p1 = ToStdString(args[0]->ToString());

	bool r =  MainWindow::m()->BackgroundMusic.Play(p1);
	if (!r)
	{
		return v8::Boolean::New(false);
	}
	return v8::Boolean::New(true);
}

v8::Handle<v8::Value> ScriptRunner::v8_background_music_stop(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:background_music_stop"  );
	ScriptRunner* _this = g_ScriptRunner_This;

	MainWindow::m()->BackgroundMusic.Stop();
	return v8::Boolean::New(true);
}


v8::Handle<v8::Value> ScriptRunner::v8_twitter_tweet(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:twitter_tweet"  );
	ScriptRunner* _this = g_ScriptRunner_This;

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("twitter_tweetには1つ以上の引数が必要です")));
	}

	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("twitter_tweetの第一引数が文字列ではありません")));
	}

	string p1 = ToStdString(args[0]->ToString());

	bool r =  ActionImplement::TwitterTweet(_A2U(p1));
	if (!r)
	{
		return v8::Boolean::New(false);
	}
	return v8::Boolean::New(true);
}

v8::Handle<v8::Value> ScriptRunner::v8_twitter_home_timeline(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:twitter_home_timeline"  );
	ScriptRunner* _this = g_ScriptRunner_This;

	string p1;
	if (args.Length() >= 1)
	{
		if ( args[0]->IsString() )
		{
			p1 = ToStdString(args[0]->ToString());
		}
		
	}

	vector< map<string,string>* > timelineMapVec;
	ActionImplement::TwitterHomeTimeLine(p1,&timelineMapVec);

	v8::Local<v8::Array> arrayV8 = v8::Array::New(timelineMapVec.size());
	unsigned countI = 0;
	for(auto it = timelineMapVec.begin() ; it != timelineMapVec.end() ; it++,countI++ )
	{
		v8::Local<v8::Object> objectV8 = v8::Object::New();
		objectV8->Set( v8::String::New( "created_at" ) , v8::String::New( (*it)->operator[]("created_at").c_str() ) );
		objectV8->Set( v8::String::New( "id" ) , v8::String::New( (*it)->operator[]("id").c_str() ) );
		objectV8->Set( v8::String::New( "text" ) , v8::String::New( _A2U((*it)->operator[]("text").c_str()) ) );
		objectV8->Set( v8::String::New( "screen_name" ) , v8::String::New( (*it)->operator[]("screen_name").c_str() ) );

		arrayV8->Set( v8::Uint32::New(countI) , objectV8 );
	}

	return arrayV8;
}
v8::Handle<v8::Value> ScriptRunner::v8_twitter_user_timeline(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:twitter_user_timeline"  );
	ScriptRunner* _this = g_ScriptRunner_This;

	string p1;
	if (args.Length() >= 1)
	{
		if ( args[0]->IsString() )
		{
			p1 = ToStdString(args[0]->ToString());
		}
		
	}

	vector< map<string,string>* > timelineMapVec;
	ActionImplement::TwitterUserTimeLine(p1,&timelineMapVec);

	v8::Local<v8::Array> arrayV8 = v8::Array::New(timelineMapVec.size());
	unsigned countI = 0;
	for(auto it = timelineMapVec.begin() ; it != timelineMapVec.end() ; it++,countI++ )
	{
		v8::Local<v8::Object> objectV8 = v8::Object::New();
		objectV8->Set( v8::String::New( "created_at" ) , v8::String::New( (*it)->operator[]("created_at").c_str() ) );
		objectV8->Set( v8::String::New( "id" ) , v8::String::New( (*it)->operator[]("id").c_str() ) );
		objectV8->Set( v8::String::New( "text" ) , v8::String::New( _A2U((*it)->operator[]("text").c_str()) ) );
		objectV8->Set( v8::String::New( "screen_name" ) , v8::String::New( (*it)->operator[]("screen_name").c_str() ) );

		arrayV8->Set( v8::Uint32::New(countI) , objectV8 );
	}

	return arrayV8;
}

v8::Handle<v8::Value> ScriptRunner::v8_multiroom_list(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:multiroom_list"  );
	ScriptRunner* _this = g_ScriptRunner_This;

	const auto configmap = MainWindow::m()->Config.FindGetsToMap("multiroom_",false);
	const string my_uuid = MainWindow::m()->Httpd.GetUUID();

	struct _multiroom{
		string uuid;
		string name;
		string url;
		string order;
	};
	vector<_multiroom> multirooms;
	for(auto it = configmap.begin(); it != configmap.end(); it++)
	{
		if(it->first.find("multiroom_") != 0)
		{
			continue;
		}
		if( it->first.find("_uuid") == string::npos)
		{
			continue;
		}
		int key=0;
		sscanf(it->first.c_str(),"multiroom_%d_uuid",&key);
		if (key <= 0)
		{
			continue;
		}

		const string prefix = "multiroom_" + num2str(key) ;
		_multiroom m;
		m.uuid = mapfind(configmap,prefix + "_uuid");
		m.name = mapfind(configmap,prefix + "_name");
		m.url = mapfind(configmap,prefix + "_url");
		m.order = mapfind(configmap,prefix + "_order");
		multirooms.push_back(m);
	}

	v8::Local<v8::Array> arrayV8 = v8::Array::New(multirooms.size());
	unsigned countI = 0;
	for(auto it = multirooms.begin(); it != multirooms.end(); it++)
	{
		v8::Local<v8::Object> objectV8 = v8::Object::New();
		objectV8->Set( v8::String::New( "uuid" ) , v8::String::New( it->uuid.c_str() ) );
		objectV8->Set( v8::String::New( "name" ) , v8::String::New( it->name.c_str() ) );
		objectV8->Set( v8::String::New( "url"  ) , v8::String::New( it->url.c_str()  ) );
		objectV8->Set( v8::String::New( "order") , v8::String::New( it->order.c_str()) );

		arrayV8->Set( v8::Uint32::New(countI) , objectV8 );
		countI++;
	}

	return arrayV8;
}
v8::Handle<v8::Value> ScriptRunner::v8_multiroom_fire(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:multiroom_fire"  );
	ScriptRunner* _this = g_ScriptRunner_This;

	if (args.Length() < 3)
	{
		const string ee = "multiroom_fireには3つ以上の引数が必要です";
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}

	if (! args[0]->IsString() )
	{
		const string ee = "multiroom_fireの第一引数が文字列ではありません";
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}
	if (! args[1]->IsString() )
	{
		const string ee = "multiroom_fireの第二引数が文字列ではありません";
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}
	if (! args[2]->IsString() )
	{
		const string ee = "multiroom_fireの第三引数が文字列ではありません";
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}

	const string p1 = ToStdString(args[0]->ToString());
	const string p2 = ToStdString(args[1]->ToString());
	const string p3 = ToStdString(args[2]->ToString());

	const auto configmap = MainWindow::m()->Config.FindGetsToMap("multiroom_",false);
	const int key = MultiRoomUtil::FindRoom(p1,configmap);
	if(key < 0 )
	{
		const string ee = "multiroom_fire 指定された部屋(" + p1 + ")が見つかりません";
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}
	const string your_authkey = MainWindow::m()->Config.Get("multiroom_"+num2str(key)+"_authkey", "" );
	const string your_url = MainWindow::m()->Config.Get("multiroom_"+num2str(key)+"_url", "" );

	const string get = 
	"?webapi_apikey=" + XLStringUtil::urlencode(your_authkey)
	+ "&elec=" + XLStringUtil::urlencode(_A2U(p2))
	+ "&action=" + XLStringUtil::urlencode(_A2U(p3))
	;

	const string url = XLStringUtil::urlcombine(your_url , "/api/elec/action");
	string your_result;
	try
	{
		map<string,string> header;
		your_result = ActionImplement::HttpGet(url + get,header,3);
	}
	catch(XLException &e)
	{
		const string ee = std::string("multiroom_fire/") + e.what();
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}

	const string result = XLStringUtil::findjsonvalue(your_result,"result");
	if ( XLStringUtil::strtolower(result) != "ok")
	{
		const string ee = "multiroom_fire/can not fire. message:" + result;
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}

	return v8::Boolean::New(true);
}


v8::Handle<v8::Value> ScriptRunner::v8_callcommand(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:callcommand"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("callcommandには1つ以上の引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("callcommandの第一引数が文字列ではありません")));
	}

	string p1 = ToStdString(args[0]->ToString());
	list<string> commandArgs;
	for( int i = 1 ; i < args.Length() ; i ++)
	{
		if (args[i]->IsNumber())
		{
			commandArgs.push_back( num2str(args[i]->ToInteger()->Value())  );
		}
		else if (args[i]->IsString())
		{
			commandArgs.push_back(ToStdString(args[i]->ToString())  );
		}
		else
		{
			const string str = string("callcommandの第") + num2str(i+1) + "引数が文字列か数字ではありません";
			return v8::ThrowException(v8::String::New(_A2U(str.c_str() )));
		}
	}

	//コマンドファイルが実在するかチェック
	if ( !MainWindow::m()->ScriptManager.ExistCommand(p1) )
	{
		const string str = string("コマンド ") + p1 + "は、存在しません。";
		return v8::ThrowException(v8::String::New(_A2U(str.c_str() )));
	}

	//実行
	try
	{
		MainWindow::m()->ScriptManager.FireCommand( CallbackPP::NoCallback() ,p1,commandArgs);
	}
	catch(XLException &e)
	{
		const string ee = std::string("callcommand/") + e.what();
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}

	return v8::Boolean::New(true);
}


v8::Handle<v8::Value> ScriptRunner::v8_speak(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:speak"  );

	ScriptRunner* _this = g_ScriptRunner_This;
	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("speakには1つ以上の引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("speakの第一引数が文字列ではありません")));
	}

	string p1 = ToStdString(args[0]->ToString());
	string speakmodel ;

	CallbackPP callback ;
	if (args.Length() == 2 )
	{
		if ( args[1]->IsFunction() )
		{
			auto callbackFunc = v8::Persistent<v8::Function>::New( args[1].As<v8::Function>() );
			callback = _this->NewCallback([=](){
				_this->callbackFunction(callbackFunc);
			});
		}
		else if ( args[1]->IsString() )
		{
			speakmodel = ToStdString(args[1]->ToString());
		}
		else
		{
			return v8::ThrowException(v8::String::New(_A2U("speakの第二引数が文字列(speakmodel)ではありません")));
		}
	}
	else if (args.Length() >= 3 )
	{
		if ( args[1]->IsString() )
		{
			speakmodel = ToStdString(args[1]->ToString());
		}
		else
		{
			return v8::ThrowException(v8::String::New(_A2U("speakの第二引数が文字列(speakmodel)ではありません")));
		}

		if ( args[2]->IsFunction() )
		{
			auto callbackFunc = v8::Persistent<v8::Function>::New( args[1].As<v8::Function>() );
			callback = _this->NewCallback([=](){
				_this->callbackFunction(callbackFunc);
			});
		}
		else
		{
			return v8::ThrowException(v8::String::New(_A2U("speakの第二引数がコールバックではありません")));
		}
	}

	try
	{
		MainWindow::m()->Speak.SpeakAsync(callback,p1);
	}
	catch(XLException &e)
	{
		const string ee = std::string("speak/") + e.what();
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}

	return v8::Boolean::New(true);

}
v8::Handle<v8::Value> ScriptRunner::v8_speak_cache(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:speak_cache"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("speak_cacheには1つ以上の引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("speak_cacheの第一引数が文字列ではありません")));
	}

	string p1 = ToStdString(args[0]->ToString());
	string speakmodel ;

	if (args.Length() >= 2 )
	{
		if ( args[1]->IsString() )
		{
			speakmodel = ToStdString(args[1]->ToString());
		}
		else
		{
			return v8::ThrowException(v8::String::New(_A2U("speak_cacheの第二引数が文字列(speakmodel)ではありません")));
		}
	}

	try
	{
		MainWindow::m()->Speak.SpeakSync(p1,true); //直接の再生はしない
	}
	catch(XLException &e)
	{
		const string ee = std::string("speak_cache/") + e.what();
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}

	return v8::Boolean::New(true);
}


v8::Handle<v8::Value> ScriptRunner::v8_play(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:play"  );
	ScriptRunner* _this = g_ScriptRunner_This;

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("playには1つ以上の引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("playの第一引数が文字列ではありません")));
	}

	string p1 = ToStdString(args[0]->ToString());
	if (! MainWindow::m()->MusicPlayAsync.IsExist(p1))
	{
		if ( MainWindow::m()->MusicPlayAsync.IsExist(p1 + ".mp3"))
		{//拡張子補完してあるならそれで
			p1 = p1 + ".mp3";
		}
		else if ( MainWindow::m()->MusicPlayAsync.IsExist(p1 + ".wav"))
		{//拡張子補完してあるならそれで
			p1 = p1 + ".wav";
		}
		else
		{//ないです。
			const string err = "playするファイル " + p1  +"は、存在しません";
			return v8::ThrowException(v8::String::New(_A2U(err.c_str())));
		}
	}

	CallbackPP callback;
	if (args.Length() >= 2 )
	{
		if (! args[1]->IsFunction() )
		{
			return v8::ThrowException(v8::String::New(_A2U("playの第二引数がコールバックではありません")));
		}
		
		auto callbackFunc = v8::Persistent<v8::Function>::New( args[1].As<v8::Function>() );
		callback = _this->NewCallback([=](){
			_this->callbackFunction(callbackFunc);
		});
	}

	try
	{
		MainWindow::m()->MusicPlayAsync.Play(callback ,p1 , 1);
	}
	catch(XLException &e)
	{
		const string ee = std::string("play/") + e.what();
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}

	return v8::Boolean::New(true);

}

v8::Handle<v8::Value> ScriptRunner::v8_kernel_config_set(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:v8_kernel_config_set"  );

	if (args.Length() < 2)
	{
		return v8::ThrowException(v8::String::New(_A2U("v8_kernel_config_setには2つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("v8_kernel_config_setの第一引数が文字列ではありません")));
	}
	if (! args[1]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("v8_kernel_config_setの第二引数が文字列ではありません")));
	}

	string p1 = ToStdString(args[0]->ToString());
	string p2 = ToStdString(args[1]->ToString());

	MainWindow::m()->Config.Set(p1,p2);
	return v8::Boolean::New(true);

}

v8::Handle<v8::Value> ScriptRunner::v8_kernel_config_get(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:v8_kernel_config_get"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("v8_kernel_config_getには1つ以上の引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("v8_kernel_config_getの第一引数が文字列ではありません")));
	}

	string p1 = ToStdString(args[0]->ToString());
	string p2;
	if (args.Length() >= 2)
	{//ディフォルト値
		if (! args[1]->IsString() )
		{
			return v8::ThrowException(v8::String::New(_A2U("v8_kernel_config_getの第二引数が文字列ではありません")));
		}
		p2 = ToStdString(args[1]->ToString());
	}

	string r = MainWindow::m()->Config.Get(p1,p2);
	return v8::String::New(_A2U(r.c_str() ));
}



v8::Handle<v8::Value> ScriptRunner::v8_config_set(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:v8_config_set"  );

	if (args.Length() < 2)
	{
		return v8::ThrowException(v8::String::New(_A2U("v8_config_setには2つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("v8_config_setの第一引数が文字列ではありません")));
	}
	if (! args[1]->IsString() )
	{
		if (! args[1]->IsNumber() )
		{
			return v8::ThrowException(v8::String::New(_A2U("v8_config_setの第二引数が文字列ではありません")));
		}
	}

	string p1 = ToStdString(args[0]->ToString());
	string p2 = ToStdString(args[1]->ToString());

	MainWindow::m()->Config.Set(USERPREFIX_JAIL+p1,p2);	//user prefix で jailします.
	return v8::Boolean::New(true);

}

v8::Handle<v8::Value> ScriptRunner::v8_config_get(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:v8_config_get"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("v8_config_getには1つ以上の引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("v8_config_getの第一引数が文字列ではありません")));
	}

	string p1 = ToStdString(args[0]->ToString());
	string p2;
	if (args.Length() >= 2)
	{//ディフォルト値
		if (! args[1]->IsString() )
		{
			if (! args[1]->IsNumber() )
			{
				return v8::ThrowException(v8::String::New(_A2U("v8_config_getの第二引数が文字列ではありません")));
			}
		}
		p2 = ToStdString(args[1]->ToString());
	}

	string r = MainWindow::m()->Config.Get(USERPREFIX_JAIL+p1,p2);	//user prefix で jailします.
	return v8::String::New(_A2U(r.c_str() ));
}


v8::Handle<v8::Value> ScriptRunner::v8_telnet(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:telnet"  );

	if (args.Length() < 4)
	{
		return v8::ThrowException(v8::String::New(_A2U("telnetには4つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("telnetの第一引数が文字列ではありません")));
	}
	if (! args[1]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("telnetの第二引数が文字列ではありません")));
	}
	if (! args[2]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("telnetの第三引数が文字列ではありません")));
	}
	if (! args[3]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("telnetの第四引数が文字列ではありません")));
	}

	string p1 = ToStdString(args[0]->ToString());
	string p2 = ToStdString(args[1]->ToString());
	string p3 = ToStdString(args[2]->ToString());
	string p4 = ToStdString(args[3]->ToString());

	string	r;
	try
	{
		r = ActionImplement::Telnet(p1 , p2,p3,p4);
	}
	catch(XLException &e)
	{
		const string ee = std::string("telnet/") + e.what();
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}
	return v8::String::New(_A2U(r.c_str()));
}

v8::Handle<v8::Value> ScriptRunner::v8_http_get(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:http_get"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("http_getには1つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("http_getの第一引数が文字列ではありません")));
	}

	string url = ToStdString(args[0]->ToString());
	map<string,string> header;
	unsigned int timeoutMS = 5000;
	string convertEncoding = "";

	if (args.Length() >= 2)
	{//ヘッダー
		if ( args[1]->IsUndefined() || args[1]->IsNull() )
		{
			// nop
		}
		else
		{
			if (! args[1]->IsObject() )
			{
				return v8::ThrowException(v8::String::New(_A2U("http_getの第二引数が連想配列(ヘッダー)ではありません")));
			}
			else
			{
				header = ToStdStringMap( args[1] );
			}
		}
	}
	if (args.Length() >= 3)
	{//タイムアウト(MSで受けとつけるが、実際はSになる)
		if ( args[2]->IsInt32() )
		{
			timeoutMS = args[2]->ToInt32()->Value();
		}
		else if ( args[2]->IsString() )
		{
			timeoutMS = ToStringToI(args[2]->ToString());
		}
		else if ( args[2]->IsUndefined() || args[2]->IsNull() )
		{
			// nop
		}
		else
		{
			return v8::ThrowException(v8::String::New(_A2U("http_getの第三引数が数字(タイムアウト)ではありません")));
		}
	}
	if (args.Length() >= 4)
	{//エンコード
		if ( args[3]->IsString() )
		{
			convertEncoding = ToStringToI(args[3]->ToString());
		}
		else if ( args[3]->IsUndefined() || args[3]->IsNull() )
		{
			// nop
		}
		else
		{
			return v8::ThrowException(v8::String::New(_A2U("http_getの第四引数が文字列(エンコード)ではありません")));
		}
	}

	string	r;
	try
	{
		r = ActionImplement::HttpGet(url,header,timeoutMS / 1000);

		if (!(convertEncoding.empty() || STRICMP(convertEncoding.c_str() ,"pass") == 0))
		{
			r = XLStringUtil::mb_convert_encoding(r ,"UTF-8",convertEncoding );
		}
	}
	catch(XLException &e)
	{
		const string ee = std::string("http_get/") + e.what();
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}
	return v8::String::New(r.c_str());
}

v8::Handle<v8::Value> ScriptRunner::v8_http_post(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:http_post"  );

	if (args.Length() < 2)
	{
		return v8::ThrowException(v8::String::New(_A2U("http_postには2つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("http_postの第一引数が文字列ではありません")));
	}

	string url = ToStdString(args[0]->ToString());
	string postdata;
	if ( args[1]->IsString() )
	{//postするデータ(文字列)
		postdata = ToStdString(args[1]->ToString());
	}
	else if ( args[1]->IsUndefined() || args[1]->IsNull() )
	{
		// nop
	}
	else if ( args[1]->IsObject() )
	{//postするデータ連想配列
		const auto postdataMap = ToStdStringMap( args[1] );
		for(auto it = postdataMap.begin() ; it != postdataMap.end() ; ++it )
		{
			postdata += "&" + XLStringUtil::urlencode(it->first) + "=" + XLStringUtil::urlencode(it->second) ;
		}
		if (!postdata.empty())
		{
			postdata = postdata.substr(1);
		}
	}
	else
	{
		return v8::ThrowException(v8::String::New(_A2U("http_postの第二引数が文字列か連想配列(postデータ)ではありません")));
	}

	map<string,string> header;
	unsigned int timeoutMS = 5000;
	string convertEncoding;
	if (args.Length() >= 3)
	{//ヘッダー
		if ( args[2]->IsUndefined() || args[2]->IsNull() )
		{
			// nop
		}
		else
		{
			if (! args[2]->IsObject() )
			{
				return v8::ThrowException(v8::String::New(_A2U("http_postの第三引数が連想配列(ヘッダー)ではありません")));
			}

			header = ToStdStringMap( args[2] );
		}
	}
	if (args.Length() >= 4)
	{//タイムアウト(MSで受けとつけるが、実際はSになる)
		if ( args[3]->IsInt32() )
		{
			timeoutMS = args[3]->ToInt32()->Value();
		}
		else if ( args[3]->IsString() )
		{
			timeoutMS = ToStringToI(args[3]->ToString());
		}
		else if ( args[3]->IsUndefined() || args[3]->IsNull() )
		{
			// nop
		}
		else
		{
			return v8::ThrowException(v8::String::New(_A2U("http_postの第四引数が数字(タイムアウト)ではありません")));
		}
	}
	if (args.Length() >= 5 )
	{//エンコード
		if ( args[4]->IsString() )
		{
			convertEncoding = ToStringToI(args[4]->ToString());
		}
		else if ( args[4]->IsUndefined() || args[4]->IsNull() )
		{
			// nop
		}
		else
		{
			return v8::ThrowException(v8::String::New(_A2U("http_postの第五引数が文字列(エンコード)ではありません")));
		}
	}

	string	r;
	try
	{
		r = ActionImplement::HttpPost(url,postdata,header,timeoutMS / 1000);

		if (!(convertEncoding.empty() || STRICMP(convertEncoding.c_str() ,"pass") == 0))
		{
			r = XLStringUtil::mb_convert_encoding(r ,"UTF-8",convertEncoding );
		}
	}
	catch(XLException &e)
	{
		const string ee = std::string("http_post/") + e.what();
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}
	return v8::String::New(r.c_str());
}

v8::Handle<v8::Value> ScriptRunner::v8_kernel_execute_local(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:kernel_execute_local"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("kernel_execute_localには1つ以上の引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("kernel_execute_localの第一引数が文字列ではありません")));
	}

	string p1 = ToStdString(args[0]->ToString());

	string	r;
	try
	{
		r = ActionImplement::ExecuteLocal(p1);
	}
	catch(XLException &e)
	{
		const string ee = std::string("kernel_execute_local/") + e.what();
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}
	return v8::String::New(_A2U(r.c_str()));
}

v8::Handle<v8::Value> ScriptRunner::v8_kernel_process_list(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:kernel_process_list"  );

	vector<SystemMisc::ProcessSt> mapper = SystemMisc::ProcessList();
	v8::Local<v8::Array> arrayV8 = v8::Array::New(mapper.size());
	unsigned countI = 0;
	for (auto it = mapper.begin() ; it != mapper.end() ; ++it ,++countI ) 
	{
		v8::Local<v8::Object> objectV8 = v8::Object::New();
		objectV8->Set( v8::String::New( "pid" ) , v8::Integer::New( it->pid ) );
		objectV8->Set( v8::String::New( "parentpid" ) , v8::Integer::New( it->parentpid ) );
		objectV8->Set( v8::String::New( "processname" ) , v8::String::New( _A2U(it->processname.c_str()) ) );
		objectV8->Set( v8::String::New( "args" ) , v8::String::New( _A2U(it->args.c_str()) ) );
		objectV8->Set( v8::String::New( "extraname" ) , v8::String::New( _A2U(it->extraname.c_str()) ) );

		arrayV8->Set( v8::Uint32::New(countI) , objectV8 );
	}

	return arrayV8;
}


v8::Handle<v8::Value> ScriptRunner::v8_kernel_kill(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:kernel_kill"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("kernel_killには1つ以上の引数が必要です")));
	}

	unsigned int p1 = 0;
	if ( args[3]->IsInt32() )
	{
		p1 = args[3]->ToInt32()->Value();
	}
	else if ( args[3]->IsString() )
	{
		p1 = ToStringToI(args[3]->ToString());
	}
	else
	{
		return v8::ThrowException(v8::String::New(_A2U("kernel_killの第一引数が数字(プロセスID)ではありません")));
	}

	bool r;
	try
	{
		r =  SystemMisc::KillProces(p1);
	}
	catch(XLException &e)
	{
		const string ee = std::string("kernel_kill/") + e.what();
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}
	return v8::Boolean::New(r);
}


v8::Handle<v8::Value> ScriptRunner::v8_execute_remote(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:execute_remote"  );

	if (args.Length() < 3)
	{
		return v8::ThrowException(v8::String::New(_A2U("execute_remoteには3つ以上の引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("execute_remoteの第一引数が文字列ではありません")));
	}
	if (! args[1]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("execute_remoteの第ニ引数が文字列ではありません")));
	}
	if (! args[2]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("execute_remoteの第三引数が文字列ではありません")));
	}

	string p1 = ToStdString(args[0]->ToString());
	string p2 = ToStdString(args[1]->ToString());
	string p3 = ToStdString(args[2]->ToString());
	string p4 ;
	if (args.Length() >= 4)
	{
		p4 = ToStdString(args[3]->ToString());
	}

	string	r;
	try
	{
		r = ActionImplement::ExecuteRemote(p1,p2,p3,p4);
	}
	catch(XLException &e)
	{
		const string ee = std::string("execute_remote/") + e.what();
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}
	return v8::String::New(_A2U(r.c_str()));
}

v8::Handle<v8::Value> ScriptRunner::v8_msleep(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:msleep"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("msleepには1つの引数が必要です")));
	}
	int sleepMS = 0;
	if ( args[0]->IsInt32() )
	{
		sleepMS = args[0]->ToInt32()->Value();
	}
	else if ( args[0]->IsString() )
	{
		sleepMS = ToStringToI(args[0]->ToString());
	}
	else
	{
		return v8::ThrowException(v8::String::New(_A2U("msleepの第一引数が数字ではありません")));
	}

	ActionImplement::MSleep("" , sleepMS);

	return v8::Boolean::New(true);

}


v8::Handle<v8::Value> ScriptRunner::v8_dump(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:dump"  );

	string out;
	bool first = true;
	for (int i = 0; i < args.Length(); i++) 
	{
		v8::HandleScope handle_scope;
		if (first) 
		{
			first = false;
		}
		else
		{
			out += " ";
			printf(" ");
		}
		out += V8ValueDump(args[i]);
	}
	g_STDOUT += out + "\r\n";
	NOTIFYLOG( out );
	return v8::Undefined();
}

string ScriptRunner::V8ValueDump(const v8::Local<v8::Value>& val)
{
	if ( val->IsString() )
	{
//		return "\""+ToStdString(val->ToString()) + "\"";
		return ToStdString(val->ToString()) ;
	}
	if ( val->IsNumber() )
	{
		return ToStdString(val->ToString());
	}
	if ( val->IsArray() )
	{
		string r;
		v8::Handle<v8::Array> arrayV8 = v8::Handle<v8::Array>::Cast(val);
		for(unsigned int i = 0 ; i < arrayV8->Length() ; ++i )
		{
			r += "," + V8ValueDump( arrayV8->Get(i) );
		}
		if ( !r.empty() )
		{
			r = r.substr(1);
		}
		return "[" + r + "]";
	}
	if ( val->IsObject() )
	{
		string r;
		v8::Handle<v8::Object> object = v8::Handle<v8::Object>::Cast(val);

		//一度キーを取得する
		v8::Local<v8::Array> propertyNames = object->GetPropertyNames();
		//キーを順繰りに見ながら値をとっていく
		for(unsigned int i = 0 ; i < propertyNames->Length() ; ++i )
		{
			r += 
				  string(",\"") + ToStdString(propertyNames->Get(i)->ToString()) + "\":" //key
				+ V8ValueDump(object->Get(propertyNames->Get(i)) )                            //value
				;
		}
		if ( !r.empty() )
		{
			r = r.substr(1);
		}
		return "{" + r + "}";
	}
	if ( val->IsBoolean() )
	{
		return ToStdString(val->ToString());
	}
	if ( val->IsUndefined() )
	{
		return "undefined";
	}
	if ( val->IsNull() )
	{
		return "null";
	}
	if ( val->IsFunction() )
	{
		return "\"<function>\"";
	}
	return "\"<nazo>\"";
}

v8::Handle<v8::Value> ScriptRunner::v8_sensor_lumi(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:sensor_lumi"  );

	//温度と明るさのセンサーの値を更新する
	float temp,lumi,sound;
	MainWindow::m()->Sensor.getSensorNow(&temp,&lumi,&sound);

	return v8::Number::New(lumi);
}

v8::Handle<v8::Value> ScriptRunner::v8_sensor_temp(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:sensor_temp" );

	//温度と明るさのセンサーの値を更新する
	float temp,lumi,sound;
	MainWindow::m()->Sensor.getSensorNow(&temp,&lumi,&sound);

	return v8::Number::New(temp);
}

v8::Handle<v8::Value> ScriptRunner::v8_sensor_sound(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:sensor_sound" );

	//温度と明るさのセンサーの値を更新する
	float temp,lumi,sound;
	MainWindow::m()->Sensor.getSensorNow(&temp,&lumi,&sound);

	return v8::Number::New(sound);
}

v8::Handle<v8::Value> ScriptRunner::v8_config_find(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:config_find" );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("config_findには1つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("config_findの第一引数が数字ではありません")));
	}

	string p1 = ToStdString(args[0]->ToString());


	auto configmap = MainWindow::m()->Config.FindGetsToMap(USERPREFIX_JAIL+p1,true); //user prefix で jail する.
	v8::Local<v8::Array> arrayV8 = v8::Array::New(configmap.size());
	for (auto it = configmap.begin() ; it != configmap.end() ; ++it ) 
	{
		arrayV8->Set( v8::String::New(it->first.c_str() + USERPREFIX_JAIL.size() ) , v8::String::New(it->second.c_str() ) );
	}

	return arrayV8;
}
v8::Handle<v8::Value> ScriptRunner::v8_kernel_config_find(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:config_find" );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("config_findには1つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("config_findの第一引数が数字ではありません")));
	}

	string p1 = ToStdString(args[0]->ToString());


	auto configmap = MainWindow::m()->Config.FindGetsToMap(p1,true);
	v8::Local<v8::Array> arrayV8 = v8::Array::New(configmap.size());
	for (auto it = configmap.begin() ; it != configmap.end() ; ++it ) 
	{
		arrayV8->Set( v8::String::New(it->first.c_str() ) , v8::String::New(it->second.c_str() ) );
	}

	return arrayV8;
}

#if _WITH_USB
v8::Handle<v8::Value> ScriptRunner::v8_usb_open(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:usb_open"  );
	ScriptRunner* _this = g_ScriptRunner_This;

	if (args.Length() < 2)
	{
		return v8::ThrowException(v8::String::New(_A2U("usb_openには2つの引数が必要です")));
	}
	if (! args[0]->IsUint32() )
	{
		return v8::ThrowException(v8::String::New(_A2U("usb_openの第一引数が数字ではありません")));
	}
	if (! args[1]->IsUint32() )
	{
		return v8::ThrowException(v8::String::New(_A2U("usb_openの第二引数が数字ではありません")));
	}

	unsigned int p1 = args[0]->ToUint32()->Value();
	unsigned int p2 = args[1]->ToUint32()->Value();
	unsigned int fd = 0;

#if _WITH_USB
	XLUSBHIDDevice* usb = new XLUSBHIDDevice();
	try
	{
		usb->Open(p1,p2);
		fd = _this->UsbDeviceList.size();
		_this->UsbDeviceList.push_back(usb);
	}
	catch(XLException& e )
	{
		const string ee = "usb_open/" << e.what();
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}
#endif //_WITH_USB

	return v8::Uint32::New(fd);
}

v8::Handle<v8::Value> ScriptRunner::v8_usb_close(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:usb_close"  );
	ScriptRunner* _this = g_ScriptRunner_This;

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("usb_closeには1つの引数が必要です")));
	}
	if (! args[0]->IsUint32() )
	{
		return v8::ThrowException(v8::String::New(_A2U("usb_closeの第一引数が数字ではありません")));
	}

	unsigned int p1 = args[0]->ToUint32()->Value();
	if (p1 >= _this->UsbDeviceList.size())
	{
		return v8::ThrowException(v8::String::New(_A2U("usb_open:指定されたハンドルはありません" )));
	}
	delete _this->UsbDeviceList[p1];
	_this->UsbDeviceList[p1] = NULL;

	return v8::Boolean::New(true);

}

v8::Handle<v8::Value> ScriptRunner::v8_usb_read(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:usb_read" );
	ScriptRunner* _this = g_ScriptRunner_This;

	if (args.Length() < 2)
	{
		return v8::ThrowException(v8::String::New(_A2U("usb_readには2つ以上の引数が必要です")));
	}
	if (! args[0]->IsUint32() )
	{
		return v8::ThrowException(v8::String::New(_A2U("usb_readの第一引数が数字ではありません")));
	}
	if (! args[1]->IsUint32() )
	{
		return v8::ThrowException(v8::String::New(_A2U("usb_readの第二引数が数字ではありません")));
	}

	unsigned int p1 = args[0]->ToUint32()->Value();
	unsigned int p2 = args[1]->ToUint32()->Value();
	unsigned int p3 = 5000;
	if (args.Length() >= 3)
	{
		if (! args[2]->IsUint32() )
		{
			return v8::ThrowException(v8::String::New(_A2U("usb_readの第三引数が数字ではありません")));
		}
		p3 = args[2]->ToUint32()->Value();
	}

	if (p1 >= _this->UsbDeviceList.size())
	{
		return v8::ThrowException(v8::String::New(_A2U("usb_read:指定されたハンドルはありません" )));
	}

	vector<unsigned char> buffer(p2);
	unsigned int r;
	try
	{
		r = _this->UsbDeviceList[p1]->Read( &buffer[0] , p2,p3);
	}
	catch(XLException& e )
	{
		const string ee = "usb_read/" << e.what();
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}


	v8::Local<v8::Array> arrayV8 = v8::Array::New(r);
	for (unsigned int i = 0 ; i < r ; ++i ) 
	{
		arrayV8->Set( v8::Uint32::New(i) , v8::Uint32::New( buffer[i] ) );
	}
	return arrayV8;
}

v8::Handle<v8::Value> ScriptRunner::v8_usb_write(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:usb_write"  );
	ScriptRunner* _this = g_ScriptRunner_This;

	if (args.Length() < 2)
	{
		return v8::ThrowException(v8::String::New(_A2U("usb_writeには2つ以上の引数が必要です")));
	}
	if (! args[0]->IsUint32() )
	{
		return v8::ThrowException(v8::String::New(_A2U("usb_writeの第一引数が数字ではありません")));
	}
	if (! args[1]->IsArray() )
	{
		return v8::ThrowException(v8::String::New(_A2U("usb_writeの第二引数が配列ではありません")));
	}

	unsigned int p1 = args[0]->ToUint32()->Value();
	unsigned int p3 = 5000;
	if (args.Length() >= 3)
	{
		if (! args[2]->IsUint32() )
		{
			return v8::ThrowException(v8::String::New(_A2U("usb_writeの第三引数が数字ではありません")));
		}
		p3 = args[2]->ToUint32()->Value();
	}

	if (p1 >= _this->UsbDeviceList.size())
	{
		return v8::ThrowException(v8::String::New(_A2U("usb_write:指定されたハンドルはありません" )));
	}

	v8::Handle<v8::Array> v8Array = v8::Handle<v8::Array>::Cast(args[1]);

	vector<unsigned char> buffer(v8Array->Length());
	for(unsigned int i = 0; i < v8Array->Length() ; ++i )
	{
		buffer[i] = v8Array->Get(i)->ToUint32()->Value();
	}

	unsigned int r;
	try
	{
		r = _this->UsbDeviceList[p1]->Write( &buffer[0] , buffer.size(),p3);
	}
	catch(XLException& e )
	{
		const string ee = "usb_write/" << e.what();
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}

	return v8::Uint32::New(r);
}

v8::Handle<v8::Value> ScriptRunner::v8_usb_messageread(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:usb_messageread"  );
	ScriptRunner* _this = g_ScriptRunner_This;

	if (args.Length() < 2)
	{
		return v8::ThrowException(v8::String::New(_A2U("usb_messagereadには2つ以上の引数が必要です")));
	}
	if (! args[0]->IsUint32() )
	{
		return v8::ThrowException(v8::String::New(_A2U("usb_messagereadの第一引数が数字ではありません")));
	}
	if (! args[1]->IsUint32() )
	{
		return v8::ThrowException(v8::String::New(_A2U("usb_messagereadの第二引数が数字ではありません")));
	}

	unsigned int p1 = args[0]->ToUint32()->Value();
	unsigned int p2 = args[1]->ToUint32()->Value();

	if (p1 >= _this->UsbDeviceList.size())
	{
		return v8::ThrowException(v8::String::New(_A2U("usb_messageread:指定されたハンドルはありません" )));
	}

	vector<unsigned char> buffer(p2);
	try
	{
		_this->UsbDeviceList[p1]->GetFeature( &buffer[0] , p2);
	}
	catch(XLException& e )
	{
		const string ee = "usb_messageread/" << e.what();
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}


	v8::Local<v8::Array> arrayV8 = v8::Array::New(p2);
	for (unsigned int i = 0 ; i < p2 ; ++i ) 
	{
		arrayV8->Set( v8::Uint32::New(i) , v8::Uint32::New( buffer[i] ) );
	}
	return arrayV8;
}

v8::Handle<v8::Value> ScriptRunner::v8_usb_messagewrite(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:usb_messagewrite" );
	ScriptRunner* _this = g_ScriptRunner_This;

	if (args.Length() < 2)
	{
		return v8::ThrowException(v8::String::New(_A2U("usb_messagewriteには2つ以上の引数が必要です")));
	}
	if (! args[0]->IsUint32() )
	{
		return v8::ThrowException(v8::String::New(_A2U("usb_messagewriteの第一引数が数字ではありません")));
	}
	if (! args[1]->IsArray() )
	{
		return v8::ThrowException(v8::String::New(_A2U("usb_messagewriteの第二引数が配列ではありません")));
	}

	unsigned int p1 = args[0]->ToUint32()->Value();

	if (p1 >= _this->UsbDeviceList.size())
	{
		return v8::ThrowException(v8::String::New(_A2U("usb_messagewrite:指定されたハンドルはありません" )));
	}

	v8::Handle<v8::Array> v8Array = v8::Handle<v8::Array>::Cast(args[1]);

	vector<unsigned char> buffer(v8Array->Length());
	for(unsigned int i = 0; i < v8Array->Length() ; ++i )
	{
		buffer[i] = v8Array->Get(i)->ToUint32()->Value();
	}

	try
	{
		_this->UsbDeviceList[p1]->SetFeature( &buffer[0] , buffer.size());
	}
	catch(XLException& e )
	{
		const string ee = "usb_messagewrite/" << e.what();
		return v8::ThrowException(v8::String::New(_A2U( ee.c_str() )));
	}

	return v8::Boolean::New(true);
}
#endif //_WITH_USB

v8::Handle<v8::Value> ScriptRunner::v8_elec_setstatus(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:elec_setstatus"  );

	if (args.Length() < 2)
	{
		return v8::ThrowException(v8::String::New(_A2U("elec_setstatusには2つ以上の引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("elec_setstatusの第一引数が文字列ではありません")));
	}
	if (! args[1]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("elec_setstatusの第二引数が文字列ではありません")));
	}

	string p1 = ToStdString(args[0]->ToString());
	string p2 = ToStdString(args[1]->ToString());

	const auto configmap = MainWindow::m()->Config.FindGetsToMap("elec_",false);

	int key1 = 0;
	if ( ! ScriptRemoconWeb::type2key(configmap ,p1 ,&key1)  )
	{
		return v8::Boolean::New(false);
	}
	MainWindow::m()->Config.Set("elec_" + num2str(key1) + "_status",p2);
	DEBUGLOG("機材 " << key1 << " のステータスを " << p2 << " に変更します。");

	return v8::Boolean::New(true);
}


v8::Handle<v8::Value> ScriptRunner::v8_elec_getstatus(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:elec_getstatus"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("elec_getstatusには1つ以上の引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("elec_getstatusの第一引数が文字列ではありません")));
	}

	const string p1 = ToStdString(args[0]->ToString());
	const string status = ScriptRemoconWeb::getElecStatus(p1);

	return v8::String::New(_A2U(status.c_str()));
}

//機材一覧の名前を配列で返す
v8::Handle<v8::Value> ScriptRunner::v8_elec_getlist(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:elec_getlist" );

	//数がわからないと配列を作れないのでvectorとかに入れとく
	vector<string> vec;

	bool showall = false;

	const auto configmap = MainWindow::m()->Config.FindGetsToMap("elec_",false);
	const vector<unsigned int> arr = ScriptRemoconWeb::getElecID_Array(configmap,showall);

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

		vec.push_back(typeIT->second);
	}


	v8::Local<v8::Array> arrayV8 = v8::Array::New( vec.size() );
	for (unsigned int i = 0 ; i < vec.size() ; ++i ) 
	{
		arrayV8->Set( v8::Uint32::New(i) , v8::String::New( _A2U(vec[i].c_str() ) ) );
	}
	return arrayV8;
}


//機材のアクション一覧を取得する
v8::Handle<v8::Value> ScriptRunner::v8_elec_getactionlist(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:elec_getactionlist" );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("elec_getactionlistには1つ以上の引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("elec_getactionlistの第一引数が文字列ではありません")));
	}
	//数がわからないと配列を作れないのでvectorとかに入れとく
	vector<string> vec;
	string p1 = ToStdString(args[0]->ToString());

	bool showall = true;

	//ソート済みのIDを取得
	const auto configmap = MainWindow::m()->Config.FindGetsToMap("elec_",false);
	unsigned int elecID = 0;
	const vector<unsigned int> arr = ScriptRemoconWeb::getElecActionID_Array(configmap,p1,&elecID,showall);

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

		vec.push_back(typeIT->second);
	}

	v8::Local<v8::Array> arrayV8 = v8::Array::New( vec.size() );
	for (unsigned int i = 0 ; i < vec.size() ; ++i ) 
	{
		arrayV8->Set( v8::Uint32::New(i) , v8::String::New( _A2U(vec[i].c_str() ) ) );
	}
	return arrayV8;
}

v8::Handle<v8::Value> ScriptRunner::v8_elec_getinfo(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:elec_getinfo" );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("elec_getinfoには1つ以上の引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("elec_getinfoの第一引数が文字列ではありません")));
	}
	//数がわからないと配列を作れないのでvectorとかに入れとく
	vector<string> vec;
	string p1 = ToStdString(args[0]->ToString());

	const auto configmap = MainWindow::m()->Config.FindGetsToMap("elec_",false);
	int key1 = 0;
	if ( ! ScriptRemoconWeb::type2key(configmap ,p1 ,&key1)  )
	{
		return v8::Boolean::New(false);
	}

	//個数がわからないと配列が作れないので、まず個数を数える
	int count = 0;
	const string prefix = "elec_" + num2str(key1) + "_";
	const auto elecInfomap = MainWindow::m()->Config.FindGetsToMap(prefix ,false);
	for(auto it = elecInfomap.begin() ; it != elecInfomap.end() ; ++it )
	{
		if ( strstr( it->first.c_str() , "_action_" ) )
		{
			continue;
		}
		count++;
		
	}
	v8::Local<v8::Object> objectV8 = v8::Object::New();

	//次にV8 array に格納していく
	for(auto it = elecInfomap.begin() ; it != elecInfomap.end() ; ++it )
	{
		if ( strstr( it->first.c_str() , "_action_" ) )
		{
			continue;
		}
		const string k = _A2U(it->first.c_str() + prefix.size() );
		const string v = _A2U(it->second );
		objectV8->Set( v8::String::New( k.c_str() ) , v8::String::New( v.c_str() ) );
	}
	return objectV8;
}


v8::Handle<v8::Value> ScriptRunner::v8_sstp_send11(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:sstp_send11"  );

	if (args.Length() < 2)
	{
		return v8::ThrowException(v8::String::New(_A2U("sstp_send11には2つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("sstp_send11の第一引数が文字列ではありません")));
	}
	if (! args[1]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("sstp_send11の第ニ引数が文字列ではありません")));
	}
	string p1 = ToStdString(args[0]->ToString());
	string p2 = ToStdString(args[1]->ToString());

	ActionImplement::SSTPSend11(p1,p2);
	return v8::Boolean::New(true);
}

v8::Handle<v8::Value> ScriptRunner::v8_soap_action(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:soap_action"  );

	if (args.Length() < 3)
	{
		return v8::ThrowException(v8::String::New(_A2U("soap_actionには3つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("soap_actionの第一引数が文字列(url)ではありません")));
	}
	if (! args[1]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("soap_actionの第二引数が文字列(SOAPACTION)ではありません")));
	}

	string url = ToStdString(args[0]->ToString());
	string soapaction = ToStdString(args[1]->ToString());
	string postdata;
	if ( args[2]->IsString() )
	{//postするデータ(文字列)
		postdata = ToStdString(args[2]->ToString());
	}
	else
	{
		return v8::ThrowException(v8::String::New(_A2U("soap_actionの第三引数が文字列(xmlデータ)ではありません")));
	}

	map<string,string> header;
	unsigned int timeoutMS = 5000;
	if (args.Length() >= 4)
	{//ヘッダー
		if (! args[3]->IsObject() )
		{
			return v8::ThrowException(v8::String::New(_A2U("soap_actionの第四引数が連想配列(ヘッダー)ではありません")));
		}

		header = ToStdStringMap( args[3] );
	}
	if (args.Length() >= 5)
	{//タイムアウト(MSで受けとつけるが、実際はSになる)
		if ( args[4]->IsInt32() )
		{
			timeoutMS = args[4]->ToInt32()->Value();
		}
		else if ( args[4]->IsString() )
		{
			timeoutMS = ToStringToI(args[4]->ToString());
		}
		else
		{
			return v8::ThrowException(v8::String::New(_A2U("soap_actionの第五引数が数字(タイムアウト)ではありません")));
		}
	}

	string	r;
	try
	{
		header["SOAPACTION"] = "\"" + soapaction + "\"";
		header["Content-Type"] = "text/xml; charset=\"utf-8\"";
		r = ActionImplement::HttpPost(url,postdata,header,timeoutMS / 1000);
	}
	catch(XLException &e)
	{
		const string ee = std::string("soap_action/") + e.what();
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}
	return v8::String::New(_A2U(r.c_str()));
}

v8::Handle<v8::Value> ScriptRunner::v8_url_hosturl(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:url_hosturl"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("url_hosturlには1つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("url_hosturlの第一引数が文字列(URL)ではありません")));
	}
	const string url = ToStdString(args[0]->ToString());
	const string retURL = XLStringUtil::hosturl(url);

	return v8::String::New(_A2U(retURL.c_str()));
}

v8::Handle<v8::Value> ScriptRunner::v8_url_baseurl(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:url_baseurl"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("url_baseurlには1つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("url_baseurlの第一引数が文字列(URL)ではありません")));
	}
	const string url = ToStdString(args[0]->ToString());
	const string retURL = XLStringUtil::baseurl(url);

	return v8::String::New(_A2U(retURL.c_str()));
}

v8::Handle<v8::Value> ScriptRunner::v8_url_changeprotocol(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:url_changeprotocol"  );

	if (args.Length() < 2)
	{
		return v8::ThrowException(v8::String::New(_A2U("url_changeprotocolには2つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("url_changeprotocolの第一引数が文字列(URL)ではありません")));
	}
	if (! args[1]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("url_changeprotocolの第ニ引数が文字列(プロトコル)ではありません")));
	}
	const string url = ToStdString(args[0]->ToString());
	const string protocol = ToStdString(args[1]->ToString());
	const string retURL = XLStringUtil::changeprotocol(url,protocol);

	return v8::String::New(_A2U(retURL.c_str()));
}
v8::Handle<v8::Value> ScriptRunner::v8_url_appendparam(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:url_appendparam"  );

	if (args.Length() < 2)
	{
		return v8::ThrowException(v8::String::New(_A2U("url_appendparamには2つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("url_appendparamの第一引数が文字列(URL)ではありません")));
	}
	if (! args[1]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("url_appendparamの第ニ引数が文字列(追加パラメタ)ではありません")));
	}
	const string url = ToStdString(args[0]->ToString());
	const string param = ToStdString(args[1]->ToString());
	const string retURL = XLStringUtil::appendparam(url,param);

	return v8::String::New(_A2U(retURL.c_str()));
}


v8::Handle<v8::Value> ScriptRunner::v8_base64encode(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:base64encode"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("base64encodeには1つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("base64encodeの第一引数が文字列ではありません")));
	}
	const string str = ToStdString(args[0]->ToString());
	const string ret =XLStringUtil::base64encode(str);

	return v8::String::New(_A2U(ret.c_str()));
}

v8::Handle<v8::Value> ScriptRunner::v8_base64decode(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:base64decode"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("base64decodeには1つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("base64decodeの第一引数が文字列ではありません")));
	}
	const string str = ToStdString(args[0]->ToString());
	const string ret =XLStringUtil::base64decode(str);

	return v8::String::New(_A2U(ret.c_str()));
}

v8::Handle<v8::Value> ScriptRunner::v8_urlencode(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:urlencode"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("urlencodeには1つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("urlencodeの第一引数が文字列ではありません")));
	}
	const string str = ToStdString(args[0]->ToString());
	const string ret =XLStringUtil::urlencode(str);

	return v8::String::New(_A2U(ret.c_str()));
}

v8::Handle<v8::Value> ScriptRunner::v8_urldecode(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:urldecode"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("urldecodeには1つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("urldecodeの第一引数が文字列ではありません")));
	}
	const string str = ToStdString(args[0]->ToString());
	const string ret =XLStringUtil::urldecode(str);

	return v8::String::New(_A2U(ret.c_str()));
}

v8::Handle<v8::Value> ScriptRunner::v8_escapeshellarg(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:escapeshellarg"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("escapeshellargには1つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("escapeshellargの第一引数が文字列ではありません")));
	}
	const string str = ToStdString(args[0]->ToString());
	const string ret =XLStringUtil::escapeshellarg(str);

	return v8::String::New(_A2U(ret.c_str()));
}

v8::Handle<v8::Value> ScriptRunner::v8_mb_convert_kana(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:mb_convert_kana"  );

	if (args.Length() < 2)
	{
		return v8::ThrowException(v8::String::New(_A2U("mb_convert_kanaには2つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("mb_convert_kanaの第一引数が文字列ではありません")));
	}
	if (! args[1]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("mb_convert_kanaの第ニ引数が文字列(オプション文字列)ではありません")));
	}
	const string str = ToStdString(args[0]->ToString());
	const string option = ToStdString(args[1]->ToString());
	const string ret =XLStringUtil::mb_convert_kana(str,option);

	return v8::String::New(_A2U(ret.c_str()));
}

v8::Handle<v8::Value> ScriptRunner::v8_mb_convert_yomi(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:mb_convert_yomi"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("mb_convert_yomiには1つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("mb_convert_yomiの第一引数が文字列ではありません")));
	}
	const string str = ToStdString(args[0]->ToString());
	const string ret = MainWindow::m()->MecabControl.KanjiAndKanakanaToHiragana(str);

	return v8::String::New(_A2U(ret.c_str()));
}

v8::Handle<v8::Value> ScriptRunner::v8_date(const v8::Arguments& args) 
{
	
	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("dateには1つ以上の引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("dateの第一引数が文字列ではありません")));
	}
	const string str = ToStdString(args[0]->ToString());
	time_t t;
	if (args.Length() >= 2)
	{
		if ( args[1]->IsInt32() )
		{
			t = args[1]->ToInt32()->Value();
		}
		else if ( args[1]->IsString() )
		{
			const string a = ToStdString(args[1]->ToString());
			t = (time_t) atof(a.c_str() );
		}
		else if ( args[1]->IsNumber() )
		{
			t = (time_t) (args[1]->ToNumber()->NumberValue());
		}
		else
		{
			return v8::ThrowException(v8::String::New(_A2U("dateの第二引数が日付ではありません")));
		}
	}
	else
	{
		t = time(NULL);
	}
	const string ret = XLStringUtil::timetostr( t , str );

	return v8::String::New(_A2U(str.c_str() ));
}

v8::Handle<v8::Value> ScriptRunner::v8_time(const v8::Arguments& args) 
{
	

	if (args.Length() <= 0 )
	{
		return v8::Number::New( (double) time(NULL));
	}
	if (args.Length() <= 2 )
	{
		return v8::ThrowException(v8::String::New(_A2U("timeで、時間を指定するには time(YYYY,MM,DD) と3つは必要です。")));
	}

	unsigned int YYYY = 0;
	if (! IsJSInt(args[0],&YYYY) )
	{
		return v8::ThrowException(v8::String::New(_A2U("timeの第一引数の年が数字(YYYY)ではありません")));
	}
	if (YYYY < 1970)
	{
		return v8::ThrowException(v8::String::New(_A2U("timeの第一引数の年が数字(YYYY)は1970より大きくないといけません")));
	}

	unsigned int MM = 0;
	if (! IsJSInt(args[1],&MM) )
	{
		return v8::ThrowException(v8::String::New(_A2U("timeの第二引数の月が数字(MM)ではありません")));
	}
	if (MM <= 0 || MM >= 13 )
	{
		return v8::ThrowException(v8::String::New(_A2U("timeの第二引数の月が数字(MM)は1以上で12以下でないといけません")));
	}

	unsigned int DD = 0;
	if (! IsJSInt(args[2],&DD) )
	{
		return v8::ThrowException(v8::String::New(_A2U("wol_sendの第三引数の日が数字(DD)ではありません")));
	}
	if (DD <= 0 || DD >= 32 )
	{
		return v8::ThrowException(v8::String::New(_A2U("timeの第三引数の日が数字(MM)は1以上で31以下でないといけません")));
	}
	struct tm tt= {0};
	tt.tm_isdst= -1;
	tt.tm_year = YYYY - 1900;
	tt.tm_mon = MM - 1;
	tt.tm_mday = DD;

	if (args.Length() == 3 )
	{
		time_t t = mktime(&tt);
		if (t == 0) return v8::ThrowException(v8::String::New(_A2U("日付を構築できませんでした")));
		return v8::Number::New( (double) t);
	}

	unsigned int HH = 0;
	if (! IsJSInt(args[3],&HH) )
	{
		return v8::ThrowException(v8::String::New(_A2U("wol_sendの第四引数の時刻が数字ではありません")));
	}
	if (HH < 0 || HH >= 24 )
	{
		return v8::ThrowException(v8::String::New(_A2U("timeの第四引数の時刻が数字は0以上で23以下でないといけません")));
	}
	tt.tm_hour = HH;
	if (args.Length() == 4 )
	{
		time_t t = mktime(&tt);
		if (t == 0) return v8::ThrowException(v8::String::New(_A2U("日付を構築できませんでした")));
		return v8::Number::New( (double) t);
	}


	unsigned int MI = 0;
	if (! IsJSInt(args[4],&MI) )
	{
		return v8::ThrowException(v8::String::New(_A2U("wol_sendの第五引数の分が数字ではありません")));
	}
	if (MI < 0 || MI >= 60 )
	{
		return v8::ThrowException(v8::String::New(_A2U("timeの第五引数の分が数字は0以上で59以下でないといけません")));
	}
	tt.tm_min = MI;
	if (args.Length() == 5 )
	{
		time_t t = mktime(&tt);
		if (t == 0) return v8::ThrowException(v8::String::New(_A2U("日付を構築できませんでした")));
		return v8::Number::New( (double) t);
	}


	unsigned int SS = 0;
	if (! IsJSInt(args[5],&SS) )
	{
		return v8::ThrowException(v8::String::New(_A2U("wol_sendの第六引数の秒が数字ではありません")));
	}
	if (SS < 0 || SS >= 60 )
	{
		return v8::ThrowException(v8::String::New(_A2U("timeの第六引数の秒が数字は0以上で59以下でないといけません")));
	}
	tt.tm_sec = SS;
	{
		time_t t = mktime(&tt);
		if (t == 0) return v8::ThrowException(v8::String::New(_A2U("日付を構築できませんでした")));
		return v8::Number::New( (double) t);
	}
}

v8::Handle<v8::Value> ScriptRunner::v8_time_dayofweek(const v8::Arguments& args) 
{
	

	time_t time = 0;
	if ( args[0]->IsInt32() )
	{
		time = args[0]->ToInt32()->Value();
	}
	else if ( args[0]->IsString() )
	{
		time = ToStringToI(args[0]->ToString());
	}
	else if ( args[0]->IsNumber() )
	{
		time = args[0]->ToInteger()->Value();
	}
	else
	{
		return v8::ThrowException(v8::String::New(_A2U("time_dayofweekの第一引数が数字(unixtime)ではありません")));
	}
		
	if (time == 0  ) 
	{//時刻ではありません
		return v8::ThrowException(v8::String::New(_A2U("time_dayofweekの第一引数が数字(unixtime)ではありません。数字化出来ません。")));
	}


#if _MSC_VER
	struct tm *dateTM = localtime(&time);
#else
	struct tm datetmp;
	struct tm *dateTM = &datetmp;
	localtime_r(&time,&datetmp);
#endif
	return v8::Integer::New(dateTM->tm_wday);
}

v8::Handle<v8::Value> ScriptRunner::v8_uuid(const v8::Arguments& args) 
{
	
	string ret = XLStringUtil::uuid();
	return v8::String::New(_A2U(ret.c_str() ));
}


v8::Handle<v8::Value> ScriptRunner::v8_sha1(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:sha1"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("sha1には1つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("sha1の第一引数が文字列ではありません")));
	}
	const string str = ToStdString(args[0]->ToString());
	const string ret =XLStringUtil::sha1(str);

	return v8::String::New(_A2U(ret.c_str()));
}

v8::Handle<v8::Value> ScriptRunner::v8_md5(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:md5"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("md5には1つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("md5の第一引数が文字列ではありません")));
	}
	const string str = ToStdString(args[0]->ToString());
	const string ret =XLStringUtil::md5(str);

	return v8::String::New(_A2U(ret.c_str()));
}

v8::Handle<v8::Value> ScriptRunner::v8_trim(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:trim"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("trimには1つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("trimの第一引数が文字列ではありません")));
	}
	const string str = ToStdString(args[0]->ToString());
	const string ret =XLStringUtil::chop(str);

	return v8::String::New(_A2U(ret.c_str()));
}

v8::Handle<v8::Value> ScriptRunner::v8_html_selector(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:html_selector"  );

	if (args.Length() < 2)
	{
		return v8::ThrowException(v8::String::New(_A2U("html_selectorには2つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("html_selectorの第一引数が文字列(html)ではありません")));
	}
	if (! args[1]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("html_selectorの第二引数が文字列(セレクタ)ではありません")));
	}
	const string html = ToStdString(args[0]->ToString());
	const string selector = ToStdString(args[1]->ToString());

	auto listArray = XLStringUtil::HTMLSelector(html,selector);

	auto it = listArray.begin();
	v8::Local<v8::Array> arrayV8 = v8::Array::New( listArray.size() );
	for (unsigned int i = 0 ; i < listArray.size() ; ++i ) 
	{
		arrayV8->Set( v8::Uint32::New(i) , v8::String::New( _A2U( it->c_str() ) ) );
		++it;
	}

	return arrayV8;
}

v8::Handle<v8::Value> ScriptRunner::v8_html_inner(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:html_inner"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("html_innerには1つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("html_innerの第一引数が文字列(html)ではありません")));
	}
	const string html = ToStdString(args[0]->ToString());

	const string ret = XLStringUtil::InnerHTML(html);
	return v8::String::New(_A2U(ret.c_str()));
}

v8::Handle<v8::Value> ScriptRunner::v8_html_text(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:html_text"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("html_textには1つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("html_textの第一引数が文字列(html)ではありません")));
	}
	const string html = ToStdString(args[0]->ToString());

	const string ret = XLStringUtil::InnerText(html);
	return v8::String::New(_A2U(ret.c_str()));
}

v8::Handle<v8::Value> ScriptRunner::v8_html_attr(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:html_attr"  );

	if (args.Length() < 2)
	{
		return v8::ThrowException(v8::String::New(_A2U("html_attrには2つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("html_attrの第一引数が文字列(html)ではありません")));
	}
	if (! args[1]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("html_attrの第二引数が文字列(属性名)ではありません")));
	}
	const string html = ToStdString(args[0]->ToString());
	const string attr = ToStdString(args[1]->ToString());

	const string ret = XLStringUtil::HTMLAttr(html,attr);
	return v8::String::New(_A2U(ret.c_str()));
}


v8::Handle<v8::Value> ScriptRunner::v8_wol_send(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:wol_send"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("wol_sendには1つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("wol_sendの第一引数が文字列(マックアドレス)ではありません")));
	}
	string macaddress = ToStdString(args[0]->ToString());
	unsigned int port = 2304;
	string boadcastip;

	if (args.Length() >= 2)
	{
		if (! IsJSInt(args[1],&port) )
		{
			return v8::ThrowException(v8::String::New(_A2U("wol_sendの第二引数が数字(port番号)ではありません")));
		}
	}
	if (args.Length() >= 3)
	{
		if (! args[2]->IsString() )
		{
			return v8::ThrowException(v8::String::New(_A2U("wol_sendの第三引数が文字列(ブロードキャストIP 例:192.168.1.255)ではありません")));
		}
		boadcastip = ToStdString(args[2]->ToString());
	}

	if (! ActionImplement::WolSend(macaddress,port,boadcastip) )
	{
		string errmsg = string("")+"wol_sendでマックアドレス(" + macaddress + ")へ送信できませんでした";
		return v8::ThrowException(v8::String::New(_A2U(errmsg.c_str())));
	}
	return v8::Boolean::New(true);
}


v8::Handle<v8::Value> ScriptRunner::v8_upnp_search_all(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:upnp_search_all"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("upnp_search_allには1つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("upnp_search_allの第一引数が文字列(検索する文字列)ではありません")));
	}
	string findstr = ToStdString(args[0]->ToString());
	unsigned int timeoutMS = 5000;

	if (args.Length() >= 2)
	{//タイムアウト MS で受け付けるが実際は S になる
		if ( args[1]->IsInt32() )
		{
			timeoutMS = args[1]->ToInt32()->Value();
		}
		else if ( args[1]->IsString() )
		{
			timeoutMS = ToStringToI(args[1]->ToString());
		}
		else
		{
			return v8::ThrowException(v8::String::New(_A2U("upnp_search_allの第二引数が数字(タイムアウトミリ秒)ではありません")));
		}
	}

	vector<ActionImplement::SSDPStruct> ssdpArray;
	ActionImplement::UpnpSearchAll(&ssdpArray,findstr,timeoutMS/1000);

	//次にV8 array に格納していく
	v8::Local<v8::Array> arrayV8 = v8::Array::New(ssdpArray.size());
	unsigned countI = 0;
	for (auto it = ssdpArray.begin() ; it != ssdpArray.end() ; ++it , ++countI ) 
	{
		v8::Local<v8::Object> objectV8 = v8::Object::New();
		objectV8->Set( v8::String::New( "location" ) , v8::String::New( _A2U(it->location.c_str()) ) );
		objectV8->Set( v8::String::New( "st" ) , v8::String::New( _A2U(it->st.c_str()) ) );
		objectV8->Set( v8::String::New( "srn" ) , v8::String::New( _A2U(it->usn.c_str()) ) );
		objectV8->Set( v8::String::New( "uuid" ) , v8::String::New( _A2U(it->uuid.c_str()) ) );

		arrayV8->Set( v8::Uint32::New(countI) , objectV8 );
	}

	return arrayV8;
}

v8::Handle<v8::Value> ScriptRunner::v8_upnp_search_one(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:upnp_search_one"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("upnp_search_oneには1つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("upnp_search_oneの第一引数が文字列(検索する文字列)ではありません")));
	}
	string findstr = ToStdString(args[0]->ToString());
	unsigned int timeoutMS = 5000;

	if (args.Length() >= 2)
	{//タイムアウト MS で受け付けるが実際は S になる
		if ( args[1]->IsInt32() )
		{
			timeoutMS = args[1]->ToInt32()->Value();
		}
		else if ( args[1]->IsString() )
		{
			timeoutMS = ToStringToI(args[1]->ToString());
		}
		else
		{
			return v8::ThrowException(v8::String::New(_A2U("upnp_search_oneの第二引数が数字(タイムアウトミリ秒)ではありません")));
		}
	}

	ActionImplement::SSDPStruct ssdp;
	if ( ! ActionImplement::UpnpSearchOne(&ssdp,findstr,timeoutMS/1000) )
	{//みつからない
		return v8::Boolean::New(false);
	}

	//次にV8 array に格納していく
	v8::Local<v8::Object> objectV8 = v8::Object::New();
	objectV8->Set( v8::String::New( "location" ) , v8::String::New( _A2U(ssdp.location.c_str()) ) );
	objectV8->Set( v8::String::New( "st" ) , v8::String::New( _A2U(ssdp.st.c_str()) ) );
	objectV8->Set( v8::String::New( "usn" ) , v8::String::New( _A2U(ssdp.usn.c_str()) ) );
	objectV8->Set( v8::String::New( "uuid" ) , v8::String::New( _A2U(ssdp.uuid.c_str()) ) );

	return objectV8;
}


v8::Handle<v8::Value> ScriptRunner::v8_upnp_search_uuid(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:upnp_search_uuid"  );

	if (args.Length() < 1)
	{
		return v8::ThrowException(v8::String::New(_A2U("upnp_search_uuidには1つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("upnp_search_uuidの第一引数が文字列(検索するUUID)ではありません")));
	}
	string uuid = ToStdString(args[0]->ToString());
	unsigned int timeoutMS = 5000;

	if (args.Length() >= 2)
	{//タイムアウト MS で受け付けるが実際は S になる
		if ( args[1]->IsInt32() )
		{
			timeoutMS = args[1]->ToInt32()->Value();
		}
		else if ( args[1]->IsString() )
		{
			timeoutMS = ToStringToI(args[1]->ToString());
		}
		else
		{
			return v8::ThrowException(v8::String::New(_A2U("upnp_search_uuidの第二引数が数字(タイムアウトミリ秒)ではありません")));
		}
	}

	ActionImplement::SSDPStruct ssdp;
	if ( ! ActionImplement::UpnpSearchUUID(&ssdp,uuid,timeoutMS/1000) )
	{//みつからない
		return v8::Boolean::New(false);
	}

	//次にV8 array に格納していく
	v8::Local<v8::Object> objectV8 = v8::Object::New();
	objectV8->Set( v8::String::New( "location" ) , v8::String::New( _A2U(ssdp.location.c_str()) ) );
	objectV8->Set( v8::String::New( "st" ) , v8::String::New( _A2U(ssdp.st.c_str()) ) );
	objectV8->Set( v8::String::New( "usn" ) , v8::String::New( _A2U(ssdp.usn.c_str()) ) );
	objectV8->Set( v8::String::New( "uuid" ) , v8::String::New( _A2U(ssdp.uuid.c_str()) ) );

	return objectV8;
}


v8::Handle<v8::Value> ScriptRunner::v8_upnp_search_xml(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:upnp_search_xml"  );

	if (args.Length() < 2)
	{
		return v8::ThrowException(v8::String::New(_A2U("upnp_search_xmlには2つの引数が必要です")));
	}
	if (! args[0]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("upnp_search_xmlの第一引数が文字列(ヘッダーを検索する文字列)ではありません")));
	}
	if (! args[1]->IsString() )
	{
		return v8::ThrowException(v8::String::New(_A2U("upnp_search_xmlの第ニ引数が文字列(XMLを検索する文字列)ではありません")));
	}
	string findstr = ToStdString(args[0]->ToString());
	string findxml = ToStdString(args[1]->ToString());;
	unsigned int timeoutMS = 5000;

	if (args.Length() >= 3)
	{//タイムアウト MS で受け付けるが実際は S になる
		if ( args[2]->IsInt32() )
		{
			timeoutMS = args[2]->ToInt32()->Value();
		}
		else if ( args[2]->IsString() )
		{
			timeoutMS = ToStringToI(args[2]->ToString());
		}
		else
		{
			return v8::ThrowException(v8::String::New(_A2U("upnp_search_xmlの第三引数が数字(タイムアウトミリ秒)ではありません")));
		}
	}

	ActionImplement::SSDPStruct ssdp;
	string xml;
	if ( ! ActionImplement::UpnpSearchXML(&ssdp,&xml,findstr,findxml,timeoutMS/1000) )
	{//みつからない
		return v8::Boolean::New(false);
	}

	//次にV8 array に格納していく
	v8::Local<v8::Object> objectV8 = v8::Object::New();
	objectV8->Set( v8::String::New( "location" ) , v8::String::New( _A2U(ssdp.location.c_str()) ) );
	objectV8->Set( v8::String::New( "st" ) , v8::String::New( _A2U(ssdp.st.c_str()) ) );
	objectV8->Set( v8::String::New( "usn" ) , v8::String::New( _A2U(ssdp.usn.c_str()) ) );
	objectV8->Set( v8::String::New( "uuid" ) , v8::String::New( _A2U(ssdp.uuid.c_str()) ) );
//	objectV8->Set( v8::String::New( "XML" ) , v8::String::New( _A2U(xml.c_str()) ) );

	return objectV8;
}


v8::Handle<v8::Value> ScriptRunner::v8_env_version(const v8::Arguments& args) 
{
	DEBUGLOG(string() + "js function:env_version"  );

	const float version = MainWindow::m()->Config.GetInt("version_number",100) / 100.f;
	const string versionString = num2str(version);
	return v8::String::New(versionString.c_str() );
}

v8::Handle<v8::Value> ScriptRunner::v8_env_model(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:env_model"  );
	const string model = SystemMisc::GetModel();
	return v8::String::New(_A2U(model.c_str()));
}

v8::Handle<v8::Value> ScriptRunner::v8_env_os(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:env_os"  );
#if _MSC_VER
	const string os = "windows";
#else
	const string os = "linux";
#endif
	return v8::String::New(_A2U(os.c_str()));
}

v8::Handle<v8::Value> ScriptRunner::v8_env_mic_volume(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:env_mic_volume"  );

	if (args.Length() < 1)
	{//引数がなければ値を取得するか!
		return v8::Integer::New( MainWindow::m()->Config.GetInt("recong_volume",100) );
	}
	unsigned int vol = 0;
	if ( args[0]->IsInt32() )
	{
		vol = args[0]->ToInt32()->Value();
	}
	else if ( args[0]->IsString() )
	{
		vol = ToStringToI(args[0]->ToString());
	}
	else
	{
		return v8::ThrowException(v8::String::New(_A2U("env_mic_volumeの第一引数が数字(ボリューム)ではありません")));
	}

	if (vol < 0 || vol > 100)
	{
		return v8::ThrowException(v8::String::New(_A2U("env_mic_volumeの第一引数が数字(ボリューム)が 0 ～ 100 の範囲ではありません")));
	}
	
	MainWindow::m()->Config.Set("recong_volume",num2str(vol));
	MainWindow::m()->VolumeControll.ChangeMicVolume();

	return v8::Boolean::New(true);
}

v8::Handle<v8::Value> ScriptRunner::v8_env_speaker_volume(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:env_speaker_volume"  );

	if (args.Length() < 1)
	{//引数がなければ値を取得するか!
		return v8::Integer::New( MainWindow::m()->Config.GetInt("speak_volume",100) );
	}
	unsigned int vol = 0;
	if ( args[0]->IsInt32() )
	{
		vol = args[0]->ToInt32()->Value();
	}
	else if ( args[0]->IsString() )
	{
		vol = ToStringToI(args[0]->ToString());
	}
	else
	{
		return v8::ThrowException(v8::String::New(_A2U("env_speaker_volumeの第一引数が数字(ボリューム)ではありません")));
	}

	if (vol < 0 || vol > 100)
	{
		return v8::ThrowException(v8::String::New(_A2U("env_speaker_volumeの第一引数が数字(ボリューム)が 0 ～ 100 の範囲ではありません")));
	}

	MainWindow::m()->Config.Set("speak_volume",num2str(vol));
	MainWindow::m()->VolumeControll.ChangeSpeakerVolume();

	return v8::Boolean::New(true);
}


v8::Handle<v8::Value> ScriptRunner::v8_kernel_system_reboot(const v8::Arguments& args) 
{
	
	DEBUGLOG(string() + "js function:kernel_system_reboot"  );

	MainWindow::m()->Shutdown(EXITCODE_LEVEL_REBOOT,true);
	return v8::Boolean::New(true);
}

void ScriptRunner::WorkResourceClear()
{
#if _WITH_USB
	for(auto it = UsbDeviceList.begin() ; it != UsbDeviceList.end() ; ++it )
	{
		if (*it)	delete *it;
	}
	this->UsbDeviceList.clear();
#endif //_WITH_USB
}


string ScriptRunner::ToStdString(const v8::Local<v8::String>& v8str)
{
	v8::HandleScope scope;
	

	const v8::String::Utf8Value param1(v8str);
	if (*param1 == NULL) return "";

	return _U2A(*param1);
}

int ScriptRunner::ToStringToI(const v8::Local<v8::String>& v8str)
{
	v8::HandleScope scope;
	const v8::String::Utf8Value param1(v8str);
	if (*param1 == NULL) return 0;

	return atoi(*param1);
}

map<string,string> ScriptRunner::ToStdStringMap(const v8::Local<v8::Value>& v8val)
{
	v8::HandleScope scope;
	map<string,string> retmap;
	string r;
	if (!v8val->IsObject())
	{
		return retmap;
	}

	//一度キーを取得する
	v8::Handle<v8::Object> object = v8::Handle<v8::Object>::Cast(v8val);
	v8::Local<v8::Array> propertyNames = object->GetPropertyNames();
	//キーを順繰りに見ながら値をとっていく
	for(unsigned int i = 0 ; i < propertyNames->Length() ; ++i )
	{
		retmap[ ToStdString(propertyNames->Get(i)->ToString()) ] = V8ValueDump(object->Get(propertyNames->Get(i)) );
	}
	return retmap;
}
map<string,string> ScriptRunner::ToStdStringMap(const v8::Handle<v8::Value>& v8val)
{
	v8::HandleScope scope;
	map<string,string> retmap;
	string r;
	if (!v8val->IsObject())
	{
		return retmap;
	}

	//一度キーを取得する
	v8::Handle<v8::Object> object = v8::Handle<v8::Object>::Cast(v8val);
	v8::Local<v8::Array> propertyNames = object->GetPropertyNames();
	//キーを順繰りに見ながら値をとっていく
	for(unsigned int i = 0 ; i < propertyNames->Length() ; ++i )
	{
		retmap[ ToStdString(propertyNames->Get(i)->ToString()) ] = V8ValueDump(object->Get(propertyNames->Get(i)) );
	}
	return retmap;
}

string ScriptRunner::ReportException(const v8::TryCatch* try_catch) 
{
	

	v8::HandleScope handle_scope;

	v8::String::Utf8Value exception(try_catch->Exception());
	string exceptionStr = *exception;
	string r ;

	v8::Handle<v8::Message> message = try_catch->Message();
	if (message.IsEmpty()) 
	{
		r = string("@exception:") + exceptionStr;
		return _U2A(r);
	}
	int linenum = message->GetLineNumber();
	r =  "@line:" + num2str(linenum) + "@exception:" + exceptionStr;
/*
	+ "\r\n";

	r += ToStdString(message->GetSourceLine()) + "\r\n";

	// Print wavy underline (GetUnderline is deprecated).
	int start = message->GetStartColumn();
	for (int i = 0; i < start; i++) 
	{
		r += " ";
	}
	int end = message->GetEndColumn();
	for (int i = start; i < end; i++) {
		r += "^";
	}
	r += "\r\n";

	v8::String::Utf8Value stack_trace(try_catch->StackTrace());
	if (*stack_trace != NULL)
	{
		r += *stack_trace ;
	}
*/
	return _U2A(r);
}

//json文字列を、オブジェクトに変換。失敗時 false
v8::Handle<v8::Value> ScriptRunner::JsonUnSerialize(const string& str)
{
	
	v8::HandleScope scope;


	const string pureStr = XLStringUtil::chop(str);
	v8::Local<v8::Value> jsonString = v8::String::New(_A2U(pureStr.c_str() ));

	v8::Handle<v8::Object> global = this->V8Context->Global();
	v8::Handle<v8::Object> JSON = global->Get(v8::String::New("JSON"))->ToObject();
	v8::Handle<v8::Function> JSON_parse = v8::Handle<v8::Function>::Cast(JSON->Get(v8::String::New("parse")));

	// return JSON.parse.apply(JSON, jsonString);
	return scope.Close(JSON_parse->Call(JSON, 1, &jsonString));
}

//オブジェクトをjson文字列に変換
v8::Handle<v8::Value> ScriptRunner::JsonSerialize(const v8::Handle<v8::Value>& obj)
{
	v8::HandleScope scope;

	v8::Handle<v8::Object> global = this->V8Context->Global();
	v8::Handle<v8::Object> JSON = global->Get(v8::String::New("JSON"))->ToObject();
	v8::Handle<v8::Function> JSON_parse = v8::Handle<v8::Function>::Cast(JSON->Get(v8::String::New("stringify")));

	//引数を作成する
	v8::Handle<v8::Value> args[3] = {obj,v8::Null(),v8::Int32::New(2)};

	// return JSON.stringify.apply(JSON, obj,null,2);
	return scope.Close(JSON_parse->Call(JSON, 3, args));
}

//無効なオブジェクトかどうかの判定
bool ScriptRunner::IsbadObject(const v8::Handle<v8::Value>& obj)
{
	return ( obj.IsEmpty() || obj->IsFalse() || obj->IsNull() || obj->IsUndefined() );
}


v8::Handle<v8::Value> ScriptRunner::SimpleAPICaller(const string& function_banner,const string& api_script_command,const string& api_script_command_def,const v8::Arguments& args) 
{
	
	ScriptRunner* _this = g_ScriptRunner_This;
	DEBUGLOG("js function:" << function_banner  );

	if (args.Length() < 1)
	{
		const string ee = function_banner + "には1つ以上の引数が必要です";
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str())));
	}
	if (! args[0]->IsString() )
	{
		const string ee = function_banner + "の第一引数が文字列ではありません";
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str())));
	}

	list<string> commandArgs;
	commandArgs.push_back(ToStdString(args[0]->ToString())  );
	const string weather_script_command = MainWindow::m()->Config.Get(api_script_command,api_script_command_def);

	string result;
	//実行
	try
	{
		result = MainWindow::m()->ScriptManager.FireCommand( CallbackPP::NoCallback() ,weather_script_command,commandArgs);
	}
	catch(XLException &e)
	{
		const string ee = function_banner + "/" + e.what();
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}

	v8::Handle<v8::Value> retobj = _this->JsonUnSerialize(result);
	if ( IsbadObject(retobj) )
	{
		const string ee = function_banner + "/結果をjsonにできません 文字列:" + result;
		return v8::ThrowException(v8::String::New(_A2U(ee.c_str() )));
	}
	return retobj;
}



string ScriptRunner::getSTDOUT()
{
	return g_STDOUT;
}

bool ScriptRunner::IsJSInt(const v8::Local<v8::Value>& val , int* outInt)
{
	if ( val->IsInt32() )
	{
		*outInt = val->ToInt32()->Value();
		return true;
	}
	else if ( val->IsString() )
	{
		*outInt = ToStringToI(val->ToString());
		return true;
	}
	return false;
}

bool ScriptRunner::IsJSInt(const v8::Local<v8::Value>& val , unsigned int* outInt)
{
	if ( val->IsInt32() )
	{
		*outInt = val->ToInt32()->Value();
		return true;
	}
	else if ( val->IsString() )
	{
		*outInt = ToStringToI(val->ToString());
		return true;
	}
	return false;
}

bool ScriptRunner::IsJSTime(const v8::Local<v8::Value>& val , time_t* outTime)
{
	time_t time = 0;
	if ( val->IsInt32() )
	{
		*outTime = val->ToInt32()->Value();
		return true;
	}
	else if ( val->IsString() )
	{
		*outTime = ToStringToI(val->ToString());
		return true;
	}
	else if ( val->IsNumber() )
	{
		*outTime = val->ToInteger()->Value();
		return true;
	}
	else
	{
		return false;
	}
}




#endif // WITH_CLIENT_ONLY_CODE==1