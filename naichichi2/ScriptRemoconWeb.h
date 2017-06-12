#pragma once

//HTML Webリモコン画面を出すにあたってのユーティリティ集 
class ScriptRemoconWeb 
{
public:
	//remocon の index status を json で返す.
	static string DrawJsonStatus(const map<string,string>& configmap,const string & appendOption) ;
	//remocon の index を html で返す
	static string DrawHTML(const map<string,string>& configmap,const string& htmlsrc ,time_t now , bool is_local , bool is_link, bool enableMultiRoom,bool is_showAD);
	//remocon の機材の id をソートされた順番で返す
	static vector<unsigned int> getElecID_Array(const map<string,string>& configmap,bool isGetHiddenElec) ;
	//remocon の機材のアクション id をソートされた順番で返す
	static vector<unsigned int> getElecActionID_Array(const map<string,string>& configmap,const string& elec,unsigned int* elecID,bool isGetHiddenElec) ;
	static string getElecKeyValue(const map<string,string>& configmap,unsigned int key1,const string& name ,const string& def = "") ;
	static string getActionKeyValue(const map<string,string>& configmap,unsigned int key1,unsigned int key2,const string& name ,const string& def = "" ) ;
	static string getElecActionPrefix(const map<string,string>& configmap,const string& type1, const string& type2);
	static string zeroToBar(const string& num);
	//特定の値をjsonでまるっと.
	static string ResultJsonStatus(const string& prefix);
	//キー値の最大値を求める.
	static int GetMaxID(const map<string,string> &configmap,const string& strip);

	//tpl読み込み static
	static string OpenTPL(const XLHttpHeader& httpHeaders,const string& tplname)  ;
	//webrootからのパスを得る static
	static string GetWebRootPath(const string& path) ;
	//エラーを返す static
	static string ResultError(int code ,const string& msg)  ;
	//設定画面のページへのURLを設定し直します
	static bool SettingPageIPAddressOverriade();
	//UUIDが設定されていなければ設定しなおします
	static void SettingUUIDOverriade();

	//webapiキーが設定されていなければ設定しなおします
	static void SettingWebAPIKeyOverriade();
	//リファラーをチェックします。
	static bool checkCSRFToken(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result,string* responsString);
	//特定の家電のステータスを取得します.
	static string getElecStatus(const string& elecname) ;
	//家電名からキーに変換します
	static bool type2key(const map<string,string> &configmap ,const string& type1,const string& type2 , int* outkey1, int* outkey2)  ;
	static bool type2key(const map<string,string> &configmap ,const string& type1 , int* outkey1)  ;

	//htmlspecialchars して、置換.
	static string escape_replace(const string& html,const string& replaceString,const string& value )
	{
		
		return XLStringUtil::replace_low(html,replaceString,_A2U(XLStringUtil::htmlspecialchars(value)));
	}

	//言語を切り替えたときにするべき設定
	static void ChangeLangSetting(const string & system_lang);

	static void DeflagConfig(const string& prefix,const string& needkey);
	static void DeflagConfig(const string& prefix,const string& needkey,const string& prefix2,const string& needkey2);
	static int NewElecID() ;
	static int NewElecActionID(int eleckey);
	//機材の種類が重複していたらエラー
	static bool checkUniqElecType(int key1,const string& type) ;
	//機材操作の種類が重複していたらエラー
	static bool checkUniqElecActionType(int key1,int key2,const string& actiontype) ;
	//新しい機材の並び順を求める
	static int newOrderElec() ;
	//新しい機材操作の並び順を求める
	static int newOrderElecAction(int key1) ;
	static string sensorMaptoJson(const map<time_t,short>& map);
	static string sensorMaptoJsonScale(const map<time_t,short>& map,double scale);
	//認証
	static bool LocalAuth(const string& id,const string& password);
	//CSRFトークンの取得
	static string GetCSRFToken(const XLHttpHeader& httpHeaders);
	//jsonpかどうか
	static WEBSERVER_RESULT_TYPE SelectJsonPResult(const map<string,string>& request);
	//リモコンの副次的情報の取得
	static string remocon_get_append_info(const map<string,string>& configmap,time_t now);


private:
	//MACアドレスからUUIDを作成します.
	static string MakeMacaddressToUUID(const vector<unsigned char>& macAddress);
	static bool MakeMacaddressToUUIDCheck(const string& uuid);
	
	//リモコン画面のpopupmenuの幅を計算する.
	static int CalcPopupmenuWidthForPC(const string & itemCount);


};
