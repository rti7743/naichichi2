//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#include "common.h"
#include "MainWindow.h"
#include "XLFileUtil.h"
#include "SensorGraph.h"
const short INVALIDATE_VALUE = (-32767-1);

//センサーグラフを保存するクラス データーは short で記録されます。それ以外はダメです。
SensorGraph::SensorGraph(const string& filename , unsigned int newSpanSec , unsigned int newRotateSpan)
{
	if (! Load(filename) )
	{
		NewCreate( newSpanSec , newRotateSpan);
	}
	this->Filename = filename;
}

SensorGraph::~SensorGraph()
{

}

//データを書き込む
void SensorGraph::WriteData(time_t now,short val)
{
	//short minは無効な値に使うので書き込めません。
	if (val == INVALIDATE_VALUE) val = INVALIDATE_VALUE+1;

	if (now < this->RecStartTime) now = this->RecStartTime;
	unsigned int block =  (unsigned int) ((now - this->RecStartTime)/ this->SpanSec); //書き込む位置で時刻を表現してます

	if (block > this->RotateSpan * 2)
	{//ローテートするべき領域ですか？
		if (FileData.size() >= block)
		{//あり得ないぐらい期間があいてしまった場合
			//このデータを基準に記録します。
			const time_t datasize = this->SpanSec*3;
			for(size_t i = 0 ; i < datasize ; ++i )
			{//無効な値にします。 memset したいところですが・・・
				FileData[i] = INVALIDATE_VALUE;
			}
			this->RecStartTime = now;
			block = (unsigned int)((now - this->RecStartTime)/ this->SpanSec);
			ASSERT(block == 0 );
		}
		else
		{
			time_t newStartTime =this->RecStartTime + (this->RotateSpan);
			memmove(&FileData[0] , &FileData[this->RotateSpan] , this->RotateSpan * 2 * sizeof(short) );
			//新しい領域を初期化
			const time_t datasize = this->RotateSpan *3;
			ASSERT(FileData.size() == datasize);
			for(size_t i = this->SpanSec * 2 ; i < datasize ; ++i )
			{//移動して作られた部分を無効な値にします。 memset したいところですが・・・
				FileData[i] = INVALIDATE_VALUE;
			}
			//新しい領域にデータを書き込みます
			block = (unsigned int)((now - this->RecStartTime)/ this->SpanSec);
		}
	}

	ASSERT(block < FileData.size());
	FileData[block] = val;
}

//データを読み込む
void SensorGraph::ReadDataMap(time_t now,unsigned int span,unsigned int intervalSec,map<time_t,short>* retmap)
{
	ASSERT(intervalSec >= 1);

	time_t t = now;
	//取得した範囲の前の時間からスタートします
	if (t < span * intervalSec)	t = 0;
	else                        t -= (span * intervalSec);


	retmap->clear();
	for(unsigned int i = 0 ; i < intervalSec ; i ++)
	{
		unsigned int block = (unsigned int)( (t > this->RecStartTime ? t-this->RecStartTime:0) / this->SpanSec); //位置で時刻を表現してます
		if (block > this->RotateSpan * 3)
		{//終端を超えるならばおしまい
			break;
		}

		//自分の次のブロックを求めます
		time_t nextt = t + span;
		unsigned int nextblock = (unsigned int)( (nextt > this->RecStartTime ? nextt-this->RecStartTime:0) / this->SpanSec); //位置で時刻を表現してます
		//その間の平均を取ります。
		long   sumVal = 0;
		unsigned int sumCount = 0;
		for (unsigned int n = block ; n < nextblock ; ++n )
		{
			const short p = FileData[block];
			if (p == INVALIDATE_VALUE)
			{
				continue;
			}
			sumVal += p;
			sumCount++;
		}
		//平均を出します。
		if (sumCount <= 0) sumVal = 0;
		else               sumVal /= sumCount;
		(*retmap)[t] = (short)sumVal;

		//次に進む
		t = nextt;
	}
}

void SensorGraph::NewCreate( unsigned int newSpanSec , unsigned int newRotateSpan)
{//新規に作る
	const unsigned int datasize = newRotateSpan*3;

	FileData.resize(datasize);
	for(unsigned int i = 0 ; i < datasize ; ++i )
	{//無効な値にします。 memset したいところですが・・・
		FileData[i] = INVALIDATE_VALUE;
	}
	this->RecStartTime = time(NULL);
	this->SpanSec = newSpanSec;
	this->RotateSpan = newRotateSpan;
}

bool SensorGraph::Load(const string& filename)
{//読み込み
	FILE * fp = fopen(filename.c_str() , "rb");
	if (!fp)
	{//読めなかったら新規作成
		ERRORLOG("センサーデータ:" << filename << " ファイルが読めません" );
		return false;
	}
	//ヘッダー部分を読み込む
#if _MSC_VER
	unsigned __int64 rectimeW; //time_t が処理系で変わってしまうので補正しておく
#else
	unsigned long long rectimeW;
#endif
	if( fread(&rectimeW,sizeof(rectimeW),1,fp) <= 0 )
	{
		ERRORLOG("センサーデータ:" << filename << " rectimeWが初期化出来ませんでした");
		return false;
	}
	this->RecStartTime = (time_t)rectimeW;

	if( fread(&this->SpanSec,sizeof(this->SpanSec),1,fp) <= 0 )
	{
		ERRORLOG("センサーデータ:" << filename << " SpanSecが初期化出来ませんでした");
		return false;
	}
	if( fread(&this->RotateSpan,sizeof(this->RotateSpan),1,fp) <= 0 )
	{
		ERRORLOG("センサーデータ:" << filename << " RotateSpanが初期化出来ませんでした");
		return false;
	}
	//記録周期とかローテート感覚が違うならばデータコンバート
	//は、未実装、なので変えられないです
	
	const unsigned int datasize = this->RotateSpan*3;
	FileData.resize(datasize);
	int readbyte = fread(&FileData[0],sizeof(short),datasize/sizeof(short),fp);
	if (readbyte <= 0 || (unsigned int)readbyte < datasize/sizeof(short))
	{//ファイルが壊れています。
		fclose(fp);
		ERRORLOG("センサーデータ:" << filename << " データ部が壊れています。readbyte:" << readbyte );
		return false;
	}
	fclose(fp);
	//OK全部読めました
	return true;
}

bool SensorGraph::Save()
{//書き込み
	ASSERT(!Filename.empty());
	const string tempfilename = this->Filename + ".tmp";
	FILE * fp = fopen(tempfilename.c_str() , "wb");
	if (!fp)
	{//むむむ・・・。
		return false;
	}
	//ヘッダー部分を書き込む
#if _MSC_VER
	unsigned __int64 rectimeW = (unsigned __int64)this->RecStartTime; //time_t が処理系で変わってしまうので補正しておく
#else
	unsigned long long rectimeW = (unsigned long long)this->RecStartTime; //time_t が処理系で変わってしまうので補正しておく
#endif
	fwrite(&rectimeW,sizeof(rectimeW),1,fp);
	fwrite(&this->SpanSec,sizeof(this->SpanSec),1,fp);
	fwrite(&this->RotateSpan,sizeof(this->RotateSpan),1,fp);
	
	const unsigned int datasize = this->RotateSpan*3;
	ASSERT(datasize == this->FileData.size());
	fwrite(&FileData[0],sizeof(short),datasize/sizeof(short),fp);

	fclose(fp);
	
	XLFileUtil::move(tempfilename,this->Filename);
	XLFileUtil::del (tempfilename);
	return true;
}
