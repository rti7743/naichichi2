//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#include "common.h"
#include "MainWindow.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"
#include "ResetConfig.h"
#include "ResetConfigEndel.h"
#include "ResetConfigIRR.h"

#ifdef _MSC_VER
#else
#include <poll.h>
#include <sys/stat.h>
#endif

ResetConfig::ResetConfig()
{
	this->Config = NULL;
}

ResetConfig::~ResetConfig()
{
	DEBUGLOG("stop...");

	if (this->Config)
	{
		Stop();
	}
	delete this->Config;
	this->Config = NULL;

	DEBUGLOG("done");
}
#include "SystemMisc.h"

void ResetConfig::Create()
{
//	if (SystemMisc::IsIRR())
//	{
//		this->Config = new ResetConfigIRR;
//	}
//	else
//	{
		this->Config = new ResetConfigEndel;
//	}
	this->Config->Create();
}

string ResetConfig::make(
	 const string& user_mail
	,const string& user_password_sha1
	,const string& network_ipaddress_type
	,const string& network_ipaddress_ip
	,const string& network_ipaddress_mask
	,const string& network_ipaddress_gateway
	,const string& network_ipaddress_dns
	,const string& network_w_ipaddress_type
	,const string& network_w_ipaddress_ip
	,const string& network_w_ipaddress_mask
	,const string& network_w_ipaddress_gateway
	,const string& network_w_ipaddress_dns
	,const string& network_w_ipaddress_ssid
	,const string& network_w_ipaddress_password
	,const string& network_w_ipaddress_wkeymgmt)
{
	const string str = string() + "NAICHICHI2_RESET"
	+ "\t" + user_mail
	+ "\t" + user_password_sha1
	+ "\t" + network_ipaddress_type
	+ "\t" + network_ipaddress_ip
	+ "\t" + network_ipaddress_mask
	+ "\t" + network_ipaddress_gateway
	+ "\t" + network_ipaddress_dns
	+ "\t" + network_w_ipaddress_type
	+ "\t" + network_w_ipaddress_ip
	+ "\t" + network_w_ipaddress_mask
	+ "\t" + network_w_ipaddress_gateway
	+ "\t" + network_w_ipaddress_dns
	+ "\t" + network_w_ipaddress_ssid
	+ "\t" + network_w_ipaddress_password
	+ "\t" + network_w_ipaddress_wkeymgmt
	;
	
	return XLStringUtil::base64encode(str);
}
