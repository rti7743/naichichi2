//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include "ScriptRunner.h"
#include "Recognition_Factory.h"
#include "Recognition_JuliusPlus.h"
#include "Recognition_JuliusPlusEnglish.h"
#include "Recognition_None.h"
#include "Fire.h"
#include "MainWindow.h"

Recognition_Factory::Recognition_Factory()
{
	this->Engine = NULL;
}

Recognition_Factory::~Recognition_Factory()
{
	DEBUGLOG("stop...");

	Free();

	DEBUGLOG("done");
}

void Recognition_Factory::Free()
{
	delete this->Engine;
	this->Engine = NULL;
}

bool Recognition_Factory::Create()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	bool isReCreate = false;
	if (this->Engine != NULL)
	{
		Free();
		isReCreate = true;
	}

	const string system_lang = MainWindow::m()->Config.Get("system_lang","");
	const string name = MainWindow::m()->Config.Get("recong_type", "" );
	const list<string> yobikakeListArray = MainWindow::m()->Config.FindGets("recong__yobikake__");
	
	if (name == "julius_standalone" || name == "" || name == "julius_standalone_light")
	{
		if (system_lang == "english")
		{
			this->Engine = new Recognition_JuliusPlusEnglish();
		}
		else
		{
			this->Engine = new Recognition_JuliusPlus();
		}
	}
	else if (name == "none")
	{
		this->Engine = new Recognition_None();
	}
	else
	{
		ERRORLOG("音声認識エンジン" << name << "がありません" );
		ASSERT(0);	//デバッグならここで落としてあげるのが優しさ。
		
		ERRORLOG("仕方ないので、ディフォルト値の音声認識エンジンを選択します");
		this->Engine = new Recognition_JuliusPlus();
	}

	try
	{
		this->Engine->Create();
		this->Engine->SetYobikake(yobikakeListArray);
		NOTIFYLOG("音声認識エンジン" << name << "(" << system_lang << ")" << "の構築完了");

		//成功/失敗時に再生する音をキャッシュする(今はファイル名だけだが)
		this->RecongOKSound = MainWindow::m()->Config.Get("recong_ok_mp3", "recong_ok_tyariri.mp3" );
		this->RecongNGSound = MainWindow::m()->Config.Get("recong_ng_mp3", "recong_ng_pyuin.mp3" );

		if (isReCreate)
		{
			//すでにあるインスタンスを殺して作り直した場合は、既存の音声認識単語をぶっこむ
			this->CommitRule();
		}
	}
	catch(XLException& e)
	{
		ERRORLOG("音声認識エンジン" << name << "を構築できませんでした。Exception:" << e.what() );
		ASSERT(0);	//デバッグならここで落としてあげるのが優しさ。
		ERRORLOG("仕方ないので、音声認識機能をオフにします.");

		delete this->Engine;
		this->Engine = new Recognition_None();

		throw e;
	}
	return true;
}

bool Recognition_Factory::AddCommandRegexp(const CallbackPP& callback ,const string& p1,const string& settingFrom)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	if (p1.empty())
	{
		return false;
	}
	if (this->Engine == NULL)
	{
		ERRORLOG("警告 音声認識エンジンがありません");
		return false;
	}

	//正規表現を展開する
	list<string> plainList;
	try
	{
		plainList = XLStringUtil::RegexToPlain(p1);
	}
	catch(XLException& e)
	{
		ERRORLOG("正規表現 " << p1 << " を解析できません。" << e.what() );
		return false;
	}

	//既に登録しているものがないかチェック
	for(auto it = plainList.begin() ; it != plainList.end() ; ++it )
	{
		if (isAlreadyRegistLow(this->AllCommandRecongTask,*it,settingFrom,"","","",""))
		{
			ERRORLOG("既に登録されています:" << *it );
			return false;
		}
		const string yomi = this->Engine->ConvertYomi(*it);
		if (! XLStringUtil::isAsciiString(yomi) )
		{
			ERRORLOG("内部エラー読みがふれません1:" << *it <<" // " << yomi);
			return false;
		}
	}

	//実際の登録をする
	for(auto it = plainList.begin() ; it != plainList.end() ; ++it )
	{
		const string yomi = this->Engine->ConvertYomi(*it);
		const string yomiNoSpace = this->Engine->ConvertNoSpaceYomi(*it);

		this->AllCommandRecongTask.push_back(RecongTask(callback,*it,yomi,yomiNoSpace,settingFrom));
	}

	DEBUGLOG(string() + "音声認識登録:" + p1 );
	return true;
}

//waveファイルデータから直接音声認識させます。
bool Recognition_Factory::RedirectWave(const vector<char>& wavedata)
{
	if (this->Engine == NULL)
	{
		ERRORLOG("警告 音声認識エンジンがありません");
		return false;
	}
	return this->Engine->RedirectWave(wavedata);
}

//このワードは、他で登録されていないか？
bool Recognition_Factory::isAlreadyRegist5(const string& now,const string& settingFromNow,const string& settingFrom2,const string& settingFrom3,const string& settingFrom4,const string& settingFrom5) const
{
	if (now.empty())
	{
		return false;
	}
	if (this->Engine == NULL)
	{
		ERRORLOG("警告 音声認識エンジンがありません");
		return false;
	}

	//正規表現を展開する
	const auto plainListNow = XLStringUtil::RegexToPlain(now);

	//既に登録しているものがないかチェック
	for(auto it = plainListNow.begin() ; it != plainListNow.end() ; ++it )
	{
		if ( it->empty() )
		{
			continue;
		}

		if (isAlreadyRegistLow(this->AllCommandRecongTask,*it,settingFromNow,settingFrom2,settingFrom3,settingFrom4,settingFrom5))
		{
			ERRORLOG(string() + "既に登録されています:" + *it );
			return true;
		}
		
		if (! checkYomi(*it) )
		{
			ERRORLOG("内部エラー読みがふれません2:" << *it );
			return true;
		}
	}

	return false;
}

//このワードは、同時に5つまで設定できるうちの他の4つと、重複していないか？
bool Recognition_Factory::isAlreadyRegistOther5(const string& now,const string& p2,const string& p3,const string& p4,const string& p5) const
{
	if (now.empty())
	{
		return false;
	}
	if (this->Engine == NULL)
	{
		ERRORLOG("警告 音声認識エンジンがありません");
		return false;
	}

	//正規表現を展開する
	const auto plainListNow = XLStringUtil::RegexToPlain(now);
	const auto plainListP2 = XLStringUtil::RegexToPlain(p2);
	const auto plainListP3 = XLStringUtil::RegexToPlain(p3);
	const auto plainListP4 = XLStringUtil::RegexToPlain(p4);
	const auto plainListP5 = XLStringUtil::RegexToPlain(p5);

	//既に登録しているものがないかチェック
	for(auto it = plainListNow.begin() ; it != plainListNow.end() ; ++it )
	{
		if ( it->empty() )
		{
			continue;
		}

		for(auto it2 = plainListP2.begin() ; it2 != plainListP2.end() ; ++it2 )
		{
			if ( *it == *it2 )
			{
				return true;
			}
		}
		for(auto it3 = plainListP3.begin() ; it3 != plainListP3.end() ; ++it3 )
		{
			if ( *it == *it3 )
			{
				return true;
			}
		}
		for(auto it4 = plainListP4.begin() ; it4 != plainListP4.end() ; ++it4 )
		{
			if ( *it == *it4 )
			{
				return true;
			}
		}
		for(auto it5 = plainListP5.begin() ; it5 != plainListP5.end() ; ++it5 )
		{
			if ( *it == *it5 )
			{
				return true;
			}
		}
	}

	return false;
}

bool Recognition_Factory::CommitRule()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	if (this->Engine == NULL)
	{
		ERRORLOG("警告 音声認識エンジンがありません");
		return false;
	}

	//アップデート処理を依頼する.
	this->Engine->CommitRule(&this->AllCommandRecongTask);
	DEBUGLOG("CommitRule");
	return true;
}
const list<RecongTask>* Recognition_Factory::getAllCommandRecongTask() const
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	return &this->AllCommandRecongTask; 
}



bool Recognition_Factory::RemoveCallback(const CallbackPP& callback , bool is_unrefCallback)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	for(auto it = this->AllCommandRecongTask.begin() ; it != this->AllCommandRecongTask.end() ;)
	{
		if (it->callback == callback)
		{
			this->AllCommandRecongTask.erase(it);
			it = this->AllCommandRecongTask.begin();
		}
		else
		{
			++it;
		}
	}

	if (this->Engine == NULL)
	{
		ERRORLOG("警告 音声認識エンジンがありません");
		return false;
	}

	//このコールバックに関連付けられているものをすべて消す
	//一連の削除が終わったら、Commit() しないといけないよ。
	this->Engine->RemoveCallback(callback,is_unrefCallback);
	return true;
}
string Recognition_Factory::convertYomi(const string& str) const
{
	if (this->Engine == NULL)
	{
		ERRORLOG("警告 音声認識エンジンがありません");
		return "";
	}
	return this->Engine->ConvertYomi(str);
}

bool Recognition_Factory::checkYomi(const string& str) const
{
	if (this->Engine == NULL)
	{
		ERRORLOG("警告 音声認識エンジンがありません");
		return false;
	}
	return this->Engine->CheckYomi(str);
}

bool Recognition_Factory::checkExprYomi(const string& str) const
{
	//正規表現の妥当性を検証する.
	int kakkoCount = 0;
	for( const char * p = str.c_str()  ; *p ; ++p )
	{
		if ( XLStringUtil::isMultiByte(p))
		{
			const char* nextP = XLStringUtil::nextChar(p);
			p = nextP - 1; //for で ++ されるので引いておく.
			continue;
		}

		if ( *p == '.' && *(p+1) == '+')
		{ // .+ --> (:?.*)
			++p;
		}
		else if (*p == '(' && *(p+1) == '?' && *(p+2) == ':' )
		{
			p+=2;
			kakkoCount++;
		}
		else if (*(p+1) == '?')
		{
			if (*p == ')')
			{// (まる|さんかく)? --> (まる|さんかく|)
				kakkoCount--;
			}
			else 
			{// なのは? --> なの(は|)
			}
			++p;
		}
		else if (*(p) == '(')
		{
			kakkoCount++;
		}
		else if (*(p) == ')')
		{
			kakkoCount--;
		}
		else if (*p == '*' || *p == '+' || *p == '.' || *p == '[' || *p == ']')
		{
			return false;
		}
		else
		{
		}
	}

	//かっこの数が一致しません。
	if (kakkoCount != 0)
	{
		return false;
	}

	//正規表現を展開する
	const auto plainList = XLStringUtil::RegexToPlain(str);
	//既に登録しているものがないかチェック
	for(auto it = plainList.begin() ; it != plainList.end() ; ++it )
	{
		if (!checkYomi(*it))
		{
			ERRORLOG("読めません" + *it );
			return false;
		}
	}
	return true;
}


bool Recognition_Factory::isAlreadyRegistLow(const list<RecongTask>& list,const string& p1
	,const string& settingFrom1,const string& settingFrom2,const string& settingFrom3,const string& settingFrom4,const string& settingFrom5) const
{
	if (this->Engine == NULL)
	{
		ERRORLOG("警告 音声認識エンジンがありません");
		return false;
	}

	const string yomi = this->Engine->ConvertYomi(p1);
	for(auto it = list.begin() ; it != list.end() ; ++it )
	{
		if ( it->yomiString == yomi )
		{//重複している
			if (it->settingFrom != settingFrom1)
			{
				
				if ( (!settingFrom2.empty()) && it->settingFrom == settingFrom2)
				{//設定2でされているのでセーフ
					continue;
				}
				if ( (!settingFrom3.empty()) && it->settingFrom == settingFrom3)
				{//設定3でされているのでセーフ
					continue;
				}
				if ( (!settingFrom4.empty()) && it->settingFrom == settingFrom4)
				{//設定4でされているのでセーフ
					continue;
				}
				if ( (!settingFrom5.empty()) && it->settingFrom == settingFrom5)
				{//設定5でされているのでセーフ
					continue;
				}

				return true;
			}
		}
	}
	return false;
}

void Recognition_Factory::PlayRecongOKSoundForce() const
{
	MainWindow::m()->MusicPlayAsync.Play(this->RecongOKSound,1);
}


void Recognition_Factory::PlayRecongOKSound() const
{
	if ( MainWindow::m()->IsRecongpause() )
	{
		return ;
	}
	MainWindow::m()->MusicPlayAsync.Play(this->RecongOKSound,1);
}
void Recognition_Factory::PlayRecongNGSound() const
{
	if ( MainWindow::m()->IsRecongpause() )
	{
		return ;
	}
	MainWindow::m()->MusicPlayAsync.PlaySync(this->RecongNGSound);
}

#endif //WITH_CLIENT_ONLY_CODE==1
