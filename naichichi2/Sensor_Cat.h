#pragma once

class Sensor_Cat : public ISensorInterface
{
public:
	Sensor_Cat();
	virtual ~Sensor_Cat() ;

	virtual void Create(const string& option) ;
	virtual float getTemperature() ;
	virtual float getLuminous() ;
	virtual float getHumidity() ;
private:
	string Filename;
};
