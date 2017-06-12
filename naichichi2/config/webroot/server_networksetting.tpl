<!DOCTYPE html>
<html lang="ja"><head>
	<meta name="format-detection" content="telephone=no" />
	<meta name="viewport" content="width=640,user-scalable=yes,maximum-scale=0.67" />
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=99" />
	<link rel="apple-touch-icon" href="/jscss/images/iphone_icon.png" />
	<link rel="shortcut icon" href="/jscss/images/favicon.ico" />
	<title>リモートからのネットワークの設定</title>
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
	
	#remotenetwork {
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
<div id="remotenetwork">
	<div class="header_pc"> 
		<h1><a href="/" ><img src="/jscss/images/fhc700.png" /></a></h1>
	</div>

	<div class="contents_wrapper">
		<div data-role="header" class="header_title">
			<fieldset class="navigationbar">
				<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_network.png" /></div>
				<div class="navigationbar_title"><span class="navigationbar_title_span">リモートからのネットワークの設定</span></div>
			</fieldset>
		</div>
		<div class="contents_main">
			<br />
			<span id="account_mail"></span>さんの端末を設定します。<br />
			<br />
			<div class="error network_error">エラーが発生しました!!</div>
			<dl>
			<dt class="sub_head">有線ケーブルのネットワーク</dt>
			<dd>
				<select id="network_ipaddress_type" size="1"  class="mselect_tag">
				<option value="DHCP">DHCPでの自動取得</option>
				<option value="STATIC">固定IP</option>
				</select>
				<div class="error" id="network_error_40010">DHCPか固定IPのいづれを選択してください。</div>
				<div id="network_ipaddress_div">
					<dl>
					<dt>IPアドレス</dt>
					<dd>
						<input class="minput_tag" type="text" id="network_ipaddress_ip" value="" placeholder="IPアドレスを入れてください。例:192.168.1.20" pattern="\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}">
						<div class="error" id="network_error_40001">IPアドレスが正しくありません。</div>
					</dd>
					<dt>netmask</dt>
					<dd>
						<input class="minput_tag" type="text" id="network_ipaddress_mask" value="" placeholder="NETMASKを入れてください。例:255.255.255.0" pattern="\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}"  list="autocomplete_netmask_list">
						<div class="error" id="network_error_40002">netmaskが正しくありません。</div>
					</dd>
					<dt>gateway</dt>
					<dd>
						<input class="minput_tag" type="text" id="network_ipaddress_gateway" value="" placeholder="gatewayを入れてください。例:192.168.1.1" pattern="\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}">
						<div class="error" id="network_error_40003">gatewayが正しくありません。</div>
					</dd>
					<dt>DNS</dt>
					<dd>
						<input class="minput_tag" type="text" id="network_ipaddress_dns" value="" placeholder="DNSを入れてください。例:192.168.1.1" pattern="\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}">
						<div class="error" id="network_error_40004">dnsが正しくありません。</div>
					</dd>
					</dl>
				</div>
				
			</dd>
			</dl>

			<div id="network_is_wifi_device">
				<dl>
				<dt class="sub_head">無線のネットワーク</dt>
				<dd>
					<select id="network_w_ipaddress_type" size="1">
					<option value="NONE">NONE</option>
					<option value="DHCP">DHCPでの自動取得</option>
					<option value="STATIC">固定IP</option>
					</select>
					<div class="error" id="network_error_40030">DHCP、固定IP、NONEのいづれを選択してください。</div>
					<div id="network_w_ipaddress_div">
						<dl>
						<dt>IPアドレス</dt>
						<dd>
							<input class="minput_tag" type="text" id="network_w_ipaddress_ip" value="" placeholder="IPアドレスを入れてください。例:192.168.2.20" pattern="\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}">
							<div class="error" id="network_error_40021">IPアドレスが正しくありません。</div>
						</dd>
						<dt>netmask</dt>
						<dd>
							<input class="minput_tag" type="text" id="network_w_ipaddress_mask" value="" placeholder="NETMASKを入れてください。例:255.255.255.0" pattern="\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}" list="autocomplete_netmask_list">
							<div class="error" id="network_error_40022">netmaskが正しくありません。</div>
						</dd>
						<dt>gateway</dt>
						<dd>
							<input class="minput_tag" type="text" id="network_w_ipaddress_gateway" value="" placeholder="gatewayを入れてください。例:192.168.2.1" pattern="\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}">
							<div class="error" id="network_error_40023">gatewayが正しくありません。</div>
						</dd>
						<dt>DNS</dt>
						<dd>
							<input class="minput_tag" type="text" id="network_w_ipaddress_dns" value="" placeholder="DNSを入れてください。例:192.168.2.1" pattern="\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}"s>
							<div class="error" id="network_error_40024">dnsが正しくありません。</div>
						</dd>
						</dl>
					</div>
					<div id="network_w_ipaddress_use">
						<dl>
						<dt>SSID</dt>
						<dd>
							<input class="minput_tag" type="text" id="network_w_ipaddress_ssid" value="" placeholder="お使いのwifiのSSIDを入れてください">
							<div class="error" id="network_error_40025">SSIDが正しくありません。</div>
						</dd>
						<dt>password</dt>
						<dd>
							<input class="minput_tag" type="password" id="network_w_ipaddress_password" value="" placeholder="お使いのwifiのpasswordを入れてください">
							<div class="error" id="network_error_40026">passwordが正しくありません。</div>
						</dd>
						<dt>暗号方式</dt>
						<dd>
							<select id="network_w_ipaddress_wkeymgmt" size="1">
							<option value="WPA2-PSK/AES">WPA2-PSK/AES</option>
							<option value="WPA2-PSK/TKIP">WPA2-PSK/TKIP</option>
							<option value="WPA-PSK/AES">WPA-PSK/AES</option>
							<option value="WPA-PSK/TKIP">WPA-PSK/TKIP</option>
							<option value="WPA">WPA</option>
							</select>
							<div class="error" id="network_error_40027">暗号方式が正しくありません。</div>
						</dd>
						</dl>
					</div>
				</dd>
				</dl>
			</div>
			<datalist id="autocomplete_netmask_list">
				<option value="255.255.255.0">
			</datalist>

				<div align="right"><br /><div  align="left" class="papertag red">
					Future Home Controller の IPアドレスの設定を行います。<br />
					項目がよくわからない場合は、何も設定しないで「設定をする」を押してください。<br />
					<br />
				</div></div>

		<br class="clearfix">
		<br />

		<div>
			<a href="javascript:void(0)" id="network_apply_button" ><span class="button">設定する</span></a>
		</div>
		<div class="error network_error">エラーが発生しました!!</div>
		<br />
	</div><!-- /contents_main -->
	</div><!-- /contents_wrapper -->



	<div style="display:none;" id="loadingwait_dialog_wait">
		<div style="text-align:center;"><img src="/jscss/images/ajax-loader.gif" style="padding-right:4px;" /></div>
		処理中です・・・・
	</div>
	<div style="display:none;" id="loadingwait_dialog_setting_readly">
		<p><a href="/networksetting/user/networksetting.reset" onclick="checkstart(); return true;"><span class="button">設定ファイルダウンロードをする</span></a></p>
		↑このファイルをダウンロードして、<br />
		welcome.html があったフォルダにコピーしてください。<br />
		<br />
		<br />
		<img src="/jscss/images/copy_networkreset.png" width="508" height="347"  /><br />
		welcome.html があったフォルダにコピー後、<br />
		1分ほどお待ちいただけると、利用できるようになります。<br />
		<br />
		<br />
		
	</div>
		
	<div style="display:none;" id="loadingwait_dialog_rebootwait">
		フューチャーホームコントローラーからの信号を受信しました。<br />
		<br />
		引き続きフューチャーホームコントローラーを再起動しますので、<br />
		このまま、2分ほどお待ちください。<br />
		<br />
		<br />
		この間に、USBケーブルの取り外しをお願いします。<br />
		(この初期化手続以外にUSBケーブルは利用しません。)<br />
		<div style="text-align:center;"><img src="/jscss/images/ajax-loader.gif" style="padding-right:4px;" /></div><br />
	</div>

	<div style="display:none;" id="loadingwait_dialog_checkok">
		初期化手続きが終わりました。<br />
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
var g_remotenetworkObject = null;
var g_SETTING = %SETTING%;
var g_AudioObject = null;

$(function(){
	g_remotenetworkObject = $("#remotenetwork");

	g_remotenetworkObject.find("#network_ipaddress_type").change(function(){
		if ( $(this).val() == "DHCP" || $(this).val() == "NONE")
		{
			g_remotenetworkObject.find("#network_ipaddress_div").hide();
		}
		else
		{
			g_remotenetworkObject.find("#network_ipaddress_div").show();
		}
	});

	g_remotenetworkObject.find("#network_w_ipaddress_type").change(function(){
		if ( $(this).val() == "DHCP" )
		{
			g_remotenetworkObject.find("#network_w_ipaddress_div").hide();
			g_remotenetworkObject.find("#network_w_ipaddress_use").show();
		}
		else if ( $(this).val() == "NONE" )
		{
			g_remotenetworkObject.find("#network_w_ipaddress_div").hide();
			g_remotenetworkObject.find("#network_w_ipaddress_use").hide();
		}
		else
		{
			g_remotenetworkObject.find("#network_w_ipaddress_div").show();
			g_remotenetworkObject.find("#network_w_ipaddress_use").show();
		}
	});

	//設定するボタンをクリックした時
	g_remotenetworkObject.find("#network_apply_button").click(function(){
		//読込中にする
		showUpdatewaitDialog();
		g_remotenetworkObject.find(".error").hide();
		var updateArray = {
			  'network_ipaddress_type':					g_remotenetworkObject.find("#network_ipaddress_type").val()
			 ,'network_ipaddress_ip':					g_remotenetworkObject.find("#network_ipaddress_ip").val()
			 ,'network_ipaddress_mask':					g_remotenetworkObject.find("#network_ipaddress_mask").val()
			 ,'network_ipaddress_gateway':				g_remotenetworkObject.find("#network_ipaddress_gateway").val()
			 ,'network_ipaddress_dns':					g_remotenetworkObject.find("#network_ipaddress_dns").val()
			 ,'network_w_ipaddress_type':				g_remotenetworkObject.find("#network_w_ipaddress_type").val()
			 ,'network_w_ipaddress_ip':					g_remotenetworkObject.find("#network_w_ipaddress_ip").val()
			 ,'network_w_ipaddress_mask':				g_remotenetworkObject.find("#network_w_ipaddress_mask").val()
			 ,'network_w_ipaddress_gateway':			g_remotenetworkObject.find("#network_w_ipaddress_gateway").val()
			 ,'network_w_ipaddress_dns':				g_remotenetworkObject.find("#network_w_ipaddress_dns").val()
			 ,'network_w_ipaddress_ssid':				g_remotenetworkObject.find("#network_w_ipaddress_ssid").val()
			 ,'network_w_ipaddress_password':			g_remotenetworkObject.find("#network_w_ipaddress_password").val()
			 ,'network_w_ipaddress_wkeymgmt':			g_remotenetworkObject.find("#network_w_ipaddress_wkeymgmt").val()
			 ,'_csrftoken':								"%CSRFTOKEN%"
		};
		$.ajax({
			type: "post",
			url: "/networksetting/network/update",
			timeout: 50000,
			data: updateArray,
			dataType: 'json',
			success: function(d){
				hideUpdatewaitDialog();
				if (d['result'] == "error")
				{//エラーになりました
					g_remotenetworkObject.find(".network_error").show();
					if(d['code'] > 40000 && d['code'] <= 40100)
					{
						g_remotenetworkObject.find("#network_error_" + d['code']).show();
					}
					return ;
				}
				//用意はいいですか？ページを出す
				showUpdatewaitDialog( {"htmlobject": g_remotenetworkObject.find("#loadingwait_dialog_setting_readly") } );
				
				return ;
			},
			error: function(d){
				hideUpdatewaitDialog();
				g_remotenetworkObject.find(".network_error").show();
				return ;
			}
		});
	});
	
	load();
});

var g_checkcount = 0;
function checkstart()
{
	if (g_checkcount >= 1)
	{//既に監視を開始しているっぽい。最初からやり直させる
		g_checkcount = 0;
	}

	var checkWaitFunction = function(deadcount) {
		g_checkcount ++;
		if(g_checkcount >= 80)
		{//160秒以内にpooling が開始されなければエラー
			g_checkcount = 0;
			return ;
		}

		//サーバーにpoolingが開始されるかを見る
		$.ajax({
			type: "post",
			url: "/networksetting/checkwait",
			timeout: 2000,
			data: {'_csrftoken': "%CSRFTOKEN%"},
			dataType: 'json',
			success: function(d){
				if (d["is_link"])
				{//コネクションが確立された!!
					//OKの画面を出す
					//引き続き、リブート待ち
					g_checkcount = 0;

					showUpdatewaitDialog( {"htmlobject": g_remotenetworkObject.find("#loadingwait_dialog_rebootwait") } );

					//再起動処理の時間を稼ぐ
					//その後は再起動後にpooling があるまで待機
					setTimeout( rebootcheckstart , 100*1000);
					return ;
				}
				else
				{
					setTimeout( checkWaitFunction , 2000);
				}
				return ;
			},
			error: function(d){
				setTimeout( checkWaitFunction , 2000);
				return ;
			}
		});
	};

	//最初のチェックは20秒後ぐらいに
	setTimeout( checkWaitFunction , 20*1000);
}

var g_rebootcheckcount = 0;
function rebootcheckstart()
{
	if (g_rebootcheckcount >= 1)
	{//既に監視を開始しているっぽい 最初から監視をやり直させる
		g_rebootcheckcount = 1;
	}

	var rebootcheckWaitFunction = function(deadcount) {
		g_rebootcheckcount ++;
		if(g_rebootcheckcount >= 80)
		{//160秒以内にpooling が開始されなければエラー
			g_rebootcheckcount = 0;
			return ;
		}

		//サーバーにpoolingが開始されるかを見る
		$.ajax({
			type: "post",
			url: "/networksetting/checkwait",
			timeout: 2000,
			data: {'_csrftoken': "%CSRFTOKEN%"},
			dataType: 'json',
			success: function(d){
				if (d["is_link"])
				{//コネクションが確立された!!
					//OKの画面を出す
					showUpdatewaitDialog( {"htmlobject": g_remotenetworkObject.find("#loadingwait_dialog_checkok") } );
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
}


function on_client_login_button()
{
	window.location.href = "/remocon/"
}



function load()
{
	g_remotenetworkObject.find(".error").hide();
	g_remotenetworkObject.find("#network_ipaddress_type").val(g_SETTING["network_ipaddress_type"]).change();
	g_remotenetworkObject.find("#network_ipaddress_ip").val(g_SETTING["network_ipaddress_ip"]).change();
	g_remotenetworkObject.find("#network_ipaddress_mask").val(g_SETTING["network_ipaddress_mask"]).change();
	g_remotenetworkObject.find("#network_ipaddress_gateway").val(g_SETTING["network_ipaddress_gateway"]).change();
	g_remotenetworkObject.find("#network_ipaddress_dns").val(g_SETTING["network_ipaddress_dns"]).change();
	g_remotenetworkObject.find("#network_w_ipaddress_type").val(g_SETTING["network_w_ipaddress_type"]).change();
	g_remotenetworkObject.find("#network_w_ipaddress_ip").val(g_SETTING["network_w_ipaddress_ip"]).change();
	g_remotenetworkObject.find("#network_w_ipaddress_mask").val(g_SETTING["network_w_ipaddress_mask"]).change();
	g_remotenetworkObject.find("#network_w_ipaddress_gateway").val(g_SETTING["network_w_ipaddress_gateway"]).change();
	g_remotenetworkObject.find("#network_w_ipaddress_dns").val(g_SETTING["network_w_ipaddress_dns"]).change();
	g_remotenetworkObject.find("#network_w_ipaddress_ssid").val(g_SETTING["network_w_ipaddress_ssid"]).change();
	g_remotenetworkObject.find("#network_w_ipaddress_password").val(g_SETTING["network_w_ipaddress_password"]).change();
	g_remotenetworkObject.find("#network_w_ipaddress_wkeymgmt").val(g_SETTING["network_w_ipaddress_wkeymgmt"]).change();
	g_remotenetworkObject.find("#account_mail").text(g_SETTING["account_mail"]);

	var model = getparam("model","")
	if (0)
	{
		g_remotenetworkObject.find("#network_is_wifi_device").show();
	}
	else
	{
		g_remotenetworkObject.find("#network_is_wifi_device").hide();
	}
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