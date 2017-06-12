#pragma once;
#include "common.h"

extern "C"{
	#include <julius/juliuslib.h>
};

class Recognition_JuliusPlus: public ISpeechRecognitionInterface
{
public:
	enum 
	{
		RecongType_UseDictationFilter = 0x40
	};

	Recognition_JuliusPlus();
	virtual ~Recognition_JuliusPlus();

	//構築
	virtual bool Create() ;
	//呼びかけを設定します。
	//設定したあと、 CommitRule() てしてね。
	virtual bool SetYobikake(const list<string> & yobikakeList) ;
	//構築したルールを音声認識エンジンにコミットします。
	virtual bool CommitRule(const list<RecongTask>* allCommandRecongTask) ;
	//このコールバックに関連付けられているものをすべて消す
	virtual bool RemoveCallback(const CallbackPP& callback , bool is_unrefCallback) ;
	//音声認識が利用できる音素を作成する
	virtual string ConvertYomi(const string& word) const ;
	//音声認識が利用できる音素を作成する(NO SPACE)
	virtual string ConvertNoSpaceYomi(const string& word) const;
	//よみを確認します。
	virtual bool CheckYomi(const string& str) const;
	//waveファイルデータから直接音声認識させます。
	virtual bool RedirectWave(const vector<char>& wavedata) ;

private:
	//juliusを稼働させます
	void JuliusStop();
	//juliusを停止させます
	bool JuliusStart();
	//呼びかけ検証用のJuliusインスタンスを作ります
	bool JuliusYobikakeStart();
	//juliusのwaveファイルから認識バージョンを起動します。
	bool JuliusWaveStart();

	void MakeMicrophoneJuliusRule(const list<RecongTask>& allCommandRecongTask);
	void MakeFileJuliusRule();

	
	//使用するphoneファイル名を得る.
	string GetPhoneFile() const;
	//認識結果の一部を waveで保存する.
	void WaveCutter(unsigned int all_frame,unsigned int startfream,unsigned int endfream,const string& filename) const;
	//認識結果の一部を抜け出す
	void WavePickup(unsigned int all_frame,unsigned int startfream,unsigned int endfream,const SP16** outStartMemory , int * outSize) const;
	//認識結果の一部を抜け出す
	void MFCCPickup(unsigned int all_frame,unsigned int startfream,unsigned int endfream, HTK_Param* param, VECT*** outVect , int * outNum) const;

	//クラウドフィルターに音声を投げる
	bool checkCouldFilter(const string yomi,unsigned int all_frame,unsigned int startfream,unsigned int endfream) const;
	//ディクテーションフィルターで呼びかけ文字列が入っているか確認する.
	bool checkYobikake(const string & dictationString) const;
	//呼びかけがあれば、waveで保存し、もう一つのjuliusインスタンスで再チェックする.
	bool checkDictation(unsigned int all_frame,unsigned int startfream,unsigned int endfream) const;
	//フレームの取得
	bool getRecongFrame(const SentenceAlign * align , int i_seq , unsigned int* retBeginFrame , unsigned int* retEndFrame) const;
	//フレームの取得
	bool getRecongFrame2(const SentenceAlign * align , int i_seq , unsigned int* retBeginFrame , unsigned int* retEndFrame, unsigned int* retEndFrame2) const;

	//コールバックとキャプチャを取得する
	bool getCallback(const string& elecYomi,CallbackPP* callback) const;

	//読みを取得する
	string ConvertJuliusYomi(const WORD_INFO * wordinfo,int index) const;

	//wav全体をログに落とす
	string wav_to_file_full(const char* flag,unsigned int rotateCount) const;
	//wavの一部だけログに落とす
	string wav_to_file(const char* flag,unsigned int all_frame,unsigned int startfream,unsigned int endfream) const;

	//julius のコールバックイベント
	void OnStatusRecready(Recog *recog);

	//認識を行います。
	void OnOutputResult(Recog *recog) ;
	void OnRecordAdinTrigger(Recog *recog, SP16 *speech, int samplenum);

	void OnOutputResultFileNoSVM(Recog *recog);

	//サウンドカウンターをすすめる
	void SoundCounterCountUp();
	
	list<string> YobikakeListArray;
	string YobikakeFirstChar;
	string YobikakeSecondChar;

	float JuliusFilter1;
	float JuliusFilter2;
	unsigned int JuliusSpeechSpeed;
	unsigned int JuliusYobikakeSpeechSpeed;
	unsigned int JuliusGomiY;
	unsigned int JuliusGomiE;
	unsigned int JuliusGomiD;

	list<RecongTask> AllCommandRecongTask;

	//ディクテーションフィルタの戻り値
	int DictationCheckResult;
	string DictationCheckLogMessage;

	//認識したwaveファイル
	SP16 WaveFileData[MAX_SOUND_BUFFER];
	unsigned int WaveFileDataLen;
	//ダミーのコールバック
	CallbackPP DummyCallback;

	//マイクから読み込んで実行するjulius
	Jconf *jconf;
	Recog *recog;
	//ディクテーションフィルターのためファイルからのjulius
	//正確には、ディクテーションではなくて、たくさんのゴミの中からの再チェック.
	Jconf *jconfYobikake;
	Recog *recogYobikake;
	Jconf *jconfWave;
	Recog *recogWave;

	//juliusをスレッドで動作させます。
	thread *JuliusThread;
	//認識の種類
	unsigned int		RecongTypeFlg;

};
