//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#pragma once
//拡張ボードのインターフェース
struct IIRDeviceInterface
{
	virtual bool Capture(const string& path) = 0;
	virtual bool Radiate(const string& path) = 0;
};

struct IRDevice_None : public IIRDeviceInterface
{
	virtual bool Capture(const string& path) { return false; }
	virtual bool Radiate(const string& path) { return false; }
};


class IRDevice
{
public:
	IRDevice()
	{
		this->Device = NULL;
	}
	virtual ~IRDevice()
	{
		delete this->Device;
		this->Device = NULL;
	}

	void Create();

	bool Capture(const string& path)
	{
		return this->Device->Capture(path);
	}
	bool Radiate(const string& path)
	{
		return this->Device->Radiate(path);
	}

private:
	struct IIRDeviceInterface* Device;
};
