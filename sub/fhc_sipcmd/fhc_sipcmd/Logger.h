#pragma once

enum LOG_LEVEL
{
	 LOG_LEVEL_DEBUG = 0			//デバッグなどに必要な情報。
	,LOG_LEVEL_NOTIFY = 3			//通知
	,LOG_LEVEL_ERROR = 9			//エラー
};

#define LOGOUT(LOGTYPE,LOGMESSAGE) {\
	if ( (int)(LOGTYPE) >= (int)(getLogLevel()) ) { \
		std::stringstream LOGOUTSTREAM; \
		LOGOUTSTREAM << LOGTYPE << "\t" << __FUNCTION__ << ":" << __LINE__ << "\t" << LOGMESSAGE << "\r\n"; \
		writeLog(LOGOUTSTREAM.str() );\
	} \
}

#define DEBUGLOG(LOGMESSAGE)  LOGOUT(LOG_LEVEL_DEBUG,LOGMESSAGE)
#define NOTIFYLOG(LOGMESSAGE) LOGOUT(LOG_LEVEL_NOTIFY,LOGMESSAGE)
#define ERRORLOG(LOGMESSAGE)  LOGOUT(LOG_LEVEL_ERROR,LOGMESSAGE)

static LOG_LEVEL getLogLevel()
{
	return LOG_LEVEL_DEBUG;
}

static void writeLog(const std::string& str)
{
//	fputs(STDERR,str.c_str() );
}
