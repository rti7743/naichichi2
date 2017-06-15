//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_SERVER_ONLY_CODE
#include "common.h"
#include "MainWindow.h"
#include "ServerScript.h"
#include "XLFileUtil.h"
#include "XLStringUtil.h"
#include "ActionImplement.h"
#include "XLHttpSocket.h"
#include "ResetConfig.h"
#include "SystemMisc.h"
#include "ScriptRemoconWeb.h"
#include "MultiRoomUtil.h"
#include "SystemMisc.h"
#include "XLGZip.h"


ServerScript::ServerScript()
{
}
ServerScript::~ServerScript()
{
	DEBUGLOG("stop...");

	DestoryScript();

	DEBUGLOG("done");
}
void ServerScript::Create()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	this->DBName = this->GetDataPath("servercache.dat");
	this->Load(this->DBName);

	const string weAreHereDBName = this->GetDataPath("wearehere.dat");
	this->WeAreHere.Create(weAreHereDBName);
}

void ServerScript::DestoryScript()
{
	lock_guard<mutex> al(this->lock);
	for(auto it = this->UserMaps.begin() ; it != this->UserMaps.end() ; ++it)
	{
		delete it->second;
	}
	this->UserMaps.clear();
}


void ServerScript::Load(const string& filename)
{
	lock_guard<mutex> al(this->lock);
	auto fp = fopen(filename.c_str(),"rb");
	if (!fp)
	{
		return ;
	}
	ERRORLOG("server read.");

	vector<char> buffer(65535+1);
	while(!feof(fp))
	{
		buffer[0] = 0;
		fgets(&buffer[0],65535,fp);
		buffer[65535] = 0;

		auto vec = XLStringUtil::split_vector("\t",XLStringUtil::chop(&buffer[0]));
		if (vec.size() <= 3)
		{
			continue;
		}
		
		if (vec[0].size() >= 1) vec[0] = vec[0].substr(1);
		if (vec[1].size() >= 1) vec[1] = vec[1].substr(1);
		if (vec[2].size() >= 1) vec[2] = vec[2].substr(1);
		if (vec[3].size() >= 1) vec[3] = vec[3].substr(1);

		if (vec.size() <= 4)
		{
			vec.push_back("0");
		}
		else
		{
			if (vec[4].size() >= 1) vec[4] = vec[4].substr(1);
		}

		unsigned int id = atoi( vec[0] );
		if (id <= 0)
		{
			continue;
		}

		struct User * user  = new User;
		user->id                    = id;
//		user->name                  = vec[1];
		user->password              = vec[2];
		user->uuid                  = vec[3];
		user->isPay                = stringbool(vec[4]);
		user->status                = "";
		user->userLoginTime         = 0;
		user->clientConnectTime     = 0;
		user->isServerStatusUpdate  = false;
		user->isLongPoolingNow      = false;
		user->authErrorCount        = 0;
		XLFileUtil::mkdirP(GetUserPath(  id , "" ));
		XLFileUtil::mkdirP(GetUserPath(  id , "/config/" ));
		XLFileUtil::mkdirP(GetUserPath(  id , "/image/" ));
		XLFileUtil::mkdirP(GetUserPath(  id , "/temp/" ));
		XLFileUtil::mkdirP(GetUserPath(  id , "/remocon/" ));
		XLFileUtil::mkdirP(GetUserPath(  id , "/actionicon/" ));

		const string configfilename = GetUserPath(  id , "/config/config.conf" );
		if ( XLFileUtil::Exist( configfilename ) )
		{
			user->config.loadConfig( configfilename ) ;
		}
		//UUIDのチェック
		if ( checkUUID(user->uuid ))
		{//UUIDチェックが通るものは、ハード購入者だから、購入フラグを立てる.
			user->isPay = true;
		}
		if (user->isPay)
		{
			ERRORLOG(id << " " << vec[1] << " payed");
		}
		else
		{
			ERRORLOG(id << " " << vec[1] << " NOPAY");
		}

		const auto httpd__uuid = user->config.Get("httpd__uuid","");
		if(httpd__uuid!="" && user->uuid!=httpd__uuid)
		{
			ERRORLOG("bad uuid missmatch!" << user->uuid);
		}


		this->UserMaps.insert(pair<string , struct User*>(vec[1],user));
	}

	fclose(fp);
}


void ServerScript::Write(const string& filename)
{
	const string tempfilename = filename + ".tmp";
	FILE * fp = fopen(tempfilename.c_str() , "wb");
	if (!fp)
	{
		return ;
	}
	ERRORLOG("server write.");
	for(auto it = UserMaps.begin() ; it != UserMaps.end() ; ++it )
	{
		string buffer = "@" +num2str( it->second->id )
					+ "\t@" + it->first
					+ "\t@" + it->second->password
					+ "\t@" + it->second->uuid
					+ "\t@" + (it->second->isPay ? "1" : "0")
					+ "\n";
		ERRORLOG(buffer);
		fwrite(buffer.c_str() , sizeof(char) , buffer.size() , fp);
	}
	fclose(fp);

	int year,month,day;
	XLStringUtil::timetoint(time(NULL),&year,&month,&day,NULL,NULL,NULL);

	XLFileUtil::copy(filename , filename + "." +num2str(year)+"_"+num2str(month)+"_"+num2str(day) + ".bak" );
	XLFileUtil::copy(tempfilename , filename);
	XLFileUtil::del(tempfilename);
}

string ServerScript::GetUserPath(unsigned int id,const string& path) const
{
	return MainWindow::m()->GetConfigBasePath("./server/user/" + num2str(id) + "/" + path);
}
string ServerScript::GetDataPath(const string& path) const
{
	return MainWindow::m()->GetConfigBasePath("./server/data/" + path);
}
string ServerScript::GetWebRootPath(const string& path) const
{//ふつーのwebrootと共有します。
	return MainWindow::m()->GetConfigBasePath("./webroot/" + path);
}




string ServerScript::Server_auto_pooling(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	const unsigned int id = atou( mapfind(cookieArray,"id") );
	const string name = mapfind(cookieArray,"name");
	const string link = httpHeaders.getCookieHeader("naichichi2");
	if (  mapfind(cookieArray,"link") != "1" )
	{
		ERRORLOG("no link session link(" << link << ")/id("<<id<<")/name("<<name<<")のlinkは存在しません.");
		return "BAD\tno link session";
	}
	if (id <= 0)
	{
		ERRORLOG("bad cookie value   link(" << link << ")/id("<<id<<")/name("<<name<<")のidはゼロ以下です.");
		return "BAD\tbad cookie value";
	}

	bool * updateFlagAddress = NULL;
	{
		lock_guard<mutex> al(this->lock);
		const auto it = UserMaps.find(name);
		if (it == UserMaps.end() )
		{
			return "BAD";
		}
		const time_t now = time(NULL);
		it->second->clientConnectTime = now;

		const string r = it->second->status;
		if (!r.empty())
		{
			it->second->status = "";
			it->second->isServerStatusUpdate = false;
			it->second->isLongPoolingNow = false;
			return r;
		}

		if (it->second->isLongPoolingNow)
		{//別のスレッドでロングポーリング中？
			return "wait\t10a";
		}
		it->second->isLongPoolingNow = true;

		updateFlagAddress = &it->second->isServerStatusUpdate;
	}

	//ステータスが変更があるかどうか10秒程度待機する
	for(int i = 0 ; i < 10 ; ++i )
	{
		if ( *updateFlagAddress ) break; //boolだからアトミックを期待しているw
		SecSleep(1);
	}

	//アップデートがあったらしいので見てみる
	{
		lock_guard<mutex> al(this->lock);
		const auto it = UserMaps.find(name);
		if (it == UserMaps.end() )
		{//イミフ さっきはデータが有ったのに。
			return "BAD";
		}
		//ロングポーリングを外す
		it->second->isLongPoolingNow = false;
		//アップデートがあった？
		if (! it->second->isServerStatusUpdate)
		{//ない 次のアップデートまで待つ
			return "wait\t0c";
		}

		//イベントを拾う
		const string r = it->second->status;
		if (!r.empty())
		{//ステータスを返す
			it->second->status = "";
			it->second->isServerStatusUpdate = false;
			return r;
		}
	}

	//他のスレッドがイベントを消化してしまったのかな・・・？
	return "wait\t1d";
}

string ServerScript::Server_auto_getlink(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	const map<string,string> get = httpHeaders.getGet();
	const map<string,string> post = httpHeaders.getPost();

	const string name = mapfind(post,"name");
	const string password = mapfind(post,"password");
	const string uuid = mapfind(post,"uuid");
	if (name.empty() || password.empty() || uuid.empty() )
	{
		ERRORLOG("baduser name password uuid のどれかが空です");
		return "BAD\tbad auth";
	}
	if (name != mapfind(get,"name"))
	{
		ERRORLOG("baduser GET(" << mapfind(get,"name") << ") とPOST(" << name << ")が一致しません。");
		return "BAD\tbad auth";
	}

	unsigned int id = 0;
	{
		lock_guard<mutex> al(this->lock);

		const auto it = UserMaps.find(name);
		if  (it == UserMaps.end() )
		{
			ERRORLOG("baduser ID(" << name << ")が存在しません");
			return "BAD\tbad auth";
		}
		if ( it->second->password != password )
		{
			ERRORLOG("baduser ID(" << name << ")のパスワードが一致しません");
			return "BAD\tbad auth";
		}
		if ( it->second->uuid != uuid )
		{
			ERRORLOG("baduser ID(" << name << ")のuuid("<<it->second->uuid<<"  /  "<<uuid<<")が一致しません");
			return "BAD\tbad auth(uuid)";
		}
		it->second->clientConnectTime = time(NULL);

		id = it->second->id;
	}
	(*cookieArray)["id"] = num2str(id);
	(*cookieArray)["name"] = name;
	(*cookieArray)["uuid"] = uuid;
	(*cookieArray)["link"] = "1";
	(*cookieArray)["__regen__"] = "1";

	ERRORLOG("ok ID(" << name << ")とgetlinkしました.");
	return "OK";
}

string ServerScript::Server_auto_syncconfig(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	
	const unsigned int id = atou( mapfind(cookieArray,"id") );
	const string name = mapfind(cookieArray,"name") ;
	const string uuid = mapfind(cookieArray,"uuid") ;
	const string link = httpHeaders.getCookieHeader("naichichi2");
	if (  mapfind(cookieArray,"link") != "1" )
	{
		ERRORLOG("no link session link(" << link << ")/id("<<id<<")/name("<<name<<")のlinkは存在しません.");
		return "BAD\tno link session";
	}
	const auto files = httpHeaders.getFilesPointer();

	if (id <= 0)
	{
		ERRORLOG("bad cookie value   link(" << link << ")/id("<<id<<")/name("<<name<<")のidはゼロ以下です.");
		return "BAD\tbad cookie value";
	}
	if (files->empty())
	{
		ERRORLOG("bad cookie value   link(" << link << ")/id("<<id<<")/name("<<name<<")はファイルを添付していません.");
		return "BAD\tbad file. (empty)";
	}
	
	//config ファイルをディスクに書き込みリロードする
	{	
		lock_guard<mutex> al(this->lock);

		const auto userIT = UserMaps.find(name);
		if (userIT == UserMaps.end() )
		{
			ERRORLOG("bad name????   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")の name は存在しません。(不思議なエラー).");
			return "BAD\tbad name????.";
		}
		if ( userIT->second->uuid != uuid )
		{
			ERRORLOG("bad uuid????   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")の uuid が一致しません。(動作中にuuidを書き換えられた可能性).");
			return "BAD\tbad name????.";
		}
		if ( userIT->second->id != id )
		{
			ERRORLOG("bad uuid????   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")の id が一致しません。 (不思議なエラー).");
			return "BAD\tbad name????.";
		}
		//ファイルを取得する
		const auto fileit = files->begin();
		if (fileit == files->end() )
		{
			ERRORLOG("bad cookie value   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")はファイルは正しくありません.");
			return "BAD\tbad file.";
		}
		const string fullpath = GetUserPath(  id , "/config/config.conf" );

		//解凍する
		XLGZip gzip;
		if (! gzip.gunzip( &fileit->second->data[0] , fileit->second->data.size()) )
		{
			ERRORLOG("bad cookie value   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")はファイルはgzipではありません.");
			return "BAD\tbad file(not gzip).";
		}
		//ディスクに書き込む
		XLFileUtil::write( fullpath , gzip.getData() ,  gzip.getSize() );

		//configを読み込む
		if ( ! userIT->second->config.loadConfig( fullpath ) )
		{
			ERRORLOG("bad cookie value   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")はconfigを読み込めませんでした.");
			return "BAD\tbad file(no config file).";
		}

		const auto httpd__uuid = userIT->second->config.Get("httpd__uuid","");
		if(uuid!=httpd__uuid)
		{
			ERRORLOG("bad cookie value   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")はconfigを読み込めませんでした.");
			return "BAD\tbad file(config file uuid missmatch).";
		}

		userIT->second->clientConnectTime = time(NULL);
		this->WeAreHere.Update(userIT->second->config);
	}

	//応答として img ファイル一覧を返す
	string r = "OK";
	XLFileUtil::findfile(GetUserPath(id,"/image/") , [&](const string& filename,const string& fullfilename) -> bool {

		r += "\t" + filename + "\t" + num2str(XLFileUtil::getfiletime(fullfilename) );
		return true;
	});

	ERRORLOG("ok  link(" << link << ")/id("<<id<<")/name("<<name<<")のconfigを取得しました.");
	return _A2U(r);
}

string ServerScript::Server_auto_syncimage(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	const map<string,string> request = httpHeaders.getRequest();
	const unsigned int id = atou( mapfind(cookieArray,"id") );
	const string name = mapfind(cookieArray,"name") ;
	const string uuid = mapfind(cookieArray,"uuid") ;
	const string link = httpHeaders.getCookieHeader("naichichi2");

	const auto files = httpHeaders.getFilesPointer();
	if (  mapfind(cookieArray,"link") != "1" )
	{
		ERRORLOG("no link session  link(" << link << ")/id("<<id<<")のlinkは存在しません.");
		return "BAD\tno link session";
	}

	if (id <= 0)
	{
		ERRORLOG("bad cookie value  link(" << link << ")/id("<<id<<")のidがo以下です.");
		return "BAD\tbad cookie value";
	}
	if (files->empty())
	{
		ERRORLOG("bad file. (empty)  link(" << link << ")/id("<<id<<")にはファイルが添付されていません.");
		return "BAD\tbad file. (empty)";
	}

	const string filename = mapfind(request,"filename");
	if (filename.empty() )
	{
		ERRORLOG("bad filename  link(" << link << ")/id("<<id<<")のfilename("<<filename<<")は空です.");
		return "BAD\tbad filename";
	}
	if (! XLStringUtil::checkSafePath(filename))
	{
		ERRORLOG("bad file path  link(" << link << ")/id("<<id<<")のfilename("<<filename<<")は危険です.");
		return "BAD\tbad file path";
	}

	//実在確認
	{	
		lock_guard<mutex> al(this->lock);

		const auto userIT = UserMaps.find(name);
		if (userIT == UserMaps.end() )
		{
			ERRORLOG("bad name????   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")の name は存在しません。(不思議なエラー).");
			return "BAD\tbad name????.";
		}
		if ( userIT->second->uuid != uuid )
		{
			ERRORLOG("bad uuid????   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")の uuid が一致しません。(動作中にuuidを書き換えられた可能性).");
			return "BAD\tbad name????.";
		}
		if ( userIT->second->id != id )
		{
			ERRORLOG("bad uuid????   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")の id が一致しません。 (不思議なエラー).");
			return "BAD\tbad name????.";
		}
		userIT->second->clientConnectTime = time(NULL);
	}

	//画像ファイルをディスクに書き込む
	const auto it = files->begin();
	if (it == files->end() )
	{
		ERRORLOG("bad file  link(" << link << ")/id("<<id<<")のfilename("<<filename<<")のデータを取得できませんでした(内部エラー).");
		return "BAD\tbad file.";
	}

	const string fullpath = GetUserPath(id,"/image/" + filename);
	XLFileUtil::write( fullpath , (it->second)->data);
	ERRORLOG("OK link(" << link << ")/id("<<id<<")のfilename("<<filename<<")を処理した.");

	return "OK";
}
string ServerScript::Server_auto_syncdelimage(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	const map<string,string> request = httpHeaders.getRequest();
	const unsigned int id = atou( mapfind(cookieArray,"id") );
	const string name = mapfind(cookieArray,"name") ;
	const string uuid = mapfind(cookieArray,"uuid") ;
	const string link = httpHeaders.getCookieHeader("naichichi2");

	if (  mapfind(cookieArray,"link") != "1" )
	{
		ERRORLOG("no link session link(" << link << ")/id("<<id<<")のlinkは存在しません.");
		return "BAD\tno link session";
	}
	if (id <= 0)
	{
		ERRORLOG("bad cookie value link(" << link << ")/id("<<id<<")のIDが0以下です.");
		return "BAD\tbad cookie value";
	}

	const string filename = mapfind(request,"filename");
	if (filename.empty() )
	{
		ERRORLOG("bad filename  link(" << link << ")/id("<<id<<")のファイル名("<<filename<<")は空です.");
		return "BAD\tbad filename";
	}
	if (! XLStringUtil::checkSafePath(filename))
	{
		ERRORLOG("bad file path  link(" << link << ")/id("<<id<<")のファイル名("<<filename<<")は危険です.");
		return "BAD\tbad file path";
	}

	//実在確認
	{	
		lock_guard<mutex> al(this->lock);

		const auto userIT = UserMaps.find(name);
		if (userIT == UserMaps.end() )
		{
			ERRORLOG("bad name????   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")の name は存在しません。(不思議なエラー).");
			return "BAD\tbad name????.";
		}
		if ( userIT->second->uuid != uuid )
		{
			ERRORLOG("bad uuid????   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")の uuid が一致しません。(動作中にuuidを書き換えられた可能性).");
			return "BAD\tbad name????.";
		}
		if ( userIT->second->id != id )
		{
			ERRORLOG("bad uuid????   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")の id が一致しません。 (不思議なエラー).");
			return "BAD\tbad name????.";
		}
		userIT->second->clientConnectTime = time(NULL);
	}

	//削除リクエスト
	const string fullpath = GetUserPath(id,"/image/" + filename);
	XLFileUtil::del(fullpath);
	ERRORLOG("OK link(" << link << ")/id("<<id<<")のファイル名("<<filename<<")を処理した.");

	return "OK";
}

string ServerScript::Type2Directory(const string& type) const
{
	if (type == "elecicon") return  "image";
	if (type == "remocon") return   "remocon";
	if (type == "actionicon") return   "actionicon";
	return "";
}

//実在確認
bool ServerScript::CheckUser(const string& link,const string& name,const string& uuid,unsigned int id,const time_t& now,string* message)
{
	//need lock
	//lock_guard<mutex> al(this->lock);

	const auto userIT = UserMaps.find(name);
	if (userIT == UserMaps.end() )
	{
		ERRORLOG("bad name????   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")の name は存在しません。(不思議なエラー).");
		*message = "bad name,can not found name.";
		return false;
	}
	if ( userIT->second->uuid != uuid )
	{
		ERRORLOG("bad uuid????   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")の uuid が一致しません。(動作中にuuidを書き換えられた可能性).");
		*message = "bad name,can not match uuid.";
		return false;
	}
	if ( userIT->second->id != id )
	{
		ERRORLOG("bad uuid????   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")の id が一致しません。 (不思議なエラー).");
		*message = "bad name,can not match id.";
		return false;
	}

	userIT->second->clientConnectTime = time(NULL);
	return true;
}



string ServerScript::Server_auto_v2syncconfig(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	
	const unsigned int id = atou( mapfind(cookieArray,"id") );
	const string name = mapfind(cookieArray,"name") ;
	const string uuid = mapfind(cookieArray,"uuid") ;
	const string link = httpHeaders.getCookieHeader("naichichi2");
	if (  mapfind(cookieArray,"link") != "1" )
	{
		ERRORLOG("no link session link(" << link << ")/id("<<id<<")/name("<<name<<")のlinkは存在しません.");
		return "BAD\tno link session";
	}
	const auto files = httpHeaders.getFilesPointer();

	if (id <= 0)
	{
		ERRORLOG("bad cookie value   link(" << link << ")/id("<<id<<")/name("<<name<<")のidはゼロ以下です.");
		return "BAD\tbad cookie value";
	}
	if (files->empty())
	{
		ERRORLOG("bad cookie value   link(" << link << ")/id("<<id<<")/name("<<name<<")はファイルを添付していません.");
		return "BAD\tbad file. (empty)";
	}
	
	//config ファイルをディスクに書き込みリロードする
	{	
		lock_guard<mutex> al(this->lock);

		const auto userIT = UserMaps.find(name);
		if (userIT == UserMaps.end() )
		{
			ERRORLOG("bad name????   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")の name は存在しません。(不思議なエラー).");
			return "BAD\tbad name????.";
		}
		if ( userIT->second->uuid != uuid )
		{
			ERRORLOG("bad uuid????   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")の uuid が一致しません。(動作中にuuidを書き換えられた可能性).");
			return "BAD\tbad name????.";
		}
		if ( userIT->second->id != id )
		{
			ERRORLOG("bad uuid????   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")の id が一致しません。 (不思議なエラー).");
			return "BAD\tbad name????.";
		}
		//ファイルを取得する
		const auto fileit = files->begin();
		if (fileit == files->end() )
		{
			ERRORLOG("bad cookie value   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")はファイルは正しくありません.");
			return "BAD\tbad file.";
		}
		const string fullpath = GetUserPath(  id , "/config/config.conf" );

		//解凍する
		XLGZip gzip;
		if (! gzip.gunzip( &fileit->second->data[0] , fileit->second->data.size()) )
		{
			ERRORLOG("bad cookie value   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")はファイルはgzipではありません.");
			return "BAD\tbad file(not gzip).";
		}
		//ディスクに書き込む
		XLFileUtil::write( fullpath , gzip.getData() ,  gzip.getSize() );

		//configを読み込む
		if ( ! userIT->second->config.loadConfig( fullpath ) )
		{
			ERRORLOG("bad cookie value   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")はconfigを読み込めませんでした.");
			return "BAD\tbad file(no config file).";
		}

		const auto httpd__uuid = userIT->second->config.Get("httpd__uuid","");
		if(uuid!=httpd__uuid)
		{
			ERRORLOG("bad cookie value   link(" << link << ")/id("<<id<<")/name("<<name<<")/uuid("<<uuid<<")はconfigを読み込めませんでした.");
			userIT->second->config.RemoveTree("");
			return "BAD\tbad file(config file uuid missmatch).";
		}

		userIT->second->clientConnectTime = time(NULL);
		this->WeAreHere.Update(userIT->second->config);
	}

	//応答として img ファイル一覧を返す
	string r = "OK";
	XLFileUtil::findfile(GetUserPath(id,"/image/") , [&](const string& filename,const string& fullfilename) -> bool {
		const time_t filetime = XLStringUtil::LocalTimetoGMT( XLFileUtil::getfiletime(fullfilename));
		r += "\t" + filename + "<>" + num2str( filetime ) + "<>" + "elecicon";
		return true;
	});
	XLFileUtil::findfile(GetUserPath(id,"/remocon/") , [&](const string& filename,const string& fullfilename) -> bool {
		const time_t filetime = XLStringUtil::LocalTimetoGMT( XLFileUtil::getfiletime(fullfilename));
		r += "\t" + filename + "<>" + num2str( filetime ) + "<>" + "remocon";
		return true;
	});
	XLFileUtil::findfile(GetUserPath(id,"/actionicon/") , [&](const string& filename,const string& fullfilename) -> bool {
		const time_t filetime = XLStringUtil::LocalTimetoGMT( XLFileUtil::getfiletime(fullfilename));
		r += "\t" + filename + "<>" + num2str( filetime ) + "<>" + "actionicon";
		return true;
	});

	ERRORLOG("ok  link(" << link << ")/id("<<id<<")/name("<<name<<")のconfigを取得しました.");
	return _A2U(r);
}


string ServerScript::Server_auto_v2syncfile(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	const map<string,string> request = httpHeaders.getRequest();
	const unsigned int id = atou( mapfind(cookieArray,"id") );
	const string link = httpHeaders.getCookieHeader("naichichi2");
	const string name = mapfind(cookieArray,"name") ;
	const string uuid = mapfind(cookieArray,"uuid") ;

	const auto files = httpHeaders.getFilesPointer();
	if (  mapfind(cookieArray,"link") != "1" )
	{
		ERRORLOG("no link session  link(" << link << ")/id("<<id<<")のlinkは存在しません.");
		return "BAD\tno link session";
	}

	if (id <= 0)
	{
		ERRORLOG("bad cookie value  link(" << link << ")/id("<<id<<")のidがo以下です.");
		return "BAD\tbad cookie value";
	}
	if (files->empty())
	{
		ERRORLOG("bad file. (empty)  link(" << link << ")/id("<<id<<")にはファイルが添付されていません.");
		return "BAD\tbad file. (empty)";
	}

	const string filename = mapfind(request,"filename");
	if (filename.empty() )
	{
		ERRORLOG("bad filename  link(" << link << ")/id("<<id<<")のfilename("<<filename<<")は空です.");
		return "BAD\tbad filename";
	}
	if (! XLStringUtil::checkSafePath(filename))
	{
		ERRORLOG("bad file path  link(" << link << ")/id("<<id<<")のfilename("<<filename<<")は危険です.");
		return "BAD\tbad file path";
	}
	
	//種類によって格納するディレクトリが違うので取得する.
	const string type = mapfind(request,"type") ;
	const string typeDirecptry = Type2Directory(type);
	if(typeDirecptry.empty())
	{
		ERRORLOG("bad type  link(" << link << ")/id("<<id<<")のfilename("<<filename<<")のtype("<<type<<")は存在しません.");
		return "BAD\tbad file type";
	}

	//実在確認
	{	
		lock_guard<mutex> al(this->lock);
		string message;
		if ( ! CheckUser(link,name,uuid,id,time(NULL),&message) )
		{
			return "BAD\t" + message;
		}
	}

	//添付ファイルをディスクに書き込む
	const auto it = files->begin();
	if (it == files->end() )
	{
		ERRORLOG("bad file  link(" << link << ")/id("<<id<<")のfilename("<<filename<<")のtype("<<type<<")のデータを取得できませんでした(内部エラー).");
		return "BAD\tbad file.";
	}

	
	const string fullpath = GetUserPath(id,"/" + typeDirecptry + "/" + filename);
	XLFileUtil::write( fullpath , (it->second)->data);
	ERRORLOG("OK link(" << link << ")/id("<<id<<")のfilename("<<filename<<")のtype("<<type<<")を書き込みました.");

	return "OK";
}

string ServerScript::Server_auto_v2syncdel(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	const map<string,string> request = httpHeaders.getRequest();
	const unsigned int id = atou( mapfind(cookieArray,"id") );
	const string link = httpHeaders.getCookieHeader("naichichi2");
	const string name = mapfind(cookieArray,"name") ;
	const string uuid = mapfind(cookieArray,"uuid") ;

	if (  mapfind(cookieArray,"link") != "1" )
	{
		ERRORLOG("no link session link(" << link << ")/id("<<id<<")のlinkは存在しません.");
		return "BAD\tno link session";
	}
	if (id <= 0)
	{
		ERRORLOG("bad cookie value link(" << link << ")/id("<<id<<")のIDが0以下です.");
		return "BAD\tbad cookie value";
	}

	const string filename = mapfind(request,"filename");
	if (filename.empty() )
	{
		ERRORLOG("bad filename  link(" << link << ")/id("<<id<<")のファイル名("<<filename<<")は空です.");
		return "BAD\tbad filename";
	}
	if (! XLStringUtil::checkSafePath(filename))
	{
		ERRORLOG("bad file path  link(" << link << ")/id("<<id<<")のファイル名("<<filename<<")は危険です.");
		return "BAD\tbad file path";
	}

	//種類によって格納するディレクトリが違うので取得する.
	const string type = mapfind(request,"type") ;
	const string typeDirecptry = Type2Directory(type);
	if(typeDirecptry.empty())
	{
		ERRORLOG("bad type  link(" << link << ")/id("<<id<<")のfilename("<<filename<<")のtype("<<type<<")は存在しません.");
		return "BAD\tbad file type";
	}

	//実在確認
	{	
		lock_guard<mutex> al(this->lock);
		string message;
		if ( ! CheckUser(link,name,uuid,id,time(NULL),&message) )
		{
			return "BAD\t" + message;
		}
	}

	//削除リクエスト
	const string fullpath = GetUserPath(id,"/" + typeDirecptry + "/" + filename);
	XLFileUtil::del(fullpath);
	ERRORLOG("OK link(" << link << ")/id("<<id<<")のファイル名("<<filename<<")のtype("<<type<<")を削除しました.");

	return "OK";
}

string ServerScript::Server_api_sendmail(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	const map<string,string> request = httpHeaders.getRequest();
	const unsigned int id = atou( mapfind(cookieArray,"id") );
	const string link = httpHeaders.getCookieHeader("naichichi2");
	const string name = mapfind(cookieArray,"name") ;
	const string uuid = mapfind(cookieArray,"uuid") ;

	if (  mapfind(cookieArray,"link") != "1" )
	{
		ERRORLOG("no link session link(" << link << ")/id("<<id<<")のlinkは存在しません.");
		return "BAD\tno link session";
	}
	if (id <= 0)
	{
		ERRORLOG("bad cookie value link(" << link << ")/id("<<id<<")のIDが0以下です.");
		return "BAD\tbad cookie value";
	}

	const string filename = mapfind(request,"filename");
	if (filename.empty() )
	{
		ERRORLOG("bad filename  link(" << link << ")/id("<<id<<")のファイル名("<<filename<<")は空です.");
		return "BAD\tbad filename";
	}
	if (! XLStringUtil::checkSafePath(filename))
	{
		ERRORLOG("bad file path  link(" << link << ")/id("<<id<<")のファイル名("<<filename<<")は危険です.");
		return "BAD\tbad file path";
	}

	//メールメッセージ本文
	const string message = mapfind(request,"message") ;
	//送り先
	const string sendto = mapfind(request,"sendto") ;

	//実在確認
	{
		lock_guard<mutex> al(this->lock);
		string message;
		if ( ! CheckUser(link,name,uuid,id,time(NULL),&message) )
		{
			return "BAD\t" + message;
		}
	}


	return "OK";
}

string ServerScript::Server_secret_change_user_payed(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string name = mapfind(request,"name");
	const bool ispay = stringbool( mapfind(request,"ispay") );

	{
		lock_guard<mutex> al(this->lock);

		const auto it = UserMaps.find(name);
		if  (it == UserMaps.end() )
		{
			ERRORLOG("baduser ID(" << name << ")が存在しません");
			return ScriptRemoconWeb::ResultError(90003 , "bad user");
		}
		it->second->isPay = ispay;
	}
	this->Write(this->DBName);

	return "OK";
}


string ServerScript::Server_auto_wearehere(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string uuid = mapfind(request,"uuid");
	const string url = mapfind(request,"url");

	//UUIDが空です.
	if (uuid.empty())
	{
		return ScriptRemoconWeb::ResultError(90001 , "");
	}
	//urlが空です.
	if (url.empty())
	{
		return ScriptRemoconWeb::ResultError(90002 , "");
	}

	{
		lock_guard<mutex> al(this->lock);
		this->WeAreHere.Update(url,uuid);
	}

	return "OK";
}


string ServerScript::Server_auto_changeregist(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	const map<string,string> request = httpHeaders.getRequest();
	
	const string link = httpHeaders.getCookieHeader("naichichi2");
	const string old_name = mapfind(request,"old_name");
	const string old_password = mapfind(request,"old_password");
	const string new_name = mapfind(request,"new_name");
	const string new_password = mapfind(request,"new_password");

	if ( ! XLStringUtil::checkMailAddress(new_name) )
	{
		ERRORLOG("bad user name("<<new_name<<") がメールアドレスではありません.");
		return "BAD\tbad name";
	}
	if ( ! XLStringUtil::checkSafePath( new_name )  )
	{
		ERRORLOG("bad user name("<<new_name<<") に不正な文字列が入力されました.");
		return "BAD\tbad name";
	}
	if ( ! XLStringUtil::checkSafePath( new_password )  )
	{//sha1ハッシュなのに変な文字が来るわけがない
		if ("<%PASSWORD NO CHANGE%>" != new_password)
		{
			ERRORLOG("bad user name("<<new_name<<") のパスワードが不正な文字列です.");
			return "BAD\tbad name";
		}
	}

	{
		lock_guard<mutex> al(this->lock);

		//登録されているかを調べる
		auto it = UserMaps.find(old_name);
		if (it == UserMaps.end() )
		{//登録されていない
			ERRORLOG("AuthError! old_name("<<old_name<<")/new_name("<<new_name<<") の old_nameは登録されていません.");
			return "BAD\tAuthError!";
		}
		if (it->second->password != old_password)
		{//パスワードが違う
			ERRORLOG("AuthError! old_name("<<old_name<<")/new_name("<<new_name<<") の old_nameのパスワードが違います.");
			return "BAD\tAuthError!";
		}


		if (old_name != new_name)
		{//IDを変える場合は、新しいIDを誰かが使っていないかを見る
			//新しいIDが許容範囲？
			if (new_name.empty()) 
			{
				ERRORLOG("Bad NewID old_name("<<old_name<<")/new_name("<<new_name<<") の new_nameは空です.");
				return "BAD\tBad NewID";
			}
			if ( ! XLStringUtil::checkMailAddress(new_name) )
			{
				ERRORLOG("Bad NewID old_name("<<old_name<<")/new_name("<<new_name<<") の new_nameはメールアドレスではありません.");
				return "BAD\tBad NewID";
			}
			if ( ! XLStringUtil::checkSafePath( new_name )  )
			{
				ERRORLOG("Bad NewID old_name("<<old_name<<")/new_name("<<new_name<<") の new_nameはメールアドレスではありません.");
				return "BAD\tBad NewID";
			}
			const auto newit = UserMaps.find(new_name);
			if (newit != UserMaps.end() )
			{//すでにいるよ!
				ERRORLOG("ID Already Regist! old_name("<<old_name<<")/new_name("<<new_name<<") の new_nameは既に登録されています.");
				return "BAD\tNewID Already Regist!";
			}
			//OK. IDとパスワードを変更する
			struct User* user = it->second;
			user->status = "";
			user->isServerStatusUpdate = false;
			UserMaps.erase(it);

			if (new_password.empty() || new_password == "<%PASSWORD NO CHANGE%>") 
			{//パスワードを変えない
				user->password = old_password;
				UserMaps.insert( pair<string , struct User*>(new_name,user) );
				ERRORLOG("OK1 old_name("<<old_name<<")/new_name("<<new_name<<") 処理しました。パスワードを変えません。IDだけ変更します.");
			}
			else
			{//パスワードも変える
				user->password = new_password;
				UserMaps.insert( pair<string , struct User*>(new_name,user) );
				ERRORLOG("OK2 old_name("<<old_name<<")/new_name("<<new_name<<") 処理しました。IDとパスワードを変えます.");
			}
		}
		else
		{//ID は変えない
			if (new_password.empty() || new_password == "<%PASSWORD NO CHANGE%>") 
			{//パスワードを変えない
				//何もすることがない
				ERRORLOG("OK3 old_name("<<old_name<<")/new_name("<<new_name<<") 処理しました。ただし何もすることはありません.");
			}
			else
			{//パスワードも変える
				it->second->password = new_password;
				ERRORLOG("OK4 old_name("<<old_name<<")/new_name("<<new_name<<") 処理しました。パスワードだけ変えます.");
			}
		}

		this->Write(this->DBName);
	}
	return "OK";
}

string ServerScript::Server_auto_checkregist(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	const map<string,string> request = httpHeaders.getRequest();
	
	const string link = httpHeaders.getCookieHeader("naichichi2");
	const string name = mapfind(request,"name");
	const string password = mapfind(request,"password");
	const string uuid = mapfind(request,"uuid");
	const string is_uuid_overriade = mapfind(request,"is_uuid_overriade");
	bool isPay = false;

	if (name.empty() || password.empty() || uuid.empty() )
	{
		ERRORLOG("bad user name("<<name<<") の IDかpassかuuidが空です.");
		return "BAD\tempty data";
	}
	if ( ! XLStringUtil::checkMailAddress(name) )
	{
		ERRORLOG("bad user name("<<name<<") がメールアドレスではありません.");
		return "BAD\tbad name";
	}
	if ( ! XLStringUtil::checkSafePath( name )  )
	{
		ERRORLOG("bad user name("<<name<<") に不正な文字列が入力されました.");
		return "BAD\tbad name";
	}
	if ( ! XLStringUtil::checkSafePath( password )  )
	{//sha1ハッシュなのに変な文字が来るわけがない
		ERRORLOG("bad user name("<<name<<") のパスワードが不正な文字列です.");
		return "BAD\tbad name";
	}
	if ( ! XLStringUtil::checkSafePath( uuid )  )
	{//sha1ハッシュなのに変な文字が来るわけがない
		ERRORLOG("bad user name("<<name<<") のUUIDが不正な文字列です.");
		return "BAD\tbad name";
	}

	{
		lock_guard<mutex> al(this->lock);

		const auto it = UserMaps.find(name);
		if (it == UserMaps.end() )
		{
			ERRORLOG("bad user name("<<name<<") の NAME は登録されていません.");
			return "BAD\tbad user";
		}
		if (it->second->password != password)
		{
			ERRORLOG("bad user name("<<name<<") の パスワードが違います.");
			return "BAD\tbad user";
		}
		isPay = it->second->isPay;

		//UUIDのチェック
		if ( checkUUID(uuid ))
		{//UUIDチェックが通るものは、ハード購入者だから、購入フラグを立てる.
			isPay = true;
		}

		if(it->second->uuid.empty())
		{
			ERRORLOG("name("<<name<<") の UUID(" << uuid << ")を設定します。");
			it->second->uuid = uuid;
		}
		else
		{
			if (it->second->uuid != uuid)
			{
				ERRORLOG("name("<<name<<") の UUID(" << uuid << ")が一致しません。 overraide:" << is_uuid_overriade );
				if (is_uuid_overriade != "1")
				{
					return "BAD\tuuid already use";
				}

				ERRORLOG("name("<<name<<") の UUID(" << uuid << ")の上書きを許可します。 overraide:" << is_uuid_overriade );
				it->second->uuid = uuid;
			}
		}

		this->Write(this->DBName);
	}

	ERRORLOG("OK name("<<name<<") uuid(" << uuid << ")を処理しました.");
	if (isPay)
	{
		return "OK\tpayed";
	}
	else
	{
		return "OK\tnopay";
	}
}

//閲覧専用のリモコン
string ServerScript::Server_user_remocon_index(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	const auto headers = httpHeaders.getHeaderPointer();

	bool isPay = false;
	map<string,string> configmap;
	const string link = httpHeaders.getCookieHeader("naichichi2");
	const time_t now = time(NULL);
	bool linked = false;
	{
		lock_guard<mutex> al(this->lock);

		if (mapfind(cookieArray,"user") != "1")
		{
			return ScriptRemoconWeb::ResultError(40000,"userクッキーではありません.");
		}
		const unsigned int cookieid = atou( mapfind(cookieArray,"id") );
		const string cookiename = mapfind(cookieArray,"name");
		if (! (cookieid >= 1  && !cookiename.empty() ) )
		{
			return ScriptRemoconWeb::ResultError(40000,"id(" + num2str(cookieid)  + ") name(" + cookiename + ")は不正なクッキーデータです");
		}
		const auto userIT = UserMaps.find(cookiename);
		if (userIT == UserMaps.end() )
		{
			return ScriptRemoconWeb::ResultError(40000,"内部エラー リモコン画面取得のためのユーザー情報がありません.");
		}

		userIT->second->userLoginTime = now;	
		isPay = userIT->second->isPay;
		configmap = userIT->second->config.ToMap();
		
		if (mapfind(headers,"cache-control") == "max-age=0" || mapfind(headers,"pragma") == "no-cache")
		{//F5とかリロードで更新した場合は、センサーの値を更新する
			userIT->second->status  = "reconfig"; //再度値を取得してもらいたい
		}

		if (now - userIT->second->clientConnectTime <= 120)
		{//端末から120秒以内にアクセスかあったら linked されているものとする
			linked = true;
		}
	}
	const bool enableMultiRoom = MultiRoomUtil::isMultiRoomEnable(configmap);

	const string htmlsrc = ScriptRemoconWeb::OpenTPL(httpHeaders,"remocon.tpl");
	return ScriptRemoconWeb::DrawHTML(configmap,htmlsrc, now, false , linked ,enableMultiRoom , !isPay);
}


string ServerScript::Server_auto_get_url_by_password(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	const map<string,string> request = httpHeaders.getRequest();

	//まず命令を出した端末の検証.
	const string link = httpHeaders.getCookieHeader("naichichi2");
	const string name = mapfind(cookieArray,"name");
	const unsigned int myid = atou( mapfind(cookieArray,"id") );
	if (  mapfind(cookieArray,"link") != "1" )
	{
		ERRORLOG("no link session link(" << link << ")/id("<<myid<<")のlinkは存在しません.");
		return "BAD\tno link session";
	}
	if (myid <= 0)
	{
		ERRORLOG("bad cookie value link(" << link << ")/id("<<myid<<")のIDが0以下です.");
		return "BAD\tbad cookie value";
	}

	//IPを取得される端末
	const string yourname = mapfind(request,"yourname") ;
	const string yourpassword = mapfind(request,"yourpassword") ;
	if (yourname.empty() || yourpassword.empty() )
	{
		ERRORLOG("bad yourname("<<yourname<<") または password が空です.");
		return "BAD\tbad data";
	}
	if ( ! XLStringUtil::checkMailAddress(yourname) )
	{
		ERRORLOG("bad user name("<<yourname<<") がメールアドレスではありません.");
		return "BAD\tbad name";
	}

	string httpd__url;
	{
		lock_guard<mutex> al(this->lock);

		const auto it = UserMaps.find(yourname);
		if (it == UserMaps.end() )
		{
			ERRORLOG("bad user name("<<yourname<<") の NAME は登録されていません.");
			return "BAD\tbad user";
		}
		if (it->second->password != yourpassword)
		{
			ERRORLOG("bad user name("<<yourpassword<<") の UUID が違います.");
			return "BAD\tbad user";
		}
		httpd__url = it->second->config.Get("httpd__url","");
	}

	ERRORLOG("OK name("<<name<<") からのリクエストで、yourname("<<yourname<<") URL(" << httpd__url << ")を取得しました.");

	return "OK\t" + httpd__url;
}

string ServerScript::Server_auto_get_url_by_uuid(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	const map<string,string> request = httpHeaders.getRequest();

	//まず命令を出した端末の検証.
	const string link = httpHeaders.getCookieHeader("naichichi2");
	const string name = mapfind(cookieArray,"name");
	const unsigned int myid = atou( mapfind(cookieArray,"id") );
	if (  mapfind(cookieArray,"link") != "1" )
	{
		ERRORLOG("no link session link(" << link << ")/id("<<myid<<")のlinkは存在しません.");
		return "BAD\tno link session";
	}
	if (myid <= 0)
	{
		ERRORLOG("bad cookie value link(" << link << ")/id("<<myid<<")のIDが0以下です.");
		return "BAD\tbad cookie value";
	}

	//IPを取得される端末
	const string yourname = mapfind(request,"yourname") ;
	const string youruuid = mapfind(request,"youruuid") ;
	if (yourname.empty() || youruuid.empty() )
	{
		ERRORLOG("bad yourname("<<yourname<<") または youruuid("<<youruuid<<") が空です.");
		return "BAD\tmyourname or youruuid";
	}
	if ( ! XLStringUtil::checkMailAddress(yourname) )
	{
		ERRORLOG("bad user name("<<yourname<<") がメールアドレスではありません.");
		return "BAD\tbad name";
	}

	string httpd__url;
	{
		lock_guard<mutex> al(this->lock);

		const auto it = UserMaps.find(yourname);
		if (it == UserMaps.end() )
		{
			ERRORLOG("bad user name("<<yourname<<") の NAME は登録されていません.");
			return "BAD\tbad user";
		}
		if (it->second->uuid != youruuid)
		{
			ERRORLOG("bad user name("<<yourname<<") の UUID が違います.");
			return "BAD\tbad user";
		}
		httpd__url = it->second->config.Get("httpd__url","");
	}

	ERRORLOG("OK name("<<name<<") からのリクエストで、yourname("<<yourname<<")  youruuid("<<youruuid<<")のURL(" << httpd__url << ")を取得しました.");

	return "OK\t" + httpd__url;
}


string ServerScript::Server_auto_recong(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	const unsigned int id = atou( mapfind(cookieArray,"id") );
	const auto files = httpHeaders.getFilesPointer();
	const string link = httpHeaders.getCookieHeader("naichichi2");

	if (  mapfind(cookieArray,"link") != "1" )
	{
		return "BAD\tno link session";
	}
	if (id <= 0)
	{
		return "BAD\tbad cookie value";
	}
	if (files->empty())
	{
		return "BAD\tbad file. (empty)";
	}

	const auto it = files->begin();
	//return recong(userid , (*it)->data);
	return "";
}

//ユーザーが利用しているアイコンの画像など
string ServerScript::Server_user_image_replace_user_elecion(const string& replacepath,const string type,const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	if (mapfind(cookieArray,"user") != "1")
	{
		return ""; //内部エラー
	}

	const unsigned int id = atou( mapfind(cookieArray,"id") );
	const string returnfilepath =  GetUserPath(  id , "/" + type + "/" + replacepath);	
	if (!XLFileUtil::Exist(returnfilepath))
	{//ない場合は、システムディフォルトの代替を返す.
		string defaultFilepath;
		if (type == "image")
		{
			defaultFilepath = "user/elecicon/" + replacepath;
		}
		else
		{
			defaultFilepath = "user/" + type + "/" + replacepath;
		}
		defaultFilepath = GetWebRootPath( defaultFilepath  );
		return defaultFilepath;
	}
	return returnfilepath;
}

//端末にリクエストを送る
string ServerScript::SendRequest(const string& cmd,map<string,string>* cookieArray)
{
	

	const time_t now = time(NULL);
	bool* updateFlagAddress = NULL;

	if (mapfind(cookieArray,"user") != "1")
	{
		return ScriptRemoconWeb::ResultError(40000,"userクッキーではありません.");
	}
	const unsigned int cookieid = atou( mapfind(cookieArray,"id") );
	const string cookiename = mapfind(cookieArray,"name");
	if (! (cookieid >= 1  && !cookiename.empty() ) )
	{
		return ScriptRemoconWeb::ResultError(40000,"id(" + num2str(cookieid)  + ") name(" + cookiename + ")は不正なクッキーデータです");
	}
	{
		lock_guard<mutex> al(this->lock);

		const auto userIT = UserMaps.find(cookiename);
		if (userIT == UserMaps.end() )
		{
			return ScriptRemoconWeb::ResultError(40000,"内部エラー リモコン画面取得のためのユーザー情報がありません");
		}

		userIT->second->userLoginTime = now;	
		userIT->second->status  = _A2U(cmd);
		userIT->second->isServerStatusUpdate = true;
		updateFlagAddress = &userIT->second->isServerStatusUpdate;
	}

	//操作が受け付けられるかポーリングする
	for(int i = 0 ; i < 10 ; ++i )
	{
		if (! *updateFlagAddress ) break; //boolだからアトミックを期待しているw
		SecSleep(1);
	}
	if(!*updateFlagAddress)
	{
		return "{ \"result\": \"ok\" }";
	}
	else
	{
		return ScriptRemoconWeb::ResultError(40009,"命令を出しましたが端末が指示を受け取れませんでした.");
	}
}

string ServerScript::Server_user_remocon_fire_bytype(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	const map<string,string> request = httpHeaders.getRequest();

	const string type1 = mapfind(request,"type1");
	const string type2 = mapfind(request,"type2");
	if (type1.empty() || type2.empty() )
	{
		return ScriptRemoconWeb::ResultError(40001,"命令がありません.");
	}

	const string cmd = "action\t"+type1+"\t"+type2;
	return SendRequest(cmd,cookieArray);
}

string ServerScript::Server_user_remocon_fire_bystring(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	const map<string,string> request = httpHeaders.getRequest();

	const string str = mapfind(request,"str");
	if (str.empty() )
	{
		return ScriptRemoconWeb::ResultError(40001,"命令がありません");
	}

	const string cmd = "actionstr\t"+str;
	return SendRequest(cmd,cookieArray);
}

string ServerScript::Server_user_remocon_sip_call(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	const map<string,string> request = httpHeaders.getRequest();

	const string call = mapfind(request,"call");
	if (call.empty() )
	{
		return ScriptRemoconWeb::ResultError(40001,"命令がありません");
	}

	const string cmd = "sip_call\t"+call;
	return SendRequest(cmd,cookieArray);
}
string ServerScript::Server_user_remocon_sip_answer(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	const map<string,string> request = httpHeaders.getRequest();

	const string cmd = "sip_answer";
	return SendRequest(cmd,cookieArray);
}
string ServerScript::Server_user_remocon_sip_hangup(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	const map<string,string> request = httpHeaders.getRequest();

	const string cmd = "sip_hangup";
	return SendRequest(cmd,cookieArray);
}
string ServerScript::Server_user_remocon_sip_numpad_close(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	const map<string,string> request = httpHeaders.getRequest();

	const string cmd = "sip_numpad_close";
	return SendRequest(cmd,cookieArray);
}



string ServerScript::Server_api_elec_action(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) 
{
	
	const map<string,string> request = httpHeaders.getRequest();
	if ( mapfind(request,"callback").empty() )
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
	}
	else
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSONP;
	}

	const string apikey = mapfind(request,"webapi_apikey");
	const string name = mapfind(request,"user");
	const string elec = mapfind(request,"elec");
	const string action = mapfind(request,"action");
	//ユーザーの検索
	unsigned int id = 0;

	const time_t now = time(NULL);
	bool* updateFlagAddress = NULL;
	{
		lock_guard<mutex> al(this->lock);

		//ユーザーの検索
		const auto userIT = UserMaps.find(name);
		if (userIT == UserMaps.end() )
		{//そんな人いません
			ERRORLOG("エラー name("<<name<<")は登録されていません.");
			return ScriptRemoconWeb::ResultError(80001, "");
		}
		//IDの取得
		id = userIT->second->id;
		if ( userIT->second->config.Get("webapi_apikey","") != apikey )
		{
			return ScriptRemoconWeb::ResultError(40002 , "apikeyが正しくない");
		}
		userIT->second->userLoginTime = now;	
		userIT->second->status  = _A2U("action\t"+elec+"\t"+action);
		userIT->second->isServerStatusUpdate = true;
		updateFlagAddress = &userIT->second->isServerStatusUpdate;
	}

	//操作が受け付けられるかポーリングする
	for(int i = 0 ; i < 10 ; ++i )
	{
		if (! *updateFlagAddress ) break; //boolだからアトミックを期待しているw
		SecSleep(1);
	}

	if(!*updateFlagAddress)
	{
		return "{ \"result\": \"ok\" }";
	}
	else
	{
		return ScriptRemoconWeb::ResultError(40009,"命令を出しましたが端末が指示を受け取れませんでした.");
	}
}

//家電の情報を取得する
string ServerScript::Server_api_elec_getinfo(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) 
{
	
	const map<string,string> request = httpHeaders.getRequest();
	if ( mapfind(request,"callback").empty() )
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
	}
	else
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSONP;
	}

	const string apikey = mapfind(request,"webapi_apikey");
	const string name = mapfind(request,"user");
	map<string,string> configmap;
	//ユーザーの検索
	unsigned int id = 0;

	{
		lock_guard<mutex> al(this->lock);

		//ユーザーの検索
		const auto userIT = UserMaps.find(name);
		if (userIT == UserMaps.end() )
		{//そんな人いません
			ERRORLOG("エラー name("<<name<<")は登録されていません.");
			return ScriptRemoconWeb::ResultError(80001, "");
		}
		//IDの取得
		id = userIT->second->id;
		if ( userIT->second->config.Get("webapi_apikey","") != apikey )
		{
			return ScriptRemoconWeb::ResultError(40002 , "apikeyが正しくない");
		}
		configmap = userIT->second->config.FindGetsToMap("elec_",false);
	}

	const string p1 = mapfind(request,"elec");
	int key1 = 0;
	if ( ! ScriptRemoconWeb::type2key(configmap ,p1 , &key1) )
	{
		return ScriptRemoconWeb::ResultError(40003 , "対応する機材が存在しない");
	}

	string jsonstring;
	const string prefix = "elec_" + num2str(key1) + "_";
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		if ( ! strstr( it->first.c_str() , prefix.c_str() ) )
		{//対象外
			continue;
		}
		if ( strstr( it->first.c_str() , "_action_" ) )
		{
			continue;
		}
		
		jsonstring += "," + XLStringUtil::jsonescape(it->first.c_str() + prefix.size() ) + ": " + XLStringUtil::jsonescape(it->second ) ;
	}
	return _A2U("{\"result\": \"ok\"" + jsonstring + "}");
}

string ServerScript::Server_api_sensor_get(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) 
{
	
	const map<string,string> request = httpHeaders.getRequest();
	if ( mapfind(request,"callback").empty() )
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
	}
	else
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSONP;
	}

	const string apikey = mapfind(request,"webapi_apikey");
	const string name = mapfind(request,"user");
	string	sensor_temp_value;
	string	sensor_lumi_value;
	string	sensor_sound_value;
	//ユーザーの検索
	unsigned int id = 0;

	{
		lock_guard<mutex> al(this->lock);

		//ユーザーの検索
		const auto userIT = UserMaps.find(name);
		if (userIT == UserMaps.end() )
		{//そんな人いません
			ERRORLOG("エラー name("<<name<<")は登録されていません.");
			return ScriptRemoconWeb::ResultError(80001, "");
		}
		//IDの取得
		id = userIT->second->id;
		if ( userIT->second->config.Get("webapi_apikey","") != apikey )
		{
			return ScriptRemoconWeb::ResultError(40002 , "apikeyが正しくない");
		}
		sensor_temp_value = userIT->second->config.Get("sensor_temp_value","");
		sensor_lumi_value = userIT->second->config.Get("sensor_lumi_value","");
		sensor_sound_value = userIT->second->config.Get("sensor_sound_value","");
	}
	
	string jsonstring;
	jsonstring  = ",\"temp\": "+ XLStringUtil::jsonescape( sensor_temp_value ) ;
	jsonstring += ",\"lumi\": "+ XLStringUtil::jsonescape( sensor_lumi_value ) ;
	jsonstring += ",\"sound\": "+ XLStringUtil::jsonescape( sensor_sound_value ) ;

	return jsonstring;
}





//リモコンの副次的情報の取得
string ServerScript::Server_user_remocon_get_append_info(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	const time_t now = time(NULL);
	map<string,string> configmap;
	{
		lock_guard<mutex> al(this->lock);

		if (mapfind(cookieArray,"user") != "1")
		{
			return ScriptRemoconWeb::ResultError(40000,"userクッキーではありません.");
		}
		const unsigned int cookieid = atou( mapfind(cookieArray,"id") );
		const string cookiename = mapfind(cookieArray,"name");
		if (! (cookieid >= 1  && !cookiename.empty() ) )
		{
			return ScriptRemoconWeb::ResultError(40000,"id(" + num2str(cookieid)  + ") name(" + cookiename + ")は不正なクッキーデータです");
		}
		const auto userIT = UserMaps.find(cookiename);
		if (userIT == UserMaps.end() )
		{
			return ScriptRemoconWeb::ResultError(40000,"内部エラー ユーザー情報がありません.");
		}

		userIT->second->userLoginTime = now;	
		configmap = userIT->second->config.FindGetsToMap("elec_",false);
	}

	return ScriptRemoconWeb::remocon_get_append_info(configmap,now);
}


string ServerScript::Server_user_networksetting_index(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	
	const map<string,string> request = httpHeaders.getRequest();

	const time_t now = time(NULL);
	string jsonstring;

	{
		lock_guard<mutex> al(this->lock);

		if (mapfind(cookieArray,"user") != "1")
		{
			return ScriptRemoconWeb::ResultError(40000,"userクッキーではありません.");
		}
		const unsigned int cookieid = atou( mapfind(cookieArray,"id") );
		const string cookiename = mapfind(cookieArray,"name");
		if (! (cookieid >= 1  && !cookiename.empty() ) )
		{
			return ScriptRemoconWeb::ResultError(40000,"id(" + num2str(cookieid)  + ") name(" + cookiename + ")は不正なクッキーデータです");
		}
		const auto userIT = UserMaps.find(cookiename);
		if (userIT == UserMaps.end() )
		{
			return ScriptRemoconWeb::ResultError(40000,"内部エラー リモコン画面取得のためのユーザー情報がありません.");
		}

		userIT->second->userLoginTime = now;	

		jsonstring +=","+XLStringUtil::jsonescape("account_mail") + ": " 
			 + XLStringUtil::jsonescape(userIT->first) ;
		jsonstring +=","+XLStringUtil::jsonescape("network_ipaddress_type") + ": " 
			 + XLStringUtil::jsonescape("DHCP") ;
		jsonstring +=","+XLStringUtil::jsonescape("network_ipaddress_ip") + ": " 
			 + XLStringUtil::jsonescape("") ;
		jsonstring +=","+XLStringUtil::jsonescape("network_ipaddress_mask") + ": " 
			 + XLStringUtil::jsonescape("") ;
		jsonstring +=","+XLStringUtil::jsonescape("network_ipaddress_gateway") + ": " 
			 + XLStringUtil::jsonescape("") ;
		jsonstring +=","+XLStringUtil::jsonescape("network_ipaddress_dns") + ": " 
			 + XLStringUtil::jsonescape("") ;
		jsonstring +=","+XLStringUtil::jsonescape("network_w_ipaddress_type") + ": " 
			 + XLStringUtil::jsonescape("") ;
		jsonstring +=","+XLStringUtil::jsonescape("network_w_ipaddress_ip") + ": " 
			 + XLStringUtil::jsonescape("") ;
		jsonstring +=","+XLStringUtil::jsonescape("network_w_ipaddress_mask") + ": " 
			 + XLStringUtil::jsonescape("") ;
		jsonstring +=","+XLStringUtil::jsonescape("network_vipaddress_gateway") + ": " 
			 + XLStringUtil::jsonescape("") ;
		jsonstring +=","+XLStringUtil::jsonescape("network_vipaddress_dns") + ": " 
			 + XLStringUtil::jsonescape("") ;
		jsonstring +=","+XLStringUtil::jsonescape("network_w_ipaddress_ssid") + ": " 
			 + XLStringUtil::jsonescape("") ;
		jsonstring +=","+XLStringUtil::jsonescape("network_w_ipaddress_password") + ": " 
			 + XLStringUtil::jsonescape("") ;
		jsonstring +=","+XLStringUtil::jsonescape("network_w_ipaddress_wkeymgmt") + ": " 
			 + XLStringUtil::jsonescape("") ;
		jsonstring +=","+XLStringUtil::jsonescape("httpd__url") + ": " 
			 + XLStringUtil::jsonescape(userIT->second->config.Get("httpd__url","")) ;
	}
	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";

	string html = ScriptRemoconWeb::OpenTPL(httpHeaders,"server_networksetting.tpl");
	html = XLStringUtil::replace(html,"%SETTING%",jsonstring);

	return html;
}

string ServerScript::Server_user_networksetting_network_update(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	
	const map<string,string> request = httpHeaders.getRequest();

	const time_t now = time(NULL);
	string account_mail;
	string account_password_sha1;
	unsigned int id;
	{
		lock_guard<mutex> al(this->lock);

		if (mapfind(cookieArray,"user") != "1")
		{
			return ScriptRemoconWeb::ResultError(40000,"userクッキーではありません.");
		}
		const unsigned int cookieid = atou( mapfind(cookieArray,"id") );
		const string cookiename = mapfind(cookieArray,"name");
		if (! (cookieid >= 1  && !cookiename.empty() ) )
		{
			return ScriptRemoconWeb::ResultError(40000,"id(" + num2str(cookieid)  + ") name(" + cookiename + ")は不正なクッキーデータです");
		}
		const auto userIT = UserMaps.find(cookiename);
		if (userIT == UserMaps.end() )
		{
			return ScriptRemoconWeb::ResultError(40000,"内部エラー リモコン画面取得のためのユーザー情報がありません.");
		}

		userIT->second->userLoginTime = now;	
		id = userIT->second->id;
		account_mail = userIT->first;
		account_password_sha1 = userIT->second->password;
	}

	string network_ipaddress_type = mapfind(request,"network_ipaddress_type");
	string network_ipaddress_ip = mapfind(request,"network_ipaddress_ip");
	string network_ipaddress_mask = mapfind(request,"network_ipaddress_mask");
	string network_ipaddress_gateway = mapfind(request,"network_ipaddress_gateway");
	string network_ipaddress_dns = mapfind(request,"network_ipaddress_dns");

	string network_w_ipaddress_type = mapfind(request,"network_w_ipaddress_type");
	string network_w_ipaddress_ip = mapfind(request,"network_w_ipaddress_ip");
	string network_w_ipaddress_mask = mapfind(request,"network_w_ipaddress_mask");
	string network_w_ipaddress_gateway = mapfind(request,"network_w_ipaddress_gateway");
	string network_w_ipaddress_dns = mapfind(request,"network_w_ipaddress_dns");
	string network_w_ipaddress_ssid = mapfind(request,"network_w_ipaddress_ssid");
	string network_w_ipaddress_password = mapfind(request,"network_w_ipaddress_password");
	string network_w_ipaddress_wkeymgmt = mapfind(request,"network_w_ipaddress_wkeymgmt");

	if(network_ipaddress_type == "DHCP")
	{
	}
	else if (network_ipaddress_type == "STATIC")
	{
		if (! XLStringUtil::checkIPAddressV4(network_ipaddress_ip) )return ScriptRemoconWeb::ResultError(40001,"bad network_ipaddress_ip");
		if (! XLStringUtil::checkIPAddressV4(network_ipaddress_mask) )return ScriptRemoconWeb::ResultError(40002,"bad network_ipaddress_mask");
		if (! XLStringUtil::checkIPAddressV4(network_ipaddress_gateway) )return ScriptRemoconWeb::ResultError(40003,"bad network_ipaddress_gateway");
		if (! XLStringUtil::checkIPAddressV4(network_ipaddress_dns) )return ScriptRemoconWeb::ResultError(40004,"bad network_ipaddress_dns");
	}
	else
	{
		return ScriptRemoconWeb::ResultError(40010,"bad network_ipaddress_type");
	}
	if(network_w_ipaddress_type == "DHCP" || network_w_ipaddress_type == "NONE")
	{
	}
	else if (network_ipaddress_type == "STATIC")
	{
		if (! XLStringUtil::checkIPAddressV4(network_w_ipaddress_ip) )return ScriptRemoconWeb::ResultError(40021,"bad network_w_ipaddress_ip");
		if (! XLStringUtil::checkIPAddressV4(network_w_ipaddress_mask) )return ScriptRemoconWeb::ResultError(40022,"bad network_w_ipaddress_mask");
		if (! XLStringUtil::checkIPAddressV4(network_w_ipaddress_gateway) )return ScriptRemoconWeb::ResultError(40023,"bad network_w_ipaddress_gateway");
		if (! XLStringUtil::checkIPAddressV4(network_w_ipaddress_dns) )return ScriptRemoconWeb::ResultError(40024,"bad network_w_ipaddress_dns");
	}
	else
	{
		network_w_ipaddress_type = "NONE";
	}
	if(network_w_ipaddress_type != "NONE")
	{
		if ( ! network_w_ipaddress_ssid.empty() )return ScriptRemoconWeb::ResultError(40040,"bad network_w_ipaddress_ssid");
	}

	const string resetbase64 = ResetConfig::make(account_mail,account_password_sha1,network_ipaddress_type,network_ipaddress_ip,network_ipaddress_mask,network_ipaddress_gateway,network_ipaddress_dns,network_w_ipaddress_type,network_w_ipaddress_ip,network_w_ipaddress_mask,network_w_ipaddress_gateway,network_w_ipaddress_dns,network_w_ipaddress_ssid,network_w_ipaddress_password,network_w_ipaddress_wkeymgmt);
	XLFileUtil::write(GetUserPath(  id , "/temp/networksetting.reset" ) ,resetbase64);

	const string jsonstring = "{\"result\": \"ok\"}";
	return _A2U(jsonstring);
}

string ServerScript::Server_user_networksetting_network_download(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	const time_t now = time(NULL);
	unsigned int id;
	{
		lock_guard<mutex> al(this->lock);

		if (mapfind(cookieArray,"user") != "1")
		{
			return ScriptRemoconWeb::ResultError(40000,"userクッキーではありません.");
		}
		const unsigned int cookieid = atou( mapfind(cookieArray,"id") );
		const string cookiename = mapfind(cookieArray,"name");
		if (! (cookieid >= 1  && !cookiename.empty() ) )
		{
			return ScriptRemoconWeb::ResultError(40000,"id(" + num2str(cookieid)  + ") name(" + cookiename + ")は不正なクッキーデータです");
		}
		const auto userIT = UserMaps.find(cookiename);
		if (userIT == UserMaps.end() )
		{
			return ScriptRemoconWeb::ResultError(40000,"内部エラー リモコン画面取得のためのユーザー情報がありません.");
		}

		userIT->second->userLoginTime = now;	
		id = userIT->second->id;
	}

	return GetUserPath(  id , "/temp/networksetting.reset" ) ;
}




string ServerScript::Server_user_networksetting_checkwait(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	
//	const map<string,string> request = httpHeaders.getRequest();

	const time_t now = time(NULL);
	bool linked = false;
	{
		lock_guard<mutex> al(this->lock);

		if (mapfind(cookieArray,"user") != "1")
		{
			return ScriptRemoconWeb::ResultError(40000,"userクッキーではありません.");
		}
		const unsigned int cookieid = atou( mapfind(cookieArray,"id") );
		const string cookiename = mapfind(cookieArray,"name");
		if (! (cookieid >= 1  && !cookiename.empty() ) )
		{
			return ScriptRemoconWeb::ResultError(40000,"id(" + num2str(cookieid)  + ") name(" + cookiename + ")は不正なクッキーデータです");
		}
		const auto userIT = UserMaps.find(cookiename);
		if (userIT == UserMaps.end() )
		{
			return ScriptRemoconWeb::ResultError(40000,"内部エラー リモコン画面取得のためのユーザー情報がありません.");
		}


		userIT->second->userLoginTime = now;	
		if (now - userIT->second->clientConnectTime <= 120)
		{//端末から120秒以内にアクセスかあったら linked されているものとする
			linked = true;
		}
	}

	string jsonstring ;
	if (linked)
	{
		jsonstring = ", \"is_link\": 1";
	}
	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	
	return _A2U(jsonstring);
}


string ServerScript::Server_user_auth_index(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result,map<string,string>* cookieArray)
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const auto headers = httpHeaders.getHeaderPointer();
	const string id = mapfind(request,"id");
	const string password = mapfind(request,"password");
	const string backurl = mapfind(request,"backurl");

	string html = ScriptRemoconWeb::OpenTPL(httpHeaders,"auth.tpl");
	html = XLStringUtil::replace_low(html,"%ID%", XLStringUtil::htmlspecialchars_low(_A2U(id) ) );
//	html = XLStringUtil::replace_low(html,"%PASSWORD%",  XLStringUtil::htmlspecialchars_low(_A2U(password) ) ); //エラーの時だけ差し込む
	html = XLStringUtil::replace_low(html,"%BACKURL%", XLStringUtil::htmlspecialchars_low(_A2U(backurl) ) );

	if ( !id.empty() )
	{
		html = XLStringUtil::replace(html,"%PASSWORD%",  XLStringUtil::htmlspecialchars_low(_A2U(password) ) );
		html = XLStringUtil::replace_low(html,"%FIRST_CALL%", "0" );

		//パスワードは sha1して保持しているので、比較するときは sha1 で比較するよ
		const string password_sha1 =  SystemMisc::MakePassword( id , password);
	
		{
			lock_guard<mutex> al(this->lock);

			const auto it = UserMaps.find(id);
			if (it == UserMaps.end() )
			{
				NOTIFYLOG("IDかパスワードが正しくない.(ID:" << id << "がありません。)");

				html = XLStringUtil::replace_low(html,"%ERROR_DISPLAY%", "block" );
				html = XLStringUtil::replace_low(html,"%UUID_ERROR_DISPLAY%", "0" );
				*result = WEBSERVER_RESULT_TYPE_OK_HTML;
				return html;
			}
			if (it->second->password != password_sha1)
			{
				NOTIFYLOG("IDかパスワードが正しくない.(passwordが違います。)");
				it->second->authErrorCount ++;
				if (it->second->authErrorCount >= 10)
				{//10回連続してログインに失敗したので、警告のメールを送る.
					NOTIFYLOG("ID(" << id << ")が10回つづけてログインに失敗しましたので、警告のメールを送ります。");

					//メールを送る
					mailto(id,"auth_bad_countmax_alert.mail.tpl");
				}

				html = XLStringUtil::replace_low(html,"%LOGIN_ERROR_DISPLAY%", "block" );
				html = XLStringUtil::replace_low(html,"%UUID_ERROR_DISPLAY%", "0" );
				*result = WEBSERVER_RESULT_TYPE_OK_HTML;
				return html;
			}

			it->second->userLoginTime = time(NULL);	
			it->second->status  = "reconfig"; //再度値を取得してもらいたい
			it->second->authErrorCount = 0;
			//OKログインできる。

			//cookieにログインの情報を書き込みます。
			EatLoginCookie(cookieArray , it->second->id , id );
		}
		*result = WEBSERVER_RESULT_TYPE_LOCATION;
		//明示的な戻り先がある場合以外は、リモコン操作に戻す
		if (backurl.empty())
		{
			return "/remocon/";
		}
		
		return backurl;
	}

	//ログイン画面を出す
	html = XLStringUtil::replace_low(html,"%PASSWORD%",  "" );
	html = XLStringUtil::replace_low(html,"%LOGIN_ERROR_DISPLAY%", "none" );
	html = XLStringUtil::replace_low(html,"%UUID_ERROR_DISPLAY%", "0" );
	html = XLStringUtil::replace_low(html,"%FIRST_CALL%", "1" );
	*result = WEBSERVER_RESULT_TYPE_OK_HTML;
	return html;
}

string ServerScript::Server_user_welcome_index(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	
	const map<string,string> request = httpHeaders.getRequest();

	return ScriptRemoconWeb::OpenTPL(httpHeaders,"server_welcome.tpl");
}

string ServerScript::Server_user_welcome2_index(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	
	const map<string,string> request = httpHeaders.getRequest();

	return ScriptRemoconWeb::OpenTPL(httpHeaders,"server_welcome2.tpl");
}

string ServerScript::Server_user_welcome_newregist(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string backurl = mapfind(request,"backurl");
	const string model = mapfind(request,"model");

	const string html = ScriptRemoconWeb::OpenTPL(httpHeaders,"server_welcome_newregist.tpl");

	string jsonstring;
	jsonstring = "{" + XLStringUtil::jsonvalue("backurl",backurl) 
				+","+XLStringUtil::jsonvalue("model",model)
				+"}";
	return XLStringUtil::replace_low(html,"%WELCOME_VALUE%", jsonstring );
}

string ServerScript::Server_user_welcome_newregist_ok(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	
	const map<string,string> request = httpHeaders.getRequest();

	const string html = ScriptRemoconWeb::OpenTPL(httpHeaders,"server_welcome_newregist_ok.tpl");
	return html;
}


//ログインさせます。 取り扱いには注意してください。
void ServerScript::EatLoginCookie(map<string,string>* cookieArray,unsigned int id , const string& name)
{
	//cookieにログインの情報を書き込みます。
	(*cookieArray)["id"] = num2str(id);
	(*cookieArray)["name"] = name;
	(*cookieArray)["user"] = "1";
	(*cookieArray)["user2"] = "1";
	(*cookieArray)["__regen__"] = "1";

	NOTIFYLOG("ユーザをログインさせます name("<<name<<")/id("<<id<<").");
}

string ServerScript::Server_user_welcome_newregist_json(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string name = mapfind(request,"id");
	const string password = mapfind(request,"password");
	const string backurl = mapfind(request,"backurl");
	unsigned int id = 0; 
	//パスワードは sha1して保持しているので、比較するときは sha1 で比較するよ
	const string password_sha1 =  SystemMisc::MakePassword( name , password);

	if(name.empty() || password.empty())
	{
		ERRORLOG("エラー name("<<name<<")または passwordが空です.");
		return ScriptRemoconWeb::ResultError(80001 , "");
	}

	if ( ! XLStringUtil::checkMailAddress(name) )
	{
		ERRORLOG("エラー name("<<name<<")はメールアドレスではありません.");
		return ScriptRemoconWeb::ResultError(80003 , "");
	}
	if ( ! XLStringUtil::checkSafePath( name )  )
	{
		ERRORLOG("bad user name("<<name<<") に不正な文字列が入力されました.");
		return ScriptRemoconWeb::ResultError(80003 , "");
	}

	{
		lock_guard<mutex> al(this->lock);

		const auto it = UserMaps.find(name);
		if (it != UserMaps.end() )
		{
			ERRORLOG("エラー name("<<name<<")は別の人がすでに使っています.");
			return ScriptRemoconWeb::ResultError(80002 , "already use.");
		}
		else
		{
			unsigned int maxid = 0;
			for(auto it = UserMaps.begin() ; it != UserMaps.end() ; ++it )
			{
				if (it->second->id > maxid) maxid = it->second->id ;
			}
			id = maxid + 1;

			struct User* user = new User;
			user->id =id;
			user->password = password_sha1;
			user->status = "";
			user->clientConnectTime = 0;
			user->userLoginTime = time(NULL);
			user->authErrorCount = 0;
			user->uuid = "";
			user->isPay = false;
			UserMaps.insert( pair<string , struct User*>(name,user) );
			XLFileUtil::mkdirP(GetUserPath(  user->id , "" ));
			XLFileUtil::mkdirP(GetUserPath(  user->id , "/config/" ));
			XLFileUtil::mkdirP(GetUserPath(  user->id , "/image/" ));
			XLFileUtil::mkdirP(GetUserPath(  user->id , "/temp/" ));
			XLFileUtil::mkdirP(GetUserPath(  user->id , "/remocon/" ));
			XLFileUtil::mkdirP(GetUserPath(  user->id , "/actionicon/" ));
			NOTIFYLOG("新規ユーザ登録 name("<<name<<")/id("<<id<<").");
		}


		//cookieにログインの情報を書き込みます。
		EatLoginCookie(cookieArray , id , name );
		this->Write(this->DBName);
	}

	{
		//メールを送る
		mailto(name,"server_welcome_newregist.mail.tpl");
		NOTIFYLOG("mailto done.");
	}


	string jsonstring;
	jsonstring = string("{ \"result\": \"ok\" ,\"backurl\":  " + XLStringUtil::jsonescape(backurl)  + "}");
	NOTIFYLOG("OK name("<<name<<")/id("<<id<<")処理しました.");

	return _A2U(jsonstring);
}

string ServerScript::Server_user_passwordreset_index(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	
	const map<string,string> request = httpHeaders.getRequest();

	return ScriptRemoconWeb::OpenTPL(httpHeaders,"server_passwordreset.tpl");
}

string ServerScript::Server_user_passwordreset_sendmail_json(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string name = mapfind(request,"id");
	if(name.empty() )
	{
		ERRORLOG("エラー name("<<name<<")が空です.");
		return ScriptRemoconWeb::ResultError(80001 , "");
	}

	unsigned int id = 0;
	{
		lock_guard<mutex> al(this->lock);

		//ユーザーの検索
		const auto it = UserMaps.find(name);
		if (it == UserMaps.end() )
		{//そんな人いません
			ERRORLOG("エラー name("<<name<<")は登録されていません.");
			return ScriptRemoconWeb::ResultError(80001, "");
		}
		else
		{//IDの取得
			id = it->second->id;
		}
	}

	//パスワードリセットの情報をファイルにストアする
	const string passwordResetID = "p" + XLStringUtil::uuid() + "@" + num2str(id);
	const string filename = GetUserPath(  id , "/temp/passwordreset.txt" );
	XLFileUtil::write(filename , passwordResetID);

	//メールを送る
	{
		map<string,string> arr;
		arr["%PASSWORDRESETID%"] = passwordResetID;
		//メールを送る
		mailto(name,"server_passwordreset.mail.tpl",arr);
	}

	string jsonstring;
	jsonstring = string("{ \"result\": \"ok\" }");

	NOTIFYLOG("パスワードリセット登録 name("<<name<<") のパスワードリセットを受け付けました。受付ID:" << passwordResetID);
	return _A2U(jsonstring);
}

bool ServerScript::checkPasswordResetID(const string& passwordResetID,unsigned int* outid) const
{
	if ( passwordResetID.empty() )
	{
		ERRORLOG("パスワードリセット(" << passwordResetID<< ")は空です");
		return false;
	}

	//ID lookupのための数字が入っているか？
	const char* lookupID =  strstr(passwordResetID.c_str() , "@");
	if (lookupID == NULL)
	{
		ERRORLOG("パスワードリセット(" << passwordResetID<< ")に@が入っていません");
		return false;
	}
	//そのIDで申請があるか？
	unsigned int id = atoi(lookupID+1); //+1 は @マークをどかすため
	const string filename = GetUserPath(  id , "/temp/passwordreset.txt" );
	if (!XLFileUtil::Exist(filename))
	{
		ERRORLOG("パスワードリセット(" << passwordResetID<< ")で申請ファイルがありません");
		return false;
	}
	//申請の文字列は一致するか？
	const string passwordResetID_file = XLFileUtil::cat(filename);
	if (passwordResetID != passwordResetID_file)
	{
		ERRORLOG("パスワードリセット(" << passwordResetID<< ")の申請文字列が一致しません。正しくは("<< passwordResetID_file << ")です" );
		return false;
	}
	//古いすぎる場合
	if ( time(NULL) - (24*60*60*7) > XLFileUtil::getfiletime(filename) )
	{
		ERRORLOG("パスワードリセット(" << passwordResetID<< ")の申請ファイルは古過ぎます。" );
		return false;
	}
	*outid = id;
	return true;
}

string ServerScript::Server_user_passwordreset_resetform(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string passwordResetID = mapfind(request,"k");

	unsigned id = 0;
	if ( ! checkPasswordResetID(passwordResetID,&id) )
	{
		return ScriptRemoconWeb::OpenTPL(httpHeaders,"server_passwordreset_resetform_sessionerror.tpl");
	}

	return ScriptRemoconWeb::OpenTPL(httpHeaders,"server_passwordreset_resetform.tpl");
}

string ServerScript::Server_user_passwordreset_resetform_json(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string passwordResetID = mapfind(request,"k");
	const string password = mapfind(request,"password");

	//パスワードが空です。
	if (password.empty())
	{
		return ScriptRemoconWeb::ResultError(80001 , "");
	}

	unsigned id = 0;
	if ( ! checkPasswordResetID(passwordResetID,&id) )
	{
		return ScriptRemoconWeb::ResultError(80002 , "");
	}

	bool found = false;
	string name;
	{
		lock_guard<mutex> al(this->lock);

		//ユーザーの検索とアップデート
		for(auto it = UserMaps.begin() ; it != UserMaps.end() ; ++ it)
		{
			if ( it->second->id == id)
			{
				name = it->first;
				const string password_sha1 = SystemMisc::MakePassword(name,password);
				it->second->password = password_sha1;
				it->second->authErrorCount = 0;

				found = true;
				break;
			}
		}
		if (!found)
		{//IDがありませんでした
			return ScriptRemoconWeb::ResultError(80003 , "");
		}
		ASSERT(id >= 1);
		ASSERT(!name.empty());

		//アップデートに成功しました!

		//ログインさせる
		EatLoginCookie(cookieArray , id , name );

		//データベースを更新する.
		this->Write(this->DBName);
	}

	//パスワードリセットの申請ファイルを消します。
	XLFileUtil::del( GetUserPath(  id , "/temp/passwordreset.txt" )  );

	{
		//メールを送る
		mailto(name,"server_passwordreset_resetform.mail.tpl");
	}

	string jsonstring;
	jsonstring = string("{ \"result\": \"ok\" }");
	NOTIFYLOG("パスワードリセット完了 name("<<name<<") のパスワードリセットを完了しました。受付ID:" << passwordResetID);
	return _A2U(jsonstring.c_str());
}

string ServerScript::Server_user_wearehere_index(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string direct_uuid = mapfind(request,"direct_uuid");

	string html = ScriptRemoconWeb::OpenTPL(httpHeaders,"server_wearehere.tpl");
	html = XLStringUtil::replace_low(html,"%DIRECT_UUID%", XLStringUtil::htmlspecialchars(direct_uuid) );

	return html;
}

string ServerScript::Server_user_wearehere_search_json(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	
	const map<string,string> request = httpHeaders.getRequest();
	const string uuid = mapfind(request,"uuid");
	if(uuid.empty() )
	{
		ERRORLOG("エラー uuid("<<uuid<<")が空です.");
		return ScriptRemoconWeb::ResultError(100001 , "");
	}

	string url;
	{
		lock_guard<mutex> al(this->lock);
		url = this->WeAreHere.GetMyURL(uuid);
	}

	string jsonstring;
	if (!url.empty())
	{
		jsonstring += string(",") + XLStringUtil::jsonescape("url") + ": " 
			+ XLStringUtil::jsonescape( url ) ;
		jsonstring += string(",") + XLStringUtil::jsonescape("found") + ": " 
			+ XLStringUtil::jsonescape( "1" ) ;
		NOTIFYLOG("WeAreHere uuid("<<uuid<<") で端末が見つかりした。url("<<url<<")");
	}
	else
	{
		NOTIFYLOG("WeAreHere uuid("<<uuid<<") の端末がありませんでした。");
	}

	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";

	return _A2U(jsonstring);
}

string ServerScript::Server_user_wearehere_wait(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	
	const map<string,string> request = httpHeaders.getRequest();

	string html = ScriptRemoconWeb::OpenTPL(httpHeaders,"server_wearehere_wait.tpl");
	return html;
}

//マルチルーム切り替え
string ServerScript::Server_user_remocon_multiroom_change_json(const XLHttpHeader& httpHeaders,map<string,string>* cookieArray)
{
	
	const auto headers = httpHeaders.getHeaderPointer();

	map<string,string> configmap;
	const string link = httpHeaders.getCookieHeader("naichichi2");
	const map<string,string> request = httpHeaders.getPost();
	{
		lock_guard<mutex> al(this->lock);

		if (mapfind(cookieArray,"user") != "1")
		{
			return ScriptRemoconWeb::ResultError(40000,"userクッキーではありません.");
		}
		const unsigned int cookieid = atou( mapfind(cookieArray,"id") );
		const string cookiename = mapfind(cookieArray,"name");
		if (! (cookieid >= 1  && !cookiename.empty() ) )
		{
			return ScriptRemoconWeb::ResultError(40000,"id(" + num2str(cookieid)  + ") name(" + cookiename + ")は不正なクッキーデータです");
		}
		const auto userIT = UserMaps.find(cookiename);
		if (userIT == UserMaps.end() )
		{
			return ScriptRemoconWeb::ResultError(40000,"内部エラー リモコン画面取得のためのユーザー情報がありません");
		}
		configmap = userIT->second->config.ToMap();
	}
	const string my_uuid = mapfind(configmap,"httpd__uuid");

	int key;
	const auto movetype = mapfind(request,"movetype");
	if(movetype=="n")
	{//next
		key = MultiRoomUtil::getNextRoom(my_uuid,configmap,false);
	}
	else 
	{//back
		key = MultiRoomUtil::getNextRoom(my_uuid,configmap,true);
	}

	if (key <= 0)
	{
		return ScriptRemoconWeb::ResultError(10001 , "no room to switch");
	}

	const string prefix = "multiroom_"+num2str(key);
	const string your_uuid = mapfind(configmap,prefix+"_uuid");
	const string your_authkey = mapfind(configmap,prefix+"_authkey");
	const string your_account = mapfind(configmap,prefix+"_account");
	if(your_uuid.empty() || your_authkey.empty() || your_account.empty() )
	{
		return ScriptRemoconWeb::ResultError(10002 , "uuid or authkey or account is empty!");
	}

	{
		lock_guard<mutex> al(this->lock);

		auto userIT = UserMaps.find(your_account);
		if(userIT == UserMaps.end())
		{
			return ScriptRemoconWeb::ResultError(40000,"内部エラー 切り替える部屋のデータがありません (uuidが不明)");
		}
		if ( your_authkey != userIT->second->config.Get("webapi_apikey") )
		{
			const string msg = "your_account:" + your_account
				+ "/->first:" + userIT->first
				+ "/your_authkey:" + your_authkey
				+ "/webapi_apikey:" + userIT->second->config.Get("webapi_apikey")
			;
			return ScriptRemoconWeb::ResultError(40000,"内部エラー 切り替える部屋のauthkeyが正しくない" + msg);
		}

		userIT->second->userLoginTime = time(NULL);	
		userIT->second->status  = "reconfig"; //再度値を取得してもらいたい
		userIT->second->authErrorCount = 0;
		//OKログインできる。

		//cookieにログインの情報を書き込みます。
		EatLoginCookie(cookieArray , userIT->second->id , userIT->first );
		this->Write(this->DBName);
	}

	string jsonstring;
	jsonstring += string(",") + XLStringUtil::jsonescape("location") + ": " 
		+"\"/remocon/\"";

	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return _A2U(jsonstring.c_str());
}


//UUIDのチェック
bool ServerScript::checkUUID(const string& uuid) const
{
	if ( uuid.size() == 36 )
	{//old uuid
		if ( uuid[8] != '-' )
		{
			NOTIFYLOG("OLD UUID 8byte ERROR UUID:" << uuid);
			return false;
		}
		if ( uuid[13] != '-' )
		{
			NOTIFYLOG("OLD UUID 13byte ERROR UUID:" << uuid);
			return false;
		}
		if ( uuid[18] != '-' )
		{
			NOTIFYLOG("OLD UUID 18byte ERROR UUID:" << uuid);
			return false;
		}
		if ( uuid[23] != '-' )
		{
			NOTIFYLOG("OLD UUID 23byte ERROR UUID:" << uuid);
			return false;
		}
		NOTIFYLOG("OK OLD UUID:" << uuid);
		return true;
	}

	const char* p = uuid.c_str();
	const char * end = p + uuid.size() - 4 ;

	unsigned int checksum = 0;
	for( ; p < end ; p++ )
	{
		if (*p == '-')
		{
			continue;
		}
		checksum += *p;
	}

	checksum = checksum % 1000;
	unsigned int targetchecksum =  atoi ( uuid.c_str() + ( uuid.size() - 3 ) ) ;
	if (checksum != targetchecksum)
	{
		NOTIFYLOG("BAD CHECKSUM "<<checksum<<" vs "<<targetchecksum << " UUID:" << uuid);
		return false;
	}
	NOTIFYLOG("OK NEW UUID:" << uuid);
	return true;
}

//メール送信
void ServerScript::mailto(const string& mailaddress,const string& tpl,const map<string,string>& arr)  const
{
	const string mailtemplate = XLFileUtil::cat(GetWebRootPath(tpl));
	map<string,string> temparr = arr;

	temparr["%NAME%"] = XLStringUtil::mailaddress_to_name( mailaddress );
	const string subject = SystemMisc::SendmailGetSubject(mailtemplate,temparr);
	const string body = SystemMisc::SendmailGetBody(mailtemplate,temparr);

	SystemMisc::Sendmail("admin@rti-giken.jp",mailaddress,"admin@rti-giken.jp",subject,body);
}

//メール送信
void ServerScript::mailto(const string& mailaddress,const string& tpl)  const
{
	const map<string,string> arr;
	mailto(mailaddress,tpl,arr);
}


bool ServerScript::checkCookie(const string& path,const map<string,string>* cookieArray,WEBSERVER_RESULT_TYPE* result,string* responsString)  const
{
	string userFlag = mapfind(cookieArray,"user");
	if (userFlag == "1")
	{
		const unsigned int id = atou( mapfind(cookieArray,"id") );
		const string name = mapfind(cookieArray,"name");
		if  (id >= 1  && !name.empty()  )
		{
			return true;
		}
		else
		{
			ERRORLOG("クッキーのID(" << id << ") name("<<name<<")が異常値です" << *cookieArray);
		}
	}
	else
	{
		ERRORLOG("クッキーに userフラグ(" << userFlag << ")がありません" << *cookieArray);
	}

	*result = WEBSERVER_RESULT_TYPE_LOCATION;
	*responsString = "/auth/?backurl=" + path;
	return false;
}


//webからアクセスがあったときに呼ばれます。
bool ServerScript::WebAccess(const string& path,const XLHttpHeader& httpHeaders,map<string,string>* cookieArray,WEBSERVER_RESULT_TYPE* result,string* responsString)
{
	if (path == "/auto/getlink")
	{
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_auto_getlink(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/auto/changeregist")
	{
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_auto_changeregist(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/auto/checkregist")
	{
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_auto_checkregist(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/auto/get_url_by_password")
	{
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_auto_get_url_by_password(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/auto/get_url_by_uuid")
	{
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_auto_get_url_by_uuid(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/auth/")
	{
		*responsString = Server_user_auth_index(httpHeaders,result,cookieArray);
		return true;
	}
	else if (path == "/welcome/")
	{
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_user_welcome_index(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/welcome2/")
	{
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_user_welcome2_index(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/welcome/newregist")
	{
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_user_welcome_newregist(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/welcome/newregist_ok")
	{
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_user_welcome_newregist_ok(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/welcome/newregist_json")
	{
		if (!ScriptRemoconWeb::checkCSRFToken(httpHeaders,result,responsString)) return true;
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = Server_user_welcome_newregist_json(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/auto/pooling")
	{
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_auto_pooling(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/auto/syncconfig")
	{
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_auto_syncconfig(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/auto/syncimage")
	{
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_auto_syncimage(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/auto/syncdelimage")
	{
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_auto_syncdelimage(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/auto/v2syncdel")
	{
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_auto_v2syncdel(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/auto/v2syncfile")
	{
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_auto_v2syncfile(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/auto/v2syncconfig")
	{
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_auto_v2syncconfig(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/auto/wearehere")
	{
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_auto_wearehere(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/secret/change_user_payed")
	{
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_secret_change_user_payed(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/passwordreset/") 
	{//パスワードリセット受付
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_user_passwordreset_index(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/passwordreset/sendmail_json") 
	{//パスワードリセット 受付のメール配信
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = Server_user_passwordreset_sendmail_json(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/passwordreset/resetform") 
	{//パスワードリセット メールから帰ってくるリセットフォーム
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_user_passwordreset_resetform(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/passwordreset/resetform_json") 
	{//パスワードリセット 実際のパスワードリセット
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = Server_user_passwordreset_resetform_json(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/wearehere/") 
	{//端末とのランデブー
		if (! checkCookie(path,cookieArray,result,responsString) ) return true;

		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_user_wearehere_index(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/wearehere/search_json") 
	{//端末とのランデブー 検索
		if (! checkCookie(path,cookieArray,result,responsString) ) return true;
		if (!ScriptRemoconWeb::checkCSRFToken(httpHeaders,result,responsString)) return true;

		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = Server_user_wearehere_search_json(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/wearehere/wait") 
	{//端末とのランデブー
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_user_wearehere_wait(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/remocon/multiroom/change") 
	{//マルチルーム切り替え
		if (! checkCookie(path,cookieArray,result,responsString) ) return true;
		if (!ScriptRemoconWeb::checkCSRFToken(httpHeaders,result,responsString)) return true;

		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = Server_user_remocon_multiroom_change_json(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/") 
	{
		*result = WEBSERVER_RESULT_TYPE_LOCATION;
		*responsString = "/remocon/";
		return true;
	}
	else if (path == "/remocon/")
	{
		if (! checkCookie(path,cookieArray,result,responsString) ) return true;

		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_user_remocon_index(httpHeaders,cookieArray);
		return true;
	}
	else if ( XLStringUtil::strfirst(path.c_str(),"/user/elecicon/")  )
	{//ユーザーが利用しているアイコンの画像など
		if (! checkCookie(path,cookieArray,result,responsString) ) return true;

		*result = WEBSERVER_RESULT_TYPE_TRASMITFILE;
		*responsString = Server_user_image_replace_user_elecion(path.c_str() + sizeof("/user/elecicon/") - 1 ,"image", httpHeaders,cookieArray);
		return true;
	}
	else if ( XLStringUtil::strfirst(path.c_str(),"/user/remocon/") )
	{//リモコン画面
		if (! checkCookie(path,cookieArray,result,responsString) ) return true;

		*result = WEBSERVER_RESULT_TYPE_TRASMITFILE;
		*responsString = Server_user_image_replace_user_elecion(path.c_str() + sizeof("/user/remocon/") - 1 ,"remocon", httpHeaders,cookieArray);
		return true;
	}
	else if ( XLStringUtil::strfirst(path.c_str(),"/user/actionicon/") )
	{//リモコン画面
		if (! checkCookie(path,cookieArray,result,responsString) ) return true;

		*result = WEBSERVER_RESULT_TYPE_TRASMITFILE;
		*responsString = Server_user_image_replace_user_elecion(path.c_str() + sizeof("/user/actionicon/") - 1 ,"actionicon", httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/remocon/fire/bytype")
	{
		if (! checkCookie(path,cookieArray,result,responsString) ) return true;
		if (!ScriptRemoconWeb::checkCSRFToken(httpHeaders,result,responsString)) return true;

		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = Server_user_remocon_fire_bytype(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/remocon/fire/bystring") 
	{//xspeechでの入力
		if (! checkCookie(path,cookieArray,result,responsString) ) return true;
		if (!ScriptRemoconWeb::checkCSRFToken(httpHeaders,result,responsString)) return true;

		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = Server_user_remocon_fire_bystring(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/remocon/get/append/info") 
	{//リモコンの副次的情報の取得
		if (! checkCookie(path,cookieArray,result,responsString) ) return true;
		if (! ScriptRemoconWeb::checkCSRFToken(httpHeaders,result,responsString) ) return true;

		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = Server_user_remocon_get_append_info(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/remocon/sip/call") 
	{//SIPで発信する
		if (! checkCookie(path,cookieArray,result,responsString) ) return true;
		if (! ScriptRemoconWeb::checkCSRFToken(httpHeaders,result,responsString) ) return true;

		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = Server_user_remocon_sip_call(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/remocon/sip/answer") 
	{//SIPでかかってきた電話にこたえる
		if (! checkCookie(path,cookieArray,result,responsString) ) return true;
		if (! ScriptRemoconWeb::checkCSRFToken(httpHeaders,result,responsString) ) return true;

		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = Server_user_remocon_sip_answer(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/remocon/sip/hangup") 
	{//SIPで電話を切る
		if (! checkCookie(path,cookieArray,result,responsString) ) return true;
		if (! ScriptRemoconWeb::checkCSRFToken(httpHeaders,result,responsString) ) return true;

		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = Server_user_remocon_sip_hangup(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/remocon/sip/numpad/close") 
	{//numpadを閉じるイベント
		if (! checkCookie(path,cookieArray,result,responsString) ) return true;
		if (! ScriptRemoconWeb::checkCSRFToken(httpHeaders,result,responsString) ) return true;

		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = Server_user_remocon_sip_numpad_close(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/networksetting/") 
	{
		if (! checkCookie(path,cookieArray,result,responsString) ) return true;

		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = Server_user_networksetting_index(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/networksetting/network/update") 
	{
		if (! checkCookie(path,cookieArray,result,responsString) ) return true;
		if (!ScriptRemoconWeb::checkCSRFToken(httpHeaders,result,responsString)) return true;

		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = Server_user_networksetting_network_update(httpHeaders,cookieArray);
		return true;
	}
	else if ( path == "/networksetting/user/networksetting.reset" )
	{//ネットワークリセット
		if (! checkCookie(path,cookieArray,result,responsString) ) return true;

		*result = WEBSERVER_RESULT_TYPE_TRASMITFILE;
		*responsString = Server_user_networksetting_network_download(httpHeaders,cookieArray);
		return true;
	}
	else if (path == "/networksetting/checkwait") 
	{
		if (! checkCookie(path,cookieArray,result,responsString) ) return true;

		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = Server_user_networksetting_checkwait(httpHeaders,cookieArray);
		return true;
	}
	else if ( path == "/api/elec/action" )
	{
		*responsString = Server_api_elec_action(httpHeaders,result);
		return true;
	}
	else if ( path == "/api/elec/getinfo" )
	{
		*responsString = Server_api_elec_getinfo(httpHeaders,result);
		return true;
	}
	else if ( path == "/api/sensor/get" )
	{
		*responsString = Server_api_sensor_get(httpHeaders,result);
		return true;
	}
	else if( path == "/%ICON%" )
	{
		*result = WEBSERVER_RESULT_TYPE_TRASMITFILE;
		*responsString = GetWebRootPath("jscss/images/ajax-loader.gif");
	}

	return false;
}

#endif //WITH_SERVER_ONLY_CODE
