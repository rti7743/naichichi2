//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#include "common.h"
#include "MultiRoomUtil.h"
#include "MainWindow.h"
#include "XLHttpSocket.h"

MultiRoomUtil::MultiRoomUtil()
{
}
MultiRoomUtil::~MultiRoomUtil()
{
}


#if WITH_CLIENT_ONLY_CODE==1
void MultiRoomUtil::Merge(const map<string,string>& json)
{
	const string myself_uuid = MainWindow::m()->Httpd.GetUUID();
	for(auto jit = json.begin(); jit != json.end(); jit++)
	{
		if(jit->first.find("multiroom_") != 0)
		{
			continue;
		}
		if( jit->first.find("_uuid") == string::npos)
		{
			continue;
		}
		int key=0;
		sscanf(jit->first.c_str(),"multiroom_%d_uuid",&key);
		if (key <= 0)
		{
			continue;
		}
		string your_prefix = "multiroom_" + num2str(key) ;
		string your_uuid = mapfind(json,your_prefix + "_uuid");
		string your_url = mapfind(json,your_prefix + "_url");
		string your_authkey = mapfind(json,your_prefix + "_authkey");
		string your_name = mapfind(json,your_prefix + "_name");
		string your_order = mapfind(json,your_prefix + "_order");
		string your_account = mapfind(json,your_prefix + "_account");

		if (your_uuid.empty() 
		||  your_url.empty() 
		||  your_authkey.empty() 
		){
			ERRORLOG("data broken! prefix:" << your_prefix << " uuid:" << your_uuid << " url:" << your_url << " authkey:" << your_authkey << " name:" << your_name);
			continue;
		}
		if (myself_uuid == your_uuid)
		{//自分自身
			continue;
		}

		const auto configmap = MainWindow::m()->Config.FindGetsToMap("multiroom_",false);
		int my_key = FindRoom(your_uuid,configmap);
		if (my_key <= 0)
		{//新規
			my_key = NewMultiRoomID(configmap);
			your_order = num2str(NewMultiRoomOrder(configmap));
		}
		string my_prefix = "multiroom_" + num2str(my_key) ;
		MainWindow::m()->Config.Set(my_prefix+"_uuid",   your_uuid);
		MainWindow::m()->Config.Set(my_prefix+"_url" ,   your_url);
		MainWindow::m()->Config.Set(my_prefix+"_authkey",your_authkey);
		MainWindow::m()->Config.Set(my_prefix+"_name",   your_name);
		MainWindow::m()->Config.Set(my_prefix+"_order",  your_order);
		MainWindow::m()->Config.Set(my_prefix+"_account",your_account);
	}
}

void MultiRoomUtil::changeAuthkey(const string& webapi_apikey)
{
	const string myself_uuid = MainWindow::m()->Httpd.GetUUID();
	if (! SystemMisc::checkUUID(myself_uuid))
	{
		NOTIFYLOG("UUIDがありません!");
		return ;
	}

	const auto configmap = MainWindow::m()->Config.FindGetsToMap("multiroom_",false);
	int key = FindRoom(myself_uuid,configmap);
	if(key <= 0)
	{
		return ;
	}
	//データの変更
	string prefix = "multiroom_" + num2str(key);
	MainWindow::m()->Config.Set(prefix + "_authkey",webapi_apikey);

	//authkeyを変えたことを通知
	MainWindow::m()->MultiRoom.startSync();
}
void MultiRoomUtil::changeName(const string& name)
{
	const string myself_uuid = MainWindow::m()->Httpd.GetUUID();
	if (! SystemMisc::checkUUID(myself_uuid))
	{
		NOTIFYLOG("UUIDがありません!");
		return ;
	}

	const auto configmap = MainWindow::m()->Config.FindGetsToMap("multiroom_",false);
	int key = FindRoom(myself_uuid,configmap);
	if(key <= 0)
	{
		return ;
	}
	//データの変更
	string prefix = "multiroom_" + num2str(key);
	MainWindow::m()->Config.Set(prefix + "_name",name);

	//authkeyを変えたことを通知
	MainWindow::m()->MultiRoom.startSync();
}

void MultiRoomUtil::changeAccount(const string& account)
{
	const string myself_uuid = MainWindow::m()->Httpd.GetUUID();
	if (! SystemMisc::checkUUID(myself_uuid))
	{
		NOTIFYLOG("UUIDがありません!");
		return ;
	}

	const auto configmap = MainWindow::m()->Config.FindGetsToMap("multiroom_",false);
	int key = FindRoom(myself_uuid,configmap);
	if(key <= 0)
	{
		return ;
	}
	//データの変更
	string prefix = "multiroom_" + num2str(key);
	MainWindow::m()->Config.Set(prefix + "_account",account);

	//authkeyを変えたことを通知
	MainWindow::m()->MultiRoom.startSync();
}
#endif // WITH_CLIENT_ONLY_CODE==1



int MultiRoomUtil::FindRoom(const string& search_uuid,const map<string,string>& configmap)
{
	for(auto it = configmap.begin(); it != configmap.end(); it++)
	{
		if(it->first.find("multiroom_") != 0)
		{
			continue;
		}
		if( it->first.find("_uuid") == string::npos)
		{
			continue;
		}
		int key=0;
		sscanf(it->first.c_str(),"multiroom_%d_uuid",&key);
		if (key <= 0)
		{
			continue;
		}
		if(search_uuid == it->second)
		{
			return key;
		}
	}
	//見つからない.
	return 0;
}

int MultiRoomUtil::NewMultiRoomID(const map<string,string>& configmap)
{
	int max=0;
	for(auto it = configmap.begin(); it != configmap.end(); it++)
	{
		if(it->first.find("multiroom_") != 0)
		{
			continue;
		}
		if( it->first.find("_uuid") == string::npos)
		{
			continue;
		}
		int key=0;
		sscanf(it->first.c_str(),"multiroom_%d_uuid",&key);
		if (key <= 0)
		{
			continue;
		}
		if( key > max)
		{
			max = key;
		}
	}
	return max+1;
}

int MultiRoomUtil::NewMultiRoomOrder(const map<string,string>& configmap)
{
	int max=0;
	for(auto it = configmap.begin(); it != configmap.end(); it++)
	{
		if(it->first.find("multiroom_") != 0)
		{
			continue;
		}
		if( it->first.find("_uuid") == string::npos)
		{
			continue;
		}
		int key=0;
		sscanf(it->first.c_str(),"multiroom_%d_uuid",&key);
		if (key <= 0)
		{
			continue;
		}
		const string prefix = "multiroom_" + num2str(key) ;
		const int order = atoi(mapfind(configmap,prefix + "_order"));

		if( order > max)
		{
			max = order;
		}
	}
	return max+1;
}


struct orderSt
{
	int order;
	int key;
};
class Order_MultiRoomUtil
{
public:
	Order_MultiRoomUtil();
	Order_MultiRoomUtil(const string& my_uuid,const map<string,string>& configmap);
	virtual ~Order_MultiRoomUtil();
	const vector<orderSt*>& getSortVec() const
	{
		return this->SortVec;
	}
	const orderSt* getMyRoom() const
	{
		return this->MyRoom;
	}
private:
	vector<orderSt*> SortVec;
	orderSt* MyRoom;
};
Order_MultiRoomUtil::Order_MultiRoomUtil()
{
	this->MyRoom = NULL;
}
Order_MultiRoomUtil::Order_MultiRoomUtil(const string& my_uuid,const map<string,string>& configmap)
{
	this->MyRoom = NULL;
	for(auto it = configmap.begin(); it != configmap.end(); it++)
	{
		if(it->first.find("multiroom_") != 0)
		{
			continue;
		}
		if( it->first.find("_uuid") == string::npos)
		{
			continue;
		}
		int key=0;
		sscanf(it->first.c_str(),"multiroom_%d_uuid",&key);
		if (key <= 0)
		{
			continue;
		}
		orderSt* room = new orderSt;
		room->key = key;

		if(my_uuid == it->second)
		{
			this->MyRoom = room;
		}
		const string prefix = "multiroom_" + num2str(key) ;
		room->order = atoi(mapfind(configmap,prefix + "_order"));

		this->SortVec.push_back(room);
	}

	//orderで並べる.
	sort(this->SortVec.begin() , this->SortVec.end() , [](const orderSt* a,const orderSt* b){
		return a->order < b->order ;
	});
}

Order_MultiRoomUtil::~Order_MultiRoomUtil()
{
	for(auto it = this->SortVec.begin() ; it != this->SortVec.end(); it++)
	{
		delete *it;
	}
}

//次の部屋を求める.
int MultiRoomUtil::getNextRoom(const string my_uuid
	,const map<string,string>& configmap
	,bool isNext
	) 
{
	Order_MultiRoomUtil order(my_uuid,configmap);

	const orderSt* myRoom = order.getMyRoom();
	const vector<orderSt*> sortVec = order.getSortVec();

	if(myRoom == NULL)
	{//自分の部屋がない
		return 0;
	}
	if(sortVec.size()==1)
	{//自分の部屋しかない
		return 0;
	}

	int retroom=0;
	if(isNext)
	{
		for(auto it = sortVec.begin() ; it != sortVec.end(); it++)
		{
			if( *it == myRoom )
			{
				auto nextIT = it+1;
				if(nextIT == sortVec.end() )
				{//今が終端らしい.
					break;
				}
				retroom = (*nextIT)->key;
				break;
			}
		}
		if(retroom == 0)
		{//リスト先頭を返す
			auto it = sortVec.begin();
			retroom = (*it)->key;
		}
	}
	else
	{
		for(auto it = sortVec.rbegin() ; it != sortVec.rend(); it++)
		{
			if( *it == myRoom )
			{
				auto nextIT = it+1;
				if(nextIT == sortVec.rend() )
				{//今が終端らしい.
					break;
				}
				retroom = (*nextIT)->key;
				break;
			}
		}
		if(retroom == 0)
		{//リスト後方を返す
			auto it = sortVec.rbegin();
			retroom = (*it)->key;
		}
	}

	return retroom;
}

//multiroom の index status を json で返す.
string MultiRoomUtil::DrawJsonStatus(const string my_uuid
		,const map<string,string>& configmap
		,bool withAuthkey
		) 
{
	

	Order_MultiRoomUtil order(my_uuid,configmap);

	const orderSt* myRoom = order.getMyRoom();
	const vector<orderSt*> sortVec = order.getSortVec();

	string jsonstring;
	for(auto it = sortVec.begin() ; it != sortVec.end(); it++)
	{
		const string prefix = "multiroom_" + num2str( (*it)->key ) ;
		jsonstring += string(",") + XLStringUtil::jsonescape(prefix+"_uuid") + ": " 
			+ _A2U(XLStringUtil::jsonescape(mapfind(configmap,prefix+"_uuid") )) ;
		jsonstring += string(",") + XLStringUtil::jsonescape(prefix+"_name") + ": " 
			+ _A2U(XLStringUtil::jsonescape(mapfind(configmap,prefix+"_name") )) ;
		jsonstring += string(",") + XLStringUtil::jsonescape(prefix+"_url") + ": " 
			+ _A2U(XLStringUtil::jsonescape(mapfind(configmap,prefix+"_url") )) ;
		jsonstring += string(",") + XLStringUtil::jsonescape(prefix+"_order") + ": " 
			+ _A2U(XLStringUtil::jsonescape(mapfind(configmap,prefix+"_order") )) ;
		if (withAuthkey)
		{
			jsonstring += string(",") + XLStringUtil::jsonescape(prefix+"_authkey") + ": " 
				+ _A2U(XLStringUtil::jsonescape(mapfind(configmap,prefix+"_authkey") )) ;
			jsonstring += string(",") + XLStringUtil::jsonescape(prefix+"_account") + ": " 
				+ _A2U(XLStringUtil::jsonescape(mapfind(configmap,prefix+"_account") )) ;
		}
	}
	return jsonstring;
}


//認証コードの生成.
string MultiRoomUtil::makeOnetimeAuthcode(time_t now,const string& my_authkey,const string& your_uuid)
{
	time_t one_minute = now/60; //1分有効
	return XLStringUtil::sha1(my_authkey + "@" + your_uuid + "@" + num2str(one_minute));
}

//マルチルームが有効かどうか調べる
bool MultiRoomUtil::isMultiRoomEnable(const map<string,string>& configmap)
{
	const string myself_uuid = mapfind(configmap,"httpd__uuid");
	if (! SystemMisc::checkUUID(myself_uuid))
	{//UUIDがない
		return false;
	}

	for(auto it = configmap.begin(); it != configmap.end(); it++)
	{
		if(it->first.find("multiroom_") != 0)
		{
			continue;
		}
		if( it->first.find("_uuid") == string::npos)
		{
			continue;
		}
		if(it->second != myself_uuid)
		{
			return true;
		}
	}
	return false;
}

