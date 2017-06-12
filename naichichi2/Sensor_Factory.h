#pragma once
#include "SensorGraph.h"

enum SensorType
{
	 SensorType_Temperature
	,SensorType_Luminous 
	,SensorType_Humidity
};

struct ISensorInterface
{
	ISensorInterface(){};
	virtual ~ISensorInterface() {};

	virtual void Create(const string& option) = 0;
	virtual float getTemperature() = 0;
	virtual float getLuminous() = 0;
	virtual float getHumidity() = 0;
};

class Sensor_Factory
{
public:
	Sensor_Factory();
	virtual ~Sensor_Factory();

	void Stop();
	void Create() ;
	//いますぐにセンサーを更新(スレッドセーフ)
	void getSensorNow(float * retTemp,float *retLumi,float *retSound);
	void getSensorNow();

	//データを読み込む
	void ReadDataMapDay(time_t now,map<time_t,short>* retTempmap,map<time_t,short>* retLumimap,map<time_t,short>* retSoundmap);
	void ReadDataMapWeek(time_t now,map<time_t,short>* retTempmap,map<time_t,short>* retLumimap,map<time_t,short>* retSoundmap);
	void Flush();

private:
	ISensorInterface* CreateSensor();

	ISensorInterface* temperatureSensor;
	ISensorInterface* luminousSensor;
	ISensorInterface* humiditySensor;


	ISensorInterface* CreateSensor(const string& sensor,const string& option);
	void ThreadMain();

	//音のセンサーからデータを取得する
	unsigned int getSoundCount(const time_t& pickTime,unsigned int checkBytes) const;

	thread*					Thread;
	mutable mutex lock;
	condition_variable EventObject;
	bool StopFlag;
	
	SensorGraph* SensorGraphTemp;
	SensorGraph* SensorGraphLumi;
	SensorGraph* SensorGraphSound;
};
