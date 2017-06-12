//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#include"common.h"
#include"JuliusOnsoUtil.h"
#include"MainWindow.h"
#include"ScriptRunner.h"
#include"XLStringUtil.h"
#include "XLFileUtil.h"

//CMUの辞書から英語Juliusの音素に変換します.
string JuliusOnsoUtil::EnglishToEnglishOnso(const string& englishOnso,bool isrev)
{
	//AH0 とか AO2 とかそういうのをすべて統一します.
	const char * replaceTablePlain[] = {
	 "0",""
	,"1",""
	,"2",""
	,NULL,NULL};

	string w =" " + XLStringUtil::replace(englishOnso, replaceTablePlain) +" ";
	w = XLStringUtil::strtolower(w);

	return w;
}

string JuliusOnsoUtil::JapaneseToEnglishOnso(const string& japaneseOnso,bool isrev)
{
	//カタカナひらがな変換
	string w = MainWindow::m()->MecabControl.KanjiAndKanakanaToHiragana(japaneseOnso);

	const char * replaceTable1[] = {
	//特殊ルール
	 "え゛","えっ"
	,"こんにちは","こんにちわ"
	,"こんばんは","こんばんわ"
	,"わたしは","わたしわ"
	,"ぼくは","ぼくわ"
	,"かれは","かれわ"
	,"かれらは","かれらわ"
	,"かのじょは","かのじょわ"
	,"これらは","これらわ"
	,"あれは","あれわ"
	////,"なのは","なのわ" //NLPの最大の課題、リリカルなのは問題によりダメです。
	,NULL,NULL};
	w = XLStringUtil::replace(w, replaceTable1);

	//yomi2vocaより
	//# 3文字以上からなる変換規則（v a）
	const char * replaceTable2[] = {
	 "う゛ぁ"," b ah"
	,"う゛ぃ"," b iy"
	,"う゛ぇ"," b eh"
	,"う゛ぉ"," b ow"
	,"う゛ゅ"," b uh"
	,"きゅー"," k y uw"
	,"にゅー"," n y uw"
	,"みゅー"," m y uw"
	,"じゅー"," jh uw"
	,"ぴゅー"," p y uw"
	,NULL,NULL};
	w = XLStringUtil::replace(w, replaceTable2);

	//# 2文字からなる変換規則
	const char * replaceTable3[] = {
     "ぅ゛"," b uw"
	,"あぁ"," ah"
	,"あー"," ah"
	,"あっ"," ae"
	,"いっ"," ih"
	,"いぃ"," ih"
	,"いー"," ih"
    ,"いぇ"," ih"
	,"いゃ"," y aa"
	,"うっ"," w uh"
	,"うぅ"," uw"
	,"うー"," uw"
	,"えっ"," eh"
	,"えぇ"," er"
	,"えー"," er"
	,"おっ"," ow"
	,"おぉ"," ow"
	,"おー"," ow"
	,"おる"," aa l"
	,"おーる"," aa l"
	,"かっ"," k ah"
	,"かぁ"," k aa r"
	,"かー"," k aa r"
	,"きっ"," k ih"
	,"きぃ"," k iy"
	,"きー"," k iy"
	,"くあ"," k uw ae"
	,"くっ"," k uh"
	,"くぅ"," k uw"
	,"くゃ"," k y aa"
	,"くゅ"," k y uh"
	,"くょ"," k oh"
	,"くー"," k uw"
	,"けっ"," k ih"
	,"けぇ"," k iy"
	,"けー"," k iy"
	,"こっ"," k ow"
	,"こぉ"," k ow"
	,"こー"," k ow"
	,"がっ"," g ah"
	,"がぁ"," g aa"
	,"がー"," g aa"
	,"ぎっ"," g ih"
	,"ぎぃ"," g iy"
	,"ぎー"," g iy"
	,"ぐっ"," g uw"
	,"ぐぅ"," g uw"
	,"ぐゃ"," g y aa"
	,"ぐゅ"," g y uh"
	,"ぐょ"," g y ow"
	,"ぐー"," g uw"
	,"げっ"," g eh"
	,"げぇ"," g eh"
	,"げー"," g eh"
	,"ごっ"," g aa"
	,"ごぉ"," g oh"
	,"ごー"," g ow"
	,"さっ"," s ah"
	,"さぁ"," s er"
	,"さー"," s er"
	,"しっ"," s ih"
	,"しぃ"," s iy"
	,"しー"," s iy"
	,"すっ"," s uh"
	,"すぅ"," s uw"
	,"すゃ"," sh aa"
	,"すゅ"," s hh ah"
	,"すょ"," sh ow"
	,"すー"," s uw"
	,"せっ"," s eh"
	,"せぇ"," s ih"
	,"せー"," s ih"
	,"そっ"," s aa"
	,"そぉ"," s ow"
	,"そー"," s ow"
	,"ざっ"," z aa"
	,"ざぁ"," z ah"
	,"ざー"," z aw"
	,"じっ"," z ih"
	,"じぃ"," z iy"
	,"じー"," z iy"
	,"ずっ"," z uh"
	,"ずぅ"," z uh"
	,"ずゃ"," jh ae"
	,"ずゅ"," jh uh"
	,"ずょ"," jh oy"
	,"ずー"," z uw"
	,"ぜっ"," z eh"
	,"ぜぇ"," z eh"
	,"ぜー"," z eh"
	,"ぞっ"," z ow"
	,"ぞぉ"," z ow"
	,"ぞー"," z ow"
	,"たっ"," t ae"
	,"たぁ"," t ah"
	,"たー"," t er"
	,"ちっ"," ch ih"
	,"ちぃ"," ch ih"
	,"ちー"," ch ih"
	,"つっ"," t ae"
	,"つぁ"," t ah"
	,"つぃ"," t w iy"
	,"つぅ"," t uw"
	,"つゃ"," ch ae"
	,"つゅ"," ch uh"
	,"つょ"," ch ow"
	,"つぇ"," ch ah"
	,"つぉ"," ch ow"
	,"つー"," t uw"
	,"てっ"," t eh"
	,"てぇ"," t ey"
	,"てー"," t ey"
	,"とっ"," t oy"
	,"とぉ"," t oy"
	,"とー"," t oy"
	,"だっ"," d eh"
	,"だぁ"," d er"
	,"だー"," d er"
	,"ぢっ"," z ih"
	,"ぢぃ"," z iy"
	,"ぢー"," z iy"
	,"づっ"," z uh"
	,"づぅ"," z uh"
	,"づゃ"," jh ae"
	,"づゅ"," jh uh"
	,"づょ"," jh oy"
	,"づー"," z uw"
	,"でっ"," d eh"
	,"でぇ"," d ey"
	,"でー"," d ey"
	,"どっ"," d ow"
	,"どぉ"," d ow"
	,"どー"," d ow"
	,"なっ"," n ae"
	,"なぁ"," n ah"
	,"なー"," n aw"
	,"にっ"," n ih"
	,"にぃ"," n ey"
	,"にー"," n ey"
	,"ぬっ"," n ah"
	,"ぬぅ"," n ah"
	,"ぬゃ"," n y ah"
	,"ぬゅ"," n uh"
	,"ぬょ"," n y uh"
	,"ぬー"," n uw"
	,"ねっ"," n ey"
	,"ねぇ"," n er"
	,"ねー"," n er"
	,"のっ"," n aa"
	,"のぉ"," n ao"
	,"のー"," n ao"
	,"はっ"," hh aa"
	,"はぁ"," hh aw"
	,"はー"," hh aw"
	,"ひっ"," hh iy"
	,"ひぃ"," hh iy"
	,"ひー"," hh iy"
	,"ふっ"," f uw"
	,"ふぅ"," f uw"
	,"ふゃ"," f ah"
	,"ふゅ"," f y uw"
	,"ふょ"," f oy"
	,"ふー"," f uw"
	,"へっ"," hh eh"
	,"へぇ"," hh iy"
	,"へー"," hh iy"
	,"ほっ"," hh ow"
	,"ほぉ"," hh ow"
	,"ほー"," hh ow"
	,"ばっ"," b aa"
	,"ばぁ"," b aa"
	,"ばー"," b aa"
	,"びっ"," b ih"
	,"びぃ"," b ih"
	,"びー"," b iy"
	,"ぶっ"," b uw"
	,"ぶぅ"," b uw"
	,"ふゃ"," b ah"
	,"ぶゅ"," b y uh"
	,"ぶょ"," b oy"
	,"ぶー"," b uw"
	,"べっ"," b eh"
	,"べぇ"," b eh"
	,"べー"," b eh"
	,"ぼっ"," b ow"
	,"ぼぉ"," b ow"
	,"ぼー"," b ow"
	,"ぱっ"," p ah"
	,"ぱぁ"," p ah"
	,"ぱー"," p ah"
	,"ぴっ"," p iy"
	,"ぴぃ"," p iy"
	,"ぴー"," p iy"
	,"ぷっ"," p uh"
	,"ぷぅ"," p uh"
	,"ぷう"," p uw"
	,"ぷゃ"," p ah"
	,"ぷゅ"," p y"
	,"ぷょ"," p ow"
	,"ぷー"," p uw"
	,"ぺっ"," p eh"
	,"ぺぇ"," p eh"
	,"ぺー"," p eh"
	,"ぽっ"," p ow"
	,"ぽぉ"," p ow"
	,"ぽっ"," p ow"
	,"ぽー"," p ow"
	,"まっ"," m ah"
	,"まぁ"," m ah"
	,"まー"," m ah"
	,"みっ"," m iy"
	,"みぃ"," m iy"
	,"みー"," m iy"
	,"むっ"," m uh"
	,"むぅ"," m uw"
	,"むゃ"," m ih"
	,"むゅ"," m uw"
	,"むょ"," m ow"
	,"むー"," m uw"
	,"めっ"," m eh"
	,"めぇ"," m eh"
	,"めー"," m eh"
	,"もっ"," m ow"
	,"もぉ"," m ow"
	,"もー"," m ow"
	,"やっ"," y ah"
	,"やぁ"," y ah"
	,"やー"," y ah"
	,"ゆっ"," y uh"
	,"ゆぅ"," y uh"
	,"ゆゃ"," y ah"
	,"ゆゅ"," y uh"
	,"ゆょ"," y ow"
	,"ゆー"," y uh"
	,"よっ"," y oy"
	,"よぉ"," y ow"
	,"よー"," y ow"
	,"らっ"," r ah"
	,"らぁ"," r aa"
	,"らー"," r er"
	,"りっ"," r ih"
	,"りぃ"," r iy"
	,"りー"," r iy"
	,"るっ"," r uh"
	,"るぅ"," r uh"
	,"るゃ"," r y ah"
	,"るゅ"," r uw"
	,"るょ"," r y ow"
	,"るぅ"," r uw"
	,"れっ"," l ah"
	,"れぇ"," l eh"
	,"れー"," l eh"
	,"ろっ"," r ow"
	,"ろぉ"," r ow"
	,"ろー"," r ow"
	,"わっ"," w ah"
	,"わぁ"," w aw"
	,"わー"," w aw"
	,"をっ"," ow"
	,"をぉ"," ow"
	,"をー"," ow"
	,"う゛"," b uw"
	,"でっ"," d iy"
	,"でぃ"," d iy"
	,"でぇ"," d ey"
	,"でゃ"," d y ih"
	,"でゅ"," d y uh"
	,"でょ"," d y ow"
	,"でー"," d ey"
	,"てぃ"," t iy"
	,"てぇ"," t eh"
	,"てゃ"," t y ah"
	,"てゅ"," t y uh"
	,"てょ"," t y ow"
	,"てー"," t iy"
	,"すぃ"," s iy"
	,"すー"," s iy"
	,"ずぁ"," z uh"
	,"ずぃ"," z iy"
	,"ずぅ"," z uw"
	,"ずゃ"," z y ah"
	,"ずゅ"," z y uw"
	,"ずょ"," z y ow"
	,"ずぇ"," z eh"
	,"ずぉ"," z ow"
	,"ずっ"," z uw"
	,"ずー"," z uw"
	,"きゃ"," k y ah"
	,"きゅ"," k y uh"
	,"きょ"," k y ow"
	,"きー"," k iy"
	,"しゃ"," sh ah"
	,"しゅ"," sh uh"
	,"しぇ"," sh eh"
	,"しょ"," sh ow"
	,"しー"," sh iy"
	,"ちゃ"," ch ah"
	,"ちゅ"," ch uh"
	,"ちぇ"," ch eh"
	,"ちょ"," ch ow"
	,"とぅ"," t uw"
	,"とう"," t uy"
	,"とゃ"," t y ah"
	,"とゅ"," t y uh"
	,"とょ"," t y ow"
	,"とー"," t uw"
	,"どぁ"," d ah"
	,"どぅ"," d uw"
	,"どゃ"," d y ah"
	,"どゅ"," d y uw"
	,"どょ"," d y ow"
	,"どぉ"," d ow"
	,"どー"," d uw"
	,"にゃ"," n y ah"
	,"にゅ"," n y uh"
	,"にょ"," n y ow"
	,"にー"," n iy"
	,"ひゃ"," hh y ah"
	,"ひゅ"," hh y uw"
	,"ひょ"," hh y ow"
	,"ひー"," hh iy"
	,"みゃ"," m y ah"
	,"みゅ"," m y uh"
	,"みょ"," m y oh"
	,"みぃ"," m iy"
	,"みー"," m iy"
	,"りゃ"," r y ah"
	,"りゅ"," r y uw"
	,"りょ"," r y ow"
	,"ぎゃ"," g y ah"
	,"ぎゅ"," g y uw"
	,"ぎょ"," g y ow"
	,"ぢぇ"," jh eh"
	,"ぢゃ"," jh ah"
	,"ぢゅ"," jh uh"
	,"ぢょ"," jh ow"
	,"ぢー"," jh uw"
	,"じぇ"," jh eh"
	,"じゃ"," jh ae"
	,"じゅ"," jh uh"
	,"じょ"," jh aa"
	,"じぃ"," jh iy"
	,"じー"," jh iy"
	,"びゃ"," b y ah"
	,"びゅ"," b y uw"
	,"びょ"," b y ow"
	,"びぃ"," b iy"
	,"びー"," b iy"
	,"ぴゃ"," p y ah"
	,"ぴゅ"," p y uh"
	,"ぴょ"," p y oh"
	,"ぴぃ"," p iy"
	,"ぴー"," p iy"
	,"うぁ"," w aw"
	,"うぃ"," w eh"
	,"うぇ"," w eh"
	,"うぉ"," w ow"
	,"ふぁ"," f ah"
	,"ふぃ"," f iy"
	,"ふぅ"," f uw"
	,"ふゃ"," f y ae"
	,"ふゅ"," f y uw"
	,"ふょ"," f y ow"
	,"ふぇ"," f eh"
	,"ふぉ"," f ow"
	,NULL,NULL};
	w = XLStringUtil::replace(w, replaceTable3);


	//# 1音からなる変換規則
	const char * replaceTable4[] = {
	 "あ"," aa"
	,"い"," iy"
	,"う"," uw"
	,"え"," eh"
	,"お"," ow"
	,"か"," k ah"
	,"き"," k iy"
	,"く"," k uh"
	,"け"," k eh"
	,"こ"," k ow"
	,"さ"," s ah"
	,"し"," sh iy"
	,"す"," s uw"
	,"せ"," s eh"
	,"そ"," s ao"
	,"た"," t ah"
	,"ち"," ch ih"
	,"つ"," t uw"
	,"て"," t eh"
	,"と"," t oy"
	,"な"," n ay"
	,"に"," n iy"
	,"ぬ"," n ah"
	,"ね"," n ey"
	,"の"," n ow"
	,"は"," hh ah"
	,"ひ"," hh iy"
	,"ふ"," f uw"
	,"へ"," hh eh"
	,"ほ"," hh ow"
	,"ま"," m ah"
	,"み"," m iy"
	,"む"," m uh"
	,"め"," m eh"
	,"も"," m ow"
	,"ら"," r ah"
	,"り"," r iy"
	,"る"," l"
	,"れ"," r eh"
	,"ろ"," r ow"
	,"が"," g aa"
	,"ぎ"," g iy"
	,"ぐ"," g uw"
	,"げ"," g eh"
	,"ご"," g ow"
	,"ざ"," z ah"
	,"じ"," z iy"
	,"ず"," z uh"
	,"ぜ"," z eh"
	,"ぞ"," z ow"
	,"だ"," d ah"
	,"ぢ"," z iy"
	,"づ"," z uh"
	,"で"," d eh"
	,"ど"," d ow"
	,"ば"," b ah"
	,"び"," b iy"
	,"ぶ"," b uw"
	,"べ"," b eh"
	,"ぼ"," b ow"
	,"ぱ"," p ah"
	,"ぴ"," p iy"
	,"ぷ"," p uh"
	,"ぺ"," p eh"
	,"ぽ"," p ow"
	,"や"," y ah"
	,"ゆ"," y uh"
	,"よ"," y oy"
	,"わ"," w ah"
	,"ゐ"," iy"
	,"ゑ"," eh"
	,"ん"," n"
	,"っ",""
	,"ー",""
	,"-",""
	,"ｰ",""
	,NULL,NULL};
	w = XLStringUtil::replace(w, replaceTable4);

	//# ここまでに処理されてない ぁぃぅぇぉ はそのまま大文字扱い
	const char * replaceTable5[] = {
	 "ぁ"," ah"
	,"ぃ"," iy"
	,"ぅ"," uh"
	,"ぇ"," eh"
	,"ぉ"," ow"
	,"ゎ"," w ah"
	,"ぉ"," ow"
	//# その他特別なルール
    ,"を"," ow"
	,NULL,NULL};
	w = XLStringUtil::replace(w, replaceTable5);

	const char * replaceTable8[] = {
	//句読点
	 "。",""
	,"、",""
	,".",""
	,",",""
	,"・",""
	,"「",""
	,"」",""
	,NULL,NULL};
	w = XLStringUtil::replace(w, replaceTable8);

	w = XLStringUtil::chop(w);
	return w;
}


//英語の発音から日本語読みを強引に取得します。
string JuliusOnsoUtil::EnglishOnsoToJapanese(const string& englishOnso,bool isrev)
{
	string w = XLStringUtil::strtoupper(englishOnso);

	//AH0 とか AO2 とかそういうのをすべて統一します.
	const char * replaceTablePlain[] = {
	 "0",""
	,"1",""
	,"2",""
	,NULL,NULL};

	w ="   " + XLStringUtil::replace(w, replaceTablePlain) +" ";
	const char * replaceTabl0[] = {
	  "   M AH","ま"
	 ,"   K IH","き"
	 ,"   AA N","おん"
	 ,NULL,NULL
	};
	w = XLStringUtil::replace(w, replaceTabl0,isrev);

	const char * replaceTabl1[] = {
	 " S T ER","すたー"
	," S T AA R","すたー"
	," L T IY","てぃ"
	," NG L IY","んぐりー"
	," M AH T","めいと"
	," M EY T","めいと"
	," T IH NG","しんぐ"
	," S AH L","せーる"
	," S AH N","せん"
	," AA T IY","あーりー"
	," G Y UW","ぎゅー"
	," D IH S","でぃす"
	," EH R IH","あり"
	," AA R IH","あーり"
	," M AH N","まん"
	," N AH L","なる"
	," S EH N","せん"
	," S K UW L","すくーる"
	," K UW L","くーる"
	," Y AA R","やー"
	," K R AE","くら"
	," G R AE","ぐら"
	," SH EH R","しぇあ"
	," R EH S","れす"
	," F AO R","ふぉー"
	," R EH JH","らっじ"
	," R EH TH","らーど"
	," R IH K","りっく"
	," R IH JH","れーじ"
	," R IH CH","りっち"
	," N AO R","なー"
	," P IH CH","ぴっち"
	," K W AH","こー"
	," W IH P","ういっぷ"
	," K AH M AE","こま"
	," K AH M","こん"
	," P Y UW","ぴゅー"
	," S IH NG","しん"
	," G AH L","ぐる"
	," K W OW","くぉー"
	," B AE K","ばっく"
	," L AY T","らいと"
	," L AY Z","らいず"
	," L AY K","らいく"
	," B AH L","ぶる"
	," AH L ER","ありー"
	," S EH L","せる"
	," B L AE","ぶらっ"
	," B L AO","ぶろー"
	," S L IY","すりー"
	," P L IY","ぷりー"
	," F L IY","ふりー"
	," L AH S","れす"
	," K AH L","かる"
	," W AO K ER","うぉーかー"
	," W AO K","うぉーく"
	," K IH NG","きんぐ"
	," K ER Z","かーず"
	," S L AY","すらい"
	," D IH NG Z","でぃんぐず"
	," D IH NG","でぃんぐ"
	," G ER IH NG","がーりんぐ"
	," T ER IH NG","とりんぐ"
	," ER IH NG","りんぐ"
	," D AH L","でる"
	," R IY AH","りあー"
	," N UW Z","ニュース"
	," IH NG K","いんぐ"
	," M Y UW","みゅー"
	," Z IH K","じっく"
	," S P AA","すぱ"
	," D AH N S IY","でんしー"
	," T AH N IY","てぃにー"
	," T R OY","とろい"
	," L UW AH","るあー"
	," L EH K S","れっくす"
	," EH V ER","えばー"
	," G R IY N","ぐりーん"
	," R IY N","りーん"
	," R AE S","らす"
	," SH AH N Z","しょんず"
	," SH AH N","しょん"
	," T IH V","てぃぶ"
	," D AW G","どっぐ"
	," B ER D","ばーど"
	," L EY S","れーす"
	," L EY N","れーん"
	," L EY T","れーと"
	," P AE M","ぱむ"
	," B AE K","ばっく"
	," OW K EY","おーけー"
	," S ER CH","さーち"
	," K T ER","くたー"
	," S T ER","すたー"
	," S T R AO","すとろ"
	," AE B S AH","あぶそ"
	," L UW T","りゅーと"
	," OW V ER","おーばー"
	," P AW ER","ぱわー"
	," T R IY","とぅりー"
	," B AE NG K","ばんく"
	," S IH T IY","してぃ"
	," B OW L","ぼーる"
	," F AE N","ふぁん"
	," HH Y UW","ひゅう"
	," T R AH","とら"
	," S T AA R","すたー"
	," S AH JH","せーじ"
	," EY SH AH N","せーしょん"
	," D Y ER","でぃ"
	," L AY V","らいぶ"
	," F AY ER","ふぁいあ"
	," W EH R","うぇあ"
	," W ER K","わーく"
	," M EY SH AH N","めーしょん"
	," P AA R T S","ぱーつ"
	," T AH M AA","とぅも"
	," P EH R","ぺあー"
	," P AA R","ぱー"
	," Z AY N","ざいん"
	," M IH NG","みんぐ"
	," W IH CH","いっち"
	," T AH M","てむ"
	," K OW P","こーぷ"
	," L T IY","てぃ"
	," R ER Z","やーど"
	," G Y UW","ぎゅー"
	," EH R IH","あり"
	," DH ER","ざー"
	," N D ER","んだー"
	," K S AH","くす"
	," K AH N","こっん"
	,NULL,NULL};
	w = XLStringUtil::replace(w, replaceTabl1,isrev);

	const char * replaceTabl2[] = {
	 " W UH","うっ"
	," F AH","ふぃ"
	," HH AH","へ"
	," P AW","ぷぁ"
	," F AW","ふぁう"
	," P AA","ぱ"
	," R AW","らう"
	," P ER","ぺー"
	," P EH","ぺ"
	," HH OW","ほ"
	," F IY","ふぃー"
	," K OW","こ"
	," K OW","こあ"
	," L AY","らい"
	," AH P","あっぷ"
	," T EY","てー"
	," IH K","えく"
	," AH L","うる"
	," S IY","しー"
	," T OY","とーい"
	," T AE","たっ"
	," F AE","ふぁ"
	," B OW","ぼー"
	," M AE","ま"
	," N IH","に"
	," AW ER","わー"
	," K AE","きゃ"
	," K EY","けー"
	," L UW","りゅー"
	," AO F","おふ"
	," OY L","おいる"
	," M EH","めっ"
	," B IY","びー"
	," N IH","にっ"
	," K IH","きっ"
	," B AE","ばっ"
	," P AE","ぱ"
	," L IY","りー"
	," S EH","せ"
	," B ER","ばー"
	," D AW","どっ"
	," N EY","ねー"
	," S IH","し"
	," V ER","ばー"
	," Z EH","ぜ"
	," L AO","ろ"
	," R OY","ろい"
	," L EH","れ"
	," N UW","にゅー"
	," R IY","りー"
	," T AY","てぃ"
	," D IY","でぃ"
	," B IH","びっ"
	," B AH","ばっ"
	," AE L","あー"
	," R AY","らい"
	," T UH","とぅ"
	," T IY","てぃ"
	," V IY","びぃ"
	," R OW","ろー"
	," T EY","てー"
	," T ER","たー"
	," L AY","らい"
	," B AE","ばっ"
	," AE M","あん"
	," AH N","あん"
	," AA N","あん"
	," S AE","さ"
	," P ER","ぱー"
	," B UH","ぶっ"
	," R UH","ろっ"
	," Y UH","ろぉ"
	," P UH","ぷっ"
	," T UH","とぅ"
	," CH UH","ちっ"
	," N UH","ぬっ"
	," M UH","むっ"
	," HH UH","はっ"
	," S UH","しっ"
	," K UH","くっ"
	," D UH","どぅ"
	," F UH","ふっ"
	," SH UH","しっ"
	," Z UH","ずっ"
	," L UH","ろっ"
	," G UH","ぐっ"
	," SH AA","しゃー"
	," D AA","どっ"
	," T AA","と"
	," V AA","ぶぁ"
	," HH AA","はぁ"
	," HH IH","はい"
	," HH OW","ほー"
	," K AA","くぉ"
	," S AA","すっ"
	," L AA","ろっ"
	," M AA","まー"
	," R OW","あろー"
	," ER OW","えあろ"
	," B IH","びっ"
	," D AE","だっ"
	," AH D","あど"
	," M IH","みい"
	," AE D","あーど"
	," AE D","あーど"
	," AE D","あど"
	," M ER","み"
	," AH L","らる"
	," AH N","あん"
	," AE N","あん"
	," M AY","まい"
	," R ER","やー"
	," IH NG","いんぐ"
	," Z IH","じー"
	," L IY","りー"
	," JH EH","じぇ"
	," V IH","ぶー"
	," L AH","ぷ"
	," M AH","め"
	," SH AH","しょ"
	," S AH","すっ"
	," R AH","ろー"
	," R EH","れ"
	," R EH","れ"
	," R AA","ろ"
	," EY P","あぷっ"
	," AE P","あぷっ"
	," P OW","ぽーす"
	," S AW","さう"
	," N D","んど"
	," L Z","るず"
	," S OW","そー"
	," AE P","あぷ"
	," AA R","あー"
	," ER IH","あり"
	," M ER","まー"
	," M Z","むず"
	," ER EY","あれい"
	," R IH","りっ"
	," T IY","とりー"
	," B IY","びー"
	," AE S","あす"
	," AA S","あす"
	," K AE","きゃっ"
	," T ER","たー"
	," K S","くす"
	," T EH","て"
	," T AY","てぃ"
	," K UW","くー"
	," T IY","てぃー"
	," CH ER","ちゃー"
	," T AY","たい"
	," W ER","わー"
	," SH OW","しょう"
	," SH AA","しょっ"
	," SH OW","しょお"
	," T IH","てぃ"
	," T EH","て"
	," K R AE","くら"
	," G R AE","ぐら"
	," W AY","ほわ"
	," HH UW","ふー"
	," EH R","えあー"
	," L AO","ろっ"
	," P AO R","ぽー"
	," SH EH","しぇ"
	," SH IH","しっ"
	," P EY","ぺー"
	," P IY","ぴー"
	," R AY","らい"
	," R IH","りっ"
	," T AY","たい"
	," R AE","ら"
	," S EY","せー"
	," W EY","うぇー"
	," W IH","うい"
	," DH IY","でぃ"
	," F AO R","ふぉー"
	," D IY","でぃ"
	," M EY","めー"
	," JH EH","じぇ"
	," D ER","だー"
	," L IY","りぃ"
	," D IH","でぃ"
	," D AA","だ"
	," IH L","える"
	," EH L","える"
	," D AO","ど"
	," R AH","らー"
	," EH L","える"
	," ER AA","らー"
	," R OW","ろー"
	," D OW","どー"
	," R IY","りー"
	," D AH","だー"
	," D EH","で"
	," W IH","うぃ"
	," N ER","なー"
	," L EH","れ"
	," K T","くと"
	," K EY","けー"
	," F IH","ふぃー"
	," T W","とぅ"
	," M IY","みぃ"
	," M IH","みぃ"
	," S EH","さー"
	," R IY","りー"
	," M IH","みっ"
	," K AH","か"
	," L ER","らー"
	," M IY","みー"
	," NG ER","んがー"
	," IH NG","いんぐ"
	," IH CH","ぴっち"
	," F AY","ふぁ"
	," Z AY","ざい"
	," P AH","ぷ"
	," R AH","ろ"
	," W AH","くー"
	," S ER","さー"
	," V IH","びぃ"
	," L LY","りー"
	," K ER","かー"
	," R IH","り"
	," S UW","すー"
	," G ER","がー"
	," HH AY","はい"
	," V IH","び"
	," T UW","とぅー"
	," Z IH","じっ"
	," Y EH","いえ"
	," L OW","ろー"
	," R IY","りー"
	," B EY","べー"
	," N OW","のー"
	," OW L","おーる"
	," AA N","おん"
	," AO N","おん"
	," S ER","さー"
	," S IY","しー"
	," R EY","れー"
	," IH M","いん"
	," IH N","いん"
	," D EH","でっ"
	," F IY","ふぃー"
	," N EH","ねっ"
	," W AO","うぉー"
	," R AH","ろ"
	," G EY","げー"
	," V AH","べ"
	," CH EY","ちぇ"
	," V EY","べ"
	," G AH","が"
	," Z ER","ざー"
	," HH AW","はう"
	,NULL,NULL};
	w = XLStringUtil::replace(w, replaceTabl2,isrev);

	const char * replaceTable3[] = {
	 " ZH","ず"
	," AH","あ"
	," AW","ぁ"
	," AA","あー"
	," AE","あー"
	," EH","え"
	," ER","えあ"
	," EY","えい"
	," NG","んぐ"
	," IH","え"
	," IY","い"
	," CH","ち"
	," JH","じ"
	," AY","い"
	," OW","おー"
	," EH","い"
	," SH","しゅ"
	," AO","おー"
	," UW","うー"
	," TH","す"
	," B","ぶ"
	," D","ど"
	," F","ふ"
	," G","ぐ"
	," K","く"
	," L","る"
	," M","む"
	," N","ん"
	," P","ぷ"
	," R","あ"
	," S","す"
	," T","と"
	," V","ぶ"
	," W","う"
	," Y","い"
	," Z","ず"
	,NULL,NULL};
	w = XLStringUtil::replace(w, replaceTable3,isrev);

	
	w = XLStringUtil::replace(w," ","");
	return w;
}


//英語の読みデータから日本語の音素データに変換します
string JuliusOnsoUtil::EnglishToJapanesOnso(const string& english,bool isrev)
{
	string w =" " + XLStringUtil::strtoupper(english) +" ";
	const char * replaceTabl0[] = {
		 " AA0 R "," a: "
		," AA1 R "," a: " //ae //CAR  K AA1 R
		," AA2 R "," a: "
		," AO0 R "," o: "
		," AO1 R "," o: " //ce //MORE  M AO1 R
		," AO2 R "," o: "
		," IH0 R "," y a: "
		," IH1 R "," y a: " //ie //NEAR  N IH1 R
		," IH2 R "," y a: "
		," UW0 R "," u a: "
		," UW1 R "," u a: " //ue //POOR  P UW1 R
		," UW2 R "," u a: "
		," EH0 R "," e a: "
		," EH1 R "," e a: " //ee //BEAR  B EH1 R
		," EH2 R "," e a: "
		," T ER0 "," r a "	//たー だけど らー の方が近いみたい.
		," T ER1 "," r a "
		," T ER2 "," r a "
		,NULL,NULL
	};
	w = XLStringUtil::replace(w, replaceTabl0,isrev);

	//母音
	const char * replaceTabl1[] = {
		 " AA0 "," o "
		," AA1 "," o: " //a  //HOT  HH AA1 T
		," AA2 "," o: "
		," AE0 "," a "
		," AE1 "," a: " //ae //BAG B AE1 G  //HAT HH AE1 T
		," AE2 "," a: "
		," AH0 "," o " //eの逆   
		," AH1 "," a: "//^の逆   
		," AH2 "," a: "   
		," AY0 "," a i "   
		," AY1 "," a i "	//ai   
		," AY2 "," a i "   
		," EY0 "," e "   
		," EY1 "," e i "	//ei //WAIT  W EY1 T   
		," EY2 "," e: i "   
		," OY0 "," o i "   
		," OY1 "," o: i "	//ci //BOY  B OY1   
		," OY2 "," o: i "   
		," AW0 "," a u "   
		," AW1 "," a u "	//au //HOUSE  HH AW1 S  
		," AW2 "," a u: "   
		," OW0 "," o u "   
		," OW1 "," o u "	//au //GO  G OW1
		," OW2 "," o u: "   
		," AO0 "," o "   
		," AO1 "," o "	//c: //DOG  D AO1 G
		," AO2 "," o "   
		," ER0 "," a: " //er //AFTER  AE1 F T ER0
		," ER1 "," a: "	//e: //BIRD  B ER1 D
		," ER2 "," a: "   
		," IY0 "," i: "
		," IY1 "," i: "	//i: //EAT  IY1 T
		," IY2 "," i: "   
		," IH0 "," i "	//i //IN  IH0 N
		," IH1 "," i "
		," IH2 "," i "   
		," UW0 "," u "
		," UW1 "," u: "	//u: //SOON  S UW1 N
		," UW2 "," u: "   
		," UH0 "," u q "
		," UH1 "," u q "	//u //LOOK  L UH1 K
		," UH2 "," u q "   
		," EH0 "," e "
		," EH1 "," e "	//e //ANY  EH1 N IY0
		," EH2 "," e "  
		," A "," a "  
		," I "," i "  
		," U "," u "  
		," E "," e "  
		," O "," o "  
		,NULL,NULL
	};
	w = XLStringUtil::replace(w, replaceTabl1,isrev);

	//Y
	const char * replaceTablY[] = {
	 " G Y a"," gy a"
	," G Y u"," gy u"
	," G Y o"," gy o"
	," S Y i"," sh i:"
	," SH Y i"," s i:"
	," ZH Y a"," zy a"
	," ZH Y u"," zy u"
	," ZH Y o"," zy o"
	," F Y a"," hy a"
	," F Y u"," hy u"
	," F Y o"," hy o"
	," P Y i"," p i:"
	," P Y a"," py a"
	," P Y u"," py u"
	," P Y o"," py o"
	," M Y i"," m i:"
	," M Y e"," m e"
	," M Y a"," my a"
	," M Y u"," my u"
	," M Y o"," my o"
	," L Y i"," r i:"
	," L Y a"," ry a"
	," L Y u"," ry u"
	," L Y o"," ry o"
	," L Y e"," r e:"
	," D Y i"," d i"
	," D Y e"," d e:"
	," D Y a"," dy a"
	," D Y u"," dy u"
	," D Y o"," dy o"
	," T Y i"," t i"
	," T Y e"," t e:"
	," T Y a"," ty a"
	," T Y u"," ty u"
	," T Y o"," ty o"
	," Z Y a"," zy a"
	," Z Y u"," zy u"
	," Z Y o"," zy o"
	," Z Y e"," z e"
	," Z Y o"," z o"
	," K Y a"," ky a"
	," K Y u"," ky u"
	," K Y o"," ky o"
	," S Y a"," sh a"
	," S Y u"," sh u"
	," S Y o"," sh o"
	," SH Y a"," sh a"
	," SH Y u"," sh u"
	," SH Y e"," sh e"
	," SH Y o"," sh o"
	," CH Y a"," ch a"
	," CH Y u"," ch u"
	," CH Y o"," ch o"
	," TH Y u"," t u"
	," TH Y a"," ty a"
	," TH Y u"," ty u"
	," TH Y o"," ty o"
	," DH Y a"," d o a"
	," DH Y u"," d u"
	," DH Y o"," dy u"
	," N Y a"," ny a"
	," N Y u"," ny u"
	," N Y o"," ny o"
	," HH Y a"," hy a"
	," HH Y u"," hy u"
	," HH Y o"," hy o"
	," R Y a"," ry a"
	," R Y u"," ry u"
	," R Y o"," ry o"
	," G Y a"," gy a"
	," G Y u"," gy u"
	," G Y o"," gy o"
	," NG Y a"," NX gy a"
	," NG Y u"," NX gy u"
	," NG Y o"," NX gy o"
	," JH Y e"," j e"
	," J Y e"," j e"
	," J Y a"," j a"
	," J Y u"," j u"
	," J Y o"," j o"
	," JH Y a"," j a"
	," JH Y u"," j u"
	," JH Y o"," j o"
	," B Y a"," by a"
	," B Y u"," by u"
	," B Y o"," by o"
	," V Y a"," by a"
	," V Y u"," by u"
	," V Y o"," by o"
	," P Y a"," py a"
	," P Y u"," py u"
	," P Y o"," py o"
	," F Y a"," hy a"
	," F Y u"," hy u"
	," F Y o"," hy o"
	," F Y e"," f e"
	," F Y o"," f o"
	," T S "," ts u "
		,NULL,NULL
	};
	w = XLStringUtil::replace(w, replaceTablY,isrev);

	//特殊ルール 孤立している子音に読みをふる
	const char * boin[] = { "a", "i", "u", "e", "o", "a:", "i:", "u:", "e:", "o:","NX","q","A","I","U","E","O",NULL } ;
	auto vec = XLStringUtil::split_vector(" ",w);
	string ww;
	for(unsigned int i = 0 ; i < vec.size() ; ++i)
	{
		if ( vec[i].empty() ) continue;
		const char * p = XLStringUtil::findConstCharTable(boin,vec[i].c_str() );
		if ( p )
		{//子音ではない
			ww = ww + " " + vec[i];
			continue;
		}
		if (i + 1 < vec.size() )
		{
			if ( !vec[i+1].empty() )
			{
				const char * pp = XLStringUtil::findConstCharTable(boin,vec[i+1].c_str() );
				if ( pp )
				{//次は子音ではないです
					ww = ww + " " + vec[i];
					continue;
				}
			}
		}
		//孤立している子音です
		if (vec[i] == "S") ww = ww + " s u ";
		else if (vec[i] == "Z") ww = ww + " z u ";
		else if (vec[i] == "SH") ww = ww + " s u ";
		else if (vec[i] == "ZH") ww = ww + " z u ";
		else if (vec[i] == "TH") ww = ww + " ts u ";
		else if (vec[i] == "DH") ww = ww + " d o ";
		else if (vec[i] == "F") ww = ww + " f u ";
		else if (vec[i] == "V") ww = ww + " b a ";
		else if (vec[i] == "R") ww = ww + ":";
		else if (vec[i] == "L") ww = ww + " r u ";
		else if (vec[i] == "T") ww = ww + " t o ";
		else if (vec[i] == "D") ww = ww + " d o ";
		else if (vec[i] == "P") ww = ww + " p u ";
		else if (vec[i] == "B") ww = ww + " b a ";
		else if (vec[i] == "K") ww = ww + " k u ";
		else if (vec[i] == "G") ww = ww + " g u ";
		else if (vec[i] == "CH") ww = ww + " ch i ";
		else if (vec[i] == "JH") ww = ww + " j a ";
		else if (vec[i] == "M") ww = ww + " NX ";
		else if (vec[i] == "N") ww = ww + " NX ";
		else if (vec[i] == "NX") ww = ww + " NX ";
		else if (vec[i] == "NG") ww = ww + " NX g u ";
		else if (vec[i] == "HH") ww = ww + " h a ";
		else if (vec[i] == "W") ww = ww + " u q ";
		else if (vec[i] == "J") ww = ww + " j a ";
		else if (vec[i] == "Y") ww = ww + " y a ";
		else 
		{//nazo
			ASSERT(0);
		}
	}
	w = ww + " ";

	//子音
	const char * replaceTabl2[] = {
		 " S "," sh "	//s //SEE  S IY1
		," Z "," z "	//Z //EASY  IY1 Z IY0
		," SH "," sh "	//s //SHE  SH IY1
		," ZH "," j "	//3 //VISION  V IH1 ZH AH0 N
		," TH "," sh "	//8 //THANK  TH AE1 NG K
		," DH "," z "	//6 //THE  DH AH0
		," F "," f "	//f //FOR  F AO1 R
		," V "," b "	//v //VERY  V EH1 R IY0
		," R "," r "	//r //RIGHT  R AY1 T
		," L "," r "	//l //LIKE  L AY1 K
		," T "," t "	//t	//TEA  T IY1
		," D "," d "	//d	//DAY  D EY1
		," P "," p "	//p	//PUT  P UH1 T
		," B "," b "	//b	//BUT  B AH1 T
		," K "," k "	//k	//CAKE K EY1 K
		," G "," g "	//g	//BIG  B IH1 G
		," CH "," ch "	//ch //TEACH  T IY1 CH
		," JH "," j "	//d3 //AGE  EY1 JH
		," M "," m "	//m //MANY  M EH1 N IY0
		," N "," n "	//n //NEW  N UW1
		," NG "," N g "	//n //THING  TH IH1 NG
		," HH "," h "	//h //HAT  HH AE1 T
		," W "," w "	//w //WE  W IY1
		," J "," y "	//j //YES  Y EH1 S
		," Y "," y "	//
		," NX "," N "
		,NULL,NULL
	};
	w = XLStringUtil::replace(w, replaceTabl2,isrev);

	//# 変換の結果長音記号が続くことがまれにあるので一つにまとめる
	const char * replaceTable6[] = {
	 ": : :",":"
	,": :",":"
	,"::",":"
	,"  "," "
	,NULL,NULL};
	w = XLStringUtil::replace(w, replaceTable6);

	//きゅう -> ky u u -> ky u: みたいに 連続する u u は u:にする
	//http://julius.sourceforge.jp/sapi/Docs/develop.html
	const char * replaceTable7[] = {
	 " a a "," a: "	//		,"ああ" , "あー"
	," i i "," i: "	//		,"いい" , "いー"
	," u u "," u: "	//		,"うう" , "うー"
	," e e "," e: "	//		,"ええ" , "えー"
	," e i "," e: "	//		,"えい" , "えー"
	," o o "," o: "	//		,"おお" , "おー"
	," o u "," o: "	//		,"おう" , "おー"
	,NULL,NULL};
	w = XLStringUtil::replace(w, replaceTable7);

	const char * replaceTable8[] = {
	//句読点
	 "。",""
	,"、",""
	,".",""
	,",",""
	,"・",""
	,"「",""
	,"」",""
	,"@",""
	,NULL,NULL};
	w = XLStringUtil::replace(w, replaceTable8);

	w = XLStringUtil::chop(w);
	return w;
}

//

//英語の音声認識で利用できる形に変換します
string JuliusOnsoUtil::ConvertEnglishSentenceToYomi(const string& sentence,bool useJapanesWakachigaki) 
{
	string yomi ;
	const vector<string> vec =
		XLStringUtil::split_vector(" ",XLStringUtil::mb_convert_kana( sentence, "as" ) );
	for(auto it = vec.begin(); it != vec.end() ; it++ )
	{
		if (it->empty())
		{
			continue;
		}

		if (! XLStringUtil::isalpnum(*it) )
		{//日本語？
			if (useJapanesWakachigaki)
			{
				vector<string> wakachigakiVec = 
					MainWindow::m()->MecabControl.Wakachigai(*it);
				for(auto wa_it = wakachigakiVec.begin(); wa_it != wakachigakiVec.end() ; wa_it++ )
				{
					yomi += " @ " + JapaneseToEnglishOnso(*wa_it,false);
				}
			}
			else
			{
				yomi += " @ " + JapaneseToEnglishOnso(*it,false);
			}
		}
		else
		{//英語
			yomi += " @ " + EnglishToEnglishOnso(MainWindow::m()->MecabControl.GetCMUYomi(*it) , false);
		}
	}

	if(yomi.size() >= 3)
	{
		yomi = yomi.substr(3);
	}

	//連続する空白を一つに
	yomi = XLStringUtil::replace(yomi,"  "," ");
	yomi = XLStringUtil::replace(yomi,"  "," ");
	ASSERT (yomi.find("  ") == string::npos);

	const char * replaceTableJikoMujyun[] = {
	//自己矛盾
	 "ah aa","ah"
	,"uh uw","uw"
	,"oh ow","ow"
	,"aa ah","aa"
	,"uw uh","uw"
	,"ow oh","ow"
	,"ow uw","ow"
	,"uw ow","uw"
	,"ah ah","ah r"
	,"uw uw","uw"
	,"ow ow","ow"
	,"b b","b"
	,"ch ch","ch"
	,"f f","f"
	,"g g","g"
	,"jh jh","jh"
	,"hh hh","hh"
	,"k k","k"
	,"l l","r uw l"
	,"m m","m"
	,"p p","p"
	,"r r","r"
	,"s s","r"
	,"sh sh","sh"
	,"t t","t"
	,"y y","y"
	,"z z","z"
	,"eh aa","eh r"
	,"oh aa","oh r"
	,"uh f y","uw f y"
	,"f uw y","f uw r y"
	,"z uh t","z uw t"
	,"n aw g","n ah g"
	,"uh g","uw g"	
	,"ah r uh","ah r uw"
	,"b ow ae","b ow"
	,"ao f ae","ao f aa"
	,NULL,NULL};
	yomi = XLStringUtil::replace(yomi, replaceTableJikoMujyun);

	yomi = XLStringUtil::chop(yomi);
	return yomi;
}

//日本語文を、音声認識にかけられる形式に変換します
string JuliusOnsoUtil::ConvertJapanesSentenceToYomi(const string& sentence) 
{
	//カタカナひらがな変換
	string w = MainWindow::m()->MecabControl.KanjiAndKanakanaToHiragana(sentence);
//	w = XLStringUtil::mb_convert_kana(w,"cHsa");

	const char * replaceTable1[] = {
	//特殊ルール
	 "え゛","えっ"
	,"こんにちは","こんにちわ"
	,"こんばんは","こんばんわ"
	,"わたしは","わたしわ"
	,"ぼくは","ぼくわ"
	,"かれは","かれわ"
	,"かれらは","かれらわ"
	,"かのじょは","かのじょわ"
	,"これらは","これらわ"
	,"あれは","あれわ"
	////,"なのは","なのわ" //NLPの最大の課題、リリカルなのは問題によりダメです。
	,NULL,NULL};
	w = XLStringUtil::replace(w, replaceTable1);

	//yomi2vocaより
	//# 3文字以上からなる変換規則（v a）
	const char * replaceTable2[] = {
	 "う゛ぁ"," b a"
	,"う゛ぃ"," b i"
	,"う゛ぇ"," b e"
	,"う゛ぉ"," b o"
	,"う゛ゅ"," by u"
	,NULL,NULL};
	w = XLStringUtil::replace(w, replaceTable2);

	//# 2文字からなる変換規則
	const char * replaceTable3[] = {
     "ぅ゛"," b u"
	,"あぁ"," a a"
	,"いぃ"," i i"
    ,"いぇ"," i e"
	,"いゃ"," y a"
	,"うぅ"," u:"
	,"えぇ"," e e"
	,"おぉ"," o:"
	,"かぁ"," k a:"
	,"きぃ"," k i:"
	,"くぅ"," k u:"
	,"くゃ"," ky a"
	,"くゅ"," ky u"
	,"くょ"," ky o"
	,"けぇ"," k e:"
	,"こぉ"," k o:"
	,"がぁ"," g a:"
	,"ぎぃ"," g i:"
	,"ぐぅ"," g u:"
	,"ぐゃ"," gy a"
	,"ぐゅ"," gy u"
	,"ぐょ"," gy o"
	,"げぇ"," g e:"
	,"ごぉ"," g o:"
	,"さぁ"," s a:"
	,"しぃ"," sh i:"
	,"すぅ"," s u:"
	,"すゃ"," sh a"
	,"すゅ"," sh u"
	,"すょ"," sh o"
	,"せぇ"," s e:"
	,"そぉ"," s o:"
	,"ざぁ"," z a:"
	,"じぃ"," j i:"
	,"ずぅ"," z u:"
	,"ずゃ"," zy a"
	,"ずゅ"," zy u"
	,"ずょ"," zy o"
	,"ぜぇ"," z e:"
	,"ぞぉ"," z o:"
	,"たぁ"," t a:"
	,"ちぃ"," ch i:"
	,"つぁ"," ts a"
	,"つぃ"," ts i"
	,"つぅ"," ts u:"
	,"つゃ"," ch a"
	,"つゅ"," ch u"
	,"つょ"," ch o"
	,"つぇ"," ts e"
	,"つぉ"," ts o"
	,"てぇ"," t e:"
	,"とぉ"," t o:"
	,"だぁ"," d a:"
	,"ぢぃ"," j i:"
	,"づぅ"," d u:"
	,"づゃ"," zy a"
	,"づゅ"," zy u"
	,"づょ"," zy o"
	,"でぇ"," d e:"
	,"どぉ"," d o:"
	,"なぁ"," n a:"
	,"にぃ"," n i:"
	,"ぬぅ"," n u:"
	,"ぬゃ"," ny a"
	,"ぬゅ"," ny u"
	,"ぬょ"," ny o"
	,"ねぇ"," n e:"
	,"のぉ"," n o:"
	,"はぁ"," h a:"
	,"ひぃ"," h i:"
	,"ふぅ"," f u:"
	,"ふゃ"," hy a"
	,"ふゅ"," hy u"
	,"ふょ"," hy o"
	,"へぇ"," h e:"
	,"ほぉ"," h o:"
	,"ばぁ"," b a:"
	,"びぃ"," b i:"
	,"ぶぅ"," b u:"
	,"ふゃ"," hy a"
	,"ぶゅ"," by u"
	,"ふょ"," hy o"
	,"べぇ"," b e:"
	,"ぼぉ"," b o:"
	,"ぱぁ"," p a:"
	,"ぴぃ"," p i:"
	,"ぷぅ"," p u:"
	,"ぷゃ"," py a"
	,"ぷゅ"," py u"
	,"ぷょ"," py o"
	,"ぺぇ"," p e:"
	,"ぽぉ"," p o:"
	,"まぁ"," m a:"
	,"みぃ"," m i:"
	,"むぅ"," m u:"
	,"むゃ"," my a"
	,"むゅ"," my u"
	,"むょ"," my o"
	,"めぇ"," m e:"
	,"もぉ"," m o:"
	,"やぁ"," y a:"
	,"ゆぅ"," y u:"
	,"ゆゃ"," y a:"
	,"ゆゅ"," y u:"
	,"ゆょ"," y o:"
	,"よぉ"," y o:"
	,"らぁ"," r a:"
	,"りぃ"," r i:"
	,"るぅ"," r u:"
	,"るゃ"," ry a"
	,"るゅ"," ry u"
	,"るょ"," ry o"
	,"れぇ"," r e:"
	,"ろぉ"," r o:"
	,"わぁ"," w a:"
	,"をぉ"," o:"

	,"う゛"," b u"
	,"でぃ"," d i"
	,"でぇ"," d e:"
	,"でゃ"," dy a"
	,"でゅ"," dy u"
	,"でょ"," dy o"
	,"てぃ"," t i"
	,"てぇ"," t e:"
	,"てゃ"," ty a"
	,"てゅ"," ty u"
	,"てょ"," ty o"
	,"すぃ"," s i"
	,"ずぁ"," z u a"
	,"ずぃ"," z i"
	,"ずぅ"," z u"
	,"ずゃ"," zy a"
	,"ずゅ"," zy u"
	,"ずょ"," zy o"
	,"ずぇ"," z e"
	,"ずぉ"," z o"
	,"きゃ"," ky a"
	,"きゅ"," ky u"
	,"きょ"," ky o"
	,"しゃ"," sh a"
	,"しゅ"," sh u"
	,"しぇ"," sh e"
	,"しょ"," sh o"
	,"ちゃ"," ch a"
	,"ちゅ"," ch u"
	,"ちぇ"," ch e"
	,"ちょ"," ch o"
	,"とぅ"," t u"
	,"とゃ"," ty a"
	,"とゅ"," ty u"
	,"とょ"," ty o"
	,"どぁ"," d o a"
	,"どぅ"," d u"
	,"どゃ"," dy a"
	,"どゅ"," dy u"
	,"どょ"," dy o"
	,"どぉ"," d o:"
	,"にゃ"," ny a"
	,"にゅ"," ny u"
	,"にょ"," ny o"
	,"ひゃ"," hy a"
	,"ひゅ"," hy u"
	,"ひょ"," hy o"
	,"みゃ"," my a"
	,"みゅ"," my u"
	,"みょ"," my o"
	,"りゃ"," ry a"
	,"りゅ"," ry u"
	,"りょ"," ry o"
	,"ぎゃ"," gy a"
	,"ぎゅ"," gy u"
	,"ぎょ"," gy o"
	,"ぢぇ"," j e"
	,"ぢゃ"," j a"
	,"ぢゅ"," j u"
	,"ぢょ"," j o"
	,"じぇ"," j e"
	,"じゃ"," j a"
	,"じゅ"," j u"
	,"じょ"," j o"
	,"びゃ"," by a"
	,"びゅ"," by u"
	,"びょ"," by o"
	,"ぴゃ"," py a"
	,"ぴゅ"," py u"
	,"ぴょ"," py o"
	,"うぁ"," u a"
	,"うぃ"," w i"
	,"うぇ"," w e"
	,"うぉ"," w o"
	,"ふぁ"," f a"
	,"ふぃ"," f i"
	,"ふぅ"," f u"
	,"ふゃ"," hy a"
	,"ふゅ"," hy u"
	,"ふょ"," hy o"
	,"ふぇ"," f e"
	,"ふぉ"," f o"
	,NULL,NULL};
	w = XLStringUtil::replace(w, replaceTable3);


	//# 1音からなる変換規則
	const char * replaceTable4[] = {
	 "あ"," a"
	,"い"," i"
	,"う"," u"
	,"え"," e"
	,"お"," o"
	,"か"," k a"
	,"き"," k i"
	,"く"," k u"
	,"け"," k e"
	,"こ"," k o"
	,"さ"," s a"
	,"し"," sh i"
	,"す"," s u"
	,"せ"," s e"
	,"そ"," s o"
	,"た"," t a"
	,"ち"," ch i"
	,"つ"," ts u"
	,"て"," t e"
	,"と"," t o"
	,"な"," n a"
	,"に"," n i"
	,"ぬ"," n u"
	,"ね"," n e"
	,"の"," n o"
	,"は"," h a"
	,"ひ"," h i"
	,"ふ"," f u"
	,"へ"," h e"
	,"ほ"," h o"
	,"ま"," m a"
	,"み"," m i"
	,"む"," m u"
	,"め"," m e"
	,"も"," m o"
	,"ら"," r a"
	,"り"," r i"
	,"る"," r u"
	,"れ"," r e"
	,"ろ"," r o"
	,"が"," g a"
	,"ぎ"," g i"
	,"ぐ"," g u"
	,"げ"," g e"
	,"ご"," g o"
	,"ざ"," z a"
	,"じ"," j i"
	,"ず"," z u"
	,"ぜ"," z e"
	,"ぞ"," z o"
	,"だ"," d a"
	,"ぢ"," j i"
	,"づ"," z u"
	,"で"," d e"
	,"ど"," d o"
	,"ば"," b a"
	,"び"," b i"
	,"ぶ"," b u"
	,"べ"," b e"
	,"ぼ"," b o"
	,"ぱ"," p a"
	,"ぴ"," p i"
	,"ぷ"," p u"
	,"ぺ"," p e"
	,"ぽ"," p o"
	,"や"," y a"
	,"ゆ"," y u"
	,"よ"," y o"
	,"わ"," w a"
	,"ゐ"," i"
	,"ゑ"," e"
	,"ん"," N"
	,"っ"," q"
	,"ー",":"
	,"-",":"
	,"ｰ",":"
	,NULL,NULL};
	w = XLStringUtil::replace(w, replaceTable4);

	//# ここまでに処理されてない ぁぃぅぇぉ はそのまま大文字扱い
	const char * replaceTable5[] = {
	 "ぁ"," a"
	,"ぃ"," i"
	,"ぅ"," u"
	,"ぇ"," e"
	,"ぉ"," o"
	,"ゎ"," w a"
	,"ぉ"," o"
	//# その他特別なルール
    ,"を"," o"
	,NULL,NULL};
	w = XLStringUtil::replace(w, replaceTable5);


	//# 変換の結果長音記号が続くことがまれにあるので一つにまとめる
	const char * replaceTable6[] = {
	 ": : :",":"
	,": :",":"
	,NULL,NULL};
	w = XLStringUtil::replace(w, replaceTable6);

	//きゅう -> ky u u -> ky u: みたいに 連続する u u は u:にする
	//http://julius.sourceforge.jp/sapi/Docs/develop.html
	const char * replaceTable7[] = {
	 " a a "," a: "	//		,"ああ" , "あー"
	," i i "," i: "	//		,"いい" , "いー"
	," u u "," u: "	//		,"うう" , "うー"
	," e e "," e: "	//		,"ええ" , "えー"
	," e i "," e: "	//		,"えい" , "えー"
	," o o "," o: "	//		,"おお" , "おー"
	," o u "," o: "	//		,"おう" , "おー"
	,NULL,NULL};
	w = XLStringUtil::replace(w, replaceTable7);

	const char * replaceTable8[] = {
	//句読点
	 "。",""
	,"、",""
	,".",""
	,",",""
	,"・",""
	,"「",""
	,"」",""
	,NULL,NULL};
	w = XLStringUtil::replace(w, replaceTable8);

	//連続する空白を一つに
	w = XLStringUtil::replace(w,"  "," ");

	w = XLStringUtil::chop(w);
	return w;
};



//よみを確認します。
bool JuliusOnsoUtil::CheckYomi(const string& phoneFilename,const string& yomi,string* outBadString) 
{
	if (yomi.empty()) return true;

	const auto vec = XLStringUtil::split_vector(" ",yomi);
	if (vec.size() == 0)
	{//読みがない
		return true;
	}

	//phoneファイルは数百キロバイトなのでメモリに呼んでしまおう(富豪的w)
	const string phoneCat = "\n" + XLFileUtil::cat(phoneFilename) + "\n";

	for(unsigned int i = 0 ; i < vec.size() ; i++ )
	{
		string searchPhone ;
		if (i + 1 >= vec.size() )
		{
			searchPhone = vec[i];
		}
		else if (i + 2 >= vec.size() )
		{
			searchPhone = vec[i] + "+" + vec[i+1];
		}
		else 
		{
			searchPhone = vec[i] + "-" + vec[i+1] + "+" + vec[i+2];
		}
		
		if ( phoneCat.find("\n" + searchPhone + "\n") == string::npos )
		{
			if ( phoneCat.find("\n" + searchPhone + " ") == string::npos )
			{
				*outBadString = searchPhone;
				return false;
			}
		}
	}
	return true;
}
/*
SEXYTEST()
{
	const string phoneFilename = "./config/julius/voxforge/tiedlist";
	string outBadString;
	{
		//よみを確認します。
		bool rr = JuliusOnsoUtil::CheckYomi(phoneFilename,"t er n ao f l ay t",&outBadString);
		ASSERT(rr);
	}
}*/

//読みからindex番目の音素(母音 or 母音+子音を取得する)
string JuliusOnsoUtil::PickupOnso(const string& yomi,unsigned int index,std::function<bool(const char*)> isBoinFunction) 
{
	unsigned c = 0;
	for(const char * p = yomi.c_str() ; *p ; ++p )
	{
		if (*p == ' ') continue;

		if ( isBoinFunction(p) )
		{
			if (c == index )
			{
				return string(p,0,1);
			}
			else
			{
				c++;
				continue;
			}
		}
		else
		{
			const char * pp = p + 1;
			for(; *pp ; ++pp )
			{
				if ( isBoinFunction(pp) )
				{
					break;
				}
			}

			if (*pp == NULL)
			{//有効な母音がない・・・
				if (c == index )
				{
					return p;
				}
				else
				{
					return "";
				}
			}
			//母音後の空白まで
			const char * sep = strchr(pp + 1,' ');
			if (sep == NULL)
			{
				if (c == index )
				{
					return p;
				}
				else
				{
					return "";
				}
			}
			else
			{
				if (c == index )
				{
					return string(p,0,sep - p );
				}
				else
				{
					c++;
					p = sep;
					continue;
				}
			}
		}
	}
	//ない
	return "";
}

//読みから最初の母音を取得する a i u e o N
string JuliusOnsoUtil::YomiToFirstBoin(const string& yomiChar,std::function<bool(const char*)> isBoinFunction ) 
{
	for(const char * p = yomiChar.c_str() ; *p ; ++p )
	{
		if ( isBoinFunction(p))
		{
			return string("") + *p;
		}
	}
	//母音がなかった
	return "";
}

//日本語母音 a i u e o N
bool JuliusOnsoUtil::isBoin(const char* p)
{
	//a i u e o N
	return (*p == 'a' ||  *p == 'i' ||  *p == 'u' ||  *p == 'e' || *p == 'o' || *p == 'N');
}

//英語母音 aa ah aw iy uh uw eh ow n m
bool JuliusOnsoUtil::isBoinEnglish(const char* p)
{
	//aa ah aw iy uh uw eh ow n m
	if(*p == 'a' ||  *p == 'i' ||  *p == 'u' ||  *p == 'e' || *p == 'o' )
	{
		if(*(p+1) == 'a'|| *(p+1) == 'w' || *(p+1) == 'h')
		{
			return true;
		}
	}
	if(*p == 'n' ||  *p == 'm')
	{
		return true;
	}
	return false;
}


unsigned int JuliusOnsoUtil::CountLength(const string& matchString)
{
	if (matchString.empty()) return 0;

	unsigned int size = matchString.size();
	const char * p = matchString.c_str() ;
	for(p = p + 1 ; *p ; p++ )
	{
		if(*p == ':') size ++;
		else if (*p == 'y'||*p=='f'||*p=='j'||*p=='z') size --;
	}
	return size;
}