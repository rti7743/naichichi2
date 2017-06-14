#include "common.h"
#include "SystemMisc.h"
#include "MainWindow.h"
#include "XLFileUtil.h"
#include "XLStringUtil.h"

#if _MSC_VER
	#include <Tlhelp32.h>

	//IPアドレスを取得するのに使う
	#include <iptypes.h>
	#include <iphlpapi.h>
	#pragma comment(lib, "ws2_32.lib")
	#pragma comment(lib, "IPHLPAPI.lib")
#else
	#include <alsa/asoundlib.h>

	#include <sys/ioctl.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <net/if.h>
	#include <linux/sockios.h>
	#include <linux/ethtool.h>
//	#include <sys/types.h>
	#include <signal.h>
#endif




#if _MSC_VER

typedef NTSTATUS (NTAPI *_NtQueryInformationProcess)(
    HANDLE ProcessHandle,
    DWORD ProcessInformationClass,
    PVOID ProcessInformation,
    DWORD ProcessInformationLength,
    PDWORD ReturnLength
    );

typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _PROCESS_BASIC_INFORMATION
{
    LONG ExitStatus;
    PVOID PebBaseAddress;
    ULONG_PTR AffinityMask;
    LONG BasePriority;
    ULONG_PTR UniqueProcessId;
    ULONG_PTR ParentProcessId;
} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;

PVOID GetPebAddress(HANDLE ProcessHandle)
{
    _NtQueryInformationProcess NtQueryInformationProcess =
        (_NtQueryInformationProcess)GetProcAddress(
        GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");
    PROCESS_BASIC_INFORMATION pbi;

    NtQueryInformationProcess(ProcessHandle, 0, &pbi, sizeof(pbi), NULL);

    return pbi.PebBaseAddress;
}

string PIDtoArgs(unsigned int pid)
{
	

	//http://wj32.org/wp/2009/01/24/howto-get-the-command-line-of-processes/
	HANDLE processHandle = NULL;
	WCHAR* commandLineContents = NULL;
	string ret;

	if ((processHandle = OpenProcess(
		PROCESS_QUERY_INFORMATION | /* required for NtQueryInformationProcess */
		PROCESS_VM_READ, /* required for ReadProcessMemory */
		FALSE, pid)) == 0)
	{
		goto err;
	}

	PVOID pebAddress = GetPebAddress(processHandle);
	PVOID rtlUserProcParamsAddress;

	if (!ReadProcessMemory(processHandle, (PCHAR)pebAddress + 0x10,
		&rtlUserProcParamsAddress, sizeof(PVOID), NULL))
	{
		goto err;
	}

	UNICODE_STRING commandLine;
	if (!ReadProcessMemory(processHandle, (PCHAR)rtlUserProcParamsAddress + 0x40,
		&commandLine, sizeof(commandLine), NULL))
	{
		goto err;
	}

	commandLineContents = (WCHAR *)malloc(commandLine.Length+sizeof(WCHAR));
	if (!commandLineContents)
	{
		goto err;
	}

	if (!ReadProcessMemory(processHandle, commandLine.Buffer,
		commandLineContents, commandLine.Length, NULL))
	{
		goto err;
	}

	commandLineContents[commandLine.Length/2] = 0;

	//プロセス名をスキップします.
	WCHAR* commandpoint = wcschr(commandLineContents+1,L'"');
	if (commandpoint == NULL || *(commandpoint+1) != L' ')
	{//うーん、コマンド名がついていないの？わけわかめ
		ret = _W2A(commandLineContents); 
	}
	else
	{
		ret = _W2A(commandpoint+2);
	}
err:
	if (processHandle)	CloseHandle(processHandle);
	if (commandLineContents)	free(commandLineContents);

	return ret;
}

//pidからwindowタイトルを取得します
string PIDtoWindowname(unsigned int pid)
{
	//http://d.hatena.ne.jp/yu-hr/20100323/1269355469 参考
	HWND hwnd = GetTopWindow(NULL);
	do {
		if(GetWindowLong( hwnd, GWL_HWNDPARENT) != 0 || !IsWindowVisible( hwnd) )
		{
			continue;
		}
		DWORD processID;
		GetWindowThreadProcessId( hwnd, &processID);
		if(pid == processID)
		{
			char buffer[MAX_PATH] = {0};
			::GetWindowText(hwnd,buffer,sizeof(buffer) );
			return buffer;
		}
	} while((hwnd = GetNextWindow( hwnd, GW_HWNDNEXT)) != NULL);

	return "";
};

#else
string PIDtoProcessname(unsigned int pid)
{
	char filenameBuffer[1024] = {0} ;
	const string filename = string() + "/proc/" + num2str(pid) + "/cmdline";
	FILE * fp = fopen(filename.c_str() , "rb");
	if (!fp) return "";

	for(int i = 0; ! feof(fp) ; i++  )
	{
		if (i >= 1023) break;
		char c = fgetc(fp);
		if (c == 0 || c == 0xff)
		{
			break;
		}
		filenameBuffer[i] = c;
	}
	fclose(fp);

	return filenameBuffer;
}

string PIDtoArgs(unsigned int pid )
{
	char filenameBuffer[1024] = {0} ;
	const string filename = string() + "/proc/" + num2str(pid) + "/cmdline";
	FILE * fp = fopen(filename.c_str() , "rb");
	if (!fp) return "";

	//skip process name
	for(; ! feof(fp) ;  )
	{
		char c = fgetc(fp);
		if (c == 0 || c == 0xff)
		{
			break;
		}
	}

	//pickup args
	for(int i = 0 ; ! feof(fp) ; i++  )
	{
		if (i >= 1023) break;
		char c = fgetc(fp);
		if (c == 0 || c == 0xff)
		{
			filenameBuffer[i] = ' ';
		}
		else
		{
			filenameBuffer[i] = c;
		}
	}
	fclose(fp);

	return filenameBuffer;
}
int PIDtoParentPID(unsigned int pid)
{
	char filenameBuffer[1024] = {0} ;
	const string filename = string() + "/proc/" + num2str(pid) + "/stat";
	FILE * fp = fopen(filename.c_str() , "rb");
	if (!fp) return 0;

	unsigned int mypid = 0;
	char cmd[1024] = {0};
	char state = 0;
	unsigned int ppid = 0;

	fscanf(fp,"%d %s %c %d ",&mypid,cmd,&state,&ppid);
	fclose(fp);
	
	return ppid;
}

#endif

void SystemMisc::SetMicVolume(int newValue100)
{
#if _MSC_VER
#else
	int r;
	{
		bool outvalFlag = false;
		long min, max;
		snd_mixer_t *handle;

		r = snd_mixer_open(&handle, 0);
		if (r < 0)
		{
			ERRORLOG("snd_mixer_open error " << r );
			return ;
		}
		r = snd_mixer_attach(handle, "default");
		if (r < 0 )
		{
			ERRORLOG("snd_mixer_attach error " << r );
			snd_mixer_close(handle);
			return ;
		}
		r = snd_mixer_selem_register(handle, NULL, NULL);
		if (r < 0 )
		{
			ERRORLOG("snd_mixer_selem_register error " << r );
			snd_mixer_close(handle);
			return ;
		}
		r = snd_mixer_load(handle);
		if (r < 0 )
		{
			ERRORLOG("snd_mixer_load error " << r );
			snd_mixer_close(handle);
			return ;
		}

		for (snd_mixer_elem_t* elem = snd_mixer_first_elem(handle); elem; elem = snd_mixer_elem_next(elem))
		{
			if (outvalFlag == false && snd_mixer_selem_has_capture_volume(elem) )
			{
				snd_mixer_selem_get_capture_volume_range(elem, &min, &max);
				snd_mixer_selem_set_capture_volume_all(elem, newValue100 * max / 100);

				outvalFlag = true;
//				DEBUGLOG("マイクの音量を (" << newValue100 << ") に設定しました " );
			}
		}
		snd_mixer_close(handle);

		if (outvalFlag == true )
		{
			return ;
		}
	}
#endif
	ERRORLOG("マイクの音量を設定できませんでした " );
}

void SystemMisc::SetSpeakerVolume(int newValue100)
{
#if _MSC_VER
#else
	int r;
	{
		bool invalFlag = false;
		bool outvalFlag = false;
		long min, max;
		snd_mixer_t *handle;

		r = snd_mixer_open(&handle, 0);
		if (r < 0)
		{
			ERRORLOG("snd_mixer_open error " << r );
			return ;
		}
		r = snd_mixer_attach(handle, "default");
		if (r < 0 )
		{
			ERRORLOG("snd_mixer_attach error " << r );
			snd_mixer_close(handle);
			return ;
		}
		r = snd_mixer_selem_register(handle, NULL, NULL);
		if (r < 0 )
		{
			ERRORLOG("snd_mixer_selem_register error " << r );
			snd_mixer_close(handle);
			return ;
		}
		r = snd_mixer_load(handle);
		if (r < 0 )
		{
			ERRORLOG("snd_mixer_load error " << r );
			snd_mixer_close(handle);
			return ;
		}

		for (snd_mixer_elem_t* elem = snd_mixer_first_elem(handle); elem; elem = snd_mixer_elem_next(elem))
		{
			if (invalFlag == false && snd_mixer_selem_has_playback_volume(elem) )
			{
				snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
				snd_mixer_selem_set_playback_volume_all(elem, newValue100 * max / 100);

				invalFlag = true;
//				NOTIFYLOG("スピーカーの音量を (" << newValue100 << ") に設定しました " );
			}
		}
		snd_mixer_close(handle);

		if (invalFlag == true)
		{
			return ;
		}
	}
#endif
	ERRORLOG("スピーカーの音量を設定できませんでした " );
}


//マイクの状態のチェック
//1			正常
//0			準備できてません
//-1		何も入力が受け取れない
int SystemMisc::CheckMic()
{
#if _MSC_VER
#else
	int r;
	{
		snd_mixer_t *handle;

		r = snd_mixer_open(&handle, 0);
		if (r < 0)
		{
			ERRORLOG("snd_mixer_open error " << r << " " << snd_strerror (r) );
			return 0;
		}
		r = snd_mixer_attach(handle, "default");
		if (r < 0 )
		{
			ERRORLOG("snd_mixer_attach error " << r << " " << snd_strerror (r) );
			snd_mixer_close(handle);
			return 0;
		}
		r = snd_mixer_selem_register(handle, NULL, NULL);
		if (r < 0 )
		{
			ERRORLOG("snd_mixer_selem_register error " << r << " " << snd_strerror (r) );
			snd_mixer_close(handle);
			return 0;
		}
		r = snd_mixer_load(handle);
		if (r < 0 )
		{
			ERRORLOG("snd_mixer_load error " << r << " " << snd_strerror (r) );
			snd_mixer_close(handle);
			return 0;
		}
		
		snd_mixer_close(handle);

		
		snd_pcm_t *playback_handle;
		r = snd_pcm_open (&playback_handle, "default", SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
		if (r < 0) 
		{
			ERRORLOG("cannot open audio device " << r  << " " << snd_strerror (r) );
			return 0;
		}
		snd_pcm_close (playback_handle);
	}
#endif
	return 1;
}

void SystemMisc::SetNTPDate()
{
	if( ! SystemMisc::is_LinkDetected("eth0") )
	{
		NOTIFYLOG("ケーブルが刺さっていないのでNTPが利用できません!");
		return ;
	}

#if _MSC_VER
#else
	//NTPによる時間合わせ
	SystemMisc::PipeExec("/usr/bin/ntpdate -b -s -u pool.ntp.org");
#endif
}


void SystemMisc::SetIPAddress(const string& ipaddress,const string& netmask,const string& gateway,const string& dns,const string& w_ipaddress,const string& w_netmask,const string& w_gateway,const string& w_dns,const string& ssid,const string& password,const string& key_mgmt)
{
#if _MSC_VER
#else
	string newinterfaces;
	//cabel
	if (ipaddress == "")
	{
		NOTIFYLOG("ipaddress is none" );
	}
	else if (ipaddress == "DHCP")
	{
		NOTIFYLOG("ipaddress is DHCP" );
		newinterfaces += "iface eth0 inet dhcp\n";
	}
	else
	{
		NOTIFYLOG("ipaddress is STATIC " <<  ipaddress << " " << netmask << " " << gateway);
		newinterfaces += "iface eth0 inet static\n";
		newinterfaces += "address " + ipaddress + "\n";
		newinterfaces += "netmask " + netmask + "\n";
		newinterfaces += "gateway " + gateway + "\n";
		newinterfaces += "dns-nameservers " + dns + "\n";
	}
	//wifi
	if (w_ipaddress == "NONE")
	{
	}
	else if (w_ipaddress == "DHCP")
	{
		newinterfaces += "iface wlan0 inet dhcp\n";
		newinterfaces += "wireless-essid "+ssid+"\n"; 
		newinterfaces += "wpa-conf /etc/wpa_supplicant/wpa_supplicant.conf\n";
	}
	else
	{
		newinterfaces += "iface wlan0 inet static\n";
		newinterfaces += "address " + w_ipaddress + "\n";
		newinterfaces += "netmask " + w_netmask + "\n";
		newinterfaces += "gateway " + w_gateway + "\n";
		newinterfaces += "dns-nameservers " + w_dns + "\n";
		newinterfaces += "wireless-essid "+ssid+"\n"; 
		newinterfaces += "wpa-conf /etc/wpa_supplicant/wpa_supplicant.conf\n";
	}

	if ( ! XLFileUtil::write("/etc/network/interfaces",newinterfaces) )
	{
		int error = errno;
		throw XLEXCEPTION("/etc/network/interfacesファイルを開くことができません " << XLException::StringErrNo(error));
	}

	string newresolve;
	//cabel
	if (ipaddress == "NONE")
	{
		NOTIFYLOG("DNS ipaddress is NONE" );
	}
	else if (ipaddress == "DHCP")
	{
		NOTIFYLOG("DNS ipaddress is DHCP" );
	}
	else
	{
		NOTIFYLOG("DNS ipaddress is DNS " << dns );
		newresolve += "nameserver " + dns + "\n";
	}
	//wifi
	if (w_ipaddress == "NONE")
	{
	}
	else if (w_ipaddress == "DHCP")
	{
	}
	else
	{
		newresolve += "nameserver " + w_dns + "\n";
	}
	if (! XLFileUtil::write("/etc/resolv.conf",newresolve) )
	{
		int error = errno;
		throw XLEXCEPTION("/etc/resolv.confファイルを開くことができません " << XLException::StringErrNo(error));
	}

	//wifi wpa
	if (w_ipaddress != "NONE")
	{
		//http://www.youchikurin.com/blog/2007/06/linuxlan_1.html
		string pskencode = PipeExec("wpa_passphrase \"" + ssid + "\" \"" + password + "\"|grep psk|tail -1|cut -d= -f 2" );
		string supplicant = "network={\n";
		supplicant += "ssid="+ssid+"\n"; 
		supplicant += "psk="+pskencode+"\n"; 
		if(key_mgmt=="WPA")
		{
		}
		else if(key_mgmt=="WPA-PSK/TKIP")
		{
			supplicant += "proto=WPA\n";
			supplicant += "key_mgmt=WPA-PSK\n";
			supplicant += "pairwise=TKIP\n";
			supplicant += "group=TKIP\n";
		}
		else if(key_mgmt=="WPA2-PSK/TKIP")
		{
			supplicant += "proto=RSN\n";
			supplicant += "key_mgmt=WPA2-PSK\n";
			supplicant += "pairwise=TKIP\n";
			supplicant += "group=TKIP\n";
		}
		else if(key_mgmt=="WPA-PSK/AES")
		{
			supplicant += "proto=WPA\n";
			supplicant += "key_mgmt=WPA-PSK\n";
			supplicant += "pairwise=CCMP\n";
			supplicant += "group=CCMP\n";
		}
		else if(key_mgmt=="WPA2-PSK/AES")
		{
			supplicant += "proto=RSN\n";
			supplicant += "key_mgmt=WPA2-PSK\n";
			supplicant += "pairwise=CCMP\n";
			supplicant += "group=CCMP\n";
		}

		supplicant += "}\n";
		if ( ! XLFileUtil::write("/etc/wpa_supplicant/wpa_supplicant.conf",supplicant) )
		{
			int error = errno;
			throw XLEXCEPTION("/etc/wpa_supplicant/wpa_supplicant.conf ファイルを開くことができません " << XLException::StringErrNo(error));
		}
	}
	if (IsBeagleboneBlack() )
	{
//まれに systemd がフリーズするらしい...
//		if (ipaddress == "DHCP")
//		{
//			NOTIFYLOG("systemctl enable connman.service" );
//			SystemMisc::PipeExec("systemctl enable connman.service");
//		}
//		else
//		{
//			NOTIFYLOG("systemctl disable connman.service" );
//			SystemMisc::PipeExec("systemctl disable connman.service");
//		}
//systemd とうまく付き合うコツは、できるだけ systemdをつかわないことではないか.
		string cmd = "/bin/rm /etc/systemd/system/multi-user.target.wants/connman.service";
		NOTIFYLOG(cmd);
		SystemMisc::PipeExec(cmd);
		if (ipaddress == "DHCP")
		{
			cmd = "/bin/cp /lib/systemd/system/connman.service /etc/systemd/system/multi-user.target.wants/connman.service";
			NOTIFYLOG(cmd);
			SystemMisc::PipeExec(cmd);
		}
	}
#endif
}


void SystemMisc::NetworkRestart(const string& ipaddress,const string& w_ipaddress)
{
#if _MSC_VER
#else
	SystemMisc::PipeExec("ifdown eth0 ; ifup eth0");

	//wifi
	if (w_ipaddress == "NONE")
	{
		SystemMisc::PipeExec("ifdown wlan0");
	}
	else
	{
		SystemMisc::PipeExec("ifdown wlan0 ; ifup wlan0");
	}

	if (ipaddress == "DHCP")
	{//DHCPを取得するわずかな時間待機する
		SecSleep(5);
	}

#endif
}

//起動時にネットワークの設定がされていなければ設定してあげます。
void SystemMisc::NetworkInit(const string& ipaddress,const string& netmask,const string& gateway,const string& dns)
{
#if _MSC_VER
	NOTIFYLOG("windowsなのでネットワークの設定はしません。OSの値を信用します " );
	return;
#else
	if (IsBeagleboneOld())
	{
		NOTIFYLOG("bbなのでネットワークの設定はしません。OSの値を信用します " );
		return;
	}
	if (IsBeagleboneBlack() )
	{//beaglebone black
		//bbbではネットワーク初期化がすっとばされるので補正する

		if (ipaddress == "DHCP" || ipaddress == "")
		{//ディフォルトはdhcp
			NOTIFYLOG("bbbですがdhcpの設定なのでネットワークの設定はしません。 " );
			return;
		}
		auto interfaceMap = GetIPAddressMap();
		auto it = interfaceMap.find("eth0");
		
		if (it == interfaceMap.end() )
		{
			NOTIFYLOG("eth0がありません。ケーブルが刺さっていません。" );
		}
		else
		{
			if (it->second == ipaddress)
			{//現在のIPと設定上のIPと同じな場合は何もしない。
				NOTIFYLOG("bbbで固定ipですがip("<<ipaddress<<")が正しく設定されているのでネットワークの設定はしません。 " );
				return;
			}
		}
		NOTIFYLOG("bbbで固定ipなのでネットワークの設定をします。ip("<<ipaddress<<")にします。 " );
		SetIPAddress(ipaddress,netmask,gateway,dns,"NONE","","","","","","");
		NetworkRestart(ipaddress,"NONE");
		
		//設定されるまで待ちます。
		for(int i = 0 ; i < 10 ; i++ )
		{
			interfaceMap = GetIPAddressMap();
			it = interfaceMap.find("eth0");
			if (it == interfaceMap.end() )
			{
				NOTIFYLOG("eth0が見えません。1秒待ちます " );
			}
			else
			{
				if (it->second == ipaddress)
				{
					NOTIFYLOG("ip" << ipaddress<<"に設定できました。dropbearを起動させます。" );

					SystemMisc::PipeExec("/usr/sbin/dropbear -p 22 -w");
					return ;
				}
				NOTIFYLOG("ip("<<ipaddress<<")にしたいのですが、今は" << it->second << "です。1秒待ちます " );
			}
			SecSleep(1);
		}
		NOTIFYLOG("ip" << ipaddress<<"に設定出来ませんでした。" );
		return;
	}
#endif
}

//eth0が取得できない。DHCPはいきてるの？喝入れをする
void SystemMisc::NetworkDHCP_DieCheck(const string& ipaddress)
{
	if (IsBeagleboneBlack() )
	{//beaglebone black
		if (ipaddress == "DHCP" || ipaddress == "")
		{
			const string isEnableConnman = XLStringUtil::chop( SystemMisc::PipeExec("systemctl is-enabled connman.service") );
			NOTIFYLOG("connman.service is " << isEnableConnman);
			if (isEnableConnman != "enabled")
			{//connmanが死んでいる？
				//よみがえらせる
				ERRORLOG("systemctl enable connman.service" );
				SystemMisc::PipeExec("systemctl enable connman.service");
				SecSleep(1);
				ERRORLOG("systemctl restart connman.service" );
				SystemMisc::PipeExec("systemctl restart connman.service");
				SecSleep(1);
			}
			NetworkRestart(ipaddress,"NONE");
		}
	}
}

//wifiデバイスだっけ？
bool SystemMisc::IsWifiDevice()
{
	return false;
}

bool SystemMisc::IsBeagleboneOld()
{
#if _MSC_VER
	return false;
#else
	return XLFileUtil::Exist("/sys/class/leds/beaglebone::usr0/brightness");
#endif
}

bool SystemMisc::IsBeagleboneBlack()
{
#if _MSC_VER
	return false;
#else
	return XLFileUtil::Exist("/sys/class/leds/beaglebone:green:usr0/brightness");
#endif
}

//新リモコンボード irr
bool SystemMisc::IsIRR()
{
#if _MSC_VER
	return false;
#else
	std::string irrbin = MainWindow::m()->GetConfigBasePath("linuxboot/irr/irr");
	return XLFileUtil::Exist(irrbin);
#endif
}

//旧リモコンボード Endel
bool SystemMisc::IsEndel()
{
#if _MSC_VER
	return false;
#else
	std::string irrbin = MainWindow::m()->GetConfigBasePath("linuxboot/endel/endelInit.sh");
	return XLFileUtil::Exist(irrbin);
#endif
}

//sshuser の SSHのパスワードリセット
void SystemMisc::SetSSHUserPassword(const string& password)
{
#if _MSC_VER
#else
	SystemMisc::PipeExec("echo " + XLStringUtil::escapeshellarg_single("sshuser:" + password ) + " | chpasswd");
#endif
}



string SystemMisc::PipeExec(const string& exec)
{
	DEBUGLOG(exec.c_str());
#if _MSC_VER
	FILE * fp = _popen(exec.c_str() , "r");
#else
	FILE * fp = popen(exec.c_str() , "r");
#endif
	if (!fp)
	{
		return "";
	}

	string ret;

	vector<char> buffer(65536);
	while (fgets(&buffer[0], 65535, fp) != NULL)
	{
		buffer[65535] = 0;
		ret += &buffer[0];
	}

#if _MSC_VER
	_pclose(fp);
#else
	pclose(fp);
#endif
	
	return ret;
}

string SystemMisc::MakePassword(const string& usermail,const string& password)
{
	return XLStringUtil::sha1( password + "\t" + usermail);
}

#if _MSC_VER
map<string,string> SystemMisc::ArgsToMap(const char* lpszCmdLine)
{
	wstring cmd = _A2W(lpszCmdLine);
	int argc;
	const LPWSTR* argvw = CommandLineToArgvW(cmd.c_str(), &argc);

	map<string,string> ret;
	for(int i = 0 ; i < argc ; i ++ )
	{
		const char* p = _W2A(argvw[i]);
		const char * eq = strchr(p,'=');
		if (eq == NULL)
		{
			ret[p] = "";
		}
		else
		{
			ret[string(p , 0 , eq - p - 1)] = eq + 1;
		}
	}

	GlobalFree((HGLOBAL)argvw);
	return ret;
}
#else
map<string,string> SystemMisc::ArgsToMap(int argc, const char **argv)
{
	map<string,string> ret;
	for(int i = 0 ; i < argc ; i ++ )
	{
		const char * eq = strchr(argv[i],'=');
		if (eq == NULL)
		{
			ret[argv[i]] = "";
		}
		else
		{
			ret[string(argv[i] , 0 , eq - argv[i] - 1)] = eq + 1;
		}
	}
	return ret;
}
#endif

string SystemMisc::SupportDump()
{
#if _MSC_VER
	return ""; //not implement!
#else
	SystemMisc::PipeExec(": > /tmp/support_log");
	SystemMisc::PipeExec("echo \"=====df -h ==\" >> /tmp/support_log");
	SystemMisc::PipeExec("df -h >> /tmp/support_log 2>&1");
	SystemMisc::PipeExec("echo \"=====mount ==\" >> /tmp/support_log");
	SystemMisc::PipeExec("mount >> /tmp/support_log 2>&1");
	SystemMisc::PipeExec("echo \"=====ps aux ==\" >> /tmp/support_log");
	SystemMisc::PipeExec("ps aux >> /tmp/support_log 2>&1");
	SystemMisc::PipeExec("echo \"=====top -b -n1 ==\" >> /tmp/support_log");
	SystemMisc::PipeExec("top -b -n1 >> /tmp/support_log 2>&1");
	SystemMisc::PipeExec("echo \"=====ifconfig ==\" >> /tmp/support_log");
	SystemMisc::PipeExec("ifconfig >> /tmp/support_log 2>&1");
	SystemMisc::PipeExec("echo \"=====route -n ==\" >> /tmp/support_log");
	SystemMisc::PipeExec("route -n >> /tmp/support_log 2>&1");
	SystemMisc::PipeExec("echo \"=====netstat -an ==\" >> /tmp/support_log");
	SystemMisc::PipeExec("netstat -an >> /tmp/support_log 2>&1");
	SystemMisc::PipeExec("echo \"=====dmesg ==\" >> /tmp/support_log");
	SystemMisc::PipeExec("dmesg >> /tmp/support_log 2>&1");
	SystemMisc::PipeExec("echo \"=====crontab -l ==\" >> /tmp/support_log");
	SystemMisc::PipeExec("crontab -l >> /tmp/support_log 2>&1");
	SystemMisc::PipeExec("echo \"=====systemctl status naichichi2.service ==\" >> /tmp/support_log");
	SystemMisc::PipeExec("systemctl status naichichi2.service >> /tmp/support_log 2>&1");
	SystemMisc::PipeExec("echo \"=====ping over ip ==\" >> /tmp/support_log");
	SystemMisc::PipeExec("ping -c 1 -W 1 8.8.8.8 >> /tmp/support_log 2>&1");
	SystemMisc::PipeExec("echo \"=====ping over name ==\" >> /tmp/support_log");
	SystemMisc::PipeExec("ping -c 1 -W 1 rti-giken.jp >> /tmp/support_log 2>&1");

	const string logFilename = sexylog::m()->getFilename();
	const string logFilenameback = sexylog::m()->getFilenameBack();
	const string configfilename = MainWindow::m()->GetConfigBasePath("config.conf");
	const string irdir = MainWindow::m()->GetConfigBasePath("ir");
	const string juliusWild = MainWindow::m()->GetConfigBasePath("julius/__temp*");
	const string sensorWild = MainWindow::m()->GetConfigBasePath("sensor");
	const string webrootUserDir = MainWindow::m()->GetConfigBasePath("webroot/user/");

	SystemMisc::PipeExec("/bin/rm /tmp/support.tar.gz");
	SystemMisc::PipeExec(string("tar zcvf /tmp/support.tar.gz ")
		+ " " + logFilename 
		+ " " + logFilenameback 
		+ " " + configfilename 
		+ " " + irdir
		+ " " + juliusWild
		+ " " + sensorWild
		+ " " + webrootUserDir
		+ " /tmp/support_log /tmp/naichichi2_updater_url /etc/issue.model /etc/hosts /etc/wpa_supplicant/wpa_supplicant.conf /etc/resolv.conf /etc/network/interfaces /tmp/naichichi2/*.wav " );
	SystemMisc::PipeExec(": > /tmp/support_log");

	return "/tmp/support.tar.gz";
#endif
}

string SystemMisc::GetModel()
{
#ifdef _MSC_VER
	return "pc-windows";
#else
	return XLStringUtil::chop( XLFileUtil::cat("/etc/issue.model") );
#endif
}

string SystemMisc::GetOSVersion()
{
#ifdef _MSC_VER
	OSVERSIONINFO osver; 
	osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO); 
	GetVersionEx(&osver); 
	return "Windows " + num2str(osver.dwMajorVersion) + "." + num2str(osver.dwMinorVersion);
#else
	return "Linux " + XLStringUtil::chop( XLFileUtil::cat("/proc/sys/kernel/osrelease") );
#endif
}


string SystemMisc::MakeWebAPIKey()
{
	return "webapi_"+XLStringUtil::randsting(64);
}

//メールテンプレートからタイトルの取得
string SystemMisc::SendmailGetSubject(const string& tplString,const map<string,string>& arr)
{
	string tplString_utf8 = _A2U(tplString);
	for(auto it = arr.begin() ; it != arr.end() ; ++it)
	{
		string k = _A2U(it->first);
		string v = _A2U(it->second);
		tplString_utf8 = XLStringUtil::replace_low(tplString_utf8,k,v);
	}
	const char * start = tplString_utf8.c_str();
	const char * firstNL = strstr(start ,"\n");
	if (firstNL == NULL)
	{
		throw XLEXCEPTION( "subject改行がありません tpl " << tplString);
	}
	return XLStringUtil::chop( string(start,0 , (int) (firstNL - start)) );
}

//メールテンプレートから本文の取得
string SystemMisc::SendmailGetBody(const string& tplString,const map<string,string>& arr)
{
	

	string tplString_utf8 = _A2U(tplString);
	for(auto it = arr.begin() ; it != arr.end() ; ++it)
	{
		string k = _A2U(it->first);
		string v = _A2U(it->second);
		tplString_utf8 = XLStringUtil::replace_low(tplString_utf8,k,v);
	}
	const char * start = tplString_utf8.c_str();
	const char * firstNL = strstr(start ,"\n");
	if (firstNL == NULL)
	{
		throw XLEXCEPTION( "subject改行がありません tpl " << tplString);
	}
	return firstNL + 1; //+1 は \n の分。
}

//簡易メール送信(unixだけ)
void SystemMisc::Sendmail(const string& from,const string& to,const string& supportbcc,const string& subject,const string& body)
{
#if _MSC_VER
	
#else
	if ( ! XLStringUtil::checkMailAddress(from) )
	{
		ERRORLOG("from(" << from << ")はメールアドレスではありません")
		return ;
	}
	if ( ! XLStringUtil::checkMailAddress(to) )
	{
		ERRORLOG("from(" << to << ")はメールアドレスではありません")
		return ;
	}
	if ( ! XLStringUtil::checkMailAddress(supportbcc) )
	{
		ERRORLOG("from(" << supportbcc << ")はメールアドレスではありません")
		return ;
	}
	
	string mail = "MIME-Version: 1.0\n";
	mail += "From: " + XLStringUtil::mailaddress_to_name(from) + " <" + from + ">\n";
	mail += "To: " + XLStringUtil::mailaddress_to_name(to) + " <" + to + ">\n";
	mail += "Bcc: <" + supportbcc + ">\n";
	mail += "Content-Type: text/plain;charset=UTF-8\n";
	mail += "Subject: " + XLStringUtil::mb_mime_header(subject) + "\n";
	mail += "\n";
	mail += body + "\n";
	mail += ".\n";
	NOTIFYLOG("sendmail -->\r\n" << mail << "\r\n");

	//SIGPIPE が怖いのでファイルにストアして実行します。
	const string tempfilename = XLFileUtil::getTempDirectory("naichichi2") + "/" + "mailtemp_" + num2str(time(NULL)) + "_" + num2str(xlrandom()) ;
	XLFileUtil::write(tempfilename , mail);

	const string sendmailCommandLine = "cat " + XLStringUtil::escapeshellarg_single(tempfilename) + " | /usr/sbin/sendmail -t -f" + from;
	system(sendmailCommandLine.c_str());

	NOTIFYLOG(sendmailCommandLine);


//	XLFileUtil::del(tempfilename);
#endif
	return ;
}


//多重起動していない、単体でのブートであるかどうか。
bool SystemMisc::IsDoubleBoot()
{
#if _MSC_VER
	DWORD pid = GetCurrentProcessId();
	string currentProcname;
	vector<string> processList;

	HANDLE snapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshotHandle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
	BOOL r = Process32First(snapshotHandle, &pe);
	do
	{
		if (pe.th32ProcessID == pid)
		{
			currentProcname = pe.szExeFile;
		}
		else
		{
			processList.push_back(pe.szExeFile);
		}
		r = Process32Next(snapshotHandle, &pe);
	}while(r);
	CloseHandle(snapshotHandle);
	
	if (currentProcname.empty())
	{
		return false;
	}
	
	for(auto it = processList.begin() ; it != processList.end() ; it++ )
	{
		if ( *it == currentProcname )
		{//二重起動
			return true;
		}
	}
	//単体起動
	return false;

#else
	int pid = getpid();
	const string currentProcName = PIDtoProcessname(pid);
	bool found = false;

	XLFileUtil::findfile("/proc/" , [&](const string& filename,const string& fullfilename) -> bool{
		if ( ! XLStringUtil::isnumlic( filename ) )
		{//数字じゃないならプロセスではない.
			return true;
		}
		
		int targetpid = atoi(filename);
		if ( targetpid == pid )
		{//自プロセス.
			return true;
		}

		const string targetProcName = PIDtoProcessname(targetpid);
		if (currentProcName == targetProcName)
		{
			found = true;
			return false;
		}
		
		return true;
	});
	
	return found;
#endif
}
//引数も含めたプロセス名を取得
vector<SystemMisc::ProcessSt> SystemMisc::ProcessList()
{
	vector<ProcessSt> retVec;

#if _MSC_VER
	DWORD pid = GetCurrentProcessId();

	HANDLE snapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshotHandle == INVALID_HANDLE_VALUE)
	{
		return retVec;
	}

	PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
	BOOL r = Process32First(snapshotHandle, &pe);
	do
	{
		ProcessSt rr;
		rr.pid = pe.th32ProcessID;
		rr.parentpid = pe.th32ParentProcessID;
		rr.processname = pe.szExeFile;
		rr.args = PIDtoArgs(pe.th32ProcessID);
		rr.extraname = PIDtoWindowname(pe.th32ProcessID);

		if (rr.processname.empty() || rr.parentpid == 0)
		{
		}
		else
		{
			retVec.push_back(rr);
		}
		memset(pe.szExeFile,0,MAX_PATH);
		r = Process32Next(snapshotHandle, &pe);
	}while(r);
	CloseHandle(snapshotHandle);
	
	return retVec;

#else
	unsigned int foundPid = 0;
	bool found = false;

	XLFileUtil::findfile("/proc/" , [&](const string& filename,const string& fullfilename) -> bool{
		if ( ! XLStringUtil::isnumlic( filename ) )
		{//数字じゃないならプロセスではない.
			return true;
		}
		
		unsigned int targetpid = atoi(filename);

		ProcessSt rr;
		rr.pid = targetpid;
		rr.parentpid = PIDtoParentPID(targetpid);
		rr.processname = PIDtoProcessname(targetpid);
		rr.args = PIDtoArgs(targetpid);
		rr.extraname = "";

		if (rr.processname.empty() )
		{
			return true;
		}
		if (rr.pid != 1 && rr.parentpid == 0)
		{//init 以外で親プロセスがイないものは不可
			return true;
		}

		retVec.push_back(rr);
		
		return true;
	});
	
	return retVec;
#endif
}


//指定したpidのプロセスをkillする
bool SystemMisc::KillProces(unsigned int pid)
{
	if (pid <= 0) return false;
#if _MSC_VER
	HANDLE han = OpenProcess(PROCESS_TERMINATE, false, pid);
	if (!han)
	{
		return false;
	}
	TerminateProcess(han , 0 );
	CloseHandle(han);
	return true;
#else
	if (pid <= 1) return false;	//init killするとかやめてー
	if ( kill(pid,9) != 0 )
	{
		return false;
	}
	return true;
#endif
}


void SystemMisc::Import(string filename)
{
	const string configfilename = MainWindow::m()->GetConfigBasePath("config.conf");
	const string irdir = MainWindow::m()->GetConfigBasePath("ir");
	const string webrootUserDir = MainWindow::m()->GetConfigBasePath("webroot/user/");

	const string extractdir = XLFileUtil::getTempDirectory("naichichi2") + "/import";
	SystemMisc::PipeExec(string("rm -rf ") 
		+ XLStringUtil::escapeshellarg_single(extractdir)
	);
	SystemMisc::PipeExec(string("mkdir ") 
		+ XLStringUtil::escapeshellarg_single(extractdir)
	);
	SystemMisc::PipeExec(string("tar zxvf ") 
		+ XLStringUtil::escapeshellarg_single(filename)
		+ " -C " 
		+ XLStringUtil::escapeshellarg_single(extractdir)
	);

	const string basedir = extractdir + "/home/rti/naichichi2/naichichi2";
	
	//configを読み込んでチェックします
	Config config;
	if (! config.loadConfig(basedir + "/config/config.conf") )
	{
		throw XLEXCEPTION( "exportしようとするconfigファイルを読み取ることができませんでした");
	}
	if ( config.Get("version_number","").empty() )
	{
		throw XLEXCEPTION("バージョン番号がありません");
	}

	//このキーはコピーしてはいけない.
	list<string> doNotCopyKeys;
	doNotCopyKeys.push_back("version_number");
	doNotCopyKeys.push_back("account_usermail");
	doNotCopyKeys.push_back("account_password");
	doNotCopyKeys.push_back("httpd__port");
	doNotCopyKeys.push_back("httpd__url");
	doNotCopyKeys.push_back("httpd__uuid");
	doNotCopyKeys.push_back("webapi_apikey");
	doNotCopyKeys.push_back("network_ipaddress_type");
	doNotCopyKeys.push_back("network_ipaddress_ip");
	doNotCopyKeys.push_back("network_ipaddress_mask");
	doNotCopyKeys.push_back("network_ipaddress_gateway");
	doNotCopyKeys.push_back("network_ipaddress_dns");
	doNotCopyKeys.push_back("network_w_ipaddress_type");
	doNotCopyKeys.push_back("network_w_ipaddress_ip");
	doNotCopyKeys.push_back("network_w_ipaddress_mask");
	doNotCopyKeys.push_back("network_w_ipaddress_gateway");
	doNotCopyKeys.push_back("network_w_ipaddress_dns");
	doNotCopyKeys.push_back("network_w_ipaddress_ssid");
	doNotCopyKeys.push_back("network_w_ipaddress_password");
	doNotCopyKeys.push_back("network_w_ipaddress_wkeymgmt");

	//configキーのコピーをします.
	map<string,string> m = config.ToMap();
	for(auto it = m.begin() ; it != m.end() ; ++it)
	{
		if (doNotCopyKeys.end() != find(doNotCopyKeys.begin(),doNotCopyKeys.end(),it->first)  )
		{
			NOTIFYLOG(string("キー:") << it->first << "は、システムキーなのでスキップします." );
			continue;
		}
		MainWindow::m()->Config.Set(it->first,it->second);
	}

	//バックアップになく現在のconfigにしかないキーを消します.
	map<string,string> m2 = MainWindow::m()->Config.ToMap();
	for(auto it = m2.begin() ; it != m2.end() ; ++it)
	{
		if (doNotCopyKeys.end() != find(doNotCopyKeys.begin(),doNotCopyKeys.end(),it->first)  )
		{
			continue;
		}

		if (m.find(it->first) != m.end())
		{//バックアップにあるキー
			continue;
		}

		MainWindow::m()->Config.Remove(it->first);
	}

	//赤外線とユーザディレクトリのコピー
	SystemMisc::PipeExec(string("/bin/rm -r ") 
		+ MainWindow::m()->GetConfigBasePath("ir") 
	);
	SystemMisc::PipeExec(string("/bin/cp -r ") 
		+ basedir + "/config/ir" 
		+ " "
		+ MainWindow::m()->GetConfigBasePath("ir") 
	);

	SystemMisc::PipeExec(string("/bin/rm -r ") 
		+ MainWindow::m()->GetConfigBasePath("webroot/user/") 
	);
	SystemMisc::PipeExec(string("/bin/cp -r ") 
		+ basedir + "/config/webroot/user/" 
		+ " "
		+ MainWindow::m()->GetConfigBasePath("webroot/user/") 
	);

	return ;
}

string SystemMisc::Export()
{
	const string configfilename = MainWindow::m()->GetConfigBasePath("config.conf");
	const string irdir = MainWindow::m()->GetConfigBasePath("ir");
	const string webrootUserDir = MainWindow::m()->GetConfigBasePath("webroot/user/");

	const string savepath = XLFileUtil::getTempDirectory("naichichi2") + "/export" + ".tar.gz";

	SystemMisc::PipeExec("/bin/rm "
		+ XLStringUtil::escapeshellarg_single(savepath)
	);
	SystemMisc::PipeExec(string("tar zcvf ")
		+ XLStringUtil::escapeshellarg_single(savepath)
		+ " " + configfilename 
		+ " " + irdir
		+ " " + webrootUserDir
		+ " " );

	return savepath;
}

#include "ScriptRemoconWeb.h"

bool SystemMisc::WaitForIPAddress(int waitsec)
{
	if( ! SystemMisc::is_LinkDetected("eth0") )
	{
		NOTIFYLOG("ケーブルが刺さっていません!!");
		return false;
	}

	int i;
	//linuxの場合、 DHCPでIPがすぐに取れないことかあるので、何回かリトライする.
	for(i = 0 ; i < waitsec ; i ++)
	{
		//設定画面のIPアドレスを取得します。
		if ( ScriptRemoconWeb::SettingPageIPAddressOverriade() )
		{
			return true;
		}

		NOTIFYLOG("1秒待機してもう一度ipアドレスを確認します.");
		//DHCPの初期化が終わっていなくて 127.0.0.1 しかとれんかったらもう一度
		SecSleep(1);
	}

	if (i >= waitsec)
	{//DHCPがとれない。 せめて喝入れだけでもしてあげる
		ERRORLOG("network down...");
		SystemMisc::NetworkDHCP_DieCheck(
			(MainWindow::m()->Config.Get("network_ipaddress_type","DHCP") == "DHCP" ? "DHCP" : MainWindow::m()->Config.Get("network_ipaddress_ip","DHCP") )
		);
		//設定画面のIPアドレスを取得します。
		if ( ScriptRemoconWeb::SettingPageIPAddressOverriade() )
		{
			return true;
		}
		return false;
	}
	return true;
}

bool SystemMisc::WaitForMicSetup(int waitsec)
{
	int i;
	for(i = 0 ; i < waitsec ; i ++)
	{
		if (SystemMisc::CheckMic() == 1) break;
		
		NOTIFYLOG("1秒待機してもう一度マイクの状態を確認します.");
		SecSleep(1);
	}
	if (i == waitsec)
	{
		NOTIFYLOG("マイクの状態が改善しませんでした.");
		return false;
	}
	return true;
}

bool SystemMisc::SettingFHCBoxEnvSetup()
{
#if _MSC_VER
#else
	//linux kernel panicしたら reboot するようにする.
	XLFileUtil::write("/proc/sys/kernel/panic","5");
	//syncで暴発しないように
	XLFileUtil::write("/proc/sys/kernel/hung_task_timeout_secs","300");
	//usb電源コントロールは常にon. 
	XLFileUtil::write("/sys/bus/usb/devices/usb1/power/control","on");
//	XLFileUtil::write("/sys/bus/usb/devices/usb1/power/autosuspend_delay_ms","-1");

	//beaglebone blackの場合は、 echo on > /sys/devices/ocp*/47400000.usb/power/control もする.
	if ( SystemMisc::IsBeagleboneBlack() ) 
	{
		const string beaglebone_ocp_dir = XLFileUtil::FindOneDirecotry("/sys/devices","ocp");
		if (beaglebone_ocp_dir.empty())
		{
			ERRORLOG("/sys/devices/ocp*/ ディレクトリが見つかりません");
		}
		else
		{
			XLFileUtil::write(beaglebone_ocp_dir + "/47400000.usb/power/control","on");
			XLFileUtil::write(beaglebone_ocp_dir + "/47400000.usb/musb-hdrc.1.auto/power/control","on");
			XLFileUtil::write(beaglebone_ocp_dir + "/47400000.usb/musb-hdrc.1.auto/power/autosuspend_delay_ms","-1");
		}
	}
#endif

	//BBBでネットワークの設定がスルーされてしまうので、こちらで補正する。
	SystemMisc::NetworkInit(
		(MainWindow::m()->Config.Get("network_ipaddress_type","DHCP") == "DHCP" ? "DHCP" : MainWindow::m()->Config.Get("network_ipaddress_ip","DHCP") )
		,MainWindow::m()->Config.Get("network_ipaddress_mask","")
		,MainWindow::m()->Config.Get("network_ipaddress_gateway","")
		,MainWindow::m()->Config.Get("network_ipaddress_dns","")
	);

	//マイクの状態をチェックします
	if ( !  SystemMisc::WaitForMicSetup(3) )
	{
		ERRORLOG("マイクの状態が改善されません。システムを再起動します");
		return false;
	}


#if _MSC_VER
#else
	//linuxの場合、 DHCPでIPがすぐに取れないことかあるので、何回かリトライする.
	SystemMisc::WaitForIPAddress(120);
#endif //_MSC_VER

	//NTPで時刻を合わせる
	NOTIFYLOG("時刻を合わせます..");
	SystemMisc::SetNTPDate();

	return true;
}

void SystemMisc::Play(const string & filename)
{
	DEBUGLOG("音楽再生 "<< filename );
#ifdef _MSC_VER
	MainWindow::m()->PlayMCISync(filename);
#else
	const string ext = XLStringUtil::strtolower(XLStringUtil::baseext_nodot(filename));
	if (ext == "mp3")
	{
		const string cmd = "nice -n -1 /usr/bin/mplayer -vo null \"" + filename + "\" > /dev/null ";
		system(cmd.c_str() );
	}
	else if (ext == "wav")
	{
		const string cmd = "nice -n -1 /usr/bin/mplayer -vo null \"" + filename + "\" > /dev/null ";
		system(cmd.c_str() );
	}
#endif
}

void SystemMisc::StopPlay()
{
#ifdef _MSC_VER
	MainWindow::m()->StopMCISync();
#else
	//引数も含めたプロセス名を取得
	vector<SystemMisc::ProcessSt> processList = SystemMisc::ProcessList();

	for(auto it = processList.begin() ; it != processList.end() ; it++ )
	{
		if ( it->args.find("/usr/bin/mplayer") != string::npos )
		{
			kill(it->pid,0);
		}
	}
#endif
}

void SystemMisc::SetCPUFreq(int type)
{
#ifdef _MSC_VER
#else
	if(type)
	{
		system("cpufreq-set -g performance");
	}
	else
	{
		system("cpufreq-set -g ondemand");
	}
#endif
}

//UUIDの生成
string SystemMisc::makeUUID()
{
#ifdef _MSC_VER
	const string fhcuuid = MainWindow::m()->GetConfigBasePath("linuxboot/fhcuuid.exe");
#else
	const string fhcuuid = MainWindow::m()->GetConfigBasePath("linuxboot/fhcuuid");
#endif
	if ( ! XLFileUtil::Exist(fhcuuid) )
	{
		NOTIFYLOG("fhcuuidがありません");
		return makeUUIDFree();
	}
	const string retUUID = XLStringUtil::chop(SystemMisc::PipeExec(fhcuuid));
	NOTIFYLOG("fhcuuidを生成します " << fhcuuid << " result:" << retUUID );
	if (retUUID.empty())
	{
		NOTIFYLOG("fhcuuidを生成できませんでした");
		return "";
	}
	return retUUID;
}

//UUIDのチェック
bool SystemMisc::checkUUID(const string& uuid)
{
	//本当はちゃんと検証したいが、
	//我々にも利益が必要だ。
	if ( uuid.size() < 15 )
	{
		return false;
	}
	return true;
}

//フリーモードのUUID生成
std::string SystemMisc::makeUUIDFree()
{
	return "F-" + XLStringUtil::uuid() + "-F";
}

bool SystemMisc::setup_mDNS()
{
#ifdef _MSC_VER
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (hSCM == NULL) 
	{
        return false;
    }

    SC_HANDLE hService = OpenService(hSCM, "Bonjour Service", SERVICE_QUERY_STATUS);
    if (hService == NULL) {
	    CloseServiceHandle(hSCM);
        return false;
    }

    //サービスの状態を取得する
    SERVICE_STATUS ServiceStatus;
    if (!QueryServiceStatus(hService, &ServiceStatus))
	{
	    CloseServiceHandle(hSCM);
	    CloseServiceHandle(hService);
        return false;
	}
	CloseServiceHandle(hSCM);
	CloseServiceHandle(hService);

	if (SERVICE_RUNNING != ServiceStatus.dwCurrentState)
	{
		ERRORLOG("Bonjour Service is down. please run this services.");
        return false;
	}

	return true;
#else
	string psResult = SystemMisc::PipeExec("ps aux | grep avahi-daemon | grep -v grep");
	if ( psResult.find("avahi-daemon") != string::npos )
	{//avahi動作してる.
		return true;
	}

	const string cmd = "/etc/init.d/avahi-daemon start";
	SystemMisc::PipeExec(cmd);
	SecSleep(1);

	psResult = SystemMisc::PipeExec("ps aux | grep avahi-daemon | grep -v grep");
	if ( psResult.find("avahi-daemon") != string::npos )
	{//avahi動作してる.
		return true;
	}
	return false;
#endif
}

//LANケーブルがつながっているかどうか？
bool SystemMisc::is_LinkDetected(const string& eth)
{
#if _MSC_VER
	//windowsではわからないので常につながっていると回答する.
	return true;
#else
	//参考: http://www.komoto.org/etc/ethlink.html

	struct ethtool_value  ethval = {0};
	struct ifreq          ifr = {0};

	/* init query structure */
	strncpy(ifr.ifr_name, eth.c_str(), IFNAMSIZ);

	ethval.cmd = ETHTOOL_GLINK;
	ifr.ifr_data = (caddr_t)&ethval;

	/* open control socket */
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if( 0 > fd )
	{
		ERRORLOG("Cannot open socket");
		return true;
	}

	/* query */
	int err = ioctl(fd, SIOCETHTOOL, &ifr);
	close(fd);

	if( 0 == err )
	{
		if( ethval.data )
		{
			//Ethernet is linked
			return true;
		}
		else
		{
			//Ethernet is not linked
			return false;
		}
	}

	ERRORLOG("ethlink: unknown. Query failed.");
	//よくわからないので OKを返す.
	return true;
#endif
}

//IPアドレスの取得
map<string,string> SystemMisc::GetIPAddressMap() 
{
	map<string,string> interfaces;
#if _MSC_VER
	IP_ADAPTER_INFO adapterInfo[256];
	ULONG ulOutBufLen = sizeof(adapterInfo);
	DWORD ret = GetAdaptersInfo(adapterInfo, &ulOutBufLen);
    if (ret != NO_ERROR)
	{
//		ERRORLOG("IPアドレス一覧が取得できませんでした");
		return interfaces;
	}
	for (const IP_ADAPTER_INFO* pAdapter = adapterInfo; pAdapter; pAdapter = pAdapter->Next) 
	{
		if (strcmp(pAdapter->IpAddressList.IpAddress.String , "0.0.0.0") == 0 )
		{//動いていないものはボツ
//			ERRORLOG("IPインターフェース " << pAdapter->Description << " は動作していないので無視します");
			continue;
		}
		if (strcmp(pAdapter->GatewayList.IpAddress.String , "0.0.0.0") == 0 )
		{//動いているけど、 gatewayが設定されていないものは無視
//			ERRORLOG("IPインターフェース " << pAdapter->Description << " の、IPアドレス " << pAdapter->IpAddressList.IpAddress.String << " は、動作しているが gatewayが設定されていないので無視します。");
			continue;
		}

		interfaces[pAdapter->Description] = pAdapter->IpAddressList.IpAddress.String;
//		DEBUGLOG("IPインターフェース " << pAdapter->Description << " の、IPアドレス " << pAdapter->IpAddressList.IpAddress.String << " を発見しました。");
	}
#else
	//http://www.geekpage.jp/programming/linux-network/book/04/4-10.php より
	int socket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if ( 0 > socket)
	{
		ERRORLOG("IPアドレス一覧が取得できませんでした");
		return interfaces;
	}

	struct ifconf ifc;
	//何個あるか取得する.
	ifc.ifc_len = 0;
	ifc.ifc_ifcu.ifcu_buf = NULL;
	ioctl(socket, SIOCGIFCONF, &ifc);

	//個数分のメモリ確保
	ifc.ifc_ifcu.ifcu_buf = (char *)malloc(ifc.ifc_len);

	//実際の数
	ioctl(socket, SIOCGIFCONF, &ifc);
	unsigned int nifs = ifc.ifc_len / sizeof(struct ifreq);
	struct ifreq * ifr = (struct ifreq *)ifc.ifc_ifcu.ifcu_buf;
	for (unsigned int i=0; i<nifs; i++)
	{
		if (strcmp(ifr[i].ifr_name,"lo")==0)
		{
//			NOTIFYLOG("IPインターフェース " << ifr[i].ifr_name << " はローカルループバックなので無視します");
		}
		else
		{
			interfaces[ifr[i].ifr_name] = inet_ntoa(((struct sockaddr_in *)&ifr[i].ifr_addr)->sin_addr);
//			DEBUGLOG("IPインターフェース " << ifr[i].ifr_name << " の、IPアドレス " << interfaces[ifr[i].ifr_name] << " を発見しました。");
		}
	}

	free(ifc.ifc_ifcu.ifcu_buf);
	close(socket);
#endif
	return interfaces;
}


//マックアドレスを取得
string SystemMisc::GetMacaddress(const string& eth,const string& sep)
{
	vector<unsigned char> vec = GetMacaddressVec(eth);
	if ( vec.size() != 6 )
	{//macアドレスが、6バイトではないらしい？
		return "";
	}
	
	// 結果を表示 
	char buffer[30];
	sprintf(buffer,"%.2x%s%.2x%s%.2x%s%.2x%s%.2x%s%.2x\n",
		vec[0],sep.c_str(),
		vec[1],sep.c_str(),
		vec[2],sep.c_str(),
		vec[3],sep.c_str(),
		vec[4],sep.c_str(),
		vec[5]
	);

	return buffer;
}

//マックアドレスを vector<uchar>で
vector<unsigned char> SystemMisc::GetMacaddressVec(const string& eth)
{
	vector<unsigned char> ret;
#if _MSC_VER

	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter;
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	unsigned char *addr;

	pAdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
	if(!pAdapterInfo)
	{
		return ret;
	}
	if(GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
		if(!pAdapterInfo)
		{
			return ret;
		}
	}

	if(GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR)
	{
		pAdapter = pAdapterInfo;
		if(pAdapter)
		{
			addr = pAdapter->Address;
			ret.push_back((unsigned char)addr[0]);
			ret.push_back((unsigned char)addr[1]);
			ret.push_back((unsigned char)addr[2]);
			ret.push_back((unsigned char)addr[3]);
			ret.push_back((unsigned char)addr[4]);
			ret.push_back((unsigned char)addr[5]);
		}
	}
	free(pAdapterInfo);
	return ret;
#else
	int fd;
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if( 0 > fd )
	{
		ERRORLOG("マックアドレスを取得できません");
		return ret;
	}

	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, eth.c_str(), IFNAMSIZ-1);

	ioctl(fd, SIOCGIFHWADDR, &ifr);

	close(fd);

	// 結果を表示 
	
	ret.push_back((unsigned char)ifr.ifr_hwaddr.sa_data[0]);
	ret.push_back((unsigned char)ifr.ifr_hwaddr.sa_data[1]);
	ret.push_back((unsigned char)ifr.ifr_hwaddr.sa_data[2]);
	ret.push_back((unsigned char)ifr.ifr_hwaddr.sa_data[3]);
	ret.push_back((unsigned char)ifr.ifr_hwaddr.sa_data[4]);
	ret.push_back((unsigned char)ifr.ifr_hwaddr.sa_data[5]);

	return ret;
#endif
}

