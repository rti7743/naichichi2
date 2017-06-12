#pragma once
#include "XLHttpHeader.h"

class ActionImplement
{
public:
	ActionImplement(void);
	~ActionImplement(void);

	static string Telnet(const string& host,const string& wait,const string& send,const string& recv);
	static string HttpGet(const string& url,const map<string,string>& header,unsigned int timeoutS);
	static string HttpPost(const string& url,const string& postdata,const map<string,string>& header,unsigned int timeoutS);
	static string ExecuteLocal(const string& command);
	static bool ExecuteRemote(const string& host,const string& key,const string& command,const string& args);
	static bool OpenWeb(const string& targetpc,const string& command);
	static bool SendKeydown(const string& targetpc,const string& windowname,int key , int keyoption);
	static bool SendMessage(const string& targetpc,const string& windowname,int message,int wparam,int  lparam);
	static bool MSleep(const string& targetpc,unsigned int mtime);
	static string SSTPSend11(const string& host,const string& message);
	static bool WolSend(const string& macaddress,unsigned int port,const string& boadcastip);

	//SSDPの結果を格納する構造体
	struct SSDPStruct{
		string location;
		string usn;
		string uuid;
		string st;
	};
	static bool UpnpSearchAll(vector<ActionImplement::SSDPStruct>* outRet,const string& findstr,unsigned int timeoutS);
	static bool UpnpSearchOne(ActionImplement::SSDPStruct* outRet,const string& findstr,unsigned int timeoutS);
	static bool UpnpSearchUUID(ActionImplement::SSDPStruct* outRet,const string& finduuid,unsigned int timeoutS);
	static bool UpnpSearchXML(ActionImplement::SSDPStruct* outSSDP,string* outXML,const string& headerstr,const string& xmlstr,unsigned int timeoutS);

	static bool TwitterTweet(const string& messages);
	static void TwitterHomeTimeLine(const string& screen_name,vector< map<string,string>* >* outTimelineMapVec);
	static void TwitterUserTimeLine(const string& screen_name,vector< map<string,string>* >* outTimelineMapVec);

	//読み上げ処理
	static void ToSpeak(const string& tospeakPrefix );
	//読み上げ処理
	static void ToSpeak(const string& tospeak_select,const string& tospeak_string,const string& tospeak_mp3 );

private:
	static void UpnpHeaderToSSDPStruct(const XLHttpHeader& header,ActionImplement::SSDPStruct *outSsdp);
};

