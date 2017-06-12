#pragma once

class MultiRoom
{
public:
	MultiRoom();
	virtual ~MultiRoom();

	void Create();
	void Stop();

	void startSync();

private:
	void ThreadMain();
private:
    thread*					Thread;
	mutable condition_variable EventObject;
	mutable mutex lock;
	bool StopFlag;

	//自分の部屋がなければ作るし、
	//あって情報が更新されていた場合、
	//syncしなおす
	bool Touch();
	//同期処理
	void Sync();

};
