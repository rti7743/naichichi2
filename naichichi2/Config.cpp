//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#include "common.h"
#include "Config.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"

Config::Config()
{
}
Config::~Config()
{
}

bool Config::Create(const string& config)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	return this->loadConfig(config);
}

bool Config::overrideSave()
{
	if ( ! this->ConfigFullpath.empty() )
	{
		this->saveConfig(this->ConfigFullpath);
	}
	return true;
}



bool Config::loadConfig(const string & configFilename)
{
	lock_guard<mutex> al(this->lock);
	
	this->ConfigData.clear();
	this->ConfigFullpath = configFilename;

	FILE * fp = fopen( XLStringUtil::pathseparator(configFilename).c_str() ,"rb");
	if (fp == NULL)
	{
		return false;
	}
	const int bufferMax = 65535;
	auto vec=vector<char>(bufferMax+1);
	char* readbuffer = &vec[0];

	while( !feof(fp) )
	{
		readbuffer[0] = 0;
		if ( fgets(readbuffer , bufferMax-1 , fp) == NULL ) break;
		readbuffer[bufferMax-1] = 0;
		if (readbuffer[0] == 0) continue;
		const string str = _U2A(readbuffer);
		const char* buf = str.c_str();

		if ( this->IsComment(buf) ) continue;

		const char * eq = strchr( buf , '=');
		if (eq == NULL) continue;

		string key = XLStringUtil::chop(  string(buf , 0 , eq - buf) ) ;
		string value = XLStringUtil::chop( string(eq + 1) ) ;

		this->ConfigData[key] = value;
	}
	fclose(fp);
	return true;
}

bool Config::saveConfig(const string & configFilename)
{
	lock_guard<mutex> al(this->lock);
	

	map<string,string>	localSetting = this->ConfigData;

	string tmpfilename = configFilename + ".tmp";
	FILE * wfp = fopen( XLStringUtil::pathseparator(tmpfilename).c_str() ,"wb");
	if (wfp == NULL)
	{
		throw XLException("設定保存用の一時ファイル" + tmpfilename + "を作成できませんでした。");
	}
	FILE * fp = fopen( XLStringUtil::pathseparator(configFilename).c_str() ,"rb");
	if (fp != NULL)
	{
			const int bufferMax = 65535;
			auto vec=vector<char>(bufferMax+1);
			char* readbuffer = &vec[0];
			int kakaline = 0;

			while( !feof(fp) )
			{
				readbuffer[0] = 0;
				if ( fgets(readbuffer , bufferMax-1 , fp) == NULL ) break;
				readbuffer[bufferMax-1] = 0;
				if (readbuffer[0] == 0) continue;

				const string str = _U2A(readbuffer);
				const char* buf = str.c_str();
				if ( (buf[0] == '\n' && buf[1] == '\0') || (buf[0] == '\r' && buf[1] == '\n' && buf[2] == '\0'))
				{
					kakaline++;
					if(kakaline < 5)
					{//複数行連続する改行は消す
						fprintf(wfp , "\r\n" );
					}
					continue;
				}
				kakaline=0;

				if ( this->IsComment(buf) )
				{
					fprintf(wfp , "%s" , _A2U(buf));
					continue;
				}

				const char * eq = strchr( buf , '=');
				if (eq == NULL)
				{//ゴミデータなので無視する
					continue;
				}

				const string key = XLStringUtil::chop( string(buf , 0 , eq - buf) ) ;
				const string value = XLStringUtil::chop( string(eq + 1) ) ;

				//このデータは読み込んでいますか?
				const auto i = localSetting.find(key);
				if (i == localSetting.end() )
				{
					//ない場合は削除
					continue;
				}
				if (value == (*i).second)
				{
					fprintf(wfp , "%s" , _A2U(buf));
					localSetting.erase(i);
					continue;
				}

				//書き換え.
				const string writekey = _A2U((*i).first);
				const string writevalue = _A2U((*i).second);
				if ( writekey.empty() )
				{
					continue;
				}
				fprintf(wfp , "%s=%s\r\n" , writekey.c_str()  , writevalue.c_str() );
				localSetting.erase(i);
			}
	}
	//まだ書き込んで値は追加.
	{
		for(map<string,string>::iterator i = localSetting.begin() ; i != localSetting.end() ; ++i)
		{
			const string writekey = _A2U((*i).first);
			const string writevalue = _A2U((*i).second);
			fprintf(wfp , "%s=%s\r\n" , writekey.c_str()  , writevalue.c_str() );
		}
	}
	if (fp) fclose(fp);
	if (wfp) fclose(wfp);

	XLFileUtil::copy(tmpfilename,configFilename);
//	XLFileUtil::del( tmpfilename );

	return true;
}




void Config::Set(const string& key,const string& value ) 
{
	if ( ! XLStringUtil::IsNotReturnAndEqualCode(key) ) return ;
	if ( ! XLStringUtil::IsNotReturnCode(value) ) return ;

	lock_guard<mutex> al(this->lock);
	this->ConfigData[key] = value;
}

void Config::Remove(const string& key ) 
{
	lock_guard<mutex> al(this->lock);

	auto it = this->ConfigData.find(key);

	if (it != this->ConfigData.end())
	{
		this->ConfigData.erase(it);
	}
}

void Config::RemoveTree(const string& prefix ) 
{
	const auto configmap = this->FindGetsToMap(prefix,false);
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		this->Remove( it->first.c_str() );
	}
}


string Config::Get(const string & key , const string & defaultValue) const
{
	lock_guard<mutex> al(this->lock);
	
	auto it = this->ConfigData.find(key);
	if (it == this->ConfigData.end())
	{
		return defaultValue;
	}
	return it->second;
}

float Config::GetDouble(const string & key , float defaultValue) const
{
	lock_guard<mutex> al(this->lock);
	
	auto it = this->ConfigData.find(key);
	if (it == this->ConfigData.end())
	{
		return defaultValue;
	}
	return (float) atof(it->second.c_str());
}

int Config::GetInt(const string & key , int defaultValue) const
{
	lock_guard<mutex> al(this->lock);
	
	auto it = this->ConfigData.find(key);
	if (it == this->ConfigData.end())
	{
		return defaultValue;
	}
	return atoi(it->second.c_str());
}

bool Config::GetBool(const string & key , bool defaultValue) const
{
	lock_guard<mutex> al(this->lock);
	
	auto it = this->ConfigData.find(key);
	if (it == this->ConfigData.end())
	{
		return defaultValue;
	}
	return atoi(it->second.c_str()) != 0;
}

list<string> Config::FindGets(const string & prefix) const
{
	lock_guard<mutex> al(this->lock);
	
	list<string> r;
	for(auto it = this->ConfigData.begin() ; it != this->ConfigData.end() ; ++it )
	{
		if ( it->first.find(prefix) == 0 )
		{
			r.push_back(it->second);
		}
	}
	return r;
}

const map<string,string>Config::FindGetsToMap(const string & prefix,bool prefixtrim) const
{
	lock_guard<mutex> al(this->lock);
	
	map<string,string> r;
	for(auto it = this->ConfigData.begin() ; it != this->ConfigData.end() ; ++it )
	{
		if ( it->first.find(prefix) == 0 )
		{
			if (prefixtrim)
			{
				const string key = it->first.substr( prefix.size() );
				r.insert( pair<string,string>(key, it->second) );
			}
			else
			{
				r.insert( *it );
			}
		}
	}
	return r;
}

const map<string,string>Config::ToMap() const
{
	lock_guard<mutex> al(this->lock);
	return this->ConfigData;
}

bool Config::ReplaceMap(const string& prefix,const map<string,string>& postData)
{
	lock_guard<mutex> al(this->lock);
	
	//まず該当するデータを消す
	for(auto it = this->ConfigData.begin()  , end = this->ConfigData.end();	it != end ; )
	{
		if ( it->first.find(prefix) == 0)
		{
			this->ConfigData.erase(it++);
		}
		else
		{
			++it;
		}
	}

	//もう一度データを突っ込む
	this->ConfigData.insert(postData.begin() , postData.end() );

	return true;
}



