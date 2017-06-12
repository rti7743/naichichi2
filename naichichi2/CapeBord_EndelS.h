//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#pragma once
#include "CapeBord_EndelSCommander.h"
#include "CapeBord.h"

#include <vector>
#include <string>
struct IRData
{
	struct IRPattern
	{
		unsigned short pulseOrder;
		unsigned long  wait;
		
		IRPattern(unsigned short pulseOrder,unsigned long wait) 
			: pulseOrder(pulseOrder),wait(wait)
		{
		}
	};

	unsigned long carrierPeriod;
	vector<IRPattern> pattern;
};

const unsigned short CMDID_ACK = 0x7E20;
const unsigned short CMDID_NAK = 0x7E21;

const unsigned short CMDID_NOOP = 0x4020;

const unsigned short CMDID_GET_ADC0 = 0x4120;
const unsigned short CMDID_REP_ADC0 = 0x4124;
const unsigned short CMDID_GET_ADC1 = 0x4128;
const unsigned short CMDID_REP_ADC1 = 0x412C;

const unsigned short CMDID_START_CAPTURE = 0x5020;
const unsigned short CMDID_GET_CAPTURE_STATUS = 0x5024;
const unsigned short CMDID_GET_CAPTURED_PATTERN = 0x5028;

const unsigned short CMDID_START_RADIATE= 0x5030;
const unsigned short CMDID_GET_RADIATE_PROGRESS= 0x5034;
const unsigned short CMDID_SET_RADIATE_PARAMETERS= 0x5038;
const unsigned short CMDID_SET_RADIATE_PATTERN= 0x503C;
const unsigned short CMDID_VERIFY_RADIATE_PATTERN = 0x5040;

//state constant
const unsigned char STATE_WAIT_FOR_CARRIER=0;
const unsigned char STATE_CARRIER_COUNT=1;
const unsigned char STATE_NOISE_DETECTED=2;
const unsigned char STATE_PATTERN_OVERFLOW=3;
const unsigned char STATE_NORMAL_END=4;
//progress state constant
const unsigned char PROGRESS_IN_INITIALIZE=1;
const unsigned char PROGRESS_READY=1;
const unsigned char PROGRESS_IN_PROGRESS=2;
const unsigned char PROGRESS_WAIT_FINISH=3;
const unsigned char PROGRESS_FINISHED=4;




class CapeBord_EndelS : public ICapeBordInterface
{
public:
	CapeBord_EndelS();
	virtual ~CapeBord_EndelS();
	
	virtual int getLumi();
	virtual float getTemp();

	virtual bool Capture(const string& path);
	virtual bool Radiate(const string& path);
	virtual void TurnOnLED();
	virtual void TurnOffLED();

private:
	int Capture(IRData * irdata);
	int Radiate(const IRData * irdata);
	void DumpIRData(const IRData * irdata);
	
	bool checkIR(const IRData * irdata);

	bool SaveIRData(const IRData * irdata,const string& filename);
	bool LoadIRData(IRData * irdata,const string& filename);

	void TurnAllOffLED();

private:
	//同時に利用しないようにロックする.
	mutable mutex updatelock;
	
	time_t lastRadiateTime;
};
