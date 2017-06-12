//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
#include "common.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"
#include "XLGISAddress.h"

XLGISAddress::XLGISAddress()
{
}
XLGISAddress::~XLGISAddress()
{
}

void XLGISAddress::Create(const string& prefTSV)
{
	this->PrefTSV = prefTSV;
}

bool XLGISAddress::Parse(const string& address,struct GISResult * result) const
{
	
	//この関数はスレッドフリーです
	ASSERT___IS_THREADFREE();

	FILE * fp = fopen(this->PrefTSV.c_str() ,"rb" );
	if (!fp)
	{
		return false;
	}

	char readbuffer[MAX_PATH];
	while( ! feof(fp) )
	{
		if ( fgets(readbuffer , MAX_PATH-1 , fp) == NULL ) break;
		auto vec = XLStringUtil::split_vector("\t",_U2A(readbuffer));
		if (vec.size() < 3) continue;

		if ( address.find(vec[0]) == 0 )
		{
			result->Country = "JP";
			if (vec[0] == "東京" )	result->Pref = vec[0] + "都";
			else if (vec[0] == "大阪" ||  vec[0] == "京都" )	result->Pref = vec[0] + "府";
			else if (vec[0] == "北海道" ) result->Pref = vec[0] ;
			else result->Pref = vec[0] + "県";
			result->Lon  = atof(vec[1]);
			result->Lat  = atof(vec[2]);
			fclose(fp);
			return true;
		}
	}
	fclose(fp);
	return false;
}

SEXYTEST()
{
	{
		XLGISAddress::GISResult result;
		XLGISAddress gis;
		gis.Create("./config/testdata/pref.tsv");
		bool r = gis.Parse("となりのととろ",&result);
		SEXYTEST_EQ(r,false);
	}
	{
		XLGISAddress::GISResult result;
		XLGISAddress gis;
		gis.Create("./config/testdata/pref.tsv");
		bool r = gis.Parse("京都",&result);
		SEXYTEST_EQ(r,true);
		SEXYTEST_EQ(result.Country , "JP");
		SEXYTEST_EQ(result.Lon , 135.44118f);
		SEXYTEST_EQ(result.Lat , 35.25432f);
		SEXYTEST_EQ(result.Pref , "京都府");
	}
	{
		XLGISAddress::GISResult result;
		XLGISAddress gis;
		gis.Create("./config/testdata/pref.tsv");
		bool r = gis.Parse("東京都",&result);
		SEXYTEST_EQ(r,true);
		SEXYTEST_EQ(result.Country , "JP");
		SEXYTEST_EQ(result.Lon , 139.691711f);
		SEXYTEST_EQ(result.Lat , 35.689499f);
		SEXYTEST_EQ(result.Pref , "東京都");
	}
	{
		XLGISAddress::GISResult result;
		XLGISAddress gis;
		gis.Create("./config/testdata/pref.tsv");
		bool r = gis.Parse("東京都台東区",&result);
		SEXYTEST_EQ(r,true);
		SEXYTEST_EQ(result.Country , "JP");
		SEXYTEST_EQ(result.Lon , 139.691711f);
		SEXYTEST_EQ(result.Lat , 35.689499f);
		SEXYTEST_EQ(result.Pref , "東京都");
	}
	{
		XLGISAddress::GISResult result;
		XLGISAddress gis;
		gis.Create("./config/testdata/pref.tsv");
		bool r = gis.Parse("北海道",&result);
		SEXYTEST_EQ(r,true);
		SEXYTEST_EQ(result.Country , "JP");
		SEXYTEST_EQ(result.Lon , 141.34660f);
		SEXYTEST_EQ(result.Lat , 43.06451f);
		SEXYTEST_EQ(result.Pref , "北海道");
	}
}
#endif // WITH_CLIENT_ONLY_CODE==1