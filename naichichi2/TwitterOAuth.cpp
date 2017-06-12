//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#include "common.h"
#include "TwitterOAuth.h"
#include "XLFileUtil.h"
#include "XLHttpSocket.h"
#include "XLStringUtil.h"
#include "../liboauthcpp/include/liboauthcpp/liboauthcpp.h"
#if _MSC_VER
	#pragma comment(lib, "liboauthcpp.lib")
#endif


const string consumer_key = "<<YOUR KEY>>"; // Key from Twitter
const string consumer_secret = "<<YOUR TWITTER SECRET>>"; // Secret from Twitter
const string request_token_url = "https://api.twitter.com/oauth/request_token";
const string authorize_url = "https://api.twitter.com/oauth/authorize";
const string access_token_url = "https://api.twitter.com/oauth/access_token";
const string oauth_update_url = "https://api.twitter.com/1.1/statuses/update.json";
const string oauth_hometimeline_url = "https://api.twitter.com/1.1/statuses/home_timeline.json";
const string oauth_usertimeline_url = "https://api.twitter.com/1.1/statuses/user_timeline.json";


bool TwitterOAuth::Auth(const string& callback_url
	,string* oauth_token
	,string* oauth_token_secret
	,string* oauth_redirect_url
	)
{
	const string request_token_query_args = "oauth_callback="+ XLStringUtil::URLENCODE(callback_url) ;

	OAuth::Consumer consumer(consumer_key, consumer_secret);
    OAuth::Client oauth(&consumer);

    // Step 1: Get a request token. This is a temporary token that is used for
    // having the user authorize an access token and to sign the request to
    // obtain said access token.
    const string base_request_token_url = request_token_url + (request_token_query_args.empty() ? string("") : (string("?")+request_token_query_args) );
    const string oAuthQueryString =
        oauth.getURLQueryString( OAuth::Http::Get, base_request_token_url);

	const string url = request_token_url+"?"+oAuthQueryString;
	const string twresult = XLHttpSocket::Get(url , 2 );
	const auto tempMap = XLStringUtil::crosssplit("&","=",twresult );
	*oauth_token = mapfind(tempMap,"oauth_token","");
	*oauth_token_secret = mapfind(tempMap,"oauth_token_secret","");
	
	if (oauth_token->empty())
	{
		this->ErrorMessage = "twitterからの応答("+ twresult +")が変です";
		return false;
	}

	*oauth_redirect_url = authorize_url + "?oauth_token=" + *oauth_token ;
	return true;
}

//認証の戻り先、もう一度データを投げて、キーを確立する
bool TwitterOAuth::AuthCallback(const string& oauth_token
,const string& oauth_token_secret
,const string& oauth_verifier
,string* oauth_access_token
,string* oauth_access_token_secret
,string* user_id
,string* screen_name
)
{
	OAuth::Consumer consumer(consumer_key, consumer_secret);
 
	if (oauth_verifier.empty())
	{
		this->ErrorMessage = "twitterからのコールバック(oauth_verifier)が変です";
		return false;
	}

	OAuth::Token request_token(oauth_token,oauth_token_secret);
    request_token.setPin(oauth_verifier);

    // Step 3: Once the consumer has redirected the user back to the
    // oauth_callback URL you can request the access token the user
    // has approved. You use the request token to sign this
    // request. After this is done you throw away the request token
    // and use the access token returned. You should store the oauth
    // token and token secret somewhere safe, like a database, for
    // future use.
    OAuth::Client oauth = OAuth::Client(&consumer, &request_token);
    // Note that we explicitly specify an empty body here (it's a GET) so we can
    // also specify to include the oauth_verifier parameter
    string oAuthQueryString = oauth.getURLQueryString( OAuth::Http::Get, access_token_url, string( "" ), true );
    string url = access_token_url + "?" + oAuthQueryString;

	const string twresult = XLHttpSocket::Get(url , 2 );
	const auto tempMap = XLStringUtil::crosssplit("&","=",twresult );
	*oauth_access_token = mapfind(tempMap,"oauth_token","");
	*oauth_access_token_secret = mapfind(tempMap,"oauth_token_secret","");
	*user_id = mapfind(tempMap,"user_id","");
	*screen_name = mapfind(tempMap,"screen_name","");

	if (oauth_access_token->empty())
	{
		this->ErrorMessage = "twitterからの応答("+ twresult +")が変です";
		return false;
	}

	return true;
}

bool TwitterOAuth::Tweet(
const string message
,const string& oauth_access_token
,const string& oauth_access_token_secret
)
{
	OAuth::Consumer consumer(consumer_key, consumer_secret);
	OAuth::Token token(oauth_access_token, oauth_access_token_secret);
	OAuth::Client oauth(&consumer, &token);

	string oauth_protected_resource_params = "status=" + XLStringUtil::URLENCODE(message);
	string oAuthQueryString =
		oauth.getURLQueryString(OAuth::Http::Post, oauth_update_url + "?" + oauth_protected_resource_params);
	const string twresult = XLHttpSocket::Post(oauth_update_url , 2 ,oAuthQueryString.c_str(),oAuthQueryString.size() );
	
	return true;
}

string TwitterOAuth::HomeTimeLine(
const string& screen_name
,const string& oauth_access_token
,const string& oauth_access_token_secret
)
{
	OAuth::Consumer consumer(consumer_key, consumer_secret);
	OAuth::Token token(oauth_access_token, oauth_access_token_secret);
	OAuth::Client oauth(&consumer, &token);

	string oauth_protected_resource_params = "";
	if(!screen_name.empty())
	{
		oauth_protected_resource_params="screen_name="+XLStringUtil::URLENCODE(screen_name);
	}

	string oAuthQueryString =
		oauth.getURLQueryString(OAuth::Http::Get, oauth_hometimeline_url + "?" + oauth_protected_resource_params);
	const string twresult = XLHttpSocket::Get(oauth_hometimeline_url+ "?" + oAuthQueryString, 2  );
	
	return twresult;
}
string TwitterOAuth::UserTimeLine(
const string& screen_name
,const string& oauth_access_token
,const string& oauth_access_token_secret
)
{
	OAuth::Consumer consumer(consumer_key, consumer_secret);
	OAuth::Token token(oauth_access_token, oauth_access_token_secret);
	OAuth::Client oauth(&consumer, &token);


	string oauth_protected_resource_params = "";
	if(!screen_name.empty())
	{
		oauth_protected_resource_params="screen_name="+XLStringUtil::URLENCODE(screen_name);
	}

	string oAuthQueryString =
		oauth.getURLQueryString(OAuth::Http::Get, oauth_usertimeline_url + "?" + oauth_protected_resource_params);
	const string twresult = XLHttpSocket::Get(oauth_usertimeline_url+ "?" + oAuthQueryString, 2  );
	
	return twresult;
}

//twitterが返すjsonは長すぎるので適当に要約します.
void TwitterOAuth::ParseTwitterTimelineResultToStringMapVec(const string& twitterResult,vector< map<string,string>* >* outTimelineMapVec) const
{
	vector<string> vec = XLStringUtil::split_vector("},{",twitterResult);
	for(auto it = vec.begin() ; it != vec.end() ; it ++ )
	{
		map<string,string>* m = new map<string,string>;
		(*m)["created_at"] = XLStringUtil::findjsonvalue(*it,"created_at");
		(*m)["id"] = XLStringUtil::findjsonvalue(*it,"id");
		(*m)["text"] = XLStringUtil::ConvertTextUnicodeUToBinary(XLStringUtil::findjsonvalue(*it,"text"));
		(*m)["screen_name"] = XLStringUtil::findjsonvalue(*it,"screen_name");

		outTimelineMapVec->push_back(m);
	}
}

