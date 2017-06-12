#pragma once

//dll読み込みヘルパー
class XLLoadLibraryHelper
{
private:
	//DLL インスタンス.
	HMODULE DllInstance;

public:
	XLLoadLibraryHelper()
	{
		this->DllInstance = NULL;
	}
	virtual ~XLLoadLibraryHelper()
	{
		if (this->DllInstance != NULL)
		{
			::FreeLibrary(this->DllInstance);
			this->DllInstance = NULL;
		}
	}
	bool Load( const string& inDLLName )//std読んでいない化石環境とかのために const char* で作る.
	{
		assert(this->DllInstance == NULL);

		this->DllInstance = ::LoadLibraryA(inDLLName.c_str() );
		if ( this->DllInstance == NULL )
		{
			DWORD lastError = ::GetLastError();
			throw XLException(string() + "ライブラリ" + inDLLName + "を読み込めませんでした",lastError);
		}
		return true;
	}
	FARPROC GetProcAddress(const char* inProcName)
	{
		assert(this->DllInstance != NULL);
		return ::GetProcAddress(this->DllInstance,inProcName);
	}
};
