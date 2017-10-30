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

#include <termios.h>    									/*PPSIX �ն˿��ƶ���*/
#include <linux/hdreg.h>
#include "RC500.h"
#include "../bzdes/stades.h"
#include "../sound/sound.h"

#include "../update/Resumedownload.h"
#include "cpucard.h"
#include "savesql.h"

#define Initprintf 1

/************************************************************/

/***�ⲿ���ñ���***/
extern RouteSelection JackCmd;
extern SectionFarPar Section,Sectionup;
extern int RecFd;
extern LongUnon Fixvalue;
extern unsigned int g_MaxPersonNumber;
extern unsigned char updataflag;	
extern unsigned short Mcolor;
extern int M26_fd;
extern pthread_mutex_t m_stationrecord;


/***��������***/

//�豸���
int mf_fd;
int bp_fd;
int mg_fd;
int beep_fd;
int uart4_fd;
int led_fd;
//�ļ����
FILE *canshu;
FILE *ParaFileBuf;
FILE *BlackFile;
FILE *Datafile;
FILE *teth;
FILE *nettab;
FILE *Rechargefile;
time_t utc_time;
FILE *MCHID;//�̻����ļ����
FILE *Stationdisfile;
//��������
unsigned char *CardLanBuf=NULL;
unsigned char *SectionParBuf=NULL;
unsigned char *SectionParUpBuf=NULL;
unsigned char *StationdisupParBuf=NULL;
unsigned char *StationdisdownParBuf=NULL;


/*************************************/
char des_key[8] = {0x20,0x05,0xAF,0x45,0xA3,0xF1,0x80,0x78}; 			//��������
char TcpIpBuf[35];
unsigned char KeyDes[8];							//��Կ
unsigned char AdminPwd[8];  //����Ա����
unsigned char SleepOverTime;							//�����رճ���ʱ��
unsigned char SecTor[7];
unsigned char PsamNum[6];
unsigned char PsamNum_bak1[6];			//��Ϊ����cpu������psamNum�ı��ݣ���ִ�п���cpu������ʱʹ��
unsigned char PsamNum_bak2[6];		    //���ݽ�ͨ����psamnum
unsigned char PsamKeyIndex;				//psam����Կ����
unsigned char Tunion;                   //��ͨ���ϱ�־����λPSAM���ɹ���˱�־Ϊ1��ʧ��Ϊ0��         
unsigned char OperBuffor[80];	        //����Ա���뱸��
unsigned char SelfAddress[36];
unsigned char OPENBEEP;                 //1������ 0�ر�����
unsigned char BankCardSwitch;           //0x55-����ˢ���п�,0xAA-��ֹˢ���п�
unsigned char OperCount;                //����Ա����
unsigned char OPENPRINTF = 0;
unsigned char SavedataErr = 0;
unsigned char ReadCardFirst;
unsigned char COMNET;                   // 1: ����TCP/IP   2: ����WIFI     3: ����CDMA   4: ����GPRS
unsigned short SectionNum;
unsigned char MerNo[4] ={0};//�̻��� 
CardLanSector LanSec;		            //�û�����
des_context ctx;                        //des�����㷨����ʹ�õ���������
st_BlackFile BlackListFile;
st_WhiteFile WhiteListFile;
bk_BlackFile BkBlackListFile;
st_BlackFile BlackListFilezhujian;
unsigned char g_FgFileOccurError = 0;
unsigned char g_FgSendErrorAgain = 0;
unsigned char g_FgHasFindCardHandle = 0;
unsigned char g_FgCardHandleOrNot = 0;  //0xaa����Ҫ��cardhandle ƥ��,0x55��Ҫ����cardhandleƥ�䣬
								        //0xcc��Ҫ����cardhandle ��ֻȡ��2λOriginalCardType = 0xXX���Ҳ�����SET
YanZhouCard Yanzhou_Card;               //��ͨ�����д���ṹ								       


#ifdef TEST_QR_CODE_SPEED
int QRTest_fd = -1;
#endif
//���������
LongUnon TransactionNum;                // �ܽ��״���
LongUnon SaveNum;                       //���ݴ洢 �洢��¼������
LongUnon SaveNumBs;                     //���ݴ洢
LongUnon SaveNumBc;                     //���ݴ洢
LongUnon CodeNum; 		                // ���ϴ���¼��
LongUnon Buf;		                    //Ӳ�����
LongUnon DevNum;	                    //�ն˻�����
LongUnon DevSID;                        //�ն˻���ˮ��
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
���� �� BCD ������ ASCII
��ڲ����� data: ת�����ݵ����ָ��
buffer: ת�����������ָ��
len : ��Ҫת���ĳ���
���ز�����ת�������ݳ���
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
- �������� : int hex_2_ascii(INT8U *INdata, char *buffer, INT16U len)
- ����˵�� : HEX �� ASCII��ת������
- ��ڲ����� INdata
- ������� : buffer
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
- �������� : unsigned char HEX2BCD(unsigned char hex_data)
- ����˵�� :
- ������� : ��
- ������� : ��
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
- �������� : unsigned char HEX2BCD(unsigned char hex_data)
- ����˵�� : BCD ���� ��HEX
- ������� : BCD
- ������� : HEX
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
- �������� : void HEX8TOBCD(unsigned int In, unsigned char *Pdata)
- ����˵�� :
- ��ڲ�����
- ������� :
*************************************************************************************************************
*/
void HEX8TOBCD(unsigned int In, unsigned char *Pdata)
{
	unsigned int iv,i;
	unsigned char BCD[4];//����8λBCD��
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
- �������� : unsigned int  BCDToDec(const unsigned char *bcd, unsigned  char length)
- ����˵�� :
- ��ڲ�����
- ������� :
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
- �������� : char  Wr_time (char *dt)
- ����˵�� : дʱ��
- ������� : dt
- ������� :
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
- �������� : char * Rd_time (char* buff)
- ����˵�� : ��ʱ��
- ������� : ��
- ������� : ��
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
- �������� : char * Rd_time (char* buff)
- ����˵�� : ��ʱ��
- ������� : ��
- ������� : ��
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
- �������� : void beepopen (unsigned char Mode)
- ����˵�� : ������
- ������� : ��
- ������� : ��
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
		case 1: 			//����
			buzz_on();
			usleep(20000);
			buzz_off();
			//thread_timer = 1;
			break;
		case 2: 			//����
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
		case 5: 			//����
			buzz_on();
			usleep(600000);
			buzz_off();
			break;
		case 6: 			//����
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

	   case 5:   //��һ�ο������ùص�ģ���Դ  test
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
- �������� : char ethopen(void)
- ����˵�� : �ж����ӷ�ʽ
- ������� : ��
- ������� : ��
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
	
	if (strstr(buff, "IP") && strstr(buff, "PORT"))   //�����ļ�����IP��˿�
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


/*===========�������·�ʽ===========*/

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
	
	//printf("�����۰���Һ�����\n");
	while (low <= high)
	{
		mid = (low + high)/ 2;

		DBG_PRINTF("BlackListFile.buf[mid].dat: %02X%02X%02X%02X%02X%02X%02X%02X\n", BlackListFile.buf[mid].dat[0],BlackListFile.buf[mid].dat[1],\
		BlackListFile.buf[mid].dat[2],BlackListFile.buf[mid].dat[3],BlackListFile.buf[mid].dat[4],BlackListFile.buf[mid].dat[5],BlackListFile.buf[mid].dat[6],BlackListFile.buf[mid].dat[7]);
		
		//printf("dest %p buf %p \n",dest.dat,BlackListFile.buf[mid].dat);
		val = memcmp(dest.dat, BlackListFile.buf[mid].dat, 10 );//izeof(BlackListFile.buf[0]));
		//  val= 1;	
		//printf("ʹ��memcmpû����\n");
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
   
//	printf("half_serach����\n");
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
	
	//printf("�����۰���Һ�����\n");
	while (low <= high)
	{
		mid = (low + high)/ 2;

		DBG_PRINTF("BlackListFile.buf[mid].dat: %02X%02X%02X%02X%02X%02X%02X%02X\n", BkBlackListFile.buf[mid].dat[0],BkBlackListFile.buf[mid].dat[1],\
		BkBlackListFile.buf[mid].dat[2],BkBlackListFile.buf[mid].dat[3],BkBlackListFile.buf[mid].dat[4],BkBlackListFile.buf[mid].dat[5],BkBlackListFile.buf[mid].dat[6],BkBlackListFile.buf[mid].dat[7]);
		
		//printf("dest %p buf %p \n",dest.dat,BlackListFile.buf[mid].dat);
		val = memcmp(dest.dat, BkBlackListFile.buf[mid].dat, 8 );//izeof(BlackListFile.buf[0]));
		//  val= 1;	
		//printf("ʹ��memcmpû����\n");
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
   
//	printf("half_serach����\n");
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
	
	//printf("�����۰���Һ�����\n");
	while (low <= high)
	{
		mid = (low + high)/ 2;

		DBG_PRINTF("BlackListFile.buf[mid].dat: %02X%02X%02X%02X%02X%02X%02X%02X\n", BlackListFilezhujian.buf[mid].dat[0],BlackListFilezhujian.buf[mid].dat[1],\
		BlackListFilezhujian.buf[mid].dat[2],BlackListFilezhujian.buf[mid].dat[3],BlackListFilezhujian.buf[mid].dat[4],BlackListFilezhujian.buf[mid].dat[5],BlackListFilezhujian.buf[mid].dat[6],BlackListFilezhujian.buf[mid].dat[7]);
		
		//printf("dest %p buf %p \n",dest.dat,BlackListFile.buf[mid].dat);
		val = memcmp(dest.dat, BlackListFilezhujian.buf[mid].dat, 10 );//izeof(BlackListFile.buf[0]));
		//  val= 1;	
		//printf("ʹ��memcmpû����\n");
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
   
//	printf("half_serach����\n");
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
	
	//printf("�����۰���Һ�����\n");
	while (low <= high)
	{
		mid = (low + high)/ 2;

		DBG_PRINTF("WhiteListFile.buf[mid].dat: %02X%02X%02X%02X\n", WhiteListFile.buf[mid].dat[0],WhiteListFile.buf[mid].dat[1],\
		WhiteListFile.buf[mid].dat[2],WhiteListFile.buf[mid].dat[3]);

		
		//printf("dest %p buf %p \n",dest.dat,BlackListFile.buf[mid].dat);
		val = memcmp(dest.dat, WhiteListFile.buf[mid].dat, 4 );//izeof(BlackListFile.buf[0]));
		//  val= 1;	
		//printf("ʹ��memcmpû����\n");
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

//	printf("half_serach����\n");
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



//��ȡ�̻���
//ע:�̻��ŵ�λ���ڲ����ļ��еڶ����ֽڿ�ʼ���ĸ��ֽ�
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
- �������� : void ReadOrWriteFile (unsigned char RW_Type)
- ����˵�� : ��д��ʼ������ ����
- ������� : RW_Type
- ������� : ��
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

	des_set_key(&ctx,des_key); //����des��������
	switch (RW_Type)
	{
	case SYSFILE:					//��ȡ��Կ
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

		memset (FileBuf,0,sizeof(FileBuf));		//��ȡ�ն˻���
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


		memset (FileBuf,0,sizeof(FileBuf));		//��ȡ����Ա����
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


		memset (FileBuf,0,sizeof(FileBuf));			//�޸Ĵ���ʱ��
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
		result = read(canshu,FileBuf,8);		//��ȡ�û�����
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
		result = read(canshu,filebuf,40);	//��ȡIP
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
		result = read(canshu,FileBuf,8);		//CPU��Ӧ��
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
		result = read(canshu,FileBuf,8);		//������ˮID
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
		result = read(canshu,FileBuf,8);	//���ͼ�¼����
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
		result = read(canshu,FileBuf,8);	//���I��
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
		result = read(canshu,FileBuf,8);		//���I��
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
		result = read(canshu,FileBuf,8);	//�����¼����
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
		result = read(canshu,FileBuf,8); //����˾�����
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
		result = read(canshu,fileBuffer,16); //�ֶ���Ϣ
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
			memcpy(&Section.SationNum,fileBuffer,10);		//��������������sationnum����ֵ


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
		result = read(canshu,FileBuf,8); //�������ȱ�־
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
		result = read(canshu,fileBuffer,16); //�ֶ�վ̨������Ԥ�۽�� ����ʱʱ��
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
		result = read(canshu,FileBuf,8); //�ֶ������б�־
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
		result = read(canshu,FileBuf,8);	//0x55����ˢ���п�,0xAA��ֹˢ���п�
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
		result = read(canshu,FileBuf,8);	//��ȡ�����
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
		result = read(canshu,FileBuf,8);	//0x55����Ҫ��cardhandle ƥ��,0xAA/0xcc��Ҫ����cardhandleƥ��
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
		result = read(canshu,FileBuf,8);  //�ֶ���·��
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


			/*��ȡ�����趨�Ĺ̶�����ֵ*/
		memset(FileBuf,0,sizeof(FileBuf));			
		result = lseek(canshu,568, SEEK_SET);
		result = read(canshu,FileBuf,8);
		
		if(mystrncmp(FileBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) == 0)//��û�иò�����¼
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

	case MUSERKEY://������Կ
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

	case MUESRPWD:	//�޸Ĺ���Ա����
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

	case CODEFILE:   //�����¼��		
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

	case RCODEFILE:   //����¼��
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
- �������� : unsigned char Number(unsigned int CardNumber,unsigned char cmd)
- ����˵�� : ��������д
- ������� :
- ������� : ��
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
	case 0:										//����
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
		}				//���
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
- �������� : void Card_SysInit(void)
- ����˵�� : ���ݳ�ʼ��
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
//00 - 08   ��ȡ��Կ
//16 - 24   ��ȡ�ն˻�
//32 - 40   ����Ա����
//272 - 312 IP��ַ
//512 - 514 ��ȡ�û�����
//528 - 528 ���Ľ�������
//544 - 560 �������ȴ�ʱ��
//560 - 576 ���Ĵ���ʱ��
//596 - 612 ���Ĵ�ӡ��״̬
//613 - 628 ����������ˮ��
//629 - 644 ����������ˮ��
//1024 + 8  �ɼ���¼ָ��
//1040      ���I����
void Card_SysInit(void)
{
	//while (FileOpenFlag == 0);
	//    FileOpenFlag = 0;
	ReadOrWriteFile (SYSFILE);
	//    FileOpenFlag = 1;
}
/*
*************************************************************************************************************
- �������� : unsigned char Para_cardlan(unsigned char *buf,unsigned int Addr,unsigned char Long,unsigned char Mode)
- ����˵�� : ���� ��д ������
- ������� : ��
- ������� : ��
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
- �������� : void CardLanFile (unsigned char RW_Type)
- ����˵�� : ��ȡ������
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
void CardLanFile (unsigned char RW_Type)
{
	FILE *ParaFile;
	int result;
	unsigned char buffer[512];
	unsigned char i;
	unsigned int mkLengthUp, mkLengthDown;

	unsigned char j; 
	int max,maxci,ci;
    ShortUnon tmp;

	printf("---->you are in cardlanfile------>\n");
	switch(RW_Type)
	{
	case SYSFILE: //ϵͳ����
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
		
		ParaFile = fopen(PARM_FILE_PATH,"rb+");
		if (NULL != ParaFile)
		{
			fseek(ParaFile, 512, SEEK_SET);
			fread(LocalCardRate, sizeof(LocalCardRate[0]), CARD_NUMBER, ParaFile);
			fclose(ParaFile);
		}
		#endif
		break;

	case SectionPar://���зֶ�
		free(SectionParBuf);
		SectionParBuf = NULL;
		SectionParBuf = (unsigned char *)malloc(16384*sizeof(unsigned char));
		if(SectionParBuf != NULL)
		{
           
			ParaFile = fopen("/mnt/record/M4","rb+");
            max = filem4.uprecordnum.i*filem4.uprecordnum.i;                         //��Ҫ�޸ĸ���Ʊ�۲����������
	        ci = 0;
			ParaFile = fopen("/mnt/record/M4","rb+");
	        for(i=0;i<max;i++) {
	            for(j=0;j<=i;j++){
	                result = fseek(ParaFile, 6, SEEK_SET);
	                result = fread(SectionParBuf+(i*max+j)*2,sizeof(unsigned char),2,ParaFile);
	              //  ci++;
	              //  if(ci>=maxci)
	              //      break;
	            }
	         }  
			

			fclose(ParaFile);
		}
		break;

	case SectionParup://���зֶ�
		free(SectionParUpBuf);
		SectionParUpBuf = NULL;
		SectionParUpBuf = (unsigned char *)malloc(16384*sizeof(unsigned char));
		if(SectionParUpBuf != NULL)
		{       
			
			ParaFile = fopen("/mnt/record/M4","rb+");
			max = filem4.downrecordnum.i*filem4.downrecordnum.i;				   //��Ҫ�޸ĸ���Ʊ�۲����������
			ci = 0;
			ParaFile = fopen("/mnt/record/M4","rb+");
			for(i=0;i<max;i++) {
				for(j=0;j<=i;j++){
					result = fseek(ParaFile, 6, SEEK_SET);
					result = fread(SectionParBuf+(i*max+j)*2,sizeof(unsigned char),2,ParaFile);
				//	ci++;
				//	if(ci>=maxci)
				//		break;
				}
			 }	
			fclose(ParaFile);


			
		}
		break;

	case SationdisupParup://���й�����
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
				result += fread(StationdisupParBuf+result,sizeof(unsigned char),mkLengthUp-result,ParaFile);
				i--;
				if(feof(ParaFile)) break;
			}while(i && (result != mkLengthUp));
			//memcpy(StationdisupParBuf+i*512,buffer,512);
		}
		fclose(ParaFile);
		break;

	case SationdisdownPardown://���й�����
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
				result += fread(StationdisdownParBuf+result,sizeof(unsigned char),mkLengthDown-result,ParaFile);
				i--;
				if(feof(ParaFile)) break;
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
    //д���ؿ�(M1����ס����CPU��)
	case 0:        
		ParaFileBuf = fopen("/mnt/record/cardlan.sys","rb+");        
		result = fseek(ParaFileBuf, 512, SEEK_SET);
        result = fwrite(CardLanBuf+512,sizeof(unsigned char),13*48,ParaFileBuf);
		fclose(ParaFileBuf);
		break;
        
    //��ͨ����ͨ��
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
    //д���к��������ڽ���վ�ź����ڽ��Ĺ�����
       
    //д���и�վ֮��Ĺ�����
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

      //д���и�վ֮��Ĺ�����
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

void WriteSection_Para(unsigned char type,unsigned char *dat,unsigned int len,unsigned int offset)
{
    int result;
    int i,j,ci,maxci,max;
    FILE *ParaFile;
   
    switch(type)
	{
    //д���в���

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
        
    //д���в���

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
		result = fwrite(dat,sizeof(unsigned char),len,ParaFile);
			

        CardLanFile(SectionParup);
		break;	      
        

	default :
		break;
	}

    
 }


//��д���ʻ�����Ϣ�ļ� 0Ϊд1Ϊ��
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
			 memcpy(&filemp.linenum[0],0,sizeof( FileMP));
             result = fseek(basicrate, 0, SEEK_SET);
             result = fread(&filemp.linenum[0],sizeof(unsigned char),127,basicrate);
             fclose(basicrate);
			 DBG_PRINTF("���ʿ�������Ϣ�ļ�MP:");
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




void InitYangZhouCard()		//����Ӧ�øĳɱ��س��п������Ϣ��ʼ��
{

	memcpy(Yanzhou_Card.CityCode,"\x10\x00",2);//���д���
	memcpy(Yanzhou_Card.IssuerLabel,"\x01\x01\x10\x00\xFF\xFF\xFF\xFF",8);//�յ�������ʾ
	memcpy(Yanzhou_Card.OperatorCode,"\x00\x00",2);//��Ӫ�̴���
	memcpy(Yanzhou_Card.LineNO,"\x00\x00",2);//��·��
	memcpy(Yanzhou_Card.BusNO,"00000000",8);//������
}






/*
*************************************************************************************************************
- �������� : void Read_Parameter(void)
- ����˵�� : ��ȡ������
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
void Read_Parameter (void)
{
	//while(FileCardLan == 0);
	//    FileCardLan = 0;
	CardLanFile (SYSFILE);
	//    FileCardLan = 1;
}

int read_ttyS0_data(int fd, char *rcv_buf, int *len)
{
	int retval;
	fd_set rfds;
	struct timeval tv;
	int ret=0,pos=0;
	tv.tv_sec = 1;//set the rcv wait time 0
	tv.tv_usec = 0;//200000us = 0.3s 100000
	pos = 0;
	if ((fd < 0) ||( NULL == rcv_buf))
    {
        printf("Read Buffer is Nc\n");
        return -1;
    }
	
	while(1)
	{
		FD_ZERO(&rfds);
		FD_SET(fd,&rfds);
		retval = select(fd+1,&rfds,NULL,NULL,&tv);
		if(retval ==-1)
		{
			perror("select()");
			return -1;
		}
		else if(retval)
		{
			ret= read(fd, rcv_buf+pos, 64);
			if(ret>0)
			{
				pos += ret;
				*len = pos;
				if (pos >= 7)   //20
					return 0;
			}
			else
				return -1;
		}
		else      //timeout
		{
				return 1;
		}
	}
	return 0;
}


int M26_init(char *dev) 
{ 
	M26_fd = open(dev,O_RDWR); 
	if(M26_fd < 0)
	{
		//printf("��ͨ��ģ��ʧ��\n");
		return -1;
	} 
	
    if(M26_fd > 0)
    {
        set_speed (M26_fd, 115200);
        if (set_Parity (M26_fd, 8,1,'n') < 0)
        {
            close(M26_fd);
            //printf ("uart set_parity error\n");
        }
        return M26_fd;
    }

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
- �������� : unsigned char InitSystem(void)
- ����˵�� : ��ʼ������
- ������� : ��
- ������� : ��
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
	ShowMessage(0,5,16,"�豸��ʼ��...");


	bp_fd=open("/dev/fullgpio",O_RDWR);
	if(bp_fd<0)
	{
    	close(bp_fd);
    	printf("Can't open /dev/fullgpio\n");
        ShowMessage(0,22,16,"����GPIO����ʧ��");
    	return -2;
	}

	buzz_off();
	printf("open /dev/fullgpio\n");

	mg_fd=open("/dev/mcugpio",O_RDWR);
	if(mg_fd<0)
	{
		close(mg_fd);
		printf("Can't open /dev/mcugpio\n");
        ShowMessage(0,22,16,"����MCU����ʧ��");
		return -2;
	}

	mf_fd=open("/dev/typea",O_RDWR);
	if(mf_fd<0)
	{
		printf("Can't open /dev/typea \n");
		close(mf_fd);
        ShowMessage(0,22,16,"���ض�ͷ����ʧ��");
		exit(-2);
	}
   
	printf("open /dev/typea\n");
    ShowMessage(0,22,16,"�����豸�����ɹ�");

	 system("chmod 755 "SYS_PARAM_FILE);
	 

	canshu = open(SYS_PARAM_FILE,O_SYNC|O_RDWR|O_CREAT);
	if(canshu)
	{
		printf("open system.sys ok!\n");
		close(canshu);
        ShowMessage(0,39,16,"�����豸��Ϣ�ɹ�");
	}

	
	else
	{
		close(bp_fd);
		close(mf_fd);
		printf("Can't open /system.sys\n");
        ShowMessage(0,39,16,"�����豸��Ϣʧ��");
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
        ShowMessage(0,56,16,"���غ������ļ�ʧ��");
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
        ShowMessage(0,56,16,"���ؼ�¼�ļ�ʧ��");
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
        ShowMessage(0,56,16,"���طֶ���Ϣ�ļ�ʧ��");
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
        ShowMessage(0,56,16,"���طֶ���Ϣ�ļ�ʧ��");
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
        ShowMessage(0,56,16,"�������й����ļ�ʧ��");
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
        ShowMessage(0,56,16,"�������й����ļ�ʧ��");
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
        ShowMessage(0,56,16,"���ػ��������ļ�ʧ��");
		exit(-1);
	}

	Filebuf = fopen(SECTION_KM_PATH_NAME,"a+");

	if(Filebuf)
	{
		printf("open /mnt/record/M3 ok!\n");
		fclose(Filebuf);
	}
	else
	{
		close(bp_fd);
		close(mf_fd);
		printf("Can't open /mnt/record/M3\n");
        ShowMessage(0,56,16,"���ع������ļ�ʧ��");
		exit(-1);
	}

	Filebuf = fopen(PARM_FILE_PATH,"a+");

	if(Filebuf)
	{
		printf("open /mnt/record/M4 ok!\n");
		fclose(Filebuf);
	}
	else
	{
		close(bp_fd);
		close(mf_fd);
		printf("Can't open /mnt/record/M4\n");
        ShowMessage(0,56,16,"����վ����Ϣ�ļ�ʧ��");
		exit(-1);
	}
	
	Filebuf = fopen("/mnt/record/M5","a+");

	if(Filebuf)
	{
		printf("open /mnt/record/M5 ok!\n");
		fclose(Filebuf);
	}
	else
	{
		close(bp_fd);
		close(mf_fd);
		printf("Can't open /mnt/record/M5\n");
        ShowMessage(0,56,16,"���ؿ�����Ϣ�ļ�ʧ��");
		exit(-1);
	}

	Filebuf = fopen("/mnt/record/MP","a+");

	if(Filebuf)
	{
		printf("open /mnt/record/MP ok!\n");
		fclose(Filebuf);
	}
	else
	{
		close(bp_fd);
		close(mf_fd);
		printf("Can't open /mnt/record/MP\n");
        ShowMessage(0,56,16,"�����豸��Ϣ�ļ�ʧ��");
		exit(-1);
	}
	
	
    ShowMessage(0,56,16,"�����豸�����ļ��ɹ�");
    
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
    �����Ӽ��̲��Ժ���
    ***/
    ShowMessage(0,73,16,"�����Լ�ɹ�");
	
    /***
    ��ѯQR״̬
    ***/
    ret = CheckQR();
    if(ret==0)
        ShowMessage(0,90,16,"��ά���ͷ���سɹ�");
    else
        {
            ShowMessage(0,90,16,"��ά���ͷ����ʧ��");
            exit(-1);
        }

    /***
    ��ѯ����ģ��״̬
    ***/
    ret = CheckWirelessModule();
    if(ret==0)
        ShowMessage(0,107,16,"����ģ����سɹ�");
    else
        {
            ShowMessage(0,107,16,"����ģ�����ʧ��");
            exit(-1);
        }


    


    Card_SysInit();   
    ReadandWriteBasicRateFile(1);               //��ʼ��MP�ļ�
	InitBlackListBuff();                        //ϵͳ��ʼ��������
    InitWhiteListBuff();                        //ϵͳ��ʼ��������    
	InitYangZhouCard();							// ��ʼ���ط�����

	




#ifdef BS
    ReadMERCHANTNO();//��ȡ�̻����ļ�
#endif   

	//Read_Parameter();							//���Ѳ�����ȡ
	CardLanFile(SectionPar);                    //���зֶβ�����ȡ
	CardLanFile(SationdisupParup);              //���й�������ȡ      
	if(Section.Enableup == 0x55)
	{
		CardLanFile(SectionParup);              //���в�����ȡ
		CardLanFile(SationdisdownPardown);      //���й�������ȡ 
	}
	printf("in inisystem the sectionum :%d\n",SectionNum);
	
	FindSavedata();	
	// added by taeguk calculate CRC16
	//Calc_UpdateCrc();	        
   	// InitEnv();



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
    ShowMessage(0,124,16,"���ڼ���PSAM��");
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



