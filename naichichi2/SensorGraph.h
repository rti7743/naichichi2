#pragma once
//センサーグラフを保存するクラス データーは short で記録されます。それ以外はダメです。
class SensorGraph
{
	string Filename;		//ファイル名
	time_t RecStartTime;		//記録開始日
	unsigned int SpanSec;		//記録間隔
	unsigned int RotateSpan;	//ローテートする間隔 SpanSec単位
	vector<short> FileData;//ファイルのデータ

public:
	SensorGraph(const string& filename , unsigned int newSpanSec , unsigned int newRotateSpan);
	virtual ~SensorGraph();

	//データを書き込む
	void WriteData(time_t now,short val);

	//データを読み込む
	void ReadDataMap(time_t now,unsigned int span,unsigned int intervalSec,map<time_t,short>* retmap);

	bool Load(const string& filename);
	bool Save();

private:
	void NewCreate( unsigned int newSpanSec , unsigned int newRotateSpan);
};
