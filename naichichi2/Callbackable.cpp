//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#include "common.h"
#include "Callbackable.h"

static unsigned int g_CallbackPPObjectCounter = 1;

CallbackPP CallbackPP::New(std::function<void (void) > f)
{
	CallbackPP pp;
	pp.ObjectID = g_CallbackPPObjectCounter++;
	pp.func = f;

	return pp;
}
