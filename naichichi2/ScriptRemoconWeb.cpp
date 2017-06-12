//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#include "common.h"
#include "MainWindow.h"
#include "ScriptRemoconWeb.h"
#include "XLFileUtil.h"
#include "XLStringUtil.h"
#include "XLSocket.h"
#include "SystemMisc.h"


class ScriptRemoconWebOrderby
{
public:
	//remocon の index を作る処理。
	//order があるので、ソートが必要。 DB使えば order by で一瞬なのだが・・・
	ScriptRemoconWebOrderby(const map<string,string>& configmap);
	//メモリ破棄
	virtual ~ScriptRemoconWebOrderby();

	//remocon の index status を json で返す.
	string DrawJsonStatus(const string & appendOption) const;
	//remocon の index を html で返す
	string DrawHTML(const string& htmlsrc) const;
	//remocon の機材の id をソートされた順番で返す
	vector<unsigned int> getElecID_Array(bool isGetHiddenElec) const;
	//remocon の機材のアクション id をソートされた順番で返す
	vector<unsigned int> getElecActionID_Array(const string& elec,unsigned int* elecID,bool isGetHiddenElec) const;
	string getElecKeyValue( unsigned int key1,const string& name ,const string& def = "") const;
	string getActionKeyValue( unsigned int key1,unsigned int key2,const string& name ,const string& def = "" ) const;

	//order でソートする必要がある・・・・
	struct sortactionwork
	{
		sortactionwork(int key) : order(9999),key(key){};

		int key;
		int order;
		map<string,string> arraykeys;
	};
	struct sortwork
	{
		sortwork(int key) : order(9999),key(key){};

		int key;
		int order;
		vector<sortactionwork*> action;
		map<string,string > arraykeys;
	};
private:

	vector<sortwork*> sortlist;
};


//remocon の index を作る処理。
//order があるので、ソートが必要。 DB使えば order by で一瞬なのだが・・・
ScriptRemoconWebOrderby::ScriptRemoconWebOrderby(const map<string,string>& configmap)
{
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		if ( ! XLStringUtil::strfirst(it->first.c_str(),"elec_") ) continue;

		const int key = atoi(it->first.c_str() + sizeof("elec_")-1);
		if (key <= 0) continue;

		sortwork * p = NULL;
		for(auto findit = this->sortlist.begin() ; findit != this->sortlist.end() ; ++findit )
		{
			if ( (*findit)->key == key )
			{
				p = *findit;
				break;
			}
		}
		if (!p)
		{
			p = new sortwork(key);
			this->sortlist.push_back(p);
		}

		const char * actionpos = strstr(it->first.c_str(),"_action_");
		if (actionpos)
		{
			const int actionkey = atoi(actionpos + sizeof("_action_")-1);
			if (actionkey <= 0) continue;

			sortactionwork * pp = NULL;
			for(auto findit = p->action.begin() ; findit != p->action.end() ; ++findit )
			{
				if ( (*findit)->key == actionkey )
				{
					pp = *findit;
					break;
				}
			}
			if (!pp)
			{
				pp = new sortactionwork(actionkey);
				p->action.push_back(pp);
			}

			if (strstr(it->first.c_str(),"_order"))
			{
				pp->order = atoi(it->second.c_str());
			}
			pp->arraykeys.insert(*it);
		}
		else
		{
			if (strstr(it->first.c_str(),"_order"))
			{
				p->order = atoi(it->second.c_str());
			}
			p->arraykeys.insert(*it);
		}
	}

	//ここでやっとソートする.
	for(auto it = this->sortlist.begin() ; it != this->sortlist.end() ; ++it )
	{
		sort((*it)->action.begin() , (*it)->action.end() , [](const sortactionwork* a,const sortactionwork* b){
			return a->order < b->order ;
		});
	}
	sort(this->sortlist.begin() , this->sortlist.end() , [](const sortwork* a,const sortwork* b){
		return a->order < b->order;
	});
}

//メモリ破棄
ScriptRemoconWebOrderby::~ScriptRemoconWebOrderby()
{
	for(auto it = this->sortlist.begin() ; it != this->sortlist.end() ; ++it )
	{
		for(auto itaction = (*it)->action.begin() ; itaction != (*it)->action.end() ; ++itaction )
		{
			delete *itaction;
		}
		delete *it;
	}
	this->sortlist.clear();
}

//remocon の index status を json で返す.
string ScriptRemoconWebOrderby::DrawJsonStatus(const string & appendOption) const
{
	

	//ソートの結果で出力する文字列を作る
	string jsonstring;
	for(auto it = sortlist.begin() ; it != sortlist.end() ; ++it )
	{
		for(auto arrayIT = (*it)->arraykeys.begin() ; arrayIT != (*it)->arraykeys.end() ; ++arrayIT )
		{
			jsonstring += string(",") + XLStringUtil::jsonescape(arrayIT->first) + string(": ") + _A2U(XLStringUtil::jsonescape(arrayIT->second)) + "";
		}
		
		for(auto itaction = (*it)->action.begin() ; itaction != (*it)->action.end() ; ++itaction )
		{
			for(auto arrayIT = (*itaction)->arraykeys.begin() ; arrayIT != (*itaction)->arraykeys.end() ; ++arrayIT )
			{
				jsonstring += string(",") + XLStringUtil::jsonescape(arrayIT->first) + string(": ") + _A2U(XLStringUtil::jsonescape(arrayIT->second)) + "";
			}
		}
	}

	//追加オプション
	if (!appendOption.empty())
	{
		jsonstring += string(",") + _A2U(appendOption);
	}
	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";

	return jsonstring;
}

//remocon の機材の id をソートされた順番で返す
vector<unsigned int> ScriptRemoconWebOrderby::getElecID_Array(bool isShowall) const
{
	

	vector<unsigned int> ret;

	//ソートの結果で出力する文字列を作る
	string jsonstring;
	for(auto it = sortlist.begin() ; it != sortlist.end() ; ++it )
	{
		ret.push_back( (*it)->key );
	}
	return ret;
}

//remocon の機材のアクション id をソートされた順番で返す
vector<unsigned int> ScriptRemoconWebOrderby::getElecActionID_Array(const string& elec,unsigned int* elecID,bool isShowall) const
{
	

	vector<unsigned int> ret;

	//ソートの結果で出力する文字列を作る
	string jsonstring;
	for(auto it = sortlist.begin() ; it != sortlist.end() ; ++it )
	{
		for(auto arrayIT = (*it)->arraykeys.begin() ; arrayIT != (*it)->arraykeys.end() ; ++arrayIT )
		{
			if ( ! strstr( arrayIT->first.c_str() , "_type" ) && !strstr( arrayIT->first.c_str() , "_action_" ) )
			{
				continue;
			}
			if (arrayIT->second == elec)
			{
				for(auto itaction = (*it)->action.begin() ; itaction != (*it)->action.end() ; ++itaction )
				{
					ret.push_back( (*itaction)->key );
				}
				*elecID = (*it)->key;
				return ret;
			}
			break;
		}
	}
	//not found!!
	*elecID = 0;
	return ret;
}

string ScriptRemoconWebOrderby::getElecKeyValue( unsigned int key1,const string& name,const string& def ) const
{
	const string findname = "elec_" + num2str(key1) + "_" + name;
	for(auto it = sortlist.begin() ; it != sortlist.end() ; ++it )
	{
		if ( (*it)->key != key1 ) continue;

		for(auto arrayIT = (*it)->arraykeys.begin() ; arrayIT != (*it)->arraykeys.end() ; ++arrayIT )
		{
			if (arrayIT->first != findname) continue;

			return arrayIT->second;
		}
	}
	return def;
}
string ScriptRemoconWebOrderby::getActionKeyValue( unsigned int key1,unsigned int key2,const string& name ,const string& def) const
{
	const string findname = "elec_" + num2str(key1) + "_action_" + num2str(key2) + "_" + name;
	for(auto it = sortlist.begin() ; it != sortlist.end() ; ++it )
	{
		if ( (*it)->key != key1 ) continue;

		for(auto itaction = (*it)->action.begin() ; itaction != (*it)->action.end() ; ++itaction )
		{
			if ( (*itaction)->key != key2 ) continue;
			for(auto arrayIT = (*itaction)->arraykeys.begin() ; arrayIT != (*itaction)->arraykeys.end() ; ++arrayIT )
			{
				if (arrayIT->first != findname) continue;

				return arrayIT->second;
			}
		}
	}
	return def;
}

//remocon の index を html で返す
string ScriptRemoconWebOrderby::DrawHTML(const string& htmlsrc) const
{
	
	string html = htmlsrc;

	//ソートの結果で出力する文字列を作る
	string patchhtml_remocon = XLStringUtil::between(html ,"%REMOCON_LOOP_START%", "%REMOCON_LOOP_END%" );
	string patchhtml_remoconaction = XLStringUtil::between(patchhtml_remocon ,"%REMOCONACTION_LOOP_START%" , "%REMOCONACTION_LOOP_END%");

	string patchhtml;
	for(auto it = sortlist.begin() ; it != sortlist.end() ; ++it )
	{
		string temphtml_remocon = patchhtml_remocon;
		const string prefix = "elec_" + num2str((*it)->key) + "_";

		if (! stringbool( mapfind((*it)->arraykeys,prefix + "showremocon") ) )
		{//リモコン画面にアイコンを表示しないらしいのでパス
			continue;
		}

		//描画するhtmlを入れます。
		string temphtml_remoconaction_total;

		const string type =  mapfind((*it)->arraykeys,prefix + "type") ;
		if (type == "SPACE")
		{//余白
			temphtml_remocon = XLStringUtil::template_if(temphtml_remocon,"%REMOCON_CONTENTSWITCH_SPACE_START%","%REMOCON_CONTENTSWITCH_SPACE_END%","%REMOCON_CONTENTSWITCH_ELEC_START%","%REMOCON_CONTENTSWITCH_ELEC_END%");
		}
		else 
		{//余白ではない実コンテンツ
			temphtml_remocon = XLStringUtil::template_if(temphtml_remocon,"%REMOCON_CONTENTSWITCH_ELEC_START%","%REMOCON_CONTENTSWITCH_ELEC_END%","%REMOCON_CONTENTSWITCH_SPACE_START%","%REMOCON_CONTENTSWITCH_SPACE_END%");

			temphtml_remocon = XLStringUtil::replace_low(temphtml_remocon,"%ELECID%",num2str((*it)->key) );
			temphtml_remocon = XLStringUtil::replace_low(temphtml_remocon,"%ICON%","/user/elecicon/" + XLStringUtil::htmlspecialchars( _A2U(mapfind((*it)->arraykeys,prefix + "elecicon")) ));
			temphtml_remocon = XLStringUtil::replace_low(temphtml_remocon,"%STATUSCOLOR%",XLStringUtil::htmlspecialchars(_A2U( mapfind((*it)->arraykeys,prefix + "statuscolor") )));
			temphtml_remocon = XLStringUtil::replace_low(temphtml_remocon,"%STATUS%",XLStringUtil::htmlspecialchars(_A2U( mapfind((*it)->arraykeys,prefix + "status") )));
			temphtml_remocon = XLStringUtil::replace_low(temphtml_remocon,"%TYPE%",XLStringUtil::htmlspecialchars(_A2U(type)));
			temphtml_remocon = XLStringUtil::replace_low(temphtml_remocon,"%CLICK_TO_MENU%",XLStringUtil::htmlspecialchars(_A2U( mapfind((*it)->arraykeys,prefix + "click_to_menu") )));

			const time_t timerFireTime = atou(mapfind((*it)->arraykeys,prefix + "timer_firetime"));
			if (timerFireTime > 0)
			{//タイマー部
				temphtml_remocon = XLStringUtil::replace_low(temphtml_remocon,"%REMOCON_USE_SETTIMER%","block" );
			}
			else
			{//タイマーは使わないので消滅させる
				temphtml_remocon = XLStringUtil::replace_low(temphtml_remocon,"%REMOCON_USE_SETTIMER%","none" );
			}
	
			//メニューダイアログ部
			for(auto itaction = (*it)->action.begin() ; itaction != (*it)->action.end() ; ++itaction )
			{
				string temphtml_remoconaction = patchhtml_remoconaction;

				const string actionprefix = "elec_" + num2str((*it)->key) + "_action_" + num2str( (*itaction)->key ) + "_";
				string optionalClass = "";
				if (! stringbool(mapfind((*itaction)->arraykeys,actionprefix + "showremocon")) )
				{
					continue;
				}

				const string actiontype = mapfind((*itaction)->arraykeys,actionprefix + "actiontype");
				if (actiontype == "SPACE")
				{//メニューダイアログの余白
					temphtml_remoconaction = XLStringUtil::template_if(temphtml_remoconaction,"%REMOCONACTION_CONTENTSWITCH_SPACE_START%","%REMOCONACTION_CONTENTSWITCH_SPACE_END%","%REMOCONACTION_CONTENTSWITCH_ACTION_START%","%REMOCONACTION_CONTENTSWITCH_ACTION_END%");
				}
				else 
				{
					temphtml_remoconaction = XLStringUtil::template_if(temphtml_remoconaction,"%REMOCONACTION_CONTENTSWITCH_ACTION_START%","%REMOCONACTION_CONTENTSWITCH_ACTION_END%","%REMOCONACTION_CONTENTSWITCH_SPACE_START%","%REMOCONACTION_CONTENTSWITCH_SPACE_END%");

					const string actionicon_use = mapfind((*itaction)->arraykeys,actionprefix + "actionicon_use");
					if (stringbool(actionicon_use))
					{//アクション/状態にもアイコンを表示する.
						temphtml_remoconaction = XLStringUtil::template_if(temphtml_remoconaction,"%STATUS_CONTENTSWITCH_ICON_START%","%STATUS_CONTENTSWITCH_ICON_END%","%STATUS_CONTENTSWITCH_STRING_START%","%STATUS_CONTENTSWITCH_STRING_END%");
						temphtml_remoconaction = XLStringUtil::replace_low(temphtml_remoconaction,"%ACTIONICON%","/user/actionicon/" + XLStringUtil::htmlspecialchars( _A2U( mapfind((*itaction)->arraykeys,actionprefix + "actionicon") ) ));
					}
					else
					{//アクション状態にはアイコンを表示しない.
						temphtml_remoconaction = XLStringUtil::template_if(temphtml_remoconaction,"%STATUS_CONTENTSWITCH_STRING_START%","%STATUS_CONTENTSWITCH_STRING_END%","%STATUS_CONTENTSWITCH_ICON_START%","%STATUS_CONTENTSWITCH_ICON_END%");
					}

					//名前.
					temphtml_remoconaction = XLStringUtil::replace_low(temphtml_remoconaction,"%ACTIONTYPE%",XLStringUtil::htmlspecialchars(_A2U(actiontype)) );

					//いろ
					temphtml_remoconaction = XLStringUtil::replace_low(temphtml_remoconaction,"%ACTIONCOLOR%",XLStringUtil::htmlspecialchars( _A2U( mapfind((*itaction)->arraykeys,actionprefix + "actioncolor") ) ) );					

					//タイマー？
					const string execflag = mapfind((*itaction)->arraykeys,actionprefix + "execflag");
					if (execflag == "timer" )
					{
						const string timer_type = mapfind((*itaction)->arraykeys,actionprefix + "timer_type");
						if (timer_type == "reset")
						{
							optionalClass += " resettimer";
						}
						else
						{
							optionalClass += " settimer";
						}
					}
					if ( stringbool(mapfind((*itaction)->arraykeys,actionprefix + "nostateremocon")) )
					{
						optionalClass += " nostateremocon";
					}


					//追加属性のセット
					temphtml_remoconaction = XLStringUtil::replace_low(temphtml_remoconaction,"%ACTIONOPTINALCLASS%",optionalClass) ;
				}
				temphtml_remoconaction_total += temphtml_remoconaction;
			}
		}
		patchhtml += XLStringUtil::between_replace(temphtml_remocon ,"%REMOCONACTION_LOOP_START%" , "%REMOCONACTION_LOOP_END%" , temphtml_remoconaction_total );
	}

	html = XLStringUtil::between_replace(html ,"%REMOCON_LOOP_START%", "%REMOCON_LOOP_END%" , patchhtml );
	return html;
}

string ScriptRemoconWeb::zeroToBar(const string& num)
{
	if ( atoi(num) <= 0 ) return "--";
	return num;
}

//リモコン画面のpopupmenuの幅を計算する.
int ScriptRemoconWeb::CalcPopupmenuWidthForPC(const string & itemCount)
{
	switch ( atoi(itemCount) )
	{
	case 3:
		return 340;
	case 5:
		return 565;
	case 6:
		return 675;
	case 7:
		return 790;
	case 8:
		return 900;
	default:
		return 450;
	}
}

//remocon の index を html で返す
string ScriptRemoconWeb::DrawHTML(const map<string,string>& configmap,const string& htmlsrc ,time_t now,bool is_local , bool is_link ,bool enableMultiRoom,bool is_showAD )
{
	ScriptRemoconWebOrderby index(configmap);
	string html = htmlsrc;

	//センサーの値を置換
	html = escape_replace(html,"%SENSOR_TEMP%",zeroToBar( mapfind(configmap,"sensor_temp_value") ) );
	html = escape_replace(html,"%SENSOR_LUMI%",zeroToBar( mapfind(configmap,"sensor_lumi_value") ) );

	//きせかえ
	html = escape_replace(html,"%ROOM_STRING%",mapfind(configmap,"system_room_string","リモコン") );
	html = escape_replace(html,"%ROOM_COLOR%",mapfind(configmap,"system_room_color","#000000") );
	html = escape_replace(html,"%ROOM_ICON%","/user/remocon/" + mapfind(configmap,"system_room_icon","room_icon1.png") );
	html = escape_replace(html,"%DEFAULT_FONT_COLOR%",mapfind(configmap,"remoconsetting_default_font_color","#000000") );
	html = escape_replace(html,"%DEFAULT_BACKGROUND_COLOR%",mapfind(configmap,"remoconsetting_default_background_color","#FFFFFF") );
	html = escape_replace(html,"%BACKGROUND_IMAGE%","/user/remocon/" + mapfind(configmap,"remoconsetting_background_image","bodybg_1.gif") );
	html = escape_replace(html,"%MENU_ICON%","/user/remocon/" + mapfind(configmap,"remoconsetting_menu_icon","list_1.png") );
	html = escape_replace(html,"%SENSOR_FONT_COLOR%",mapfind(configmap,"remoconsetting_sensor_font_color","#000000") );
	html = escape_replace(html,"%SENSOR_TEMP_STRING%",mapfind(configmap,"remoconsetting_sensor_temp_string","温度") );
	html = escape_replace(html,"%SENSOR_LUMI_STRING%",mapfind(configmap,"remoconsetting_sensor_lumi_string","明るさ") );
	html = escape_replace(html,"%FOOTERMENU_REMOCONEDIT_STRING%",mapfind(configmap,"remoconsetting_footermenu_remoconedit_string","リモコンを編集") );
	html = escape_replace(html,"%FOOTERMENU_REMOCONEDIT_ICON%","/user/remocon/" + mapfind(configmap,"remoconsetting_footermenu_remoconedit_icon","remoconedit_icon1.png") );
	html = escape_replace(html,"%FOOTERMENU_SETTING_STRING%",mapfind(configmap,"remoconsetting_footermenu_setting_string","設定画面に行く") );
	html = escape_replace(html,"%FOOTERMENU_SETTING_ICON%","/user/remocon/" + mapfind(configmap,"remoconsetting_footermenu_setting_icon","setting_icon1.png") );
	html = escape_replace(html,"%FOOTERMENU_FONT_COLOR%",mapfind(configmap,"remoconsetting_footermenu_font_color","#000000") );
	html = escape_replace(html,"%FOOTERMENU_BACKGROUND_COLOR%",mapfind(configmap,"remoconsetting_footermenu_background_color","#FFFFDD") );
	html = escape_replace(html,"%FOOTERMENU_BORDER_COLOR%",mapfind(configmap,"remoconsetting_footermenu_border_color","#CCCCCC") );
	html = escape_replace(html,"%FOOTERMENU_HOVER_COLOR%",mapfind(configmap,"remoconsetting_footermenu_hover_color","#FFF070") );
	html = escape_replace(html,"%DIALOG_CLOSE_STRING%",mapfind(configmap,"remoconsetting_dialog_close_string","close") );
	html = escape_replace(html,"%DIALOG_FONT_COLOR%",mapfind(configmap,"remoconsetting_dialog_font_color","#000000") );
	html = escape_replace(html,"%DIALOG_BACKGROUND_COLOR%",mapfind(configmap,"remoconsetting_dialog_background_color","#FFFFFF") );
	html = escape_replace(html,"%DIALOG_BORDER_COLOR%",mapfind(configmap,"remoconsetting_dialog_border_color","#AAAAAA") );
	html = escape_replace(html,"%BUTTON_FONT_COLOR%",mapfind(configmap,"remoconsetting_button_font_color","#000000") );
	html = escape_replace(html,"%BUTTON_BACKGROUND_COLOR1%",mapfind(configmap,"remoconsetting_button_background_color1","#f4f5f5") );
	html = escape_replace(html,"%BUTTON_BACKGROUND_COLOR2%",mapfind(configmap,"remoconsetting_button_background_color2","#dddddd") );
	html = escape_replace(html,"%BUTTON_BORDER_COLOR%",mapfind(configmap,"remoconsetting_button_border_color","#d7dada") );
	html = escape_replace(html,"%BUTTON_HOVER_COLOR1%",mapfind(configmap,"remoconsetting_button_hover_color1","#f4f5f5") );
	html = escape_replace(html,"%BUTTON_HOVER_COLOR2%",mapfind(configmap,"remoconsetting_button_hover_color2","#c6c3c3") );
	html = escape_replace(html,"%BUTTON_HOVER_BORDER_COLOR%",mapfind(configmap,"remoconsetting_button_hover_border_color","#bfc4c4") );
	html = escape_replace(html,"%BADGES_TIMER_ICON%","/user/remocon/" + mapfind(configmap,"remoconsetting_badges_timer_icon","timer.png") );
	
	const int dialogWidth = CalcPopupmenuWidthForPC( mapfind(configmap,"remoconsetting_dialog_iconcount_pc","4") );
	html = escape_replace(html,"%POPUPMENU_REMOCONEDIT_WIDTH_FORPC%",num2str(dialogWidth)+"px");
	html = escape_replace(html,"%POPUPMENU_REMOCONEDIT_CLOSEBUTTON_FORPC%",num2str(dialogWidth-50)+"px");

	if (enableMultiRoom)
	{
		html = escape_replace(html,"%IF_MULTIROOM_USE%","block");		
	}
	else
	{
		html = escape_replace(html,"%IF_MULTIROOM_USE%","none");
	}

	if (is_showAD)
	{
		const std::string uuid = XLStringUtil::htmlspecialchars_low( mapfind(configmap,"httpd__uuid") );
		const string adhtml = "<ifream id=\"ad_ifream\" src=\"https://rti-giken.jp/fhc/ad/" + uuid + "\" />";
		html = XLStringUtil::replace(html ,"%SHOW_AD_BUNNER%" , adhtml );
	}
	else
	{
		html = XLStringUtil::replace(html ,"%SHOW_AD_BUNNER%" , "" );
	}

	//追加ステータスを入れます。
	string jsonstring ;
	jsonstring += string(",") + XLStringUtil::jsonescape("httpd__url") + ": " 
		+ XLStringUtil::jsonescape(mapfind(configmap,"httpd__url")) ;
	//自動的にメニューを閉じない
	jsonstring += string(",") + XLStringUtil::jsonescape("remoconsetting_dialog_donot_autoclose") + ": " 
		+ XLStringUtil::jsonescape(mapfind(configmap,"remoconsetting_dialog_donot_autoclose")) ;
	//着信表示
	jsonstring += string(",") + XLStringUtil::jsonescape("sipevent_incoming") + ": " 
		+ XLStringUtil::jsonescape(mapfind(configmap,"sipevent_incoming")) ;
	//通話表示
	jsonstring += string(",") + XLStringUtil::jsonescape("sipevent_call") + ": " 
		+ XLStringUtil::jsonescape(mapfind(configmap,"sipevent_call")) ;
	//キーパッドを出すか
	const time_t sipevent_numpad = atoi(mapfind(configmap,"sipevent_numpad"));
	if (now < sipevent_numpad)
	{
		jsonstring += string(",") + XLStringUtil::jsonescape("sipevent_numpad") + ": 1" ;
	}

	if (is_local)
	{
		jsonstring += ",\"is_local\": 1";
	}
	if (is_link)
	{
		jsonstring += ",\"is_link\": 1";
	}
	if (enableMultiRoom)
	{
		jsonstring += ",\"is_multiroom\": 1";
	}
	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	html = XLStringUtil::replace_low(html,"%REMOCON_VALUE%", jsonstring  );

	return index.DrawHTML( html );
}



//remocon の index status を json で返す.
string ScriptRemoconWeb::DrawJsonStatus(const map<string,string>& configmap,const string & appendOption) 
{
	ScriptRemoconWebOrderby index(configmap);
	return index.DrawJsonStatus(appendOption);
}

//remocon の機材の id をソートされた順番で返す
vector<unsigned int> ScriptRemoconWeb::getElecID_Array(const map<string,string>& configmap,bool isGetHiddenElec) 
{
	ScriptRemoconWebOrderby index(configmap);
	return index.getElecID_Array(isGetHiddenElec);
}

//remocon の機材のアクション id をソートされた順番で返す
vector<unsigned int> ScriptRemoconWeb::getElecActionID_Array(const map<string,string>& configmap,const string& elec,unsigned int* elecID,bool isGetHiddenElec) 
{
	ScriptRemoconWebOrderby index(configmap);
	return index.getElecActionID_Array(elec,elecID,isGetHiddenElec);
}

string ScriptRemoconWeb::getElecKeyValue(const map<string,string>& configmap,unsigned int key1,const string& name ,const string& def) 
{
	ScriptRemoconWebOrderby index(configmap);
	return index.getElecKeyValue(key1,name ,def);
}

string ScriptRemoconWeb::getActionKeyValue(const map<string,string>& configmap,unsigned int key1,unsigned int key2,const string& name ,const string& def ) 
{
	ScriptRemoconWebOrderby index(configmap);
	return index.getActionKeyValue(key1,key2,name ,def);
}

string ScriptRemoconWeb::getElecActionPrefix(const map<string,string>& configmap,const string& type1, const string& type2)
{
	//システムで予約されているものはfireできません。
	if (type1 == "" || type1 == "none" || type1 == "SPACE" ) return "";
	if (type2 == "" || type2 == "none" || type2 == "SPACE" ) return "";

	int key1 ,key2 ;
	if( ! ScriptRemoconWeb::type2key(configmap,type1,type2,&key1,&key2) )
	{
		return "";
	}
	return "elec_" + num2str(key1)+ "_action_" + num2str(key2);
}


//CSRFトークンの取得
string ScriptRemoconWeb::GetCSRFToken(const XLHttpHeader& httpHeaders)
{
	const string token1 = httpHeaders.getCookieHeader("naichichi2");
	return XLStringUtil::sha1(token1);
}

//tpl読み込み static
string ScriptRemoconWeb::OpenTPL(const XLHttpHeader& httpHeaders,const string& tplname)  
{
	const string filename = GetWebRootPath(tplname);

	const auto header = httpHeaders.getHeaderPointer();
	const auto useragent = mapfind(header,"user-agent");

	string html = XLFileUtil::cat(filename);
	html = XLStringUtil::replace_low(html  ,"%CSRFTOKEN%" , GetCSRFToken(httpHeaders) );

	if ( strstr(useragent.c_str(),"WebKit" ))
	{
		html = XLStringUtil::replace_low(html  ,"%BCSS%" , "-webkit-");
	}
	else if ( strstr(useragent.c_str(),"Firefox"  ))
	{
		html = XLStringUtil::replace_low(html  ,"%BCSS%" , "-moz-");
	}
	else if ( strstr(useragent.c_str(),"Opera"  ))
	{
		html = XLStringUtil::replace_low(html  ,"%BCSS%" , "-o-");
	}
	else if ( strstr(useragent.c_str(),"MSIE" ))
	{
		html = XLStringUtil::replace_low(html  ,"%BCSS%" , "-ms-");
	}
	else
	{
		html = XLStringUtil::replace_low(html  ,"%BCSS%" , "");
	}

	return html;
}
//webrootからのパスを得る static
string ScriptRemoconWeb::GetWebRootPath(const string& path) 
{
	return MainWindow::m()->GetConfigBasePath("webroot/" + path);
}


//エラーを返す static
string ScriptRemoconWeb::ResultError(int code ,const string& msg)  
{
	

	bool isError = true;
	if(code == 10502)
	{//timer
		isError = false;
	}

	if(isError)
	{
		ERRORLOG("webがエラーになりました。 " << code << "\t" << msg );
	}

	const string jsonstring = string("{ \"result\": \"error\" , \"code\": \"") + num2str(code)  + "\", \"message\": " + XLStringUtil::jsonescape(msg) + "}";
	return _A2U(jsonstring.c_str());
}



//設定画面のページへのURLを設定し直します static
bool ScriptRemoconWeb::SettingPageIPAddressOverriade()
{
	const int port = MainWindow::m()->Config.GetInt("httpd__port", 80 );
	const auto interfaceMap = SystemMisc::GetIPAddressMap();
	auto it = interfaceMap.end();
#if _MSC_VER
	if ( interfaceMap.size() >= 1 )
	{//とりあえず最初に見つかった稼働しているものを使う.
		it = interfaceMap.begin();
	}
#else
	if ( interfaceMap.size() >= 1 )
	{
		it = interfaceMap.find("wlan0");
		if (it != interfaceMap.end() && !it->second.empty() )
		{//無線があれば無線を優先的に使う
		}
		else
		{
			it = interfaceMap.find("eth0");
			if (it != interfaceMap.end())
			{//有線ケーブル
			}
			else
			{//わかんないので最初に見つかった奴
				it = interfaceMap.begin();
			}
		}
	}
#endif
	if (it == interfaceMap.end())
	{//何もネットデバイスがない
		NOTIFYLOG("ネットワークデバイスがありませんでした。 127.0.0.1 にします。");
		MainWindow::m()->Config.Set("httpd__url", "http://127.0.0.1:" + num2str(port) );
		return false;
	}
	else
	{//見つかったものにする
		NOTIFYLOG("IPアドレス:" + it->second + "(" + it->first + ")を利用します。");
		MainWindow::m()->Config.Set("httpd__url", "http://" + it->second + ":" + num2str(port) );
		return true;
	}
}

//UUIDが設定されていなければ設定しなおします
void ScriptRemoconWeb::SettingUUIDOverriade()
{
	const string uuid = MainWindow::m()->Config.Get("httpd__uuid", "" );
	if (!uuid.empty())
	{
		if( SystemMisc::checkUUID(uuid) )
		{
			DEBUGLOG("UUIDは既にあります UUID:"<< uuid );
			return ;
		}

		ERRORLOG("UUIDはすでにありますが、正しくありません UUID:" << uuid );
		MainWindow::m()->Config.Set("httpd__uuid", "" );
	}

	string newUUID = SystemMisc::makeUUID();
	if (newUUID.empty())
	{
		ERRORLOG("UUIDを作成ができません " );
		return ;
	}
	if( ! SystemMisc::checkUUID(newUUID) )
	{
		ERRORLOG("生成したUUIDが正しくありません" );
		return ;
	}
	
	MainWindow::m()->Config.Set("httpd__uuid", newUUID );
	NOTIFYLOG("UUIDを新規作成しました NEW UUID:"<<MainWindow::m()->Config.Get("httpd__uuid", "") );
}

//webapiキーが設定されていなければ設定しなおします
void ScriptRemoconWeb::SettingWebAPIKeyOverriade()
{
	const string webapi_apikey = MainWindow::m()->Config.Get("webapi_apikey", "" );
	if (webapi_apikey.empty()) MainWindow::m()->Config.Set("webapi_apikey", SystemMisc::MakeWebAPIKey());
}

bool ScriptRemoconWeb::checkCSRFToken(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result,string* responsString)
{
	auto post = httpHeaders.getPostPointer();
	const string _csrftoken1 = mapfind(post,"_csrftoken");
	const string _csrftoken2 = GetCSRFToken(httpHeaders);
	if (_csrftoken1 != _csrftoken2)
	{
		*result = WEBSERVER_RESULT_TYPE_FORBIDDEN;
		*responsString = ScriptRemoconWeb::ResultError(9000,"変更が発生するリクエストには、CSRFトークンが必要です。");
		return false;
	}
	return true;

	//リファラーとHostを確認します。

	//内部からのリファラーを持つアクセスだったら許可する。
	//理由:ブラウザからのXSSを防止するのが目的なのでこれで大丈夫だと思われる
	const auto header = httpHeaders.getHeader();
	auto refererIT = header.find("referer");
	if (refererIT != header.end())
	{
		auto hostIT = header.find("host");
		if (hostIT != header.end())
		{
			if ( XLStringUtil::strpos(refererIT->second.c_str() , "http://" + hostIT->second + "/" ) == 0 )
			{//先頭にマッチする必要がある
				return true;
			}
			if ( XLStringUtil::strpos(refererIT->second.c_str() , "https://" + hostIT->second + "/" ) == 0 )
			{//先頭にマッチする必要がある
				return true;
			}
		}
	}

	//XSSいたずら防止のためにリファラーを求める.
	*result = WEBSERVER_RESULT_TYPE_FORBIDDEN;
	*responsString = ScriptRemoconWeb::ResultError(9000,"変更が発生するリクエストには、FutureHomeControllerからのリファラーが必要です。");
	return false;
}

string ScriptRemoconWeb::getElecStatus(const string& elecname) 
{
	const auto configmap = MainWindow::m()->Config.FindGetsToMap("elec_",false);
	int key1 = 0;
	if ( ! ScriptRemoconWeb::type2key(configmap ,elecname , &key1) )
	{
		return "";
	}

	return MainWindow::m()->Config.Get("elec_" + num2str(key1) + "_status","");
}


bool ScriptRemoconWeb::type2key(const map<string,string> &configmap ,const string& type1,const string& type2 , int* outkey1, int* outkey2) 
{
	int key1 = -1;
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		if (strstr(it->first.c_str(),"_type") == NULL )
		{
			continue;
		}
		if ( strstr(it->first.c_str(),"_action_")  )
		{
			continue;
		}

		if ( it->second == type1 )
		{
			sscanf(it->first.c_str(),"elec_%d_type",&key1);
			break;
		}
	}
	if (key1 <= 0)
	{
		return false;
	}

	int key2 = -1;
	const string prefix = "elec_" + num2str(key1) + "_action_";
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		if ( strstr( it->first.c_str() , "_actiontype" ) == NULL  )
		{
			continue;
		}
		if ( strstr( it->first.c_str() , prefix.c_str() ) == NULL )
		{
			continue;
		}
		if ( it->second == type2 )
		{
			sscanf(it->first.c_str(),"elec_%d_action_%d_type",&key1,&key2);
			break;
		}
	}
	if (key1 <= 0 || key2 <= 0 )
	{
		return false;
	}
	*outkey1=key1;
	*outkey2=key2;

	return true;
}

bool ScriptRemoconWeb::type2key(const map<string,string> &configmap ,const string& type1 , int* outkey1)  
{
	int key1 = -1;
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		if (strstr(it->first.c_str(),"_type") == NULL )
		{
			continue;
		}
		if ( strstr(it->first.c_str(),"_action_")  )
		{
			continue;
		}

		if ( it->second == type1 )
		{
			sscanf(it->first.c_str(),"elec_%d_type",&key1);
			break;
		}
	}
	if (key1 <= 0)
	{
		return false;
	}

	*outkey1=key1;

	return true;
}

//キー値の最大値を求める.
int ScriptRemoconWeb::GetMaxID(const map<string,string> &configmap,const string& strip)
{
	int max = 0;
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		const string head = it->first.substr(0,strip.size());
		if ( head != strip )
		{
			continue;
		}
		const auto ii = atoi( it->first.substr(strip.size()) );

		if (max < ii ) max = ii;
	}
	return max;
}

//特定の値をjsonでまるっと.
string ScriptRemoconWeb::ResultJsonStatus(const string& prefix)
{
	

	string jsonstring;
	const auto cmap = MainWindow::m()->Config.FindGetsToMap(prefix,false);
	for(auto it = cmap.begin() ; it != cmap.end() ; ++it )
	{
		jsonstring += string(",") + XLStringUtil::jsonescape(it->first) + ": " 
			+ _A2U(XLStringUtil::jsonescape(it->second)) ;
	}

	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return jsonstring;
}

//言語を切り替えたときにするべき設定
void ScriptRemoconWeb::ChangeLangSetting(const string & system_lang)
{
	//合成音声エンジンと音声認識エンジンを再構築する
	if (system_lang == "japanese")
	{
		MainWindow::m()->Config.Set( "speak_type" , "openjtalk_mei_normal" );
		MainWindow::m()->Config.Set( "recong__yobikake__1" , "コンピューター" );
		MainWindow::m()->Config.Set( "recong__yobikake__2" , "コンピュータ" );
		MainWindow::m()->Config.Set( "recong__yobikake__3" , "" );
		MainWindow::m()->Config.Set( "recong__yobikake__4" , "" );
		MainWindow::m()->Config.Set( "recong__yobikake__5" , "" );
		MainWindow::m()->Config.Set( "remoconsetting_footermenu_remoconedit_string" , "リモコンを編集" );
		MainWindow::m()->Config.Set( "remoconsetting_footermenu_setting_string" , "設定画面に行く" );
		MainWindow::m()->Config.Set( "system_room_string" , "リモコン" );
		MainWindow::m()->Config.Set( "remoconsetting_sensor_temp_string" , "温度" );
		MainWindow::m()->Config.Set( "remoconsetting_sensor_lumi_string" , "明るさ" );
		MainWindow::m()->Config.Set( "remoconsetting_dialog_close_string" , "close" );
	}
	else if (system_lang == "english")
	{
		MainWindow::m()->Config.Set( "speak_type" , "festival_cmu_us_slt" );
		MainWindow::m()->Config.Set( "recong__yobikake__1" , "computer" );
		MainWindow::m()->Config.Set( "recong__yobikake__2" , "" );
		MainWindow::m()->Config.Set( "recong__yobikake__3" , "" );
		MainWindow::m()->Config.Set( "recong__yobikake__4" , "" );
		MainWindow::m()->Config.Set( "recong__yobikake__5" , "" );
		MainWindow::m()->Config.Set( "remoconsetting_footermenu_remoconedit_string" , "Remote control Edit" );
		MainWindow::m()->Config.Set( "remoconsetting_footermenu_setting_string" , "Manage Menu" );
		MainWindow::m()->Config.Set( "system_room_string" , "Remote control" );
		MainWindow::m()->Config.Set( "remoconsetting_sensor_temp_string" , "Temperature" );
		MainWindow::m()->Config.Set( "remoconsetting_sensor_lumi_string" , "Brightness" );
		MainWindow::m()->Config.Set( "remoconsetting_dialog_close_string" , "Close" );
	}
}


void ScriptRemoconWeb::DeflagConfig(const string& prefix,const string& needkey)
{
	const auto configmap = MainWindow::m()->Config.FindGetsToMap(prefix,false);
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		const int key = atoi( it->first.c_str() + prefix.size() );
		if (key <= 0)
		{
			DEBUGLOG("不明なゴミデータ key:" << it->first << " // value:" << it->second << "があります。" );
			continue;
		}

		//必須キーがあるかどうかチェックする.
		//毎回消していいかチェックするのは重いが、どうせ初回しかやらないし、、気にしないことにする。
		const string needkeyString = prefix + num2str(key) + "_" + needkey;
		if (configmap.find(needkeyString) != configmap.end() )
		{//キーがあるのでこのデータは消してはいけない
		}
		else
		{
			//消します。
			NOTIFYLOG("データ key:" << it->first << " // value:" << it->second << "は、断片データなので消去します。" );
			MainWindow::m()->Config.Remove(it->first);
		}
	}
}

void ScriptRemoconWeb::DeflagConfig(const string& prefix,const string& needkey,const string& prefix2,const string& needkey2)
{
	const auto configmap = MainWindow::m()->Config.FindGetsToMap(prefix,false);
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		const int key = atoi( it->first.c_str() + prefix.size() );
		if (key <= 0)
		{
			DEBUGLOG("不明なゴミデータ key:" << it->first << " // value:" << it->second << "があります。" );
			continue;
		}

		//必須キーがあるかどうかチェックする.
		//毎回消していいかチェックするのは重いが、どうせ初回しかやらないし、、気にしないことにする。
		const string needkeyString = prefix + num2str(key) + "_" + needkey;
		if (configmap.find(needkeyString) != configmap.end() )
		{//キーがあるのでこのデータは消してはいけないかも

			//2番目のキーはどうだろう？
			const char * action = it->first.c_str();
			const char * found = strstr(action,prefix2.c_str());
			if ( found == NULL )
			{
				continue;
			}
			const int key2 = atoi( found + prefix2.size()  );
			if (key2 <= 0)
			{
				DEBUGLOG("不明なゴミデータ key:" << it->first << " // value:" << it->second << "があります。" );
				continue;
			}
			const string needkeyString2 = prefix + num2str(key) + "_" + prefix2 + num2str(key2) + "_" + needkey2;
			if (configmap.find(needkeyString2) != configmap.end() )
			{//やはり消してはいけない
			}
			else
			{
				//消します。
				NOTIFYLOG("データ key:" << it->first << " // value:" << it->second << "は、断片データなので消去します。" );
				MainWindow::m()->Config.Remove(it->first);
			}
		}
		else
		{
			//消します。
			NOTIFYLOG("データ key:" << it->first << " // value:" << it->second << "は、断片データなので消去します。" );
			MainWindow::m()->Config.Remove(it->first);
		}
	}
}

int ScriptRemoconWeb::NewElecID()
{
	int max = 1;
	const auto configmap = MainWindow::m()->Config.FindGetsToMap("elec_",false);
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		const int key = atoi( it->first.c_str() + (sizeof("elec_") - 1) );
		if (key > max)
		{
			max = key;
		}
	}
	return max + 1;
}

int ScriptRemoconWeb::NewElecActionID(int eleckey)
{
	int max = 1;
	const string prefix = string("elec_") + num2str(eleckey) + "_action_";
	const auto configmap = MainWindow::m()->Config.FindGetsToMap(prefix,false);
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		const int key = atoi( it->first.c_str() + prefix.size() );
		if (key > max)
		{
			max = key;
		}
	}
	return max + 1;
}

//機材の種類が重複していたらエラー
bool ScriptRemoconWeb::checkUniqElecType(int key1,const string& type) 
{
	//余白ならば問答無用で重複可
	if (type == "SPACE") return true;

	const string prefix = string("elec_") ;
	const auto configmap = MainWindow::m()->Config.FindGetsToMap(prefix,false);

	//その他があるのでめんどい所
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		const int targetkey = atoi( it->first.c_str() + prefix.size() );
		if (targetkey == key1)
		{//自分自身と重複している分にはいいわけで
			continue;
		}
		if (!strstr(it->first.c_str(),"_type")  )
		{
			continue;
		}
		if (strstr(it->first.c_str(),"_action_")  )
		{
			continue;
		}

		if (it->second == type)
		{//既に使われている!
			return false;
		}
	}
	return true;
}

//機材操作の種類が重複していたらエラー
bool ScriptRemoconWeb::checkUniqElecActionType(int key1,int key2,const string& actiontype) 
{
	//余白ならば問答無用で重複可
	if (actiontype == "SPACE") return true;

	const string prefix = string("elec_") + num2str(key1) + "_action_";
	const auto configmap = MainWindow::m()->Config.FindGetsToMap(prefix,false);

	//その他があるのでめんどい所
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		const int targetkey = atoi( it->first.c_str() + prefix.size() );
		if (targetkey == key2)
		{//自分自身と重複している分にはいいわけで
			continue;
		}
		if (!strstr(it->first.c_str(),"_actiontype")  )
		{
			continue;
		}

		if (it->second == actiontype)
		{//既に使われている!
			return false;
		}
	}
	return true;
}

//新しい機材の並び順を求める(末尾に追加する)
int ScriptRemoconWeb::newOrderElec() 
{
	int max = 0;
	const string prefix = string("elec_") ;
	const auto configmap = MainWindow::m()->Config.FindGetsToMap(prefix,false);
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		if (!strstr(it->first.c_str(),"_order")  )
		{
			continue;
		}
		if (strstr(it->first.c_str(),"_action_")  )
		{
			continue;
		}
		const int order = atoi(it->second.c_str());
		if (max < order ) max = order;
	}
	return max+1;
}

//新しい機材操作の並び順を求める(末尾に追加する)
int ScriptRemoconWeb::newOrderElecAction(int key1) 
{
	int max = 0;
	const string prefix = string("elec_") + num2str(key1) + "_action_";
	const auto configmap = MainWindow::m()->Config.FindGetsToMap(prefix,false);
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		if (!strstr(it->first.c_str(),"_order"))
		{
			continue;
		}
		const int order = atoi(it->second.c_str());
		if (max < order ) max = order;
	}
	return max+1;
}
string ScriptRemoconWeb::sensorMaptoJson(const map<time_t,short>& map)
{
	string j;
	for(auto it = map.begin() ; it != map.end() ; ++ it )
	{
		j += ",\"" + num2str(it->first) + "\": " + num2str(it->second);
	}
	return "{" + j.substr(1) + "}";
}

string ScriptRemoconWeb::sensorMaptoJsonScale(const map<time_t,short>& map,double scale)
{
	string j;
	for(auto it = map.begin() ; it != map.end() ; ++ it )
	{
		j += ",\"" + num2str(it->first) + "\": " + num2str(it->second / scale);
	}
	return "{" + j.substr(1) + "}";
}

//認証
bool ScriptRemoconWeb::LocalAuth(const string& id,const string& password)
{
	

	if ( id.empty() )
	{
		return false;
	}
	//パスワードは sha1して保持しているので、比較するときは sha1 で比較するよ
	const string password_sha1 =  SystemMisc::MakePassword( id , password);

	//現在設定されている内容と比較
	const string account_usermail = MainWindow::m()->Config.Get("account_usermail","");
	const string account_password = MainWindow::m()->Config.Get("account_password","");

	if (account_usermail.empty() || account_password.empty())
	{//そもそも登録されていないっす
		NOTIFYLOG("このID(" << account_usermail << ")はそもそも登録されていません");
		return false;
	}
	if (account_usermail == id && account_password == password_sha1 )
	{//OK
		NOTIFYLOG("このID(" << account_usermail << ")の認証OKです");
		return true;
	}
	return false;
}

WEBSERVER_RESULT_TYPE ScriptRemoconWeb::SelectJsonPResult(const map<string,string>& request)
{
	if ( mapfind(request,"callback").empty() )
	{
		return WEBSERVER_RESULT_TYPE_OK_JSON;
	}
	else
	{
		return WEBSERVER_RESULT_TYPE_OK_JSONP;
	}
}

//リモコンの副次的情報の取得
string ScriptRemoconWeb::remocon_get_append_info(const map<string,string>& configmap,time_t now)
{
	
	const vector<unsigned int> arr = ScriptRemoconWeb::getElecID_Array(configmap,false);

	string inner_jsonstring;
	for(auto it = arr.begin() ; it != arr.end() ; ++it )
	{
		int key1 = *it;
		const string prefix = "elec_" + num2str(key1);
		const string status = mapfind(configmap,prefix + "_status");

		inner_jsonstring += string(",{");
		inner_jsonstring += XLStringUtil::jsonescape("key") + ": " 
			+ num2str(key1);
		inner_jsonstring += string(",") + XLStringUtil::jsonescape("status") + ": " 
			+ XLStringUtil::jsonescape( status ) ;

		const string type = mapfind(configmap,prefix + "_type");
		const time_t firetime = atou(mapfind(configmap,prefix + "_timer_firetime") ) ;
		const int firekey = atoi( mapfind(configmap,prefix + "_timer_firekey") );
		if ( firetime > 0 && firekey > 0)
		{
			const string prefixAction = prefix + "_action_" + num2str(firekey) ;
			const string timer_exec_elec = mapfind(configmap,prefixAction + "_timer_exec_elec");
			const string timer_exec_action = mapfind(configmap,prefixAction + "_timer_exec_action");

			int fireMinute = (int) (firetime - (time(NULL)/ 60)) ;
			if (fireMinute < 1) fireMinute=1;

			inner_jsonstring += string(",") + XLStringUtil::jsonescape("timer_minute") + ": " 
				+ num2str( fireMinute ) ;
			inner_jsonstring += string(",") + XLStringUtil::jsonescape("timer_elec") + ": " 
				+ XLStringUtil::jsonescape( timer_exec_elec ) ;
			inner_jsonstring += string(",") + XLStringUtil::jsonescape("timer_action") + ": " 
				+ XLStringUtil::jsonescape( timer_exec_action ) ;
		}
		inner_jsonstring += "}" ;
	}

	if (! inner_jsonstring.empty() )
	{
		inner_jsonstring = inner_jsonstring.substr(1);
	}
	string jsonstring = string(",") + XLStringUtil::jsonescape("elec") + ": [" + inner_jsonstring + "]";

	const string sensor_temp_value = mapfind(configmap,"sensor_temp_value");
	const string sensor_lumi_value = mapfind(configmap,"sensor_lumi_value");
	jsonstring += string(",") + XLStringUtil::jsonescape("sensor_temp") + ": "
		+ XLStringUtil::jsonescape( sensor_temp_value ) ;
	jsonstring += string(",") + XLStringUtil::jsonescape("sensor_lumi") + ": "
		+ XLStringUtil::jsonescape( sensor_lumi_value ) ;

	const string sipevent_incoming = mapfind(configmap,"sipevent_incoming");
	if ( ! sipevent_incoming.empty() )
	{//電話がかかってきた
		jsonstring += string(",") + XLStringUtil::jsonescape("sipevent_incoming") + ": "
			+ XLStringUtil::jsonescape( sipevent_incoming ) ;
	}

	const string sipevent_call = mapfind(configmap,"sipevent_call");
	if ( ! sipevent_call.empty() )
	{//呼び出し中
		jsonstring += string(",") + XLStringUtil::jsonescape("sipevent_call") + ": "
			+ XLStringUtil::jsonescape( sipevent_call ) ;
	}

	const time_t sipevent_numpad = atoi(mapfind(configmap,"sipevent_numpad"));
	if (now < sipevent_numpad)
	{//電話のキーパッドを出す
		jsonstring += string(",") + XLStringUtil::jsonescape("sipevent_numpad") + ": 1" ;
	}

	jsonstring = string("{ \"result\": \"ok\" ") + jsonstring + "}";
	return _A2U(jsonstring.c_str());
}
