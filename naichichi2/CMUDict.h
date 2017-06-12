#pragma once

//英語の発音辞書の読込
class CMUDict
{
	string Filename;
	
	//[a][p]lle みたいに、先頭2文字で索引を作る
	unsigned long Index[26*26 + 1];
public:
	CMUDict();
	virtual ~CMUDict();
	void Create(const string& filename);

	string GetYomi(const string& word) const;
};
