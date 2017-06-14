//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include "ScriptManager.h"
#include "MainWindow.h"
#include "XLFileUtil.h"
#include "XLStringUtil.h"
#include "Fire.h"

ScriptManager::ScriptManager()
{
}
ScriptManager::~ScriptManager()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	DEBUGLOG("stop...");

	this->DestoryScript();

	DEBUGLOG("done");
}

//作成
bool ScriptManager::Create()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	this->DestoryScript();

	
	DEBUGLOG("ScriptManager初期化開始");

	this->loadScript( MainWindow::m()->GetConfigBasePath("/webroot/user/script_scenario/") );

	this->WebScript.Create();

	DEBUGLOG("ScriptManager初期化完了");
	return true;
}


//Scriptファイル郡の読み込み
void ScriptManager::loadScript(const string & baseDirectory)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	this->Scripts.clear();
	bool ret = XLFileUtil::findfile(baseDirectory , [&](const string& filename,const string& fullfilename) -> bool {
		//*.js を探索
		if ( XLStringUtil::baseext_nodot(filename) != "js" )
		{
			return true;
		}

		ScriptRunner* runner = new ScriptRunner( true );
		this->Scripts.push_back(runner);

		try
		{
			runner->LoadScript(fullfilename);
			runner->callFunction("main");
		}
		catch(XLException &e)
		{
			//解析エラーだが、起動時なのでどうすることもできない。
			ERRORLOG( e.what() );
		}

		return true;
	});
	if (!ret)
	{
		throw XLEXCEPTION("findfirstに失敗 ディレクトリ:" << baseDirectory << " を検索できません。");
	}

	return ;
}

//Scriptファイルの再読み込み 失敗したら例外を飛ばす
string ScriptManager::ReloadScript(const string & scriptfilename)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	ScriptRunner* runner;
	for(auto it = this->Scripts.begin() ; it != this->Scripts.end() ; ++it )
	{
		runner = (*it);
		if ( runner->getFilename() != scriptfilename )
		{
			continue;
		}

		//再読み込みを指示します。
		//失敗したら例外が飛んできます。
		runner->Reset();
		runner->LoadScript(scriptfilename);
		runner->callFunction("main");
		return runner->getSTDOUT();
	}

	//まだ読み込んでいない js らしいので、読み込みます。
	//失敗したら例外が飛んできます。
	runner = new ScriptRunner( true );
	this->Scripts.push_back(runner);

	//読み込みます。失敗したら例外が飛びます
	runner->LoadScript(scriptfilename);
	runner->callFunction("main");
	return runner->getSTDOUT();
}


//Scriptの終了
void ScriptManager::DestoryScript()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	auto it = this->Scripts.begin();
	for( ; it != this->Scripts.end() ; ++it )
	{
		delete *it;
	}
	this->Scripts.clear();

	this->WebScript.DestoryScript();
}

//コマンドファイルが実在するか
bool ScriptManager::ExistCommand(const string & commandName)
{
	if ( ! XLStringUtil::checkSafePath(commandName) )
	{//危険なファイルなのでないことにする
		return false;
	}

	string commandScriptFilename = MainWindow::m()->GetConfigBasePath("/webroot/user/script_command/" + commandName );
	if  (!XLFileUtil::Exist(commandScriptFilename)) 
	{
		if ( XLStringUtil::baseext_nodotsmall(commandName) != "js" )
		{
			commandScriptFilename += ".js";
			if  (!XLFileUtil::Exist(commandScriptFilename)) 
			{//ファイルがない
				return false;
			}
		}
	}
	return true;
}

//Commandの実行 失敗したら例外を変えす
string ScriptManager::FireCommand(const CallbackPP& callback , const string & commandName,const list<string>& args)
{
	string commandScriptFilename = MainWindow::m()->GetConfigBasePath("/webroot/user/script_command/" + commandName );
	if ( ! XLStringUtil::checkSafePath(commandName) )
	{//危険なファイルなのでないことにする
		throw XLEXCEPTION("コマンドファイル " << commandName << "は、無効なファイル名を含んでいます。");
	}
	if  (!XLFileUtil::Exist(commandScriptFilename)) 
	{
		if ( XLStringUtil::baseext_nodotsmall(commandName) != "js" )
		{
			commandScriptFilename += ".js";
			if  (!XLFileUtil::Exist(commandScriptFilename)) 
			{//ファイルがない
				throw XLEXCEPTION("コマンドファイル " << commandName << "が存在しません。");
			}
		}
	}

	ScriptRunner runner( false );
	runner.LoadScript(commandScriptFilename);
	const string result = runner.callFunction("main",args);

	this->ActionEnd( callback );
	return result;
}

//Actionの実行 失敗したら例外を変えす actionとはこんな名前の家電を操作してほしいなという指示。その結果 command が呼ばれる
void ScriptManager::FireElec(const CallbackPP& callback , const string & elecType1,const string & elecType2)
{
	Fire f;
	f.FireByType(elecType1,elecType2);

	this->ActionEnd( callback );
}

//Actionの実行を文字列でやります 失敗したら例外を返す 
void ScriptManager::FireElecString(const CallbackPP& callback , const string & str)
{
	Fire f;
	f.FireByString(str);
	this->ActionEnd( callback );
}

void ScriptManager::SensorConfigUpdateNow(float * retTemp,float *retLumi,float *retSound)
{
	MainWindow::m()->Sensor.getSensorNow(retTemp,retLumi,retSound);
}


//音声認識した結果
void ScriptManager::VoiceRecogntion(const CallbackPP& callback)
{
	try
	{
		callback.fire();
	}
	catch(XLException &e)
	{
		ERRORLOG(string("") + "スクリプト実行時にエラー:" << e.what() );
	}
}

//トリガーの実行
void ScriptManager::FireTrigger(const CallbackPP& callback)
{
	try
	{
		callback.fire();
	}
	catch(XLException &e)
	{
		ERRORLOG(string("") + "スクリプト実行時にエラー:" << e.what() );
	}
}

//喋り終わった時
void ScriptManager::SpeakEnd(const CallbackPP& callback,const string& text)
{
	try
	{
		callback.fire();
	}
	catch(XLException &e)
	{
		ERRORLOG("スクリプト実行時にエラー:" << e.what()  );
	}
}


//家電制御が終わった時
void ScriptManager::ActionEnd(const CallbackPP& callback)
{
	try
	{
		callback.fire();
	}
	catch(XLException &e)
	{
		ERRORLOG(string("") + "スクリプト実行時にエラー:" << e.what() );
	}
}

//トリガーリロード
void ScriptManager::ReloadTrigger(const time_t& now)
{
	return this->WebScript.ReloadTrigger(now);
}


//webからアクセスがあった時
bool ScriptManager::WebAccess(const string& path,const XLHttpHeader& httpHeaders,map<string,string>* cookieArray,WEBSERVER_RESULT_TYPE* result,string* responsString)
{
	return this->WebScript.WebAccess(path,httpHeaders,cookieArray,result,responsString);
}
#endif //WITH_CLIENT_ONLY_CODE