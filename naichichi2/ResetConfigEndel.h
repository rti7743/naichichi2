#pragma once

class ResetConfigEndel : public IResetConfigInterface
{
private:
	void ThreadMain();
private:
    thread*					Thread;
	mutable mutex lock;
	condition_variable EventObject;
	bool StopFlag;

public:
	ResetConfigEndel();
	virtual ~ResetConfigEndel();

	virtual void Create();
	virtual void Stop();

	static int loadConfig(const string& filename);
	static string make(const string& user_mail
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
