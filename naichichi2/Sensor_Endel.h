#pragma once

class Sensor_Endel : public ISensorInterface
{
public:
	Sensor_Endel();
	virtual ~Sensor_Endel() ;

	virtual void Create(const string& option) ;
	virtual float getTemperature() ;
	virtual float getLuminous() ;
	virtual float getHumidity() ;

	//ケース内の温度上昇を考慮した温度補正
	float Temp2Temp(float temp) const;
};
