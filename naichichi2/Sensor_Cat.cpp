//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#include "common.h"
#include "Sensor_Factory.h"
#include "Sensor_Cat.h"
#include "MainWindow.h"
#include "SystemMisc.h"
#include "XLFileUtil.h"

Sensor_Cat::Sensor_Cat()
{
}
Sensor_Cat::~Sensor_Cat()
{
}

void Sensor_Cat::Create(const string& option) 
{
	this->Filename = option;
}

float Sensor_Cat::getTemperature() 
{
	const string r = XLFileUtil::cat(this->Filename);
	return (float)atof(r.c_str());
}

float Sensor_Cat::getLuminous() 
{
	const string r = XLFileUtil::cat(this->Filename);
	return (float)atof(r.c_str());
}

float Sensor_Cat::getHumidity() 
{
	const string r = XLFileUtil::cat(this->Filename);
	return (float)atof(r.c_str());
}

