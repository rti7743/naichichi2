#pragma once



class SystemMisc
{
public:
	//オーディオ入出力の音量調節
	static void SetMicVolume(int newValue100);
	static void SetSpeakerVolume(int newValue100);

	//マイクの状態のチェック
	//1			正常
	//0			準備できてません
	//-1		何も入力が受け取れない
	static int CheckMic();

	//NTPで時間を合わせる
	static void SetNTPDate();


	//ネットワークの設定
	static void SetIPAddress(const string& ipaddress,const string& netmask,const string& gateway,const string& dns,const string& w_ipaddress,const string& w_netmask,const string& w_gateway,const string& w_dns,const string& ssid,const string& password,const string& key_mgmt);
	//ネットワークの設定てせifdownuoする
	static void NetworkRestart(const string& ipaddress,const string& w_ipaddress);
	//wifiデバイスだっけ？
	static bool IsWifiDevice();
	//コマンド実行
	static string PipeExec(const string& exec);
	//sshuserさんのパスワード変更
	static void SetSSHUserPassword(const string& password);
	//起動時にネットワークの設定がされていなければ設定してあげます。
	static void NetworkInit(const string& ipaddress,const string& netmask,const string& gateway,const string& dns);

	static bool IsBeagleboneOld();
	static bool IsBeagleboneBlack();
	static bool IsIRR();
	static bool IsEndel();

	static string MakePassword(const string& usermail,const string& password);
	static string MakeWebAPIKey();


#if _MSC_VER
	static map<string,string> ArgsToMap(const char* lpszCmdLine);
#else
	static map<string,string> ArgsToMap(int argc, const char **argv);
#endif

	static string SupportDump();


	static string GetModel();

	static string GetOSVersion();

	//subjectの取得
	static string SendmailGetSubject(const string& tplString,const map<string,string>& arr);

	//メールテンプレートから本文の取得
	static string SendmailGetBody(const string& tplString,const map<string,string>& arr);

	//簡易メール送信(unixだけ)
	static void Sendmail(const string& from,const string& to,const string& supportbcc,const string& subject,const string& body);

	//多重起動していない、単体でのブートであるかどうか。
	static bool IsDoubleBoot();

	//eth0が取得できない。DHCPはいきてるの？喝入れをする
	static void NetworkDHCP_DieCheck(const string& ipaddress);

	struct ProcessSt{
		unsigned int pid;
		unsigned int parentpid;
		string processname;
		string args;
		string extraname;
	};
	static vector<SystemMisc::ProcessSt> ProcessList();

	//音を再生する.
	static void Play(const string & filename);

	//音を停止する.
	static void StopPlay();

	//指定したpidのプロセスをkillする
	static bool KillProces(unsigned int pid);

	static void Import(string filename);
	static string Export();
	static bool WaitForIPAddress(int waitsec);
	static bool WaitForMicSetup(int waitsec);

	//CPUのパフォーマンス設定 1=最大 0=オンデマンド
	static void SetCPUFreq(int type);

	//FHC端末として、環境のセットアップをします.
	static bool SettingFHCBoxEnvSetup();

	//UUIDの生成
	static string makeUUID();

	//UUIDのチェック
	static bool checkUUID(const string& uuid);
	//フリーモードのUUID生成
	static std::string makeUUIDFree();

	//mDNSのセットアップ
	static bool setup_mDNS();

	//LANケーブルがつながっているかどうか？
	static bool is_LinkDetected(const string& eth);

	//IPアドレスの取得
	static map<string,string> GetIPAddressMap() ;
	//マックアドレスを取得
	static string GetMacaddress(const string& eth,const string& sep=".");
	//マックアドレスを vector<uchar>で
	static vector<unsigned char> GetMacaddressVec(const string& eth);
};
