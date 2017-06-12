#include "FHC_SipCmd.h"

#if _MSC_VER
	#pragma comment(lib, "ws2_32.lib")
	#if _DEBUG
	#pragma comment(lib, "../pjproject/third_party/lib/libbaseclasses-i386-Win32-vc14-Debug.lib")
	#pragma comment(lib, "../pjproject/third_party/lib/libg7221codec-i386-Win32-vc14-Debug.lib")
	#pragma comment(lib, "../pjproject/third_party/lib/libgsmcodec-i386-Win32-vc14-Debug.lib")
	#pragma comment(lib, "../pjproject/third_party/lib/libilbccodec-i386-Win32-vc14-Debug.lib")
	#pragma comment(lib, "../pjproject/third_party/lib/libmilenage-i386-Win32-vc14-Debug.lib")
	#pragma comment(lib, "../pjproject/third_party/lib/libresample-i386-Win32-vc14-Debug.lib")
	#pragma comment(lib, "../pjproject/third_party/lib/libspeex-i386-Win32-vc14-Debug.lib")
	#pragma comment(lib, "../pjproject/third_party/lib/libsrtp-i386-Win32-vc14-Debug.lib")
	#pragma comment(lib, "../pjproject/pjlib/lib/pjlib-i386-Win32-vc14-Debug.lib")
	#pragma comment(lib, "../pjproject/pjsip/lib/pjsip-ua-i386-Win32-vc14-Debug.lib")
	#pragma comment(lib, "../pjproject/pjsip/lib/pjsua-lib-i386-Win32-vc14-Debug.lib")
	#pragma comment(lib, "../pjproject/pjsip/lib/pjsip-core-i386-Win32-vc14-Debug.lib")
	#pragma comment(lib, "../pjproject/pjlib-util/lib/pjlib-util-i386-Win32-vc14-Debug.lib")
	#pragma comment(lib, "../pjproject/pjsip/lib/pjsip-simple-i386-Win32-vc14-Debug.lib")
	#pragma comment(lib, "../pjproject/pjmedia/lib/pjmedia-i386-Win32-vc14-Debug.lib")
	#pragma comment(lib, "../pjproject/pjmedia/lib/pjmedia-codec-i386-Win32-vc14-Debug.lib")
	#pragma comment(lib, "../pjproject/pjmedia/lib/pjmedia-audiodev-i386-Win32-vc14-Debug.lib")
	#pragma comment(lib, "../pjproject/pjnath/lib/pjnath-i386-Win32-vc14-Debug.lib")
	#pragma comment(lib, "../pjproject/pjsip/lib/pjsip-core-i386-Win32-vc14-Debug.lib")
	#else
	#pragma comment(lib, "../pjproject/third_party/lib/libbaseclasses-i386-Win32-vc14-Release.lib")
	#pragma comment(lib, "../pjproject/third_party/lib/libg7221codec-i386-Win32-vc14-Release.lib")
	#pragma comment(lib, "../pjproject/third_party/lib/libgsmcodec-i386-Win32-vc14-Release.lib")
	#pragma comment(lib, "../pjproject/third_party/lib/libilbccodec-i386-Win32-vc14-Release.lib")
	#pragma comment(lib, "../pjproject/third_party/lib/libmilenage-i386-Win32-vc14-Release.lib")
	#pragma comment(lib, "../pjproject/third_party/lib/libresample-i386-Win32-vc14-Release.lib")
	#pragma comment(lib, "../pjproject/third_party/lib/libspeex-i386-Win32-vc14-Release.lib")
	#pragma comment(lib, "../pjproject/third_party/lib/libsrtp-i386-Win32-vc14-Release.lib")
	#pragma comment(lib, "../pjproject/pjlib/lib/pjlib-i386-Win32-vc14-Release.lib")
	#pragma comment(lib, "../pjproject/pjsip/lib/pjsip-ua-i386-Win32-vc14-Release.lib")
	#pragma comment(lib, "../pjproject/pjsip/lib/pjsua-lib-i386-Win32-vc14-Release.lib")
	#pragma comment(lib, "../pjproject/pjsip/lib/pjsip-core-i386-Win32-vc14-Release.lib")
	#pragma comment(lib, "../pjproject/pjlib-util/lib/pjlib-util-i386-Win32-vc14-Release.lib")
	#pragma comment(lib, "../pjproject/pjsip/lib/pjsip-simple-i386-Win32-vc14-Release.lib")
	#pragma comment(lib, "../pjproject/pjmedia/lib/pjmedia-i386-Win32-vc14-Release.lib")
	#pragma comment(lib, "../pjproject/pjmedia/lib/pjmedia-codec-i386-Win32-vc14-Release.lib")
	#pragma comment(lib, "../pjproject/pjmedia/lib/pjmedia-audiodev-i386-Win32-vc14-Release.lib")
	#pragma comment(lib, "../pjproject/pjnath/lib/pjnath-i386-Win32-vc14-Release.lib")
	#pragma comment(lib, "../pjproject/pjsip/lib/pjsip-core-i386-Win32-vc14-Release.lib")
	#endif
#endif


//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
FHC_SipCmd::FHC_SipCmd()
{
	this->SipAccID = 0;
	this->SipCallID = 0;
	this->SipRingCallID = 0;
	this->isEnable = false;
}
FHC_SipCmd::~FHC_SipCmd()
{
	Stop();
}

bool FHC_SipCmd::Create()
{
	if (this->isEnable)
	{//既に有効化されている.
		NOTIFYLOG("sip server is aleary enable.");
		return false;
	}
	this->isEnable = false;
	this->SipCallID = 0;
	this->SipRingCallID = 0;

	bool r = SetupSIP();
	if (!r)
	{//失敗.
		ERRORLOG("SetupIPに失敗しました.");
		Stop();
		return false;
	}
	return true;
}


void FHC_SipCmd::Stop()
{
	//すべての通話の切断(必要?)
	pjsua_call_hangup_all();

	//Destroy pjsua
	pjsua_destroy();

	this->SipAccID = 0;
	this->SipCallID = 0;
	this->isEnable = false;
}

//電話がかかってきたときのコールバック.
void FHC_SipCmd::OnIncomingEvent(int call_id,const pjsua_call_info& ci)
{
	if ( IsTalkingNow() )
	{
		//話し中です.
		//とりあえずキャッチフォンはサポートしない.
		pjsua_call_answer(call_id, 486, NULL, NULL);
		return ;
	}
	if ( IsIncoming() )
	{
		//別の電話が先にかかってきている.
		//話し中です.
		pjsua_call_answer(call_id, 486, NULL, NULL);
		return ;
	}
	//電話がかかってきました。
	this->SipRingCallID = call_id;
	std::string sipurl = ParseSipAddress(ci.remote_info.ptr);
	printf(">INCOMING\t200\t%s\r\n",sipurl.c_str() );
}


//callの状態が変わった時に呼び出される.
void FHC_SipCmd::OnCallStateEvent(pjsua_call_id call_id,const pjsua_call_info& ci)
{
    pjsua_call_info call_info;
    pjsua_call_get_info(call_id, &call_info);
	NOTIFYLOG("Call " 
		<< num2str(call_id) 
		<< " state="
		<< std::string(call_info.state_text.ptr,0,call_info.state_text.slen) );

    if (call_info.state == PJSIP_INV_STATE_DISCONNECTED) 
	{
//		NOTIFYLOG("Call " << call_id << 
//			" is DISCONNECTED [reason=" << call_info.last_status << 
//			" (" << call_info.last_status_text.ptr << ")]");

		this->SipCallID = 0;
		this->SipRingCallID = 0;
		printf(">HANGUP\t200\r\n",""); //?
	}
}



void FHC_SipCmd::OnCallMediaStateEvent(pjsua_call_id call_id,const pjsua_call_info& ci)
{

	if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) 
	{
		//呼び出しのベルを止める.
		//ring_stop(call_id);

		if (this->SipRingCallID)
		{
			//電話がきたので、受話器を上げて答えた.
			std::string sipurl = ParseSipAddress(ci.remote_info.ptr);
			printf(">ANSWER\t200\t%s\r\n",sipurl.c_str());
		}
		else
		{
			//電話の相手が出た.
			std::string sipurl = ParseSipAddress(ci.local_info.ptr);
			printf(">CALL\t200\t%s\r\n",sipurl.c_str());
		}

		this->SipCallID = call_id;
		this->SipRingCallID = 0;


		// When media is active, connect call to sound device.
		pjsua_conf_connect(ci.conf_slot, 0);
		pjsua_conf_connect(0, ci.conf_slot);
	}
}

//有効かどうか
bool FHC_SipCmd::IsEnable() const
{
	return this->isEnable;
}

//SIP URLの妥当性のチェック
bool FHC_SipCmd::URLCheck(const std::string& sipurl) const
{
	pj_status_t status;
	status = pjsua_verify_url(sipurl.c_str());
	return (status == PJ_SUCCESS);
}

//SIP URLの抽出
std::string FHC_SipCmd::ParseSipAddress(const std::string& msg)
{
	//"1234" <sip:123123@rti-giken.jp> --> sip:123123@rti-giken.jp
	const char* p = msg.c_str();
	const char* start = strstr(p,"<sip:");
	if(start == NULL)
	{//番号不明
		return "";
	}
	start=start+1; //skip <
	const char* end = strstr(start,">");
	if(end == NULL)
	{
		return start;
	}
	return std::string(start,0,end - start );
}

SEXYTEST()
{
	{
		const std::string r = FHC_SipCmd::ParseSipAddress("\"1234\" <sip:123123@rti-giken.jp>");
		SEXYTEST_ASSERT(r == "sip:123123@rti-giken.jp");
	}
	{
		const std::string r = FHC_SipCmd::ParseSipAddress("<sip:123123@rti-giken.jp>");
		SEXYTEST_ASSERT(r == "sip:123123@rti-giken.jp");
	}
	{
		const std::string r = FHC_SipCmd::ParseSipAddress(">sip:123123@rti-giken.jp>");
		SEXYTEST_ASSERT(r == "");
	}
}

//電話をかける.
void FHC_SipCmd::Call(const std::string& sipurl)
{
	pj_status_t status;
	if ( ! URLCheck(sipurl) )
	{
		ERRORLOG("指定されたSIPURL(" << sipurl << ")は無効です");
		printf(">CALL\t403\t%s\tSIPURL IS INVALIDATE \r\n",sipurl.c_str() );
		return ;
	}
	if ( IsTalkingNow() )
	{
		ERRORLOG("現在、あなたが話し中のため、指定されたSIPURL(" << sipurl << ")に電話をかけることができません");
		printf(">CALL\t403\t%s\tYOUR ARE TALKING NOW \r\n",sipurl.c_str() );
		return ;
	}
	if ( IsIncoming() )
	{
		ERRORLOG("電話がかかってきているので通話できません");
		printf(">CALL\t403\t%s\tINCOMING NOW \r\n",sipurl.c_str() );
		return;
	}

	pj_str_t uri = pj_str( (char*) sipurl.c_str());
	status = pjsua_call_make_call( this->SipAccID , &uri, 0, this, NULL, NULL);
	if (status != PJ_SUCCESS)
	{
		ERRORLOG("SIPURL(" << num2str(sipurl) << ")に電話をかけられません status:" << num2str(status) );
		printf(">CALL\t403\t%s\r\n",sipurl.c_str() );
		return;
	}
}

//電話がかかってきているか？
bool FHC_SipCmd::IsIncoming()
{
	return this->SipRingCallID != 0;
}

//電話で話しているか？
bool FHC_SipCmd::IsTalkingNow()
{
	return this->SipCallID != 0;
}

//電話に出る
void FHC_SipCmd::Answer()
{
	if ( IsTalkingNow() )
	{
		ERRORLOG("現在、あなたが話し中のため、電話がとれません");
		printf(">ANSWER\t403\r\n");
		return;
	}
	if (! IsIncoming() )
	{
		ERRORLOG("電話がかかってきていないので、電話をとれません。");
		printf(">ANSWER\t403\r\n");
		return;
	}

	/* Automatically answer incoming calls with 200/OK */
	pjsua_call_answer(this->SipRingCallID, 200, NULL, NULL);
}

//電話を切る
void FHC_SipCmd::Hangup()
{
	pjsua_call_hangup_all();

	SipCallID = 0;
	SipRingCallID = 0;
}

void FHC_SipCmd::Auth(const std::string& id,const std::string& domain,const std::string& password)
{
	pj_status_t status;

	/* Register to SIP server by creating SIP account. */
	std::string sipurl = "sip:" + id + "@" + domain;
	std::string sipdomain = "sip:" + domain;

	pjsua_acc_config cfg;

	pjsua_acc_config_default(&cfg);
	cfg.id = pj_str((char*) sipurl.c_str());
	cfg.reg_uri = pj_str((char*)sipdomain.c_str() );
	cfg.cred_count = 1;
	cfg.cred_info[0].realm = pj_str((char*)"*" );
	cfg.cred_info[0].scheme = pj_str("digest");
	cfg.cred_info[0].username = pj_str((char*)id.c_str() );
	cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
	cfg.cred_info[0].data = pj_str( (char*)password.c_str() );

	status = pjsua_acc_add(&cfg, PJ_TRUE, &this->SipAccID);
	if (status != PJ_SUCCESS)
	{
		this->isEnable = false;
		ERRORLOG("Error adding account" + num2str(status) );
		printf(">AUTH\t403 ACCOUNT ERROR\r\n");
		return ;
	}
	//thisポインタの保存.
	pjsua_acc_set_user_data(this->SipAccID,this);

	SecSleep(1);

	//認証完了待ち.
	bool isAuth = false;
	for(int i = 0 ; i < 10 ; i++)
	{
		pjsua_acc_info info;
		pjsua_acc_get_info(this->SipAccID,&info);
		if ( info.status == PJSIP_SC_OK  || info.status == PJSIP_SC_ACCEPTED )
		{
			isAuth = true;
			break;
		}

		if ( info.status != PJSIP_SC_TRYING )
		{
			break;
		}
		MiriSleep(500);
	}

	if ( ! isAuth )
	{
		this->isEnable = false;
		ERRORLOG("Error online_status is false" );
		printf(">AUTH\t403 ACCOUNT ERROR\r\n");
		return ;
	}
	
	this->SipDomain = domain;
	this->isEnable = true;
	printf(">AUTH\t200\r\n");
	return;
}

//電話がかかってきたときのコールバック.
static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
			     pjsip_rx_data *rdata)
{
	FHC_SipCmd* _this = (FHC_SipCmd*)pjsua_acc_get_user_data(acc_id);

	pjsua_call_info ci;
	pjsua_call_get_info(call_id, &ci);
//	NOTIFYLOG("ci: call:" << num2str(call_id) 
//		<< "state:" << std::string(ci.state_text.ptr,0,ci.state_text.slen) 
//		<< "remote_info:" << std::string(ci.remote_info.ptr,0,(int)ci.remote_info.slen) 
//		<< "reason:" << ci.last_status << " (" << std::string(ci.last_status_text.ptr) << ")" 
//		<< "this:" << num2str(_this)
//		);

	if (_this)
	{
		pjsua_call_set_user_data(call_id,_this);
		_this->OnIncomingEvent(call_id,ci);
		fflush(stdout);
	}
}

static void on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
	FHC_SipCmd* _this = (FHC_SipCmd*)pjsua_call_get_user_data(call_id);

	pjsua_call_info ci;
	pjsua_call_get_info(call_id, &ci);
//	NOTIFYLOG("ci: call:" << num2str(call_id) 
//		<< "state:" << std::string(ci.state_text.ptr,0,ci.state_text.slen) 
//		<< "remote_info:" << std::string(ci.remote_info.ptr,0,(int)ci.remote_info.slen) 
//		<< "reason:" << ci.last_status << " (" << std::string(ci.last_status_text.ptr) << ")" 
//		<< "this:" << num2str(_this)
//		);

	if (_this)
	{
		_this->OnCallStateEvent(call_id,ci);
		fflush(stdout);
	}
}

//メディアの状態がかわった (つまり、電話がつながって話し始めたとか、切れたとかそういうとき)
static void on_call_media_state(pjsua_call_id call_id)
{
	FHC_SipCmd* _this = (FHC_SipCmd*)pjsua_call_get_user_data(call_id);

	pjsua_call_info ci;
	pjsua_call_get_info(call_id, &ci);
	NOTIFYLOG("ci: call:" << num2str(call_id) 
		<< "state:" << std::string(ci.state_text.ptr,0,ci.state_text.slen) 
		<< "remote_info:" << std::string(ci.remote_info.ptr,0,(int)ci.remote_info.slen) 
		<< "reason:" << ci.last_status << " (" << std::string(ci.last_status_text.ptr) << ")" 
		<< "this:" << num2str(_this)
		);

	if (_this)
	{
		_this->OnCallMediaStateEvent(call_id,ci);
		fflush(stdout);
	}
}
static void on_logger(int level, const char *data, int len)
{
	//ログについてはまだ意見がまとまらない.
	//ログ回収方法が何かほしいね。
}


bool FHC_SipCmd::SetupSIP()
{
	pj_status_t status;
	
	/* Create pjsua first! */
	status = pjsua_create();
	if (status != PJ_SUCCESS)
	{
		ERRORLOG("Error in pjsua_create()" + num2str(status) );
		return false;
	}

	/* Init pjsua */
	{
		pjsua_config cfg;
		pjsua_logging_config log_cfg;

		pjsua_config_default(&cfg);
		cfg.cb.on_incoming_call = &on_incoming_call;
		cfg.cb.on_call_media_state = &on_call_media_state;
		cfg.cb.on_call_state = &on_call_state;

		pjsua_logging_config_default(&log_cfg);
		log_cfg.msg_logging = false;
	    log_cfg.level = 0;
		log_cfg.console_level = 0;
		log_cfg.cb = on_logger;

		status = pjsua_init(&cfg, &log_cfg, NULL);
		if (status != PJ_SUCCESS)
		{
			ERRORLOG("Error in pjsua_init()" + num2str(status) );
			return false;
		}
    }

    /* Add UDP transport. */
    {
		pjsua_transport_config cfg;

		pjsua_transport_config_default(&cfg);
		cfg.port = 5060;
		status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, NULL);
		if (status != PJ_SUCCESS)
		{
			ERRORLOG("Error creating transport" + num2str(status) );
			return false;
		}
	}

	/* Initialization is done, now start pjsua */
	status = pjsua_start();
	if (status != PJ_SUCCESS)
	{
		ERRORLOG("Error starting pjsua" + num2str(status) );
		return false;
	}

	return true;
}

