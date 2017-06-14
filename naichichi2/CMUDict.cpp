//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#include"common.h"
#include"CMUDict.h"
#include"MainWindow.h"
#include"ScriptRunner.h"
#include"XLStringUtil.h"
#include"XLFileUtil.h"

CMUDict::CMUDict()
{
}
CMUDict::~CMUDict()
{
}
void CMUDict::Create(const string& filename)
{
	this->Filename = filename;

	memset(Index , 0 , sizeof(this->Index) );

	unsigned long lasttell = 0;
	unsigned int lastIndex = 9999999;
	char readbuffer[MAX_PATH];
	FILE * fp = fopen(this->Filename.c_str() ,"rb" );
	if (!fp)
	{
		throw XLEXCEPTION("CMUDictファイル" << this->Filename << "が開けません");
	}
	while(!feof(fp))
	{
		lasttell = ftell(fp);
		readbuffer[0] = 0;
		readbuffer[1] = 0;
		if ( fgets(readbuffer , MAX_PATH-1 , fp) == NULL ) break;
		readbuffer[MAX_PATH-1] = 0;

		//索引を作る
		if ( ! (( readbuffer[0] >= 'A' && readbuffer[0] <= 'Z' ) && (readbuffer[1] >= 'A' && readbuffer[1] <= 'Z' )) )
		{
			continue;
		}
		unsigned int index = ((readbuffer[0] - 'A')*26) + (readbuffer[1] - 'A');
		if (lastIndex == index)
		{
			continue;
		}
		lastIndex = index;

		ASSERT(index <= 26*26);
		this->Index[index] = lasttell;
	}

	//番兵
	lasttell = ftell(fp);
	this->Index[26*26] = lasttell;

	//歯抜けのデータを保管する
	for(int i = 26*26 ; i >= 0  ; i -- )
	{
		if ( this->Index[i] == 0 )
		{
			this->Index[i] = lasttell;
		}
		else
		{
			lasttell = this->Index[i];
		}
	}

	fclose(fp);
}

string CMUDict::GetYomi(const string& word) const
{
	if ( word.size() <= 1 ) return"";
	string str=XLStringUtil::strtoupper( word );
	const char * p = str.c_str();

	if ( ! (( p[0] >= 'A' && p[0] <= 'Z' ) && (p[1] >= 'A' && p[1] <= 'Z' )) )
	{
		return"";
	}
	unsigned int index = ((p[0] - 'A')*26) + (p[1] - 'A');
	
	unsigned long lasttell = 0;
	char readbuffer[MAX_PATH];
	FILE * fp = fopen(this->Filename.c_str() ,"rb" );
	if (!fp)
	{
		throw XLEXCEPTION("CMUDictファイル" << this->Filename << "が開けません");
	}
	//索引の場所にワープ
	fseek(fp , this->Index[index] ,0);
	unsigned long endPoint = this->Index[index+1];

	//単語のあとにスペースを入れる
	const string searchWord = str +" ";

	while(!feof(fp))
	{
		lasttell = ftell(fp);
		if (lasttell > endPoint)
		{
			break;
		}
		readbuffer[0] = 0;
		readbuffer[1] = 0;
		if ( fgets(readbuffer , MAX_PATH-1 , fp) == NULL ) break;
		readbuffer[MAX_PATH-1] = 0;

		//単語ある？
		if (strncmp(searchWord.c_str() , readbuffer , searchWord.size() ) != 0 )
		{
			continue;
		}
		const char * yomiBuffer = strchr(readbuffer,' ');
		if (yomiBuffer == NULL)
		{
			continue;
		}
		yomiBuffer++; // ' 'を飛ばす

		return XLStringUtil::chop(yomiBuffer);
	}
	fclose(fp);
	return"";
}
