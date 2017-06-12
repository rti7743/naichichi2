//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include "IRDevice.h"
#include "MainWindow.h"
#include "IRDevice_Cape.h"


IRDevice_Cape::IRDevice_Cape()
{
}

IRDevice_Cape::~IRDevice_Cape()
{
}

bool IRDevice_Cape::Capture(const string& path)
{
	return MainWindow::m()->CapeBord.Capture(path) ;
}

bool IRDevice_Cape::Radiate(const string& path)
{
	return MainWindow::m()->CapeBord.Radiate(path) ;
}
#endif