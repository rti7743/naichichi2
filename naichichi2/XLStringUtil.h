// XLStringUtil.h: XLStringUtil クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XLSTRINGUTIL_H__B1B80C81_45F8_4E25_9AD6_FA9AC57294F5__INCLUDED_)
#define AFX_XLSTRINGUTIL_H__B1B80C81_45F8_4E25_9AD6_FA9AC57294F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//文字列全般を扱うユーティリティ
class XLStringUtil  
{
public:
	XLStringUtil();
	virtual ~XLStringUtil();


	//みんな大好きPHPのstrtoupper
	static string strtoupper(const string & str);

	//みんな大好きPHPのstrtolower
	static string strtolower(const string & str);

	static int atoi(const string & str);

	//HTTPヘッダのキャメル
	static string HeaderUpperCamel(const string & str);

	//次の文字列へ
	static inline const char* nextChar(const char * p)
	{
	#if _MSC_VER
		if (((0x81 <= (unsigned char)(*p) && (unsigned char)(*p) <= 0x9f) || (0xe0 <= (unsigned char)(*p) && (unsigned char)(*p) <= 0xfc)))
		{
			return p + 2;
		}
		return p + 1;
	#else
		if ( (((unsigned char)*p) & 0x80) == 0) return p + 1; 
		if ( (((unsigned char)*p) & 0x20) == 0) return p + 2;
		if ( (((unsigned char)*p) & 0x10) == 0) return p + 3;
		if ( (((unsigned char)*p) & 0x08) == 0) return p + 4;
		if ( (((unsigned char)*p) & 0x04) == 0) return p + 5;
		return p + 6;
	#endif
	}

	static unsigned int getChar(const char* p)
	{
	#if _MSC_VER
		if (((0x81 <= (unsigned char)(*p) && (unsigned char)(*p) <= 0x9f) || (0xe0 <= (unsigned char)(*p) && (unsigned char)(*p) <= 0xfc)))
		{
			return (((unsigned char)*p)<<8) + (((unsigned char)*(p+1)));
		}
		return (unsigned char)*p;
	#else
		if ( (((unsigned char)*p) & 0x80) == 0) return (unsigned char)*p; 
		if ( (((unsigned char)*p) & 0x20) == 0) return (((unsigned char)*p)<<8) + (((unsigned char)*(p+1)));
		if ( (((unsigned char)*p) & 0x10) == 0) return (((unsigned char)*p)<<16) + (((unsigned char)*(p+1))<<8) + (((unsigned char)*(p+2)));
		if ( (((unsigned char)*p) & 0x08) == 0) return (((unsigned char)*p)<<24) + (((unsigned char)*(p+1))<<16) + (((unsigned char)*(p+2))<<8) + (((unsigned char)*(p+3)));
		return 0xffffff; //表現不能.
	#endif
	}

	//次の文字列へ
	static inline char* nextChar(char * p)
	{
		return (char*)nextChar((const char*)p);
	}
	//マルチバイトか？
	static inline bool isMultiByte(const char * p)
	{
	#if _MSC_VER
		return ((0x81 <= (unsigned char)(*p) && (unsigned char)(*p) <= 0x9f) || (0xe0 <= (unsigned char)(*p) && (unsigned char)(*p) <= 0xfc));
	#else
		return ( (((unsigned char)*p) & 0x80) != 0); 
	#endif
	}

	//なぜか標準にない /^foo/ みたいに前方マッチ.
	static const char* strfirst(const char* s,const char* n);
	//なぜか標準にない /foo$/ みたいに後方マッチ.
	static const char* strend(const char* s,const char* n);
	//strstrのマルチバイトセーフ 文字列検索
	static const char* strstr(const string& target, const string & need );
	//strstrのマルチバイトセーフ 文字列検索
	static const char* strstr(const char* target, const char* need );
	//strstrのマルチバイトセーフ 文字列検索 //若干非効率
	static const char* strrstr(const string& target, const string & need );
	//strstrのマルチバイトセーフ 文字列検索 //若干非効率
	static const char* strrstr(const char* target, const char* need );
	//stristrのマルチバイトセーフ 大文字小文字関係なしの検索
	static const char* stristr(const string& target, const string & need );
	//strchrのマルチバイトセーフ 文字列の最初ら検索して最初見に使った文字の位置
	static const char* strchr(const string& target, char need );
	//strchrのマルチバイトセーフ 文字列の最初ら検索して最初見に使った文字の位置
	static const char* strchr(const char* target, char need );
	//strrchrのマルチバイトセーフ 文字列の後ろから検索して最初見に使った文字の位置
	static const char* strrchr(const string& target, char need );
	//strrchrのマルチバイトセーフ 文字列の後ろから検索して最初見に使った文字の位置
	static const char* strrchr(const char* target, char need );
	//strposのマルチバイトセーフ (strposはPHPにアルやつね)
	//最初に見つかった場所。見つからなければ -1 を返します。
	static int strpos(const string& target, const string & need );
	//strposのマルチバイトセーフ (strposはPHPにアルやつね)
	//最初に見つかった場所。見つからなければ -1 を返します。
	static int strpos(const char* target, const char* need );
	//strrposのマルチバイトセーフ (strposはPHPにアルやつね)
	//逆から検索して最初に見つかった場所。見つからなければ -1 を返します。
	static int strrpos(const string& target, const string & need );
	//strrposのマルチバイトセーフ (strposはPHPにアルやつね)
	//逆から検索して最初に見つかった場所。見つからなければ -1 を返します。
	static int strrpos(const char* target, const char* need );
	//striposのマルチバイトセーフ (striposはPHPにアルやつね)
	//大文字小文字関係なしで検索して最初に見つかった場所。見つからなければ -1 を返します。
	static int stripos(const string& target, const string & need );

	//みんな大好きPHPのjoin
	static string join(const string& glue , const map<string,string> & pieces );
	static string join(const string& glue , const list<string> & pieces );
	static string join(const string& glue , const vector<int> & pieces );

	//template って知ってるけど、とりあずこれで。
	static string join(const string& glue , const vector<string> & pieces );

	//key=value& みたいな感じの join
	static string crossjoin(const string& glue1 ,const string& glue2 , const map<string,string> & pieces );
	//split
	static list<string> split(const string& glue, const string & inTarget );
	//vector
	static vector<string> split_vector(const string& glue, const string & inTarget );
	static pair<string,string> split_two(const string& glue, const string & inTarget );
	static bool split_two(const string& glue, const string & inTarget , string* first,string* second );
	static bool split_two(const string& glue, const string & inTarget , pair<string,string>* ppp );

	//key=value& みたいな感じの split
	static map<string,string> crosssplit(const string& glue1 ,const string& glue2 , const string & inTarget );
	//key=value& みたいな感じの split キーに対するchopを実行する
	static map<string,string> crosssplitChop(const string& glue1 ,const string& glue2 , const string & inTarget );
	//stringmap 同士のマージ
	static map<string,string> merge(const map<string,string>& a ,const map<string,string>& b , bool overideB );
	static list<string> merge(const list<string>& a ,const list<string>& b );

	//みんな大好きPHPのurldecode
	static string urldecode(const string & inUrl);
	static string urlencode(const string &str) ;
	static string URLENCODE(const string &str) ;

	//base64エンコード
	static string base64encode(const string& src) ;
	//base64エンコード
	static string base64encode(const char* src,int len) ;
	//base64デコード
	static string base64decode(const string& src) ;
	//base64デコード
	static void base64decode(const string& src ,vector<char>* out) ;
	//巡回して、関数 func を適応。 true を返したものだけを結合する。
	static string string_filter(const list<string>& list,const std::function<bool(const string&)>& func);
	//巡回して、関数 func を適応。 funcの戻り文字列を結合します。
	static string string_map(const list<string>& list,const std::function<string (const string&)>& func);
	//巡回して、関数 func を適応。 true を返したものだけを返す。
	static list<string> array_filter(const list<string>& list,const std::function<bool(const string&)>& func);
	//巡回して、関数 func を適応。 funcの戻りで配列作って返します。
	static list<string> array_map(const list<string>& list,const std::function<string (const string&)>& func);


	//みんな大好きPHPのchop 左右の空白の除去
	static string chop(const string & str,const char * replaceTable = NULL);
	static string trim(const string & str,const char * replaceTable = NULL)
	{
		return chop(str,replaceTable);
	}

	//なぜか string に標準で用意されていない置換。ふぁっく。
	static string replace(const string &inTarget ,const string &inOld ,const string &inNew);
	//マルチバイト非対応 の文字列置換
	static string replace_low(const string &inTarget ,const string &inOld ,const string &inNew);

	//みんな大好きPHPのhtmlspecialchars
	//タグをエスケープ 基本的に PHP の htmlspecialchars と同じ.
	//http://search.net-newbie.com/php/function.htmlspecialchars.html
	static string htmlspecialchars(const string &inStr);
	//マルチバイト非対応 タグをエスケープ
	static string htmlspecialchars_low(const string &inStr);

	//みんな大好きPHPのnl2br
	//\nを<br>に 基本的に PHP の nl2br と同じ.
	static string nl2br(const string &inStr);
	//マルチバイト非対応 \nを<br>に
	static string nl2br_low(const string &inStr);
	
	//jsonで利用できる文字列表記にする
	static string jsonescape(const string & str);
	//jsonのエスケープ表記を元に戻す
	static string jsonunescape(const string & str);
	//jsonに追加できる文字列として返す
	static string jsonvalue(const string & name,const string & str);
	//制御文字を飛ばす
	static string clearcontrollcode(const string & str);
	//jsonをmap<sting,string>に変換
	static map<string,string> parsejson(const string & json);
	//json文字列の中から、キーに対するvalueを抜く.
	static string findjsonvalue(const string & json,const string & key);


	//拡張子を取得する. abc.cpp -> ".cpp" のような感じになるよ
	static string baseext(const string &fullpath);
	//拡張子を取得する. abc.cpp -> "cpp" のような感じになるよ . をつけない
	static string baseext_nodot(const string &fullpath);
	//拡張子を取得する. abc.Cpp -> "cpp" のような感じになるよ . をつけないで小文字
	static string baseext_nodotsmall(const string &fullpath);

	//ベースディレクトリを取得する  c:\\hoge\\hoge.txt -> c:\\hoge にする  最後の\\ は消える。
	static string basedir(const string &fullpath);
	//ファイル名を取得する  c:\\hoge\\hoge.txt -> hoge.txt
	static string basename(const string &fullpath);
	//ファイル名だけ(拡張子なし)を取得する  c:\\hoge\\hoge.txt -> hoge
	static string basenameonly(const string &fullpath);
	//先頭から a と b の同一部分の文字数を返す
	static int strmatchpos(const string& a,const string& b);
	//フルパスかどうか
	static bool isfullpath(const string& dir,const string& pathsep = "");
	//相対パスから絶対パスに変換する
	static string pathcombine(const string& base,const string& dir,const string& pathsep = "");
	static string urlcombine(const string& base,const string& dir);

	//パスの区切り文字を平らにする.
	static string pathseparator(const string& path,const string& pathsep = "");
	//URLパラメーターの追加.
	static string AppendURLParam(const string& url,const string& append);


	//inTarget の inStart ～ inEnd までを取得
	static string cut(const string &inTarget , const string & inStart , const string &inEnd , string * outNext = NULL);

	//コマンドライン引数パース
	static list<string> parse_command(const string & command);

	//bigramによる文字列分割
	static list<string> makebigram(const string & words);

	//指定した幅で丸める
	static string strimwidth(const string &  str , int startMoji , int widthMoji ,const string& trimmarker);

	//マルチバイト対応 ダブルクウォート
	static string doublequote(const string& str);
	//非マルチバイトのダブルクウォート
	static string doublequote_low(const string& str);
	//quoteをはがす
	static string dequote(const string& str);

	//重複削除
	static list<string> unique(const list<string>& list);
	//マルチバイト対応 inOldにマッチしたものがあったら消します
	static string remove(const string &inTarget ,const string &inOld );
	//マルチバイト対応 複数の候補を一括置換 const char * replacetable[] = { "A","あ"  ,"I","い"  , "上","うえ"  , NULL , NULL}  //必ず2つ揃えで書いてね
	static string replace(const string &inTarget ,const char** replacetable,bool isrev = false);
	static string replace_low(const string &inTarget ,const char** replacetable,bool isrev);

	//remove 複数の候補を一括削除  const char * replacetable[] = {"A","B","あ","うえお" , NULL}
	static string remove(const string &inTarget ,const char** replacetable);
	//typo修正
	//r	 「ローマ字」を「ひらがな」に変換します。
	//R	 「ひらがな」を「ローマ字」に変換します。
	//k	 「かな入力typo」を「ひらがな」に変換します。
	//K	 「ひらがな」を「かな入力typo」に変換します。
	static string mb_convert_typo(const string &inTarget,const string& option);
	//みんな大好き PHPのmb_convert_kanaの移植
	//n	 「全角」数字を「半角」に変換します。
	//N	 「半角」数字を「全角」に変換します。
	//a	 「全角」英数字を「半角」に変換します。
	//A	 「半角」英数字を「全角」に変換します 
	//s	 「全角」スペースを「半角」に変換します（U+3000 -> U+0020）。
	//S	 「半角」スペースを「全角」に変換します（U+0020 -> U+3000）。
	//k	 「全角カタカナ」を「半角カタカナ」に変換します。
	//K	 「半角カタカナ」を「全角カタカナ」に変換します。
	//h	 「全角ひらがな」を「半角カタカナ」に変換します。
	//H	 「半角カタカナ」を「全角ひらがな」に変換します。
	//c	 「全角カタカナ」を「全角ひらがな」に変換します。
	//C	 「全角ひらがな」を「全角カタカナ」に変換します。
	static string mb_convert_kana(const string &inTarget,const string& option);
	//みんな大好きPHPのescapeshellarg
	static string escapeshellarg(const string &inStr);
	//みんな大好きPHPのescapeshellarg
	static string escapeshellarg_single(const string &inStr);
	//数字の桁数を求める
	static int getScaler(unsigned int num);

	//findfirstとかのワイルドカードを使った文字列比較
	static bool findFilter(const string& base,const string& filter);

	//文字列を時刻に変換
	static time_t strtotime(const string& time);
	//時刻を文字列に変換
	static string timetostr(const time_t& time);
	static string timetostr(const time_t& time,const string & format);
	static void timetoint(const time_t& time,int* outYear,int* outMouth,int* outDay,int* outHour,int* outMinute,int* outSecond);


	//md5を求める
	static string md5(const string & str);
	//sha1を求める
	static void sha1(const string & str,vector<unsigned int>* outVec);
	static string sha1(const string & str);

	static void hmac_sha1(const string & str,vector<unsigned int>* outVec);
	static string hmac_sha1(const string & str);

	//uuid
	static string uuid();

	//16進数dumpされた長い文字列を再びバイナリにする
	static bool X02HexsStringToBinary(const string & str,vector<char>* retVec);

	//ファイル名にイカれた文字が入っていないか確認する.
	static bool checkSafePath(const string& filename);
	//改行が入っていないこと
	static bool IsNotReturnCode(const string& name) ;
	//改行と=が入っていないこと
	static bool IsNotReturnAndEqualCode(const string& name) ;

	static string UnixTimeToHttpTime(const time_t& t );
	//Http時間を Unix時間に変換.
	static time_t HttpTimeToUnixTime(const string& date);

	static string between(const string& str ,const string& start , const string& end);
	static string between_replace(const string& str ,const string& start , const string& end ,const string& newString);
	static string template_if(const string& str ,const string& usestart , const string& useend ,const string& killstart , const string& killend);

	static unsigned mb_strlen(const string& str);
	static string mb_substr(const string& str,unsigned int start,unsigned int len);

	static void StringtoVector(vector<char>* vec,const string& str);

	static bool checkSingleBytes(const string& str);
	static bool checkNumlic(const string& str);
	static bool checkMailAddress(const string& mail);
	static bool checkIPAddressV4(const string& ip);


	static time_t LocalTimetoGMT(const time_t& t);
	static time_t GMTtoLocalTime(const time_t& t);

	static string mb_mime_header(const string& str );
	static string mailaddress_to_name(const string& mailaddress );
	static string mailaddress_to_domain(const string& mailaddress );

	static bool isnumlic(const string & str);
	static bool isnumlic16(const string & str);
	static bool isalpha(const string & str);
	static bool isalpnum(const string & str);
	static bool isAsciiString(const string & str);

	//簡易正規表現の展開
	static list<string> RegexToPlain(const string & str);
	//何個検索文字が含まれているかを返す
	static unsigned int strcount(const string &inTarget ,const string &inSearch);
	//const char** 配列(終端はNULL)から文字を探す。
	static const char* findConstCharTable(const char** table , const char* search);
	static const char* findConstCharTable(const char** table , const string& search){ return findConstCharTable(table,search.c_str()); };

	// http://hoghoge.com:123/dir/hoghgoe?aa=1 --> http://hoghoge.com:123 に変換する
	static string hosturl(const string& url);
	// http://hoghoge.com:123/dir/hoghgoe?aa=1 --> http://hoghoge.com:123/dir/ に変換する
	static string baseurl(const string& url);
	// http://hoghoge.com:123/dir/hoghgoe?aa=1 --> https://hoghoge.com:123/dir/hoghgoe?aa=1 に変換する
	static string changeprotocol(const string& url,const string newProtocol);
	// http://hoghoge.com:123/dir/hoghgoe?aa=1 --> http://hoghoge.com:123/dir/hoghgoe?aa=1&bb=2 に変換する
	static string appendparam(const string& url,const string param);

	//mapどうしの比較
	static bool CompareStringMap(const map<string,string>& a ,const map<string,string>& b);

	//指定サイズのランダムな文字列を返す
	static string randsting(unsigned int size);

	//祝日判定
	static bool is_japan_holiday(const time_t& now,string *outName = NULL);

	static string DayofweekToJapanese(int dayofweek);
	static string DayofweekToEnglish(int dayofweek);
	static string MonthToJapanese(int month);
	static string MonthToEnglish(int month);

	//今いるタグの中を取得する.
	static string InnerHTML(const string& html);
	//今いるタグのテキストを取得する.
	static string InnerText(const string& html);
	//属性を取得する
	static string HTMLAttr(const string& html,const string& attributeName);
	//すーぱーHTMLセレクター
	static vector<string> HTMLSelector(const string& html,const string& selector);

	//文字コード変換
	static string mb_convert_encoding(const string& str , const string& to , const string& from = "");
	static string mb_convert_utf8(const string& str , const string& from = "");
	static string mb_detect_encoding(const string& str,const string& def = "ASCII");

	//正規表現でチェック
	static bool regex_check(const string& str , const string& regex );
	static bool regex_checkE(const string& str , const string& regex );

	//正規表現でマッチしたものを取得
	static bool regex_match(const string& str , const string& regex,vector<string>* retArray);
	static bool regex_matchE(const string& str , const string& regex,vector<string>* retArray);

	//日や月などの特殊な読み方を行います。
	static string nichi2yomi(const string& numyomi,const string& type) ;
	//数字を読み上げます
	static string num2yomi(const string& str) ;
	//小数か数字を読み上げます。
	static string double2yomi(const string& str) ;

	static bool IsTimeOver(const struct tm *date,int hour ,int minute);
	static bool IsTimeOver(const struct tm *date,int start_hour ,int start_minute,int end_hour ,int end_minute);

	// \U123F などのunicode表記のテキストを元のテキストに変換する. 
	static string ConvertTextUnicodeUToBinary(const string& str);

	// 配列中に文字列があるか検索
	static const char* strmap(const char* src,const char* map[]);

	//マルチバイト対応 NGRAM で結果を文字列で返す
	static string mb_ngram_string(const string& str,int ngram,const string& sep);

	//空白を読み飛ばす
	static string spaceSkip(const char* n,const char** outNext);
	static string alpnumSkip(const char* n,const char** outNext);
	static string numberSkip(const char* n,const char** outNext);
	//文字列区クウォートを読み飛ばす
	static string stringQuoteSkip(const char* n,const char** outNext,char quote);
	//kakoを読み飛ばす
	static string blaketQuoteSkip(const char* n,const char** outNext,char kakoStart,char kakoEnd);
};

#endif // !defined(AFX_XLSTRINGUTIL_H__B1B80C81_45F8_4E25_9AD6_FA9AC57294F5__INCLUDED_)
