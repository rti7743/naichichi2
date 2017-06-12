#pragma once

#include "../v8/include/v8.h"
#include "Callbackable.h"
#if _WITH_USB
	#include "XLUSBHIDDevice.h"
#endif //_WITH_USB

//javascript インターフェースの実装 
class ScriptRunner 
{
	v8::HandleScope handle_scope;
	v8::Handle<v8::ObjectTemplate> V8Global;
	v8::Persistent<v8::Context> V8Context;
	
	bool IsScenario;
	string filename;

#if _WITH_USB
	vector<XLUSBHIDDevice*> UsbDeviceList;
#endif //_WITH_USB
	vector<CallbackPP> TimerCallbackList;

	list<CallbackPP>	CallbackList;
public:
	ScriptRunner(  bool isScenario)
	{		
		this->IsScenario = isScenario;
	}
	virtual ~ScriptRunner();

	bool LoadScript(const string & filename);
	//メモリからv8のプログラムを構築(テスト用)
	bool EvalScript(const string & script);

	bool IsMethodExist(const string& name);
	string callFunction(const string& name);
	string callFunction(const string& name,const map<string , string> & match);
	string callFunction(const string& name,const list<string> & list);
	string callFunctionResultJson(const string& name,const list<string> & list,map<string,string>* outResult);

	//V8インスタンスを解放し初期化します。 変更を加えたコールバックを破棄します。
	void Reset();

	string getFilename() const
	{
		return this->filename;
	}
	static string getSTDOUT();

	string StringToJson(const string result,map<string,string>* outResult);

private:
	//onvoiceのcallback
	void OnVoiceCallback(v8::Persistent<v8::Function> callback);

	//タイムアウト付きで func を実行する.
	bool RunScriptWithTimeout(const std::function<void(void)>& func );

	//コールバック時に呼ばれる
	void callbackFunction(v8::Persistent<v8::Function> callback);

	//V8インスタンスを作る
	bool CreateV8Instance(const string& source);

	//ワーカーリソースの解放
	void WorkResourceClear();
	//V8の関数を実行する
	v8::Handle<v8::Value> RunV8Function(v8::Handle<v8::Function>& function,unsigned int argCount,v8::Handle<v8::Value> args[]);

	void ClearCallback();
	CallbackPP NewCallback(std::function<void (void) > f);

	static v8::Handle<v8::Value> v8_onvoice(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_callcommand(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_action(const v8::Arguments& args); 
	static v8::Handle<v8::Value> v8_speak(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_speak_cache(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_play(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_config_set(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_config_get(const v8::Arguments& args); 
	static v8::Handle<v8::Value> v8_kernel_config_set(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_kernel_config_get(const v8::Arguments& args); 
	static v8::Handle<v8::Value> v8_kernel_system_reboot(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_telnet(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_http_get(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_http_post(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_kernel_execute_local(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_execute_remote(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_msleep(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_sleep(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_dump(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_sensor_lumi(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_sensor_temp(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_sensor_sound(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_config_find(const v8::Arguments& args); 
	static v8::Handle<v8::Value> v8_kernel_config_find(const v8::Arguments& args); 
	static v8::Handle<v8::Value> v8_usb_open(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_usb_close(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_usb_read(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_usb_write(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_usb_messageread(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_usb_messagewrite(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_elec_getlist(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_elec_getactionlist(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_elec_getinfo(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_sstp_send11(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_soap_action(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_wol_send(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_env_version(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_env_model(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_env_os(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_upnp_search_all(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_upnp_search_one(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_upnp_search_uuid(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_upnp_search_xml(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_url_hosturl(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_url_baseurl(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_url_changeprotocol(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_url_appendparam(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_base64encode(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_base64decode(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_urlencode(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_urldecode(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_escapeshellarg(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_sha1(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_md5(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_trim(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_mb_convert_kana(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_mb_convert_yomi(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_date(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_time(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_time_dayofweek(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_uuid(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_elec_getstatus(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_elec_setstatus(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_setTimeout(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_setInterval(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_clearTimeout(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_html_selector(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_html_inner(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_html_text(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_html_attr(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_time_gmt_to_local(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_time_local_to_gmt(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_time_is_holiday(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_time_day_info(const v8::Arguments& args) ;

	static v8::Handle<v8::Value> v8_address_parse(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_weather_info(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_wearher_is_rain(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_wearher_is_rain_soon(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_train_info(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_train_is_delay(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_env_mic_volume(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_env_speaker_volume(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_kernel_process_list(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_kernel_kill(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_background_music_play(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_background_music_stop(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_trigger_is_enable(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_trigger_change_enable(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_twitter_tweet(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_twitter_home_timeline(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_twitter_user_timeline(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_multiroom_list(const v8::Arguments& args) ;
	static v8::Handle<v8::Value> v8_multiroom_fire(const v8::Arguments& args) ;

	//V8の文字列を 普通の文字列に変換
	static string ToStdString(const v8::Local<v8::String>& v8str);
	static int ToStringToI(const v8::Local<v8::String>& v8str);
	static map<string,string> ToStdStringMap(const v8::Local<v8::Value>& v8val);
	static map<string,string> ToStdStringMap(const v8::Handle<v8::Value>& v8val);
	static bool IsJSInt(const v8::Local<v8::Value>& val , int* outInt);
	static bool IsJSInt(const v8::Local<v8::Value>& val , unsigned int* outInt);
	static bool IsJSTime(const v8::Local<v8::Value>& val , time_t* outTime);


	static string V8ValueDump(const v8::Local<v8::Value>& val);
	static string ReportException(const v8::TryCatch* try_catch) ;
	v8::Handle<v8::Value> SimpleAPICaller(const string& function_banner,const string& api_script_command,const string& api_script_command_def,const v8::Arguments& args) ;


	//json文字列を、オブジェクトに変換 失敗時 false
	v8::Handle<v8::Value> JsonUnSerialize(const string& str);
	//オブジェクトをjson文字列に変換
	v8::Handle<v8::Value> JsonSerialize(const v8::Handle<v8::Value>& obj);
	//無効なオブジェクトかどうかの判定
	static bool IsbadObject(const v8::Handle<v8::Value>& obj);

};

