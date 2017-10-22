#pragma once
#include "Callbackable.h"
#include "HttpServer.h"
#include "XLHttpHeader.h"
#include "ScriptWebRunnerRecogn.h"
#include "ScriptWebRunnerSpeak.h"
#include "ScriptWebRunnerTrigger.h"

//Web画面の実装 
class ScriptWebRunner
{
public:
	ScriptWebRunner();
	virtual ~ScriptWebRunner();
	void Create();
	//webからアクセスがあったときに呼ばれます。
	bool WebAccess(const string& path,const XLHttpHeader& httpHeaders,map<string,string>* cookieArray,WEBSERVER_RESULT_TYPE* result,string* responsString);
	//トリガーリロード
	void ReloadTrigger(const time_t& now);
	//Scriptの終了
	void DestoryScript();
	//Configを変更した場合
	void ConfigUpdated(bool fullsync);

private:
	//ステータスを jsonで
	string RemoconStatus(const string& appendOption)  const;
	string GetSensorJson() ;
	//変更画面全部
	string EditIndex(const XLHttpHeader& httpHeaders) const;
	string remocon_fire_bystring(const XLHttpHeader& httpHeaders) ;
	string remocon_preview_tospeak_string(const XLHttpHeader& httpHeaders) ;
	string remocon_preview_elec_mp3(const XLHttpHeader& httpHeaders) ;
	string remocon_preview_elec_macro(const XLHttpHeader& httpHeaders) ;
	//SIPで発信する
	string remocon_sip_call(const XLHttpHeader& httpHeaders) ;
	//SIPでかかってきた電話にこたえる
	string remocon_sip_answer(const XLHttpHeader& httpHeaders) ;
	//SIPで電話を切る
	string remocon_sip_hangup(const XLHttpHeader& httpHeaders) ;
	//SIP numpadを閉じるイベント
	string remocon_sip_numpad_close(const XLHttpHeader& httpHeaders) ;
	//リモコンの副次的情報の取得
	string remocon_get_append_info(const XLHttpHeader& httpHeaders);

	//音声認識等のリロード
	string remocon_recogn_reload(const XLHttpHeader& httpHeaders);
	//リモコン学習の後処理前処理
	string remocon_toplevel_invoke(const XLHttpHeader& httpHeaders);
	//リモコン学習
	string remocon_ir_leaning(const XLHttpHeader& httpHeaders);
	string remocon_ir_fire(const XLHttpHeader& httpHeaders);
	bool remocon_ir_leaning(const string& exec_ir);
	//アイコンの並び順の更新
	string remocon_update_icon_order(const XLHttpHeader& httpHeaders);
	//アクションアイコンの並び順の更新
	string remocon_update_elec_action_order(const XLHttpHeader& httpHeaders);
	//家電を消す
	string remocon_delete_elec(const XLHttpHeader& httpHeaders) ;
	//家電の操作を消す
	string remocon_delete_elec_action(const XLHttpHeader& httpHeaders);
	//機材のアップデート
	//elec_key=123&type=タイプ&type_other=タイプその他
	string remocon_update_elec(const XLHttpHeader& httpHeaders) ;
	//機材のアクション項目のアップデート
	string remocon_update_elec_action(const XLHttpHeader& httpHeaders) ;
	//設定のステータスを jsonで
	string SettingStatus()  const;
	//トリガー設定のステータスを jsonで
	string TriggerStatus()  const;
	//マルチルーム設定のステータスを jsonで
	string MultiRoomStatus()  const;
	string setting_account_update(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray) ;
	//ネットワークの設定を変更する通信 (設定を変えるだけ)
	string setting_network_update(const XLHttpHeader& httpHeaders) ;
	string setting_recong_update(const XLHttpHeader& httpHeaders) ;
	string setting_speak_update(const XLHttpHeader& httpHeaders) ;
	string setting_version_upgradecheck(const XLHttpHeader& httpHeaders) ;
	//アップグレード実行
	string setting_version_upgraderun(const XLHttpHeader& httpHeaders) ;
	//再起動させる
	string setting_version_reboot(const XLHttpHeader& httpHeaders) ;
	//終了させる
	string setting_version_shutdown(const XLHttpHeader& httpHeaders) ;
	//ログを取得する
	string setting_version_log(const XLHttpHeader& httpHeaders)  const;
	//サポート用のtar玉の取得
	string setting_version_download_supportfile(const XLHttpHeader& httpHeaders)  const;
	string setting_twitter_regist(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result)  const;
	string setting_twitter_callback(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result)  ;
	string setting_twitter_cancel(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result)  ;
	//最新の音声認識のログを取得する
	string mictest_log(const XLHttpHeader& httpHeaders) const;
	//録音されたファイルをダウンロード
	string mictest_wavdownload(const XLHttpHeader& httpHeaders) const;
	string setting_sensor_getall(const XLHttpHeader& httpHeaders) ;
	string setting_sensor_update(const XLHttpHeader& httpHeaders) ;
	string setting_system_update(const XLHttpHeader& httpHeaders) ;
	string setting_changelang_update(const XLHttpHeader& httpHeaders) ;
	string setting_remoconsetting_update(const XLHttpHeader& httpHeaders) ;
	string setting_trigger_enable_change(const XLHttpHeader& httpHeaders);
	string setting_trigger_update(const XLHttpHeader& httpHeaders) ;
	//webapiのキーリセット
	string setting_webapi_resetkey(const XLHttpHeader& httpHeaders) ;
	//homekitの起動と停止
	string setting_homekit_boot(const XLHttpHeader& httpHeaders) ;
	//homekitのキーリセット
	string setting_homekit_resetkey(const XLHttpHeader& httpHeaders) ;
	//sip
	string setting_sip_update(const XLHttpHeader& httpHeaders) ;
	string remocon_sip_fire(const XLHttpHeader& httpHeaders) ;

	string remocon_fileselectpage_find(const XLHttpHeader& httpHeaders) const;

	string remocon_fileselectpage_upload(const XLHttpHeader& httpHeaders) ;
	string remocon_fileselectpage_delete(const XLHttpHeader& httpHeaders) ;
	string script_scripteditor_load(const XLHttpHeader& httpHeaders) const;
	//会話シナリオの実行
	string script_scripteditor_save(const XLHttpHeader& httpHeaders);
	string version_import(const XLHttpHeader& httpHeaders) const;
	string version_export(const XLHttpHeader& httpHeaders) ;
	//認証しているかどうかのチェック
	bool checkAuth(map<string,string>* cookieArray) const;
	//認証
	string AuthIndex(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result,map<string,string>* cookieArray);
	//リモコン
	string RemoconIndex(const XLHttpHeader& httpHeaders,const map<string,string>& cookieArray) ;
	//認証されているか？
	string auth_is_auth(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray);
	//ID Passwordに変更があった時
	void IDPasswordUpdated();
	bool checkPasswordAuth(const string& id,const string& password);
	string api_upnp_xml(const XLHttpHeader& httpHeaders) ;
	string api_elec_action(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) ;
	string api_elec_actionstr(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) ;
	//すごく詳しいけどわかりづらいリストを返す
	string api_elec_detaillist(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) ;
	//家電一覧だけ取得する
	string api_elec_getlist(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) ;
	//機材のアクション一覧を取得する
	string api_elec_getactionlist(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) ;
	//家電の情報を取得する
	string api_elec_getinfo(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) ;
	string api_sensor_get(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) ;
	string api_speak(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) ;
	//環境情報の取得
	string api_env(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) ;
	string api_play(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) ;
	string api_recong_list(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) ;
	string api_recong_firebystring(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) ;
	//マルチルームapi
	string api_multiroom_elec_action(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) ;
	string api_multiroom_elec_getlist(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) ;
	string api_multiroom_elec_getactionlist(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) ;
	//ネット家電api
	string remocon_netdevice_fire(const XLHttpHeader& httpHeaders) ;
	string remocon_netdevice_elec_getlist(const XLHttpHeader& httpHeaders) ;
	string remocon_netdevice_action_getlist(const XLHttpHeader& httpHeaders) ;
	string remocon_netdevice_value_getlist(const XLHttpHeader& httpHeaders) ;

	//実行!!
	string remocon_fire_bytype(const XLHttpHeader& httpHeaders) ;
	//コマンドの実行
	string script_scripteditor_command_run(const XLHttpHeader& httpHeaders);
	//スクリプト内のコメントに書いてあるアノテーションから引数情報を取得する.
	string script_scripteditor_annotation(const XLHttpHeader& httpHeaders) const;
	//トリガーを消す
	string setting_trigger_delete(const XLHttpHeader& httpHeaders) ;
	//トリガーのweb検索のプレビュー
	string trigger_preview_if_web(const XLHttpHeader& httpHeaders) ;
	//トリガーのコマンドの実行
	string trigger_preview_if_command(const XLHttpHeader& httpHeaders);
	//天気のトリガ条件のテスト
	string trigger_preview_if_weather(const XLHttpHeader& httpHeaders);

	//部屋の並び順の更新
	string setting_multiroom_order(const XLHttpHeader& httpHeaders) ;
	//削除
	string setting_multiroom_delete(const XLHttpHeader& httpHeaders) ;
	//追加処理
	string setting_multiroom_append(const XLHttpHeader& httpHeaders) ;
	//ほかの端末からマルチルームの追加時に呼ばれます. 機械が勝手にコール.
	string multiroom_registadd(const XLHttpHeader& httpHeaders) ;
	//ほかの端末からマルチルームの削除時に呼ばれます. 機械が勝手にコール.
	string multiroom_registdel(const XLHttpHeader& httpHeaders) ;
	//sync POSTされたMultiRoomデータで上書きます. 機械が勝手にコール.
	string multiroom_sync(const XLHttpHeader& httpHeaders) ;
	//リモコン画面からの部屋変更.
	string remocon_multiroom_change(const XLHttpHeader& httpHeaders) ;
	//ワンタイムな認証コードの生成
	string auth_multiroom_challenge_generation(const XLHttpHeader& httpHeaders) ;
	//ワンタイムな認証コードを入れて認証する.
	string auth_multiroom_challenge_try(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray,WEBSERVER_RESULT_TYPE* result) ;
	//検索
	string setting_multiroom_search(const XLHttpHeader& httpHeaders) ;

	
	ScriptWebRunnerRecogn Recogn;
	ScriptWebRunnerSpeak Speak;
	ScriptWebRunnerTrigger Trigger;
};
