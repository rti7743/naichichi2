//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#pragma once

class IRDevice_Cape : public IIRDeviceInterface
{
public:
	IRDevice_Cape();
	virtual ~IRDevice_Cape();

	bool Capture(const string& path);
	bool Radiate(const string& path);

private:
};

