#pragma once

class JuliusOnsoUtil
{
public:
	JuliusOnsoUtil(){}
	virtual ~JuliusOnsoUtil(){}

	//CMUの辞書から英語Juliusの音素に変換します.
	static string EnglishToEnglishOnso(const string& englishOnso,bool isrev);
	//日本語から英語Juliusの音素に変換します.
	static string JapaneseToEnglishOnso(const string& englishOnso,bool isrev);

	//英語の発音から日本語読みを強引に取得します。
	static string EnglishOnsoToJapanese(const string& englishOnso,bool isrev);
	//英語の読みデータから日本語の音素データに変換します
	static string EnglishToJapanesOnso(const string& english,bool isrev);
	//英語文を日本語の音声認識で利用できる形に変換します
	static string ConvertEnglishSentenceToYomi(const string& sentence,bool useJapanesWakachigaki) ;
	//日本語の音声認識で利用できる形に変換します
	static string ConvertJapanesSentenceToYomi(const string& sentence) ;
	//よみを確認します。
	static bool CheckYomi(const string& phoneFilename,const string& yomi,string* outBadString) ;
	//読みからindex番目の音素(母音 or 母音+子音を取得する)
	static string PickupOnso(const string& yomi,unsigned int index,std::function<bool(const char*)> isBoinFunction) ;
	//読みから最初の母音を取得する
	static string YomiToFirstBoin(const string& yomiChar,std::function<bool(const char*)> isBoinFunction ) ;
	//日本語母音 a i u e o N
	static bool isBoin(const char* p);
	//英語母音 aa ah aw iy uh uw eh ew ow n m
	static bool isBoinEnglish(const char* p);

	//長さを推定する.
	static unsigned int CountLength(const string& matchString) ;
};

