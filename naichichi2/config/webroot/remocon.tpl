<!DOCTYPE html>
<html lang="ja"><head>
	<meta name="format-detection" content="telephone=no" />
	<meta name="viewport" content="width=640,user-scalable=no,target-densitydpi=device-dpi" />
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=99" />
	<link rel="apple-touch-icon" href="/jscss/images/iphone_icon.png" />
	<link rel="shortcut icon" href="/jscss/images/favicon.ico" />
	<title>%ROOM_STRING%</title>
	<!-- このコードは GPL でライセンスされてます。 -->
	<base href="/"></base>
	<style>
	h1,h2,h3,h4,h5,h6{
		font-size: 100%;
		font-weight: normal;
		padding:10px;
		margin:0;
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
		color: %DEFAULT_FONT_COLOR%;
		background: %DEFAULT_BACKGROUND_COLOR%;
	}
	
	body > .page {
		height: auto;
	}

	.header_pc{
		margin:0 auto;
		border: 1px solid #AAA ;
		background: %DEFAULT_BACKGROUND_COLOR% ;
		color: %DEFAULT_FONT_COLOR% ;
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
	}

	.buttoncolor {
		cursor: pointer;
		background-color: %BUTTON_BACKGROUND_COLOR2%;
		color:            %BUTTON_FONT_COLOR%;

		border:1px solid %BUTTON_BORDER_COLOR%;

		text-shadow: 0px 1px 1px #fff;
		background-image: -webkit-gradient(linear, left top, left bottom, color-stop(0%, %BUTTON_BACKGROUND_COLOR1%), color-stop(100%, %BUTTON_BACKGROUND_COLOR2%));
		background-image: %BCSS%linear-gradient(top, %BUTTON_BACKGROUND_COLOR1%, %BUTTON_BACKGROUND_COLOR2%);
		background-image: linear-gradient(top, %BUTTON_BACKGROUND_COLOR1%, %BUTTON_BACKGROUND_COLOR2%);filter:progid:DXImageTransform.Microsoft.gradient(GradientType=0,startColorstr=%BUTTON_BACKGROUND_COLOR1%, endColorstr=%BUTTON_BACKGROUND_COLOR2%);
   	}
   	.buttoncolor:hover {
		background-color: %BUTTON_HOVER_COLOR2%;
		border:1px solid %BUTTON_HOVER_BORDER_COLOR%; 
		background-image: -webkit-gradient(linear, left top, left bottom, color-stop(0%,%BUTTON_HOVER_COLOR1%), color-stop(100%, %BUTTON_HOVER_COLOR2%));
		background-image: %BCSS%linear-gradient(top, %BUTTON_HOVER_COLOR1%, %BUTTON_HOVER_COLOR2%);
		background-image: linear-gradient(top, %BUTTON_HOVER_COLOR1%, %BUTTON_HOVER_COLOR2%);filter:progid:DXImageTransform.Microsoft.gradient(GradientType=0,startColorstr=%BUTTON_HOVER_COLOR1%, endColorstr=%BUTTON_HOVER_COLOR2%);
   	}
   	

	.clearfix
	{
		clear:both;
		float:none;
	}
	
	#remoconstatic {
		min-height: 100%;
		height: 100%;
	}

	.contents_wrapper {
		width:1000px;
		margin:0 auto;
		min-height: 100%;
		background: url("%BACKGROUND_IMAGE%") repeat-y scroll center 0 !important;
		background-color: %DEFAULT_BACKGROUND_COLOR%;
	}
	.contents_main {
		margin:0px 70px;
	}

	.header_title { /* ヘッダー 要素を全力で消す。 ただし下線 border-buttom だけ残します */
		width: 950px;
		margin: 0 auto;
		position: relative;
		border-bottom: 1px solid #AAA;
		border-top: none;
		border-left: none;
		border-right: none;
		color: black; ;
		font-weight: bold;
	}
	.navigationbar {
		height: 64px;
		border: none;
		border-width: 0;
		padding: 0;
		margin: 0;
	}

	.navigationbar .navigationbar_icon {
		display:inline-table;
		vertical-align:middle;
		width: 64px;
	}
	.navigationbar .navigationbar_title {
		display:inline-table;
		vertical-align:middle;
		width: 650px;
	}
	.navigationbar .navigationbar_title .navigationbar_title_span {
		display:inline-table;
		vertical-align:middle;
		font-size:1.8em;
		color: %ROOM_COLOR%;
	}
	.navigationbar .navigationbar_sensor {
		display:inline-table;
		vertical-align:middle;
		margin-top: 10px;
		width: 180px;
		font-size:16px;
	}
	.navigationbar .navigationbar_sensor .sensor_name {
	}
	.navigationbar .navigationbar_sensor .sensor_value {
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
		color: %FOOTERMENU_FONT_COLOR%;
		border: 1px solid %FOOTERMENU_BORDER_COLOR%;
		background: %FOOTERMENU_BACKGROUND_COLOR%;
		width: 400px;
		font-size: 1.6em;
		float: left;
		position: relative;
	}

	.footermenu_buton:hover {
		cursor: pointer;
		border: 1px solid rgba(0,0,0,0.2);
		background: %FOOTERMENU_HOVER_COLOR%;
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
	
	#remocon_remoconmenu_ul {
		list-style-type: none;
		margin: 0px;
		padding: 0px;
		border: none;
		width: 950px;
		margin:0 auto;
	}
	#remocon_remoconmenu_ul li.item {
		z-index:	1;
		margin: 12px;
		padding: 5px;
		float: left;
		width: 200px;
		height: 220px;
		color: #1C94C4;
		position:	relative;
	}
	#remocon_remoconmenu_ul .clickable {	
		z-index:		20;
		position:		absolute;
		top:			30px;
		left:			30px;
		width:			170px;
		height:			170px;
		background-color: transparent;
		cursor: pointer;
	}

	#remocon_remoconmenu_ul .mainicon {
		z-index:	2;
		position:	absolute;
		top:		38px;
		left:		38px;
		width:		128px;
		height:		128px;
	}

	#remocon_remoconmenu_ul .actionmenuicon {
		z-index:		10;
		font-weight:	normal;
		position:		absolute;
		top:			0px;
		left:			166px;
		width:			32px;
		height:			32px;
		background: url("%MENU_ICON%") no-repeat scroll 0 !important;
	}
	#remocon_remoconmenu_ul .actionmenuicon:hover {
		cursor: pointer;
	}
	#remocon_remoconmenu_ul .actionmenuicon_clickable { 
		cursor: pointer;
		z-index:		30;
		position:		absolute;
		top:			0px;
		left:			145px;
		width:			60px;
		height:			60px;
		background-color: transparent;
	}

	#remocon_remoconmenu_ul .timericon {
		z-index:		40;
		font-weight:	normal;
		position:		absolute;
		top:			100px;
		left:			136px;
		width:			64px;
		height:			64px;

		background: url("/user/remocon/timer.png") no-repeat scroll 0 !important;
	}

	#remocon_remoconmenu_ul .type {
		z-index:		10;
		font-weight:	normal;
		position:		absolute;
		top:			170px;
		left:			5px;
		width:			195px;
		color:			%DEFAULT_FONT_COLOR%;
		text-shadow: 0px 1px 1px #FFFFFF;
		text-align: center;
		font-size:	14px;
	}

	#remocon_remoconmenu_ul .status_label {
		z-index:	10;
		position:	absolute;
		top:		190px;
		left:		5px;
		width:		195px;
		text-align: center;
		font-size:	12px;
	}
	#remocon_remoconmenu_ul .status_jyotai {
		font-size:	12px;
	}
	#remocon_remoconmenu_ul .status {
		font-size:		18px;
		font-weight:	bolder;
	}
	
	#remocon_remoconmenu_ul .click_to_menu {
		display:		none;
	}

	.color_buttun {
		box-shadow: 0 2px 6px rgba(0,0,0,0.5), inset 0 1px rgba(255,255,255,0.3), inset 0 10px rgba(255,255,255,0.2), inset 0 10px 20px rgba(255,255,255,0.25), inset 0 -15px 30px rgba(0,0,0,0.1);
		%BCSS%box-shadow: 0 2px 6px rgba(0,0,0,0.5), inset 0 1px rgba(255,255,255,0.3), inset 0 10px rgba(255,255,255,0.2), inset 0 10px 20px rgba(255,255,255,0.25), inset 0 -15px 30px rgba(0,0,0,0.1);

		border-radius: 10px;
		%BCSS%border-radius: 10px;
		margin: 10px;
		padding: 10px;
		color: %FOOTERMENU_FONT_COLOR%;
		border: 1px solid %FOOTERMENU_BORDER_COLOR%;
		background: %FOOTERMENU_BACKGROUND_COLOR%;
		width: 150px;
		text-align: center;
	}
	.color_buttun:hover {
		cursor: pointer;
		border: 1px solid rgba(0,0,0,0.2);
		background: %FOOTERMENU_HOVER_COLOR%;
	}


   	.popupselect {
		width: %POPUPMENU_REMOCONEDIT_WIDTH_FORPC%;
		margin:0 auto;
		background-color: %DIALOG_BACKGROUND_COLOR%;
	}

	.popupselect .pitembox {
		position:	relative;
		width: 150px;
		height: 128px;
	}
  	.popupselect .pitembox .pmainicon {
		position:	absolute;
		top:		0px;
		left:		0px;
		width:		128px;
		height:		128px;
	}

	.popupselect .pitembox .ptype {
		font-weight:	normal;
		position:		absolute;
		top:			20px;
		left:			140px;
		width:			250px;
		font-size:	14px;
	}

	.popupselect .pitembox .pstatus {
		font-weight:	normal;
		position:		absolute;
		top:			35px;
		left:			140px;
		width:			190px;
		font-size:	14px;
	}
	
	.popupselect .pitembox .pclose {
		cursor: pointer;
		font-weight:	normal;
		position:		absolute;
		top:			5px;
		left:			%POPUPMENU_REMOCONEDIT_CLOSEBUTTON_FORPC%;
		width:			50px;
		height:			20px;
		font-size:		14px;
	}

   	.dialog_sipevent {
		width: 300px;
		margin:0 auto;
		background-color: %DIALOG_BACKGROUND_COLOR%;
	}
	
	.dialog_sipevent .button {
		width: 200px;
	}

	.dialog_sipevent .pclose {
		cursor: pointer;
		font-weight:	normal;
		position:		absolute;
		top:			5px;
		left:			260px;
		width:			50px;
		height:			20px;
		font-size:		14px;
	}
	.dialog_sipevent .numpad a span{
		float: left;
		font-weight:      bolder;
		text-align:       center;
		font-size: 25px;

		%BCSS%border-radius: 10px;
		border-radius: 10px;

		width:  35px;
		height: 35px;
		min-height: 35px;
		max-height: 35px;
		padding: 15px;
		margin:5px;

		position:	relative;
	}
	.dialog_sipevent .numpad {
		padding-left: 30px;
		clear:both;
		float:none;
	}

	
	.popupselect .popupselect_ul {
		list-style-type: none;
		border: none;
		padding: 0px;
		width: %POPUPMENU_REMOCONEDIT_WIDTH_FORPC%;
		height:  100%;
	}
	.popupselect .popupselect_li {
		float: left;
		font-weight:      bolder;
		text-align:       center;
		font-size: 20px;

		%BCSS%border-radius: 10px;
		border-radius: 10px;

		width:  90px;
		height: 35px;
		min-height: 35px;
		max-height: 35px;
		padding:35px 5px;
		margin:5px;

		position:	relative;
   	}
	.popupselect .noborder {
		border:1px solid transparent;
   	}
	.popupselect .popupselect_li .actionimage {
		position:		absolute;
		top:			-1px;
		left:			-1px;
		width:			102px;
		height:			107px;

		cursor: pointer;
	}

	.popupselect .psettimer {
		font-weight:	normal;
		position:		absolute;
		top:			96px;
		left:			136px;
		width:			64px;
		height:			64px;
		background: url("%BADGES_TIMER_ICON%") no-repeat scroll 0 !important;
	}
	.popupselect .psettimer_detail {
		font-weight:	normal;
		position:		absolute;
		top:			65px;
		left:			125px;
		width:			320px;
		height:			55px;
		font-size:		12px;
		border: 1px solid %DIALOG_BORDER_COLOR% ;
		display:		none;
	}

	#loadingwait_dialog {
		background-color: %DIALOG_BACKGROUND_COLOR%;
		color:	%DIALOG_FONT_COLOR% ;
		
		border: 10px solid %DIALOG_BORDER_COLOR% ;
	}

	.is_mobile_display_none {
		display: block;
	}
	
	.switch_room {
		display: none;
	}

/* for mobile */
@media screen and (max-device-width: 640px), screen and (max-width: 640px) {
	body {
		font-size:22px;
		font-family: Helvetica, Arial, 'Hiragino Kaku Gothic ProN', 'ヒラギノ角ゴ ProN W3', 'モリサワ 新ゴ R', 'Droid Sans', メイリオ, 'sans-serif';
		line-height:1.4;
		-webkit-text-size-adjust: 100%;
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
		width: 300px;
	}

	.navigationbar {
		height: 64px;
		border: none;
		border-width: 0;
		padding: 0;
		margin: 0;
	}
	.navigationbar .navigationbar_icon {
		display:inline-table;
		vertical-align:middle;
		width: 64px;
	}
	.navigationbar .navigationbar_title {
		display:inline-table;
		vertical-align:middle;
		width: 390px;
	}
	.navigationbar .navigationbar_title .navigationbar_title_span {
		display:inline-table;
		vertical-align:middle;
		font-size:1.8em;
	}
	
	.navigationbar .navigationbar_sensor {
		width: 140px;
		margin-top: 5px;
		font-style: normal;
		font-size: 18px;
		white-space: nowrap;
		color: %SENSOR_FONT_COLOR%;
	}
	.navigationbar .navigationbar_sensor .sensor_name {
	}
	.navigationbar .navigationbar_sensor .sensor_value {
	}

	
	.footermenu{
		margin-left: 0px;
	}
	.footermenu_buton {
		width: 270px;
		font-size: 22px;
		position: relative;
		white-space: nowrap;
	}

	.footermenu_buton:hover {
		cursor: pointer;
		border: 1px solid rgba(0,0,0,0.2);
		background: rgba(255,240,112,1);
	}

	.footermenu_buton .footermenu_buton_icon {
		position:		absolute;
		width: 64px;
		height: 64px;
	}
	.footermenu_buton .footermenu_buton_name {
		position:		absolute;
		top:  6px
		left: 200px;
	}

	#remocon_remoconmenu_ul {
		list-style-type: none;
		margin: 0px;
		padding: 0px;
		width: 664px;
		border: none;
		margin-left:-24px;
	}

	#remocon_remoconmenu_ul li.item {
		margin: 11px 22px 12px 0px;
		padding: 2px;
		height: 190px;
		width: 140px;
	}

	#remocon_remoconmenu_ul .clickable {	
		z-index:		20;
		position:		absolute;
		top:			10px;
		left:			30px;
		width:			100px;
		height:			180px;
		background-color: transparent;
	}


	#remocon_remoconmenu_ul .type {
		z-index:		10;
		font-weight:	normal;
		position:		absolute;
		top:			170px;
		left:			0px;
		width:			130px;
		text-align: center;
	}

	#remocon_remoconmenu_ul .status_label {
		z-index:	10;
		position:	absolute;
		top:		190px;
		left:		35px;
		width:		130px;
		text-align: center;
	}
	#remocon_remoconmenu_ul .status_jyotai {
		display: none;
	}
	#remocon_remoconmenu_ul .status {
		font-size:		22px;
		font-weight:	bolder;
		white-space:	nowrap;
	}

	#remocon_remoconmenu_ul .mainicon {
		z-index:	1;
		position:	absolute;
		top:		38px;
		left:		30px;
		width:		120px;
		height:		120px;
	}

	#remocon_remoconmenu_ul .type {
		left:			35px;
		font-size:		20px;
		line-height:	1.4;
		font-weight:	bolder;
		white-space:	nowrap;
	}

	#remocon_remoconmenu_ul .actionmenuicon {
		left:130px;
		top:15px;
	}
	
	#remocon_remoconmenu_ul .actionmenuicon_clickable { 
		z-index:		30;
		position:		absolute;
		top:			0px;
		left:			110px;
		width:			60px;
		height:			160px;
		background-color: transparent;
	}

	#remocon_remoconmenu_ul .timericon {
		z-index:		40;
		font-weight:	normal;
		position:		absolute;
		top:			100px;
		left:			100px;
		width:			64px;
		height:			64px;
	}

   	.popupselect {
		width: 450px;
		margin:0 auto;
		background-color: %DIALOG_BACKGROUND_COLOR%;
	}

	.popupselect .pitembox .pclose {
		cursor: pointer;
		font-weight:	normal;
		position:		absolute;
		top:			5px;
		left:			400px;
		width:			50px;
		height:			20px;
		font-size:		14px;
	}
	.popupselect .popupselect_ul {
		list-style-type: none;
		border: none;
		padding: 0px;
		width:   450px;
		height:  100%;
	}

	.is_mobile_display_none {
		display: none;
	}
	
}
	</style>
</head>
<body>
<div id="remoconstatic" class="page">

	<div class="header_pc"> 
		<h1><a href="/" ><img src="/jscss/images/fhc700.png" /></a></h1>
	</div>

	<div class="contents_wrapper">
		<div data-role="header" class="header_title">
			<fieldset class="navigationbar">
				<div class="navigationbar_icon"><img src="%ROOM_ICON%" /></div>
				<div class="navigationbar_title"><span class="navigationbar_title_span">%ROOM_STRING%</span></div>
				<div class="navigationbar_sensor"><span class="sensor_name">%SENSOR_TEMP_STRING%　:</span><span class="sensor_value">%SENSOR_TEMP%</span><br /><span class="sensor_name">%SENSOR_LUMI_STRING%:</span><span class="sensor_value">%SENSOR_LUMI%</span></div>
			</fieldset>
		</div>
	<ul id="remocon_remoconmenu_ul">
%REMOCON_LOOP_START%
	%REMOCON_CONTENTSWITCH_SPACE_START%
			<li class="item itemshow">
			</li>
	%REMOCON_CONTENTSWITCH_SPACE_END%
	%REMOCON_CONTENTSWITCH_ELEC_START%
			<li class="item itemshow">
					<div class="mainicon"><img src="%ICON%" width="120" height="120" /></div>
					<div class="type">%TYPE%</div>
					<div class="status_label" style="color:%STATUSCOLOR%;"><span class="status_jyotai">状態:</span><span class="status elec_%ELECID%_status">%STATUS%</span></div>
					<div class="click_to_menu">%CLICK_TO_MENU%</div>

					<div class="clickable"></div>
					<div class="actionmenuicon_clickable"></div>
					<div class="actionmenuicon"></div>

					<div class="timericon elec_%ELECID%_timericon" style="display: %REMOCON_USE_SETTIMER%;"></div>
					<div style="display: none;">
						<div class="popupselect">
							<div class="pitembox">
								<div class="pmainicon"><img src="%ICON%" width="128" height="128" /></div>
								<div class="ptype">%TYPE%</div>
								<div class="pstatus" style="color: %STATUSCOLOR%;">%STATUS%</div>
								<div class="pclose buttoncolor" onclick="hideUpdatewaitDialog();">%DIALOG_CLOSE_STRING%</div>
								<div class="psettimer_detail elec_%ELECID%_psettimer_detail"></div>
							</div>
							<ul class="popupselect_ul">
%REMOCONACTION_LOOP_START%
								%REMOCONACTION_CONTENTSWITCH_SPACE_START%
									<li class="popupselect_li noborder itemshow">
									</li>
								%REMOCONACTION_CONTENTSWITCH_SPACE_END%
								%REMOCONACTION_CONTENTSWITCH_ACTION_START%
									<li class="popupselect_li buttoncolor itemshow %ACTIONOPTINALCLASS%" onclick="remocon_fire_bytype_ad($(this)); return false;" style="color: %ACTIONCOLOR%;" actiontype="%ACTIONTYPE%">
									%STATUS_CONTENTSWITCH_STRING_START%
										<span class="actiontype" >%ACTIONTYPE%</span>
									%STATUS_CONTENTSWITCH_STRING_END%
									%STATUS_CONTENTSWITCH_ICON_START%
											<img class="actionimage" src="%ACTIONICON%" width="102" height="107" />
									%STATUS_CONTENTSWITCH_ICON_END%
									</li>
								%REMOCONACTION_CONTENTSWITCH_ACTION_END%
%REMOCONACTION_LOOP_END%
							</ul>
						</div>
					</div>
			</li>
	%REMOCON_CONTENTSWITCH_ELEC_END%

%REMOCON_LOOP_END%
	</ul>
	<br class="clearfix" />
	<br />

	<div class="footermenu">
		<div class="footermenu_buton" id="footermenu_buton_edit">
			<div class="footermenu_buton_icon"><img src="%FOOTERMENU_REMOCONEDIT_ICON%" /></div>
			<div class="footermenu_buton_name">%FOOTERMENU_REMOCONEDIT_STRING%</div>
		</div>
		<div class="footermenu_buton" id="footermenu_buton_setting">
			<div class="footermenu_buton_icon"><img src="%FOOTERMENU_SETTING_ICON%" /></div>
			<div class="footermenu_buton_name">%FOOTERMENU_SETTING_STRING%</div>
		</div>
		<br class="clearfix">
	</div>
	<br class="clearfix">

	<!-- 
		%SHOW_AD_BUNNER%
	-->

	</div><!-- /contents_wrapper -->


	<div style="display:none;" id="loadingwait_dialog_wait">
		<div style="text-align:center;"><img src="/jscss/images/ajax-loader.gif" style="padding-right:4px;" /></div>
	</div>
	<div style="display:none;" id="loadingwait_dialog_bad_connect">
		Future Home Controller との接続が取れていません。<br />
		本体の電源が入っているか、ネットワークの設定が正しいか見なおしてください。<br />
		<br />
		<br />
		<div>
			<a href="/remocon/"><span class="button buttoncolor">リロード</span></a><br />
			<a href="/auth/"><span class="button buttoncolor">再ログイン</span></a><br />
		</div>
	</div>
	<div style="display:none;" id="loadingwait_dialog_sipevent_incoming">
		<div class="dialog_sipevent dialog_sipevent_incoming">
			<div class="pclose buttoncolor" onclick="hideUpdatewaitDialog();">%DIALOG_CLOSE_STRING%</div>
			<div>
			電話がかかってきています。<br />
			相手:<span id="sip_incoming_number"></span><br/>
			</div>
			<br />
			<br />
			<div>
				<a onclick="sip_answer();"><span class="button buttoncolor">電話に出る</span></a><br />
				<a onclick="sip_hangup();"><span class="button buttoncolor">出ないで切る</span></a><br />
			</div>
		</div>
	</div>
	<div style="display:none;" id="loadingwait_dialog_sipevent_call">
		<div class="dialog_sipevent dialog_sipevent_call">
			<div class="pclose buttoncolor" onclick="hideUpdatewaitDialog();">%DIALOG_CLOSE_STRING%</div>
			<div>
			通話中です。<br />
			相手:<span id="sip_call_number"></span><br/>
			</div>
			<br />
			<br />
			<div>
				<a onclick="sip_hangup();"><span class="button buttoncolor">電話をきる</span></a><br />
			</div>
		</div>
	</div>
	<div style="display:none;" id="loadingwait_dialog_sipevent_numpad">
		<div class="dialog_sipevent dialog_sipevent_numpad">
			<div class="pclose buttoncolor" onclick="sip_numpad_close();">%DIALOG_CLOSE_STRING%</div>
			<div>
				TEL:<input type="tel" class="sip_numpad_edit" value="" />
			</div>
			<div class="numpad">
				<a onclick="sip_pushnumber(this,'1',697,1209);"><span class="button buttoncolor">1</span></a>
				<a onclick="sip_pushnumber(this,'2',697,1336);"><span class="button buttoncolor">2</span></a>
				<a onclick="sip_pushnumber(this,'3',697,1477);"><span class="button buttoncolor">3</span></a>
			</div>
			<div class="numpad">
				<a onclick="sip_pushnumber(this,'4',770,1209);"><span class="button buttoncolor">4</span></a>
				<a onclick="sip_pushnumber(this,'5',770,1336);"><span class="button buttoncolor">5</span></a>
				<a onclick="sip_pushnumber(this,'6',770,1477);"><span class="button buttoncolor">6</span></a>
			</div>
			<div class="numpad">
				<a onclick="sip_pushnumber(this,'7',852,1209);"><span class="button buttoncolor">7</span></a>
				<a onclick="sip_pushnumber(this,'8',852,1336);"><span class="button buttoncolor">8</span></a>
				<a onclick="sip_pushnumber(this,'9',852,1477);"><span class="button buttoncolor">9</span></a>
			</div>
			<div class="numpad">
				<a onclick="sip_pushnumber(this,'*',941,1209);"><span class="button buttoncolor">*</span></a>
				<a onclick="sip_pushnumber(this,'0',941,1336);"><span class="button buttoncolor">0</span></a>
				<a onclick="sip_pushnumber(this,'#',941,1477);"><span class="button buttoncolor">#</span></a>
			</div>
			<br />
			<br />
			<div>
				<a onclick="sip_call(this);"><span class="button buttoncolor">電話をかける</span></a><br />
			</div>
		</div>
	</div>


</div><!-- /page top -->
</body>

<script type="text/javascript" src="/jscss/jquery.js"></script>
<script type="text/javascript" src="/jscss/jquery.transit.js"></script>
<script>
var g_remoconValue = %REMOCON_VALUE%;

function updateS(url,arr,success_callback,options)
{
	g_remoconValue['update_wait'] = 1;
	if (!options || options.load != 'no')
	{
		showUpdatewaitDialog();
	}

	arr['_csrftoken'] = "%CSRFTOKEN%";

	$.ajax({
		type: "post",
		url: url,
		timeout: 50000,
		data: arr,
		dataType: 'json',
		success: function(d){
			g_remoconValue['update_wait'] = 0;
			if (!options || options.load != 'no')
			{
				hideUpdatewaitDialog();
			}

			if (d['result'] != "ok")
			{//エラーになりました
				if (options && options.error)
				{
					options.error(d);
				}
				else
				{
					alert("エラー: " + d['message'] + "\r\nエラーコード:"+d['code']);
				}
				return ;
			}
			success_callback && success_callback(d);
		},
		error: function(d){
			g_remoconValue['update_wait'] = 0;
			if (!options || options.load != 'no')
			{
				hideUpdatewaitDialog();
			}

			if (options && options.error)
			{
				options.error({result: 'error'});
			}
			else
			{
				alert("エラー:応答がありません");
			}
		}
	});
}

//状態の更新をする
function remocon_get_append_info()
{
	updateS(
		 "/remocon/get/append/info"
		,{}
		,function(d){
			for ( var i in d['elec'] )
			{
				var key = d['elec'][i]['key'];
				$('.elec_' + key + '_status').text(d['elec'][i]['status']);
				if (d['elec'][i]['timer_elec'])
				{
					var msg = "あと" + d['elec'][i]["timer_minute"] + "分後に「" + d['elec'][i]["timer_elec"] + "」を「" + d['elec'][i]["timer_action"] + "」にするタイマーが動作します。";
					$('.elec_' + key + '_timericon').show();
					$('.elec_' + key + '_psettimer_detail').text(msg);
					$('.elec_' + key + '_psettimer_detail').show();
				}
				else
				{
					$('.elec_' + key + '_timericon').hide();
					$('.elec_' + key + '_psettimer_detail').hide();
				}
			}
			$('.sensor_value[1]').text(d['sensor_temp']);
			$('.sensor_value[2]').text(d['sensor_lumi']);

			//電話がかかってきているなら、相手を表示する.
			show_dialog_sipevent_incoming(d["sipevent_incoming"]);
			//通話中なら相手を表示
			show_dialog_sipevent_call(d["sipevent_call"]);
			//電話のキーパッドを表示
			show_dialog_sipevent_numpad(d["sipevent_numpad"]);
		}
		,{'load': 'no'
		   ,'error': function(){
		   }
		}
	);
}
//タイマーで定期監視
function on_timer_remocon_get_append_info()
{
console.log("on_timer_remocon_get_append_info");
	if (g_remoconValue['update_wait'])
	{//アップデート作業中なので何もしない.
		return ;
	}
	remocon_get_append_info();
}



function on_elec_icon_longclick(itemObject)
{
	itemObject.find(".actionmenuicon_clickable,.timericon").mousedown();
}

function on_elec_icon_click(itemObject,menutype)
{
	if ( itemObject.find(".click_to_menu").text() == "1" )
	{//メニュークリックしたら、動作ダイアログを出す
		itemObject.find(".actionmenuicon_clickable,.timericon").mousedown();

		return ;
	}

	//現在のステータス
	var status = itemObject.find(".status").text();

	//メニュークリックしたら、次の動作をする
	//次のステータスを探す
	var newstatus = "";
	var hit = false;
	itemObject.find("[actiontype]").each(function(){
		var obj = $(this);
		var actiontype = obj.attr("actiontype");
		if ( obj.hasClass("nostateremocon") )
		{//遷移禁止.
			return true;
		}
		if (newstatus == "") {
			newstatus = actiontype;
		}
		if (hit) {
			newstatus = actiontype;
			hit = false;
			return ;
		}
		if (actiontype == status ) {
			hit = true;
		}
	});
	remocon_fire_bytype(menutype,newstatus);
}

//電話がかかってきているなら、相手を表示する.
function show_dialog_sipevent_incoming(v)
{
	var show = isShowDialog('.dialog_sipevent_incoming');
console.log("show_dialog_sipevent_incoming",v,show);

	if (!v)
	{//相手が電話をきるかもしれない
		if ( show )
		{
			console.log("show_dialog_sipevent_incoming -> close");
			hideUpdatewaitDialog();
		}
		return ;
	}

	if (!show)
	{
		console.log("show_dialog_sipevent_incoming -> show");

		var dialog_sipevent_incoming = $("#loadingwait_dialog_sipevent_incoming");
		dialog_sipevent_incoming.find("#sip_incoming_number").text(v);
		showUpdatewaitDialog( {"htmlobject": dialog_sipevent_incoming , "cancel": hideUpdatewaitDialog } );
	}
}

//通話中なら相手を表示
function show_dialog_sipevent_call(v)
{
	var show = isShowDialog('.dialog_sipevent_call');
console.log("show_dialog_sipevent_call",v,show);

	if (!v)
	{
		if ( show )
		{
			console.log("show_dialog_sipevent_call -> close");
			hideUpdatewaitDialog();
		}
		return ;
	}

	if (!show)
	{
		console.log("show_dialog_sipevent_call -> show");

		var dialog_sipevent_call = $("#loadingwait_dialog_sipevent_call");
		dialog_sipevent_call.find("#sip_call_number").text(v);
		showUpdatewaitDialog( {"htmlobject": dialog_sipevent_call , "cancel": hideUpdatewaitDialog } );
	}
}

//電話のキーパッドを表示
function show_dialog_sipevent_numpad(v)
{
	var show = isShowDialog('.dialog_sipevent_numpad');
console.log("show_dialog_sipevent_numpad",v,show);

	if (!v)
	{
		if ( show )
		{
			console.log("show_dialog_sipevent_numpad -> close");
			hideUpdatewaitDialog();
		}
		return ;
	}

	if (!show)
	{
		console.log("show_dialog_sipevent_numpad -> show");

		var dialog_sipevent_numpad = $("#loadingwait_dialog_sipevent_numpad");
		showUpdatewaitDialog( {"htmlobject": dialog_sipevent_numpad , "cancel": sip_numpad_close } );
	}
}

//長押しチェックタイマー
var g_longtouchtimer = null;
//スワイプ判定X座標
var g_swipePos = {x: 0,y: 0};

$(function(){
	//家電アイコンに動作を付ける.
	$("#remocon_remoconmenu_ul li").each(function(){
		var itemObject = $(this);

		var menutype = itemObject.find(".type").text();
		if (menutype == "SPACE")
		{
			itemObject.html("");
			return ;
		}

		itemObject.find(".actionmenuicon_clickable,.timericon").bind({'touchstart mousedown': function(){
			showUpdatewaitDialog( {"htmlobject": itemObject.find(".popupselect").parent()  , "cancel": hideUpdatewaitDialog , "callback": remocon_get_append_info } );
			}
		});

		//でかいアイコンをクリックした時に、ステータスをトグルスイッチみたいに切り替えます。
		itemObject.find(".clickable").bind({'touchstart mousedown': function(e) {
			e.preventDefault();
			g_longtouchtimer = setTimeout(function(){
				g_longtouchtimer = null;
				on_elec_icon_longclick(itemObject,menutype);
			},2000);
		}
		,'touchend mouseup': function(e) {
			e.preventDefault();
			if(g_longtouchtimer)
			{
				clearTimeout(g_longtouchtimer);
				g_longtouchtimer = null;
				on_elec_icon_click(itemObject,menutype);
			}
		}
		,'mouseout': function(e) {
			e.preventDefault();
			if(g_longtouchtimer)
			{
				clearTimeout(g_longtouchtimer);
				g_longtouchtimer = null;
			}
		}
		});

		if (jQuery.browser.msie)
		{//IEは透明divのz-indexがバグっているので補正する
			itemObject.find(".mainicon")
				.css("cursor","pointer")
				.bind({'touchstart mousedown':function(){
					itemObject.find(".clickable").mousedown();
				}
				});
		}
	});

	//ローカルだったらフッターメニューを表示する
	if (g_remoconValue["is_local"]) 
	{
		$(".footermenu").show();
	}

	if (! g_remoconValue["is_link"])
	{//ローカル端末とリンクできているかどうか？
		//リンクできなていない
		showUpdatewaitDialog( {"htmlobject": $("#loadingwait_dialog_bad_connect") , "cancel": hideUpdatewaitDialog } );
	}

	//電話がかかってきているなら、相手を表示する.
	show_dialog_sipevent_incoming(g_remoconValue["sipevent_incoming"]);
	//通話中なら相手を表示
	show_dialog_sipevent_call(g_remoconValue["sipevent_call"]);
	//電話のキーパッドを表示
	show_dialog_sipevent_numpad(g_remoconValue["sipevent_numpad"]);

	if( g_remoconValue["is_multiroom"] )
	{
		$('.navigationbar_icon').bind({'touchstart mousedown': function(){
			multiroom_change("n");
		}}).css({cursor: 'pointer'});
		
		//マルチルームがあれば スワイプ移動登録
		$("#remoconstatic").bind({'touchstart mousedown': function(e) {
			g_swipePos.x = e.pageX || e.originalEvent.changedTouches[0].pageX;
			g_swipePos.y = e.pageY || e.originalEvent.changedTouches[0].pageY;
			if(e.button && e.button != 2)
			{
				g_swipePos.x = 0;
				g_swipePos.y = 0;
			}
		}
		,'contextmenu': function(e){
			g_swipePos.x = 0;
			g_swipePos.y = 0;
		}
		,'touchend mouseup': function(e) {
			if(g_swipePos.x > 0)
			{
				var x = e.pageX || e.originalEvent.changedTouches[0].pageX;
				var y = e.pageY || e.originalEvent.changedTouches[0].pageY;
				var moveX = g_swipePos.x - x;
				var moveY = g_swipePos.y - y;
				if(moveY <= 100 && moveY >= -100 )
				{
					if(moveX >= 150)
					{
						e.preventDefault();
						multiroom_change("n");
					}
					if(moveX <= -150)
					{
						e.preventDefault();
						multiroom_change("b");
					}
				}
			}
			g_swipePos.x = 0;
			g_swipePos.y = 0;
		}
		});
	}

	$('#footermenu_buton_edit').bind({'touchstart mousedown': function(){
		goto_local('/edit/');
	}});
	$('#footermenu_buton_setting').bind({'touchstart mousedown': function(){
		goto_local('/edit/#setting');
	}});

	$(document).keydown(function(e)
	{
		if (e.keyCode == 27) 
		{// ESCAPE key pressed
		   g_UpdatewaitDialogCancelFunction();
		}
	});
	
	setInterval( on_timer_remocon_get_append_info , 5000);
});

function remocon_fire_bytype_ad(obj)
{
	hideUpdatewaitDialog( );

	var itemObject = obj.parent().parent();
	var type = itemObject.find(".ptype").text();
	var newstatus = obj.attr("actiontype");

	if (g_remoconValue["remoconsetting_dialog_donot_autoclose"] == "1")
	{//自動的にダイアログを閉じない(再び自動的に開くことで実現する)
		remocon_fire_bytype(type,newstatus,function(){
			showUpdatewaitDialog( {"htmlobject": itemObject.parent()  , "cancel": hideUpdatewaitDialog , "callback": remocon_get_append_info ,"nowait": true } );
		});
	}
	else
	{
		remocon_fire_bytype(type,newstatus);
	}
}

function remocon_fire_bytype(type1,type2,callback)
{
	var reloadpage = false;
	//表示するステータスを更新する.
	$("#remocon_remoconmenu_ul li").each(function(){
		var itemObject = $(this);
		if ( itemObject.find(".type").text() != type1 )
		{
			return true;
		}
		var nostateremocon = false;
		var statuscolor = "#000000";
		itemObject.find("[actiontype]").each(function(){
			var actionObject = $(this);

			if ( actionObject.attr("actiontype") == type2 ) 
			{
				if ( actionObject.hasClass("settimer") )
				{//ページをリロードした方がいい？
				
					itemObject.find(".timericon").show();
				}
				else if ( actionObject.hasClass("resettimer") )
				{//ページをリロードした方がいい？
					itemObject.find(".timericon").hide();
				}

				if ( actionObject.hasClass("nostateremocon") )
				{//ステータスを変更しない
					nostateremocon = true;
				}
				else
				{//ステータスを変更してもいい
					nostateremocon = false;
					statuscolor = actionObject.css("color");
					actionObject.addClass("selected");
				}
			}
			else
			{
				actionObject.removeClass("selected");
			}
		});

		if (!nostateremocon)
		{
			itemObject.find(".status_label").css("color",statuscolor);
			itemObject.find(".status").text(type2);
			itemObject.find(".pstatus").text(type2).css("color",statuscolor);
		}
	});

	var arr = {};
	arr['type1']   =  type1;
	arr['type2']   =  type2;

	updateS(
		 "/remocon/fire/bytype"
		,arr
		,function(d){ 
			if ( d['result'] != "ok" )
			{
				if ( d['code'] == "40009" )
				{
					alert("命令を出しましたが結果を受け取れませんでした。");
				}
				else
				{
					alert("操作に失敗しました。\r\n\r\n" + d['message'] + "\r\nエラーコード:"+d['code']);
				}
			}
			
			callback && callback();
		}
		,{'error': function(){
		   }
		}
	);
}

function goto_local(url)
{
	//ローカルに移動する
	window.location.href = g_remoconValue["httpd__url"] + url;
}

function nothing()
{
}

function multiroom_change(movetype)
{
	showUpdatewaitDialog();

	var arr = {};
	arr['movetype']   =  movetype;

	updateS(
		 "/remocon/multiroom/change"
		,arr
		,function(d){ 
			location.replace(d['location']);
		}
	);
}

function sip_answer()
{
	updateS(
		 "/remocon/sip/answer"
		,{}
		,function(d){
			remocon_get_append_info();
		}
	);
}
function sip_hangup()
{
	updateS(
		 "/remocon/sip/hangup"
		,{}
		,function(d){ 
			remocon_get_append_info();
		}
	);
}
function sip_call(self)
{
	var sip_numpad_edit = $(self).parent().parent().find('.sip_numpad_edit');

	var tel = sip_numpad_edit.val();
	if (tel.length <= 2)
	{
		return;
	}

	updateS(
		 "/remocon/sip/call"
		,{ 'call': tel }
		,function(d){ 
			remocon_get_append_info();
		}
	);
}
function sip_numpad_close()
{
	hideUpdatewaitDialog();
	updateS(
		 "/remocon/sip/numpad/close"
		,{}
		,function(d){ 
		}
		,{'load': 'no' }
	);
}

var g_audioCtx = null;
function sip_pushnumber(self,str,hz1,hz2)
{
	//memo http://iphoneteq.net/faq/post-13562
	if (!g_audioCtx)
	{
		g_audioCtx = new (window.AudioContext || window.webkitAudioContext)();
	}
	var o1 = g_audioCtx.createOscillator();
	o1.type = 'square';
	o1.connect(g_audioCtx.destination);
	var o2 = g_audioCtx.createOscillator();
	o2.type = 'square';
	o2.connect(g_audioCtx.destination);
	
	o1.frequency.value = hz1;
	o2.frequency.value = hz2;
	o1.start = o1.start || o1.noteOn;
	o2.start = o1.start || o1.noteOn;
	o1.start();
	o2.start();
	setTimeout(function(){
		o1.stop();
		o2.stop();
		o1.connect(g_audioCtx.destination);
		o2.connect(g_audioCtx.destination);
	},300);

	var sip_numpad_edit = $(self).parent().parent().find('.sip_numpad_edit');
	sip_numpad_edit.val(sip_numpad_edit.val()+str).change();
}

//アニメーションステート
var g_UpdatewaitDialogStatus = 0;	//0:何もない  1:アニメーションウエイト  2:アニメーション終了  3:アニメーション終了待ち
var g_UpdatewaitDialogCancelFunction=nothing;
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
	g_UpdatewaitDialogCancelFunction = arr["cancel"] ? arr["cancel"] : nothing;

	$( "body" ).append('<div id="loadingwait" onclick="g_UpdatewaitDialogCancelFunction();" style="position: fixed; left: 0px; top: 0px; width: 100%; height: 100%; opacity: 0.9; z-index: 98; background-color: #000000;"></div><div id="loadingwait_dialog" style="position:absolute; color: #000000; background-color: #ffffff; left: -9999px; top: 0px; opacity: 1; z-index: 99;border-radius: 15px;padding:10px;" ></div>');
	var loadingwait = $( "body #loadingwait" );
	var loadingwait_dialog = $( "body #loadingwait_dialog" ).html( arr["htmlobject"].html() );
	if (loadingwait_dialog.width() > document.documentElement.clientWidth - 20)
	{
		loadingwait_dialog.width(document.documentElement.clientWidth- 20);
		loadingwait_dialog.height( loadingwait_dialog.height() + 20 ); //縦に押し出されることがあるので、少し伸ばす
	}
	var goal_left	= (document.documentElement.clientWidth  / 2) - (loadingwait_dialog.width()  / 2);
	var goal_top	= (document.documentElement.clientHeight / 2) - (loadingwait_dialog.height() / 2);
	if (goal_top < 0)
	{
		goal_top = 0;
	}

	//1:アニメーションウエイト
	g_UpdatewaitDialogStatus = 1;

	if(arr["nowait"])
	{
		loadingwait_dialog.css({left: goal_left,top: goal_top,opacity: 1,zIndex: 99}).show();
		UpdatewaitDialogAnimateEndCallback();
	}
	else
	{
		loadingwait_dialog.css({left: goal_left,top: 0,opacity: 1,zIndex: 99});
		loadingwait_dialog.transition({
			 left: goal_left
			,top:  goal_top
		},'fast',UpdatewaitDialogAnimateEndCallback);
	}

	if ( arr["callback"] )
	{
		var callbackFunction = arr["callback"];
		callbackFunction();
	}
}

function isShowDialog(dialog_name)
{
	if ( g_UpdatewaitDialogStatus == 0 )
	{//ダイアログを表示していない.
		return false;
	}
	return $( "body #loadingwait_dialog " + dialog_name )[0];
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
		g_UpdatewaitDialogCancelFunction = nothing;
	}
}

function UpdatewaitDialogAnimateEndCallback()
{
	if (g_UpdatewaitDialogStatus == 3)
	{
		$( "body #loadingwait" ).remove();
		$( "body #loadingwait_dialog" ).remove();
		g_UpdatewaitDialogStatus = 0;
		g_UpdatewaitDialogCancelFunction = nothing;
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