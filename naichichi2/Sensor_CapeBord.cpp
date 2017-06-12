//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include "Sensor_Factory.h"
#include "Sensor_CapeBord.h"
#include "MainWindow.h"
#include "sexylog.h"
#if _MSC_VER
#else
#include "CapeBord.h"
#include <sys/sysinfo.h>
#endif
#include "XLFileUtil.h"

Sensor_CapeBord::Sensor_CapeBord()
{
}
Sensor_CapeBord::~Sensor_CapeBord()
{
}

void Sensor_CapeBord::Create(const string& option) 
{
}

//ケース内の温度上昇を考慮した温度補正
float Sensor_CapeBord::Temp2Temp(float temp) const
{
#if _MSC_VER
	return 0;
#else
	//だいたい30分ぐらいたつと +5.9度ぐらい影響を受ける
	//それ以降はほぼ同じ
	struct sysinfo info;
	sysinfo(&info);

	float hosei ;
	if (info.uptime >= 30*60)
	{
		hosei =  5.9;
	}
	else
	{
		hosei = (5.9 * info.uptime) / (float)1800;
	}

	DEBUGLOG("temp:" << temp << " uptime:" << info.uptime << " hosei:" << hosei << " --> " << temp - hosei);
	return temp - hosei;
#endif
}

float Sensor_CapeBord::getTemperature() 
{
#if _MSC_VER
	return 0;
#else
	return Temp2Temp(MainWindow::m()->CapeBord.getTemp());
#endif
}

float Sensor_CapeBord::getLuminous() 
{
#if _MSC_VER
	return 0;
#else
	return MainWindow::m()->CapeBord.getLumi();
#endif
}

float Sensor_CapeBord::getHumidity() 
{
	return 0;
}

#endif //WITH_CLIENT_ONLY_CODE==1
