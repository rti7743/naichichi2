#pragma once

//音声認識する録音バッファの上限
const unsigned int MAX_SOUND_BUFFER = 1024*1024; // 1024kb


struct RecongTask
{
	CallbackPP callback;
	string orignalString;			//ユーザが登録した文字列
	string yomiString;				//音声認識エンジンで利用する読み方(音素ごとにスペース)
	string yomiNoSpaceString;		//音声認識エンジンで利用する読み方(音素の連続)
	string settingFrom;			//どこに設定されていたか名前が入る.

	RecongTask(const CallbackPP& callback,const string& orignalString,const string& yomiString,const string& yomiNoSpaceString,const string& settingFrom)
		: callback(callback)
		 ,orignalString(orignalString)
		 ,yomiString(yomiString)
		 ,yomiNoSpaceString(yomiNoSpaceString)
		 ,settingFrom(settingFrom)
	{
	}
	RecongTask(const RecongTask& o)
	{
		this->callback = o.callback;
		this->orignalString = o.orignalString;
		this->yomiString = o.yomiString;
		this->yomiNoSpaceString = o.yomiNoSpaceString;
		this->settingFrom = o.settingFrom;
	}
};


const string RecongLogResult_Match						= "XX"; //音声認識成功
const string RecongLogResult_SpliterError				= "SP"; //周りがうるさすぎる
const string RecongLogResult_YGomiMatchError			= "GY"; //呼びかけではないゴミにマッチした
const string RecongLogResult_EGomiMatchError			= "GE"; //命令ではないゴミにマッチした
const string RecongLogResult_HardwareError				= "HW"; //ハードウェアエラーが発生。(自動再起動対象)
const string RecongLogResult_SoftwareError				= "SW"; //ソフトウェアエラーが発生。
const string RecongLogResult_ProbabilityError			= "PB"; //確率が足りない
const string RecongLogResult_LongSpeechError			= "LG"; //ゆっくりすぎる
const string RecongLogResult_ShortSpeechError			= "SH"; //早口すぎる
const string RecongLogResult_YohakuError				= "YO"; //普通は設定されている余白が設定されていない
const string RecongLogResult_FuseikakuError			= "FZ"; //不正確
const string RecongLogResult_SMatchError				= "TS"; //終端データにマッチ
const string RecongLogResult_DictationFilterGomiError	= "DD"; //2passモードのディクテーションフィルタでゴミ棄却
const string RecongLogResult_DictationFilterProbError	= "DP"; //2passモードのディクテーションフィルタで確率が足りずに棄却
const string RecongLogResult_DictationFilterShortError	= "DS"; //2passモードのディクテーションフィルタで短すぎる棄却
const string RecongLogResult_DictationFilterLongError	= "DL"; //2passモードのディクテーションフィルタで長すぎる棄却

struct ISpeechRecognitionInterface
{

	ISpeechRecognitionInterface(){}
	virtual ~ISpeechRecognitionInterface(){}

	virtual bool Create() = 0;
	//呼びかけを設定します。
	//設定したあと、 CommitRule() てしてね。
	virtual bool SetYobikake(const list<string> & yobikakeList) = 0;
	//構築したルールを音声認識エンジンにコミットします。
	virtual bool CommitRule(const list<RecongTask>* allCommandRecongTask) = 0;
	//このコールバックに関連付けられているものをすべて消す
	virtual bool RemoveCallback(const CallbackPP& callback , bool is_unrefCallback) = 0;
	//音声認識が利用できる音素を作成する
	virtual string ConvertYomi(const string& word) const = 0;
	//音声認識が利用できる音素を作成する
	virtual string ConvertNoSpaceYomi(const string& word) const = 0;
	//よみを確認します。
	virtual bool CheckYomi(const string& str) const = 0;
	//waveファイルデータから直接音声認識させます。
	virtual bool RedirectWave(const vector<char>& wavedata) = 0;
};


class Recognition_Factory
{
public:
	Recognition_Factory();
	virtual ~Recognition_Factory();

	void Free();
	bool Create();
	bool AddCommandRegexp(const CallbackPP& callback ,const string& p1,const string& settingFrom);
	bool CommitRule();

	bool isCretated() const
	{
		return this->Engine != NULL;
	}

	//waveファイルデータから直接音声認識させます。
	bool RedirectWave(const vector<char>& wavedata);

	bool RemoveCallback(const CallbackPP& callback , bool is_unrefCallback);
	const list<RecongTask>* getAllCommandRecongTask() const;
	//このワードは、他で登録されていないか？
	bool isAlreadyRegist5(const string& now,const string& settingFromNow,const string& settingFrom2,const string& settingFrom3,const string& settingFrom4,const string& settingFrom5) const;
	//このワードは、同時に5つまで設定できるうちの他の4つと、重複していないか？
	bool isAlreadyRegistOther5(const string& now,const string& p2,const string& p3,const string& p4,const string& p5) const;

	string convertYomi(const string& yomi) const;
	bool checkYomi(const string& str) const;
	bool checkExprYomi(const string& str) const;
	bool isAlreadyRegistLow(const list<RecongTask>& list,const string& p1,const string& settingFrom) const;

	void PlayRecongOKSoundForce() const;
	void PlayRecongOKSound() const;
	void PlayRecongNGSound() const;

private:
	bool isAlreadyRegistLow(const list<RecongTask>& list,const string& str
		,const string& settingFrom1,const string& settingFrom2,const string& settingFrom3,const string& settingFrom4,const string& settingFrom5) const;
	bool isAlreadyRegistTemporaryLow(const CallbackPP& callback,const string& str) const;

	ISpeechRecognitionInterface* Engine;


	list<RecongTask> AllCommandRecongTask;

	//音声認識成功/失敗時に再生する音
	string RecongOKSound;
	string RecongNGSound;
	
};
