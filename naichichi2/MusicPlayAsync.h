// MusicPlayAsync.h: MusicPlayAsync クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MusicPlayAsync_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_MusicPlayAsync_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct MusicPlayAsyncTask
{
	CallbackPP callback;
	string filename;
	unsigned int playcount;

	MusicPlayAsyncTask(const CallbackPP& callback,const string& filename,unsigned int playcount)
		: callback(callback),filename(filename),playcount(playcount)
	{

	}
	MusicPlayAsyncTask()
	{
	}
};

class MusicPlayAsync
{
public:
	MusicPlayAsync();
	virtual ~MusicPlayAsync	();

	//音声のためのオブジェクトの構築.
	bool Create();
	bool Play(const CallbackPP& callback,const string & filename,unsigned int loop);
	bool Play(const string & filename,unsigned int loop);
	bool PlaySync(const string & filename);
	bool PlaySyncLow(const string & fullpath);
	void StopPlay();

	bool RemoveCallback(const CallbackPP& callback , bool is_unrefCallback) ;
	bool IsExist(const string & filename);

private:
	void Run();

	//夜間ボリュームを使いますか？
	bool IsUseNightVolume() const;
	//今は、夜間ボリュームの夜間時間帯ですか？
	bool IsUseNightVolume(const time_t& now) const;

	list<MusicPlayAsyncTask> PlayQueue;

	thread* Thread;
	mutex   Lock;
	mutex   PlayLock;
	condition_variable EventObject;
	bool           StopFlag;
	//再生停止フラグ
	bool           StopPlayFlag;
};

#endif // !defined(AFX_MusicPlayAsync_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
