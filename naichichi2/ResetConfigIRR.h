#pragma once

class ResetConfigIRR : public IResetConfigInterface
{
private:
	void ThreadMain();
private:
    thread*					Thread;
	mutable mutex lock;
	bool StopFlag;

	int GPIOFD;
public:
	ResetConfigIRR();
	virtual ~ResetConfigIRR();

	virtual void Create();
	virtual void Stop();
private:
	int FireFactoryReset();

};
