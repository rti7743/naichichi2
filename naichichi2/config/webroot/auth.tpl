<!DOCTYPE html>
<html lang="ja"><head>
	<meta name="format-detection" content="telephone=no" />
	<meta name="viewport" content="width=640,user-scalable=yes" />
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=99" />
	<link rel="apple-touch-icon" href="/jscss/images/iphone_icon.png" />
	<link rel="shortcut icon" href="/jscss/images/favicon.ico" />
	<title>ログイン</title>
	<!-- このコードは GPL でライセンスされてます。 -->
	<base href="/"></base>
	<style>
	h1,h2,h3,h4,h5,h6{
		font-size: 100%;
		font-weight: normal;
	}	
	::-webkit-input-placeholder {
		color:    #999;
	}
	:-moz-placeholder {
		color:    #999;
	}	
	::-moz-placeholder {
		color:    #999;
	}	
	:-ms-input-placeholder {
		color:    #999;
	}	
	:-o-placeholder {
		color:    #999;
	}	

	html{
		height:100%;
		min-height: 100%;
	}
	body{
		font-size: 16px;
		font-family: Helvetica, Arial, sans-serif ;
		margin: 0;
		padding: 0;
		height:100%; 
	}
	
	body > .page {
		height: auto;
	}

	.header_pc{
		margin:0 auto;
		border: 1px solid #AAA ;
		background: white ;
		color: black ;
		font-weight: bold;
		text-shadow: 0  -1px  1px  black ;
		background-image: -webkit-gradient(linear, left top, left bottom, from( #3C3C3C ), to( #111 ));
		background-image: %BCSS%linear-gradient( white , #CCC );
		background-image: linear-gradient( white , #CCC );
		text-align:center;
		border-width: 0 0 1px 0;
	}
	.header_pc img{
		border-style:none;
	}
	

	.button {
		width:500px;

		padding: .6em 20px;
		display: block;
		text-overflow: ellipsis;
		overflow: hidden;

		%BCSS%border-radius: 1em ;
		border-radius: 1em ;
		margin:0 auto;

		border: 1px solid #AAA ;
		background: white ;
		color: black ;
		background-image: -webkit-gradient(linear, left top, left bottom, from( #3C3C3C ), to( #111 ));
		background-image: %BCSS%linear-gradient( white , #CCC );
		background-image: linear-gradient( white , #CCC );
	}

	.button:hover {
		background-image: -webkit-gradient(linear, left top, left bottom, from( #CCCCCC ), to( #111 ));
		background-image: %BCSS%linear-gradient( white , #999 );
		background-image: linear-gradient( white , #999 );
	}

	.clearfix{
		clear:both;
		float:none;
	}
	
	#login {
		min-height: 100%;
		height: 100%;
	}

	.contents_wrapper {
		width:1000px;
		margin:0 auto;
		min-height: 100%;
		height: 100%;
		background: url("/jscss/images/bodybg.gif") repeat-y scroll center 0 !important;
		background-color: #ffffff;
	}
	.contents_main {
		margin:0px 70px;
	}

	.header_title { /* ヘッダー 要素を全力で消す。 ただし下線 border-buttom だけ残します */
		width: 950px;
		margin: 0 auto;
		position: relative;
		font-family: Helvetica, Arial, sans-serif; ;
		border-bottom: 1px solid #AAA;
		border-top: none;
		border-left: none;
		border-right: none;
		color: black; ;
		font-weight: bold;
		text-shadow: 0;  -1px  1px  black;
	}
	.navigationbar {
		height: 64px;
		border: none;
		border-width: 0;
		padding: 0;
		margin: 0;
	}
	.navigationbar .navigationbar_backbutton {
		width: 100px;
		display:inline-table;
		vertical-align:middle;
	}

	.navigationbar .navigationbar_icon {
		display:inline-table;
		vertical-align:middle;
		width: 64px;
	}
	.navigationbar .navigationbar_title {
		display:inline-table;
		vertical-align:middle;
		width: 800px;
	}
	.navigationbar .navigationbar_title .navigationbar_title_span {
		display:inline-table;
		vertical-align:middle;
		font-size:1.8em;
	}
	.sub_head {
		border-left:5px solid #1871B8;padding:5px;
		margin-top:10px;
		margin-bottom:10px;
	}
	.error {
		display: none;
		color: red;
		padding:10px;
	}
	
	.minput_tag {
		display: block;

		%BCSS%appearance: none;
		appearance: none;

		background-image: none;
		padding: .4em;
		line-height: 1.4;
		width: 50%;
		outline: 0;
		margin: .25em 5px;
		%BCSS%box-shadow: inset 0px 1px 4px rgba(0, 0, 0, .2);
		box-shadow: inset 0px 1px 4px rgba(0, 0, 0, .2);

		%BCSS%background-clip: padding-box;background-clip: padding-box;
		border: 1px solid #AAA ;
		text-shadow: 0  1px  0  white ;
		background: url(/jscss/images/bodybg.gif) repeat-y scroll center 0 white;

		%BCSS%border-radius: .6em ;
		border-radius: .6em ;
	}

	.idpass_name {
		width: 150px;
		display:inline-table;
		vertical-align:middle;
		font-size: 1.2em;
	}
	.idpass_name .cell {
		display:table-cell;
		vertical-align:middle;
	}
	
	.idpass_input {
		width: 600px;
		display:inline-table;
		vertical-align:middle;
	}
	.idpass_input .cell {
		display:table-cell;
		vertical-align:middle;
	}

	.dialog {
		display: none;
		font-size: 1.6em;
		position: absolute;
		background-color: white;
		color:	black ;
		
		top: 50%;
		left: 50%;
		width: 760px;
		height: 340px;
		margin-left: -380px;
		margin-top: -220px;
		border: 10px solid #AAA ;
	}

/* for mobile */
@media screen and (max-device-width: 640px), screen and (max-width: 640px) {
	body {
		font-size:22px;
		font-family:Helvetica, HiraKakuProN-W3, sans-serif;
		line-height:1.4;
	}

	.contents_wrapper {
		width:640px;
		margin:0 auto;
	}
	.contents_main {
		margin:0px 20px;
	}
	.header_title {
		width:640px;
	}
	.header_pc {
		display: none;
	}

	.button {
		font-size: 1.2em;
		display: inline;
		width: 400px;
	}

	.navigationbar {
		height: 64px;
		border: none;
		border-width: 0;
		padding: 0;
		margin: 0;
	}
	.navigationbar .navigationbar_backbutton {
		width: 100px;
		display:inline-table;
		vertical-align:middle;
	}
	.navigationbar .navigationbar_icon {
		display:inline-table;
		vertical-align:middle;
		width: 64px;
	}
	.navigationbar .navigationbar_title {
		display:inline-table;
		vertical-align:middle;
		width: 520px;
	}
	.navigationbar .navigationbar_title .navigationbar_title_span {
		display:inline-table;
		vertical-align:middle;
		font-size:1.8em;
	}
	.minput_tag {
		font-size: 1.2em;
		width: 70%;
	}

	.dialog {
		display: none;
		font-size: 1.6em;
		position: absolute;
		background-color: white;
		color:	black ;
		
		top: 50%;
		left: 50%;
		width: 600px;
		height: 500px;
		margin-left: -310px;
		margin-top: -260px; 
		border: 10px solid #AAA ;
	}
}
	</style>
</head>
<body>
<div id="login" class="page">
	<div class="header_pc"> 
		<h1><a href="/"><img src="/jscss/images/fhc700.png" /></a></h1>
	</div>

	<div class="contents_wrapper">
		<div data-role="header" class="header_title">
			<fieldset class="navigationbar">
				<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_auth.png" /></div>
				<div class="navigationbar_title"><span class="navigationbar_title_span">ログイン</span></div>
			</fieldset>
		</div>
		<div class="contents_main">
			<p>ログインしてください。</p>
			<form action="/auth/" method="post" id="login_form">
				<div>
					<div class="idpass_name"  ><span class="cell">mailaddress</span></div>
					<div class="idpass_input" ><span class="cell"><input type="email" class="minput_tag" name="id" value="%ID%" placeholder="mailaddressを入れてください"></span></div>
				</div>
				<div>
					<div class="idpass_name"  ><span class="cell">password</span></div>
					<div class="idpass_input" ><span class="cell"><input type="password" class="minput_tag" name="password" value="%PASSWORD%" placeholder="passwordを入れてください"></span></div>
				</div>
				<br />
				<input type="hidden" name="uuidoverraide" id="login_uuidoverraide" value="0" />
				
				<div style="display: none;"></div>
				<input type="submit" class="button" value="ログインする">
				<br />
				<br />
				<input type="hidden" name="backurl" value="%BACKURL%">
				<a href="https://fhc.rti-giken.jp/passwordreset/" target"_blank">パスワードを忘れたので、リセットする</a>
			</form>
				
			<div id="login_40001_error" class="error" style="display: %LOGIN_ERROR_DISPLAY%;">
				ログインできませんでした。 IDまたは、パスワードを確認してください。<br />
			</div>
		</div><!-- /contents_main -->
	</div><!-- /contents_wrapper -->
	<div style="display:none;" id="login_not_modern_dialog" class="dialog">
		このブラウザは古過ぎてモダンではないのでサポートしていません。<br />
		<br />
		サポート範囲<br />
		IE 10+ Firefox 4+ Safari 5+ Chrome 10+ Opera 11+ Mobile Safari<br />
		<br />
		<div>
			<div><a href="javascript:void(0)" onclick="$('#login_not_modern_dialog').hide(); "><span class="button">それでもはじめる</span></a></div>
			<br />
			<div><a href="https://www.google.com/intl/ja/chrome/browser/" ><span class="button">もっとモダンなブラウザをダウンロードする。</span></a></div>
			<br />
		</div>
	</div>
	<div style="display:none;" id="login_uuid_error_dialog" class="dialog">
		このIDは別の端末に関連付けられています。<br />
		別端末との関連付けを解除して、この端末で使えるようにしますか？<br />
		<br />
		<br />
		<div>
			<div><a href="javascript:void(0)" onclick="uuid_overraide(); "><span class="button">利用できるようにする</span></a></div>
			<br />
			<div><a href="javascript:void(0)" onclick="$('#login_uuid_error_dialog').hide(); "><span class="button">キャンセルする</span></a></div>
			<br />
		</div>
	</div>


</div><!-- /page top -->
</body>

<script type="text/javascript" src="/jscss/jquery.js"></script>
<script>
g_loginObject = null;

$(function(){
	g_loginObject = $("#login");

	if (%FIRST_CALL%)
	{
		//古すぎるブラウザの場合は警告を出す.
		if ( !checkModern() )
		{
			$("#login_not_modern_dialog").show();
		}
	}
	if (%UUID_ERROR_DISPLAY%)
	{
		$("#login_uuid_error_dialog").show();
	}
});

//uuidを上書きしてログイン
function uuid_overraide()
{
	var loginFormObject = $("#login_form");
	loginFormObject.find("#login_uuidoverraide").val(1);
	loginFormObject.submit();
}

//モダンなブラウザかどうか判別する
function checkModern()
{
	if (jQuery.browser.msie && parseInt(jQuery.browser.version) < 10 )
	{
		return false;
	}
	if (jQuery.browser.mozilla && parseInt(jQuery.browser.version) < 4 )
	{
		return false;
	}
	if (jQuery.browser.safari && parseInt(jQuery.browser.version) < 5 )
	{
		return false;
	}
	if (jQuery.browser.chrome && parseInt(jQuery.browser.version) < 10 )
	{
		return false;
	}
	if (jQuery.browser.opera && parseInt(jQuery.browser.version) < 11 )
	{
		return false;
	}

	//よくわからんけどok
	return true;
}

</script>
</html>