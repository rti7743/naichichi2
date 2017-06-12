//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#pragma once

//機器に動作信号を送る.
class Fire
{
public:
	Fire();
	virtual ~Fire();
	
	bool FireByString(const string& str);
	bool FireByType(const string& type1,const string& type2) ;
	bool FireAction(int key1,int key2);

	//コマンドを実効する
	string run_commandE(const string& execcommand,const string& execargs1,const string& execargs2,const string& execargs3,const string& execargs4,const string& execargs5,string* appendSTDOUT);
	bool run_command(const string& execcommand,const string& execargs1,const string& execargs2,const string& execargs3,const string& execargs4,const string& execargs5,string* appendSTDOUT);
	//マクロを実行する
	void run_macroE(const map<string,string>& request , const string& prefix);
	bool run_macro(const map<string,string>& request , const string& prefix);
	//循環参照になって無限ループにならないようにマクロの上限をチェックする
	bool check_macroE(const string& type1,const string& type2 , int* nestcount)  const;

	int remocon_ir_fire(const string& exec_ir);
	bool remocon_ir_leaning(const string& exec_ir);

	void run_multiroom(const string& uuid,const string& elec,const string& action,string* outStdout);
	bool run_netdev(const string& elec,const string& action,const string& value);
	bool run_sip(const string& sip_action_type,const string& sip_call_number);

	string getError() const 
	{
		return this->ErrorMessage;
	}
private:
	bool fireStringAction(const string& type1,const string& type2 , int* nestcount) ;
	void fireAction(int key1,int key2,int* nestcount);
	void run_macroE(const map<string,string>& request , const string& prefix,int* nestcount);
	void run_recongpause(const string& recongpause_type);
	//家電タイマーのセット(おやす○タイマー)
	void run_set_electimer(const string& timer_type,unsigned int key1,unsigned int key2);
	//Configを変更した場合
	void ConfigUpdated();

	string ErrorMessage;
};

#endif //WITH_CLIENT_ONLY_CODE