#include "common.h"
#include "CapeBord_EndelS.h"
#include "MainWindow.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "XLFileUtil.h"

CapeBord_EndelS::CapeBord_EndelS()
{
	this->lastRadiateTime = time(NULL);

	//Beaglebone LED OFF
	TurnAllOffLED();
}

CapeBord_EndelS::~CapeBord_EndelS()
{
}

int CapeBord_EndelS::getLumi()
{
	unique_lock<mutex> al(this->updatelock);

	unsigned char src[PACKET_MAXLEN];
	unsigned char dest[PACKET_MAXLEN];
	unsigned short command;
	int destlen;
		
	CapeBord_EndelSCommander commander;
	destlen = commander.transaction(src,dest,0,CMDID_GET_ADC1,&command);
	if (  destlen < 0  )
	{
		return -100 + destlen;
	}
	if (command != CMDID_REP_ADC1)
	{
		return -200;
	}

	const int r = (int) ((dest[0]) | (dest[1]<<8));
	if (r > 300) return 300;
	return r;
}

float CapeBord_EndelS::getTemp()
{
	unique_lock<mutex> al(this->updatelock);

	unsigned char src[PACKET_MAXLEN];
	unsigned char dest[PACKET_MAXLEN];
	unsigned short command;
	int destlen;
		
	CapeBord_EndelSCommander commander;
	destlen = commander.transaction(src,dest,0,CMDID_GET_ADC0,&command);
	if (  destlen < 0  )
	{
		return -100 + destlen;
	}
	if (command != CMDID_REP_ADC0)
	{
		return -200;
	}

	int temp = (int) ((dest[0]) | (dest[1]<<8));
	if (temp <= 0) temp = 1;
	const float reg = 12000.0*(1023.0/temp) - 12000.0;
	return (1.0/(log(reg/10000.0)/3380.0+1.0/298.15))+(-273.15);
}

bool CapeBord_EndelS::Capture(const string& path)
{
	IRData irdata;
	int r = this->Capture(&irdata);
	if (r <= 0)
	{
		ERRORLOG("学習に失敗しました。r:" << r );
		return false;
	}
	if ( ! this->checkIR(&irdata) )
	{
		ERRORLOG("学習したデータはダメっぽいです。" );
		return false;
	}

	DEBUGLOG("学習できました。保存します。 " << path );

	this->SaveIRData(&irdata,path);
	return true;
}

bool CapeBord_EndelS::Radiate(const string& path)
{
	IRData irdata;
	int r = this->LoadIRData(&irdata, path );
	if (r <= 0)
	{
		ERRORLOG("IR LOAD ERROR! " << path  );
		return false;
	}

	r = this->Radiate(&irdata );
	if (r <= 0)
	{
		ERRORLOG("IR Radiate ERROR! " << path  );
		return false;
	}
	return true;
}


int CapeBord_EndelS::Capture(IRData * irdata)
{
	unique_lock<mutex> al(this->updatelock);
	NOTIFYLOG("IR Capture Start!!");

	irdata->pattern.clear();

	unsigned char src[PACKET_MAXLEN];
	unsigned char dest[PACKET_MAXLEN];
	unsigned short command;
	int destlen;
		
	CapeBord_EndelSCommander commander;
	destlen = commander.transaction(src,dest,0,CMDID_START_CAPTURE,&command);
	if (  destlen < 0  )
	{
		return -100 + destlen;
	}
	if (command != CMDID_ACK)
	{
		return -200;
	}
	sleep(5);

	unsigned char state;
	unsigned long carrierPeriod;
	unsigned short patternLength;
	while(1)
	{
		destlen = commander.transaction(src,dest,0,CMDID_GET_CAPTURE_STATUS,&command);
		if (  destlen < 0  )
		{
			return -300 + destlen;
		}
			
		unsigned char progress = dest[0];
		state = dest[1];
		carrierPeriod = dest[2] | (dest[3]<<8) | (dest[4]<<16) | (dest[5]<<24);
		patternLength = dest[6] | (dest[7]<<8);
		if (progress == PROGRESS_FINISHED)
		{
			break;
		}
		else
		{
			sleep(1);
		}
	}

	if (state != STATE_NORMAL_END )
	{
		return -400;
	}
		
	if (carrierPeriod != 0)
	{
		NOTIFYLOG("Carrier " << (72.0E+6/carrierPeriod) << " Hz");
	}
	else
	{
		NOTIFYLOG("Carrier period is zero");
	}
	NOTIFYLOG("IR Length: " << patternLength);

	irdata->carrierPeriod = carrierPeriod;
	for(unsigned short i = 0 ; i < patternLength ; ++i )
	{
		src[0] = i&0xFF;
		src[1] = (i&0xFF00)>>8;
		destlen = commander.transaction(src,dest,2,CMDID_GET_CAPTURED_PATTERN,&command);
		if (  destlen < 0  )
		{
			return -500 + destlen;
		}

		unsigned short pulseOrder = dest[0] | ( ((unsigned short)dest[1])<<8);
		unsigned long  wait       = dest[2] | ( ((unsigned long)dest[3])<<8) | ( ((unsigned long)dest[4])<<16) | ( ((unsigned long)dest[5])<<24);

		irdata->pattern.push_back(IRData::IRPattern(pulseOrder,wait));
	}
	return 1;
}

int CapeBord_EndelS::Radiate(const IRData * irdata)
{
	unique_lock<mutex> al(this->updatelock);
	const time_t limitTime = (time(NULL) - 0);
	if ( this->lastRadiateTime  >= limitTime )
	{//短い間隔で Radiate されたら、強制的にsleepを挟む
		unsigned int sleepSec = (this->lastRadiateTime - limitTime) + 1;
		NOTIFYLOG("短い時間で赤外線を連続して送ろうとしているのでsleepを差し込みます。sleep(" << sleepSec <<")sec.");
		sleep( sleepSec );
		this->lastRadiateTime  = time(NULL);
	}


	unsigned char src[PACKET_MAXLEN];
	unsigned char dest[PACKET_MAXLEN];
	unsigned short command;
	int destlen;

	CapeBord_EndelSCommander commander;

	unsigned short order = 0;
	for(auto it = irdata->pattern.begin() ; it != irdata->pattern.end() ; ++it , ++order)
	{
		src[0] = (order & 0xFF);
		src[1] = ((order & 0xFF00)>>8);
		src[2] = (it->pulseOrder&0xFF);
		src[3] = ((it->pulseOrder&0xFF00)>>8);
		src[4] = (unsigned char)(it->wait & 0xFF);
		src[5] = (unsigned char)((it->wait & 0xFF00)>>8);
		src[6] = (unsigned char)((it->wait & 0xFF0000)>>16);
		src[7] = (unsigned char)((it->wait & 0xFF000000)>>24);

		destlen = commander.transaction(src,dest,8,CMDID_SET_RADIATE_PATTERN,&command);
		if (  destlen < 0  )
		{
			return -100 + destlen;
		}
	}

	unsigned short patternlen = irdata->pattern.size();
	src[0] = (unsigned char)(irdata->carrierPeriod & 0xFF);
	src[1] = (unsigned char)((irdata->carrierPeriod & 0xFF00)>>8);
	src[2] = (unsigned char)((irdata->carrierPeriod & 0xFF0000)>>16);
	src[3] = (unsigned char)((irdata->carrierPeriod & 0xFF000000)>>24);
	src[4] = (unsigned char)((patternlen & 0xFF));
	src[5] = (unsigned char)((patternlen & 0xFF00)>>8);
	destlen = commander.transaction(src,dest,6,CMDID_SET_RADIATE_PARAMETERS,&command);
	if (  destlen < 0  )
	{
		return -200 + destlen;
	}

	destlen = commander.transaction(src,dest,0,CMDID_VERIFY_RADIATE_PATTERN,&command);
	if (  destlen < 0  )
	{
		return -300 + destlen;
	}
	if(command == CMDID_NAK)
	{
		return -400;
	}

	destlen = commander.transaction(src,dest,0,CMDID_START_RADIATE,&command);
	if (  destlen < 0  )
	{
		return -500 + destlen;
	}
	
	this->lastRadiateTime  = time(NULL);
	return 1;
}

void CapeBord_EndelS::DumpIRData(const IRData * irdata)
{
	printf("==IR DATA==\r\n");
	printf("carrierPeroid:%u\r\n",irdata->carrierPeriod);
	printf("pattern:%d\r\n",irdata->pattern.size());
	for(auto it = irdata->pattern.begin() ; it != irdata->pattern.end() ; ++it ) 
	{
		printf("pulseOrder: %3u @@@ wait: %u\r\n",it->pulseOrder,it->wait);
	}
}

bool CapeBord_EndelS::SaveIRData(const IRData * irdata,const string& filename)
{
	auto fp = fopen(filename.c_str() ,"wb");
	if (!fp)
	{
		return false;
	}
	unsigned int pattern = irdata->pattern.size();
	fwrite(&irdata->carrierPeriod,sizeof(unsigned long),1,fp);
	fwrite(&pattern,sizeof(unsigned int),1,fp);
	for(auto it = irdata->pattern.begin() ; it != irdata->pattern.end() ; ++it ) 
	{
		fwrite(&it->pulseOrder,sizeof(unsigned short),1,fp);
		fwrite(&it->wait,sizeof(unsigned long),1,fp);
	}
	
	fclose(fp);
	return true;
}

bool CapeBord_EndelS::LoadIRData(IRData * irdata,const string& filename)
{
	auto fp = fopen(filename.c_str(),"rb");
	if (!fp)
	{
		return false;
	}
	unsigned int pattern = 0;
	fread((void*)&irdata->carrierPeriod,sizeof(unsigned long),1,fp);
	fread((void*)&pattern,sizeof(unsigned int),1,fp);

	irdata->pattern.clear();
	irdata->pattern.reserve(pattern);

	unsigned int i = 0;
	for( ; i < pattern ; ++i )
	{
		unsigned short pulseOrder = 0;
		unsigned long wait = 0;

		fread(&pulseOrder,sizeof(unsigned short),1,fp);
		fread(&wait,sizeof(unsigned long),1,fp);

		if (pulseOrder <= 0 || wait <= 0) break;
		irdata->pattern.push_back(IRData::IRPattern(pulseOrder,wait));
	}
	fclose(fp);

	if (pattern != i)
	{
		//データが破損している!!
		return false;
	}
	return true;
}



bool CapeBord_EndelS::checkIR(const IRData * irdata)
{
	//データが少なすぎる 4の根拠はない。
	if ( irdata->pattern.size() <= 4 ) return false;

	//pulseOrder 1が続くのはダメなデータの典型 ノイズを誤認している
	if ( irdata->pattern[0].pulseOrder <= 1 ) return false;
	if ( irdata->pattern[1].pulseOrder <= 1 ) return false;


	//OK たぶん取得できた気がする
	return true;
}

void CapeBord_EndelS::TurnOnLED()
{
	if (XLFileUtil::Exist("/sys/class/leds/beaglebone:green:usr0/brightness") )
	{//beaglebone black
		XLFileUtil::write("/sys/class/leds/beaglebone:green:usr0/brightness","1");
	}
	else if (XLFileUtil::Exist("/sys/class/leds/beaglebone::usr0/brightness") )
	{//beaglebone
//		XLFileUtil::write("/sys/class/leds/beaglebone::usr0/brightness","1");
	}
	else
	{
	}
}


void CapeBord_EndelS::TurnOffLED()
{
	if (XLFileUtil::Exist("/sys/class/leds/beaglebone:green:usr0/brightness") )
	{//beaglebone black
		XLFileUtil::write("/sys/class/leds/beaglebone:green:usr0/brightness","0");
	}
	else if (XLFileUtil::Exist("/sys/class/leds/beaglebone::usr0/brightness") )
	{//beaglebone
//		XLFileUtil::write("/sys/class/leds/beaglebone::usr0/brightness","0");
	}
	else
	{
	}
}

void CapeBord_EndelS::TurnAllOffLED()
{
	if (XLFileUtil::Exist("/sys/class/leds/beaglebone:green:usr0/brightness") )
	{//beaglebone black
		XLFileUtil::write("/sys/class/leds/beaglebone:green:usr0/brightness","0");
		XLFileUtil::write("/sys/class/leds/beaglebone:green:usr1/brightness","0");
		XLFileUtil::write("/sys/class/leds/beaglebone:green:usr2/brightness","0");
		XLFileUtil::write("/sys/class/leds/beaglebone:green:usr3/brightness","0");
	}
	else if (XLFileUtil::Exist("/sys/class/leds/beaglebone::usr0/brightness") )
	{//beaglebone
//		XLFileUtil::write("/sys/class/leds/beaglebone::usr0/brightness","0");
//		XLFileUtil::write("/sys/class/leds/beaglebone::usr1/brightness","0");
//		XLFileUtil::write("/sys/class/leds/beaglebone::usr2/brightness","0");
//		XLFileUtil::write("/sys/class/leds/beaglebone::usr3/brightness","0");
	}
	else
	{
	}
}
