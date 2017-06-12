//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#pragma once
//拡張ボードのインターフェース
struct ICapeBordInterface
{
	virtual int getLumi() = 0;
	virtual float getTemp() = 0;
	virtual bool Capture(const string& path) = 0;
	virtual bool Radiate(const string& path) = 0;

	virtual void TurnOnLED() = 0;
	virtual void TurnOffLED() = 0;
};

//拡張ボードがない場合
struct CapeBord_None : public ICapeBordInterface
{
	virtual int getLumi() { return 0; };
	virtual float getTemp() { return 0; };
	virtual bool Capture(const string& path) { return false; }
	virtual bool Radiate(const string& path) { return false; }
	virtual void TurnOnLED() {};
	virtual void TurnOffLED() {};
};

//LEDチカチカ
class AutoLEDOn
{
public:
	AutoLEDOn();
	virtual ~AutoLEDOn();
};

class CapeBord
{
public:
	CapeBord();
	virtual ~CapeBord();
	void Create();

	int getLumi();
	float getTemp();
	bool Capture(const string& path);
	bool Radiate(const string& path);
	void TurnOnLED() ;
	void TurnOffLED() ;

	void SleepAndBrightLED(int patternCount,int mirisec);
private:
	struct ICapeBordInterface* Cape;
};
