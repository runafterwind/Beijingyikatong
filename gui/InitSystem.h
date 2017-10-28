#ifndef INITSYSTEM_H
#define INITSYSTEM_H

#include  <apparel.h>

/******** printf *******/
#define DEBUG			0
#define DBG_RC500		0
#define DBG_DATA		1
#define DBG_ISO14443	0
#define DBG_ISO7816		0
#define DBG_QPBOC		1
#define	DBG_RF_TRANS_TIME       0

#define  UartDis		0
#define  GPRSPR			1
#define DBG_AUDIO		0

#define VERIFY_PIN		0

//#define	SAVE_CONSUM_DATA_DURECT
//#undef	SAVE_CONSUM_DATA_DURECT

#if DEBUG
#include "stdio.h"
#define DBG_PRINTF					printf
#else
#define DBG_PRINTF(...)  
#endif


#if DBG_RC500
#define DBG_RC500_PRINTF			DBG_PRINTF
#else
#define DBG_RC500_PRINTF(...)  
#endif

#if DBG_AUDIO
#define DBG_AUDIO_PRINTF			DBG_PRINTF
#else
#define DBG_AUDIO_PRINTF(...)  
#endif

#if UartDis
#define DBG_UART_PRINTF			DBG_PRINTF
#else
#define DBG_UART_PRINTF(...)  
#endif

#if DBG_DATA
#define DBG_DATA_PRINTF			DBG_PRINTF
#else
#define DBG_DATA_PRINTF(...)  
#endif

#if DBG_QPBOC
#define DBG_QPBOC_PRINTF			DBG_PRINTF
#else
#define DBG_QPBOC_PRINTF(...)  
#endif

#if(DBG_RF_TRANS_TIME==0)
#undef	DBG_RF_TRANS_TIME
#endif

#define GC_ENTER()					DBG_PRINTF("[%-20s] : Enter...\n", __FUNCTION__)
#define GC_LEAVE()					DBG_PRINTF("[%-20s] : Leave...\n", __FUNCTION__)

// LCD keep display time 3s
#define	LCD_KEEP_DISP_TIME 	(3)

//通讯协议定义
#define  HENDDAT    "80"
#define  ENDDATA    "0D"
#define  ST_RX      0x80
#define  ST_END     0x0D
#define  CMDSTART   "DATA:"
#define  CMD_HEAD	"SYS:"
#define  CMD_TAIL	":0D"

//打印定义
#define  PRINT_START     "/var/run/start.txt"
#define  PRINT_END       "/var/run/end.txt"

//文件关闭定义 
#define fclose_nosync(x) {fclose(x); /*system("sync");*/}
#define fclose(x) {fclose(x);system("sync;");}


//文件操作状态
#define         SYSFILE                 1
#define         MUESRPWD                2
#define         MSEVERIP                3
#define         MUSERKEY                4
#define         MUSERSETOR              5

#define         OPERADORED              6
#define         MTEMNO                  7
//#define         SETTOPUP              8
#define         SLEEPOVER               9
#define         DEVSERIALID             10

#define         CODEFILE                11
#define         RCODEFILE               12
#define         OPENBUTTON              13
#define         OPERBUFFER              14
//#define         MACMONEY                14
#define         SELFIP                  15

#define         RSDATA                  16
#define         WSDATA                  17
#define         DRIVER                  18
#define         SETSECTION              19
#define         SETCARDFIRST            20

#define         SETSECTIONUP            21
#define         SETSECTINO_             22
#define	        SETBANKCARDSWITCH	    23
#define         WRITECARDHANDLE	        24
#define         PERSONWRITE	            25

#define		    SETSECTIONLINE			26	
#define 	    WRFIXVALUE				27
//#define         DRIVER                19
//系统参数定义
#define         SectionPar              (SYSFILE+1)
#define         SectionParup            (SYSFILE+2)
#define         SationdisupParup        (SYSFILE+3)
#define         SationdisdownPardown    (SYSFILE+4)


//################################-----M1卡操作命令-----########################################
//KEYA KEYB
#define		KEYA			        0x0A
#define		KEYB			        0x0B
#define 	DO_TYPEA_M1             0xB0
#define		DO_TYPEA_CPU            0xB1
#define		DO_TYPEB_CPU		    0xB4
#define     FIFO_RCV_LEN            0xB3
#define		WRITE_TYPE		        0xB2
//WRITE type
#define	    W_CARD                  0x01
#define 	W_CHAR			        0x02
#define 	W_CPU                   0x03
#define 	RC531_M1_CSN            0xAB
//TYPEA--------------M1 Command
#define		RC531_MFOUTSELECT       0xA0
#define 	RC531_REQIDL            0xA1
#define 	RC531_REQALL            0xA2
#define 	RC531_ANTICOLL          0xA3
#define 	RC531_SELECT            0xA4
#define 	RC531_AUTHENT           0xA5
#define 	RC531_READ              0xA6
#define 	RC531_WRITE             0xA7
#define 	RC531_INC               0xA8
#define 	RC531_DEC               0xA9
#define 	RC531_HALT              0xAA
#define		RC531_TRANSFER          0xC0
#define		RC531_RESTORE           0xC1
//TYPEA--------------FM1208------------------
#define		TYPEA_CPU_REST		    0xAC
#define		TYPEA_CPU_PPS		    0xAD
//TYPEB--------------SFEN------------------
#define		GET_SFEN_CSN	        0xAE

#define 	MI_OK		            0x00
#define 	MI_FAIL				    0x01
#define     MI_NOFOUND              0x02
#define 	SWIPE_CARD_ERROR	    0xFF
#define 	BACKUP_COMPLETE			0x02	//是否已经备份完成
#define 	OPERATION_COMPLETE		0x01	//操作区的数据已经写完
#define 	WALLET_FORMAT			0x00
#define 	CHEAK_BIT_FORMAT		0x01
#define 	COPY_BACKUP_TO_OPERATION		0x01
#define 	COPY_OPERATION_TO_BACKUP		0x00
#define 	WRITE_KEY			    1
#define 	VERIFY_KEY			    2
#define 	READ_ONLY			    3
#define 	WRITE_ONLY			    3
#define 	GET_NO_SERTOR			0x00
#define 	GET_BACKUP_SERTOR		0x01	
#define 	GET_OPERATION_SERTOR	0x02
#define		MIFARE_X		        10
#define		MIFARE_Y		        70
#define 	GET_RECORD				0x0001
#define 	SAVE_RECORD				0x0002

/* GPIO group definition 芯片GPIO操作定义*/
#define GPIO_OUTPUT		0
#define GPIO_VALUE		1
#define GPIO_GROUP_A 0
#define GPIO_GROUP_B 1
#define GPIO_GROUP_C 2
#define GPIO_GROUP_D 3
#define GPIO_GROUP_E 4

#define w55fa93_setio(group,num,state)	ioctl(bp_fd, GPIO_OUTPUT, ((group << 4) | (num << 8) | state));
#define w55fa93_io(group,num,state)		ioctl(bp_fd, GPIO_VALUE,  ((group << 4) | (num << 8) | state));

//峰鸣器操作定义
#if NEWBUS
#define CMD_DISABLE_PWM		0
#define CMD_ENABLE_PWM		1
extern int beep_fd;
#define buzz_off()	ioctl(beep_fd, CMD_DISABLE_PWM); 
#define buzz_on()	ioctl(beep_fd, CMD_ENABLE_PWM); 
#else
#define buzz_off()	w55fa93_setio(GPIO_GROUP_D, 3, 0);
#define buzz_on()	w55fa93_setio(GPIO_GROUP_D, 3, 1);
#endif

//功放操作定义
#define soundon()  w55fa93_setio(GPIO_GROUP_B, 5, 1);
#define soundoff()  w55fa93_setio(GPIO_GROUP_B, 5, 0);

//继电器操作定义
#define relayon()  w55fa93_setio(GPIO_GROUP_A, 6, 1);
#define relayoff()  w55fa93_setio(GPIO_GROUP_A, 6, 0);
#define SPI_SERVER_GPIO_RELAY		0x08    //继电器

/*external muc gpio define*/
#define SPI_SERVER_GPIO_LEDRED		0x01
#define SPI_SERVER_GPIO_LEDGREEN	0x02

//其他使用这定义
#define DATA_START                  100
#define DATA_END                    1115
#define CARD_SPEC_M1_8BIT 		    0
#define CARD_SPEC_M1_32BIT 	        0
#define CARD_SPEC_M1_LINUX 	        0
#define CARD_SPEC_CPU_PBCO20 	    1
#define CARD_SPEC_CPU_PBCO30 	    2
#define CARD_SPEC_EMV_DATA 	        3
#define CARD_SPEC_CPU_PBCO30_Tao 	5
#define CARD_SPEC_QR_CODE		    195
#define CARD_SPEC_QR_CODE_EXT		0x10
#define CONSUME_MODE_PRESET		    0
#define CONSUME_MODE_FLEXIBLE		1
#define CONSUME_MODE_SECTIONAL	    2
#define CONSUME_MODE_TIMING		    3
#define CONSUME_MODE_ERROR          55
#define CONSUME_MODE_RECHARGE       166
#define CONSUME_MODE_QRCODE		    200


//结构体定义
typedef struct
{
	unsigned char CityCode[2];		//城市代码
	unsigned char IssuerLabel[8];		//机构标示
	unsigned char OperatorCode[2];	//运营商代码
	unsigned char LineNO[2];			//线路号
	unsigned char BusNO[8];			//车辆号
}YanZhouCard;

struct gpio_config
{
	int port;
	int num;
	int data;
};

typedef  struct
{
	unsigned short date;
	unsigned short min;
}Interval;

//扇区缓冲
struct card_buf
{
        unsigned char key[6];
        unsigned char mode;
        unsigned char rwbuf[16];
        unsigned char money[4];
};

//公用体
typedef union
{
	unsigned char intbuf[2];
	unsigned short i;
} ShortUnon;

typedef union
{
	unsigned char longbuf[4];
	unsigned int  i;
} LongUnon;

typedef union
{
	unsigned char longbuf[8];
	unsigned long long  i;
} LongLongUnon;


//用户扇区
typedef  struct
{
// unsigned char Zer;
 unsigned char One;
 unsigned char Two;
 unsigned char Thr;
 unsigned char For;
 unsigned char Fiv;
 unsigned char Six;
 unsigned char Sev;
 unsigned char eig;
 unsigned char Nin;
 unsigned char NOP[6];
 unsigned char ADFNUM[2];
}CardLanSector;

typedef  struct
{
	unsigned char dat[8];
}BkBlackItem;
typedef  struct
{
	BkBlackItem *buf;
	unsigned int count;
}bk_BlackFile;

typedef  struct
{
	unsigned char dat[10];
}BlackItem;

typedef  struct
{
	BlackItem *buf;
	unsigned int count;
}st_BlackFile;

typedef  struct
{
	unsigned char dat[8];
}WhiteItem;
typedef  struct
{
	WhiteItem *buf;
	unsigned int count;
}st_WhiteFile;

//使用二维码相关定义
#ifdef SUPPORT_QR_CODE

struct QRCode {
	unsigned char id[40];
	unsigned char length;
	unsigned char status;
	unsigned char type;
	LongUnon  tranNo;
	unsigned char name_len;
	unsigned char name[128];
};
enum OutPut_Status{
	/*normal no error*/
	QR_NO_ERROR			= 0,
	/*below is transaction status*/
	QR_NO_SUPPOR_TRANSACTION,
	QR_SUPPORT_WECHAT_ONLY,  /*weixin only, when scan alipay*/
	QR_TRANSACTION_FAIL,
	QR_SCAN_AGAIN, /*scan qrcode again*/
	/*server return correct from 21*/
	//QR_RCV_SERVER_CORRECT = 21,
	/*server error status  from 41*/
	QR_ERROR_OFFLINE    =  41,
	QR_ERROR_NO_RESPOND	=  42,
	QR_ERROR_NO_MONEY	=  43	
};

#define    QR_CODE_STATUS	      0xF1
#define    QR_CODE_TYPE			  195   //from 195
#define	   QR_CODE_CMD_REQ		  0xD8
#define	   QR_CODE_CMD_ACK		  0xD9
#define    QR_CODE_WECHAT_TYPE	  200   //weixin
#define	   QR_CODE_ALIPAY_TYPE	  201  //zhifubao
#define	   QR_CODE_MAX_TYPE	  	  210
#define	   QR_CODE_DISPLAY_TYPE	  10
/*下面延时不准，延时1ms，实际有点像延时10ms*/
#define	   QR_CODE_WAITE_Nx10MS		(20*100)
#define    QRCode_1msDelay(n)		usleep(n*1000)  //real delay 10ms 
#define    QR_CODE_SEND_CMD_TIMES	3

extern unsigned int g_SendScanMarkCnt;
extern unsigned char g_FgQRCodeRcvAck;
extern unsigned char g_QRCodeRcvDataFg;

extern void *uart0_Pthread (void * args);
extern unsigned char QRCodeScanOutPut(enum OutPut_Status out_sta);
#if QR_CODE_USE_USBHID
extern void *UsbHid_Pthread (void * args);
#endif

#endif


#define MAX_BLACK_CONNT			    500000
#define CITYUNION_BL_FILE 			"/mnt/record/Blacklist.sys"             //IC卡黑名单
#define CITYUNION_BL_FILEBAK 		"/mnt/record/Blacklistbak.sys"          //交通部黑名单
#define CITYUNION_BL_FILEWHI        "/mnt/record/JWhitelist.sys"            //交通部白名单
#define BANK_BL_FILE                "/mnt/record/BankBlacklist.sys"         //银行卡黑名单


//引用的外部变量和函数
extern LongUnon DevNum;	//终端机机号
extern LongUnon HostValue,DecValue;
extern LongUnon FValue,LonDbuf,BFValue;
extern LongUnon DevSID;
extern unsigned char MerNo[4];//商户号 


extern void SoundMode(unsigned char mode);
extern void beepopen (unsigned char Mode);
extern unsigned char cdma_rest(unsigned int dev);
extern unsigned char mystrncmp(const unsigned char *istra,const unsigned char *istrb,unsigned char len);
extern char Rd_timed (struct tm * tm);
extern void CardLanFile (unsigned char RW_Type);
extern char ethopen(void);
extern void ReadOrWriteFileB(unsigned char RW_Type);
extern unsigned char CheckGprsStu (void);
extern int initialize (const char *com, int speed);
extern void Check_db(void);
extern unsigned char SendHandData(unsigned char *send55AA);
extern char FindSavedata(void);
extern char SaveCardData(int spec, int Mode, int OperationFlag);
extern unsigned char InitSystem(void);
extern void LEDL(unsigned char ON);
extern void LEDR(unsigned char ON);
extern void Bcd_To_Asc(unsigned char *Asc, unsigned char *Bcd, unsigned char nlen);
extern int hex_2_ascii(unsigned char *INdata, char *buffer, unsigned int len);
extern unsigned char HEX2BCD(unsigned char hex_data);
extern unsigned char BCD2HEX(char bcd_data);
extern void HEX8TOBCD(unsigned int In, unsigned char *Pdata);
extern unsigned int  BCDToDec(const unsigned char *bcd, unsigned  char length);
int USB_Updata(void);
//////////////  savesql.c
extern char RMAllRecord(unsigned int recordnum);
extern int SqlCheckNewDat (unsigned int Dtype);
extern char UPdateRecord(unsigned int Drdevnum,int SValue);
extern unsigned char SaveDirverData(void);
extern void Display_signal(unsigned char type);
extern int write_datas_gprs(int fd, unsigned char *buffer, int buf_len);
extern void InitUart(int *fd,char *uart,int speed);
extern int Uart_Printf(int fd,char *uartdata);
extern int Uart_Readdata(int fd,unsigned char *buffer, int size);
extern void * Readuart_Pthread (void * args);
extern int GetTransactionNumFromCfg(void);

//线程锁
extern pthread_mutex_t m_sysfile;
#define ReadOrWriteFile(a)  { pthread_mutex_lock(&m_sysfile);  ReadOrWriteFileB(a);  pthread_mutex_unlock(&m_sysfile); }
#define Dis 0

#endif
