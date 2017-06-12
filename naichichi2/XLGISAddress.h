//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1

//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#pragma once

#pragma once
#include <vector>
#include <string>


class XLGISAddress
{
public:
	struct GISResult
	{
		string		Country;	//国 JP..
		string		Pref;		//県
	//	string		City;		//地方自治体	//今は使わない
	//	string		Town;		//				//今は使わない
	//	string		Local;		//				//今は使わない
	//	string		Building;	//				//今は使わない
	//	string		Zipcode;	//				//今は使わない
		float			Lat;		//経緯度
		float			Lon;		//
	};
	XLGISAddress();
	virtual ~XLGISAddress();

	void Create(const string& prefTSV);
	bool Parse(const string& address,struct GISResult * result) const;
private:
	string PrefTSV;
};
#endif