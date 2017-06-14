//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include <string>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>
#include "ScriptRunner.h"
#include "Recognition_Factory.h"
#include "Recognition_JuliusPlusEnglish.h"
#include "MainWindow.h"
#include "XLFileUtil.h"
#include "XLStringUtil.h"
#include "JuliusOnsoUtil.h"
#include "SystemMisc.h"


#if _MSC_VER
	#pragma comment(lib, "julius.lib")
	#pragma comment(lib, "zlib.lib")
	#pragma comment(lib, "portaudio.lib")
#endif

//使用するphoneファイルを得る.
string Recognition_JuliusPlusEnglish::GetPhoneFile() const
{
	return MainWindow::m()->GetConfigBasePath("/julius/voxforge/tiedlist");
}

//読みを作成する。
string Recognition_JuliusPlusEnglish::ConvertYomi(const string& word) const
{
	const string yomi = JuliusOnsoUtil::ConvertEnglishSentenceToYomi(word,true);

	const string phoneFilename = this->GetPhoneFile();
	string badString;
	auto yomiWords = XLStringUtil::split(" @ ",yomi );
	for(auto it = yomiWords.begin() ; it != yomiWords.end() ; it++ )
	{
		string badString;
		bool r = JuliusOnsoUtil::CheckYomi(phoneFilename,*it,&badString);
		if ( !r )
		{
			NOTIFYLOG("この音は認識できません。文字列:" << word << "読み: " << yomi << " 部分:" << *it << " ダメ部分:" << badString );
			return "";
		}
	}

	return yomi;
}

//読みを作成する。
string Recognition_JuliusPlusEnglish::ConvertYomiNoJapanesWakachigaki(const string& word) const
{
	const string yomi = JuliusOnsoUtil::ConvertEnglishSentenceToYomi(word,false);

	const string phoneFilename = this->GetPhoneFile();
	string badString;
	auto yomiWords = XLStringUtil::split(" @ ",yomi );
	for(auto it = yomiWords.begin() ; it != yomiWords.end() ; it++ )
	{
		string badString;
		bool r = JuliusOnsoUtil::CheckYomi(phoneFilename,*it,&badString);
		if ( !r )
		{
			NOTIFYLOG("この音は認識できません。文字列:" << word << "読み: " << yomi << " 部分:" << *it << " ダメ部分:" << badString );
			return "";
		}
	}

	return yomi;
}

//音声認識が利用できる音素を作成する(NO SPACE)
string Recognition_JuliusPlusEnglish::ConvertNoSpaceYomi(const string& word) const
{
	const string yomi = ConvertYomi(word);
	return XLStringUtil::replace(yomi," ","");
}

//よみを確認します。
bool Recognition_JuliusPlusEnglish::CheckYomi(const string& str) const
{
	if (str.empty())
	{
		return true;
	}
	const string yomi = ConvertYomi(str);
	if (yomi.empty())
	{
		return false;
	}

	const string phoneFilename = MainWindow::m()->GetConfigBasePath("/julius/voxforge/tiedlist");

	auto yomiWords = XLStringUtil::split(" @ ",yomi );
	for(auto it = yomiWords.begin() ; it != yomiWords.end() ; it++ )
	{
		string badString;
		bool r = JuliusOnsoUtil::CheckYomi(phoneFilename,*it,&badString);
		if ( !r )
		{
			NOTIFYLOG("この音は認識できません。文字列:" << str << "読み: " << yomi << " 部分:" << *it << " ダメ部分:" << badString );
			return false;
		}
	}


	return true;
}

string Recognition_JuliusPlusEnglish::wav_to_file_full(const char* flag,unsigned int rotateCount) const
{
	ASSERT(flag[0]=='F');
	ASSERT(flag[1]=='B'||flag[1]=='X');
	const string filename = string(flag)+"_" + num2str(time(NULL));
	const string filepath = XLFileUtil::getTempDirectory("naichichi2") + "/"+ filename + ".wav";
	FILE *recfile_fp = wrwav_open( (char*) filepath.c_str(), 16000);
	if(recfile_fp)
	{
		wrwav_data(recfile_fp, (short*)this->WaveFileData , this->WaveFileDataLen );
		wrwav_close(recfile_fp);
	}
	string flagString = string(flag)+"_";
	XLFileUtil::rotate( XLFileUtil::getTempDirectory("naichichi2") ,flagString ,rotateCount);
	return filename;
}

string Recognition_JuliusPlusEnglish::wav_to_file(const char* flag,unsigned int all_frame,unsigned int startfream,unsigned int endfream) const
{
	ASSERT(flag[0]=='S');
	ASSERT(flag[1]=='B'||flag[1]=='X');
	const SP16 * wavStart = NULL;
	int wavSize = 0;
	WavePickup(all_frame,startfream,endfream, &wavStart, &wavSize );
	const string filename = string(flag)+"_" + num2str(time(NULL));
	const string filepath = XLFileUtil::getTempDirectory("naichichi2") + "/"+filename + ".wav";
	FILE *recfile_fp = wrwav_open( (char*) filename.c_str(), 16000);
	if(recfile_fp)
	{
		wrwav_data(recfile_fp, (short*)wavStart , wavSize/2 );
		wrwav_close(recfile_fp);
	}
//	DEBUGLOG("wav_yobikake_log save: " << filename );
	string flagString = string(flag)+"_";
	XLFileUtil::rotate( XLFileUtil::getTempDirectory("naichichi2") ,flagString ,100000);
	return filename;
}

Recognition_JuliusPlusEnglish::Recognition_JuliusPlusEnglish()
{
	this->jconf = NULL;
	this->recog = NULL;
	this->JuliusThread = NULL;
	this->recogYobikake = NULL;
	this->jconfYobikake = NULL;
	this->recogWave = NULL;
	this->jconfWave = NULL;

	const string julius_type = MainWindow::m()->Config.Get("recong_type","");
	const int julius_gomi_y = MainWindow::m()->Config.GetInt("recong_julius_gomi_y",6);
	const int julius_gomi_e = MainWindow::m()->Config.GetInt("recong_julius_gomi_e",3);
	const int julius_gomi_d = MainWindow::m()->Config.GetInt("recong_julius_gomi_d",5);
	const int julius_filter1 = MainWindow::m()->Config.GetInt("recong_julius_filter1",45);
	const int julius_filter2 = MainWindow::m()->Config.GetInt("recong_julius_filter2",25);
//	const int julius_speechspeed = MainWindow::m()->Config.GetInt("recong_julius_speechspeed",13);
	const int julius_speechspeed = 13;
	if (julius_type == "julius_standalone_light")
	{//簡易
		this->RecongTypeFlg = 0;
	}
	else
	{//2pass
		this->RecongTypeFlg = RecongType_UseDictationFilter;
	}

	this->JuliusFilter1 = sizehosei((float)(julius_filter1) / 100.0f - 0.10f , 0.0f,1.0f);
	this->JuliusFilter2 = sizehosei((float)(julius_filter2) / 100.0f - 0.10f , 0.0f,1.0f);
	this->JuliusGomiY   = sizehosei(julius_gomi_y,1,7);
	this->JuliusGomiE   = sizehosei(julius_gomi_e,1,7);
	this->JuliusGomiD   = sizehosei(julius_gomi_d,1,7);

	this->JuliusSpeechSpeed = julius_speechspeed;
//	this->JuliusYobikakeSpeechSpeed = julius_speechspeed - 4;
	this->JuliusYobikakeSpeechSpeed = julius_speechspeed - 7; //英語は早口だから
	this->WaveFileDataLen = 0;

	struct _ref
	{
		static void logger(const char* msg)
		{
			sexylog::m()->Echo(msg);
		}
		static void OnOutputResultReport(unsigned int code)
		{
			if (code == 501)
			{//Warning: adin_alsa: no data fragment after 300 msec? が 連続した
				ERRORLOG("julius内でデータが受信できないエラーが発生しました。システムを再起動します。"
					<< "\t" << RecongLogResult_HardwareError
				);
				MainWindow::m()->Config.Set("is_adin_alsa_fragment_error","1");

				MainWindow::m()->Shutdown(EXITCODE_LEVEL_REBOOT , true);
				return ; //ここには到達できない
			}
			else if (code == 503)
			{//Warning: adin_alsa: no data fragment after 300 msec? が 連続した
				ERRORLOG("alsaを初期化できません。システムを再起動します。"
					<< "\t" << RecongLogResult_HardwareError
				);
				SecSleep(5);
				MainWindow::m()->Shutdown(EXITCODE_LEVEL_REBOOT , true);
				return ; //ここには到達できない
			}
			else if (code == 502)
			{//WARNING: adin_thread_process: too long input (> %d samples), segmented now
				//音が長すぎる
				NOTIFYLOG("周りが騒がしすぎます" 
					<< "\t" << RecongLogResult_SpliterError
				);
				return ;
			}
			else
			{//不明なレポート
				ERRORLOG("julius内で不明なエラー " << code << "が発生しました");
				NOTIFYLOG("" 
					<< "\t" << RecongLogResult_SoftwareError
				);
			}
		}
	};
	jlog_set_output_function(_ref::logger);
	jlogreport_set_output_function(_ref::OnOutputResultReport);
}

Recognition_JuliusPlusEnglish::~Recognition_JuliusPlusEnglish()
{
	JuliusStop();


}


void Recognition_JuliusPlusEnglish::OnStatusRecready(Recog *recog)
{
	//認識したwaveファイルを消去します。
	this->WaveFileDataLen = 0;
}

//認識結果の一部を waveで保存する.
void Recognition_JuliusPlusEnglish::WaveCutter(unsigned int all_frame,unsigned int startfream,unsigned int endfream,const string& filename) const
{
	if (startfream == 0 && endfream == 0)
	{//all
		endfream = this->WaveFileDataLen;
	}
	else
	{
		if (endfream < startfream)
		{
			return ;
		}
	}
	unsigned int allsize = this->WaveFileDataLen;
	unsigned int of_fream = allsize / all_frame;
	startfream *= of_fream;
	endfream *= of_fream;

	FILE *recfile_fp = wrwav_open( (char*) filename.c_str(), recog->jconf->input.sfreq);
	if(recfile_fp)
	{
		wrwav_data(recfile_fp,(SP16*)(this->WaveFileData + startfream), endfream - startfream);
		wrwav_close(recfile_fp);
	}
}

//認識結果の一部を抜け出す
void Recognition_JuliusPlusEnglish::WavePickup(unsigned int all_frame,unsigned int startfream,unsigned int endfream,const SP16** outStartMemory , int * outSize) const
{
	if (startfream == 0 && endfream == 0)
	{//all
		endfream = this->WaveFileDataLen;
	}
	else
	{
		if (endfream < startfream)
		{
			return ;
		}
	}
	unsigned int allsize = this->WaveFileDataLen;
	unsigned int of_fream = allsize / all_frame;
	startfream *= of_fream;
	endfream *= of_fream;

	*outStartMemory = this->WaveFileData + startfream;
	*outSize        = (endfream - startfream) * sizeof(SP16);
}

//認識結果の一部を抜け出す
void Recognition_JuliusPlusEnglish::MFCCPickup(unsigned int all_frame,unsigned int startfream,unsigned int endfream, HTK_Param* param, VECT*** outVect , int * outNum) const
{
	if (startfream == 0 && endfream == 0)
	{//all
		endfream = this->WaveFileDataLen;
	}
	else
	{
		if (endfream < startfream)
		{
			return ;
		}
	}
	unsigned int allsize = this->WaveFileDataLen;
	unsigned int of_fream = allsize / all_frame;
	startfream *= of_fream;
	endfream *= of_fream;
	const int samplenumSecond =  param->header.samplenum; //DEBUG

	*outVect = param->parvec + startfream;
	*outNum  = (endfream - startfream);
}


//呼びかけがあれば、waveで保存し、もう一つのjuliusインスタンスで再チェックする.
bool Recognition_JuliusPlusEnglish::checkDictation(unsigned int all_frame,unsigned int startfream,unsigned int endfream) const
{
	j_recognize_mfcc_yobikake(this->recogYobikake,this->recog->process_list->lm->am->mfcc->param,startfream,endfream);
//	j_recognize_mfcc_yobikake(this->recogYobikake,this->recog->process_list->lm->am->mfcc->param,0,endfream);

	//内容のチェック
	if ( this->DictationCheckResult <= 0)
	{
		return false;
	}
	return true;
}



//認識waveファイルの断片が入ります。
void Recognition_JuliusPlusEnglish::OnRecordAdinTrigger(Recog *recog, SP16 *speech, int samplenum)
{
	//Waveの断片をつなぎあわせます。
	unsigned int copysize = samplenum;
	if (this->WaveFileDataLen + copysize > MAX_SOUND_BUFFER )
	{
		copysize = MAX_SOUND_BUFFER - this->WaveFileDataLen;
	}
	memcpy(this->WaveFileData+this->WaveFileDataLen , speech , copysize*sizeof(SP16));
	this->WaveFileDataLen += copysize;
}

//読みを取得する
string Recognition_JuliusPlusEnglish::ConvertJuliusYomi(const WORD_INFO * wordinfo,int index) const
{
	const int maxJ = wordinfo->wlen[index];

	string yomi;
	yomi.reserve(256);

	char buf[64];
	for (int j=0;j<maxJ;j++) 
	{
		yomi += center_name( (wordinfo->wseq[index][j]->name),buf);
	}
	return yomi;
}

//連続した読みを取得する
string Recognition_JuliusPlusEnglish::ConvertJuliusYomiJoin(const WORD_ID* seq,int seqIndex,const WORD_INFO * wordinfo,const LOGPROB* confidence,LOGPROB* avgConfidence) const
{
	string yomi;
	yomi.reserve(256);

	//平均確率
	int count = 0;
	LOGPROB sum = 0;

	for(int i = seqIndex ; true ; i++ )
	{
		int index = seq[i];
		if (   wordinfo->woutput[index][0] == '<')
		{
			break;
		}


		sum += confidence[i];
		count ++;

		const int maxJ = wordinfo->wlen[index];

		yomi += "@" ;

		char buf[64];
		for (int j=0;j<maxJ;j++) 
		{
			yomi += center_name( (wordinfo->wseq[index][j]->name),buf);
		}
	}

	if(count <= 0)
	{
		*avgConfidence = 0;
	}
	else
	{
		*avgConfidence = sum / count;
	}

	if(yomi.empty())
	{
		return "";
	}
	return yomi.substr(1);
}

//フレームの取得
bool Recognition_JuliusPlusEnglish::getRecongFrame3(const SentenceAlign * align , int i_seq , unsigned int* retBeginFrame , unsigned int* retEndFrame, unsigned int* retEndFrame2) const
{
	//呼びかけの開始フレームと終了フレームを取得します。
	for ( ; align; align = align->next)
	{
		if (align->unittype != PER_WORD)
		{//-walign 以外無視
			continue;
		}
		int ii=0;
		for(;ii<align->num;ii++) 
		{
			if (align->w[ii] != i_seq)
			{//現在単語ノードi と同じものじゃないと見ない
				continue;
			}
			//開始フレーム
			*retBeginFrame = align->begin_frame[ii];
			//終了フレーム
			*retEndFrame = align->end_frame[ii];

			//終了フレーム
			*retEndFrame2 = align->end_frame[align->num - 1];
			return true;
		}
	}
	return false;
}


//あまり行儀が良くないが、音声認識パラメーターでログに書くものが長くなるので、まとめとく。
#define BASIC_VALMENU \
   " match:(" << yobikakeYomi << " " << elecYomi << ") "  \
<< " samplenum:" << samplenum << " basesize:" << basesize << " sec:" << WaveFileDataLen / 16000 \
<< " startfream:" <<  startfream << " endfream:" << endfream << " endfreamElec:" <<endfreamElec << " elecyohaku:" << elecyohaku \
<< " ashikiriC:" << ashikiriConfidenceYobikake << " ashikiriEC:" << ashikiriConfidenceElec << " cmscore0:" << s->confidence[seqIndex+0] << " cmscore1:" << avgElecConfidence \
<< " よびかけサイズ(" << yobikakeSampleNum << ") 期待値(" << yobikakeBasesize / 2 - (yobikakeBasesize / 8)<< "  "  << yobikakeBasesize + (yobikakeBasesize/4) << ")" \
<< " elecサイズ(" << elecSampleNum << ")  期待値(" << elecBasesize / 2 - (elecBasesize / 2) << "  " << elecBasesize + (elecBasesize/2) << ")" \
<< " sumSampleNum:" << sumSampleNum << " 期待値(" << sumbasesize / 2 - (sumbasesize / 4) << " " << sumbasesize + (sumbasesize / 8) << ")" \
<< " baseSampleRatio:" << baseSampleRatio << "  期待値(" << (baseRatio - 0.30) << "<" << baseRatio << "<" << (baseRatio + 0.55) << " )" \
<< " yobikakebaseSampleRatio:" << yobikakebaseSampleRatio << "  期待値(" << (yobikakebaseRatio - 0.30) << "<" << yobikakebaseRatio << "<" << (yobikakebaseRatio + 0.55) << " )" \
<< " elecbaseSampleRatio:" << elecbaseSampleRatio << " 期待値(" << (elecbaseRatio - 0.30) << "<" << elecbaseRatio << "<" << (elecbaseRatio + 0.55) << " )" \
""


//認識を行います。
void Recognition_JuliusPlusEnglish::OnOutputResult(Recog *recog) 
{
	const RecogProcess* r = recog->process_list;

	// output results for all the obtained sentences
	const auto winfo = r->lm->winfo;
	for(auto n = 0; n < r->result.sentnum; n++) 
	{ // for all sentences
		if (r->result.sent == NULL)  continue;
		const Sentence* s = &(r->result.sent[n]);
		if (s == NULL) continue;
		const WORD_ID* seq = s->word;
		const int seqnum = s->word_num;
		unsigned int seqIndex = 0;

		const int seq_0 = seq[0];
		if (   winfo->woutput[seq_0][0] == '<'
			&& winfo->woutput[seq_0][1] == 's' )
		{
			seqIndex++;
		}

		const int seq_i = seq[seqIndex];

		//呼びかけの読みを取る
		const string yobikakeYomi = ConvertJuliusYomi(winfo,seq_i);
		//呼びかけの次の命令ノードの値を見てみる
		const int elec_seq_i = seq[seqIndex+1];


		if (   winfo->woutput[seq_i][0] == '<'
			&& winfo->woutput[seq_i][1] == '/' )
		{
			NOTIFYLOG("呼びかけが終端コード" 
//					<< " file:" << wav_to_file_full("FB1",20) 
				<< "\t" << RecongLogResult_SMatchError
			);
			return;
		}

		
		if (   winfo->woutput[elec_seq_i][0] == '<'
			&& winfo->woutput[elec_seq_i][1] == '/' )
		{
			NOTIFYLOG("命令が終端コード" 
//					<< " file:" << wav_to_file_full("FB1",20) 
				<< "\t" << RecongLogResult_SMatchError
			);
			return;
		}

		LOGPROB avgElecConfidence;
		const string elecYomi     = ConvertJuliusYomiJoin(seq,seqIndex+1,winfo,s->confidence,&avgElecConfidence);

		//認識した読みとデータ長さで許可スべき確率を求める
		//juliusで誤認識するときは、ムダに長いワードを拾っている傾向があるため
		//認識した文字列を13倍したぐらいのデータがだいたいちょうどいいらしい。
		const unsigned int yobikakeYomiWordSize = JuliusOnsoUtil::CountLength(yobikakeYomi);
		const unsigned int elecYomiWordSize = JuliusOnsoUtil::CountLength(elecYomi);
		const unsigned int samplenum = recog->mfcclist->param->samplenum;
		const unsigned int basesize = (yobikakeYomiWordSize+elecYomiWordSize ) * this->JuliusSpeechSpeed; //13;
		const unsigned int basesizeparts = basesize / 4;
		float ashikiriConfidenceYobikake = 0;
		float ashikiriConfidenceElec = 0;

		//呼びかけのサイズをつい検証する
		unsigned int startfream ;
		unsigned int endfream ;
		unsigned int endfreamElec ;
		if (! getRecongFrame3(s->align , seq_i , &startfream , &endfream,  &endfreamElec) )
		{
			NOTIFYLOG("フレームが取得できない" 
//					<< " file:" << wav_to_file_full("FB1",20) 
				<< "\t" << RecongLogResult_SoftwareError
			);
			return;
		}

		//認識した文字列をN倍したぐらいのデータがだいたいちょうどいいらしい。
		const unsigned int yobikakeBasesize = (yobikakeYomiWordSize) * this->JuliusYobikakeSpeechSpeed; //9;
		const unsigned int yobikakeSampleNum = (endfream - startfream);
		const unsigned int elecBasesize = (elecYomiWordSize) * this->JuliusSpeechSpeed; //13;
		const unsigned int elecSampleNum = (endfreamElec - endfream);
		const unsigned int sumbasesize = yobikakeBasesize+elecBasesize;
		const unsigned int sumSampleNum = (endfreamElec - startfream);

		//音のバランスの計算
		const float baseRatio = yobikakeBasesize / (float)elecBasesize;
		const float yobikakebaseRatio = yobikakeBasesize / (float)basesize;
		const float elecbaseRatio = elecBasesize / (float)basesize;
		const float baseSampleRatio = yobikakeSampleNum / (float)elecSampleNum;
		const float yobikakebaseSampleRatio = yobikakeSampleNum / (float)samplenum;
		const float elecbaseSampleRatio = elecSampleNum / (float)samplenum;
		const unsigned int elecyohaku = (samplenum - endfreamElec);

		//ゴミノードの場合は相手をしない
		if (   winfo->woutput[seq_i][0] == 'g' 
			&& winfo->woutput[seq_i][1] == 'o' 
			&& winfo->woutput[seq_i][2] == 'm' 
			&& winfo->woutput[seq_i][3] == 'i' )
		{
			NOTIFYLOG("呼びかけがgomi" 
				<< BASIC_VALMENU
				<< " file:" << wav_to_file_full("FB1",10) 
				<< "\t" << RecongLogResult_YGomiMatchError
			);
			return;
		}

		
		if (   winfo->woutput[elec_seq_i][0] == 'g' 
			&& winfo->woutput[elec_seq_i][1] == 'o' 
			&& winfo->woutput[elec_seq_i][2] == 'm' 
			&& winfo->woutput[elec_seq_i][3] == 'i' )
		{
			NOTIFYLOG("命令がgomi" 
				<< BASIC_VALMENU
				<< " file:" << wav_to_file_full("FB1",10) 
				<< "\t" << RecongLogResult_EGomiMatchError
			);
			return;
		}

		//長さを見る
		if (samplenum <= basesize / 2 - (basesizeparts/8))
		{
			NOTIFYLOG("音声データが短すぎます" 
				<< BASIC_VALMENU
				<< " file:" << wav_to_file_full("FB5",20) 
				<< "\t" << RecongLogResult_ShortSpeechError
			);
			return;
		}
		else if (samplenum <= basesize + basesizeparts*2)
		{
			ashikiriConfidenceYobikake = 0;
			ashikiriConfidenceElec = this->JuliusFilter1; //0.30;
		}
		else
		{
			NOTIFYLOG("音声データが長すぎます" 
				<< BASIC_VALMENU
				<< " file:" << wav_to_file_full("FB5",20) 
				<< "\t" << RecongLogResult_LongSpeechError
			);
			return;
		}
/*
		//確率の検証
		if (s->confidence[seqIndex+0] <= ashikiriConfidenceYobikake)
		{
			NOTIFYLOG("呼びかけが足切り点以下" 
				<< BASIC_VALMENU
				<< " file:" << wav_to_file_full("FB6",20) 
				<< "\t" << RecongLogResult_ProbabilityError
			);
			return;
		}
		if (avgElecConfidence <= ashikiriConfidenceElec)
		{
			NOTIFYLOG("命令が足切り点以下" 
				<< BASIC_VALMENU
				<< " file:" << wav_to_file_full("FB6",20) 
				<< "\t" << RecongLogResult_ProbabilityError
			);
			return;
		}
*/

		if (yobikakeSampleNum <= yobikakeBasesize / 2 - (yobikakeBasesize / 8) )
		{//ダメ
			MainWindow::m()->Recognition.PlayRecongNGSound();
			NOTIFYLOG("呼びかけのサイズが期待値以下" 
				<< BASIC_VALMENU
				<< " file:" << wav_to_file_full("FB11",20) 
				<< "\t" << RecongLogResult_ShortSpeechError
			);
			return;
		}
		else if (yobikakeSampleNum <= yobikakeBasesize + (yobikakeBasesize/4) )
		{//OKとする
			//ベストチョイス
		}
		else 
		{//ダメ
			MainWindow::m()->Recognition.PlayRecongNGSound();
			NOTIFYLOG("呼びかけのサイズが期待値以上" 
				<< BASIC_VALMENU
				<< " file:" << wav_to_file_full("FB11",20) 
				<< "\t" << RecongLogResult_LongSpeechError
			);
			return;
		}

		if (elecSampleNum <= elecBasesize / 2 - (elecBasesize / 2) )
		{//ダメ
			MainWindow::m()->Recognition.PlayRecongNGSound();
			NOTIFYLOG("elecサイズが期待値以下" 
				<< BASIC_VALMENU
				<< " file:" << wav_to_file_full("FB12",20) 
				<< "\t" << RecongLogResult_ShortSpeechError
			);
			return;
		}
		else if (elecSampleNum <= elecBasesize + (elecBasesize/2) )
		{//OKとする
			//ベストチョイス
		}
		else 
		{//ダメ
			MainWindow::m()->Recognition.PlayRecongNGSound();
			NOTIFYLOG("elecサイズが期待値以上" 
				<< BASIC_VALMENU
				<< " file:" << wav_to_file_full("FB12",20) 
				<< "\t" << RecongLogResult_LongSpeechError
			);
			return;
		}
		
		if (sumSampleNum <= sumbasesize / 2 - (sumbasesize / 4) )
		{//ダメ
			MainWindow::m()->Recognition.PlayRecongNGSound();
			NOTIFYLOG("sumサイズが期待値以下" 
				<< BASIC_VALMENU
				<< " file:" << wav_to_file_full("FB12",20) 
				<< "\t" << RecongLogResult_ShortSpeechError
			);
			return;
		}
		else if (sumSampleNum <= sumbasesize + (sumbasesize / 4))
		{//OKとする
			//ベストチョイス
		}
		else 
		{//ダメ
			MainWindow::m()->Recognition.PlayRecongNGSound();
			NOTIFYLOG("sumサイズが期待値以上" 
				<< BASIC_VALMENU
				<< " file:" << wav_to_file_full("FB12",20) 
				<< "\t" << RecongLogResult_LongSpeechError
			);
			return;
		}

		//バランスを見る
		if (! (baseSampleRatio >= baseRatio - 0.40 && baseSampleRatio <= baseRatio + 0.55) )
		{
			MainWindow::m()->Recognition.PlayRecongNGSound();
			NOTIFYLOG("baseSampleRatioが期待の範囲にありません" 
				<< BASIC_VALMENU
				<< " file:" << wav_to_file_full("FB13",20) 
				<< "\t" << RecongLogResult_FuseikakuError
			);
			return ;
		}
		if (! (yobikakebaseSampleRatio >= yobikakebaseRatio - 0.40 && yobikakebaseSampleRatio <= yobikakebaseRatio + 0.55) )
		{
			MainWindow::m()->Recognition.PlayRecongNGSound();
			NOTIFYLOG("yobikakebaseSampleRatioが期待の範囲にありません" 
				<< BASIC_VALMENU
				<< " file:" << wav_to_file_full("FB13",20) 
				<< "\t" << RecongLogResult_FuseikakuError
			);
			return ;
		}
		if (! (elecbaseSampleRatio >= elecbaseRatio - 0.40 && elecbaseSampleRatio <= elecbaseRatio + 0.55) )
		{
			MainWindow::m()->Recognition.PlayRecongNGSound();
			NOTIFYLOG("elecbaseSampleRatioが期待の範囲にありません" 
				<< BASIC_VALMENU
				<< " file:" << wav_to_file_full("FB13",20) 
				<< "\t" << RecongLogResult_FuseikakuError
			);
			return ;
		}

		//余白を見る
//		if (elecyohaku < 10) //なぜか英語だと余白がすくなくなるらしい.
		if (elecyohaku < 1)
		{
			MainWindow::m()->Recognition.PlayRecongNGSound();
			NOTIFYLOG("命令終了後の余白が小さすぎます" 
				<< BASIC_VALMENU
				<< " file:" << wav_to_file_full("FB14",20) 
				<< "\t" << RecongLogResult_YohakuError
			);
			return ;
		}

		if ( this->RecongTypeFlg & RecongType_UseDictationFilter )
		{
			//検出した呼びかけをもう一度再検証する。
			if ( ! checkDictation(r->result.num_frame,startfream,endfream) )
			{
				MainWindow::m()->Recognition.PlayRecongNGSound();

				const string errorflag = 
					  (this->DictationCheckResult == -2 ? RecongLogResult_DictationFilterProbError   
					: (this->DictationCheckResult == -3 ? RecongLogResult_DictationFilterLongError   
					: (this->DictationCheckResult == -4 ? RecongLogResult_DictationFilterShortError  
					: RecongLogResult_DictationFilterGomiError
					)));

				NOTIFYLOG("誤認識、ディクテーションフィルター、呼びかけチェックの結果エラー" 
					<< " " << this->DictationCheckLogMessage 
					<< BASIC_VALMENU
					<< " file:" << wav_to_file_full("FB99",50) 
					<< "\t" << errorflag
				);
//				wav_to_file("SB99",r->result.num_frame,startfream,endfream);
				return;
			}
		}
		else
		{
			this->DictationCheckLogMessage.clear();
		}
		//成功した時の音を鳴らす
		MainWindow::m()->Recognition.PlayRecongOKSound();

		//コールバックを取得する
		CallbackPP callback;
		if (! getCallback(elecYomi,&callback) )
		{
			MainWindow::m()->Recognition.PlayRecongNGSound();
			NOTIFYLOG("コールバックを取得できません" 
				<< " " << this->DictationCheckLogMessage 
				<< BASIC_VALMENU
				<< " file:" << wav_to_file_full("FB0",20) 
				<< "\t" << RecongLogResult_SoftwareError
			);
			return ;
		}

		//マッチしたのでコールバックする
		MainWindow::m()->ScriptManager.VoiceRecogntion(callback);
		MainWindow::m()->PopupMessage("音声認識",elecYomi );
		NOTIFYLOG("音声認識完了しました。 マッチ!!!!!!!!!!!!!" 
			<< " " << this->DictationCheckLogMessage 
			<< BASIC_VALMENU
			<< " file:" << wav_to_file_full("FX",50) 
			<< "\t" << RecongLogResult_Match
		);
//		wav_to_file("SX",r->result.num_frame,startfream,endfream);
		return ;
	}
}





void Recognition_JuliusPlusEnglish::MakeMicrophoneJuliusRule(const list<RecongTask>& allCommandRecongTask)
{
	//マイクから入力用
	ofstream dfa(MainWindow::m()->GetConfigBasePath("/julius/__temp__regexp_test.dfa"));
	ofstream dict(MainWindow::m()->GetConfigBasePath("/julius/__temp__regexp_test.dict"));

	int yobikakeRule = 4;
	int elecRule = 5;
	int gomiDict = 2;
	int gomiPPDict = 3;
	int yobikakeDict = 4;
	int elecDict = 5;
	int endRule = 0;
	int startRule = 1;
	dfa << endRule << " " << 0 << " " << elecRule << " " << 0 << " " << 1 << endl;
	dict << 0 << "\t" << "@-2" << "\tN\t" << "[</s>]" << "\t" << " " << "sil" << endl;

	dfa << startRule << " " << 1 << " " << 2 << " " << 1 << " " << 1 << endl;
	dict << 1 << "\t"  << "@-2" << "\tN\t" << "[<s>]" << "\t" << " " << "sil" << endl;
	dfa << 2 << " " << -1 << " " << -1 << " " << 1 << " " << 0 << endl;

	dfa << elecRule << " " << gomiPPDict << " " << yobikakeRule << " 0 0" << endl;
	dfa << elecRule << " " << elecDict   << " " << yobikakeRule << " 0 0" << endl;

	dfa << yobikakeRule << " " << gomiDict     << " " << startRule << " 0 0" << endl;
	dfa << yobikakeRule << " " << yobikakeDict << " " << startRule << " 0 0" << endl;

	//ゴミデータを作ります
	{
		//2文字列を使って、あわなさそうな音をチョイス
		const char* akasatanaNMapping[] = {"あ","か","さ","た","な","は","ま","ら","が","ざ","だ","ば","ぱ","や","る","い","ね","け",NULL};
		const char* akasatanaAMapping[] = {"あ","か","さ","た","な","は","ま","ら","が","ざ","だ","ば","ぱ","ん","や","る","う","お","き",NULL};
		const char* akasatanaIMapping[] = {"い","き","し","ち","に","ひ","み","り","ぎ","じ","ぢ","び","ぴ","ん","れ","え","あ",NULL};
		const char* akasatanaUMapping[] = {"う","く","す","つ","ぬ","ふ","む","る","ぐ","ず","づ","ぶ","ぷ","ん","ゆ","れ","え","い",NULL};
		const char* akasatanaEMapping[] = {"え","け","せ","て","ね","へ","め","れ","げ","ぜ","で","べ","ぺ","ん","ら","あ","う",NULL};
		const char* akasatanaOMapping[] = {"お","こ","そ","と","の","ほ","も","ろ","ご","ぞ","ど","ぼ","ぽ","ん","よ","り","い","あ",NULL};

		//2文字目で検索します
		const string searchBoin = JuliusOnsoUtil::YomiToFirstBoin(this->YobikakeSecondChar
			,JuliusOnsoUtil::isBoinEnglish);

		const char** useMapping  = NULL;
		if (searchBoin == "a")
		{
			useMapping = akasatanaEMapping;
		}
		else if (searchBoin == "i")
		{
			useMapping = akasatanaEMapping;
		}
		else if (searchBoin == "u")
		{
			useMapping = akasatanaIMapping;
		}
		else if (searchBoin == "e")
		{
			useMapping = akasatanaAMapping;
		}
		else if (searchBoin == "o")
		{
			useMapping = akasatanaUMapping;
		}
		else
		{//n m
			useMapping = akasatanaNMapping;
		}
		
		if  (this->JuliusGomiY >= 2)
		{
			for(const char** t = useMapping ; *t ; t++ )
			{
				const string gomi_yomi = ConvertYomiNoJapanesWakachigaki(this->YobikakeFirstKanaChar + *t);

//				dict << gomiDict	<< "\t"  << "@-2" << "\tN\t" << "[gomi]" << "\t" << gomi_yomi << endl;

				if (this->JuliusGomiY >= 7)
				{
				}
				else if (this->JuliusGomiY == 6)
				{
					t++; if (!*t) break;
				}
				else if (this->JuliusGomiY == 5)
				{
					t++; if (!*t) break;
					t++; if (!*t) break;
				}
				else if (this->JuliusGomiY == 4)
				{
					t++; if (!*t) break;
					t++; if (!*t) break;
					t++; if (!*t) break;
				}
				else if (this->JuliusGomiY == 3)
				{
					t++; if (!*t) break;
					t++; if (!*t) break;
					t++; if (!*t) break;
					t++; if (!*t) break;
				}
				else if (this->JuliusGomiY == 2)
				{
					t++; if (!*t) break;
					t++; if (!*t) break;
					t++; if (!*t) break;
					t++; if (!*t) break;
					t++; if (!*t) break;
				}
			}
				
		}


		//家電のところにノイズを入れる
		const string useGomiNodeHintFilenameE = MainWindow::m()->GetConfigBasePath("/julius/eng_recong_julius_gomi_e" + num2str(this->JuliusGomiE) + ".txt");
		const auto filterVec = XLStringUtil::split_vector("\n", XLFileUtil::cat(useGomiNodeHintFilenameE) );
		for(auto fit = filterVec.begin() ; fit != filterVec.end() ; ++fit )
		{
			const string yomi = XLStringUtil::chop(*fit);
			const string yomiBoin = JuliusOnsoUtil::YomiToFirstBoin(yomi
				,JuliusOnsoUtil::isBoinEnglish);
			if (yomi.empty())
			{
				continue;
			}

			dict << gomiPPDict << "\t" << "@-2" << "\tN\t" << "[gomi]" << "\t"	<< XLStringUtil::chop(*fit) << endl;
		}
	}
	//呼びかけのdictを作る
	for( auto yit = this->YobikakeListArray.begin(); this->YobikakeListArray.end() != yit ; ++yit)
	{
		const string yomi = *yit;
		dict << yobikakeDict << "\t" << "@-1" << "\tN\t" << "[" << yomi << "]" << "\t" << yomi << endl;
	}

	map<string,int> dictCache;

	//命令のdictを作る
	int nowRule = elecRule + 1;
	int nowDict = elecDict ;
	for( auto acit = allCommandRecongTask.begin(); allCommandRecongTask.end() != acit ; ++acit)
	{
		const string yomi = acit->yomiString;
		auto acsp = XLStringUtil::split(" @ ",yomi);

		int beforeRule = yobikakeRule;
		for(auto acspit = acsp.begin() ; acspit != acsp.end() ; ++acspit )
		{
			int useDict;
			auto dictCacheIT = dictCache.find(*acspit);
			if (dictCacheIT == dictCache.end() )
			{
				useDict = ++nowDict;
				dict << useDict << "\t" << "@-1" << "\tN\t" << "[" << *acspit << "]" << "\t" << *acspit << endl;
				dictCache[*acspit] = useDict;
			}
			else
			{
				useDict = dictCacheIT->second;
			}
			dfa << nowRule << " " << useDict << " " << beforeRule << " " << 0 << " " << 0 << endl;
			beforeRule = nowRule;
			nowRule++;
		}
		dfa << 0 << " " << 0 << " " << beforeRule << " 0 1" << endl;
	}
}

void Recognition_JuliusPlusEnglish::MakeFileJuliusRule()
{
	//ディクテーションフィルター用
	ofstream dfa(MainWindow::m()->GetConfigBasePath("/julius/__temp__regexp_test_file.dfa"));
	ofstream dict(MainWindow::m()->GetConfigBasePath("/julius/__temp__regexp_test_file.dict"));

	int yobikakeRule = 4;
	int gomiDict = 2;
	int yobikakeDict = 4;
	int endRule = 0;
	int startRule = 1;
	dfa << endRule << " " << 0 << " " << yobikakeRule << " " << 0 << " " << 1 << endl;
	dict << 0 << "\t" << "@-2" << "\tN\t" << "[</s>]" << "\t" << " " << "sil" << endl;

	dfa << startRule << " " << 1 << " " << 2 << " " << 1 << " " << 1 << endl;
	dict << 1 << "\t" << "@-2" << "\tN\t" << "[<s>]" << "\t" << " " << "sil" << endl;

	dfa << 2 << " " << -1 << " " << -1 << " " << 1 << " " << 0 << endl;

	dfa << yobikakeRule << " " << gomiDict     << " " << startRule << " 0 0" << endl;
	dfa << yobikakeRule << " " << yobikakeDict << " " << startRule << " 0 0" << endl;

	//呼びかけのdictを作る
	for( auto yit = this->YobikakeListArray.begin(); this->YobikakeListArray.end() != yit ; ++yit)
	{
		const string yomi = *yit;
		dict << yobikakeDict << "\t" << "@-1" << "\tN\t" << "[" << yomi << "]" << "\t" << yomi << endl;
	}


	const auto gomiYomiFirst = this->YobikakeFirstChar;
	const auto gomiYomiSecond = this->YobikakeSecondChar;
	const auto gomiBoinFirst = JuliusOnsoUtil::YomiToFirstBoin(gomiYomiFirst
			,JuliusOnsoUtil::isBoinEnglish);
	const auto gomiBoinSecond = JuliusOnsoUtil::YomiToFirstBoin(gomiYomiSecond
			,JuliusOnsoUtil::isBoinEnglish);
	//ゴミノード用のファイル
	string useGomiNodeHintFilename;

	//ゴミノード用のファイル
	if (this->JuliusGomiD <= 3)
	{
		useGomiNodeHintFilename = MainWindow::m()->GetConfigBasePath("/julius/eng_recong_julius_gomi_d" + num2str(this->JuliusGomiD+3) + ".txt");
	}
	else
	{
		useGomiNodeHintFilename = MainWindow::m()->GetConfigBasePath("/julius/eng_recong_julius_gomi_d" + num2str(this->JuliusGomiD) + ".txt");
	}
	const auto filterVec = XLStringUtil::split_vector("\n", XLFileUtil::cat(useGomiNodeHintFilename) );
	for(auto fit = filterVec.begin() ; fit != filterVec.end() ; ++fit )
	{
		const string yomi = XLStringUtil::chop(*fit);
		const string yomiBoin = JuliusOnsoUtil::YomiToFirstBoin(yomi
			,JuliusOnsoUtil::isBoinEnglish);
		if (yomi.empty())
		{
			continue;
		}
		if (this->JuliusGomiD <= 5)
		{
			if (yomi.find(gomiYomiFirst) == 0)
			{//「こ」んぴゅーた 同じ音が先頭に入ってはいけない
				continue;
			}
			if (yomi.find(gomiYomiSecond) == 0)
			{//こ「ん」ぴゅーた 同じ音が先頭に入ってはいけない
				continue;
			}
			if (yomiBoin == gomiBoinFirst)
			{//「こ」んぴゅーた 同じ母音が先頭に入ってはいけない
				continue;
			}
			if (yomiBoin == gomiBoinSecond)
			{//こ「ん」ぴゅーた 同じ母音が先頭に入ってはいけない
				continue;
			}
		}
		dict << gomiDict << "\t" << "@-2" << "\tN\t" << "[gomi]" << "\t"	<< XLStringUtil::chop(*fit) << endl;
	}
}

//コールバックとキャプチャを取得する
bool Recognition_JuliusPlusEnglish::getCallback(const string& elecYomi,CallbackPP* callback) const
{
	for(auto it = AllCommandRecongTask.begin() ; it != AllCommandRecongTask.end() ; ++it )
	{
		if (it->yomiNoSpaceString == elecYomi)
		{
			*callback = it->callback;
//			*capture = it->orignalString;
			return true;
		}
	}
	return false;
}




bool Recognition_JuliusPlusEnglish::Create()
{
	DEBUGLOG("音声認識エンジンの構築完了");
	return true;
}

//呼びかけを設定します。
//設定したあと、 CommitRule() てしてね。
bool Recognition_JuliusPlusEnglish::SetYobikake(const list<string> & yobikakeList)
{
	this->YobikakeListArray.clear();
	this->YobikakeFirstKanaChar = "";
	this->YobikakeSecondKanaChar = "";
	this->YobikakeFirstChar = "";
	this->YobikakeSecondChar = "";

	for(auto it = yobikakeList.begin();  yobikakeList.end() != it ; ++it)
	{
		if (it->empty() ) continue;
		string yobikakeYomi = ConvertYomi(*it);
		if (yobikakeYomi.empty() ) continue;

		this->YobikakeListArray.push_back( yobikakeYomi );
		if (this->YobikakeFirstKanaChar.empty())
		{
			string japanese = JuliusOnsoUtil::EnglishOnsoToJapanese(yobikakeYomi,false);
			this->YobikakeFirstKanaChar = XLStringUtil::mb_substr(japanese,0,1);
			this->YobikakeSecondKanaChar = XLStringUtil::mb_substr(japanese,1,1);
			
			//最初のいち文字目を取得します。
			//誤認識フィルタを作るときに使います。
			this->YobikakeFirstChar = JuliusOnsoUtil::PickupOnso(yobikakeYomi,0
				,JuliusOnsoUtil::isBoinEnglish);
			this->YobikakeSecondChar = JuliusOnsoUtil::PickupOnso(yobikakeYomi,1
				,JuliusOnsoUtil::isBoinEnglish);
		}
	}
	if (this->YobikakeListArray.empty() )
	{
		return false;
	}
	
	return true;
}

//構築したルールを音声認識エンジンにコミットします。
bool Recognition_JuliusPlusEnglish::CommitRule(const list<RecongTask>* allCommandRecongTask)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	DEBUGLOG("juliusを停止します");
	this->JuliusStop();

	this->AllCommandRecongTask = *allCommandRecongTask;

	//マイクから入力用
	MakeMicrophoneJuliusRule(*allCommandRecongTask);

	//ディクテーションフィルター用
	MakeFileJuliusRule();

	try
	{
		DEBUGLOG("juliusを構築");
		this->JuliusYobikakeStart();
		this->JuliusStart();
		this->JuliusWaveStart();
	}
	catch(XLException& e )
	{
		ERRORLOG("juliusを構築できませんでした。" << e.what() );
		throw e;
	}



	return true;
}


void Recognition_JuliusPlusEnglish::JuliusStop()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	if (this->recog)
	{
		//たまに、ストリーム閉じても、コールバックの中で迷子になることがあるので、
		//強制的にコールバックテーブルを初期化する!!
		callback_init(this->recog);
		//ストリームを閉じる
		j_close_stream(this->recog);

		//スレッド停止までまつ
		this->JuliusThread->join();

		delete this->JuliusThread;
		this->JuliusThread = NULL;

		if (this->jconf)
		{
			//これで開放すると、 j_recog_free で落ちる・・・
			//			j_jconf_free(this->jconf);
			this->jconf = NULL;
		}
		//メモリ開放
		j_recog_free(this->recog);
		this->recog = NULL;
	}

	if (this->recogYobikake)
	{
		if (this->jconfYobikake)
		{
			//これで開放すると、 j_recog_free で落ちる・・・
			//			j_jconf_free(this->jconfYobikake);
			this->jconfYobikake = NULL;
		}

		j_recog_free(this->recogYobikake);
		this->recogYobikake = NULL;
	}
	if (this->recogWave)
	{
		if (this->jconfWave)
		{
			//これで開放すると、 j_recog_free で落ちる・・・
			//			j_jconf_free(this->jconfWave);
			this->jconfWave = NULL;
		}

		j_recog_free(this->recogWave);
		this->recogWave = NULL;
	}
}

bool Recognition_JuliusPlusEnglish::JuliusStart()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	assert(this->recog == NULL);
	assert(this->jconf == NULL);
	assert(this->JuliusThread == NULL);

	const string testmic_jconf = MainWindow::m()->GetConfigBasePath("/julius/engmic.jconf");
	const char* argv[]={
		"juliusplus"
		,"-C"
		,testmic_jconf.c_str()
	};
	int argc = sizeof(argv)/sizeof(argv[0]);
	int ret;

	//julusはC関数なので、const外して char** にするしかない・・・
	this->jconf = j_config_load_args_new(argc, (char**)argv);
	/* else, you can load configurations from a jconf file */
	//jconf = j_config_load_file_new(jconf_filename);
	if (this->jconf == NULL) 
	{
		throw XLEXCEPTION("Try `-help' for more information.\n");
	}

	/* 2. create recognition instance according to the jconf */
	/* it loads models, setup final parameters, build lexicon
	and set up work area for recognition */
	this->recog = j_create_instance_from_jconf(this->jconf);
	if (this->recog == NULL)
	{
		throw XLEXCEPTION("Error in startup(j_create_instance_from_jconf)\n");
	}
	struct _ref{
		static void status_recready(Recog *recog, void *_this)
		{
			((Recognition_JuliusPlusEnglish*)_this)->OnStatusRecready(recog);
		}
		static void output_result(Recog *recog, void *_this)
		{
			((Recognition_JuliusPlusEnglish*)_this)->OnOutputResult(recog);
		}
		static void error_exit(Recog *recog, void *_this)
		{
			ERRORLOG("julius内エラーのため再起動します。"
				<< "\t" << RecongLogResult_HardwareError
			);

			MainWindow::m()->Shutdown(EXITCODE_LEVEL_ERROR , true);
		}
		static void record_adin_trigger(Recog *recog, SP16 *speech, int samplenum, void *_this)
		{
			((Recognition_JuliusPlusEnglish*)_this)->OnRecordAdinTrigger(recog,speech,samplenum);
		}
	};
	callback_add(this->recog, CALLBACK_EVENT_SPEECH_READY, _ref::status_recready, this);
	callback_add(this->recog, CALLBACK_RESULT, _ref::output_result, this);
	
	callback_add(this->recog, CALLBACK_ERROR_EXIT, _ref::error_exit, this);
	callback_add_adin(this->recog, CALLBACK_ADIN_TRIGGERED, _ref::record_adin_trigger, this);

	// Initialize audio input
	if (j_adin_init(this->recog) == FALSE) 
	{
		throw XLEXCEPTION("Error in startup(j_adin_init)\n");
	}
	// update initial recognition process status
	for(auto r=this->recog->process_list;r;r=r->next) {
		r->live = TRUE;
		r->active = 0;
	}

	// check for grammar to change, and rebuild if necessary
	for(auto lm=this->recog->lmlist;lm;lm=lm->next) {
		if (lm->lmtype == LM_DFA) {
			multigram_update(lm); // some modification occured if return TRUE
		}
	}

	//区間切り出しフィルター
	int julius_lv_base = MainWindow::m()->Config.GetInt("recong_julius_lv_base",20);
	int julius_lv_min = julius_lv_base; //MainWindow::m()->Config.GetInt("recong_julius_lv_min",10);
	int julius_lv_max = MainWindow::m()->Config.GetInt("recong_julius_lv_max",150);
	if (julius_lv_min > julius_lv_base) julius_lv_min = julius_lv_base;
	if (julius_lv_max < julius_lv_base) julius_lv_max = julius_lv_base;
	j_adin_set_param_bacon_filter(recog,julius_lv_base*100,julius_lv_min*100,julius_lv_max*100);

	//output system information to log
	//j_recog_info(this->recog);
	ret = j_open_stream(recog, NULL);
	if(ret < 0)
	{
		throw XLEXCEPTION("Error in startup(j_open_stream)\n");
	}

	this->JuliusThread = new thread( [&]()
	{
		j_recognize_stream_naichichi(recog);
	} );

	return true;
}


bool Recognition_JuliusPlusEnglish::JuliusYobikakeStart()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	assert(this->recogYobikake == NULL);
	assert(this->jconfYobikake == NULL);
	const string testfile_jconf = MainWindow::m()->GetConfigBasePath("/julius/engfile.jconf");
	const char* argv[]={
		"juliusplus"
		,"-C"
		,testfile_jconf.c_str()
	};
	int argc = sizeof(argv)/sizeof(argv[0]);

	//julius は C関数だから、const外して char** にするしかない。
	this->jconfYobikake = j_config_load_args_new(argc, (char**)argv);
	/* else, you can load configurations from a jconf file */
	//jconf = j_config_load_file_new(jconf_filename);
	if (this->jconfYobikake == NULL) 
	{
		throw XLEXCEPTION("Try `-help' for more information.\n");
	}

	/* 2. create recognition instance according to the jconf */
	/* it loads models, setup final parameters, build lexicon
	and set up work area for recognitiustonf(this->jconfYobikake);
	*/
	this->recogYobikake = j_create_instance_from_jconf(this->jconfYobikake);
	if (this->recogYobikake == NULL)
	{
		throw XLEXCEPTION("Error in startup(j_create_instance_from_jconf)\n");
	}


	struct _ref{
		static void output_result_No_SVM(Recog *recog, void *_this)
		{
			((Recognition_JuliusPlusEnglish*)_this)->OnOutputResultFileNoSVM(recog);
		}
		static void error_exit(Recog *recog, void *_this)
		{
			ERRORLOG("julius内エラーのため再起動します。"
				<< "\t" << RecongLogResult_HardwareError
			);
			MainWindow::m()->Shutdown(EXITCODE_LEVEL_ERROR , true);
		}
	};

	callback_add(this->recogYobikake, CALLBACK_RESULT, _ref::output_result_No_SVM, this);
	callback_add(this->recogYobikake, CALLBACK_ERROR_EXIT, _ref::error_exit, this);
	
	// Initialize audio input
	if (j_adin_init(this->recogYobikake) == FALSE) 
	{
		throw XLEXCEPTION("Error in startup(j_adin_init)\n");
	}
	// update initial recognition process status
	for(auto r=this->recogYobikake->process_list;r;r=r->next) {
		r->live = TRUE;
		r->active = 0;
		break;
	}

	// check for grammar to change, and rebuild if necessary
	for(auto lm=this->recogYobikake->lmlist;lm;lm=lm->next) {
		if (lm->lmtype == LM_DFA) {
			multigram_update(lm); // some modification occured if return TRUE
		}
		break;
	}
	//以上、準備だけしておいて、
	//認識ルーチンは、後から呼びます。


	return true;
}


bool Recognition_JuliusPlusEnglish::JuliusWaveStart()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	assert(this->recogWave == NULL);
	assert(this->jconfWave == NULL);
	const string testfile_jconf = MainWindow::m()->GetConfigBasePath("/julius/engwave.jconf");
	const char* argv[]={
		"juliusplus"
		,"-C"
		,testfile_jconf.c_str()
	};
	int argc = sizeof(argv)/sizeof(argv[0]);

	//julius は C関数だから、const外して char** にするしかない。
	this->jconfWave = j_config_load_args_new(argc, (char**)argv);
	/* else, you can load configurations from a jconf file */
	//jconf = j_config_load_file_new(jconf_filename);
	if (this->jconfWave == NULL) 
	{
		throw XLEXCEPTION("Try `-help' for more information.\n");
	}

	/* 2. create recognition instance according to the jconf */
	/* it loads models, setup final parameters, build lexicon
	and set up work area for recognitiustonf(this->jconfYobikake);
	*/
	this->recogWave = j_create_instance_from_jconf(this->jconfWave);
	if (this->recogWave == NULL)
	{
		throw XLEXCEPTION("Error in startup(j_create_instance_from_jconf)\n");
	}


	struct _ref{
		static void output_result(Recog *recog, void *_this)
		{
			((Recognition_JuliusPlusEnglish*)_this)->OnOutputResult(recog);
		}
	};

	callback_add(this->recogWave, CALLBACK_RESULT, _ref::output_result, this);
	
	// Initialize audio input
	if (j_adin_init(this->recogWave) == FALSE) 
	{
		throw XLEXCEPTION("Error in startup(j_adin_init)\n");
	}
	// update initial recognition process status
	for(auto r=this->recogWave->process_list;r;r=r->next) {
		r->live = TRUE;
		r->active = 0;
	}

	// check for grammar to change, and rebuild if necessary
	for(auto lm=this->recogWave->lmlist;lm;lm=lm->next) {
		if (lm->lmtype == LM_DFA) {
			multigram_update(lm); // some modification occured if return TRUE
		}
	}
	//以上、準備だけしておいて、
	//認識ルーチンは、後から呼びます。


	return true;
}

//このコールバックに関連付けられているものをすべて消す
bool Recognition_JuliusPlusEnglish::RemoveCallback(const CallbackPP& callback , bool is_unrefCallback) 
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	return true;
}

//ディクテーションフィルター利用時の認識
void Recognition_JuliusPlusEnglish::OnOutputResultFileNoSVM(Recog *recog)
{
	this->DictationCheckResult = 0;

	const RecogProcess* r = recog->process_list;
	{
		// output results for all the obtained sentences
		const auto winfo = r->lm->winfo;
		for(auto n = 0; n < r->result.sentnum; n++) 
		{ // for all sentences
			if (r->result.sent == NULL)  continue;
			const auto s = &(r->result.sent[n]);
			if (s == NULL) continue;
			const auto seq = s->word;
			const auto seqnum = s->word_num;

			int seqIndex = 0;
			const int seq_0 = seq[0];
			if (   winfo->woutput[seq_0][0] == '<'
				&& winfo->woutput[seq_0][1] == 's' )
			{
				seqIndex++;
			}
			else
			{
//				DEBUGLOG("<s>がないです");
			}

			const int seq_i = seq[seqIndex];
			if (   winfo->woutput[seq_i][0] == '<'
				&& winfo->woutput[seq_i][1] == '/' )
			{
//				NOTIFYLOG("認識結果:</s>です");
				this->DictationCheckResult = -1;
				return ;
			}

			//よみを求める
			const string yomi = ConvertJuliusYomi(winfo,seq_i);
			//ゴミノードの場合は相手をしない
			if (   winfo->woutput[seq_i][0] == 'g' 
				&& winfo->woutput[seq_i][1] == 'o' 
				&& winfo->woutput[seq_i][2] == 'm' 
				&& winfo->woutput[seq_i][3] == 'i' )
			{
				this->DictationCheckLogMessage = "ディクテーションフィルターで棄却 [matchD:(" + yomi + ") confidenceD:" + num2str( s->confidence[seqIndex] ) +"]";
				this->DictationCheckResult = -1;
				return;
			}
	

			//認識した文字列をN倍したぐらいのデータがだいたいちょうどいいらしい。
			const unsigned int basesize = JuliusOnsoUtil::CountLength(yomi) * this->JuliusYobikakeSpeechSpeed; //9;
			const unsigned int samplenum = recog->mfcclist->param->samplenum;

			float ashikiriConfidenceYobikake = 0;
			if (samplenum <= basesize / 2 - (basesize / 8  ))
			{
//				ashikiriConfidenceYobikake = this->JuliusFilter2 + 0.35f ; //0.55;
				this->DictationCheckLogMessage = "ディクテーションフィルターで利用する音声データが短すぎます [matchD:(" + yomi + ") lenD:" + num2str(samplenum) + " confidenceD:" + num2str( s->confidence[seqIndex] ) + " basesizeD:" + num2str( basesize ) + " ashikiriCD:" + num2str( ashikiriConfidenceYobikake ) + "]";
				this->DictationCheckResult = -4;
				return ;
			}
			else if (samplenum <= basesize + (basesize / 4) )
			{//ベストフィット
				ashikiriConfidenceYobikake = this->JuliusFilter2; //0.20;
			}
			else
			{
				this->DictationCheckLogMessage = "ディクテーションフィルターで利用する音声データが長すぎます [matchD:(" + yomi + ") lenD:" + num2str(samplenum) + " confidenceD:" + num2str( s->confidence[seqIndex] ) + " basesizeD:" + num2str( basesize ) + " ashikiriCD:" + num2str( ashikiriConfidenceYobikake ) + "]";
				this->DictationCheckResult = -3;
				return ;
			}
/*
			if (s->confidence[seqIndex] <= ashikiriConfidenceYobikake )
			{
				this->DictationCheckLogMessage = "ディクテーションフィルターで 呼びかけが足切り点以下 [matchD:(" + yomi + ") lenD:" + num2str(samplenum) + " confidenceD:" + num2str( s->confidence[seqIndex] ) + " basesizeD:" + num2str( basesize ) + " ashikiriCD:" + num2str( ashikiriConfidenceYobikake ) + "]";
				this->DictationCheckResult = -2;
				return ;
			}
*/
			this->DictationCheckLogMessage = "ディクテーションフィルター [matchD:(" + yomi + ") lenD:" + num2str(samplenum) + " confidenceD:" + num2str( s->confidence[seqIndex] ) + " basesizeD:" + num2str( basesize ) + " ashikiriCD:" + num2str( ashikiriConfidenceYobikake ) + "]";
			this->DictationCheckResult = 1;
			return ;
		}
	}
}


//waveファイルデータから直接音声認識させます。
bool Recognition_JuliusPlusEnglish::RedirectWave(const vector<char>& wavedata) 
{
	//データを渡す
	int* arg[3];
	arg[0] = (int*)0x12341234; //念のためのヘッダー
	arg[1] = (int*)&wavedata[0];   //wave data
	arg[2] = (int*)wavedata.size();    //wave dataの長さ(byte)

	int ret = j_open_stream(this->recogWave, (char*)arg );
	if(ret < 0)
	{
		return false;
	}

	j_recognize_stream(this->recogWave);
	return true;
}



#endif
