#pragma once
#include "common.h"
#include "XLStringUtil.h"

//翻訳する.
class TranslateFile
{
public:
	TranslateFile();
	virtual ~TranslateFile();

	void Create(const string& path,const string& lang);
	string TransRemocon(const string& html) const
	{
		if (this->remocon ==NULL) return html;
		return replace(html,(const char**)this->remocon);
	}
	string TransEdit(const string& html) const
	{
		if (this->edit ==NULL) return html;
		return replace(html,(const char**)this->edit);
	}
	string TransAuth(const string& html) const
	{
		if (this->auth ==NULL) return html;
		return replace(html,(const char**)this->auth);
	}
	string TransServerNetworkSetting(const string& html) const
	{
		if (this->serverNetworkSetting ==NULL) return html;
		return replace(html,(const char**)this->serverNetworkSetting);
	}
	string TransClientWelcome(const string& html) const
	{
		if (this->clientWelcome ==NULL) return html;
		return replace(html,(const char**)this->clientWelcome);
	}
	string TransServerPasswordreset(const string& html) const
	{
		if (this->serverPasswordreset ==NULL) return html;
		return replace(html,(const char**)this->serverPasswordreset);
	}
	
private:
	void ClearCharList(char** list);
	char** Load(const string& fullname) const;
	string replace(const string &inTarget ,const char** replacetable) const;

	char**	remocon;
	char**	edit;
	char**	auth;
	char**	clientWelcome;
	char**	serverNetworkSetting;
	char**	serverPasswordreset;
	char**	serverWelcome;
};
