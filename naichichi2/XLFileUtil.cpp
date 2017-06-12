// XLFileUtil.cpp: XLFileUtil クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "XLFileUtil.h"
#include "XLStringUtil.h"
#include "sexylog.h"
#include <sys/stat.h>

#if _MSC_VER
#include <sys/utime.h>
#else
#include <utime.h>
#endif

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

XLFileUtil::XLFileUtil()
{

}

XLFileUtil::~XLFileUtil()
{

}


//ファイルが存在するか?
bool XLFileUtil::Exist(const string & inFileName)
{
	const auto a = XLStringUtil::pathseparator(inFileName);
#if _MSC_VER
	return (0xFFFFFFFF != GetFileAttributes(a.c_str() ));
#else
	struct stat st;
	return stat(a.c_str() ,&st)  == 0;
#endif
}

//削除
bool XLFileUtil::del(const string & inFileName)
{
	const auto filename = XLStringUtil::pathseparator(inFileName);
	::unlink(filename.c_str());
	return true;
}

//ローテートする
struct rotateSt
{
	time_t time;
	string fullfilename;

	rotateSt(time_t time,const string& fullfilename)
		: time(time) , fullfilename(fullfilename)
	{
	}
};
void XLFileUtil::rotate(const string & inDir,const string& filenameprefix , unsigned int limit)
{
	//ファイルを見つけてリストに入れる
	vector<rotateSt> filelist;
	XLFileUtil::findfile(inDir , [&](const string& filename,const string& fullfilename) -> bool {
		if ( strstr(filename.c_str() , filenameprefix.c_str() )  )
		{
			filelist.push_back(rotateSt(getfiletime(fullfilename),fullfilename));
		}
		return true;
	});
	//limit 以上だったら、古いものから消す
	if (filelist.size() < limit)
	{
		return ;
	}
	//古いものを見つけるためにソートする
	sort(filelist.begin() , filelist.end() , [](const rotateSt& a,const rotateSt& b){
		return a.time < b.time ;
	});
	//リストの前の方から消していく
	int delCount = filelist.size() - limit;
	for(auto it = filelist.begin() ; it != filelist.end() ; ++it )
	{
		XLFileUtil::del(it->fullfilename);
		delCount --;
		if (delCount <= 0) break;
	}
	return ;
}

bool XLFileUtil::copy(const string & inFileNameA,const string & inFileNameB)
{
	const auto a = XLStringUtil::pathseparator(inFileNameA);
	const auto b = XLStringUtil::pathseparator(inFileNameB);
#if _MSC_VER
	return Btob(::CopyFile(a.c_str(),b.c_str(),FALSE));
#else
	string command = "/bin/cp -f " + XLStringUtil::escapeshellarg_single(a) + " " + XLStringUtil::escapeshellarg_single(b);
	system(command.c_str());
	return true;
#endif
}

bool XLFileUtil::copydir(const string & inFileNameA,const string & inFileNameB)
{
	const auto a = XLStringUtil::pathseparator(inFileNameA);
	const auto b = XLStringUtil::pathseparator(inFileNameB);
#if _MSC_VER
	return Btob(::CopyFile(a.c_str(),b.c_str(),FALSE));
#else
	string command = "/bin/cp -rf " + XLStringUtil::escapeshellarg_single(a) + " " + XLStringUtil::escapeshellarg_single(b);
	system(command.c_str());
	return true;
#endif
}

bool XLFileUtil::move(const string & inFileNameA,const string & inFileNameB)
{
	const auto a = XLStringUtil::pathseparator(inFileNameA);
	const auto b = XLStringUtil::pathseparator(inFileNameB);

	::rename(a.c_str(),b.c_str());
	return true;
}


//ファイルをすべて string に読み込む.
string XLFileUtil::cat(const string & inFileName)
{
	const auto filename = XLStringUtil::pathseparator(inFileName);
	vector<char> out;

	FILE * fp = fopen(filename.c_str() , "rb");
	//存在しない場合は空
	if (fp == NULL) return "";

#if _MSC_VER
#else
	if (XLStringUtil::strfirst(filename.c_str() ,"/sys/")  || XLStringUtil::strfirst(filename.c_str() ,"/proc/"))
	{//linuxの仮想ファイルはこの手法が使えない
		char buffer[1024];
		while(!feof(fp))
		{
			int readsize = fread( buffer , 1 , sizeof(buffer)  , fp);
			if (readsize <= 0)
			{
				break;
			}
			out.insert(out.end(),buffer,buffer+readsize);
		}
		fclose(fp);
		if (out.empty())
		{
			return "";
		}
		return string(&out[0],0,out.size() ) ;
	}
#endif
	//ケツに持っていって.
	fseek(fp , 0 ,SEEK_END);

	//これでサイズがわかる.
	unsigned long size = ftell(fp);
	if (size <= 0)
	{
		fclose(fp);
		return "";
	}

	//先頭に戻す.
	fseek(fp , 0 ,SEEK_SET);

	//領域を確保して読み込む
	out.resize(size);
	fread( &out[0] , 1 , size  , fp);

	fclose(fp);

	return string(&out[0],0,out.size() ) ;
}

//ファイルをすべて vector<char> に読み込む.
vector<char> XLFileUtil::cat_b(const string & inFileName)
{
	vector<char> out;
	cat_b(inFileName,&out);
	return out;
}

//ファイルをすべて vector<char> に読み込む.
bool XLFileUtil::cat_b(const string & inFileName , vector<char>* out)
{
	out->clear();
	const auto filename = XLStringUtil::pathseparator(inFileName);

	FILE * fp = fopen(filename.c_str() , "rb");
	//存在しない場合は空
	if (fp == NULL) return false;
#if _MSC_VER
#else
	if (XLStringUtil::strfirst(filename.c_str() ,"/sys/")  || XLStringUtil::strfirst(filename.c_str() ,"/proc/"))
	{//linuxの仮想ファイルはこの手法が使えない
		char buffer[1024];
		while(!feof(fp))
		{
			int readsize = fread( buffer , 1 , sizeof(buffer)  , fp);
			if (readsize <= 0)
			{
				break;
			}
			out->insert(out->end(),buffer,buffer+readsize);
		}
		fclose(fp);
		return true;
	}
#endif

	//ケツに持っていって.
	fseek(fp , 0 ,SEEK_END);

	//これでサイズがわかる.
	unsigned long size = ftell(fp);
	if(size<=0)
	{
		fclose(fp);
		return false;
	}

	//先頭に戻す.
	fseek(fp , 0 ,SEEK_SET);

	//領域を確保して読み込む
	out->resize(size);
	fread( &((*out)[0]) , 1 , size  , fp);
	
	fclose(fp);

	return true;
}

//ファイルの終端からN行読み込む.(あんまり効率は良くない)
string XLFileUtil::tail(const string & inFileName,unsigned int lines)
{
	//すごく効率の悪いやり方
	auto filelines = XLStringUtil::split_vector("\n", cat(inFileName) );
	if ( filelines.size() < lines )
	{
		return XLStringUtil::join("\n",filelines);
	}
	string r ;
	for(unsigned int i = filelines.size() - lines ; i < filelines.size() ; i ++ )
	{
		r += filelines[i] + "\n";
	}
	return r;
}


string XLFileUtil::pwd()
{
	char buffer[MAX_PATH] = {0};
#if _MSC_VER
	GetCurrentDirectory(MAX_PATH , buffer);
#else
	getcwd(buffer,MAX_PATH-1);
#endif
	return buffer;
}

string XLFileUtil::getTempDirectory(const string& projectname)
{
#if _MSC_VER
	char buffer[MAX_PATH] = {0};
	::GetTempPath(MAX_PATH , buffer);
	const string retTemp = buffer + projectname;
	if (!Exist(retTemp))
	{
		CreateDirectory(retTemp.c_str(),NULL);
	}
#else
	const string retTemp = "/tmp/" + projectname;
	if (!Exist(retTemp))
	{
		mkdir(retTemp.c_str(),0755);
	}
#endif
	return retTemp;
}

//実行ファイルがあるディレクトリ
string XLFileUtil::getSelfExeDirectory()
{
	return XLStringUtil::basedir(getSelfEXEPath());
}

//自分自身へのパス
string XLFileUtil::getSelfEXEPath()
{
	char buffer[MAX_PATH];
#if _MSC_VER
	if(!GetModuleFileName(NULL, buffer, MAX_PATH))
	{
		return "";
	}
#else
	int r = readlink("/proc/self/exe", buffer, MAX_PATH); 
	if (r < 0)
	{
		return "";
	}
#endif
	return buffer;
}


bool XLFileUtil::findfile(const string & dir,const std::function< bool(const string& filename,const string& fullfilename) >& callback)
{
	const auto dirname = XLStringUtil::pathseparator(dir);

#if _MSC_VER
	const auto whilddirname = dirname + PATHSEP + "*.*";

	WIN32_FIND_DATA data;
	HANDLE han = ::FindFirstFile(whilddirname.c_str() , &data);
	if (han == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	do
	{
		if (
			(data.cFileName[0] == '.' && data.cFileName[1] == 0)
				||
			(data.cFileName[0] == '.' && data.cFileName[1] == '.' && data.cFileName[2] == 0)
			)
		{
			continue;
		}

		if ( ! callback(data.cFileName,dirname + PATHSEP + data.cFileName) )
		{
			break;
		}
	}
	while( ::FindNextFile(han,&data) );
	FindClose(han);
	return true;

#else
	DIR * han = opendir(dirname.c_str());
	if (!han)
	{
		return false;
	}
	dirent* entry;
	while (entry = readdir(han) ) {
		if (
			(entry->d_name[0] == '.' && entry->d_name[1] == 0)
				||
			(entry->d_name[0] == '.' && entry->d_name[1] == '.' && entry->d_name[2] == 0)
			)
		{
			continue;
		}

		if ( ! callback(entry->d_name,dirname + PATHSEP + entry->d_name) )
		{
			break;
		}
	}
	closedir(han);
#endif
	return true;
}


bool XLFileUtil::findfile_orderby(const string & dir,findfile_orderby_enum orderby,const std::function< bool(const string& filename,const string& fullfilename) >& callback)
{
	const auto dirname = XLStringUtil::pathseparator(dir);

#if _MSC_VER
	const auto whilddirname = dirname + PATHSEP + "*.*";
	struct FileEntry
	{
		string name;
		unsigned __int64 size;
		unsigned __int64 date;
	};
	vector<FileEntry*> entrys;

	WIN32_FIND_DATA data;
	HANDLE han = ::FindFirstFile(whilddirname.c_str() , &data);
	if (han == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	do
	{
		if (
			(data.cFileName[0] == '.' && data.cFileName[1] == 0)
				||
			(data.cFileName[0] == '.' && data.cFileName[1] == '.' && data.cFileName[2] == 0)
			)
		{
			continue;
		}
		FileEntry *f = new FileEntry;
		f->name = data.cFileName;
		f->size = static_cast<__int64>(data.nFileSizeHigh) << 32 | data.nFileSizeLow;
		f->date = static_cast<__int64>(data.ftLastWriteTime.dwHighDateTime) << 32 | data.ftLastWriteTime.dwLowDateTime;
		entrys.push_back(f);
	}
	while( ::FindNextFile(han,&data) );
	FindClose(han);
#else
	struct FileEntry
	{
		string name;
		size_t size;
		time_t date;
	};
	vector<FileEntry*> entrys;

	DIR * han = opendir(dirname.c_str());
	if (!han)
	{
		return false;
	}
	dirent* entry;
	while (entry = readdir(han) ) {
		if (
			(entry->d_name[0] == '.' && entry->d_name[1] == 0)
				||
			(entry->d_name[0] == '.' && entry->d_name[1] == '.' && entry->d_name[2] == 0)
			)
		{
			continue;
		}


		FileEntry *f = new FileEntry;
		f->name = entry->d_name;

		struct  stat st = {0};
		const string fullname  = dirname + PATHSEP + entry->d_name;
		::stat( fullname.c_str() ,&st );
		f->size = st.st_size;
		f->date = st.st_ctime > st.st_mtime ? st.st_ctime : st.st_mtime;
		entrys.push_back(f);
	}
	closedir(han);
#endif
	switch(orderby)
	{
	case findfile_orderby_name_ascending:
	default:
		sort(entrys.begin() ,entrys.end() , [&](const FileEntry* a,const FileEntry* b) -> int {
			return strcmp(a->name.c_str(),b->name.c_str()) < 0 ; 
		});
		break;
	case findfile_orderby_name_descending:
		sort(entrys.begin() ,entrys.end() , [&](const FileEntry* a,const FileEntry* b) -> int {
			return strcmp(b->name.c_str(),a->name.c_str()) < 0; 
		});
		break;
	case findfile_orderby_size_ascending:
		sort(entrys.begin() ,entrys.end() , [&](const FileEntry* a,const FileEntry* b) -> int {
			return INTCMP(a->size,b->size);
		});
		break;
	case findfile_orderby_size_descending:
		sort(entrys.begin() ,entrys.end() , [&](const FileEntry* a,const FileEntry* b) -> int {
			return INTCMP(b->size,a->size);
		});
		break;
	case findfile_orderby_date_ascending:
		sort(entrys.begin() ,entrys.end() , [&](const FileEntry* a,const FileEntry* b) -> int {
			return INTCMP(a->date,b->date);
		});
		break;
	case findfile_orderby_date_descending:
		sort(entrys.begin() ,entrys.end() , [&](const FileEntry* a,const FileEntry* b) -> int {
			return INTCMP(b->date,a->date);
		});
		break;
	}

	for(auto it = entrys.begin() ; it != entrys.end() ; ++it )
	{
		if ( ! callback((*it)->name,dirname + PATHSEP + (*it)->name) )
		{
			break;
		}
	}
	for(auto it = entrys.begin() ; it != entrys.end() ; ++it )
	{
		delete *it;
	}
	return true;
}



size_t XLFileUtil::getfilesize(const string & inFileName)
{
	struct  stat st = {0};
	::stat( inFileName.c_str() ,&st );
	return st.st_size;
}

time_t XLFileUtil::getfiletime(const string & inFileName)
{
	struct  stat st = {0};
	::stat( inFileName.c_str() ,&st );
	
	return st.st_ctime > st.st_mtime ? st.st_ctime : st.st_mtime;
}


bool XLFileUtil::setfiletime(const string & inFileName,time_t now)
{
#if _MSC_VER
	struct _utimbuf val;
#else
	struct utimbuf val;
#endif
	val.actime = now;
	val.modtime = now;
#if _MSC_VER
	int r = _utime(inFileName.c_str(),&val);
#else
	int r = utime(inFileName.c_str(),&val);
#endif
	return (r == 0);
}



//inStr を ファイルに書き込む
bool XLFileUtil::write(const string & inFileName,const string & inStr )
{
	return write( inFileName , inStr.c_str() , inStr.size());
}

//inBuffer を ファイルに書き込む
bool XLFileUtil::write(const string & inFileName,const vector<char> & inBuffer)
{
	return write(inFileName,&inBuffer[0] ,  inBuffer.size());
}
//inBuffer を ファイルに書き込む
bool XLFileUtil::write(const string & inFileName,const char* data , int size)
{
	FILE * fp = fopen(inFileName.c_str() , "wb");
	if (fp == NULL)
	{
		return false;
	}

	fwrite( data , 1, size  , fp);

	fclose(fp);
	return true;
}

void XLFileUtil::mkdirP(const string &dir)
{
	const auto dirname = XLStringUtil::pathseparator(dir);
	const auto dirs = XLStringUtil::split(PATHSEP,dirname);

	string path;
	for(auto it = dirs.begin();it != dirs.end(); ++it)
	{
		path += *it + PATHSEP;
		if (!Exist(path))
		{
	#if _MSC_VER
			CreateDirectory(path.c_str(),NULL);
	#else
			mkdir(path.c_str(),0755);
	#endif
		}
	}
}


void XLFileUtil::DelTree(const string &path)
{
	auto pathname = XLStringUtil::pathseparator(path);
	#if _MSC_VER
		if (pathname == "c:\\") return ;
		if (pathname == "C:\\") return ;
	#else
		if (pathname == "/") return ;
	#endif
	if (pathname == "") return ;
	//とりあえずセーフティかけとくかw
	if (pathname.size() <= 7 ) return ;

	if ( Exist(pathname) )
	{//ファイル？
		XLFileUtil::del(pathname);
		if (! Exist(pathname) )
		{
			return ;
		}
	}
	else
	{//ディレクトリかな
		pathname = pathname + PATHSEP;
		if (! Exist(pathname) )
		{//もう存在しないらしい
			return ;
		}
	}

	//pathnameはディレクトリ /終端 が入っている。
	//ディレクトリを巡回しながら消していく
	findfile(pathname , [&](const string& filename,const string& fullfilename) -> bool{
		auto dirname = fullfilename + PATHSEP;
		if (Exist(dirname) )
		{//ディレクトリなので再帰する
			DelTree(fullfilename);
		}
		else
		{
			del(fullfilename);
		}
		return true;
	});

}

string XLFileUtil::FindOneDirecotry(const string& dir ,const string& name)
{
	string retDirname ;
	findfile(dir , [&](const string& filename,const string& fullfilename) -> bool{
		if (! XLStringUtil::strfirst(filename.c_str(),name.c_str() ) )
		{
			return true;
		}
		retDirname = fullfilename;
		return false;
	});

	return retDirname;
}

//ファイルがない場合はなんとかして作成する
bool XLFileUtil::touch(const string & inFileName)
{
	const auto _filename = XLStringUtil::pathseparator(inFileName);
	if ( XLFileUtil::Exist( _filename ) )
	{//ファイルがある
		return true;
	}

	//作ってみる
	FILE * fp = fopen(_filename.c_str() , "wb");
	if (fp != NULL)
	{
		fclose(fp);
		return true;
	}

	//ディレクトが壊れてるのかなあ？
	const auto _basedir = XLStringUtil::basedir(_filename);
	if ( XLFileUtil::Exist( _basedir ) )
	{//ディレクトリがあるのに作れない場合はギブアップ.
		return false;
	}

	//ディレクトリ作成
	XLFileUtil::mkdirP(_basedir);

	//再度作ってみる
	fp = fopen(_filename.c_str() , "wb");
	if (fp != NULL)
	{
		fclose(fp);
		return true;
	}
	
	//ギブアップ.
	return false;
}
