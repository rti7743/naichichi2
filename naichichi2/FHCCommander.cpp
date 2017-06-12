//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#include "common.h"
#include "Config.h"
#include "MainWindow.h"
#include "FHCCommander.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"
#include "XLHttpSocket.h"
#include "XLUrlParser.h"

string FHCCommander::reparseHost(const string& host)
{
	XLUrlParser parse;
	parse.Parse("naichichi2://"+host);
	if ( parse.getPort() <= 0 )
	{
		return parse.getHost() + ":15551";
	}
	else
	{
		return parse.getHost() + ":" + num2str(parse.getPort());
	}
}

bool FHCCommander::checkAuth(const string& host , const string& key )
{
	
	//一度パースして整形しなおします
	const string host_ = reparseHost(host);
	const string key_  = XLStringUtil::chop(key);


	map<string,string> header;
	header["X-FHCSecret"] = XLStringUtil::sha1( key_ );

	string r;
	try
	{
		r = XLHttpSocket::Get(host_ + "/api/remote_auth" , header ,5 );
	}
	catch(XLException& e)
	{
		ERRORLOG("FHCコマンダー接続中に例外:" << e.what() );
		return false;
	}

	if (r != "OK")
	{
		NOTIFYLOG("FHCコマンダー接続中にOKを返しませんでした 返信:" + r );
		return false;
	}
	return  true;
}
	
bool FHCCommander::Play(const string& host , const string& key , const string& musicFilename )
{
	
	//一度パースして整形しなおします
	const string host_ = reparseHost(host);
	const string key_  = XLStringUtil::chop(key);

	//フォーマット(拡張子)
	const string type = XLStringUtil::urlencode( XLStringUtil::baseext_nodot(musicFilename) );

	//ファイルを読み込んでサーバに送りつける
	vector<char> buffer = XLFileUtil::cat_b( musicFilename );
	if(buffer.empty() )
	{
		ERRORLOG("ファイルが0bytesです。 " + musicFilename );
		return false;
	}

	map<string,string> header;
	header["X-FullFileUpload"] = "1";
	header["X-FHCSecret"] = XLStringUtil::sha1( key_ );


	unsigned int vol = MainWindow::m()->VolumeControll.GetSpeakerVolume();

	string r;
	try
	{
		r = XLHttpSocket::Post(host_ + "/api/play?type=" + type  + "&vol=" + num2str(vol), header ,4 , &buffer[0] , buffer.size() );
	}
	catch(XLException& e)
	{
		ERRORLOG("FHCコマンダー接続中に例外:" << e.what() );
		return false;
	}

	if (r != "OK")
	{
		NOTIFYLOG("FHCコマンダー接続中にOKを返しませんでした 返信:" + r );
		return false;
	}
	return  true;
}
	
bool FHCCommander::Execute(const string& host , const string& key , const string& command,const string& args)
{
	
	//一度パースして整形しなおします
	const string host_ = reparseHost(host);
	const string key_  = XLStringUtil::chop(key);

	map<string,string> header;
	header["X-FHCSecret"] = XLStringUtil::sha1( key_ );
	
	string post = "command=" + XLStringUtil::urlencode(_A2U(command)) 
					+ "&args=" + XLStringUtil::urlencode(_A2U(args)) ;

	string r;
	try
	{
		r = XLHttpSocket::Post(host_ + "/api/execute" ,header ,4, post.c_str() , post.size() );
	}
	catch(XLException& e)
	{
		ERRORLOG("FHCコマンダー接続中に例外:" << e.what() );
		return false;
	}

	if (r != "OK")
	{
		NOTIFYLOG("FHCコマンダー接続中にOKを返しませんでした 返信:" + r );
		return false;
	}
	return  true;
}

