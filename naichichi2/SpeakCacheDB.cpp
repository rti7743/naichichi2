#include "common.h"
#include "MainWindow.h"
#include "XLFileUtil.h"
#include "XLStringUtil.h"
#include "MecabControl.h"
#include "SpeakCacheDB.h"

SpeakCacheDB::SpeakCacheDB()
{
}

SpeakCacheDB::~SpeakCacheDB()
{
}


void SpeakCacheDB::Create(const string& ext)
{
	this->SpeakCacheDir = MainWindow::m()->Config.Get("speak__cachedir", MainWindow::m()->GetConfigBasePath("speakcache") );
	this->Ext = "." + ext;
}

bool SpeakCacheDB::Find(const string& str,string* outFilename)
{
	assert( !str.empty() );
	
	//登録するファイルを作る
	const auto fullname = XLStringUtil::pathseparator(
		this->SpeakCacheDir + "/" + XLStringUtil::md5(str) + this->Ext );
	*outFilename = fullname;

	//実ファイルにはありますか？
	if ( XLFileUtil::Exist(fullname) )
	{//実ファイルにあるので再利用
		time_t now = time(NULL);
		XLFileUtil::setfiletime(fullname, now );

		return true;
	}
	else
	{//実ファイルにない
		
		GC();	//こっそりGCを回す.
		return false;
	}
}

void SpeakCacheDB::GC()
{
	time_t removetime = 60 * 60 * 24 * 7; //一週間以上使われていないものは消す.
	time_t gctime = time(NULL);

	//指定時間より前のものを消す
	if (gctime > removetime) gctime = gctime - removetime;
	else					 gctime = 0;
	
	XLFileUtil::findfile(this->SpeakCacheDir , [&](const string& filename,const string& fullfilename) -> bool {
		const time_t t = XLFileUtil::getfiletime(fullfilename);
		if (t < gctime )
		{
			DEBUGLOG("GCでスピーチキャッシュを消します: " << fullfilename );
			XLFileUtil::del(fullfilename);
		}

		return true;
	});
}

void SpeakCacheDB::Delete(const string& str)
{
	assert( !str.empty() );

	const auto fullname = XLStringUtil::pathseparator(
		this->SpeakCacheDir + "/" + XLStringUtil::md5(str) + this->Ext );
	XLFileUtil::del(fullname);
}

void SpeakCacheDB::Clear()
{
	XLFileUtil::findfile( this->SpeakCacheDir , [&](const string& filename,const string& fullfilename) -> bool {
		XLFileUtil::del(fullfilename);
		return true;
	});
}
