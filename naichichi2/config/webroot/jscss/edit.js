function change_gradient(obj,start,end,nograflag)
{
	if (jQuery.browser.msie )
	{
		obj.css("filter", "progid:DXImageTransform.Microsoft.gradient(GradientType=0,startColorstr="+start+", endColorstr="+end+");");
		obj.css("background","-ms-linear-gradient(top, "+start+", "+end+");");
	}
	else if (jQuery.browser.mozilla)
	{
		obj.css("background","-moz-linear-gradient(top, "+start+", "+end+");");
	}
	else if (jQuery.browser.chrome || jQuery.browser.safari || jQuery.browser.webkit )
	{
		obj.css("background","-webkit-gradient(linear, left top, left bottom, from("+start+"), to("+end+"))");
	}
	else if (jQuery.browser.opera )
	{
		obj.css("background","-o-linear-gradient(top, "+start+", "+end+");");
	}
	else
	{
		obj.css("background","linear-gradient(top, "+start+", "+end+");");
	}

	//グラデーションが適応できない場合の色
	if (nograflag == 1)
	{
		obj.css("background-color",start);
	}
	else if (nograflag == 2)
	{
		obj.css("background-color",end);
	}
}
function gotohref(link)
{
	window.location.href = link;
}

function typeotherSelect(type,typeother)
{
	if (type == "other") return typeother;
	return type;
}

function ismobile()
{
	return $(".contents_wrapper").width() == 640;
}

//見やすいサイズに変換
function sizescale(sizeString)
{
	var i = parseInt(sizeString);
//	if (i > 1000000000000) return parseInt(i / 1000000000000) + "TByte";
	if (i > 1000000000 ) return parseInt(i / 1000000000) + "GByte";
	if (i > 1000000) return parseInt(i / 1000000) + "MByte";
	if (i > 1000) return parseInt(i / 1000) + "KByte";
	return i + "Byte";
}

function getElecText(value,key1,key2)
{
	if (key2 == undefined)
	{
		var v = g_Remocon["elec_" + key1 + "_" + value ];
		if ( v == undefined )
		{
			return "";
		}
		return v;
	}

	var v = g_Remocon["elec_" + key1 + "_action_" + key2 + "_" + value ];
	if ( v == undefined )
	{
		return "";
	}
	return v;
}
function getElecBool(value,key1,key2)
{
	var v = "";
	if (key2 == undefined)
	{
		v = g_Remocon["elec_" + key1 + "_" + value ];
		if ( v == undefined )
		{
			return false;
		}
	}
	else
	{
		v = g_Remocon["elec_" + key1 + "_action_" + key2 + "_" + value ];
		if ( v == undefined )
		{
			return false;
		}
	}

	if (parseInt(v))
	{
		return true;
	}
	return false;
}

function parseIntOrNew(v)
{
	if (v == "new")
	{
		return "new";
	}
	return parseInt(v);
}
function nl2br(str)
{
	return str.replace(/(\r\n|\r|\n)/g,'$1' + '<br />' );
}
function h(ch) { 
	if (!ch) return "";
	ch = ch.replace(/&/g,"&amp;") ;
	ch = ch.replace(/"/g,"&quot;") ;
	ch = ch.replace(/'/g,"&#039;") ;
	ch = ch.replace(/</g,"&lt;") ;
	ch = ch.replace(/>/g,"&gt;") ;
	return ch ;
}
//長いテキストなので適当な所で省略する
function longText(str,clickText,showCount)
{
	str = h(str);
	var l = str.split("\n");
	var htmlsp="<div>";
	var i = 0;
	if ( l.length < showCount )
	{//省略なし
		for( ; i < l.length ; i++)
		{
			htmlsp += l[i] + "<br />";
		}
		return htmlsp;
	}

	for( ; i < showCount ; i++)
	{
		htmlsp += l[i] + "<br />";
	}
	

	var anonid = "longText_"+parseInt((new Date)/1000);
	htmlsp += "<a href=\"javascript:void(0)\" onclick=\"$('#" + anonid + "').toggle(); \">" + clickText + "</a><div id=\""+anonid+"\" style=\"display: none;\">";
	for( ; i < l.length ; i++)
	{
		htmlsp += l[i] + "<br />";
	}
	htmlsp += "</div></div>";
	return htmlsp;
}

//ajaxのエラー表示
function alertErrorDialog(ajaxReturn,functionName)
{
	if ( ajaxReturn['result'] == "error" )
	{
		alert("Error Function:" + functionName + "\r\n" + "error code: " + ajaxReturn['code'] + "\r\n" + "error message: " + ajaxReturn['message'] );
	}
	else
	{
		if (ajaxReturn.responseText)
		{
			alert("Error Function:" + functionName + "\r\nerror text:" + ajaxReturn.responseText);
		}
		else
		{
			alert("Error Function:" + functionName + "\r\ncan not respoons");
		}
	}
}
//selectタグでtargetを選択させるのだが、 targetが存在していない場合は、その他 を選択する.
function optionSelect(obj,target)
{
	var found = false;
	obj.find("option").each(function(){
		if ( $(this).attr('value') == target )
		{
			found = true;
			return false;
		}
	});
	if (found)
	{
		obj.val(target);
	}
	else
	{
		obj.val("other");
	}
	return obj;
}
function getindexOptionSelect(obj,target)
{
	var i = -1;
	obj.find("option").each(function(){
		i++;
		if ( $(this).attr('value') == target )
		{
			return false;
		}
	});
	return i;
}

//ソート listview (touch & 軽量化)
function sortableul(ulObj,options) 
{
    var isRectMatct = function (x, y, width, height, px, py, pwidth, pheight)
    {
        if ( ( x < px + pwidth ) && ( px < x + width ) && ( y < py + pheight ) && ( py < y + height ) ) {
            return true;
        }
        return false;
    }
    var dragSampleLiObj;
    //ドラッグ中にサンプルとして表紙する li要素
    var dragSpacerLiObj;
    //ドラッグ中に挿入位置に表示するスペーサー li要素
    var droplevel;
    var dragstart = function (div, x, y, width, height)
    {
        //      		div.css("background-color","yellow");
        droplevel = 1;
        //マウスに追従する見本を作る
        dragSampleLiObj = $("<div style=\"position:absolute; z-index: 99; opacity: 0.6; left: "+x+"px; top: "+ y +"px; \"></div>");
        dragSampleLiObj.append(div.clone(false));
        ulObj.append(dragSampleLiObj);

        //スペーサー要素
        dragSpacerLiObj = div.clone(false);
        dragSpacerLiObj.html("");
        div.before(dragSpacerLiObj);
        div.hide();
        
    }
    var dragmove = function (div, x, y, width, height)
    {
        dragSampleLiObj.css({left: x+"px",top: y+"px"});
        if (droplevel == 1) 
        {
            //スペーサーを作るぐらい接近した
            var spacerPos = dragSpacerLiObj.position();
            if ( isRectMatct(x, y, width, height, spacerPos.left, spacerPos.top, dragSpacerLiObj.width(), 
            dragSpacerLiObj.height()) ) {
                //スペーサーノードの上にいるみたい
                return ;
                //そのまま
            }
            //スペーサーノードから離れた
            //スペーサーノードを最後に移動して、スペーサーノードを入れる前の状態にする
            ulObj.append(dragSpacerLiObj);
            droplevel = 0;
        }
        //このパーツが入る要素を探します
        var founddiv = null;
        ulObj.children("li:visible").each(function ()
        {
            //スペーサーだったら無視
            if (dragSpacerLiObj.get(0) == this) {
                return true;
            }
            var obj = $(this);
            //それ以外だったら衝突判定します
            var pos = obj.position();
            var pwidth = obj.width();
            var pheight = obj.height();
            if ( isRectMatct(x, y, width, height, pos.left, pos.top, pwidth, pheight) ) {
                //match!
                founddiv = $(this);
                return false;
            }
        });
        if ( founddiv == null) {
            //どこにもマッチする要素はなかった
            return ;
        }
        //この要素の前にスペーサー要素を入れる
        founddiv.before(dragSpacerLiObj);
        droplevel = 1;
    }
    //http://sewa.se/drag/
    var dragend = function (div, x, y, width, height)
    {
        //      		div.css("background-color","red");
        dragSpacerLiObj.before(div);

        dragSampleLiObj.remove();
        dragSpacerLiObj.remove();
        div.show();
        options["update"] && options["update"](div);
    }
    //for mobile
    var start = function (e) 
    {
        var orig = e.originalEvent;
        var captureDiv = $(this);
        var pos = captureDiv.position();
        var offset = 
        {
            x : orig.changedTouches[0].pageX - pos.left , y : orig.changedTouches[0].pageY - pos.top , 
            width : captureDiv.width() , height : captureDiv.height() 
        };
        dragstart(captureDiv, pos.left, pos.top, offset.width, offset.height);
        
	    var moveMe = function (e) 
	    {
	        e.preventDefault();
	    	if (captureDiv == null) return ;

	        var orig = e.originalEvent;
	        dragmove(captureDiv, orig.changedTouches[0].pageX - offset.x, orig.changedTouches[0].pageY - offset.y, 
	        offset.width, offset.height);
	    };
	    var end = function (e) 
	    {
	    	if (captureDiv == null) return ;
	    	captureDiv.unbind("touchmove");
	    	captureDiv.unbind("touchend");
	    	captureDiv.unbind("touchcancel");
	        var orig = e.originalEvent;
	        dragend(captureDiv, orig.changedTouches[0].pageX - offset.x, orig.changedTouches[0].pageY - offset.y, 
	        offset.width, offset.height);
	        captureDiv = null;
	    }
		captureDiv.bind("touchmove", moveMe);
		captureDiv.bind("touchend", end);
		captureDiv.bind("touchcancel", end);
    };

    //http://saruzaurus.blogspot.jp/2008/06/jquerydraggables-httpallabout.html
    //for PC
    var mousedown = function (event)
    {
        var captureDiv = $(this);
        var pos = captureDiv.position();
        var eX = event.pageX - pos.left;
        var eY = event.pageY - pos.top;
        var width = captureDiv.width();
        var height = captureDiv.height();
        //子供の要素の上？
        var isChildElemnt = false;
        captureDiv.children("a").each(function(){
        	var childElemnt = $(this);
        	var childElemntPos = childElemnt.position();
			if(eX >= childElemntPos.left && eX <= childElemntPos.left+childElemnt.outerWidth()*1.3 && eY >= childElemntPos.top && eY <= childElemntPos.top+childElemnt.outerHeight()*1.3 ){
				isChildElemnt = true;
				return false;
			}
        });
		if (isChildElemnt) 
		{//子供のイベントを優先する
			return ;
		}
		
        event.preventDefault();
        dragstart(captureDiv, pos.left, pos.top, width, height);
        $(document).mousemove(function (event)
        {
            if (captureDiv == null) {
                return ;
            }
            dragmove(captureDiv, event.pageX - eX, event.pageY - eY, width, height);
        });
        $(document).mouseup(function (event)
        {
            $(document).unbind("mousemove");
            if (captureDiv == null) {
                return ;
            }
            dragend(captureDiv, event.pageX - eX, event.pageY - eY, width, height);
            captureDiv = null;
        });
    };
    
    //li要素を見てイベントを詰めていきます。
    ulObj.children("li").each(function(){
    	var captureDiv = $(this);
    	if ( captureDiv.data("is_sortable") != 1)
    	{
			captureDiv.bind("touchstart", start);
			captureDiv.bind("mousedown", mousedown) 
			captureDiv.data("is_sortable" , 1)
    	}
    });
    
};
function showhidden(obj,boolFlag)
{
	if ( boolFlag )
	{
		obj.show();
	}
	else
	{
		obj.hide();
	}
}
function macro_preview(obj,prefix)
{
	var arr = {};
	for(var i = 1 ; i <= 9 ; i ++)
	{
		arr["exec_macro_"+i+"_elec"] = obj.find("#" + prefix + "_"+i+"_elec").val();
		arr["exec_macro_"+i+"_action"] = obj.find("#" + prefix + "_"+i+"_action").val();
		arr["exec_macro_"+i+"_after"] = obj.find("#" + prefix + "_"+i+"_after").val();
	}
	arr["_csrftoken"] = g_CSRFTOKEN;

	showUpdatewaitDialog();
	$.ajax({
		type: "post",
		url: "/remocon/preview/elec_macro",
		timeout: 50000,
		data: arr,
		dataType: 'json',
		success: function(d){

			hideUpdatewaitDialog();
			if ( d['result'] != "ok" )
			{
				alertErrorDialog(d,"remocon_preview_elec_macro");
				return ;
			}
		},
		error: function(d){
			hideUpdatewaitDialog();
			return ;
		}
	});
}
function macro_set(obj,prefix)
{
	//マクロ
	for( var i = 1 ; i <= 9 ; i ++ )
	{
		obj.find("#" + prefix + "_" + i + "_elec").change(function(){
			var thisobj = $(this);
			var actionname = "#" + thisobj.prop("id").replace(/_elec$/g,"_action");
			make_macro_combo_action(true, thisobj.val() , obj.find(actionname));
			
			if ( obj.find(actionname+":visible").length != 0 )
			{
				//表示されている
				obj.find(actionname).selectmenu("refresh");
			}
		});
	}
}

//音声認識のリロード
function ReloadRecogn()
{
	var arr = {};
	arr['c'] = 'recogn';
	arr['arg1'] = 'true';

	update(
		null
		,"/remocon/recogn/reload"
		,arr
		,function(d){ 
		}
		,{ load: 'no'
			,timeout: 5000
			,error: function(){
			}
		 }
	);
}

//すべてを一つの処理でやると、linuxだとまれに落ちるらしい...意味がわからないので処理を分割する.
function TopLevelInvoke(object,url,updateArray,success_callback,options)
{
	var destructor_func = function()
	{
		update(
			null
			,"/remocon/toplevel/invoke"
			,{ "c": "end" }
			,function(d){ 
			}
			,{
			   timeout: 5000
			  ,error: function(d){
				alert("音声認識を再開できませんでした");
			}}
			);
	};

	update(
		null
		,"/remocon/toplevel/invoke"
		,{ "c": "start" }
		,function(d){ 
		
			var orignal_error = options.error;
			options.error = function(d){
				destructor_func();
				orignal_error && orignal_error(d);
			};
			
			//最優先実行したい処理
			update(object
				,url
				,updateArray
				,function(d){ 
					destructor_func();
					success_callback(d);
				}
				,options
			);
		}
		,{
			 timeout: 5000
			,error: function(d){
			  	alert("音声認識を止めようとしましたが、止められませんでした。")
			  	destructor_func();
		  }
		}
	);
}



function update(object,url,updateArray,success_callback,options)
{
	//読込中にする
	if(options && options.load)
	{
		if (options.load != 'no')
		{
			showUpdatewaitDialog({ htmlobject: options.load } );
		}
	}
	else
	{
		showUpdatewaitDialog();
	}
	
	var timeout = 50000;
	if (options && options.timeout)
	{
		timeout = options.timeout;
	}

	if (object)
	{
		object.find(".error").hide();
	}
	updateArray["_csrftoken"] = g_CSRFTOKEN;

	$.ajax({
		type: "post",
		url: url,
		timeout: timeout,
		data: updateArray,
		dataType: 'json',
		success: function(d){
			if (!options || options.load != 'no')
			{
				hideUpdatewaitDialog();
			}

			if (d['result'] != "ok")
			{//エラーになりました
				if (object)
				{
					object.find(".bunner_error").show();
					var err = object.find(".error_" + d['code']);
					if(err.get(0))
					{
						err.show()
					}
					else
					{
						alertErrorDialog(d,url);
					}
				}

				if (options && options.error)
				{
					options.error(d);
				}
				return ;
			}

			success_callback && success_callback(d);
		},
		error: function(d){
			if (!options || options.load != 'no')
			{
				hideUpdatewaitDialog();
			}

			if (object)
			{
				object.find(".bunner_error").show();
				alertErrorDialog(d,url);
			}

			if (options && options.error)
			{
				options.error({result: 'error'});
			}
		}
	});
}

function command_preview(obj,prefix)
{
	var arr = {};
	arr["filename"] = obj.find("#" + prefix + "_command").val();
	if (arr["filename"] == "") return ;

	for(var i = 1 ; i <= 5 ; i ++)
	{
		arr["args"+i] = obj.find("#" + prefix + "_command_args"+i).val();
	}
	arr["_csrftoken"] = g_CSRFTOKEN;

	showUpdatewaitDialog();
	$.ajax({
		type: "post",
		url: "/remocon/preview/elec_command",
		timeout: 50000,
		data: arr,
		dataType: 'json',
		success: function(d){
			hideUpdatewaitDialog();
			if ( d['result'] != "ok" )
			{
				obj.find("#" + prefix + "_command_preview_error").text(d['message']).show();
				alertErrorDialog(d,"remocon_preview_elec_command");
				return ;
			}
			obj.find("#" + prefix + "_command_preview_error").hide();
		},
		error: function(d){
			hideUpdatewaitDialog();
			obj.find("#" + prefix + "_command_preview_error").text(d.responseText).show();
			return ;
		}
	});
}

function json2graphconfig(jsonarray,lastdata,type,spantype,title,calfirst,valfirst,minY,maxY,color,bgGradient)
{
	var unixtime;
	var calarray = [calfirst];
	var valarray = [valfirst];
	for(unixtime in jsonarray )
	{
		var date = new Date();
		date.setTime(parseInt(unixtime) * 1000);
		
		if (spantype == "day") {
			calarray.push(date.getHours());
		}
		else {
			calarray.push(date.getDate());
		}
		
		valarray.push( jsonarray[unixtime]);
	}
	//グラフの見栄えを良くするために、現在地もプロットする
	calarray.push("");
	valarray.push(lastdata);

	return {
	  "config": {
	     "title": title
	    ,"SubTitle": ""
	    ,"type": "bezi"
	    ,"lineWidth": 12
	    ,"colorSet":  color
	    ,"bgGradient": bgGradient
	    ,"paddingTop":    70
	    ,"paddingLeft":   45
	    ,"paddingRight":  20
	    ,"hanreiMarkerStyle": "no"
	    ,"width": $(".sensor_graph").width()
	    ,"height": $(".sensor_graph").height()
	    ,"minY": minY
	    ,"maxY": maxY
	  },
	  "data": [ calarray ,valarray ]
	};		
}

function sample_sensor_reload(baseObject,type,prefix)
{
	baseObject.find("#" + prefix + "_value").text("reload....");

	var arr = {}
	arr["_csrftoken"] =  g_CSRFTOKEN;

	$.ajax({
		type: "post",
		url: "/setting/sensor/getall",
		timeout: 50000,
		data: arr,
		dataType: 'json',
		success: function(d){
			if ( d['result'] != "ok" )
			{
				alertErrorDialog(d,"sample_sensor_reload");
				return ;
			}
			
			if (type == "temp")
			{
				baseObject.find("#" + prefix + "_value").text(d["temp"]);
				ccchart.init(prefix + "_day_graph",
					json2graphconfig(d["tempday"],d["temp"],"temp","day","一日の温度","時","℃",0,40,["RED"],{"direction":"vertical","from":"#687478","to":"#222222"})
				);
			}
			else if (type == "lumi")
			{
				baseObject.find("#" + prefix + "_value").text(d["lumi"]);
				ccchart.init(prefix + "_day_graph",
					json2graphconfig(d["lumiday"],d["lumi"],"lumi","day","一日の明るさ","時","度合い",0,300,["YELLOW"],{"direction":"vertical","from":"#687478","to":"#222222"})
				);
			}
			else if (type == "sound")
			{
				baseObject.find("#" + prefix + "_value").text(d["sound"]);
				ccchart.init(prefix + "_day_graph",
					json2graphconfig(d["soundday"],d["sound"],"sound","day","一日の音の数","時","数",0,200,["GREEN"],{"direction":"vertical","from":"#687478","to":"#222222"})
				);
			}
			else
			{
				alert("sample_sensor_reload unknown type(" + type +")");
			}
		}
	});
}
function UntixtimeToString(unixtime)
{
	var ddd = new Date( unixtime * 1000 );
	yy = ddd.getYear();
	mm = ddd.getMonth() + 1;
	dd = ddd.getDate();
	hh = ddd.getHours();
	mi = ddd.getMinutes();
	ss = ddd.getSeconds();
	if (yy < 2000) { yy += 1900; }
	if (mm < 10) { mm = "0" + mm; }
	if (dd < 10) { dd = "0" + dd; }
	if (hh < 10) { hh = "0" + hh; }
	if (mi < 10) { mi = "0" + mi; }
	if (ss < 10) { ss = "0" + ss; }
	
	return yy + "/" + mm + "/" + dd + " " + hh + ":" + mi + ":" + ss;
}


var g_remoconeditObject = null;

function remoconedit_init()
{
	if (g_remoconeditObject) return ;
	g_remoconeditObject = $("#remoconedit");

	if (! window.location.href.match(/#/) )
	{
		//家電アイコンに動作を付ける.
		remoconedit_iconreset();
	}
}



//並び順の更新
function remocon_update_icon_order(menu,action)
{
	var orderbyArray = {};

	var orderby = 1;
	g_remoconeditObject.find("#remocon_remoconmenu_ul li").each(function(){
		var itemObject = $(this);
		var menukey = itemObject.find(".key").text();
		var keyString = "elec_" + menukey + "_order";

		orderbyArray[keyString] = orderby++;
	});
	orderbyArray["_csrftoken"] = g_CSRFTOKEN;

	//elec_ID_order=hogehoge&elec_ID_order=hogehoge&elec_ID_order=hogehoge&elec_ID_order=hogehoge&elec_ID_order=hogehoge&elec_ID_order=hogehoge&
	$.post("/remocon/update/icon_order",orderbyArray, function(d){
		if ( d['result'] != "ok" )
		{
			alertErrorDialog(d,"remocon_update_icon_order");
			return ;
		}
		g_Remocon = d;
	},'json');
}

function remoconedit_stringtospan(str)
{
	if ( str == undefined)
	{
		str = "";
	}

	if ( str.length <= 4) return "<span class=\"lensize_0\">" + str + "</span>";
	else if ( str.length <= 6) return "<span class=\"lensize_1\">" + str + "</span>";
	else if ( str.length <= 10) return "<span class=\"lensize_2\">" + str + "</span>";
	else return "<span class=\"lensize_3\">" + str + "</span>";
}

function remoconedit_iconreset()
{
	//既存のアイコンをすべて消す
	g_remoconeditObject.find("#remocon_remoconmenu_ul").html("");

	var htmltemplate = g_remoconeditObject.find("#remoconmenu_li_sample").html();
	var totaldom = "";
	for(var arraykey in g_Remocon)
	{
		var s = arraykey.split("_");
		if ( !(s[0] == "elec" && s[2] == "type" && s[3] == undefined )) continue;
		var key = s[1];

		var appendclass = "";
		if (getElecBool("showremocon",key) )
		{
			appendclass = "itemshow";
		}
		else
		{
			appendclass = "itemhide";
		}

		var html = htmltemplate;
		html = html.replace(/%KEY%/g , key);
		html = html.replace(/%ICON%/g , "/user/elecicon/" + g_Remocon["elec_" + key + "_elecicon"]);
		html = html.replace(/%TYPE%/g , remoconedit_stringtospan(g_Remocon["elec_" + key + "_type"]) );
		html = html.replace(/%STATUS%/g , remoconedit_stringtospan(g_Remocon["elec_" + key + "_status"]) );
		html = html.replace(/%APPENDCLASS%/g , appendclass);

		totaldom += html;
	}
	g_remoconeditObject.find("#remocon_remoconmenu_ul").html(totaldom);


	//ソートできるようにします
	sortableul( g_remoconeditObject.find( "#remocon_remoconmenu_ul" ) , {
		update: function(obj) { remocon_update_icon_order(); }
	});

	//ボタンにイベントを仕込む
	g_remoconeditObject.find("#remocon_remoconmenu_ul li").each(function(){
		var itemObject = $(this);

		var menukey = itemObject.find(".key").text();
		if (menukey == "new")
		{
			return true;
		}

		var menutype = itemObject.find(".type").text();

		//削除アイコンがクリックされた時の動作
		itemObject.find(".delicon").click(function(){
			delConfirm(function(e){
				var arr = {};
				arr["key"] = menukey;
				arr["_csrftoken"] = g_CSRFTOKEN;
			
				//update待ちにする
				showUpdatewaitDialog();

				//サーバにデータを送信
				//elec_key=123&type=タイプ&type_other=タイプその他&elecicon=filename
				$.ajax({
						type: "post",
						url: "/remocon/delete/elec",
						timeout: 50000,
						data: arr,
						dataType: 'json',
						success: function(d){
							if ( d['result'] != "ok" )
							{
								hideUpdatewaitDialog();
								alertErrorDialog(d,"remocon_delete_elec");
								return ;
							}

							//画面の情報を全入れ替え
							g_Remocon = d;
							
							//トップ画面を更新
							remoconedit_load();
							//音声認識の再読み込み(時間かかるので裏でやる)
							ReloadRecogn();

							hideUpdatewaitDialog();
							return ;
						},
						error: function(d){
							hideUpdatewaitDialog();
							alertErrorDialog(d,"remocon_delete_elec");
							return ;
						}
				});
			}
			,function(e){
				hideUpdatewaitDialog();
			} );
		});

		//編集アイコンがクリックされた時の動作
		itemObject.find(".editicon").click(function(){
			elec_load(menukey);
			return false;
		});

	});


}
function remoconedit_load()
{
	remoconedit_init();
	$('#remoconmenu_li_sample_content').show();

	//トップ画面のアイコンの動作をリセット
	remoconedit_iconreset();
	//トップ画面に戻す
	$.mobile.changePage("#remoconedit", { transition: "flip" });
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
	if ( ! arr["htmlobject"] ) arr["htmlobject"] = $("#loadingwait_dialog_setting");
	if ( ! arr["transition"] ) arr["transition"] = parseInt(Math.random() * 3);

	var screenWidth  = Math.max.apply( null, [document.body.clientWidth , document.body.scrollWidth, document.documentElement.scrollWidth, document.documentElement.clientWidth] );
	var screenHeight = Math.max.apply( null, [document.body.clientHeight , document.body.scrollHeight, document.documentElement.scrollHeight, document.documentElement.clientHeight] );
	var sclLeft = document.body.scrollLeft || document.documentElement.scrollLeft;
	var sclTop = document.body.scrollTop || document.documentElement.scrollTop;

	$( "body" ).append('<div id="loadingwait" style="position: absolute; left: 0px; top: 0px; width: ' + screenWidth + 'px; height: ' + screenHeight + 'px; opacity: 0.9; z-index: 100; background-color: #000000;" ><div id="loadingwait_dialog" style="position:absolute; color: #000000; background-color: #ffffff; left: -9999px; top: 0px; opacity: 1; z-index: 101;" ></div></div>');
	var loadingwait = $( "body #loadingwait" );
	var loadingwait_dialog = $( "body #loadingwait_dialog" ).html( arr["htmlobject"].html() );
	var goal_left	= (document.documentElement.clientWidth  / 2) - (loadingwait_dialog.width()  / 2)+sclLeft;
	var goal_top	= (document.documentElement.clientHeight / 2) - (loadingwait_dialog.height() / 2)+sclTop;


//arr["transition"] = "pop";

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
	else if (arr["transition"] == "rotatedown" || arr["transition"] == 3)
	{
		loadingwait_dialog.css({left: goal_left,top: goal_top,rotate: 0, scale: 10 });
		loadingwait_dialog.transition({
			 left: goal_left
			,top:  goal_top
			,rotate: 360
			,scale: 1
		},'fast',UpdatewaitDialogAnimateEndCallback);
	}
	return loadingwait_dialog;
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
		g_UpdatewaitDialogStatus = 0;
	}
}

function UpdatewaitDialogAnimateEndCallback()
{
	if (g_UpdatewaitDialogStatus == 3)
	{
		$( "body #loadingwait" ).remove();
		g_UpdatewaitDialogStatus = 0;
	}
	else
	{
		//chromeのバグなのか、cssアニメーションが完了しない時があるので、念のため喝入れしとく.
		$( "body #loadingwait_dialog" ).css({rotate: 0, scale: 1 });

		g_UpdatewaitDialogStatus = 2;
	}
	$( "body #loadingwait_dialog" ).find(".focus").focus();
}
//新しい名前を入れてもらうプロンプト
function newnamePrompt(callbackOK,callbackNG)
{
	var dialog = showUpdatewaitDialog( { htmlobject: $("#loadingwait_dialog_newnameprompt") }  );

	dialog.find(".value").val("");

	dialog.find(".ok_button").click(function(e){
		e.value = dialog.find(".value").val();
		callbackOK(e);
		return true;
	});
	dialog.find(".cancel_button").click(function(e){
		callbackNG(e);
		return true;
	});
}

//削除してもいいの？
function delConfirm(callbackOK,callbackNG)
{
	var dialog = showUpdatewaitDialog( { htmlobject: $("#loadingwait_dialog_delconfirm") } );
	
	dialog.find(".ok_button").click(function(e){
		callbackOK(e);
		return true;
	});
	dialog.find(".cancel_button").click(function(e){
		callbackNG(e);
		return true;
	});
}

function updateconfirm(callbackOK,callbackNG)
{
	var dialog = showUpdatewaitDialog( { htmlobject: $("#loadingwait_dialog_updateconfirm") } );
	
	dialog.find(".ok_button").click(function(e){
		callbackOK(e);
		return true;
	});
	dialog.find(".cancel_button").click(function(e){
		callbackNG(e);
		return true;
	});
}

function rebootConfirm(callbackOK,callbackNG)
{
	var dialog = showUpdatewaitDialog( { htmlobject: $("#loadingwait_dialog_rebootconfirm") } );
	
	dialog.find(".ok_button").click(function(e){
		callbackOK(e);
		return true;
	});
	dialog.find(".cancel_button").click(function(e){
		callbackNG(e);
		return true;
	});
}

function shutdownConfirm(callbackOK,callbackNG)
{
	var dialog = showUpdatewaitDialog( { htmlobject: $("#loadingwait_dialog_shutdownconfirm") } );

	dialog.find(".ok_button").click(function(e){
		callbackOK(e);
		return true;
	});
	dialog.find(".cancel_button").click(function(e){
		callbackNG(e);
		return true;
	});
}

function webapiKeyChangeConfirm(callbackOK,callbackNG)
{
	var dialog = showUpdatewaitDialog( { htmlobject: $("#loadingwait_dialog_webapi_key_change") } );
	
	dialog.find(".ok_button").click(function(e){
		callbackOK(e);
		return true;
	});
	dialog.find(".cancel_button").click(function(e){
		callbackNG(e);
		return true;
	});
}

var g_fileselectpageObject = null;
var g_fileselectpageCallback = null;

function fileselectpage_init()
{
	if (g_fileselectpageObject) return ;
	g_fileselectpageObject = $("#fileselectpage");
	g_fileselectpageCallback = null;

	//検索したときにリアルタイムに書き換える
	g_fileselectpageObject.find(".search").keyup(function(){
		g_fileselectpageObject.find(".filelist_ul").html("検索中・・・・");

		var arr = { };
		arr["typepath"] = g_fileselectpageObject.find(".typepath").val();
		arr["search"] = $(this).val();
		arr["_csrftoken"] = g_CSRFTOKEN;
		$.post("/remocon/fileselectpage/find",arr, function(d){
			if ( d['result'] != "ok" )
			{
				alertErrorDialog(d,"remocon_fileselectpage");
				return ;
			}

			var totaldom = "";
			var htmltemplate = g_fileselectpageObject.find(".filelist_li_sample").html();

			//ファイルリストをつくる
			for(var arraykey in d)
			{
				var s = arraykey.split("_");
				if ( !(s[0] == "file" && s[2] == "name" && s[3] == undefined )) continue;
				var key = s[1];

				var html = htmltemplate;
				html = html.replace(/%ICON%/g , d["file_" + key + "_icon"] );
				html = html.replace(/%NAME%/g , d["file_" + key + "_name"] );
				html = html.replace(/%SIZE%/g , sizescale(d["file_" + key + "_size"]) );
				html = html.replace(/%DATE%/g , d["file_" + key + "_date"]);

				totaldom += html;
			}
			//種類によっては、利用しないという選択肢を一番下に出す
			if (arr["typepath"] == "tospeak_mp3")
			{
				totaldom += g_fileselectpageObject.find(".filelist_li_none_sample").html();
			}
			//htmlのはめ込み リストを出す
			g_fileselectpageObject.find(".filelist_ul").html(totaldom);

			//イベントを設定する.
			g_fileselectpageObject.find(".filelist_ul li").each(function(){
				var li = $(this);
				var filenameObject = li.find(".filename");
				var filename = filenameObject.html();
				if (filenameObject.hasClass("nouse"))
				{//選択しない
					filename = "";
				}

				li.find(".delete").click(function(){
					delConfirm(function(e){
						showUpdatewaitDialog();
						fileselectpage_delete(arr["typepath"] , filename , function(){
							//削除したので画面を再描画させる
							g_fileselectpageObject.find(".search").keyup(); 
							hideUpdatewaitDialog();
						} );
					}
					,function(e){
						hideUpdatewaitDialog();
					} );
					return false;
				});
				li.find(".useicon").click(function(){
					history.back();
					g_fileselectpageCallback( filename );
				});
				//種類によっては、アイコンクリックでプレビューを出す
				if (filename != "" && arr["typepath"] == "tospeak_mp3")
				{
					li.find(".fileicon").click(function(){
						tospeak_preview("musicfile",filename);
					})
					.css("cursor","pointer");
				}
			});
		},'json');
	});

	//ajax upload の準備
	g_fileselectpageObject.find('input[type=file]').change(function() {
		g_fileselectpageObject.find(".error").hide();

		var uploadObject = $(this);
		if ( uploadObject.val() == "") return ;
		var typepath = g_fileselectpageObject.find(".typepath").val();
		var url = '/remocon/fileselectpage/upload';
		var arr = {
			 "typepath":	typepath
			,"_csrftoken":	g_CSRFTOKEN
		};

		uploadObject.upload(url,arr,function(d){
			if ( d['result'] != "ok" )
			{
				var err = g_fileselectpageObject.find(".error_" + d['code']);
				if(err.get(0))
				{
					err.show()
				}
				else
				{
					alertErrorDialog(d,"script_scripteditor_annotation");
				}

				return ;
			}

			g_fileselectpageObject.find(".search").val("").keyup();
			uploadObject.val("");

		}, 'json');
	});
}

function fileselectpage_load( typepath , selectfilename , message, okfunction )
{
	fileselectpage_init();
	g_fileselectpageCallback = okfunction;

	g_fileselectpageObject.find(".typepath").val(typepath);
	g_fileselectpageObject.find(".search").val("").keyup().focus();
	if (typepath == "elecicon")
	{
		g_fileselectpageObject.find(".fileselectpage_is_tospeak_mp3").hide();
		g_fileselectpageObject.find(".fileselectpage_is_script_command").hide();
		g_fileselectpageObject.find(".fileselectpage_is_actionicon").hide();
		g_fileselectpageObject.find(".fileselectpage_is_elecicon").show();
	}
	else if (typepath == "script_command")
	{
		g_fileselectpageObject.find(".fileselectpage_is_tospeak_mp3").hide();
		g_fileselectpageObject.find(".fileselectpage_is_elecicon").hide();
		g_fileselectpageObject.find(".fileselectpage_is_actionicon").hide();
		g_fileselectpageObject.find(".fileselectpage_is_script_command").show();
	}
	else if (typepath == "tospeak_mp3")
	{
		g_fileselectpageObject.find(".fileselectpage_is_elecicon").hide();
		g_fileselectpageObject.find(".fileselectpage_is_script_command").hide();
		g_fileselectpageObject.find(".fileselectpage_is_actionicon").hide();
		g_fileselectpageObject.find(".fileselectpage_is_tospeak_mp3").show();
	}
	else if (typepath == "remocon")
	{
		g_fileselectpageObject.find(".fileselectpage_is_tospeak_mp3").hide();
		g_fileselectpageObject.find(".fileselectpage_is_script_command").hide();
		g_fileselectpageObject.find(".fileselectpage_is_actionicon").hide();
		g_fileselectpageObject.find(".fileselectpage_is_elecicon").show();
	}
	else if (typepath == "actionicon")
	{
		g_fileselectpageObject.find(".fileselectpage_is_tospeak_mp3").hide();
		g_fileselectpageObject.find(".fileselectpage_is_script_command").hide();
		g_fileselectpageObject.find(".fileselectpage_is_elecicon").hide();
		g_fileselectpageObject.find(".fileselectpage_is_actionicon").show();
	}

	$.mobile.changePage("#fileselectpage", { transition: "flip" });
}


function fileselectpage_getAnnotation( typepath , selectfilename , callback )
{
	var arr = {};
	arr["typepath"] = typepath;
	arr["filename"] = selectfilename;
	arr["_csrftoken"] = g_CSRFTOKEN;

	$.ajax({
			type: "post",
			url: "/script/scripteditor/annotation",
			timeout: 50000,
			data: arr,
			dataType: 'json',
			success: function(d){
				if ( d['result'] != "ok" )
				{
					alertErrorDialog(d,"script_scripteditor_annotation");
					return ;
				}
				callback(d);

				return ;
			},
			error: function(d){
				alertErrorDialog(d,"script_scripteditor_annotation");
				return ;
			}
	});
}

function fileselectpage_delete( typepath , selectfilename , callback )
{
	var arr = {};
	arr["typepath"] = typepath;
	arr["filename"] = selectfilename;
	arr["_csrftoken"] = g_CSRFTOKEN;

	$.ajax({
			type: "post",
			url: "/remocon/fileselectpage/delete",
			timeout: 50000,
			data: arr,
			dataType: 'json',
			success: function(d){
				if ( d['result'] != "ok" )
				{
					alertErrorDialog(d,"fileselectpage_delete");
					return ;
				}
				callback(d);

				return ;
			},
			error: function(d){
				alertErrorDialog(d,"fileselectpage_delete");
				return ;
			}
	});
}

g_elecObject = null;
function elec_init()
{
	if (g_elecObject) return ;
	g_elecObject = $("#elec");

	//編集画面のイベント
	g_elecObject.find("#type").change(function(){
		var selectObj = g_elecObject.find("#type option:selected");
		if ( $(this).val() == "other" )
		{
			g_elecObject.find("#type_is_SPACE_div").hide();
			g_elecObject.find("#type_not_SPACE_div").show();

			g_elecObject.find("#type_other_div").show();
			g_elecObject.find("#type_yomi").val( g_elecObject.find("#type_other_div").val() );
		}
		else if ( $(this).val() == "SPACE" )
		{
			g_elecObject.find("#type_is_SPACE_div").show();
			g_elecObject.find("#type_not_SPACE_div").hide();
		}
		else
		{
			g_elecObject.find("#type_is_SPACE_div").hide();
			g_elecObject.find("#type_not_SPACE_div").show();

			g_elecObject.find("#type_other_div").hide();
			g_elecObject.find("#type_yomi").val( selectObj.attr("yomi") );
		}

		var filename = selectObj.attr("title");
		var iconname = filename.replace(/^.*\//g,'');

		//アイコンを変更する
		g_elecObject.find("#elecicon").val(iconname);
		//画像をセットする
		g_elecObject.find(".fileicon img").prop("src",filename);
	});

	g_elecObject.find( "#type" )
		.width("500px")
		.msDropDown({rowHeight: 60 }
	);


	g_elecObject.find("#elecicon_select_button").click(function(){
		fileselectpage_load( "elecicon" , g_elecObject.find("#elecicon").val() , "アイコン選択"
		, function(filename){
			//ファイル名を確定
			g_elecObject.find("#elecicon").val(filename);
			
			//画像をセットする
			g_elecObject.find(".fileicon img").prop("src","/user/elecicon/" + filename);
		});
	});
}

//actionの並び順の更新
function elec_update_elec_action_order(menu,action)
{
	var orderbyArray = {};

	var orderby = 1;
	var elec_key = g_elecObject.find("#elec_key").val();
	g_elecObject.find("#elec_actionmenu_ul li").each(function(){
		var itemObject = $(this);
		var actionkey = itemObject.find(".id").text();
		var keyString = "elec_" + elec_key + "_action_" + actionkey + "_order";

		orderbyArray[keyString] = orderby++;
	});
	orderbyArray["_csrftoken"] = g_CSRFTOKEN;

	//elec_ID1_action_ID2_order=hogehoge&elec_ID1_action_ID2_order=hogehoge&elec_ID1_action_ID2_order=hogehoge&elec_ID1_action_ID2_order=hogehoge
	$.post("/remocon/update/elec_action_order",orderbyArray, function(d){
		if ( d['result'] != "ok" )
		{
			alertErrorDialog(d,"elec_update_elec_action_order");
			return ;
		}
	},'json');
}

function elec_save(key)
{
	elec_save_callback(key , function(){
		//トップ画面を更新
		remoconedit_load();
		//音声認識の再読み込み(時間かかるので裏でやる)
		ReloadRecogn();
	});
}

function elec_goto_action(key,actionkey)
{
	elec_save_callback(key , function(){
		action_load(g_elecObject.find('#elec_key').val(),actionkey);
		//音声認識の再読み込み(時間かかるので裏でやる)
		ReloadRecogn();
	});
}


function elec_save_callback(key , callback)
{
	if (g_elecObject.find("#elec_key").val() != key)
	{
		alert("elecページが壊れています。キーと現在編注中のデータが一致しません");
		return false;
	}
	
	var updateArray = {};
	var bad = false;
	updateArray["elec_key"] = key;
	updateArray["type"] = g_elecObject.find("#type").val();
	updateArray["type_other"] = g_elecObject.find("#type_other").val();
	updateArray["showremocon"] = g_elecObject.find("#showremocon").prop("checked") ? 1 : 0;
	updateArray["ignore_recogn"] = g_elecObject.find("#ignore_recogn").prop("checked") ? 1 : 0;
	updateArray["click_to_menu"] = g_elecObject.find("#click_to_menu").prop("checked") ? 1 : 0;

	if (updateArray["type"] == "")
	{
		g_elecObject.find(".error_10922").show();
		bad = true;
	}
	if (updateArray["type"] == "other")
	{
		if (updateArray["type_other"] == "" || updateArray["type_other"] == "SPACE")
		{
			g_elecObject.find(".error_10921").show();
			bad = true;
		}
	}
	updateArray["elecicon"] = g_elecObject.find("#elecicon").val();

	if ( bad )
	{//エラー
		g_elecObject.find(".bunner_error").show();
		return false;
	}
	updateArray["_csrftoken"] = g_CSRFTOKEN;

	//update待ちにする
	showUpdatewaitDialog();

	//サーバにデータを送信
	//elec_key=123&type=タイプ&type_other=タイプその他&elecicon=filename
	$.ajax({
			type: "post",
			url: "/remocon/update/elec",
			timeout: 50000,
			data: updateArray,
			dataType: 'json',
			success: function(d){
				hideUpdatewaitDialog();
				if ( d['result'] != "ok" )
				{
					g_elecObject.find(".bunner_error").show();
					var err = g_elecObject.find(".error_" + d['code']);
					if(err.get(0))
					{
						err.show()
					}
					else
					{
						alertErrorDialog(d,"remocon_update_elec");
					}

					return ;
				}

				//画面の情報を全入れ替え
				g_Remocon = d;

				g_elecObject.find("#elec_key").val(g_Remocon['updatekey']);


				callback();
				return ;
			},
			error: function(d){
				hideUpdatewaitDialog();
				g_elecObject.find(".bunner_error").show();
				alertErrorDialog(d,"remocon_update_elec");
				return ;
			}
	});

	//データを更新
	return true;
}

function elec_delete(parentkey,key)
{
	//いたずら防止のために補正する.
	parentkey = parseInt(parentkey);
	key = parseIntOrNew(key);

	delConfirm(function(e){
		var arr = {};
		arr["key1"] = parentkey;
		arr["key2"] = key;
		arr["_csrftoken"] = g_CSRFTOKEN;
	
		//update待ちにする
		showUpdatewaitDialog();

		//サーバにデータを送信
		$.ajax({
				type: "post",
				url: "/remocon/delete/elec_action",
				timeout: 50000,
				data: arr,
				dataType: 'json',
				success: function(d){
					if ( d['result'] != "ok" )
					{
						alertErrorDialog(d,"remocon_delete_elec_action");
						return ;
					}

					//画面の情報を全入れ替え
					g_Remocon = d;
					
					g_elecObject.find("#elec_actionmenu_ul li").each(function(){
						var itemObject = $(this);
						var actionkey = itemObject.find(".id").text();
						if (actionkey == key)
						{
							itemObject.hide();
							return false;
						}
						return true;
					});

					hideUpdatewaitDialog();

					//音声認識の再読み込み(時間かかるので裏でやる)
					ReloadRecogn();
					return ;
				},
				error: function(d){
					hideUpdatewaitDialog();
					alertErrorDialog(d,"remocon_delete_elec_action");
					return ;
				}
		});
	}
	,function(e){
		hideUpdatewaitDialog();
	} );
}


function elec_load(key)
{
	elec_init();
	$('#remoconmenu_li_sample_content').hide();

	//いたづら防止のために補正する
	key = parseIntOrNew(key);

	//エラーを消す.
	g_elecObject.find(".error").hide();
	//通知も消す
	g_elecObject.find(".notify").hide();

	g_elecObject.find("#elec_actionmenu_ul").html("");
	g_elecObject.find("#elec_key").val(key);

	if (key == "new")
	{//新規作成
		g_elecObject.find("#type").val("").change();
		g_elecObject.find("#type_other_div").hide();
		g_elecObject.find("#type_other").val("");
		var typeObject = g_elecObject.find("#type");
		typeObject.data("dd").set("selectedIndex",0);

		g_elecObject.find("#elecicon").val("onoff.png");
		g_elecObject.find(".fileicon img").prop("src","/user/elecicon/onoff.png");
		g_elecObject.find("#showremocon").prop("checked",true).change();	//リモコン画面に表示する
		g_elecObject.find("#ignore_recogn").prop("checked",false).change();	//音声認識対象から消す
		g_elecObject.find("#click_to_menu").prop("checked",false).change();	//大きなアイコンクリックで動作ではなくメニューを開く
	}
	else
	{//新規作成でないので、データを取得してくる.
		if (! g_Remocon["elec_" + key + "_type"] )
		{
			//データがない!!
			alert(g_Strings["DATA_NOT_FOUND"]);
			return ;
		}
		var typeObject = g_elecObject.find("#type");
		optionSelect(typeObject, g_Remocon["elec_" + key + "_type"] );
		typeObject.data("dd").set("selectedIndex",  getindexOptionSelect(typeObject , g_Remocon["elec_" + key + "_type"] ));

		var selectObj = g_elecObject.find("#type option:selected");

		if ( typeObject.val() == "other" )
		{
			g_elecObject.find("#type_is_SPACE_div").hide();
			g_elecObject.find("#type_not_SPACE_div").show();
			g_elecObject.find("#type_other_div").show();
			g_elecObject.find("#type_yomi").val( g_elecObject.find("#type_other_div").val() );
		}
		else if ( typeObject.val() == "SPACE" )
		{
			g_elecObject.find("#type_is_SPACE_div").show();
			g_elecObject.find("#type_not_SPACE_div").hide();
			g_elecObject.find("#type_other_div").hide();
			g_elecObject.find("#type_yomi").val( selectObj.attr("yomi") );
		}
		else
		{
			g_elecObject.find("#type_is_SPACE_div").hide();
			g_elecObject.find("#type_not_SPACE_div").show();
			g_elecObject.find("#type_other_div").hide();
			g_elecObject.find("#type_yomi").val( selectObj.attr("yomi") );
		}

		g_elecObject.find("#type_other").val(g_Remocon["elec_" + key + "_type"]);

		g_elecObject.find("#elecicon").val(g_Remocon["elec_" + key + "_elecicon"]);
		g_elecObject.find(".fileicon img").prop("src","/user/elecicon/" + g_Remocon["elec_" + key + "_elecicon"]);
		g_elecObject.find("#showremocon").prop("checked",getElecBool("showremocon",key)).change();	//リモコン画面に表示する
		g_elecObject.find("#ignore_recogn").prop("checked",getElecBool("ignore_recogn",key)).change();	//音声認識対象から消す
		g_elecObject.find("#click_to_menu").prop("checked",getElecBool("click_to_menu",key)).change();	//大きなアイコンクリックで動作ではなくメニューを開く


		var htmltemplate = g_elecObject.find("#elec_actionmenu_li_sample").html();
		var totaldom = "";
		for(var arraykey2 in g_Remocon)
		{
			var s = arraykey2.split("_");
			if ( !(s[0] == "elec" && s[1] == key && s[2] == "action" && s[4] == "actiontype" && s[5] == undefined )) continue;
			var actionkey = s[3];

			var appendclass = "";
			if (getElecBool("showremocon",key,actionkey) )
			{
				appendclass = "itemshow";
			}
			else
			{
				appendclass = "itemhide";
			}

			var html = htmltemplate;
			html = html.replace(/%ACTIONKEY%/g,actionkey);
			html = html.replace(/%ACTIONTYPE%/g,g_Remocon["elec_" + key + "_action_" + actionkey + "_actiontype"] );
			html = html.replace(/%ACTIONCOLOR%/g,g_Remocon["elec_" + key + "_action_" + actionkey + "_actioncolor"] );
			html = html.replace(/%APPENDCLASS%/g,appendclass);
			totaldom += html;
		}
		g_elecObject.find("#elec_actionmenu_ul").html(totaldom);
	}

	//jquery ui sorttable を構築
	sortableul(g_elecObject.find( "#elec_actionmenu_ul" ) , {
		update: function(obj) { elec_update_elec_action_order(); }
	});

	$.mobile.changePage("#elec", { transition: "flip" });
	g_elecObject.find("input[type='checkbox']").checkboxradio("refresh");
	g_elecObject.find("input[type='select']").selectmenu("refresh");
}


g_actionObject = null;

function action_save(parentkey,key)
{
	if (g_actionObject.find("#action_key1").val() != parentkey || g_actionObject.find("#action_key2").val() != key)
	{
		alert("actionページが壊れています。キーと現在編注中のデータが一致しません");
		return false;
	}
	
	var updateArray = {};
	var bad = false;
	updateArray["action_key1"] = parentkey;
	updateArray["action_key2"] = key;
	updateArray["actiontype"] = g_actionObject.find("#actiontype").val();
	updateArray["actiontype_other"] = g_actionObject.find("#actiontype_other").val();
	updateArray["actionvoice"] = g_actionObject.find("#actionvoice").prop("checked") ? 1 : 0;
	for(var i = 1 ; i <= 5 ; i ++)
	{
		updateArray["actionvoice_command_"+i] = g_actionObject.find("#actionvoice_command_"+i).val();
	}
	updateArray["showremocon"] = g_actionObject.find("#showremocon").prop("checked") ? 1 : 0;
	updateArray["nostateremocon"] = g_actionObject.find("#nostateremocon").prop("checked") ? 1 : 0;
	updateArray["actionicon_use"] = g_actionObject.find("#actionicon_use").prop("checked") ? 1 : 0;
	updateArray["actionicon"] = g_actionObject.find("#actionicon").text();

	updateArray["tospeak_select"] = g_actionObject.find("#tospeak_select").val();
	updateArray["actioncolor"] = g_actionObject.find("#actioncolor").val();
	updateArray["tospeak_string"] = g_actionObject.find("#tospeak_string").val();
	updateArray["tospeak_mp3"] = g_actionObject.find("#tospeak_mp3").text();

	updateArray["execflag"] = g_actionObject.find("#execflag").val();
	updateArray["exec_ir"] = g_actionObject.find("#exec_ir").val();
	updateArray["exec_command"] = g_actionObject.find("#exec_command").val();
	for(var i = 1 ; i <= 5 ; i ++)
	{
		updateArray["exec_command_args"+i] = g_actionObject.find("#exec_command_args"+i).val();
	}
	for(var i = 1 ; i <= 9 ; i ++)
	{
		updateArray["exec_macro_"+i+"_elec"] = g_actionObject.find("#exec_macro_"+i+"_elec").val();
		updateArray["exec_macro_"+i+"_action"] = g_actionObject.find("#exec_macro_"+i+"_action").val();
		updateArray["exec_macro_"+i+"_after"] = g_actionObject.find("#exec_macro_"+i+"_after").val();
	}
	updateArray["timer_type"] = g_actionObject.find("#timer_type").val();
	updateArray["timer_exec_elec"] = g_actionObject.find("#timer_exec_elec").val();
	updateArray["timer_exec_action"] = g_actionObject.find("#timer_exec_action").val();
	updateArray["timer_tospeak_select"] = g_actionObject.find("#timer_tospeak_select").val();
	updateArray["timer_tospeak_string"] = g_actionObject.find("#timer_tospeak_string").val();
	updateArray["timer_tospeak_mp3"] = g_actionObject.find("#timer_tospeak_mp3").text();
	updateArray["timer_no_status_overraide"] = g_actionObject.find("#timer_no_status_overraide").prop("checked") ? 1 : 0;

	updateArray["recongpause_type"] = g_actionObject.find("#recongpause_type").val();

	updateArray["multiroom_room"] = g_actionObject.find("#multiroom_room").val();
	updateArray["multiroom_elec"] = g_actionObject.find("#multiroom_elec").val();
	updateArray["multiroom_action"] = g_actionObject.find("#multiroom_action").val();

	updateArray["netdevice_elec"] = g_actionObject.find("#netdevice_elec").val();
	updateArray["netdevice_action"] = g_actionObject.find("#netdevice_action").val();
	updateArray["netdevice_value"] = g_actionObject.find("#netdevice_value").val();

	updateArray["sip_action_type"] = g_actionObject.find("#sip_action_type").val();
	updateArray["sip_call_number"] = g_actionObject.find("#sip_call_number").val();

	if (updateArray["actiontype"] == "")
	{
		g_actionObject.find(".error_20022").show();
		bad = true;
	}
	if (updateArray["actiontype"] == "other" && updateArray["actiontype_other"] == "")
	{
		g_actionObject.find(".error_20021").show();
		bad = true;
	}

	if (updateArray["tospeak"])
	{
		if (updateArray["tospeak_select"] == "musicfile")
		{
			if (updateArray["tospeak_mp3"] == "")
			{
				g_actionObject.find("#tospeak_mp3_error").show();
				bad = true;
			}
		}
	}
	if (updateArray["timer_tospeak"])
	{
		if (updateArray["timer_tospeak_select"] == "musicfile")
		{
			if (updateArray["timer_tospeak_mp3"] == "")
			{
				g_actionObject.find("#timer_tospeak_mp3_error").show();
				bad = true;
			}
		}
	}

	if (updateArray["execflag"] == "ir")
	{
		if (updateArray["exec_ir"] == "")
		{
			g_actionObject.find("#exec_ir_error").show();
			bad = true;
		}
	}
	else if (updateArray["execflag"] == "command")
	{
		if (updateArray["exec_command"] == "")
		{
			g_actionObject.find("#exec_command_error").show();
			bad = true;
		}
	}

	if ( bad )
	{//エラー
		g_actionObject.find(".bunner_error").show();
		return false;
	}
	updateArray["_csrftoken"] = g_CSRFTOKEN;
	
	//update待ちにする
	showUpdatewaitDialog();

	//サーバにデータを送信
	$.ajax({
			type: "post",
			url: "/remocon/update/elec_action",
			timeout: 50000,
			data: updateArray,
			dataType: 'json',
			success: function(d){
				hideUpdatewaitDialog();
				if ( d['result'] != "ok" )
				{
					g_actionObject.find(".bunner_error").show();
					var err = g_actionObject.find(".error_" + d['code']);
					if(err.get(0))
					{
						err.show()
					}
					else
					{
						alertErrorDialog(d,"action_save");
					}
					return ;
				}

				g_Remocon = d;

				//機材の画面を更新
				elec_load(parentkey);

				//音声認識の再読み込み(時間かかるので裏でやる)
				ReloadRecogn();

				return ;
			},
			error: function(d){
				hideUpdatewaitDialog();
				alertErrorDialog(d,"action_save");
				return ;
			}
	});

	//データを更新
	return true;
}

function action_load(parentkey,key)
{
	$('#remoconmenu_li_sample_content').hide();

	//いたずら防止のために補正する.
	parentkey = parseInt(parentkey);
	key = parseIntOrNew(key);
	
	action_init();

	g_actionObject.find("#action_key1").val(parentkey).change();
	g_actionObject.find("#action_key2").val(key).change();

	//エラーを消す.
	g_actionObject.find(".error").hide();
	//通知も消す
	g_actionObject.find(".notify").hide();
	//赤外線正常学習の通知も消す
	g_actionObject.find("#exec_ir_ok").hide();

	if (key == "new")
	{//新規作成
		for( var i = 1 ; i <= 5 ; i ++ )
		{
			g_actionObject.find("#actionvoice_command_"+i).val( "" ).change();		//音声認識のコマンド
		}

		g_actionObject.find("#showremocon").prop("checked",true).change();	//リモコン画面に表示する
		g_actionObject.find("#nostateremocon").prop("checked",false).change();	//家電の状態を変えない

		g_actionObject.find("#tospeak_select").val("string").change();
		g_actionObject.find("#tospeak_string").val("").change();
		g_actionObject.find("#tospeak_mp3").text("").change();

		g_actionObject.find("#execflag").val("ir").change();		//操作方法
		g_actionObject.find("#exec_ir").val("").change();

		g_actionObject.find("#exec_command").val("").change();			//コマンド本体
		for( var i = 1 ; i <= 5 ; i ++ )
		{
			g_actionObject.find("#exec_command_args" + i).val("").change();		//引数
		}
		action_command(getElecText("exec_command",parentkey,key),g_actionObject,"exec_command");

		//マクロ部分の選択肢を作る
		for( var i = 1 ; i <= 9 ; i ++ )
		{
			make_macro_combo_elec(true,g_actionObject.find("#exec_macro_" + i + "_elec")).val("").change();
			g_actionObject.find("#exec_macro_" + i + "_action").val("").change();
			g_actionObject.find("#exec_macro_" + i + "_after").val("").change();
		}

		g_actionObject.find("#timer_type").val("").change();
		g_actionObject.find("#timer_tospeak_select").val("string").change();
		g_actionObject.find("#timer_tospeak_string").val("").change();
		g_actionObject.find("#timer_tospeak_mp3").text( getElecText("timer_tospeak_mp3",parentkey,key) ).change();
		make_macro_combo_elec(false,  g_actionObject.find("#timer_exec_elec")  );
		g_actionObject.find("#timer_exec_elec").val("").change();
		g_actionObject.find("#timer_exec_action").val("").change();
		g_actionObject.find("#timer_no_status_overraide").prop("checked",false).change();

		g_actionObject.find("#recongpause_type").val("").change();

		make_multiroom_combo_room(g_actionObject.find("#multiroom_room")  );
		g_actionObject.find("#multiroom_room").val("").change();
		g_actionObject.find("#multiroom_elec").val("").change();
		g_actionObject.find("#multiroom_action").val("").change();

		g_actionObject.find("#netdevice_elec").val("").change();
		g_actionObject.find("#netdevice_action").val("").change();
		g_actionObject.find("#netdevice_value").val("").change();
		make_netdevice_combo_elec(g_actionObject.find("#netdevice_elec"),1,function(){
			g_actionObject.find("#netdevice_elec").selectmenu("refresh");
		});

		g_actionObject.find("#sip_action_type").val("").change();
		g_actionObject.find("#sip_call_number").val("").change();

		g_actionObject.find("#actiontype_other").val("").change();
		g_actionObject.find("#actiontype").val("").change();	//種類

		g_actionObject.find("#actionicon_use").prop("checked",false).change(); //アイコンで表すか？
		g_actionObject.find("#actionicon").text("std_setting.png").change();
	}
	else
	{//新規作成でないので、データを取得してくる.
		key = parseInt(key);
		if ( getElecText("actiontype",parentkey,key) == "")
		{
			//データがない!!
			alert(g_Strings["DATA_NOT_FOUND"]);
			return ;
		}

		g_actionObject.find("#actiontype_other").val( getElecText("actiontype",parentkey,key) ).change();
		optionSelect(g_actionObject.find("#actiontype") , getElecText("actiontype",parentkey,key) ).change();

		for( var i = 1 ; i <= 5 ; i ++ )
		{
			g_actionObject.find("#actionvoice_command_"+i).val( getElecText("actionvoice_command_"+i,parentkey,key) ).change();		//音声認識のコマンド
		}

		g_actionObject.find("#showremocon").prop("checked", getElecBool("showremocon",parentkey,key) ).change();	//リモコン画面に表示する
		g_actionObject.find("#nostateremocon").prop("checked", getElecBool("nostateremocon",parentkey,key) ).change();	//家電の状態を変えない
		g_actionObject.find("#actioncolor").val(getElecText("actioncolor",parentkey,key)).change();

		g_actionObject.find("#tospeak_select").val(getElecText("tospeak_select",parentkey,key)).change();					//読み上げ方法
		g_actionObject.find("#tospeak_string").val(getElecText("tospeak_string",parentkey,key)).change();					//文字列読み上げ
		g_actionObject.find("#tospeak_mp3").text(getElecText("tospeak_mp3",parentkey,key)).change();						//音楽ファイル読み上げ

		g_actionObject.find("#timer_type").val(getElecText("timer_type",parentkey,key)).change();
		g_actionObject.find("#timer_tospeak_select").val(getElecText("timer_tospeak_select",parentkey,key)).change();					//読み上げ方法
		g_actionObject.find("#timer_tospeak_string").val(getElecText("timer_tospeak_string",parentkey,key)).change();					//文字列読み上げ
		g_actionObject.find("#timer_tospeak_mp3").text(getElecText("timer_tospeak_mp3",parentkey,key)).change();						//音楽ファイル読み上げ
		make_macro_combo_elec(false,  g_actionObject.find("#timer_exec_elec")  );
		g_actionObject.find("#timer_exec_elec").val(getElecText("timer_exec_elec",parentkey,key)).change();
		g_actionObject.find("#timer_exec_action").val(getElecText("timer_exec_action",parentkey,key)).change();
		g_actionObject.find("#timer_no_status_overraide").prop("checked",getElecBool("timer_no_status_overraide",parentkey,key) ).change();

		make_multiroom_combo_room(g_actionObject.find("#multiroom_room")  );
		g_actionObject.find("#multiroom_room").val(getElecText("multiroom_room",parentkey,key)).change();
		g_actionObject.find("#multiroom_elec").val(getElecText("multiroom_elec",parentkey,key)).change();
		make_multiroom_combo_elec(g_actionObject.find("#multiroom_room"),g_actionObject.find("#multiroom_elec"),g_actionObject.find("#multiroom_action"),function(){
			g_actionObject.find("#multiroom_action").val(getElecText("multiroom_action",parentkey,key)).selectmenu("refresh");
		});

		make_netdevice_combo_elec(g_actionObject.find("#netdevice_elec"),1,function(){
			g_actionObject.find("#netdevice_elec").val(getElecText("netdevice_elec",parentkey,key)).selectmenu("refresh");
			make_netdevice_combo_action(g_actionObject.find("#netdevice_elec"),g_actionObject.find("#netdevice_action"),function(){
				g_actionObject.find("#netdevice_action").val(getElecText("netdevice_action",parentkey,key)).selectmenu("refresh");
				make_netdevice_combo_value(g_actionObject.find("#netdevice_elec"),g_actionObject.find("#netdevice_action"),g_actionObject.find("#netdevice_value"),function(){
					g_actionObject.find("#netdevice_value").val(getElecText("netdevice_value",parentkey,key)).selectmenu("refresh");
				});
			});
		});

		g_actionObject.find("#sip_action_type").val(getElecText("sip_action_type",parentkey,key)).change();
		g_actionObject.find("#sip_call_number").val(getElecText("sip_call_number",parentkey,key)).change();

		g_actionObject.find("#recongpause_type").val(getElecText("recongpause_type",parentkey,key)).change();

		g_actionObject.find("#execflag").val(getElecText("execflag",parentkey,key)).change();			//操作方法
		g_actionObject.find("#exec_ir").val(getElecText("exec_ir",parentkey,key)).change();

		g_actionObject.find("#exec_command").val(getElecText("exec_command",parentkey,key)).change();
		for( var i = 1 ; i <= 5 ; i ++ )
		{
			g_actionObject.find("#exec_command_args" + i).val(getElecText("exec_command_args" + i,parentkey,key)).change();		//引数
		}
		action_command(getElecText("exec_command",parentkey,key),g_actionObject,"exec_command");

		//マクロ部分の選択肢を作る
		for( var i = 1 ; i <= 9 ; i ++ )
		{
			make_macro_combo_elec(true,g_actionObject.find("#exec_macro_" + i + "_elec")).val(getElecText("exec_macro_" + i + "_elec",parentkey,key)).change();
			g_actionObject.find("#exec_macro_" + i + "_action").val(getElecText("exec_macro_" + i + "_action",parentkey,key)).change();
			g_actionObject.find("#exec_macro_" + i + "_after").val(getElecText("exec_macro_" + i + "_after",parentkey,key)).change();
		}

		g_actionObject.find("#actionicon_use").prop("checked",getElecBool("actionicon_use",parentkey,key) ).change();
		g_actionObject.find("#actionicon").text(getElecText("actionicon",parentkey,key)).change();
	}

	//機材設定で音声認識を無効にするフラグが立っていたら警告を出す.
	if ( getElecBool("ignore_recogn",parentkey) )
	{
		g_actionObject.find("#actionvoice_ignore_recogn").show();
	}

	$.mobile.changePage("#action", { transition: "flip" });
	g_actionObject.find("input[type='checkbox']").checkboxradio("refresh");
	g_actionObject.find("input[type='select']").selectmenu("refresh");
}

function make_macro_combo_elec(noselectoption,obj)
{
	obj.empty();
	if (noselectoption)
	{
		obj.append("<option value=\"none\" selected>" + g_Strings["SELECT_NO_CONTROL"] + "</option>");
	}

	for(var arraykey in g_Remocon)
	{
		var s = arraykey.split("_");
		if ( !(s[0] == "elec" && s[2] == "type" && s[3] == undefined )) continue;
		var key = s[1];

		obj.append("<option value=\"" + h(g_Remocon["elec_" + key + "_type"]) +"\">" + g_Remocon["elec_" + key + "_type"] + "</option>");
	}

	if (noselectoption)
	{
		obj.val("none");
	}
	return obj;
}

function make_macro_combo_action(noselectoption,selectval,obj)
{
	obj.empty();
	if (noselectoption)
	{
		obj.append("<option value=\"none\" selected>" + g_Strings["SELECT_NO_CONTROL"] + "</option>");
	}
	else
	{
		obj.append("<option value=\"\" selected>" + g_Strings["SELECT_CONTROL"] + "</option>");
	}

	for(var arraykey in g_Remocon)
	{
		var s = arraykey.split("_");
		if ( !(s[0] == "elec" && s[2] == "type" && s[3] == undefined )) continue;
		var key = s[1];

		if ( selectval != g_Remocon["elec_" + key + "_type"] ) continue;
		for(var arraykey2 in g_Remocon)
		{
			var s = arraykey2.split("_");
			if ( !(s[0] == "elec" && s[1] == key && s[2] == "action" && s[4] == "actiontype" && s[5] == undefined)) continue;
			var actionkey = s[3];

			obj.append("<option value=\"" + h(g_Remocon["elec_" + key + "_action_" + actionkey + "_actiontype"]) +"\">" + g_Remocon["elec_" + key + "_action_" + actionkey + "_actiontype"] + "</option>");
		}
	}

	if (noselectoption)
	{
		obj.val("none");
	}
	return obj;
}
function make_multiroom_combo_room(roomobj)
{
	roomobj.empty();
	roomobj.append("<option value=\"\" selected>" + g_Strings["SELECT_FHC_TERMINAL"] + "</option>");
	roomobj.val('');

	for(var arraykey in g_MultiRoom)
	{
		var s = arraykey.split("_");
		if ( !(s[0] == "multiroom" && s[2] == "uuid" )) continue;
		var key = s[1];

		var uuid = h(g_MultiRoom["multiroom_" + key + "_uuid"]);
		var name = h(g_MultiRoom["multiroom_" + key + "_name"]);
		var url  = h(g_MultiRoom["multiroom_" + key + "_url"]);

		if (uuid == g_SETTING["httpd__uuid"])
		{//自機
			continue;
		}

		roomobj.append("<option value=\"" + uuid +"\">" + name + "(" + url + ")" + "(" + uuid + ")</option>");
	}
}
function combo_empty_clear(comboobj)
{
	comboobj.empty();
	comboobj.append("<option value=\"\" selected>" + g_Strings["SELECT_CONTROL"] + "</option>");
	comboobj.val('');
}

function make_multiroom_combo_elec(roomobj,elecobj,callback)
{
	var arr = { uuid: roomobj.val() 
		,webapi_apikey: g_SETTING["webapi_apikey"] };

	if( arr['uuid'] == '')
	{
		combo_empty_clear(elecobj);
		return;
	}

	update(
		null
		,"/api/multiroom/elec/getlist"
		,arr
		,function(d){ 
			combo_empty_clear(elecobj);
			for(var i in d['list'])
			{
				name = h(d['list'][i]);
				elecobj.append("<option value=\"" + name +"\">" + name + "</option>");
			}
			callback && callback();
		}
		,{ load: 'no'
			,timeout: 1000
			,error: function(){
				combo_empty_clear(elecobj);
				callback && callback();
			}
		 }
	);
}


function make_multiroom_combo_action(roomobj,elecobj,actionobj,callback)
{
	var arr = { uuid: roomobj.val() 
		,elec: elecobj.val()
		,webapi_apikey: g_SETTING["webapi_apikey"] };

	if( arr['uuid'] == '' || arr['elec'] == '')
	{
		combo_empty_clear(actionobj);
		callback && callback();
		return;
	}

	update(
		null
		,"/api/multiroom/elec/getactionlist"
		,arr
		,function(d){ 
			combo_empty_clear(actionobj);
			for(var i in d['list'])
			{
				name = h(d['list'][i]);
				actionobj.append("<option value=\"" + name +"\">" + name + "</option>");
			}
			callback && callback();
		}
		,{ load: 'no'
			,timeout: 1000
			,error: function(){
				combo_empty_clear(actionobj);
				callback && callback();
			}
		 }
	);
}

function make_netdevice_combo_elec(elecobj,searchtime,callback)
{
	var arr = {};
	arr['searchtime'] = searchtime;

	update(
		null
		,"/remocon/netdevice/elec/getlist"
		,arr
		,function(d){ 
			combo_empty_clear(elecobj);
			for(var i in d['list'])
			{
				name = h(d['list'][i]);
				elecobj.append("<option value=\"" + name +"\">" + name + "</option>");
			}
			callback && callback();
		}
		,{ load: 'no'
		  ,timeout: 1000 * (searchtime+4)
		  ,error: function(d){
				combo_empty_clear(elecobj);
				callback && callback();
			}
		 }
	);
}
function make_netdevice_combo_action(elecobj,actionobj,callback)
{
	var arr = {"elec":	elecobj.val()};

	update(
		null
		,"/remocon/netdevice/action/getlist"
		,arr
		,function(d){ 
			combo_empty_clear(actionobj);
			for(var i in d['list'])
			{
				name = h(d['list'][i]);
				actionobj.append("<option value=\"" + name +"\">" + name + "</option>");
			}
			callback && callback();
		}
		,{ load: 'no'
		  ,timeout: 2000
		  ,error: function(){
				combo_empty_clear(actionobj);
				callback && callback();
			}
		 }
	);
}
function make_netdevice_combo_value(elecobj,actionobj,valueobj,callback)
{
	var arr = {"elec":	elecobj.val() , "action": actionobj.val()};

	update(
		null
		,"/remocon/netdevice/value/getlist"
		,arr
		,function(d){ 
			combo_empty_clear(valueobj);
			for(var i in d['list'])
			{
				name = h(d['list'][i]);
				valueobj.append("<option value=\"" + name +"\">" + name + "</option>");
			}
			callback && callback();
		}
		,{ load: 'no'
		  ,timeout: 2000
		  ,error: function(){
				combo_empty_clear(valueobj);
				callback && callback();
			}
		 }
	);
}



function iconselect_command(baseObject,prefix,type,title)
{
	var controllObject = baseObject.find("#" + prefix);
	var controllSampleObject = controllObject.closest(".filesample");
	controllSampleObject.find(".changebutton").click(function(){
		fileselectpage_load( type , controllObject.text() ,title
		, function(filename){
			controllObject.text(filename).change();
		});
	});
	
	controllObject.change(function(){
		var filename = controllObject.text();
		if (filename == "")
		{
			filename = "no_image.png";
		}
		controllSampleObject.find(".fileicon img").prop("src","/user/" + type + "/"+filename).show();
	});
}


function mp3select_command(baseObject,prefix)
{
	var controllObject = baseObject.find("#" + prefix);
	var controllSampleObject = controllObject.closest(".filesample");
	controllSampleObject.find(".changebutton").click(function(){
		fileselectpage_load( "tospeak_mp3" , controllObject.text() ,g_Strings["SELECT_SPEAK_FILE"]
		, function(filename){
			controllObject.text(filename);
		});
	});
	//読み上げるmp3をプレビュー再生
	controllSampleObject.find(".preview").click(function(){
		tospeak_preview("musicfile",controllObject.text());
	});
}
function speaktext_command(baseObject,prefix)
{
	var controllObject = baseObject.find("#" + prefix);
	var controllSampleObject = controllObject.closest(".filesample");

	//プレビュー再生
	controllSampleObject.find(".preview").click(function(){
		tospeak_preview("string",controllObject.val());
	});
}

function tospeak_command(baseObject,prefix)
{
	baseObject.find("#" + prefix + "_select").change(function(){
		if ( baseObject.find("#" + prefix + "_select").val() == "string" )
		{
			baseObject.find("#" + prefix + "_none_div").hide();
			baseObject.find("#" + prefix + "_mp3_div").hide();
			baseObject.find("#" + prefix + "_string_div").show();
		}
		else if ( baseObject.find("#" + prefix + "_select").val() == "musicfile" )
		{
			baseObject.find("#" + prefix + "_none_div").hide();
			baseObject.find("#" + prefix + "_string_div").hide();
			baseObject.find("#" + prefix + "_mp3_div").show();
		}
		else if ( baseObject.find("#" + prefix + "_select").val() == "none" )
		{
			baseObject.find("#" + prefix + "_string_div").hide();
			baseObject.find("#" + prefix + "_mp3_div").hide();
			baseObject.find("#" + prefix + "_none_div").show();
		}
	});

	//読み上げるtextをプレビュー再生
	speaktext_command(baseObject,prefix + "_string");

	//mp3選択フォーム
	mp3select_command(baseObject,prefix + "_mp3");
}

//コマンド選択
function action_command(filename,baseObject,prefix)
{
	//選択画面に表示する画像
	if (filename != "")
	{
		baseObject.find("#" + prefix + "").val(filename).change();
		baseObject.find("#" + prefix + "_display").html(filename);
		baseObject.find("#" + prefix + "_select_icon img").prop("src","/jscss/images/icon_script_command.png");
	}
	else
	{
		baseObject.find("#" + prefix + "").val("").change();
		baseObject.find("#" + prefix + "_display").html("未定義");
		baseObject.find("#" + prefix + "_select_icon img").prop("src","/jscss/images/icon_script_command_nocommand.png");
	}

	for ( var key = 1 ; key < 6 ; key ++){
		baseObject.find("#" + prefix + "_args" + key + "_mustlabel").text("未定義");
		baseObject.find("#" + prefix + "_args" + key + "_label").text("未定義の引数"+key);
		baseObject.find("#" + prefix + "_args" + key + "").prop("placeholder","");
		baseObject.find("#" + prefix + "_args" + key + "").prop("type","text");
		baseObject.find("#" + prefix + "_args" + key + "").removeClass("");
	}

	if (filename != "")
	{
		//ファイル内のアノテーションによって、コメントを動的に変化させる.
		fileselectpage_getAnnotation( "script_command" , filename , function(d){
			for ( var key = 1 ; key < 6 ; key ++){
				if (d["args_" + key + "_must"] == "must")
				{
					baseObject.find("#" + prefix + "_args" + key + "_mustlabel").text("必須");
				}
				else if (d["args_" + key + "_must"] == "optionl")
				{
					baseObject.find("#" + prefix + "_args" + key + "_mustlabel").text("任意");
				}
				else if (d["args_" + key + "_must"] == "none")
				{
					baseObject.find("#" + prefix + "_args" + key + "_mustlabel").text("利用しません");
					baseObject.find("#" + prefix + "_args" + key + "").addClass("");
				}
				else
				{
					baseObject.find("#" + prefix + "_args" + key + "").addClass("");
				}

				if ( d["args_" + key + "_name"] && d["args_" + key + "_name"] != "")
				{
					if (d["args_" + key + "_must"] != "none")
					{
						baseObject.find("#" + prefix + "_args" + key + "_label").text(d["args_" + key + "_name"]);
					}
				}
				baseObject.find("#" + prefix + "_args" + key + "").prop("placeholder",d["args_" + key + "_def"]);
			}
		});
	}
}

//音声読み上げ系のpreview
function tospeak_preview(type,str)
{
	if (str == "") return ;

	var arr = {};
	arr["_csrftoken"] = g_CSRFTOKEN;

	showUpdatewaitDialog();
	if (type == "musicfile")
	{
		arr["tospeak_mp3"] = str;

		$.ajax({
			type: "post",
			url: "/remocon/preview/elec_mp3",
			timeout: 50000,
			data: arr,
			dataType: 'json',
			success: function(d){

				hideUpdatewaitDialog();
				if ( d['result'] != "ok" )
				{
					alertErrorDialog(d,"remocon_preview_elec_mp3");
				}
			},
			error: function(d){
				hideUpdatewaitDialog();
			}
		});
	}
	else
	{//str
		arr["tospeak_string"] = str;

		$.ajax({
			type: "post",
			url: "/remocon/preview/tospeak_string",
			timeout: 50000,
			data: arr,
			dataType: 'json',
			success: function(d){

				hideUpdatewaitDialog();
				if ( d['result'] != "ok" )
				{
					alertErrorDialog(d,"remocon_preview_tospeak_string");
				}
			},
			error: function(d){
				hideUpdatewaitDialog();
			}
		});
	}
}

//カラーピッカー
function colorpicker_command(baseObject,prefix)
{
	var controllObject = baseObject.find("#" + prefix);
	var controllSampleObject = controllObject.parent(".colorpicker");

	controllSampleObject.click(function(){
		//カラーピッカーを作る
		var colorpicker = new_cpick( controllSampleObject.get(0) ,{
			 target: $("#" + prefix + "").get(0)
			,callback: function(color){
				baseObject.find("#" + prefix + "").val(color).change();
			}
			,posstion: "up"
			,show: 0
			,hide: 0
			,height: 300
			,width:  400
		});

		colorpicker.flip();
	});

	//色の見本も変更する
	controllObject.change(function(){
		var color = controllObject.val();
		controllSampleObject.find(".colorpicker_box").css("background-color"  , color );
//		controllSampleObject.find(".colorpicker_string").css("color", color );
	});
}


function action_init()
{
	if (g_actionObject != null) return ;
	g_actionObject = $("#action");

	g_actionObject.find("#actiontype").change(function(){
		var value = $(this).val();
		if ( value == "SPACE" )
		{//余白
			g_actionObject.find("#action_not_SPACE_div").hide();
			g_actionObject.find("#action_is_SPACE_div").show();

			g_actionObject.find("#actiontype_other_div").hide();
			g_actionObject.find("#actionvoice_command_1").val("").change();
			g_actionObject.find("#actionvoice_command_2").val("").change();
			g_actionObject.find("#actionvoice_command_3").val("").change();
			g_actionObject.find("#actionvoice_command_4").val("").change();
			g_actionObject.find("#actionvoice_command_5").val("").change();

			g_actionObject.find("#tospeak_string").val("").change();

			g_actionObject.find("#execflag").val("none").change();
			g_actionObject.find("#showremocon").prop("checked",true).change();
			g_actionObject.find("#showremocon").prop("checked",true).change();
		}
		else
		{//余白ではなくコンテンツ
			g_actionObject.find("#action_is_SPACE_div").hide();
			g_actionObject.find("#action_not_SPACE_div").show();
			if ( value == "other" )
			{
				g_actionObject.find("#actiontype_other_div").show();
			}
			else
			{
				g_actionObject.find("#actiontype_other_div").hide();
			}

			var type_yomi  = g_elecObject.find("#type_yomi").val();
			var selectobj = g_actionObject.find("#actiontype option:selected");
			var color = selectobj.attr("color");
			g_actionObject.find("#actioncolor").val("#"+color).change();

			var recong = selectobj.attr("recong").replace(/@/g,type_yomi);
			g_actionObject.find("#actionvoice_command_1").val(recong).change();

			var speak = selectobj.attr("speak").replace(/@/g,type_yomi);
			g_actionObject.find("#tospeak_string").val(speak).change();

			$custom_execflag = '';
			if (value == g_Strings["ACTION_TIMER_SET"])
			{
				$custom_execflag = 'timer';
				g_actionObject.find("#timer_type").val("60").change();
				g_actionObject.find("#timer_tospeak_string").val(type_yomi + "のタイマーのじかんです");
			}
			else if (value == g_Strings["ACTION_TIMER_RESET"])
			{
				$custom_execflag = 'timer';
				g_actionObject.find("#timer_type").val("reset").change();
			}

			else if (value == g_Strings["ACTION_RECOGN_PAUSE"])
			{
				$custom_execflag = 'recongpause';
				g_actionObject.find("#recongpause_type").val("stop").change();
			}
			else if (value == g_Strings["ACTION_RECOGN_RESTART"])
			{
				$custom_execflag = 'recongpause';
				g_actionObject.find("#recongpause_type").val("start").change();
			}
			else if (value == g_Strings["ACTION_SIP_CALL"])
			{
				$custom_execflag = 'sip';
				g_actionObject.find("#sip_action_type").val("call").change();
			}
			else if (value == g_Strings["ACTION_SIP_ANSWER"])
			{
				$custom_execflag = 'sip';
				g_actionObject.find("#sip_action_type").val("answer").change();
			}
			else if (value == g_Strings["ACTION_SIP_HANGUP"])
			{
				$custom_execflag = 'sip';
				g_actionObject.find("#sip_action_type").val("hangup").change();
			}
			else if (value == g_Strings["ACTION_SIP_NUMPAD"])
			{
				$custom_execflag = 'sip';
				g_actionObject.find("#sip_action_type").val("numpad").change();
			}

			if ($custom_execflag != '')
			{//IR実行以外のカスタム表示
				g_actionObject.find("#execflag").val($custom_execflag).change();
				g_actionObject.find("#nostateremocon").prop("checked",true).change();
			}
		}

		if ($.mobile.activePage.attr('id') == "action")
		{
			g_actionObject.find("input[type='checkbox']").checkboxradio("refresh");
			g_actionObject.find("input[type='select']").selectmenu("refresh");
		}
	});

	//カラーピッカーを作る
	colorpicker_command(g_actionObject,"actioncolor");

	//読み上げ text/mp3選択
	tospeak_command(g_actionObject,"tospeak");

	g_actionObject.find("#timer_type").change(function(){
		if ( g_actionObject.find("#timer_type").val() == "reset" )
		{
			g_actionObject.find("#timer_setting_div").hide();
			g_actionObject.find("#timer_reset_setting_div").show();
		}
		else
		{
			g_actionObject.find("#timer_reset_setting_div").hide();
			g_actionObject.find("#timer_setting_div").show();
		}
	});

	g_actionObject.find("#sip_action_type").change(function(){
		g_actionObject.find(".sip_action_setting_div").hide();
		g_actionObject.find(".sip_action_" + $(this).val() + "_setting_div").show();
	});

	g_actionObject.find("#timer_exec_elec").change(function(){
		make_macro_combo_action(false,g_actionObject.find("#timer_exec_elec").val() , g_actionObject.find("#timer_exec_action")  );
	});
	//読み上げ text/mp3選択
	tospeak_command(g_actionObject,"timer_tospeak");

	g_actionObject.find("#multiroom_room").change(function(){
		make_multiroom_combo_elec(g_actionObject.find("#multiroom_room"),g_actionObject.find("#multiroom_elec"),g_actionObject.find("#multiroom_action"));
	});
	g_actionObject.find("#multiroom_elec").change(function(){
		make_multiroom_combo_action(g_actionObject.find("#multiroom_room"),g_actionObject.find("#multiroom_elec"),g_actionObject.find("#multiroom_action"));
	});

	g_actionObject.find("#netdevice_elec").change(function(){
		make_netdevice_combo_action(g_actionObject.find("#netdevice_elec"),g_actionObject.find("#netdevice_action"));
	});
	g_actionObject.find("#netdevice_action").change(function(){
		make_netdevice_combo_value(g_actionObject.find("#netdevice_elec"),g_actionObject.find("#netdevice_action"),g_actionObject.find("#netdevice_value"));
	});

	g_actionObject.find("#exec_ir").change(function(){
		if ( $(this).val() == "" )
		{
			g_actionObject.find("#exec_ir_registlabel").hide();
		}
		else
		{
			g_actionObject.find("#exec_ir_registlabel").show();
		}
	});

	g_actionObject.find("#execflag").change(function(){
		g_actionObject.find(".action_exec_page").hide();
		g_actionObject.find("#exec_" + $(this).val() + "_div").show();
	});

	g_actionObject.find("#exec_ir_leaning").click(function(){
		var arr = { };
		arr["exec_ir"] = g_actionObject.find("#exec_ir").val();

		//すべてを一つの処理でやると、linuxだとまれに落ちるらしい...意味がわからないので処理を分割する.
		TopLevelInvoke(
			null
			,"/remocon/ir/leaning"
			,arr
			,function(d){ 
				g_actionObject.find("#exec_ir").val(d["exec_ir"]).change();
				g_actionObject.find("#exec_ir_registlabel").show();
				g_actionObject.find("#exec_ir_error2").hide();
				g_actionObject.find("#exec_ir_error").hide();
				g_actionObject.find("#exec_ir_ok").show();
			}
			,{ load: $("#loadingwait_dialog_irlearn") 
			  ,timeout: 100000
			  ,error: function(d){
					g_actionObject.find("#exec_ir").val("").change();
					g_actionObject.find("#exec_ir_registlabel").hide();
					g_actionObject.find("#exec_ir_ok").hide();
					g_actionObject.find("#exec_ir_error2").show();
			  }
			 }
		);
	});


	g_actionObject.find("#exec_ir_preview_button").click(function(){
		var arr = { };
		arr["exec_ir"] = g_actionObject.find("#exec_ir").val();

		update(
			null
			,"/remocon/ir/fire"
			,arr
			,function(d){ 
			}
			,{ load: $("#loadingwait_dialog_fire") }
		);
	});


	//コマンドのプレビュー
	g_actionObject.find("#exec_command_preview_button").click(function(){
		command_preview(g_actionObject,"exec");
	});

	//コマンド選択
	g_actionObject.find("#exec_command_select_button").click(function(){
		fileselectpage_load( "script_command" , g_actionObject.find("#exec_command").val() , g_Strings["SELECT_COMMAND_FILE"]
		, function(filename){
			action_command(filename,g_actionObject,"exec_command");
		});
	});
	
	//マクロのプレビュー
	g_actionObject.find("#exec_macro_preview_button").click(function(){
		macro_preview(g_actionObject,"exec_macro");
	});

	//マクロ
	macro_set(g_actionObject,"exec_macro");
	
	//タイマーのプレビュー
	g_actionObject.find("#exec_timer_preview_button").click(function(){
		var arr = { };
		arr["type1"] = g_actionObject.find("#timer_exec_elec").val();
		arr["type2"] = g_actionObject.find("#timer_exec_action").val();

		update(
			null
			,"/remocon/fire/bytype"
			,arr
			,function(d){ 
			}
			,{ load: $("#loadingwait_dialog_fire") }
		);
	});

	//マルチルームのプレビュー
	g_actionObject.find("#exec_multiroom_preview_button").click(function(){
		var arr = { };
		arr["uuid"] = g_actionObject.find("#multiroom_room").val();
		arr["elec"] = g_actionObject.find("#multiroom_elec").val();
		arr["action"] = g_actionObject.find("#multiroom_action").val();
		arr["webapi_apikey"] = g_SETTING["webapi_apikey"];

		update(
			null
			,"/api/multiroom/elec/action"
			,arr
			,function(d){ 
			}
			,{ load: $("#loadingwait_dialog_fire") }
		);
	});

	//ネット家電のプレビュー
	g_actionObject.find("#exec_netdevice_preview_button").click(function(){
		var arr = { };
		arr["elec"] = g_actionObject.find("#netdevice_elec").val();
		arr["action"] = g_actionObject.find("#netdevice_action").val();
		arr["value"] = g_actionObject.find("#netdevice_value").val();

		update(
			null
			,"/remocon/netdevice/fire"
			,arr
			,function(d){ 
			}
			,{ load: $("#loadingwait_dialog_fire") }
		);
	});

	//ネット家電の再スキャン
	g_actionObject.find("#exec_netdevice_rescan_button").click(function(){
		showUpdatewaitDialog();
		make_netdevice_combo_elec(g_actionObject.find("#netdevice_elec"),2,function(){
			hideUpdatewaitDialog();
		});
	});

	//SIP電話連携のプレビュー
	g_actionObject.find("#exec_sip_preview_button").click(function(){
		var arr = { };
		arr["sip_action_type"] = g_actionObject.find("#sip_action_type").val();
		arr["sip_call_number"] = g_actionObject.find("#sip_call_number").val();

		update(
			null
			,"/remocon/sip/fire"
			,arr
			,function(d){ 
			}
			,{ load: $("#loadingwait_dialog_fire") }
		);
	});

	//状態をアイコンで表示する
	g_actionObject.find("#actionicon_use").change(function(){
		showhidden(g_actionObject.find("#actionicon_use_div"),$(this).prop("checked") );
	});

	//状態表示のアイコン
	iconselect_command(g_actionObject,"actionicon","actionicon","状態のアイコン");
}

var g_account = null;
function account_init()
{
	if (g_account) return ;
	g_account = $("#account");

	//設定するボタンをクリックした時
	g_account.find("#account_auth_button").click(function(){
		g_account.find(".error").hide();
		var updateArray = {
			 'account_usermail':	g_account.find("#account_usermail").val()
			,'account_password':	g_account.find("#account_password").val()
			,'account_sync_server':	g_account.find("#account_sync_server").prop("checked") ? 1 : 0
			,'_csrftoken':			g_CSRFTOKEN
		};
		showUpdatewaitDialog();

		$.ajax({
			type: "post",
			url: "/setting/account/update",
			timeout: 50000,
			data: updateArray,
			dataType: 'json',
			success: function(d){
				hideUpdatewaitDialog();
				if (d['result'] != "ok")
				{
					if (d['code'] == 39091)
					{
						g_SETTING["account_auth_status"] = "error";
						g_account.find(".bunner_error").show();
						g_account.find("#account_error_uuid_error").show();
						return ;
					}
					g_SETTING["account_auth_status"] = "error";
					g_account.find(".bunner_error").show();
					g_account.find("#account_error_auth_error").show();
					return ;
				}
				g_SETTING = d;
				g_account.find(".error").hide();
				$.mobile.changePage("#system", { transition: "flip" });
			},
			error: function(d){
				hideUpdatewaitDialog();
				g_account.find(".bunner_error").show();
				return ;
			}
		});
	});
}

function account_load()
{
	account_init();
	g_account.find(".error").hide();

	g_account.find("#account_usermail").val(g_SETTING["account_usermail"]).change();
	g_account.find("#account_password").val("<%PASSWORD NO CHANGE%>").change();
	g_account.find("#account_sync_server").prop("checked" ,g_SETTING["account_sync_server"]=="1" ).change();

	$.mobile.changePage("#account", { transition: "flip" });
	g_account.find("input[type='checkbox']").checkboxradio("refresh");
}


var g_network = null;
function network_init()
{
	if (g_network) return ;
	g_network = $("#network");

	g_network.find("#network_ipaddress_type").change(function(){
		if ( $(this).val() == "DHCP" || $(this).val() == "NONE")
		{
			g_network.find("#network_ipaddress_div").hide();
		}
		else
		{
			g_network.find("#network_ipaddress_div").show();
		}
	});
	g_network.find("#network_w_ipaddress_type").change(function(){
		if ( $(this).val() == "DHCP" )
		{
			g_network.find("#network_w_ipaddress_div").hide();
			g_network.find("#network_w_ipaddress_use").show();
		}
		else if ( $(this).val() == "NONE" )
		{
			g_network.find("#network_w_ipaddress_div").hide();
			g_network.find("#network_w_ipaddress_use").hide();
		}
		else
		{
			g_network.find("#network_w_ipaddress_div").show();
			g_network.find("#network_w_ipaddress_use").show();
		}
	});
	
	
	var networkrebootFunction = function()
	{
		showUpdatewaitDialog( { htmlobject: $("#loadingwait_dialog_reboot") } )
		
		//再起動待ち
		if (g_network.find("#network_ipaddress_type").val() == "DHCP" )
		{
			rebootfunction("");
		}
		else
		{
			rebootfunction("http://" + g_network.find("#network_ipaddress_ip").val() );
		}
	};

	//認証するボタンをクリックした時
	g_network.find("#network_apply_button").click(function(){
		//読込中にする
		showUpdatewaitDialog();
		g_network.find(".error").hide();
		var updateArray = {
			  'network_ipaddress_type':					g_network.find("#network_ipaddress_type").val()
			 ,'network_ipaddress_ip':		g_network.find("#network_ipaddress_ip").val()
			 ,'network_ipaddress_mask':		g_network.find("#network_ipaddress_mask").val()
			 ,'network_ipaddress_gateway':	g_network.find("#network_ipaddress_gateway").val()
			 ,'network_ipaddress_dns':		g_network.find("#network_ipaddress_dns").val()
			 ,'network_w_ipaddress_type':				g_network.find("#network_w_ipaddress_type").val()
			 ,'network_w_ipaddress_ip':		g_network.find("#network_w_ipaddress_ip").val()
			 ,'network_w_ipaddress_mask':	g_network.find("#network_w_ipaddress_mask").val()
			 ,'network_w_ipaddress_gateway':	g_network.find("#network_w_ipaddress_gateway").val()
			 ,'network_w_ipaddress_dns':		g_network.find("#network_w_ipaddress_dns").val()
			 ,'network_w_ipaddress_ssid':				g_network.find("#network_w_ipaddress_ssid").val()
			 ,'network_w_ipaddress_password':			g_network.find("#network_w_ipaddress_password").val()
			 ,'network_w_ipaddress_wkeymgmt':			g_network.find("#network_w_ipaddress_wkeymgmt").val()
			 ,'_csrftoken':			g_CSRFTOKEN
		};
		$.ajax({
			type: "post",
			url: "/setting/network/update",
			timeout: 10000,
			data: updateArray,
			dataType: 'json',
			success: function(d){
				if (d['result'] != "ok")
				{//エラーになりました
					hideUpdatewaitDialog();
					g_network.find(".bunner_error").show();
					if(d['code'] > 40000 && d['code'] <= 40100)
					{
						g_remotenetworkObject.find("#network_error_" + d['code']).show();
					}
					
					return ;
				}
				
				g_SETTING = d;

				networkrebootFunction();
				return ;
			},
			error: function(d){
				//IPが変更されたので応答がなくなったと考えられる
				networkrebootFunction();
				return ;
			}
		});
	});
}

function network_load()
{
	network_init();

	g_network.find(".error").hide();
	g_network.find("#network_ipaddress_type").val(g_SETTING["network_ipaddress_type"]).change();
	g_network.find("#network_ipaddress_ip").val(g_SETTING["network_ipaddress_ip"]).change();
	g_network.find("#network_ipaddress_mask").val(g_SETTING["network_ipaddress_mask"]).change();
	g_network.find("#network_ipaddress_gateway").val(g_SETTING["network_ipaddress_gateway"]).change();
	g_network.find("#network_ipaddress_dns").val(g_SETTING["network_ipaddress_dns"]).change();

	g_network.find("#network_w_ipaddress_type").val(g_SETTING["network_w_ipaddress_type"]).change();
	g_network.find("#network_w_ipaddress_ip").val(g_SETTING["network_w_ipaddress_ip"]).change();
	g_network.find("#network_w_ipaddress_mask").val(g_SETTING["network_w_ipaddress_mask"]).change();
	g_network.find("#network_w_ipaddress_gateway").val(g_SETTING["network_w_ipaddress_gateway"]).change();
	g_network.find("#network_w_ipaddress_dns").val(g_SETTING["network_w_ipaddress_dns"]).change();
	g_network.find("#network_w_ipaddress_ssid").val(g_SETTING["network_w_ipaddress_ssid"]).change();
	g_network.find("#network_w_ipaddress_password").val(g_SETTING["network_w_ipaddress_password"]).change();
	g_network.find("#network_w_ipaddress_wkeymgmt").val(g_SETTING["network_w_ipaddress_wkeymgmt"]).change();
	
	if (g_SETTING["network_is_wifi_device"] == "1")
	{
		g_network.find("#network_is_wifi_device").show();
	}
	else
	{
		g_network.find("#network_is_wifi_device").hide();
	}
	

	$.mobile.changePage("#network", { transition: "flip" });
}


var g_recong = null;

function recong_init()
{
	if (g_recong) return ;
	g_recong = $("#recong");

	//設定するボタンをクリックした時
	g_recong.find("#recong_apply_button").click(function(){
		//読込中にする
		showUpdatewaitDialog();
		g_recong.find(".error").hide();
		var updateArray = {
			  'recong_type':				g_recong.find("#recong_type").val()
			 ,'recong_julius_gomi_y':		g_recong.find("#recong_julius_gomi_y").val()
			 ,'recong_julius_gomi_e':		g_recong.find("#recong_julius_gomi_e").val()
			 ,'recong_julius_gomi_d':		g_recong.find("#recong_julius_gomi_d").val()
			 ,'recong_julius_filter1':		g_recong.find("#recong_julius_filter1").val()
			 ,'recong_julius_filter2':		g_recong.find("#recong_julius_filter2").val()
			 ,'recong_julius_lv_base':		g_recong.find("#recong_julius_lv_base").val()
			 ,'recong_julius_lv_max':		g_recong.find("#recong_julius_lv_max").val()
			 ,'recong_volume':				g_recong.find("#recong_volume").val()
			 ,'recong__yobikake__1':		g_recong.find("#recong__yobikake__1").val()
			 ,'recong__yobikake__2':		g_recong.find("#recong__yobikake__2").val()
			 ,'recong__yobikake__3':		g_recong.find("#recong__yobikake__3").val()
			 ,'recong__yobikake__4':		g_recong.find("#recong__yobikake__4").val()
			 ,'recong__yobikake__5':		g_recong.find("#recong__yobikake__5").val()
			 ,'recong_ok_mp3':				g_recong.find("#recong_ok_mp3").text()
			 ,'recong_ng_mp3':				g_recong.find("#recong_ng_mp3").text()
			 ,'recong_pause_mp3':			g_recong.find("#recong_pause_mp3").text()
			 ,'_csrftoken':			g_CSRFTOKEN
		};
		$.ajax({
			type: "post",
			url: "/setting/recong/update",
			timeout: 50000,
			data: updateArray,
			dataType: 'json',
			success: function(d){
				hideUpdatewaitDialog();
				if (d['result'] != "ok")
				{//エラーになりました
					g_recong.find("#recong_error_"+d['code']).show();
					g_recong.find(".bunner_error").show();
					return ;
				}

				g_SETTING = d;

				$.mobile.changePage("#setting", { transition: "flip" });
				return ;
			},
			error: function(d){
				hideUpdatewaitDialog();
				g_recong.find(".bunner_error").show();
				return ;
			}
		});
	});
	g_recong.find("#recong_type").change(function(){
		var value = $(this).val();
		if (value == "julius_standalone" || value == "julius_english_standalone" )
		{
			g_recong.find(".recong_use_class").show();
			g_recong.find(".recong_2pass_filter_class").show();
		}
		else if (value == "julius_standalone_light" || value == "julius_english_standalone_light")
		{
			g_recong.find(".recong_use_class").show();
			g_recong.find(".recong_2pass_filter_class").hide();
		}
		else
		{
			g_recong.find(".recong_use_class").hide();
		}
	});
	//mp3選択フォーム
	mp3select_command(g_recong,"recong_ok_mp3");
	mp3select_command(g_recong,"recong_ng_mp3");
	mp3select_command(g_recong,"recong_pause_mp3");
}

function recong_load()
{
	recong_init();

	g_recong.find(".error").hide();
	g_recong.find("#recong_type").val(g_SETTING["recong_type"]).change();
	g_recong.find("#recong__yobikake__1").val(g_SETTING["recong__yobikake__1"]).change();
	g_recong.find("#recong__yobikake__2").val(g_SETTING["recong__yobikake__2"]).change();
	g_recong.find("#recong__yobikake__3").val(g_SETTING["recong__yobikake__3"]).change();
	g_recong.find("#recong__yobikake__4").val(g_SETTING["recong__yobikake__4"]).change();
	g_recong.find("#recong__yobikake__5").val(g_SETTING["recong__yobikake__5"]).change();
	g_recong.find("#recong_ok_mp3").text(g_SETTING["recong_ok_mp3"]).change();
	g_recong.find("#recong_ng_mp3").text(g_SETTING["recong_ng_mp3"]).change();
	g_recong.find("#recong_pause_mp3").text(g_SETTING["recong_pause_mp3"]).change();

	$.mobile.changePage("#recong", { transition: "flip" });
	g_recong.find("#recong_volume").val(g_SETTING["recong_volume"]).change().slider('refresh');
	g_recong.find("#recong_julius_gomi_y").val(g_SETTING["recong_julius_gomi_y"]).change().slider('refresh');
	g_recong.find("#recong_julius_gomi_e").val(g_SETTING["recong_julius_gomi_e"]).change().slider('refresh');
	g_recong.find("#recong_julius_gomi_d").val(g_SETTING["recong_julius_gomi_d"]).change().slider('refresh');
	g_recong.find("#recong_julius_filter1").val(g_SETTING["recong_julius_filter1"]).change().slider('refresh');
	g_recong.find("#recong_julius_filter2").val(g_SETTING["recong_julius_filter2"]).change().slider('refresh');
	g_recong.find("#recong_julius_lv_base").val(g_SETTING["recong_julius_lv_base"]).change().slider('refresh');
	g_recong.find("#recong_julius_lv_max").val(g_SETTING["recong_julius_lv_max"]).change().slider('refresh');
}


var g_speak = null;
function speak_init()
{
	if (g_speak) return ;
	g_speak = $("#speak");

	//読み上げエンジンの名前だけじゃ分かりにくいので、ヒントを入れる
	getSpeakHint = function(name)
	{
		if (name.indexOf("openjtalk_") == 0)
		{
			return name + "(" + g_Strings["LANG_JA"] + ")";
		}
		else if (name.indexOf("festival_") == 0)
		{
			return name + "(" + g_Strings["LANG_EN"] + ")";
		}
		return name;
	}
	//読み上げエンジンを登録
	for(var x in g_SETTING)
	{
		if ( x.indexOf("setting_speak_type_list_") != 0) continue;

		var name = g_SETTING[x];
		var displayname = getSpeakHint(name);
		g_speak.find("#speak_type").append("<option value=\"" + g_SETTING[x] + "\">" + displayname + "</option>");
	}

	//設定するボタンをクリックした時
	g_speak.find("#speak_apply_button").click(function(){
		//読込中にする
		showUpdatewaitDialog();

		g_speak.find(".error").hide();

		var updateArray = {
			  'speak_type':						g_speak.find("#speak_type").val()
			 ,'speak_volume':					g_speak.find("#speak_volume").val()
			 ,'speak_use_remote':				g_speak.find("#speak_use_remote").prop("checked") ? 1 : 0
			 ,'speak_remote1_ip':				g_speak.find("#speak_remote1_ip").val()
			 ,'speak_remote1_secretkey':		g_speak.find("#speak_remote1_secretkey").val()
			 ,'speak_use_nightvolume':			g_speak.find("#speak_use_nightvolume").prop("checked") ? 1 : 0
			 ,'speak_nightvolume_start_hour':	g_speak.find("#speak_nightvolume_start_hour").val()
			 ,'speak_nightvolume_start_minute':	g_speak.find("#speak_nightvolume_start_minute").val()
			 ,'speak_nightvolume_end_hour':		g_speak.find("#speak_nightvolume_end_hour").val()
			 ,'speak_nightvolume_end_minute':	g_speak.find("#speak_nightvolume_end_minute").val()
			 ,'speak_nightvolume_volume':		g_speak.find("#speak_nightvolume_volume").val()
			 ,'_csrftoken':			g_CSRFTOKEN
		};
		$.ajax({
			type: "post",
			url: "/setting/speak/update",
			timeout: 50000,
			data: updateArray,
			dataType: 'json',
			success: function(d){
				hideUpdatewaitDialog();

				if (d['result'] != "ok")
				{//エラーになりました
					g_speak.find(".bunner_error").show();
					var err = g_speak.find(".error_" + d['code']);
					if(err.get(0))
					{
						err.show()
					}
					else
					{
						alertErrorDialog(d,"setting_speak_apply");
					}
					return ;
				}

				g_SETTING = d;

				$.mobile.changePage("#setting", { transition: "flip" });
				return ;
			},
			error: function(d){
				hideUpdatewaitDialog();

				g_speak.find(".bunner_error").show();
				return ;
			}
		});
	});

	g_speak.find("#speak_use_remote").change(function(){
		showhidden(g_speak.find("#speak_use_remote_div"),$(this).prop("checked") );
	});

	g_speak.find("#speak_use_nightvolume").change(function(){
		showhidden(g_speak.find("#speak_use_nightvolume_div"),$(this).prop("checked") );
	});
}


function speak_load()
{
	speak_init();

	g_speak.find(".error").hide();
	g_speak.find("#speak_type").val(g_SETTING["speak_type"]).change();

	$.mobile.changePage("#speak", { transition: "flip" });
	g_speak.find("#speak_volume").val(g_SETTING["speak_volume"]).change().slider('refresh');
	g_speak.find("#speak_nightvolume_volume").val(g_SETTING["speak_nightvolume_volume"]).change().slider('refresh');


	g_speak.find("#speak_use_remote").prop("checked" ,g_SETTING["speak_use_remote"]=="1" ).change();
	g_speak.find("#speak_remote1_ip").val(g_SETTING["speak_remote1_ip"]).change();
	g_speak.find("#speak_remote1_secretkey").val(g_SETTING["speak_remote1_secretkey"]).change();

	g_speak.find("#speak_use_nightvolume").prop("checked" ,g_SETTING["speak_use_nightvolume"]=="1" ).change();
	g_speak.find("#speak_nightvolume_start_hour").val(g_SETTING["speak_nightvolume_start_hour"]).change();
	g_speak.find("#speak_nightvolume_start_minute").val(g_SETTING["speak_nightvolume_start_minute"]).change();
	g_speak.find("#speak_nightvolume_end_hour").val(g_SETTING["speak_nightvolume_end_hour"]).change();
	g_speak.find("#speak_nightvolume_end_minute").val(g_SETTING["speak_nightvolume_end_minute"]).change();

	g_speak.find("input[type='checkbox']").checkboxradio("refresh");
}


var g_sensor = null;
function sensor_init()
{
	if (g_sensor) return ;
	g_sensor = $("#sensor");

	//設定するボタンをクリックした時
	g_sensor.find("#sensor_apply_button").click(function(){
		//読込中にする
		showUpdatewaitDialog();

		g_sensor.find(".error").hide();

		var updateArray = {
			  'sensor_lumi_type':		g_sensor.find("#sensor_lumi_type").val()
			 ,'sensor_temp_type':		g_sensor.find("#sensor_temp_type").val()
			 ,'_csrftoken':			g_CSRFTOKEN
		};
		$.ajax({
			type: "post",
			url: "/setting/sensor/update",
			timeout: 50000,
			data: updateArray,
			dataType: 'json',
			success: function(d){
				hideUpdatewaitDialog();

				if (d['result'] != "ok")
				{//エラーになりました
					g_sensor.find(".bunner_error").show();
					return ;
				}

				g_SETTING = d;

				$.mobile.changePage("#setting", { transition: "flip" });
				return ;
			},
			error: function(d){
				hideUpdatewaitDialog();

				g_sensor.find(".bunner_error").show();
				return ;
			}
		});
	});
}

function sensor_load()
{
	sensor_init();

	g_sensor.find(".error").hide();
	g_sensor.find("#sensor_lumi_type").val(g_SETTING["sensor_lumi_type"]).change();
	g_sensor.find("#sensor_temp_type").val(g_SETTING["sensor_temp_type"]).change();

	var arr = {}
	arr["_csrftoken"] =  g_CSRFTOKEN;

	$.ajax({
		type: "post",
		url: "/setting/sensor/getall",
		timeout: 50000,
		data: arr,
		dataType: 'json',
		success: function(d){
			if ( d['result'] != "ok" )
			{
				alertErrorDialog(d,"setting_sensor_get");
				return ;
			}
			$("#sensor_temp").text(d["temp"]);
			$("#sensor_lumi").text(d["lumi"]);
			$("#sensor_sound").text(d["sound"]);

			ccchart.init('sensor_temp_day_graph',
				json2graphconfig(d["tempday"],d["temp"],"temp","day","一日の温度","時","℃",0,40,["RED"],{"direction":"vertical","from":"#687478","to":"#222222"})
			);

			ccchart.init('sensor_temp_week_graph',
				json2graphconfig(d["tempweek"],d["temp"],"temp","week","一週間の温度","日","℃",0,40,["RED"],{"direction":"vertical","from":"#687478","to":"#222222"})
			);

			ccchart.init('sensor_lumi_day_graph',
				json2graphconfig(d["lumiday"],d["lumi"],"lumi","day","一日の明るさ","時","度合い",0,300,["YELLOW"],{"direction":"vertical","from":"#687478","to":"#222222"})
			);

			ccchart.init('sensor_lumi_week_graph',
				json2graphconfig(d["lumiweek"],d["lumi"],"lumi","week","一週間の明るさ","日","度合い",0,300,["YELLOW"],{"direction":"vertical","from":"#687478","to":"#222222"})
			);

			ccchart.init('sensor_sound_day_graph',
				json2graphconfig(d["soundday"],d["sound"],"sound","day","一日の音の数","時","数",0,200,["GREEN"],{"direction":"vertical","from":"#687478","to":"#222222"})
			);

			ccchart.init('sensor_sound_week_graph',
				json2graphconfig(d["soundweek"],d["sound"],"sound","week","一週間の音の数","日","数",0,200,["GREEN"],{"direction":"vertical","from":"#687478","to":"#222222"})
			);
		},
		error: function(d){
		}
	});

	$.mobile.changePage("#sensor", { transition: "flip" });
}




var g_mictest = null;
function mictest_init()
{
	if (g_mictest) return ;
	g_mictest = $("#mictest");
}

function mictest_load()
{
	mictest_init();

	var messageObject = g_mictest.find("#mictest_messge");
	messageObject.html("");
	var arr = {
		  'lasttime':		0
		 ,'limit':			7
		 ,'_csrftoken':		g_CSRFTOKEN
	};
	var objects = [];
	
	
	g_mictest.find("#mictest_log_textarea_autoupdate_div").show();
	g_mictest.find("#mictest_log_textarea_updatetop_div").hide();
	
	var ismouseenter = false;
	messageObject.focus(function(){
		g_mictest.find("#mictest_log_textarea_autoupdate_div").hide();
		g_mictest.find("#mictest_log_textarea_updatetop_div").show();
		ismouseenter = true; 
	})
	.blur(function(){
		g_mictest.find("#mictest_log_textarea_autoupdate_div").show();
		g_mictest.find("#mictest_log_textarea_updatetop_div").hide();
		ismouseenter = false;
		miclogcallbackfunction(); 
	});
	
	g_mictest.find("#mictest_log_textarea_updatetop_div").click(function(){
		g_mictest.find("#mictest_log_textarea_autoupdate_div").show();
		g_mictest.find("#mictest_log_textarea_updatetop_div").hide();
		ismouseenter = false;
		miclogcallbackfunction(); 
	});

	var miclogcallbackfunction = function(){
		//もうログのページにいないなら没
		if ($.mobile.activePage.attr('id') != "mictest") return ;
		//マウスが入っているなら更新停止
		if (ismouseenter) return ;

		$.ajax({
			type: "post",
			url: "/recong/mictest/log",
			timeout: 50000,
			data: arr,
			dataType: 'json',
			success: function(dd){
				for (index in dd['log'])
				{
					var d = dd['log'][index];
					if ( d['time'] )
					{
						messageObject.find(".mictest_messge_timef").each(function(){
							if ( $(this).text() == d['time'] )
							{
								d['time'] = 0;
								return false;
							}
						});
						if ( d['time'] == 0 ) continue;

						var newObject = $(g_mictest.find("#mictest_messge_sample").html());
						arr['lasttime'] = d['time'];

						var text1 = "";
						var text2 = "";
						var fixit = "";
						if (d['code'] == "XX")
						{
							text1 = g_Strings["mictest_XX1_TEXT1"];
							text2 = "(" + d['match'] + ")" + "[" + d['matchd'] + "]";
							if (g_SETTING["recong_type"] == "julius_standalone_light")
							{
								fixit += g_Strings["mictest_XX1_FIXIT1"] + "<br />";
								fixit += "<a href=\"https://rti-giken.jp/fhc/help/ref/mictest_XX1.html\" class=\"ui-link\" target=\"_blank\">"+g_Strings["mictest_manual"]+"</a><br />";
							}
							else
							{
								fixit += g_Strings["mictest_XX1_FIXIT2"] + "<br />";
								fixit += "<a href=\"https://rti-giken.jp/fhc/help/ref/mictest_XX3.html\" class=\"ui-link\" target=\"_blank\">"+g_Strings["mictest_manual"]+"</a><br />";
							}
							fixit += "<br />"
							fixit += g_Strings["mictest_XX1_FIXIT3"] + "<br />";
							fixit += "<a href=\"https://rti-giken.jp/fhc/help/ref/mictest_XX4.html\" class=\"ui-link\" target=\"_blank\">"+g_Strings["mictest_manual"]+"</a><br />";
							
							newObject.addClass("mictest_messge_div_succes");
						}
						else if (d['code'] == "SP")
						{
							text1 = g_Strings["mictest_SP_TEXT1"];
							fixit += g_Strings["mictest_SP_FIXIT1"] + "<br />";
							fixit += g_Strings["mictest_SP_FIXIT2"] + "<br />";
							fixit += g_Strings["mictest_SP_FIXIT3"] + "<br />";
							fixit += "<a href=\"https://rti-giken.jp/fhc/help/ref/mictest_SP.html\" class=\"ui-link\" target=\"_blank\">"+g_Strings["mictest_manual"]+"</a><br />";
						}
						else if (d['code'] == "GY")
						{
							text1 = g_Strings["mictest_GY_TEXT1"];
							fixit += g_Strings["mictest_GY_FIXIT1"] + "<br />";
							fixit += g_Strings["mictest_GY_FIXIT2"] + "<br />";
							fixit += g_Strings["mictest_GY_FIXIT3"] + "<br />";
							fixit += "<a href=\"https://rti-giken.jp/fhc/help/ref/mictest_GY.html\" class=\"ui-link\" target=\"_blank\">"+g_Strings["mictest_manual"]+"</a><br />";
						}
						else if (d['code'] == "GE")
						{
							text1 = g_Strings["mictest_GE_TEXT1"];
							fixit += g_Strings["mictest_GE_FIXIT1"] + "<br />";
							fixit += g_Strings["mictest_GE_FIXIT2"] + "<br />";
							fixit += g_Strings["mictest_GE_FIXIT3"] + "<br />";
							fixit += "<a href=\"https://rti-giken.jp/fhc/help/ref/mictest_GE.html\" class=\"ui-link\" target=\"_blank\">"+g_Strings["mictest_manual"]+"</a><br />";
						}
						else if (d['code'] == "HW")
						{
							text1 = g_Strings["mictest_HW_TEXT1"];
							fixit += g_Strings["mictest_UN_FIXIT1"] + "<br />";
							fixit += "<a href=\"https://rti-giken.jp/fhc/help/ref/mictest_HW.html\" class=\"ui-link\" target=\"_blank\">"+g_Strings["mictest_manual"]+"</a><br />";
						}
						else if (d['code'] == "SW")
						{
							text1 = g_Strings["mictest_SW_TEXT1"];
							fixit += g_Strings["mictest_UN_FIXIT1"] + "<br />";
							fixit += "<a href=\"https://rti-giken.jp/fhc/help/ref/mictest_SW.html\" class=\"ui-link\" target=\"_blank\">"+g_Strings["mictest_manual"]+"</a><br />";
						}
						else if (d['code'] == "PB")
						{
							text1 = g_Strings["mictest_PB_TEXT1"];
							text2 = "(" + d['match'] + ")";
							fixit += g_Strings["mictest_PB_FIXIT1"] + "<br />";
							fixit += g_Strings["mictest_PB_FIXIT2"] + "<br />";
							fixit += "<a href=\"https://rti-giken.jp/fhc/help/ref/mictest_PB.html\" class=\"ui-link\" target=\"_blank\">"+g_Strings["mictest_manual"]+"</a><br />";
						}
						else if (d['code'] == "LG")
						{
							text1 = g_Strings["mictest_LG_TEXT1"];
							text2 = "(" + d['match'] + ")";
							fixit += g_Strings["mictest_LG_FIXIT1"] + "<br />";
							fixit += "<a href=\"https://rti-giken.jp/fhc/help/ref/mictest_LG.html\" class=\"ui-link\" target=\"_blank\">"+g_Strings["mictest_manual"]+"</a><br />";
						}
						else if (d['code'] == "SH")
						{
							text1 = g_Strings["mictest_SH_TEXT1"];
							text2 = "(" + d['match'] + ")";
							fixit += g_Strings["mictest_SH_FIXIT1"] + "<br />";
							fixit += "<a href=\"https://rti-giken.jp/fhc/help/ref/mictest_SH.html\" class=\"ui-link\" target=\"_blank\">"+g_Strings["mictest_manual"]+"</a><br />";
						}
						else if (d['code'] == "YO")
						{
							text1 = g_Strings["mictest_YO_TEXT1"];
							text2 = "(" + d['match'] + ")";
							fixit += g_Strings["mictest_YO_FIXIT1"] + "<br />";
							fixit += g_Strings["mictest_YO_FIXIT2"] + "<br />";
							fixit += "<a href=\"https://rti-giken.jp/fhc/help/ref/mictest_YO.html\" class=\"ui-link\" target=\"_blank\">"+g_Strings["mictest_manual"]+"</a><br />";
						}
						else if (d['code'] == "FZ")
						{
							text1 = g_Strings["mictest_FZ_TEXT1"];
							text2 = "(" + d['match'] + ")";
							fixit += g_Strings["mictest_YO_FIXIT1"] + "<br />";
							fixit += g_Strings["mictest_YO_FIXIT2"] + "<br />";
							fixit += "<a href=\"https://rti-giken.jp/fhc/help/ref/mictest_FZ.html\" class=\"ui-link\" target=\"_blank\">"+g_Strings["mictest_manual"]+"</a><br />";
						}
						else if (d['code'] == "TS")
						{
							text1 = g_Strings["mictest_TS_TEXT1"];
							text2 = "(" + d['match'] + ")";
							fixit += g_Strings["mictest_YO_FIXIT1"] + "<br />";
							fixit += g_Strings["mictest_YO_FIXIT2"] + "<br />";
							fixit += "<a href=\"https://rti-giken.jp/fhc/help/ref/mictest_TS.html\" class=\"ui-link\" target=\"_blank\">"+g_Strings["mictest_manual"]+"</a><br />";
						}
						else if (d['code'] == "DD")
						{
							text1 = g_Strings["mictest_DD_TEXT1"];
							text2 = "(" + d['match'] + ")" + "[" + d['matchd'] + "]";
							fixit += g_Strings["mictest_DD_FIXIT1"] + "<br />";
							fixit += g_Strings["mictest_DD_FIXIT2"] + "<br />";
							fixit += g_Strings["mictest_DD_FIXIT3"] + "<br />";
							fixit += "<a href=\"https://rti-giken.jp/fhc/help/ref/mictest_DD.html\" class=\"ui-link\" target=\"_blank\">"+g_Strings["mictest_manual"]+"</a><br />";
						}
						else if (d['code'] == "DP")
						{
							text1 = g_Strings["mictest_DP_TEXT1"];
							text2 = "(" + d['match'] + ")" + "[" + d['matchd'] + "]";
							fixit += g_Strings["mictest_DP_FIXIT1"] + "<br />";
							fixit += g_Strings["mictest_DP_FIXIT2"] + "<br />";
							fixit += "<a href=\"https://rti-giken.jp/fhc/help/ref/mictest_DP.html\" class=\"ui-link\" target=\"_blank\">"+g_Strings["mictest_manual"]+"</a><br />";
						}
						else if (d['code'] == "DS")
						{
							text1 = g_Strings["mictest_DS_TEXT1"];
							text2 = "(" + d['match'] + ")" + "[" + d['matchd'] + "]";
							fixit += g_Strings["mictest_DS_FIXIT1"] + "<br />";
							fixit += "<a href=\"https://rti-giken.jp/fhc/help/ref/mictest_DS.html\" class=\"ui-link\" target=\"_blank\">"+g_Strings["mictest_manual"]+"</a><br />";
						}
						else if (d['code'] == "DL")
						{
							text1 = g_Strings["mictest_DL_TEXT1"];
							text2 = "(" + d['match'] + ")" + "[" + d['matchd'] + "]";
							fixit += g_Strings["mictest_DL_FIXIT1"] + "<br />";
							fixit += "<a href=\"https://rti-giken.jp/fhc/help/ref/mictest_DL.html\" class=\"ui-link\" target=\"_blank\">"+g_Strings["mictest_manual"]+"</a><br />";
						}
						else
						{
							text1 = g_Strings["mictest_UN_TEXT1"] + " code:" + d['code'];
							fixit += g_Strings["mictest_UN_FIXIT1"] + "<br />";
							fixit += "<a href=\"https://rti-giken.jp/fhc/help/ref/mictest_SW.html\" class=\"ui-link\" target=\"_blank\">"+g_Strings["mictest_manual"]+"</a><br />";
						}

						if ( ! d['file'] )
						{
							newObject.find(".mictest_tab_ul_sound").hide();
						}

						newObject.find(".mictest_messge_timef").text(d['timef']);
						newObject.find(".mictest_messge_text1").text(text1);
						newObject.find(".mictest_messge_text2").text(text2);
						var mictest_messge_data = newObject.find(".mictest_messge_data");
						newObject.find(".mictest_tab_ul_detail").click(function(){
							var pobject = $(this).parent().parent().parent();
							pobject.find(".mictest_messge_data").show();
							pobject.find(".mictest_messge_data_fixit").hide();
							pobject.find(".mictest_messge_data_sound").hide();
							pobject.find(".mictest_messge_data_detail").toggle();
						});
						newObject.find(".mictest_tab_ul_sound").click(function(){
							var pobject = $(this).parent().parent().parent();
							pobject.find(".mictest_messge_data").show();
							pobject.find(".mictest_messge_data_fixit").hide();
							pobject.find(".mictest_messge_data_detail").hide();
							pobject.find(".mictest_messge_data_sound").toggle();
						});
						newObject.find(".mictest_tab_ul_fixit").click(function(){
							var pobject = $(this).parent().parent().parent();
							pobject.find(".mictest_messge_data").show();
							pobject.find(".mictest_messge_data_sound").hide();
							pobject.find(".mictest_messge_data_detail").hide();
							pobject.find(".mictest_messge_data_fixit").toggle();
						});
						newObject.find(".mictest_messge_data_fixit").html(fixit);
						newObject.find(".mictest_messge_data_detail").text(d['timef'] + " " + d['body']);
						newObject.find(".mictest_messge_data_sound_name").text(d['file']);

						newObject.find(".mictest_messge_data_sound_play").click(function(){
							var pobject = $(this).parent().parent();
							var name = pobject.find(".mictest_messge_data_sound_name").text();
							
							var audio = new Audio("/recong/mictest/wavdownload?name=" + name);
							audio.play();
						});
						newObject.find(".mictest_messge_data_sound_download").click(function(){
							var pobject = $(this).parent().parent();
							var name = pobject.find(".mictest_messge_data_sound_name").text();

							location.href = "/recong/mictest/wavdownload?name=" + name;
						});

						var cardsize = messageObject.find(".mictest_messge_div").size();
						if(cardsize >= arr['limit'])
						{
							var cardsizecount = 0;
							messageObject.find(".mictest_messge_div").each(function(){
								if (cardsize - cardsizecount < arr['limit']) return false;
								cardsizecount++;
								var targetObject = $(this);
								targetObject.remove();
							});
						}
						
						//横からスライドしながら登場させる.
						newObject.css({marginLeft: 500 });

						messageObject.append(newObject)
						newObject.animate({marginLeft: 0});
					}
				}
				setTimeout( miclogcallbackfunction , 1000);
				return ;
			},
			error: function(d){
				setTimeout( miclogcallbackfunction , 1000);
				return ;
			}
		});
	};
	setTimeout( miclogcallbackfunction , 1000);

	$.mobile.changePage("#mictest", { transition: "flip" });
}


var g_triggerlist = null;
function triggerlist_init()
{
	if (g_triggerlist) return ;
	g_triggerlist = $("#triggerlist");
}

function trigger_load()
{
	triggerlist_init();

	var minuteFunction = function(displayObject,value)
	{
		var foundtext = "";
		displayObject.find("option").each(function(){
			if ( $(this).val() == value )
			{
				foundtext = $(this).text();
				return false;
			}
		});
		return foundtext;
	};

	var htmltemplate = g_triggerlist.find("#trigger_li_sample").html();
	var totaldom = "";
	for(var arraykey in g_Trigger)
	{
		var s = arraykey.split("_");
		if ( !(s[0] == "trigger" && s[2] == "if" && s[3] == "type" && s[4] == undefined )) continue;
		var key = s[1];

		var html = htmltemplate;
		html = html.replace(/%TRIGGERKEY%/g,key);
		html = html.replace(/%TRIGGERSTOPPER%/g,g_Trigger["trigger_" + key + "_lastfire_date_string"] );
		if ( parseInt(g_Trigger["trigger_" + key + "_enable"]) )
		{
			html = html.replace(/%TRIGGERSWITCH%/g,
				"<select data-role='slider' onchange='trigger_enable_change(" + key + ",this)'>" + 
				"	<option value='0' >" + g_Strings["mukou"] + "</option>" + 
				"	<option value='1' selected>" + g_Strings["yukou"] + "</option>" + 
				"</select>");
		}
		else
		{
			html = html.replace(/%TRIGGERSWITCH%/g,
				"<select data-role='slider' onchange='trigger_enable_change(" + key + ",this)'>" + 
				"	<option value='0' selected>" + g_Strings["mukou"] + "</option>" + 
				"	<option value='1' >" + g_Strings["yukou"] + "</option>" + 
				"</select>");
		}

		var sss = "";
		var type = g_Trigger["trigger_" + key + "_if_type"];
		if (type == "date")
		{
			var hour   = g_Trigger["trigger_" + key + "_if_date_hour"];
			var minute = g_Trigger["trigger_" + key + "_if_date_minute"];
			var if_date_loop_hourly = parseInt(g_Trigger["trigger_" + key + "_if_date_loop_hourly"]);
			if (if_date_loop_hourly > 0)
			{
				hour = "(*/" + if_date_loop_hourly+")";
			}

			sss += "<div><span class='thumtoppix'>" + g_Strings["jikan"] + "</span> <span class='thumvalue'>" + h(hour) + ":" + h(minute) + "</span>";
			if ( parseInt(g_Trigger["trigger_" + key + "_if_date_loop_use"]) )
			{
				sss += "(";
				var type = parseInt(g_Trigger["trigger_" + key + "_if_date_loop_dayofweek"]);
				if ( type & 1 ) sss += g_Strings["week0"];
				if ( type & 2 ) sss += g_Strings["week1"];
				if ( type & 4 ) sss += g_Strings["week2"];
				if ( type & 8 ) sss += g_Strings["week3"];
				if ( type & 16) sss += g_Strings["week4"];
				if ( type & 32) sss += g_Strings["week5"];
				if ( type & 64) sss += g_Strings["week6"];
				var holiday = parseInt(g_Trigger["trigger_" + key + "_if_date_loop_holiday"]);
				if (holiday == 2)
				{
					sss += " " + g_Strings["syukujitu_wo_nozoku"];
				}
				else if (holiday == 3)
				{
					sss += " " + g_Strings["syukujitu_nomi"];
				}
				sss += ")";
			}
			sss += "</div>"
		}
		else if (type == "command")
		{
			sss += "<div><span class='thumtoppix'>" + g_Strings["command"] + "</span> " + h(g_Trigger["trigger_" + key + "_if_command"]);
			sss += "</div>"
		}
		else if (type == "web")
		{
			sss += "<div><span class='thumtoppix'>Web</span> " + h(g_Trigger["trigger_" + key + "_if_web_url"]);
			sss += "</div>"
		}

		//表示スペースの関係で追加条件はまとめて一行にする
		var aaa = "";
		if ( parseInt(g_Trigger["trigger_" + key + "_if_weather_use"]) )
		{
			aaa += "<span class='appendthumtoppix'>(" + g_Strings["tenki"] + ":" ;
			var expr = g_Trigger["trigger_" + key + "_if_weather_value"];
			if (expr == "rain")	aaa += g_Strings["amega_furu"];
			else if (expr == "not_rain")	aaa += g_Strings["amega_denai"];
			if (expr == "clear")	aaa += g_Strings["hare"];
			else if (expr == "clouds")	aaa += g_Strings["kumori"];
			if (expr == "soon_rain")	aaa += g_Strings["mosugu_ame"];
			else if (expr == "soon_clear")	aaa += g_Strings["mosugu_ame_yamu"];
			aaa += ")</span>"
		}
		if ( parseInt(g_Trigger["trigger_" + key + "_if_sound_use"]) )
		{
			aaa += "<span class='appendthumtoppix'>(";
			var expr = g_Trigger["trigger_" + key + "_if_sound_expr"];
			if (expr == "equal")	aaa += g_Strings["hitoga_irutoki"];
			else if (expr == "not")	aaa += g_Strings["hitoga_inaitoki"];
			aaa += ")</span>"
		}
		if ( parseInt(g_Trigger["trigger_" + key + "_if_elec_use"]) )
		{
			aaa += "<span class='appendthumtoppix'>(" + g_Trigger["trigger_" + key + "_if_elec_type"] + " " +g_Trigger["trigger_" + key + "_if_elec_action"] + " ";
			var expr = g_Trigger["trigger_" + key + "_if_elec_expr"];
			if (expr == "equal")	aaa += g_Strings["notoki"];
			else if (expr == "not")	aaa += g_Strings["igai"];
			aaa += ")</span>"
		}
		if ( parseInt(g_Trigger["trigger_" + key + "_if_stopper_use"]) )
		{
			aaa += "<span class='appendthumtoppix'>(" + g_Strings["zenkaiyori"] + minuteFunction( $("#trigger_if_stopper_minute") ,g_Trigger["trigger_" + key + "_if_stopper_minute"]) ;
			aaa += ")</span>"
		}
		if (aaa != "")
		{
			sss += "<div><span class='thumtoppix'>+</span>" + aaa + "</div>";
		}

		var name = g_Trigger["trigger_" + key + "_display_name"];
		if ( name && name != "")
		{
			sss += "<div class='triggername'>" + h(name) + "</div>";
		}

		html = html.replace(/%TRIGGERSAMPLE%/g,sss );
		totaldom += html;
	}
	g_triggerlist.find("#trigger_menu_ul").html(totaldom);

	g_triggerlist.trigger( "create" );
	$.mobile.changePage("#triggerlist", { transition: "flip" });
}

function trigger_delete(key)
{
	//いたずら防止のために補正する.
	key = parseInt(key);

	delConfirm(function(e){
		var arr = {};
		arr["key"] = key;
		arr["_csrftoken"] = g_CSRFTOKEN;

		//update待ちにする
		showUpdatewaitDialog();

		//サーバにデータを送信
		$.ajax({
				type: "post",
				url: "/setting/trigger/delete",
				timeout: 50000,
				data: arr,
				dataType: 'json',
				success: function(d){
					if ( d['result'] != "ok" )
					{
						alertErrorDialog(d,"trigger_delete");
						return ;
					}

					//画面の情報を全入れ替え
					g_Trigger = d;

					g_triggerlist.find("#trigger_menu_ul li").each(function(){
						var itemObject = $(this);
						var triggerkey = itemObject.find(".id").text();
						if (triggerkey == key)
						{
							itemObject.hide();
							return false;
						}
						return true;
					});
					hideUpdatewaitDialog();
					return ;
				},
				error: function(d){
					hideUpdatewaitDialog();
					alertErrorDialog(d,"trigger_delete");
					return ;
				}
		});
	}
	,function(e){
		hideUpdatewaitDialog();
	} );
}

function trigger_enable_change(key,sliderObject)
{
	var arr = {};
	arr['key'] = parseInt(key);
	arr['enable'] = $(sliderObject).val();
	arr['_csrftoken'] = g_CSRFTOKEN;

	//サーバにデータを送信
	$.ajax({
			type: "post",
			url: "/setting/trigger/enable_change",
			timeout: 50000,
			data: arr,
			dataType: 'json',
			success: function(d){
				if ( d['result'] != "ok" )
				{
					alertErrorDialog(d,"trigger_enable_change");
					return ;
				}
				//画面の情報を全入れ替え
				g_Trigger = d;
				return ;
			},
			error: function(d){
				alertErrorDialog(d,"trigger_enable_change");
				return ;
			}
	});
}



var g_triggerset = null;
function triggerset_init()
{
	if (g_triggerset) return ;
	g_triggerset = $("#triggerset");
	
	//設定するボタンをクリックした時
	g_triggerset.find("#trigger_apply_button").click(function(){
		//読込中にする
		showUpdatewaitDialog();

		g_triggerset.find(".error").hide();

		var arr= {};
		arr['trigger_key'] = g_triggerset.find("#trigger_key").val();	//値
		arr['trigger_if_type'] = g_triggerset.find("#trigger_if_type").val();	//条件
		arr['trigger_if_date_hour'] = g_triggerset.find("#trigger_if_date_hour").val();	//時間
		arr['trigger_if_date_minute'] = g_triggerset.find("#trigger_if_date_minute").val();	//
		arr['trigger_if_date_loop_use'] = g_triggerset.find("#trigger_if_date_loop_use").prop("checked") ? 1 : 0;	//

		var dayofweek = 0;
		if (g_triggerset.find("#trigger_if_date_loop_dayofweek_1").prop("checked")) dayofweek |= 1;
		if (g_triggerset.find("#trigger_if_date_loop_dayofweek_2").prop("checked")) dayofweek |= 2;
		if (g_triggerset.find("#trigger_if_date_loop_dayofweek_4").prop("checked")) dayofweek |= 4;
		if (g_triggerset.find("#trigger_if_date_loop_dayofweek_8").prop("checked")) dayofweek |= 8;
		if (g_triggerset.find("#trigger_if_date_loop_dayofweek_16").prop("checked")) dayofweek |= 16;
		if (g_triggerset.find("#trigger_if_date_loop_dayofweek_32").prop("checked")) dayofweek |= 32;
		if (g_triggerset.find("#trigger_if_date_loop_dayofweek_64").prop("checked")) dayofweek |= 64;
		arr['trigger_if_date_loop_dayofweek'] = dayofweek;
		
		var holiday = 0;
		if (g_triggerset.find("#trigger_if_date_loop_holiday_1").prop("checked")) holiday = 1;
		if (g_triggerset.find("#trigger_if_date_loop_holiday_2").prop("checked")) holiday = 2;
		if (g_triggerset.find("#trigger_if_date_loop_holiday_3").prop("checked")) holiday = 3;
		arr['trigger_if_date_loop_holiday'] = holiday;
		arr['trigger_if_date_loop_holiday_dataof'] = g_triggerset.find("#trigger_if_date_loop_holiday_dataof").val();	//祝日データ
		arr['trigger_if_date_loop_hourly'] = g_triggerset.find("#trigger_if_date_loop_hourly").val();					//毎時
		

		arr['trigger_if_command'] = g_triggerset.find("#trigger_if_command").val();			//コマンド本体
		for( var i = 1 ; i <= 5 ; i ++ )
		{
			arr["trigger_if_command_select_args" + i] = g_triggerset.find("#trigger_if_command_select_args" + i).val();		//引数
		}
		arr['trigger_if_command_pooling'] = g_triggerset.find("#trigger_if_command_pooling").val();	//ポーリング間隔
		arr['trigger_if_web_url'] = g_triggerset.find("#trigger_if_web_url").val();	//URL
		arr['trigger_if_web_string'] = g_triggerset.find("#trigger_if_web_string").val();	//文字列
		arr['trigger_if_web_pooling'] = g_triggerset.find("#trigger_if_web_pooling").val();	//ポーリング間隔

		arr['trigger_if_elec_use'] = g_triggerset.find("#trigger_if_elec_use").prop("checked") ? 1 : 0;	//家電を条件
		arr['trigger_if_elec_type'] = g_triggerset.find("#trigger_if_elec_type").val();
		arr['trigger_if_elec_action'] = g_triggerset.find("#trigger_if_elec_action").val();	//アクション
		arr['trigger_if_elec_expr'] = g_triggerset.find("#trigger_if_elec_expr").val();	//条件

		arr['trigger_if_stopper_use'] = g_triggerset.find("#trigger_if_stopper_use").prop("checked") ? 1 : 0;	//前回動作から
		arr['trigger_if_stopper_minute'] = g_triggerset.find("#trigger_if_stopper_minute").val();

		arr['trigger_if_sound_use'] = g_triggerset.find("#trigger_if_sound_use").prop("checked") ? 1 : 0;	//近くに人がいるとき
		arr['trigger_if_sound_expr'] = g_triggerset.find("#trigger_if_sound_expr").val();					//近くに人がいるとき
		arr['trigger_if_sound_value'] = g_triggerset.find("#trigger_if_sound_value").val();					//人がいるレベル

		arr['trigger_if_weather_use'] = g_triggerset.find("#trigger_if_weather_use").prop("checked") ? 1 : 0;		//天気
		arr['trigger_if_weather_address'] = g_triggerset.find("#trigger_if_weather_address").val();					//天気の場所
		arr['trigger_if_weather_value'] = g_triggerset.find("#trigger_if_weather_value").val();		//天気の種類(晴れとか)

		arr['trigger_then_tospeak_select'] = g_triggerset.find("#trigger_then_tospeak_select").val();	//
		arr['trigger_then_tospeak_string'] = g_triggerset.find("#trigger_then_tospeak_string").val();	//文字列
		arr['trigger_then_tospeak_mp3'] = g_triggerset.find("#trigger_then_tospeak_mp3").text();	//mp3

		arr['trigger_then_macro_use'] = g_triggerset.find("#trigger_then_macro_use").prop("checked") ? 1 : 0;	//家電を操作する
		for( var i = 1 ; i <= 9 ; i ++ )
		{
			arr["trigger_then_macro_" + i + "_elec"] = g_triggerset.find("#trigger_then_macro_" + i + "_elec").val();
			arr["trigger_then_macro_" + i + "_action"] = g_triggerset.find("#trigger_then_macro_" + i + "_action").val();
			arr["trigger_then_macro_" + i + "_after"] = g_triggerset.find("#trigger_then_macro_" + i + "_after").val();
		}
		arr['trigger_then_command_use'] = g_triggerset.find("#trigger_then_command_use").prop("checked") ? 1: 0;	//コマンドを操作する
		arr['trigger_then_command'] = g_triggerset.find("#trigger_then_command").val();			//コマンド本体
		for( var i = 1 ; i <= 5 ; i ++ )
		{
			arr["trigger_then_command_select_args" + i] = g_triggerset.find("#trigger_then_command_select_args" + i).val();		//引数
		}
		arr["trigger_display_name"] = g_triggerset.find("#trigger_display_name").val();
		arr['_csrftoken'] = g_CSRFTOKEN;

		$.ajax({
			type: "post",
			url: "/setting/trigger/update",
			timeout: 50000,
			data: arr,
			dataType: 'json',
			success: function(d){
				hideUpdatewaitDialog();

				if (d['result'] != "ok")
				{//エラーになりました
					g_triggerset.find(".error_"+d['code']).show();
					g_triggerset.find(".bunner_error").show();
					return ;
				}

				g_Trigger = d;

				trigger_load();
				return ;
			},
			error: function(d){
				hideUpdatewaitDialog();

				g_triggerset.find(".bunner_error").show();
				return ;
			}
		});
	});
	
	
	g_triggerset.find("#trigger_if_type").change(function(){
		var s = $(this).val();
		if ( s == "date" )
		{
			g_triggerset.find("#trigger_if_web_div").hide();
			g_triggerset.find("#trigger_if_command_div").hide();
			g_triggerset.find("#trigger_if_date_div").show();
		}
		else if ( s == "command" )
		{
			g_triggerset.find("#trigger_if_web_div").hide();
			g_triggerset.find("#trigger_if_date_div").hide();
			g_triggerset.find("#trigger_if_command_div").show();
		}
		else if ( s == "web" )
		{
			g_triggerset.find("#trigger_if_date_div").hide();
			g_triggerset.find("#trigger_if_command_div").hide();
			g_triggerset.find("#trigger_if_web_div").show();
		}
	});
	g_triggerset.find("#trigger_if_date_loop_hourly").change(function(){
		var x = parseInt($(this).val());
		if (x <= 0 )
		{
			g_triggerset.find("#trigger_if_date_loop_hourly_banner").text("");
			return ;
		}
		var banner = "";
		var minute = g_triggerset.find("#trigger_if_date_minute").val();
		for(var h = 0 ; h < 24 ; h += x)
		{
			banner += " " + h + ":" + minute + " ";
		}
		g_triggerset.find("#trigger_if_date_loop_hourly_banner").html("毎時指定のため、以下の時刻に動きます。" + "<br />" + banner);
	});

	g_triggerset.find("#trigger_if_weather_preview_button").click(function(){
		var arr = {};
		arr["address"] = g_triggerset.find("#trigger_if_weather_address").val();
		arr["value"] = g_triggerset.find("#trigger_if_weather_value").val();
		arr["_csrftoken"] = g_CSRFTOKEN;

		showUpdatewaitDialog();
		$.ajax({
			type: "post",
			url: "/trigger/preview/if_weather",
			timeout: 50000,
			data: arr,
			dataType: 'json',
			success: function(d){
				hideUpdatewaitDialog();
				if ( d['result'] != "ok" )
				{
					g_triggerset.find("#trigger_if_weather_preview_out").text(d['message']).show();
					alertErrorDialog(d,"trigger_if_weather_preview_button");
					return ;
				}
				var html = "";
				if (d['match'] == "1")
				{
					html += g_Strings["match_done"] + "<br /><br />";
				}
				else
				{
					html += g_Strings["match_not"] + "<br /><br />";
				}
				html += g_Strings['result'] + ": " + h(d['returnValue']) + "<br />";
				html += g_Strings['stdout'] + ": " + longText(d['stdout'],g_Strings["click_de"],15) + "<br />";

				g_triggerset.find("#trigger_if_weather_preview_out").html(html).show();
			},
			error: function(d){
				hideUpdatewaitDialog();
				g_triggerset.find("#trigger_if_weather_preview_out").text(d.responseText).show();
				return ;
			}
		});
	});

	g_triggerset.find("#trigger_if_date_loop_use").change(function(){
		showhidden(g_triggerset.find("#trigger_if_date_loop_use_div"),$(this).prop("checked") );
	});
	g_triggerset.find("#trigger_if_elec_use").change(function(){
		showhidden(g_triggerset.find("#trigger_if_elec_use_div"),$(this).prop("checked") );
	});
	//家電一覧の埋め込み
	g_triggerset.find("#trigger_if_elec_type").change(function(){
		make_macro_combo_action(false,$(this).val() , g_triggerset.find("#trigger_if_elec_action")  );
	});

	g_triggerset.find("#trigger_if_stopper_use").change(function(){
		showhidden(g_triggerset.find("#trigger_if_stopper_use_div"),$(this).prop("checked") );
	});

	g_triggerset.find("#trigger_if_sound_use").change(function(){
		showhidden(g_triggerset.find("#trigger_if_sound_use_div"),$(this).prop("checked") );
		if ($(this).prop("checked"))
		{
			g_triggerset.find("#trigger_if_sound_sample_reload_button").click();
		}
	});
	g_triggerset.find("#trigger_if_sound_sample_reload_button").click(function(){
		sample_sensor_reload(g_triggerset,"sound","trigger_if_sound_sample");
	});

	g_triggerset.find("#trigger_if_weather_use").change(function(){
		showhidden(g_triggerset.find("#trigger_if_weather_use_div"),$(this).prop("checked") );
	});

	g_triggerset.find("#trigger_then_command_use").change(function(){
		showhidden(g_triggerset.find("#trigger_then_command_div"),$(this).prop("checked") );
	});
	g_triggerset.find("#trigger_then_macro_use").change(function(){
		showhidden(g_triggerset.find("#trigger_then_macro_div"),$(this).prop("checked") );
	});
	//読み上げ text/mp3選択
	tospeak_command(g_triggerset,"trigger_then_tospeak");

	//コマンドのプレビューボタン
	g_triggerset.find("#trigger_if_command_preview_button").click(function(){
		var arr = {};
		arr["filename"] = g_triggerset.find("#trigger_if_command").val();
		if (arr["filename"] == "") return ;

		for(var i = 1 ; i <= 5 ; i ++)
		{
			arr["args"+i] = g_triggerset.find("#trigger_if_command_select_args"+i).val();
		}
		arr["_csrftoken"] = g_CSRFTOKEN;

		showUpdatewaitDialog();
		$.ajax({
			type: "post",
			url: "/trigger/preview/if_command",
			timeout: 50000,
			data: arr,
			dataType: 'json',
			success: function(d){
				hideUpdatewaitDialog();
				if ( d['result'] != "ok" )
				{
					g_triggerset.find("#trigger_if_command_preview_out").text(d['message']).show();
					alertErrorDialog(d,"trigger_if_command_preview_button");
					return ;
				}
				var html = "";
				if (d['match'] == "1")
				{
					html += g_Strings["match_done"] + "<br /><br />";
				}
				else
				{
					html += g_Strings["match_not"] + "<br /><br />";
				}
				html += g_Strings['result'] + ": " + h(d['returnValue']) + "<br />";
				html += g_Strings['stdout'] + ": " + longText(d['stdout'],g_Strings["click_de"],15) + "<br />";

				g_triggerset.find("#trigger_if_command_preview_out").html(html).show();
			},
			error: function(d){
				hideUpdatewaitDialog();
				g_triggerset.find("#trigger_if_command_preview_out").text(d.responseText).show();
				return ;
			}
		});
	});

	//コマンドのイベント
	g_triggerset.find("#trigger_if_command_select_button").click(function(){
		fileselectpage_load( "script_command" , g_triggerset.find("#trigger_if_command").val() , g_Strings["SELECT_COMMAND_FILE"]
		, function(filename){
			action_command(filename,g_triggerset,"trigger_if_command");
		});
	});

	//webのプレビューボタン
	g_triggerset.find("#trigger_if_web_preview_button").click(function(){
		var arr = {};
		arr["url"] = g_triggerset.find("#trigger_if_web_url").val();
		if (arr["url"] == "") return ;

		arr["string"] = g_triggerset.find("#trigger_if_web_string").val();
		arr["_csrftoken"] = g_CSRFTOKEN;

		showUpdatewaitDialog();
		$.ajax({
			type: "post",
			url: "/trigger/preview/if_web",
			timeout: 50000,
			data: arr,
			dataType: 'json',
			success: function(d){
				hideUpdatewaitDialog();
				if ( d['result'] != "ok" )
				{
					g_triggerset.find("#trigger_if_web_preview_out").text(d['message']).show();
					alertErrorDialog(d,"trigger_if_web_preview_button");
					return ;
				}
				var html = "";
				if (d['match'] == "1")
				{
					html += g_Strings["match_done"] + "<br /><br />";
				}
				else
				{
					html += g_Strings["match_not"] + "<br /><br />";
				}

				if (d['regex'] == "1")
				{
					html += g_Strings["regex_de"] + "<br />";
				}
				if (d['error'] && d['error'] !="")
				{
					html += g_Strings["error_ga"] + " " + h(d['error']) + "<br />";
				}
				html += g_Strings["content_encode"] + ": " + h(d['encoding']) + "<br />";
				html += g_Strings["content"] + ": " + longText(d['html'],g_Strings["click_de"],15) + "<br />";

				g_triggerset.find("#trigger_if_web_preview_out").html(html).show();
			},
			error: function(d){
				hideUpdatewaitDialog();
				g_triggerset.find("#trigger_if_web_preview_out").text(d.responseText).show();
				return ;
			}
		});
	});

	//失敗した。トリガーだけ命名規則を間違えてしまった。 trigger_then_command_select_args*
	function trigger_command_preview(obj,prefix)
	{
		var arr = {};
		arr["filename"] = obj.find("#" + prefix + "_command").val();
		if (arr["filename"] == "") return ;

		for(var i = 1 ; i <= 5 ; i ++)
		{
			arr["args"+i] = obj.find("#" + prefix + "_command_select_args"+i).val();
		}
		arr["_csrftoken"] = g_CSRFTOKEN;

		showUpdatewaitDialog();
		$.ajax({
			type: "post",
			url: "/remocon/preview/elec_command",
			timeout: 50000,
			data: arr,
			dataType: 'json',
			success: function(d){
				hideUpdatewaitDialog();
				if ( d['result'] != "ok" )
				{
					obj.find("#" + prefix + "_command_preview_error").text(d['message']).show();
					alertErrorDialog(d,"remocon_preview_elec_command");
					return ;
				}
				obj.find("#" + prefix + "_command_preview_error").hide();
			},
			error: function(d){
				hideUpdatewaitDialog();
				obj.find("#" + prefix + "_command_preview_error").text(d.responseText).show();
				return ;
			}
		});
	}
	//コマンドのプレビューボタン
	g_triggerset.find("#trigger_then_command_preview_button").click(function(){
		trigger_command_preview(g_triggerset,"trigger_then");
	});
	//コマンド選択
	g_triggerset.find("#trigger_then_command_select_button").click(function(){
		fileselectpage_load( "script_command" , g_triggerset.find("#trigger_then_command").val() , g_Strings["SELECT_COMMAND_FILE"]
		, function(filename){
			action_command(filename,g_triggerset,"trigger_then_command");
		});
	});

	//マクロのプレビューボタン
	g_triggerset.find("#trigger_then_macro_preview_button").click(function(){
		macro_preview(g_triggerset,"trigger_then_macro");
	});
	//マクロのイベント
	macro_set(g_triggerset,"trigger_then_macro")
}

function triggerset_load(key)
{
	triggerset_init();

	key = parseIntOrNew(key);

	g_triggerset.find("#trigger_key").val(key).change();

	//エラーを消す.
	g_triggerset.find(".error").hide();
	//通知も消す
	g_triggerset.find(".notify").hide();

	//最後にトリガーを実行した時間.
	var lastfireTime = 0;
	
	if (key == "new")
	{//新規作成
		var nowDate = new Date();
		g_triggerset.find("#trigger_if_type").val("date").change();	//条件
		g_triggerset.find("#trigger_if_date_hour").val(nowDate.getHours()).change();	//現在の時間をセット
		g_triggerset.find("#trigger_if_date_minute").val(nowDate.getMinutes()).change();	//
		g_triggerset.find("#trigger_if_date_loop_use").prop("checked",0).change();	//曜日を条件にする
		g_triggerset.find("#trigger_if_date_loop_dayofweek_1").prop("checked",0).change();	//日曜日
		g_triggerset.find("#trigger_if_date_loop_dayofweek_2").prop("checked",0).change();	//
		g_triggerset.find("#trigger_if_date_loop_dayofweek_4").prop("checked",0).change();	//
		g_triggerset.find("#trigger_if_date_loop_dayofweek_8").prop("checked",0).change();	//
		g_triggerset.find("#trigger_if_date_loop_dayofweek_16").prop("checked",0).change();	//
		g_triggerset.find("#trigger_if_date_loop_dayofweek_32").prop("checked",0).change();	//
		g_triggerset.find("#trigger_if_date_loop_dayofweek_64").prop("checked",0).change();	//土曜日
		g_triggerset.find("#trigger_if_date_loop_holiday_1").prop("checked",true).change();	//祝日関係なく
		g_triggerset.find("#trigger_if_date_loop_holiday_dataof").val("japan").change();	//祝日データ
		g_triggerset.find("#trigger_if_date_loop_hourly").val("0").change();	//祝日データ

		g_triggerset.find("#trigger_if_command").val("").change();			//コマンド本体
		for( var i = 1 ; i <= 5 ; i ++ )
		{
			g_triggerset.find("#trigger_if_command_select_args" + i).val("").change();		//引数
		}
		action_command("",g_triggerset,"trigger_if_command");
		g_triggerset.find("#trigger_if_command_pooling").val("60minute").change();	//ポーリング間隔
		g_triggerset.find("#trigger_if_web_url").val("").change();	//URL
		g_triggerset.find("#trigger_if_web_string").val("").change();	//文字列
		g_triggerset.find("#trigger_if_web_pooling").val("60minute").change();	//ポーリング間隔

		g_triggerset.find("#trigger_if_elec_use").prop("checked",0).change();	//家電を条件
		make_macro_combo_elec(false, g_triggerset.find("#trigger_if_elec_type") ).val("").change();
		make_macro_combo_action(false,g_triggerset.find("#trigger_if_elec_type").val() , g_triggerset.find("#trigger_if_elec_action")  );
		g_triggerset.find("#trigger_if_elec_expr").val("equal").change();	//条件

		g_triggerset.find("#trigger_if_stopper_use").prop("checked",0).change();	//特定時間
		g_triggerset.find("#trigger_if_stopper_minute").val("1440minute");			//分

		g_triggerset.find("#trigger_if_sound_use").prop("checked",0).change();	//近くに人がいる
		g_triggerset.find("#trigger_if_sound_expr").val("equal").change();	//近くに人がいる
		g_triggerset.find("#trigger_if_sound_value").val(20).change();	//20回音が鳴っていれば人がいると判断する

		g_triggerset.find("#trigger_if_weather_use").prop("checked",0).change();	//天気
		g_triggerset.find("#trigger_if_weather_address").val(g_Strings["tokyo"]).change();	//天気の場所のディフォルトは東京都
		g_triggerset.find("#trigger_if_weather_value").val("not_rain").change();//今日は晴れであるとき

		g_triggerset.find("#trigger_then_tospeak_select").val("string").change();	//文字列
		g_triggerset.find("#trigger_then_tospeak_string").val("").change();	//URL
		g_triggerset.find("#trigger_then_tospeak_mp3").text("").change();	//URL
		g_triggerset.find("#trigger_then_macro_use").prop("checked",1).change();	//家電を操作する
		for( var i = 1 ; i <= 9 ; i ++ )
		{
			make_macro_combo_elec(true,g_triggerset.find("#trigger_then_macro_" + i + "_elec")).val("").change();
			g_triggerset.find("#trigger_then_macro_" + i + "_action").val("").change();
			g_triggerset.find("#trigger_then_macro_" + i + "_after").val("").change();
		}
		g_triggerset.find("#trigger_then_command_use").prop("checked",0).change();	//コマンドを操作する
		g_triggerset.find("#trigger_then_command").val("").change();			//コマンド本体
		for( var i = 1 ; i <= 5 ; i ++ )
		{
			g_triggerset.find("#trigger_then_command_select_args" + i).val("").change();		//引数
		}
		action_command("",g_triggerset,"trigger_then_command");

		//文字の色
		g_triggerset.find("#trigger_display_name").val("").change();

		//最後に実行した時間
		lastfireTime = 0;
	}
	else
	{//新規作成でないので、データを取得してくる.
		key = parseInt(key);

		g_triggerset.find("#trigger_if_type").val(g_Trigger["trigger_" + key + "_if_type"]).change();	//条件
		g_triggerset.find("#trigger_if_date_hour").val(parseInt( g_Trigger["trigger_" + key + "_if_date_hour"] )).change();	//現在の時間をセット
		g_triggerset.find("#trigger_if_date_minute").val(parseInt( g_Trigger["trigger_" + key + "_if_date_minute"] )).change();	//
		g_triggerset.find("#trigger_if_date_loop_use").prop("checked",parseInt( g_Trigger["trigger_" + key + "_if_date_loop_use"] )).change();	//曜日を条件にする
		var if_date_loop_dayofweek = parseInt( g_Trigger["trigger_" + key + "_if_date_loop_dayofweek"] );
		g_triggerset.find("#trigger_if_date_loop_dayofweek_1").prop("checked",if_date_loop_dayofweek & 1).change();	//日曜日
		g_triggerset.find("#trigger_if_date_loop_dayofweek_2").prop("checked",if_date_loop_dayofweek & 2).change();	//
		g_triggerset.find("#trigger_if_date_loop_dayofweek_4").prop("checked",if_date_loop_dayofweek & 4).change();	//
		g_triggerset.find("#trigger_if_date_loop_dayofweek_8").prop("checked",if_date_loop_dayofweek & 8).change();	//
		g_triggerset.find("#trigger_if_date_loop_dayofweek_16").prop("checked",if_date_loop_dayofweek & 16).change();	//
		g_triggerset.find("#trigger_if_date_loop_dayofweek_32").prop("checked",if_date_loop_dayofweek & 32).change();	//
		g_triggerset.find("#trigger_if_date_loop_dayofweek_64").prop("checked",if_date_loop_dayofweek & 64).change();	//土曜日
		g_triggerset.find("#trigger_if_date_loop_holiday_" +  parseInt(g_Trigger["trigger_" + key + "_if_date_loop_holiday"]) ).prop("checked",1).change();	//祝日関係なく
		g_triggerset.find("#trigger_if_date_loop_holiday_dataof").val(parseInt( g_Trigger["trigger_" + key + "_if_date_loop_holiday_dataof"] )).change();	//祝日データ
		g_triggerset.find("#trigger_if_date_loop_hourly").val(parseInt( g_Trigger["trigger_" + key + "_if_date_loop_hourly"] )).change();	//毎時

		g_triggerset.find("#trigger_if_command").val(g_Trigger["trigger_" + key + "_if_command"] ).change();			//コマンド本体
		for( var i = 1 ; i <= 5 ; i ++ )
		{
			g_triggerset.find("#trigger_if_command_select_args" + i).val(g_Trigger["trigger_" + key + "_if_command_select_args" + i]).change();		//引数
		}
		action_command(g_Trigger["trigger_" + key + "_if_command"],g_triggerset,"trigger_if_command");
		g_triggerset.find("#trigger_if_command_pooling").val(g_Trigger["trigger_" + key + "_if_command_pooling"]).change();	//ポーリング間隔

		g_triggerset.find("#trigger_if_elec_use").prop("checked",parseInt( g_Trigger["trigger_" + key + "_if_elec_use"] )).change();	//家電を条件
		make_macro_combo_elec(false,g_triggerset.find("#trigger_if_elec_type")).val(g_Trigger["trigger_" + key + "_if_elec_type"]).change();
		g_triggerset.find("#trigger_if_elec_action").val(g_Trigger["trigger_" + key + "_if_elec_action"]).change();		//アクション
		g_triggerset.find("#trigger_if_elec_expr").val(g_Trigger["trigger_" + key + "_if_elec_expr"]).change();	//条件

		g_triggerset.find("#trigger_if_stopper_use").prop("checked",parseInt( g_Trigger["trigger_" + key + "_if_stopper_use"] ) ).change();	//特定時間
		g_triggerset.find("#trigger_if_stopper_minute").val(g_Trigger["trigger_" + key + "_if_stopper_minute"]);			//分

		g_triggerset.find("#trigger_if_sound_use").prop("checked",parseInt( g_Trigger["trigger_" + key + "_if_sound_use"] ) ).change();	//人が近くいる
		g_triggerset.find("#trigger_if_sound_expr").val(g_Trigger["trigger_" + key + "_if_sound_expr"]).change();	//近くに人がいる
		g_triggerset.find("#trigger_if_sound_value").val(parseInt( g_Trigger["trigger_" + key + "_if_sound_value"] ) ).change();	//鳴っていれば人がいると判断するしきい値

		g_triggerset.find("#trigger_if_weather_use").prop("checked",parseInt( g_Trigger["trigger_" + key + "_if_weather_use"] ) ).change();	//天気
		g_triggerset.find("#trigger_if_weather_address").val( g_Trigger["trigger_" + key + "_if_weather_address"] ).change();	//天気の場所のディフォルトは東京都
		g_triggerset.find("#trigger_if_weather_value").val(g_Trigger["trigger_" + key + "_if_weather_value"] ).change();//今日は晴れであるとき

		g_triggerset.find("#trigger_if_web_url").val(g_Trigger["trigger_" + key + "_if_web_url"]).change();	//URL
		g_triggerset.find("#trigger_if_web_string").val(g_Trigger["trigger_" + key + "_if_web_string"]).change();	//文字列
		g_triggerset.find("#trigger_if_web_pooling").val(g_Trigger["trigger_" + key + "_if_web_pooling"]).change();	//ポーリング間隔

		g_triggerset.find("#trigger_then_tospeak_select").val(g_Trigger["trigger_" + key + "_then_tospeak_select"]).change();	//文字列
		g_triggerset.find("#trigger_then_tospeak_string").val(g_Trigger["trigger_" + key + "_then_tospeak_string"]).change();	//URL
		g_triggerset.find("#trigger_then_tospeak_mp3").text(g_Trigger["trigger_" + key + "_then_tospeak_mp3"]).change();	//URL
		g_triggerset.find("#trigger_then_macro_use").prop("checked",parseInt(g_Trigger["trigger_" + key + "_then_macro_use"]) ).change();	//家電を操作する
		for( var i = 1 ; i <= 9 ; i ++ )
		{
			make_macro_combo_elec(true,g_triggerset.find("#trigger_then_macro_" + i + "_elec")).val(g_Trigger["trigger_" + key + "_then_macro_" + i + "_elec"]).change();
			g_triggerset.find("#trigger_then_macro_" + i + "_action").val(g_Trigger["trigger_" + key + "_then_macro_" + i + "_action"]).change();
			g_triggerset.find("#trigger_then_macro_" + i + "_after").val(g_Trigger["trigger_" + key + "_then_macro_" + i + "_after"]).change();
		}
		g_triggerset.find("#trigger_if_command_use").prop("checked",parseInt( g_Trigger["trigger_" + key + "_if_command_use"] )).change();	//コマンドを条件
		g_triggerset.find("#trigger_if_command").val(g_Trigger["trigger_" + key + "_if_command"] ).change();			//コマンド本体
		for( var i = 1 ; i <= 5 ; i ++ )
		{
			g_triggerset.find("#trigger_if_command_select_args" + i).val(g_Trigger["trigger_" + key + "_if_command_select_args" + i]).change();		//引数
		}
		action_command(g_Trigger["trigger_" + key + "_if_command"],g_triggerset,"trigger_if_command");

		g_triggerset.find("#trigger_then_command_use").prop("checked",parseInt( g_Trigger["trigger_" + key + "_then_command_use"] )).change();	//コマンドを条件
		g_triggerset.find("#trigger_then_command").val(g_Trigger["trigger_" + key + "_then_command"] ).change();			//コマンド本体
		for( var i = 1 ; i <= 5 ; i ++ )
		{
			g_triggerset.find("#trigger_then_command_select_args" + i).val(g_Trigger["trigger_" + key + "_then_command_select_args" + i]).change();		//引数
		}
		action_command(g_Trigger["trigger_" + key + "_then_command"],g_triggerset,"trigger_then_command");
		//トリガー名
		g_triggerset.find("#trigger_display_name").val( g_Trigger["trigger_" + key + "_display_name"]).change();
		//最後に実行した時間
		lastfireTime = g_Trigger["trigger_" + key + "_lastfire"];
	}
	
	//最後に実行した時間を表示する
	if (lastfireTime <= 0)
	{
		g_triggerset.find("#trigger_if_stopper_display_value").html("まだ一度も実行されていません。");
	}
	else
	{
		g_triggerset.find("#trigger_if_stopper_display_value").html("前回は " + UntixtimeToString(lastfireTime) + " に実行しました。");
	}

	$.mobile.changePage("#triggerset", { transition: "flip" });
	g_triggerset.find("input[type='checkbox']").checkboxradio("refresh");
	g_triggerset.find("input[type='select']").selectmenu("refresh");
	g_triggerset.find("#trigger_if_sound_value").slider('refresh');
}



var g_system = null;
function system_init()
{
	if (g_system) return ;
	g_system = $("#system");

	//設定するボタンをクリックした時
	g_system.find("#system_apply_button").click(function(){
		//読込中にする
		showUpdatewaitDialog();

		g_system.find(".error").hide();

		var updateArray = {
			  'system_fhcboot_mp3':			g_system.find("#system_fhcboot_mp3").text()
			 ,'weather_script_command':		g_system.find("#weather_script_command").val()
			 ,'train_script_command':		g_system.find("#train_script_command").val()
			 ,'_csrftoken':					g_CSRFTOKEN
		};
		$.ajax({
			type: "post",
			url: "/setting/system/update",
			timeout: 50000,
			data: updateArray,
			dataType: 'json',
			success: function(d){
				hideUpdatewaitDialog();

				if (d['result'] != "ok")
				{//エラーになりました
					g_system.find(".bunner_error").show();
					return ;
				}

				g_SETTING = d;

				$.mobile.changePage("#setting", { transition: "flip" });
				return ;
			},
			error: function(d){
				hideUpdatewaitDialog();

				g_system.find(".bunner_error").show();
				return ;
			}
		});
	});
	//起動時に鳴らすmp3選択フォーム
	mp3select_command(g_system,"system_fhcboot_mp3");

	//コマンドのイベント
	g_system.find("#weather_script_command_select_button").click(function(){
		fileselectpage_load( "script_command" , g_system.find("#weather_script_command").val() , g_Strings["SELECT_WATHER_COMMAND"]
		, function(filename){
			action_command(filename,g_system,"weather_script_command");
		});
	});
	g_system.find("#train_script_command_select_button").click(function(){
		fileselectpage_load( "script_command" , g_system.find("#train_script_command").val() , g_Strings["SELECT_TRAIN_COMMAND"]
		, function(filename){
			action_command(filename,g_system,"train_script_command");
		});
	});

	g_system.find("#weather_script_command_preview_button").click(function(){
		script_command_test_dialog(
			 g_system.find("#weather_script_command").val()
			,g_system.find("#weather_script_command_preview_out")
		);
	});

	g_system.find("#train_script_command_preview_button").click(function(){
		script_command_test_dialog(
			 g_system.find("#train_script_command").val()
			,g_system.find("#train_script_command_preview_out")
		);
	});
}
function script_command_test_dialog(filename,messageObj)
{
	commandrundialog_load( filename , function(d){
		g_commandrundialogObject.dialog("close");

		//ダイアログからコマンドを実行した戻り
		if ( d['result'] != "ok" )
		{
			if (d['code'] == "38020")
			{
				var errArr = errorMessageToLineAndMessage(d['message']);

				//エラー
				var html = "";
				html += g_Strings['error_ga'] + "<br />";
				html += g_Strings['error'] + ":" + errArr.message + "<br />";
				if (errArr.line > 0) {
					html += "line:" + errArr.line + "<br />";
				}
				html += "<br />";
				html += g_Strings['result'] + ": " + h(d['returnValue']) + "<br />";
				html += g_Strings['stdout'] + ": " + longText(d['stdout'],"クリックで表示します。",15) + "<br />";
				messageObj.html(html).show();
				return ;
			}
			else
			{
				alertErrorDialog(d,"script_command_test_dialogに失敗しました");
			}
			return ;
		}

		//結果を表示する.
		var html = "";
		html += g_Strings['result'] + ": " + h(d['returnValue']) + "<br />";
		html += g_Strings['stdout'] + ": " + longText(d['stdout'],"クリックで表示します。",15) + "<br />";
		messageObj.html(html).show();
		return ;
	});
}
function system_load(typepath)
{
	system_init();

	g_system.find(".error").hide();
	g_system.find("#system_fhcboot_mp3").text(g_SETTING["system_fhcboot_mp3"]).change();
	g_system.find("#weather_script_command").text(g_SETTING["weather_script_command"]).change();
	action_command(g_SETTING["weather_script_command"],g_system,"weather_script_command");
	g_system.find("#train_script_command").text(g_SETTING["train_script_command"]).change();
	action_command(g_SETTING["train_script_command"],g_system,"train_script_command");

	if(g_SETTING["twitter_screen_name"]=="")
	{
		g_system.find("#twitter_command_display").html("");
		g_system.find("#twitter_command_cancel_button").hide();
		g_system.find("#twitter_command_linked_notify").hide();
	}
	else
	{
		g_system.find("#twitter_command_display").html("@"+g_SETTING["twitter_screen_name"]);
		g_system.find("#twitter_command_cancel_button").show();
		g_system.find("#twitter_command_linked_notify").show();
	}

	$.mobile.changePage("#system", { transition: "flip" });
}


var g_changelang = null;
function changelang_init()
{
	if (g_changelang) return ;
	g_changelang = $("#changelang");

	//設定するボタンをクリックした時
	g_changelang.find("#changelang_apply_button").click(function(){
		//読込中にする
		showUpdatewaitDialog();

		g_changelang.find(".error").hide();

		var updateArray = {
			  'system_lang':			g_changelang.find("#system_lang").val()
			 ,'_csrftoken': 			g_CSRFTOKEN
		};
		$.ajax({
			type: "post",
			url: "/setting/changelang/update",
			timeout: 50000,
			data: updateArray,
			dataType: 'json',
			success: function(d){
				hideUpdatewaitDialog();

				if (d['result'] != "ok")
				{//エラーになりました
					g_changelang.find(".bunner_error").show();
					return ;
				}

				g_SETTING = d;

				$.mobile.changePage("#system", { transition: "flip" });
				alert(g_Strings["NEED_REBOOT"]);
				return ;
			},
			error: function(d){
				hideUpdatewaitDialog();

				g_changelang.find(".bunner_error").show();
				return ;
			}
		});
	});
}



function changelang_load(typepath)
{
	changelang_init();

	g_changelang.find(".error").hide();
	g_changelang.find("#system_lang").val(g_SETTING["system_lang"]).change();

	$.mobile.changePage("#changelang", { transition: "flip" });
}



var g_homekit = null;
function homekit_init()
{
	if (g_homekit) return ;
	g_homekit = $("#homekit");

	//有効にするボタンを押したとき
	g_homekit.find("#homekit_enable_button").click(function(){
		update(
			g_homekit
			,"/setting/homekit/boot"
			,{ 
				'homekit_boot': 1
			}
			,function(d){ 
				g_SETTING = d;
				homekit_load();
			}
			,{ error: function(d){
					g_homekit.find(".homekit_cannot_start").show();
				} 
			}
		);
	});

	//無効にするボタンを押したとき
	g_homekit.find("#homekit_disable_button").click(function(){
		update(
			g_homekit
			,"/setting/homekit/boot"
			,{ 
				'homekit_boot': 0
			}
			,function(d){ 
				g_SETTING = d;
				homekit_load();
			}
		);
	});

	g_homekit.find("#homekit_keyreset_button").click(function(){
		webapiKeyChangeConfirm(function(){
			update(
				g_homekit
				,"/setting/homekit/resetkey"
				,{ 
				}
				,function(d){ 
					g_SETTING = d;
					homekit_load();
				}
			);
		});
	});
}

function homekit_load()
{
	homekit_init();

	g_homekit.find(".error").hide();
	g_homekit.find("#homekit_pin").text(g_SETTING["homekit_pin"]);

	if ( g_SETTING["homekit_boot"] == "1")
	{
		g_homekit.find('.homekit_is_disable').hide();
		g_homekit.find('.homekit_is_enable').show();
	}
	else
	{
		g_homekit.find('.homekit_is_enable').hide();
		g_homekit.find('.homekit_is_disable').show();
	}

	$.mobile.changePage("#homekit", { transition: "flip" });
}

var g_sip = null;
function sip_init()
{
	if (g_sip) return ;
	g_sip = $("#sip");

	//設定するボタンをクリックした時
	g_sip.find("#sip_apply_button").click(function(){
		//読込中にする
		showUpdatewaitDialog();

		g_sip.find(".error").hide();

		var updateArray = {
			  'sip_enable_incoming':	g_sip.find("#sip_enable_incoming").val()
			 ,'sip_domain':				g_sip.find("#sip_domain").val()
			 ,'sip_password':			g_sip.find("#sip_password").val()
			 ,'sip_id':					g_sip.find("#sip_id").val()
			 ,'sip_enable':				g_sip.find("#sip_enable").val()
			 ,'sip_calling_mp3':		g_sip.find("#sip_calling_mp3").text()
			 ,'sip_incoming_mp3':		g_sip.find("#sip_incoming_mp3").text()
			 ,'sip_when_taking_stop_recogn':g_sip.find("#sip_when_taking_stop_recogn").val()
			 ,'_csrftoken':				g_CSRFTOKEN
		};
		update(
			g_sip
			,"/setting/sip/update"
			,updateArray
			,function(d){ 
				g_SETTING = d;

				$.mobile.changePage("#system", { transition: "flip" });
			}
			,{ error: function(d){
					g_sip.find(".bunner_error").show();
					var err = g_sip.find(".error_" + d['code']);
					if(err.get(0))
					{
						err.show()
					}
					else
					{
						alertErrorDialog(d,"setting_sip_apply");
					}
					return ;
				} 
			}
		);
	});
}
function sip_load()
{
	sip_init();

	g_sip.find(".error").hide();
	g_sip.find("#sip_enable").change(function(){
		showhidden(g_sip.find(".sip_is_enable_div"),$(this).val()=="1" );
	});
	g_sip.find("#sip_enable_incoming").change(function(){
		showhidden(g_sip.find(".sip_is_enable_incoming_div"),$(this).val()=="1" );
	});
	mp3select_command(g_sip,"sip_calling_mp3");
	mp3select_command(g_sip,"sip_incoming_mp3");

	g_sip.find("#sip_enable").val(g_SETTING["sip_enable"]).change();
	g_sip.find("#sip_id").val(g_SETTING["sip_id"]).change();
	g_sip.find("#sip_password").val(g_SETTING["sip_password"]).change();
	g_sip.find("#sip_domain").val(g_SETTING["sip_domain"]).change();
	g_sip.find("#sip_enable_incoming").val(g_SETTING["sip_enable_incoming"]).change();
	g_sip.find("#sip_when_taking_stop_recogn").val(g_SETTING["sip_when_taking_stop_recogn"]).change();
	g_sip.find("#sip_calling_mp3").text(g_SETTING["sip_calling_mp3"]);
	g_sip.find("#sip_incoming_mp3").text(g_SETTING["sip_incoming_mp3"]);

	$.mobile.changePage("#sip", { transition: "flip" });
}


var g_multiroomlist = null;
function multiroomlist_init()
{
	if (g_multiroomlist) return ;
	g_multiroomlist = $("#multiroomlist");
}

function multiroomlist_load()
{
	multiroomlist_init();

	var htmltemplate = g_multiroomlist.find("#multiroomlist_li_sample").html();
	var totaldom = "";
	for(var arraykey in g_MultiRoom)
	{
		var s = arraykey.split("_");
		if ( !(s[0] == "multiroom" && s[2] == "name" )) continue;
		var key = s[1];

		var html = htmltemplate;
		html = html.replace(/%MULTIROOMKEY%/g,key);

		html = html.replace(/%MULTIROOMNAME%/g,g_MultiRoom["multiroom_" + key + "_name"] );
		html = html.replace(/%MULTIROOMURL%/g,g_MultiRoom["multiroom_" + key + "_url"] );
		html = html.replace(/%MULTIROOMUUID%/g,g_MultiRoom["multiroom_" + key + "_uuid"] );

		if(g_MultiRoom["multiroom_" + key + "_uuid"] != g_SETTING["httpd__uuid"] )
		{
			html = html.replace(/%MULTIROOMMYSELF%/g,"none");
			html = html.replace(/%MULTIROOMMYSELF_HIDE%/g,"block");
			
		}
		else
		{
			html = html.replace(/%MULTIROOMMYSELF%/g,"block");
			html = html.replace(/%MULTIROOMMYSELF_HIDE%/g,"none");
		}
		totaldom += html;
	}
	g_multiroomlist.find("#multiroomlist_menu_ul").html(totaldom);

	//ソートできるようにします
	sortableul( g_multiroomlist.find("#multiroomlist_menu_ul") , {
		update: function(obj) { multiroomlist_update_order(); }
	});

	//ボタンにイベントを仕込む
	g_multiroomlist.find("#multiroomlist_menu_ul li").each(function(){
		var itemObject = $(this);
		var menukey = itemObject.find(".key").text();

		//削除アイコンがクリックされた時の動作
		itemObject.find(".delicon").bind("mousedown touchstart",function(e){
			e.preventDefault();
			multiroomlist_delete(menukey);
		});
	});

	$.mobile.changePage("#multiroomlist", { transition: "flip" });
}

function multiroomlist_delete(key)
{
	key = parseInt(key);

	delConfirm(function(e){
		var arr = {};
		arr["key"] = key;
	
		update(
			g_multiroomlist
			,"/setting/multiroom/delete"
			,arr
			,function(d){ 
				g_MultiRoom = d;
				multiroomlist_load();
			}
		);
	}
	,function(e){
		hideUpdatewaitDialog();
	} );
}

//並び順の更新
function multiroomlist_update_order(menu,action)
{
	var orderbyArray = {};

	var orderby = 1;
	g_multiroomlist.find("#multiroomlist_menu_ul li").each(function(){
		var itemObject = $(this);
		var menukey = itemObject.find(".key").text();
		var keyString = "multiroom_" + menukey + "_order";

		orderbyArray[keyString] = orderby++;
	});

	update(
		g_multiroomlist
		,"/setting/multiroom/order"
		,orderbyArray
		,function(d){ 
			g_MultiRoom = d;
		}
		,{ load: 'no' }
	);
}


var g_multiroomappend = null;

function multiroomappend_init()
{
	if (g_multiroomappend) return ;
	g_multiroomappend = $("#multiroomappend");

	g_multiroomappend.find("#multiroomappend_apply_button").click(function(){
		//読込中にする
		showUpdatewaitDialog();
		g_multiroomappend.find(".error").hide();
		var updateArray = {
			  'multiroomappend_url':			g_multiroomappend.find("#multiroomappend_url").val()
			 ,'multiroomappend_usermail':		g_multiroomappend.find("#multiroomappend_usermail").val()
			 ,'multiroomappend_password':		g_multiroomappend.find("#multiroomappend_password").val()
		};

		update(
			g_multiroomappend
			,"/setting/multiroom/append"
			,updateArray
			,function(d){ 
				g_MultiRoom = d;
				multiroomlist_load();
			}
		);
	});
}

function multiroomappend_load(url)
{
	multiroomappend_init();

	g_multiroomappend.find(".error").hide();
	g_multiroomappend.find("#multiroomappend_url").val(url);

	$.mobile.changePage("#multiroomappend", { transition: "flip" });
}


var g_multiroomsearch = null;

function multiroomsearch_init()
{
	if (g_multiroomsearch) return ;
	g_multiroomsearch = $("#multiroomsearch");

	//もう一度検索するボタンを押したとき
	g_multiroomsearch.find("#multiroomsearch_search_button").click(function(){
		$('#multiroomsearch_hint_no_hit').hide();
		update(
			g_multiroomsearch
			,"/setting/multiroom/search"
			,{}
			,function(d){ 
				g_multiroomsearch.find('.hint').hide();

				var htmltemplate = g_multiroomsearch.find("#multiroomsearch_li_sample").html();
				var totaldom = "";
				for(var arraykey in d)
				{
					var s = arraykey.split("_");
					if ( !(s[0] == "multiroom" && s[2] == "url" )) continue;
					var key = s[1];
					
					var multiroom_key = d["multiroom_" + key + "_key"]

					var html = htmltemplate;
					html = html.replace(/%MULTIROOMURL%/g,d["multiroom_" + key + "_url"] );
					html = html.replace(/%MULTIROOMUUID%/g,d["multiroom_" + key + "_uuid"] );
					if (multiroom_key > 0)
					{
						html = html.replace(/%MULTIROOMJOIN%/g,"none");
						html = html.replace(/%MULTIROOMALREADYJOIN%/g,"block");
					}
					else
					{
						html = html.replace(/%MULTIROOMJOIN%/g,"block");
						html = html.replace(/%MULTIROOMALREADYJOIN%/g,"none");
					}
					totaldom += html;
				}
				g_multiroomsearch.find("#multiroomsearch_menu_ul").html(totaldom);
				if (totaldom == "")
				{
					g_multiroomsearch.find('.hint_nohit').show();
				}
			}
			,{ load: $("#loadingwait_dialog_search") }
		);
	});
}
function multiroomsearch_load(url)
{
	multiroomsearch_init();

	g_multiroomsearch.find('.hint').hide();
	g_multiroomsearch.find('.hint_start').show();
	$.mobile.changePage("#multiroomsearch", { transition: "flip" });
}



var g_remoconsetting = null;
function remoconsetting_init()
{
	if (g_remoconsetting) return ;
	g_remoconsetting = $("#remoconsetting");

	//設定するボタンをクリックした時
	g_remoconsetting.find("#remoconsetting_apply_button").click(function(){
		//読込中にする
		showUpdatewaitDialog();

		g_remoconsetting.find(".error").hide();


		var updateArray = {
			 "system_room_string":							g_remoconsetting.find("#system_room_string").val()
			,"system_room_color":							g_remoconsetting.find("#system_room_color").val()
			,"system_room_icon":							g_remoconsetting.find("#system_room_icon").text()
			,"remoconsetting_default_font_color":			g_remoconsetting.find("#remoconsetting_default_font_color").val()
			,"remoconsetting_default_background_color":		g_remoconsetting.find("#remoconsetting_default_background_color").val()
			,"remoconsetting_background_image":				g_remoconsetting.find("#remoconsetting_background_image").text()
			,"remoconsetting_menu_icon":					g_remoconsetting.find("#remoconsetting_menu_icon").text()
			,"remoconsetting_badges_timer_icon":			g_remoconsetting.find("#remoconsetting_badges_timer_icon").text()
			,"remoconsetting_sensor_font_color":			g_remoconsetting.find("#remoconsetting_sensor_font_color").val()
			,"remoconsetting_sensor_temp_string":			g_remoconsetting.find("#remoconsetting_sensor_temp_string").val()
			,"remoconsetting_sensor_lumi_string":			g_remoconsetting.find("#remoconsetting_sensor_lumi_string").val()
			,"remoconsetting_footermenu_remoconedit_string":g_remoconsetting.find("#remoconsetting_footermenu_remoconedit_string").val()
			,"remoconsetting_footermenu_remoconedit_icon":	g_remoconsetting.find("#remoconsetting_footermenu_remoconedit_icon").text()
			,"remoconsetting_footermenu_setting_string":	g_remoconsetting.find("#remoconsetting_footermenu_setting_string").val()
			,"remoconsetting_footermenu_setting_icon":		g_remoconsetting.find("#remoconsetting_footermenu_setting_icon").text()
			,"remoconsetting_footermenu_font_color":		g_remoconsetting.find("#remoconsetting_footermenu_font_color").val()
			,"remoconsetting_footermenu_background_color":	g_remoconsetting.find("#remoconsetting_footermenu_background_color").val()
			,"remoconsetting_footermenu_border_color":		g_remoconsetting.find("#remoconsetting_footermenu_border_color").val()
			,"remoconsetting_footermenu_hover_color":		g_remoconsetting.find("#remoconsetting_footermenu_hover_color").val()

			,"remoconsetting_dialog_close_string":			g_remoconsetting.find("#remoconsetting_dialog_close_string").val()
			,"remoconsetting_dialog_background_color":		g_remoconsetting.find("#remoconsetting_dialog_background_color").val()
			,"remoconsetting_dialog_font_color":			g_remoconsetting.find("#remoconsetting_dialog_font_color").val()
			,"remoconsetting_dialog_border_color":			g_remoconsetting.find("#remoconsetting_dialog_border_color").val()
			,"remoconsetting_button_font_color":			g_remoconsetting.find("#remoconsetting_button_font_color").val()
			,"remoconsetting_button_background_color1":		g_remoconsetting.find("#remoconsetting_button_background_color1").val()
			,"remoconsetting_button_background_color2":		g_remoconsetting.find("#remoconsetting_button_background_color2").val()
			,"remoconsetting_button_border_color":			g_remoconsetting.find("#remoconsetting_button_border_color").val()
			,"remoconsetting_button_hover_color1":			g_remoconsetting.find("#remoconsetting_button_hover_color1").val()
			,"remoconsetting_button_hover_color2":			g_remoconsetting.find("#remoconsetting_button_hover_color2").val()
			,"remoconsetting_button_hover_border_color":	g_remoconsetting.find("#remoconsetting_button_hover_border_color").val()
			,"remoconsetting_dialog_donot_autoclose":		g_remoconsetting.find("#remoconsetting_dialog_donot_autoclose").val()
			,"remoconsetting_dialog_iconcount_pc":			g_remoconsetting.find("#remoconsetting_dialog_iconcount_pc").val()
			,"_csrftoken": g_CSRFTOKEN
		};
		$.ajax({
			type: "post",
			url: "/setting/remoconsetting/update",
			timeout: 50000,
			data: updateArray,
			dataType: 'json',
			success: function(d){
				hideUpdatewaitDialog();

				if (d['result'] != "ok")
				{//エラーになりました
					g_system.find(".bunner_error").show();
					return ;
				}

				g_SETTING = d;

				$.mobile.changePage("#setting", { transition: "flip" });
				return ;
			},
			error: function(d){
				hideUpdatewaitDialog();

				g_system.find(".bunner_error").show();
				return ;
			}
		});
	});

	//カラーピッカーを作る
	colorpicker_command(g_remoconsetting,"system_room_color");
	colorpicker_command(g_remoconsetting,"remoconsetting_default_font_color");
	colorpicker_command(g_remoconsetting,"remoconsetting_default_background_color");
	colorpicker_command(g_remoconsetting,"remoconsetting_sensor_font_color");
	colorpicker_command(g_remoconsetting,"remoconsetting_footermenu_font_color");
	colorpicker_command(g_remoconsetting,"remoconsetting_footermenu_background_color");
	colorpicker_command(g_remoconsetting,"remoconsetting_footermenu_border_color");
	colorpicker_command(g_remoconsetting,"remoconsetting_footermenu_hover_color");
	colorpicker_command(g_remoconsetting,"remoconsetting_dialog_background_color");
	colorpicker_command(g_remoconsetting,"remoconsetting_dialog_font_color");
	colorpicker_command(g_remoconsetting,"remoconsetting_dialog_border_color");
	colorpicker_command(g_remoconsetting,"remoconsetting_button_font_color");
	colorpicker_command(g_remoconsetting,"remoconsetting_button_background_color1");
	colorpicker_command(g_remoconsetting,"remoconsetting_button_background_color2");
	colorpicker_command(g_remoconsetting,"remoconsetting_button_border_color");
	colorpicker_command(g_remoconsetting,"remoconsetting_button_hover_color1");
	colorpicker_command(g_remoconsetting,"remoconsetting_button_hover_color2");
	colorpicker_command(g_remoconsetting,"remoconsetting_button_hover_border_color");

	iconselect_command(g_remoconsetting,"system_room_icon","remocon","部屋のアイコン");
	iconselect_command(g_remoconsetting,"remoconsetting_background_image","remocon","背景画像");
	iconselect_command(g_remoconsetting,"remoconsetting_menu_icon","remocon","メニューのアイコン");
	iconselect_command(g_remoconsetting,"remoconsetting_footermenu_remoconedit_icon","remocon","リモコンを編集のアイコン");
	iconselect_command(g_remoconsetting,"remoconsetting_footermenu_setting_icon","remocon","設定画面を開くのアイコン");
	iconselect_command(g_remoconsetting,"remoconsetting_badges_timer_icon","remocon","タイマーのアイコン");
	
	
	g_remoconsetting.find("#system_room_string").bind("keyup change",function(){
		g_remoconsetting.find(".remoconcontroll_ui_contents_wrapper .navigationbar_title_span").text($(this).val());
	});
	g_remoconsetting.find("#remoconsetting_footermenu_remoconedit_string").bind("keyup change",function(){
		g_remoconsetting.find(".remoconcontroll_ui_contents_wrapper .footermenu_buton_name_remoconedit").text($(this).val());
	});
	g_remoconsetting.find("#remoconsetting_footermenu_setting_string").bind("keyup change",function(){
		g_remoconsetting.find(".remoconcontroll_ui_contents_wrapper .footermenu_buton_name_setting").text($(this).val());
	});
	g_remoconsetting.find("#remoconsetting_sensor_temp_string").bind("keyup change",function(){
		g_remoconsetting.find(".remoconcontroll_ui_contents_wrapper .sensor_name_temp").text($(this).val());
	});
	g_remoconsetting.find("#remoconsetting_sensor_lumi_string").bind("keyup change",function(){
		g_remoconsetting.find(".remoconcontroll_ui_contents_wrapper .sensor_name_lumi").text($(this).val());
	});
	
	g_remoconsetting.find("#remoconsetting_dialog_close_string").bind("keyup change",function(){
		g_remoconsetting.find(".remoconcontroll_ui2_loadingwait_dialog .pclose").text($(this).val());
	});


	g_remoconsetting.find("#remoconsetting_default_font_color").change(function(){
		g_remoconsetting.find(".remoconcontroll_ui_contents_wrapper").css("color",$(this).val());
		g_remoconsetting.find(".remoconcontroll_ui2_loadingwait_dialog").css("color",$(this).val());
	});
	g_remoconsetting.find("#remoconsetting_default_background_color").change(function(){
		g_remoconsetting.find(".remoconcontroll_ui_contents_wrapper").css("background-color",$(this).val());
		g_remoconsetting.find(".remoconcontroll_ui2_loadingwait_dialog").css("background-color",$(this).val());
	});
	g_remoconsetting.find("#remoconsetting_sensor_font_color").change(function(){
		g_remoconsetting.find(".remoconcontroll_ui_contents_wrapper .navigationbar_sensor").css("color",$(this).val());
	});
	g_remoconsetting.find("#remoconsetting_footermenu_font_color").change(function(){
		g_remoconsetting.find(".remoconcontroll_ui_footermenu_buton").css("color",$(this).val());
	});
	g_remoconsetting.find("#remoconsetting_footermenu_background_color").change(function(){
		g_remoconsetting.find(".remoconcontroll_ui_footermenu_buton").css("background-color",$(this).val());
	});
	g_remoconsetting.find("#remoconsetting_footermenu_border_color").change(function(){
		g_remoconsetting.find(".remoconcontroll_ui_footermenu_buton").css("border-color",$(this).val());
	});
	g_remoconsetting.find("#remoconsetting_footermenu_hover_color").change(function(){
		g_remoconsetting.find(".remoconcontroll_ui_footermenu_buton").css("background-color",$(this).val());
	});

	g_remoconsetting.find("#system_room_icon").change(function(){
		g_remoconsetting.find(".remoconcontroll_ui_contents_wrapper .navigationbar_icon img").prop("src","/user/remocon/"+$(this).text());
	});

	g_remoconsetting.find("#system_room_color").change(function(){
		g_remoconsetting.find(".remoconcontroll_ui_contents_wrapper .navigationbar_title_span").css("color",$(this).val());
	});

	g_remoconsetting.find("#remoconsetting_background_image").change(function(){
		g_remoconsetting.find(".remoconcontroll_ui_contents_wrapper").css("background","url(/user/remocon/"+$(this).text()+") repeat-y scroll center 0");
	});
	g_remoconsetting.find("#remoconsetting_menu_icon").change(function(){
		g_remoconsetting.find(".actionmenuicon").css("background","url(/user/remocon/"+$(this).text()+") no-repeat scroll 0px");
	});
	g_remoconsetting.find("#remoconsetting_footermenu_remoconedit_icon").change(function(){
		g_remoconsetting.find(".footermenu_buton_icon_remoconedit img").prop("src","/user/remocon/"+$(this).text());
	});
	g_remoconsetting.find("#remoconsetting_footermenu_setting_icon").change(function(){
		g_remoconsetting.find(".footermenu_buton_icon_setting img").prop("src","/user/remocon/"+$(this).text());
	});
	g_remoconsetting.find("#remoconsetting_badges_timer_icon").change(function(){
		g_remoconsetting.find(".timericon").css("background","url(/user/remocon/"+$(this).text()+") no-repeat scroll 0px");
	});

	g_remoconsetting.find("#remoconsetting_button_border_color").change(function(){
		g_remoconsetting.find(".buttoncolor").css("border-color",$(this).val());
	});
	g_remoconsetting.find("#remoconsetting_button_font_color").change(function(){
		g_remoconsetting.find(".buttoncolor").css("color",$(this).val());
	});
	
	g_remoconsetting.find("#remoconsetting_button_background_color1").change(function(){
		change_gradient( g_remoconsetting.find(".buttoncolor")
			,g_remoconsetting.find("#remoconsetting_button_background_color1").val()
			,g_remoconsetting.find("#remoconsetting_button_background_color2").val()
			,2
		);
	});
	g_remoconsetting.find("#remoconsetting_button_background_color2").change(function(){
		change_gradient( g_remoconsetting.find(".buttoncolor")
			,g_remoconsetting.find("#remoconsetting_button_background_color1").val()
			,g_remoconsetting.find("#remoconsetting_button_background_color2").val()
			,2
		);
	});

	g_remoconsetting.find("#remoconsetting_button_hover_color1").change(function(){
		change_gradient( g_remoconsetting.find(".buttoncolor")
			,g_remoconsetting.find("#remoconsetting_button_hover_color1").val()
			,g_remoconsetting.find("#remoconsetting_button_hover_color2").val()
			,2
		);
	});
	g_remoconsetting.find("#remoconsetting_button_hover_color2").change(function(){
		change_gradient(	g_remoconsetting.find(".buttoncolor")
			,g_remoconsetting.find("#remoconsetting_button_hover_color1").val()
			,g_remoconsetting.find("#remoconsetting_button_hover_color2").val()
			,2
		);
	});
	g_remoconsetting.find("#remoconsetting_button_hover_border_color").change(function(){
		g_remoconsetting.find(".buttoncolor").css("border-color",$(this).val());
	});

	g_remoconsetting.find("#remoconsetting_dialog_background_color").change(function(){
		g_remoconsetting.find(".remoconcontroll_ui2_loadingwait_dialog").css("background-color",$(this).val());
	});
	g_remoconsetting.find("#remoconsetting_dialog_font_color").change(function(){
		g_remoconsetting.find(".remoconcontroll_ui2_loadingwait_dialog").css("color",$(this).val());
	});
	g_remoconsetting.find("#remoconsetting_dialog_border_color").change(function(){
		g_remoconsetting.find(".remoconcontroll_ui2_loadingwait_dialog").css("border-color",$(this).val());
	});


	g_remoconsetting.find("#remoconsetting_typeas").change(function(){
		var type = $(this).val();
		g_remoconsetting.find(".remoconsetting_typeas_select").hide();
		g_remoconsetting.find(".remoconsetting_sample").hide();

		if (type == "1")
		{
			g_remoconsetting.find(".remoconsetting_footermenu_unhover").change();
			g_remoconsetting.find("#remoconsetting_typeas_1").show();
			g_remoconsetting.find("#remoconsetting_sample_1").show();
		}
		else if (type == "2")
		{
			g_remoconsetting.find(".remoconsetting_footermenu_unhover").change();
			g_remoconsetting.find("#remoconsetting_typeas_2").show();
			g_remoconsetting.find("#remoconsetting_sample_1").show();
		}
		else if (type == "3")
		{
			g_remoconsetting.find(".remoconsetting_footermenu_unhover").change();
			g_remoconsetting.find("#remoconsetting_typeas_3").show();
			g_remoconsetting.find("#remoconsetting_sample_1").show();
		}
		else if (type == "4")
		{
			g_remoconsetting.find(".remoconsetting_footermenu_unhover").change();
			g_remoconsetting.find("#remoconsetting_typeas_4").show();
			g_remoconsetting.find("#remoconsetting_sample_1").show();
		}
		else if (type == "5")
		{
			g_remoconsetting.find(".remoconsetting_footermenu_hover").change();
			g_remoconsetting.find("#remoconsetting_typeas_5").show();
			g_remoconsetting.find("#remoconsetting_sample_1").show();
		}
		else if (type == "9")
		{
			g_remoconsetting.find(".remoconsetting_footermenu_unhover").change();
			g_remoconsetting.find("#remoconsetting_typeas_9").show();
			g_remoconsetting.find("#remoconsetting_sample_1").show();
		}
		else if (type == "6")
		{
			g_remoconsetting.find(".remoconsetting_footermenu_unhover").change();
			g_remoconsetting.find("#remoconsetting_typeas_6").show();
			g_remoconsetting.find("#remoconsetting_sample_2").show();
		}
		else if (type == "7")
		{
			g_remoconsetting.find(".remoconsetting_footermenu_unhover").change();
			g_remoconsetting.find("#remoconsetting_typeas_7").show();
			g_remoconsetting.find("#remoconsetting_sample_2").show();
		}
		else if (type == "8")
		{
			g_remoconsetting.find(".remoconsetting_footermenu_hover").change();
			g_remoconsetting.find("#remoconsetting_typeas_8").show();
			g_remoconsetting.find("#remoconsetting_sample_2").show();
		}
		else if (type == "10")
		{
			g_remoconsetting.find(".remoconsetting_footermenu_unhover").change();
			g_remoconsetting.find("#remoconsetting_typeas_10").show();
		}
		else if (type == "11")
		{
			g_remoconsetting.find(".remoconsetting_footermenu_unhover").change();
			g_remoconsetting.find("#remoconsetting_typeas_11").show();
		}
	});
}


function remoconsetting_load()
{
	remoconsetting_init();

	g_remoconsetting.find(".error").hide();
	g_remoconsetting.find("#system_room_string").val(g_SETTING["system_room_string"]).change();
	g_remoconsetting.find("#system_room_color").val(g_SETTING["system_room_color"]).change();
	g_remoconsetting.find("#system_room_icon").text(g_SETTING["system_room_icon"]).change();
	g_remoconsetting.find("#remoconsetting_default_font_color").val(g_SETTING["remoconsetting_default_font_color"]).change();
	g_remoconsetting.find("#remoconsetting_default_background_color").val(g_SETTING["remoconsetting_default_background_color"]).change();
	g_remoconsetting.find("#remoconsetting_background_image").text(g_SETTING["remoconsetting_background_image"]).change();
	g_remoconsetting.find("#remoconsetting_menu_icon").text(g_SETTING["remoconsetting_menu_icon"]).change();
	g_remoconsetting.find("#remoconsetting_badges_timer_icon").text(g_SETTING["remoconsetting_badges_timer_icon"]).change();
	g_remoconsetting.find("#remoconsetting_sensor_font_color").val(g_SETTING["remoconsetting_sensor_font_color"]).change();
	g_remoconsetting.find("#remoconsetting_sensor_temp_string").val(g_SETTING["remoconsetting_sensor_temp_string"]).change();
	g_remoconsetting.find("#remoconsetting_sensor_lumi_string").val(g_SETTING["remoconsetting_sensor_lumi_string"]).change();
	g_remoconsetting.find("#remoconsetting_footermenu_remoconedit_string").val(g_SETTING["remoconsetting_footermenu_remoconedit_string"]).change();
	g_remoconsetting.find("#remoconsetting_footermenu_remoconedit_icon").text(g_SETTING["remoconsetting_footermenu_remoconedit_icon"]).change();
	g_remoconsetting.find("#remoconsetting_footermenu_setting_string").val(g_SETTING["remoconsetting_footermenu_setting_string"]).change();
	g_remoconsetting.find("#remoconsetting_footermenu_setting_icon").text(g_SETTING["remoconsetting_footermenu_setting_icon"]).change();
	g_remoconsetting.find("#remoconsetting_footermenu_font_color").val(g_SETTING["remoconsetting_footermenu_font_color"]).change();
	g_remoconsetting.find("#remoconsetting_footermenu_background_color").val(g_SETTING["remoconsetting_footermenu_background_color"]).change();
	g_remoconsetting.find("#remoconsetting_footermenu_border_color").val(g_SETTING["remoconsetting_footermenu_border_color"]).change();
	g_remoconsetting.find("#remoconsetting_footermenu_hover_color").val(g_SETTING["remoconsetting_footermenu_hover_color"]).change();
	g_remoconsetting.find("#remoconsetting_dialog_close_string").val(g_SETTING["remoconsetting_dialog_close_string"]).change();
	g_remoconsetting.find("#remoconsetting_dialog_background_color").val(g_SETTING["remoconsetting_dialog_background_color"]).change();
	g_remoconsetting.find("#remoconsetting_dialog_font_color").val(g_SETTING["remoconsetting_dialog_font_color"]).change();
	g_remoconsetting.find("#remoconsetting_dialog_border_color").val(g_SETTING["remoconsetting_dialog_border_color"]).change();
	g_remoconsetting.find("#remoconsetting_button_font_color").val(g_SETTING["remoconsetting_button_font_color"]).change();
	g_remoconsetting.find("#remoconsetting_button_background_color1").val(g_SETTING["remoconsetting_button_background_color1"]).change();
	g_remoconsetting.find("#remoconsetting_button_background_color2").val(g_SETTING["remoconsetting_button_background_color2"]).change();
	g_remoconsetting.find("#remoconsetting_button_border_color").val(g_SETTING["remoconsetting_button_border_color"]).change();
	g_remoconsetting.find("#remoconsetting_button_hover_color1").val(g_SETTING["remoconsetting_button_hover_color1"]).change();
	g_remoconsetting.find("#remoconsetting_button_hover_color2").val(g_SETTING["remoconsetting_button_hover_color2"]).change();
	g_remoconsetting.find("#remoconsetting_button_hover_border_color").val(g_SETTING["remoconsetting_button_hover_border_color"]).change();
	g_remoconsetting.find("#remoconsetting_dialog_donot_autoclose").val(g_SETTING["remoconsetting_dialog_donot_autoclose"]).change();
	g_remoconsetting.find("#remoconsetting_dialog_iconcount_pc").val(g_SETTING["remoconsetting_dialog_iconcount_pc"]).change();

	g_remoconsetting.find("#remoconsetting_typeas").val("1").change();

	$.mobile.changePage("#remoconsetting", { transition: "flip" });
}

function download_post_form(url)
{
	var form = document.createElement('form');
	document.body.appendChild( form );
	var input = document.createElement('input');
	input.setAttribute( 'type' , 'hidden' );
	input.setAttribute( 'name' , '_csrftoken' );
	input.setAttribute( 'value' , g_CSRFTOKEN );
	form.appendChild( input );
	form.setAttribute( 'action' , url );
	form.setAttribute( 'method' , 'post' );
	form.setAttribute( 'target' , '_blank' );
	form.submit();
}


var g_version = null;
function version_init()
{
	if (g_version) return ;
	g_version = $("#version");

	//アップデート
	g_version.find("#version_update_button").click(function(){
		var updateArray = {
			  'version_updateurl':		g_version.find("#version_updateurl").val()
			 ,'_csrftoken': g_CSRFTOKEN
		};
		showUpdatewaitDialog();

		$.ajax({
			type: "post",
			url: "/setting/version/upgradecheck",
			timeout: 50000,
			data: updateArray,
			dataType: 'json',
			success: function(d){
				hideUpdatewaitDialog();
				if (d['result'] != "ok")
				{
					alert(g_Strings["NO_UPDATE_IS_REQUIRED"]);
					return ;
				}
				//アップデートできるらしいので、ユーザに聞く
				updateconfirm(
					 function(e){
						$.ajax({
							type: "post",
							url: "/setting/version/upgraderun",
							timeout: 50000,
							data: updateArray,
							dataType: 'json',
							success: function(d){
								//再起動させる
								showUpdatewaitDialog( { htmlobject: $("#loadingwait_dialog_upgrade") } )
								rebootfunction("");
								return ;
							},
							error: function(d){
								//再起動させる
								showUpdatewaitDialog( { htmlobject: $("#loadingwait_dialog_upgrade") } )
								rebootfunction("");
								return ;
							}
						});
					}
					,function(e){
						hideUpdatewaitDialog();
					}
				);
				return ;
			},
			error: function(d){
				hideUpdatewaitDialog();
				return ;
			}
		});
	});
	g_version.find("#version_reboot_button").click(function(){
		rebootConfirm(function(e){
			//再起動させる
			showUpdatewaitDialog( { htmlobject: $("#loadingwait_dialog_reboot") } )
			rebootfunction("");
		}
		,function(e){
			hideUpdatewaitDialog();
		} );
	});
	g_version.find("#version_shutdown_button").click(function(){
		shutdownConfirm(function(e){
			showUpdatewaitDialog( { htmlobject: $("#loadingwait_dialog_shutdown") } )
			var updateArray = { };
			$.ajax({
				type: "post",
				url: "/setting/version/shutdown",
				timeout: 50000,
				data: updateArray,
				dataType: 'json',
				success: function(d){
					if (d['result'] == "error")
					{//エラーになりました
						hideUpdatewaitDialog();
						g_recong.find(".bunner_error").show();
						return ;
					}

					return ;
				},
				error: function(d){
					return ;
				}
			});
		}
		,function(e){
			hideUpdatewaitDialog();
		} );
	});

	//エクスポート
	g_version.find('#version_export_button').click(function(){
		download_post_form('/setting/version/export');
	});

	//サポートログ
	g_version.find('#version_suport_log').click(function(){
		download_post_form('/setting/version/download/supportfile');
	});

	//インポート
	g_version.find('#version_import_button').click(function(){
		$('#version_import_upload').click();
	});
	g_version.find('#version_import_upload').change(function(){
		var uploadObject = $(this);
		if ( uploadObject.val() == "") return ;

		var url = "/setting/version/import";
		var arr = {
			"_csrftoken":	g_CSRFTOKEN
		};

		uploadObject.upload(url,arr,function(d) {
			if ( d['result'] != "ok" )
			{
				alertErrorDialog(d,"script_scripteditor_annotation");
				return ;
			}
			//再起動させる
			showUpdatewaitDialog( { htmlobject: $("#loadingwait_dialog_import") } )
			rebootfunction("");
			return ;
		}, 'json');
	});


}

function version_load(typepath)
{
	version_init();

	g_version.find("#version_versionid").text(g_SETTING["version_number"]).change();
	g_version.find("#version_osversionid").text(g_SETTING["version_osversion"]).change();

	g_version.find("#version_uuid").text(g_SETTING["httpd__uuid"]).change();
	g_version.find("#version_updateurl").val(g_SETTING["version_updateurl"]).change();

	g_version.find("#version_log_textarea_autoupdate_div").show();
	g_version.find("#version_log_textarea_updatetop_div").hide();

	//ログはでかいので非同期で読み込む
	g_version.find("#version_log_textarea").val("ログを読み込んでいます・・・");
	
	var arr = { };
	arr['_csrftoken'] =  g_CSRFTOKEN;
	var logObject = $.post("/setting/version/log?line=100",arr,function(data){
		var logTextArea = g_version.find("#version_log_textarea");
		logTextArea.val( logObject.responseText ).scrollTop( logTextArea[0].scrollHeight );

		logupdate();
	} );

	$.mobile.changePage("#version", { transition: "flip" });
}

function logupdate()
{
	var logTextArea = g_version.find("#version_log_textarea");

	var ismouseenter = false;
	logTextArea.focus(function(){
		g_version.find("#version_log_textarea_autoupdate_div").hide();
		g_version.find("#version_log_textarea_updatetop_div").show();
		ismouseenter = true; 
	})
	.blur(function(){
		g_version.find("#version_log_textarea_autoupdate_div").show();
		g_version.find("#version_log_textarea_updatetop_div").hide();
		ismouseenter = false;
		logcallbackfunction(); 
	})
	g_version.find("#version_log_textarea_updatetop_div").click(function(){
		g_version.find("#version_log_textarea_autoupdate_div").show();
		g_version.find("#version_log_textarea_updatetop_div").hide();
		ismouseenter = false;
		logcallbackfunction(); 
	});

	var logcallbackfunction = function(){
		//もうログのページにいないなら没
		if ($.mobile.activePage.attr('id') != "version") return ;
		//マウスが入っているなら更新停止
		if (ismouseenter) return ;

		//ログを更新する
		var arr = { };
		arr['_csrftoken'] =  g_CSRFTOKEN;
		var logObject = $.post("/setting/version/log?update=1",arr,function(data){
			//戻ってきたテキストを現在表示されているテキストとでマージする
			logTextArea.val( logObject.responseText ).scrollTop( logTextArea[0].scrollHeight );

			setTimeout( logcallbackfunction , 1000);
		} );
	};

	setTimeout( logcallbackfunction , 1000);
}


function rebootfunction(baseurl)
{
	var rebootwaitcount = 0;
	var rebootwaitfunction = function()
	{
		$.ajax({
			type: "post",
			url: baseurl + "/auth/is_auth?randtime=" + ((new Date).getTime()),
			timeout: 1000,
			data: {},
			dataType: 'jsonp',
			success: function(d){
				if (d['result'] == "error")
				{//エラーになりました
					setTimeout( rebootwaitfunction , 2000);
					return ;
				}

				//OK読み込みが完了しした
				gotohref(baseurl + '/remocon/');
				return ;
			},
			error: function(d){
				setTimeout( rebootwaitfunction , 2000);
				return ;
			}
		});
	}

	var updateArray = { };
	updateArray['_csrftoken'] =  g_CSRFTOKEN;
	$.ajax({
		type: "post",
		url: "/setting/version/reboot",
		timeout: 50000,
		data: updateArray,
		dataType: 'json',
		success: function(d){
			if (d['result'] == "error")
			{//エラーになりました
				hideUpdatewaitDialog();
				g_recong.find(".bunner_error").show();
				return ;
			}
			setTimeout( rebootwaitfunction , 2000);
			return ;
		},
		error: function(d){
			setTimeout( rebootwaitfunction , 2000);
			return ;
		}
	});
}



var g_scriptselect = null;

function scriptselect_init()
{
	if (g_scriptselect) return ;
	g_scriptselect = $("#scriptselect");

	//検索したときにリアルタイムに書き換える
	g_scriptselect.find(".search").keyup(function(){
		g_scriptselect.find(".filelist_ul").html("検索中・・・・");

		var arr = { };
		arr["typepath"] = g_scriptselect.find(".typepath").val();
		arr["search"] = $(this).val();
		arr["_csrftoken"] =  g_CSRFTOKEN;
		$.post("/remocon/fileselectpage/find",arr, function(d){
			if ( d['result'] != "ok" )
			{
				alertErrorDialog(d,"remocon_fileselectpage");
				return ;
			}

			var totaldom = "";
			var htmltemplate = g_scriptselect.find(".filelist_li_sample").html();
			for(var arraykey in d)
			{
				var s = arraykey.split("_");
				if ( !(s[0] == "file" && s[2] == "name" && s[3] == undefined )) continue;
				var key = s[1];

				var html = htmltemplate;
				html = html.replace(/%KEY%/g , key);
				html = html.replace(/%ICON%/g , d["file_" + key + "_icon"] );
				html = html.replace(/%NAME%/g , d["file_" + key + "_name"] );
				html = html.replace(/%SIZE%/g , sizescale(d["file_" + key + "_size"]) );
				html = html.replace(/%DATE%/g , d["file_" + key + "_date"]);

				totaldom += html;
			}
			g_scriptselect.find(".filelist_ul").html(totaldom);

			//イベントを設定する.
			g_scriptselect.find(".filelist_ul li").each(function(){
				var li = $(this);
				var key = li.find(".filekey").text();
				li.find(".delete").click(function(){
					delConfirm(function(e){
						showUpdatewaitDialog();
						fileselectpage_delete(arr["typepath"] , li.find(".filename").html() , function(){
							//削除したので画面を再描画させる
							g_scriptselect.find(".search").keyup(); 

							hideUpdatewaitDialog();
						} );
					}
					,function(e){
						hideUpdatewaitDialog();
					} );
					return false;
				});
				li.find(".check").click(function(){
					scripteditor_load(arr["typepath"] , li.find(".filename").html());
					return false;
				});
			});
		},'json');
	});
	
	g_scriptselect.find(".new").click(function(){
		var filename = "";
		var isok
		newnamePrompt(function(e){
			filename = e.value;
			hideUpdatewaitDialog();
			if (!filename)
			{
				return false;
			}
			if ( filename.match( /^.*[(\\|/|:|\*|?|\"|<|>|\|)].*$/ ) ) // " 
			{
				alert("ファイル名に利用できない文字を含んでいます");
				return false;
			}
			//拡張子がなかったり変だったら .js をつける
			if (! filename.match( /\.js$/ ) )
			{
				filename = filename + ".js";
			}
			//新規作成したので再描画させとく.
			g_scriptselect.find(".search").keyup();
			//編集ページに移動
			scripteditor_load( g_scriptselect.find(".typepath").val() , filename);
		},
		function(){
			hideUpdatewaitDialog();
			return false;
		});
		return false;
	});
}

function scriptselect_load(typepath)
{
	scriptselect_init();

	g_scriptselect.find(".typepath").val(typepath);
	g_scriptselect.find(".search").val("").keyup();
	if (typepath == 'script_scenario')
	{
		g_scriptselect.find(".scriptselect_is_script_command").hide();
		g_scriptselect.find(".scriptselect_is_script_scenario").show();
	}
	else if (typepath == 'script_command')
	{
		g_scriptselect.find(".scriptselect_is_script_scenario").hide();
		g_scriptselect.find(".scriptselect_is_script_command").show();
	}
	$.mobile.changePage("#scriptselect", { transition: "flip" });
}


g_scripteditorObjcet = null;
g_editor = null;
g_editor_widgets = [];

function errorMessageToLineAndMessage(errormessage)
{
	//エラーの位置をハイライト
	var regexp = /@line:([0-9]+)/.exec(errormessage);
	var lineCount = parseInt(regexp[1]) - 1;
	//ファイル名があるとカッコ悪いので削る
	regexp = /(@exception:.+)/.exec(errormessage);
	if (regexp && regexp.length > 1)
	{
		errormessage = regexp[1];
	}
	return {
		 "line":    lineCount
		,"message": errormessage
	};
}

function showScriptError(errormessage,stdout,returnValue)
{
	var errArr = errorMessageToLineAndMessage(errormessage);

	if (errArr.line > 0)
	{
		//エラーを表示する.
		g_editor.operation(function(){
			var msg = document.createElement("div");
			var icon = msg.appendChild(document.createElement("span"));
			icon.innerHTML = "!!";
			icon.className = "lint-error-icon";
			msg.appendChild(document.createTextNode(errArr.message));
			msg.className = "lint-error";
			g_editor_widgets.push(g_editor.addLineWidget(errArr.line, msg, {coverGutter: false, noHScroll: true}));
		});
		var info = g_editor.getScrollInfo();
		var after = g_editor.charCoords({line: g_editor.getCursor().line + 1, ch: 0}, "local").top;
		if (info.top + info.clientHeight < after)
		g_editor.scrollTo(null, after - info.clientHeight + 3);
	}

	//コンパイルエラーをセットする.
	g_scripteditorObjcet.find(".scripterror .errormessage").text(errArr.message);
	showScriptStdout(stdout,returnValue);
}

function showScriptStdout(stdout,returnValue)
{
	var stdoutmessage = longText(stdout,"クリックで表示します。",15);
	if (returnValue && returnValue != "")
	{
		stdoutmessage += g_Strings['result'] + ":" + longText(returnValue,"クリックで表示します。",15);
	}

	//メッセージを出力する
	g_scripteditorObjcet.find(".scriptnotify .stdoutmessage").html( stdoutmessage );
	g_scripteditorObjcet.find(".scriptnotify").show();
}

function scripteditor_init()
{
	if (g_scripteditorObjcet) return ;
	g_scripteditorObjcet = $("#scripteditor");

	g_scripteditorObjcet.live('pageshow', function(event, ui) {
		g_scripteditorObjcet.find(".filecontexnt").parent().show();
		
		g_editor.refresh();	//ページが表示されたタイミングで 喝入れが必要らしい
	});

	g_scripteditorObjcet.find(".run").click(function(){
		//読込中にする
		showUpdatewaitDialog();

		//エラーを隠す.
		g_scripteditorObjcet.find(".scripterror").hide();
		g_scripteditorObjcet.find(".scriptnotify").hide();

		//前回表示していたエラーを消す.
    	g_editor.operation(function(){
			for (var i = 0; i < g_editor_widgets.length; ++i)
			{
				g_editor.removeLineWidget(g_editor_widgets[i]);
			}
			g_editor_widgets.length = 0;
    	});
		g_editor.refresh();	//ページが表示されたタイミングで 喝入れが必要らしい

		var updateArray = {};
		updateArray["typepath"] = g_scripteditorObjcet.find(".typepath").val();
		updateArray["filename"] = g_scripteditorObjcet.find(".filename").html();
		updateArray["filecontexnt"] = g_editor.getValue();
		updateArray["_csrftoken"] =  g_CSRFTOKEN;

		//保存して実行
		$.ajax({
				type: "post",
				url: "/script/scripteditor/save",
				timeout: 50000,
				data: updateArray,
				dataType: 'json',
				success: function(d){
					//実行結果を取得する
					hideUpdatewaitDialog();

					if ( d['result'] != "ok" )
					{
						if (d['code'] == "39020")
						{
							//エラーの位置をハイライト
							showScriptError(d['message'],d['stdout'],d['returnValue']);
						}
						else
						{
							alertErrorDialog(d,"script_scripteditor_save");
						}
						return ;
					}

					if (updateArray["typepath"] == "script_command")
					{//コマンドだと、引数が必要なので、ダイアログを出す
						commandrundialog_load( updateArray["filename"] , function(d){
							g_commandrundialogObject.dialog("close");
							g_editor.refresh();	//ページが表示されたタイミングで 喝入れが必要らしい
							//ダイアログからコマンドを実行した戻り
							if ( d['result'] != "ok" )
							{
								if (d['code'] == "38020")
								{
									//エラーの位置をハイライト
									showScriptError(d['message'],d['stdout'],d['returnValue']);
								}
								else
								{
									alertErrorDialog(d,"script_scripteditor_command_runに失敗しました");
								}
								return ;
							}

							//結果を表示する.
							showScriptStdout(d['stdout'],d['returnValue']);
							return ;
						});
					}
					else
					{
						//結果を表示する.
						showScriptStdout(d['stdout'],d['returnValue']);
					}
				},
				error: function(d){
					hideUpdatewaitDialog();

					alert("script_scripteditor_save 未知のエラーが発生しました");
					return ;
				}
		});
	});

}

function scripteditor_load(typepath,filename)
{
	scripteditor_init();

	var updateArray = {};
	updateArray["typepath"] = typepath;
	updateArray["filename"] = filename;
	updateArray["_csrftoken"] =  g_CSRFTOKEN;

	g_scripteditorObjcet.find(".typepath").val(typepath);
	g_scripteditorObjcet.find(".filename").html(filename);
	if (typepath == 'script_scenario')
	{
		g_scripteditorObjcet.find(".scripteditor_is_script_command").hide();
		g_scripteditorObjcet.find(".scripteditor_is_script_scenario").show();
	}
	else if (typepath == 'script_command')
	{
		g_scripteditorObjcet.find(".scripteditor_is_script_scenario").hide();
		g_scripteditorObjcet.find(".scripteditor_is_script_command").show();
	}

	if (!g_editor)
	{
      	g_editor = CodeMirror.fromTextArea( g_scripteditorObjcet.find(".filecontexnt").get(0) , {
			 mode: "text/javascript"
			,styleActiveLine: true
			,styleSelectedText: true
			,lineNumbers: true
			,lineWrapping: true
			,matchBrackets: true
			,selectionHint: true
			,extraKeys: {
				 "Ctrl-Space": "autocomplete" 
				,"Ctrl-S": function(){ g_scripteditorObjcet.find(".run").click(); }
				,"F5": function(){ g_scripteditorObjcet.find(".run").click(); }
				,"F9": function(){ g_scripteditorObjcet.find(".run").click(); }
			}
			,tabMode: "indent"
		});
		
		$(window).resize(function(){
			if (!g_editor) return ;
			g_editor.setSize(  $(".contents_wrapper").width() - 30 , g_scripteditorObjcet.height() - 300);
		});
		$(window).resize();
	}

	//ソースコードエディタを隠す。表示がおかしくなるため。 pageshowイベントで表示する.
	g_scripteditorObjcet.find(".filecontexnt").parent().hide();
	
	//エラーを隠す.
	g_scripteditorObjcet.find(".scripterror").hide();
	g_scripteditorObjcet.find(".scriptnotify").hide();

	//サーバにデータを送信
	$.ajax({
			type: "post",
			url: "/script/scripteditor/load",
			timeout: 50000,
			data: updateArray,
			dataType: 'text',
			success: function(d){
				g_scripteditorObjcet.find(".filecontexnt").html(d);
				g_editor.setValue(d);
				$.mobile.changePage("#scripteditor", { transition: "flip" });
				return ;
			},
			error: function(d){
				return ;
			}
	});

}


var g_commandrundialogObject = null;
var g_commandrundialogSaveCallback = null;
var g_commandrundialogLastLoadFilename = null;

//コマンド選択
function commandrundialog_action_command(filename)
{
	//選択画面に表示する画像
	if (filename != "")
	{
		g_commandrundialogObject.find("#exec_command").val(filename).change();
		g_commandrundialogObject.find("#exec_command_display").html(filename);
		g_commandrundialogObject.find("#exec_command_select_icon img").prop("src","/jscss/images/icon_script_command.png");
	}
	else
	{
		g_commandrundialogObject.find("#exec_command").val("").change();
		g_commandrundialogObject.find("#exec_command_display").html("未定義");
		g_commandrundialogObject.find("#exec_command_select_icon img").prop("src","/jscss/images/icon_script_command_nocommand.png");
	}

	for ( var key = 1 ; key < 6 ; key ++){
		g_commandrundialogObject.find("#exec_command_args" + key + "_mustlabel").text("未定義");
		g_commandrundialogObject.find("#exec_command_args" + key + "_label").text("未定義の引数"+key);
		g_commandrundialogObject.find("#exec_command_args" + key + "").prop("placeholder","");
		g_commandrundialogObject.find("#exec_command_args" + key + "").prop("type","text");
		g_commandrundialogObject.find("#exec_command_args" + key + "").removeClass("");
	}

	if (filename != "")
	{
		//ファイル内のアノテーションによって、コメントを動的に変化させる.
		fileselectpage_getAnnotation( "script_command" , filename , function(d){
			for ( var key = 1 ; key < 6 ; key ++){
				if (d["args_" + key + "_must"] == "must")
				{
					g_commandrundialogObject.find("#exec_command_args" + key + "_mustlabel").text("必須");
				}
				else if (d["args_" + key + "_must"] == "optionl")
				{
					g_commandrundialogObject.find("#exec_command_args" + key + "_mustlabel").text("任意");
				}
				else if (d["args_" + key + "_must"] == "none")
				{
					g_commandrundialogObject.find("#exec_command_args" + key + "_mustlabel").text("利用しません");
					g_commandrundialogObject.find("#exec_command_args" + key + "").addClass("");
				}
				else
				{
					g_commandrundialogObject.find("#exec_command_args" + key + "").addClass("");
				}

				if ( d["args_" + key + "_name"] && d["args_" + key + "_name"] != "")
				{
					if (d["args_" + key + "_must"] != "none")
					{
						g_commandrundialogObject.find("#exec_command_args" + key + "_label").text(d["args_" + key + "_name"]);
					}
				}
				g_commandrundialogObject.find("#exec_command_args" + key + "").prop("placeholder",d["args_" + key + "_def"]);
			}
		});
	}
}


function commandrundialog_load( filename , callback)
{
	commandrundialog_init();

	commandrundialog_action_command(filename);
	g_commandrundialogSaveCallback = callback;

	$.mobile.changePage('#commandrundialog', {transition: "slidedown", role: 'dialog'}); 
	if (g_commandrundialogLastLoadFilename != filename)
	{//実行するファイル名が違うときは、引数を空にクリアする。
		g_commandrundialogLastLoadFilename = filename;
		g_commandrundialogObject.find("input[type=text]").val("");
	}
}

function commandrundialog_init()
{
	if (g_commandrundialogObject) return ;
	g_commandrundialogObject = $("#commandrundialog");
	g_commandrundialogSaveCallback = null;

	g_commandrundialogObject.find("#exec_run_button").click(function(){
	
		var updateArray = {};
		updateArray["filename"] = g_commandrundialogObject.find("#exec_command").val();
		updateArray["args1"] = g_commandrundialogObject.find("#exec_command_args1").val();
		updateArray["args2"] = g_commandrundialogObject.find("#exec_command_args2").val();
		updateArray["args3"] = g_commandrundialogObject.find("#exec_command_args3").val();
		updateArray["args4"] = g_commandrundialogObject.find("#exec_command_args4").val();
		updateArray["args5"] = g_commandrundialogObject.find("#exec_command_args5").val();
		updateArray["_csrftoken"] =  g_CSRFTOKEN;

		//読込中にする
		showUpdatewaitDialog();

		//実行だけ
		$.ajax({
				type: "post",
				url: "/script/scripteditor/command_run",
				timeout: 50000,
				data: updateArray,
				dataType: 'json',
				success: function(d){
					hideUpdatewaitDialog();
					if (g_commandrundialogSaveCallback) g_commandrundialogSaveCallback(d);
					g_commandrundialogSaveCallback = null;
					return ;
				},
				error: function(d){
					hideUpdatewaitDialog();
					alert("未知のエラーが発生しました");

					if (g_commandrundialogSaveCallback) g_commandrundialogSaveCallback(d);
					g_commandrundialogSaveCallback = null;
					return ;
				}
		});
	});
}


var g_webapi = null;
function webapi_init()
{
	if (g_webapi) return ;
	g_webapi = $("#webapi");

	var apikey = g_SETTING["webapi_apikey"];

	//家電の情報の取得
	g_webapi.find("#webapi_elec_getinfo_button").click(function(){
		var url = window.location.href.replace(/\/edit\/.*$/,"");
		url += "/api/elec/getinfo";
		url += "?webapi_apikey=" + apikey;
		url += "&elec=" + g_webapi.find("#webapi_elec_forgetinfo").val();

		window.open(url);
	});
	g_webapi.find("#webapi_remote_elec_getinfo_button").click(function(){
		var url = "https://fhc.rti-giken.jp" ;
		url += "/api/elec/getinfo";
		url += "?webapi_apikey=" + apikey;
		url += "&user=" + g_SETTING["account_usermail"];
		url += "&elec=" + g_webapi.find("#webapi_elec_forgetinfo").val();

		window.open(url);
	});

	//家電のアクションの取得
	g_webapi.find("#webapi_elec_getactionlist_button").click(function(){
		var url = window.location.href.replace(/\/edit\/.*$/,"");
		url += "/api/elec/getactionlist";
		url += "?webapi_apikey=" + apikey;
		url += "&elec=" + g_webapi.find("#webapi_elec_forgetactionlist").val();

		window.open(url);
	});

	//家電の一覧の取得
	g_webapi.find("#webapi_elec_getlist_button").click(function(){
		var url = window.location.href.replace(/\/edit\/.*$/,"");
		url += "/api/elec/getlist";
		url += "?webapi_apikey=" + apikey;

		window.open(url);
	});

	//家電に関するすべての情報の取得
	g_webapi.find("#webapi_elec_detail_button").click(function(){
		var url = window.location.href.replace(/\/edit\/.*$/,"");
		url += "/api/elec/detaillist";
		url += "?webapi_apikey=" + apikey;

		window.open(url);
	});

	//家電操作ボタン
	g_webapi.find("#webapi_elec_action_button").click(function(){
		var url = window.location.href.replace(/\/edit\/.*$/,"");
		url += "/api/elec/action";
		url += "?webapi_apikey=" + apikey;
		url += "&elec=" + g_webapi.find("#webapi_elec_foraction").val();
		url += "&action=" + g_webapi.find("#webapi_action_foraction").val();

		window.open(url);
	});
	g_webapi.find("#webapi_remote_elec_action_button").click(function(){
		var url = "https://fhc.rti-giken.jp" ;
		url += "/api/elec/action";
		url += "?webapi_apikey=" + apikey;
		url += "&user=" + g_SETTING["account_usermail"];
		url += "&elec=" + g_webapi.find("#webapi_elec_foraction").val();
		url += "&action=" + g_webapi.find("#webapi_action_foraction").val();

		window.open(url);
	});

	//センサーボタン
	g_webapi.find("#webapi_sensor_button").click(function(){

		var url = window.location.href.replace(/\/edit\/.*$/,"");
		url += "/api/sensor/get";
		url += "?webapi_apikey=" + apikey;
		url += "&sensor=" + g_webapi.find("#webapi_sensor").val();

		window.open(url);
	});
	g_webapi.find("#webapi_remote_sensor_button").click(function(){
		var url = "https://fhc.rti-giken.jp" ;
		url += "/api/sensor/get";
		url += "?webapi_apikey=" + apikey;
		url += "&user=" + g_SETTING["account_usermail"];
		url += "&sensor=" + g_webapi.find("#webapi_sensor").val();

		window.open(url);
	});

	//合成音声ボタン
	g_webapi.find("#webapi_speak_button").click(function(){

		var url = window.location.href.replace(/\/edit\/.*$/,"");
		url += "/api/speak";
		url += "?webapi_apikey=" + apikey;
		url += "&str=" + g_webapi.find("#webapi_speak_str").val();
		url += "&noplay=" + g_webapi.find("#webapi_speak_noplay").val();
		url += "&async=" + g_webapi.find("#webapi_speak_async").val();

		window.open(url);
	});

	//登録されている音楽ボタン
	g_webapi.find("#webapi_play_button").click(function(){

		var url = window.location.href.replace(/\/edit\/.*$/,"");
		url += "/api/play";
		url += "?webapi_apikey=" + apikey;
		url += "&name=" + g_webapi.find("#webapi_play_name").val();
		url += "&async=" + g_webapi.find("#webapi_play_async").val();

		window.open(url);
	});

	//認識されている音声認識ワード
	g_webapi.find("#webapi_recong_list_button").click(function(){

		var url = window.location.href.replace(/\/edit\/.*$/,"");
		url += "/api/recong/list";
		url += "?webapi_apikey=" + apikey;

		window.open(url);
	});
	//音声認識ワードの文字列呼び出し
	g_webapi.find("#webapi_recong_firebystring_button").click(function(){

		var url = window.location.href.replace(/\/edit\/.*$/,"");
		url += "/api/recong/firebystring";
		url += "?webapi_apikey=" + apikey;
		url += "&str=" + g_webapi.find("#webapi_recong_firebystring_str").val();

		window.open(url);
	});

	//環境情報の取得
	g_webapi.find("#webapi_env_button").click(function(){

		var url = window.location.href.replace(/\/edit\/.*$/,"");
		url += "/api/env";
		url += "?webapi_apikey=" + apikey;

		window.open(url);
	});
	

	g_webapi.find("#webapi_keyreset_button").click(function(){
		var updateArray = {}
		updateArray["_csrftoken"] =  g_CSRFTOKEN;

		webapiKeyChangeConfirm(function(){
			$.ajax({
				type: "post",
				url: "/setting/webapi/resetkey",
				timeout: 50000,
				data: updateArray,
				dataType: 'json',
				success: function(d){
					hideUpdatewaitDialog();

					if (d['result'] == "error")
					{//エラーになりました
						return ;
					}
					g_webapi.find("#webapi_apikey").text(d["webapi_apikey"]);
					apikey = d["webapi_apikey"];

					g_SETTING = d;
					return ;
				},
				error: function(d){
					hideUpdatewaitDialog();
					return ;
				}
			});
			},
			function(){
				hideUpdatewaitDialog();
			}
		);
	});

	//家電一覧の埋め込み
	g_webapi.find("#webapi_elec_foraction").change(function(){
		make_macro_combo_action(false,$(this).val() , g_webapi.find("#webapi_action_foraction")  );
	});
	//mp3選択フォーム
	g_webapi.find("#webapi_play_mp3_select_button").click(function(){
		fileselectpage_load( "tospeak_mp3" , g_webapi.find("#webapi_play_name").val() , g_Strings["SELECT_SPEAK_FILE"]
		, function(filename){
			g_webapi.find("#webapi_play_name").val(filename);
		});
	});
	//google音声認識
	g_webapi.find("#webapi_recong_firebystring_str input").bind("webkitspeechchange", function() {
		var str = $(this).val();
		if (str == "") return ;
		g_webapi.find("#webapi_recong_firebystring_button").click();
	});
}

function webapi_load()
{
	webapi_init();
	g_webapi.find(".error").hide();
	make_macro_combo_elec(false,  g_webapi.find("#webapi_elec_foraction")  );
	make_macro_combo_elec(false,  g_webapi.find("#webapi_elec_forgetinfo")  );
	make_macro_combo_elec(false,  g_webapi.find("#webapi_elec_forgetactionlist")  );

	g_webapi.find("#webapi_elec_foraction").change();
	g_webapi.find("#webapi_elec_forgetinfo").change();
	g_webapi.find("#webapi_elec_forgetactionlist").change();

	var arr = {}
	arr["_csrftoken"] =  g_CSRFTOKEN;

	$.ajax({
		type: "post",
		url: "/setting/sensor/getall",
		timeout: 50000,
		data: arr,
		dataType: 'json',
		success: function(d){
			if ( d['result'] != "ok" )
			{
				alertErrorDialog(d,"setting_sensor_get");
				return ;
			}
			$("#webapi_sensor_temp").text(d["temp"]);
			$("#webapi_sensor_lumi").text(d["lumi"]);
		},
		error: function(d){
		}
	});
	
	g_webapi.find("#webapi_apikey").text(g_SETTING["webapi_apikey"]);


	$.mobile.changePage("#webapi", { transition: "flip" });
	g_webapi.find("input[type='checkbox']").checkboxradio("refresh");
	g_webapi.find("input[type='select']").selectmenu("refresh");
}
