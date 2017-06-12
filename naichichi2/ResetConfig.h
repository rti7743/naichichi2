#pragma once

struct IResetConfigInterface
{
	virtual void Create() = 0;
	virtual void Stop() = 0;
};

class ResetConfigNone : public IResetConfigInterface
{
public:
	virtual void Create(){ }
	virtual void Stop(){ }
};

class ResetConfig
{
public:
	ResetConfig();
	virtual ~ResetConfig();
	
	void Create();
	void Stop(){ this->Config->Stop(); }
	
	IResetConfigInterface* Config;

	static string make(
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
		,const string& network_w_ipaddress_wkeymgmt);
};
