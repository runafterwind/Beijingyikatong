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


/*站点信息，用于站点记录文件*/
#if RUSHAN_BUS
struct STATION_INFO_			//24个字节大小
{
	unsigned short infortype;					//可表示是正常的还是异常的，未定义
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
#define TASKTATOL   15  //下载参数选项个数
typedef  struct
{
    unsigned char Parameters; 	//下载参数
    unsigned char Blacklist;	//下载黑名单
    unsigned char PrintfStart;	//打印头
    unsigned char PrintfEnd;	//打印尾
    unsigned char Collection;	//采集数据
    unsigned char Sound;		//更新语音
    unsigned char RmAll;		//格式化数据
    unsigned char Upgrade;		//升级
    unsigned char SetSector;	//设置扇区
    unsigned char SetDev;		//设置机号
    unsigned char Section;		//分段收费
    unsigned char Sectionup;	//分段收费
//在此增加项
    unsigned char nop; 			//空
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
    unsigned char FlagValue;			//钱包指示:现金钱包
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
    unsigned  char SationNum[2];			  //站台个数
    unsigned  char DeductMoney[4]; 			  //预扣金额
    unsigned  char DeductTime[2];			  //上限时间
    unsigned  char Enable;          //启用标志
    unsigned  char Enableup;        //启用标志
    unsigned  char StationOn;
    unsigned  char Updoor;          //前后门，上下定义
    unsigned  char Updown;          //上下行定义
	unsigned  char SationNow;
	unsigned  char Sationdis;		//站号
	unsigned  char Sationkey;
	unsigned  char Linenum[2];			//线路号,高字节在前，低字节在后
}SectionFarPar;


typedef  struct
{
    unsigned char  IncDecFlag;		//预扣/补回
    unsigned char  Money[3];		//预扣总金额
    unsigned char  TimeBuf[2];		//预扣时间
} FixTimerBuf;


//缓存区
typedef  struct
{
    unsigned char UserIcNo[4];     //开始读
    unsigned char MonthOrCountflag;//包月/包次标志           HEX  OK
    unsigned char SMonth[3];       //启用包月  年月日                      HEX  OK
    unsigned char EMonth[3];       //结束包月  年月日                      HEX  OK
    unsigned char CityId[2];       //城市代码 	HEX  OK
    unsigned char AppId[2];        //应用代码 	HEX  OK
    unsigned char UserNum[2];      //用户编号 	HEX  OK
    unsigned char CardCsnB[4];     //卡号 		BCD  OK
    unsigned char CardCsnB_blk[8];
    unsigned char CardId[4];       //卡认证号 	HEX  OK
    unsigned char CardType;        //卡类 		HEX  OK
    unsigned char CardGroup;        //卡类 		HEX  OK
    unsigned char Pwdflag;         //密码启用标志	HEX  OK
    unsigned char EnableH[4];      //启用日期 	HEX  OK
    unsigned char Effective[4];    //有效日期	HEX  OK
    unsigned char UserWord[3];     //用户密码 	BCD  OK
    unsigned char Views[4];        //次数钱包	HEX  OK
    unsigned char Subsidies[4];    //补贴钱包 	HEX  OK
    unsigned char QCash[4];        //现金钱包	HEX  OK
    unsigned char MoneyJack[4];    //积分区域	HEX  OK
    unsigned char ViewsValue[3];   //当次消费金额 	HEX  OK
    unsigned char DayValue[3];     //当天消费金额 	HEX  OK
    unsigned char MonthValue[4];   //当月消费金额 	HEX  OK
    unsigned char Period;          //上次交易的时段 	HEX  OK
    unsigned char OldTime[6];      //上次交易时间   	HEX  OK
    unsigned char OldTransType;    //上次交易类型    	HEX  OK
    unsigned char OldTermNo[4];    //上次交易终端机号 	HEX  OK
    unsigned char ViewMoney[4];    //钱包累计交易次数     HEX  OK
    unsigned char EnterExitFlag;   //进出标志                      HEX  OK
    unsigned char StationID;       // 站台编号                     HEX  OK
    unsigned char EnterCarCi;      // 上车人数                   HEX  OK
    unsigned char StationOn;	   // 限次限额标准
    unsigned char StationDEC;	   // 限次限额标准   
    unsigned char CardTac[4];	   // CPU卡返回TAC
    unsigned char CiMoneyFlag;	   // 限次限额标准
    unsigned char TimesTransfer[4];
    unsigned char TransCiFlag;
    unsigned char NoDiscountTime[6]; //非转乘优惠打卡所记录的时间
    unsigned char FirstTimeDiscount[6];//记录第一次在转乘优惠区间内打折的时间，临时设置2分钟一次循环，可修改
    //住建部CPU卡普通文件结构//
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
    unsigned char RFIccsn[4];    	//0芯片系列号码	  	0-3
    unsigned char RFDtac[4];        //数据区TAC       		4-7
    unsigned char RFcsn[8];      	//用户卡号    	  	8-15
    unsigned char RFrove[4];    	//硬件流水号     		16-19  若是CPU卡，则为psam卡交易序号
    unsigned char RFtype;        	//5卡类        	  	20-20
    unsigned char RFXFtype;      	//6消费类型       		21-21 0x00 表示数据 01:预冲 02:冲值 03:转移   04:时间包月   05:次数包月    0x55表示错误代码
    unsigned char RFStationID;   	//7站台编号       		22-22
    unsigned char RFvalueq[3];   	//8交易前卡片金额 		23-25
    unsigned char RFvaluej[3];   	//9实际交易金额   		26-28
    unsigned char RFtime[4];     	//10交易时间(UTC秒)    29-32
    unsigned char RFtran;        	//11终端类型      		33-33   01:公交 02:餐饮 03:会员 04:小额消费1 05：出租车消费   06:脱机充值机   07:固定消费机1 08:时间消费机
    unsigned char RFMoneyJF[4];     //12总积分        	34-37	用作发卡机构标示
    unsigned char RFMoneyJFbuf[2];  //13积分数        	38-39	用作城市代码
    unsigned char RFtac[3];      	//14卡片操作次数  		40-42
    unsigned char RFpurse;       	//15交易钱包类型  		43-43  0.次数 1.补贴 2.现金      8 时间包月
    unsigned char RFvalueh[3];   	//16交易后余额    		44-46
    unsigned char RFvaluey[3];   	//17原交易金额    		47-49
    unsigned char RFtimeno;      	//18时间编号      		50-50
    unsigned char RFderno[4];    	//19终端机号      		51-54
    unsigned char RFEnterOrExit; //20进出标志	  		55-55(0进  1表示出)
    unsigned char RFcarp;        	//21上下行标志    		56-56     0为上行 1为下行
    unsigned char RFoperator[4]; //22操作员        	57-60   	57-58为psam卡前2字节，59-60为操作员编号
    unsigned char RFflag;        	//23卡标志 	      	61-61	0.M1卡 1.PBOC2.0标准CPU卡 2:PBOC3.0标准CPU卡  61
    unsigned char RFspare;       	//24交易类型      		62-62 (定额:0 自由:1 分段:2 计时:3)
    unsigned char RFXor;          	//25效验位 水平效验	63-63
} RecordFormat;

#else
//记录格式
typedef  struct
{
    unsigned char RFIccsn[4];    	//0芯片系列号码	  0 - 3
// unsigned char RFcity[2];     	//1城市代码       4 - 5
//unsigned char RFapp[2];      	    //2应用代码    	  6 - 7
    unsigned char RFDtac[4];        //数据区TAC       4 - 7
   unsigned char RFcsn[4];      	//3用户卡号    	  8 - 11
 //   unsigned char RFcsn[8];
    unsigned char RFrove[4];     	//4硬件流水号     12- 15
    unsigned char RFtype;        	//5卡类        	  16
    unsigned char RFXFtype;      	//6消费类型       17 -17 0x00 表示数据 01:预冲 02:冲值 03:转移   04:时间包月   05:次数包月    0x55表示错误代码
    unsigned char RFStationID;   	//7站台编号       18 - 18
    unsigned char RFvalueq[4];   	//8交易前卡片金额 19 - 22
//    unsigned char RFvalueq[3];
    unsigned char RFvaluej[3];   	//9实际交易金额   23 - 25
    unsigned char RFtime[7];     	//10交易时间      26 - 32
 //   unsigned char RFtime[6];	
    unsigned char RFtran;        	//11终端类型      33       01:公交 02:餐饮 03:会员 04:小额消费1 05：出租车消费   06:脱机充值机   07:固定消费机1 08:时间消费机
    unsigned char RFMoneyJF[4];     //12总积分        34 - 37
    unsigned char RFMoneyJFbuf[2];  //13积分数        38 - 39	//公交用来存放线路号
    unsigned char RFtac[4];      	//14卡片操作次数  40 - 43
 //	unsigned char RFtac[3];
    unsigned char RFpurse;       	//15交易钱包类型  44       0.次数 1.补贴 2.现金      8 时间包月
    unsigned char RFvalueh[4];   	//16交易后余额    45 - 48
 //	unsigned char RFvalueh[3];
    unsigned char RFvaluey[3];   	//17原交易金额    49 - 51
    unsigned char RFtimeno;      	//18时间编号      52
    unsigned char RFderno[4];    	//19终端机号      53 - 56
    unsigned char RFEnterOrExit; 	//20进出标志	  57		(0进  1表示出)
    unsigned char RFcarp;        	//21上下行标志    58         0为上行 1为下行
    unsigned char RFoperator[2]; 	//22操作员        59 - 60
    unsigned char RFflag;        	//23卡标志 	      61		0.M1卡 1.PBOC2.0标准CPU卡 2:PBOC3.0标准CPU卡  61
    unsigned char RFspare;       	//24交易类型      62 - 62 (定额:0 自由:1 分段:2 计时:3)
    unsigned char RFXor;          	//25效验位 水平效验63
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


//该结构体是在补刷的时候用于屏幕正确显示
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
	time_t 	RecordTime;						//刷卡出错的时间
	unsigned char CardNo[4];				//卡号
	unsigned char SectorOneData[16];		//用于修复第1扇区0块的数据
	RecordFormat SaveRecordBuf;				//保存的记录数据
	struct UesForDisplayData DisplayData;	//用于屏幕正确显示的数据
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
以下是北京公交
卡的数据结构。
*/

/*非记名成人卡- 普通储值票*/
#define  UNREG_COMMON_CARD		0x01

/*公交管理卡(司机卡、监票员卡、费率卡、采集员卡)*/
#define  OPERATOR_CARD				0x02

#define   OPERATOR_DRIVER_CARD		0x02
#define	OPERATOR_WATCHER_CARD	0x04
#define   OPERATOR_CAPTURE_CARD	0x10


/*记名成人卡- 普通储值票*/
#define  REG_COMMON_CARD			0x03

/*定值卡*/
#define  SAME_VALUE_CARD			0x04

/*福利卡*/
#define  WELFARE_CARD				0x05

/*纪念卡*/
#define  SOUVENIR_CARD				0x06

/*企业员工卡*/
#define  EMPLOYEE_CARD				0x07

/*老人卡*/
#define  OLDMAN_CARD				0x10

/*大钱包测试专用卡*/
#define  BIG_TEST_CARD				0x11

/*学生卡1*/
#define  STUDENT_ONE_CARD			0x12
/*学生卡2*/
#define  STUDENT_TWO_CARD			0x13
/*学生卡3*/
#define  STUDENT_THREE_CARD		0x14


/*地铁定期计次卡*/
#define  METRO_COUNT_CARD			0x20

/*地铁员工卡*/
#define  METRO_EMPLOYEE_CARD		0x22

/*车站工作卡*/
#define  STATION_CARD				0x24

/*出租车司机卡*/
#define  TAXI_DRIVER_CARD			0x40

/*奥运专用卡1*/
#define OLYMPIC_ONE_CARD			0xA1
/*奥运专用卡2*/
#define OLYMPIC_TWO_CARD			0xA2

#define CARD_NUMBER				19

#define OPERATOR_MODE			1
#define USECARD_MODE			2



/*读取扇区*/
#define SC_ZERO		0x01
#define SC_ONE		0x02
#define SC_TWO		0x04


/*用户卡*/
//应用标识区
struct User_Sector_Zero {
		unsigned char VendorSpace[16];  //厂商占用 (含CSN)  HEX
		
		unsigned char Version; //0x30 means 3.0 0x31 means 3.1 发行版本  HEX
		unsigned char DirZoom1[15];  //目录区1   HEX
		
		unsigned char DirZoom2[11]; //目录区1   HEX
		unsigned char RegisterNum[5]; //国家注册发行号  91 56 00 00 14   HEX
};

//发行区
struct User_Sector_One {
		unsigned char CityNum[2]; //BCD
		unsigned char TradeNum[2]; //行业代码 BCD
		unsigned char IssueNum[4]; //发行流水号 BCD
		unsigned char AuthCode[4]; //卡认证码  HEX
		unsigned char CardType; // 卡类 HEX
		unsigned char CardStatus; //卡状态 HEX  (01 一次发行、未充值、未启用；02 启用；03 停用；04 退回卡、换回卡；其他值为非法卡) 非02 为非法卡
		unsigned char Reserve[2];

		unsigned char IDType; //证件类型  BCD ( 00 身份证；01 军官证；02 护照；03 入境证；04 临时身份证)
		unsigned char IDCode[9]; // 证件号码 BCD
		unsigned char Pin[3]; // 个人密码 HEX
		unsigned char OtherUse[3]; //其他应用

		unsigned char IssueDate[4]; //发行日期 BCD  YYYYMMDD
		unsigned char InvalidDate[4];  //失效日期 BCD
		unsigned char OperEncode[3];  //预留 运营单位编码表 BCD
		unsigned char Reserve1[2]; //预留 and 其他应用
		unsigned char CardCost[2];  //卡成本 HEX
		unsigned char CountFlag; //计次标识 HEX

};

//钱包区
struct User_Sector_Two {
		unsigned char Reserve1[4];
		unsigned char Balance[3];  // HEX 余额
		unsigned char Reserve2[9];

		unsigned char Wallet1[4]; // 钱包 HEX
		unsigned char AgaWallet[4]; // 钱包取反 HEX
		unsigned char Wallet2[4];  // 钱包 HEX
		unsigned char Addr[4]; // 钱包地址 HEX

		unsigned char Wallet1Bak[4]; //备份 钱包 HEX 
		unsigned char AgaWalletBak[4];
		unsigned char Wallet2Bak[4];
		unsigned char AddrBak[4];
};
// 公交专用卡区/定期计次卡
struct User_Sector_Four {
		unsigned char VaildDate[3];  //生效日期 BCD  YYMMDD
		unsigned char InvaildDate[3]; // 失效日期 BCD 
		unsigned char TopupPara;  // 充值参数 HEX (bit3~0 单位个数，bit7~4为类型 1 月票，2周票，3旬票，4季票，5年票)
		unsigned char MonCount;  // 单月额定  每月给予的次数 HEX
		unsigned char IssueDevCode[3]; //ISAM 卡号后3字节 BCD
		unsigned char MonValue[2];  //单月金额 HEX
		unsigned char OperCode[3]; //运营商代码 BCD

		unsigned char LitWallet1[4]; // 小钱包 HEX
		unsigned char LitAgaWallet[4];
		unsigned char LitWallet2[4];
		unsigned char LitAddr[4];

		unsigned char LitWallet1Bak[4];  //小钱包备份 HEX
		unsigned char LitAgaWalletBak[4];
		unsigned char LitWallet2Bak[4];
		unsigned char LitAddrBak[4];
};

// 公共信息区

struct User_Sector_Five {
		unsigned char BusRegFlag;  //公交过程记录指针 HEX
		unsigned char BlackCardFlag; // 黑名单卡标志 HEX  (0正常卡，A5黑名单卡，其他为非法卡)
		unsigned char TransTime[5];  //本次交易时间 HEX ? 
		unsigned char TransRegFlag; //消费交易记录指针 HEX
		unsigned char Reserve;  //保留
		unsigned char TransCntBak[4]; //卡累计交易次数备份 HEX
		unsigned char OperCode[3];  //运营商代码 BCD

	//	unsigned char Reserve[16]; //预留

		unsigned char TransCnt1[4]; //卡累计交易次数 HEX
		unsigned char AgaTransCnt[4]; //卡累计交易次数反码
		unsigned char TransCnt2[4]; //卡累计交易次数
		unsigned char Addr[4]; //卡累计交易次数地址
};

struct User_Sector_Six {
		unsigned char GetOnTime[5];  //上车时间 YYMMDDHHMM  BCD
		unsigned char GetOnStation; // 记录上车站的站号  BCD
		unsigned char DirFlag;  //方向标识 HEX    AB 上行      BA  下行
		unsigned char OnTEndVal[2]; //上车到车站终点 金额  HEX
		unsigned char Reserve[2];  //保留
		unsigned char RoadNum[2]; //线路号 HEX
		unsigned char BusNum[3];  //车辆号 BCD

		unsigned char GetOnBusStatus[16];

	//	unsigned char Reserve[16]; //预留

};

//扇区8 0block预留,1,2 blcok  与扇区9  0,1,2 block 都是记录
struct User_Trans_Record{
		unsigned char TransTime[5];  //交易时间 YYMMDDHHMM  BCD
		unsigned char OldMoney[3]; //本次交易前的钱包余额   HEX 
		unsigned char TransMoney[2];  //本次交易金额  HEX 
		unsigned char TransType; // 交易类型  HEX
		unsigned char SystemCode;  //系统代码  HEX    01 是公交  。。。。
		unsigned char SystemInfo[4]; //系统信息 复用字段 HEX  2bytes 线路号，上车站标 1byte + 下车站标 1byte
};

// 员工卡/管理卡 扇区15
struct Oper_Sector_Fifteen {
		unsigned char EmployeeCode; //员工卡行业编号  HEX 
		unsigned char SelfInfo1[15];  //复用信息段1

		unsigned char SelfInfo2[16];  //复用信息段2
};

struct Oper_Sector_One {
		unsigned char CityNum[2]; //BCD
		unsigned char TradeNum[2]; //行业代码 BCD
		unsigned char IssueNum[4]; //发行流水号 BCD
		unsigned char AuthCode[4]; //卡认证码  HEX
		unsigned char CardType; // 卡类 HEX
		unsigned char CardStatus; //卡状态 HEX  (01 一次发行、未充值、未启用；02 启用；03 停用；04 退回卡、换回卡；其他值为非法卡) 非02 为非法卡
		unsigned char Reserve[2];

		unsigned char OReserve[16];

		unsigned char IssueDate[4]; //发行日期 BCD  YYYYMMDD
		unsigned char InvalidDate[4];  //失效日期 BCD
		unsigned char OperEncode[2];  //管理卡发卡机代码 BCD
		unsigned char OperID[2]; // 管理卡发卡机操作员ID BCD
		unsigned char CardCost[2];  //卡成本 HEX
		unsigned char Reserve1[2]; //保留

};	

struct Oper_Sector_Two {
		unsigned char OperType; //管理卡类 HEX 02 司机卡 04 是监票员  10 是采集卡  HEX 
		unsigned char Reserve1[7];
		unsigned char DriverID[4];  //	司机卡ID吗  BCD
		unsigned char OperCodeL[3];  // 运营分公司代码  BCD
		unsigned char Reserve2;
};	

//采集卡
struct Capture_Sector_Two {
		unsigned char OperType; //管理卡类 HEX 02 司机卡 04 是监票员  10 是采集卡  HEX 
		unsigned char Reserve1;
		unsigned char CapCentCode[2]; // 收集分中心代码  BCD
		unsigned char CapID[2];  //采集卡ID  BCD
		unsigned char Reserve2[4];
		unsigned char OperCode[3]; //运营分公司代码  BCD
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
		struct Capture_Sector_Two CScTwo;  //采集卡
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

	ShortUnon	  Version; //M3文件版本号

};

extern struct GetOnOff_Struct GetOnOffInfo;

#endif
