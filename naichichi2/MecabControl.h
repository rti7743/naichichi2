#pragma once

//extern "C" {
	#include "../openjtalk/open_jtalk/mecab/src/mecab.h"
//};
#include "CMUDict.h"


class MecabControl
{
public:
	MecabControl();
	virtual ~MecabControl();
	bool Create();
	bool Create(const string& dicpath,const string& cmudictFilename);
	void Parse(const string& str,const std::function< void(const mecab_node_t* node) >& callbackNode);
	string num2yomi(const string str) const;

	//分かち書きしたものを返します
	vector<string> Wakachigai(const string& str);


	//漢字その他をすべてひらがなに直します
	string KanjiAndKanakanaToHiragana(const string& str);
	//漢字その他をすべてローマ字に変換します。(英語はそのまま)
	string KanjiAndKanaToRomaji(const string& str,bool isRomajiOneSepalate);

	//英語の単語の日本語読みを取得する
	string EnglishWordToKana(const string& englishWord ) const;
	//英語のcmedictでのよみを返します
	string GetCMUYomi(const string& str)
	{
		return CMUDict.GetYomi(str);
	}

	mecab_t* getMecabHandle()
	{
		return this->mecab;
	}

	MecabThreadFeature* mallocFeatures(const string& str);
	void FreeFeatures(MecabThreadFeature* mf);

	
private:
	class CMUDict CMUDict;

	mecab_t* mecab;
	mutable mutex lock;
};
