#pragma once
class TwitterOAuth
{
public:
	//認証開始
	bool Auth(const string& callback_url
	,string* oauth_token
	,string* oauth_token_secret
	,string* oauth_redirect_url
	);

	//認証の戻り先、もう一度データを投げて、キーを確立する
	bool AuthCallback(const string& oauth_token
	,const string& oauth_token_secret
	,const string& oauth_verifier
	,string* oauth_access_token
	,string* oauth_access_token_secret
	,string* user_id
	,string* screen_name
	);
	
	//ツイート
	bool Tweet(
	const string message
	,const string& oauth_access_token
	,const string& oauth_access_token_secret
	);
	string HomeTimeLine(
	 const string& screen_name
	,const string& oauth_access_token
	,const string& oauth_access_token_secret
	);
	string UserTimeLine(
	 const string& screen_name
	,const string& oauth_access_token
	,const string& oauth_access_token_secret
	);
	
	
	string what() const
	{
		return this->ErrorMessage;
	}

	//twitterが返すjsonは長すぎるので適当に要約します.
	void ParseTwitterTimelineResultToStringMapVec(
	const string& twitterResult
	,vector< map<string,string>* >* outTimelineMapVec) const;

private:
	string ErrorMessage;
};
