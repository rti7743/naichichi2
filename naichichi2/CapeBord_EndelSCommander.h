﻿#pragma once

//旧リモコンボードEndelと通信するプロトコル
//速度がほしいのでC++で書き直す.

const unsigned char SERIAL_STX = 0x2;
const unsigned char SERIAL_ETX = 0x3;
const unsigned char SERIAL_ESC = 0x1b;
const unsigned char SERIAL_ESC_SHIFT = 0x20;

const int SERIAL_MAX_PAGE = 2;
const int SERIAL_PAYLOAD_MAX = 64;
const int PACKET_MAXLEN = 192;

class CapeBord_EndelSCommander
{
private:
	unsigned char page;
	int fd;

public:
	CapeBord_EndelSCommander();
	virtual ~CapeBord_EndelSCommander();

	void Dump(const unsigned char* src , int size);
	unsigned char* escapeCtrl(const unsigned char* src , unsigned char* dest ,unsigned int size );
	unsigned char calcChecksum(const unsigned char* src,int size);
	const unsigned char* unEscapeCtrl(const unsigned char* src , unsigned char* dest ,unsigned int size );
	int decodePacket(const unsigned char* src ,unsigned char* dest, unsigned short *command);
	int encodePacket(const unsigned char* src ,unsigned char* dest, int srclen,unsigned short command);
	int transaction(const unsigned char* src ,unsigned char* dest, int srclen,unsigned short command,unsigned short* outCommand);
};
