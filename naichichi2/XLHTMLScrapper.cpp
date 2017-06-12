#include "common.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"
#include "XLHTMLScrapper.h"

//今いるタグの中を取得する.
string XLHTMLScrapper::InnerHTML(const string& html)
{
	const char * s = html.c_str();
	const char * e = s + html.size();

	//開始タグ終端まで飛ばす
	while(s < e)
	{
		if (*s == '<') break;
		s++;
	}

	while(s < e)
	{
		if (*s == '>') break;
		else if (*s == '"')
		{
			while(s < e)
			{
				if (*s == '\\' && *(s+1) == '"')
				{
					s++;
				}
				else if (*s == '"')
				{
					break;
				}
				s++;
			}
		}
		else if (*s == '\'')
		{
			while(s < e)
			{
				if (*s == '\\' && *(s+1) == '\'')
				{
					s++;
				}
				else if (*s == '\'')
				{
					break;
				}
				s++;
			}
		}
		s++;
	}
	if (*s == '>')
	{
		s++ ;
	}

	//次は閉じタグを削る
	while(s < e)
	{
		if (*e == '<') break;
		e--;
	}

	return XLStringUtil::chop(string(s , 0 , e - s ));
}

SEXYTEST()
{
	{
		string a = 
						"<html>\r\n" 
							"<body>\r\n"
								"hoge\r\n" 
							"</body>\r\n"
						"</html>\r\n"
						;
		string b = 
						"<body>\r\n"
							"hoge\r\n" 
						"</body>"
						;
		string c = XLHTMLScrapper::InnerHTML(a);
		SEXYTEST_EQ(c ,b); 
	}
}

//今いるタグの中を取得する.
string XLHTMLScrapper::InnerText(const string& html)
{
	const char * s = html.c_str();
	const char * e = s + html.size();

	string textNode;
	const char* textStart = NULL;
	while(s < e)
	{
		if (*s != '<')
		{
			s++;
			continue;
		}
		if (textStart && s != textStart)
		{
			textNode+=string(textStart,0,s - textStart);
			textStart = NULL;
		}

		while(s < e)
		{
			if (*s == '>') break;
			else if (*s == '"')
			{
				while(s < e)
				{
					if (*s == '\\' && *(s+1) == '"')
					{
						s++;
					}
					else if (*s == '"')
					{
						break;
					}
					s++;
				}
			}
			else if (*s == '\'')
			{
				while(s < e)
				{
					if (*s == '\\' && *(s+1) == '\'')
					{
						s++;
					}
					else if (*s == '\'')
					{
						break;
					}
					s++;
				}
			}
			s++;
		}
		textNode = s;
	}

	if (textStart && s != textStart)
	{
		textNode+=string(textStart,0,s - textStart);
	}

	return XLStringUtil::chop(textNode);
}

//属性を取得する
string XLHTMLScrapper::HTMLAttr(const string& html,const string& attributeName)
{
	const char * s = html.c_str();
	const char * e = s + html.size();

	while(s < e)
	{
		if (*s == '<') break;
		s++;
	}
	if (*s != '<')
	{
		return "";
	}
	NamePicker(s+1,e,&s);

	while(s < e)
	{
		if (*s == ' ' || *s == '\t' || *s == '/')
		{
			s++;
			continue;
		}
		if (*s == '>')
		{//終了タグ
			break;
		}

		string name = NamePicker(s,e,&s);
		string value;
		if ( *s == '=' )
		{
			s++;
			value = ValuePicker(s,e,&s);
		}
		if (name == attributeName)
		{
			return value;
		}
		s++;
	}
	return "";
}
SEXYTEST()
{
	{
		string a = 
						"<html>\r\n" 
							"<body>\r\n"
								"hoge\r\n" 
							"</body>\r\n"
						"</html>\r\n"
						;
		string c = XLHTMLScrapper::HTMLAttr(a,"href");
		SEXYTEST_EQ(c ,""); 
	}
	{
		string a = 
						"<html href='123' hoga='456'>\r\n" 
							"<body>\r\n"
								"hoge\r\n" 
							"</body>\r\n"
						"</html>\r\n"
						;
		string c = XLHTMLScrapper::HTMLAttr(a,"hoga");
		SEXYTEST_EQ(c ,"456"); 
	}
	{
		string a = 
						"<html href='123'>\r\n" 
							"<body>\r\n"
								"hoge\r\n" 
							"</body>\r\n"
						"</html>\r\n"
						;
		string c = XLHTMLScrapper::HTMLAttr(a,"data");
		SEXYTEST_EQ(c ,""); 
	}
	{
		string a = 
						"<html href='123'>\r\n" 
							"<body>\r\n"
								"hoge\r\n" 
							"</body>\r\n"
						"</html>\r\n"
						;
		string c = XLHTMLScrapper::HTMLAttr(a,"href");
		SEXYTEST_EQ(c ,"123"); 
	}
}


string XLHTMLScrapper::NamePicker(const char* s,const char* e,const char** next)
{
	const char* start = s;
	for( ;s < e ; ++s )
	{
		if (*s == ' ' || *s == '\t' || *s == '>' || *s == '=' || *s == '[' || *s == ']' || *s == '/') 
		{
			break;
		}
	}
	if (start == s)
	{
		if (next != NULL)	*next = s + 1;
		return "";
	}

	if (next != NULL)	*next = s;
	return string(start , 0 , s - start);
}

string XLHTMLScrapper::ValuePicker(const char* s,const char* e,const char** next)
{
	const char* start = s;
	for( ;s < e ; ++s )
	{
		if (! (*s == ' ' || *s == '\t') ) break;
	}
			
	if (*s == '"' )
	{
		s++;
		start = s;
		for( ;s < e ; ++s )
		{
			if (*s == '\\' && *(s+1) == '"' )
			{
				s++;
			}
			if (*s == '"' )
			{
				break;
			}
		}
		*next = s + 1;
	}
	else if (*s == '\'' )
	{
		s++;
		start = s;
		for( ;s < e ; ++s )
		{
			if (*s == '\\' && *(s+1) == '\'' )
			{
				s++;
			}
			if (*s == '\'' )
			{
				break;
			}
		}
		*next = s + 1;
	}
	else
	{
		start = s;
		for( ;s < e ; ++s )
		{
			if (*s == ' ' || *s == '\t' || *s == '>' || *s == ']')
			{
				break;
			}
		}
		*next = s;
	}

	return string(start , 0 , s - start);
}

void XLHTMLScrapper::Cleanup(vector<tagSt*>* tags)
{
	for(auto it = tags->begin() ; it != tags->end() ; ++it)
	{
		tagSt* tagS = *it;
		for(auto ita = tagS->attributes.begin() ; ita != tagS->attributes.end() ; ++ita)
		{
			delete *ita;
		}
		delete tagS;
	}
}

const char* XLHTMLScrapper::skipTagStart(const char* s, const char* e)
{
	while(s < e)
	{
		if (*s == '<') break;
		s++;
	}
	return s;
}


bool XLHTMLScrapper::tryCommentSkip(const char* s, const char* e,const char** outS)
{
	if (*s == '<') s++;
	//コメント？
	if (*(s) == '!' && *(s+1) == '-' && *(s+2) == '-')
	{
		//コメント終端まで飛ばす
		for(s=s+3 ; s < e ; ++s )
		{
			if (*s == '-' && *(s+1) == '-' && *(s+2) == '>')
			{
				s+=3;
				break;
			}
		}
		*outS = s;
		return true;
	}
	return false;
}



bool XLHTMLScrapper::checkTextAttribute(unsigned int posstionCount ,const vector<attributeSt*>* checkAttributes )
{
	for(auto it = checkAttributes->begin() ; it != checkAttributes->end() ; ++ it)
	{
		attributeSt* attribute = *it;

		bool found = false;
		if ( attribute->expr == expr_posstion )
		{//個数 N番目のタグ
			if ( posstionCount == atoi(attribute->value) )
			{
				found = true;
				break;
			}
		}
		if (!found)
		{//マッチせず
			return false;
		}
	}

	//マッチ
	return true;
}

bool XLHTMLScrapper::checkExprPosstionAttriute(const tagSt* tag )
{
	const vector<attributeSt*>* checkAttributes = &tag->attributes;
	for(auto it = checkAttributes->begin() ; it != checkAttributes->end() ; ++ it)
	{
		attributeSt* attribute = *it;

		if ( attribute->expr == expr_posstion )
		{//個数 N番目のタグ
			return true;
		}
	}

	//マッチしない。
	return false;
}


bool XLHTMLScrapper::checkEmptyNode(const char* s,const char* e)
{
	while(s < e)
	{
		if (!(*s == ' ' || *s == '\r' || *s == '\n'))
		{
			return false;
		}
		s++;
	}
	return true;
}

//expr_at で指定されてる値を取得する。 指定されていなければ 0 になる
unsigned int XLHTMLScrapper::getExprAt(const tagSt* tag)
{
	for ( auto it = tag->attributes.begin(); it != tag->attributes.end() ; ++it)
	{
		if ((*it)->expr == expr_at)
		{
			return atoi( (*it)->value );
		}
	}
	//expr_at は指定されていなかった.
	return 0;
}


string XLHTMLScrapper::HtmlFinderWithExprPosstion(const char* parentTagS,const char* s, const char* e,const char** next,tagSt* tag)
{
	unsigned int posstionCount = 0;

	while(s < e)
	{
		if (*s != '<')
		{//タグやコメントは < から始まるので、それ以外は無視
			if ("text()" != tag->name)
			{//テキストノードを探していないならば無視
				s = skipTagStart(s,e);
				continue;
			}
			const char *valueStart = s;
			s = skipTagStart(s,e);
			if ( checkEmptyNode(valueStart,s) )
			{//空ノードなので無視する
				continue;
			}
			//OK.こいつをいただこう
			*next = JmpCloseTag(parentTagS,e);

			//outerHTML的に取得します.
			return string(valueStart , 0 , s - valueStart);
		}

		//コメント？
		if (tryCommentSkip(s,e,&s))
		{
			continue;
		}
		const char* tagStart = s;

		//タグらしい
		if (*(s+1) == '/')
		{//終了タグなので無視
			s = JmpEndTag(s+2,e);
			continue;
		}

		//タグの名前
		const string tagname = XLHTMLScrapper::NamePicker(s+1,e,NULL);
		if (tag->name.empty() || STRICMP(tag->name.c_str(),tagname.c_str() ) == 0 )
		{//名前は、探していたタグっぽい
			posstionCount++;
			if ( checkTagAttribute(s , e , posstionCount , &(tag->attributes) ) )
			{//属性も探していたタグっぽい
				//OK.こいつをいただこう
				const char* tagCloseS = JmpCloseTag(s,e);
				*next = JmpCloseTag(parentTagS,e);

				//outerHTML的に取得します.
				return string(tagStart , 0 , tagCloseS - tagStart);
			}
			//違うっぽい
			if (tag->childOnly)
			{
				//この開始タグの終端までスキップする
				s = JmpCloseTag(s,e);
			}
			else
			{
				if (tag->name.empty())
				{
					//この開始タグの終端までスキップする
					const char* tagCloseS = JmpCloseTag(s,e);
					s  = JmpEndTag(s,tagCloseS);

					const string outerHTML = HtmlFinderWithExprPosstion(s,tagStart,tagCloseS,next,tag);
					if (! outerHTML.empty() )
					{
						return outerHTML;
					}
					s = tagCloseS;
				}
				else
				{
					//このタグに対応する閉じタグの先までスキップする
					s = JmpCloseTag(s,e);
				}
			}

		}
		else
		{
			//違うっぽい
			if (tag->childOnly)
			{//このタグに対応する閉じタグの先までスキップする
				s = JmpCloseTag(s,e);
			}
			else
			{//この開始タグの終端までスキップする
				const char* tagCloseS = JmpCloseTag(s,e);
				s  = JmpEndTag(s,tagCloseS);

				const string outerHTML = HtmlFinderWithExprPosstion(tagStart , s,tagCloseS,next,tag);
				if (! outerHTML.empty() )
				{
					return outerHTML;
				}
				s = tagCloseS;
			}
		}
	}
	*next = e;
	return "";
}

string XLHTMLScrapper::HtmlFinderWithoutExprPosstion(const char* s, const char* e,const char** next,tagSt* tag)
{
	while(s < e)
	{
		if (*s != '<')
		{//タグやコメントは < から始まるので、それ以外は無視
			if ("text()" != tag->name)
			{//テキストノードを探していないならば無視
				s = skipTagStart(s,e);
				continue;
			}
			const char *valueStart = s;
			s = skipTagStart(s,e);
			if ( checkEmptyNode(valueStart,s) )
			{//空ノードなので無視する
				continue;
			}
			//OK.こいつをいただこう
			*next = s;

			//outerHTML的に取得します.
			return string(valueStart , 0 , s - valueStart);
		}

		//コメント？
		if (tryCommentSkip(s,e,&s))
		{
			continue;
		}
		const char* tagStart = s;

		//タグらしい
		if (*(s+1) == '/')
		{//終了タグなので無視
			s = JmpEndTag(s+2,e);
			continue;
		}

		//タグの名前
		const string tagname = XLHTMLScrapper::NamePicker(s+1,e,NULL);

		if (tag->name.empty() || STRICMP(tag->name.c_str(),tagname.c_str() ) == 0 )
		{//名前は、探していたタグっぽい
			if ( checkTagAttribute(s , e , 0 , &(tag->attributes) ) )
			{//属性も探していたタグっぽい
				//OK.こいつをいただこう
				const char* tagCloseS = JmpCloseTag(s,e);
				*next = tagCloseS;

				//outerHTML的に取得します.
				return string(tagStart , 0 , tagCloseS - tagStart);
			}
			//違うっぽい
			if (tag->childOnly)
			{
				//この開始タグの終端までスキップする
				s = JmpCloseTag(s,e);
			}
			else
			{
				if (tag->name.empty())
				{
					//この開始タグの終端までスキップする
					s = JmpEndTag(s,e);
				}
				else
				{
					//このタグに対応する閉じタグの先までスキップする
					s = JmpCloseTag(s,e);
				}
			}
		}
		else
		{
			//違うっぽい
			if (tag->childOnly)
			{//このタグに対応する閉じタグの先までスキップする
				s = JmpCloseTag(s,e);
			}
			else
			{//この開始タグの終端までスキップする
				s = JmpEndTag(s,e);
			}
		}
	}

	*next = e;
	return "";
}

bool XLHTMLScrapper::checkTagAttribute(const char* s,const char* e,unsigned int posstionCount,const vector<attributeSt*>* checkAttributes)
{
	if (checkAttributes->empty())
	{//チェックするべき属性がないので、即成立
		return true;
	}

	//一つしか属性が指定されていない場合は、 //p[1] とか //p[$1] の可能性があるので、優先して調べます。
	if (checkAttributes->size() == 1)
	{
		auto attributeIT = checkAttributes->begin();
		auto attribute = *attributeIT;

		if ( attribute->expr == expr_posstion )
		{//個数 N番目のタグ
			if ( posstionCount == atoi(attribute->value) )
			{
				return true;
			}
		}
		else if ( attribute->expr == expr_at )
		{//expr_at しか指定されていない場合は、即成立.
			return true;
		}
	}


	//htmlの属性をパースしてmapに詰めます。
	vector<pair<string,string> > attributeMap;
	while(s < e)
	{
		if (*s == ' ' || *s == '\t' || *s == '/')
		{
			s++;
			continue;
		}
		if (*s == '>')
		{//タグおわり
			break;
		}

		string name = XLHTMLScrapper::NamePicker(s,e,&s);
		string value;
		if ( *s == '=' )
		{
			s++;
			value = XLHTMLScrapper::ValuePicker(s,e,&s);
		}

		if (name.empty() && value.empty() )
		{//何も値がないとチェック対象が前に進まないので進めておく
			s++;
		}
		else
		{
			attributeMap.push_back(pair<string,string>(name,value));
		}
	}

	//属性をチェックしていきます。
	for(auto it = checkAttributes->begin() ; it != checkAttributes->end() ; ++ it)
	{
		attributeSt* attribute = *it;
		if ( attribute->expr == expr_posstion )
		{//個数 N番目のタグ
			if ( posstionCount != atoi(attribute->value) )
			{//マッチせず
				return false;
			}
		}
		else if ( attribute->expr == expr_at )
		{//無視.
		}
		else
		{//属性で検索
			bool found = false;
			for(auto itm = attributeMap.begin() ; itm != attributeMap.end() ; ++ itm)
			{
				if ( STRICMP( itm->first.c_str() , attribute->name.c_str() ) == 0)
				{
					if ( checkAttributeExpr(attribute,itm->second) )
					{
						found = true;
						break;
					}
				}
			}
			if (!found)
			{//マッチせず
				return false;
			}
		}
	}

	//マッチ
	return true;
}

bool XLHTMLScrapper::checkAttributeExpr(const attributeSt* attribute,string& value )
{
	switch(attribute->expr)
	{
	case expr_exist:
		return true;
	case expr_eq:
		return attribute->value == value;
	case expr_noteq:
		return attribute->value != value;
//			case expr_first:
//				return XLHTMLScrapper:isfirst(attribute->value , value);
//			case expr_end:
//				return XLHTMLScrapper:isend(attribute->value , value);
//			case expr_end:
//				return XLHTMLScrapper:ismid(attribute->value , value);
	}
	return false;
}

//このタグの閉じタグ終了の位置まで移動する
const char* XLHTMLScrapper::JmpCloseTag(const char* s,const char* e)
{
	//まずタグ開始までスキップします.
	while(s < e)
	{
		if (*s == '<') break;
		s++;
	}
	if (s >= e) return e;
	s++; //skip '<'

	if (*s == '/')
	{//このタグは閉じるタグです。
		return JmpEndTag(s+1,e);
	}
	//タグの名前を取得します.
	const string name = XLHTMLScrapper::NamePicker(s,e,&s);

	//では、今のタグを終わらせる
	s = JmpEndTag(s,e);

	//ネストしないタグは現在のタグ終端でおしまい.
	if (    STRICMP(name.c_str(), "br") == 0    
		 || STRICMP(name.c_str(), "img") == 0   
		 || STRICMP(name.c_str(), "hr") == 0   
		 || STRICMP(name.c_str(), "meta") == 0  
		 || STRICMP(name.c_str(), "link") == 0 
	   )
	{
		return s;
	}
	
	//こいつに対応する閉じタグを探す.
	unsigned int nestCount = 1;
	while(s < e)
	{
		if (*s != '<')
		{//タグやコメントは < から始まるので、それ以外は無視
			s++;
			continue;
		}

		//コメント？
		if (tryCommentSkip(s,e,&s))
		{
			continue;
		}
		const char* tagStart = s;
		s++;

		//タグらしい
		bool closeTag = false;
		if (*s == '/')
		{
			closeTag = true;
			s++;
		}

		//タグの名前
		string tagname = XLHTMLScrapper::NamePicker(s,e,&s);
		s = JmpEndTag(s,e);
		if (STRICMP( tagname.c_str(),name.c_str()) != 0)
		{
			continue;
		}
		
		//探していたタグの閉じタグか？
		if (closeTag)
		{
			nestCount --;
			if (nestCount <= 0)
			{//OK.探していたものに対応する閉じタグだ
				return s;
			}
		}
		else
		{//入れ子になっているらしい
			nestCount ++;
			continue;
		}
	}

	return s;
}

//この開始タグの終端までスキップする
const char* XLHTMLScrapper::JmpEndTag(const char* s,const char* e)
{
	while(s < e)
	{
		if (*s == ' ' || *s == '\t' || *s == '/')
		{
			s++;
			continue;
		}
		else if (*s == '>')
		{//終了タグ
			s++;
			break;
		}

		if (*s == '"')
		{
			while(s < e)
			{
				if (*s == '\\' && *(s+1) == '"')
				{
					s++;
				}
				else if (*s == '"')
				{
					break;
				}
				s++;
			}
		}
		if (*s == '\'')
		{
			while(s < e)
			{
				if (*s == '\\' && *(s+1) == '\'')
				{
					s++;
				}
				else if (*s == '\'')
				{
					break;
				}
				s++;
			}
		}

		s++;
	}
	return s;
}

vector<XLHTMLScrapper::tagSt*> XLHTMLScrapper::SelectorToLang(const string& selector)
{
	vector<tagSt*> result;

	bool childOnly = false;
	vector<attributeSt*>* currentAttributes = NULL;
	
	const char * s = selector.c_str();
	const char * e = s + selector.size();
	while( s < e )
	{
		if (*s == '#')
		{
			attributeSt* attribute = new attributeSt;
			attribute->name = "id";
			attribute->expr = expr_eq;
			attribute->value = XLHTMLScrapper::NamePicker(s+1,e,&s);

			tagSt*  tag = new tagSt(childOnly);
			currentAttributes = &(tag->attributes);
			currentAttributes->push_back( attribute );
			result.push_back(tag);

			childOnly = false;
		}
		else if (*s == '.')
		{
			attributeSt* attribute = new attributeSt;
			attribute->name = "class";
			attribute->expr = expr_eq;
			attribute->value = XLHTMLScrapper::NamePicker(s+1,e,&s);

			tagSt*  tag = new tagSt(childOnly);
			currentAttributes = &(tag->attributes);
			currentAttributes->push_back( attribute );
			result.push_back(tag);

			childOnly = false;
		}
		else if (*s == '/' )
		{
			if ( *(s+1) == '/')
			{
				childOnly = false;
				s = s + 2;
			}
			else
			{
				childOnly = true;
				s = s + 1;
			}
		}
		else if (*s == '>' )
		{
			childOnly = true;
			s = s + 1;
		}
		else if (*s == '*' )
		{
			tagSt* tag = new tagSt( childOnly );
			currentAttributes = &(tag->attributes);
			result.push_back(tag);

			childOnly = false;
			s = s + 1;
		}
		else if (*s == ' ' || *s == '\t')
		{
			s++;
		}
		else if (*s == '[')
		{//attribute
			s++;
			if (*s == '@')
			{//XPATH風の @ マークがったら無視する.
				s++;
			}
			const string name = XLHTMLScrapper::NamePicker(s,e,&s);
			if(name.empty())
			{
				continue;
			}
			if (currentAttributes == NULL)
			{
				tagSt* tag = new tagSt( false );
				currentAttributes = &(tag->attributes);
				result.push_back(tag);
			}
			attributeSt* attribute = new attributeSt;
			attribute->name = name;
			attribute->expr = AttributeExprPicker(s,e,&s);
			attribute->value = XLHTMLScrapper::ValuePicker(s,e,&s);

			//XPATHの個数指定
			if ( XLStringUtil::isnumlic(attribute->name) )
			{
				attribute->value = attribute->name;
				attribute->name = "@";
				attribute->expr = expr_posstion;
			}
			else if (attribute->name == "position()" && attribute->expr == expr_eq)
			{
				attribute->name = "@";
				attribute->expr = expr_posstion;
			}

			//拡張 :eq(1) のような絶対指定
			if (attribute->name == "at()")
			{
				attribute->name = "@";
				attribute->expr = expr_at;
			}
			else if (attribute->name[0] == '$' && XLStringUtil::isnumlic(attribute->name.c_str() + 1) )
			{//[at()=2] -> [$2]
				attribute->value = attribute->name.c_str() + 1;
				attribute->name = "@";
				attribute->expr = expr_at;
			}

			//アトリビュートの追加
			currentAttributes->push_back(attribute);

			//確実にattributeの先頭に移動させる
			while(s < e)
			{
				if(*s == ']') break;
				s++;
			}
			//skip ']'
			s++;
		}
		else
		{
			const string name = XLHTMLScrapper::NamePicker(s,e,&s);
			if(name.empty())
			{
				continue;
			}
			tagSt* tag = new tagSt( name , childOnly );
			currentAttributes = &(tag->attributes);
			result.push_back(tag);

			childOnly = false;
		}
	}

	return result;
}


XLHTMLScrapper::exprEnum XLHTMLScrapper::AttributeExprPicker(const char* s,const char* e,const char** next)
{
	const char* start = s;
	for( ;s != e ; ++s )
	{
		if (! (*s == ' ' || *s == '\t') ) break;
	}
	
	if (*s == '=')
	{
		*next = s + 1;
		return expr_eq;
	}
	if (*s == '!' && *(s+1) == '=')
	{
		*next = s + 2;
		return expr_noteq;
	}
	if (*s == '^' && *(s+1) == '=')
	{
		*next = s + 2;
		return expr_first;
	}
	if (*s == '$' && *(s+1) == '=')
	{
		*next = s + 2;
		return expr_end;
	}
	if (*s == '*' && *(s+1) == '=')
	{
		*next = s + 2;
		return expr_mid;
	}

	//不明な命令
	*next = s;
	return expr_exist;
}

void XLHTMLScrapper::runTag(const vector<tagSt*>& tags,vector<tagSt*>::const_iterator& it,vector<string>* result,const string& html)
{
	if (tags.end() == it) return ;

	unsigned int matchCount = 0;
	const char * s = html.c_str();
	const char * e = s + html.size();

	while(s < e)
	{
		string parts;
		if ( checkExprPosstionAttriute(*it) )
		{
			parts = HtmlFinderWithExprPosstion(s,s,e,&s,*it);
		}
		else
		{
			parts = HtmlFinderWithoutExprPosstion(s,e,&s,*it);
		}
		if (parts.empty() )
		{
			return ;
		}
		matchCount++;

		//expr_at の値をみてみる。 指定されていなければ 0 になる.
		unsigned int exprAt = getExprAt(*it);
		if (exprAt >= 1 && exprAt != matchCount)
		{// expr_at が指定されている。が、その条件にはマッチしなかった
			//演算結果を握りつぶします。 そてし、次の部分で再度トライします。
			//parts = "";
			continue;
		}

		//セレクト結果の処理
		if (tags.end() == it+1)
		{//自分が最後の命令なので保存する.
			result->push_back(parts);

			//マッチした部分にさらにマッチすることもあるので、その中を調べる.
			vector<tagSt*>::const_iterator newIT = tags.begin();
			runTag(tags,newIT,result,InnerHTML(parts));
		}
		else
		{//次の命令を実行する.
			vector<tagSt*>::const_iterator newIT = it; ++newIT;
			runTag(tags,newIT,result,InnerHTML(parts));
		}

		if (exprAt >= 1 && exprAt == matchCount)
		{//expr_at が指定されており、これが最後の保存である
			return ;
		}
		if ( (*it)->childOnly == true ) 
		{//親の直後の子しかマッチしてはいけない
			return ;
		}

	}
}

//すーぱーHTMLセレクター
vector<string> XLHTMLScrapper::HTMLSelector(const string& html,const string& selector)
{
	//結果
	vector<string> result;
	
	//セレクターを構文パースして得られたリスト構造(要メモリ解放)
	vector<tagSt*> tags = SelectorToLang(selector);
	vector<tagSt*>::const_iterator newIT = tags.begin();
	runTag(tags,newIT ,&result,html);

	Cleanup(&tags);
	return result;
}

SEXYTEST()
{
	
	{
		string html = string("")
			+ "<html>"
			+ "<body>"
			+ "	<div>"
			+ "		<p>1p1</p>"
			+ "		<p>1p2</p>"
			+ "		<p>1p3</p>"
			+ "	</div>"
			+ "	<div>"
			+ "		<p>2p1</p>"
			+ "		<p>2p2</p>"
			+ "		<p>2p3</p>"
			+ "		<p>2p4</p>"
			+ "	</div>"
			+ "	<div>"
			+ "		<p>3p1</p>"
			+ "		<p>3p2</p>"
			+ "		<p>3p3</p>"
			+ "	</div>"
			+ "	<div>"
			+ "		<p>4p1</p>"
			+ "		<p>4p2</p>"
			+ "		<p>4p3</p>"
			+ "	</div>"
			+ "</body>"
			+ "</html>";
		{
			auto c = XLHTMLScrapper::HTMLSelector(html,"/html/body/div[2]/p[2]");
			SEXYTEST_EQ(c.size() , 1); 

			auto it = c.begin();
			SEXYTEST_EQ(*it ,"<p>2p2</p>"); 
		}
		{
			auto c = XLHTMLScrapper::HTMLSelector(html,"//p[1]");
			SEXYTEST_EQ(c.size() , 4); 

			auto it = c.begin();
			SEXYTEST_EQ(*it ,"<p>1p1</p>"); 
			it++;
			SEXYTEST_EQ(*it ,"<p>2p1</p>"); 
			it++;
			SEXYTEST_EQ(*it ,"<p>3p1</p>"); 
			it++;
			SEXYTEST_EQ(*it ,"<p>4p1</p>"); 
		}
		{
			auto c = XLHTMLScrapper::HTMLSelector(html,"//p[4]");
			SEXYTEST_EQ(c.size() , 1); 

			auto it = c.begin();
			SEXYTEST_EQ(*it ,"<p>2p4</p>"); 
		}
	}
	{
		string html = string("")
		 + "<html>"
         + "    <body>"
         + "        <div id='id_name'>"
         + "            div1<span>hoge1</span>"
         + "        </div>"
         + "        <div class='class_name'>"
         + "            div2<span>hoge2</span>"
         + "        </div>"
         + "        <div>"
         + "            div3<span>hoge</span>"
         + "        </div>"
         + "        <div>"
         + "            div4"
         + "        </div>"
         + "    </body>"
         + "</html>";
		auto c = XLHTMLScrapper::HTMLSelector(html,"//div[3]");
		SEXYTEST_EQ(c.size() , 1); 
	}

	{
		string html = XLFileUtil::cat("./config/testdata/hitokuchi_4510");
		auto c = XLHTMLScrapper::HTMLSelector(html,"//wm:forecast[$2]//wm:weather");
		SEXYTEST_EQ(c.size() , 1); 
		auto it = c.begin();
		string hhh =  _U2A(*it);
//		SEXYTEST_EQ( hhh, "<wm:weather>晴れ時々くもり</wm:weather>" ); 
	}
	{
		string html = XLFileUtil::cat("./config/testdata/hitokuchi_4510");
		auto c = XLHTMLScrapper::HTMLSelector(html,"//wm:forecast[$2]/wm:content/wm:weather");
		SEXYTEST_EQ(c.size() , 1); 
		auto it = c.begin();
		string hhh =  _U2A(*it);
//		SEXYTEST_EQ( hhh, "<wm:weather>晴れ時々くもり</wm:weather>" ); 
	}
	{
		string html = XLFileUtil::cat("./config/testdata/hitokuchi_4510");
		auto c = XLHTMLScrapper::HTMLSelector(html,"//wm:forecast[$2]/wm:content/wm:temperature/wm:max/text()");
		SEXYTEST_EQ(c.size() , 1); 
		auto it = c.begin();
		string hhh =  _U2A(*it);
		SEXYTEST_EQ( hhh, "22" ); 
	}
	{
		string html = XLFileUtil::cat("./config/testdata/hitokuchi_4510");
		auto c = XLHTMLScrapper::HTMLSelector(html,"//wm:forecast[$1]/wm:content/wm:weather");
		SEXYTEST_EQ(c.size() , 1); 
		auto it = c.begin();
		string hhh =  _U2A(*it);
//		SEXYTEST_EQ( hhh, "<wm:weather>晴れのちくもり</wm:weather>" ); 
	}

	{
		string html = XLFileUtil::cat("./config/testdata/selector_test1.htm");
//		auto c = XLHTMLScrapper::HTMLSelector(html,"//*[@id=\"info\"]/div/div[1]/h3");
//		SEXYTEST_EQ(c.size() , 1); 
	}
	{
		string a = 
			"<html>\r\n"
				"<body>\r\n"
					"hoge\r\n"
					"<div class=ccc style=''>\r\n"
						"text1\r\n"
						"<div class=nise style=''>\r\n"
							"totoro2\r\n"
						"</div>\r\n"
						"text2\r\n"
					"</div>\r\n"
					"<div style='aaa: bbb' class=ccc>\r\n"
						"may\r\n"
					"</div>\r\n"
					"<div class=ccc>\r\n"
						"satsuki\r\n"
					"</div>\r\n"
				"</body>\r\n"
			"</html>\r\n"
			;
						;
		string b1 = "hoge\r\n";

		auto c = XLHTMLScrapper::HTMLSelector(a,"/html/body/text()");
		SEXYTEST_EQ(c.size() , 1); 
		auto it = c.begin();
		SEXYTEST_EQ(*it ,b1); 
	}
	{
		string a = 
						"<html>\r\n" 
							"<body>\r\n"
								"hoge\r\n" 
								"<div class=ccc style=''>\r\n"
									"totoro\r\n"
								"</div>\r\n"
						"</html>\r\n"
						;
		string b = 
						"<div class=ccc style=''>\r\n"
							"totoro\r\n"
						"</div>"
						;
		auto c = XLHTMLScrapper::HTMLSelector(a,".ccc");
		SEXYTEST_EQ(c.size() , 1); 
		SEXYTEST_EQ(*(c.begin()) ,b); 
	}
	{
		string a = 
						"<html>\r\n" 
							"<body>\r\n"
								"hoge\r\n" 
								"<div class=ccc style=''>\r\n"
									"totoro\r\n"
									"<div class=nise style=''>\r\n"
										"totoro2\r\n"
									"</div>\r\n"
								"</div>\r\n"
								"<div style='aaa: bbb' class=ccc>\r\n"
									"may\r\n"
								"</div>\r\n"
								"<div class=ccc>\r\n"
									"satsuki\r\n"
								"</div>\r\n"
							"</body>\r\n"
						"</html>\r\n"
						;
		string b1 = 
						"<div class=ccc style=''>\r\n"	
							"totoro\r\n"
							"<div class=nise style=''>\r\n"
								"totoro2\r\n"
							"</div>\r\n"
						"</div>"
						;
		string b2 = 
							"<div class=nise style=''>\r\n"
								"totoro2\r\n"
							"</div>"
						;
		string b3 = 
						"<div style='aaa: bbb' class=ccc>\r\n"
							"may\r\n"
						"</div>"
						;
		string b4 = 
						"<div class=ccc>\r\n"
							"satsuki\r\n"
						"</div>"
						;
		auto c2 = XLHTMLScrapper::HTMLSelector(a,"//div[2]");
		SEXYTEST_EQ(c2.size() , 1); 
		auto it2 = c2.begin();
		SEXYTEST_EQ(*it2 ,b3); 

		auto c = XLHTMLScrapper::HTMLSelector(a,"//div");
		SEXYTEST_EQ(c.size() , 4); 
		auto it = c.begin();
		SEXYTEST_EQ(*it++ ,b1); 
		SEXYTEST_EQ(*it++ ,b2); 
		SEXYTEST_EQ(*it++ ,b3); 
		SEXYTEST_EQ(*it++ ,b4); 

		auto c3 = XLHTMLScrapper::HTMLSelector(a,"//div[-1]");
		SEXYTEST_EQ(c3.size() , 0); 

		auto c4 = XLHTMLScrapper::HTMLSelector(a,"//div[9]");
		SEXYTEST_EQ(c4.size() , 0); 
	}
	{//壊れたhtml
		string a = 
						"<html>\r\n" 
							"<body>\r\n"
								"hoge\r\n" 
								"<div class=ccc style=''>\r\n"
									"totoro\r\n"
							"</body>\r\n"
						"</html>\r\n"
						;
		string b = 
								"<div class=ccc style=''>\r\n"
									"totoro\r\n"
							"</body>\r\n"
						"</html>\r\n"
						;
		auto c = XLHTMLScrapper::HTMLSelector(a,".ccc");
		SEXYTEST_EQ(c.size() , 1); 
		SEXYTEST_EQ(*(c.begin()) ,b); 
	}
	{
		string a = 
						"<html>\r\n" 
							"<body>\r\n"
								"hoge\r\n" 
								"<div class=ccc style=''>\r\n"
									"totoro\r\n"
								"</div>\r\n"
								"<div style='aaa: bbb' class=ccc>\r\n"
									"may\r\n"
								"</div>\r\n"
								"<div class=ccc>\r\n"
									"satsuki\r\n"
								"</div>\r\n"
							"</body>\r\n"
						"</html>\r\n"
						;
		string b1 = 
						"<div class=ccc style=''>\r\n"	
							"totoro\r\n"
						"</div>"
						;
		string b2 = 
						"<div style='aaa: bbb' class=ccc>\r\n"
							"may\r\n"
						"</div>"
						;
		string b3 = 
						"<div class=ccc>\r\n"
							"satsuki\r\n"
						"</div>"
						;
		auto c = XLHTMLScrapper::HTMLSelector(a,"//div");
		SEXYTEST_EQ(c.size() , 3); 
		auto it = c.begin();
		SEXYTEST_EQ(*it++ ,b1); 
		SEXYTEST_EQ(*it++ ,b2); 
		SEXYTEST_EQ(*it++ ,b3); 

		auto c2 = XLHTMLScrapper::HTMLSelector(a,"//div[2]");
		SEXYTEST_EQ(c2.size() , 1); 
		auto it2 = c2.begin();
		SEXYTEST_EQ(*it2 ,b2); 

		auto c3 = XLHTMLScrapper::HTMLSelector(a,"//div[-1]");
		SEXYTEST_EQ(c3.size() , 0); 

		auto c4 = XLHTMLScrapper::HTMLSelector(a,"//div[9]");
		SEXYTEST_EQ(c4.size() , 0); 
	}
	{
		string a = 
						"<html>\r\n" 
							"<body>\r\n"
								"hoge\r\n" 
								"<div class=ccc style=''>\r\n"
									"totoro\r\n"
								"</div>\r\n"
							"</body>\r\n"
						"</html>\r\n"
						;
		string b = 
						"<div class=ccc style=''>\r\n"	
							"totoro\r\n"
						"</div>"
						;
		auto c = XLHTMLScrapper::HTMLSelector(a,"//div");
		SEXYTEST_EQ(c.size() , 1); 
		SEXYTEST_EQ(*(c.begin()) ,b); 
	}
	{
		string a = 
						"<html>\r\n" 
							"<body>\r\n"
								"hoge\r\n" 
								"<div class=ccc style=''>\r\n"
									"totoro\r\n"
								"</div>\r\n"
							"</body>\r\n"
						"</html>\r\n"
						;
		string b = 
						"<div class=ccc style=''>\r\n"
							"totoro\r\n"
						"</div>"
						;
		auto c = XLHTMLScrapper::HTMLSelector(a,".ccc");
		SEXYTEST_EQ(c.size() , 1); 
		SEXYTEST_EQ(*(c.begin()) ,b); 
	}
	{
		string a = 
						"<html>\r\n" 
							"<body>\r\n"
								"hoge\r\n" 
								"<div id=ddd style=''>\r\n"
									"totoro\r\n"
								"</div>\r\n"
							"</body>\r\n"
						"</html>\r\n"
						;
		string b = 
						"<div id=ddd style=''>\r\n"
							"totoro\r\n"
						"</div>"
						;
		auto c = XLHTMLScrapper::HTMLSelector(a,"#ddd");
		SEXYTEST_EQ(c.size() , 1); 
		SEXYTEST_EQ(*(c.begin()) ,b); 
	}

	{
		string a = 
						"<html>\r\n" 
							"<body>\r\n"
								"hoge\r\n" 
							"</body>\r\n"
						"</html>\r\n"
						;
		string b = 
						"<html>\r\n" 
							"<body>\r\n"
								"hoge\r\n" 
							"</body>\r\n"
						"</html>"
						;
		auto c = XLHTMLScrapper::HTMLSelector(a,"/html");
		SEXYTEST_EQ(c.size() , 1); 
		SEXYTEST_EQ(*(c.begin()) ,b); 
	}

	{
		string a = 
						"<html>\r\n" 
							"<body>\r\n"
								"hoge\r\n" 
							"</body>\r\n"
						"</html>\r\n"
						;
		string b = 
						"<body>\r\n"
							"hoge\r\n" 
						"</body>"
						;
		auto c = XLHTMLScrapper::HTMLSelector(a,"/html/body");
		SEXYTEST_EQ(c.size() , 1); 
		SEXYTEST_EQ(*(c.begin()) ,b); 
	}
	{
		string html = "";
		auto c = XLHTMLScrapper::HTMLSelector(html,"");
		SEXYTEST_EQ(c.size() , 0); 
	}
	{
		string html = "";
		auto c = XLHTMLScrapper::HTMLSelector(html,"/");
		SEXYTEST_EQ(c.size() , 0); 
	}
	{
		string html = "";
		auto c = XLHTMLScrapper::HTMLSelector(html,"/ /");
		SEXYTEST_EQ(c.size() , 0); 
	}
	{
		string html = "";
		auto c = XLHTMLScrapper::HTMLSelector(html,"/[");
		SEXYTEST_EQ(c.size() , 0); 
	}
	{
		string html = "";
		auto c = XLHTMLScrapper::HTMLSelector(html,"/[=");
		SEXYTEST_EQ(c.size() , 0); 
	}
	{
		string html = "";
		auto c = XLHTMLScrapper::HTMLSelector(html,"/[aa=");
		SEXYTEST_EQ(c.size() , 0); 
	}
	{
		string html = "";
		auto c = XLHTMLScrapper::HTMLSelector(html,"/[aa='");
		SEXYTEST_EQ(c.size() , 0); 
	}
	{
		string html = "";
		auto c = XLHTMLScrapper::HTMLSelector(html,"/[aa=''");
		SEXYTEST_EQ(c.size() , 0); 
	}
	{
		string html = "";
		auto c = XLHTMLScrapper::HTMLSelector(html,">");
		SEXYTEST_EQ(c.size() , 0); 
	}
}
