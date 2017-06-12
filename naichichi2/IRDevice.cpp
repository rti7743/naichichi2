#include "common.h"
#include "IRDevice.h"
#include "MainWindow.h"
#include "IRDevice_Cape.h"

void IRDevice::Create()
{
	delete this->Device;

	const string type = MainWindow::m()->Config.Get("irdevice", "" );

	if (type == "iremocon")
	{
	}
#if _MSC_VER
	else
	{//IRデバイスを使わない
		this->Device = new IRDevice_None();
		NOTIFYLOG("IRDevice_None が選択されました");
	}
#else
	else //if (type == "endel" || type == "cape" || type == "")
	{//拡張ボードを使う
		this->Device = new IRDevice_Cape();
		NOTIFYLOG("IRDevice_Cape が選択されました");
	}
#endif
}

