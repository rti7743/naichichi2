#pragma once

class Sensor_Process : public ISensorInterface
{
public:
	Sensor_Process();
	virtual ~Sensor_Process();

	virtual void Create(const string& option) ;
	virtual float getTemperature() ;
	virtual float getLuminous() ;
	virtual float getHumidity() ;
private:
	string Filename;
};
