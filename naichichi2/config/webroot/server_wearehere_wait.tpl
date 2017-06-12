<!DOCTYPE html>
<html lang="ja"><head>
	<meta name="format-detection" content="telephone=no" />
	<meta name="viewport" content="width=640,user-scalable=yes,maximum-scale=0.67" />
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=99" />
	<link rel="apple-touch-icon" href="/jscss/images/iphone_icon.png" />
	<link rel="shortcut icon" href="/jscss/images/favicon.ico" />
	<title>端末との接続</title>
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
		height: 100%;
	}
	body{
		margin: 0;
		padding: 0;
		height:100%; 
	}
	
	body > #remoconstatic {
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
		padding: .6em 20px;
		min-width: .75em;
		display: block;
		text-overflow: ellipsis;
		overflow: hidden;

		%BCSS%border-radius: 1em ;
		border-radius: 1em ;

		width:500px;
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
	
	#wearehere {
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
		margin:0px 20px;
	}

	.header_title { 
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
		text-shadow: 0;  -1px  1px  black ;
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
		font-size: 14px;
		%BCSS%appearance: none;
		appearance: none;

		background-image: none;
		padding: .4em;
		line-height: 1.4;
		display: block;
		width: 50%;
		outline: 0;
		font-family: Helvetica, Arial, sans-serif ;
		margin: .25em 5px;

		%BCSS%box-shadow: inset 0px 1px 4px rgba(0, 0, 0, .2);
		box-shadow: inset 0px 1px 4px rgba(0, 0, 0, .2);

		%BCSS%background-clip: padding-box;background-clip: padding-box;
		background-clip: padding-box;background-clip: padding-box;

		border: 1px solid #AAA ;
		color: #333 ;
		text-shadow: 0  1px  0  white ;
		background: url(/jscss/images/bodybg.gif) repeat-y scroll center 0 white;

		%BCSS%border-radius: .6em ;
		border-radius: .6em ;
	}

	.mselect_tag {
		font-size: 14px;

		padding: .4em;
		line-height: 1.4;
		display: block;
		width: 50%;
		outline: 0;
		font-family: Helvetica, Arial, sans-serif ;
		margin: .25em 5px;

		%BCSS%box-shadow: inset 0px 1px 4px rgba(0, 0, 0, .2);
		box-shadow: inset 0px 1px 4px rgba(0, 0, 0, .2);

		%BCSS%background-clip: padding-box;background-clip: padding-box;
		background-clip: padding-box;background-clip: padding-box;

		border: 1px solid #AAA ;
		text-shadow: 0  1px  0  white ;

		%BCSS%border-radius: .6em ;
		border-radius: .6em ;

		background: white ;
		color: black ;
		background-image: -webkit-gradient(linear, left top, left bottom, from( #3C3C3C ), to( #111 ));
		background-image: %BCSS%linear-gradient( white , #CCC );
		background-image: linear-gradient( white , #CCC );
	}

	.mselect_tag:hover {
		cursor: pointer;
		background-image: -webkit-gradient(linear, left top, left bottom, from( #CCCCCC ), to( #111 ));
		background-image: %BCSS%linear-gradient( white , #999 );
		background-image: linear-gradient( white , #999 );
	}

	/* http://jsdo.it/kkotaro0111/jdUJ */
	.papertag{
		margin:0;
		padding:10px;
		background-color:rgba(0, 0, 0, .1);
		width:500px;
		border-left:15px solid #eeeeee;
		padding:15px 20px;
		position:relative;
		z-index:1;
		margin-bottom:20px;
	}
	.papertag p {
		%BCSS%transform: skew(0, -1deg);
		transform: skew(0, -1deg);
		margin-top:-0.5em;
		margin-left:-1em;
	}

	.papertag:before{
		margin:0;
		padding:0;
		content:" ";
		width:100%;
		height:100%;
		position:absolute;
		top:0px;
		left:0px;
		z-index:-2;
		box-shadow: 6px 2px 4px rgba(0, 0, 0, .1);
	}

	.papertag:after{
		margin:0;
		padding:0;
		content:" ";
		width:100%;
		height:100%;
		position:absolute;
		top:-5px;
		left:0px;
		z-index:-1;
		background-color:#eeeeee;

		%BCSS%transform: skew(0, -1deg);
		transform: skew(0, -1deg);
	}

	.papertag.yellow{
		margin:0;
		padding:10px;
		border-left:15px solid #F5F2BA;
	}
	.papertag.yellow:after{
		margin:0;
		padding:0;
		 background-color:#F5F2BA;
	}

	.papertag.green{
		margin:0;
		padding:10px;
		border-left:15px solid #D0F5D4;
	}
	.papertag.green:after{
		margin:0;
		padding:0;
		 background-color:#D0F5D4;
	}

	.papertag.red{
		margin:0;
		padding:10px;
		border-left:15px solid #F5D0E4;
	}
	.papertag.red:after{
		margin: 0;
		padding:0;
		 background-color:#F5D0E4;
	}

	.papertag.purple{
		margin:0;
		padding:10px;
		border-left:15px solid #D0D0F5;
	}
	.papertag.purple:after{
		margin: 0;
		padding:0;
		 background-color:#D0D0F5;
	}

	.papertag.blue{
		margin:0;
		padding:10px;
		border-left:15px solid #A0D0F5;
	}
	.papertag.blue:after{
		margin: 0;
		padding:0;
		 background-color:#A0D0F5;
	}

	#loadingwait_dialog {
		background-color: white;
		color:	black ;
		
		border: 10px solid #AAA ;
	}


@media screen and (max-device-width: 640px), screen and (max-width: 640px) {
	body {
		font-size:20px;
		font-family:Helvetica, HiraKakuProN-W3, sans-serif;
		line-height:1.4;
	}

	.contents_wrapper {
		width:640px;
		margin:0 auto;
	}
	.header_title {
		width:640px;
	}
	.header_pc {
		display: none;
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
}
	</style>
</head>
<body>
<div id="wearehere">
	<div class="header_pc"> 
		<h1><a href="/" ><img src="/jscss/images/fhc700.png" /></a></h1>
	</div>

	<div class="contents_wrapper">
		<div data-role="header" class="header_title">
			<fieldset class="navigationbar">
				<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_network.png" /></div>
				<div class="navigationbar_title"><span class="navigationbar_title_span">端末を探す</span></div>
			</fieldset>
		</div>
		<div class="contents_main">
			<dl>
			<dt class="sub_head">サーバーとの同期を待っています</dt>
			<dd>
				<div style="text-align:left; float:left;">
					<img src="/jscss/images/ajax-loader.gif" style="padding-right:4px;" />
				</div>
				<div style="padding-left: 70px;padding-top: 20px;">
					端末とサーバーとの同期を待っています。<br />
					1～2分ほどお待ちください。
				</div>
				<br class="clearfix">
			</dd>
			</dl>
		<br class="clearfix">
		<br />
	</div><!-- /contents_main -->
	</div><!-- /contents_wrapper -->

	<div style="display:none;" id="loadingwait_dialog_checkok">
		初期化手続きがすべて終わりました。<br />

		フューチャーホームコントローラーにログインできます。<br />

		<br />
		<br />
		<div>
			<a href="javascript:void(0)" onclick="on_client_login_button()"><span class="button">ログインする</span></a>
		</div>
		
	</div>

</div><!-- /page top -->
</body>

<script type="text/javascript" src="/jscss/jquery.js"></script>
<script type="text/javascript" src="/jscss/jquery.transit.js"></script>
<script>

$(function(){
	g_wearehereObject = $("#wearehere");

	var g_rebootcheckcount = 0;

	var rebootcheckWaitFunction = function(deadcount) {
		g_rebootcheckcount ++;
		if(g_rebootcheckcount >= 80)
		{//160秒以内にpooling が開始されなければエラー・・・にしたいけど成功でいいや
			showUpdatewaitDialog( {"htmlobject": g_wearehereObject.find("#loadingwait_dialog_checkok") } );
			return ;
		}

		//サーバーにpoolingが開始されるかを見る
		$.ajax({
			type: "post",
			url: "/networksetting/checkwait",
			timeout: 2000,
			data: { "_csrftoken": "%CSRFTOKEN%" },
			dataType: 'json',
			success: function(d){
				if (d["is_link"])
				{//コネクションが確立された!!
					//OKの画面を出す
					showUpdatewaitDialog( {"htmlobject": g_wearehereObject.find("#loadingwait_dialog_checkok") } );
					g_checkcount = 0;
					return ;
				}
				else
				{
					setTimeout( rebootcheckWaitFunction , 2000);
				}
				return ;
			},
			error: function(d){
				setTimeout( rebootcheckWaitFunction , 2000);
				return ;
			}
		});
	};
	rebootcheckWaitFunction();

});

function on_client_login_button()
{
	window.location.href = "/remocon/"
}


//アニメーションステート
var g_UpdatewaitDialogStatus = 0;	//0:何もない  1:アニメーションウエイト  2:アニメーション終了  3:アニメーション終了待ち

function showUpdatewaitDialog( arr )
{
	//前のアニメーションが残っている？
	if ( g_UpdatewaitDialogStatus != 0 )
	{
		$( "body #loadingwait" ).remove();
		$( "body #loadingwait_dialog" ).remove();
		g_UpdatewaitDialogStatus = 0;
	}

	if ( !arr )
	{
		arr = {};
	}
	if ( ! arr["htmlobject"] ) arr["htmlobject"] = $("#loadingwait_dialog_wait");
	if ( ! arr["transition"] ) arr["transition"] = Math.floor(Math.random() * 3 + 0.5);

	$( "body" ).append('<div id="loadingwait" style="position: fixed; left: 0px; top: 0px; width: 100%; height: 100%; opacity: 0.9; z-index: 98; background-color: #000000;"></div><div id="loadingwait_dialog" style="position:absolute; color: #000000; background-color: #ffffff; left: -9999px; top: 0px; opacity: 1; z-index: 99;border-radius: 15px;padding:10px;" ></div>');
	var loadingwait = $( "body #loadingwait" );
	var loadingwait_dialog = $( "body #loadingwait_dialog" ).html( arr["htmlobject"].html() );
	if (loadingwait_dialog.width() > document.documentElement.clientWidth)
	{
		loadingwait_dialog.width(document.documentElement.clientWidth);
		loadingwait_dialog.height( loadingwait_dialog.height() + 20 ); //縦に押し出されることがあるので、少し伸ばす
	}
	var goal_left	= (document.documentElement.clientWidth  / 2) - (loadingwait_dialog.width()  / 2);
	var goal_top	= (document.documentElement.clientHeight / 2) - (loadingwait_dialog.height() / 2);

	//1:アニメーションウエイト
	g_UpdatewaitDialogStatus = 1;

	if (arr["transition"] == "pop" || arr["transition"] == 0)
	{
		loadingwait_dialog.css({left: goal_left,top: 0});
		loadingwait_dialog.transition({
			 left: goal_left
			,top:  goal_top
		},'fast',UpdatewaitDialogAnimateEndCallback);
	}
	else if (arr["transition"] == "slidedown" || arr["transition"] == 1)
	{
		loadingwait_dialog.css({left: goal_left,top: 0});
		loadingwait_dialog.transition({
			 left: goal_left
			,top:  goal_top
		},'fast',UpdatewaitDialogAnimateEndCallback);
	}
	else if (arr["transition"] == "rotateup" || arr["transition"] == 2)
	{
		loadingwait_dialog.css({left: goal_left,top: goal_top,rotate: 0, scale: 0 });
		loadingwait_dialog.transition({
			 left: goal_left
			,top:  goal_top
			,rotate: 360
			,scale: 1
		},'fast',UpdatewaitDialogAnimateEndCallback);
	}
	else //if (arr["transition"] == "rotatedown" || arr["transition"] == 3)
	{
		loadingwait_dialog.css({left: goal_left,top: goal_top,rotate: 0, scale: 10 });
			loadingwait_dialog.transition({
			 left: goal_left
			,top:  goal_top
			,rotate: 360
			,scale: 1
		},'fast',UpdatewaitDialogAnimateEndCallback);
	}
}
function hideUpdatewaitDialog( )
{
	if (g_UpdatewaitDialogStatus == 1)
	{
		g_UpdatewaitDialogStatus = 3;
	}
	else
	{
		$( "body #loadingwait" ).remove();
		$( "body #loadingwait_dialog" ).remove();
		g_UpdatewaitDialogStatus = 0;
	}
}

function UpdatewaitDialogAnimateEndCallback()
{
	if (g_UpdatewaitDialogStatus == 3)
	{
		$( "body #loadingwait" ).remove();
		$( "body #loadingwait_dialog" ).remove();
		g_UpdatewaitDialogStatus = 0;
	}
	else
	{
		//chromeのバグなのか、cssアニメーションが完了しない時があるので、念のため喝入れしとく.
		$( "body #loadingwait_dialog" ).css({rotate: 0, scale: 1 });

		g_UpdatewaitDialogStatus = 2;
	}
}

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