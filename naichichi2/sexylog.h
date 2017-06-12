#pragma once

//ヘッダ単体だけで使えるログクラス
//
//DEBUGLOG("デバッグ" << "の時だけ" << "ログに出すよ" << 123);
//NOTIFYLOG("一般的な通知をログに出すよ" << 123);
//ERRORLOG("エラーをログに出すよ" << 123);
//
//何もしなければコンソール出力
//
//コンソールに出したくない場合はこれを定義
//sexylog:m()->SetCanStdout(false);
//
//ファイルに出した場合
//sexylog:m()->SetLogFile("log.txt");
//
//sexylog:m()->SetLogFile(ファイル名,バッファサイズ,ローテートサイズ);
//sexylog:m()->SetLogFile("log.txt",65535,1024*1024*1);
//
//とりあえずログファイルのローテートは1代だけ行われます.
//
//ログを見たいときは、TailByteで見れます.
//ローテートされていても大丈夫、つなぎ合わせてちゃんと表示します.
//std::string logout = sexylog:m()->TailByte(1000); //最後から1000バイト分のログを見る
//std::string logout = sexylog:m()->Tail(100);      //最後から100行のログを見る
//
//今すぐローテートしたい場合
//sexylog:m()->Rotate();
//
//テスト等で、ログをすべて消したい場合
//sexylog:m()->CleanupLogFile();
//

typedef void(*SEXYLOGOUT_LOGGER_FUNCTION)(const std::string& msg);

enum SEXYLOG_LEVEL
{
	 SEXYLOG_LEVEL_DEBUG = 0			//デバッグなどに必要な情報。
	,SEXYLOG_LEVEL_NOTIFY = 3			//通知
	,SEXYLOG_LEVEL_ERROR = 9			//エラー
};

//ログ出力
#ifndef SEXYLOGOUT
#define SEXYLOGOUT(LOGTYPE,LOGMESSAGE) {\
	if ( (int)(LOGTYPE) >= (int)(sexylog::m()->getLevel()) ) { \
		std::stringstream LOGOUTSTREAM; \
		LOGOUTSTREAM << sexylog::m()->now(::time(NULL)) \
			<< "\t" << LOGTYPE << "\t" << __FUNCTION__ << ":" << __LINE__ << "\t" << LOGMESSAGE << "\r\n"; \
		sexylog::m()->Echo( LOGOUTSTREAM.str() );\
	} \
}
#endif //SEXYLOGOUT

//デバッグ用のログ
#ifndef DEBUGLOG
	//定義されていないなら定義する
	#if _DEBUG
		//デバッグの時のみ有効にする
		#define DEBUGLOG(LOGMESSAGE)  SEXYLOGOUT(SEXYLOG_LEVEL_DEBUG,LOGMESSAGE)
	#else
		#define DEBUGLOG(LOGMESSAGE)
	#endif //_DEBUG
#endif //DEBUGLOG

//一般的なログ
#ifndef NOTIFYLOG
	#define NOTIFYLOG(LOGMESSAGE) SEXYLOGOUT(SEXYLOG_LEVEL_NOTIFY,LOGMESSAGE)
#endif

//エラー
#ifndef ERRORLOG
	#define ERRORLOG(LOGMESSAGE)  SEXYLOGOUT(SEXYLOG_LEVEL_ERROR,LOGMESSAGE)
#endif


class sexylog
{
private:
	//singleton
	sexylog()
	{
		this->FP = NULL;
		this->level = SEXYLOG_LEVEL_NOTIFY;
		this->canStdout = true;			//ディフォルトは画面出力
		this->rotateSize = 1024*1024*1; //1MB 
		this->customLoggerFunc = NULL;
	}
public:
	virtual ~sexylog()
	{
		Flush();
		Close();
	}

	//singleton
	static sexylog* m()
	{
		static sexylog a;
		return &a;
	}
	
	void Close()
	{
		if (this->FP)
		{
			fclose(this->FP);
			this->FP = NULL;
		}
	}
	
	bool SetLogFile(const std::string& logfilename,unsigned int logBufferByte = 65535,unsigned int logRotateByte = 1024*1024*1)
	{
		Close();
		this->rotateSize = logRotateByte;

		
		this->FP = fopen(logfilename.c_str() , "a+b");
		if (!this->FP)
		{
			printf("can not open log file : %s",logfilename.c_str() );
			return false;
		}

		this->FPBuffer.resize(logBufferByte);
		setbuf(this->FP,&this->FPBuffer[0]);

		this->filename = logfilename;
		return true;
	}
	void Flush()
	{
		lock_guard<mutex> al(this->lock);
		if (this->FP)
		{
			fflush(this->FP);
		}
	}
	void Echo(const std::string& log)
	{
		if (this->customLoggerFunc)
		{
			this->customLoggerFunc(log);
		}

		if (this->canStdout)
		{
			puts(log.c_str());
		}

		if (!this->FP)
		{//ファイルにかかない.
			return ;
		}

		{
			lock_guard<mutex> al(this->lock);	
			fwrite( log.c_str(), sizeof(char) , log.size() , this->FP );

			if (this->rotateSize <= 0)
			{//ローテートしない
				return ;
			}
			//ローテートサイズを超えていたら新規にスタートする
			unsigned long pos = ftell(this->FP);
			if ( pos <  this->rotateSize )
			{
				return ;
			}
		}

		Rotate();
	}

	void SetLogLevel(SEXYLOG_LEVEL level)
	{
		this->level = level;
	}
	SEXYLOG_LEVEL getLevel() const
	{
		return this->level;
	}
 	void SetCanStdout(bool canStdout)
	{
		this->canStdout = canStdout;
	}
 	void SetCustomLoggerFunc(SEXYLOGOUT_LOGGER_FUNCTION func)
	{
		this->customLoggerFunc = func;
	}

	std::string getFilename() const
	{
		return this->filename;
	}
	std::string getFilenameBack() const
	{
		return this->filename + ".back";
	}
	
	std::string now(const time_t& time)
	{
	#if _MSC_VER
		struct tm *date = localtime(&time);
	#else
		struct tm dateT;
		localtime_r(&time,&dateT);

		struct tm *date = &dateT;
	#endif
		char buffer[30];
		#if snprintf
			snprintf(buffer,30,"%d/%d/%d %d:%d:%d",date->tm_year + 1900,date->tm_mon + 1,date->tm_mday,date->tm_hour,date->tm_min,date->tm_sec);
		#else
			sprintf(buffer,"%d/%d/%d %d:%d:%d",date->tm_year + 1900,date->tm_mon + 1,date->tm_mday,date->tm_hour,date->tm_min,date->tm_sec);
		#endif
		return buffer;
	}

	std::string Tail(unsigned int lines)
	{
		const int BUFFER_SIZE = 1024 * lines;
		const std::string logdata = TailByte(BUFFER_SIZE+1);
		const char * p = logdata.c_str();
		const char * firstline = strchr(p,'\n');
		if (firstline == NULL)
		{//とても巨大な一行があることになる。	
			//この関数では対応できない
			return "";
		}

		//最初の半端な一行を飛ばして渡す
		firstline = firstline + 1;
			
		//最後から数えてlines番目のところまでスキップ
		unsigned int linecount = 0;
		for(p = logdata.c_str() + logdata.size() ; p >= firstline ; p-- )
		{//UTF8やSJISでも \n は2バイト目に被らないので気にしない
			if ( *p == '\n' )
			{
				linecount++;
				if (linecount >= lines)
				{
					break;
				}
			}
		}
		//今のPの場所が 返す値の先頭だよ
		return p;
	}

	
	std::string TailByte(unsigned int bytes)
	{
		if (!this->FP)
		{
			return "";
		}

		vector<char> buffer;
		buffer.resize(bytes + 1);

		char * bufferStr = &buffer[0];

		int readsize = 0;
		unsigned int pos = 0;
		{//終端からバッファの数だけ読み込む
			lock_guard<mutex> al(this->lock);
	#if _MSC_VER
			fflush(this->FP);
	#endif

			pos = ftell(this->FP);
			if (pos < bytes)
			{
				fseek(this->FP,0,SEEK_SET);
				readsize = fread((void*) (bufferStr+(bytes-pos)),sizeof(char),pos,this->FP);
				fseek(this->FP,pos,SEEK_SET); //元の位置に戻す

			}
			else
			{
				fseek(this->FP,pos - bytes,SEEK_SET);
				readsize = fread((void*)bufferStr,sizeof(char),bytes,this->FP);
				fseek(this->FP,pos,SEEK_SET); //元の位置に戻す
			}
		}

		//バイトが足りないならバックアップから読む
		if (pos < bytes)
		{
			const unsigned int tarinai = (bytes-pos); //このバイト数足りない
			unsigned int backupread = 0;
			const std::string backfilename = getFilenameBack();
			FILE * bfp = fopen(backfilename.c_str() , "rb");
			if (bfp)
			{
				fseek(bfp,0,SEEK_END); //後ろから
				const unsigned int filesize = ftell(bfp);
				fseek(bfp,filesize - tarinai,SEEK_SET);

				backupread = fread((void*)bufferStr,sizeof(char),tarinai,bfp);
				fclose(bfp);
			}
			//うまくつなげたか？
			if (tarinai > backupread)
			{//つなげていない。今のログのデータだけにします。
				memcpy((void*)bufferStr,(void*)(bufferStr+tarinai),readsize);
			}
			else
			{//つなげている
				readsize += backupread;
			}
		}

		if (readsize < 0)
		{//読み込みエラー
			return "";
		}
		
		//\0などの文字列をホワイトスペースに
		for(int i = 0 ; i < readsize ; i++)
		{
			if ( bufferStr[i] == '\0')
			{
				bufferStr[i] = ' ';
			}
		}

		//終端の追加
		bufferStr[readsize] = 0;
		return bufferStr;
	}

	//ログファイルのローテート
	void Rotate()
	{
		lock_guard<mutex> al(this->lock);
		Close();
		//常に1世代保存
		::unlink( getFilenameBack().c_str());
		::rename(getFilename().c_str() , getFilenameBack().c_str() );
		SetLogFile( getFilename() ,this->FPBuffer.size() , this->rotateSize );
	}
	//すべてのログを消します.(テスト用)
	void CleanupLogFile()
	{
		lock_guard<mutex> al(this->lock);
		Close();
		::unlink( getFilenameBack().c_str());
		::unlink( getFilename().c_str());
		SetLogFile( getFilename() ,this->FPBuffer.size() , this->rotateSize );
	}

	
private:
	SEXYLOG_LEVEL level;	//ログレベル

	FILE* FP;				//ファイルに書き込む場合のポインタ
	vector<char> FPBuffer;  //ファイルに書き込み場合のバッファ値
	unsigned long rotateSize; //このサイズを超えた場合、ローテートする
	std::string filename;	//ログファイル名

	bool canStdout;			//標準出力に出すかどうか ディフォントON
	SEXYLOGOUT_LOGGER_FUNCTION	customLoggerFunc; //カスタムログ関数

	mutable mutex lock;
};
