#pragma once

class Config
{
public:
	Config();
	virtual ~Config();
	bool Create(const string& config);

	void Set(const string& key,const string& value ) ;
	void Remove(const string& key ) ;
	void RemoveTree(const string& prefix ) ;
	string Get(const string & key , const string & defaultValue="") const;
	float GetDouble(const string & key , float defaultValue) const;
	int GetInt(const string & key , int defaultValue) const;
	bool GetBool(const string & key , bool defaultValue) const;
	list<string> FindGets(const string & prefix) const;
	const map<string,string>FindGetsToMap(const string & prefix,bool prefixtrim) const;
	const map<string,string>ToMap() const;
	bool ReplaceMap(const string& prefix,const map<string,string>& postData);

	bool loadConfig(const string & configFilename);
	bool saveConfig(const string & configFilename);
	bool overrideSave();
private:
	bool IsComment(const char * line) const
	{
		return ( line[0] == '\0' ||  line[0] == '#' || line[0] == ';' || line[0] == '\'' || (line[0] == '/' &&  line[1] == '/') || (line[0] == '-' &&  line[1] == '-') );
	}
	
	
	map<string , string > ConfigData;
	string ConfigFullpath;

	mutable mutex lock;
};