//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <termio.h>
#include <asm-generic/ioctls.h>
#include <time.h>
#include "EndelSCommander.h"

EndelSCommander::EndelSCommander()
{
	this->fd = open("/dev/ttyO1",O_RDWR);
	if (this->fd == -1)
	{//どうしよう・・・
		
	}
	this->page = 0;

/*
	struct termios oldtio;
	ioctl(fd, TCGETS, &oldtio);

	printf("c_i %d\r\n",oldtio.c_iflag);
	printf("c_o %d\r\n",oldtio.c_oflag);
	printf("c_c %d\r\n",oldtio.c_cflag);
	printf("c_l %d\r\n",oldtio.c_lflag);
	for(int i = 0 ; i < NCCS ; i ++)
	{
		printf("c_c[%d] %d\r\n",i,oldtio.c_cc[i]);
	}
*/
	struct termios oldtio;
	oldtio.c_iflag = 0;
	oldtio.c_oflag = 4;
	oldtio.c_cflag = 7346;
	oldtio.c_lflag = 0;
	oldtio.c_cc[0] = 3;
	oldtio.c_cc[1] = 28;
	oldtio.c_cc[2] = 127;
	oldtio.c_cc[3] = 21;
	oldtio.c_cc[4] = 4;
	oldtio.c_cc[5] = 0;
	oldtio.c_cc[6] = 0;
	oldtio.c_cc[7] = 0;
	oldtio.c_cc[8] = 17;
	oldtio.c_cc[9] = 19;
	oldtio.c_cc[10] = 26;
	oldtio.c_cc[11] = 0;
	oldtio.c_cc[12] = 18;
	oldtio.c_cc[13] = 15;
	oldtio.c_cc[14] = 23;
	oldtio.c_cc[15] = 22;
	oldtio.c_cc[16] = 0;
	oldtio.c_cc[17] = 0;
	oldtio.c_cc[18] = 0;
	oldtio.c_cc[19] = 0;
	oldtio.c_cc[20] = 0;
	oldtio.c_cc[21] = 0;
	oldtio.c_cc[22] = 0;
	oldtio.c_cc[23] = 0;
	oldtio.c_cc[24] = 0;
	oldtio.c_cc[25] = 0;
	oldtio.c_cc[26] = 0;
	oldtio.c_cc[27] = 0;
	oldtio.c_cc[28] = 0;
	oldtio.c_cc[29] = 0;
	oldtio.c_cc[30] = 0;
	oldtio.c_cc[31] = 0;
	ioctl(this->fd, TCSETS, &oldtio);
}

EndelSCommander::~EndelSCommander()
{
	close(this->fd);
	this->fd = 0;
}
void EndelSCommander::Dump(const unsigned char* src , int size)
{
	printf("\r\n");
	const unsigned char * srcP = src;
	const unsigned char * srcEnd = src + size;
	for( ; srcP != srcEnd ; ++srcP )
	{
		printf("%x ",*srcP);
	}
	printf("\r\n");
}

unsigned char* EndelSCommander::escapeCtrl(const unsigned char* src , unsigned char* dest ,unsigned int size )
{
	const unsigned char * srcP = src;
	const unsigned char * srcEnd = src + size;
	unsigned char * destP = dest;
	for( ; srcP != srcEnd ; ++srcP )
	{
		if ( *srcP == SERIAL_STX || *srcP == SERIAL_ETX || *srcP == SERIAL_ESC )
		{
			*destP++ = SERIAL_ESC;
			*destP++ = *srcP | SERIAL_ESC_SHIFT;
		}
		else
		{
			*destP++ = *srcP;
		}
	}

	return destP;
}

unsigned char EndelSCommander::calcChecksum(const unsigned char* src,int size)
{
	const unsigned char * srcP = src ;
	const unsigned char * srcEnd = src + size;
	unsigned char sum = 0;
	for( ; srcP != srcEnd ; ++srcP )
	{
		sum += *srcP;
	}

	return sum;
}

const unsigned char* EndelSCommander::unEscapeCtrl(const unsigned char* src , unsigned char* dest ,unsigned int size )
{
	const unsigned char * srcP = src;
	unsigned char * destP = dest;
	unsigned char * destEnd = dest + size;
	for( ; destP != destEnd ;  )
	{
		if (*srcP == SERIAL_ESC)
		{
			++srcP;
			*destP++ = *srcP & (~SERIAL_ESC_SHIFT);
		}
		else
		{
			*destP++ = *srcP;
		}
		srcP++;
	}

	return srcP;
}

int EndelSCommander::decodePacket(const unsigned char* src ,unsigned char* dest, unsigned short *command)
{
	unsigned char header[4];

	//+1はSTXをスキップする.
	const unsigned char* srcR = src+1;
	srcR = unEscapeCtrl(srcR,header,4);

	*command = ((header[0]) & 0xFF) | (((header[1]) & 0xFF)<<8);
	unsigned char length = (header[2]);
//	unsigned char page = (header[3]);

	if (length > SERIAL_PAYLOAD_MAX)	length = SERIAL_PAYLOAD_MAX;

	srcR = unEscapeCtrl(srcR,dest,length);

	unsigned char sumdest;
	unEscapeCtrl(srcR,&sumdest,1);

	unsigned char sum = calcChecksum(src,srcR - src);
	if (sumdest != sum)
	{
		printf("checksum error! %x vs %x\r\n",sumdest , sum);
		return -4;
	}
	return length;
}

int EndelSCommander::encodePacket(const unsigned char* src ,unsigned char* dest, int srclen,unsigned short command)
{
	unsigned char rawBuf[PACKET_MAXLEN];

	unsigned int length = srclen;
	if(length > SERIAL_PAYLOAD_MAX)
	{
		length = SERIAL_PAYLOAD_MAX;
	}
	
	//build packet with stx, header and payload
	rawBuf[0] = (unsigned char)(command & 0xFF);
	rawBuf[1] = (unsigned char)((command & 0xFF00) >> 8);
	rawBuf[2] = (unsigned char)(length & 0x3F);
	rawBuf[3] = (unsigned char)(this->page & 0x3F);
	memcpy(rawBuf+4,src,srclen);

	//エスケープして
	dest[0] = SERIAL_STX;
	unsigned char* destR = escapeCtrl(rawBuf,dest+1,4+srclen);
	
	//calculate check sum
	unsigned char cSum = calcChecksum(dest, destR - dest );
	//チェックサムを追加
	destR = escapeCtrl(&cSum,destR,1);
	
	//add ETX
	*destR++ = SERIAL_ETX;

	this->page++;
	if (this->page >= SERIAL_MAX_PAGE)
	{
		this->page = 0;
	}

	return (int)(destR - dest);
}

int EndelSCommander::transaction(const unsigned char* src ,unsigned char* dest, int srclen,unsigned short command,unsigned short* outCommand)
{
	unsigned char buf[PACKET_MAXLEN];

	//パケットをエスケープします。
	int buflen = encodePacket(src,buf , srclen,command);
	if ( buflen < 0  )
	{
		return -10 + buflen;
	}
	//Dump(buf,buflen);
	
	//送信バッファにあるものを送ってしまう.
	for(int sendsize = 0 ; sendsize < buflen ;)
	{
		int len = write(this->fd,&buf[0] + sendsize , buflen - sendsize);
		if (len <= 0)
		{
			return -20;
		}
		sendsize += len;
	}
	//printf("write complated %dbytes\r\n",buflen);

	buflen = 0;
	for(int retry = 0 ; retry < 3 ; retry ++ )
	{
		//データを送ったので返事を聞く
		int readsize = 0;
		int readretry = 0;
		while(1)
		{
			int len = read(this->fd , buf+buflen ,PACKET_MAXLEN-buflen);
			if (len <= 0)
			{
				if (len == 0) 
				{
					if (readsize <= 0)
					{
						readretry ++;
						if (readretry >= 1000)
						{//1sec
							printf("retry count over!!");
							return -30;
						}
						usleep(1000);
						continue;
					}
					break;
				}
				return -30;
			}
			readsize += len;
			buflen += len;
		}
		//printf("read complated %d bytes\r\n",buflen);
	
		//スタートフラグを検出できたかどうか
		const unsigned char* p = buf;
		const unsigned char* bufEnd = buf + buflen;
		for( ; p != bufEnd ; ++p  )
		{
			if (*p == SERIAL_STX)
			{
				break;
			}
		}
		if (p == bufEnd)
		{//STXを検出できない
			printf("STX flag not found.");
			usleep(1000);
			continue;
		}
		const unsigned char * bufSTX = p;
	
		//終了フラグを検出できたかどうか
		for(++p ; p != bufEnd ; ++p  )
		{
			if (*p == SERIAL_ETX)
			{
				break;
			}
		}
		if (p == bufEnd)
		{//ETXを検出できない
			printf("ETX flag not found.");
			usleep(1000);
			continue;
		}
	
		//Dump(bufSTX,p-bufSTX+1);
		const int destlen = decodePacket(bufSTX  , dest , outCommand);
		if ( destlen < 0 )
		{
			return -60 + destlen;
		}
		return destlen;
	}
	return -50; //timeout.
}
