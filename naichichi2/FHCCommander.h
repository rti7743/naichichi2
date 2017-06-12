#pragma once

//Windows用ユーティリティ FHCCommanderと通信する.
class FHCCommander
{
public:
	static bool checkAuth(const string& host , const string& key );
	static bool Play(const string& host , const string& key , const string& musicFilename );
	static bool Execute(const string& host , const string& key , const string& command,const string& args);

	static string reparseHost(const string& host);

};
