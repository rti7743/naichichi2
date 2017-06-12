//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#pragma once

class Sensor_None : public ISensorInterface
{
public:
	Sensor_None(){};
	virtual ~Sensor_None() {};

	virtual void Create(const string& option) {}
	virtual float getTemperature() { return 0; }
	virtual float getLuminous() { return 0; }
	virtual float getHumidity() { return 0; }
};
