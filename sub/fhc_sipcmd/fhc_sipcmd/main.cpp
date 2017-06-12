#include "common.h"
#include "FHC_SipCmd.h"
#include "XLStringUtil.h"

FHC_SipCmd g_SIP;

#if _MSC_VER
#include <ctime>
#else
#include <signal.h>
void sigterm_to_log(int sig) 
{
//	ERRORLOG("signal:" << sig << "を受信しました。終了させます。");
	g_SIP.Stop();
}
#endif


#if _MSC_VER
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst,
//                   LPSTR lpszCmdLine, int nCmdShow)
int main(int argc, const char **argv)
#else
int main(int argc, const char **argv)
#endif
{
	::srand(time(NULL));
	{

		WinSockInit winsockinit;

		{
			SEXYTEST_RUNNER();
		
			bool isdebug = true;
#if _MSC_VER
#else
			//終了signalを受け取ったらログに書いて死ぬようにする
			signal(SIGINT|SIGKILL|SIGTERM, sigterm_to_log);
#endif

			try
			{
				g_SIP.Create();

				char cmd[1024];
				while(1)
				{
					fgets(cmd,sizeof(cmd)-1,stdin);

					std::vector<std::string> vec = XLStringUtil::split_vector("\t",XLStringUtil::trim(cmd) );
					if (vec[0] == "QUIT")
					{
						break;
					}
					else if (vec[0] == "CALL" && vec.size() >= 2)
					{
						g_SIP.Call(vec[1]);
					}
					else if (vec[0] == "AUTH" && vec.size() >= 4)
					{
						g_SIP.Auth(vec[1],vec[2],vec[3]);
					}
					else if (vec[0] == "ANSWER")
					{
						g_SIP.Answer();
					}
					else if (vec[0] == "HANGUP")
					{
						g_SIP.Hangup();
					}
					else
					{
						printf(">ERROR\tUnknown Command:%s\r\n",cmd);
					}
					fflush(stdout);
				}
			}
			catch (std::exception& e) 
			{
				ERRORLOG( "トップレベル std::exception例外をキャッチしました" );
				ERRORLOG( e.what() );
			}
			catch(...)
			{
				ERRORLOG( "不明な例外をキャッチしました" );
			}
			g_SIP.Stop();
		}
	}

	return (int)0;
}
