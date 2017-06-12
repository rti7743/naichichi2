#pragma once

#include "ColudSyncConfig.h"

class ColudSyncPooling
{
public:
	ColudSyncPooling();
	virtual ~ColudSyncPooling();

	void Create(const string& serverurl,const string& id,const string& pass,const string& uuid,bool isSyncServerOK,const string& linkid);
	void Stop();
	void WakeupByNewSyncID(const string& id,const string& pass,const string& uuid,bool isSyncServerOK);
	void WakeupSyncConfigByconfig();
	void WakeupSyncConfigByconfigOnly();

	//Configを変更した場合
	void ConfigUpdated();
	//Configを変更した場合 画像も同期する場合
	void ConfigUpdatedFull();
	
	bool IsSync() const { this->isSync; } 

	string getLinkID() const;

	string OpeartionChangeRegist(const string& old_id,const string& old_password,const string& new_id,const string& new_password,const string& uuid,const string& is_uuid_overriade) const;
	string OpeartionCheckRegist(const string& id,const string& password,const string& uuid,const string& is_uuid_overriade) const;
	string OpeartionWeAreHere(const string& uuid,const string& url) const;
	string OpeartionGetURLByPassword(const string id,const string password) const;
	string OpeartionGetURLByUUID(const string id,const string uuid) const;

	//syncが完了するまで待ちます.
	bool SyncJoin(unsigned int waitSec) const;

private:
	void ThreadMain();
	bool RunOpline(const string& opline);
	void Wait(unsigned int second) ;
	string OpeartionGetLink(const string& id,const string& pass,const string& uuid,const string& linkid) const;
	string OpeartionPooling(const string& linkid) const;

private:
    thread*					Thread;
	mutable mutex lock;
	mutable condition_variable	EventObject;
	bool StopFlag;

	string id;
	string pass;
	string uuid;
	bool isSyncServerOK;	//サーバーとsyncしていいかどうか.
	string linkid;	//サーバーとのクッキー.
	bool isSync;	//syncできたかどうかのフラグ.
	bool isWakeupFlag;	//起こすフラグ(この書き方はあまりいけていない・・・)

	string serverurl;
	ColudSyncConfig SyncConfig;
};
