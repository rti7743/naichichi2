//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#include "common.h"
#include "Sensor_Factory.h"
#include "Sensor_Process.h"
#include "MainWindow.h"
#include "SystemMisc.h"
#include "XLFileUtil.h"

Sensor_Process::Sensor_Process()
{
}
Sensor_Process::~Sensor_Process()
{
}

void Sensor_Process::Create(const string& option) 
{
	this->Filename = option;
}

float Sensor_Process::getTemperature() 
{
	const string r = SystemMisc::PipeExec(this->Filename);
	return (float) atof(r.c_str());
}

float Sensor_Process::getLuminous() 
{
	const string r = SystemMisc::PipeExec(this->Filename);
	return (float) atof(r.c_str());
}

float Sensor_Process::getHumidity() 
{
	const string r = SystemMisc::PipeExec(this->Filename);
	return (float) atof(r.c_str());
}

