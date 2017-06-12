#pragma once

class CMUDict
{
	string Filename;
	unsigned long Index[26*26 + 1];
public:
	CMUDict();
	virtual ~CMUDict();
	void Create(const string& filename);

	string GetYomi(const string& word) const;
};
