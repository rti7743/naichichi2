// XLHttpHeader.cpp: XLHttpHeader クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "XLHttpHeader.h"
#include "XLStringUtil.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

XLHttpHeader::XLHttpHeader()
{
	this->HeaderSize = 0;
}

XLHttpHeader::~XLHttpHeader()
{
	for(auto it = this->Files.begin() ; it != this->Files.end() ; ++it)
	{
		delete it->second;
	}
}

bool XLHttpHeader::Parse(const char * inHeader ,unsigned int size)
{
	
	this->HeaderSize = 0;

	const char * p = inHeader;
	const char * endP = inHeader + size;
	//最初のヘッダー
	{
		int firstHeadSpaceCount = 0;
		for( const char * start = p; p < endP; ++p )
		{
			if (*p == '\r' || *p == '\n')
			{
				this->FirstHeader[2] = XLStringUtil::strtoupper( string(start , 0 , (int)(p - start)) );
				if (*p == '\r' && *(p+1) == '\n')
				{
					p++;
				}
				break;
			}
			else if (*p == ' ')
			{
				if (firstHeadSpaceCount >= 2)
				{
					continue;
				}

				this->FirstHeader[firstHeadSpaceCount] = string(start , 0 , (int)(p - start));
				firstHeadSpaceCount++;
				start = p + 1;
			}
		}
	}

	//2行位以降のヘッダー
	while(*p)
	{
		//改行までシーク.
		const char * sep = NULL;
		const char * value_start = NULL;
		for( const char * start = p; p < endP ; ++p )
		{
			//複数行にまたがっているヘッダーは考慮しないことにするw
			if (*p == '\r' || *p == '\n')
			{
				if (p != start && sep != NULL)
				{
					if (value_start == NULL)
					{
						 value_start = p;
					}
					string key = XLStringUtil::strtolower( string(start , 0, (int)(sep - start)) );
					string value = string(value_start , 0, (int)(p - value_start));
					if (Header.find(key) == Header.end())
					{
						this->Header[key] = value;
					}
					else
					{
						this->Header[key] += value;
					}
				}

				if (*p == '\r' && *(p+1) == '\n')
				{
					p++;
				}
				break;
			}
			else if (sep == NULL && *p == ':')
			{
				sep = p;
			}
			else if (sep != NULL && value_start == NULL && (*p != ' ' && *p != '\t'))
			{
				value_start = p;
			}
		}

		//次のヘッダへ
		p++;

		//ヘッダー終端チェック
		if (*p == '\r' || *p == '\n')
		{
			if (*p == '\r' && *(p+1) == '\n')
			{
				p+=2;
			}
			else
			{
				p++;
			}
			this->HeaderSize = (unsigned int)(p - inHeader) ;
			break;
		}
	}

	//GETパース
	{
		int getsep = this->FirstHeader[1].find("?");
		if (getsep >= 0)
		{
			const string str = this->FirstHeader[1].substr(getsep+1);
			this->Get.clear();
			const auto tempMap = XLStringUtil::crosssplit("&","=",str );
			for(auto it = tempMap.begin() ; it != tempMap.end() ; ++it )
			{
				this->Get[_U2A(XLStringUtil::urldecode(it->first))] = _U2A(XLStringUtil::urldecode(it->second));
			}
			this->FirstHeader[1] = _U2A(XLStringUtil::urldecode(this->FirstHeader[1].substr(0,getsep)));
		}
		else
		{//get引数はないがURLのencodeing は解除しておく
			this->FirstHeader[1] = _U2A(XLStringUtil::urldecode(this->FirstHeader[1]));
		}
	}
	return true;
}

bool XLHttpHeader::MultipartBounderParse( const char * inBody ,unsigned int size )
{
	

	const string contenttype =  getAt("content-type");
	if (! strstr(contenttype.c_str() , "multipart/form-data")  )
	{
		return false;
	}

	//普通じゃないPOSTパース multipart/form-data
	const char * bounder = strstr( contenttype.c_str() , "boundary=");
	if (!bounder)
	{
		this->Post.clear();
	}
	else
	{
		bounder += (sizeof("boundary=") - 1);
		string fullbounder = string("--") + bounder; //bounder は -- を余計につける
		const char * endP = inBody + size;
		const char * p = inBody;
		while(p < endP)
		{
			const char * nextbounder = (const char *) 
				memmem(p ,endP-p, fullbounder.c_str(), fullbounder.size() );
			if (!nextbounder)
			{//bounder がない
				nextbounder = endP;
			}
			if (nextbounder > p)
			{
				// p ～ nextbounder の間の解析
				map<string,string> innerheader;
				while(p < nextbounder)
				{
					//改行までシーク.
					const char * sep = NULL;
					const char * value_start = NULL;
					for( const char * start = p; p < nextbounder ; ++p )
					{
						//複数行にまたがっているヘッダーは考慮しないことにする
						if (*p == '\r' || *p == '\n')
						{
							if (p != start && sep != NULL)
							{
								if (value_start == NULL)
								{
									 value_start = p;
								}
								string key = XLStringUtil::strtolower( string(start , 0, (int)(sep - start)) );
								string value = string(value_start , 0, (int)(p - value_start));
								if (innerheader.find(key) == innerheader.end())
								{
									innerheader[key] = value;
								}
								else
								{
									innerheader[key] += value;
								}
							}

							if (*p == '\r' && *(p+1) == '\n')
							{
								p++;
							}
							break;
						}
						else if (sep == NULL && *p == ':')
						{
							sep = p;
						}
						else if (sep != NULL && value_start == NULL && (*p != ' ' && *p != '\t'))
						{
							value_start = p;
						}
					}

					//次のヘッダへ
					p++;

					//ヘッダー終端チェック
					if (*p == '\r' || *p == '\n')
					{
						if (*p == '\r' && *(p+1) == '\n')
						{
							p+=2;
						}
						else
						{
							p++;
						}
						break;
					}
				}
				//次のデータの前に改行があるので読み飛ばす.
				const char * enddataP = nextbounder;
				if (enddataP-2 >= p )
				{
					if ( *(enddataP-2) == '\r' && *(enddataP-1) == '\n')
					{
						enddataP = enddataP - 2;
					}
					else if ( *(enddataP-1) == '\r' || *(enddataP-1) == '\n')
					{
						enddataP = enddataP - 1;
					}
				}
				else if (enddataP-1 >= p )
				{
					if ( *(enddataP-1) == '\r' || *(enddataP-1) == '\n')
					{
						enddataP = enddataP - 1;
					}
				}

				//content-disposition から、 name と filename を取得する.
				const string contentDisposition = mapfind(innerheader,"content-disposition"); 
				const auto contentDispositionMap = XLStringUtil::crosssplitChop(";","=", contentDisposition ); 
				const string name = _U2A( XLStringUtil::urldecode( XLStringUtil::dequote( mapfind(contentDispositionMap,"name","nanashi") ) ) );

				const string contentType = mapfind(innerheader,"content-type"); 
				if (contentType.empty())
				{
					this->Post[name] = string(p, enddataP);
				}
				else
				{
					auto alreadyit = this->Files.find(name);
					if (alreadyit != this->Files.end())
					{
						delete alreadyit->second;
						this->Files.erase(alreadyit);
					}

					__file_struct* filestruct = new __file_struct;
					filestruct->data.insert(filestruct->data.end() , p, enddataP);
					filestruct->filename = _U2A( XLStringUtil::urldecode( XLStringUtil::dequote( mapfind(contentDispositionMap,"filename","nanachisan") ) ) );
					filestruct->mime = innerheader["content-type"];

					this->Files[name] = filestruct;
				}
			}

			//次のbounderを探す
			p = nextbounder + fullbounder.size();
			if (p > endP -2)
			{
				break;
			}
			if (p[0] == '-' && p[1] == '-')
			{//終端
				break;
			}
			if (p[0] == '\r')
			{
				p ++;
				if (p[0] == '\n') p++;
			}
			else if (p[0] == '\n')
			{
				p ++;
			}
			else
			{//よくわからない bounder!!
				break;
			}
		}
	}
	return ! this->Files.empty();
}
SEXYTEST()
{
	string str = 
	"POST / HTTP/1.0\r\n"
	"Accept-Language:ja,en-US;q=0.8,en;q=0.6\r\n"
	"Cache-Control:max-age=0\r\n"
	"Connection:keep-alive\r\n"
	"Content-Length:455\r\n"
	"Content-Type:multipart/form-data; boundary=----WebKitFormBoundaryn6cbKz11e2ul2lvN\r\n"
	"Cookie:naichichi2=S-e_WYgNrQUc1JhOfWiaMXBVRlH0hwyMImJdcXsPxXv7NkotcJHpkCmr6hhz3xoB41305\r\n"
	"Host:127.0.0.1\r\n"
	"\r\n"
	"------WebKitFormBoundaryn6cbKz11e2ul2lvN\r\n"
	"Content-Disposition: form-data; name=\"uploadfile1\"; filename=\"123.png\"\r\n"
	"Content-Type: image/png\r\n"
	"\r\n"
	"123456\r\n"
	"------WebKitFormBoundaryn6cbKz11e2ul2lvN\r\n"
	"Content-Disposition: form-data; name=\"typepath\"\r\n"
	"\r\n"
	"elecicon\r\n"
	"------WebKitFormBoundaryn6cbKz11e2ul2lvN\r\n"
	"Content-Disposition: form-data; name=\"_csrftoken\"\r\n"
	"\r\n"
	"c624b426c5290df8cdd6bb83b2e093b36e2d254d\r\n"
	"------WebKitFormBoundaryn6cbKz11e2ul2lvN--\r\n"
	;
	XLHttpHeader header;
	header.Parse(str);

	string body = str.substr(header.getHeaderSize());
	header.PostParse(body.c_str(),body.size());  

	auto post = header.getPostPointer();
	string r = mapfind(post,"typepath");
	ASSERT(r == "elecicon");

	r = mapfind(post,"_csrftoken");
	ASSERT(r == "c624b426c5290df8cdd6bb83b2e093b36e2d254d");

	auto request = header.getRequest();
	r = mapfind(request,"_csrftoken");
	ASSERT(r == "c624b426c5290df8cdd6bb83b2e093b36e2d254d");
}

/*
SEXYTEST()
{
	string str = 
	"POST / HTTP/1.0\r\n"
	"Accept-Language:ja,en-US;q=0.8,en;q=0.6\r\n"
	"Cache-Control:max-age=0\r\n"
	"Connection:keep-alive\r\n"
	"Content-Length:455\r\n"
	"Content-Type:multipart/form-data; boundary=----WebKitFormBoundaryQ5J3YRE0jlAbPq7X\r\n"
	"Cookie:naichichi2=S-e_WYgNrQUc1JhOfWiaMXBVRlH0hwyMImJdcXsPxXv7NkotcJHpkCmr6hhz3xoB41305\r\n"
	"Host:127.0.0.1\r\n"
	"\r\n"
	"------WebKitFormBoundaryQ5J3YRE0jlAbPq7X\r\n"
	"Content-Disposition: form-data; name=\"uploadfile1\"; filename=\"a.php\"\r\n"
	"Content-Type: application/octet-stream\r\n"
	"\r\n"
	"\r\n"
	"------WebKitFormBoundaryQ5J3YRE0jlAbPq7X\r\n"
	"Content-Disposition: form-data; name=\"typepath\"\r\n"
	"\r\n"
	"elecicon\r\n"
	"------WebKitFormBoundaryQ5J3YRE0jlAbPq7X\r\n"
	"Content-Disposition: form-data; name=\"_csrftoken\"\r\n"
	"\r\n"
	"2c5dcb7615928c3a9becee2b87394ab3648554a0\r\n"
	"------WebKitFormBoundaryQ5J3YRE0jlAbPq7X--\r\n"
	;
	XLHttpHeader header;
	header.Parse(str);

	string body = str.substr(header.getHeaderSize());
	header.PostParse(body.c_str(),body.size());  

	auto post = header.getPostPointer();
	string r = mapfind(post,"typepath");
	ASSERT(r == "elecicon");
}
*/
bool XLHttpHeader::FullfileUploadParse(const char * inBody ,unsigned int size)
{
	if ( this->Header.end() == this->Header.find("x-fullfileupload") )
	{
		return false;
	}
	__file_struct* filestruct = new __file_struct;
	filestruct->data.insert(filestruct->data.end() , inBody,inBody+ size);
	filestruct->filename = "__all__";
	filestruct->mime = "";

	this->Files["__all__"] = filestruct;

	return true;
}

void XLHttpHeader::PostParse( const char * inBody ,unsigned int size )
{
	

	if (  MultipartBounderParse(inBody,size) )
	{
		return ;
	}
	if (FullfileUploadParse(inBody,size))
	{
		return ;
	}
	//普通のPOSTパース
	string str = string(inBody,0,size);

	this->Post.clear();
	const auto tempMap = XLStringUtil::crosssplit("&","=",str );
	for(auto it = tempMap.begin() ; it != tempMap.end() ; ++it )
	{
		this->Post[_U2A(XLStringUtil::urldecode(it->first))] = _U2A(XLStringUtil::urldecode(it->second));
	}
	return ;
}


void XLHttpHeader::setAt(const string& inKey , const string & inValue)
{
	this->Header[inKey] = inValue;
}

string XLHttpHeader::getAt(const string& inKey) const
{
	const auto i = this->Header.find(inKey);
	if (i == this->Header.end() ) return "";
	return i->second;
}

string XLHttpHeader::Build() const
{
	//最初のライン.
	string ret = this->FirstHeader[0]  + " " + this->FirstHeader[1]  + " " + this->FirstHeader[2] + "\r\n";
	//それ以降.
	auto i = this->Header.begin();
	auto e = this->Header.end();

	for( ; i != e ; i ++)
	{
		ret += i->first + ": " + i->second + "\r\n";
	}
	ret += "\r\n";

	return ret;
}


string XLHttpHeader::getFirstParam(int inNumebr) const
{
	assert(inNumebr < 3);
	return this->FirstHeader[inNumebr];
}

string XLHttpHeader::getCookieHeader(const string& cookiename) const
{
	const string searchName = cookiename + "=";
	const string cookie = mapfind(this->Header,"cookie");
	const char * p = cookie.c_str();
	const char * pp = strstr(p,searchName.c_str() );
	if (pp == NULL)
	{
		return "";
	}
	pp += searchName.size();
	const char * ppp = strchr(pp ,';' );
	if (ppp == NULL)
	{
		ppp = pp + strlen(pp);
	}

	return cookie.substr( (int) (pp - p) , (int)(ppp - pp) );
}

string XLHttpHeader::getHostHeader() const
{
	string host = this->getAt("host");
	//もしポート番号がついている場合は削る
	const char * port = strchr(host.c_str() , ':');
	if (port)
	{
		host = host.substr( 0 ,(unsigned int) (port - host.c_str()) );
	}
	if (! XLStringUtil::checkSafePath(host))
	{//危ないやつだったら、なかったことにする
		return "";
	}
	return host;
}

SEXYTEST()
{
	{
		XLHttpHeader t;
		string th = 
				"HTTP/1.1 200 Document follows\r\n"
				"MIME-Version: 1.0\r\n"
				"Server: AnWeb/1.42n\r\n"
				"Date: Sat, 12 Mar 2005 05:45:20 GMT\r\n"
				"Content-Type: text/html\r\n"
				"Transfer-Encoding: chunked\r\n"
				"\r\n";
		bool r;
		assert( ( r = t.Parse(th.c_str() , th.size() - 1) ) == true );

		string rr;
		assert( ( rr = t.getResponsProtocol() ) == "HTTP/1.1" );
		assert( ( rr = t.getResponsResultCode() ) == "200" );
		assert( ( rr = t.getResponsResultState() ) == "DOCUMENT FOLLOWS" );
		assert( ( rr = t.getAt("date") ) == "Sat, 12 Mar 2005 05:45:20 GMT" );
		assert( ( rr = t.getAt("mime-version") ) == "1.0" );
		assert( ( rr = t.getAt("content-type") ) == "text/html" );
		assert( ( rr = t.getAt("transfer-encoding") ) == "chunked" );
	}
}

