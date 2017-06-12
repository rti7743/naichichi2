// TODO actually recognize syntax of TypeScript constructs
(function() {

var g_fhckeyword = [
	 ["onvoice","音声認識イベントを設定します。","onvoice(音声認識する文字列, [コールバック関数]);"]
	,["action","家電に命令を送ります。","action(家電名文字列,操作文字列);"]
	,["callcommand","FHC内コマンドを実行します。","callcommand(コマンド名称文字列,引数1 , ...引数N);"]
	,["speak","合成音声で読み上げを行います。","speak(読み上げる文字列);"]
	,["speak_cache","合成音声を読み上げずにキャッシュにだけ作成します","speak_cache(キャッシュに入れる読み上げ文字列);"]
	,["play","音声ファイルを再生します。","play(音楽ファイル名文字列);"]
	,["config_get","ユーザー領域にある設定を読みます。","config_get(取得したいキー文字列,[取得できなかった時のディフォルト値]);"]
	,["config_set","ユーザー領域に設定を書き込みます。","config_set(設定するキー文字列,設定する値);"]
	,["kernel_config_get","すべての設定を読みます。","kernel_config_get(取得したいキー文字列,[取得できなかった時のディフォルト値]);"]
	,["kernel_config_set","すべての設定を書き込めます。","kernel_config_set(設定するキー文字列,設定する値);"]
	,["telnet","telnetで通信を行います。","telnet(接続するサーバIP:port文字列,ウェイト文字列,送信文字列,接続断ウエイト文字列);"]
	,["http_get","HTTP GETメソッドを投げて、結果を返します。","http_get(URL文字列,[ヘッダー連想配列],[タイムアウトミリ秒],[エンコード]);"]
	,["http_post","HTTP POSTメソッドを投げて、結果を返します。","http_post(URL文字列,POST文字列,[ヘッダー連想配列],[タイムアウトミリ秒],[エンコード]);"]
	,["kernel_execute_local","FHC端末内でプログラムを実行します。","kernel_execute_local(実行したいコマンド文字列);"]
	,["execute_remote","リモートのPCをFHCコマンダーを経由して操作します。","execute_remote(PCのIP文字列,FHCコマンダーのAPIキー,実行するコマンド文字列);"]
//	,["remote_execute","リモートのPCをFHCコマンダーを経由して操作します。","remote_execute(PCのIP文字列,FHCコマンダーのAPIキー,実行するコマンド文字列);"]
//	,["remote_pidof","リモートPCから特定のプロセス名で検索します。","remote_pidof(プロセス名)"]
//	,["remote_pidgrep","リモートPCのプロセス情報から検索して詳細な情報を連想配列で返します。","remote_pidgrep([検索ワード])"]
//	,["remote_kill","プロセスIDのプロセスを終了させます","remote_kill(プロセスID)"]
//	,["remote_system_reboot","リモートPCを再起動させます","remote_system_reboot()"]
//	,["remote_system_shutdown","リモートPCの電源を切ります","remote_system_shutdown()"]
	,["msleep","ミリ秒待機します。","msleep(待機するミリ秒);"]
	,["dump","引数で指定したものをデバッグコンソールに出力します。","dump(出力したい変数);"]
	,["print","引数で指定したものをデバッグコンソールに出力します。dump関数の別名。","print(出力したい変数);"]
	,["var_dump","引数で指定したものをデバッグコンソールに出力します。dump関数の別名。","var_dump(出力したい変数);"]
	,["alert","引数で指定したものをデバッグコンソールに出力します。dump関数の別名。","alert(出力したい変数);"]
	,["echo","引数で指定したものをデバッグコンソールに出力します。dump関数の別名。","echo(出力したい変数);"]
	,["sensor_lumi","光センサーから明るさの値を取得します。 0 - 300の値を取ります。 大きいほど明るくなります。","sensor_lumi();"]
	,["sensor_temp","温度センサーから温度の値を取得します。","sensor_temp();"]
	,["config_find","",""]
	,["kernel_config_find","",""]
	,["elec_getlist","登録している家電一覧を取得します。","elec_getlist();"]
	,["elec_getactionlist","登録している家電の動作(アクション)を取得します。","elec_getactionlist(家電名文字列);"]
	,["elec_getinfo","家電の詳細を取得します。","elec_getinfo(家電名文字列);"]
	,["elec_setstatus","家電のステータスを設定します。ステータスを変えるだけで動作させるわけではありません。","elec_setstatus(家電名文字列,ステータス文字列)"]
	,["elec_getstatus","家電のステータスを取得します","elec_getstatus(家電名文字列)"]
	,["sstp_send11","SSTP SEND 1.1 で伺かにメッセージを出させます。","sstp_send11(伺かのIP文字列,メッセージ文字列);"]
	,["soap_action","SOAPコマンドを発行します。","soap_action(URL文字列,SOAPACTION,xml文字列,[ヘッダー連想配列],[タイムアウトミリ秒]);"]
	,["wol_send","WakeupOnLanでマシンを起動します。","wol_send(起動したいマシンのMACアドレス,[port番号],[ブロードキャストIP]);"]
	,["env_version","バージョン番号を返します。","env_version();"]
	,["env_model","FHCのモデル型番を返します。","env_model();"]
	,["env_os","FHCが動作しているOSを返します。","env_os();"]
	,["upnp_search_all","UPNP(SSDP)で、特定の全てのデバイスを検索して配列で返します。","upnp_search_all(検索したい文字列,[タイムアウトミリ秒]);"]
	,["upnp_search_one","UPNP(SSDP)で、特定の一つのデバイスを検索します。","upnp_search_one(検索したい文字列,[タイムアウトミリ秒]);"]
	,["upnp_search_uuid","UPNP(SSDP)で、特定のUUIDを持つ一つのデバイスを検索します。","upnp_search_uuid(検索したい文字列,[タイムアウトミリ秒]);"]
	,["upnp_search_xml","UPNP(SSDP)とXMLで、特定の一つのデバイスを検索します。","upnp_search_xml(SSDPヘッダーから検索したい文字列,XMLから検索したい文字列,[タイムアウトミリ秒]);"]
	,["url_hosturl","URLからホスト部分(www.example.com)までを返します。","url_hosturl(URL);"]
	,["url_baseurl","URLから直前のディレクトリまで(http://www.example.com/dir/)を取得します。","url_baseurl(URL);"]
	,["url_changeprotocol","URLのプロトコルをすげ替えます。","url_changeprotocol(URL,変更するプロトコル);"]
	,["url_appendparam","URLに引数パラメータを追加します。","url_appendparam(URL,追加するパラメータ);"]
	,["base64encode","base64でエンコードした文字列を返します。","base64encode(str);"]
	,["base64decode","base64でエンコードされた文字列をデコードします。","base64decode(str);"]
	,["urlencode","URLエンコードした文字列を返します。","urlencode(str);"]
	,["urldecode","URLエンコードされた文字列をデコードします。","urldecode(str);"]
	,["md5","文字列からmd5を計算します。","md5(str);"]
	,["sha1","文字列からsha1を計算します。","sha1(str);"]
	,["mb_convert_kana","ひらがな・カタカナ、半角・倍角を相互に変換します。","mb_convert_kana(str,option);"]
	,["mb_convert_yomi","漢字や英単語から、読みの平仮名を取得します。","mb_convert_yomi(str);"]
	,["date","時刻をフォーマッティングして表示ます。","date(フォーマット,[unixtime])"]
	,["time","現在時刻を取得します","time()"]
	,["time","現在時刻を取得します","time(YYYY,MM,DD)"]
	,["time","現在時刻を取得します","time(YYYY,MM,DD,[HH],[MI],[SS])"]
	,["uuid","UUIDを取得します。UUIDは他と重複しません。","uuid()"]
	,["trim","文字列の前後の半角スペースや改行を削除します","trim(文字列)"]
	,["html_selector","htmlから特定の項目を選択肢ます。XPATHやjQueryのセレクターが使えます","html_selector(HTML文字列,セレクター)"]
	,["html_inner","htmlの直前のタグ内を取得します。一つタグを外すイメージ。","html_inner(HTML文字列)"]
	,["html_text","htmlの直前のタグ内のテキストを取得します","html_text(HTML文字列)"]
	,["innerHTML","htmlの直前のタグ内を取得します。一つタグを外すイメージ。html_innerの別名","innerHTML(HTML文字列)"]
	,["innerText","htmlの直前のタグ内のテキストを取得します。html_textの別名","innerText(HTML文字列)"]
	,["innerhtml","htmlの直前のタグ内を取得します。一つタグを外すイメージ。html_innerの別名","innerhtml(HTML文字列)"]
	,["innertext","htmlの直前のタグ内のテキストを取得します。html_textの別名","innertext(HTML文字列)"]
	,["html_attr","直前のタグ内の属性の値を取得します","html_attr(HTML文字列,属性名)"]
	,["setInterval","定期的に実行します","setInterval(実行する関数,定期実行間隔のミリ秒)"]
	,["setTimeout","時間が来たら一度だけ実行します","setTimeout(実行する関数,指定時間ミリ秒)"]
	,["clearTimeout","タイマー処理を停止します。","clearTimeout(タイマーID)"]
	,["clearInterval","タイマー処理を停止します。clearTimeoutの別名","clearInterval(タイマーID)"]
	,["setinterval","定期的に実行します。setIntervalの別名","setinterval(実行する関数,定期実行間隔のミリ秒)"]
	,["settimeout","時間が来たら一度だけ実行します。setTimeoutの別名","settimeout(実行する関数,指定時間ミリ秒)"]
	,["cleartimeout","タイマー処理を停止します。clearTimeoutの別名","cleartimeout(タイマーID)"]
	,["clearinterval","タイマー処理を停止します。clearTimeoutの別名","clearinterval(タイマーID)"]
	,["env_mic_volume","マイク音量を変更します","env_mic_volume(マイク音量0-100)"]
	,["env_speaker_volume","スピーカー音量を変更します","env_speaker_volume(スピーカー音量0-100)"]
	,["kernel_process_list","プロセス情報を連想配列で返します。","kernel_process_list()"]
	,["kernel_kill","プロセスIDのプロセスを終了させます","kernel_kill(プロセスID)"]
	,["kernel_system_reboot","フューチャーホームコントローラーを再起動させます","kernel_system_reboot()"]
	,["weather_info","天気予報APIから情報を取得します","weather_info(天気を取得する住所)"]
	,["address_parse","住所から経緯度を求めます。(現在は都道府県のみ対応)","address_parse(住所)"]
	,["sensor_sound","一時間内に発生した音の数を求めます。(0-200が返されます)","sensor_sound()"]
	,["background_music_play","裏側で音楽かけます","background_music_play(音楽)"]
	,["background_music_stop","裏側でかけてる音楽を停止します","background_music_stop()"]
	,["wearher_info","天気の情報を取得する","wearher_info(天気を取得する住所)"]
	,["wearher_is_rain","雨が降るか？","wearher_is_rain(天気を取得する住所)"]
	,["wearher_is_rain_soon","もうすぐ雨が降るか？","wearher_is_rain_soon(天気を取得する住所)"]
	,["train_info","路線APIから情報を取得します","train_info(取得する路線名)"]
	,["train_is_delay","路線が遅れているか？","train_is_delay(取得する路線名)"]
	,["time_is_holiday","祝日かどうかを判定します。","time_is_holiday(UNIXTIME)"]
	,["time_day_info","その日について詳しい情報を返します。","time_day_info(UNIXTIME)"]
	,["time_local_to_gmt","ローカルタイムからGMT時刻に変換する","time_local_to_gmt(UNIXTIME)"]
	,["time_dayofweek","曜日を求める","time_dayofweek(UNIXTIME)"]
	,["time_gmt_to_local","GMTからローカルタイム時刻に変換する","time_gmt_to_local(UNIXTIME)"]
	,["trigger_is_enable","トリガーが有効かどうか","trigger_is_enable(トリガーID)"]
	,["trigger_change_enable","トリガーを有効/無効にする","trigger_change_enable(トリガーID,有効 true / 無効 false)"]

	,["twitter_tweet","twitterに投稿します","twitter_tweet(書き込みたい文字列);"]
	,["twitter_home_timeline","つぶやきを取得します","twitter_user_timeline(ユーザ名)"]
	,["twitter_user_timeline","タイムライン(フォローしている人たちのつぶやき)を取得します","twitter_home_timeline(ユーザ名)"]
	,["multiroom_list","複数台連携で連携している端末一覧の取得","multiroom_list()"]
	,["multiroom_fire","複数台連携で連携している端末へ命令の送信","multiroom_fire(UUID 1234-5678-1234,家電名 扇風機,操作 つける)"]
];


CodeMirror.defineMode("javascript", function(config, parserConfig) {
  var indentUnit = config.indentUnit;
  var jsonMode = parserConfig.json;
  var isTS = parserConfig.typescript;

  // Tokenizer

  var keywords = function(){
    function kw(type) {return {type: type, style: "keyword"};}
    var A = kw("keyword a"), B = kw("keyword b"), C = kw("keyword c");
    var operator = kw("operator"), atom = {type: "atom", style: "atom"};
    
    var jsKeywords = {
      "if": A, "while": A, "with": A, "else": B, "do": B, "try": B, "finally": B,
      "return": C, "break": C, "continue": C, "new": C, "delete": C, "throw": C,
      "var": kw("var"), "const": kw("var"), "let": kw("var"),
      "function": kw("function"), "catch": kw("catch"),
      "for": kw("for"), "switch": kw("switch"), "case": kw("case"), "default": kw("default"),
      "in": operator, "typeof": operator, "instanceof": operator,
      "true": atom, "false": atom, "null": atom, "undefined": atom, "NaN": atom, "Infinity": atom,
      "this": kw("this")
    };
    var buildin = atom = {type: "builtin", style: "builtin"};
	for ( var i = 0; i < g_fhckeyword.length ; i++ )
	{
        jsKeywords[ g_fhckeyword[i][0] ] = buildin;
	}


    // Extend the 'normal' keywords with the TypeScript language extensions
    if (isTS) {
      var type = {type: "variable", style: "variable-3"};
      var tsKeywords = {
        // object-like things
        "interface": kw("interface"),
        "class": kw("class"),
        "extends": kw("extends"),
        "constructor": kw("constructor"),

        // scope modifiers
        "public": kw("public"),
        "private": kw("private"),
        "protected": kw("protected"),
        "static": kw("static"),

        "super": kw("super"),

        // types
        "string": type, "number": type, "bool": type, "any": type
      };

      for (var attr in tsKeywords) {
        jsKeywords[attr] = tsKeywords[attr];
      }
    }

    return jsKeywords;
  }();

  var isOperatorChar = /[+\-*&%=<>!?|~^]/;

  function chain(stream, state, f) {
    state.tokenize = f;
    return f(stream, state);
  }

  function nextUntilUnescaped(stream, end) {
    var escaped = false, next;
    while ((next = stream.next()) != null) {
      if (next == end && !escaped)
        return false;
      escaped = !escaped && next == "\\";
    }
    return escaped;
  }

  // Used as scratch variables to communicate multiple values without
  // consing up tons of objects.
  var type, content;
  function ret(tp, style, cont) {
    type = tp; content = cont;
    return style;
  }

  function jsTokenBase(stream, state) {
    var ch = stream.next();
    if (ch == '"' || ch == "'")
      return chain(stream, state, jsTokenString(ch));
    else if (/[\[\]{}\(\),;\:\.]/.test(ch))
      return ret(ch);
    else if (ch == "0" && stream.eat(/x/i)) {
      stream.eatWhile(/[\da-f]/i);
      return ret("number", "number");
    }      
    else if (/\d/.test(ch) || ch == "-" && stream.eat(/\d/)) {
      stream.match(/^\d*(?:\.\d*)?(?:[eE][+\-]?\d+)?/);
      return ret("number", "number");
    }
    else if (ch == "/") {
      if (stream.eat("*")) {
        return chain(stream, state, jsTokenComment);
      }
      else if (stream.eat("/")) {
        stream.skipToEnd();
        return ret("comment", "comment");
      }
      else if (state.lastType == "operator" || state.lastType == "keyword c" ||
               /^[\[{}\(,;:]$/.test(state.lastType)) {
        nextUntilUnescaped(stream, "/");
        stream.eatWhile(/[gimy]/); // 'y' is "sticky" option in Mozilla
        return ret("regexp", "string-2");
      }
      else {
        stream.eatWhile(isOperatorChar);
        return ret("operator", null, stream.current());
      }
    }
    else if (ch == "#") {
      stream.skipToEnd();
      return ret("error", "error");
    }
    else if (isOperatorChar.test(ch)) {
      stream.eatWhile(isOperatorChar);
      return ret("operator", null, stream.current());
    }
    else {
      stream.eatWhile(/[\w\$_]/);
      var word = stream.current(), known = keywords.propertyIsEnumerable(word) && keywords[word];
      return (known && state.lastType != ".") ? ret(known.type, known.style, word) :
                     ret("variable", "variable", word);
    }
  }

  function jsTokenString(quote) {
    return function(stream, state) {
      if (!nextUntilUnescaped(stream, quote))
        state.tokenize = jsTokenBase;
      return ret("string", "string");
    };
  }

  function jsTokenComment(stream, state) {
    var maybeEnd = false, ch;
    while (ch = stream.next()) {
      if (ch == "/" && maybeEnd) {
        state.tokenize = jsTokenBase;
        break;
      }
      maybeEnd = (ch == "*");
    }
    return ret("comment", "comment");
  }

  // Parser

  var atomicTypes = {"atom": true, "number": true, "variable": true, "string": true, "regexp": true, "this": true};

  function JSLexical(indented, column, type, align, prev, info) {
    this.indented = indented;
    this.column = column;
    this.type = type;
    this.prev = prev;
    this.info = info;
    if (align != null) this.align = align;
  }

  function inScope(state, varname) {
    for (var v = state.localVars; v; v = v.next)
      if (v.name == varname) return true;
  }

  function parseJS(state, style, type, content, stream) {
    var cc = state.cc;
    // Communicate our context to the combinators.
    // (Less wasteful than consing up a hundred closures on every call.)
    cx.state = state; cx.stream = stream; cx.marked = null, cx.cc = cc;
  
    if (!state.lexical.hasOwnProperty("align"))
      state.lexical.align = true;

    while(true) {
      var combinator = cc.length ? cc.pop() : jsonMode ? expression : statement;
      if (combinator(type, content)) {
        while(cc.length && cc[cc.length - 1].lex)
          cc.pop()();
        if (cx.marked) return cx.marked;
        if (type == "variable" && inScope(state, content)) return "variable-2";
        return style;
      }
    }
  }

  // Combinator utils

  var cx = {state: null, column: null, marked: null, cc: null};
  function pass() {
    for (var i = arguments.length - 1; i >= 0; i--) cx.cc.push(arguments[i]);
  }
  function cont() {
    pass.apply(null, arguments);
    return true;
  }
  function register(varname) {
    function inList(list) {
      for (var v = list; v; v = v.next)
        if (v.name == varname) return true;
      return false;
    }
    var state = cx.state;
    if (state.context) {
      cx.marked = "def";
      if (inList(state.localVars)) return;
      state.localVars = {name: varname, next: state.localVars};
    } else {
      if (inList(state.globalVars)) return;
      state.globalVars = {name: varname, next: state.globalVars};
    }
  }

  // Combinators

  var defaultVars = {name: "this", next: {name: "arguments"}};
  function pushcontext() {
    cx.state.context = {prev: cx.state.context, vars: cx.state.localVars};
    cx.state.localVars = defaultVars;
  }
  function popcontext() {
    cx.state.localVars = cx.state.context.vars;
    cx.state.context = cx.state.context.prev;
  }
  function pushlex(type, info) {
    var result = function() {
      var state = cx.state;
      state.lexical = new JSLexical(state.indented, cx.stream.column(), type, null, state.lexical, info);
    };
    result.lex = true;
    return result;
  }
  function poplex() {
    var state = cx.state;
    if (state.lexical.prev) {
      if (state.lexical.type == ")")
        state.indented = state.lexical.indented;
      state.lexical = state.lexical.prev;
    }
  }
  poplex.lex = true;

  function expect(wanted) {
    return function(type) {
      if (type == wanted) return cont();
      else if (wanted == ";") return pass();
      else return cont(arguments.callee);
    };
  }

  function statement(type) {
    if (type == "var") return cont(pushlex("vardef"), vardef1, expect(";"), poplex);
    if (type == "keyword a") return cont(pushlex("form"), expression, statement, poplex);
    if (type == "keyword b") return cont(pushlex("form"), statement, poplex);
    if (type == "{") return cont(pushlex("}"), block, poplex);
    if (type == ";") return cont();
    if (type == "function") return cont(functiondef);
    if (type == "for") return cont(pushlex("form"), expect("("), pushlex(")"), forspec1, expect(")"),
                                      poplex, statement, poplex);
    if (type == "variable") return cont(pushlex("stat"), maybelabel);
    if (type == "switch") return cont(pushlex("form"), expression, pushlex("}", "switch"), expect("{"),
                                         block, poplex, poplex);
    if (type == "case") return cont(expression, expect(":"));
    if (type == "default") return cont(expect(":"));
    if (type == "catch") return cont(pushlex("form"), pushcontext, expect("("), funarg, expect(")"),
                                        statement, poplex, popcontext);
    return pass(pushlex("stat"), expression, expect(";"), poplex);
  }
  function expression(type) {
    if (atomicTypes.hasOwnProperty(type)) return cont(maybeoperator);
    if (type == "function") return cont(functiondef);
    if (type == "keyword c") return cont(maybeexpression);
    if (type == "(") return cont(pushlex(")"), maybeexpression, expect(")"), poplex, maybeoperator);
    if (type == "operator") return cont(expression);
    if (type == "[") return cont(pushlex("]"), commasep(expression, "]"), poplex, maybeoperator);
    if (type == "{") return cont(pushlex("}"), commasep(objprop, "}"), poplex, maybeoperator);
    return cont();
  }
  function maybeexpression(type) {
    if (type.match(/[;\}\)\],]/)) return pass();
    return pass(expression);
  }
    
  function maybeoperator(type, value) {
    if (type == "operator") {
      if (/\+\+|--/.test(value)) return cont(maybeoperator);
      if (value == "?") return cont(expression, expect(":"), expression);
      return cont(expression);
    }
    if (type == ";") return;
    if (type == "(") return cont(pushlex(")"), commasep(expression, ")"), poplex, maybeoperator);
    if (type == ".") return cont(property, maybeoperator);
    if (type == "[") return cont(pushlex("]"), expression, expect("]"), poplex, maybeoperator);
  }
  function maybelabel(type) {
    if (type == ":") return cont(poplex, statement);
    return pass(maybeoperator, expect(";"), poplex);
  }
  function property(type) {
    if (type == "variable") {cx.marked = "property"; return cont();}
  }
  function objprop(type, value) {
    if (type == "variable") {
      cx.marked = "property";
      if (value == "get" || value == "set") return cont(getterSetter);
    } else if (type == "number" || type == "string") {
      cx.marked = type + " property";
    }
    if (atomicTypes.hasOwnProperty(type)) return cont(expect(":"), expression);
  }
  function getterSetter(type) {
    if (type == ":") return cont(expression);
    if (type != "variable") return cont(expect(":"), expression);
    cx.marked = "property";
    return cont(functiondef);
  }
  function commasep(what, end) {
    function proceed(type) {
      if (type == ",") return cont(what, proceed);
      if (type == end) return cont();
      return cont(expect(end));
    }
    return function(type) {
      if (type == end) return cont();
      else return pass(what, proceed);
    };
  }
  function block(type) {
    if (type == "}") return cont();
    return pass(statement, block);
  }
  function maybetype(type) {
    if (type == ":") return cont(typedef);
    return pass();
  }
  function typedef(type) {
    if (type == "variable"){cx.marked = "variable-3"; return cont();}
    return pass();
  }
  function vardef1(type, value) {
    if (type == "variable") {
      register(value);
      return isTS ? cont(maybetype, vardef2) : cont(vardef2);
    }
    return pass();
  }
  function vardef2(type, value) {
    if (value == "=") return cont(expression, vardef2);
    if (type == ",") return cont(vardef1);
  }
  function forspec1(type) {
    if (type == "var") return cont(vardef1, expect(";"), forspec2);
    if (type == ";") return cont(forspec2);
    if (type == "variable") return cont(formaybein);
    return cont(forspec2);
  }
  function formaybein(_type, value) {
    if (value == "in") return cont(expression);
    return cont(maybeoperator, forspec2);
  }
  function forspec2(type, value) {
    if (type == ";") return cont(forspec3);
    if (value == "in") return cont(expression);
    return cont(expression, expect(";"), forspec3);
  }
  function forspec3(type) {
    if (type != ")") cont(expression);
  }
  function functiondef(type, value) {
    if (type == "variable") {register(value); return cont(functiondef);}
    if (type == "(") return cont(pushlex(")"), pushcontext, commasep(funarg, ")"), poplex, statement, popcontext);
  }
  function funarg(type, value) {
    if (type == "variable") {register(value); return isTS ? cont(maybetype) : cont();}
  }

  // Interface

  return {
    startState: function(basecolumn) {
      return {
        tokenize: jsTokenBase,
        lastType: null,
        cc: [],
        lexical: new JSLexical((basecolumn || 0) - indentUnit, 0, "block", false),
        localVars: parserConfig.localVars,
        globalVars: parserConfig.globalVars,
        context: parserConfig.localVars && {vars: parserConfig.localVars},
        indented: 0
      };
    },

    token: function(stream, state) {
      if (stream.sol()) {
        if (!state.lexical.hasOwnProperty("align"))
          state.lexical.align = false;
        state.indented = stream.indentation();
      }
      if (stream.eatSpace()) return null;
      var style = state.tokenize(stream, state);
      if (type == "comment") return style;
      state.lastType = type;
      return parseJS(state, style, type, content, stream);
    },

    indent: function(state, textAfter) {
      if (state.tokenize == jsTokenComment) return CodeMirror.Pass;
      if (state.tokenize != jsTokenBase) return 0;
      var firstChar = textAfter && textAfter.charAt(0), lexical = state.lexical;
      if (lexical.type == "stat" && firstChar == "}") lexical = lexical.prev;
      var type = lexical.type, closing = firstChar == type;
      if (type == "vardef") return lexical.indented + (state.lastType == "operator" || state.lastType == "," ? 4 : 0);
      else if (type == "form" && firstChar == "{") return lexical.indented;
      else if (type == "form") return lexical.indented + indentUnit;
      else if (type == "stat")
        return lexical.indented + (state.lastType == "operator" || state.lastType == "," ? indentUnit : 0);
      else if (lexical.info == "switch" && !closing)
        return lexical.indented + (/^(?:case|default)\b/.test(textAfter) ? indentUnit : 2 * indentUnit);
      else if (lexical.align) return lexical.column + (closing ? 0 : 1);
      else return lexical.indented + (closing ? 0 : indentUnit);
    },

    electricChars: ":{}",

    jsonMode: jsonMode
  };
});

CodeMirror.defineMIME("text/javascript", "javascript");
CodeMirror.defineMIME("text/ecmascript", "javascript");
CodeMirror.defineMIME("application/javascript", "javascript");
CodeMirror.defineMIME("application/ecmascript", "javascript");
CodeMirror.defineMIME("application/json", {name: "javascript", json: true});
CodeMirror.defineMIME("text/typescript", { name: "javascript", typescript: true });
CodeMirror.defineMIME("application/typescript", { name: "javascript", typescript: true });

  ////////////////////////////////////////////////////////////
  // line mark
  ////////////////////////////////////////////////////////////
  var WRAP_CLASS = "CodeMirror-activeline";
  var BACK_CLASS = "CodeMirror-activeline-background";

  CodeMirror.defineOption("styleActiveLine", false, function(cm, val, old) {
    var prev = old && old != CodeMirror.Init;
    if (val && !prev) {
      updateActiveLine(cm);
      cm.on("cursorActivity", updateActiveLine);
    } else if (!val && prev) {
      cm.off("cursorActivity", updateActiveLine);
      clearActiveLine(cm);
      delete cm._activeLine;
    }
  });
  
  function clearActiveLine(cm) {
    if ("_activeLine" in cm) {
      cm.removeLineClass(cm._activeLine, "wrap", WRAP_CLASS);
      cm.removeLineClass(cm._activeLine, "background", BACK_CLASS);
    }
  }

  function updateActiveLine(cm) {
    if (cm.somethingSelected())
    {
       var cur = cm.getCursor();
       var start = cm.getCursor("start");
       var end = cm.getCursor("end");
       if (start.line == end.line)
       {//one line.
           CodeMirror.showHint(cm, CodeMirror.javascriptHint,{ "isfullmatch": 1 });
       }
    }
    var line = cm.getLineHandle(cm.getCursor().line);
    if (cm._activeLine == line) return;
    clearActiveLine(cm);
    cm.addLineClass(line, "wrap", WRAP_CLASS);
    cm.addLineClass(line, "background", BACK_CLASS);
    cm._activeLine = line;
  }


  ////////////////////////////////////////////////////////////
  // hint popup
  ////////////////////////////////////////////////////////////
  CodeMirror.commands.autocomplete = function(cm) {
    CodeMirror.showHint(cm, CodeMirror.javascriptHint);
  }


  var Pos = CodeMirror.Pos;

  function forEach(arr, f) {
    for (var i = 0, e = arr.length; i < e; ++i) f(arr[i]);
  }
  
  function arrayContains(arr, item) {
    if (!Array.prototype.indexOf) {
      var i = arr.length;
      while (i--) {
        if (arr[i] === item) {
          return true;
        }
      }
      return false;
    }
    return arr.indexOf(item) != -1;
  }

  function scriptHint(editor,  getToken, options) {
    // Find the token at the cursor
    var cur = editor.getCursor(), token = getToken(editor, cur), tprop = token;
    // If it's not a 'word-style' token, ignore the token.
		if (!/^[\w$_]*$/.test(token.string)) {
      token = tprop = {start: cur.ch, end: cur.ch, string: "", state: token.state,
                       type: token.string == "." ? "property" : null};
    }
    // If it is a property, find out what it is a property of.
    while (tprop.type == "property") {
      tprop = getToken(editor, Pos(cur.line, tprop.start));
      if (tprop.string != ".") return;
      tprop = getToken(editor, Pos(cur.line, tprop.start));
      if (tprop.string == ')') {
        var level = 1;
        do {
          tprop = getToken(editor, Pos(cur.line, tprop.start));
          switch (tprop.string) {
          case ')': level++; break;
          case '(': level--; break;
          default: break;
          }
        } while (level > 0);
        tprop = getToken(editor, Pos(cur.line, tprop.start));
	if (tprop.type.indexOf("variable") === 0)
	  tprop.type = "function";
	else return; // no clue
      }
      if (!context) var context = [];
      context.push(tprop);
    }
    return {list: getCompletions(token.string, options),
            from: Pos(cur.line, token.start),
            to: Pos(cur.line, token.end)};
  }

  CodeMirror.javascriptHint = function(editor, options) {
    if (editor.somethingSelected())
    {
       var cur = editor.getCursor();
       var str = editor.getSelection();
//       getCursor("start"), cm.getCursor("end")

       return {list: getCompletions(str , options),
               from: editor.getCursor("start"),
               to: editor.getCursor("end")};
    }
    else
    {
       return scriptHint(editor, 
                         function (e, cur) {return e.getTokenAt(cur);},
                         options);
    }
  };

  function getCompletions(str,  options) {
    var found = [];
    function maybeAdd(obj) {
      if (obj[0].length <= 0) return ;
      if (options.isfullmatch)
      {
         if (obj[0] != str ) return ;
      }
      else
      {
         if (obj[0].indexOf(str) != 0 ) return ;
      }
      found.push(obj);
    }
    for ( var i = 0; i < g_fhckeyword.length ; i++ )
    {
         maybeAdd(g_fhckeyword[i]);
    }
    return found;
  }


CodeMirror.showHint = function(cm, getHints, options) {
  if (!options) options = {};
  var startCh = cm.getCursor().ch, continued = false;
  var closeOn = options.closeCharacters || /[\s()\[\]{};:]/;

  function startHinting() {
    // We want a single cursor position.
//    if (cm.somethingSelected()) return;

    if (options.async)
      getHints(cm, showHints, options);
    else
      return showHints(getHints(cm, options));
  }

  function getText(completion) {
    if (typeof completion == "string") return completion;
    else return completion.text;
  }

  function pickCompletion(cm, data, completion) {
    if (completion.hint) completion.hint(cm, data, completion);
    else cm.replaceRange(getText(completion), data.from, data.to);
  }

  function showHints(data) {
    if (!data || !data.list.length) return;
    var completions = data.list;

    // Build the select widget
    var hints = document.createElement("ul"), selectedHint = 0;
    hints.className = "CodeMirror-hints";
    for (var i = 0; i < completions.length; ++i) {
      var elt = hints.appendChild(document.createElement("li")), completion = completions[i];
      var className = "CodeMirror-hint" + (i ? "" : " CodeMirror-hint-active");
      elt.className = className;

      var divTitle = document.createElement("span"); divTitle.className = "CodeMirror-hint-title"; divTitle.appendChild(document.createTextNode(completion[2]));
      var divMemo = document.createElement("span");  divMemo.className = "CodeMirror-hint-memo";   divMemo.appendChild(document.createTextNode(completion[1]));
      var aLink = document.createElement("a");  aLink.className = "CodeMirror-hint-link";   aLink.appendChild(document.createTextNode("help")); aLink.href="http://rti-giken.jp/fhc/help/script/" + completion[0] + ".html"; aLink.setAttribute("target","_blank");
      
      elt.appendChild(divTitle);
      elt.appendChild(divMemo);
      elt.appendChild(aLink);

      elt.hintId = i;
    }
    var pos = cm.cursorCoords(options.alignWithWord !== false ? data.from : null);
    var left = pos.left, top = pos.bottom, below = true;
    hints.style.left = left + "px";
    hints.style.top = top + "px";
    document.body.appendChild(hints);

    // If we're at the edge of the screen, then we want the menu to appear on the left of the cursor.
    var winW = window.innerWidth || Math.max(document.body.offsetWidth, document.documentElement.offsetWidth);
    var winH = window.innerHeight || Math.max(document.body.offsetHeight, document.documentElement.offsetHeight);
    var box = hints.getBoundingClientRect();
    var overlapX = box.right - winW, overlapY = box.bottom - winH;
    if (overlapX > 0) {
      if (box.right - box.left > winW) {
        hints.style.width = (winW - 5) + "px";
        overlapX -= (box.right - box.left) - winW;
      }
      hints.style.left = (left = pos.left - overlapX) + "px";
    }
    if (overlapY > 0) {
      var height = box.bottom - box.top;
      if (box.top - (pos.bottom - pos.top) - height > 0) {
        overlapY = height + (pos.bottom - pos.top);
        below = false;
      } else if (height > winH) {
        hints.style.height = (winH - 5) + "px";
        overlapY -= height - winH;
      }
      hints.style.top = (top = pos.bottom - overlapY) + "px";
    }

    function changeActive(i) {
      i = Math.max(0, Math.min(i, completions.length - 1));
      if (selectedHint == i) return;
      var node = hints.childNodes[selectedHint];
      node.className = node.className.replace(" CodeMirror-hint-active", "");
      node = hints.childNodes[selectedHint = i];
      node.className += " CodeMirror-hint-active";
      if (node.offsetTop < hints.scrollTop)
        hints.scrollTop = node.offsetTop - 3;
      else if (node.offsetTop + node.offsetHeight > hints.scrollTop + hints.clientHeight)
        hints.scrollTop = node.offsetTop + node.offsetHeight - hints.clientHeight + 3;
    }

    function screenAmount() {
      return Math.floor(hints.clientHeight / hints.firstChild.offsetHeight) || 1;
    }

    var ourMap = {
      Up: function() {changeActive(selectedHint - 1);},
      Down: function() {changeActive(selectedHint + 1);},
      PageUp: function() {changeActive(selectedHint - screenAmount());},
      PageDown: function() {changeActive(selectedHint + screenAmount());},
      Home: function() {changeActive(0);},
      End: function() {changeActive(completions.length - 1);},
      Enter: pick,
      Tab: pick,
      Esc: close
    };
    if (options.customKeys) for (var key in options.customKeys) if (options.customKeys.hasOwnProperty(key)) {
      var val = options.customKeys[key];
      if (/^(Up|Down|Enter|Esc)$/.test(key)) val = ourMap[val];
      ourMap[key] = val;
    }

    cm.addKeyMap(ourMap);
    cm.on("cursorActivity", cursorActivity);
    var closingOnBlur;
    function onBlur(){ closingOnBlur = setTimeout(close, 100); };
    function onFocus(){ clearTimeout(closingOnBlur); };
    cm.on("blur", onBlur);
    cm.on("focus", onFocus);
    var startScroll = cm.getScrollInfo();
    function onScroll() {
      var curScroll = cm.getScrollInfo(), editor = cm.getWrapperElement().getBoundingClientRect();
      var newTop = top + startScroll.top - curScroll.top, point = newTop;
      if (!below) point += hints.offsetHeight;
      if (point <= editor.top || point >= editor.bottom) return close();
      hints.style.top = newTop + "px";
      hints.style.left = (left + startScroll.left - curScroll.left) + "px";
    }
    cm.on("scroll", onScroll);
    CodeMirror.on(hints, "click", function(e) {
      var t = e.target || e.srcElement;
      if (t.hintId != null) {selectedHint = t.hintId; pick();}
    });
    CodeMirror.on(hints, "mousedown", function() {
      setTimeout(function(){cm.focus();}, 20);
    });

    var done = false, once;
    function close() {
      if (done) return;
      done = true;
      clearTimeout(once);
      hints.parentNode.removeChild(hints);
      cm.removeKeyMap(ourMap);
      cm.off("cursorActivity", cursorActivity);
      cm.off("blur", onBlur);
      cm.off("focus", onFocus);
      cm.off("scroll", onScroll);
    }
    function pick() {
      pickCompletion(cm, data, completions[selectedHint][0]);
      close();
    }
    var once, lastPos = cm.getCursor(), lastLen = cm.getLine(lastPos.line).length;
    function cursorActivity() {
      clearTimeout(once);

      var pos = cm.getCursor(), line = cm.getLine(pos.line);
      if (pos.line != lastPos.line || line.length - pos.ch != lastLen - lastPos.ch ||
          pos.ch < startCh || cm.somethingSelected() ||
          (pos.ch && closeOn.test(line.charAt(pos.ch - 1))))
        close();
      else
        once = setTimeout(function(){close(); continued = true; startHinting();}, 70);
    }
    return true;
  }

  return startHinting();
};





  ////////////////////////////////////////////////////////////
  // matchbraket
  ////////////////////////////////////////////////////////////
  var ie_lt8 = /MSIE \d/.test(navigator.userAgent) &&
    (document.documentMode == null || document.documentMode < 8);

  var Pos = CodeMirror.Pos;

  var matching = {"(": ")>", ")": "(<", "[": "]>", "]": "[<", "{": "}>", "}": "{<"};
  function findMatchingBracket(cm) {
    var maxScanLen = cm.state._matchBrackets.maxScanLineLength || 10000;

    var cur = cm.getCursor(), line = cm.getLineHandle(cur.line), pos = cur.ch - 1;
    var match = (pos >= 0 && matching[line.text.charAt(pos)]) || matching[line.text.charAt(++pos)];
    if (!match) return null;
    var forward = match.charAt(1) == ">", d = forward ? 1 : -1;
    var style = cm.getTokenAt(Pos(cur.line, pos + 1)).type;

    var stack = [line.text.charAt(pos)], re = /[(){}[\]]/;
    function scan(line, lineNo, start) {
      if (!line.text) return;
      var pos = forward ? 0 : line.text.length - 1, end = forward ? line.text.length : -1;
      if (line.text.length > maxScanLen) return null;
      var checkTokenStyles = line.text.length < 1000;
      if (start != null) pos = start + d;
      for (; pos != end; pos += d) {
        var ch = line.text.charAt(pos);
        if (re.test(ch) && (!checkTokenStyles || cm.getTokenAt(Pos(lineNo, pos + 1)).type == style)) {
          var match = matching[ch];
          if (match.charAt(1) == ">" == forward) stack.push(ch);
          else if (stack.pop() != match.charAt(0)) return {pos: pos, match: false};
          else if (!stack.length) return {pos: pos, match: true};
        }
      }
    }
    for (var i = cur.line, found, e = forward ? Math.min(i + 100, cm.lineCount()) : Math.max(-1, i - 100); i != e; i+=d) {
      if (i == cur.line) found = scan(line, i, pos);
      else found = scan(cm.getLineHandle(i), i);
      if (found) break;
    }
    return {from: Pos(cur.line, pos), to: found && Pos(i, found.pos), match: found && found.match};
  }

  function matchBrackets(cm, autoclear) {
    // Disable brace matching in long lines, since it'll cause hugely slow updates
    var maxHighlightLen = cm.state._matchBrackets.maxHighlightLineLength || 1000;
    var found = findMatchingBracket(cm);
    if (!found || cm.getLine(found.from.line).length > maxHighlightLen ||
       found.to && cm.getLine(found.to.line).length > maxHighlightLen)
      return;

    var style = found.match ? "CodeMirror-matchingbracket" : "CodeMirror-nonmatchingbracket";
    var one = cm.markText(found.from, Pos(found.from.line, found.from.ch + 1), {className: style});
    var two = found.to && cm.markText(found.to, Pos(found.to.line, found.to.ch + 1), {className: style});
    // Kludge to work around the IE bug from issue #1193, where text
    // input stops going to the textare whever this fires.
    if (ie_lt8 && cm.state.focused) cm.display.input.focus();
    var clear = function() {
      cm.operation(function() { one.clear(); two && two.clear(); });
    };
    if (autoclear) setTimeout(clear, 800);
    else return clear;
  }

  var currentlyHighlighted = null;
  function doMatchBrackets(cm) {
    cm.operation(function() {
      if (currentlyHighlighted) {currentlyHighlighted(); currentlyHighlighted = null;}
      if (!cm.somethingSelected()) currentlyHighlighted = matchBrackets(cm, false);
    });
  }

  CodeMirror.defineOption("matchBrackets", false, function(cm, val, old) {
    if (old && old != CodeMirror.Init)
      cm.off("cursorActivity", doMatchBrackets);
    if (val) {
      cm.state._matchBrackets = typeof val == "object" ? val : {};
      cm.on("cursorActivity", doMatchBrackets);
    }
  });

  CodeMirror.defineExtension("matchBrackets", function() {matchBrackets(this, true);});
  CodeMirror.defineExtension("findMatchingBracket", function(){return findMatchingBracket(this);});




  ////////////////////////////////////////////////////////////
  // selection hint
  ////////////////////////////////////////////////////////////
  CodeMirror.defineOption("selectionHint", false, function(cm, val, old) {
    var prev = old && old != CodeMirror.Init;
    if (val && !prev) {
      updateSelectedText(cm);
      cm.on("cursorActivity", updateSelectedText);
    } else if (!val && prev) {
      cm.off("cursorActivity", updateSelectedText);
    }
  });


  function updateSelectedText(cm) {

    if (cm.somethingSelected())
    {
      var aa = cm.getSelection();
      CodeMirror.showHint(cm, CodeMirror.javascriptHint);
    }
  }


})();
