/* IRR  

このコードは GPL でライセンスされています。
*/
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <math.h>
#include <sched.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string>
#include <wiringPi.h>

//RASBERYPIの DMTIMER秒からナノ秒に変換するときに利用する定数.
#define DMTIME_TO_NS		(41.65)
#define DMTIME_TO_NS_HALF	(21.65)

#define IR_RECV_BUFFER_SIZE 600000
#define IR_BIGWAVE_SIZE     100000

int g_gpio_mem_fd = -1;
volatile unsigned int* g_gpio_mem_map = (unsigned int*)MAP_FAILED;

#define MMAP_OFFSET 0x3F000000
#define MMAP_SIZE   (0x40000000 - MMAP_OFFSET)

class CurrentDir
{
public:
	std::string getCurrentDir() const
	{
		return this->dir;
	}
	void setCurrentDir(const char* hintarg0)
	{
		const char* slash = strrchr(hintarg0,'/');
		if (slash == NULL || *slash == '.')
		{
			char buffer[255] = {0};
			getcwd(buffer,255-1);
			this->dir = buffer;
		}
		else
		{
			this->dir = std::string(hintarg0,0,(int)(slash-hintarg0));
		}
	}
private:
	std::string dir;
};
CurrentDir g_CurrentDir;

void rasberypi_clean()
{
	if (g_gpio_mem_fd < 0)
	{
		return ;
	}
	close(g_gpio_mem_fd);
}


//rasberypiを初期化します
int rasberypi_init()
{
	rasberypi_clean();

	g_gpio_mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (g_gpio_mem_fd < 0)
	{
		printf("Could not open GPIO memory fd\n");
		return -1;
	}

	g_gpio_mem_map = ( unsigned int*) mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE,MAP_SHARED, g_gpio_mem_fd, MMAP_OFFSET);
	if (g_gpio_mem_map == MAP_FAILED) {
		printf ("GPIO Mapping failed\n");
		close(g_gpio_mem_fd);
		return -2;
	}
	return 1;
}

inline unsigned int dmtimer_get()
{
//	return g_gpio_mem_map[0xB000 / 4];
	return g_gpio_mem_map[0x3004 / 4];
}

class IRR
{
private:
//	const struct GPIOPIN* IRCaptureGPIO;		//IR受信 不要 kernellandでやるわ
	const struct GPIOPIN* IRControllGPIO;		//IR送信用PWM切り替え用(使わないかも)
	struct PWM*     IRControllPWM;				//IR送信用PWM 38khz

public:
	IRR();
	virtual ~IRR();
	//構築
	int Create();
	int Free();

	//IRキャプチャ
	int CaptureIR(const char* filename);
	
	//IR送信(超最優先で動作する)
	int FireIR(const char* filename);

	//IRデータを可視化するため、画面にいろいろ表示する.
	int DumpIR(const char* filename);

private:
	unsigned int convertReti(unsigned int size,unsigned int* ioBuffer);
	int readDriverIRR(const char* filename,unsigned int srcIndex,unsigned int* srcWaveMalloc);
	int readCountDriverIRR(const char* filename);
	int PullData(const char* filename,int waitSec);
	
	bool ir_convertpwm(unsigned int* timer,unsigned int maxindex
		,unsigned int* outTimer,unsigned int* retindex
		,unsigned int* retOnAvg,unsigned int* retOffAvg);
	int checkIRData(unsigned int onAvg,unsigned int offAvg,unsigned int* timer2,unsigned int timerlen);
	//irコードをバイト列に変換します
	void irdata2binary(unsigned int* timer2,unsigned int timerlen,char* outMsg,unsigned int msgmax);
	void FireIRLow2(struct PWM* pwm,const struct GPIOPIN* gpio,unsigned int onAvg,unsigned int offAvg,unsigned int* timer2,unsigned int timerlen);
	void DumpHeaderIR(unsigned int onAvg,unsigned int offAvg,unsigned int* bigwaveMalloc,unsigned int  bigwaveIndex);
};


IRR::IRR()
{
	this->IRControllGPIO = NULL;
	this->IRControllPWM = NULL;
}

IRR::~IRR()
{
	Free();
}
int IRR::Free()
{
}

int IRR::Create()
{
	return 1;
}

#define IRR_RECV_DRIVER_FILENAME "/proc/driver/irr_recv"
#define IRR_COUNT_DRIVER_FILENAME "/proc/driver/irr_recv_count"

int IRR::readDriverIRR(const char* filename,unsigned int srcIndex,unsigned int* srcWaveMalloc)
{
	int fp = open(filename,O_RDONLY);
	if (fp<0)
	{
		printf("can not open file(%s)\n",filename);
		return -1;
	}

	unsigned int totalByte = 0;
	while(1)
	{
		int readByte = read(fp,((char*)srcWaveMalloc)+totalByte,sizeof(unsigned int)*(srcIndex*4-totalByte) );
		if (readByte <= 0)
		{
			break;
		}
			printf("!! read readByte: %d \n",readByte);
		totalByte += readByte;
		if (totalByte >= srcIndex*4)
		{
			break;
		}
	}
	
	close(fp);
	
	if (totalByte != srcIndex*4)
	{
		printf("can not read. totalByte:%u  srcIndex*4:%u\n",totalByte,srcIndex*4);
		return -2;
	}
	return 1;
}

int IRR::readCountDriverIRR(const char* filename)
{
	int fd = open(filename,O_RDONLY);
	if (fd<0)
	{
		printf("can not open file(%s)\n",filename);
		return -1;
	}

	char buf[10];
	read(fd,buf,sizeof(buf));
	close(fd);

	return atoi(buf);
}

//データ補正
unsigned int IRR::convertReti(unsigned int size,unsigned int* ioBuffer)
{
	unsigned int index = 0;
	unsigned int basetime = 0;
	
	if (size <= 0) return 0;

	//タイマーは必ず一周させる.
	basetime = 0xffffffff - ioBuffer[0] + 1;

	//データをコピーします。結構非効率です.
	for(index = 1 ; index < size ; index++)
	{
		//前回との差を求める.
		unsigned int irdata = (ioBuffer[index]+basetime) - (ioBuffer[index-1]+basetime);
		ioBuffer[index-1] = irdata;
	}
	return size - 1;
}

int IRR::PullData(const char* filename,int waitSec)
{
	int r;
	for(int i = 0 ; i < waitSec ; i++ )
	{
		sleep(1);

		r = readCountDriverIRR(filename);
		if (r > 0) break;
	}
	
	//結局データは来ましたか？
	r = readCountDriverIRR(filename);
	if (r <= 0)
	{
		//データがきませんでした.
		return -1;
	}

	//データの終端を待ちます. 無限ループで待ってもいいが、カッコ悪いのでwaitSecで代用する.
	int oldr = r;
	for(int i = 0 ; i < waitSec ; i++ )
	{
		sleep(1);

		r = readCountDriverIRR(filename);
		if (r <= oldr) break;
	}
	
	return r;
}



//IRキャプチャ
int IRR::CaptureIR(const char* filename)
{
	int r;
	int size;

	
	//データを待ちます.
	size = PullData(IRR_COUNT_DRIVER_FILENAME,5);
	

	if (size <= 0)
	{//データを受信できませんでした.
		printf("can not get data!");
		return -8;
	}
	
	//元データ.
	unsigned int* srcWaveMalloc = NULL;
	unsigned int  srcIndex = size; //IRデータsrcの個数
	
	//元データを整形して作るデータ.
	unsigned int* bigwaveMalloc = NULL; //のるしかない。
	unsigned int  bigwaveIndex = 0;

	srcWaveMalloc = (unsigned int*)malloc(sizeof(unsigned int)*srcIndex);
	if (!srcWaveMalloc)
	{
		printf("can not alloc src ir data memory.");
		return -9;
	}

	//結果読み出し
	r = readDriverIRR(IRR_RECV_DRIVER_FILENAME,srcIndex,srcWaveMalloc);
	if (r < 0)
	{
		if (srcWaveMalloc) free(srcWaveMalloc);
		if (bigwaveMalloc) free(bigwaveMalloc);
		
		printf("can not read irr : %d",r);
		return -10;
	}
	if (srcIndex < 32)
	{
		if (srcWaveMalloc) free(srcWaveMalloc);
		if (bigwaveMalloc) free(bigwaveMalloc);

		printf("recv IR data too short.");
		return -20;
	}
	//絶対表記を相対表記に直します.
	srcIndex = convertReti(srcIndex,srcWaveMalloc);

	unsigned int onAvg = 0;
	unsigned int offAvg = 0;

	//タイマーデータからPWM波形の元になるduty比等を計算します。
	//ついでに、データの補正も行います.
	//そして、大きな波、bigwaveを作ります。もう乗るしかない。このビッグウェーブに。
	bigwaveMalloc = (unsigned int*)malloc(sizeof(unsigned int)*srcIndex);
	if ( ! ir_convertpwm(srcWaveMalloc,srcIndex,bigwaveMalloc,&bigwaveIndex,&onAvg,&offAvg) )
	{
		if (srcWaveMalloc) free(srcWaveMalloc);
		if (bigwaveMalloc) free(bigwaveMalloc);

		puts("bad IR DATA ir_convertpwm func-->error! .");
		return -30;
	}
	

	//このデータあってそう？
	bool isTrueData = true;
	r = checkIRData(onAvg,offAvg,bigwaveMalloc,bigwaveIndex);
	if ( r <= 0)
	{
//		if (srcWaveMalloc) free(srcWaveMalloc);
//		if (bigwaveMalloc) free(bigwaveMalloc);

		isTrueData = false;
		printf("bad IR Data file:%s  r:%d\n",filename,r);
		//ここではまだエラーにしない. ファイルに書いた後で。
	}
	DumpHeaderIR(onAvg,offAvg,bigwaveMalloc,bigwaveIndex);

	//セーブします.
	{
		FILE* fp = fopen(filename,"wb");
		if (!fp)
		{
			if (srcWaveMalloc) free(srcWaveMalloc);
			if (bigwaveMalloc) free(bigwaveMalloc);

			printf("can not open save file (%s)\n",filename);
			return -30;
		}
		//ヘッダー
		fwrite("IRR ",sizeof(char),4,fp);

		//ON/OFF平均
		fwrite(&onAvg,sizeof(onAvg),1,fp);
		fwrite(&offAvg,sizeof(offAvg),1,fp);
		
		//BIGWave
		fwrite(&bigwaveIndex,sizeof(unsigned int),1,fp);
		fwrite(bigwaveMalloc,sizeof(unsigned int),bigwaveIndex,fp);
		
		//SRC
		fwrite(&srcIndex,sizeof(unsigned int),1,fp);
		fwrite(srcWaveMalloc,sizeof(unsigned int),srcIndex,fp);
		
		fclose(fp);
	}
	
	if (srcWaveMalloc) free(srcWaveMalloc);
	if (bigwaveMalloc) free(bigwaveMalloc);



	//データがおかしいならばここで警告.
	if (! isTrueData )
	{
		printf("maybe broken IR data.\n");
		return -9999;
	}
	
	//成功
	return 1;
}


int IRR::FireIR(const char* filename)
{
	unsigned int onAvg,offAvg;
	
	//元データを整形して作るデータ.
	unsigned int* bigwaveMalloc = NULL; //のるしかない。
	unsigned int  bigwaveIndex;

	{
		FILE* fp = fopen(filename,"rb");
		if (!fp)
		{
			printf("can not open ir file (%s)\n",filename);
			return -30;
		}
		//IRR のヘッダー
		char header[4];
		fread(header  ,sizeof(unsigned char),4,fp);
		if ( memcmp(header,"IRR ",4) != 0  )
		{
			fclose(fp);
			printf("no IRR1 header file:%s\n",filename);
			return -2;
		}

		fread(&onAvg   ,sizeof(unsigned int),1,fp);	//平均点灯
		fread(&offAvg  ,sizeof(unsigned int),1,fp);	//平均消灯

		fread(&bigwaveIndex   ,sizeof(unsigned int),1,fp);	//bigwaveを読む
		if (bigwaveIndex >= IR_BIGWAVE_SIZE)
		{
			fclose(fp);
			printf("data to big file:%s\n",filename);
			return -3;
		}

		bigwaveMalloc = (unsigned int*)malloc(sizeof(unsigned int)*bigwaveIndex);
		fread(bigwaveMalloc  ,sizeof(unsigned int),bigwaveIndex,fp);			//timer2のデータ

		fclose(fp);
	}
	
	int r = checkIRData(onAvg,offAvg,bigwaveMalloc,bigwaveIndex);
	if ( r <= 0)
	{
		if (bigwaveMalloc) free(bigwaveMalloc);

		printf("broken IR data file:%s  r:%d\n",filename,r);
		return -10 + r;
	}
	DumpHeaderIR(onAvg,offAvg,bigwaveMalloc,bigwaveIndex);


	{
	//優先度を再優先にします。
	setpriority(PRIO_PROCESS,0,-20);
	struct sched_param sp;
	sp.sched_priority=99;
	sched_setscheduler(0,SCHED_FIFO,&sp);

	FireIRLow2(this->IRControllPWM,this->IRControllGPIO,onAvg,offAvg,bigwaveMalloc,bigwaveIndex);

	//最優先の終了
	sp.sched_priority=0;
	sched_setscheduler(0,SCHED_FIFO,&sp);
	setpriority(PRIO_PROCESS,0,0);
	}

	if (bigwaveMalloc) free(bigwaveMalloc);
	return 1;
}

int IRR::DumpIR(const char* filename)
{
	unsigned int onAvg,offAvg;

	//元データを整形して作るデータ.
	unsigned int* bigwaveMalloc = NULL; //のるしかない。
	unsigned int  bigwaveIndex;

	unsigned int* srcMalloc = NULL; //元データ
	unsigned int  srcIndex;
	
	{
		FILE* fp = fopen(filename,"rb");
		if (!fp)
		{
			printf("can not open ir file (%s)\n",filename);
			return -30;
		}
		//IRR のヘッダー
		char header[4];
		fread(header  ,sizeof(unsigned char),4,fp);
		if ( memcmp(header,"IRR ",4) != 0  )
		{
			fclose(fp);
			printf("no IRR1 header file:%s\n",filename);
			return -2;
		}

		fread(&onAvg   ,sizeof(unsigned int),1,fp);	//平均点灯
		fread(&offAvg  ,sizeof(unsigned int),1,fp);	//平均消灯

		fread(&bigwaveIndex   ,sizeof(unsigned int),1,fp);	//bigwaveを読む
		if (bigwaveIndex >= IR_BIGWAVE_SIZE)
		{
			fclose(fp);
			printf("data to big file:%s\n",filename);
			return -3;
		}

		bigwaveMalloc = (unsigned int*)malloc(sizeof(unsigned int)*bigwaveIndex);
		fread(bigwaveMalloc  ,sizeof(unsigned int),bigwaveIndex,fp);			//bigwaveのデータ

		fread(&srcIndex   ,sizeof(unsigned int),1,fp);	//srcを読む
		if (srcIndex >= IR_RECV_BUFFER_SIZE)
		{
			fclose(fp);
			printf("data to big file:%s\n",filename);
			return -3;
		}

		srcMalloc = (unsigned int*)malloc(sizeof(unsigned int)*srcIndex);
		fread(srcMalloc  ,sizeof(unsigned int),srcIndex,fp);			//srcのデータ

		fclose(fp);
	}

	for(unsigned int i = 0 ; i < srcIndex ; i++)
	{
		printf("S	%u	%u\n",srcMalloc[i],i);
	}
	for(unsigned int i = 0 ; i < bigwaveIndex ; i++)
	{
		printf("B	%u	%u\n",bigwaveMalloc[i],i);
	}
	
	DumpHeaderIR(onAvg,offAvg,bigwaveMalloc,bigwaveIndex);


	if (bigwaveMalloc) free(bigwaveMalloc);
	if (srcMalloc) free(srcMalloc);
	return 1;
}

void IRR::DumpHeaderIR(unsigned int onAvg,unsigned int offAvg,unsigned int* bigwaveMalloc,unsigned int  bigwaveIndex)
{
	unsigned int firetime = 0;
	for(unsigned int i = 0 ; i < bigwaveIndex ; i++)
	{
		firetime += bigwaveMalloc[i];
	}
				
	unsigned int period = (onAvg+offAvg)*DMTIME_TO_NS;
	unsigned int duty = onAvg*DMTIME_TO_NS;
	float freq = 1000000/(((float)onAvg+offAvg)*DMTIME_TO_NS);
	float dutycycle = onAvg/((float)onAvg+offAvg)*100;

	char irmessage[8192] = {0};
	irdata2binary(bigwaveMalloc,bigwaveIndex,irmessage,sizeof(irmessage));

	int r = checkIRData(onAvg,offAvg,bigwaveMalloc,bigwaveIndex);
	printf("check		:	%d\n",r);
	printf("onAvg		:	%u\n",onAvg);
	printf("offAvg		:	%u\n",offAvg);
	printf("period		:	%u\n",period);
	printf("duty		:	%u\n",duty);
	printf("freq		:	%f\n",freq);
	printf("dutycycle	:	%f\n",dutycycle);
	printf("bigwaveIndex	:	%u\n",bigwaveIndex);
	printf("firetime	:	%u us(%f sec)\n",firetime,(float)firetime/1000.f/1000.f);
	printf("irMessage	:	%s\n",irmessage);
}



//大きな命令の波と判断するサイズ
#define IR_BIGWAVE 700

bool IRR::ir_convertpwm(unsigned int* timer,unsigned int maxindex
	,unsigned int* outTimer,unsigned int* retindex
	,unsigned int* retOnAvg,unsigned int* retOffAvg)
{
	unsigned int i;
	unsigned int f_on_sum = 0;    //短い時間のon
	unsigned int f_off_sum = 0;   //短い時間のoff
	unsigned int f_on_index = 0;
	unsigned int f_off_index = 0;
	unsigned int index = 0;
	int isON = 1;

	//短い周期はonに分類し直します。
	index = 0;
	outTimer[index] = 0;
	isON = 1;
	for( i = 2 ; i < maxindex ; ++i )
	{
		if (timer[i]  > IR_BIGWAVE)
		{
			if (isON)
			{//on
				if (outTimer[index] <= IR_BIGWAVE)
				{
					//onにいれるよ派
					outTimer[index] += timer[i];
					isON = 0;
				}
				else
				{
					//offを間違った可能性があるよ派
					index++;
					outTimer[index] = timer[i];
					index++;
					outTimer[index] = 0;
					isON = 1;
				}
			}
			else
			{//off
				index++;
				outTimer[index] = timer[i];
				index++;
				outTimer[index] = 0;

				isON = 1;
			}
		}
		else
		{
			outTimer[index] += timer[i];
			if (isON)
			{//on
				if (index >= 1 && f_on_index < 300)
				{
					if (timer[i] < 100)
					{
						//noise
					}
					else
					{
						f_on_sum += timer[i];
						f_on_index ++;
					}
				}
				isON = 0;
			}
			else
			{//off
				if (index >= 1 && f_off_index < 300)
				{
					if (timer[i] < 100)
					{
						//noise
					}
					else
					{
						f_off_sum += timer[i];
						f_off_index ++;
					}
				}
				isON = 1;
			}
		}
	}

	if (f_on_index <= 0)
	{
		printf(  "f_on_index is  zero!\n");
	}
	if (f_off_index <= 0)
	{
		printf(  "f_off_index is  zero!\n");
	}
	if (f_on_index <= 0 || f_off_index <= 0)
	{//共に短い波がない場合 すべてを大波とする
		*retOnAvg  = 8800/DMTIME_TO_NS;
		*retOffAvg = 17500/DMTIME_TO_NS;
	}
	else
	{
		*retOnAvg  = f_on_sum  / f_on_index;
		*retOffAvg = f_off_sum / f_off_index;
	}

	*retindex = index + 1;
	return true;
}

int IRR::checkIRData(unsigned int onAvg,unsigned int offAvg,unsigned int* timer2,unsigned int timerlen)
{
	float Fkhz = 1000000/(((float)onAvg+offAvg)*DMTIME_TO_NS);
	float dutycycle = onAvg/((float)onAvg+offAvg)*100;
	if (Fkhz <= 38.f-30.f || Fkhz >= 38.f+30.f)
	{//38khz から上下30khzまで容認。それ以外は没
		printf("IR check error. Fkhz:%fkhz \n",Fkhz);
		return -1;
	}
	if (dutycycle > 80)
	{
		printf("IR check error. dutycycle:%f \n",dutycycle);
		return -2;
	}
	if (timerlen < 10)
	{//短すぎる
		printf("IR check error. too short puls timerlen:%d \n",timerlen);
		return -3;
	}
	//トータル送信時間を求めます.
	unsigned int sumNS = 0;
	for(unsigned int i = 0 ;i < timerlen ; i++ )
	{
		sumNS += timer2[i];
	}
	if (sumNS > 10000000 * 16)
	{//16秒 以上の送信をしてはいけません
		printf("IR check error. too long ir send sumNS:%d us\n",sumNS);
		return -4;
	}
	return 1;
}

//irコードをバイト列に変換します
void IRR::irdata2binary(unsigned int* timer2,unsigned int timerlen,char* outMsg,unsigned int msgmax)
{
	unsigned int slitcount = 0;
	unsigned int len = 0;
	for(unsigned int i = 3 ; i < timerlen ; i+=2)
	{
		//データ	全体の長さ	ONの長さ	OFFの長さ
		//1	2.25ms	0.56ms	1.69ms
		//0	1.125ms	0.56ms	0.565ms
		if ( timer2[i] >= 25000 )
		{
			outMsg[len++] = '1';
		}
		else
		{
			outMsg[len++] = '0';
		}
		if (len >= msgmax) break;

		slitcount ++;
		if ( slitcount%4 == 0) outMsg[len++] = ' '; //読みやすいように4bit区切り
		if (len >= msgmax) break;
	}

	
	if (timerlen%2 == 1)
	{//奇数で終わっているということは、ONになったあとにながーいOFFがある
//		outMsg[len++] = '1';
	}

	outMsg[len] = '\0';
}

void IRR::FireIRLow2(struct PWM* pwm,const struct GPIOPIN* gpio,unsigned int onAvg,unsigned int offAvg,unsigned int* timer2,unsigned int timerlen)
{
	//配列とか、そんな遅いものは使えません。メモリ直 命!
	unsigned int* recP = timer2;
	//終端ここを超えてはいけない
	const unsigned int* recE = &timer2[timerlen];
	register unsigned int stopTime = 0;
	register unsigned int basetime = 0;

	unsigned int period = (onAvg+offAvg)*DMTIME_TO_NS;
	unsigned int duty = onAvg*DMTIME_TO_NS;
	float Fhz = 1000000/(((float)onAvg+offAvg)*DMTIME_TO_NS)*1000;
	
	unsigned int clock = (int)(19200000/3/Fhz);

	pinMode(18, PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS);
	pwmSetClock(clock);
	pwmSetRange(3); // suport 33% duty only

	pwmWrite(18, 0);

	while(1)
	{
		//ONにする.
		pwmWrite(18, 1);
		basetime = 0xffffffff - dmtimer_get() + 1;
		stopTime = *recP;
		while( dmtimer_get() + basetime < stopTime ) ;
//		printf("ON %u vs %u \n" , dmtimer_get() + basetime , *recP );
		recP++;
		if (recP >= recE) break;

		//OFFにする.
		pwmWrite(18, 0);
		basetime = 0xffffffff - dmtimer_get() + 1;
		stopTime = *recP;
		while( dmtimer_get() + basetime < stopTime ) ;
//		printf("OFF %u vs %u \n" , dmtimer_get() + basetime , *recP );
		recP++;
		if (recP >= recE) break;
	}
	pwmWrite(18, 0);
}

void usage()
{
	puts("");
	puts("-------------------------------------------------------------------------------");
	puts("irr ir test program");
	puts("");
	puts(" irr save filename             (IR Capture! [P8_15])");
	puts(" irr load filename             (IR Fire by PWM [P9_16] and GPIO [P9_42])");
	puts(" irr dump filename             (IR filedump!)");
	puts("");
}

static IRR irr;

#include <signal.h>
void irr_abort(int sig) 
{
	printf("catch signal:%d\n");
	irr.Free();
}


int mainIRR(int argc, const char * argv[])
{
	//以下IRRメインルーチン
	signal(SIGINT|SIGKILL|SIGTERM, irr_abort);
	{
		int r = irr.Create();
		if ( r <= 0 )
		{
			printf("irr.Create ERROR! r:%d\n",r);
			return -1;
		}
		printf("irr Create ...\n");
	}
	if (strcmp(argv[1],"save") == 0)
	{
		if (argc <= 2)
		{
			printf("you forget filename. irr save filename \n");
			return -2;
		}

		const std::string insmod = "insmod '" + g_CurrentDir.getCurrentDir() + "/irr_driver.ko'";
		system(insmod.c_str());

		//赤外線を受信します.
		int r = irr.CaptureIR(argv[2]);

		const std::string rmmod = "rmmod irr_driver";
		system(rmmod.c_str());

		if ( r <= 0 )
		{
			printf("irr.CaptureIR ERROR! r:%d\n",r);
			return -3;
		}

		printf("!OK!\n");
	}
	else if (strcmp(argv[1],"load") == 0)
	{
		if (argc <= 2)
		{
			printf("you forget filename. irr load1 filename \n");
			return -6;
		}
		//赤外線を送信します。PWMとGPIOで実行します.
		int r = irr.FireIR(argv[2]);
		if ( r <= 0 )
		{
			printf("irr.FireIR ERROR! r:%d\n",r);
			return -7;
		}

		printf("!OK!\n");
	}
	else if (strcmp(argv[1],"dump") == 0)
	{
		if (argc <= 2)
		{
			printf("you forget filename. irr load1 filename \n");
			return -6;
		}
		//赤外線を送信します。PWMとGPIOで実行します.
		int r = irr.DumpIR(argv[2]);
		if ( r <= 0 )
		{
			printf("irr.DumpIR ERROR! r:%d\n",r);
			return -7;
		}

		printf("!OK!\n");
	}
	
	return 1;
}


int main(int argc, const char * argv[])
{
	if (argc <= 1)
	{
		usage();
		return -1;
	}
	g_CurrentDir.setCurrentDir(argv[0]);

	int r;
	r = rasberypi_init();
	if (!r)
	{
		printf("rasberypi_init ERROR! r:%d\n",r);
		return -1;
	}
	r = wiringPiSetupGpio();
	if (r == -1)
	{
		printf("wiringPiSetupGpio ERROR! r:%d\n",r);
		return -1;
	}

	if (strcmp(argv[1],"test") == 0)
	{
		pinMode(18, PWM_OUTPUT);
		pwmSetMode(PWM_MODE_MS);
		pwmSetClock(500);
		pwmSetRange(3); // suport 33% duty only

		for(int a = 0 ; a <10;a++)
		{
			pwmWrite(18, 1);
			sleep(1);
			pwmWrite(18, 0);
			sleep(1);
		}
		pwmWrite(18, 0);
		rasberypi_clean();
		return 0;
	}



	r = mainIRR(argc,argv);
	irr.Free();
	if (r <= 0)
	{
		printf("mainIRR ERROR! r:%d\n",r);
		usage();
		rasberypi_clean();
		return -1;
	}

	rasberypi_clean();
}

