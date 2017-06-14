#include "common.h"
#include "MainWindow.h"
#include "sexylog.h"
#include "MecabControl.h"
#include "XLStringUtil.h"
#include "JuliusOnsoUtil.h"
#if _MSC_VER
	#pragma comment(lib, "openjtalk.lib")
#endif

MecabControl::MecabControl(void)
{
	this->mecab = NULL;
}


MecabControl::~MecabControl(void)
{
	DEBUGLOG("stop...");
	if (this->mecab)
	{
		mecab_destroy(this->mecab);
		this->mecab = NULL;
	}
	DEBUGLOG("done");
}

bool MecabControl::Create(const string& dicpath,const string& cmudictFilename)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	ASSERT(this->mecab == NULL);

	//英語の発音辞書
	this->CMUDict.Create(cmudictFilename);

	//mecabの構築
	const int argc = 3;
	char *argv[] = {(char *) "mecab", (char *) "-d",(char *) dicpath.c_str() ,NULL };
	this->mecab = mecab_new(argc,argv);
	if (!this->mecab)
	{
		throw XLEXCEPTION("mecabを構築できませんでした。" << argv[1] );
	}
	return true;
}


void MecabControl::Parse(const string& str,const std::function< void(const mecab_node_t* node) >& callbackNode)
{
	
	mecab_node_t *head;
	mecab_node_t *node;

	lock_guard<mutex> al(this->lock);
	
	head = (mecab_node_t *) mecab_sparse_tonode(this->mecab,(char*) (_A2U(str.c_str())) );
	if(head == NULL) return;

	for (node = head; node != NULL; node = node->next)
	{
		if(node->stat != MECAB_BOS_NODE && node->stat != MECAB_EOS_NODE)
		{
			callbackNode(node);
		}
	}
}

//分かち書きしたものを返します
vector<string> MecabControl::Wakachigai(const string& str)
{
	
	vector<string> retVec;

	mecab_node_t *head;
	mecab_node_t *node;

	lock_guard<mutex> al(this->lock);
	int mecabLen = 0;
	const string strU = _A2U(str.c_str());

	
	head = (mecab_node_t *) mecab_sparse_tonode(this->mecab,strU.c_str() );
	if(head == NULL) return retVec;

	for (node = head; node != NULL; node = node->next)
	{
		if(node->stat != MECAB_BOS_NODE && node->stat != MECAB_EOS_NODE)
		{
			string s = _U2A( string(node->surface, 0,node->length) );
			retVec.push_back(s);
		}
	}

	return retVec;
}


//漢字その他をすべてひらがなに直します
string MecabControl::KanjiAndKanakanaToHiragana(const string& str)
{
	
	string yomi;

	mecab_node_t *head;
	mecab_node_t *node;

	lock_guard<mutex> al(this->lock);
	int mecabLen = 0;
	const string strU = _A2U(str.c_str());
	
	head = (mecab_node_t *) mecab_sparse_tonode(this->mecab,strU.c_str() );
	if(head == NULL) return "";

	for (node = head; node != NULL; node = node->next)
	{
		if(node->stat != MECAB_BOS_NODE && node->stat != MECAB_EOS_NODE)
		{
			const string feature = _U2A(node->feature);
			vector<string> kammalist = XLStringUtil::split_vector(",",feature);
			if (kammalist.size() > 7 && kammalist[7] != "*")
			{
				yomi += kammalist[7];
			}
			else
			{
				string s = _U2A( string(node->surface, 0,node->length) );
				if (s.size() >= 1 && s[0] == '?' )
				{
					s = _U2A( string(strU,mecabLen,node->length) );
				}

				if ( XLStringUtil::isnumlic( s ) )
				{
					yomi += XLStringUtil::double2yomi(s);
				}
				else if ( XLStringUtil::isalpha( s ) )
				{//強引に英単語を読む
					yomi += EnglishWordToKana(s);
				}
				else
				{
					yomi += s;
				}
			}
			mecabLen += node->length;
		}
	}


	//mecabだとカタカナ読みしか取れないので、強制的にひらがなに直します。
	return XLStringUtil::mb_convert_kana(yomi,"cHsa");
}

//英語の単語の日本語読みを取得する
string MecabControl::EnglishWordToKana(const string& englishWord ) const
{
	string r = this->CMUDict.GetYomi(englishWord);
	if (! r.empty())
	{//辞書で読めた発音記号から文字列を復元する
		return JuliusOnsoUtil::EnglishOnsoToJapanese(r,false);
	}
	//辞書にない!
	//ローマ字として読んでみる
	r = XLStringUtil::mb_convert_typo(englishWord,"r");
	const char * p = &r[0];
	for(; *p ; )
	{
		if (XLStringUtil::isMultiByte(p))
		{
			return "";
		}
		if ( ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z')) )
		{//読めていない
			break;
		}
		p ++;
	}
	if (p == NULL)
	{//ローマ字で読めた
		return r;
	}
	//ローマ字でもよめない
	return "";
}

MecabThreadFeature* MecabControl::mallocFeatures(const string& str)
{
	
	lock_guard<mutex> al(this->lock);

	int i = 0;
	mecab_node_t *head;
	mecab_node_t *node;

	int mecabLen = 0;
	const string strU = _A2U(str.c_str());
	
	head = (mecab_node_t *) mecab_sparse_tonode(this->mecab,strU.c_str() );
	if(head == NULL) return NULL;

	MecabThreadFeature* mf = (MecabThreadFeature*)malloc(sizeof(MecabThreadFeature));
	if (!mf) return NULL;
	mf->feature = NULL;
	mf->size = 0;

	for (node = head; node != NULL; node = node->next) {
		if(node->stat != MECAB_BOS_NODE && node->stat != MECAB_EOS_NODE)
			mf->size++;
	}
	mf->feature = (char **) calloc(mf->size, sizeof(char *));
	for (node = head; node != NULL; node = node->next) {
		if(node->stat != MECAB_BOS_NODE && node->stat != MECAB_EOS_NODE){

			string feature = _U2A(node->feature);
			string r;

			const vector<string> kammalist = XLStringUtil::split_vector(",",feature);
			if (kammalist.size() > 7 && kammalist[7] != "*")
			{
				r = kammalist[7];
			}
			else
			{
				r = _U2A(string(node->surface, 0,node->length));
				if (r.size() >= 1 && r[0] == '?' )
				{
					r = _U2A( string(strU,mecabLen,node->length) );
				}

				if ( XLStringUtil::isnumlic( r ) )
				{
					r = XLStringUtil::double2yomi(r);
					r = XLStringUtil::mb_convert_kana(r,"C");
					feature = "名詞,数,*,*,*,*,"+r+","+r+","+r+",1/2,C3";
				}
				else if ( XLStringUtil::isalpha( r ) )
				{//強引に英単語を読む
					r = EnglishWordToKana(r);
					r = XLStringUtil::mb_convert_kana(r,"C");
					feature = "名詞,英語,*,*,*,*,"+r+","+r+","+r+",1/2,C3";
				}
				
			}
			r += "," + feature;

			mf->feature[i] = strdup(r.c_str() );
			mecabLen += node->length;
			i++;
		}
	}

	return mf;
}

//漢字その他をすべてローマ字に変換します。(英語はそのまま)
string MecabControl::KanjiAndKanaToRomaji(const string& str,bool isRomajiOneSepalate)
{
	
	string yomi ;
	const auto vec = XLStringUtil::split_vector(" ",XLStringUtil::mb_convert_kana( str, "as" ) );
	for(unsigned int i = 0 ; i < vec.size() ; i ++ )
	{
		if (i >= 1)
		{
			yomi += " ";
		}
		if (! XLStringUtil::isalpnum(vec[i]) )
		{//日本語ならローマ字にしちゃう

			mecab_node_t *head;
			mecab_node_t *node;

			int mecabLen = 0;
			const string strU = _A2U(vec[i].c_str());
	
			head = (mecab_node_t *) mecab_sparse_tonode(this->mecab,strU.c_str() );
			if(head != NULL) 
			{
				for (node = head; node != NULL; node = node->next)
				{
					if(node->stat != MECAB_BOS_NODE && node->stat != MECAB_EOS_NODE)
					{
						const string feature = _U2A(node->feature);
						vector<string> kammalist = XLStringUtil::split_vector(",",feature);
						if (kammalist.size() > 7 && kammalist[7] != "*")
						{
							string s = XLStringUtil::mb_convert_kana(kammalist[7],"cHsa");
							if (isRomajiOneSepalate)
							{
								s = XLStringUtil::mb_ngram_string(s,1," ");
							}
							s = XLStringUtil::mb_convert_typo( s , "R") ;

							if (kammalist[0] == "助詞")	yomi += " " + s + " ";
							else                        yomi += s;
						}
						else
						{
							string s = _U2A( string(node->surface, 0,node->length) );
							if (s.size() >= 1 && s[0] == '?' )
							{
								s = _U2A( string(strU,mecabLen,node->length) );
							}

							if ( XLStringUtil::isnumlic( s ) )
							{//数字もそのままに
								yomi += s;
							}
							else if ( XLStringUtil::isalpha( s ) )
							{//英単語はそのままに
								yomi += " " + s + " ";
							}
							else
							{//nazo
								s = XLStringUtil::mb_convert_kana(s,"cHsa");
								if (isRomajiOneSepalate)
								{
									s = XLStringUtil::mb_ngram_string(s,1," ");
								}
								yomi += XLStringUtil::mb_convert_typo( s , "R") ;
							}
						}
						mecabLen += node->length;
					}
				}
			}
		}
		else
		{//英語
			yomi += vec[i];
		}
	}
	return yomi;
/*
	string yomi;

	mecab_node_t *head;
	mecab_node_t *node;

	lock_guard<mutex> al(this->lock);
	int mecabLen = 0;
	const string strU = _A2U(str.c_str());
	
	head = (mecab_node_t *) mecab_sparse_tonode(this->mecab,strU.c_str() );
	if(head == NULL) return "";

	for (node = head; node != NULL; node = node->next)
	{
		if(node->stat != MECAB_BOS_NODE && node->stat != MECAB_EOS_NODE)
		{
			const string feature = _U2A(node->feature);
			vector<string> kammalist = XLStringUtil::split_vector(",",feature);
			if (kammalist.size() > 7 && kammalist[7] != "*")
			{
				yomi += XLStringUtil::mb_convert_typo( XLStringUtil::mb_convert_kana(kammalist[7],"cHsa") , "R") ;
			}
			else
			{
				string s = _U2A( string(node->surface, 0,node->length) );
				if (s.size() >= 1 && s[0] == '?' )
				{
					s = _U2A( string(strU,mecabLen,node->length) );
				}

				if ( XLStringUtil::isnumlic( s ) )
				{//数字もそのままに
					yomi += s;
				}
				else if ( XLStringUtil::isalpha( s ) )
				{//英単語はそのままに
					yomi += " " + s + " ";
				}
				else
				{//nazo
					yomi += " " +  XLStringUtil::mb_convert_typo( XLStringUtil::mb_convert_kana(s,"cHsa") , "R");
				}
			}
			mecabLen += node->length;
		}
	}
	if (yomi.empty()) return "";
	return yomi.c_str() + 1;
*/
}

void MecabControl::FreeFeatures(MecabThreadFeature* mf)
{
//  int i;
  if (mf == NULL) return ;
  if(mf->feature != NULL){
    for(int i = 0;i < mf->size;i++)
	{
		if (mf->feature[i])
		{
			free(mf->feature[i]);
		}
	}
    free(mf->feature);
    mf->feature = NULL;
    mf->size = 0;
  }
  free(mf);
}
