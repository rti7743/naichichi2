#pragma once

class MultiRoomUtil
{
public:
	MultiRoomUtil();
	virtual ~MultiRoomUtil();

#if WITH_CLIENT_ONLY_CODE==1
	static void Merge(const map<string,string>& json);
	static void changeAuthkey(const string& webapi_apikey);
	static void changeName(const string& name);
	static void changeAccount(const string& name);
#endif // WITH_CLIENT_ONLY_CODE==1

	static int NewMultiRoomID(const map<string,string>& configmap);
	static int NewMultiRoomOrder(const map<string,string>& configmap);
	//部屋の検索
	static int FindRoom(const string& search_uuid,const map<string,string>& configmap);

	//次の部屋を求める.
	static int getNextRoom(const string my_uuid
		,const map<string,string>& configmap
		,bool isNext
		) ;
	//multiroom の index status を json で返す.
	static string DrawJsonStatus(const string my_uuid
			,const map<string,string>& configmap
			,bool withAuthkey
			) ;
	//認証コードの生成.
	static string makeOnetimeAuthcode(time_t now,const string& my_authkey,const string& your_uuid);
	//マルチルームが有効かどうか調べる
	static bool isMultiRoomEnable(const map<string,string>& configmap);


};
