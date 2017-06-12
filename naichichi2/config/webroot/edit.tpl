<!DOCTYPE html>
<html lang="ja"><head>
	<meta name="format-detection" content="telephone=no" />
	<meta name="viewport" content="width=640,user-scalable=no,target-densitydpi=device-dpi" />
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=99" />
	<link rel="apple-touch-icon" href="/jscss/images/iphone_icon.png" />
	<link rel="shortcut icon" href="/jscss/images/favicon.ico" />
	<title>設定</title>
	<base href="/"></base>
	<link type="text/css" href="/jscss/jquery.mobile.css" rel="stylesheet" />
    <link rel="stylesheet" href="/jscss/codemirror/codemirror.css">
    <link rel="stylesheet" href="/jscss/jquery.dd/dd.css">
    <link rel="stylesheet" href="/jscss/edit.css">
	<script type="text/javascript" src="/jscss/jquery.js"></script>
	<script type="text/javascript" src="/jscss/jquery.mobile-1.2.0.min.js"></script>
	<script type="text/javascript" src="/jscss/jquery.upload-1.0.2.js"></script>
	<script type="text/javascript" src="/jscss/codemirror/codemirror.js"></script>
	<script type="text/javascript" src="/jscss/codemirror/javascript.js"></script>
	<script type="text/javascript" src="/jscss/jquery.transit.js"></script>
	<script type="text/javascript" src="/jscss/jquery.dd.js"></script>
	<script type="text/javascript" src="/jscss/ccchart.js"></script>
	<script type="text/javascript" src="/jscss/cpick.js"></script>
	<script type="text/javascript" src="/jscss/edit.js"></script>
</head>
<body> 
<div data-role="page" id="remoconedit" data-url="remoconedit" data-theme="c" class="">
	<div data-role="header" class="header_pc"> 
		<h1><a data-role="none" href="#setting" ><img src="/jscss/images/fhc700.png" /></h1> 
	</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a href="/remocon/" data-role="button" data-ajax="false" data-icon="back"><span>リモコン</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_remoconedit.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">リモコンの編集</span></div>
		</fieldset>
	</div> 
	<div data-role="content" data-theme="c" class="contents_wrapper" role="main" id="remoconmenu_li_sample_content">

	<ul id="remoconmenu_li_sample" class="js_template">
		<li class="item %APPENDCLASS%">
				<span class="key hidden">%KEY%</span>
				<a href="javascript:void(0)" class="delicon"><img src="/jscss/images/delicon.png" /></a>
				<div class="mainicon"><img src="%ICON%" width="128" height="128" /></div>
				<div class="type">%TYPE%</div>
				<div class="status">%STATUS%</div>
				<a href="javascript:void(0)" class="editicon"><img src="/jscss/images/editicon.png" /></a>
		</li>
	</ul>
	<ul id="remocon_remoconmenu_ul">
	</ul>
	<br class="clearfix">
	<br />
	<div align="right"><div  align="left" class="papertag red">
		<p>テレビなどの家電の項目を作ります。<br />
		アイコンをドラッグして並び順を変えられます。<br />
		リモコンを使うには、「リモコンを使う」を押してください。
		</p>
	</div></div>

	<div class="footermenu">
		<div class="footermenu_buton" onclick="elec_load('new');">
			<div class="footermenu_buton_icon"><img src="/jscss/images/icon_newelec.png" width="64" height="64" /></div>
			<div class="footermenu_buton_name">　新規作成する</div>
		</div>
		<div class="footermenu_buton" onclick="gotohref('/remocon/');">
			<div class="footermenu_buton_icon"><img src="/jscss/images/setting_icon64_returnremocon.png" /></div>
			<div class="footermenu_buton_name">リモコンを使う</div>
		</div>
		<br class="clearfix">
	</div>

	</div><!-- /content -->
</div><!-- /page top -->
<div class="hidden" id="loadingwait_dialog_setting">
	<div class="dialog">
		<div style="text-align:center;"><img src="/jscss/images/ajax-loader.gif" style="padding-right:4px;" /></div>
		<span class="dialogmessage">
		設定中です・・・
		</span>
	</div>
</div>
<div class="hidden" id="loadingwait_dialog_irlearn">
	<div class="dialog">
		<div style="text-align:center;">
			<div style="float: left">
				<img src="/jscss/images/icon_ir.png" style="padding-right:4px;" />
			</div>
			<div style="float: left">
				<span class="dialogmessage">
				赤外線信号を学習中です。<br />
				フューチャーホームコントローラーの上面にある三角マークの先に向けて、リモコンボタンを押してください。<br />
				</span>
			</div>
			<br class="clearfix">
		</div>
	</div>
</div>
<div class="hidden" id="loadingwait_dialog_reboot">
	<div class="dialog">
		<div style="text-align:center;"><img src="/jscss/images/ajax-loader.gif" style="padding-right:4px;" /></div>
		<span class="dialogmessage">
		再起動中です。<br />
		1分ほどお待ちください。<br />
		</span>
	</div>
</div>
<div class="hidden" id="loadingwait_dialog_upgrade">
	<div class="dialog">
		<div style="text-align:center;"><img src="/jscss/images/ajax-loader.gif" style="padding-right:4px;" /></div>
		<span class="dialogmessage">
		アップデート中です。<br />
		数分ほどお待ちください。<br />
		アップデート中に電源を抜かないでください。<br />
		</span>
	</div>
</div>
<div class="hidden" id="loadingwait_dialog_shutdown">
	<div class="dialog">
		<div style="text-align:center;"><img src="/jscss/images/ajax-loader.gif" style="padding-right:4px;" /></div>
		<span class="dialogmessage">
		システムを停止しています。<br />
		1分ほどしてから電源を抜いてください。<br />
		</span>
	</div>
</div>
<div class="hidden" id="loadingwait_dialog_import">
	<div class="dialog">
		<div style="text-align:center;"><img src="/jscss/images/ajax-loader.gif" style="padding-right:4px;" /></div>
		<span class="dialogmessage">
		インポート中です。<br />
		数分ほどお待ちください。<br />
		インポート中には、電源を抜かないでください。<br />
		</span>
	</div>
</div>

<div class="hidden" id="loadingwait_dialog_newnameprompt">
	<div class="dialog">
		<div style="text-align:center;">
			<div style="float: left">
				<img src="/jscss/images/dialog_icon64_append.png" style="padding-right:4px;" />
			</div>
			<div style="float: left">
				<br /><span class="dialogmessage">新規ファイル名を入れてください</span>
			</div>
			<br class="clearfix">
		</div>
		<br />
		<input type="text" class="value focus"><br />
		<input type="button" value="OK" class="ok_button"><input type="button" value="cancel" class="cancel_button"><br />
	</div>
</div>
<div class="hidden" id="loadingwait_dialog_webapi_key_change">
	<div class="dialog">
		<div style="text-align:center;">
			<div style="float: left">
				<img src="/jscss/images/dialog_icon64_delete.png" style="padding-right:4px;" />
			</div>
			<div style="float: left">
				<br /><span class="dialogmessage">キーを変更してもよろしいですか？</span>
			</div>
			<br class="clearfix">
		</div>
		<br />
		<input type="button" value="OK" class="ok_button"><input type="button" value="cancel" class="cancel_button focus"><br />
	</div>
</div>
<div class="hidden" id="loadingwait_dialog_delconfirm">
	<div class="dialog">
		<div style="text-align:center;">
			<div style="float: left">
				<img src="/jscss/images/dialog_icon64_delete.png" style="padding-right:4px;" />
			</div>
			<div style="float: left">
				<br /><span class="dialogmessage">削除してもよろしいですか？</span>
			</div>
			<br class="clearfix">
		</div>
		<br />
		<input type="button" value="OK" class="ok_button"><input type="button" value="cancel" class="cancel_button focus"><br />
	</div>
</div>
<div class="hidden" id="loadingwait_dialog_updateconfirm">
	<div class="dialog">
		<div style="text-align:center;">
			<div style="float: left">
				<img src="/jscss/images/dialog_icon64_hatena.png" style="padding-right:4px;" />
			</div>
			<div style="float: left">
				<br /><span class="dialogmessage">アップデートしてもよろしいですか？</span>
			</div>
			<br class="clearfix">
		</div>
		<br />
		<input type="button" value="OK" class="ok_button"><input type="button" value="cancel" class="cancel_button focus"><br />
	</div>
</div>
<div class="hidden" id="loadingwait_dialog_rebootconfirm">
	<div class="dialog">
		<div style="text-align:center;">
			<div style="float: left">
				<img src="/jscss/images/dialog_icon64_hatena.png" style="padding-right:4px;" />
			</div>
			<div style="float: left">
				<br /><span class="dialogmessage">再起動してもよろしいですか？</span>
			</div>
			<br class="clearfix">
		</div>
		<br />
		<input type="button" value="OK" class="ok_button"><input type="button" value="cancel" class="cancel_button focus"><br />
	</div>
</div>
<div class="hidden" id="loadingwait_dialog_shutdownconfirm">
	<div class="dialog">
		<div style="text-align:center;">
			<div style="float: left">
				<img src="/jscss/images/dialog_icon64_hatena.png" style="padding-right:4px;" />
			</div>
			<div style="float: left">
				<br /><span class="dialogmessage">シャットダウンしてもよろしいですか？</span>
			</div>
			<br class="clearfix" />
		</div>
		<br />
		<input type="button" value="OK" class="ok_button"><input type="button" value="cancel" class="cancel_button focus"><br />
	</div>
</div>
<div class="hidden" id="loadingwait_dialog_search">
	<div class="dialog">
		<div style="text-align:center;"><img src="/jscss/images/ajax-loader.gif" style="padding-right:4px;" /></div>
		<span class="dialogmessage">
		検索中・・・
		</span>
	</div>
</div>
<div data-role="page" data-url="fileselectpage" id="fileselectpage" data-theme="c" data-url="]" tabindex="0">
	<div data-role="header" class="header_pc"> 
		<h1><a data-role="none" href="#setting" ><img src="/jscss/images/fhc700.png" ></a></h1> 
	</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a href="javascript:history.back()" data-role="button" class="back" data-icon="back"><span>戻る</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/page_icon64_fileselectpage.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">
				<span class="fileselectpage_is_actionicon">アイコンの選択</span>
				<span class="fileselectpage_is_elecicon">アイコンの選択</span>
				<span class="fileselectpage_is_script_command">スクリプトの選択</span>
				<span class="fileselectpage_is_tospeak_mp3">mp3音楽の選択</span></span></div>
		</fieldset>
	</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">
		<div class="filelist_controll">
			<fieldset class="tr">
				<div class="th"><span>検索</span></div>
				<div class="td"><span><input type="text" data-mini="true" class="search" value="" placeholder="検索するファイル名"></span></div>
			</fieldset>

			<input type="hidden" class="typepath" value="" />
					<span class="fileselectpage_is_elecicon">
						<div align="right"><div  align="left" class="papertag green">
							<p>リモコン画面に表示したいアイコン画像を選択してください。<br />
							<br />
							</p>
						</div></div>
					</span>
					<span class="fileselectpage_is_actionicon">
						<div align="right"><div  align="left" class="papertag green">
							<p>メニューダイアログの状態に表示するアイコンです。<br />
							102px * 107pxです。<br />
							<br />
							</p>
						</div></div>
					</span>
					<span class="fileselectpage_is_tospeak_mp3">
						<div align="right"><div  align="left" class="papertag green">
							<p>しゃべらせる音声ファイルを選択してください。<br />
							<br />
							</p>
						</div></div>
					</span>
					<span class="fileselectpage_is_script_command">
						<div align="right"><div  align="left" class="papertag green">
							<p>外部の機材やパソコンと連携させるコマンドを指定してください<br />
							<br />
							</p>
						</div></div>
					</span>

		</div><!-- /filelist_controll -->
		<ul class="filelist_li_sample js_template">
			<li class="filelist_li">
				<div class="fileicon"  >
					<img src="%ICON%" width="96" height="96" />
				</div>
				<div class="filename">%NAME%</div>
				<span class="filesize">サイズ:<span>%SIZE%</span></span>
				<span class="filedate">日付:<span>%DATE%</span></span>
				<div class="fileusebutton" >
					<a href="javascript:void(0)" data-role="button" data-icon="check"  data-mini="true" class="useicon">これを使う</a>
				</div>
				<div class="filedelbutton">
					<a href="javascript:void(0)" data-role="button" data-icon="delete"  data-mini="true" class="delete">削除</a>
				</div>
			</li>
		</ul>
		<ul class="filelist_li_none_sample js_template">
			<li class="filelist_li">
				<div class="fileicon"  >
					<img src="/jscss/images/fileselectpage_none_icon.png" width="96" height="96" />
				</div>
				<div class="filename nouse">
					<span class="fileselectpage_is_tospeak_mp3">
						(再生しない)
					</span>
				</div>
				<span class="filesize"></span>
				<span class="filedate"></span>
				<div class="fileusebutton" >
					<a href="javascript:void(0)" data-role="button" data-icon="check"  data-mini="true" class="useicon">これを使う</a>
				</div>
				<div class="filedelbutton">
				</div>
			</li>
		</ul>
		<ul class="filelist_ul">
		</ul><!-- /filelist_ul -->
		
		<div class="fileselectpage_is_elecicon fileselectpage_is_actionicon fileselectpage_is_tospeak_mp3"> <!-- new -->
			<div>
				<div align="right">
						<span class="fileselectpage_is_actionicon">
							<a data-role="button" class="new" data-inline="true" data-icon="plus" onclick="g_fileselectpageObject.find('#uploadfile1').click();"><span>新しい画像ファイルを新規アップロード</span></a>
						</span>
						<span class="fileselectpage_is_elecicon">
							<a data-role="button" class="new" data-inline="true" data-icon="plus" onclick="g_fileselectpageObject.find('#uploadfile1').click();"><span>新しい画像ファイルを新規アップロード</span></a>
						</span>
						<span class="fileselectpage_is_tospeak_mp3">
							<a data-role="button" class="new" data-inline="true" data-icon="plus" onclick="g_fileselectpageObject.find('#uploadfile1').click();"><span>新しい音楽ファイルを新規アップロード</span></a>
						</span>
						<span class="hidden">
							<input type="file" id="uploadfile1" name="uploadfile1" value="アップロード"></span>
						</span>
						
				</div>
			</div>
			<div class="error error_20005">エラー:未サポートのファイル形式です。</div>
		</div>

	</div><!-- /content -->
</div><!-- /page fileselectpage -->

<div data-role="page" id="elec" data-theme="c" data-url="elec" tabindex="0">
	<div data-role="header" class="header_pc"> 
		<h1><a data-role="none" href="#setting" ><img src="/jscss/images/fhc700.png" /></h1> 
	</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a data-role="button" class="menubutton" data-inline="true" href="javascript:void(0)" onclick="remoconedit_load();" data-ajax="false" data-icon="back"><span>リモコンの編集</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/page_icon64_elec.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">機材設定</span></div>
		</fieldset>
	</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">	
	<div class="error bunner_error">エラーが発生しました!!</div>

	<input type="hidden" id="elec_key" value="">
	<dl>
	<dt class="sub_head">種類</dt>
	<dd>
		<select id="type" size="1">
		<option value="エアコン" yomi="エアコン" title="/jscss/images/elecicon/aircon.png">エアコン</option>
		<option value="テレビ" yomi="テレビ" title="/jscss/images/elecicon/tv.png">テレビ</option>
		<option value="照明" yomi="でんき" title="/jscss/images/elecicon/elec.png">照明</option>
		<optgroup label="あ-">
			<option value="エアコン" yomi="エアコン" title="/jscss/images/elecicon/aircon.png">エアコン</option>
		</optgroup>
		<optgroup label="か-">
			<option value="カーテン" yomi="カーテン" title="/jscss/images/elecicon/icon.png">カーテン</option>
			<option value="空気清浄機" yomi="くうきせいじょうき" title="/jscss/images/elecicon/elec5.png">空気清浄機</option>
			<option value="コンポーザ" yomi="コンポーザ" title="/jscss/images/elecicon/speaker.png">コンポーザ</option>
			<option value="ゲーム機" yomi="ゲームき" title="/jscss/images/elecicon/game.png">ゲーム機</option>
		</optgroup>
		<optgroup label="さ-">
			<option value="照明" yomi="でんき" title="/jscss/images/elecicon/elec.png">照明</option>
			<option value="扇風機" yomi="せんぷうき" title="/jscss/images/elecicon/fan.png">扇風機</option>
			<option value="衛星放送" yomi="えいせいほうそう" title="/jscss/images/elecicon/satellite.png">衛星放送</option>
		</optgroup>
		<optgroup label="た-">
			<option value="テレビ" yomi="テレビ" title="/jscss/images/elecicon/tv.png">テレビ</option>
			<option value="ディスプレイ" yomi="ディスプレイ" title="/jscss/images/elecicon/monitor.png">ディスプレイ</option>
			<option value="レコーダ" yomi="レコーダー" title="/jscss/images/elecicon/cd.png">レコーダ</option>
			<option value="ドア" yomi="ドア" title="/jscss/images/elecicon/door.png">ドア</option>
		</optgroup>
		<optgroup label="な-">
		</optgroup>
		<optgroup label="は-">
			<option value="パソコン" yomi="パソコン" title="/jscss/images/elecicon/pc.png">パソコン</option>
			<option value="プロジェクタ" yomi="プロジェクター"  title="/jscss/images/elecicon/project.png">プロジェクタ</option>
			<option value="ファン" yomi="ファン" title="/jscss/images/elecicon/fan.png">ファン</option>
		</optgroup>
		<optgroup label="ま-">
			<option value="窓" yomi="まど" title="/jscss/images/elecicon/door.png">ドア</option>
		</optgroup>
		<optgroup label="や-">
		</optgroup>
		<optgroup label="ら-">
		</optgroup>
		<optgroup label="わ-">
		</optgroup>
		<option value="SPACE" yomi="よはく" title="/jscss/images/elecicon/space.png">SPACE</option>
		<option value="other" title="/jscss/images/elecicon/onoff.png">その他</option>
		</select>

		<div id="type_other_div">
		<p>その他種類名:</p>
		<input type="text" id="type_other" value="">
		<input type="hidden" id="type_yomi" value="">
		<div class="error error_10921">その他の名前を入れてください。</div>
		</div>
		<div class="error error_10922">この種類は既に使われています。<br />別の種類を選択してください。</div>
			<div align="right"><br /><div  align="left" class="papertag red">
				<p>新規に登録する家電の種類を教えて下さい。<br />
				リストにない場合は、その他を選択して名前を入れてください。<br />
				複数台の機器を操作したい場合、その他を選択して別名をつけてください。<br />
				</p>
			</div></div>
	</dd>
	<div id="type_is_SPACE_div">
			<div align="right"><br /><div  align="left" class="papertag green">
				<p>余白として登録します。<br />
				</p>
			</div></div>
	</div><!-- type_is_SPACE_div -->
	<div id="type_not_SPACE_div">
	<dt class="sub_head">アイコン</dt>
	<dd>
		<div class="filesample">
			<div class="fileicon">
				<img src="/user/elecicon/onoff.png" width="96" height="96" />
			</div>
			<div class="filename">
				<span id="elecicon"></span>
			</div>
			<div class="changebutton">
				<a href="javascript:void(0)" id="elecicon_select_button" data-role="button" data-icon="arrow-u"  class="useicon"><span>アイコンを変える</span></a>
			</div>
		</div>
		<br />
		<div class="checkbox_div">
		<label><input type="checkbox" id="showremocon">リモコン画面に表示する</label>
		</div >
			<div align="right"><br /><div  align="left" class="papertag green">
				<p>リモコン画面に表示するアイコンです。<br />
				好きなファイルをアップロードすることもできます。<br />
				</p>
			</div></div>
	</dd>
	<dt class="sub_head">動作オプション</dt>
	<dd>
		<label><input type="checkbox" id="ignore_recogn">音声認識対象から消す</label>
		<label><input type="checkbox" id="click_to_menu">クリックで動作ではなくメニューダイアログを開く</label>
			<div align="right"><br /><div  align="left" class="papertag yellow">
				<p>音声認識対象から消す<br/>
				チェックすると、機材に関連付けられている音声認識コマンドをすべて無効にします。</p>
				<p>クリックで動作ではなくメニューダイアログを開く<br/>
				チェックすると、リモコン画面で、機材の大きなアイコンをクリックしたら、動作を繰り替えるのではなく、メニューダイアログを開きます。エアコンやテレビなどの複数の動作をたくさん持つ家電に設定すると便利です。</p>
			</div></div>
	</dd>
	<dt class="sub_head">動作</dt>
	<dd>
		<ul id="elec_actionmenu_li_sample" class="js_template">
			<li class="%APPENDCLASS%">
					<a href="javascript:void(0)"  onclick="elec_delete($('#elec_key').val(),'%ACTIONKEY%');" class="delicon"><img src="/jscss/images/delicon.png" /></a>

					<span class="id hidden">%ACTIONKEY%</span>
					<span class="type" style="color:%ACTIONCOLOR%; ">%ACTIONTYPE%</span>

					<a href="javascript:void(0)" onclick="elec_goto_action($('#elec_key').val(),'%ACTIONKEY%');" class="editicon"><img src="/jscss/images/editicon.png" /></a>
			</li>
		</ul>
		<ul id="elec_actionmenu_ul">
		</ul>
		<br class="clearfix" />
		<p><a data-role="button" data-icon="arrow-r" data-iconpos="right" onclick="elec_goto_action($('#elec_key').val(),'new');" >動作を新規に追加する</a></p>
			<div align="right"><div  align="left" class="papertag purple">
				<p>「つける」「けす」などの動作させる項目です。<br />
				項目をドラッグすると並べ替えもできます。<br />
				</p>
			</div></div>
	</dd>
	<br />
	<br />
	</div><!-- type_not_SPACE_div -->
	<p><a data-role="button" data-icon="arrow-l" data-iconpos="left" onclick="elec_save($('#elec_key').val());" >保存して戻る</a></p>
	<div class="error bunner_error">エラーが発生しました!!</div>
	</div><!-- /content -->
</div><!-- /page elec -->

<div data-role="page" id="action" data-theme="c" data-url="action" tabindex="0">
	<div data-role="header" class="header_pc"> 
		<h1><a data-role="none" href="#setting" ><img src="/jscss/images/fhc700.png" /></h1> 
	</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a data-role="button" class="menubutton" data-inline="true" href="javascript:void(0)" onclick="elec_load($('#elec_key').val());" data-ajax="false" data-icon="back"><span>機材設定</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/page_icon64_action.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">動作設定</span></div>
		</fieldset>
	</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">	
	<div class="error bunner_error">エラーが発生しました!!</div>

	<input type="hidden" id="action_key1" value="">
	<input type="hidden" id="action_key2" value="">
	<dl>
	<dt class="sub_head">種類</dt>
	<dd>
		<select id="actiontype" size="1">
		<option color="ff4500" recong="@つけて" speak="@をつけます" value="つける">つける</option>
		<option color="008000" recong="@けして" speak="@をけします" value="けす">けす</option>
		<optgroup label="大きさ">
			<option color="006400" recong="@さいしょう" speak="@をさいしょうにします" value="小">小</option>
			<option color="00ff7f" recong="@ふつう" speak="@をふつうにします" value="中">中</option>
			<option color="7cfc00" recong="@さいだい" speak="@をさいだいにします" value="大">大</option>
		</optgroup>
		<optgroup label="エアコン">
			<option color="008000" recong="エアコンけして|だんぼう、けして" speak="エアコンをけします" value="けす">けす</option>
			<option color="b0c4de" recong="エアコンさいしょう" speak="エアコンをさいしょうにします" value="冷房最小">冷房最小</option>
			<option color="4169e1" recong="エアコンつけて" speak="エアコンをつけます" value="冷房普通">冷房普通</option>
			<option color="0000cd" recong="エアコンさいだい" speak="エアコンをさいだいにします" value="冷房最大">冷房最大</option>
			<option color="ff7f50" recong="だんぼうさいしょう" speak="だんぼうをさいしょうにします" value="暖房最小">暖房最小</option>
			<option color="ff4500" recong="だんぼうつけて" speak="だんぼうをつけます" value="暖房普通">暖房普通</option>
			<option color="ff0000" recong="だんぼうさいだい" speak="だんぼうをさいだいにします" value="暖房最大">暖房最大</option>
		</optgroup>
		<optgroup label="テレビ">
			<option color="ff4500" recong="テレビいちらんひょうだして" speak="@のいちらんひょうだします" value="一覧表">一覧表</option>
			<option color="008000" recong="えぬえいちけーにして" speak="@をえぬえいちけーにします" value="NHK">NHK</option>
			<option color="008000" recong="きょういくにして" speak="@をきょういくにします" value="教育">教育</option>
			<option color="b0c4de" recong="フジテレビにして" speak="@をフジテレビにします" value="フジテレビ">フジテレビ</option>
			<option color="4169e1" recong="テレビあさひにして" speak="@をテレビあさひにします" value="テレビ朝日">テレビ朝日</option>
			<option color="0000cd" recong="にほんテレビにして" speak="@をにほんテレビにします" value="日本テレビ">日本テレビ</option>
			<option color="ff7f50" recong="テレビとうきょうにして" speak="@をテレビとうきょうにします" value="テレビ東京">テレビ東京</option>
			<option color="ff7f00" recong="てぃーびーえすにして" speak="@をてぃーびーえすにします" value="TBS">TBS</option>
			<option color="4169e1" recong="がいぶしゅつりょくにして" speak="@をがいぶしゅつりょくにします" value="外部接続1">外部接続</option>
			<option color="4169e1" recong="おんりょうあげて" speak="@のおんりょうをあげます" value="音量上げ">音量上げ</option>
			<option color="ff69e1" recong="おんりょうさげて" speak="@のおんりょうをさげます" value="音量下げ">音量下げ</option>
			<option color="4169e1" recong="チャンネルまえへ" speak="@のチャンネルをまえにもどします" value="チャンネル前へ">チャンネル前へ</option>
			<option color="ff69e1" recong="チャンネルつぎへ" speak="@のおんりょうをつぎにすすめます" value="チャンネル次へ">チャンネル次へ</option>
		</optgroup>
		<optgroup label="タイマー機能">
			<option color="ff4500" recong="@のタイマーセット" speak="@のタイマーセットをします" value="タイマーセット">タイマーセット</option>
			<option color="4169e1" recong="@のタイマーとりけし" speak="@のタイマーをとりけします" value="タイマー取り消し">タイマー取り消し</option>
		</optgroup>
		<optgroup label="音声認識一時停止">
			<option color="ff4500" recong="おんせいにんしきていし" speak="おんせいにんしきをていしします" value="音声認識停止">音声認識停止</option>
			<option color="4169e1" recong="おんせいにんしきさいかい" speak="おんせいにんしきをさいかいします" value="音声認識再開">音声認識再開</option>
		</optgroup>
<!--
		<optgroup label="電話">
			<option color="ff4500" recong="なんとかさんにでんわして" speak="なんとかさんにでんわして" value="電話を掛ける">電話をかける</option>
			<option color="4169e1" recong="でんわをとって" speak="でんわをとります" value="電話をとる">電話をとる</option>
			<option color="ff7f00" recong="でんわをきって" speak="でんわをきります" value="電話をきる">電話をきる</option>
			<option color="0000cd" recong="でんわパッドをひょうじして" speak="でんわパッドをひょうじします" value="電話パッドを表示">電話パッドを表示</option>
		</optgroup>
-->
		<optgroup label="余白">
			<option value="SPACE">SPACE</option>
		</optgroup>
		<option color="ff0000" recong="" speak="" value="other">その他</option>
		</select>
		<div id="actiontype_other_div">
		<p>その他種類名:</p><input type="text" id="actiontype_other" value="">
		<div class="error error_20021">その他の動作名を入れてください。</div>
		</div>
		<div class="error error_20022">この動作名は既に使われています。別の名前にしてください。</div>
			<br />
			<div align="right"><div  align="left" class="papertag red">
				<p>どんな動作なのかを教えます<br />
				</p>
			</div></div>
	</dd>
	<div id="action_is_SPACE_div">
		<dl>
		<dt class="sub_head">余白</dt>
		<dd>
			<div id="action_is_SPACE_div">
				<div align="right"><br /><div  align="left" class="papertag green">
					<p>余白として登録します。<br />
					</p>
				</div></div>
			</div><!--  action_is_SPACE_div -->
		</dd>
		</dt>
		</dl>
	</div><!-- action_is_SPACE_div -->
	<div id="action_not_SPACE_div">
		<dl>
		<dt class="sub_head">音声認識</dt>
		<dd>
			<p>音声認識させたい項目を入力してください。(読み間違い防止の為、すべてひらがなかカタカナで入れてください)</p>
			<div class="notify" id="actionvoice_ignore_recogn">現在、機材設定で音声認識を無効にする設定がされています。</div>
			<input type="text" id="actionvoice_command_1" value=""  placeholder="例:でんきつけて">
			<div class="error error_20031">認識できない文字列があります。</div>
			<div class="error error_20061">既に別のコマンドで利用されています。</div>
			<input type="text" id="actionvoice_command_2" value=""  placeholder="例:おんせいにんしき、こうほそのにー">
			<div class="error error_20032">認識できない文字列があります。</div>
			<div class="error error_20062">既に別のコマンドで利用されています。</div>
			<input type="text" id="actionvoice_command_3" value=""  placeholder="例:(せいきひょうげん|すっごいけんさく)もつかえます">
			<div class="error error_20033">認識できない文字列があります。</div>
			<div class="error error_20063">既に別のコマンドで利用されています。</div>
			<input type="text" id="actionvoice_command_4" value=""  placeholder="例:でんきけして|でんきけしてください|でんきけしてござらぬか">
			<div class="error error_20034">認識できない文字列があります。</div>
			<div class="error error_20064">既に別のコマンドで利用されています。</div>
			<input type="text" id="actionvoice_command_5" value=""  placeholder="例:バルス">
			<div class="error error_20035">認識できない文字列があります。</div>
			<div class="error error_20065">既に別のコマンドで利用されています。</div>
				<br />
				<div align="right"><div  align="left" class="papertag blue">
					<p>この言葉を喋ったら動作させる、音声認識ワードを入れます。<br />
					平仮名かカタカナで入れてください。<br />
					</p>
				</div></div>
		</dd>
		<dt class="sub_head">実行時しゃべる</dt>
		<dd>
			<select id="tospeak_select" size="1">
			<option value="string">文字列</option>
			<option value="musicfile">音楽ファイル</option>
			<option value="none">読み上げない</option>
			</select>
			<br />
			<div id="tospeak_string_div">
				<dl>
					<dt class="sub_head">文字列を読み上げる<dt>
					<dd>
						<div class="filesample">
							<div class="fileicon">
								<img src="/jscss/images/icon_speak.png" width="96" height="96" />
							</div>
							<div class="changebutton">
								<input type="text" id="tospeak_string" value=""   size="40" placeholder="例:それじゃあ、でんきつけるよ">
							</div>
							<div class="preview">
								<a href="javascript:void(0)" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a>
							</div>
						</div>
					</dd>
				</dl>
				<div class="error error_20041">読み上げできない文字列があります。</div>
					<div align="right"><div  align="left" class="papertag yellow">
						<p>応答時に文字列を合成音で読み上げます。<br />
						誤読をさけるために平仮名かカタカナで入れることをおすすめします。<br />
						</p>
					</div></div>
			</div>
			<div id="tospeak_mp3_div">
				<dl>
					<dt class="sub_head">音楽ファイルを読み上げる<dt>
					<dd>
						<div class="filesample">
							<div class="fileicon">
								<img src="/jscss/images/icon_audio.png" width="96" height="96" />
							</div>
							<div class="filename">
								<span id="tospeak_mp3"></span><br />
							</div>
							<div class="changebutton">
								<a href="javascript:void(0)" data-role="button" data-icon="arrow-u"  class="useicon"><span>ファイルを選択する</span></a>
							</div>
							<div class="preview">
								<a href="javascript:void(0)" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a>
							</div>
						</div>
					</dd>
				</dl>
				<div class="error error_20042">読み上げるファイル名を入れてください。</div>
					<div align="right"><div  align="left" class="papertag green">
						<p>応答時にmp3ファイルを再生します。<br />
						</p>
					</div></div>
			</div>
			<div id="tospeak_none_div">
					<div align="right"><div  align="left" class="papertag red">
						<p>読み上げを行いません。<br />
						</p>
					</div></div>
			</div>
		</dd>
		</dl>

		<dl>
		<dt class="sub_head">操作方法</dt>
		<dd>
			<select id="execflag" size="1">
			<option value="ir">赤外線学習</option>
			<option value="command">コマンド実行</option>
			<option value="macro">マクロ実行</option>
			<option value="timer">タイマー</option>
			<option value="multiroom">複数台連携</option>
			<option value="netdevice">ネット家電連携</option>
<!--			<option value="sip">SIP電話連携</option>		-->
			<option value="recongpause">音声認識の一時停止</option>
			<option value="none">何もしない</option>
			</select>
			<br />
			<div id="exec_ir_div" class="action_exec_page">
			<dl>
				<dt class="sub_head">赤外線学習<dt>
				<dd>
					リモコンの信号を記憶して再送することができます。<br />
					<div class="filesample">
						<div class="fileicon">
							<img src="/jscss/images/icon_ir.png" id="exec_ir_fire_icon" width="96" height="96" />
						</div>
						<div class="filename">
							<span id="exec_ir_registlabel">現在登録されています。</span>
							<input type="hidden" id="exec_ir" value=""  />
						</div>
						<div class="changebutton">
							<a href="javascript:void(0)" id="exec_ir_leaning" data-role="button" data-icon="arrow-u"  class="useicon">赤外線を学習させる</a>
						</div>
						<div class="preview">
							<span id="exec_ir_registlabel">
								<a href="javascript:void(0)" id="exec_ir_preview_button" data-role="button" data-icon="check"  class="useicon">preview</a>
							</span>
						</div>
					</div>

					<div class="error" id="exec_ir_error">リモコン学習がされていません。<br />
					学習は、「赤外線を学習させる」ボタンを押した後で、フューチャーホームコントローラーの上面にある三角マークの先に向かって、リモコンの信号を送信してください<br />
					</div>
					<div class="error" id="exec_ir_error2">学習に失敗しました<br />
					リモコンコードの学習に失敗しました。<br />
					お手数ですが、もう一度、「赤外線を学習させる」ボタンを押して学習し直させてください。<br />
					<br />
					リモコンによっては、信号が弱いものがあり、学習しにくいものがあります。<br />
					赤外線も光なので、距離が開くと信号の強度が落ちてしまいます。<br />
					フューチャーホームコントローラーの上面にある三角マークの先にリモコンを立てるように近づけてボタンを押してください。<br />
					リモコンと本体の距離は数cmぐらいが目安です。<br />
					<img src="/jscss/images/fhc_ir_learning2.jpg" width="438" height="403"><br />
					</div>
					<div id="exec_ir_ok" class="hidden">学習に成功しました<br />
					リモコン信号の学習が完了しました。<br />
					<br />
					正しく学習されているか確認するために、「perview」ボタンをクリックしてください。<br />
					「perview」ボタンをクリックすると、記憶した信号を、フューチャーホームコントローラーが再送します。<br />
					覚えさせた家電が動作をするかをご確認ください。<br />
					正しく動作しない場合は、お手数ですが、もう一度、「赤外線を学習させる」ボタンを押して学習し直させてください。<br />
					</div>
					<br />
					<br />
					<img src="/jscss/images/fhc_ir_learning1.jpg" width="640" height="457"><br />
					<br />
						<div align="right"><div  align="left" class="papertag red">
							<p>動作時に赤外線を飛ばします。<br />
							<br />
							手元の赤外線リモコンを登録できます。<br />
							「赤外線を学習させる」ボタンをクリックし、ホームコントローラーにリモコンのコマンドを送信してください。<br />
							<br />
							リモコンと本体の距離は数cmぐらいが目安です。三角マークの先に垂直に立てるようにして近づけてください。<br />
							<a href="https://rti-giken.jp/fhc/help/howto/remocon_leaning.html" target="_blank">詳しくは、「リモコンを学習させるコツ」を御覧ください。</a><br />
							</p>
						</div></div>
				</dd>
			</dl>
			</div>

			<div id="exec_macro_div" class="action_exec_page">
			<dl>
				<dt class="sub_head">マクロ<dt>
				<dd>
					<div class="filesample">
						<div class="fileicon">
							<img src="/jscss/images/icon_macro_96.png" width="96" height="96" />
						</div>
						<div class="text">
							複数の動作を連動させることができます。<br />
							ボタンを複数回押すといった動作は、マクロを使って設定します。<br />
						</div>
					</div>
					<div data-role="controlgroup" data-type="horizontal" class="macrotable">
						<span class="label">マクロ1</span>
						<div class="selectbox">
							<select id="exec_macro_1_elec" size="1">
								<option value="none">利用しない</option>
							</select>
							<select id="exec_macro_1_action" size="1">
								<option value="none">利用しない</option>
							</select>
							<select id="exec_macro_1_after" size="1">
								<option value="next">次のコマンドへ</option>
								<option value="wait_1sec">1秒待ってから次へ</option>
								<option value="wait_1sec">2秒待ってから次へ</option>
								<option value="ifnext">状態が同じなら何もせず次へ</option>
							</select>
						</div>
					</div>
						<div class="error error_20051">呼び出し回数の上限を超えました。<br />
						マクロ1の中で無限ループに入っている可能性があります。<br />
						</div>

					<div data-role="controlgroup" data-type="horizontal" class="macrotable">
						<span class="label">マクロ2</span>
						<div class="selectbox">
							<select id="exec_macro_2_elec" size="1">
								<option value="none">利用しない</option>
							</select>
							<select id="exec_macro_2_action" size="1">
								<option value="none">利用しない</option>
							</select>
							<select id="exec_macro_2_after" size="1">
								<option value="next">次のコマンドへ</option>
								<option value="wait_1sec">1秒待ってから次へ</option>
								<option value="wait_2sec">2秒待ってから次へ</option>
								<option value="ifnext">状態が同じなら何もせず次へ</option>
							</select>
						</div>
					</div>
						<div class="error error_20052">呼び出し回数の上限を超えました。<br />
						マクロ2の中で無限ループに入っている可能性があります。<br />
						</div>

					<div data-role="controlgroup" data-type="horizontal" class="macrotable">
						<span class="label">マクロ3</span>
						<div class="selectbox">
							<select id="exec_macro_3_elec" size="1">
								<option value="none">利用しない</option>
							</select>
							<select id="exec_macro_3_action" size="1">
								<option value="none">利用しない</option>
							</select>
							<select id="exec_macro_3_after" size="1">
								<option value="next">次のコマンドへ</option>
								<option value="wait_1sec">1秒待ってから次へ</option>
								<option value="wait_2sec">2秒待ってから次へ</option>
								<option value="ifnext">状態が同じなら何もせず次へ</option>
							</select>
						</div>
					</div>
						<div class="error error_20053">呼び出し回数の上限を超えました。<br />
						マクロ3の中で無限ループに入っている可能性があります。<br />
						</div>

					<div data-role="controlgroup" data-type="horizontal" class="macrotable">
						<span class="label">マクロ4</span>
						<div class="selectbox">
							<select id="exec_macro_4_elec" size="1">
								<option value="none">利用しない</option>
							</select>
							<select id="exec_macro_4_action" size="1">
								<option value="none">利用しない</option>
							</select>
							<select id="exec_macro_4_after" size="1">
								<option value="next">次のコマンドへ</option>
								<option value="wait_1sec">1秒待ってから次へ</option>
								<option value="wait_2sec">2秒待ってから次へ</option>
								<option value="ifnext">状態が同じなら何もせず次へ</option>
							</select>
						</div>
					</div>
						<div class="error error_20054">呼び出し回数の上限を超えました。<br />
						マクロ4の中で無限ループに入っている可能性があります。<br />
						</div>

					<div data-role="controlgroup" data-type="horizontal" class="macrotable">
						<span class="label">マクロ5</span>
						<div class="selectbox">
							<select id="exec_macro_5_elec" size="1">
								<option value="none">利用しない</option>
							</select>
							<select id="exec_macro_5_action" size="1">
								<option value="none">利用しない</option>
							</select>
							<select id="exec_macro_5_after" size="1">
								<option value="next">次のコマンドへ</option>
								<option value="wait_1sec">1秒待ってから次へ</option>
								<option value="wait_2sec">2秒待ってから次へ</option>
								<option value="ifnext">状態が同じなら何もせず次へ</option>
							</select>
						</div>
					</div>
						<div class="error error_20055">呼び出し回数の上限を超えました。<br />
						マクロ5の中で無限ループに入っている可能性があります。<br />
						</div>

					<div data-role="controlgroup" data-type="horizontal" class="macrotable">
						<span class="label">マクロ6</span>
						<div class="selectbox">
							<select id="exec_macro_6_elec" size="1">
								<option value="none">利用しない</option>
							</select>
							<select id="exec_macro_6_action" size="1">
								<option value="none">利用しない</option>
							</select>
							<select id="exec_macro_6_after" size="1">
								<option value="next">次のコマンドへ</option>
								<option value="wait_1sec">1秒待ってから次へ</option>
								<option value="wait_2sec">2秒待ってから次へ</option>
								<option value="ifnext">状態が同じなら何もせず次へ</option>
							</select>
						</div>
					</div>
						<div class="error error_20056">呼び出し回数の上限を超えました。<br />
						マクロ6の中で無限ループに入っている可能性があります。<br />
						</div>

					<div data-role="controlgroup" data-type="horizontal" class="macrotable">
						<span class="label">マクロ7</span>
						<div class="selectbox">
							<select id="exec_macro_7_elec" size="1">
								<option value="none">利用しない</option>
							</select>
							<select id="exec_macro_7_action" size="1">
								<option value="none">利用しない</option>
							</select>
							<select id="exec_macro_7_after" size="1">
								<option value="next">次のコマンドへ</option>
								<option value="wait_1sec">1秒待ってから次へ</option>
								<option value="wait_2sec">2秒待ってから次へ</option>
								<option value="ifnext">状態が同じなら何もせず次へ</option>
							</select>
						</div>
					</div>
						<div class="error error_20057">呼び出し回数の上限を超えました。<br />
						マクロ7の中で無限ループに入っている可能性があります。<br />
						</div>

					<div data-role="controlgroup" data-type="horizontal" class="macrotable">
						<span class="label">マクロ8</span>
						<div class="selectbox">
							<select id="exec_macro_8_elec" size="1">
								<option value="none">利用しない</option>
							</select>
							<select id="exec_macro_8_action" size="1">
								<option value="none">利用しない</option>
							</select>
							<select id="exec_macro_8_after" size="1">
								<option value="next">次のコマンドへ</option>
								<option value="wait_1sec">1秒待ってから次へ</option>
								<option value="wait_2sec">2秒待ってから次へ</option>
								<option value="ifnext">状態が同じなら何もせず次へ</option>
							</select>
						</div>
					</div>
						<div class="error error_20058">呼び出し回数の上限を超えました。<br />
						マクロ8の中で無限ループに入っている可能性があります。<br />
						</div>

					<div data-role="controlgroup" data-type="horizontal" class="macrotable">
						<span class="label">マクロ9</span>
						<div class="selectbox">
							<select id="exec_macro_9_elec" size="1">
								<option value="none">利用しない</option>
							</select>
							<select id="exec_macro_9_action" size="1">
								<option value="none">利用しない</option>
							</select>
							<select id="exec_macro_9_after" size="1">
								<option value="next">次のコマンドへ</option>
								<option value="wait_1sec">1秒待ってから次へ</option>
								<option value="wait_2sec">2秒待ってから次へ</option>
								<option value="ifnext">状態が同じなら何もせず次へ</option>
							</select>
						</div>
					</div>
						<div class="error error_20059">呼び出し回数の上限を超えました。<br />
						マクロ9の中で無限ループに入っている可能性があります。<br />
						</div>

				</dd>
			</dl>
			<div class="preview_button_div"><div><a href="javascript:void(0)" id="exec_macro_preview_button" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a></div></div>
			<br />
						<div align="right"><div  align="left" class="papertag yellow">
							<p>複数の動作を連動させることができます。<br />
							<br />
							複数の家電を連動させるといった使い方や、リモコンキーを2回押さないといけない動作などを設定できます。<br >
							<br />
							(無限再帰をさけるために最大30ステップまで実行できます)<br />
							</p>
						</div></div>
			</div>

			<div id="exec_timer_div" class="action_exec_page">
			<dl>
				<dt class="sub_head">タイマー<dt>
				<dd>
					時間差で動作させます。<br />
					<br />
					<div class="filesample">
						<div class="fileicon">
							<img src="/jscss/images/icon_timer_96.png" width="96" height="96" />
						</div>
						<div class="filename">
						</div>
						<div class="changebutton">
							<select id="timer_type" size="1">
								<option value="1">1分後</option>
								<option value="2">2分後</option>
								<option value="3">3分後</option>
								<option value="4">4分後</option>
								<option value="5">5分後</option>
								<option value="6">6分後</option>
								<option value="7">7分後</option>
								<option value="8">8分後</option>
								<option value="9">9分後</option>
								<option value="10">10分後</option>
								<option value="15">15分後</option>
								<option value="30">30分後</option>
								<option value="45">45分後</option>
								<option value="60">1時間後</option>
								<option value="90">1時間半後</option>
								<option value="120">2時間後</option>
								<option value="180">3時間後</option>
								<option value="240">4時間後</option>
								<option value="300">5時間後</option>
								<option value="360">6時間後</option>
								<option value="420">7時間後</option>
								<option value="480">8時間後</option>
								<option value="540">9時間後</option>
								<option value="600">10時間後</option>
								<option value="660">11時間後</option>
								<option value="720">12時間後</option>
								<option value="780">13時間後</option>
								<option value="840">14時間後</option>
								<option value="900">15時間後</option>
								<option value="960">16時間後</option>
								<option value="1020">17時間後</option>
								<option value="1080">18時間後</option>
								<option value="1140">19時間後</option>
								<option value="1200">20時間後</option>
								<option value="1260">21時間後</option>
								<option value="1320">22時間後</option>
								<option value="1380">23時間後</option>
								<option value="1440">24時間後</option>
								<option value="reset">タイマー取り消し</option>
							</select>
						</div>
					</div>
					<br />
				</dd>
				<div id="timer_setting_div">
					<dt class="sub_head">タイマー動作<dt>
					<dd>
						<div class="table100">
							<div class="tr">
								<div class="th">家電名:</div>
								<div class="td">
									<select id="timer_exec_elec" size="1">
									</select>
								</div>
							</div>
							<div class="tr">
								<div class="th">アクション名:</div>
								<div class="td">
									<select id="timer_exec_action" size="1">
									</select>
								</div>
							</div>
						</div>
						<div class="checkbox_div">
							<label><input type="checkbox" id="timer_no_status_overraide">既にその状態にあれば追加で命令を送らない</label>
						</div>
						<div class="preview_button_div"><div><a href="javascript:void(0)" id="exec_timer_preview_button" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a></div></div>
						<br />
						<div align="right"><div  align="left" class="papertag green">
							<p>
							今から数時間後に動作させます。<br />
							<br />
							「既にその状態にあれば追加で命令を送らない」をチェックにすると、<br />
							タイマー動作時に電気が変えている時は、電気を消すタイマーが動いたとしても、追加で消す命令を送らなくなります。<br />
							家電によっては、ONの信号とOFFの信号が同一のものがあるため、誤動作を避けるために用意されています。<br />
							「状態を遷移させない」とセットでご利用ください。<br />
							</p>
						</div></div>
					</dd>
					<dt class="sub_head">タイマー動作時に読み上げ<dt>
					<dd>
						<select id="timer_tospeak_select" size="1">
						<option value="string">文字列</option>
						<option value="musicfile">音楽ファイル</option>
						<option value="none">読み上げない</option>
						</select>
						<br />
						<div id="timer_tospeak_string_div">
							<dl>
								<dt class="sub_head">文字列を読み上げる<dt>
								<dd>
									<div class="filesample">
										<div class="fileicon">
											<img src="/jscss/images/icon_speak.png" width="96" height="96" />
										</div>
										<div class="changebutton">
											<input type="text" id="timer_tospeak_string" value=""   size="40" placeholder="例:じかんだ、でんきをけそう">
										</div>
										<div class="preview">
											<a href="javascript:void(0)" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a>
										</div>
									</div>
								</dd>
							</dl>
							<div class="error error_20141">読み上げできない文字列があります。</div>
								<div align="right"><div  align="left" class="papertag yellow">
									<p>タイマー実行時に文字列を合成音で読み上げます。<br />
									誤読をさけるために平仮名かカタカナで入れることをおすすめします。<br />
									</p>
								</div></div>
						</div>
						<div id="timer_tospeak_mp3_div">
							<dl>
								<dt class="sub_head">音楽ファイルを読み上げる<dt>
								<dd>
									<div class="filesample">
										<div class="fileicon">
											<img src="/jscss/images/icon_audio.png" width="96" height="96" />
										</div>
										<div class="filename">
											<span id="timer_tospeak_mp3"></span><br />
										</div>
										<div class="changebutton">
											<a href="javascript:void(0)" data-role="button" data-icon="arrow-u"  class="useicon"><span>ファイルを選択する</span></a>
										</div>
										<div class="preview">
											<a href="javascript:void(0)" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a>
										</div>
									</div>
								</dd>
							</dl>
							<div class="error error_20142">読み上げるファイル名を入れてください。</div>
								<div align="right"><div align="left" class="papertag green">
									<p>タイマー実行時にmp3ファイルを再生します。<br />
									</p>
								</div></div>
						</div>
						<div id="timer_tospeak_none_div">
								<div align="right"><div  align="left" class="papertag red">
									<p>読み上げを行いません。<br />
									</p>
								</div></div>
						</div>
				</dd>
				</div>
				<div id="timer_reset_setting_div">
					<br />
					タイマーを停止します。<br />
					<br />
					<div align="right"><div  align="left" class="papertag green">
						<p>
						設定されているタイマーを停止させます。<br />
						<br />
						</p>
					</div></div>
				</div>
			</div>

			<div id="exec_recongpause_div" class="action_exec_page">
			<dl>
				<dt class="sub_head">音声認識の一時停止<dt>
				<dd>
					音声認識を一時停止/再開を行います。<br />
					<br />
					<div class="filesample">
						<div class="fileicon">
							<img src="/jscss/images/recong_pause_mp3_icon96.png" width="96" height="96" />
						</div>
						<div class="filename">
						</div>
						<div class="changebutton">
							<select id="recongpause_type" size="1">
								<option value="stop">音声認識を一時停止にする</option>
								<option value="start">音声認識を再開する</option>
							</select>
						</div>
					</div>
					<br />
				</dd>
				<br />
				<div align="right"><div  align="left" class="papertag green">
					<p>音声認識を一時停止にします。</p>
					<p>音声認識を再開もしないといけないので、一時停止を作るときは、必ず再開するも同時に作ってください。そうしないと、音声認識を永久に再開できません。</p>
				</div></div>
			</div>
			<div id="exec_multiroom_div" class="action_exec_page">
			<dl>
				<dt class="sub_head">複数台連携<dt>
				<dd>
					<div class="filesample">
						<div class="fileicon">
							<img src="/jscss/images/setting_icon96_multiroomlist.png" width="96" height="96" />
						</div>
						<div class="filename">
							別の部屋に設置しているFHCへ命令を送ります。<br />
						</div>
						<div class="changebutton">
						</div>
					</div>
					<div class="table100">
						<div class="tr">
							<div class="th">部屋名:</div>
							<div class="td">
								<select id="multiroom_room" size="1">
								</select>
							</div>
						</div>
						<div class="tr">
							<div class="th">家電名:</div>
							<div class="td">
								<select id="multiroom_elec" size="1">
								</select>
							</div>
						</div>
						<div class="tr">
							<div class="th">アクション名:</div>
							<div class="td">
								<select id="multiroom_action" size="1">
								</select>
							</div>
						</div>
					</div>
					<div class="preview_button_div"><div><a href="javascript:void(0)" id="exec_multiroom_preview_button" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a></div></div>
					<br />
					<div align="right"><div  align="left" class="papertag green">
						<p>
						別の部屋に設置しているFHC端末へ命令を送ります。<br />
						事前に複数台設定で認証している必要があります。<br />
						<a href="https://rti-giken.jp/fhc/help/howto/multiroom.html" target="_blank">詳しくは、こちらの「複数台設定をする」の項目をご覧ください。</a><br />
						</p>
					</div></div>
				</dd>
			</div>
			<div id="exec_netdevice_div" class="action_exec_page">
			<dl>
				<dt class="sub_head">ネットワーク家電対応<dt>
				<dd>
					<div class="filesample">
						<div class="fileicon">
							<img src="/jscss/images/setting_icon96_netdevice.png" width="96" height="96" />
						</div>
						<div class="filename">
							ネットワーク家電へ命令を送ります。<br />
						</div>
						<div class="changebutton">
						</div>
					</div>
					<div class="table100">
						<div class="tr">
							<div class="th">ネットワーク家電名:</div>
							<div class="td">
								<select id="netdevice_elec" size="1">
								</select>
							</div>
						</div>
						<div class="tr">
							<div class="th">アクション名:</div>
							<div class="td">
								<select id="netdevice_action" size="1">
								</select>
							</div>
						</div>
						<div class="tr">
							<div class="th">送信する値:</div>
							<div class="td">
								<select id="netdevice_value" size="1">
								</select>
							</div>
						</div>
					</div>
					<div class="preview_button_div"><div><a href="javascript:void(0)" id="exec_netdevice_preview_button" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a></div></div>
					<br />
					<div class="preview_button_div"><div><a href="javascript:void(0)" id="exec_netdevice_rescan_button" data-role="button" data-icon="check"  class="useicon"><span>ネット家電の再スキャン</span></a></div></div>
					<br />
					<div align="right"><div align="left" class="papertag green">
						<p>
						ネットワーク家電へ命令を送ります。<br />
						<a href="https://rti-giken.jp/fhc/help/howto/netdevice.html" target="_blank">対応しているネットワーク家電はこちらをご覧ください。</a><br />
						</p>
					</div></div>
				</dd>
			</div>

			<div id="exec_sip_div" class="action_exec_page">
			<dl>
				<dt class="sub_head">SIP電話連携<dt>
				<dd>
					<div class="filesample">
						<div class="fileicon">
							<img src="/jscss/images/setting_icon96_sip.png" width="96" height="96" />
						</div>
						<div class="filename">
						</div>
						<div class="changebutton">
							<select id="sip_action_type" size="1">
								<option value="call">電話をかける</option>
								<option value="numpad">電話キーパッドを画面に表示</option>
								<option value="answer">かかってきた電話を受ける</option>
								<option value="hangup">電話を切る</option>
							</select>
						</div>
					</div>
					<div class="sip_action_setting_div sip_action_call_setting_div">
						<dl>
							<dt class="sub_head">発信する電話番号<dt>
							<dd>
								<input type="text" id="sip_call_number" value=""   size="40" placeholder="例:123-4567-8761 or user@example.com">
							</dd>
						</dl>
					</div>
					<br/>
					<div align="right"><div  align="left" class="papertag green">
						<p>
						IP電話との連携を設定します。<br />
						<a href="https://rti-giken.jp/fhc/help/howto/sip.html" target="_blank">対応しているネットワーク家電はこちらをご覧ください。</a><br />
						</p>
					</div></div>
				</dd>
			</div>

			<div id="exec_command_div" class="action_exec_page">
			<dl>
				<dt class="sub_head">コマンド<dt>
				<dd>
					<div class="filesample">
						<div class="fileicon">
							<img src="/jscss/images/icon_script_command_nocommand.png" id="exec_command_select_icon" width="96" height="96" />
						</div>
						<div class="filename">
							<input type="hidden" id="exec_command" value=""  />
							<span id="exec_command_display"></span>
						</div>
						<div class="changebutton">
							<a href="javascript:void(0)" id="exec_command_select_button" data-role="button" data-icon="arrow-u"  class="useicon"><span>コマンドを選択する</span></a>
						</div>
					</div>
					<div class="error" id="exec_command_error">実行するコマンドを入れてください。</div>
				</dd>
				<dt><p><span id="exec_command_args1_mustlabel"></span><span id="exec_command_args1_label" class="command_args_text1">未定義の引数1</span></p></dt>
				<dd class="ddshift">
					<input type="text" id="exec_command_args1" value="" class="command_args_input1">
				</dd>
				<dt><p><span id="exec_command_args2_mustlabel"></span><span id="exec_command_args2_label" class="command_args_text2">未定義の引数2</span></p></dt>
				<dd class="ddshift">
					<input type="text" id="exec_command_args2" value="" class="command_args_input2">
				</dd>
				<dt><p><span id="exec_command_args3_mustlabel"></span><span id="exec_command_args3_label" class="command_args_text1">未定義の引数3</span></p></dt>
				<dd class="ddshift">
					<input type="text" id="exec_command_args3" value="" class="command_args_input1">
				</dd>
				<dt><p><span id="exec_command_args4_mustlabel"></span><span id="exec_command_args4_label" class="command_args_text2">未定義の引数4</span></p></dt>
				<dd class="ddshift">
					<input type="text" id="exec_command_args4" value="" class="command_args_input2">
				</dd>
				<dt><p><span id="exec_command_args5_mustlabel"></span><span id="exec_command_args5_label" class="command_args_text1">未定義の引数5</span></p></dt>
				<dd class="ddshift">
					<input type="text" id="exec_command_args5" value="" class="command_args_input1">
				</dd>
			</dl>
			<div class="preview_button_div"><div><a href="javascript:void(0)" id="exec_command_preview_button" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a></div></div>
			<br />
			<div class="error" id="exec_command_preview_error"></div>
						<div align="right"><div  align="left" class="papertag green">
							<p>特別なコマンドを実行することで、外部の機材やパソコンと連携させたりできます。
							</p>
						</div></div>
			</div>
		</dd>
		</dl>

		<dl>
		<dt class="sub_head">メニューへの表示</dt>
		<dd>
			<div class="colorpicker">
				<input type="hidden" id="actioncolor" />
				<div class="colorpicker_box"></div>
				<div class="colorpicker_string">メニューの色を変更する</div>
			</div>
			<br />
			<div class="checkbox_div">
				<label><input type="checkbox" id="actionicon_use">状態をアイコンで表示する</label>
			</div>
			<div id="actionicon_use_div">
				<div class="filesample">
					<div class="fileicon">
						<img src="/user/actionicon/std_setting.png" width="90" height="90" />
					</div>
					<div class="filename">
						<span id="actionicon"></span>
					</div>
					<div class="changebutton">
						<a href="javascript:void(0)" data-role="button" data-icon="arrow-u"  class="useicon"><span>アイコンを変更する</span></a>
					</div>
				</div>
				<br />
			</div>
			<br />
			<br />
			<div class="checkbox_div">
				<label><input type="checkbox" id="showremocon">リモコン画面に表示する</label>
			</div>
			<div class="checkbox_div">
				<label><input type="checkbox" id="nostateremocon">状態を遷移させない</label>
			</div>
			<br />
			<div align="right"><div  align="left" class="papertag yellow">
				<p>
				「リモコン画面に表示する」のチェックを外すと項目をリモコン画面に表示しなくなります。<br />
				チェックを外すのは、マクロのためだけに設置したコマンドに使います。<br />
				<br />
				「状態を遷移させない」のチェックを入れると、家電アイコンボタンでの遷移を禁止します。<br />
				チェックを入れるのは、テレビの音量を変更するや、タイマー動作などのONOFF動作以外の補助的な動作なコマンドに使います。<br />
				</p>
			</div></div>
		</dd>
		</dl>
	</div><!-- action_not_SPACE_div -->

	<br />
	<br />
	<p><a data-role="button" data-icon="arrow-l" data-iconpos="left" onclick="action_save(  g_actionObject.find('#action_key1').val()  ,  g_actionObject.find('#action_key2').val()  );" >保存して戻る</a></p>
	<div class="error bunner_error">エラーが発生しました!!</div>

	</div><!-- /content -->

</div><!-- /page action -->

<div data-role="page" id="account" data-theme="c" data-url="account" tabindex="0">
<div data-role="header" class="header_pc"> 
	<h1><a data-role="none" href="#setting" ><img src="/jscss/images/fhc700.png" /></h1> 
</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a data-role="button" class="menubutton" data-inline="true" href="/edit/#system" data-ajax="false" data-icon="back"><span>システム全体設定</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_account.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">アカウント設定</span></div>
		</fieldset>
	</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">	
	<div class="error bunner_error">エラーが発生しました!!</div>

	<dl>
	<dt class="sub_head">メールアドレス</dt>
	<dd>
		<input type="email" id="account_usermail" value="">
	</dd>
	<dt class="sub_head">パスワード</dt>
	<dd>
		<input type="password" id="account_password" value="">

		<div class="error" id="account_error_auth_error">認証できませんでした。</div>
		<div class="error" id="account_error_uuid_error">このIDは別の端末に関連付けされているので利用できません。</div>
		<br />
		<div align="right"><div  align="left" class="papertag red">
			(上級者向け)ここで指定したパスワードで ID:sshuser の ssh loginを利用できます。<br />
		</div></div>
	</dd>
	<dt class="sub_head">リモート操作のためのサーバ同期</dt>
	<dd>
		<label><input type="checkbox" id="account_sync_server">遠隔操作のためにサーバと常に設定を同期する</label>
			<div align="right"><div  align="left" class="papertag green">
				スマホとかで遠隔で家電を操作する場合は、有効にします。<br />
				ONだと、遠隔操作のために、サーバと端末とで設定を常に同期します。<br />
				OFFだと、サーバと端末とで設定を同期しなくなりますが、スマホとかで遠隔操作はできなくなります。<br />
			</div></div>
	</dd>
	</dl>

	<p><a data-role="button" id="account_auth_button" data-icon="arrow-l" data-iconpos="left">設定する</a></p>
	<div class="error bunner_error">エラーが発生しました!!</div>

	</div><!-- /content -->
</div><!-- /page account -->

<div data-role="page" id="network" data-theme="c" data-url="network" tabindex="0">
<div data-role="header" class="header_pc"> 
	<h1><a data-role="none" href="#setting" ><img src="/jscss/images/fhc700.png" /></h1> 
</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a data-role="button" class="menubutton" data-inline="true" href="/edit/#system" data-ajax="false" data-icon="back"><span>システム全体設定</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_network.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">ネットワーク設定</span></div>
		</fieldset>
	</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">	
	<div class="error bunner_error">エラーが発生しました!!</div>

	<dl>
	<dt class="sub_head">有線ケーブルのネットワーク</dt>
	<dd>
		<select id="network_ipaddress_type" size="1">
		<option value="DHCP">DHCP</option>
		<option value="STATIC">固定IP</option>
		</select>
		<div class="error" id="network_error_40010">DHCPか固定IPのいづれを選択してください。</div>
		<div id="network_ipaddress_div">
			<dl>
			<dt>IPアドレス</dt>
			<dd>
				<input type="text" id="network_ipaddress_ip" value="">
				<div class="error" id="network_error_40001">IPアドレスが正しくありません。</div>
			</dd>
			<dt>netmask</dt>
			<dd>
				<input type="text" id="network_ipaddress_mask" value="">
				<div class="error" id="network_error_40002">netmaskが正しくありません。</div>
			</dd>
			<dt>gateway</dt>
			<dd>
				<input type="text" id="network_ipaddress_gateway" value="">
				<div class="error" id="network_error_40003">gatewayが正しくありません。</div>
			</dd>
			<dt>DNS</dt>
			<dd>
				<input type="text" id="network_ipaddress_dns" value="">
				<div class="error" id="network_error_40004">dnsが正しくありません。</div>
			</dd>
			</dl>
		</div>
	</dd>
	</dl>

	<dl id="network_is_wifi_device">
	<dt class="sub_head">無線ケーブルのネットワーク</dt>
	<dd>
		<select id="network_w_ipaddress_type" size="1">
		<option value="NONE">NONE</option>
		<option value="DHCP">DHCP</option>
		<option value="STATIC">固定IP</option>
		</select>
		<div class="error" id="network_error_40030">DHCP、固定IP、NONEのいづれを選択してください。</div>
		<div id="network_w_ipaddress_div">
			<dl>
			<dt>IPアドレス</dt>
			<dd>
				<input type="text" id="network_w_ipaddress_ip" value="">
				<div class="error" id="network_error_40021">IPアドレスが正しくありません。</div>
			</dd>
			<dt>netmask</dt>
			<dd>
				<input type="text" id="network_w_ipaddress_mask" value="">
				<div class="error" id="network_error_40022">netmaskが正しくありません。</div>
			</dd>
			<dt>gateway</dt>
			<dd>
				<input type="text" id="network_w_ipaddress_gateway" value="">
				<div class="error" id="network_error_40023">gatewayが正しくありません。</div>
			</dd>
			<dt>DNS</dt>
			<dd>
				<input type="text" id="network_w_ipaddress_dns" value="">
				<div class="error" id="network_error_40024">dnsが正しくありません。</div>
			</dd>
			</dl>
		</div>
		<div id="network_w_ipaddress_use">
			<dl>
			<dt>SSID</dt>
			<dd>
				<input type="text" id="network_w_ipaddress_ssid" value="">
				<div class="error" id="network_error_40025">SSIDが正しくありません。</div>
			</dd>
			<dt>password</dt>
			<dd>
				<input type="password" id="network_w_ipaddress_password" value="">
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
		<div align="right"><div  align="left" class="papertag red">
			<p>Future Home Controller の IPアドレスの設定を行います。<br />
			ネットワークの設定を間違えて接続できなくなった場合は、<br /><a href="https://fhc.rti-giken.jp/networksetting/" target="_blank">リモートからのネットワークの設定</a>が利用できます。<br/>
			</p>
		</div></div>
	<p>
	
	</p>
	<p><a data-role="button" id="network_apply_button" data-icon="arrow-l" data-iconpos="left">設定する</a></p>
	<div class="error bunner_error">エラーが発生しました!!</div>
	</div><!-- /content -->
</div><!-- /page network -->

<div data-role="page" id="recong" data-theme="c" data-url="recong" tabindex="0">
<div data-role="header" class="header_pc"> 
	<h1><a data-role="none" href="#setting" ><img src="/jscss/images/fhc700.png" /></h1> 
</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a data-role="button" class="menubutton" data-inline="true" href="/edit/#setting" data-ajax="false" data-icon="back"><span>設定</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_recong.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">音声認識設定</span></div>
		</fieldset>
	</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">	
	<div class="error bunner_error">エラーが発生しました!!</div>

	<dl>
	<dt  class="sub_head">誤認識フィルタ</dt>
	<dd>
		<select id="recong_type" size="1">
		<option value="julius_standalone">2パスチェックする</option>
		<option value="julius_standalone_light">簡易認識をする</option>
		<option value="none">音声認識を利用しない</option>
		</select>
		<br />
		<div align="right"><div  align="left" class="papertag red">
			<p>
			2パスチェックする<br />
			精度:普通　速度:普通<br />一度認識したものを再度音声認識に投入することで、精度を確保します。
			</p>
			<p>
			簡易認識をする<br />
			精度:低い　速度:高速<br />一度の音声認識の結果を採用します。<br />
			動作は高速ですが、精度が下がります。
			</p>
			<p>
			音声認識を利用しない<br />
			精度:--　速度:--<br />音声認識を利用しません。
			</p>
		</div></div>
		<br />
	</dd>
	<dt  class="sub_head recong_use_class">1次フィルタオプション</dt>
	<dd class="recong_use_class">
		<div>
			<label for="recong_julius_gomi_y">呼びかけサポーター強度: 大きいほど強度に弾きます。4が標準です。</label>
			<input type="range" id="recong_julius_gomi_y"  data-theme="b"  min="1" max="7" data-highlight="true" /><br />
		</div>
		<br />
		<div>
			<label for="recong_julius_gomi_e">命令サポーター強度: 大きいほど強度に弾きます。3が標準です。</label>
			<input type="range" id="recong_julius_gomi_e"  data-theme="b"  min="1" max="7" data-highlight="true" /><br />
		</div>
		<br />
		<div>
			<label for="recong_julius_filter1">1次通過確率: 大きいほど強度に弾きます。45が標準です。</label>
			<input type="range" id="recong_julius_filter1"  data-theme="b"  min="1" max="70" data-highlight="true" /><br />
		</div>
		<br />
		<br />
		<div align="right"><div  align="left" class="papertag white">
			<p>
			音声認識エンジンに渡すパラメータを指定します。<br />
			値を小さくすると、曖昧な声でも認識しますが、誤動作のリスクがあります。<br />
			大きくすると、より強固になりますが、曖昧な声では認識しなくなります。<br />
			</p>

			<p>
			呼びかけサポーター強度<br />
			呼びかけをチェックするときに、比較検討する候補のレベルを設定します。<br />
			呼びかけとは、「コンピュータ、電気つけて」の「コンピュータ」と呼びかける部分です。<br />
			</p>

			<p>
			命令サポーター強度<br />
			命令をチェックするときに、比較検討する候補のレベルを設定します。<br />
			命令とは、「コンピュータ、電気つけて」の「電気つけて」とやってほしい命令する部分のことです。<br />
			</p>

			<p>
			1次通過確率<br />
			命令の認識精度として利用します。値は最終的に確率になります。<br />
			命令とは、「コンピュータ、電気つけて」の「電気つけて」とやってほしい命令する部分のことです。<br />
			</p>
		</div></div>
		<br />
	</dd>
	<dt  class="sub_head recong_use_class recong_2pass_filter_class">2次フィルタオプション</dt>
	<dd class="recong_use_class recong_2pass_filter_class">
		<div>
			<label for="recong_julius_gomi_d">2次フィルタサポーター強度: 大きいほど強度に弾きます。5が標準です。</label>
			<input type="range" id="recong_julius_gomi_d"  data-theme="d"  min="1" max="7" data-highlight="true" /><br />
		</div>
		<br />
		<div>
			<label for="recong_julius_filter2">2次通過確率: 大きいほど強度に弾きます。25が標準です。</label>
			<input type="range" id="recong_julius_filter2" data-theme="d" min="1" max="70" data-highlight="true" /><br />
		</div>
		<br />
		<br />
		<div align="right"><div  align="left" class="papertag white">
			<p>
			音声認識エンジン(2パスチェック時)に渡すパラメータを指定します。<br />
			2パスチェックでは、呼びかけ部分を再評価します。<br />
			呼びかけとは、「コンピュータ、電気つけて」の「コンピュータ」と呼びかける部分です。<br />
			<br />
			値を小さくすると、曖昧な声でも認識しますが、誤動作のリスクがあります。<br />
			大きくすると、より強固になりますが、曖昧な声では認識しなくなります。<br />
			</p>

			<p>
			2次フィルタサポーター強度<br />
			2パスチェックするときに、比較検討する候補のレベルを設定します。<br />
			</p>
			
			<p>
			2次通過確率<br />
			2パスチェックした場合の呼びかけの認識精度として利用します。値は最終的に確率になります。<br />
			</p>
		</div></div>
		<br />
	</dd>
	<dt class="sub_head">マイク音量</dt>
	<dd>
		<label for="recong_volume">音量:</label>
		<input type="range" id="recong_volume"  min="0" max="100" data-highlight="true" /><br />
		<br />
		<div align="right"><div  align="left" class="papertag purple">
			<p>
			マイク音量を調整します。<br />
			数字が100に近くなるほどマイクは小さい音を拾うようになります。<br />
			<br />
			周りが騒がしいと、その雑音までを拾ってしまい音声認識に失敗します。<br />
			終わりが騒がしい場合は、マイクの音量を下げてみてください。<br />
			</p>
		</div></div>
	</dd>
	<dt  class="sub_head recong_use_class">区間切り出しオプション</dt>
	<dd class="recong_use_class">
		<div>
			<label for="recong_julius_lv_base">区間切り出し標準レベル: 大きいほど周囲の音の影響を受けなくなります。</label>
			<input type="range" id="recong_julius_lv_base"  data-theme="b"  min="1" max="350" data-highlight="true" /><br />
		</div>
		<br />
		<div>
			<label for="recong_julius_lv_max">区間切り出し最大レベル: 周囲がうるさい時の切り出しレベルです。</label>
			<input type="range" id="recong_julius_lv_max"  data-theme="b"  min="1" max="350" data-highlight="true" /><br />
		</div>
		<br />
		<br />
		<div align="right"><div  align="left" class="papertag white">
			<p>
			マイクで拾った音から有効な音を切り出すときに利用するオプションです。<br />
			大きくすると、周囲の雑音に惑わされなく成りますが、大きな声でしゃべる必要があります。<br />
			小さくすると、小さな声でも受け取りますが、雑音に惑わされます。<br />
			<br />
			区間切り出し標準レベルからスタートし、周囲がうるさいと判断されると、区間切り出し最大レベルまで上がります。<br />
			周囲が静かだと判断されると、区間切り出し標準レベルに戻します。<br />
			</p>
		</div></div>
		<br />
	</dd>

	<dt class="sub_head">呼びかけワード</dt>
	<dd>
		<dl>
		<dt>音声認識を開始する呼びかけワードを設定します。<br />
		呼びかけワードは表記の揺れを吸収するため、最大5個まで入れることができます。<br />
		</dt>
		<dd>
			<input type="text" id="recong__yobikake__1" value="" placeholder="例:コンピューター"><br />
			<div class="error" id="recong_error_39051">読み上げできない文字です</div>
			<div class="error" id="recong_error_39061">呼びかけが短すぎます</div>
			<input type="text" id="recong__yobikake__2" value="" placeholder="例:コンピュータ"><br />
			<div class="error" id="recong_error_39052">読み上げできない文字です</div>
			<div class="error" id="recong_error_39062">呼びかけが短すぎます</div>
			<div class="error" id="recong_error_39072">呼びかけの最初のニ文字までは同じ音でなければいけません</div>
			<div class="error" id="recong_error_39082">既に利用されています</div>
			<input type="text" id="recong__yobikake__3" value=""><br />
			<div class="error" id="recong_error_39053">読み上げできない文字です</div>
			<div class="error" id="recong_error_39063">呼びかけが短すぎます</div>
			<div class="error" id="recong_error_39073">呼びかけの最初のニ文字までは同じ音でなければいけません</div>
			<div class="error" id="recong_error_39083">既に利用されています</div>
			<input type="text" id="recong__yobikake__4" value=""><br />
			<div class="error" id="recong_error_39054">読み上げできない文字です</div>
			<div class="error" id="recong_error_39064">呼びかけが短すぎます</div>
			<div class="error" id="recong_error_39074">呼びかけの最初のニ文字までは同じ音でなければいけません</div>
			<div class="error" id="recong_error_39084">既に利用されています</div>
			<input type="text" id="recong__yobikake__5" value=""><br />
			<div class="error" id="recong_error_39055">読み上げできない文字です</div>
			<div class="error" id="recong_error_39065">呼びかけが短すぎます</div>
			<div class="error" id="recong_error_39075">呼びかけの最初のニ文字までは同じ音でなければいけません</div>
			<div class="error" id="recong_error_39085">既に利用されています</div>
		</dd>
		</dl>
		<div align="right"><div  align="left" class="papertag red">
			<p>
			呼びかけは誤動作を防ぐために利用されます。<br />
			<br />
			フューチャーホームコントローラーでは、呼びかけ+命令という音声認識を行います。<br />
			例えば、呼びかけが「コンピュータ」で、命令が「電気つけて」の場合、「コンピュータ、電気つけて」のようになります。<br />
			<br />
			短すぎる呼びかけは誤動作のリスクがあるので、最低3文字以上は必要です。<br />
			実際には、6文字ぐらいを推奨します。<br />
			<br />
			複数設定する目的は、表記のゆれ(発話の揺れ)を吸収するためのものです。<br />
			複数の呼びかけを設定する場合、<br />
			呼びかけワードの最初の2文字を同じ文字で始める必要があります。<br />
			「コンピュータ」という呼びかけを設定した場合、<br />
			次の呼びかけは、「コン○○○○」のように「コン」という最初の2文字が必要です。<br />
			<br />
			呼びかけを変更することもできますが、現在、コンピュータに、最適化しているのでそのまま使うことをおすすめします。<br />
			</p>
		</div></div>
		<br />
	</dd>

	<dt class="sub_head">音声認識成功時のアラーム音</dt>
	<dd>
		<div class="filesample">
			<div class="fileicon">
				<img src="/jscss/images/recong_ok_mp3_icon.png" width="96" height="96" />
			</div>
			<div class="filename">
				<span id="recong_ok_mp3"></span><br />
			</div>
			<div class="changebutton">
				<a href="javascript:void(0)" data-role="button" data-icon="arrow-u"  class="useicon"><span>ファイルを選択する</span></a>
			</div>
			<div class="preview">
				<a href="javascript:void(0)" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a>
			</div>
		</div>
		<br />
	</dd>

	<dt class="sub_head">音声認識失敗時のアラーム音</dt>
	<dd>
		<div class="filesample">
			<div class="fileicon">
				<img src="/jscss/images/recong_ng_mp3_icon.png" width="96" height="96" />
			</div>
			<div class="filename">
				<span id="recong_ng_mp3"></span><br />
			</div>
			<div class="changebutton">
				<a href="javascript:void(0)" data-role="button" data-icon="arrow-u"  class="useicon"><span>ファイルを選択する</span></a>
			</div>
			<div class="preview">
				<a href="javascript:void(0)" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a>
			</div>
		</div>
		<br />
	</dd>


	<dt class="sub_head">音声認識一時停止中のアラーム音</dt>
	<dd>
		<div class="filesample">
			<div class="fileicon">
				<img src="/jscss/images/recong_pause_mp3_icon.png" width="96" height="96" />
			</div>
			<div class="filename">
				<span id="recong_pause_mp3"></span><br />
			</div>
			<div class="changebutton">
				<a href="javascript:void(0)" data-role="button" data-icon="arrow-u"  class="useicon"><span>ファイルを選択する</span></a>
			</div>
			<div class="preview">
				<a href="javascript:void(0)" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a>
			</div>
		</div>
		<br />
	</dd>

	</dl>

	<p><a data-role="button" id="recong_apply_button" data-icon="arrow-l" data-iconpos="left">設定する</a></p>
	<div class="error bunner_error">エラーが発生しました!!</div>
	</div><!-- /content -->
</div><!-- /page recong -->

<div data-role="page" id="speak" data-theme="c" data-url="speak" tabindex="0">
<div data-role="header" class="header_pc"> 
	<h1><a data-role="none" href="#setting" ><img src="/jscss/images/fhc700.png" /></h1> 
</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a data-role="button" class="menubutton" data-inline="true" href="/edit/#setting" data-ajax="false" data-icon="back"><span>設定</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_speak.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">合成音声設定</span></div>
		</fieldset>
	</div> 
	<div data-role="content" data-theme="c" class="contents_wrapper">	
	<div class="error bunner_error">エラーが発生しました!!</div>

	<dl>
	<dt class="sub_head">合成音声</dt>
	<dd>
		<select id="speak_type" size="1">
		<option value="none">読み上げを行わない</option>
		</select>
	</dd>
		<br />
		<div align="right"><div  align="left" class="papertag blue">
			合成音声で読み上げる声を選択できます
		</div></div>
	<dt class="sub_head">スピーカ音量</dt>
	<dd>
		<label for="speak_volume">音量:</label>
		<input type="range" id="speak_volume"  min="0" max="100" data-highlight="true" /><br />
		<div align="right"><div  align="left" class="papertag green">
			読み上げる声の大きさです。
		</div></div>
	</dd>
	<dt class="sub_head">リモートスピーカー</dt>
	<dd>
		<label><input type="checkbox" id="speak_use_remote">リモートスピーカーを利用する</label>
		<div id="speak_use_remote_div">
			<div class="table100">
				<div class="tr">
					<div class="th">IP:</div>
					<div class="td">
						<input id="speak_remote1_ip" type="text" value="" placeholder="FHCコマンダーを動かしているHOST 例:192.168.10.20:15551">&nbsp;&nbsp;
					</div>
				</div>
				<div class="tr">
					<div class="th">SECRETKEY:</div>
					<div class="td">
						<input id="speak_remote1_secretkey" type="text" value="" placeholder="FHCコマンダーのsecretkey  例:e60e41cd-fca4-4b63-8b9c-dd2c92217472">
					</div>
				</div>
			</div>
			<div class="error error_39030">リモートスピーカーと接続できません</div>
		</div>
		<div align="right"><div  align="left" class="papertag red">
			FHCコマンダーを動かしている別マシンで音を再生できます。<br />
			「<a href="https://rti-giken.jp/fhc/help/howto/fhc_commander_remotespeaker.html">詳しくは、PCのスピーカーを利用するにはを御覧ください。</a>」<br />
		</div></div>
	</dd>
	<dt class="sub_head">夜間は静かにする</dt>
	<dd>
		<label><input type="checkbox" id="speak_use_nightvolume">夜間は静かにする</label>
		<div id="speak_use_nightvolume_div">
			<dl>
				<dt>夜間時間帯</dt>
				<dd>
					<div data-role="fieldcontain">
						<fieldset data-role="controlgroup" data-type="horizontal" >
							<select id="speak_nightvolume_start_hour">
								<option value="0">0</option><option value="1">1</option><option value="2">2</option><option value="3">3</option><option value="4">4</option><option value="5">5</option><option value="6">6</option><option value="7">7</option><option value="8">8</option><option value="9">9</option>
								<option value="10">10</option><option value="11">11</option><option value="12">12</option><option value="13">13</option><option value="14">14</option><option value="15">15</option><option value="16">16</option><option value="17">17</option><option value="18">18</option><option value="19">19</option>
								<option value="20">20</option><option value="21">21</option><option value="22">22</option><option value="23">23</option>
							</select>
							<span style="font-size: 1.5em">&nbsp;:&nbsp;</span>
							<select id="speak_nightvolume_start_minute">
								<option value="0">0</option><option value="1">1</option><option value="2">2</option><option value="3">3</option><option value="4">4</option><option value="5">5</option><option value="6">6</option><option value="7">7</option><option value="8">8</option><option value="9">9</option>
								<option value="10">10</option><option value="11">11</option><option value="12">12</option><option value="13">13</option><option value="14">14</option><option value="15">15</option><option value="16">16</option><option value="17">17</option><option value="18">18</option><option value="19">19</option>
								<option value="20">20</option><option value="21">21</option><option value="22">22</option><option value="23">23</option><option value="24">24</option><option value="25">25</option><option value="26">26</option><option value="27">27</option><option value="28">28</option><option value="29">29</option>
								<option value="30">30</option><option value="31">31</option><option value="32">32</option><option value="33">33</option><option value="34">34</option><option value="35">35</option><option value="36">36</option><option value="37">37</option><option value="38">38</option><option value="39">39</option>
								<option value="40">40</option><option value="41">41</option><option value="42">42</option><option value="43">43</option><option value="44">44</option><option value="45">45</option><option value="46">46</option><option value="47">47</option><option value="48">48</option><option value="49">49</option>
								<option value="50">50</option><option value="51">51</option><option value="52">52</option><option value="53">53</option><option value="54">54</option><option value="55">55</option><option value="56">56</option><option value="57">57</option><option value="58">58</option><option value="59">59</option>
							</select>
							<span style="font-size: 1.5em">&nbsp;～&nbsp;</span>
							<select id="speak_nightvolume_end_hour">
								<option value="0">0</option><option value="1">1</option><option value="2">2</option><option value="3">3</option><option value="4">4</option><option value="5">5</option><option value="6">6</option><option value="7">7</option><option value="8">8</option><option value="9">9</option>
								<option value="10">10</option><option value="11">11</option><option value="12">12</option><option value="13">13</option><option value="14">14</option><option value="15">15</option><option value="16">16</option><option value="17">17</option><option value="18">18</option><option value="19">19</option>
								<option value="20">20</option><option value="21">21</option><option value="22">22</option><option value="23">23</option>
							</select>
							<span style="font-size: 1.5em">&nbsp;:&nbsp;</span>
							<select id="speak_nightvolume_end_minute">
								<option value="0">0</option><option value="1">1</option><option value="2">2</option><option value="3">3</option><option value="4">4</option><option value="5">5</option><option value="6">6</option><option value="7">7</option><option value="8">8</option><option value="9">9</option>
								<option value="10">10</option><option value="11">11</option><option value="12">12</option><option value="13">13</option><option value="14">14</option><option value="15">15</option><option value="16">16</option><option value="17">17</option><option value="18">18</option><option value="19">19</option>
								<option value="20">20</option><option value="21">21</option><option value="22">22</option><option value="23">23</option><option value="24">24</option><option value="25">25</option><option value="26">26</option><option value="27">27</option><option value="28">28</option><option value="29">29</option>
								<option value="30">30</option><option value="31">31</option><option value="32">32</option><option value="33">33</option><option value="34">34</option><option value="35">35</option><option value="36">36</option><option value="37">37</option><option value="38">38</option><option value="39">39</option>
								<option value="40">40</option><option value="41">41</option><option value="42">42</option><option value="43">43</option><option value="44">44</option><option value="45">45</option><option value="46">46</option><option value="47">47</option><option value="48">48</option><option value="49">49</option>
								<option value="50">50</option><option value="51">51</option><option value="52">52</option><option value="53">53</option><option value="54">54</option><option value="55">55</option><option value="56">56</option><option value="57">57</option><option value="58">58</option><option value="59">59</option>
							</select>
						</fieldset>
					</div>
				</dd>
				<dt>夜間ボリューム</dt>
					<input type="range" id="speak_nightvolume_volume"  min="0" max="100" data-highlight="true" /><br />
					<br />
				<dd>
				</dd>
			</dl>
		</div>
		<div align="right"><div  align="left" class="papertag red">
			指定した夜間時間帯には、音量を下げます。<br />
		</div></div>
	</dd>
	</dl>
	<p><a data-role="button" id="speak_apply_button" data-icon="arrow-l" data-iconpos="left">設定する</a></p>
	<div class="error bunner_error">エラーが発生しました!!</div>
	</div><!-- /content -->
</div><!-- /page speak -->

<div data-role="page" id="sensor" data-theme="c" data-url="sensor" tabindex="0">
<div data-role="header" class="header_pc"> 
	<h1><a data-role="none" href="#setting" ><img src="/jscss/images/fhc700.png" /></h1> 
</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a data-role="button" class="menubutton" data-inline="true" href="/edit/#setting" data-ajax="false" data-icon="back"><span>設定</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_sensor.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">センサー設定</span></div>
		</fieldset>
	</div> 
	<div data-role="content" data-theme="c" class="contents_wrapper">	
	<div class="error bunner_error">エラーが発生しました!!</div>

	<div class="sensor_graph" style="display: none"></div>
	<dl>
	<dt class="sub_head">温度センサー</dt>
	<dd>
		<canvas id="sensor_temp_day_graph"></canvas>
		<canvas id="sensor_temp_week_graph"></canvas>
		<br />
		<br />
		<div class="table100">
			<div class="tr">
				<div class="th">現在値</div>
				<div class="td">
					<span id="sensor_temp"></span>℃
				</div>
			</div>
			<div class="tr">
				<div class="th">取得元</div>
				<div class="td">
					<select id="sensor_temp_type" size="1">
						<option value="endel">ホームコントローラーより取得</option>
						<option value="none">取得しない</option>
					</select>
				</div>
			</div>
		</div>

		<div align="right">	<div  align="left" class="papertag red">
			<p>室温温度を取ります。<br />
			多少ズレる時があります。あくまで目安としてご利用ください。<br />
			</p>
		</div></div>

	</dd>
	<dt class="sub_head">光センサー</dt>
	<dd>
		<canvas id="sensor_lumi_day_graph"></canvas>
		<canvas id="sensor_lumi_week_graph"></canvas>
		<br />
		<br />
		<div class="table100">
			<div class="tr">
				<div class="th">現在値</div>
				<div class="td">
					<span id="sensor_lumi"></span>
				</div>
			</div>
			<div class="tr">
				<div class="th">取得元</div>
				<div class="td">
					<select id="sensor_lumi_type" size="1">
						<option value="endel">ホームコントローラーより取得</option>
						<option value="none">取得しない</option>
					</select>
				</div>
			</div>
		</div>

		<div align="right"><div align="left" class="papertag green">
			<p>0～300の値を取ります。<br />
			300に近づくと明るくなります。<br />
			</p>
		</div></div>

	</dd>
	<dt class="sub_head">人センサー(音センサー)</dt>
	<dd>
		<canvas id="sensor_sound_day_graph"></canvas>
		<canvas id="sensor_sound_week_graph"></canvas>
		<br />
		<br />
		<div class="table100">
			<div class="tr">
				<div class="th">現在値</div>
				<div class="td">
				</div>
			</div>
			<div class="tr">
				<div class="th">取得元</div>
				<div class="td">
					<span id="sensor_sound"></span>
				</div>
			</div>
		</div>
		<div align="right"><div align="left" class="papertag green">
			<p>1時間でマイク音を拾った数を表しています。(最高200個)<br />
			音がするということは、人が近くにいるという、指標の一つになります。<br />
			</p>
		</div></div>

	</dd>
	</dl>

	<p><a data-role="button" id="sensor_apply_button" data-icon="arrow-l" data-iconpos="left">設定する</a></p>
	<div class="error bunner_error">エラーが発生しました!!</div>
	</div><!-- /content -->
</div><!-- /page sensor -->

<div data-role="page" id="mictest" data-theme="c" data-url="mictest" tabindex="0">
<div data-role="header" class="header_pc"> 
	<h1><a data-role="none" href="#setting" ><img src="/jscss/images/fhc700.png" /></h1> 
</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a data-role="button" class="menubutton" data-inline="true" href="/edit/#setting" data-ajax="false" data-icon="back"><span>設定</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_mictest.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">マイクテスト</span></div>
		</fieldset>
	</div> 
	<div data-role="content" data-theme="c" class="contents_wrapper">	

	<div id="mictest_messge_sample" class="js_template">
		<div class="mictest_messge_div">
			<span class="mictest_messge_timef"></span>
			<span class="mictest_messge_text1"></span>
			<span class="mictest_messge_text2"></span>
			<div class="mictest_messge_button">
				<ul class="mictest_tab_ul">
					<li class="mictest_tab_ul_fixit">間違って認識してる場合</li>
					<li class="mictest_tab_ul_sound">音を聞いてみる</li>
					<li class="mictest_tab_ul_detail">詳細を確認する</li>
				</ul>
			</div>
			<div class="mictest_messge_data">
				<div class="mictest_messge_data_fixit hidden"></div>
				<div class="mictest_messge_data_detail hidden"></div>
				<div class="mictest_messge_data_sound hidden">
					<div>
						ファイル名:<span class="mictest_messge_data_sound_name"></span>
						<ul class="mictest_tab_ul">
							<li class="mictest_messge_data_sound_play">再生する</li>
							<li class="mictest_messge_data_sound_download">ダウンロードする</li>
						</ul>
					</div>
					<br class="clearfix" />
				</div>
			</div>
		</div>
	</div>

	<div class="fukidashi">
		<div class="fukidashi_char">
			<img src="/jscss/images/setting_icon64_speak.png" width="64" heoght="64" />
		</div>
		<div class="fukidashi_box">
			<p><span style="font-weight:bold; font-size: 1.2em;">マイクに向かって話しかけてください。</span><br/>
			例:「コンピュータ、こんにちは」 「コンピュータ、電気消して」<br />
			<br />
			<a href="https://rti-giken.jp/fhc/help/howto/recogn.html" target="_blank">音声認識のコツについてはヘルプを御覧ください。</a></p>
		</div>
		<br class="clearfix" />
	</div>

	<dl>
	<dt class="sub_head">音声認識ログ</dt>
	<dd>
	<div id="mictest_messge" tabIndex="1"></div>

	<br />
	<div id="mictest_log_textarea_autoupdate_div"><span class="start_text_marker">自動更新中。</span> 1秒ごとにログを自動更新しています。</div>
	<div id="mictest_log_textarea_updatetop_div"><span class="stop_text_marker">更新停止中。</span>ログのボックスから選択を外すと更新を再度開始します。</div>
	</dt>
	</dl>
	</div><!-- /content -->
</div><!-- /page mictest -->


<div data-role="page" id="triggerlist" data-theme="c" data-url="triggerlist" tabindex="0">
<div data-role="header" class="header_pc"> 
	<h1><a data-role="none" href="#setting" ><img src="/jscss/images/fhc700.png" /></h1> 
</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a data-role="button" class="menubutton" data-inline="true" href="/edit/#setting" data-ajax="false" data-icon="back"><span>設定</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_trigger.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">トリガー設定</span></div>
		</fieldset>
	</div> 
	<div data-role="content" data-theme="c" class="contents_wrapper">	

	<ul id="trigger_li_sample" class="js_template">
		<li>
			<a href="javascript:void(0)"  onclick="trigger_delete('%TRIGGERKEY%');" class="delicon"><img src="/jscss/images/delicon.png" /></a>

			<span class="id hidden">%TRIGGERKEY%</span>
			<div class="thum" >%TRIGGERSAMPLE%</div>
			<div class="switch">
				%TRIGGERSWITCH%
			</div>
			<a href="javascript:void(0)" onclick="triggerset_load('%TRIGGERKEY%');" class="editicon"><img src="/jscss/images/editicon.png" /></a>
		</li>
	</ul>
	<ul id="trigger_menu_ul">
	</ul>
	<p><a data-role="button" data-icon="arrow-r" data-iconpos="right" onclick="triggerset_load('new');" >トリガーを新規に追加する</a></p>
		<div align="right"><div  align="left" class="papertag purple">
			<p>「何がが発生したら動作をさせる」トリガーをセット出来ます。<br />
			</p>
		</div></div>
	</div><!-- /content -->
</div><!-- /page triggerlist -->

<div data-role="page" id="triggerset" data-theme="c" data-url="triggerset" tabindex="0">
<div data-role="header" class="header_pc"> 
	<h1><a data-role="none" href="#trigger" ><img src="/jscss/images/fhc700.png" /></h1> 
</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a data-role="button" class="menubutton" data-inline="true" href="/edit/#triggerlist" data-ajax="false" data-icon="back"><span>トリガー</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_trigger.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">トリガーセット</span></div>
		</fieldset>
	</div> 
	<div data-role="content" data-theme="c" class="contents_wrapper">	
	<div class="error bunner_error">エラーが発生しました!!</div>
	<input type="hidden" id="trigger_key" value="new">

	<dl>
	<dt class="sub_head">条件(この条件が成立した時に)</dt>
	<dd>
		<select id="trigger_if_type">
			<option value="date">時間</option>
			<option value="command">コマンド</option>
			<option value="web">Web</option>
		</select>
		<div id="trigger_if_date_div">
			<dl>
			<dt class="sub_head">条件:時間</dt>
			<dd>
				<div data-role="fieldcontain">
					<fieldset data-role="controlgroup" data-type="horizontal" >
						<select id="trigger_if_date_hour">
							<option value="0">0</option><option value="1">1</option><option value="2">2</option><option value="3">3</option><option value="4">4</option><option value="5">5</option><option value="6">6</option><option value="7">7</option><option value="8">8</option><option value="9">9</option>
							<option value="10">10</option><option value="11">11</option><option value="12">12</option><option value="13">13</option><option value="14">14</option><option value="15">15</option><option value="16">16</option><option value="17">17</option><option value="18">18</option><option value="19">19</option>
							<option value="20">20</option><option value="21">21</option><option value="22">22</option><option value="23">23</option>
						</select>
						<span style="font-size: 1.5em">&nbsp;:&nbsp;</span>
						<select id="trigger_if_date_minute">
							<option value="0">0</option><option value="1">1</option><option value="2">2</option><option value="3">3</option><option value="4">4</option><option value="5">5</option><option value="6">6</option><option value="7">7</option><option value="8">8</option><option value="9">9</option>
							<option value="10">10</option><option value="11">11</option><option value="12">12</option><option value="13">13</option><option value="14">14</option><option value="15">15</option><option value="16">16</option><option value="17">17</option><option value="18">18</option><option value="19">19</option>
							<option value="20">20</option><option value="21">21</option><option value="22">22</option><option value="23">23</option><option value="24">24</option><option value="25">25</option><option value="26">26</option><option value="27">27</option><option value="28">28</option><option value="29">29</option>
							<option value="30">30</option><option value="31">31</option><option value="32">32</option><option value="33">33</option><option value="34">34</option><option value="35">35</option><option value="36">36</option><option value="37">37</option><option value="38">38</option><option value="39">39</option>
							<option value="40">40</option><option value="41">41</option><option value="42">42</option><option value="43">43</option><option value="44">44</option><option value="45">45</option><option value="46">46</option><option value="47">47</option><option value="48">48</option><option value="49">49</option>
							<option value="50">50</option><option value="51">51</option><option value="52">52</option><option value="53">53</option><option value="54">54</option><option value="55">55</option><option value="56">56</option><option value="57">57</option><option value="58">58</option><option value="59">59</option>
						</select>
					</fieldset>
				</div>
			</dd>
			</dl>

			<label><input type="checkbox" id="trigger_if_date_loop_use">繰り返し実行する</label>
			<div id="trigger_if_date_loop_use_div">
				<dl>
				<dt class="sub_head">曜日</dt>
				<dd>
					<div  data-role="fieldcontain"> 
						<fieldset data-role="controlgroup"> 
							<label><input type="checkbox" id="trigger_if_date_loop_dayofweek_1">日曜日に実行する</label>
							<label><input type="checkbox" id="trigger_if_date_loop_dayofweek_2">月曜日に実行する</label>
							<label><input type="checkbox" id="trigger_if_date_loop_dayofweek_4">火曜日に実行する</label>
							<label><input type="checkbox" id="trigger_if_date_loop_dayofweek_8">水曜日に実行する</label>
							<label><input type="checkbox" id="trigger_if_date_loop_dayofweek_16">木曜日に実行する</label>
							<label><input type="checkbox" id="trigger_if_date_loop_dayofweek_32">金曜日に実行する</label>
							<label><input type="checkbox" id="trigger_if_date_loop_dayofweek_64">土曜日に実行する</label>
						</fieldset> 
					</div>
				</dd>
				<dt class="sub_head">祝日</dt>
				<dd>
					<div  data-role="fieldcontain"> 
						<fieldset data-role="controlgroup"> 
							<label><input type="radio" name="trigger_if_date_loop_holiday" id="trigger_if_date_loop_holiday_1" value="1">祝日関係なく</label>
							<label><input type="radio" name="trigger_if_date_loop_holiday" id="trigger_if_date_loop_holiday_2" value="2">祝日を除く</label>
							<label><input type="radio" name="trigger_if_date_loop_holiday" id="trigger_if_date_loop_holiday_3" value="3">祝日のみ</label>
						</fieldset> 
					</div>
					<select id="trigger_if_date_loop_holiday_dataof">
						<option value="japan">日本の祝日データに基づく</option>
					</select>
				</dd>
				<dt class="sub_head">毎時</dt>
				<dd>
					<select id="trigger_if_date_loop_hourly">
						<option value="0">毎時実行しない</option>
						<option value="1">1時間毎</option>
						<option value="2">2時間毎</option>
						<option value="3">3時間毎</option>
						<option value="4">4時間毎</option>
						<option value="6">6時間毎</option>
						<option value="12">12時間毎</option>
					</select>
					<br />
					<span id="trigger_if_date_loop_hourly_banner"></span>
				</dd>
				</dl>
			</div>
		</div>

		<div id="trigger_if_command_div">
			<dl>
				<dt class="sub_head">条件:コマンド<dt>
				<dd>
					<div class="filesample">
						<div class="fileicon">
							<img src="/jscss/images/icon_script_command_nocommand.png" id="trigger_if_command_icon" width="96" height="96" />
						</div>
						<div class="filename">
							<input type="hidden" id="trigger_if_command" value=""  />
							<span id="trigger_if_command_display"></span>
						</div>
						<div class="changebutton">
							<a href="javascript:void(0)" id="trigger_if_command_select_button" data-role="button" data-icon="arrow-u"  class="useicon"><span>コマンドを選択する</span></a>
						</div>
					</div>
					<div class="error" id="trigger_if_command_error">実行するコマンドを入れてください。</div>
				</dd>
				<dt><p><span id="trigger_if_command_args1_mustlabel"></span><span id="trigger_if_command_args1_label" class="command_args_text1">未定義の引数1</span></p></dt>
				<dd class="ddshift">
					<input type="text" id="trigger_if_command_select_args1" value="" class="command_args_input1">
				</dd>
				<dt><p><span id="trigger_if_command_args2_mustlabel"></span><span id="trigger_if_command_args2_label" class="command_args_text2">未定義の引数2</span></p></dt>
				<dd class="ddshift">
					<input type="text" id="trigger_if_command_select_args2" value="" class="command_args_input2">
				</dd>
				<dt><p><span id="trigger_if_command_args3_mustlabel"></span><span id="trigger_if_command_args3_label" class="command_args_text1">未定義の引数3</span></p></dt>
				<dd class="ddshift">
					<input type="text" id="trigger_if_command_select_args3" value="" class="command_args_input1">
				</dd>
				<dt><p><span id="trigger_if_command_args4_mustlabel"></span><span id="trigger_if_command_args4_label" class="command_args_text2">未定義の引数4</span></p></dt>
				<dd class="ddshift">
					<input type="text" id="trigger_if_command_select_args4" value="" class="command_args_input2">
				</dd>
				<dt><p><span id="trigger_if_command_args5_mustlabel"></span><span id="trigger_if_command_args5_label" class="command_args_text1">未定義の引数5</span></p></dt>
				<dd class="ddshift">
					<input type="text" id="trigger_if_command_select_args5" value="" class="command_args_input1">
				</dd>
			</dl>
			<div class="preview_button_div"><div><a href="javascript:void(0)" id="trigger_if_command_preview_button" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a></div></div>
			<div class="notify" id="trigger_if_command_preview_out"></div>
			<dl>
				<dt class="sub_head">ポーリング間隔<dt>
				<dd>
					<select id="trigger_if_command_pooling">
						<option value="1minute">1分間隔</option>
						<option value="2minute">2分間隔</option>
						<option value="5minute">5分間隔</option>
						<option value="10minute">10分間隔</option>
						<option value="30minute">30分間隔</option>
					</select>
				</dd>
			</dl>
		</div>

		<div id="trigger_if_web_div">
			<dl>
				<dt class="sub_head">条件:Web</dt>
				<dd>
					<dl>
					<dt>URL</dt>
					<dd>
						<input id="trigger_if_web_url" type="text" value="" placeholder="GETするWebページのURL 例:https://rti-giken.jp/?trigger=name">
						<div class="error error_20080">
							URLを入れてください
						</div>
					</dd>
					<dt>マッチ文字列</dt>
					<dd>
						<input id="trigger_if_web_string" type="text" value="" placeholder="マッチする文字列を書きます 例:電気をつける"><br />
						<div class="error error_20081">
							マッチ文字列を入れてください
						</div>
					</dd>
					</dl>
					<div class="preview_button_div"><div><a href="javascript:void(0)" id="trigger_if_web_preview_button" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a></div></div>
					<div class="notify" id="trigger_if_web_preview_out"></div>
				</dd>
				<dt class="sub_head">ポーリング間隔<dt>
				<dd>
					<select id="trigger_if_web_pooling">
						<option value="1minute">1分間隔</option>
						<option value="2minute">2分間隔</option>
						<option value="5minute">5分間隔</option>
						<option value="10minute">10分間隔</option>
						<option value="30minute">30分間隔</option>
					</select>
				</dd>
			</dl>
		</div>
		
	</dd>
	<dt class="sub_head">追加補助条件</dt>
	<dd>
		<label><input type="checkbox" id="trigger_if_elec_use">家電を条件にする</label>
		<div id="trigger_if_elec_use_div">
			<dl>
			<dt class="sub_head">追加条件:家電</dt>
			<dd>
			<div class="table100">
				<div class="tr">
					<div class="th">家電名:</div>
					<div class="td">
						<select id="trigger_if_elec_type" size="1">
						</select>
					</div>
				</div>
				<div class="tr">
					<div class="th">アクション名:</div>
					<div class="td">
						<select id="trigger_if_elec_action" size="1">
						</select>
					</div>
				</div>
				<div class="tr">
					<div class="th">条件:</div>
					<div class="td">
						<select id="trigger_if_elec_expr">
							<option value="equal">この状態の時発動する</option>
							<option value="not">この状態でない時発動する</option>
						</select>
					</div>
				</div>
			</div>
			</dd>
			</dl>
		</div>
	</dd>
	<dd>
		<label><input type="checkbox" id="trigger_if_sound_use">近くに人がいるとき</label>
		<div id="trigger_if_sound_use_div">
			<dl>
			<dt class="sub_head">追加条件:近くに人がいるとき</dt>
			<dd>
				しきい値:<br />
				<input type="range" id="trigger_if_sound_value"  data-theme="e"  min="1" max="200" step="1" data-highlight="true" /><br />
				<br />
				<select id="trigger_if_sound_expr">
					<option value="equal">しきい値以上の時(人がいるとき)</option>
					<option value="not">しきい値以下の時(人がいないとき)</option>
				</select>
				<br />
				<div id="trigger_if_sound_sample">
					<canvas id="trigger_if_sound_sample_day_graph"></canvas><br />
					<div class="table100">
						<div class="tr">
							<div class="th">現在値</div>
							<div class="td">
								<span id="trigger_if_sound_sample_value"></span>
							</div>
						</div>
					</div>
					<div class="preview_button_div"><div><a href="javascript:void(0)" id="trigger_if_sound_sample_reload_button" data-role="button" data-icon="refresh"  class="useicon"><span>reload</span></a></div></div>
				</div >
				<br />
				<br />
			</dd>
			</dl>
		</div>
	</dd>
	<dd>
		<label><input type="checkbox" id="trigger_if_weather_use">今日の天気</label>
		<div id="trigger_if_weather_use_div">
			<dl>
			<dt class="sub_head">追加条件:今日の天気</dt>
			<dd>
				<dl>
				<dt class="sub_head">場所</dt>
				<dd>
					<input type="text" id="trigger_if_weather_address" value="" placeholder="例:東京都">
				</dd>
				<dt class="sub_head">条件</dt>
				<dd>
					<select id="trigger_if_weather_value">
						<option value="rain">雨が降る</option>
						<option value="not_rain">雨ではない</option>
						<option value="soon_rain">もうすぐ雨が降る</option>
						<option value="soon_clear">もうすぐ雨がやむ</option>
					</select>
				</dd>
				<br />
				<div class="preview_button_div"><div><a href="javascript:void(0)" id="trigger_if_weather_preview_button" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a></div></div>
				<div class="notify" id="trigger_if_weather_preview_out"></div>
				<br />
				<br />
			</dd>
			</dl>
		</div>
	</dd>
	<dd>
		<label><input type="checkbox" id="trigger_if_stopper_use">前回動作時から</label>
		<div id="trigger_if_stopper_use_div">
			<dl>
			<dt class="sub_head">追加条件:前回の動作時から</dt>
			<dd>
				<select id="trigger_if_stopper_minute">
					<option value="5minute">5分後</option>
					<option value="10minute">10分後</option>
					<option value="30minute">30分後</option>
					<option value="60minute">60分後</option>
					<option value="120minute">120分後</option>
					<option value="300minute">300分後</option>
					<option value="1440minute">1440分後</option>
				</select>
				<br >
				<div id="trigger_if_stopper_display_value">
				</div>
			</dd>
			</dl>
		</div>
	</dd>
	<dt class="sub_head">実行(この動作を実行する)</dt>
	<dd>

		<select id="trigger_then_tospeak_select" size="1">
			<option value="string">文字列</option>
			<option value="musicfile">音楽ファイル</option>
			<option value="none">読み上げない</option>
		</select>

		<div id="trigger_then_tospeak_string_div">
			<dl>
				<dt class="sub_head">文字列を読み上げる<dt>
				<dd>
					<div class="filesample">
						<div class="fileicon">
							<img src="/jscss/images/icon_speak.png" width="96" height="96" />
						</div>
						<div class="changebutton">
							<input type="text" id="trigger_then_tospeak_string" value=""   size="40" placeholder="例:それじゃあ、でんきつけるよ">
						</div>
						<div class="preview">
							<a href="javascript:void(0)" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a>
						</div>
					</div>
				</dd>
			</dl>
			<div class="error" id="trigger_then_tospeak_string_error">読み上げできない文字列があります。</div>
				<div align="right"><div  align="left" class="papertag yellow">
					<p>トリガー実行時に文字列を合成音で読み上げます。<br />
					誤読をさけるために平仮名かカタカナで入れることをおすすめします。<br />
					</p>
				</div></div>
		</div>
		<div id="trigger_then_tospeak_mp3_div">
			<dl>
				<dt class="sub_head">音楽ファイルを読み上げる<dt>
				<dd>
					<div class="filesample">
						<div class="fileicon">
							<img src="/jscss/images/icon_audio.png" width="96" height="96" />
						</div>
						<div class="filename">
							<span id="trigger_then_tospeak_mp3"></span><br />
						</div>
						<div class="changebutton">
							<a href="javascript:void(0)" data-role="button" data-icon="arrow-u"  class="useicon"><span>ファイルを選択する</span></a>
						</div>
						<div class="preview">
							<a href="javascript:void(0)" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a>
						</div>
					</div>
				</dd>
			</dl>
			<div class="error" id="trigger_then_tospeak_mp3_error">読み上げるファイル名を入れてください。</div>
				<div align="right"><div  align="left" class="papertag green">
					<p>トリガー実行時にmp3ファイルを再生します。<br />
					</p>
				</div></div>
		</div>


		<dl>
		<dt class="sub_head">実行:マクロ</dt>
		<dd>
			<label><input type="checkbox" id="trigger_then_macro_use">家電を操作する</label>
			<div id="trigger_then_macro_div">
				<div data-role="controlgroup" data-type="horizontal" class="macrotable">
					<span class="label">マクロ1</span>
					<div class="selectbox">
						<select id="trigger_then_macro_1_elec" size="1">
							<option value="none">利用しない</option>
						</select>
						<select id="trigger_then_macro_1_action" size="1">
							<option value="none">利用しない</option>
						</select>
						<select id="trigger_then_macro_1_after" size="1">
							<option value="next">次のコマンドへ</option>
							<option value="wait_1sec">1秒待ってから次へ</option>
							<option value="wait_1sec">2秒待ってから次へ</option>
							<option value="ifnext">状態が同じなら何もせず次へ</option>
						</select>
					</div>
				</div>
					<div class="error" id="trigger_then_macro_1_error">呼び出し回数の上限を超えました。<br />
					マクロ1の中で無限ループに入っている可能性があります。<br />
					</div>

				<div data-role="controlgroup" data-type="horizontal" class="macrotable">
					<span class="label">マクロ2</span>
					<div class="selectbox">
						<select id="trigger_then_macro_2_elec" size="1">
							<option value="none">利用しない</option>
						</select>
						<select id="trigger_then_macro_2_action" size="1">
							<option value="none">利用しない</option>
						</select>
						<select id="trigger_then_macro_2_after" size="1">
							<option value="next">次のコマンドへ</option>
							<option value="wait_1sec">1秒待ってから次へ</option>
							<option value="wait_2sec">2秒待ってから次へ</option>
							<option value="ifnext">状態が同じなら何もせず次へ</option>
						</select>
					</div>
				</div>
					<div class="error" id="trigger_then_macro_2_error">呼び出し回数の上限を超えました。<br />
					マクロ2の中で無限ループに入っている可能性があります。<br />
					</div>

				<div data-role="controlgroup" data-type="horizontal" class="macrotable">
					<span class="label">マクロ3</span>
					<div class="selectbox">
						<select id="trigger_then_macro_3_elec" size="1">
							<option value="none">利用しない</option>
						</select>
						<select id="trigger_then_macro_3_action" size="1">
							<option value="none">利用しない</option>
						</select>
						<select id="trigger_then_macro_3_after" size="1">
							<option value="next">次のコマンドへ</option>
							<option value="wait_1sec">1秒待ってから次へ</option>
							<option value="wait_2sec">2秒待ってから次へ</option>
							<option value="ifnext">状態が同じなら何もせず次へ</option>
						</select>
					</div>
				</div>
					<div class="error" id="trigger_then_macro_3_error">呼び出し回数の上限を超えました。<br />
					マクロ3の中で無限ループに入っている可能性があります。<br />
					</div>

				<div data-role="controlgroup" data-type="horizontal" class="macrotable">
					<span class="label">マクロ4</span>
					<div class="selectbox">
						<select id="trigger_then_macro_4_elec" size="1">
							<option value="none">利用しない</option>
						</select>
						<select id="trigger_then_macro_4_action" size="1">
							<option value="none">利用しない</option>
						</select>
						<select id="trigger_then_macro_4_after" size="1">
							<option value="next">次のコマンドへ</option>
							<option value="wait_1sec">1秒待ってから次へ</option>
							<option value="wait_2sec">2秒待ってから次へ</option>
							<option value="ifnext">状態が同じなら何もせず次へ</option>
						</select>
					</div>
				</div>
					<div class="error" id="trigger_then_macro_4_error">呼び出し回数の上限を超えました。<br />
					マクロ4の中で無限ループに入っている可能性があります。<br />
					</div>

				<div data-role="controlgroup" data-type="horizontal" class="macrotable">
					<span class="label">マクロ5</span>
					<div class="selectbox">
						<select id="trigger_then_macro_5_elec" size="1">
							<option value="none">利用しない</option>
						</select>
						<select id="trigger_then_macro_5_action" size="1">
							<option value="none">利用しない</option>
						</select>
						<select id="trigger_then_macro_5_after" size="1">
							<option value="next">次のコマンドへ</option>
							<option value="wait_1sec">1秒待ってから次へ</option>
							<option value="wait_2sec">2秒待ってから次へ</option>
							<option value="ifnext">状態が同じなら何もせず次へ</option>
						</select>
					<div>
				</div>
					<div class="error" id="trigger_then_macro_5_error">呼び出し回数の上限を超えました。<br />
					マクロ5の中で無限ループに入っている可能性があります。<br />
					</div>

				<div data-role="controlgroup" data-type="horizontal" class="macrotable">
					<span class="label">マクロ6</span>
					<div class="selectbox">
						<select id="trigger_then_macro_6_elec" size="1">
							<option value="none">利用しない</option>
						</select>
						<select id="trigger_then_macro_6_action" size="1">
							<option value="none">利用しない</option>
						</select>
						<select id="trigger_then_macro_6_after" size="1">
							<option value="next">次のコマンドへ</option>
							<option value="wait_1sec">1秒待ってから次へ</option>
							<option value="wait_2sec">2秒待ってから次へ</option>
							<option value="ifnext">状態が同じなら何もせず次へ</option>
						</select>
					</div>
				</div>
					<div class="error" id="trigger_then_macro_6_error">呼び出し回数の上限を超えました。<br />
					マクロ6の中で無限ループに入っている可能性があります。<br />
					</div>

				<div data-role="controlgroup" data-type="horizontal" class="macrotable">
					<span class="label">マクロ7</span>
					<div class="selectbox">
						<select id="trigger_then_macro_7_elec" size="1">
							<option value="none">利用しない</option>
						</select>
						<select id="trigger_then_macro_7_action" size="1">
							<option value="none">利用しない</option>
						</select>
						<select id="trigger_then_macro_7_after" size="1">
							<option value="next">次のコマンドへ</option>
							<option value="wait_1sec">1秒待ってから次へ</option>
							<option value="wait_2sec">2秒待ってから次へ</option>
							<option value="ifnext">状態が同じなら何もせず次へ</option>
						</select>
					</div>
				</div>
					<div class="error" id="trigger_then_macro_7_error">呼び出し回数の上限を超えました。<br />
					マクロ7の中で無限ループに入っている可能性があります。<br />
					</div>

				<div data-role="controlgroup" data-type="horizontal" class="macrotable">
					<span class="label">マクロ8</span>
					<div class="selectbox">
						<select id="trigger_then_macro_8_elec" size="1">
							<option value="none">利用しない</option>
						</select>
						<select id="trigger_then_macro_8_action" size="1">
							<option value="none">利用しない</option>
						</select>
						<select id="trigger_then_macro_8_after" size="1">
							<option value="next">次のコマンドへ</option>
							<option value="wait_1sec">1秒待ってから次へ</option>
							<option value="wait_2sec">2秒待ってから次へ</option>
							<option value="ifnext">状態が同じなら何もせず次へ</option>
						</select>
					</div>
				</div>
					<div class="error" id="trigger_then_macro_8_error">呼び出し回数の上限を超えました。<br />
					マクロ8の中で無限ループに入っている可能性があります。<br />
					</div>

				<div data-role="controlgroup" data-type="horizontal" class="macrotable">
					<span class="label">マクロ9</span>
					<div class="selectbox">
						<select id="trigger_then_macro_9_elec" size="1">
							<option value="none">利用しない</option>
						</select>
						<select id="trigger_then_macro_9_action" size="1">
							<option value="none">利用しない</option>
						</select>
						<select id="trigger_then_macro_9_after" size="1">
							<option value="next">次のコマンドへ</option>
							<option value="wait_1sec">1秒待ってから次へ</option>
							<option value="wait_2sec">2秒待ってから次へ</option>
							<option value="ifnext">状態が同じなら何もせず次へ</option>
						</select>
					</div>
				</div>
					<div class="error" id="trigger_then_macro_9_error">呼び出し回数の上限を超えました。<br />
					マクロ9の中で無限ループに入っている可能性があります。<br />
					</div>

				<div class="preview_button_div"><div><a href="javascript:void(0)" id="trigger_then_macro_preview_button" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a></div></div>
							<div align="right"><div  align="left" class="papertag yellow">
								<p>複数の動作を連動させることができます。<br />
								<br />
								複数の家電を連動させるといった使い方や、リモコンキーを2回押さないといけない動作などを設定できます。<br >
								<br />
								(無限再帰をさけるために最大30ステップまで実行できます)<br />
								</p>
							</div></div>
			</div>
		</dd>
		</dl>

		<dl>
		<dt class="sub_head">実行:コマンド<dt>
		<dd class="ddshift">
			<label><input type="checkbox" id="trigger_then_command_use">コマンドを実行する</label>
			<div id="trigger_then_command_div">
				<dl>
				<dd>
					<div class="filesample">
						<div class="fileicon">
							<img src="/jscss/images/icon_script_command_nocommand.png" id="trigger_then_command_select_icon" width="96" height="96" />
						</div>
						<div class="filename">
							<input type="hidden" id="trigger_then_command" value=""  />
							<span id="trigger_then_command_display"></span>
						</div>
						<div class="changebutton">
							<a href="javascript:void(0)" id="trigger_then_command_select_button" data-role="button" data-icon="arrow-u"  class="useicon"><span>コマンドを選択する</span></a>
						</div>
					</div>
					<div class="error" id="trigger_then_command_error">実行するコマンドを入れてください。</div>
				</dd>
				<dt><p><span id="trigger_then_command_args1_mustlabel"></span><span id="trigger_then_command_args1_label" class="command_args_text1">未定義の引数1</span></p></dt>
				<dd class="ddshift">
					<input type="text" id="trigger_then_command_select_args1" value="" class="command_args_input1">
				</dd>
				<dt><p><span id="trigger_then_command_args2_mustlabel"></span><span id="trigger_then_command_args2_label" class="command_args_text2">未定義の引数2</span></p></dt>
				<dd class="ddshift">
					<input type="text" id="trigger_then_command_select_args2" value="" class="command_args_input2">
				</dd>
				<dt><p><span id="trigger_then_command_args3_mustlabel"></span><span id="trigger_then_command_args3_label" class="command_args_text1">未定義の引数3</span></p></dt>
				<dd class="ddshift">
					<input type="text" id="trigger_then_command_select_args3" value="" class="command_args_input1">
				</dd>
				<dt><p><span id="trigger_then_command_args4_mustlabel"></span><span id="trigger_then_command_args4_label" class="command_args_text2">未定義の引数4</span></p></dt>
				<dd class="ddshift">
					<input type="text" id="trigger_then_command_select_args4" value="" class="command_args_input2">
				</dd>
				<dt><p><span id="trigger_then_command_args5_mustlabel"></span><span id="trigger_then_command_args5_label" class="command_args_text1">未定義の引数5</span></p></dt>
				<dd class="ddshift">
					<input type="text" id="trigger_then_command_select_args5" value="" class="command_args_input1">
				</dd>
				</dl>
			</div>
		</dd>
		</dl>
		<div class="preview_button_div"><div><a href="javascript:void(0)" id="trigger_then_command_preview_button" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a></div></div>
	</dd>
	</dl>

	<dl>
	<dt class="sub_head">表示</dt>
	<dd>
		<dl>
		<dt class="sub_head">トリガーの名前<dt>
		<dd class="ddshift">
			<div style="width: 70%;">
				<input type="text" id="trigger_display_name" value="" placeholder="トリガー名">
			</div>
		</dd>
		</dl>
	</dd>
	</dl>


	<p><a data-role="button" id="trigger_apply_button" data-icon="arrow-l" data-iconpos="left">設定する</a></p>
	<div class="error bunner_error">エラーが発生しました!!</div>
	</div><!-- /content -->
</div><!-- /page triggerset -->

<div data-role="page" id="system" data-theme="c" data-url="system" tabindex="0">
<div data-role="header" class="header_pc"> 
	<h1><a data-role="none" href="#setting" ><img src="/jscss/images/fhc700.png" /></h1> 
</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a data-role="button" class="menubutton" data-inline="true" href="/edit/#setting" data-ajax="false" data-icon="back"><span>設定</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_system.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">システム全般設定</span></div>
		</fieldset>
	</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">	
	<div class="error bunner_error">エラーが発生しました!!</div>

	<dl>
	<dt class="sub_head">システム起動音</dt>
	<dd>
		<div class="filesample">
			<div class="fileicon">
				<img src="/jscss/images/recong_ok_mp3_icon.png" width="96" height="96" />
			</div>
			<div class="filename">
				<span id="system_fhcboot_mp3"></span><br />
			</div>
			<div class="changebutton">
				<a href="javascript:void(0)" data-role="button" data-icon="arrow-u"  class="useicon"><span>ファイルを選択する</span></a>
			</div>
			<div class="preview">
				<a href="javascript:void(0)" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a>
			</div>
		</div>
		<br />
		<div align="right"><div align="left" class="papertag green">
			<p>フューチャーホームコントローラーが起動した時に鳴らす起動音を設定します。	</p>
		</div></div>
		<br />
	</dd>
	<dt class="sub_head">アカウント設定</dt>
	<dd>
		<div class="filesample">
			<div class="fileicon">
				<img src="/jscss/images/setting_icon96_account.png" width="96" height="96" />
			</div>
			<div class="changebutton">
				<a href="javascript:void(0)" onclick="account_load();" data-role="button" data-icon="arrow-u"  class="useicon"><span>アカウントの設定に行く</span></a>
			</div>
		</div>
		<br />
		<div align="right"><div align="left" class="papertag green">
			<p>ユーザ名とパスワードの変更を行います。</p>
		</div></div>
	</dd>
	<dt class="sub_head">ネットワーク設定</dt>
	<dd>
		<div class="filesample">
			<div class="fileicon">
				<img src="/jscss/images/setting_icon96_network.png" width="96" height="96" />
			</div>
			<div class="changebutton">
				<a href="javascript:void(0)" onclick="network_load();" data-role="button" data-icon="arrow-u"  class="useicon"><span>ネットワークの設定に行く</span></a>
			</div>
		</div>
		<br />
		<div align="right"><div align="left" class="papertag green">
			<p>IPアドレス等の変更を行います。</p>
		</div></div>
	</dd>
	<dt class="sub_head">Homekitエミュレーション</dt>
	<dd>
		<div class="filesample">
			<div class="fileicon">
				<img src="/jscss/images/setting_icon96_homekit.png" width="96" height="96" />
			</div>
			<div class="changebutton">
				<a href="javascript:void(0)" onclick="homekit_load();" data-role="button" data-icon="arrow-u"  class="useicon"><span>Homekit設定に行く</span></a>
			</div>
		</div>
		<br />
		<div align="right"><div align="left" class="papertag green">
			<p>ios Homekitとの接続設定をします</p>
		</div></div>
	</dd>
	<dt class="sub_head">複数台連携</dt>
	<dd>
		<div class="filesample">
			<div class="fileicon">
				<img src="/jscss/images/setting_icon96_multiroomlist.png" width="96" height="96" />
			</div>
			<div class="changebutton">
				<a href="javascript:void(0)" onclick="multiroomlist_load();" data-role="button" data-icon="arrow-u"  class="useicon"><span>複数台連携の設定に行く</span></a>
			</div>
		</div>
		<br />
		<div align="right"><div align="left" class="papertag white">
			<p>複数の部屋に設置されたFHC端末を連動させる設定をします。</p>
		</div></div>
	</dd>
<!--
	<dt class="sub_head">SIP電話連携</dt>
	<dd>
		<div class="filesample">
			<div class="fileicon">
				<img src="/jscss/images/setting_icon96_sip.png" width="96" height="96" />
			</div>
			<div class="changebutton">
				<a href="javascript:void(0)" onclick="sip_load();" data-role="button" data-icon="arrow-u"  class="useicon"><span>SIP電話連携の設定に行く</span></a>
			</div>
		</div>
		<br />
		<div align="right"><div align="left" class="papertag white">
			<p>IP電話として、電話を発信、着信させるための設定をします。</p>
		</div></div>
	</dd>
-->
	<dt class="sub_head">言語切り替え</dt>
	<dd>
		<div class="filesample">
			<div class="fileicon">
				<img src="/jscss/images/setting_icon96_changelang.png" width="96" height="96" />
			</div>
			<div class="changebutton">
				<a href="javascript:void(0)" onclick="changelang_load();" data-role="button" data-icon="arrow-u"  class="useicon"><span>言語切り替えに行く</span></a>
			</div>
		</div>
		<br />
		<div align="right"><div align="left" class="papertag green">
			<p>利用する言語を切り替えます。(テスト版)</p>
		</div></div>
	</dd>
	<dt class="sub_head">twitter</dt>
	<dd>
		<div class="filesample">
			<div class="fileicon">
				<img src="/jscss/images/setting_icon96_twitter.png" width="96" height="96" />
			</div>
			<div class="filename">
				<span id="twitter_command_display"></span>
			</div>
			<div class="changebutton">
				<a href="/setting/twitter/regist" id="twitter_command_select_button" data-role="button" data-icon="arrow-u" data-ajax="false" class="useicon"><span>認証する</span></a>
			</div>
			<div class="preview">
				<a href="/setting/twitter/cancel" id="twitter_command_cancel_button" data-role="button" data-icon="arrow-u" data-ajax="false" class="useicon"><span>認証解除</span></a>
			</div>
		</div>
		<br />
		<br />
		<div class="notify" id="twitter_command_linked_notify">
			twitter認証が成功しています。<br />
			twitter関係の命令を呼び出すことで操作することができます。<br />
			<a href="https://rti-giken.jp/fhc/help/howto/twitter.html" data-ajax="false" target="_blank">詳しくはこちらをご覧ください。</a><br />
			<br />
			また、twitter認証を解除したい場合は、「認証解除」をクリックしてください。<br />
		</div>
		<br />
		<br />
		<div align="right"><div align="left" class="papertag green">
			<p>twitter連携コマンドを利用するため認証します。<br />
			<a href="https://rti-giken.jp/fhc/help/howto/twitter.html" data-ajax="false" target="_blank">コマンドの詳細はこちらをご覧ください。</a></p>
		</div></div>
	</dd>

	<dt class="sub_head">天気取得スクリプト</dt>
	<dd>
		<div class="filesample">
			<div class="fileicon">
				<img src="/jscss/images/setting_icon96_weather.png" width="96" height="96" />
			</div>
			<div class="filename">
				<input type="hidden" id="weather_script_command" value=""  />
				<span id="weather_script_command_display"></span>
			</div>
			<div class="changebutton">
				<a href="javascript:void(0)" id="weather_script_command_select_button" data-role="button" data-icon="arrow-u"  class="useicon"><span>コマンドを選択する</span></a>
			</div>
			<div class="preview">
				<a href="javascript:void(0)" id="weather_script_command_preview_button" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a>
			</div>
		</div>
		<br />
		<div class="error" id="weather_script_command_error">実行するコマンドを入れてください。</div>
		<div class="notify" id="weather_script_command_preview_out"></div>
		
		<br />
		<div align="right"><div align="left" class="papertag green">
			<p>天気予報を取得するスクリプトを選択します。</p>
		</div></div>
	</dd>
	<dt class="sub_head">電車遅延スクリプト</dt>
	<dd>
		<div class="filesample">
			<div class="fileicon">
				<img src="/jscss/images/setting_icon96_train.png" width="96" height="96" />
			</div>
			<div class="filename">
				<input type="hidden" id="train_script_command" value=""  />
				<span id="train_script_command_display"></span>
			</div>
			<div class="changebutton">
				<a href="javascript:void(0)" id="train_script_command_select_button" data-role="button" data-icon="arrow-u"  class="useicon"><span>コマンドを選択する</span></a>
			</div>
			<div class="preview">
				<a href="javascript:void(0)" id="train_script_command_preview_button" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a>
			</div>
		</div>
		<br />
		<div class="error" id="train_script_command_error">実行するコマンドを入れてください。</div>
		<div class="notify" id="train_script_command_preview_out"></div>
		<br />
		<div align="right"><div align="left" class="papertag green">
			<p>電車などの遅延を取得するスクリプトを選択します。</p>
		</div></div>
	</dd>
	</dl>
	<p><a data-role="button" id="system_apply_button" data-icon="arrow-l" data-iconpos="left">設定する</a></p>
	<div class="error bunner_error">エラーが発生しました!!</div>
	</div><!-- /content -->
</div><!-- /page system -->

<div data-role="page" id="changelang" data-theme="c" data-url="changelang" tabindex="0">
<div data-role="header" class="header_pc"> 
	<h1><a data-role="none" href="#setting" ><img src="/jscss/images/fhc700.png" /></h1> 
</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a data-role="button" class="menubutton" data-inline="true" href="/edit/#setting" data-ajax="false" data-icon="back"><span>設定</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_changelang.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">言語切り替え</span></div>
		</fieldset>
	</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">	
	<div class="error bunner_error">エラーが発生しました!!</div>

	<dl>
	<dt class="sub_head">言語切り替え</dt>
	<dd>
		<select id="system_lang" size="1">
			<option value="japanese">日本語</option>
			<option value="english">英語</option>
		</select>
		<br />
		<br />
		<div align="right"><div align="left" class="papertag green">
			<p>システムの言語を切り替えます。<br/>
			(この機能は実験的な機能です。通常は使用しないでください。)<br/>
			<br/>
			日本語に変更すると、日本語の音声認識、合成音声を利用します。<br/>
			呼びかけワードのディフォルトはコンピュータです。<br/>
			ある程度の英語の日本語読みに対応しています<br/>
			<br/>
			英語に変更すると、英語の音声認識、合成音声を利用します。呼びかけワードのディフォルトはcomputerです。<br/>
			命名の日本語読みには対応していないので、英語で読みを入れていく必要があります。例:computer, turn off light<br/>
			</p>
		</div></div>
	</dd>
	</dl>
	<p><a data-role="button" id="changelang_apply_button" data-icon="arrow-l" data-iconpos="left">設定する</a></p>
	<div class="error bunner_error">エラーが発生しました!!</div>
	</div><!-- /content -->

</div><!-- /page changelang -->

<div data-role="page" id="homekit" data-theme="c" data-url="homekit" tabindex="0">
<div data-role="header" class="header_pc"> 
	<h1><a data-role="none" href="#setting" ><img src="/jscss/images/fhc700.png" /></h1> 
</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a data-role="button" class="menubutton" data-inline="true" href="/edit/#system" data-ajax="false" data-icon="back"><span>システム全体設定</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_homekit.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">Homekit</span></div>
		</fieldset>
	</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">	
	<div class="error bunner_error">エラーが発生しました!!</div>

	<dl>
	<dt class="sub_head">Homekit</dt>
	<dd>
		<div class="homekit_is_disable">
			Homekitエミュレーションは無効になっています。<br/>
			有効にするには、「有効にする」ボタンを押してください。<br/>
			<a id="homekit_enable_button" data-role="button" data-icon="arrow-u" class="useicon"><span>Homekitを有効にする</span></a>
		</div>
		<div class="homekit_is_enable">
			Homekitエミュレーションは有効になっています。<br/>
			無効にするには、「無効にする」ボタンを押してください。<br/>
			<a id="homekit_disable_button" data-role="button" data-icon="arrow-u" class="useicon"><span>Homekitを無効にする</span></a>
		</div>
		<div class="error homekit_cannot_start">システムを開始できません。avahi(Bonjour)が無効です。</div>
		<div align="right"><div align="left" class="papertag green">
			<p>Homekitとの通信をエミュレーションします。<br/>
			(この機能は実験的な機能です。)<br/>
			</p>
		</div></div>
	</dd>
	<dt class="sub_head">PINコード</dt>
	<dd class="homekit_is_enable">
		<span id="homekit_pin" style="font-size: 2.0em; letter-spacing: 1px; border: solid 4px;"></span>
		<br />
		<br />
		<div align="right"><div align="left" class="papertag red">
			<p>このPINコードを iosのホームアプリで入力すると接続できます。<br/>
			もし、接続できない場合は、「キーをリセット」を押してキーと接続情報をクリアしてください。<br/>
			<a href="https://rti-giken.jp/fhc/help/howto/homekit.html" data-ajax="false" target="_blank">詳細はこちらをご覧ください。</a><br/>
			</p>
		</div></div>
		<input type="button" id="homekit_keyreset_button" value="キーをリセットする" />
	</dd>
	</dl>
	<div class="error bunner_error">エラーが発生しました!!</div>
	</div><!-- /content -->
</div><!-- /page homekit -->

<div data-role="page" id="sip" data-theme="c" data-url="sip" tabindex="0">
<div data-role="header" class="header_pc"> 
	<h1><a data-role="none" href="#setting" ><img src="/jscss/images/fhc700.png" /></h1> 
</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a data-role="button" class="menubutton" data-inline="true" href="/edit/#system" data-ajax="false" data-icon="back"><span>システム全体設定</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_sip.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">SIP電話連携</span></div>
		</fieldset>
	</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">	
	<div class="error bunner_error">エラーが発生しました!!</div>

	<dl>
	<dt class="sub_head">SIP電話連携の是非</dt>
	<dd>
		<select id="sip_enable" size="1">
		<option value="0">SIP電話連携を無効にする</option>
		<option value="1">SIP電話連携を有効にする</option>
		</select>
		<br />
		<br />
		<div align="right"><div align="left" class="papertag red">
			<p>SIP機能を有効にすると、電話をかけることができます。<br/>
			<a href="https://rti-giken.jp/fhc/help/howto/sip.html" data-ajax="false" target="_blank">詳細はこちらをご覧ください。</a><br/>
			</p>
		</div></div>
	</dd>
	<dt  class="sub_head sip_is_enable_div">SIP ID</dt>
	<dd class="sip_is_enable_div" >
		<input type="text" id="sip_id" value="" placeholder="例:username123"><br />
		<div class="error error_39021">
			認証できません。ID、パスワード、ドメインが正しくありません。
		</div>
		<br />
		<br />
		<div align="right"><div align="left" class="papertag red">
			<p>IP電話ベンダーから取得した ID,PASSWORD,DOMAINを入れてください。<br/>
			SIPに対応しているIP電話ベンダーから入手できます。<br/>
			<a href="https://rti-giken.jp/fhc/help/howto/sip.html" data-ajax="false" target="_blank">詳細はこちらをご覧ください。</a><br/>
			</p>
		</div></div>
	</dd>
	<dt  class="sub_head sip_is_enable_div">SIP PASSWORD</dt>
	<dd class="sip_is_enable_div" >
		<input type="password" id="sip_password" value="" placeholder="例:password"><br />
	</dd>
	<dt  class="sub_head sip_is_enable_div">SIP DOMAIN</dt>
	<dd class="sip_is_enable_div">
		<input type="text" id="sip_domain" value="" placeholder="例:example.com"><br />
	</dd>
	<dt class="sub_head sip_is_enable_div">発信音</dt>
	<dd class="sip_is_enable_div">
		<div class="filesample">
			<div class="fileicon">
				<img src="/jscss/images/sip_calling_mp3_icon.png" width="96" height="96" />
			</div>
			<div class="filename">
				<span id="sip_calling_mp3"></span><br />
			</div>
			<div class="changebutton">
				<a href="javascript:void(0)" data-role="button" data-icon="arrow-u"  class="useicon"><span>ファイルを選択する</span></a>
			</div>
			<div class="preview">
				<a href="javascript:void(0)" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a>
			</div>
		</div>
		<br />
		<div align="right"><div align="left" class="papertag green">
			<p>電話をかける時に鳴らす音を設定します。</p>
		</div></div>
		<br />
	</dd>
	<dt class="sub_head sip_is_enable_div">着信の是非</dt>
	<dd class="sip_is_enable_div">
		<select id="sip_enable_incoming" size="1">
		<option value="0">着信を受け付けない</option>
		<option value="1">着信を受け付ける</option>
		</select>

		<br />
		<br />
		<div align="right"><div align="left" class="papertag red">
			<p>着信を有効にすると、電話を受けることができます。<br/>
			<a href="https://rti-giken.jp/fhc/help/howto/sip.html" data-ajax="false" target="_blank">詳細はこちらをご覧ください。</a><br/>
			</p>
		</div></div>
	</dd>
	<dt class="sub_head sip_is_enable_incoming_div">着信音</dt>
	<dd class="sip_is_enable_incoming_div">
		<div class="filesample">
			<div class="fileicon">
				<img src="/jscss/images/sip_incoming_mp3_icon.png" width="96" height="96" />
			</div>
			<div class="filename">
				<span id="sip_incoming_mp3"></span><br />
			</div>
			<div class="changebutton">
				<a href="javascript:void(0)" data-role="button" data-icon="arrow-u"  class="useicon"><span>ファイルを選択する</span></a>
			</div>
			<div class="preview">
				<a href="javascript:void(0)" data-role="button" data-icon="check"  class="useicon"><span>preview</span></a>
			</div>
		</div>
		<br />
		<div align="right"><div align="left" class="papertag green">
			<p>電話が着信した時に鳴らす音を設定します。</p>
		</div></div>
		<br />
	</dd>
	<dt class="sub_head sip_is_enable_div">通話中は電話以外の音声認識を停止する</dt>
	<dd class="sip_is_enable_div">
		<select id="sip_when_taking_stop_recogn" size="1">
		<option value="0">通話中でも全部有効にする</option>
		<option value="1">通話中は電話以外の音声認識を停止する</option>
		</select>

		<br />
		<br />
		<div align="right"><div align="left" class="papertag red">
			<p>通話中、電話発信中、着信中は、電話関係の音声認識以外の音声認識を停止するかどうかの設定です。<br/>
			</p>
		</div></div>
	</dd>
	</dl>
	<p><a data-role="button" id="sip_apply_button" data-icon="arrow-l" data-iconpos="left">設定する</a></p>
	<div class="error bunner_error">エラーが発生しました!!</div>
	</div><!-- /content -->
</div><!-- /page sip -->

<div data-role="page" id="multiroomlist" data-theme="c" data-url="multiroomlist" tabindex="0">
<div data-role="header" class="header_pc"> 
	<h1><a data-role="none" href="#setting" ><img src="/jscss/images/fhc700.png" /></h1> 
</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a data-role="button" class="menubutton" data-inline="true" href="/edit/#system" data-ajax="false" data-icon="back"><span>システム全体設定</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_multiroomlist.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">複数台連携</span></div>
		</fieldset>
	</div> 
	<div data-role="content" data-theme="c" class="contents_wrapper">	

	<ul id="multiroomlist_li_sample" class="js_template">
		<li>
			<span class="delicon" style="display: %MULTIROOMMYSELF_HIDE%;"><a href="javascript:void(0)" class="delicon"><img src="/jscss/images/delicon.png" /></a></span>
			<span class="key hidden">%MULTIROOMKEY%</span>
			<span class="name">%MULTIROOMNAME%</span>
			<span class="url">%MULTIROOMURL%</span>
			<span class="uuid">%MULTIROOMUUID%</span>
			<div class="fileusebutton" style="display: %MULTIROOMMYSELF%;">
				<span style="font-size: 2.0em;">(自機)</span>
			</div>
		</li>
	</ul>
	<ul id="multiroomlist_menu_ul" class="multiroomlist_menu_ul">
	</ul>
		<div align="right"><div  align="left" class="papertag purple">
			<p>複数の部屋に設置されたFHC端末を連動させて使えるようにします。<br />
			連動した機器は、リモコン画面からスワイプで簡単に切り替えられるようになります。<br />
			<br/>
			部屋の名前は、それぞれの端末のリモコンUI設定から変更できます。<br />
			アイコンをドラッグすると並び順を変えられます。<br />
			</p>
		</div></div>
	<p>
		<fieldset>
			<div class="posstion_50"><a data-role="button" data-icon="plus" data-iconpos="right" onclick="multiroomappend_load('');" >IPを指定して追加</a></div>
			<div class="posstion_50"><a data-role="button" data-icon="search" data-iconpos="right" onclick="multiroomsearch_load();" >検索して追加</a></div>
		</fieldset>
	</p>
	</div><!-- /content -->
</div><!-- /page multiroomlist -->

<div data-role="page" id="multiroomappend" data-theme="c" data-url="multiroomappend" tabindex="0">
<div data-role="header" class="header_pc"> 
	<h1><a data-role="none" href="#multiroomlist" ><img src="/jscss/images/fhc700.png" /></h1> 
</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a data-role="button" class="menubutton" data-inline="true" href="/edit/#multiroomlist" data-ajax="false" data-icon="back"><span>複数台連携</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_multiroomappend.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">部屋の追加</span></div>
		</fieldset>
	</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">	
	<div class="error bunner_error">エラーが発生しました!!</div>

	<dl>
	<dt  class="sub_head">追加するFHC端末のURL</dt>
	<dd>
		<input type="text" id="multiroomappend_url" value="" placeholder="例:http://192.168.10.123/"><br />
		<div class="error error_40000">
			接続できませんでした。URLが正しいか確認してください。
		</div>
	</dd>
	<dt  class="sub_head">追加するFHC端末のログインメールアドレス</dt>
	<dd>
		<input type="email" id="multiroomappend_usermail" value="" placeholder="例:foo@example.com"><br />
		<div class="error error_40001">
			ログインできませんでした。
		</div>
	</dd>
	<dt  class="sub_head">追加するFHC端末のログインパスワード</dt>
	<dd>
		<input type="password" id="multiroomappend_password" value="" placeholder="例:secret"><br />
	</dd>
	</dl>

	<p><a data-role="button" id="multiroomappend_apply_button" data-icon="arrow-r" data-iconpos="right">追加する</a></p>
	<div class="error bunner_error">エラーが発生しました!!</div>
	</div><!-- /content -->
</div><!-- /page multiroomappend -->

<div data-role="page" id="multiroomsearch" data-theme="c" data-url="multiroomsearch" tabindex="0">
<div data-role="header" class="header_pc"> 
	<h1><a data-role="none" href="#multiroomlist" ><img src="/jscss/images/fhc700.png" /></h1> 
</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a data-role="button" class="menubutton" data-inline="true" href="/edit/#multiroomlist" data-ajax="false" data-icon="back"><span>複数台連携</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_multiroomsearch.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">部屋の検索</span></div>
		</fieldset>
	</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">	

	<ul id="multiroomsearch_li_sample" class="js_template">
		<li>
			<span class="url">%MULTIROOMURL%</span>
			<span class="uuid">%MULTIROOMUUID%</span>

			<div class="fileusebutton" style="display: %MULTIROOMJOIN%;">
				<a href="javascript:void(0)" onclick="multiroomappend_load('%MULTIROOMURL%');" data-role="button" data-icon="check"  data-mini="true" class="useicon">これを使う</a>
			</div>
			<div class="fileusebutton" style="display: %MULTIROOMALREADYJOIN%;">
				既に追加されています
			</div>
		</li>
	</ul>
	<ul id="multiroomsearch_menu_ul" class="multiroomlist_menu_ul">
	</ul>
	<div id="multiroomsearch_hint_no_hit" class="hidden">
		連携できるFHC端末が見つかりませんでした。<br />
		端末の電源が入っていますか？<br />
		既に複数台連携で利用していませんか？<br />
		または、ルーター等で検出用のUUDP信号を止めていませんか？<br />
		<br />
		<a href="javascript:void(0)" onclick="multiroomappend_load('');">FHC端末のIPアドレスがわかる場合は、ここをクリックをIPを指定して直接追加することができます。</a>
	</div>

		<div align="right" class="hint hint_nohit"><div  align="left" class="papertag red">
			<p>連携できるFHC端末が見つかりませんでした。<br />
			端末の電源が入っていますか？<br />
			既に複数台連携で利用していませんか？<br />
			または、ルーター等で検出用のUUDP信号を止めていませんか？<br />
			上記を確認して、検索するボタンを再び押して検索しなおしてください。<br />
			<br />
			<a href="javascript:void(0)" onclick="multiroomappend_load('');">FHC端末のIPアドレスがわかる場合は、ここをクリックして、IPを指定して直接追加することができます。</a>
			</p>
		</div></div>
		<div align="right" class="hint hint_start"><div  align="left" class="papertag purple">
			<p>連携できるFHC端末を検索します。<br />
			連携相手となるFHCの電源を入れ、ネットワークに接続してください。<br />
			準備ができたら、検索するボタンを押して検索してください。<br/>
			</p>
		</div></div>

	<p><a data-role="button" id="multiroomsearch_search_button" data-icon="arrow-l" data-iconpos="left">連携できるFHC端末の検索する</a></p>
	</div><!-- /content -->

	
</div><!-- /page multiroomsearch -->
<div data-role="page" id="remoconsetting" data-theme="c" data-url="remoconsetting" tabindex="0">

<div data-role="header" class="header_pc"> 
	<h1><a data-role="none" href="#remoconsetting" ><img src="/jscss/images/fhc700.png" /></h1> 
</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a data-role="button" class="menubutton" data-inline="true" href="/edit/#setting" data-ajax="false" data-icon="back"><span>設定</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_remoconsetting.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">リモコンUI設定</span></div>
		</fieldset>
	</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">	
	<div class="error bunner_error">エラーが発生しました!!</div>


	<dl>
	<dd>
		<select id="remoconsetting_typeas" size="1">
		<option value="1">部屋の名前</option>
		<option value="2">ディフォルト</option>
		<option value="3">センサー</option>
		<option value="4">フッターメニューボタン</option>
		<option value="9">フッターメニューボタンの色</option>
		<option value="5">フッターメニューボタンのホバー</option>
		<option value="6">メニューダイアログ全般</option>
		<option value="10">メニューダイアログ挙動</option>
		<option value="7">メニューダイアログのボタン</option>
		<option value="8">メニューダイアログのボタンのホバー</option>
		</select>
	</dd>
	</dl>

	<div id="remoconsetting_typeas_1" class="remoconsetting_typeas_select">
		<dl>
		<dt class="sub_head">部屋の名前</dt>
		<dd>
			<div>
				<div class="idpass_name"  ><span class="cell">部屋の名前</span></div>
				<div class="idpass_input" ><span class="cell">
					<input type="text" id="system_room_string" placeholder="リモコン">
				</span></div>
			</div>
			<br />
			<div class="filesample">
				<div class="fileicon">
					<img src="/user/elecicon/onoff.png" width="96" height="96" />
				</div>
				<div class="filename">
					<span id="system_room_icon"></span>
				</div>
				<div class="changebutton">
					<a href="javascript:void(0)" data-role="button" data-icon="arrow-u"  class="useicon"><span>アイコンを変更する</span></a>
				</div>
			</div>
			<br />
			<div class="colorpicker">
				<input type="hidden" id="system_room_color" />
				<div class="colorpicker_box"></div>
				<div class="colorpicker_string">部屋の名前の文字色を変更する</div>
			</div>
		</dd>
		</dl>
	</div>
	<div id="remoconsetting_typeas_2" class="remoconsetting_typeas_select">
		<dl>
		<dt class="sub_head">ディフォルト</dt>
		<dd>
			<div class="colorpicker">
				<input type="hidden" id="remoconsetting_default_font_color" />
				<div class="colorpicker_box"></div>
				<div class="colorpicker_string">ディフォルト文字色を変更する</div>
			</div>
			<br />
			<div class="colorpicker">
				<input type="hidden" id="remoconsetting_default_background_color" />
				<div class="colorpicker_box"></div>
				<div class="colorpicker_string">ディフォルト背景色を変更する</div>
			</div>
			<br />
			<div class="filesample">
				<div class="fileicon">
					<img src="/user/elecicon/onoff.png" width="96" height="96" />
				</div>
				<div class="filename">
					<span id="remoconsetting_background_image"></span>
				</div>
				<div class="changebutton">
					<a href="javascript:void(0)" data-role="button" data-icon="arrow-u"  class="useicon"><span>背景画像を変更する</span></a>
				</div>
			</div>
			<br />
			<div class="filesample">
				<div class="fileicon">
					<img src="/user/elecicon/onoff.png" width="96" height="96" />
				</div>
				<div class="filename">
					<span id="remoconsetting_menu_icon"></span>
				</div>
				<div class="changebutton">
					<a href="javascript:void(0)" data-role="button" data-icon="arrow-u"  class="useicon"><span>詳細メニューボタンを変更する</span></a>
				</div>
			</div>
			<br />
			<div class="filesample">
				<div class="fileicon">
					<img src="/user/elecicon/onoff.png" width="64" height="64" />
				</div>
				<div class="filename">
					<span id="remoconsetting_badges_timer_icon"></span>
				</div>
				<div class="changebutton">
					<a href="javascript:void(0)" data-role="button" data-icon="arrow-u"  class="useicon"><span>タイマーバッチのアイコンを変更する</span></a>
				</div>
			</div>
		</dd>
		</dl>
	</div>
	<div id="remoconsetting_typeas_3" class="remoconsetting_typeas_select">
		<dl>
		<dt class="sub_head">センサー</dt>
		<dd>
			<div>
				<div class="idpass_name"  ><span class="cell">温度の文言</span></div>
				<div class="idpass_input" ><span class="cell">
					<input type="text" id="remoconsetting_sensor_temp_string" placeholder="温度">
				</span></div>
			</div>
			<br />
			<div>
				<div class="idpass_name"  ><span class="cell">明るさの文言</span></div>
				<div class="idpass_input" ><span class="cell">
					<input type="text" id="remoconsetting_sensor_lumi_string" placeholder="明るさ">
				</span></div>
			</div>
			<br />
			<div class="colorpicker">
				<input type="hidden" id="remoconsetting_sensor_font_color" />
				<div class="colorpicker_box"></div>
				<div class="colorpicker_string">センサーの色を変更する</div>
			</div>
		</dd>
		</dl>
	</div>
	<div id="remoconsetting_typeas_4" class="remoconsetting_typeas_select">
		<dl>
		<dt class="sub_head">フッターメニューボタン</dt>
		<dd>
			<div>
				<div class="idpass_name"  ><span class="cell">リモコンを編集の文言</span></div>
				<div class="idpass_input" ><span class="cell">
					<input type="text" id="remoconsetting_footermenu_remoconedit_string" placeholder="リモコンを編集">
				</span></div>
			</div>
			<br />
			<div class="filesample">
				<div class="fileicon">
					<img src="/user/elecicon/onoff.png" width="64" height="64" />
				</div>
				<div class="filename">
					<span id="remoconsetting_footermenu_remoconedit_icon"></span>
				</div>
				<div class="changebutton">
					<a href="javascript:void(0)" data-role="button" data-icon="arrow-u"  class="useicon"><span>リモコンを編集のアイコンを変更する</span></a>
				</div>
			</div>
			<br />
			<div>
				<div class="idpass_name"  ><span class="cell">編集画面に行くの文言</span></div>
				<div class="idpass_input" ><span class="cell">
					<input type="text" id="remoconsetting_footermenu_setting_string" placeholder="編集画面に行く">
				</span></div>
			</div>
			<br />
			<div class="filesample">
				<div class="fileicon">
					<img src="/user/elecicon/onoff.png" width="64" height="64" />
				</div>
				<div class="filename">
					<span id="remoconsetting_footermenu_setting_icon"></span>
				</div>
				<div class="changebutton">
					<a href="javascript:void(0)" data-role="button" data-icon="arrow-u"  class="useicon"><span>編集画面に行くのアイコンを変更する</span></a>
				</div>
			</div>
		</dd>
		</dl>
	</div>
	<div id="remoconsetting_typeas_9" class="remoconsetting_typeas_select">
		<dl>
		<dt class="sub_head">フッターメニューボタンの色</dt>
		<dd>
			<div class="colorpicker">
				<input type="hidden" id="remoconsetting_footermenu_font_color" />
				<div class="colorpicker_box"></div>
				<div class="colorpicker_string">ボタンの文字色を変更する</div>
			</div>
			<br />
			<div class="colorpicker">
				<input type="hidden" id="remoconsetting_footermenu_background_color"  class="remoconsetting_footermenu_unhover" />
				<div class="colorpicker_box"></div>
				<div class="colorpicker_string">ボタンの背景色を変更する</div>
			</div>
			<br />
			<div class="colorpicker">
				<input type="hidden" id="remoconsetting_footermenu_border_color"  class="remoconsetting_footermenu_unhover" />
				<div class="colorpicker_box"></div>
				<div class="colorpicker_string">ボタンのボーダーを変更する</div>
			</div>
		</dd>
		</dl>
	</div>
	<div id="remoconsetting_typeas_5" class="remoconsetting_typeas_select">
		<dl>
		<dt class="sub_head">フッターメニューボタンのホバー</dt>
		<dd>
			<div class="colorpicker">
				<input type="hidden" id="remoconsetting_footermenu_hover_color" class="remoconsetting_footermenu_hover" />
				<div class="colorpicker_box"></div>
				<div class="colorpicker_string">ボタンのホバー色を変更する</div>
			</div>
		</dd>
		</dl>
	</div>
	<div id="remoconsetting_typeas_6" class="remoconsetting_typeas_select">
		<dl>
		<dt class="sub_head">メニューダイアログ全般</dt>
		<dd>
			<div>
				<div class="idpass_name"  ><span class="cell">メニューダイアログを閉じる文言</span></div>
				<div class="idpass_input" ><span class="cell">
					<input type="text" id="remoconsetting_dialog_close_string" placeholder="close">
				</span></div>
			</div>
			<br />
			<div class="colorpicker">
				<input type="hidden" id="remoconsetting_dialog_font_color" />
				<div class="colorpicker_box"></div>
				<div class="colorpicker_string">ダイアログのフォント色</div>
			</div>
			<br />
			<div class="colorpicker">
				<input type="hidden" id="remoconsetting_dialog_background_color" />
				<div class="colorpicker_box"></div>
				<div class="colorpicker_string">ダイアログの背景色</div>
			</div>
			<br />
			<div class="colorpicker">
				<input type="hidden" id="remoconsetting_dialog_border_color" />
				<div class="colorpicker_box"></div>
				<div class="colorpicker_string">ダイアログのボーダー色</div>
			</div>
		</dd>
		</dl>
	</div>
	<div id="remoconsetting_typeas_10" class="remoconsetting_typeas_select">
		<dl>
		<dt class="sub_head">メニューダイアログ挙動</dt>
		<dd>
			<div>
				<select id="remoconsetting_dialog_donot_autoclose" size="1">
				<option value="0">ボタンを選択したら自動的に閉じる</option>
				<option value="1">ボタンを選択しても自動的に閉じない</option>
				</select>
				<br />
				<div align="right"><div align="left" class="papertag green">
					メニューダイアログから「電気つける」などのボタンを選択した時に、メニューダイアログを閉じるかどうかをきめます。<br />
					ディフォルトは、選択されると自動的に閉じます。<br />
				</div></div>
			</div>
			<br />
			<div>
				<select id="remoconsetting_dialog_iconcount_pc" size="1">
				<option value="4">ボタンを4つごとに並べる</option>
				<option value="5">ボタンを5つごとに並べる</option>
				<option value="6">ボタンを6つごとに並べる</option>
				<option value="7">ボタンを7つごとに並べる</option>
				<option value="8">ボタンを8つごとに並べる</option>
				</select>
				<br />
				<div align="right"><div align="left" class="papertag red">
					メニューダイアログの「つける」「けす」などのボタンを横方向にいくつ並べるのか指定します。<br />
				</div></div>
			</div>
		</dd>
		</dl>
	</div>
	<div id="remoconsetting_typeas_7" class="remoconsetting_typeas_select">
		<dl>
		<dt class="sub_head">メニューダイアログのボタン</dt>
		<dd>
			<div class="colorpicker">
				<input type="hidden" id="remoconsetting_button_font_color" />
				<div class="colorpicker_box"></div>
				<div class="colorpicker_string">ボタンの文字色を変更する</div>
			</div>
			<br />
			<div class="colorpicker">
				<input type="hidden" id="remoconsetting_button_background_color1"   class="remoconsetting_footermenu_unhover"/>
				<div class="colorpicker_box"></div>
				<div class="colorpicker_string">ボタンの背景色を変更する(グラデーション開始)</div>
			</div>
			<br />
			<div class="colorpicker">
				<input type="hidden" id="remoconsetting_button_background_color2"  class="remoconsetting_footermenu_unhover" />
				<div class="colorpicker_box"></div>
				<div class="colorpicker_string">ボタンの背景色を変更する(グラデーション終了)</div>
			</div>
			<br />
			<div class="colorpicker">
				<input type="hidden" id="remoconsetting_button_border_color"   class="remoconsetting_footermenu_unhover"/>
				<div class="colorpicker_box"></div>
				<div class="colorpicker_string">ボタンのボーダーを変更する</div>
			</div>
		</dd>
		</dl>
	</div>
	<div id="remoconsetting_typeas_8" class="remoconsetting_typeas_select">
		<dl>
		<dt class="sub_head">メニューダイアログのボタンのホバー</dt>
		<dd>
			<div class="colorpicker">
				<input type="hidden" id="remoconsetting_button_hover_color1" class="remoconsetting_footermenu_hover" />
				<div class="colorpicker_box"></div>
				<div class="colorpicker_string">ボタンのホバー色を変更する(グラデーション開始)</div>
			</div>
			<br />
			<div class="colorpicker">
				<input type="hidden" id="remoconsetting_button_hover_color2" class="remoconsetting_footermenu_hover" />
				<div class="colorpicker_box"></div>
				<div class="colorpicker_string">ボタンのホバー色を変更する(グラデーション終了)</div>
			</div>
			<br />
			<div class="colorpicker">
				<input type="hidden" id="remoconsetting_button_hover_border_color" class="remoconsetting_footermenu_hover" />
				<div class="colorpicker_box"></div>
				<div class="colorpicker_string">ボタンのホバー色をボーダー色を変更する</div>
			</div>
		</dd>
		</dl>
	</div>
	<div id="remoconsetting_sample_1" class="remoconsetting_sample">
		<div>SAMPLE</div>
		<div class="remoconcontroll_ui_sample">
			<div class="remoconcontroll_ui_sample_zoom">
				<div class="remoconcontroll_ui_contents_wrapper">
					<div class="remoconcontroll_ui_header">
						<fieldset class="remoconcontroll_ui_navigationbar">
							<div class="navigationbar_icon"><img src="/user/remocon/room_icon1.png" /></div>
							<div class="navigationbar_title"><span class="navigationbar_title_span">リモコン</span></div>
							<div class="navigationbar_sensor"><span class="sensor_name_temp">温度　:</span><span class="sensor_value">--</span><br /><span class="sensor_name_lumi">明るさ:</span><span class="sensor_value">--</span></div>
						</fieldset>
					</div>
					<ul class="remoconcontroll_ui_remocon_remoconmenu_ul">
							<li class="item itemshow">
									<div class="mainicon"><img src="/user/elecicon/elec.png" width="120" height="120" /></div>
									<div class="type">照明</div>
									<div class="status_label" style="color:#ff4500;"><span class="status_jyotai">状態:</span><span class="status">つける</span></div>
									<div class="actionmenuicon"></div>
							</li>
							<li class="item itemshow">
									<div class="mainicon"><img src="/user/elecicon/aircon.png" width="120" height="120" /></div>
									<div class="type">エアコン</div>
									<div class="status_label" style="color:#4169e1;"><span class="status_jyotai">状態:</span><span class="status">冷房普通</span></div>
									<div class="actionmenuicon"></div>
									<div class="timericon"></div>
							</li>
							<li class="item itemshow">
									<div class="mainicon"><img src="/user/elecicon/fan.png" width="120" height="120" /></div>
									<div class="type">扇風機</div>
									<div class="status_label" style="color:#008000;"><span class="status_jyotai">状態:</span><span class="status">けす</span></div>
									<div class="actionmenuicon"></div>
							</li>
							<li class="item itemshow">
									<div class="mainicon"><img src="/user/elecicon/project.png" width="120" height="120" /></div>
									<div class="type">プロジェクタ</div>
									<div class="status_label" style="color:#ff4500;"><span class="status_jyotai">状態:</span><span class="status">つける</span></div>
									<div class="actionmenuicon"></div>
							</li>
					</ul>
					<br class="clearfix">
					<br />
					<div class="remoconcontroll_ui_footermenu" >
						<div class="remoconcontroll_ui_footermenu_buton">
							<div class="footermenu_buton_icon_remoconedit"><img src="/user/remocon/remoconedit_icon1.png" /></div>
							<div class="footermenu_buton_name_remoconedit">リモコンを編集</div>
						</div>
						<div class="remoconcontroll_ui_footermenu_buton">
							<div class="footermenu_buton_icon_setting"><img src="/user/remocon/setting_icon1.png" /></div>
							<div class="footermenu_buton_name_setting">設定画面を開く</div>
						</div>
					</div>
					<br class="clearfix">
				</div>
			</div>
		</div>
	</div>
	<div id="remoconsetting_sample_2" class="remoconsetting_sample">
		<div>SAMPLE</div>
		<div class="remoconcontroll_ui2_sample">
			<div class="remoconcontroll_ui2_sample_zoom">
				<div class="remoconcontroll_ui2_loadingwait_dialog">
					<div class="remoconcontroll_ui2_popupselect">
						<div class="pitembox">
							<div class="pmainicon"><img src="/user/elecicon/fan.png" width="120" height="120"></div>
							<div class="ptype">扇風機</div>
							<div class="pstatus" style="color: #008000;">けす</div>
							<div class="pclose buttoncolor" >close</div>
						</div>
						<ul class="popupselect_ul">
							<li class="popupselect_li buttoncolor itemshow">
								<span class="actiontype" style="color: #008000;">けす</span>
							</li>
							<li class="popupselect_li buttoncolor itemshow">
								<span class="actiontype" style="color: #ff4500;">つける</span>
							</li>
						</ul>
					</div>
				</div>
			</div>
		</div>
	</div>

	<p><a data-role="button" id="remoconsetting_apply_button" data-icon="arrow-l" data-iconpos="left">設定する</a></p>
	<div class="error bunner_error">エラーが発生しました!!</div>
	</div><!-- /content -->
</div><!-- /page remoconsetting -->

<div data-role="page" id="version" data-theme="c" data-url="version" tabindex="0">
<div data-role="header" class="header_pc"> 
	<h1><a data-role="none" href="#setting" ><img src="/jscss/images/fhc700.png" /></h1> 
</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a data-role="button" class="menubutton" data-inline="true" href="/edit/#setting" data-ajax="false" data-icon="back"><span>設定</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_version.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">バージョン情報</span></div>
		</fieldset>
	</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">	
	<div class="error bunner_error">エラーが発生しました!!</div>

	<dl>
	<dt class="sub_head">バージョン情報</dt>
	<dd>
		<span id="version_versionid"></span><br>
	</dd>
	<dt class="sub_head">アップデートURL</dt>
	<dd>
		<input type="text" id="version_updateurl" value=""><br />
		<a data-role="button" id="version_update_button" >アップデートする</a>
	</dd>
	</dl>

	<dl>
	<dt class="sub_head">ログとサポート</dt>
	<dd>
		<div>
			過去のログを表示しています。<br />
		</div>
		<textarea id="version_log_textarea" rows=30 style="resize: none !important;  height: 400px !important; min-height:400px !important;  max-height:400px !important; ">ログを読み込んでいます</textarea>
		<div id="version_log_textarea_autoupdate_div"><span class="start_text_marker">自動更新中。</span> 1秒ごとにログを自動更新しています。</div>
		<div id="version_log_textarea_updatetop_div"><span class="stop_text_marker">更新停止中。</span>ログのボックスから選択を外すと更新を再度開始します。</div>
		<a data-role="button" id="version_suport_log" >サポート用のログを生成する</a>
	</dd>
	</dl>

	<dl>
	<dt class="sub_head">設定のインポート/エクスポート</dt>
	<dd>
		<fieldset>
			<div class="posstion_50">
				<a data-role="button" id="version_export_button" >設定の保存(エクスポート)</a>
			</div>
			<div class="posstion_50">
				<a data-role="button" id="version_import_button" >設定の読込(インポート)</a>
				<span class="hidden">
					<input type="file" id="version_import_upload" name="version_import_upload" value="アップロード"></span>
				</span>
			</div>
		</fieldset>
		<br class="clearfix" />
	</dd>
	</dl>

	<dl>
	<dt class="sub_head">著作表示とスタッフロール</dt>
	<dd>
		rti giken.　(2012/12-)<br>
		<br>
		以下のハードウェア/ソフトウェアを含んでいます<br>
		<ul class="version_bundle_software">
			<li>beaglebone</li>
			<li>Angstrom Linux</li>
			<li>julius</li>
			<li>openjtalk</li>
			<li>mecab</li>
			<li>V8</li>
			<li>mmdagent</li>
			<li>cmudict</li>
			<li>ICON HOIHOI</li>
			<li>レンタルサウンド</li>
			<li>festival</li>
			<li>liboauthcpp</li>
			<li>voxforge</li>
			<li>Personal-HomeKit-HAP</li>
			<li>C++11 and boost</li>
			<li>naichichi2</li>
		</ul>
		<br class="clearfix" />
		etc..(<a href="https://rti-giken.jp/staffroll.html"  data-ajax="false" target="_blank">全部を見るにはスタッフロールでどうぞ</a>)<br>
	</dd>
	</dl>
	<dt class="sub_head">OSVersion</dt>
	<dd>
		<span id="version_osversionid"></span><br>
	</dd>
	<dt class="sub_head">オペレーション</dt>
	<dd>
		<fieldset>
			<div class="posstion_50"><a data-role="button" id="version_reboot_button" >再起動する</a></div>
			<div class="posstion_50"><a data-role="button" id="version_shutdown_button" >シャットダウンする</a></div>
		</fieldset>
		<br class="clearfix" />
	</dd>
	</dl>
	<div class="error bunner_error">エラーが発生しました!!</div>
	</div><!-- /content -->
</div><!-- /page version -->

<div data-role="page" id="scriptselect" data-url="scriptselect" data-theme="c" class="">
	<div data-role="header" class="header_pc"> 
		<h1><a data-role="none" href="#setting" ><img src="/jscss/images/fhc700.png" /></h1> 
	</div>
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a data-role="button" class="menubutton" data-inline="true" href="/edit/#setting" data-ajax="false" data-icon="back"><span>設定</span></a></div>
			<div class="navigationbar_icon"><span class="scriptselect_is_script_scenario"><img src="/jscss/images/setting_icon64_script_scenario.png" /></span><span class="scriptselect_is_script_command"><img src="/jscss/images/setting_icon64_script_command.png" /></span></div>
			<div class="navigationbar_title"><span class="scriptselect_is_script_scenario">会話シナリオ選択</span><span class="scriptselect_is_script_command">コマンド設定選択</span></div>
		</fieldset>
	</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper" role="main">

		<div class="filelist_controll">
			<fieldset class="tr">
				<div class="th"><span>検索</span></div>
				<div class="td"><span><input type="text"  class="search" value="" placeholder="検索する文字列"></span></div>
			</fieldset>
			<input type="hidden" class="typepath" value="">
		</div><!-- /filelist_controll -->
		<ul class="filelist_li_sample js_template">

			<li class="filelist_li">
				<div class="fileicon"  >
					<img src="%ICON%" width="96" height="96" />
				</div>
				<div class="filename">%NAME%</div>
				<div class="filekey hidden">%KEY%</div>
				<span class="filesize">サイズ:<span>%SIZE%</span></span>
				<span class="filedate">日付:<span>%DATE%</span></span>
				<div class="fileusebutton" >
					<a href="javascript:void(0)" data-role="button"  class="check">編集する</a>
				</div>
				<div class="filedelbutton">
					<a href="javascript:void(0)" data-role="button"  class="delete">削除</a>
				</div>
			</li>

		</ul>
		<ul class="filelist_ul">
		</ul><!-- /filelist_ul -->

		<div><!-- new -->
			<div align="right">
				<a data-role="button" class="new" data-inline="true" data-icon="plus"><span>新しいスクリプトを新規作成</span></a>
			</div>
		</div>

	</div><!-- /content -->

</div><!-- /page scriptselect -->

<div data-role="page" id="scripteditor" data-url="scripteditor" data-theme="c" class="">
	<div data-role="header" class="header_pc"> 
		<h1><a data-role="none" href="#setting" ><img src="/jscss/images/fhc700.png" /></h1> 
	</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a data-role="button" class="menubutton" data-inline="true" href="/edit/#setting" data-ajax="false" data-icon="back"><span>設定</span></a></div>
			<div class="navigationbar_icon"><span class="scripteditor_is_script_scenario"><img src="/jscss/images/setting_icon64_script_scenario.png" /></span><span class="scripteditor_is_script_command"><img src="/jscss/images/setting_icon64_script_command.png" /></span></div>
			<div class="navigationbar_title"><span class="scripteditor_is_script_scenario">会話シナリオ編集</span><span class="scripteditor_is_script_command">コマンド設定編集</span></div>
		</fieldset>
	</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper" role="main">

		<div class="controll" style="width:100%; height: 100%;">
			<div style="float: left;" data-role="controlgroup" data-type="horizontal">
				<input type="hidden" class="typepath" value="">
				<span  class="filename"></span>
				<input type="button" class="run" value="セーブして実行">
				<a href="javascript:history.back()" data-role="button" class="back"><span>戻る</span></a>
			</div>
			<br class="clearfix" />
			<div class="scriptnotify">
				<div class="script_success_marker">正常に実行されました</div>
				<pre class="stdoutmessage"></pre>
			</div>
			<div class="scripterror">
				<div class="script_error_marker">スクリプトエラーが発生しました</div>
				<pre class="errormessage"></pre>
				<pre class="stdoutmessage"></pre>
			</div>
			<div style="width:100%;height: 100%;">
				<textarea class="filecontexnt" style="width:100%; height: 100%;" ></textarea>
			</div>
		</div><!-- /controll -->

		<span class="scripteditor_is_script_scenario">
			<div align="right"><br /><div  align="left" class="papertag purple">
				<a href="https://rti-giken.jp/fhc/help/script/" target="_blank">APIについては、プログラミングリファレンスをご覧ください。</a><br />
				<br />
			</div></div>
		</span>
		<span class="scripteditor_is_script_command">
			<div align="right"><br /><div  align="left" class="papertag red">
				<a href="https://rti-giken.jp/fhc/help/script/" target="_blank">APIについては、プログラミングリファレンスをご覧ください。</a><br />
				<br />
			</div></div>
		</span>

	</div><!-- /content -->
</div><!-- /page scripteditor -->

<div data-role="dialog" data-url="commandrundialog" id="commandrundialog" data-external-page="true" style="min-height: 619px; ">
	<div role="dialog" >
		<div data-role="content" data-theme="c"  role="main">	
		<dl>
			<dt class="sub_head">コマンド引数ダイアログ</dt>
			<dd>
				<input type="hidden" id="exec_command" value=""  />
				<span id="exec_command_display"></span>
			</dd>
			<dt><span id="exec_command_args1_mustlabel"></span><span id="exec_command_args1_label" class="command_args_text1">未定義の引数1</span></dt>
			<dd>
				<input type="text" id="exec_command_args1" value="">
			</dd>
			<dt><span id="exec_command_args2_mustlabel"></span><span id="exec_command_args2_label" class="command_args_text2">未定義の引数2</span></dt>
			<dd>
				<input type="text" id="exec_command_args2" value="">
			</dd>
			<dt><span id="exec_command_args3_mustlabel"></span><span id="exec_command_args3_label" class="command_args_text1">未定義の引数3</span></dt>
			<dd>
				<input type="text" id="exec_command_args3" value="">
			</dd>
			<dt><span id="exec_command_args4_mustlabel"></span><span id="exec_command_args4_label" class="command_args_text2">未定義の引数4</span></dt>
			<dd>
				<input type="text" id="exec_command_args4" value="">
			</dd>
			<dt><span id="exec_command_args5_mustlabel"></span><span id="exec_command_args5_label" class="command_args_text1">未定義の引数5</span></dt>
			<dd>
				<input type="text" id="exec_command_args5" value="">
			</dd>
		</dl>
		<a href="javascript:void(0)" id="exec_run_button">実行する</a>
		<a href="javascript:void(0)" data-rel="back">戻る</a>

		</div><!-- /content -->
	</div><!-- /dialog-->
</div><!-- /commandrundialog -->

<div data-role="page" id="webapi" data-theme="c" data-url="webapi" tabindex="0">
<div data-role="header" class="header_pc"> 
	<h1><a data-role="none" href="#setting" ><img src="/jscss/images/fhc700.png" /></h1> 
</div> 
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a data-role="button" class="menubutton" data-inline="true" href="/edit/#setting" data-ajax="false" data-icon="back"><span>設定</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_webapi.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">WebAPI</span></div>
		</fieldset>
	</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">	

		<span class="scripteditor_is_script_command">
			<div align="right"><div  align="left" class="papertag red">
				<p>WebAPIを利用すると、LAN内の外部プログラムから、フューチャーホームコントローラーの機能を呼び出すことができます。<br />
				家電と連携するアプリケーションを作成できます。<br />
				<br />
				<a href="https://rti-giken.jp/fhc/help/ref/setting_webapi.html" target="_blank">WebAPIの詳細については、WebAPIリファレンスをご覧ください。</a><br />
				</p>
			</div></div>
		</span>
	<dl>
	<dt class="sub_head">家電名一覧の取得</dt>
	<dd>
		<h3>設定されている家電名一覧を返します。</h3>
		<br />
		<input type="button" id="webapi_elec_getlist_button" value="webapi経由で呼び出してみる" />
		<br />
			<div align="right"><div  align="left" class="papertag green">
				<p>設定されている家電名一覧を配列で返します。<br />
				引数: なし<br />
				戻り値例: { "result": "ok","list": ["エアコン","テレビ","扇風機"] }<br />
				</p>
			</div></div>
	</dd>
	<dt class="sub_head">家電のアクションの取得</dt>
	<dd>
		<h3>設定されている家電のアクション動作一覧を返します。</h3>
		<br />
		<div class="table100">
			<div class="tr">
				<div class="th">家電名:</div>
				<div class="td">
					<select id="webapi_elec_forgetactionlist" size="1">
					</select>
				</div>
			</div>
		</div>
		<input type="button" id="webapi_elec_getactionlist_button" value="webapi経由で呼び出してみる" />
		<br />
			<div align="right"><div  align="left" class="papertag red">
				<p>設定されている家電のアクション動作一覧を配列で返します。<br />
				引数: 家電名<br />
				戻り値例: { "result": "ok", "list": ["つける","けす"] } <br />
				</p>
			</div></div>
	</dd>
	<dt class="sub_head">家電の情報の取得</dt>
	<dd>
		<h3>設定されている家電の詳細情報を連想配列で返します。</h3>
		<br />
		<div class="table100">
			<div class="tr">
				<div class="th">家電名:</div>
				<div class="td">
					<select id="webapi_elec_forgetinfo" size="1">
					</select>
				</div>
			</div>
		</div>
		<input type="button" id="webapi_elec_getinfo_button" value="webapi経由で呼び出してみる" />
		<input type="button" id="webapi_remote_elec_getinfo_button" value="remote webapi経由で呼び出してみる" />
		<br />
			<div align="right"><div  align="left" class="papertag yellow">
				<p>設定されている家電の詳細情報を連想配列で返します。<br />
				引数: 家電名<br />
				戻り値例: {"result","ok","status": "つける","type": "エアコン","elecicon": "satellite.png" .... }<br/>
				<br/>
				webapiは、ローカルIPに対して動作させます。<br/>
				remote webapiは、インターネット経由で動作させます。<br/>
				</p>
			</div></div>
	</dd>
	<dt class="sub_head">家電に関するすべての情報の取得</dt>
	<dd>
		<h3>家電について設定されている詳細データを取得します</h3>
		<br />
		<input type="button" id="webapi_elec_detail_button" value="webapi経由で呼び出してみる" />
		<br />
			<div align="right"><div  align="left" class="papertag purple">
				<p>設定すべての家電の情報を連想配列で返します。<br />
				引数: なし<br />
				</p>
			</div></div>
	</dd>
	<dt class="sub_head">家電操作</dt>
	<dd>
		<h3>家電に指定した動作をとらせることができます</h3>
		<br />
		<br />
		<div class="table100">
			<div class="tr">
				<div class="th">家電名:</div>
				<div class="td">
					<select id="webapi_elec_foraction" size="1">
					</select>
				</div>
			</div>
			<div class="tr">
				<div class="th">アクション名:</div>
				<div class="td">
					<select id="webapi_action_foraction" size="1">
					</select>
				</div>
			</div>
		</div>
		<br />
		<input type="button" id="webapi_elec_action_button" value="webapi経由で呼び出してみる" />
		<input type="button" id="webapi_remote_elec_action_button" value="remote webapi経由で呼び出してみる" />
		<br />
			<div align="right"><div  align="left" class="papertag green">
				<p>webAPI経由で家電を操作します。<br />
				引数: 家電名,アクション名<br />
				戻り値: { "result": "ok" }<br />
				<br/>
				webapiは、ローカルIPに対して動作させます。<br/>
				remote webapiは、インターネット経由で動作させます。<br/>
				</p>
			</div></div>
	</dd>

	<dt class="sub_head">センサー</dt>
	<dd>
		<h3>センサーの値を取得します。</h3>
		<br />
		<div class="table100">
			<div class="tr">
				<div class="th">温度センサー</div>
				<div class="td">
					<span id="webapi_sensor_temp"></span>℃
				</div>
			</div>
			<div class="tr">
				<div class="th">光センサー</div>
				<div class="td">
					<span id="webapi_sensor_lumi"></span>
				</div>
			</div>
		</div>

		<input type="button" id="webapi_sensor_button" value="webapi経由で呼び出す" />
		<br />
			<div align="right"><div  align="left" class="papertag red">
				<p>センサーの値を連想配列で返します。<br />
				引数: なし<br />
				戻り値: { "result": "ok", "temp": "22","lumi": "176",<br />
				"tempday": {"1354887209": 0, ... },"lumiday": {"1354887209": 0, ... } ...}<br />
				</p>
			</div></div>
	</dd>

	<dt class="sub_head">音声読上げ</dt>
	<dd>
		<h3>合成音声で読み上げを行います。</h3>
		<br />
		<div class="table100">
			<div class="tr">
				<div class="th">文字列</div>
				<div class="td">
					<input type="text" id="webapi_speak_str" value="はろーわーるど" size=40>
				</div>
			</div>
			<div class="tr">
				<div class="th">読み上げない</div>
				<div class="td">
					<select id="webapi_speak_noplay" size="1">
						<option value="0">音声で読み上げる</option>
						<option value="1">音声で読み上げない(キャッシュだけを生成する)</option>
					</select>
				</div>
			</div>
			<div class="tr">
				<div class="th">同期/非同期</div>
				<div class="td">
					<select id="webapi_speak_async" size="1">
						<option value="0">同期(読み上げが終わるまで応答を返さない)</option>
						<option value="1">非同期(すぐに応答を返し、裏側で読み上げさせる)</option>
					</select>
				</div>
			</div>
		</div>
		<input type="button" id="webapi_speak_button" value="webapi経由で呼び出す" />
		<br />
			<div align="right"><div  align="left" class="papertag red">
				<p>合成音声で文字列を読み上げます。<br />
				キャッシュにない初めて読み上げる音声は、合成音を生成するため数秒の時間がかかります。<br />
				<br />
				引数: 読みあげたい文字列,読み上げるかどうかのフラグ,同期非同期フラグ<br />
				戻り値: { "result": "ok" }<br />
				</p>
			</div></div>
	</dd>

	<dt class="sub_head">音楽ファイルの再生</dt>
	<dd>
		<h3>登録されている音楽ファイルを再生します。</h3>
		<br />
		<div class="table100">
			<div class="tr">
				<div class="th">ファイル名</div>
				<div class="td">
					<input type="text" id="webapi_play_name" value="hello.mp3" placeholder="hello.mp3" size=40>
				</div>
				<div class="td">
					<div class="changebutton">
						<a href="javascript:void(0)" id="webapi_play_mp3_select_button" data-role="button" data-icon="arrow-u"  class="useicon"><span>ファイルを選択する</span></a>
					</div>
				</div>
			</div>
			<div class="tr">
				<div class="th">同期/非同期</div>
				<div class="td">
					<select id="webapi_play_async" size="1">
						<option value="0">同期(読み上げが終わるまで応答を返さない)</option>
						<option value="1">非同期(すぐに応答を返し、裏側で読み上げさせる)</option>
					</select>
				</div>
			</div>
		</div>

		<input type="button" id="webapi_play_button" value="webapi経由で呼び出す" />
		<br />
			<div align="right"><div  align="left" class="papertag red">
				<p>登録されている音楽ファイルを再生します。<br />
				<br />
				引数: 読みあげたいファイル名,同期非同期フラグ<br />
				戻り値: { "result": "ok" }<br />
				</p>
			</div></div>
	</dd>

	<dt class="sub_head">登録されている音声認識ワード</dt>
	<dd>
		<h3>登録されている音声認識ワード一覧を返します。</h3>
		<br />

		<input type="button" id="webapi_recong_list_button" value="webapi経由で呼び出す" />
		<br />
			<div align="right"><div  align="left" class="papertag red">
				<p>登録されている音声認識ワード一覧を返します。<br />
				<br />
				引数: なし<br />
				戻り値: { "result": "ok" , list: ["でんきけして","でんきつけて" ... ] }<br />
				</p>
			</div></div>
	</dd>

	<dt class="sub_head">音声認識ワードの文字列呼び出し</dt>
	<dd>
		<h3>登録されている音声認識ワードを文字列で呼び出します。</h3>
		<br />
		<div class="table100">
			<div class="tr">
				<div class="th">文字列</div>
				<div class="td">
					<input type="text" id="webapi_recong_firebystring_str" placeholder="でんきつけて" size=40 x-webkit-speech>
				</div>
			</div>
		</div>

		<input type="button" id="webapi_recong_firebystring_button" value="webapi経由で呼び出す" />
		<br />
			<div align="right"><div  align="left" class="papertag red">
				<p>登録されている音声認識ワードを文字列で呼び出します。<br />
				(呼びかけはあってもなくても、どちらでも動作します。)<br />
				<br />
				引数: 実行したい音声認識文字列<br />
				戻り値: { "result": "ok" }<br />
				</p>
			</div></div>
	</dd>


	<dt class="sub_head">環境情報</dt>
	<dd>
		<h3>フューチャーホームコントローラーが動作している、バージョン情報、モデル型番、動作OSを取得します。</h3>
		<br />

		<input type="button" id="webapi_env_button" value="webapi経由で呼び出す" />
		<br />
			<div align="right"><div  align="left" class="papertag yellow">
				<p>環境情報を取得します。<br />
				<br />
				引数: なし<br />
				戻り値: { "result": "ok","version": "バージョン番号","model": "FHCのモデル型番","os": "動作OS" }<br />
				</p>
			</div></div>
	</dd>

	<dt class="sub_head">API KEY</dt>
	<dd>
		<h3>APIを実行するためのキー情報です。</h3>
		<br />
		<br />
		<div>
			<span id="webapi_apikey" style="font-size: 1.6em;"></span>
		</div>
		<br />
		APIキーは大切に保管してください。<br />
		<br />
		<input type="button" id="webapi_keyreset_button" value="キーをリセットする" />
	</dd>

	</dl>
	</div><!-- /content -->
</div><!-- /page webapi -->

<div data-role="page" id="setting" data-theme="c" data-url="setting" tabindex="0">
	<div data-role="header" class="header_pc"> 
		<h1><a data-role="none" href="#top" ><img src="/jscss/images/fhc700.png" /></h1> 
	</div> <!-- /header -->
	<div data-role="header" class="header_title">
		<fieldset class="navigationbar">
			<div class="navigationbar_backbutton"><a href="/remocon/" data-role="button" data-ajax="false" data-icon="back"><span>リモコン</span></a></div>
			<div class="navigationbar_icon"><img src="/jscss/images/setting_icon64_setting.png" /></div>
			<div class="navigationbar_title"><span class="navigationbar_title_span">設定</span></div>
		</fieldset>
	</div> 

	<div data-role="content" class="contents_wrapper" role="main">
	<p>
	<div class="setting_buton" onclick="mictest_load();">
		<div class="setting_buton_icon"><img src="/jscss/images/setting_icon64_mictest.png" /></div>
		<div class="setting_buton_name">マイクテストを行う</div>
		<div class="setting_buton_hosuku">音声認識ログから精度改善のアドバイスを行います。</div>
		<div class="clearfix"></div>
	</div>
	</p>

	<p>
	<div class="setting_buton" onclick="recong_load();">
		<div class="setting_buton_icon"><img src="/jscss/images/setting_icon64_recong.png" /></div>
		<div class="setting_buton_name">音声認識設定を行う</div>
		<div class="setting_buton_hosuku">音声認識の精度やマイクの音量を調整します。</div>
		<div class="clearfix"></div>
	</div>
	<div class="setting_buton" onclick="speak_load();">
		<div class="setting_buton_icon"><img src="/jscss/images/setting_icon64_speak.png" /></div>
		<div class="setting_buton_name">合成音声設定を行う</div>
		<div class="setting_buton_hosuku">文字読み上げの音量調節を行います。</div>
		<div class="clearfix"></div>
	</div>
	<div class="setting_buton" onclick="sensor_load();">
		<div class="setting_buton_icon"><img src="/jscss/images/setting_icon64_sensor.png" /></div>
		<div class="setting_buton_name">センサー設定を行う</div>
		<div class="setting_buton_hosuku">センサーの値を見ることができます。</div>
		<div class="clearfix"></div>
	</div>
	
	<div class="setting_buton" onclick="trigger_load();">
		<div class="setting_buton_icon"><img src="/jscss/images/setting_icon64_trigger.png" /></div>
		<div class="setting_buton_name">トリガーの設定を行う</div>
		<div class="setting_buton_hosuku">ある条件が発生した時に自動実行することできます。</div>
		<div class="clearfix"></div>
	</div>

	<div class="setting_buton" onclick="remoconsetting_load();">
		<div class="setting_buton_icon"><img src="/jscss/images/setting_icon64_remoconsetting.png" /></div>
		<div class="setting_buton_name">リモコンUI設定</div>
		<div class="setting_buton_hosuku">リモコン画面をカスタマイズします。</div>
		<div class="clearfix"></div>
	</div>

	<div class="setting_buton" onclick="system_load();">
		<div class="setting_buton_icon"><img src="/jscss/images/setting_icon64_system.png" /></div>
		<div class="setting_buton_name">システム全般の設定を行う</div>
		<div class="setting_buton_hosuku">システム全般の様々な設定を行います。</div>
		<div class="clearfix"></div>
	</div>
	<div class="setting_buton" onclick="version_load();">
		<div class="setting_buton_icon"><img src="/jscss/images/setting_icon64_version.png" /></div>
		<div class="setting_buton_name">バージョン情報を見る</div>
		<div class="setting_buton_hosuku">バージョン情報の確認とアップデートを行えます。</div>
		<div class="clearfix"></div>
	</div>
	</p>

	<p>
	<div class="setting_buton" onclick="webapi_load();">
		<div class="setting_buton_icon"><img src="/jscss/images/setting_icon64_webapi.png" /></div>
		<div class="setting_buton_name">webapi設定を行う</div>
		<div class="setting_buton_hosuku">他のアプリから操作するインターフェースを設定します。</div>
		<div class="clearfix"></div>
	</div>
	<div class="setting_buton" onclick="scriptselect_load('script_scenario');">
		<div class="setting_buton_icon"><img src="/jscss/images/setting_icon64_script_scenario.png" /></div>
		<div class="setting_buton_name">会話シナリオ設定を行う</div>
		<div class="setting_buton_hosuku">複雑な受け答えのプログラムを作成します。(上級者用)</div>
		<div class="clearfix"></div>
	</div>
	<div class="setting_buton" onclick="scriptselect_load('script_command');">
		<div class="setting_buton_icon"><img src="/jscss/images/setting_icon64_script_command.png" /></div>
		<div class="setting_buton_name">コマンド設定を行う</div>
		<div class="setting_buton_hosuku">外部機材を操作するプログラムを作成します。(上級者用)</div>
		<div class="clearfix"></div>
	</div>
	</p>

	</div><!-- /content -->


</div><!-- /page top -->
<script>
var g_Strings = {
	 mukou:		"無効"
	,yukou:		"有効"
	,jikan:		"時間"
	,week0:		"日"
	,week1:		"月"
	,week2:		"火"
	,week3:		"水"
	,week4:		"木"
	,week5:		"金"
	,week6:		"土"
	,syukujitu_wo_nozoku:	"祝日を除く"
	,syukujitu_nomi:		"祝日のみ"
	,command:	"コマンド"
	,error_ga:	"エラーが発生しました。"
	,error:		"エラー"
	,result:	"戻り値"
	,stdout:	"標準出力"
	,tokyo:		"東京都"
	,content_encode:	"コンテンツのエンコード"
	,content:	"コンテンツ"
	,click_de:	"クリックで表示します。"
	,regex_de:	"正規表現として検索しました。"
	,match_not:	"マッチしませんでした。"
	,match_done:"マッチしました。"
	,zenkaiyori:	"前回より"
	,notoki:	"の時"
	,igai:		"以外"
	,hitoga_irutoki:	"人がいるとき"
	,hitoga_inaitoki:	"人がいないとき"
	,tenki:			"天気"
	,amega_furu:	"雨が降る"
	,amega_denai:	"雨ではない"
	,hare:			"晴れている"
	,kumori:		"曇ってる"
	,mosugu_ame:	"もうすぐ雨が降る"
	,mosugu_ame_yamu:"もうすぐ雨がやむ"
	,mictest_manual: "解説マニュアルへ"
	,mictest_XX1_TEXT1:	"音声認識に成功しました。"
	,mictest_XX1_FIXIT1:"なんでもない会話を認識してしまった場合は、2パスチェックを検討してください。または、サポーター強度をもっと上げてください。"
	,mictest_XX1_FIXIT2:"なんでもない会話を認識してしまった場合は、サポーター強度をもっと上げてください。"
	,mictest_XX1_FIXIT3:"別の命令と取り違えた場合は、1次通過確率をもっと上げてください。"
	,mictest_SP_TEXT1: "失敗。回りが騒がしいようです。"
	,mictest_SP_FIXIT1: "回りに音源がある場合は、マイクから音源を遠ざけてください。"
	,mictest_SP_FIXIT2: "音源がない場合は、マイク端子を挿しなおしてください。半挿しの可能性があります。"
	,mictest_SP_FIXIT3: "それでも発生する場合は、マイクの音量をもっと下げてください。"
	,mictest_GY_TEXT1:	"失敗。1次フィルタの呼びかけサポーターで棄却しました。"
	,mictest_GY_FIXIT1:	"もう少し正確に発音してください。"
	,mictest_GY_FIXIT2:	"正確に発音しているのに、頻繁に表示される場合は、呼びかけのサポーター強度を下げてください。"
	,mictest_GY_FIXIT3:	"ただし、サポーター強度を下げすぎると、誤動作のリスクが高まりますので下げ過ぎには注意してください。"
	,mictest_GE_TEXT1:	"失敗。1次フィルタの命令サポーターで棄却しました。"
	,mictest_GE_FIXIT1:	"もう少し正確に発音してください。"
	,mictest_GE_FIXIT2:	"正確に発音しているのに、頻繁に表示される場合は、命令のサポーター強度を下げてください。"
	,mictest_GE_FIXIT3:	"ただし、サポーター強度を下げすぎると、誤動作のリスクが高まりますので下げ過ぎには注意してください。"
	,mictest_HW_TEXT1:	"ハードウェアエラーが発生しました。システムを自動再起動します。"
	,mictest_SW_TEXT1:	"ソフトウェアエラーが発生しました。"
	,mictest_PB_TEXT1:	"失敗。1次フィルタを通過できるだけの確率ではありません。"
	,mictest_PB_FIXIT1:	"もう少し正確に発音してください。"
	,mictest_PB_FIXIT2:	"発音した単語が正しければ、1次通過確率をもっと下げると動作するようになります。"
	,mictest_LG_TEXT1:	"失敗。発音がゆっくり過ぎます。"
	,mictest_LG_FIXIT1:	"もう少し早く発音してください。"
	,mictest_SH_TEXT1:	"失敗。発音が早すぎます。"
	,mictest_SH_FIXIT1:	"もう少しゆっくり発音してください。"
	,mictest_YO_TEXT1:	"失敗。音のデータが過剰に詰まっています。"
	,mictest_YO_FIXIT1:	"もう少し正確に発音してください。"
	,mictest_YO_FIXIT2:	"正確に発音しているのに多発する場合は、弊社までログを送ってください。"
	,mictest_FZ_TEXT1:	"失敗。音のデータが不正確です。"
	,mictest_TS_TEXT1:	"失敗。終端データにマッチしました。"
	,mictest_DD_TEXT1:	"失敗。2次フィルタのサポーターで棄却しました。"
	,mictest_DD_FIXIT1:	"もう少し正確に発音してください。"
	,mictest_DD_FIXIT2:	"正確に発音しているのに多発する場合は、2次フィルターのサポーター強度を下げてください。"
	,mictest_DD_FIXIT3:	"ただ、サポーター強度を下げすぎると、誤動作のリスクが高まりますので下げ過ぎには注意してください。"
	,mictest_DP_TEXT1:	"失敗。2次フィルタを通過できるだけの確率ではありません。"
	,mictest_DP_FIXIT1:	"もう少し正確に発音してください。"
	,mictest_DP_FIXIT2:	"正確に発音しているのに多発する場合は、2次フィルターの通過強度を下げてください。"
	,mictest_DS_TEXT1:	"失敗。2次フィルタで棄却。呼びかけの発音が早すぎます。"
	,mictest_DS_FIXIT1:	"もう少し早く正確に発音してください。"
	,mictest_DL_TEXT1:	"失敗。2次フィルタで棄却。呼びかけの発音が長過ぎます。"
	,mictest_DL_FIXIT1:	"もう少しゆっくり正確に発音してください。"
	,mictest_UN_TEXT1:	"「不明な」ソフトウェアエラーが発生しました。"
	,mictest_UN_FIXIT1:	"頻繁に起きるようでしたら、弊社までログを送ってください。"
	,NO_UPDATE_IS_REQUIRED:	"アップデートの必要はありません"
	,DATA_NOT_FOUND:	"指定されたデータが見つかりません。削除されている可能性があります。"
	,NEED_REBOOT:		"設定は、再起動後に有効になります。"
	,LANG_JA:			"日本語"
	,LANG_EN:			"英語"
	,ACTION_TIMER_SET:		"タイマーセット"
	,ACTION_TIMER_RESET:	"タイマー取り消し"
	,ACTION_RECOGN_PAUSE:	"音声認識一時停止"
	,ACTION_RECOGN_RESTART:	"音声認識再開"
	,ACTION_SIP_CALL:	"電話をかける"
	,ACTION_SIP_ANSWER:	"電話をとる"
	,ACTION_SIP_HANGUP:	"電話をきる"
	,ACTION_SIP_NUMPAD:	"電話パッドを表示"
	,SELECT_FHC_TERMINAL:	"操作するFHC端末"
	,SELECT_CONTROL:		"操作を選択してください"
	,SELECT_NO_CONTROL:		"操作を行わない"
	
	,SELECT_SPEAK_FILE:		"読み上げるファイルの選択"
	,SELECT_COMMAND_FILE:	"コマンドの選択"
	,SELECT_WATHER_COMMAND:	"天気予報コマンドの選択"
	,SELECT_TRAIN_COMMAND:	"電車遅延コマンドの選択"
};

var g_Remocon  = %REMOCON%;
var g_SETTING  = %SETTING%;
var g_Trigger  = %TRIGGER%;
var g_MultiRoom  = %MULTIROOM%;
var g_CSRFTOKEN = "%CSRFTOKEN%";

$(function(){
	//jquery mobileバグ対応のために消しているメニューを戻す.
	$(window).on("pagebeforechange", function (event, data) {
		if (data.toPage == "/edit/")
		{
	    	$('#remoconmenu_li_sample_content').show();
		}
	});

	//jquery mobile でリロードした時にデータが引き継がれない問題を強引に何とかする・・・(もっといい方法はあるはず)
	if ( window.location.href.match(/#(.+)/) )
	{
		switch(RegExp.$1)
		{
		case "setting":
			break;
		case "account":
			account_load();
			break;
		case "network":
			network_load();
			break;
		case "recong":
			recong_load();
			break;
		case "speak":
			speak_load();
			break;
		case "sensor":
			sensor_load();
			break;
		case "triggerset":
		case "triggerlist":
			trigger_load();
			break;
		case "system":
			system_load();
			break;
		case "version":
			version_load();
			break;
		case "webapi":
			webapi_load();
			break;
		case "mictest":
			mictest_load();
			break;
		case "changelang":
			changelang_load();
			break;
		case "homekit":
			homekit_load();
			break;
		case "multiroomlist":
			multiroomlist_load();
			break;
		case "multiroomappend":
			multiroomappend_load("");
			break;
		case "multiroomsearch":
			multiroomsearch_load();
			break;
		case "sip":
			sip_load();
			break;
		case "remoconsetting":
			remoconsetting_load();
			break;
		case "scripteditor":
			window.location.href = window.location.href.replace(/#.*$/,"#setting");
			break;
		case "index":
			remoconedit_init();
			break;
		default:
			//パラメーターが付くものは無理
			window.location.href = window.location.href.replace(/#.*$/,"");
			break;
		}
	}
	else
	{
		remoconedit_init();
	}
});
</script>
</body></html>