//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include "Sensor_Factory.h"
#include "Sensor_None.h"
#include "Sensor_CapeBord.h"
#include "Sensor_Process.h"
#include "Sensor_Cat.h"

#include "Trigger.h"

#include "MainWindow.h"


Sensor_Factory::Sensor_Factory()
{
	this->temperatureSensor = NULL;
	this->luminousSensor = NULL;
	this->humiditySensor = NULL;
	this->SensorGraphTemp = NULL;
	this->SensorGraphLumi = NULL;
	this->SensorGraphSound = NULL;
	
	this->Thread = NULL;
	this->StopFlag = false;
}

Sensor_Factory::~Sensor_Factory()
{
	DEBUGLOG("stop...");

	Stop();
	delete this->temperatureSensor;
	delete this->luminousSensor;
	//delete this->humiditySensor;
	if (this->SensorGraphTemp != NULL)
	{
		this->SensorGraphTemp->Save();
		delete this->SensorGraphTemp;
		this->SensorGraphTemp = NULL;
	}
	if (this->SensorGraphLumi != NULL)
	{
		this->SensorGraphLumi->Save();
		delete this->SensorGraphLumi;
		this->SensorGraphLumi = NULL;
	}

	DEBUGLOG("done");
}

void Sensor_Factory::Stop()
{
	if (this->Thread)
	{
		this->StopFlag = true;
		this->EventObject.notify_one();
		this->Thread->join();
		delete this->Thread;
		this->Thread = NULL;
	}
}


ISensorInterface* Sensor_Factory::CreateSensor(const string& sensor,const string& option)
{
	ASSERT_IS_MAIN_THREAD_RUNNING();

	ISensorInterface* p;
	if (sensor == "" || sensor == "endel" || sensor == "capebord")
	{
		p = new Sensor_CapeBord();
	}
//	else if (sensor == "process")
//	{
//		p = new Sensor_Process();
//	}
//	else if (sensor == "snmp")
//	{
//		p = new Sensor_SNMP();
//	}
//	else if (sensor == "cat")
//	{
//		p = new Sensor_Cat();
//	}
	else if (sensor == "none")
	{
		p = new Sensor_None();
	}
	else
	{
		ERRORLOG("センサー" << sensor << "がありません" );
		ASSERT(0);	//デバッグならここで落としてあげるのが優しさ。
		
		ERRORLOG("仕方ないので、ディフォルト値のセンサーエンジンを選択します");
		p = new Sensor_CapeBord();
	}
	
	try
	{
		p->Create(option);
	}
	catch(XLException& e)
	{
		ERRORLOG("センサー" << sensor << "を構築できませんでした。Exception:" << e.what() );
		ASSERT(0);	//デバッグならここで落としてあげるのが優しさ。
		ERRORLOG("仕方ないので、センサーをオフにします.");

		delete p;
		p = new Sensor_None();
	}
	
	return p;
}

void Sensor_Factory::Create() 
{
	ASSERT_IS_MAIN_THREAD_RUNNING();

	Stop();
	delete this->temperatureSensor;
	delete this->luminousSensor;
	//delete this->humiditySensor;
	if (this->SensorGraphTemp != NULL)
	{
		this->SensorGraphTemp->Save();
		delete this->SensorGraphTemp;
	}
	if (this->SensorGraphLumi != NULL)
	{
		this->SensorGraphLumi->Save();
		delete this->SensorGraphLumi;
	}

	//センサーの構築
	string sensortype = MainWindow::m()->Config.Get("sensor_temp_type","endel");
	string sensoroption = MainWindow::m()->Config.Get("sensor_temp_option","endel");
	this->temperatureSensor = CreateSensor(sensortype,sensoroption);

	sensortype = MainWindow::m()->Config.Get("sensor_lumi_type","");
	sensoroption = MainWindow::m()->Config.Get("sensor_lumi_option","");
	this->luminousSensor = CreateSensor(sensortype,sensoroption);
//	sensortype = this->Config.Get("sensor_humi_type","");
//	sensoroption = this->Config.Get("sensor_humi_option","");
//	this->humiditySensor = CreateSensor(sensortype,sensoroption);

	//センサーの値を格納するグラフ
	string graphfilename = MainWindow::m()->Config.Get("sensor_temp_graph_filename",MainWindow::m()->GetConfigBasePath("sensor/temp_graph.dat"));
	this->SensorGraphTemp = new SensorGraph(graphfilename, 300 , 60*60*24*365/300); //5分間隔で1年分のデータを記録

	graphfilename = MainWindow::m()->Config.Get("sensor_lumi_graph_filename",MainWindow::m()->GetConfigBasePath("sensor/lumi_graph.dat"));
	this->SensorGraphLumi = new SensorGraph(graphfilename, 300 , 60*60*24*365/300); //5分間隔で1年分のデータを記録

	graphfilename = MainWindow::m()->Config.Get("sensor_sound_graph_filename",MainWindow::m()->GetConfigBasePath("sensor/sound_graph.dat"));
	this->SensorGraphSound = new SensorGraph(graphfilename, 300 , 60*60*24*365/300); //5分間隔で1年分のデータを記録

	this->Thread = new thread([=](){
		this->ThreadMain(); 
	});
	
}

//音のセンサーからデータを取得する
unsigned int Sensor_Factory::getSoundCount(const time_t& pickTime,unsigned int checkBytes) const
{
	

	unsigned int count = 0;

	string lastlog;
	lastlog = sexylog::m()->TailByte(checkBytes);

	vector<string> pickerVec;
	
	//改行で分離して見ていきます。
	const auto lines = XLStringUtil::split_vector("\n",lastlog);
	for(auto lineIT = lines.begin() ; lineIT != lines.end() ; lineIT++ )
	{
		//ログデータは\tでゆるく切られているので、それでばらしていきます。
		const auto parts = XLStringUtil::split_vector("\t",*lineIT);
		if (parts.size() <= 4)
		{
			continue;
		}

//		2013/9/14 8:32:9	3	OnOutputResult_2Pass:363	認識結果:gomi (koka) file://aaaa.wav GM
		if ( parts[2].find("OutputResult") == string::npos )
		{
			continue;
		}

		//最後に符号がつきます。2バイト固定です
		string code = parts[ parts.size()-1 ];
		if (code.size() != 2+1 || (('A' >= code[0] && 'Z' <= code[0]) && ('A' >= code[1] && 'Z' <= code[1]) ) )
		{
			continue;
		}
		code = code.substr(0,2);

		//時間が今より新しいかどうか
		const time_t logtime = XLStringUtil::strtotime(parts[0]);
		if (pickTime > logtime)
		{
			continue;
		}

		count ++;
	}

	return MIN(200, count);
}


//いますぐにセンサーを更新(スレッドセーフ)
void Sensor_Factory::getSensorNow(float * retTemp,float *retLumi,float *retSound)
{
	ASSERT___IS_THREADFREE();
	ASSERT(retTemp != NULL);
	ASSERT(retLumi != NULL);
	ASSERT(retSound != NULL);

	DEBUGLOG("センサーから値を読み取ります。");

	//今すぐ値を取得する (せっかくだから取得したものをファイルに書く)
	float temp,lumi,sound;

	//今の時刻を取得
	const time_t now = time(NULL);
	{
		lock_guard<mutex> al(this->lock);

		//センサーから値を取得する
		temp = this->temperatureSensor->getTemperature();
		if (temp <= 0)
		{//リトライ
			temp = this->temperatureSensor->getTemperature();
		}
		lumi = this->luminousSensor->getLuminous();
		if (lumi <= 0)
		{//リトライ
			lumi = this->luminousSensor->getLuminous();
		}
		sound =	(float) this->getSoundCount(now - (30*60),500*1024);
		if (sound <= 0)
		{//リトライ
			sound =	(float) this->getSoundCount(now - (30*60),500*1024);
		}
	}

	if (temp <= -100 || temp >= 100)
	{//エラー
		const float last_temp = MainWindow::m()->Config.GetDouble("sensor_temp_value",0.0f);
		NOTIFYLOG("温度センサーから読み込んだ値("<<temp<<")が変なので前回の値("<<last_temp<<")を利用します。");
		temp = last_temp;
	}
	if (lumi <= -100 || lumi >= 1001)
	{//エラー
		const float last_lumi = MainWindow::m()->Config.GetDouble("sensor_lumi_value",0.0f);
		NOTIFYLOG("光センサーから読み込んだ値("<<lumi<<")が変なので前回の値("<<last_lumi<<")を利用します。");
		lumi = last_lumi;
	}

	if (sound <= -100)
	{//エラー
		const float last_sound = MainWindow::m()->Config.GetDouble("sensor_sound_value",0.0f);
		NOTIFYLOG("音センサーから読み込んだ値("<<sound<<")が変なので前回の値("<<last_sound<<")を利用します。");
		sound = last_sound;
	}
	DEBUGLOG("センサーから値を読み取りました。temp: " << temp << " lumi:" << lumi << " sound:" << sound );

	{
		lock_guard<mutex> al(this->lock);

		//ファイルに記録
		this->SensorGraphTemp->WriteData(now,(short)(temp*100));
		this->SensorGraphLumi->WriteData(now,(short)(lumi));
		this->SensorGraphSound->WriteData(now,(short)(sound));
	}
	//値を書き込む
	MainWindow::m()->Config.Set("sensor_temp_value",num2str(temp));
	MainWindow::m()->Config.Set("sensor_lumi_value",num2str(lumi));
	MainWindow::m()->Config.Set("sensor_sound_value",num2str(sound));

	//値を返す
	*retTemp = temp;
	*retLumi = lumi;
	*retSound = sound;


}

//いますぐにセンサーを更新(スレッドセーフ)
void Sensor_Factory::getSensorNow()
{
	float sensor_temp,sensor_lumi,sensor_sound;
	MainWindow::m()->Sensor.getSensorNow(&sensor_temp,&sensor_lumi,&sensor_sound);
}

void Sensor_Factory::Flush()
{
	lock_guard<mutex> al(this->lock);
	if (this->SensorGraphTemp != NULL)
	{
		this->SensorGraphTemp->Save();
	}
	if (this->SensorGraphLumi != NULL)
	{
		this->SensorGraphLumi->Save();
	}
	if (this->SensorGraphSound != NULL)
	{
		this->SensorGraphSound->Save();
	}
}

//1日分のデータを読み込む
void Sensor_Factory::ReadDataMapDay(time_t now,map<time_t,short>* retTempmap,map<time_t,short>* retLumimap,map<time_t,short>* retSoundmap)
{
	ASSERT___IS_THREADFREE();

	//ファイルからデータを読み込む
	{
		lock_guard<mutex> al(this->lock);

		this->SensorGraphTemp->ReadDataMap(now, 60*60 , 12 , retTempmap); //1時間足
		this->SensorGraphLumi->ReadDataMap(now, 60*60 , 12 , retLumimap);
		this->SensorGraphSound->ReadDataMap(now, 60*60 , 12 , retSoundmap);
	}
}

//週間のデータを読み込む
void Sensor_Factory::ReadDataMapWeek(time_t now,map<time_t,short>* retTempmap,map<time_t,short>* retLumimap,map<time_t,short>* retSoundmap)
{
	ASSERT___IS_THREADFREE();

	//ファイルからデータを読み込む
	{
		lock_guard<mutex> al(this->lock);

		this->SensorGraphTemp->ReadDataMap(now,  60*60*24 , 7 , retTempmap); //1日間足
		this->SensorGraphLumi->ReadDataMap(now,  60*60*24 , 7 , retLumimap);
		this->SensorGraphSound->ReadDataMap(now, 60*60*24 , 7 , retSoundmap);
	}
}


void Sensor_Factory::ThreadMain()
{
	unsigned int count = 0;
	while(! this->StopFlag)
	{
		float temp,lumi,sound;
		getSensorNow(&temp,&lumi,&sound);
		MainWindow::m()->Trigger.OnSensorUpdate(temp,lumi);

		count++;
		if (count >= 12)
		{//12回 つまり 5分*12=1時間たったら、ファイルをディスクに書き込む。 
		 //linuxがSD cardで動いているので、優しい実行を心がける。
			DEBUGLOG("メモリ上のSensorGrapをディスクにsyncします");
			{
				lock_guard<mutex> al(this->lock);
				this->SensorGraphTemp->Save();
				this->SensorGraphLumi->Save();
				this->SensorGraphSound->Save();
			}
			count = 0;
		}

		SecSleepEx(this->lock,this->EventObject,5 * 60);
		if (this->StopFlag) return ;

		//入れるべきか非常に迷うんだけど、watchdogしとくか・・・
		if (temp >= 50)
		{//40度を超える数値を観測した場合は、プログラムを強制的に落とす。
			ERRORLOG("!!! 温度が50度以上が観測されました。危険なのでシステムを停止します");
			MainWindow::m()->Shutdown(EXITCODE_LEVEL_NORMALEND,true); //強制終了
			return ;
		}
	}
}
#endif // WITH_CLIENT_ONLY_CODE==1
