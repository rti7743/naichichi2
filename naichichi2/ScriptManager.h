#pragma once

#include "common.h"
#include "ScriptRunner.h"
#include "ScriptWebRunner.h"
#include "XLHttpHeader.h"

//スクリプトエンジンの管理 
class ScriptManager
{
public:
	ScriptManager();

	virtual ~ScriptManager();
	//作成
	bool Create();
	//Scriptファイルの再読み込み
	string ReloadScript(const string & scriptfilename);
	//コマンドファイルが実在するか
	bool ExistCommand(const string & commandName);
	//Commandの実行 失敗したら例外を変えす commandとは実際に家電を操作する Script
	string FireCommand(const CallbackPP& callback , const string & commandName,const list<string>& args);
	//Actionの実行 失敗したら例外を返す actionとはこんな名前の家電を操作してほしいなという指示。その結果 command が呼ばれる
	void FireElec(const CallbackPP& callback , const string & elecType1,const string & elecType2);
	//Actionの実行を文字列でやります 失敗したら例外を返す 
	void FireElecString(const CallbackPP& callback , const string & str);
	//Configを今すぐ同期する
	void SensorConfigUpdateNow(float * retTemp,float *retLumi,float *retSound);

	//音声認識した結果
	void VoiceRecogntion(const CallbackPP& callback);
	//トリガーの実行
	void FireTrigger(const CallbackPP& callback);
	//喋り終わった時
	void SpeakEnd(const CallbackPP& callback,const string& text);
	//トリガーが呼ばれたとき
	void TriggerCall(const CallbackPP& callback,const map< string , string >& args,string * respons);

	//トリガーリロード
	void ReloadTrigger(const time_t& now);

	//Scriptの終了
	void DestoryScript();

	//webからアクセスがあった時
	bool WebAccess(const string& path,const XLHttpHeader& httpHeaders,map<string,string>* cookieArray,WEBSERVER_RESULT_TYPE* result,string* responsString);
private:

	//家電制御が終わった時
	void ActionEnd(const CallbackPP& callback);

	//Scriptファイル郡の読み込み
	void loadScript(const string & baseDirectory);


	list<ScriptRunner*>  Scripts;
	ScriptWebRunner			  WebScript;
};
