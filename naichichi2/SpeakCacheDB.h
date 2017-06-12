#pragma once

//読み上げデータのキャッシュ
//wave生成に時間がかかるため一度読んだものはキャッシュして使いまわす.
class SpeakCacheDB
{
public:
	SpeakCacheDB();
	virtual ~SpeakCacheDB();
	void Create(const string& ext);
	bool Find(const string& str,string* outFilename);
	void Delete(const string& str);
	void Clear();

private:
	void GC();

	string SpeakCacheDir;
	string Ext;
};
