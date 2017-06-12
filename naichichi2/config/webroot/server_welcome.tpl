<!DOCTYPE html>
<html lang="ja"><head>
	<meta name="format-detection" content="telephone=no" />
	<meta name="viewport" content="width=640,user-scalable=yes,maximum-scale=0.67" />
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=99" />
	<link rel="apple-touch-icon" href="/jscss/images/iphone_icon.png" />
	<link rel="shortcut icon" href="/jscss/images/favicon.ico" />
	<title>新規登録</title>
	<!-- このコードは GPL でライセンスされてます。 -->
	<base href="/"></base>
	<style>
	h1,h2,h3,h4,h5,h6{
		font-size: 100%;
		font-weight: normal;
		padding:10px;
		margin:0;
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
	
	#welcome {
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
		color: #333 ;
		text-shadow: 0  1px  0  white ;
		background: url(/jscss/images/bodybg.gif) repeat-y scroll center 0 white;

		%BCSS%border-radius: .6em ;
		border-radius: .6em ;
	}

	.footermenu{
		margin-left: 60px;
	}

	.footermenu_buton {
		box-shadow: 0 2px 6px rgba(0,0,0,0.5), inset 0 1px rgba(255,255,255,0.3), inset 0 10px rgba(255,255,255,0.2), inset 0 10px 20px rgba(255,255,255,0.25), inset 0 -15px 30px rgba(0,0,0,0.1);
		%BCSS%box-shadow: 0 2px 6px rgba(0,0,0,0.5), inset 0 1px rgba(255,255,255,0.3), inset 0 10px rgba(255,255,255,0.2), inset 0 10px 20px rgba(255,255,255,0.25), inset 0 -15px 30px rgba(0,0,0,0.1);

		border-radius: 10px;
		%BCSS%border-radius: 10px;

		height: 64px;
		margin: 10px;
		padding: 10px;
		border: 1px solid #ccc;
		background: #ffd;
		width: 400px;
		font-size: 1.6em;
		float: left;
		position: relative;
	}
	.footermenu_buton:hover {
		cursor: pointer;
		border: 1px solid rgba(0,0,0,0.2);
		background: rgba(255,240,112,1);
	}
	.footermenu_buton .footermenu_buton_icon {
		position:		absolute;
		top:  10px;
		left: 10px;
		width: 64px;
		height: 64px;
	}
	.footermenu_buton .footermenu_buton_name {
		position:		absolute;
		top:  28px;
		left: 78px;
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

	.footermenu{
		margin-left: 0px;
	}
	.footermenu_buton {
		width: 270px;
		font-size: 1.1em;
	}
}
	</style>
</head>
<body>
<div id="welcome" class="page">
	<div class="header_pc"> 
		<h1><a href="/" ><img src="/jscss/images/fhc700.png" /></a></h1>
	</div>

	<div class="contents_wrapper">
		<div data-role="header" class="header_title">
			<fieldset class="navigationbar">
				<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_returnremocon.png" /></div>
				<div class="navigationbar_title"><span class="navigationbar_title_span">未来へようこそ</span></div>
			</fieldset>
		</div>
		<div class="contents_main">
			<br />
			<center>
			<img src="/jscss/images/big_logo.jpg" />
			</center>
			<br />
			future home contorller を購入いただきありがとうございます。<br />
			これよりセットアップ手続きを行います。<br />
			<br />
			新規登録の方は、「新規登録を開始する」を、<br />
			端末の設定をリセットされる場合は、「既存端末をリセットする」を選択してください。<br />
			<br />
		</div><!-- /contents_main -->
		<div class="footermenu">
			<div class="footermenu_buton" onclick="welcome_newregist_button();">
				<div class="footermenu_buton_icon"><img src="/jscss/images/setting_icon64_newregist.png" /></div>
				<div class="footermenu_buton_name">新規登録を開始する</div>
			</div>
			<div class="footermenu_buton" onclick="welcome_reset_button();">
				<div class="footermenu_buton_icon"><img src="/jscss/images/setting_icon64_network.png" /></div>
				<div class="footermenu_buton_name">既存端末をリセットする</div>
			</div>
			<br class="clearfix">
		</div>
		<br class="clearfix">
		<br />
	</div><!-- /contents_wrapper -->
</div><!-- /page top -->
</body>


<script type="text/javascript" src="/jscss/jquery.js"></script>
<script>

//新規登録
function welcome_newregist_button()
{
	url = "/welcome/newregist" 
		+ "?model=" + encodeURI(getparam("model",""))
		+ "&backurl=" + encodeURI(getparam("backurl","")) 
		+ "&isconnected=" + encodeURI(getparam("isconnected","")) 
		;
	window.location.href = url;
}

//リセット
function welcome_reset_button()
{
	url = "/auth/?backurl=/networksetting/" ;
	window.location.href = url;
}



$(function(){
});


function getparam(name,def)
{
	var vars = [], hash;
	var hashes = window.location.href.slice(window.location.href.indexOf('?') + 1).split('&');
	for(var i = 0; i <hashes.length; i++)
	{
		hash = hashes[i].split('=');
		if (hash[0] == name)
		{
			if (hash[1])
			{
				return hash[1];
			}
		}
	}
	return def;
}

</script>
</html>