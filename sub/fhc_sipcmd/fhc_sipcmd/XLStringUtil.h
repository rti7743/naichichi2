// XLStringUtil.h: XLStringUtil クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XLSTRINGUTIL_H__B1B80C81_45F8_4E25_9AD6_FA9AC57294F5__INCLUDED_)
#define AFX_XLSTRINGUTIL_H__B1B80C81_45F8_4E25_9AD6_FA9AC57294F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class XLStringUtil  
{
public:
	XLStringUtil();
	virtual ~XLStringUtil();


	//みんな大好きPHPのstrtoupper
	static std::string strtoupper(const std::string & str);

	//みんな大好きPHPのstrtolower
	static std::string strtolower(const std::string & str);

	static int atoi(const std::string & str);

	//HTTPヘッダのキャメル
	static std::string HeaderUpperCamel(const std::string & str);

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
	static const char* strstr(const std::string& target, const std::string & need );
	//strstrのマルチバイトセーフ 文字列検索
	static const char* strstr(const char* target, const char* need );
	//strstrのマルチバイトセーフ 文字列検索 //若干非効率
	static const char* strrstr(const std::string& target, const std::string & need );
	//strstrのマルチバイトセーフ 文字列検索 //若干非効率
	static const char* strrstr(const char* target, const char* need );
	//stristrのマルチバイトセーフ 大文字小文字関係なしの検索
	static const char* stristr(const std::string& target, const std::string & need );
	//strchrのマルチバイトセーフ 文字列の最初ら検索して最初見に使った文字の位置
	static const char* strchr(const std::string& target, char need );
	//strchrのマルチバイトセーフ 文字列の最初ら検索して最初見に使った文字の位置
	static const char* strchr(const char* target, char need );
	//strrchrのマルチバイトセーフ 文字列の後ろから検索して最初見に使った文字の位置
	static const char* strrchr(const std::string& target, char need );
	//strrchrのマルチバイトセーフ 文字列の後ろから検索して最初見に使った文字の位置
	static const char* strrchr(const char* target, char need );
	//strposのマルチバイトセーフ (strposはPHPにアルやつね)
	//最初に見つかった場所。見つからなければ -1 を返します。
	static int strpos(const std::string& target, const std::string & need );
	//strposのマルチバイトセーフ (strposはPHPにアルやつね)
	//最初に見つかった場所。見つからなければ -1 を返します。
	static int strpos(const char* target, const char* need );
	//strrposのマルチバイトセーフ (strposはPHPにアルやつね)
	//逆から検索して最初に見つかった場所。見つからなければ -1 を返します。
	static int strrpos(const std::string& target, const std::string & need );
	//strrposのマルチバイトセーフ (strposはPHPにアルやつね)
	//逆から検索して最初に見つかった場所。見つからなければ -1 を返します。
	static int strrpos(const char* target, const char* need );
	//striposのマルチバイトセーフ (striposはPHPにアルやつね)
	//大文字小文字関係なしで検索して最初に見つかった場所。見つからなければ -1 を返します。
	static int stripos(const std::string& target, const std::string & need );

	//みんな大好きPHPのjoin
	static std::string join(const std::string& glue , const std::map<std::string,std::string> & pieces );
	static std::string join(const std::string& glue , const std::list<std::string> & pieces );
	static std::string join(const std::string& glue , const std::vector<int> & pieces );

	//template って知ってるけど、とりあずこれで。
	static std::string join(const std::string& glue , const std::vector<std::string> & pieces );

	//key=value& みたいな感じの join
	static std::string crossjoin(const std::string& glue1 ,const std::string& glue2 , const std::map<std::string,std::string> & pieces );
	//split
	static std::list<std::string> split(const std::string& glue, const std::string & inTarget );
	//vector
	static std::vector<std::string> split_vector(const std::string& glue, const std::string & inTarget );
	static std::pair<std::string,std::string> split_two(const std::string& glue, const std::string & inTarget );
	static bool split_two(const std::string& glue, const std::string & inTarget , std::string* first,std::string* second );
	static bool split_two(const std::string& glue, const std::string & inTarget , std::pair<std::string,std::string>* ppp );

	//key=value& みたいな感じの split
	static std::map<std::string,std::string> crosssplit(const std::string& glue1 ,const std::string& glue2 , const std::string & inTarget );
	//key=value& みたいな感じの split キーに対するchopを実行する
	static std::map<std::string,std::string> crosssplitChop(const std::string& glue1 ,const std::string& glue2 , const std::string & inTarget );
	//stringmap 同士のマージ
	static std::map<std::string,std::string> merge(const std::map<std::string,std::string>& a ,const std::map<std::string,std::string>& b , bool overideB );
	static std::list<std::string> merge(const std::list<std::string>& a ,const std::list<std::string>& b );

	//みんな大好きPHPのurldecode
	static std::string urldecode(const std::string & inUrl);
	static std::string urlencode(const std::string &str) ;
	static std::string URLENCODE(const std::string &str) ;

	//base64エンコード
	static std::string base64encode(const std::string& src) ;
	//base64エンコード
	static std::string base64encode(const char* src,int len) ;
	//base64デコード
	static std::string base64decode(const std::string& src) ;
	//base64デコード
	static void base64decode(const std::string& src ,std::vector<char>* out) ;
	//巡回して、関数 func を適応。 true を返したものだけを結合する。
	static std::string string_filter(const std::list<std::string>& list,const std::function<bool(const std::string&)>& func);
	//巡回して、関数 func を適応。 funcの戻り文字列を結合します。
	static std::string string_map(const std::list<std::string>& list,const std::function<std::string (const std::string&)>& func);
	//巡回して、関数 func を適応。 true を返したものだけを返す。
	static std::list<std::string> array_filter(const std::list<std::string>& list,const std::function<bool(const std::string&)>& func);
	//巡回して、関数 func を適応。 funcの戻りで配列作って返します。
	static std::list<std::string> array_map(const std::list<std::string>& list,const std::function<std::string (const std::string&)>& func);


	//みんな大好きPHPのchop 左右の空白の除去
	static std::string chop(const std::string & str,const char * replaceTable = NULL);
	static std::string trim(const std::string & str,const char * replaceTable = NULL)
	{
		return chop(str,replaceTable);
	}

	//なぜか std::string に標準で用意されていない置換。ふぁっく。
	static std::string replace(const std::string &inTarget ,const std::string &inOld ,const std::string &inNew);
	//マルチバイト非対応 の文字列置換
	static std::string replace_low(const std::string &inTarget ,const std::string &inOld ,const std::string &inNew);

	//みんな大好きPHPのhtmlspecialchars
	//タグをエスケープ 基本的に PHP の htmlspecialchars と同じ.
	//http://search.net-newbie.com/php/function.htmlspecialchars.html
	static std::string htmlspecialchars(const std::string &inStr);
	//マルチバイト非対応 タグをエスケープ
	static std::string htmlspecialchars_low(const std::string &inStr);

	//みんな大好きPHPのnl2br
	//\nを<br>に 基本的に PHP の nl2br と同じ.
	static std::string nl2br(const std::string &inStr);
	//マルチバイト非対応 \nを<br>に
	static std::string nl2br_low(const std::string &inStr);
	
	//jsonで利用できる文字列表記にする
	static std::string jsonescape(const std::string & str);
	//jsonのエスケープ表記を元に戻す
	static std::string jsonunescape(const std::string & str);
	//jsonに追加できる文字列として返す
	static std::string jsonvalue(const std::string & name,const std::string & str);
	//制御文字を飛ばす
	static std::string clearcontrollcode(const std::string & str);
	//jsonをmap<sting,string>に変換
	static std::map<std::string,std::string> parsejson(const std::string & json);
	//json文字列の中から、キーに対するvalueを抜く.
	static std::string findjsonvalue(const std::string & json,const std::string & key);


	//拡張子を取得する. abc.cpp -> ".cpp" のような感じになるよ
	static std::string baseext(const std::string &fullpath);
	//拡張子を取得する. abc.cpp -> "cpp" のような感じになるよ . をつけない
	static std::string baseext_nodot(const std::string &fullpath);
	//拡張子を取得する. abc.Cpp -> "cpp" のような感じになるよ . をつけないで小文字
	static std::string baseext_nodotsmall(const std::string &fullpath);

	//ベースディレクトリを取得する  c:\\hoge\\hoge.txt -> c:\\hoge にする  最後の\\ は消える。
	static std::string basedir(const std::string &fullpath);
	//ファイル名を取得する  c:\\hoge\\hoge.txt -> hoge.txt
	static std::string basename(const std::string &fullpath);
	//ファイル名だけ(拡張子なし)を取得する  c:\\hoge\\hoge.txt -> hoge
	static std::string basenameonly(const std::string &fullpath);
	//先頭から a と b の同一部分の文字数を返す
	static int strmatchpos(const std::string& a,const std::string& b);
	//フルパスかどうか
	static bool isfullpath(const std::string& dir,const std::string& pathsep = "");
	//相対パスから絶対パスに変換する
	static std::string pathcombine(const std::string& base,const std::string& dir,const std::string& pathsep = "");
	static std::string urlcombine(const std::string& base,const std::string& dir);

	//パスの区切り文字を平らにする.
	static std::string pathseparator(const std::string& path,const std::string& pathsep = "");
	//URLパラメーターの追加.
	static std::string AppendURLParam(const std::string& url,const std::string& append);


	//inTarget の inStart ～ inEnd までを取得
	static std::string cut(const std::string &inTarget , const std::string & inStart , const std::string &inEnd , std::string * outNext = NULL);

	//コマンドライン引数パース
	static std::list<std::string> parse_command(const std::string & command);

	//bigramによる文字列分割
	static std::list<std::string> makebigram(const std::string & words);

	//指定した幅で丸める
	static std::string strimwidth(const std::string &  str , int startMoji , int widthMoji ,const std::string& trimmarker);

	//マルチバイト対応 ダブルクウォート
	static std::string doublequote(const std::string& str);
	//非マルチバイトのダブルクウォート
	static std::string doublequote_low(const std::string& str);
	//quoteをはがす
	static std::string dequote(const std::string& str);

	//重複削除
	static std::list<std::string> unique(const std::list<std::string>& list);
	//マルチバイト対応 inOldにマッチしたものがあったら消します
	static std::string remove(const std::string &inTarget ,const std::string &inOld );
	//マルチバイト対応 複数の候補を一括置換 const char * replacetable[] = { "A","あ"  ,"I","い"  , "上","うえ"  , NULL , NULL}  //必ず2つ揃えで書いてね
	static std::string replace(const std::string &inTarget ,const char** replacetable,bool isrev = false);
	static std::string replace_low(const std::string &inTarget ,const char** replacetable,bool isrev);

	//remove 複数の候補を一括削除  const char * replacetable[] = {"A","B","あ","うえお" , NULL}
	static std::string remove(const std::string &inTarget ,const char** replacetable);
	//typo修正
	//r	 「ローマ字」を「ひらがな」に変換します。
	//R	 「ひらがな」を「ローマ字」に変換します。
	//k	 「かな入力typo」を「ひらがな」に変換します。
	//K	 「ひらがな」を「かな入力typo」に変換します。
	static std::string mb_convert_typo(const std::string &inTarget,const std::string& option);
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
	static std::string mb_convert_kana(const std::string &inTarget,const std::string& option);
	//みんな大好きPHPのescapeshellarg
	static std::string escapeshellarg(const std::string &inStr);
	//みんな大好きPHPのescapeshellarg
	static std::string escapeshellarg_single(const std::string &inStr);
	//数字の桁数を求める
	static int getScaler(unsigned int num);

	//findfirstとかのワイルドカードを使った文字列比較
	static bool findFilter(const std::string& base,const std::string& filter);

	//文字列を時刻に変換
	static time_t strtotime(const std::string& time);
	//時刻を文字列に変換
	static std::string timetostr(const time_t& time);
	static std::string timetostr(const time_t& time,const std::string & format);
	static void timetoint(const time_t& time,int* outYear,int* outMouth,int* outDay,int* outHour,int* outMinute,int* outSecond);


	//md5を求める
	static std::string md5(const std::string & str);
	//sha1を求める
	static void sha1(const std::string & str,std::vector<unsigned int>* outVec);
	static std::string sha1(const std::string & str);

	static void hmac_sha1(const std::string & str,std::vector<unsigned int>* outVec);
	static std::string hmac_sha1(const std::string & str);

	//uuid
	static std::string uuid();

	//16進数dumpされた長い文字列を再びバイナリにする
	static bool X02HexsStringToBinary(const std::string & str,std::vector<char>* retVec);

	//ファイル名にイカれた文字が入っていないか確認する.
	static bool checkSafePath(const std::string& filename);
	//改行が入っていないこと
	static bool IsNotReturnCode(const std::string& name) ;
	//改行と=が入っていないこと
	static bool IsNotReturnAndEqualCode(const std::string& name) ;

	static std::string UnixTimeToHttpTime(const time_t& t );
	//Http時間を Unix時間に変換.
	static time_t HttpTimeToUnixTime(const std::string& date);

	static std::string between(const std::string& str ,const std::string& start , const std::string& end);
	static std::string between_replace(const std::string& str ,const std::string& start , const std::string& end ,const std::string& newString);
	static std::string template_if(const std::string& str ,const std::string& usestart , const std::string& useend ,const std::string& killstart , const std::string& killend);

	static unsigned mb_strlen(const std::string& str);
	static std::string mb_substr(const std::string& str,unsigned int start,unsigned int len);

	static void StringtoVector(std::vector<char>* vec,const std::string& str);

	static bool checkSingleBytes(const std::string& str);
	static bool checkNumlic(const std::string& str);
	static bool checkMailAddress(const std::string& mail);
	static bool checkIPAddressV4(const std::string& ip);


	static time_t LocalTimetoGMT(const time_t& t);
	static time_t GMTtoLocalTime(const time_t& t);

	static std::string mb_mime_header(const std::string& str );
	static std::string mailaddress_to_name(const std::string& mailaddress );
	static std::string mailaddress_to_domain(const std::string& mailaddress );

	static bool isnumlic(const std::string & str);
	static bool isnumlic16(const std::string & str);
	static bool isalpha(const std::string & str);
	static bool isalpnum(const std::string & str);
	static bool isAsciiString(const std::string & str);

	//簡易正規表現の展開
	static std::list<std::string> RegexToPlain(const std::string & str);
	//何個検索文字が含まれているかを返す
	static unsigned int strcount(const std::string &inTarget ,const std::string &inSearch);
	//const char** 配列(終端はNULL)から文字を探す。
	static const char* findConstCharTable(const char** table , const char* search);
	static const char* findConstCharTable(const char** table , const std::string& search){ return findConstCharTable(table,search.c_str()); };

	// http://hoghoge.com:123/dir/hoghgoe?aa=1 --> http://hoghoge.com:123 に変換する
	static std::string hosturl(const std::string& url);
	// http://hoghoge.com:123/dir/hoghgoe?aa=1 --> http://hoghoge.com:123/dir/ に変換する
	static std::string baseurl(const std::string& url);
	// http://hoghoge.com:123/dir/hoghgoe?aa=1 --> https://hoghoge.com:123/dir/hoghgoe?aa=1 に変換する
	static std::string changeprotocol(const std::string& url,const std::string newProtocol);
	// http://hoghoge.com:123/dir/hoghgoe?aa=1 --> http://hoghoge.com:123/dir/hoghgoe?aa=1&bb=2 に変換する
	static std::string appendparam(const std::string& url,const std::string param);

	//std::mapどうしの比較
	static bool CompareStringMap(const std::map<std::string,std::string>& a ,const std::map<std::string,std::string>& b);

	//指定サイズのランダムな文字列を返す
	static std::string randsting(unsigned int size);

	//祝日判定
	static bool is_japan_holiday(const time_t& now,std::string *outName = NULL);

	static std::string DayofweekToJapanese(int dayofweek);
	static std::string DayofweekToEnglish(int dayofweek);
	static std::string MonthToJapanese(int month);
	static std::string MonthToEnglish(int month);

	//今いるタグの中を取得する.
	static std::string InnerHTML(const std::string& html);
	//今いるタグのテキストを取得する.
	static std::string InnerText(const std::string& html);
	//属性を取得する
	static std::string HTMLAttr(const std::string& html,const std::string& attributeName);
	//すーぱーHTMLセレクター
	static std::vector<std::string> HTMLSelector(const std::string& html,const std::string& selector);

	//文字コード変換
	static std::string mb_convert_encoding(const std::string& str , const std::string& to , const std::string& from = "");
	static std::string mb_convert_utf8(const std::string& str , const std::string& from = "");
	static std::string mb_detect_encoding(const std::string& str,const std::string& def = "ASCII");

	//正規表現でチェック
	static bool regex_check(const std::string& str , const std::string& regex );
	static bool regex_checkE(const std::string& str , const std::string& regex );

	//正規表現でマッチしたものを取得
	static bool regex_match(const std::string& str , const std::string& regex,std::vector<std::string>* retArray);
	static bool regex_matchE(const std::string& str , const std::string& regex,std::vector<std::string>* retArray);

	//日や月などの特殊な読み方を行います。
	static std::string nichi2yomi(const std::string& numyomi,const std::string& type) ;
	//数字を読み上げます
	static std::string num2yomi(const std::string& str) ;
	//小数か数字を読み上げます。
	static std::string double2yomi(const std::string& str) ;

	static bool IsTimeOver(const struct tm *date,int hour ,int minute);
	static bool IsTimeOver(const struct tm *date,int start_hour ,int start_minute,int end_hour ,int end_minute);

	// \U123F などのunicode表記のテキストを元のテキストに変換する. 
	static std::string ConvertTextUnicodeUToBinary(const std::string& str);

	// 配列中に文字列があるか検索
	static const char* strmap(const char* src,const char* map[]);

	//マルチバイト対応 NGRAM で結果を文字列で返す
	static std::string mb_ngram_string(const std::string& str,int ngram,const std::string& sep);

	//空白を読み飛ばす
	static std::string spaceSkip(const char* n,const char** outNext);
	static std::string alpnumSkip(const char* n,const char** outNext);
	static std::string numberSkip(const char* n,const char** outNext);
	//文字列区クウォートを読み飛ばす
	static std::string stringQuoteSkip(const char* n,const char** outNext,char quote);
	//kakoを読み飛ばす
	static std::string blaketQuoteSkip(const char* n,const char** outNext,char kakoStart,char kakoEnd);
};

#endif // !defined(AFX_XLSTRINGUTIL_H__B1B80C81_45F8_4E25_9AD6_FA9AC57294F5__INCLUDED_)
