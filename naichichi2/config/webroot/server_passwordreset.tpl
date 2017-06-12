<!DOCTYPE html>
<html lang="ja"><head>
	<meta name="format-detection" content="telephone=no" />
	<meta name="viewport" content="width=640,user-scalable=yes,maximum-scale=0.67" />
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=99" />
	<link rel="apple-touch-icon" href="/jscss/images/iphone_icon.png" />
	<link rel="shortcut icon" href="/jscss/images/favicon.ico" />
	<title>パスワードリセット</title>
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
	
	#passwordreset {
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

	.idpass_name {
		width: 200px;
		display:inline-table;
		vertical-align:middle;
		font-size: 1.2em;
	}
	.idpass_name .cell {
		display:table-cell;
		vertical-align:middle;
	}
	
	.idpass_input {
		width: 650px;
		display:inline-table;
		vertical-align:middle;
	}
	.idpass_input .cell {
		display:table-cell;
		vertical-align:middle;
	}

	#loadingwait_dialog {
		background-color: white;
		color:	black ;
		
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
}
	</style>
</head>
<body>
<div id="passwordreset" class="page">
	<div class="header_pc"> 
		<h1><a href="/" ><img src="/jscss/images/fhc700.png" /></a></h1>
	</div>

	<div class="contents_wrapper">
		<div data-role="header" class="header_title">
			<fieldset class="navigationbar">
				<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_passwordreset.png" /></div>
				<div class="navigationbar_title"><span class="navigationbar_title_span">パスワードリセット</span></div>
			</fieldset>
		</div>
		<div class="contents_main">
			<p>
				パスワードを再設定します。<br />
				<br />
				本人確認の為、メールアドレスを入れてください。<br />
				メールの確認後、パスワードリセットが行えます。<br />
			</p>
				<div class="error">エラーが発生しました!!</div>

				<div>
					<div class="idpass_name" ><span class="cell">mail-address</span></div>
					<div class="idpass_input"><span class="cell"><input type="email" class="minput_tag" id="passwordreset_id" name="passwordreset_id" value="" placeholder="mailaddressを入れてください 例:test@eaxmple.com"></span></div>
				</div>
				<div id="passwordreset_80001_error" class="error">
					メールアドレスが登録されていないか正しくありません<br />
				</div>

				<br />
				<div style="width:500px;margin:0 auto;">
					<input type="button" class="button" id="passwordreset_button" value="確認のメールを送信する" />
				</div>
				<div class="error">エラーが発生しました!!</div>
		</div><!-- /contents_main -->
	</div><!-- /contents_wrapper -->
	<div style="display:none;" id="loadingwait_dialog_wait">
			<div style="text-align:center;"><img src="/jscss/images/ajax-loader.gif" style="padding-right:4px;" /></div>
		処理中です・・・
	</div>
	<div style="display:none;" id="loadingwait_dialog_wait_passwordreset_sendmail">
		メールをお送りいたしました。<br />
		メール内に掲載されているURLよりパスワードがリセットできます。<br />
		<br />
		メールが届かないなどありましたら、<br />
		再度申請を行われるか、<br />
		<a href="https://rti-giken.jp/?page=contact">弊社サポートまで</a>ご連絡ください。<br />
		<br />
		<a href="/">トップページに戻る</a><br />
	</div>
</div><!-- /page top -->
</body>

<script type="text/javascript" src="/jscss/jquery.js"></script>
<script type="text/javascript" src="/jscss/jquery.transit.js"></script>
<script>
g_passwordresetObject = null;

$(function(){
	g_passwordresetObject = $("#passwordreset");
	g_passwordresetObject.find(".error").hide();

	g_passwordresetObject.find("#passwordreset_button").click(function(){
		g_passwordresetObject.find(".error").hide();

		if (g_passwordresetObject.find("#passwordreset_id").val() == "" )
		{
			return ;
		}

		var arr = {};
		arr["id"] = g_passwordresetObject.find("#passwordreset_id").val();
		//サーバーにpoolingが開始されるかを見る
		$.ajax({
			type: "post",
			url: "/passwordreset/sendmail_json",
			timeout: 5000,
			data: arr,
			dataType: 'json',
			success: function(d){
				if ( d['result'] != "ok" )
				{
					if ( d['code'] == "80001" )
					{
						g_passwordresetObject.find("#passwordreset_80001_error").show();
					}
					else
					{
						alert("passwordreset_start_json関数で未知のエラーにより失敗しました");
					}
					return false;
				}

				var arr = {};
				arr["htmlobject"] = $("#loadingwait_dialog_wait_passwordreset_sendmail");
				showUpdatewaitDialog(arr);
				
				return false;
			},
			error: function(d){
				alert("サーバーがエラーを返しました");
				return ;
			}
		});
	});

});

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

	$( "body" ).append('<div id="loadingwait" onclick="hideUpdatewaitDialog();" style="position: fixed; left: 0px; top: 0px; width: 100%; height: 100%; opacity: 0.9; z-index: 98; background-color: #000000;"></div><div id="loadingwait_dialog" style="position:absolute; color: #000000; background-color: #ffffff; left: -9999px; top: 0px; opacity: 1; z-index: 99;border-radius: 15px;padding:10px;" ></div>');
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


</script>
</html>