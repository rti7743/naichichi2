//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include "ColudSyncConfig.h"
#include "MainWindow.h"
#include "SystemMisc.h"
#include "XLFileUtil.h"
#include "XLHttpSocket.h"
#include "XLGZip.h"
#include "ScriptRemoconWeb.h"

ColudSyncConfig::ColudSyncConfig()
{
	this->Thread = NULL;
	this->StopFlag = false;
	this->isSync = false;
}

ColudSyncConfig::~ColudSyncConfig()
{
	DEBUGLOG("stop...");
	Stop();
	DEBUGLOG("done");
}

void ColudSyncConfig::Create(const string& serverurl)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	this->StopFlag = false;
	this->serverurl = serverurl;
	this->SyncTarget = SyncTarget_Full;
	this->isSync = false;

	this->Thread = new thread([=](){
		this->ThreadMain(); 
	});
}
void ColudSyncConfig::Stop()
{
	if (this->Thread)
	{
		this->StopFlag = true;
		this->EventObject.notify_one();
		this->Thread->join();
		delete this->Thread;
		this->Thread = NULL;
	}
}
void ColudSyncConfig::WakeupByNewSyncID(const string& linkid)
{
	lock_guard<mutex> al(this->lock);	

	this->linkid = linkid;
	this->SyncTarget = SyncTarget_Full;
	this->isSync = false;
	this->EventObject.notify_one();
}

void ColudSyncConfig::WakeupByconfig()
{
	lock_guard<mutex> al(this->lock);
	this->SyncTarget = SyncTarget_Full;
	this->isSync = false;
	this->EventObject.notify_one();
}
void ColudSyncConfig::WakeupByconfigOnly()
{
	lock_guard<mutex> al(this->lock);
	this->SyncTarget = SyncTarget_ConfigOnly;
	this->isSync = false;
	this->EventObject.notify_one();
}

bool ColudSyncConfig::IsSync() const
{
	return this->isSync;
}

void ColudSyncConfig::ThreadMain()
{
	while(! this->StopFlag)
	{
		//寝る
		{
			unique_lock<mutex> al(this->lock);
			this->EventObject.wait(al);
		}
		if ( this->StopFlag )
		{
			break;
		}
		if ( this->linkid.empty() )
		{
			continue;
		}

		try
		{
			if (this->SyncTarget == SyncTarget_ConfigOnly)
			{
				SyncConfgOnly();
			}
			else
			{
				SyncAll();
			}
			this->isSync = true;
		}
		catch(XLException& e)
		{
			ERRORLOG("サーバとの同期中にエラー:" << e.what() );
		}
	}
}

void ColudSyncConfig::SyncConfgOnly()
{
	DEBUGLOG("configだけ同期を開始します");

	const string imglistTXT = SyncConfig();
	if ( this->StopFlag )
	{
		return ;
	}
	DEBUGLOG("config同期の結果:" + imglistTXT);

	const auto vec = XLStringUtil::split_vector("\t",imglistTXT);
	auto it = vec.begin();
	if ( it == vec.end())
	{
		throw XLEXCEPTION("受信したデータが壊れています データ:" << imglistTXT);
	}
	if ( *it != "OK" )
	{
		throw XLEXCEPTION("サーバがエラーを返しました データ:" << imglistTXT);
	}
}

void ColudSyncConfig::SyncAll()
{
	DEBUGLOG("config同期を開始します");

	const string imglistTXT = SyncConfig();
	if ( this->StopFlag )
	{
		return ;
	}
	DEBUGLOG("config同期の結果:" + imglistTXT);

	const auto vec = XLStringUtil::split_vector("\t",imglistTXT);
	auto it = vec.begin();
	if ( it == vec.end())
	{
		throw XLEXCEPTION("受信したデータが壊れています データ:" << imglistTXT);
	}
	if ( *it != "OK" )
	{
		throw XLEXCEPTION("サーバがエラーを返しました データ:" << imglistTXT);
	}
	//OKを飛ばす
	++it;

	map<string , ColudSyncConfig::uploadfiles> timetable;
	for(  ; it != vec.end() ; it++ )
	{
		const string line = *it;
		if ( line.empty() )
		{
			continue;
		}

		const auto vec = XLStringUtil::split_vector("<>",line );
		if (vec.size() <= 2)
		{
			ERRORLOG("受信したデータが壊れています。タブで3カラム以下です データ:" <<  imglistTXT << " 該当行:" << line);
			continue;
		}
		const string filename = vec[0]; //
		const string unixtime = vec[1]; //
		const string type = vec[2];  //

		if ( ! XLStringUtil::checkSafePath(filename) )
		{//危険なファイル名です
			ERRORLOG("サーバが返してきたファイル" + filename + "は危険な名前です。無視します。 該当行:" + line);
			continue;
		}

		time_t time = atou( unixtime );
		timetable[type + "/" + filename] = ColudSyncConfig::uploadfiles(type,filename,time , false, false);
	}
	V2SyncDirecotry("remocon",&timetable);
	V2SyncDirecotry("elecicon",&timetable);
	V2SyncDirecotry("actionicon",&timetable);
}

string ColudSyncConfig::SyncConfig()
{
	
	
	//もし httpd_url が 127.0.0.1 になっている場合は再設定を行なってみる
	const string httpd__url = MainWindow::m()->Config.Get("httpd__url","");
	if ( httpd__url.empty() || strstr( httpd__url.c_str() ,"127.0.0.1") )
	{
		NOTIFYLOG("httpd__urlがローカルになっているので再設定し直します。");
		//configの設定変更と保存
		MainWindow::m()->SyncInvoke([=](){
			ScriptRemoconWeb::SettingPageIPAddressOverriade();
			MainWindow::m()->Config.overrideSave();
		});
	}
	
	
	vector<char> buffer = XLFileUtil::cat_b( MainWindow::m()->GetConfigBasePath("./config.conf") );
	if(buffer.empty() )
	{
		ERRORLOG("SyncConfig: configが0bytesです。 ");
		return "";
	}
	XLGZip gzip;
	if ( ! gzip.gzip( &buffer[0] , buffer.size() , 9) )
	{
		ERRORLOG("SyncConfig: configを圧縮できません。 ");
		return "";
	}

	map<string,string> header;
	header["X-FullFileUpload"] = "1";
	header["Cookie"] = linkid;
	header["Accept-Encoding"] = "gzip";
	header["Content-Type"] = "application/plain";

	return _U2A(XLHttpSocket::Post(this->serverurl + "auto/v2syncconfig",header , gzip.getData() , gzip.getSize() ));
}

void ColudSyncConfig::V2SyncDirecotry(const string type,map<string , ColudSyncConfig::uploadfiles>* timetable)
{
	string dir;
	if (type == "elecicon")
	{
		dir = MainWindow::m()->GetConfigBasePath("./webroot/user/elecicon/");
	}
	else if (type == "remocon")
	{
		dir = MainWindow::m()->GetConfigBasePath("./webroot/user/remocon/");
	}
	else if (type == "actionicon")
	{
		dir = MainWindow::m()->GetConfigBasePath("./webroot/user/actionicon/");
	}
	else
	{
		DEBUGLOG("未対応のディレクトリです。" << type);
		return ;
	}

	XLFileUtil::findfile(dir , [&](const string& filename,const string& fullfilename) -> bool {
		if ( this->StopFlag )
		{
			return false;
		}

		const time_t time = XLStringUtil::LocalTimetoGMT( XLFileUtil::getfiletime(fullfilename) );
		const string searchkey = type + "/" + filename;
		const auto timetableIT = timetable->find(searchkey);

		if ( timetableIT == timetable->end() )
		{
			timetable->insert(
				pair<string , ColudSyncConfig::uploadfiles>( searchkey,ColudSyncConfig::uploadfiles(type,filename,time,true,true) ) 
			);
			DEBUGLOG("ローカルのファイル" << searchkey << "はサーバにないのでアップロードします。");
		}
		else if ( timetableIT->second.isupload )
		{
			timetableIT->second.islocalexist = true;
			DEBUGLOG("ローカルのファイルは既にアップロードしています。???");
		}
		else if ( time > timetableIT->second.time )
		{
			timetableIT->second.isupload = true;
			timetableIT->second.islocalexist = true;
			DEBUGLOG("ローカルのファイル" << searchkey << "は新しくなっているのでアップロードします。local:" << num2str(time) << " server:" + num2str(timetableIT->second.time) );
		}
		else
		{
			timetableIT->second.isupload = false;
			timetableIT->second.islocalexist = true;
//			DEBUGLOG("ローカルのファイル" << searchkey << "は同期しません。local:" << num2str(time) << " server:" << num2str(timetableIT->second.time)  );
			return true;
		}

		//サーバーにないか古いならば更新
		string imageResult = V2SyncFile(type,fullfilename ,filename, time);
		const auto imageResultVec = XLStringUtil::split_vector("\t",imageResult);

		auto imageResultIT = imageResultVec.begin();
		if ( imageResultIT == imageResultVec.end())
		{
			ERRORLOG("受信したデータが壊れています データ:" << imageResult);
			return true;	//エラーで止まらずに突き進む
		}
		if ( *imageResultIT != "OK" )
		{
			ERRORLOG("サーバがエラーを返しました データ:" << imageResult);
			return true;	//エラーで止まらずに突き進む
		}
		return true;
	});

	//サーバにしかないものは消す
	for(auto it = timetable->begin() ; it != timetable->end() ; ++it)
	{
		if ( this->StopFlag )
		{
			return ;
		}
		if (it->second.type != type )
		{//種類じゃない
			continue;
		}
		if (it->second.islocalexist)
		{//ローカルにあるものはそのまま
			continue;
		}
		DEBUGLOG("サーバのファイル" << it->second.type << "/" << it->second.filename << "はローカルから削除されているので消します。"  );
		V2SyncDel(it->second.type,it->second.filename);
	}
}


string ColudSyncConfig::V2SyncFile(const string& type,const string& fullfilename ,const string& filename , time_t time)
{
	
	vector<char> buffer = XLFileUtil::cat_b( fullfilename );
	if(buffer.empty() )
	{
		ERRORLOG("SyncImage: ファイルが0bytesです。 " + fullfilename );
		return "";
	}

	map<string,string> header;
	header["X-FullFileUpload"] = "1";
	header["Cookie"] = linkid;
	header["Accept-Encoding"] = "gzip";
	header["Content-Type"] = "application/plain";

	string getparam;
	getparam = "?type=" + XLStringUtil::urlencode(_A2U(type));
	getparam+= "&filename=" + XLStringUtil::urlencode(_A2U(filename));

	return _U2A(XLHttpSocket::Post(this->serverurl + "auto/v2syncfile" + getparam ,header , &buffer[0] , buffer.size() ));
}

string ColudSyncConfig::V2SyncDel(const string& type,const string& filename )
{
	
	map<string,string> header;
	header["Cookie"] = linkid;
	header["Accept-Encoding"] = "gzip";

	string getparam;
	getparam = "?type=" + XLStringUtil::urlencode(_A2U(type));
	getparam+= "&filename=" + XLStringUtil::urlencode(_A2U(filename));

	return _U2A(XLHttpSocket::Get(this->serverurl + "auto/v2syncdel" + getparam ,header));
}



#endif //WITH_CLIENT_ONLY_CODE