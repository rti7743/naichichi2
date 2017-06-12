#include "common.h"
#include "XLException.h"
#include "XLGZip.h"
#include <zlib.h>
#if _MSC_VER
	#pragma comment(lib, "zlib.lib")
#endif
//#include "../wavpack/include/wavpack.h"
//#pragma comment(lib, "wavpack.lib")
//#include "XLFileUtil.h"

/*
#include "../lz4/lz4.h"
#include "../lz4/lz4hc.h"
#pragma comment(lib, "lz4.lib")
*/

XLGZip::XLGZip()
{
	this->Data = NULL;
	this->Size = 0;
}

XLGZip::~XLGZip()
{
	Free();
}

void XLGZip::Free()
{
	if (this->Data)
	{
		free(this->Data);
		this->Data = NULL;
		this->Size = 0;
	}
}

bool XLGZip::gzip(const char* data ,unsigned int len , int compression_level )
{
	Free();

	z_stream stream = {Z_NULL};
	int r;
	r = deflateInit2(&stream, compression_level, Z_DEFLATED, 31, 8, Z_DEFAULT_STRATEGY );
	if (r != Z_OK)
	{
		return false;
	}
	this->Data = (char*)malloc(len * 2); //メモリは潤沢にあるって感じで
	stream.next_in = (Bytef*) data;
	stream.avail_in = len;
	stream.next_out = (Bytef*) this->Data;
	stream.avail_out = len * 2;
	r = deflate(&stream, Z_FINISH);
	if (!(r == Z_STREAM_END || r == Z_OK))
	{
		return false;
	}
	this->Size = stream.total_out;

	deflateEnd(&stream);

	return true;
}

bool XLGZip::gunzip(const char* data ,unsigned int len)
{
	Free();

	z_stream stream = {Z_NULL};
	int r;
	r = inflateInit2(&stream ,16+MAX_WBITS );
	if (r != Z_OK)
	{
		return false;
	}

	bool isOK = true;

	unsigned int memorysize = len * 10;
	this->Data = (char*)malloc(memorysize);//メモリは潤沢にあるって感じで
	this->Size = 0;
	stream.next_in = (Bytef*)data;
	stream.avail_in = len;
	stream.next_out = (Bytef*)this->Data;
	stream.avail_out = memorysize;
	while(1)
	{
		r = inflate(&stream, Z_NO_FLUSH);
		if (r == Z_STREAM_END)
		{
			this->Size = stream.total_out;
			break;
		}
		else if (r == Z_OK)
		{
			this->Size = stream.total_out;
			char* newP = (char*)realloc(this->Data,this->Size + memorysize);
			if (!newP)
			{
				isOK = false;
				break;
			}
			this->Data = newP;
			stream.next_out = (Bytef*) (this->Data + this->Size);
			stream.avail_out = memorysize;
		}
		else
		{//error!
			isOK = false;
			break;
		}
	}
	
	inflateEnd(&stream);

	return (isOK);
}
#include "XLFileUtil.h"

SEXYTEST()
{
	{
		const string testdata = XLFileUtil::cat("./config/testdata/gziptest.txt");

		XLGZip zip;
		bool r = zip.gzip(testdata.c_str() , testdata.size() );
		SEXYTEST_EQ(r,true);

		XLGZip unzip;
		r = unzip.gunzip(zip.getData(),zip.getSize() );
		SEXYTEST_EQ(r,true);

		string rr = string(unzip.getData(),0,unzip.getSize() );
		SEXYTEST_EQ(testdata,rr);
	}
}




/* //あんまり縮まなかった
bool XLGZip::lz4(const char* data ,unsigned int len , int compression_level )
{
	Free();
	this->Data = (char*)malloc(len * 2); //メモリは潤沢にあるって感じで
	if (compression_level == 1)
	{
		this->Size = LZ4_compressHC(data,this->Data, len);
	}
	else
	{
		this->Size = LZ4_compress(data,this->Data, len);
	}

	return true;
}

bool XLGZip::unlz4(const char* data ,unsigned int len)
{
	Free();
	this->Data = (char*)malloc(len * 10); //メモリは潤沢にあるって感じで
	this->Size = LZ4_uncompress_unknownOutputSize(data,this->Data, len , len * 10);
	if (this->Size < 0)
	{
		return false;
	}
	return true;
}
*/
/*
bool XLGZip::wavpack(const char* data ,unsigned int len , long sfreq )
{
	Free();
	this->Data = (char*)malloc(len * 2); //メモリは潤沢にあるって感じで

	struct _callback
	{
		static int write(void *id, void *data, int32_t length)
		{
			_callback* p = ((_callback*)id);
			if (p->maxlen - p->nowlen < length)
			{
				length = p->maxlen - p->nowlen;
				if (length <= 0)
				{
					return 1;
				}
			}

			memcpy( p->d + p->nowlen, data , length);
			p->nowlen += length;
			return 1;
		}
		char* d;
		int32_t   maxlen;
		int32_t   nowlen;
	};
	_callback callback;
	callback.d = this->Data;
	callback.maxlen = len * 2;
	callback.nowlen = 0;

	WavpackContext* context = WavpackOpenFileOutput(callback.write, (void *)&callback, NULL);
	if (!context) return false;

	WavpackConfig config = {0};
	config.bytes_per_sample = sizeof(short);
    config.bits_per_sample  = sizeof(short) * 8; //16
    config.channel_mask     =  0x4;
    config.num_channels     =  1;
    config.sample_rate      =  16000;
	config.flags = CONFIG_OPTIMIZE_MONO ;
	//wavpackは、int* でないと受け取らないらしい謎仕様
	int* sample_buffer = (int*)malloc(65535 * sizeof(int) * 1);
	{
		int* p = sample_buffer;
		short* s = (short*)data;
		const int halflen = len/2;

		for(int i = 0 ; i < halflen ; ++i )
		{
			*p++ = s[i];
		}
	}

	WavpackSetConfiguration (context, &config, len / sizeof(short));
	WavpackPackInit(context);
	WavpackPackSamples(context,(int32_t*)sample_buffer, len / sizeof(short));
	WavpackFlushSamples(context);

	WavpackCloseFile(context);

	this->Size = callback.nowlen;

	free(sample_buffer);
	return true;
}
*/

bool XLGZip::rtiwavpack(const char* data ,unsigned int len , int compression_level )
{
	if (len <= 1) return false;

	//ワークメモリを確保する
	char * work = (char*)malloc(len);
	const int halflen = len/2;

	work[0] = data[0];
	work[1] = data[1];
	int worklen = 2;
	for(int i = 1 ; i < halflen ; ++i )
	{
		short a = ((short*)data)[i - 1] - ((short*)data)[i];
		work[worklen++] = ((char*)&a)[0] ;
		work[worklen++] = ((char*)&a)[1] ;
	}
	if (! gzip(work,worklen, compression_level ) )
	{
		free(work);
		return false;
	}

	free(work);
	return true;
}

bool XLGZip::rtiwavpack2(const char* data ,unsigned int len , int compression_level )
{
	if (len <= 1) return false;

	//ワークメモリを確保する
	char * work = (char*)malloc(len * 2);
	const int halflen = len/2;

	work[0] = data[0];
	work[1] = data[1];
	int worklenH = 2;
	int worklenL = 2 + ((len-2)/2);
	for(int i = 1 ; i < halflen ; ++i )
	{
		short a = ((short*)data)[i - 1] - ((short*)data)[i];
		work[worklenH++] = ((char*)&a)[0] ;
		work[worklenL++] = ((char*)&a)[1] ;
	}
	if (! gzip(work,len, compression_level ) )
	{
		free(work);
		return false;
	}

	free(work);
	return true;
}

bool XLGZip::rtiwavpack3(const char* data ,unsigned int len , int compression_level )
{
	if (len <= 1) return false;

	//ワークメモリを確保する
	char * work = (char*)malloc(len);
	const int halflen = len/2;

	work[0] = data[0];
	work[1] = data[1];
	int worklenH = 2;
	int worklenL = 2 + ((len-2)/2);
	for(int i = 1 ; i < halflen ; ++i )
	{
		short a = ((short*)data)[i - 1] - ((short*)data)[i];
		work[worklenH++] = ((char*)&a)[0] ;
		work[worklenL++] = ((char*)&a)[1] ;
	}
	if (! gzip(work,len, compression_level ) )
	{
		free(work);
		return false;
	}

	free(work);
	return true;
}

bool XLGZip::rtiwavunpack2(const char* data ,unsigned int len )
{
	if (len <= 1) return false;


	if ( ! gunzip(data,len) )
	{
		return false;
	}

	//ワークメモリを確保する
	char * work = (char*)malloc(this->Size);
	const int halflen = this->Size/2;

	work[0] = this->Data[0];
	work[1] = this->Data[1];
	short basedata = ((short*)this->Data)[0];

	int worklenH = 2;
	int worklenL = 2 + ((this->Size-2)/2);
	int workp = 2;
	for(int i = 1 ; i < halflen ; ++i )
	{
		unsigned char aH = this->Data[worklenH++] ;
		unsigned char aL = this->Data[worklenL++] ;
		{
			basedata -= (aL << 8) | aH;
		}
		work[workp++] = ((char*)&basedata)[0];
		work[workp++] = ((char*)&basedata)[1];
	}

	memcpy(this->Data,work,this->Size);

	free(work);
	return true;
}
