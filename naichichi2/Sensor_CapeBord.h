#pragma once

class Sensor_CapeBord : public ISensorInterface
{
public:
	Sensor_CapeBord();
	virtual ~Sensor_CapeBord() ;

	virtual void Create(const string& option) ;
	virtual float getTemperature() ;
	virtual float getLuminous() ;
	virtual float getHumidity() ;

private:
	//ケース内の温度上昇を考慮した温度補正
	float Temp2Temp(float temp) const;
};
