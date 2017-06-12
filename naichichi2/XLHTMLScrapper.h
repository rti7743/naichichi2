#pragma once
#include <vector>
#include <string>

class XLHTMLScrapper
{
public:
	//今いるタグの中を取得する.
	static string InnerHTML(const string& html);
	//今いるタグの中を取得する.
	static string InnerText(const string& html);
	//属性を取得する
	static string HTMLAttr(const string& html,const string& attributeName);
	//すーぱーHTMLセレクター
	static vector<string> HTMLSelector(const string& html,const string& selector);
private:
	enum exprEnum
	{
		 expr_exist
		,expr_eq
		,expr_noteq
		,expr_first
		,expr_end
		,expr_mid
		,expr_posstion			//親タグからN番目
		,expr_at				//全体からN番目 必ず1つだけ選択される
	};

	struct attributeSt
	{
		string name;
		string value;
		exprEnum	expr;
	};

	struct tagSt
	{
		string name;
		bool childOnly;
		vector<attributeSt*> attributes;
		
		tagSt(bool childOnly) : childOnly(childOnly)
		{
		}
		tagSt(string name,bool childOnly) : name(name) , childOnly(childOnly)
		{
		}
	};

	static string NamePicker(const char* s,const char* e,const char** next);
	static string ValuePicker(const char* s,const char* e,const char** next);
	static void Cleanup(vector<tagSt*>* tags);
	static const char* skipTagStart(const char* s, const char* e);
	static bool checkTextAttribute(unsigned int posstionCount,const vector<attributeSt*>* checkAttributes );
	static bool checkEmptyNode(const char* s,const char* e);
	static bool checkTagAttribute(const char* s,const char* e,unsigned int posstionCount,const vector<attributeSt*>* checkAttributes);
	static bool checkAttributeExpr(const attributeSt* attribute,string& value );
	//このタグの閉じタグ終了の位置まで移動する
	static const char* JmpCloseTag(const char* s,const char* e);
	//この開始タグの終端までスキップする
	static const char* JmpEndTag(const char* s,const char* e);
	static vector<tagSt*> SelectorToLang(const string& selector);
	static exprEnum AttributeExprPicker(const char* s,const char* e,const char** next);
	static void runTag(const vector<tagSt*>& tags,vector<tagSt*>::const_iterator& it,vector<string>* result,const string& html);
	static bool checkExprPosstionAttriute(const tagSt* tag );
	static string HtmlFinderWithoutExprPosstion(const char* s, const char* e,const char** next,tagSt* tag);
	static string HtmlFinderWithExprPosstion(const char* parentTagS,const char* s, const char* e,const char** next,tagSt* tag);
	static bool tryCommentSkip(const char* s, const char* e,const char** outS);
	static unsigned int getExprAt(const tagSt* tag);

};
