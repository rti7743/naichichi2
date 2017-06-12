#include "common.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"
#include "MainWindow.h"

TranslateFile::TranslateFile()
{
	this->edit = NULL;
	this->remocon = NULL;
	this->auth = NULL;
	this->serverNetworkSetting = NULL;
	this->serverPasswordreset = NULL;
	this->serverWelcome = NULL;
	this->clientWelcome = NULL;
}

TranslateFile::~TranslateFile()
{
	ClearCharList(this->edit);
	ClearCharList(this->remocon);
	ClearCharList(this->auth);
	ClearCharList(this->serverNetworkSetting);
	ClearCharList(this->serverPasswordreset);
	ClearCharList(this->serverWelcome);
	ClearCharList(this->clientWelcome);
}

void TranslateFile::ClearCharList(char** list)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	if (list == NULL) return ;
	for(char** p = list; *p ; p++ )
	{
		free(*p);
	}
	free(list);
}

void TranslateFile::Create(const string& path,const string& lang)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	ClearCharList(this->edit);
	ClearCharList(this->remocon);
	ClearCharList(this->auth);
	
	this->edit = Load(path+"/edit.tpl."+lang);
	this->remocon = Load(path+"/remocon.tpl."+lang);
	this->auth = Load(path+"/auth.tpl."+lang);
	this->serverNetworkSetting = Load(path+"/server_networksetting.tpl."+lang);
	this->serverPasswordreset = Load(path+"/server_passwordreset_resetform.tpl."+lang);
	this->serverWelcome = Load(path+"/server_welcome.tpl."+lang);
	this->clientWelcome = Load(path+"/client_welcome.tpl."+lang);
}

char** TranslateFile::Load(const string& fullname) const
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	

	vector<pair<char*,char*>> tempList;

	enum inner_state
	{
		 inner_state_none
		,inner_state_string
	};
	inner_state state = inner_state_none;
	string genbun;

	FILE * fp = fopen( XLStringUtil::pathseparator(fullname).c_str() ,"rb");
	if (fp == NULL)
	{
		ERRORLOG("trans file can not open " << fullname);
		return NULL;
	}
	vector<char> readbufferVec(1024*4);
	char* readbuffer = &readbufferVec[0];

	while( !feof(fp) )
	{
		readbuffer[0] = 0;
		if ( fgets(readbuffer , readbufferVec.size()-1 , fp) == NULL ) break;
		readbuffer[readbufferVec.size()-1] = 0;
		if (readbuffer[0] == 0) continue;
		const string str = XLStringUtil::chop(_U2A( readbuffer ));
		const char* buf = str.c_str();

		if (state == inner_state_none)
		{
			if ( buf[0] == ':' )
			{
				state = inner_state_string;
				genbun = buf + 1;
			}
		}
		else
		{
			if ( buf[0] == ':' )
			{
				state = inner_state_string;
				genbun = buf + 1;
			}
			else if ( buf[0] == 0 )
			{
				state = inner_state_none;
				genbun = "";
			}
			else if ( genbun.empty() )
			{
				state = inner_state_none;
				genbun = "";
			}
			else
			{
				string a = _A2U(genbun);
				string b = _A2U(buf);
				tempList.push_back(pair<char*,char*>(strdup(a.c_str()),strdup(b.c_str()) ));
				state = inner_state_none;
			}
		}
	}
	fclose(fp);

	//長さ順にソートする
	sort(tempList.begin() ,tempList.end() , [](const pair<char*,char*>& a,const pair<char*,char*>& b) -> bool {
		unsigned int aa = strlen(a.first);
		unsigned int bb = strlen(b.first);
		return bb<aa;
	});
	char** retArray = (char**)malloc((tempList.size() + 1) * 2*sizeof(char*) );//+2は NULL NULL 2つ分
	int count = 0;
	for(auto it = tempList.begin() ; it != tempList.end(); ++it  )
	{
		retArray[count++] = it->first;
		retArray[count++] = it->second;
	}
	//終端
	retArray[count++] = NULL;
	retArray[count++] = NULL;

	return retArray;
}

string TranslateFile::replace(const string &inTarget ,const char** replacetable) const
{
	string ret = inTarget;
	if (ret.empty())
	{
		return ret;
	}

	return XLStringUtil::replace(ret,replacetable);
}
