#pragma once

class ColudSyncConfig
{
public:

	struct uploadfiles {
		uploadfiles() : time(0) , isupload(false) , islocalexist(false)
		{
		}
		uploadfiles(const string& type,const string& filename,time_t time,bool isupload,bool islocalexist)
			: type(type) ,filename(filename) ,time(time) , isupload(isupload) , islocalexist(islocalexist)
		{
		}

		string type;
		string filename;
		time_t time;
		bool   isupload;
		bool   islocalexist;
	};


	ColudSyncConfig();
	virtual ~ColudSyncConfig();

	void Create(const string& serverurl);
	void Stop();
	void WakeupByNewSyncID(const string& linkid);
	void WakeupByconfig();
	void WakeupByconfigOnly();
	bool IsSync() const;
	
private:
	void ThreadMain();
	void SyncAll();
	void SyncConfgOnly();


	string SyncConfig();

	void V2SyncDirecotry(const string type,map<string , ColudSyncConfig::uploadfiles>* timetable);
	string V2SyncFile(const string& type,const string& fullfilename ,const string& filename , time_t time);
	string V2SyncDel(const string& type,const string& filename );

private:
    thread*					Thread;
	mutable condition_variable EventObject;
	mutable mutex lock;
	bool StopFlag;
	bool isSync;
	enum SyncTargetEnum
	{
		 SyncTarget_Full
		,SyncTarget_ConfigOnly
	};
	SyncTargetEnum SyncTarget;

	string linkid;
	string serverurl;
};
