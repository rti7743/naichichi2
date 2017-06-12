// XLFileUtil.h: XLFileUtil クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XLFILEUTIL_H__A51E414C_8F9A_46E5_A5CF_364F04C9FA00__INCLUDED_)
#define AFX_XLFILEUTIL_H__A51E414C_8F9A_46E5_A5CF_364F04C9FA00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class XLFileUtil  
{
public:
	XLFileUtil();
	virtual ~XLFileUtil();

	//ファイルが存在するか?
	static bool Exist(const string & inFileName);
	//削除
	static bool del(const string & inFileName);
	//ローテートする
	static void rotate(const string & inDir,const string& filenameprefix ,unsigned  int limit);
	static bool copy(const string & inFileNameA,const string & inFileNameB);
	static bool copydir(const string & inFileNameA,const string & inFileNameB);
	//カレントディレクトリ
	static string pwd();
	static string getCurrentDirectory(){ return pwd(); }
	//テンポラリディレクトリ
	static string getTempDirectory(const string& projectname);
	//実行ファイルがあるディレクトリ
	static string getSelfExeDirectory();
	//自分自身へのパス
	static string getSelfEXEPath();

	enum findfile_orderby_enum
	{
		 findfile_orderby_name_ascending
		,findfile_orderby_name_descending
		,findfile_orderby_size_ascending
		,findfile_orderby_size_descending
		,findfile_orderby_date_ascending
		,findfile_orderby_date_descending
	};

	static bool findfile(const string & dir,const std::function< bool(const string& filename,const string& fullfilename) >& callback);
	static bool findfile_orderby(const string & dir,findfile_orderby_enum orderby,const std::function< bool(const string& filename,const string& fullfilename) >& callback);

	static bool move(const string & inFileNameA,const string & inFileNameB);
	//ファイルをすべて string に読み込む.
	static string cat(const string & inFileName);
	//ファイルをすべて vector<char> に読み込む.
	static vector<char> cat_b(const string & inFileName);
	//ファイルをすべて out に読み込む
	static bool cat_b(const string & inFileName , vector<char>* out);
	//ファイルの終端からN行読み込む.(あんまり効率は良くない)
	static string tail(const string & inFileName,unsigned int lines);
	//ファイルがない場合はなんとかして作成する
	static bool touch(const string & inFileName);

	//inStr を ファイルに書き込む
	static bool write(const string & inFileName,const string & inStr );
	//inBuffer を ファイルに書き込む
	static bool write(const string & inFileName,const vector<char> & inBuffer);
	//inBuffer を ファイルに書き込む
	static bool write(const string & inFileName,const char* data , int size);

	static  size_t getfilesize(const string & inFileName);

	static  time_t getfiletime(const string & inFileName);
	static  bool setfiletime(const string & inFileName,time_t now);

	static void mkdirP(const string &dirname);

	static void DelTree(const string &path);

	static string FindOneDirecotry(const string& dir ,const string& name);
};

#endif // !defined(AFX_XLFILEUTIL_H__A51E414C_8F9A_46E5_A5CF_364F04C9FA00__INCLUDED_)
