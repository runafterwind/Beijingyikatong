#ifndef _RC500_H_
#define _RC500_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "InitSystem.h"


/*
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
//#include <minigui/dti.c>
#include <minigui/mgext.h>
*/

#define RC531_NO_PATH 		"/var/run/rc531_No.bin"


#define ProEnd   0x22
#define ProStart 0x11
#define WRITE_RRROR_ECORD_INTERVAL 		300


/*վ����Ϣ������վ���¼�ļ�*/
#if RUSHAN_BUS
struct STATION_INFO_			//24���ֽڴ�С
{
	unsigned short infortype;					//�ɱ�ʾ�������Ļ����쳣�ģ�δ����
	unsigned char time[5];
	unsigned char section_stationNow;
	unsigned char section_Updown;
	unsigned char section_stationNum;
	unsigned char sectionup_stationNum;
	unsigned char sectionNum;
	unsigned short line;
	unsigned int currentperson;
	struct STATION_INFO_ *next;
	struct STATION_INFO_ *pre;
};
#endif



/**********************************************************************/
#define TASKTATOL   15  //���ز���ѡ�����
typedef  struct
{
    unsigned char Parameters; 	//���ز���
    unsigned char Blacklist;	//���غ�����
    unsigned char PrintfStart;	//��ӡͷ
    unsigned char PrintfEnd;	//��ӡβ
    unsigned char Collection;	//�ɼ�����
    unsigned char Sound;		//��������
    unsigned char RmAll;		//��ʽ������
    unsigned char Upgrade;		//����
    unsigned char SetSector;	//��������
    unsigned char SetDev;		//���û���
    unsigned char Section;		//�ֶ��շ�
    unsigned char Sectionup;	//�ֶ��շ�
//�ڴ�������
    unsigned char nop; 			//��
} Operat;

typedef  struct
{
    unsigned char TrackSign;
    unsigned char Parameter[16];
} RouteSelection;

typedef  struct
{
    unsigned char Type;
    unsigned char TimeNum;
} RouteJack;

typedef  struct
{
    unsigned char year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
    unsigned char weekday;
} SysTime;

typedef  struct
{
    unsigned char FlagValue;			//Ǯ��ָʾ:�ֽ�Ǯ��
    unsigned char SOneZero[16];
    unsigned char SOneOne[16];
    unsigned char SOneTwo[16];
    unsigned char STwoZero[16];
    unsigned char STwoOne[16];
    unsigned char STwoTwo[16];
    unsigned char SThrZero[16];
    unsigned char SThrOne[16];
    unsigned char SThrTwo[16];
    unsigned char SForZero[16];
    unsigned char SForOne[16];
    unsigned char SForTwo[16];
    unsigned char SFivZero[16];
    unsigned char SFivOne[16];
    unsigned char SFivTwo[16];
	unsigned char SEigZero[16];
    unsigned char SEigOne[16];
	
	//unsigned char SNinZero[16];
	unsigned char SSevnBuf[16];
} JackRegal;

typedef  struct
{
    unsigned int Ci;
    unsigned int Day;
    unsigned int Mon;
} JackValue;

typedef  struct
{
    unsigned char  ci;
    unsigned char  re;
    unsigned short yue;
} NumValue;


typedef  struct
{
    unsigned  char SationNum[2];			  //վ̨����
    unsigned  char DeductMoney[4]; 			  //Ԥ�۽��
    unsigned  char DeductTime[2];			  //����ʱ��
    unsigned  char Enable;          //���ñ�־
    unsigned  char Enableup;        //���ñ�־
    unsigned  char StationOn;
    unsigned  char Updoor;          //ǰ���ţ����¶���
    unsigned  char Updown;          //�����ж���
	unsigned  char SationNow;
	unsigned  char Sationdis;		//վ��
	unsigned  char Sationkey;
	unsigned  char Linenum[2];			//��·��,���ֽ���ǰ�����ֽ��ں�
}SectionFarPar;


typedef  struct
{
    unsigned char  IncDecFlag;		//Ԥ��/����
    unsigned char  Money[3];		//Ԥ���ܽ��
    unsigned char  TimeBuf[2];		//Ԥ��ʱ��
} FixTimerBuf;


//������
typedef  struct
{
    unsigned char UserIcNo[4];     //��ʼ��
    unsigned char MonthOrCountflag;//����/���α�־           HEX  OK
    unsigned char SMonth[3];       //���ð���  ������                      HEX  OK
    unsigned char EMonth[3];       //��������  ������                      HEX  OK
    unsigned char CityId[2];       //���д��� 	HEX  OK
    unsigned char AppId[2];        //Ӧ�ô��� 	HEX  OK
    unsigned char UserNum[2];      //�û���� 	HEX  OK
    unsigned char CardCsnB[4];     //���� 		BCD  OK
    unsigned char CardCsnB_blk[8];
    unsigned char CardId[4];       //����֤�� 	HEX  OK
    unsigned char CardType;        //���� 		HEX  OK
    unsigned char CardGroup;        //���� 		HEX  OK
    unsigned char Pwdflag;         //�������ñ�־	HEX  OK
    unsigned char EnableH[4];      //�������� 	HEX  OK
    unsigned char Effective[4];    //��Ч����	HEX  OK
    unsigned char UserWord[3];     //�û����� 	BCD  OK
    unsigned char Views[4];        //����Ǯ��	HEX  OK
    unsigned char Subsidies[4];    //����Ǯ�� 	HEX  OK
    unsigned char QCash[4];        //�ֽ�Ǯ��	HEX  OK
    unsigned char MoneyJack[4];    //��������	HEX  OK
    unsigned char ViewsValue[3];   //�������ѽ�� 	HEX  OK
    unsigned char DayValue[3];     //�������ѽ�� 	HEX  OK
    unsigned char MonthValue[4];   //�������ѽ�� 	HEX  OK
    unsigned char Period;          //�ϴν��׵�ʱ�� 	HEX  OK
    unsigned char OldTime[6];      //�ϴν���ʱ��   	HEX  OK
    unsigned char OldTransType;    //�ϴν�������    	HEX  OK
    unsigned char OldTermNo[4];    //�ϴν����ն˻��� 	HEX  OK
    unsigned char ViewMoney[4];    //Ǯ���ۼƽ��״���     HEX  OK
    unsigned char EnterExitFlag;   //������־                      HEX  OK
    unsigned char StationID;       // վ̨���                     HEX  OK
    unsigned char EnterCarCi;      // �ϳ�����                   HEX  OK
    unsigned char StationOn;	   // �޴��޶��׼
    unsigned char StationDEC;	   // �޴��޶��׼   
    unsigned char CardTac[4];	   // CPU������TAC
    unsigned char CiMoneyFlag;	   // �޴��޶��׼
    unsigned char TimesTransfer[4];
    unsigned char TransCiFlag;
    unsigned char NoDiscountTime[6]; //��ת���Żݴ�����¼��ʱ��
    unsigned char FirstTimeDiscount[6];//��¼��һ����ת���Ż������ڴ��۵�ʱ�䣬��ʱ����2����һ��ѭ�������޸�
    //ס����CPU����ͨ�ļ��ṹ//
    unsigned char Appstate;
    unsigned char Appfalw[2];
    unsigned char Appcsn[8];	
	unsigned char CountType;
    unsigned char CAppstate;
	unsigned char CAppTermNo[6];
    unsigned char CAppCash[4];
	unsigned char CAppTime[6];
	unsigned char CardTypebak;
    
} CardInform;






#if (((defined Transport_Stander) && \
     ((defined YAN_AN_BUS)||(defined YUXI_BUS)||   \
     (defined NINGXIA_GUYUAN)||(defined PUER_BUS)|| \
    /* (defined CANGNAN_BUS)||*/(defined ZHAOTONG_BUS)|| \
     (defined LIJIANG_BUS)||(defined DIQING_BUS) || \
     (defined LIUKU_BUS)))||(defined CANGNAN_BUS) ||\
     (defined LANPING_BUS))

typedef  struct
{
    unsigned char RFIccsn[4];    	//0оƬϵ�к���	  	0-3
    unsigned char RFDtac[4];        //������TAC       		4-7
    unsigned char RFcsn[8];      	//�û�����    	  	8-15
    unsigned char RFrove[4];    	//Ӳ����ˮ��     		16-19  ����CPU������Ϊpsam���������
    unsigned char RFtype;        	//5����        	  	20-20
    unsigned char RFXFtype;      	//6��������       		21-21 0x00 ��ʾ���� 01:Ԥ�� 02:��ֵ 03:ת��   04:ʱ�����   05:��������    0x55��ʾ�������
    unsigned char RFStationID;   	//7վ̨���       		22-22
    unsigned char RFvalueq[3];   	//8����ǰ��Ƭ��� 		23-25
    unsigned char RFvaluej[3];   	//9ʵ�ʽ��׽��   		26-28
    unsigned char RFtime[4];     	//10����ʱ��(UTC��)    29-32
    unsigned char RFtran;        	//11�ն�����      		33-33   01:���� 02:���� 03:��Ա 04:С������1 05�����⳵����   06:�ѻ���ֵ��   07:�̶����ѻ�1 08:ʱ�����ѻ�
    unsigned char RFMoneyJF[4];     //12�ܻ���        	34-37	��������������ʾ
    unsigned char RFMoneyJFbuf[2];  //13������        	38-39	�������д���
    unsigned char RFtac[3];      	//14��Ƭ��������  		40-42
    unsigned char RFpurse;       	//15����Ǯ������  		43-43  0.���� 1.���� 2.�ֽ�      8 ʱ�����
    unsigned char RFvalueh[3];   	//16���׺����    		44-46
    unsigned char RFvaluey[3];   	//17ԭ���׽��    		47-49
    unsigned char RFtimeno;      	//18ʱ����      		50-50
    unsigned char RFderno[4];    	//19�ն˻���      		51-54
    unsigned char RFEnterOrExit; //20������־	  		55-55(0��  1��ʾ��)
    unsigned char RFcarp;        	//21�����б�־    		56-56     0Ϊ���� 1Ϊ����
    unsigned char RFoperator[4]; //22����Ա        	57-60   	57-58Ϊpsam��ǰ2�ֽڣ�59-60Ϊ����Ա���
    unsigned char RFflag;        	//23����־ 	      	61-61	0.M1�� 1.PBOC2.0��׼CPU�� 2:PBOC3.0��׼CPU��  61
    unsigned char RFspare;       	//24��������      		62-62 (����:0 ����:1 �ֶ�:2 ��ʱ:3)
    unsigned char RFXor;          	//25Ч��λ ˮƽЧ��	63-63
} RecordFormat;

#else
//��¼��ʽ
typedef  struct
{
    unsigned char RFIccsn[4];    	//0оƬϵ�к���	  0 - 3
// unsigned char RFcity[2];     	//1���д���       4 - 5
//unsigned char RFapp[2];      	    //2Ӧ�ô���    	  6 - 7
    unsigned char RFDtac[4];        //������TAC       4 - 7
   unsigned char RFcsn[4];      	//3�û�����    	  8 - 11
 //   unsigned char RFcsn[8];
    unsigned char RFrove[4];     	//4Ӳ����ˮ��     12- 15
    unsigned char RFtype;        	//5����        	  16
    unsigned char RFXFtype;      	//6��������       17 -17 0x00 ��ʾ���� 01:Ԥ�� 02:��ֵ 03:ת��   04:ʱ�����   05:��������    0x55��ʾ�������
    unsigned char RFStationID;   	//7վ̨���       18 - 18
    unsigned char RFvalueq[4];   	//8����ǰ��Ƭ��� 19 - 22
//    unsigned char RFvalueq[3];
    unsigned char RFvaluej[3];   	//9ʵ�ʽ��׽��   23 - 25
    unsigned char RFtime[7];     	//10����ʱ��      26 - 32
 //   unsigned char RFtime[6];	
    unsigned char RFtran;        	//11�ն�����      33       01:���� 02:���� 03:��Ա 04:С������1 05�����⳵����   06:�ѻ���ֵ��   07:�̶����ѻ�1 08:ʱ�����ѻ�
    unsigned char RFMoneyJF[4];     //12�ܻ���        34 - 37
    unsigned char RFMoneyJFbuf[2];  //13������        38 - 39	//�������������·��
    unsigned char RFtac[4];      	//14��Ƭ��������  40 - 43
 //	unsigned char RFtac[3];
    unsigned char RFpurse;       	//15����Ǯ������  44       0.���� 1.���� 2.�ֽ�      8 ʱ�����
    unsigned char RFvalueh[4];   	//16���׺����    45 - 48
 //	unsigned char RFvalueh[3];
    unsigned char RFvaluey[3];   	//17ԭ���׽��    49 - 51
    unsigned char RFtimeno;      	//18ʱ����      52
    unsigned char RFderno[4];    	//19�ն˻���      53 - 56
    unsigned char RFEnterOrExit; 	//20������־	  57		(0��  1��ʾ��)
    unsigned char RFcarp;        	//21�����б�־    58         0Ϊ���� 1Ϊ����
    unsigned char RFoperator[2]; 	//22����Ա        59 - 60
    unsigned char RFflag;        	//23����־ 	      61		0.M1�� 1.PBOC2.0��׼CPU�� 2:PBOC3.0��׼CPU��  61
    unsigned char RFspare;       	//24��������      62 - 62 (����:0 ����:1 �ֶ�:2 ��ʱ:3)
    unsigned char RFXor;          	//25Ч��λ ˮƽЧ��63
} RecordFormat;

#endif

struct USER_RESERCH
{
	unsigned char id[50];
	unsigned char tim[30];
	unsigned char val[10];
};

struct DRIVER_SEARCH
{
	unsigned char bFuncStart;
	unsigned char bDisStart;
	unsigned int addr;
	struct timeval tim;
};


//�ýṹ�����ڲ�ˢ��ʱ��������Ļ��ȷ��ʾ
struct UesForDisplayData
{
	CardInform cardlan;
	LongUnon HostValue;
	unsigned char FlagValue;
	unsigned char bStolenDis;
	unsigned char StolenAmount;
};

struct ErrorRecordLsit
{
	time_t 	RecordTime;						//ˢ�������ʱ��
	unsigned char CardNo[4];				//����
	unsigned char SectorOneData[16];		//�����޸���1����0�������
	RecordFormat SaveRecordBuf;				//����ļ�¼����
	struct UesForDisplayData DisplayData;	//������Ļ��ȷ��ʾ������
	struct ErrorRecordLsit *next;
};

extern CardInform CardLan;
extern JackRegal Sector;
extern JackValue MoneyNo;
extern SysTime Time;
extern SectionFarPar Section,Sectionup;
extern unsigned char CardTypeIC;

extern unsigned char Save_Data_Xor(unsigned char *YDate);
extern char WaveCOmPc(unsigned char *Date);
extern unsigned char HEX2BCD(unsigned char hex_data);
extern void HEX8TOBCD(unsigned int In, unsigned char *Pdata);
extern unsigned char CheckTopUpSql(int ID);
extern char * Rd_time (char* buff);
extern char WriteTopUpSql(unsigned char *Data);

extern int hex_2_ascii(unsigned char *INdata, char *buffer, unsigned int len);
extern unsigned int myatoi(char *src);
extern void myitoa(char *s, unsigned int n);

extern void IncSerId (void);
extern void Err_display(unsigned int messcode);
extern unsigned char Card_JudgeDate(void);
extern unsigned char Permissions (unsigned char Mode);



extern unsigned char TimeRange(void);
extern unsigned char SupportType(void);
extern unsigned char AnalysisSheet(unsigned char TypeVae);
extern unsigned char YesOrNoCiShu(void);
extern unsigned char YesOrNoMoney(void);
extern unsigned char Number(unsigned int CardNumber,unsigned char cmd);

extern unsigned char CardDiscounts(unsigned char Type,unsigned char Mode);

extern unsigned char SectionSheet(void);
extern unsigned char FindUpdoorType(void);
extern unsigned int SectionDiscountRate(unsigned int val,unsigned char Type);
extern unsigned char OverTimeEnter(void);
extern unsigned char FindCardValue(unsigned int value);



extern void Read_Parameter (void);
extern char TotalAllDat (void);
extern void * ReadPara_Pthread (void * args);
extern unsigned char ReadEightDat (unsigned char *StartDat);
extern unsigned char Para_cardlan(unsigned char *buf,unsigned int Addr,unsigned char Long,unsigned char Mode);

extern unsigned char CardReset(char *data,unsigned char *plen,unsigned char type);
extern unsigned char TypeAPiccSendAPDU(unsigned char  *Send, unsigned char *Rcvdata, unsigned char Slen, unsigned int *Rlen);
extern unsigned char ReadCardInfor (void);
extern unsigned char ReadorRepairCard (void);
extern unsigned char RepairCardInfor (void);
extern unsigned char FreeReadorRepairCard(void);
extern unsigned char SectionFares(void);
extern int SqlGetNewDat (unsigned char *Data,unsigned int Dtype,unsigned char num);
//extern void ReadErrorRecordFile(int RecFd);

extern unsigned char ReadPartCardSectorData(void);
extern int SeekErrorRecordList(unsigned char *CardNo);

extern int Timer_PollErrorRecordList(void);

extern unsigned char ShowSwipeCardStatus(unsigned char type);

extern unsigned char WriteNoDiscountTime(void);//

//extern unsigned char FreeReadorRepairCard(HWND hDlg);
//extern unsigned char RepairCardInfor (HWND hDlg);
//extern unsigned char Permissions (HWND hDlg);
//extern void Err_display (HWND hDlg,unsigned int Mode);

extern void IncSerId (void);
extern void IncTransactionNum (void);
extern unsigned char DiverCard(void);



#ifdef SUPPORT_QR_CODE
extern enum OutPut_Status QRCodeProcessConsume(unsigned char freed);
#endif
//extern unsigned char TopUpCardInfor (HWND hDlg);
//extern int SqlCheckNewDat (unsigned int Dtype);
//querysql.c
//char read_card(void);




extern unsigned char ReadCardInfor_8bit(void);
extern unsigned char RepairCardInfor_8bit (void);
extern unsigned char SectionFares_8bit(void);

/*
�����Ǳ�������
�������ݽṹ��
*/

/*�Ǽ������˿�- ��ͨ��ֵƱ*/
#define  UNREG_COMMON_CARD		0x01

/*��������(˾��������ƱԱ�������ʿ����ɼ�Ա��)*/
#define  OPERATOR_CARD				0x02

#define   OPERATOR_DRIVER_CARD		0x02
#define	OPERATOR_WATCHER_CARD	0x04
#define   OPERATOR_CAPTURE_CARD	0x10


/*�������˿�- ��ͨ��ֵƱ*/
#define  REG_COMMON_CARD			0x03

/*��ֵ��*/
#define  SAME_VALUE_CARD			0x04

/*������*/
#define  WELFARE_CARD				0x05

/*���*/
#define  SOUVENIR_CARD				0x06

/*��ҵԱ����*/
#define  EMPLOYEE_CARD				0x07

/*���˿�*/
#define  OLDMAN_CARD				0x10

/*��Ǯ������ר�ÿ�*/
#define  BIG_TEST_CARD				0x11

/*ѧ����1*/
#define  STUDENT_ONE_CARD			0x12
/*ѧ����2*/
#define  STUDENT_TWO_CARD			0x13
/*ѧ����3*/
#define  STUDENT_THREE_CARD		0x14


/*�������ڼƴο�*/
#define  METRO_COUNT_CARD			0x20

/*����Ա����*/
#define  METRO_EMPLOYEE_CARD		0x22

/*��վ������*/
#define  STATION_CARD				0x24

/*���⳵˾����*/
#define  TAXI_DRIVER_CARD			0x40

/*����ר�ÿ�1*/
#define OLYMPIC_ONE_CARD			0xA1
/*����ר�ÿ�2*/
#define OLYMPIC_TWO_CARD			0xA2

#define CARD_NUMBER				19


/*��ȡ����*/
#define SC_ZERO		0x01
#define SC_ONE		0x02
#define SC_TWO		0x04


/*�û���*/
//Ӧ�ñ�ʶ��
struct User_Sector_Zero {
		unsigned char VendorSpace[16];  //����ռ�� (��CSN)  HEX
		
		unsigned char Version; //0x30 means 3.0 0x31 means 3.1 ���а汾  HEX
		unsigned char DirZoom1[15];  //Ŀ¼��1   HEX
		
		unsigned char DirZoom2[11]; //Ŀ¼��1   HEX
		unsigned char RegisterNum[5]; //����ע�ᷢ�к�  91 56 00 00 14   HEX
};

//������
struct User_Sector_One {
		unsigned char CityNum[2]; //BCD
		unsigned char TradeNum[2]; //��ҵ���� BCD
		unsigned char IssueNum[4]; //������ˮ�� BCD
		unsigned char AuthCode[4]; //����֤��  HEX
		unsigned char CardType; // ���� HEX
		unsigned char CardStatus; //��״̬ HEX  (01 һ�η��С�δ��ֵ��δ���ã�02 ���ã�03 ͣ�ã�04 �˻ؿ������ؿ�������ֵΪ�Ƿ���) ��02 Ϊ�Ƿ���
		unsigned char Reserve[2];

		unsigned char IDType; //֤������  BCD ( 00 ���֤��01 ����֤��02 ���գ�03 �뾳֤��04 ��ʱ���֤)
		unsigned char IDCode[9]; // ֤������ BCD
		unsigned char Pin[3]; // �������� HEX
		unsigned char OtherUse[3]; //����Ӧ��

		unsigned char IssueDate[4]; //�������� BCD  YYYYMMDD
		unsigned char InvalidDate[4];  //ʧЧ���� BCD
		unsigned char OperEncode[3];  //Ԥ�� ��Ӫ��λ����� BCD
		unsigned char Reserve1[2]; //Ԥ�� and ����Ӧ��
		unsigned char CardCost[2];  //���ɱ� HEX
		unsigned char CountFlag; //�ƴα�ʶ HEX

};

//Ǯ����
struct User_Sector_Two {
		unsigned char Reserve1[4];
		unsigned char Balance[3];  // HEX ���
		unsigned char Reserve2[9];

		unsigned char Wallet1[4]; // Ǯ�� HEX
		unsigned char AgaWallet[4]; // Ǯ��ȡ�� HEX
		unsigned char Wallet2[4];  // Ǯ�� HEX
		unsigned char Addr[4]; // Ǯ����ַ HEX

		unsigned char Wallet1Bak[4]; //���� Ǯ�� HEX 
		unsigned char AgaWalletBak[4];
		unsigned char Wallet2Bak[4];
		unsigned char AddrBak[4];
};
// ����ר�ÿ���/���ڼƴο�
struct User_Sector_Four {
		unsigned char VaildDate[3];  //��Ч���� BCD  YYMMDD
		unsigned char InvaildDate[3]; // ʧЧ���� BCD 
		unsigned char TopupPara;  // ��ֵ���� HEX (bit3~0 ��λ������bit7~4Ϊ���� 1 ��Ʊ��2��Ʊ��3ѮƱ��4��Ʊ��5��Ʊ)
		unsigned char MonCount;  // ���¶  ÿ�¸���Ĵ��� HEX
		unsigned char IssueDevCode[3]; //ISAM ���ź�3�ֽ� BCD
		unsigned char MonValue[2];  //���½�� HEX
		unsigned char OperCode[3]; //��Ӫ�̴��� BCD

		unsigned char LitWallet1[4]; // СǮ�� HEX
		unsigned char LitAgaWallet[4];
		unsigned char LitWallet2[4];
		unsigned char LitAddr[4];

		unsigned char LitWallet1Bak[4];  //СǮ������ HEX
		unsigned char LitAgaWalletBak[4];
		unsigned char LitWallet2Bak[4];
		unsigned char LitAddrBak[4];
};

// ������Ϣ��

struct User_Sector_Five {
		unsigned char BusRegFlag;  //�������̼�¼ָ�� HEX
		unsigned char BlackCardFlag; // ����������־ HEX  (0��������A5��������������Ϊ�Ƿ���)
		unsigned char TransTime[5];  //���ν���ʱ�� HEX ? 
		unsigned char TransRegFlag; //���ѽ��׼�¼ָ�� HEX
		unsigned char Reserve;  //����
		unsigned char TransCntBak[4]; //���ۼƽ��״������� HEX
		unsigned char OperCode[3];  //��Ӫ�̴��� BCD

	//	unsigned char Reserve[16]; //Ԥ��

		unsigned char TransCnt1[4]; //���ۼƽ��״��� HEX
		unsigned char AgaTransCnt[4]; //���ۼƽ��״�������
		unsigned char TransCnt2[4]; //���ۼƽ��״���
		unsigned char Addr[4]; //���ۼƽ��״�����ַ
};

struct User_Sector_Six {
		unsigned char GetOnTime[5];  //�ϳ�ʱ�� YYMMDDHHMM  BCD
		unsigned char GetOnStation; // ��¼�ϳ�վ��վ��  BCD
		unsigned char DirFlag;  //�����ʶ HEX    AB ����      BA  ����
		unsigned char OnTEndVal[2]; //�ϳ�����վ�յ� ���  HEX
		unsigned char Reserve[2];  //����
		unsigned char RoadNum[2]; //��·�� HEX
		unsigned char BusNum[3];  //������ BCD

		unsigned char GetOnBusStatus[16];

	//	unsigned char Reserve[16]; //Ԥ��

};

//����8 0blockԤ��,1,2 blcok  ������9  0,1,2 block ���Ǽ�¼
struct User_Trans_Record{
		unsigned char TransTime[5];  //����ʱ�� YYMMDDHHMM  BCD
		unsigned char OldMoney[3]; //���ν���ǰ��Ǯ�����   HEX 
		unsigned char TransMoney[2];  //���ν��׽��  HEX 
		unsigned char TransType; // ��������  HEX
		unsigned char SystemCode;  //ϵͳ����  HEX    01 �ǹ���  ��������
		unsigned char SystemInfo[4]; //ϵͳ��Ϣ �����ֶ� HEX  2bytes ��·�ţ��ϳ�վ�� 1byte + �³�վ�� 1byte
};

// Ա����/���� ����15
struct Oper_Sector_Fifteen {
		unsigned char EmployeeCode; //Ա������ҵ���  HEX 
		unsigned char SelfInfo1[15];  //������Ϣ��1

		unsigned char SelfInfo2[16];  //������Ϣ��2
};

struct Oper_Sector_One {
		unsigned char CityNum[2]; //BCD
		unsigned char TradeNum[2]; //��ҵ���� BCD
		unsigned char IssueNum[4]; //������ˮ�� BCD
		unsigned char AuthCode[4]; //����֤��  HEX
		unsigned char CardType; // ���� HEX
		unsigned char CardStatus; //��״̬ HEX  (01 һ�η��С�δ��ֵ��δ���ã�02 ���ã�03 ͣ�ã�04 �˻ؿ������ؿ�������ֵΪ�Ƿ���) ��02 Ϊ�Ƿ���
		unsigned char Reserve[2];

		unsigned char OReserve[16];

		unsigned char IssueDate[4]; //�������� BCD  YYYYMMDD
		unsigned char InvalidDate[4];  //ʧЧ���� BCD
		unsigned char OperEncode[2];  //�������������� BCD
		unsigned char OperID[2]; // ��������������ԱID BCD
		unsigned char CardCost[2];  //���ɱ� HEX
		unsigned char Reserve1[2]; //����

};	

struct Oper_Sector_Two {
		unsigned char OperType; //������ HEX 02 ˾���� 04 �Ǽ�ƱԱ  10 �ǲɼ���  HEX 
		unsigned char Reserve1[7];
		unsigned char DriverID[4];  //	˾����ID��  BCD
		unsigned char OperCodeL[3];  // ��Ӫ�ֹ�˾����  BCD
		unsigned char Reserve2;
};	

//�ɼ���
struct Capture_Sector_Two {
		unsigned char OperType; //������ HEX 02 ˾���� 04 �Ǽ�ƱԱ  10 �ǲɼ���  HEX 
		unsigned char Reserve1;
		unsigned char CapCentCode[2]; // �ռ������Ĵ���  BCD
		unsigned char CapID[2];  //�ɼ���ID  BCD
		unsigned char Reserve2[4];
		unsigned char OperCode[3]; //��Ӫ�ֹ�˾����  BCD
		unsigned char Reserve3[3];
};	

struct UserCard_Struct {
	struct User_Sector_Zero ScZero;
	struct User_Sector_One ScOne;
	struct User_Sector_Two ScTwo;
	struct User_Sector_Four ScFour;
	struct User_Sector_Five ScFive;
	struct User_Sector_Six ScSix;
	struct User_Trans_Record TsRecord;
};
extern struct UserCard_Struct UserCard;
struct UserCard_Key {
	unsigned char ScOne[6];
	unsigned char ScTwo[6];
	unsigned char ScFour[6];
	unsigned char ScFive[6];
	unsigned char ScSix[6];
	unsigned char ScEigth[6];
	unsigned char ScNine[6];
	unsigned char ScFifteen[6];
};

struct OperatCard_Struct {
	struct User_Sector_Zero OScZero;
	struct Oper_Sector_One OScOne;
	union {
		struct Oper_Sector_Two OScTwo;
		struct Capture_Sector_Two CScTwo;  //�ɼ���
	}Un;
	struct User_Sector_Five OScFive;
	//struct Oper_Sector_Fifteen OscFifteen;
};


struct GetOnOff_Struct{
	unsigned char On_StationNo;
	ShortUnon     On_Mileage;
	unsigned char On_Name[19];
	unsigned char Off_StationNo;
	ShortUnon     Off_Mileage;
	unsigned char Off_Name[19];
	
	ShortUnon	  Real_Mileage;

	ShortUnon	  Version; //M3�ļ��汾��

};

extern struct GetOnOff_Struct GetOnOffInfo;

#endif
