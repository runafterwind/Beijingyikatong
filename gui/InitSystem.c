#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "InitSystem.h"
#include "../display/fbtools.h"

#include <termios.h>    									/*PPSIX 终端控制定义*/
#include <linux/hdreg.h>
#include "RC500.h"
#include "../bzdes/stades.h"
#include "../sound/sound.h"

#include "../update/Resumedownload.h"
#include "cpucard.h"
#include "savesql.h"

#define Initprintf 1

/************************************************************/

/***外部引用变量***/
extern RouteSelection JackCmd;
extern SectionFarPar Section,Sectionup;
extern int RecFd;
extern LongUnon Fixvalue;
extern unsigned int g_MaxPersonNumber;
extern unsigned char updataflag;	
extern unsigned short Mcolor;
extern int M26_fd;
extern pthread_mutex_t m_stationrecord;


/***变量定义***/

//设备句柄
int mf_fd;
int bp_fd;
int mg_fd;
int beep_fd;
int uart4_fd;
int led_fd;
//文件句柄
FILE *canshu;
FILE *ParaFileBuf;
FILE *BlackFile;
FILE *Datafile;
FILE *teth;
FILE *nettab;
FILE *Rechargefile;
time_t utc_time;
FILE *MCHID;//商户号文件句柄
FILE *Stationdisfile;
//其他变量
unsigned char *CardLanBuf=NULL;
unsigned char *SectionParBuf=NULL;
unsigned char *SectionParUpBuf=NULL;
unsigned char *StationdisupParBuf=NULL;
unsigned char *StationdisdownParBuf=NULL;


/*************************************/
char des_key[8] = {0x20,0x05,0xAF,0x45,0xA3,0xF1,0x80,0x78}; 			//加密密码
char TcpIpBuf[35];
unsigned char KeyDes[8];							//密钥
unsigned char AdminPwd[8];  //管理员密码
unsigned char SleepOverTime;							//待机关闭超出时间
unsigned char SecTor[7];
unsigned char PsamNum[6];
unsigned char PsamNum_bak1[6];			//作为卡联cpu卡流程psamNum的备份，在执行卡联cpu卡流程时使用
unsigned char PsamNum_bak2[6];		    //备份交通部的psamnum
unsigned char PsamKeyIndex;				//psam卡密钥索引
unsigned char Tunion;                   //交通联合标志，复位PSAM卡成功后此标志为1，失败为0；         
unsigned char OperBuffor[80];	        //操作员密码备份
unsigned char SelfAddress[36];
unsigned char OPENBEEP;                 //1打开声音 0关闭声音
unsigned char BankCardSwitch;           //0x55-允许刷银行卡,0xAA-禁止刷银行卡
unsigned char OperCount;                //操作员个数
unsigned char OPENPRINTF = 0;
unsigned char SavedataErr = 0;
unsigned char ReadCardFirst;
unsigned char COMNET;                   // 1: 有线TCP/IP   2: 天线WIFI     3: 天线CDMA   4: 无线GPRS
unsigned short SectionNum;
unsigned char MerNo[4] ={0};//商户号 
CardLanSector LanSec;		            //用户扇区
des_context ctx;                        //des加密算法函数使用到的上下文
st_BlackFile BlackListFile;
st_WhiteFile WhiteListFile;
bk_BlackFile BkBlackListFile;
st_BlackFile BlackListFilezhujian;
unsigned char g_FgFileOccurError = 0;
unsigned char g_FgSendErrorAgain = 0;
unsigned char g_FgHasFindCardHandle = 0;
unsigned char g_FgCardHandleOrNot = 0;  //0xaa不需要过cardhandle 匹配,0x55需要经过cardhandle匹配，
								        //0xcc需要进过cardhandle 但只取后2位OriginalCardType = 0xXX，找不到则不SET
YanZhouCard Yanzhou_Card;               //交通部城市代码结构								       


#ifdef TEST_QR_CODE_SPEED
int QRTest_fd = -1;
#endif
//联合体变量
LongUnon TransactionNum;                // 总交易次数
LongUnon SaveNum;                       //数据存储 存储记录的数量
LongUnon SaveNumBs;                     //数据存储
LongUnon SaveNumBc;                     //数据存储
LongUnon CodeNum; 		                // 已上传记录数
LongUnon Buf;		                    //硬件金额
LongUnon DevNum;	                    //终端机机号
LongUnon DevSID;                        //终端机流水号
ShortUnon Infor;
ShortUnon Driver;

/************************************************************/

extern RouteSelection JackCmd;
extern SectionFarPar Section,Sectionup;
extern int RecFd;
extern LongUnon Fixvalue;

extern pthread_mutex_t m_stationrecord;

st_BlackFile BlackListFile;
st_WhiteFile WhiteListFile;
bk_BlackFile BkBlackListFile;

extern unsigned int g_MaxPersonNumber;
extern unsigned char updataflag;	



#ifdef TEST_QR_CODE_SPEED
int QRTest_fd = -1;
#endif


extern LongUnon SeDriverCard;
extern LongUnon WatcherCard;

/*****************************************
void Bcd_To_Asc(unsigned char *Asc, unsigned char *Bcd, unsigned char nlen)
功能 ： BCD －－＞ ASCII
入口参数： data: 转换数据的入口指针
buffer: 转换后数据入口指针
len : 需要转换的长度
返回参数：转换后数据长度
*******************************************/
void Bcd_To_Asc(unsigned char *Asc, unsigned char *Bcd, unsigned char nlen)
{
	unsigned char i;
	for(i = 0; i < nlen/2; i++)
	{
		Asc[2*i] = (Bcd[i]>>4) + '0';
		Asc[2*i+1] = (Bcd[i] & 0x0f) + '0';
	}
}
/*
*************************************************************************************************************
- 函数名称 : int hex_2_ascii(INT8U *INdata, char *buffer, INT16U len)
- 函数说明 : HEX 到 ASCII的转换函数
- 入口参数： INdata
- 输出参数 : buffer
*************************************************************************************************************
*/
int hex_2_ascii(unsigned char *INdata, char *buffer, unsigned int len)
{
	const char ascTable[17] = {"0123456789ABCDEF"};
	char *tmp_p = buffer;
	unsigned int i, pos;

	pos = 0;
	for(i = 0; i < len; i++)
	{
		tmp_p[pos++] = ascTable[INdata[i] >> 4];
		tmp_p[pos++] = ascTable[INdata[i] & 0x0f];
	}
	tmp_p[pos] = '\0';
	return pos;
}

int hex_2_ascii_gps(unsigned char *INdata, char *buffer, unsigned int len)
{
	const char ascTable[17] = {"0123456789ABCDEF"};
	char *tmp_p = buffer;
	unsigned int i, pos;

	pos = 0;
	for(i = 0; i < len; i++)
	{
		tmp_p[pos++] = ascTable[INdata[i] >> 4];
		tmp_p[pos++] = ascTable[INdata[i] & 0x0f];
	}
	tmp_p[pos] = '\0';
	return pos;
}
/*
*************************************************************************************************************
- 函数名称 : unsigned char HEX2BCD(unsigned char hex_data)
- 函数说明 :
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
unsigned char HEX2BCD(unsigned char hex_data)
{
	unsigned int bcd_data;
	unsigned char temp;
	temp=hex_data%100;
	bcd_data=((unsigned int)hex_data)/100<<8;
	bcd_data=bcd_data|temp/10<<4;
	bcd_data=bcd_data|temp%10;
	temp = (unsigned char)bcd_data;
	return temp;
}

/*
*************************************************************************************************************
- 函数名称 : unsigned char HEX2BCD(unsigned char hex_data)
- 函数说明 : BCD －－ ＞HEX
- 输入参数 : BCD
- 输出参数 : HEX
*************************************************************************************************************
*/
unsigned char BCD2HEX(char bcd_data)
{
	unsigned char temp;
	temp=(((bcd_data>>4)*10)+(bcd_data&0x0f));
	return temp;
}


/*
*************************************************************************************************************
- 函数名称 : void HEX8TOBCD(unsigned int In, unsigned char *Pdata)
- 函数说明 :
- 入口参数：
- 输出参数 :
*************************************************************************************************************
*/
void HEX8TOBCD(unsigned int In, unsigned char *Pdata)
{
	unsigned int iv,i;
	unsigned char BCD[4];//定长8位BCD码
	unsigned char sv[9];
	iv = In;
	sprintf(sv,"%08u",iv);
	for(i=0; i<8; i+=2)
	{
		BCD[i/2]=(sv[i]<<4)|(sv[i+1]&0x0F);
	}
	memcpy(Pdata,BCD,4);
}


/*
*************************************************************************************************************
- 函数名称 : unsigned int  BCDToDec(const unsigned char *bcd, unsigned  char length)
- 函数说明 :
- 入口参数：
- 输出参数 :
*************************************************************************************************************
*/
unsigned int  BCDToDec(const unsigned char *bcd, unsigned  char length)
{
	int tmp;
	unsigned int dec = 0;
	unsigned char i;

	for(i = 0; i < length; i++)
	{
		tmp = ((bcd[i] >> 4) & 0x0F) * 10 + (bcd[i] & 0x0F);
		dec += tmp * pow(100, length - 1 - i);
	}

	return dec;
}


unsigned char mystrncmp(const unsigned char *istra,const unsigned char *istrb,unsigned char len)
{
	unsigned char i;

	for(i=0; i<len; i++)
	{
		if(istra[i] != istrb[i])
		{
			break;
		}
	}

	if(i==len) i=0;
	else i=1;

	return i;
}

/*
*************************************************************************************************************
- 函数名称 : char  Wr_time (char *dt)
- 函数说明 : 写时间
- 输入参数 : dt
- 输出参数 :
*************************************************************************************************************
*/
char  Wr_time (char *dt)
{
	struct tm tm;
	struct tm _tm;
	struct timeval tv;
	time_t timep;
	sscanf(dt, "%d-%d-%d %d:%d:%d", &tm.tm_year,
	       &tm.tm_mon, &tm.tm_mday,&tm.tm_hour,
	       &tm.tm_min, &tm.tm_sec);
	_tm.tm_sec = tm.tm_sec;
	_tm.tm_min = tm.tm_min;
	_tm.tm_hour = tm.tm_hour;
	_tm.tm_mday = tm.tm_mday;
	_tm.tm_mon = tm.tm_mon - 1;
	_tm.tm_year = tm.tm_year - 1900;
	timep = mktime(&_tm);
	tv.tv_sec = timep;
	tv.tv_usec = 0;
	if(settimeofday (&tv, (struct timezone *) 0) < 0)
	{
		printf("Set system datatime error!\n");
		return 255;
	}
	return 0;
}
/*
*************************************************************************************************************
- 函数名称 : char * Rd_time (char* buff)
- 函数说明 : 读时间
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
char * Rd_time (char* buff)
{
	time_t t;
	struct tm * tm;
	time (&t);
    utc_time = t;
	tm = localtime (&t);
	buff[0] = HEX2BCD((unsigned char)tm->tm_year-100);
	buff[1] = HEX2BCD(tm->tm_mon+1);
	buff[2] = HEX2BCD(tm->tm_mday);
	buff[3] = HEX2BCD(tm->tm_hour);
	buff[4] = HEX2BCD(tm->tm_min);
	buff[5] = HEX2BCD(tm->tm_sec);
	return buff;
}


/*
*************************************************************************************************************
- 函数名称 : char * Rd_time (char* buff)
- 函数说明 : 读时间
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
char Rd_timed (struct tm * tm)
{
	time_t t;
	// struct tm * tm;
	time (&t);
	tm = localtime (&t);
	return tm->tm_sec;
}


void menu_print1(char *buffer, int length)
{
	int i;

	for (i = 0; i < length; i++)
	{
		printf("%02X ", *(buffer + i));
		if ((i + 1) % 8 == 0) printf(" ");
		if ((i + 1) % 16 == 0) printf("\n");
	}
	printf("\n");
}


void sigroutine(int signal)
{
	switch (signal)
	{
	case SIGALRM:
		// close buzzer
		ioctl(bp_fd,0);
		break;
		
	case SIGVTALRM:
		break;
	}
	return;
}


/*
*************************************************************************************************************
- 函数名称 : void beepopen (unsigned char Mode)
- 函数说明 : 蜂呜器
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
extern int thread_timer;
void beepopen (unsigned char Mode)
{
#if DBG_RC500
	return;
#endif
//	printf("beep Mode %d\n",Mode);
	switch(Mode)
		{
		case 1: 			//短声
			buzz_on();
			usleep(20000);
			buzz_off();
			//thread_timer = 1;
			break;
		case 2: 			//长声
		#if 1
			buzz_on();
			usleep(50000);
			buzz_off();
		#endif
			//buzz_on();
			//thread_timer = 4;
			break;
		case 3:
			buzz_on();
			usleep(150000);
			buzz_off();
			usleep(80000);
			buzz_on();
			usleep(150000);
			buzz_off();
			usleep(80000);
			buzz_on();
			usleep(150000);
			buzz_off();
			break;
		case 4:
			buzz_on();
			usleep(150000);
			buzz_off();
			usleep(80000);
			buzz_on();
			usleep(150000);
			buzz_off();
			break;
		case 5: 			//长声
			buzz_on();
			usleep(600000);
			buzz_off();
			break;
		case 6: 			//长声
			buzz_on();
			usleep(100000);
			buzz_off();
			break;
		case 10:
			buzz_on();
			break;
	
		case 11:
			buzz_off();
			break;
	
		default :
			break;
		}

}



void SoundMode(unsigned char mode)
{
    switch(mode)
    {
        case 1:
            soundon();
            break;
        case 2:
            soundoff();
            break;
        default:
           break;
        }
    
    }

void LEDR(unsigned char ON)
{
	char sw;
	if(led_fd <= 0)	return;
		
	if(ON)
	{
		sw = 1;
		ioctl(led_fd, SPI_SERVER_GPIO_LEDRED, &sw);
		ioctl(led_fd, SPI_SERVER_GPIO_LEDRED, &sw);
	}
	else
	{
		sw = 2;
		ioctl(led_fd, SPI_SERVER_GPIO_LEDRED, &sw);
		ioctl(led_fd, SPI_SERVER_GPIO_LEDRED, &sw);
	}
}


void LEDL(unsigned char ON)
{
	char sw;
	if(led_fd <= 0)	return;

	if(ON)
	{
		sw = 1;
		ioctl(led_fd, SPI_SERVER_GPIO_LEDGREEN, &sw);
		ioctl(led_fd, SPI_SERVER_GPIO_LEDGREEN, &sw);
	}
	else
	{
		sw = 2;
		ioctl(led_fd, SPI_SERVER_GPIO_LEDGREEN, &sw);
		ioctl(led_fd, SPI_SERVER_GPIO_LEDGREEN, &sw);
	}
}




unsigned char cdma_rest(unsigned int dev)
{

	switch(dev)
	{
		case 3:
			/*
			w55fa93_setio(GPIO_GROUP_D, 6, 0);
			sleep(3);
			w55fa93_setio(GPIO_GROUP_D, 6, 1);	
			*/
			w55fa93_setio(GPIO_GROUP_A, 1, 0);
			sleep(5);
			w55fa93_setio(GPIO_GROUP_A, 1, 1);	
			break;
		case 4:
#if WCDMA
            w55fa93_setio(GPIO_GROUP_D, 6, 1);
			usleep(150000);
			w55fa93_setio(GPIO_GROUP_D, 6, 0);	
			sleep(7);
#else
          //  usleep(5000);
		/*	w55fa93_setio(GPIO_GROUP_D, 6, 0);
			usleep(150000);
			w55fa93_setio(GPIO_GROUP_D, 6, 1);		
		*/	
		#if 1//def SHENGKE_TIANGUAN
			break;  //no reset
		#endif
			w55fa93_setio(GPIO_GROUP_A, 1, 0);
			sleep(5);
			w55fa93_setio(GPIO_GROUP_A, 1, 1);
			//sleep(7);  //7
#endif            
			break;

	   case 5:   //第一次开机不用关掉模块电源  test
		w55fa93_setio(GPIO_GROUP_A, 1, 1);
		sleep(5);  //30
		break;
		
		default:
			break;
	}
	return 0;
}





unsigned char relaycontrol(unsigned ON)
{
	
	char sw;
	if(led_fd <= 0)	return 1;
		
    if(ON)
    {
        sw = 1;
		ioctl(led_fd, SPI_SERVER_GPIO_RELAY, &sw);
    }
    else
    {
        sw = 0;
		ioctl(led_fd, SPI_SERVER_GPIO_RELAY, &sw);
    }
}



/*
*************************************************************************************************************
- 函数名称 : char ethopen(void)
- 函数说明 : 判断连接方式
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
char Ipkey[50];
char ethopen(void)
{
	int ret;
	char status;
	char buff[60];
	 char strings[5][32]={0};
	 char *result = NULL;
	 int index = 0;
	if(access("tabneton.bin",0)== 0)
	{

#if Initprintf
		printf("tabneton.bin  open\n");
#endif
		nettab = fopen("tabneton.bin","rb+");
		ret = fseek(nettab,0, SEEK_SET);
		memset (buff,0,sizeof(buff));
		ret = fread(buff,sizeof(unsigned char),60,nettab);
		fclose(nettab);

#if Initprintf
		printf("ethopen  buff== %s \n",buff);
#endif

		switch(buff[0])
		{
		case '1': // TCP/IP
			status = 1;
			break;

		case '2':// WIFI
			/*memset(Ipkey,0,sizeof(Ipkey));
			memcpy(Ipkey,buff+1,48);
			system("ifconfig ra0 > teth.bin");
			usleep(50000);
			teth = fopen("teth.bin","rb+");
			memset(buff,0,sizeof(buff));
			ret = fseek(teth,0, SEEK_SET);
			memset (buff,0,sizeof(buff));
			ret = fread(buff,sizeof(unsigned char),8,teth);
			fclose(teth);
			if(mystrncmp(buff,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)
			{
				status = 1;
			}
			else
			{
				status = 2;
			}
            */
            status = 2;
			break;

		case '3':// CDMA

			status = 3;
			break;

		case '4':// GPRS
			status = 4;
			break;
		default:
			status = 1;
			break;
		}

	}
	else
	{
		memset(buff,0,sizeof(buff));
		sprintf(buff,"4cardlan 89966666");
		nettab = fopen("tabneton.bin","a+");
		ret = fseek(nettab,0, SEEK_SET);
		ret = fwrite(buff,sizeof(unsigned char),strlen(buff),nettab);
		fclose(nettab);
		status = 1;
	}
	
	if (strstr(buff, "IP") && strstr(buff, "PORT"))   //配置文件设置IP与端口
	{
		result = strtok(buff, ":");
		while(result != NULL) {
			strcpy(strings[index], result);
		         index++;
		        result = strtok( NULL, ":");
		}
		printf("IP = %s\n", strings[1]);
		strcpy(TcpIpBuf, strings[1]);
		result = strchr(strings[3], 'E');
		if(result)
			*result = '\0';
		printf("PORT = %s\n", strings[3]);
		Infor.i = atoi(strings[3]);
		memcpy(TcpIpBuf+32, Infor.intbuf, 2);
	}
#if Initprintf
	printf("ethopen status == %d \n",status);
#endif

	return status;

}


/*===========黑名单新方式===========*/

//-----------------------------------------------------------------------------------
int half_search(BlackItem dest, int *find)
{
	int low,high,mid,val;
	low = 0;
	*find = 0;
	low = 0;
	mid = 0;
  
	if((BlackListFile.count == 0)||(BlackListFile.buf==NULL))
		return 0;

	high = BlackListFile.count - 1;

	DBG_PRINTF("high= %d\n", high);
	DBG_PRINTF("dest: %02X%02X%02X%02X%02X%02X%02X%02X\n", dest.dat[0],dest.dat[1],\
		dest.dat[2],dest.dat[3],dest.dat[4],dest.dat[5],dest.dat[6],dest.dat[7]);
	
	//printf("正在折半查找黑名单\n");
	while (low <= high)
	{
		mid = (low + high)/ 2;

		DBG_PRINTF("BlackListFile.buf[mid].dat: %02X%02X%02X%02X%02X%02X%02X%02X\n", BlackListFile.buf[mid].dat[0],BlackListFile.buf[mid].dat[1],\
		BlackListFile.buf[mid].dat[2],BlackListFile.buf[mid].dat[3],BlackListFile.buf[mid].dat[4],BlackListFile.buf[mid].dat[5],BlackListFile.buf[mid].dat[6],BlackListFile.buf[mid].dat[7]);
		
		//printf("dest %p buf %p \n",dest.dat,BlackListFile.buf[mid].dat);
		val = memcmp(dest.dat, BlackListFile.buf[mid].dat, 10 );//izeof(BlackListFile.buf[0]));
		//  val= 1;	
		//printf("使用memcmp没死掉\n");
		if (val == 0)
		{
			unsigned char src[10];
			memcpy(src, BlackListFile.buf[mid].dat, sizeof(BlackListFile.buf[0]));
			DBG_PRINTF("src: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n", src[0],src[1],\
				src[2],src[3],src[4],src[5],src[6],src[7],src[8],src[9]);

			*find = 1;
			low = mid;
			break;
		}
		else if (val > 0)
			low = mid + 1;	//dest > src
		else
			high = mid - 1;
	}

	DBG_PRINTF("low= %d find=%d\n", low, *find);
   
//	printf("half_serach结束\n");
	return low;
}

int half_search_bank(BkBlackItem dest, int *find)
{
	int low,high,mid,val;
    unsigned char src[8];
	low = 0;
	*find = 0;
	low = 0;
	mid = 0;
  
	if((BkBlackListFile.count == 0)||(BkBlackListFile.buf==NULL))
		return 0;

	high = BkBlackListFile.count - 1;

	DBG_PRINTF("high= %d\n", high);
	DBG_PRINTF("dest: %02X%02X%02X%02X%02X%02X%02X%02X\n", dest.dat[0],dest.dat[1],\
		dest.dat[2],dest.dat[3],dest.dat[4],dest.dat[5],dest.dat[6],dest.dat[7]);
	
	//printf("正在折半查找黑名单\n");
	while (low <= high)
	{
		mid = (low + high)/ 2;

		DBG_PRINTF("BlackListFile.buf[mid].dat: %02X%02X%02X%02X%02X%02X%02X%02X\n", BkBlackListFile.buf[mid].dat[0],BkBlackListFile.buf[mid].dat[1],\
		BkBlackListFile.buf[mid].dat[2],BkBlackListFile.buf[mid].dat[3],BkBlackListFile.buf[mid].dat[4],BkBlackListFile.buf[mid].dat[5],BkBlackListFile.buf[mid].dat[6],BkBlackListFile.buf[mid].dat[7]);
		
		//printf("dest %p buf %p \n",dest.dat,BlackListFile.buf[mid].dat);
		val = memcmp(dest.dat, BkBlackListFile.buf[mid].dat, 8 );//izeof(BlackListFile.buf[0]));
		//  val= 1;	
		//printf("使用memcmp没死掉\n");
		if (val == 0)
		{			
			memcpy(src, BkBlackListFile.buf[mid].dat, sizeof(BkBlackListFile.buf[0]));
			DBG_PRINTF("src: %02X%02X%02X%02X%02X%02X%02X%02X\n", src[0],src[1],\
				src[2],src[3],src[4],src[5],src[6],src[7]);

			*find = 1;
			low = mid;
			break;
		}
		else if (val > 0)
			low = mid + 1;	//dest > src
		else
			high = mid - 1;
	}

	DBG_PRINTF("low= %d find=%d\n", low, *find);
   
//	printf("half_serach结束\n");
	return low;
}

int half_search_zhujian(BlackItem dest, int *find)
{
	int low,high,mid,val;
	low = 0;
	*find = 0;
	low = 0;
	mid = 0;
  
	if((BlackListFilezhujian.count == 0)||(BlackListFilezhujian.buf==NULL))
		return 0;

	high = BlackListFilezhujian.count - 1;

	DBG_PRINTF("high= %d\n", high);
	DBG_PRINTF("dest: %02X%02X%02X%02X%02X%02X%02X%02X\n", dest.dat[0],dest.dat[1],\
		dest.dat[2],dest.dat[3],dest.dat[4],dest.dat[5],dest.dat[6],dest.dat[7]);
	
	//printf("正在折半查找黑名单\n");
	while (low <= high)
	{
		mid = (low + high)/ 2;

		DBG_PRINTF("BlackListFile.buf[mid].dat: %02X%02X%02X%02X%02X%02X%02X%02X\n", BlackListFilezhujian.buf[mid].dat[0],BlackListFilezhujian.buf[mid].dat[1],\
		BlackListFilezhujian.buf[mid].dat[2],BlackListFilezhujian.buf[mid].dat[3],BlackListFilezhujian.buf[mid].dat[4],BlackListFilezhujian.buf[mid].dat[5],BlackListFilezhujian.buf[mid].dat[6],BlackListFilezhujian.buf[mid].dat[7]);
		
		//printf("dest %p buf %p \n",dest.dat,BlackListFile.buf[mid].dat);
		val = memcmp(dest.dat, BlackListFilezhujian.buf[mid].dat, 10 );//izeof(BlackListFile.buf[0]));
		//  val= 1;	
		//printf("使用memcmp没死掉\n");
		if (val == 0)
		{
			unsigned char src[10];
			memcpy(src, BlackListFilezhujian.buf[mid].dat, sizeof(BlackListFilezhujian.buf[0]));
			DBG_PRINTF("src: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n", src[0],src[1],\
				src[2],src[3],src[4],src[5],src[6],src[7],src[8],src[9]);

			*find = 1;
			low = mid;
			break;
		}
		else if (val > 0)
			low = mid + 1;	//dest > src
		else
			high = mid - 1;
	}

	DBG_PRINTF("low= %d find=%d\n", low, *find);
   
//	printf("half_serach结束\n");
	return low;
}


void ReloadBlackListBuff(void)
{
	FILE *fp;
	int i,j;
//	BlackListFile.buf = NULL;
	BkBlackListFile.count = 0;
	
	memset(BkBlackListFile.buf, 0x00, (MAX_BLACK_CONNT * sizeof(BkBlackListFile.buf[0])));

	fp = fopen(BANK_BL_FILE, "rb+");
	if(fp == NULL)
	{	
		printf("Can't open tht Blacklist.sys\n");
		return;
	}

	fseek(fp,0,SEEK_END);
	BkBlackListFile.count = ftell(fp) / sizeof(BkBlackListFile.buf[0]);
	fseek(fp,0,SEEK_SET);
	fread(BkBlackListFile.buf, sizeof(BlackItem), BkBlackListFile.count, fp);
	fclose(fp);
}

void update_sortfile(BlackItem dat, unsigned char cmd)
{
	int find,count;
	int i,pos;

	count = BlackListFile.count;
	i = 0;
	pos = half_search(dat, &find);

	if (cmd == 0 && !find)
	{ /*update*/
		for(i=BlackListFile.count; i>pos; i--)
		{
			BlackListFile.buf[i] = BlackListFile.buf[i-1];
		}
		BlackListFile.buf[pos] = dat;
		BlackListFile.count++;
	}
	else if (cmd == 1 && find && (BlackListFile.count > 0))
	{ /*enbale*/
		i=pos;
		while(i < (BlackListFile.count-1))
		{
			BlackListFile.buf[i] = BlackListFile.buf[i+1];
			i++;
		}
		memset( BlackListFile.buf[BlackListFile.count-1].dat, 0x00, sizeof(BlackListFile.buf[0]));
		BlackListFile.count--;
	}
}

void SavetBlackListBuff()
{
	FILE *stream;
	
	stream= fopen(CITYUNION_BL_FILEBAK,"wb+");
	fseek(stream, 0, SEEK_SET);
	fwrite(BlackListFile.buf, sizeof(unsigned char), BlackListFile.count * sizeof(BlackListFile.buf[0]), stream);

	printf("BlackListFile.count %d\n", BlackListFile.count);
	printf("sizeof(BlackListFile.buf[0] %d\n", sizeof(BlackListFile.buf[0]) );
	printf("SavetBlackListBuff(): %d\n", BlackListFile.count * sizeof(BlackListFile.buf[0]));

	fclose_nosync(stream);	
}


void InitBlackListBuff()
{
#if 0
	FILE *fp;
	FILE *fpbak;

	BlackListFile.buf = NULL;
	BlackListFile.count = 0;
	
	BlackListFile.buf = (BlackItem *)malloc(MAX_BLACK_CONNT * sizeof(BlackListFile.buf[0]));
	if(BlackListFile.buf == NULL) return;
	memset(BlackListFile.buf, 0x00, (MAX_BLACK_CONNT * sizeof(BlackListFile.buf[0])));
	
	fp = fopen(CITYUNION_BL_FILE, "rb+");
	if(fp == NULL)
	{	
		printf("Can't open tht Blacklist.sys\n");
		return;
	}

	fseek(fp,0,SEEK_END);
	BlackListFile.count = ftell(fp) / sizeof(BlackListFile.buf[0]);	
	fseek(fp,0,SEEK_SET);
	fread(BlackListFile.buf, sizeof(BlackItem), BlackListFile.count, fp);
	fclose(fp);
#else
		FILE *fp;
		int i,j;
	BlackListFile.buf = NULL;
	BlackListFile.count = 0;
	
	//BlackListFile.buf = (BlackItem *)malloc(MAX_BLACK_CONNT * sizeof(BlackListFile.buf[0]));
	BlackListFile.buf = (BlackItem *)malloc(MAX_BLACK_CONNT * sizeof(BlackItem));
	if(BlackListFile.buf == NULL) return;
	memset(BlackListFile.buf, 0x00, (MAX_BLACK_CONNT * sizeof(BlackListFile.buf[0])));

	fp = fopen(CITYUNION_BL_FILEBAK, "rb");
	if(fp == NULL)
	{	
		printf("Can't open the Blacklist.sys\n");
		return;
	}

	fseek(fp,0,SEEK_END);
	BlackListFile.count = ftell(fp) / sizeof(BlackListFile.buf[0]);
	fseek(fp,0,SEEK_SET);
	fread(BlackListFile.buf, sizeof(BlackItem), BlackListFile.count, fp);
	fclose_nosync(fp);
#if DEBUG	
	for(i=0;i<BlackListFile.count;i++)
	{
		printf("BlackListFile.buf[%d].dat: %02X%02X%02X%02X%02X%02X%02X%02X\n", i,BlackListFile.buf[i].dat[0],BlackListFile.buf[i].dat[1],\
		BlackListFile.buf[i].dat[2],BlackListFile.buf[i].dat[3],BlackListFile.buf[i].dat[4],BlackListFile.buf[i].dat[5],BlackListFile.buf[i].dat[6],BlackListFile.buf[i].dat[7]);
	}
#endif		
#endif
}

int half_search_white(WhiteItem dest, int *find)
{
	int low,high,mid,val;
	low = 0;
	*find = 0;
	low = 0;
	mid = 0;

	if((WhiteListFile.count == 0)||(WhiteListFile.buf==NULL))
		return 0;

	high = WhiteListFile.count - 1;

	DBG_PRINTF("high= %d\n", high);
	DBG_PRINTF("dest: %02X%02X%02X%02X\n", dest.dat[0],dest.dat[1],\
		dest.dat[2],dest.dat[3]);
	
	//printf("正在折半查找黑名单\n");
	while (low <= high)
	{
		mid = (low + high)/ 2;

		DBG_PRINTF("WhiteListFile.buf[mid].dat: %02X%02X%02X%02X\n", WhiteListFile.buf[mid].dat[0],WhiteListFile.buf[mid].dat[1],\
		WhiteListFile.buf[mid].dat[2],WhiteListFile.buf[mid].dat[3]);

		
		//printf("dest %p buf %p \n",dest.dat,BlackListFile.buf[mid].dat);
		val = memcmp(dest.dat, WhiteListFile.buf[mid].dat, 4 );//izeof(BlackListFile.buf[0]));
		//  val= 1;	
		//printf("使用memcmp没死掉\n");
		if (val == 0)
		{
			unsigned char src[4];
			memcpy(src, WhiteListFile.buf[mid].dat, sizeof(WhiteListFile.buf[0]));
			DBG_PRINTF("src: %02X%02X%02X%02X%\n", src[0],src[1],\
				src[2],src[3]);

			*find = 1;
			low = mid;
			break;
		}
		else if (val > 0)
			low = mid + 1;	//dest > src
		else
			high = mid - 1;
	}

	DBG_PRINTF("low= %d find=%d\n", low, *find);

//	printf("half_serach结束\n");
	return low;
}


void update_sortfile_white(WhiteItem dat, unsigned char cmd)
{
	int find,count;
	int i,pos;

	count = WhiteListFile.count;
	i = 0;
	pos = half_search_white(dat, &find);

	if (cmd == 0 && !find)
	{ /*update*/
		for(i=WhiteListFile.count; i>pos; i--)
		{
			WhiteListFile.buf[i] = WhiteListFile.buf[i-1];
		}
		WhiteListFile.buf[pos] = dat;
		WhiteListFile.count++;
	}
	else if (cmd == 1 && find && (WhiteListFile.count > 0))
	{ /*enbale*/
		i=pos;
		while(i < (WhiteListFile.count-1))
		{
			WhiteListFile.buf[i] = WhiteListFile.buf[i+1];
			i++;
		}
		memset( WhiteListFile.buf[WhiteListFile.count-1].dat, 0x00, sizeof(WhiteListFile.buf[0]));
		WhiteListFile.count--;
	}
}

void SavetWhiteListBuff()
{
	FILE *stream;
	
	stream= fopen(CITYUNION_BL_FILEWHI,"wb+");
	fseek(stream, 0, SEEK_SET);
	fwrite(WhiteListFile.buf, sizeof(unsigned char), WhiteListFile.count * sizeof(WhiteListFile.buf[0]), stream);

	printf("BlackListFile.count %d\n", WhiteListFile.count);
	printf("sizeof(BlackListFile.buf[0] %d\n", sizeof(WhiteListFile.buf[0]) );
	printf("SavetBlackListBuff(): %d\n", WhiteListFile.count * sizeof(WhiteListFile.buf[0]));

	fclose_nosync(stream);	
}



void InitWhiteListBuff()
{

		FILE *fp;
		int i,j;
	WhiteListFile.buf = NULL;
	WhiteListFile.count = 0;
	
	//BlackListFile.buf = (BlackItem *)malloc(MAX_BLACK_CONNT * sizeof(BlackListFile.buf[0]));
	WhiteListFile.buf = (WhiteItem *)malloc(MAX_BLACK_CONNT * sizeof(WhiteItem));
	if(WhiteListFile.buf == NULL) return;
	memset(WhiteListFile.buf, 0x00, (MAX_BLACK_CONNT * sizeof(WhiteListFile.buf[0])));

	fp = fopen(CITYUNION_BL_FILEWHI, "rb");
	if(fp == NULL)
	{	
		printf("Can't open the Whitelist.sys\n");
		return;
	}

	fseek(fp,0,SEEK_END);
	WhiteListFile.count = ftell(fp) / sizeof(WhiteListFile.buf[0]);
	fseek(fp,0,SEEK_SET);
	fread(WhiteListFile.buf, sizeof(WhiteItem), WhiteListFile.count, fp);
	fclose_nosync(fp);
#if DEBUG	
	for(i=0;i<WhiteListFile.count;i++)
	{
		printf("WhiteListFile.buf[%d].dat: %02X%02X%02X%02X\n", i,WhiteListFile.buf[i].dat[0],BlackListFile.buf[i].dat[1],\
		WhiteListFile.buf[i].dat[2],WhiteListFile.buf[i].dat[3]);
	}
#endif		

}



//读取商户号
//注:商户号的位置在参数文件中第二个字节开始的四个字节
void ReadMERCHANTNO(void)
{
	int ret;
	char buff[50];

	if(access("/mnt/record/mercode.bin",0)== 0) 
	{ 

#if Initprintf
		printf("mercode.bin  open\n");       
#endif      
		if((MCHID = fopen("/mnt/record/mercode.bin","rb+")) == NULL)
		{
			perror ("open mercode.bin error !");    
			return;
		}
		fseek(MCHID,0, SEEK_SET);
		memset (buff,0,sizeof(buff));
		ret = fread(buff,sizeof(unsigned char),32,MCHID);
		//printf("MCHID = %02x%02x%02x%02x\n",MCHID[0],MCHID[1],MCHID[2],MCHID[3]);
		if(ret >0)  
		{ 
			memcpy(MerNo,buff,4);        
		}
		else
		{
			perror("read error"); 
		} 
		fclose(MCHID); 
#if Initprintf 
		printf(" buff== %s \n",buff);
#endif
	}
	/*
	else
	{
		fclose(MCHID);
	}   
	*/
}


/*
*************************************************************************************************************
- 函数名称 : void ReadOrWriteFile (unsigned char RW_Type)
- 函数说明 : 读写初始化数据 函数
- 输入参数 : RW_Type
- 输出参数 : 无
*************************************************************************************************************
*/



void ReadOrWriteFileB(unsigned char RW_Type)
{
	int canshu, result;
	char status;
	unsigned char i;//j;
	unsigned char fileBuffer[16];
	unsigned char FileBuf[8];
	unsigned char filebuf[80];
	unsigned int j;
	LongUnon tmp,tmp1;

	des_set_key(&ctx,des_key); //设置des加密密码
	switch (RW_Type)
	{
	case SYSFILE:					//读取密钥
		canshu = open(SYS_PARAM_FILE, O_SYNC|O_RDWR);
		result = lseek(canshu, 0, SEEK_SET);
		memset (FileBuf,0,sizeof(FileBuf));
		result = read(canshu,FileBuf,8);
		if(mystrncmp(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)
		{
			memcpy(FileBuf,"\x11\x22\x33\x44\x55\x66\x77\x88",8);
			des_encrypt(&ctx, FileBuf, FileBuf);
			result = lseek(canshu,0, SEEK_SET);
			result = write(canshu,FileBuf,8);

			result = lseek(canshu, 0, SEEK_SET);
			memset (FileBuf,0,sizeof(FileBuf));
			result = read(canshu,FileBuf,8);
			des_decrypt( &ctx, FileBuf, FileBuf);
			memcpy(KeyDes,FileBuf,8);
		}
		else
		{
			des_decrypt( &ctx, FileBuf, FileBuf);
			memcpy(KeyDes,FileBuf,8);
		}

		memset (FileBuf,0,sizeof(FileBuf));		//读取终端机号
		result = lseek(canshu, 16, SEEK_SET);
		result = read(canshu,FileBuf,8);
		if(mystrncmp(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)
		{
			memcpy(FileBuf,"\x01\x00\x00\x00\x00\x00\x00\x00",8);
			des_encrypt(&ctx, FileBuf, FileBuf);
			result = lseek(canshu,16, SEEK_SET);
			result = write(canshu,FileBuf,8);

			result = lseek(canshu, 16, SEEK_SET);
			memset (FileBuf,0,sizeof(FileBuf));
			result = read(canshu,FileBuf,8);
			des_decrypt( &ctx, FileBuf, FileBuf);
			memcpy(DevNum.longbuf,FileBuf,4);
			memset(PsamNum,0,sizeof(PsamNum));
			memcpy(PsamNum+2,FileBuf,4);
			memcpy(PsamNum_bak1,PsamNum,sizeof(PsamNum));
		}
		else
		{
			des_decrypt( &ctx, FileBuf, FileBuf);
			memcpy(DevNum.longbuf,FileBuf,4);
			memset(PsamNum,0,sizeof(PsamNum));
			memcpy(PsamNum+2,FileBuf,4);
			memcpy(PsamNum_bak1,PsamNum,sizeof(PsamNum));
		}


		memset (FileBuf,0,sizeof(FileBuf));		//读取管理员密码
		result = lseek(canshu, 32, SEEK_SET);
		result = read(canshu,FileBuf,8);
		if(mystrncmp(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)
		{
			memcpy(FileBuf,"\x30\x30\x30\x30\x30\x30\x00\x00",8);
			des_encrypt(&ctx, FileBuf, FileBuf);
			result = lseek(canshu,32, SEEK_SET);
			result = write(canshu,FileBuf,8);

			result = lseek(canshu, 32, SEEK_SET);
			memset (FileBuf,0,sizeof(FileBuf));
			result = read(canshu,FileBuf,8);
			des_decrypt( &ctx, FileBuf, FileBuf);
			memcpy(AdminPwd,FileBuf,8);
		}
		else
		{
			des_decrypt( &ctx, FileBuf, FileBuf);
			memcpy(AdminPwd,FileBuf,8);
		}


		memset (FileBuf,0,sizeof(FileBuf));			//修改待机时间
		result = lseek(canshu, 48, SEEK_SET);
		result = read(canshu,FileBuf,8);
		if(mystrncmp(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)
		{
			memcpy(FileBuf,"\x1E\x00\x00\x00\x00\x00\x00\x00",8);
			des_encrypt(&ctx, FileBuf, FileBuf);
			result = lseek(canshu,48, SEEK_SET);
			result = write(canshu,FileBuf,8);

			result = lseek(canshu, 48, SEEK_SET);
			memset (FileBuf,0,sizeof(FileBuf));
			result = read(canshu,FileBuf,8);
			des_decrypt(&ctx, FileBuf, FileBuf);
			SleepOverTime	= FileBuf[0];
		}
		else
		{
			des_decrypt(&ctx, FileBuf, FileBuf);
			SleepOverTime	= FileBuf[0];
		}

		memset (FileBuf,0,sizeof(FileBuf));
		result = lseek(canshu, 64, SEEK_SET);
		result = read(canshu,FileBuf,8);		//读取用户扇区
		if(mystrncmp(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)
		{
			memcpy(FileBuf,"\x01\x02\x03\x04\x05\x06\x07\x00",8);
			des_encrypt(&ctx, FileBuf, FileBuf);
			result = lseek(canshu,64, SEEK_SET);
			result = write(canshu,FileBuf,8);

			result = lseek(canshu, 64, SEEK_SET);
			memset (FileBuf,0,sizeof(FileBuf));
			result = read(canshu,FileBuf,8);
			des_decrypt( &ctx, FileBuf, FileBuf);
			memcpy(&LanSec,FileBuf,8);
		}
		else
		{
			des_decrypt( &ctx, FileBuf, FileBuf);
			memcpy(&LanSec,FileBuf,8);
		}

		memset (filebuf,0,sizeof(filebuf));
		result = lseek(canshu, 80, SEEK_SET);
		result = read(canshu,filebuf,40);	//读取IP
		if(mystrncmp(filebuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)
		{
			memset(filebuf,0,sizeof(filebuf));
			sprintf(filebuf,"172.0.9.119");
			memcpy(filebuf+32,"\x0c\x1a",2);
			for(i = 0; i < 5; i ++)
			{
				des_encrypt(&ctx, filebuf+i*8, filebuf+i*8);
			}
			result = lseek(canshu,80, SEEK_SET);
			result = write(canshu,filebuf,40);

			result = lseek(canshu, 80, SEEK_SET);
			memset (filebuf,0,sizeof(filebuf));
			result = read(canshu,filebuf,40);
			for(i = 0; i < 5; i ++)
			{
				des_decrypt(&ctx, filebuf+i*8, filebuf+i*8);
			}
			memcpy(TcpIpBuf,filebuf,35);
		}
		else
		{
			for(i = 0; i < 5; i ++)
			{
				des_decrypt(&ctx, filebuf+i*8, filebuf+i*8);
			}
			memcpy(TcpIpBuf,filebuf,35);
		}



		memset (FileBuf,0,sizeof(FileBuf));
		result = lseek(canshu, 128, SEEK_SET);
		result = read(canshu,FileBuf,8);		//CPU卡应用
		if(mystrncmp(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)
		{
			memcpy(FileBuf,"\x3f\x00\x00\x00\x00\x00\x00\x00",8);
			des_encrypt(&ctx, FileBuf, FileBuf);
			result = lseek(canshu,128, SEEK_SET);
			result = write(canshu,FileBuf,8);

			result = lseek(canshu, 128, SEEK_SET);
			memset (FileBuf,0,sizeof(FileBuf));
			result = read(canshu,FileBuf,8);
			des_decrypt(&ctx, FileBuf, FileBuf);
			memcpy(LanSec.ADFNUM,FileBuf,2);

		}
		else
		{
			des_decrypt(&ctx, FileBuf, FileBuf);
			memcpy(LanSec.ADFNUM,FileBuf,2);
		}


		memset (FileBuf,0,sizeof(FileBuf));
		result = lseek(canshu, 144, SEEK_SET);
		result = read(canshu,FileBuf,8);		//出车流水ID
		if(mystrncmp(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)
		{
			memcpy(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8);
			des_encrypt(&ctx, FileBuf, FileBuf);
			result = lseek(canshu,144, SEEK_SET);
			result = write(canshu,FileBuf,8);

			result = lseek(canshu, 144, SEEK_SET);
			memset (FileBuf,0,sizeof(FileBuf));
			result = read(canshu,FileBuf,8);
			des_decrypt(&ctx, FileBuf, FileBuf);
			memcpy(DevSID.longbuf,FileBuf,4);
		}
		else
		{
			des_decrypt(&ctx, FileBuf, FileBuf);
			memcpy(DevSID.longbuf,FileBuf,4);
		}

		memset (FileBuf,0,sizeof(FileBuf));
		result = lseek(canshu, 160, SEEK_SET);
		result = read(canshu,FileBuf,8);	//上送记录条数
		if(mystrncmp(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)
		{
			memcpy(FileBuf,"\x01\x00\x00\x00\x00\x00\x00\x00",8);
			des_encrypt(&ctx, FileBuf, FileBuf);
			result = lseek(canshu,160, SEEK_SET);
			result = write(canshu,FileBuf,8);

			result = lseek(canshu, 160, SEEK_SET);
			memset (FileBuf,0,sizeof(FileBuf));
			result = read(canshu,FileBuf,8);
			des_decrypt(&ctx, FileBuf, FileBuf);
			memcpy(CodeNum.longbuf,FileBuf,4);
		}
		else
		{
			des_decrypt(&ctx, FileBuf, FileBuf);
			memcpy(CodeNum.longbuf,FileBuf,4);
		}

		memset (FileBuf,0,sizeof(FileBuf));
		result = lseek(canshu, 176, SEEK_SET);
		result = read(canshu,FileBuf,8);	//按鍵声
		if(mystrncmp(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)
		{
			memcpy(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8);
			des_encrypt(&ctx, FileBuf, FileBuf);
			result = lseek(canshu,176, SEEK_SET);
			result = write(canshu,FileBuf,8);

			result = lseek(canshu, 176, SEEK_SET);
			memset (FileBuf,0,sizeof(FileBuf));
			result = read(canshu,FileBuf,8);
			des_decrypt(&ctx, FileBuf, FileBuf);
			OPENBEEP = FileBuf[0];
		}
		else
		{
			des_decrypt(&ctx, FileBuf, FileBuf);
			OPENBEEP = FileBuf[0];
		}


		memset (FileBuf,0,sizeof(FileBuf));
		result = lseek(canshu, 192, SEEK_SET);
		result = read(canshu,FileBuf,8);		//按鍵声
		if(mystrncmp(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)
		{
			memcpy(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8);
			des_encrypt(&ctx, FileBuf, FileBuf);
			result = lseek(canshu,192, SEEK_SET);
			result = write(canshu,FileBuf,8);

			result = lseek(canshu, 192, SEEK_SET);
			memset (FileBuf,0,sizeof(FileBuf));
			result = read(canshu,FileBuf,8);
			des_decrypt(&ctx, FileBuf, FileBuf);
			OperCount = FileBuf[0];
			if(OperCount >= 11)  OperCount = 0;
		}
		else
		{
			des_decrypt(&ctx, FileBuf, FileBuf);
			OperCount = FileBuf[0];
			if(OperCount >= 11)  OperCount = 0;
		}

		memset(SelfAddress,0,sizeof(SelfAddress));
		memset (filebuf,0,sizeof(filebuf));
		result = lseek(canshu, 384, SEEK_SET);
		result = read(canshu,filebuf,32);
		if(mystrncmp(filebuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)
		{
			memset(filebuf,0,sizeof(filebuf));
			memset(fileBuffer,0,sizeof(fileBuffer));

			sprintf(filebuf,"00.00.00.00");
			sprintf(fileBuffer,"00.00.00.00");
			//sprintf(filebuf,"172.0.9.115");
			//sprintf(fileBuffer,"172.0.0.1");
			memcpy(filebuf+16,fileBuffer,16);

			for(i = 0; i < 4; i ++)
			{
				des_encrypt(&ctx, filebuf+i*8, filebuf+i*8);
			}
			result = lseek(canshu,384, SEEK_SET);
			result = write(canshu,filebuf,32);

			result = lseek(canshu, 384, SEEK_SET);
			memset (filebuf,0,sizeof(filebuf));
			result = read(canshu,filebuf,32);
			for(i = 0; i < 4; i ++)
			{
				des_decrypt(&ctx, filebuf+i*8, filebuf+i*8);
			}
			memcpy(SelfAddress,filebuf,32);
		}
		else
		{
			for(i = 0; i < 4; i ++)
			{
				des_decrypt(&ctx, filebuf+i*8, filebuf+i*8);
			}
			memcpy(SelfAddress,filebuf,32);
		}

		status = ethopen();
		switch(status)
		{
		case 4: //GPRS
			COMNET = 4;
#if Initprintf
			printf("gprs  ppp  \n");
#endif
			break;

		case 3: // CDMA
			COMNET = 3;
#if Initprintf
			printf("CDMA ppp  \n");
#endif
			break;

		case 2: //WIFI
			COMNET = 2;
		/*	system("ifconfig ra0 192.168.1.1");
			usleep(50000);
			memset(filebuf,0,sizeof(filebuf));
			sprintf(filebuf,"wpa_passphrase ");
			memcpy(filebuf+15,Ipkey,strlen(Ipkey));
			strcat(filebuf," >/var/run/wpa_supplicant.conf");
#if Initprintf
			printf("ip wifi %s \n",filebuf);
#endif
			system(filebuf);
			// system("wpa_passphrase cardlan 89966666 >/var/run/wpa_supplicant.conf");
			usleep(50000);
			system("killall linkwifi.sh");
			usleep(50000);
			system("./linkwifi.sh&");
			usleep(50000);*/
			break;

		case 1: // TCP/IP
		default :
			COMNET = 1;
			if((TcpIpBuf[0]>='0')&&(TcpIpBuf[0]<='9')&&(SelfAddress[0]>'0')&&(SelfAddress[0]<='9'))
			{
				memset(fileBuffer,0,sizeof(fileBuffer));
				memset(filebuf,0,sizeof(filebuf));
				sprintf(filebuf,"nameserver ");
				memcpy(fileBuffer,SelfAddress+16,16);
				strcat(filebuf,fileBuffer);

				// nettab = fopen("resolv.conf","a+");
				//  ret = fseek(nettab,0, SEEK_SET);
				//   ret = fwrite(filebuf,sizeof(unsigned char),strlen(filebuf),nettab);
				//  fclose(nettab);
				// system("cp resolv.conf /etc/");
				// usleep(50000);
				// system("rm resolv.conf");

				memset(fileBuffer,'\0',sizeof(fileBuffer));
				memset(filebuf,'\0',sizeof(filebuf));
				memcpy(fileBuffer,SelfAddress,16);
				sprintf(filebuf,"ifconfig eth0 ");
				strcat(filebuf,fileBuffer);
				system(filebuf);
				usleep(50000);

#if Initprintf
				printf("ip eth0 %s \n",fileBuffer);
#endif

				memset(fileBuffer,'\0',sizeof(fileBuffer));
				memcpy(fileBuffer,SelfAddress+16,16);
				sprintf(filebuf,"route add default gw ");
				strcat(filebuf,fileBuffer);
				system(filebuf);
				usleep(50000);
				system("route&");

			}
			else
			{
				system("killall -9 udhcpc");
				system("udhcpc &");
				usleep(50000);
			}
			break;
		}


		memset (FileBuf,0,sizeof(FileBuf));
		result = lseek(canshu,432, SEEK_SET);
		result = read(canshu,FileBuf,8);	//保存记录条数
		if(mystrncmp(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)
		{
			memcpy(FileBuf,"\x01\x00\x00\x00\x01\x00\x00\x00",8);
			des_encrypt(&ctx, FileBuf, FileBuf);
			result = lseek(canshu,432, SEEK_SET);
			result = write(canshu,FileBuf,8);

			result = lseek(canshu,432, SEEK_SET);
			memset (FileBuf,0,sizeof(FileBuf));
			result = read(canshu,FileBuf,8);
			des_decrypt(&ctx, FileBuf, FileBuf);
			memcpy(SaveNum.longbuf,FileBuf,4);
			memcpy(TransactionNum.longbuf,FileBuf + 4,4);
			SaveNumBc.i = SaveNumBs.i = SaveNum.i ;
		}
		else
		{
			des_decrypt(&ctx, FileBuf, FileBuf);
			memcpy(SaveNum.longbuf,FileBuf,4);
			memcpy(TransactionNum.longbuf,FileBuf + 4,4);
			SaveNumBc.i = SaveNumBs.i = SaveNum.i ;
		}

        printf("func=%s:line=%d:TransactionNum.i = %d:SaveNumBc.i=%d\n",
            __func__,__LINE__,TransactionNum.i,SaveNum.i);

		//SeDriverCard
		memset (FileBuf,0,sizeof(FileBuf));
		result = lseek(canshu,448, SEEK_SET);
		result = read(canshu,FileBuf,8); //保存司机编号
		if(mystrncmp(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)
		{
			memcpy(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8);
			des_encrypt(&ctx, FileBuf, FileBuf);
			result = lseek(canshu,448, SEEK_SET);
			result = write(canshu,FileBuf,8);

			result = lseek(canshu,448, SEEK_SET);
			memset (FileBuf,0,sizeof(FileBuf));
			result = read(canshu,FileBuf,8);
			des_decrypt(&ctx, FileBuf, FileBuf);
			//memcpy(Driver.intbuf,FileBuf,2);
			memcpy(SeDriverCard.longbuf, FileBuf, 4);
			memcpy(WatcherCard.longbuf, FileBuf+4, 4);
		}
		else
		{
			des_decrypt(&ctx, FileBuf, FileBuf);
			//memcpy(Driver.intbuf,FileBuf,2);
			memcpy(SeDriverCard.longbuf, FileBuf, 4);
			memcpy(WatcherCard.longbuf, FileBuf+4, 4);
			
		}

		memset(fileBuffer,0,sizeof(fileBuffer));
		result = lseek(canshu,464, SEEK_SET);
		result = read(canshu,fileBuffer,16); //分段信息
		if(mystrncmp(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00\0x00",9) == 0)
		{
			memset(fileBuffer,0,sizeof(fileBuffer));
			for(i=0; i<2; i++)
			{
				des_encrypt(&ctx, fileBuffer+i*8, fileBuffer+i*8);
			}
			result = lseek(canshu,464, SEEK_SET);
			result = write(canshu,fileBuffer,16);

			result = lseek(canshu,464, SEEK_SET);
			memset (fileBuffer,0,sizeof(fileBuffer));
			result = read(canshu,fileBuffer,16);

			for(i=0; i<2; i++)
			{
				des_decrypt(&ctx, fileBuffer+i*8, fileBuffer+i*8);
			}
			memcpy(&Section.SationNum,fileBuffer,10);		//连考，不仅仅是sationnum被赋值


		}
		else
		{
			for(i=0; i<2; i++)
			{
				des_decrypt(&ctx, fileBuffer+i*8, fileBuffer+i*8);
			}
			memcpy(&Section.SationNum,fileBuffer,10);
		}

		memset (FileBuf,0,sizeof(FileBuf));
		result = lseek(canshu,480, SEEK_SET);
		result = read(canshu,FileBuf,8); //读卡优先标志
		if(mystrncmp(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)
		{
			memcpy(FileBuf,"\xAA\x00\x00\x00\x00\x00\x00\x00",8);
			des_encrypt(&ctx, FileBuf, FileBuf);
			result = lseek(canshu,480, SEEK_SET);
			result = write(canshu,FileBuf,8);

			result = lseek(canshu,480, SEEK_SET);
			memset (FileBuf,0,sizeof(FileBuf));
			result = read(canshu,FileBuf,8);
			des_decrypt(&ctx, FileBuf, FileBuf);
			ReadCardFirst = FileBuf[0];
		}
		else
		{
			des_decrypt(&ctx, FileBuf, FileBuf);
			ReadCardFirst = FileBuf[0];
		}

		memset(fileBuffer,0,sizeof(fileBuffer));
		result = lseek(canshu,496, SEEK_SET);
		result = read(canshu,fileBuffer,16); //分段站台个数，预扣金额 ，超时时间
		if(mystrncmp(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00\0x00",9) == 0)
		{
			memset(fileBuffer,0,sizeof(fileBuffer));
			for(i=0; i<2; i++)
			{
				des_encrypt(&ctx, fileBuffer+i*8, fileBuffer+i*8);
			}
			result = lseek(canshu,496, SEEK_SET);
			result = write(canshu,fileBuffer,16);

			result = lseek(canshu,496, SEEK_SET);
			memset (fileBuffer,0,sizeof(fileBuffer));
			result = read(canshu,fileBuffer,16);

			for(i=0; i<2; i++)
			{
				des_decrypt(&ctx, fileBuffer+i*8, fileBuffer+i*8);
			}
			memcpy(&Sectionup.SationNum,fileBuffer,6);
		}
		else
		{
			for(i=0; i<2; i++)
			{
				des_decrypt(&ctx, fileBuffer+i*8, fileBuffer+i*8);
			}
			memcpy(&Sectionup.SationNum,fileBuffer,6);
		}


		memset (FileBuf,0,sizeof(FileBuf));
		result = lseek(canshu,512, SEEK_SET);
		result = read(canshu,FileBuf,8); //分段上下行标志
		if(mystrncmp(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)
		{
			memcpy(FileBuf,"\x00\x01\x00\x00\x00\x00\x00\x00",8);
			des_encrypt(&ctx, FileBuf, FileBuf);
			result = lseek(canshu,512, SEEK_SET);
			result = write(canshu,FileBuf,8);

			result = lseek(canshu,512, SEEK_SET);
			memset (FileBuf,0,sizeof(FileBuf));
			result = read(canshu,FileBuf,8);
			des_decrypt(&ctx, FileBuf, FileBuf);
			memcpy(&Section.Updown,FileBuf,4);
			SectionNum = Section.SationNum[0];
		}
		else
		{
			des_decrypt(&ctx, FileBuf, FileBuf);
			memcpy(&Section.Updown,FileBuf,4);
			if(Section.Updown  == 0)
			{
				SectionNum = Section.SationNum[0];
			}
			else
			{
				SectionNum = Sectionup.SationNum[0];
			}
		}

		memset (FileBuf,0,sizeof(FileBuf));
		result = lseek(canshu,520, SEEK_SET);
		result = read(canshu,FileBuf,8);	//0x55允许刷银行卡,0xAA禁止刷银行卡
		if(mystrncmp(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)
		{
			memcpy(FileBuf,"\xAA\x00\x00\x00\x00\x00\x00\x00",8);
			des_encrypt(&ctx, FileBuf, FileBuf);
			result = lseek(canshu,520, SEEK_SET);
			result = write(canshu,FileBuf,8);

			result = lseek(canshu,520, SEEK_SET);
			memset (FileBuf,0,sizeof(FileBuf));
			result = read(canshu,FileBuf,8);
			des_decrypt(&ctx, FileBuf, FileBuf);
			BankCardSwitch = FileBuf[0];
		}
		else
		{
			des_decrypt(&ctx, FileBuf, FileBuf);
			BankCardSwitch = FileBuf[0];
		}
		
		memset (FileBuf,0,sizeof(FileBuf));
		result = lseek(canshu,528, SEEK_SET);
		result = read(canshu,FileBuf,8);	//读取额定人数
		if(mystrncmp(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)
		{
			memcpy(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8);
			des_encrypt(&ctx, FileBuf, FileBuf);
			result = lseek(canshu,528, SEEK_SET);
			result = write(canshu,FileBuf,8);

			result = lseek(canshu,528, SEEK_SET);
			memset (FileBuf,0,sizeof(FileBuf));
			result = read(canshu,FileBuf,8);
			des_decrypt(&ctx, FileBuf, FileBuf);
			//g_MaxPersonNumber = FileBuf[0];
			memcpy((unsigned char *)&g_MaxPersonNumber, FileBuf, 4);
		}
		else
		{
			des_decrypt(&ctx, FileBuf, FileBuf);
			//g_MaxPersonNumber = FileBuf[0];
			memcpy((unsigned char *)&g_MaxPersonNumber, FileBuf, 4);
		}

		memset (FileBuf,0,sizeof(FileBuf));
		result = lseek(canshu,536, SEEK_SET);
		result = read(canshu,FileBuf,8);	//0x55不需要过cardhandle 匹配,0xAA/0xcc需要经过cardhandle匹配
		if(mystrncmp(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)
		{
			memcpy(FileBuf,"\xAA\x00\x00\x00\x00\x00\x00\x00",8);
			des_encrypt(&ctx, FileBuf, FileBuf);
			result = lseek(canshu,536, SEEK_SET);
			result = write(canshu,FileBuf,8);

			result = lseek(canshu,536, SEEK_SET);
			memset (FileBuf,0,sizeof(FileBuf));
			result = read(canshu,FileBuf,8);
			des_decrypt(&ctx, FileBuf, FileBuf);
			g_FgCardHandleOrNot = FileBuf[0];
		}
		else
		{
			des_decrypt(&ctx, FileBuf, FileBuf);
			g_FgCardHandleOrNot = FileBuf[0];
		}

		memset(FileBuf,0,sizeof(FileBuf));			
		result = lseek(canshu,560, SEEK_SET);
		result = read(canshu,FileBuf,8);  //分段线路号
		if(mystrncmp(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)
		{
			memcpy(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8);
			des_encrypt(&ctx, FileBuf, FileBuf);
			result = lseek(canshu,560, SEEK_SET);
			result = write(canshu,FileBuf,8);

			result = lseek(canshu,560, SEEK_SET);
			memset (FileBuf,0,sizeof(FileBuf));
			result = read(canshu,FileBuf,8);
			des_decrypt(&ctx, FileBuf, FileBuf);
			Section.Linenum[0]=FileBuf[0];
			Section.Linenum[1]=FileBuf[1];
			
		}
		else{
			des_decrypt(&ctx, FileBuf, FileBuf);
			Section.Linenum[0]=FileBuf[0];
			Section.Linenum[1]=FileBuf[1];

		}


			/*读取按键设定的固定消费值*/
		memset(FileBuf,0,sizeof(FileBuf));			
		result = lseek(canshu,568, SEEK_SET);
		result = read(canshu,FileBuf,8);
		
		if(mystrncmp(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)//若没有该参数记录
		{
					memcpy(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8);
					des_encrypt(&ctx, FileBuf, FileBuf);
					result = lseek(canshu,568, SEEK_SET);
					result = write(canshu,FileBuf,8);

					result = lseek(canshu,568, SEEK_SET);
					memset (FileBuf,0,sizeof(FileBuf));
					result = read(canshu,FileBuf,8);
					des_decrypt(&ctx, FileBuf, FileBuf);
					memcpy(Fixvalue.longbuf,FileBuf,4);
		}
		else{
					des_decrypt(&ctx, FileBuf, FileBuf);
					memcpy(Fixvalue.longbuf,FileBuf,4);
		}
//strcpy(TcpIpBuf, "0.0.0.0");
#if Initprintf
		printf("DevNum = %d\n",DevNum.i);
		printf("LanSec= %d,%d,%d,%d,%d,%d,%d\n",\
		       LanSec.One,LanSec.Two,LanSec.Thr,LanSec.For,LanSec.Fiv,LanSec.Six,LanSec.Sev);  //LanSec.Zer,
		printf("Keydes= %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\n",\
		       KeyDes[0],KeyDes[1],KeyDes[2],KeyDes[3],KeyDes[4],KeyDes[5],KeyDes[6],KeyDes[7]);  //LanSec.Zer,
		printf("LanSec ADFNUM= %02X%02X\n",LanSec.ADFNUM[0],LanSec.ADFNUM[1]);  //LanSec.Zer,
		memcpy(Infor.intbuf,TcpIpBuf+32,2);
		printf("TcpIpBuf = %s,%d \n",TcpIpBuf,Infor.i);
		printf("SelfAddress = %s \n",SelfAddress);
		printf("DevSID = %04d \n",DevSID.i);
		printf("CodeNum.i = %05d \n",CodeNum.i);
		printf("SaveNum.i = %05d \n",SaveNum.i);
		printf("TransactionNum.i = %05d \n",TransactionNum.i);
		printf("OPENBEEP = %05d \n",OPENBEEP);
		printf("Driver = %05d \n",Driver.i);
		printf("Section down = %d :Section.DeductMoney :%02x%02x%02x%02x \n",Section.SationNum[0],Section.DeductMoney[0],Section.DeductMoney[1],Section.DeductMoney[2],Section.DeductMoney[3]);
		printf("Section up = %d : Section.DeductMoney :%02x%02x%02x%02x\n",Sectionup.SationNum[0],Section.DeductMoney[0],Section.DeductMoney[1],Section.DeductMoney[2],Section.DeductMoney[3]);
		printf("Section Enable = %02X \n",Section.Enable);
		printf("ReadCardFirst = %02X \n",ReadCardFirst);
		printf("BankCardSwitch = %02X\n",BankCardSwitch);
		printf("g_MaxPerson = %u\n",g_MaxPersonNumber);
		printf("g_FgCardHandleOrNot = 0x%x\n", g_FgCardHandleOrNot);
#endif

		close(canshu);
		break;

	case SELFIP:
		canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR);
		memcpy(filebuf,SelfAddress,32);
		for(i = 0; i < 4; i ++)
		{
			des_encrypt(&ctx, filebuf+i*8, filebuf+i*8);
		}
		result = lseek(canshu,384, SEEK_SET);
		result = write(canshu,filebuf,40);

		result = lseek(canshu, 384, SEEK_SET);
		memset (filebuf,0,sizeof(filebuf));
		result = read(canshu,filebuf,40);
		for(i = 0; i < 4; i ++)
		{
			des_decrypt(&ctx, filebuf+i*8, filebuf+i*8);
		}
		memcpy(SelfAddress,filebuf,32);
		close(canshu);
		break;

	case MUSERKEY://更改密钥
		canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR);
		memcpy(FileBuf,KeyDes,8);
		des_encrypt(&ctx, FileBuf, FileBuf);
		result = lseek(canshu,0, SEEK_SET);
		result = write(canshu,FileBuf,8);
		result = lseek(canshu, 0, SEEK_SET);
		memset (FileBuf,0,sizeof(FileBuf));
		result = read(canshu,FileBuf,8);
		des_decrypt( &ctx, FileBuf, FileBuf);
		memcpy(KeyDes,FileBuf,8);
		close(canshu);
		break;

	case MTEMNO:
		canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR);
		memset(FileBuf,0,sizeof(FileBuf));
		memcpy(FileBuf,DevNum.longbuf,4);
		des_encrypt(&ctx, FileBuf, FileBuf);
		result = lseek(canshu,16, SEEK_SET);
		result = write(canshu,FileBuf,8);

		result = lseek(canshu, 16, SEEK_SET);
		memset (FileBuf,0,sizeof(FileBuf));
		result = read(canshu,FileBuf,8);
		des_decrypt( &ctx, FileBuf, FileBuf);
		memcpy(DevNum.longbuf,FileBuf,4);
		memcpy(PsamNum+2,FileBuf,4);
		close(canshu);

#if Initprintf
		printf("MTEMNO  DevNum==%d \n",DevNum.i);
#endif

		break;

	case MUESRPWD:	//修改管理员密码
		canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR);
		memcpy(FileBuf,AdminPwd,8);
		des_encrypt(&ctx, FileBuf, FileBuf);
		result = lseek(canshu,32, SEEK_SET);
		result = write(canshu,FileBuf,8);

		result = lseek(canshu, 32, SEEK_SET);
		memset (FileBuf,0,sizeof(FileBuf));
		result = read(canshu,FileBuf,8);
		des_decrypt( &ctx, FileBuf, FileBuf);
		memcpy(AdminPwd,FileBuf,8);
		close(canshu);
		break;

	case MUSERSETOR:
		canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR);
		memcpy(FileBuf,&LanSec,8);
		des_encrypt(&ctx, FileBuf, FileBuf);
		result = lseek(canshu,64, SEEK_SET);
		result = write(canshu,FileBuf,8);

		memset(FileBuf,0,sizeof(FileBuf));
		memcpy(FileBuf,LanSec.ADFNUM,2);
		des_encrypt(&ctx, FileBuf, FileBuf);
		result = lseek(canshu,128, SEEK_SET);
		result = write(canshu,FileBuf,8);
		close(canshu);
		break;

	case MSEVERIP:
		canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR);
		memcpy(filebuf,TcpIpBuf,40);
		for(i = 0; i < 5; i ++)
		{
			des_encrypt(&ctx, filebuf+i*8, filebuf+i*8);
		}
		result = lseek(canshu,80, SEEK_SET);
		result = write(canshu,filebuf,40);

		result = lseek(canshu, 80, SEEK_SET);
		memset (filebuf,0,sizeof(filebuf));
		result = read(canshu,filebuf,40);
		for(i = 0; i < 5; i ++)
		{
			des_decrypt(&ctx, filebuf+i*8, filebuf+i*8);
		}
		memcpy(TcpIpBuf,filebuf,40);
		close(canshu);
		break;

	case OPERADORED:
		canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR);
		FileBuf[0] = OperCount;
		des_encrypt(&ctx, FileBuf, FileBuf);
		result = lseek(canshu,192, SEEK_SET);
		result = write(canshu,FileBuf,8);

		result = lseek(canshu, 192, SEEK_SET);
		memset (FileBuf,0,sizeof(FileBuf));
		result = read(canshu,FileBuf,8);
		des_decrypt( &ctx, FileBuf, FileBuf);
		OperCount = FileBuf[0];
		close(canshu);
		break;

	case SLEEPOVER:
		canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR);
		memset(FileBuf,0,sizeof(FileBuf));
		FileBuf[0] = SleepOverTime;
		des_encrypt(&ctx, FileBuf, FileBuf);
		result = lseek(canshu,48, SEEK_SET);
		result = write(canshu,FileBuf,8);

		result = lseek(canshu, 48, SEEK_SET);
		memset (FileBuf,0,sizeof(FileBuf));
		result = read(canshu,FileBuf,8);
		des_decrypt( &ctx, FileBuf, FileBuf);
		SleepOverTime = FileBuf[0];
		close(canshu);
		break;

	case DEVSERIALID:		
		canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR);
		memset(FileBuf,0,sizeof(FileBuf));
		memcpy(FileBuf,DevSID.longbuf,4);
		des_encrypt(&ctx, FileBuf, FileBuf);
		result = lseek(canshu,144, SEEK_SET);
		result = write(canshu,FileBuf,8);

		result = lseek(canshu, 144, SEEK_SET);
		memset (FileBuf,0,sizeof(FileBuf));
		result = read(canshu,FileBuf,8);
		des_decrypt( &ctx, FileBuf, FileBuf);
		memcpy(DevSID.longbuf,FileBuf,4);		
		close(canshu);
		break;

	case CODEFILE:   //保存记录数		
		for(j = 0; j < 6; j++)
		{
			canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR);
			memset(FileBuf,0,sizeof(FileBuf));
			memcpy(FileBuf,CodeNum.longbuf,4);
			des_encrypt(&ctx, FileBuf, FileBuf);
			result = lseek(canshu,160, SEEK_SET);
			result = write(canshu,FileBuf,8);

			result = lseek(canshu, 160, SEEK_SET);
			memset (FileBuf,0,sizeof(FileBuf));
			result = read(canshu,FileBuf,8);
			des_decrypt( &ctx, FileBuf, FileBuf);
			close(canshu);
			//memcpy(CodeNum.longbuf,FileBuf,4);				
			memcpy(tmp.longbuf,FileBuf,4);				
			if(tmp.i == CodeNum.i)
				break;
		}
#if Initprintf
		printf("CODEFILE CodeNum==%d \n",CodeNum.i);
#endif
		break;

	case RCODEFILE:   //读记录数
		canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR);
		memset(FileBuf,0,sizeof(FileBuf));
		result = lseek(canshu, 160, SEEK_SET);
		memset (FileBuf,0,sizeof(FileBuf));
		result = read(canshu,FileBuf,8);
		des_decrypt( &ctx, FileBuf, FileBuf);
		memcpy(CodeNum.longbuf,FileBuf,4);
		close(canshu);

#if Initprintf
		printf("RCODEFILE CodeNum==%d \n",CodeNum.i);
#endif
		break;

	case OPENBUTTON:
		canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR);
		memset(FileBuf,0,sizeof(FileBuf));
		FileBuf[0] = OPENBEEP;
		des_encrypt(&ctx, FileBuf, FileBuf);
		result = lseek(canshu,176, SEEK_SET);
		result = write(canshu,FileBuf,8);

		result = lseek(canshu,176, SEEK_SET);
		memset (FileBuf,0,sizeof(FileBuf));
		result = read(canshu,FileBuf,8);
		des_decrypt( &ctx, FileBuf, FileBuf);
		OPENBEEP = FileBuf[0];
		close(canshu);
		break;

	case  OPERBUFFER:
		canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR);
		memcpy(filebuf,OperBuffor,80);
		for(i = 0; i < 10; i ++)
		{
			des_encrypt(&ctx, filebuf+i*8, filebuf+i*8);
		}
		result = lseek(canshu,1024, SEEK_SET);
		result = write(canshu,filebuf,80);

		result = lseek(canshu, 1024, SEEK_SET);
		memset (filebuf,0,sizeof(filebuf));
		result = read(canshu,filebuf,80);
		for(i = 0; i < 10; i ++)
		{
			des_decrypt(&ctx, filebuf+i*8, filebuf+i*8);
		}
		memcpy(OperBuffor,filebuf,80);
		close(canshu);
		break;

	case  WSDATA:		
		for(j = 0; j < 6; j++)
		{
			canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR);
			memset(FileBuf,0,sizeof(FileBuf));
			memcpy(FileBuf,SaveNum.longbuf,4);
			memcpy(FileBuf + 4,TransactionNum.longbuf,4);
			des_encrypt(&ctx, FileBuf, FileBuf);
			result = lseek(canshu,432, SEEK_SET);
			result = write(canshu,FileBuf,8);
			result = lseek(canshu,432, SEEK_SET);
			memset (FileBuf,0,sizeof(FileBuf));
			result = read(canshu,FileBuf,8);
			des_decrypt( &ctx, FileBuf, FileBuf);
			
			//memcpy(SaveNum.longbuf,FileBuf,4);
			//memcpy(TransactionNum.longbuf,FileBuf + 4,4);
			close(canshu);
			memcpy(tmp.longbuf,FileBuf,4);		
			memcpy(tmp1.longbuf,FileBuf + 4,4);

			if((tmp.i == SaveNum.i) && (tmp1.i == TransactionNum.i))
			{				
				SaveNumBc.i = SaveNumBs.i = SaveNum.i;
				break;
			}
			
			//fclose(canshu);					
		}
#if Initprintf
		printf("WSDATA SaveNum=%d, TransactionNum=%d\n", SaveNum.i, TransactionNum.i);
#endif
		break;

	case  RSDATA:
		canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR);
		memset(FileBuf,0,sizeof(FileBuf));
		result = lseek(canshu, 432, SEEK_SET);
		memset (FileBuf,0,sizeof(FileBuf));
		result = read(canshu,FileBuf,8);
		des_decrypt( &ctx, FileBuf, FileBuf);
		memcpy(SaveNum.longbuf,FileBuf,4);
		memcpy(TransactionNum.longbuf,FileBuf + 4,4);
		SaveNumBc.i = SaveNumBs.i = SaveNum.i ;
		close(canshu);

#if Initprintf
		printf("RSDATA SaveNum=%d, TransactionNum=%d\n", SaveNum.i, TransactionNum.i);
#endif
		break;  //Driver



	case  DRIVER:
		canshu = open(SYS_PARAM_FILE,O_RDWR|O_SYNC);
		memset(FileBuf,0,sizeof(FileBuf));
		//memcpy(FileBuf,Driver.intbuf,2);
		memcpy(FileBuf, SeDriverCard.longbuf, 4);
		memcpy(FileBuf+4, WatcherCard.longbuf, 4);
		des_encrypt(&ctx, FileBuf, FileBuf);
		result = lseek(canshu,448, SEEK_SET);
		result = write(canshu,FileBuf,8);

		memset (FileBuf,0,sizeof(FileBuf));
		result = lseek(canshu,448, SEEK_SET);
		result = read(canshu,FileBuf,8);
		des_decrypt( &ctx, FileBuf, FileBuf);
		memcpy(SeDriverCard.longbuf, FileBuf, 4);
		memcpy(WatcherCard.longbuf, FileBuf+4, 4);
		//memcpy(Driver.intbuf,FileBuf,2);
		close(canshu);

#if Initprintf
		printf("DRIVER Driver ==%d \n",Driver.i);
#endif
		break;

//

	case  SETSECTION:
		canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR);
		memset(fileBuffer,0,sizeof(fileBuffer));
		memcpy(fileBuffer,&Section.SationNum,10);

		for(i=0; i<2; i++)
		{
			des_encrypt(&ctx, fileBuffer+i*8, fileBuffer+i*8);
		}

		result = lseek(canshu,464, SEEK_SET);
		result = write(canshu,fileBuffer,16);

		result = lseek(canshu,464, SEEK_SET);

		memset (fileBuffer,0,sizeof(fileBuffer));
		result = read(canshu,fileBuffer,16);
		for(i=0; i<2; i++)
		{
			des_decrypt(&ctx, fileBuffer+i*8, fileBuffer+i*8);
		}
		memcpy(&Section.SationNum,fileBuffer,10);

#if Initprintf
		printf(" set Section %d  Ena=%02X  \n",Section.SationNum[0],Section.Enable);
#endif
		close(canshu);
		break;

	case SETCARDFIRST:

		canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR);
		printf("gc canshu is %d \n",canshu);
		memset(FileBuf,0,sizeof(FileBuf));
		FileBuf[0] = ReadCardFirst;
		des_encrypt(&ctx, FileBuf, FileBuf);
		result = lseek(canshu,480, SEEK_SET);
		result = write(canshu,FileBuf,8);

		memset (FileBuf,0,sizeof(FileBuf));
		result = lseek(canshu,480, SEEK_SET);
		result = read(canshu,FileBuf,8);
		des_decrypt( &ctx, FileBuf, FileBuf);
		ReadCardFirst = FileBuf[0];
		close(canshu);

#if Initprintf
		printf("SETCARDFIRST  ==%02X \n",ReadCardFirst);
#endif

		break;



	case  SETSECTIONUP:
		canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR);
		memset(fileBuffer,0,sizeof(fileBuffer));
		memcpy(fileBuffer,&Sectionup.SationNum,6);
		des_encrypt(&ctx, fileBuffer, fileBuffer);
		result = lseek(canshu,496, SEEK_SET);
		result = write(canshu,fileBuffer,8);
		result = lseek(canshu,496, SEEK_SET);
		memset (fileBuffer,0,sizeof(fileBuffer));
		result = read(canshu,fileBuffer,8);
		des_decrypt(&ctx, fileBuffer, fileBuffer);
		memcpy(&Sectionup.SationNum,fileBuffer,6);
		close(canshu);
#if Initprintf
		printf(" set SectionUP %d  \n",Sectionup.SationNum[0]);
#endif
		break;


	case  SETSECTINO_:
		canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR);
		memset(fileBuffer,0,sizeof(fileBuffer));
		memcpy(fileBuffer,&Section.Updown,4);
		des_encrypt(&ctx, fileBuffer, fileBuffer);
		result = lseek(canshu,512, SEEK_SET);
		result = write(canshu,fileBuffer,8);
		close(canshu);
#if Initprintf
		printf("Set SETSECTINO_ Updown %d  \n",Section.Updown);
#endif
		break;


	case SETBANKCARDSWITCH:
		canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR);
		memset(fileBuffer,0,sizeof(fileBuffer));
		fileBuffer[0]=BankCardSwitch;
		des_encrypt(&ctx, fileBuffer, fileBuffer);
		result = lseek(canshu,520, SEEK_SET);
		result = write(canshu,fileBuffer,8);
		close(canshu);
#if Initprintf
		printf("Set BankCardSwitch %d  \n",BankCardSwitch);
#endif
		break;

	case WRITECARDHANDLE:
		canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR);
		memset(fileBuffer,0,sizeof(fileBuffer));
		fileBuffer[0]=g_FgCardHandleOrNot;
		des_encrypt(&ctx, fileBuffer, fileBuffer);
		result = lseek(canshu,536, SEEK_SET);
		result = write(canshu,fileBuffer,8);
		close(canshu);
#if Initprintf
		printf("g_FgCardHandleOrNot %d  \n",g_FgCardHandleOrNot);
#endif
		break;

	case PERSONWRITE:
		canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR);
		memset(fileBuffer,0,sizeof(fileBuffer));
		//fileBuffer[0]=g_FgCardHandleOrNot;
		memcpy(fileBuffer, (unsigned char *)&g_MaxPersonNumber, 4);
		des_encrypt(&ctx, fileBuffer, fileBuffer);
		result = lseek(canshu,528, SEEK_SET);
		result = write(canshu,fileBuffer,8);
		close(canshu);
#if Initprintf
		printf("g_MaxPersonNumber %d  \n",g_MaxPersonNumber);
#endif
		break;
	case  SETSECTIONLINE:
		canshu=fopen(SYS_PARAM_FILE,"rb+");
		memset(FileBuf,0,sizeof(FileBuf));
		FileBuf[0]=Section.Linenum[0];
		FileBuf[1]=Section.Linenum[1];
		des_encrypt(&ctx,FileBuf,FileBuf);
		result=fseek(canshu,560,SEEK_SET);
		result=fwrite(FileBuf,sizeof(unsigned char),8,canshu);
		result=fseek(canshu,560,SEEK_SET);
		result=fread(FileBuf,sizeof(unsigned char),8,canshu);
		des_decrypt(&ctx,FileBuf,FileBuf);
		Section.Linenum[0]=FileBuf[0];
		Section.Linenum[1]=FileBuf[1];
		fclose_nosync(canshu);
		break;

	case WRFIXVALUE:
		canshu = fopen(SYS_PARAM_FILE,"rb+");
		memset(fileBuffer,0,sizeof(fileBuffer));
		memcpy(fileBuffer,Fixvalue.longbuf,4);
		des_encrypt(&ctx, fileBuffer, fileBuffer);
		result = fseek(canshu,568, SEEK_SET);
		result = fwrite(fileBuffer,sizeof(unsigned char),8,canshu);
		fclose_nosync(canshu);
		break;

	default :
		break;
	}
	
	//system("sync");
}



/*
*************************************************************************************************************
- 函数名称 : unsigned char Number(unsigned int CardNumber,unsigned char cmd)
- 函数说明 : 黑名单读写
- 输入参数 :
- 输出参数 : 无
*************************************************************************************************************
*/
unsigned char Number(unsigned int CardNumber,unsigned char cmd)
{
	int result,status;
	unsigned int CardByte,CardBit;
	unsigned char NumBuf,i;
	unsigned char FileBuf[8];
	
	memset(FileBuf,0,sizeof(FileBuf));
	CardByte = CardNumber/8;
	CardBit = CardNumber%8;
    #ifdef SAVE_CONSUM_DATA_DIRECT
	BlackFile = fopen("/mnt/record/Blacklist.sys","rb+");
    #else
    BlackFile = fopen("/var/run/Blacklist.sys","rb+");
    #endif
	result = fseek(BlackFile,CardByte, SEEK_SET);
	result = fread(FileBuf,sizeof(unsigned char), 2, BlackFile);
	switch(cmd)
	{
	case 0:										//下载
		NumBuf = 0x01;
		NumBuf = NumBuf << CardBit;
		FileBuf[0] = FileBuf[0] | NumBuf;
		result = fseek(BlackFile,CardByte, SEEK_SET);
		result = fwrite(FileBuf,sizeof(unsigned char),2,BlackFile);
		status = 0;
		break;
	case 1:
		NumBuf = 0x01;
		NumBuf = NumBuf << CardBit;
		if((NumBuf & FileBuf[0]))
		{
			NumBuf = 0xFE;
			for(i = 0; i < CardBit; i++) NumBuf = (NumBuf << 1)|0x01;
			FileBuf[0] = FileBuf[0] & NumBuf;
			result = fseek(BlackFile,CardByte, SEEK_SET);
			result = fwrite(FileBuf,sizeof(unsigned char),2,BlackFile);
		}				//解挂
		status = 0;
		break;
	case 2:
		status = FileBuf[0] >> CardBit;
		status = status&0x01;
		break;
	}
	fclose(BlackFile);
	return(status);
}
/*
*************************************************************************************************************
- 函数名称 : void Card_SysInit(void)
- 函数说明 : 数据初始化
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
//00 - 08   读取密钥
//16 - 24   读取终端机
//32 - 40   管理员密码
//272 - 312 IP地址
//512 - 514 读取用户扇区
//528 - 528 更改进车流程
//544 - 560 服务器等待时间
//560 - 576 更改待机时间
//596 - 612 更改打印机状态
//613 - 628 进车交易流水号
//629 - 644 出车交易流水号
//1024 + 8  采集记录指针
//1040      按鍵声打开
void Card_SysInit(void)
{
	//while (FileOpenFlag == 0);
	//    FileOpenFlag = 0;
	ReadOrWriteFile (SYSFILE);
	//    FileOpenFlag = 1;
}
/*
*************************************************************************************************************
- 函数名称 : unsigned char Para_cardlan(unsigned char *buf,unsigned int Addr,unsigned char Long,unsigned char Mode)
- 函数说明 : 创建 读写 参数表
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
unsigned char Para_cardlan(unsigned char *buf,unsigned int Addr,unsigned char Long,unsigned char Mode)
{
	int result;

	if(Mode>1)
	{
        #ifdef SAVE_CONSUM_DATA_DIRECT
		if(access("/mnt/record/section.bin",0)!= 0)
		{
			ParaFileBuf = fopen("/mnt/record/section.bin","a+");       
			if(ParaFileBuf < 0)
			{
				return -1;
			}
			fclose(ParaFileBuf);
			usleep(1000);
		}
        #else
		if(access("/var/run/section.bin",0)!= 0)
		{
			ParaFileBuf = fopen("/var/run/section.bin","a+");       
			if(ParaFileBuf < 0)
			{
				return -1;
			}
			fclose(ParaFileBuf);
			usleep(1000);
		}

        #endif
	}

	switch(Mode)
	{
	case 0:
        #ifdef SAVE_CONSUM_DATA_DIRECT
		ParaFileBuf = fopen("/mnt/record/cardlan.bin","rb+");
        #else
        ParaFileBuf = fopen("/var/run/cardlan.bin","rb+");
        #endif
		result = fseek(ParaFileBuf, Addr, SEEK_SET);
		result = fwrite(buf,sizeof(unsigned char),Long,ParaFileBuf);
		fclose(ParaFileBuf);
		break;

	case 1:
        #ifdef SAVE_CONSUM_DATA_DIRECT
		ParaFileBuf = fopen("/mnt/record/cardlan.bin","rb+");
        #else
        ParaFileBuf = fopen("/var/run/cardlan.bin","rb+");
        #endif
		result = fseek(ParaFileBuf, Addr, SEEK_SET);
		result = fread(buf,sizeof(unsigned char),Long,ParaFileBuf);
		fclose(ParaFileBuf);
		break;

	case 2:
        #ifdef SAVE_CONSUM_DATA_DIRECT
		ParaFileBuf = fopen("/mnt/record/section.bin","rb+");
        #else
        ParaFileBuf = fopen("/var/run/section.bin","rb+");
        #endif
		result = fseek(ParaFileBuf, Addr, SEEK_SET);
		result = fwrite(buf,sizeof(unsigned char),Long,ParaFileBuf);
		fclose(ParaFileBuf);
		break;

	case 3:
        #ifdef SAVE_CONSUM_DATA_DIRECT
		ParaFileBuf = fopen("/mnt/record/section.bin","rb+");
        #else
        ParaFileBuf = fopen("/var/run/section.bin","rb+");
        #endif
		result = fseek(ParaFileBuf, Addr, SEEK_SET);
		result = fread(buf,sizeof(unsigned char),Long,ParaFileBuf);
		fclose(ParaFileBuf);
		break;

	default :
		break;
	}
	return 0;
}
/*
*************************************************************************************************************
- 函数名称 : void CardLanFile (unsigned char RW_Type)
- 函数说明 : 读取参数表
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void CardLanFile (unsigned char RW_Type)
{
	FILE *ParaFile;
	int result;
	unsigned char buffer[512];
	unsigned char i;
	unsigned int mkLengthUp, mkLengthDown;

	unsigned char i,j;
	int max,maxci,ci;
    ShortUnon tmp;

	printf("---->you are in cardlanfile------>\n");
	switch(RW_Type)
	{
	case SYSFILE: //系统参数
	#if 0
		free(CardLanBuf);
		CardLanBuf = NULL;
		CardLanBuf = (unsigned char *)malloc(16384*sizeof(unsigned char));		
	
		if(CardLanBuf != NULL)
		{
			memset (CardLanBuf,0,sizeof(CardLanBuf));
			ParaFile = fopen(PARM_FILE_PATH,"rb+");
			for(i = 0; i<32; i++)
			{
				memset(buffer,0,sizeof(buffer));
				result = fseek(ParaFile, i*512, SEEK_SET);
				result = fread(buffer,sizeof(unsigned char),512,ParaFile);
				memcpy(CardLanBuf+i*512,buffer,512);
			}
			fclose(ParaFile);
		}		
		#else
		ParaFile = fopen(PARM_FILE_PATH,"rb+");
		if (NULL != ParaFile)
		{
			fseek(ParaFile, 512, SEEK_SET);
			fread(LocalCardRate, sizeof(LocalCardRate[0]), CARD_NUMBER, ParaFile);
			fclose(ParaFile);
		}
		#endif
		break;

	case SectionPar://上行分段
		free(SectionParBuf);
		SectionParBuf = NULL;
		SectionParBuf = (unsigned char *)malloc(16384*sizeof(unsigned char));
		if(SectionParBuf != NULL)
		{

            tmp.i = 0;
			memcpy(tmp.intbuf,filem4.uprecordnun,2);
			ParaFile = fopen("/mnt/record/M4","rb+");
            max = filem4.uprecord;                         //需要修改根据票价参数定义个数
	        maxci = tmp.i/2;
	        ci = 0;
			ParaFile = fopen("/mnt/record/M4","rb+");
	        for(i=0;i<max;i++) {
	            for(j=0;j<=i;j++){
	                result = fseek(ParaFile, 6, SEEK_SET);
	                result = fread(SectionParBuf+(i*max+j)*2,sizeof(unsigned char),2,ParaFile);
	                ci++;
	                if(ci>=maxci)
	                    break;
	            }
	         }  
			

			fclose(ParaFile);
		}
		break;

	case SectionParup://下行分段
		free(SectionParUpBuf);
		SectionParUpBuf = NULL;
		SectionParUpBuf = (unsigned char *)malloc(16384*sizeof(unsigned char));
		if(SectionParUpBuf != NULL)
		{
            #ifdef SAVE_CONSUM_DATA_DIRECT
			ParaFile = fopen("/mnt/record/sectionup.sys","rb+");
            #else
            ParaFile = fopen("/var/run/sectionup.sys","rb+");
            #endif
			for(i = 0; i<32; i++)
			{
				memset(buffer,0,sizeof(buffer));
				result = fseek(ParaFile, i*512, SEEK_SET);
				result = fread(buffer,sizeof(unsigned char),512,ParaFile);
				memcpy(SectionParUpBuf+i*512,buffer,512);
			}
			fclose(ParaFile);
		}
		break;

	case SationdisupParup://上行公里数
		free(StationdisupParBuf);
		StationdisupParBuf = NULL;
		//ParaFile = fopen("/mnt/record/sationdisup.sys","rb+");
		ParaFile = fopen(SECTION_KM_PATH_NAME,"rb+");
		if (NULL == ParaFile) break;
		memset(buffer,0,sizeof(buffer));
		result = fread(buffer,sizeof(unsigned char),2,ParaFile);
		mkLengthUp = buffer[0]*buffer[1];
		if (mkLengthUp == 0)
		{
			printf("no up parameter kM \n");
			break;
		}
		StationdisupParBuf = (unsigned char *)malloc(mkLengthUp);
		if(StationdisupParBuf != NULL)
		{

			//memset(buffer,0,sizeof(buffer));
			result = fseek(ParaFile, 4, SEEK_SET);
			i = 5;
			do {
				result += fread(StationdisupParBuf,sizeof(unsigned char),mkLengthUp-result,ParaFile);
				i--;
			}while(i && (result != mkLengthUp));
			//memcpy(StationdisupParBuf+i*512,buffer,512);
		}
		fclose(ParaFile);
		break;

	case SationdisdownPardown://下行公里数
		free(StationdisdownParBuf);
		StationdisdownParBuf = NULL;
		//ParaFile = fopen("/mnt/record/sationdisup.sys","rb+");
		ParaFile = fopen(SECTION_KM_PATH_NAME,"rb+");
		if (NULL == ParaFile) break;
		memset(buffer,0,sizeof(buffer));
		result = fread(buffer,sizeof(unsigned char),4,ParaFile);
		mkLengthUp = buffer[0]*buffer[1];
		mkLengthDown = buffer[2]*buffer[3];
		if (mkLengthDown == 0)
		{
			printf("no down parameter kM \n");
			break;
		}
		StationdisdownParBuf = (unsigned char *)malloc(mkLengthDown);
		if(StationdisdownParBuf != NULL)
		{

			//memset(buffer,0,sizeof(buffer));
			
			result = fseek(ParaFile, 4+mkLengthUp, SEEK_SET);
			i = 5;
			result = 0;
			do {
				result += fread(StationdisdownParBuf,sizeof(unsigned char),mkLengthDown-result,ParaFile);
				i--;
			}while(i && (result != mkLengthDown));
		}
		fclose(ParaFile);
		
		break;





	default :
		break;
	}

	FindUpdoorType();
}


void WriteCardRate_Para(unsigned char type)
{
    int result;
    
    switch(type)
	{
    //写本地卡(M1卡和住建部CPU卡)
	case 0:        
		ParaFileBuf = fopen("/mnt/record/cardlan.sys","rb+");        
		result = fseek(ParaFileBuf, 512, SEEK_SET);
        result = fwrite(CardLanBuf+512,sizeof(unsigned char),13*48,ParaFileBuf);
		fclose(ParaFileBuf);
		break;
        
    //交通部互通卡
	case 1:        
		ParaFileBuf = fopen("/mnt/record/cardlan.sys","rb+");        
		result = fseek(ParaFileBuf, 1152, SEEK_SET);
        result = fwrite(CardLanBuf+1152,sizeof(unsigned char),13*48,ParaFileBuf);
		fclose(ParaFileBuf);
		break;	

	default :
		break;
	}

    
 }


void WriteSationDis_Para(unsigned char type,unsigned char *dat,unsigned int len)
{
    int result;
    FILE *stationdisfile;
    int i,j,ci,maxci,max;
    switch(type)
	{
    //写上行和下行市内界点的站号和市内界点的公里数
       
    //写上行各站之间的公里数
	case 0: 
        max = flc0005.gupstationnum;
        maxci = len/2;
        ci = 0;
        stationdisfile = fopen("/mnt/record/stationdisup.sys","rb+");
         for(i=0;i<max;i++) {
            for(j=0;j<=i;j++){
                result = fseek(stationdisfile, (i*max+j)*2, SEEK_SET);
                result = fwrite(dat+2*ci,sizeof(unsigned char),2,stationdisfile);
                ci++;
                if(ci>=maxci)
                    break;
            }
          }
         fclose(stationdisfile);
         CardLanFile(SationdisupParup);
		break;	

      //写下行各站之间的公里数
	case 1: 
        max = flc0005.gdownstationnum;
        maxci = len/2;
        ci = 0;        
        stationdisfile = fopen("/mnt/record/stationdisdown.sys","rb+");        
		for(i=0;i<max;i++) {
            for(j=0;j<=i;j++){
                result = fseek(stationdisfile, (i*max+j)*2, SEEK_SET);
                result = fwrite(dat+2*ci,sizeof(unsigned char),2,stationdisfile);
                ci++;
                if(ci>=maxci)
                    break;
            }
          }   
		fclose(stationdisfile);	
        CardLanFile(SationdisdownPardown);
		break;	  
        

	default :
		break;
	}

    
 }

void WriteSection_Para(unsigned char type,unsigned char *dat,unsigned int len)
{
    int result;
    int i,j,ci,maxci,max;
    FILE *ParaFile;
   
    switch(type)
	{
    //写上行参数

	case 0:
			/*
	        max = flc0005.gupstationnum;
	        maxci = len/2;
	        ci = 0;
			ParaFile = fopen("/mnt/record/M4","rb+");
	        for(i=0;i<max;i++) {
	            for(j=0;j<=i;j++){
	                result = fseek(ParaFile, (i*max+j)*2, SEEK_SET);
	                result = fwrite(dat+2*ci,sizeof(unsigned char),2,ParaFile);
	                ci++;
	                if(ci>=maxci)
	                    break;
	            }
	         }      
			fclose(ParaFile);
		*/
		ParaFile = fopen("/mnt/record/M4","rb+");
		result = fseek(ParaFile, offset, SEEK_SET);
	    result = fwrite(dat,sizeof(unsigned char),len,ParaFile);

        CardLanFile(SectionPar);
		break;
        
    //写下行参数

	case 1:
		/*
        max = flc0005.gdownstationnum;
        maxci = len/2;
        ci = 0;
        ParaFile = fopen("/mnt/record/M4","rb+");        

		for(i=0;i<max;i++) {
           for(j=0;j<=i;j++){
            result = fseek(ParaFile, (i*max+j)*2, SEEK_SET);
            result = fwrite(dat+2*ci,sizeof(unsigned char),2,ParaFile);
            ci++;
            if(ci>=maxci)
                break;
        }
         }  
		fclose(ParaFile);	

		*/
		ParaFile = fopen("/mnt/record/M4","rb+");
		result = fseek(ParaFile, offset, SEEK_SET);

        CardLanFile(SectionParup);
		break;	      
        

	default :
		break;
	}

    
 }


//读写费率基本信息文件 0为写1为读
void ReadandWriteBasicRateFile(unsigned char type)
{
    int result;
    FILE *basicrate;

    switch(type)
    {
        case 0:

             basicrate = fopen("/mnt/record/MP","rb+");
		     memcpy(filemp.linenum,flc0005.glinenum,2);
			 filemp.lineattr = flc0005.glineattr;
			 memcpy(filemp.defaultbaseprice.intbuf,flc0005.gbasicpice,2);
			 filemp.modebj = flc0005.glocalnodefinecard;
			 filemp.modehl = flc0005.gremotnodefinecard;
			 filemp.modehlbp = flc0005.gyidibupiaomo;
			 filemp.onoffdir = flc0005.gruleofupanddowm;
			 
             result = fseek(basicrate, 0, SEEK_SET);
             result = fwrite(&filemp.linenum[0],sizeof(unsigned char),24,basicrate);
			 
             fclose(basicrate);
          //   Section.SationNum[0] = flc0005.gupstationnum;
          //   Sectionup.SationNum[0] = flc0005.gdownstationnum;          
          //   memcpy(Section.Linenum,flc0005.glinenum,2);             
           //  ReadOrWriteFile(SETSECTIONUP);
          //   ReadOrWriteFile(SETSECTIONLINE);
         
         	memcpy(Section.DeductTime,flc0005.gbupiaolimittime,2);
			ReadOrWriteFile(SETSECTION);
            break;

        case 1:
             basicrate = fopen("/mnt/record/MP","rb+");
			 memcpy(&filemp,0,sizeof(struct FileMP));
             result = fseek(basicrate, 0, SEEK_SET);
             result = fread(&filemp,sizeof(unsigned char),127,basicrate);
             fclose(basicrate);
			 DBG_PRINTF("费率卡二次信息文件:");
			 menu_print(filemp.linenum,127);			 
			 Section.SationNum[0] = filemp.uppricesitemnum.i;			 
			 Sectionup.SationNum[0] = filemp.downpricesitemnum.i;             
             memcpy(Section.DeductTime,flc0005.gbupiaolimittime,2);
             memcpy(Section.Linenum,filemp.linenum,2);

             break;

        default :
            break;
        }
    
    }

char FindSavedata(void)
{
	unsigned short i,t;
	int result;
	unsigned int addr,savedata;
	unsigned char buff[SAVE_DATA_LEN];
	unsigned char dbuff[16];
//  unsigned char aa;

	//  while(savedataflag);
	//     savedataflag =1;

	pthread_mutex_lock(&m_datafile);
#ifdef SAVE_CONSUM_DATA_DIRECT
	Datafile = open(OFF_LINE_CONSUM_FILE,O_RDWR|O_SYNC);
#else
	Datafile = fopen(OFF_LINE_CONSUM_FILE,"rb+");
#endif
	for(i=0; i<500; i++)
	{
		addr = i*SAVE_DATA_LEN*1000;
#ifdef SAVE_CONSUM_DATA_DIRECT
		result = lseek(Datafile,addr, SEEK_SET);
		memset(buff,0,sizeof(buff));
		result = read(Datafile,buff,SAVE_DATA_LEN-SAVE_DATA_RESERVE);
#else
		result = fseek(Datafile,addr, SEEK_SET);
		memset(buff,0,sizeof(buff));
		result = fread(buff,sizeof(unsigned char), SAVE_DATA_LEN-SAVE_DATA_RESERVE, Datafile);
#endif		
#if Initprintf
		//   printf("FindS 1==:");
		// for(aa= 0;aa< 64;aa++)
		//   {
		//     printf("%02X",buff[aa]);
		//   }
		//  printf("\n");
#endif
		for(t=0; t<8; t++)
		{
			memset(dbuff,0,sizeof(dbuff));
			memcpy(dbuff,buff+t*8,8);
			if(mystrncmp(dbuff,"\x00\x00\x00\x00\x00\x00\x00\x00",8) != 0)
			{
				t = 0;
				break;
			}
		}
		if(t >= 8)
		{
#if Initprintf
			printf("FindSavedata  iii  1=%d  \n", i);
#endif
			break;
		}
	}

	if(i > 0)
	{
		savedata = (i-1)*SAVE_DATA_LEN*1000;

#if Initprintf
		printf("FindSavedata  savedata=%d  \n", savedata);
#endif
		for(i = 0; i<1000; i++)
		{
			addr = savedata + i*SAVE_DATA_LEN;
#ifdef SAVE_CONSUM_DATA_DIRECT
			result = lseek(Datafile,addr, SEEK_SET);
			memset(buff,0,sizeof(buff));
			result = read(Datafile,buff,SAVE_DATA_LEN-SAVE_DATA_RESERVE);
#else
			result = fseek(Datafile,addr, SEEK_SET);
			memset(buff,0,sizeof(buff));
			result = fread(buff,sizeof(unsigned char),64,Datafile);
#endif
#if Initprintf
			//    printf("FindS 2==:");
			//  for(aa= 0;aa< 64;aa++)
			//   {
			//      printf("%02X",buff[aa]);
			//    }
			//    printf("\n");
#endif

			for(t=0; t<8; t++)
			{
				memset(dbuff,0,sizeof(dbuff));
				memcpy(dbuff,buff+t*8,8);
				if(mystrncmp(dbuff,"\x00\x00\x00\x00\x00\x00\x00\x00",8) != 0)
				{
					t = 0 ;
					break;
				}

			}
			if(t >= 8)
			{
#if Initprintf
				printf("FindSavedata  iii2=%d  \n", i);
#endif
				break;
			}
		}
		//fclose(Datafile);
#ifdef SAVE_CONSUM_DATA_DIRECT
		close(Datafile);
#else
		fclose_nosync(Datafile);
#endif
		if(i != 1000)
		{
			SaveNum.i = (unsigned int)(savedata/SAVE_DATA_LEN + i + 1);
		}
		else
		{
			SaveNum.i = (unsigned int)(savedata/SAVE_DATA_LEN + 1001);
		}

		if (TransactionNum.i < 1)
		{
			TransactionNum.i = SaveNum.i;
		}

#if Initprintf
		printf("FindSavedata 1 == %d  \n", SaveNum.i);
#endif

		if(SaveNum.i < 200000)
		{
			//  while (FileOpenFlag == 0);
			//FileOpenFlag = 0;
			ReadOrWriteFile(WSDATA);
			//  FileOpenFlag = 1;
		}
		else
		{
			SavedataErr = 1;
		}
	}
	else
	{
		//fclose(Datafile);
#ifdef SAVE_CONSUM_DATA_DIRECT
		close(Datafile);
#else
		fclose_nosync(Datafile);
#endif

#ifndef ZHUHAI_DUSHUGAO
		SaveNum.i = 1;
		TransactionNum.i = 1;
		//  while (FileOpenFlag == 0);
		//FileOpenFlag = 0;
		ReadOrWriteFile(WSDATA);
#endif
		//   FileOpenFlag = 1;
	}

	if(SaveNum.i > 100000) SavedataErr = 2; //

#if Initprintf
	printf("FindSavedata 2 == %d  \n", SaveNum.i);
#endif

	pthread_mutex_unlock(&m_datafile);
//  savedataflag = 0;
	return 0;
}




void InitYangZhouCard()		//这里应该改成本地城市卡相关信息初始化
{

	memcpy(Yanzhou_Card.CityCode,"\x10\x00",2);//城市代码
	memcpy(Yanzhou_Card.IssuerLabel,"\x01\x01\x10\x00\xFF\xFF\xFF\xFF",8);//收单机构标示
	memcpy(Yanzhou_Card.OperatorCode,"\x00\x00",2);//运营商代码
	memcpy(Yanzhou_Card.LineNO,"\x00\x00",2);//线路号
	memcpy(Yanzhou_Card.BusNO,"00000000",8);//车辆号
}






/*
*************************************************************************************************************
- 函数名称 : void Read_Parameter(void)
- 函数说明 : 读取参数表
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void Read_Parameter (void)
{
	//while(FileCardLan == 0);
	//    FileCardLan = 0;
	CardLanFile (SYSFILE);
	//    FileCardLan = 1;
}


int CheckWirelessModule()
{
    int ret1,ret=1;
    int cnt,rlen;
    char buf[10]={"AT\r\n"};
    char RX_buff[64] = {"\0"};
    w55fa93_setio(GPIO_GROUP_A, 1, 1);
    usleep(500000);
    ret1 = M26_init("/dev/ttyS0");    
    if(ret1>0)
     {
        cnt = 0;
	 do {
	 	 rlen = 0;
		 memset(RX_buff,0,sizeof(RX_buff));  
	 	 write(M26_fd,buf,sizeof(buf)); 
		ret = read_ttyS0_data(M26_fd, RX_buff, &rlen);
        
		if (rlen)
		{
			if((strchr(RX_buff, 'O') !=  NULL) || (strchr(RX_buff, 'K') !=  NULL)) 
				break;
		}
		usleep(500000);
		cnt++;
	   }while(cnt < 2); //30s check network status
	   if(cnt<=2)
        ret = 0;
      close(M26_fd);
        }
      return ret;      
    }


void ShowMessage(int x ,int y,int font,char *out)
{
	SetTextSize(font);
	SetTextColor(Color_white);
	TextOut(x,y,out);
    beepopen(1);
	usleep(500000);
}



// added by taeguk calculate CRC16
extern void Calc_UpdateCrc(void);
/*
*************************************************************************************************************
- 函数名称 : unsigned char InitSystem(void)
- 函数说明 : 初始化数据
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
unsigned char InitSystem(void)
{
	FILE *Filebuf;
    char buff[9000];
    char i;
    int ret;
	system("echo 0 > /proc/sys/vm/dirty_writeback_centisecs;");   


	if(access("/mnt/nand1-2/app/cardlan.sys" , F_OK)==0)
	{
		system("mv /mnt/nand1-2/app/cardlan.sys /mnt/record/");
	}
	if(access("/mnt/nand1-2/app/system.sys" , F_OK)==0)
	{
		system("mv /mnt/nand1-2/app/system.sys /mnt/record/");
	}
	if(access("/mnt/nand1-2/app/Blacklist.sys" , F_OK)==0)
	{
		system("mv /mnt/nand1-2/app/Blacklist.sys /mnt/record/");
	}
	if(access("/mnt/nand1-2/app/section.sys" , F_OK)==0)
	{
		system("mv /mnt/nand1-2/app/section.sys /mnt/record/");
	}
	if(access("/mnt/nand1-2/app/sectionup.sys" , F_OK)==0)
	{
		system("mv /mnt/nand1-2/app/sectionup.sys /mnt/record/");
	}
	if(access("/mnt/nand1-2/app/section.bin" , F_OK)==0)
	{
		system("mv /mnt/nand1-2/app/section.bin /mnt/record/");
	}
	if(access("/mnt/nand1-2/app/RecordErrorFile.txt" , F_OK)==0)
	{
		system("mv /mnt/nand1-2/app/RecordErrorFile.txt /mnt/record/");
	}  
    system("sync;");

    SetColor(Mcolor);
	ShowMessage(0,5,16,"设备初始化...");


	bp_fd=open("/dev/fullgpio",O_RDWR);
	if(bp_fd<0)
	{
    	close(bp_fd);
    	printf("Can't open /dev/fullgpio\n");
        ShowMessage(0,22,16,"加载GPIO驱动失败");
    	return -2;
	}

	buzz_off();
	printf("open /dev/fullgpio\n");

	mg_fd=open("/dev/mcugpio",O_RDWR);
	if(mg_fd<0)
	{
		close(mg_fd);
		printf("Can't open /dev/mcugpio\n");
        ShowMessage(0,22,16,"加载MCU驱动失败");
		return -2;
	}

	mf_fd=open("/dev/typea",O_RDWR);
	if(mf_fd<0)
	{
		printf("Can't open /dev/typea \n");
		close(mf_fd);
        ShowMessage(0,22,16,"加载读头驱动失败");
		exit(-2);
	}
   
	printf("open /dev/typea\n");
    ShowMessage(0,22,16,"加载设备驱动成功");

	 system("chmod 755 "SYS_PARAM_FILE);
	 

	canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR|O_CREAT);
	if(canshu)
	{
		printf("open system.sys ok!\n");
		close(canshu);
        ShowMessage(0,39,16,"加载设备信息成功");
	}

	
	else
	{
		close(bp_fd);
		close(mf_fd);
		printf("Can't open /system.sys\n");
        ShowMessage(0,39,16,"加载设备信息失败");
		exit(-3);
	}

	Filebuf = fopen(PARM_FILE_PATH,"a+");
	if(Filebuf)
	{
		printf("open /mnt/record/cardlan.sys ok!\n");
		fclose(Filebuf);
	}
	else
	{
		close(bp_fd);
		close(mf_fd);
		printf("Can't open /mnt/record/cardlan.sys\n");
		exit(-4);
	}


	 BlackFile = fopen("/mnt/record/Blacklist.sys","a+");

	if(BlackFile)
	{        
		printf("open /mnt/record/Blacklist.sys ok!\n");
		fclose(BlackFile);
	}
	else
	{
		close(bp_fd);
		close(mf_fd);
		printf("Can't open /mnt/record/Blacklist.sys\n");
        ShowMessage(0,56,16,"加载黑名单文件失败");
		exit(-4);
	}
    
   

	Datafile = open(OFF_LINE_CONSUM_FILE,O_RDWR|O_SYNC|O_CREAT);

	if(Datafile)
	{
		printf("open  cardsave.bin ok!\n");        
		close(Datafile);
	}
	else
	{
		close(bp_fd);
		close(mf_fd);
		printf("Can't open cardsave.bin \n");
        ShowMessage(0,56,16,"加载记录文件失败");
		exit(-1);
	}
	
    system("sync;");
    system("chmod 755 "OFF_LINE_CONSUM_FILE);
    #ifdef TEST_QR_CODE_SPEED
	QRTest_fd = open("/mnt/nand1-2/app/qr_speed.txt",O_RDWR|O_SYNC|O_CREAT);
	if (QRTest_fd < 0)
	{
		printf("open qr_speed failed\n");
	}
	else
		close(QRTest_fd); //success
    #endif


	Filebuf = fopen(SECTION_FILE_PATH,"a+");

	if(Filebuf)
	{
		printf("open /mnt/record/section.sys ok!\n");
		fclose(Filebuf);
	}
	else
	{
		close(bp_fd);
		close(mf_fd);
		printf("Can't open /mnt/record/section.sys\n");
        ShowMessage(0,56,16,"加载分段信息文件失败");
		exit(-1);
	}

	Filebuf = fopen(SECTIONUP_FILE_PATH,"a+");
	if(Filebuf)
	{
		printf("open /mnt/record/sectionup.sys ok!\n");
		fclose(Filebuf);
	}
	else
	{
		close(bp_fd);
		close(mf_fd);
		printf("Can't open /mnt/record/sectionup.sys\n");
        ShowMessage(0,56,16,"加载分段信息文件失败");
		exit(-1);
	}

	Filebuf = fopen(SECTIONDISUP_FILE_PATH,"a+");

	if(Filebuf)
	{
		printf("open /mnt/record/stationdisup.sys ok!\n");
		fclose(Filebuf);
	}
	else
	{
		close(bp_fd);
		close(mf_fd);
		printf("Can't open /mnt/record/stationdisup.sys\n");
        ShowMessage(0,56,16,"加载上行公里文件失败");
		exit(-1);
	}

	Filebuf = fopen(SECTIONDISDOWN_FILE_PATH,"a+");
	if(Filebuf)
	{
		printf("open /mnt/record/stationdisdown.sys ok!\n");
		fclose(Filebuf);
	}
	else
	{
		close(bp_fd);
		close(mf_fd);
		printf("Can't open /mnt/record/stationdisdown.sys\n");
        ShowMessage(0,56,16,"加载下行公里文件失败");
		exit(-1);
	}

	Filebuf = fopen(BASICRATE_FILE_PATH,"a+");

	if(Filebuf)
	{
		printf("open /mnt/record/basicrate.sys ok!\n");
		fclose(Filebuf);
	}
	else
	{
		close(bp_fd);
		close(mf_fd);
		printf("Can't open /mnt/record/basicrate.sys\n");
        ShowMessage(0,56,16,"加载基本费率文件失败");
		exit(-1);
	}


    ShowMessage(0,56,16,"加载设备运行文件成功");
    
    g_FgFileOccurError = 0;
	InitErrorRecordList();
	ReadErrorRecordFile(0);

	w55fa93_setio(GPIO_GROUP_B, 5, 0);
	usleep(100000);
	w55fa93_setio(GPIO_GROUP_B, 5, 1);

    relayoff();

	printf("open /dev/mcugpio\n");
	led_fd=open("/dev/mcugpio",O_RDWR);
	if(led_fd<0)
	{
		printf("Can't open /dev/mcugpio\n");
		close(led_fd);
	}
	

	InitUart(&uart4_fd,"ttyC1",9600);
    /***
    待增加键盘测试函数
    ***/
    ShowMessage(0,73,16,"键盘自检成功");
	
    /***
    查询QR状态
    ***/
    ret = CheckQR();
    if(ret==0)
        ShowMessage(0,90,16,"二维码读头加载成功");
    else
        {
            ShowMessage(0,90,16,"二维码读头加载失败");
            exit(-1);
        }

    /***
    查询无线模块状态
    ***/
    ret = CheckWirelessModule();
    if(ret==0)
        ShowMessage(0,107,16,"无线模块加载成功");
    else
        {
            ShowMessage(0,107,16,"无线模块加载失败");
            exit(-1);
        }

<<<<<<< HEAD
    


    Card_SysInit();
    ReadandWriteBasicRateFile(1);               //初始化MP文件
	InitBlackListBuff();                        //系统初始化黑名单
    InitWhiteListBuff();                        //系统初始化白名单    
	InitYangZhouCard();							// 初始化

	




#ifdef BS
    ReadMERCHANTNO();//读取商户号文件
#endif   

	//Read_Parameter();							//消费参数读取
	//CardLanFile(SectionPar);                    //上行分段参数读取
	//CardLanFile(SationdisupParup);              //上行公里数读取      
	if(Section.Enableup == 0x55)
	{
		//CardLanFile(SectionParup);              //下行参数读取
		//CardLanFile(SationdisdownPardown);      //下行公里数读取 
	}
	printf("in inisystem the sectionum :%d\n",SectionNum);
	
	FindSavedata();	
	// added by taeguk calculate CRC16
	Calc_UpdateCrc();	        
    InitEnv();



	system("rm -rf /mnt/nand1-2/app/font/.svn");
	system("rm -rf /mnt/nand1-2/app/res/.svn");
	system("rm -rf /mnt/nand1-2/app/sound/.svn");
	system("rm -rf /mnt/nand1-2/app/.svn");
	system("rm -rf /mnt/nand1-2/bin/.svn");
	system("rm -rf /mnt/nand1-2/.svn");
   
	system("sync;");
	
	printf("%s\n",__DATE__);
	printf("%s\n",__TIME__);

	system("rm pppd2.tdb");
	system("rm ppp0.pid");
    ShowMessage(0,124,16,"正在加载PSAM卡");
	return 0;
}


void GetTimData(unsigned int *pdate)
{
	time_t t;
	struct tm * tm;
	time (&t);
	tm = localtime (&t);

	*pdate = (((tm->tm_year + 1900)*100*100)+((tm->tm_mon + 1)*100)+tm->tm_mday);		
	//printf("year: %d , month: %d , day:%d \n",(tm->tm_year + 1900),(tm->tm_mon + 1),tm->tm_mday);
}



