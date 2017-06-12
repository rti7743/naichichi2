//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#pragma once
#include "CapeBord.h"


class CapeBord_IRR : public ICapeBordInterface
{
public:
	CapeBord_IRR();
	virtual ~CapeBord_IRR();
	
	virtual int getLumi();
	virtual float getTemp();

	virtual bool Capture(const string& path);
	virtual bool Radiate(const string& path);
	
	virtual void TurnOnLED();
	virtual void TurnOffLED();
private:
	void TurnAllOffLED();
private:
	//同時に利用しないようにロックする.
	mutable mutex updatelock;
	
	//温度 AIN
	string TempAIN;
	//光 AIN
	string LumiAIN;
	//IRRコマンド
	string CapeBord_IRRBIN;
};
