#pragma once
#if _MSC_VER
//windows でも UTF8で快適にコーディングできるようにする!!


namespace windows_encoding
{
	const int _CONVERT_BUFFER_SIZE = 65535;

	static std::wstring _atou16_convert(const char* inSrc)
	{
		std::vector<wchar_t> outBuffer(_CONVERT_BUFFER_SIZE);

		outBuffer[0] = 0;
		::MultiByteToWideChar(CP_ACP,0,inSrc,-1,&outBuffer[0],_CONVERT_BUFFER_SIZE);

		return &outBuffer[0];
	}
	static std::wstring _atou16_convert(const string&  inSrc)
	{
		return _atou16_convert(inSrc.c_str());
	}

	static std::string _u16toa_convert(const wchar_t* inSrc)
	{
		std::vector<char> outBuffer(_CONVERT_BUFFER_SIZE);

		outBuffer[0] = 0;
		::WideCharToMultiByte(CP_ACP,0,inSrc,-1,&outBuffer[0],_CONVERT_BUFFER_SIZE , NULL,NULL);

		return &outBuffer[0];
	}
	static std::string _u16toa_convert(const std::wstring&  inSrc)
	{
		return _u16toa_convert(inSrc.c_str());
	}

	static std::string _u16tou8_convert(const wchar_t* inSrc)
	{
		std::vector<char> outBuffer(_CONVERT_BUFFER_SIZE);

		outBuffer[0] = 0;
		::WideCharToMultiByte(CP_UTF8,0,inSrc,-1,&outBuffer[0],_CONVERT_BUFFER_SIZE , NULL,NULL);

		return &outBuffer[0];
	}
	static std::string _u16tou8_convert(const std::wstring&  inSrc)
	{
		return _u16tou8_convert(inSrc.c_str());
	}

	static std::wstring _u8tou16_convert(const char* inSrc)
	{
		std::vector<wchar_t> outBuffer(_CONVERT_BUFFER_SIZE);

		outBuffer[0] = 0;
		::MultiByteToWideChar(CP_UTF8,0,inSrc,-1,&outBuffer[0],_CONVERT_BUFFER_SIZE);

		return &outBuffer[0];
	}
	static std::wstring _u8tou16_convert(const string&  inSrc)
	{
		return _u8tou16_convert(inSrc.c_str());
	}

	static std::string _atou8_convert(const char * inSrc)
	{
		//ascii -> utf16に変換
		const std::wstring temp16str = _atou16_convert(inSrc );

		//utf16 -> utf8に変換
		return _u16tou8_convert(temp16str);
	}
	static std::string _atou8_convert(const string&  inSrc)
	{
		return _atou8_convert(inSrc.c_str());
	}

	static std::string _u8toa_convert(const char* inSrc)
	{
		//utf8 -> utf16に変換
		const std::wstring temp16str = _u8tou16_convert(inSrc);

		//utf16 -> asciiに変換
		return _u16toa_convert(temp16str);
	}
	static std::string _u8toa_convert(const std::string&  inSrc)
	{
		return _u8toa_convert(inSrc.c_str());
	}

};

//ascii -> utf16
#define _A2W(lpa) windows_encoding::_atou16_convert(lpa).c_str()

//utf16 -> acsii
#define _W2A(lpa) windows_encoding::_u16toa_convert(lpa).c_str()

//ascii -> utf8
#define _A2U(lpa) windows_encoding::_atou8_convert(lpa).c_str()

//utf8 -> ascii
#define _U2A(lpa) windows_encoding::_u8toa_convert(lpa).c_str()

//utf8 -> utf16
#define _U2W(lpa) windows_encoding::_u8tou16_convert(lpa).c_str()

//utf16 -> utf8
#define _W2U(lpa) windows_encoding::_u16tou8_convert(lpa).c_str()



/*
使い方
void hoge()
{
	const char * sjis = "アスキー"; 	//SJIS
//	UTF8が必要な関数(sjis);     		//UTF8にしたい・・・

					//おまじない
	UTF8が必要な関数(_U2A(sjis));		//UTF8に変換して利用
}


void hoge2()
{
	const char * sjis = "アスキー"; //SJIS

					//おまじない
	UTF8が必要な関数(_A2U(sjis));		//SJIS -> UTF8に変換して利用
	UTF16が必要な関数(_A2W(sjis));		//SJIS -> UTF16変換して利用

	const char * utf8 = "UTF8な文字列";	//UTF8
	SJISが必要な関数(_U2A(sjis));		//UTF8 -> SJISに変換
	UTF16が必要な関数(_U2W(sjis));		//UTF8 -> UTF16変換して利用
}
*/

#else  //_WINDOWS

//windows以外だとすべて無効にする.

//ascii -> utf16
#define _A2W(lpa)  lpa
//utf16 -> acsii
#define _W2A(lpa)  lpa

//ascii -> utf8
#define _A2U(lpa)  lpa
//utf8 -> ascii
#define _U2A(lpa)  lpa

//utf8 -> utf16
#define _U2W(lpa)  lpa
//utf16 -> utf8
#define _W2U(lpa)  lpa

#endif //_WINDOWS