#ifndef _ONLINERECHARGE_H_
#define _ONLINERECHARGE_H_

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
#include "RC500.h"


struct DALI_TransferMsg {
	unsigned char head[2];			/* 消息头标志位 */
	unsigned char In_Out_station;	/* 进出站标志 */
	unsigned char StationNum;		/* 站台编号 */
	unsigned char PathNum;			/* 路径编号 */
	unsigned char SectionParUp;		/* 上下行标志 */
	unsigned char CheakCode_H;		/* 数据包校验高字节 */
	unsigned char CheakCode_L;		/* 数据包校验低字节*/
};

#define SAVE_STATION_NUM_FILE		"stationNum.txt"

#define 	EXT_KEY_ADDR		0
#define		BUS_DEV1_ADDR		1
#define		BUS_DEV2_ADDR		2
#define		BUS_DEV3_ADDR		3
#define		BUS_GPS_ADDR		0x0F

#define		DEV_TYPE_KEY		0
#define		DEV_TYPE_POS		1
#define		DEV_TYPE_GPS		0X0F




#define		CMD_FLAG_CH			'@'


/*设备询问*/
#define		CMD_FUNC_REQ		"00"


/*查询/设置当前站台号于方向*/
#define		CMD_FUNC_DIR_NO		"01"

/*查询/设置机器日期*/
#define		CMD_FUNC_DATE		"02"

/*查询/设置机器时间*/
#define		CMD_FUNC_TIME		"03"

/*查询/设置机器时间与日期*/
#define		CMD_FUNC_TIME_DATE		"08"

/*查询/设置机器线路号*/
#define		CMD_FUNC_ROAD_NO		"10"

/*设置终端机音量增大*/
#define		CMD_FUNC_VOL_UP		"18"

/*设置终端机音量减小*/
#define		CMD_FUNC_VOL_DOWN		"19"

/*检查终端机各项参数*/
#define		CMD_FUNC_CHECK_PARAM		"68"


extern unsigned char SectionSta(unsigned char srct,unsigned char type);
extern void SectionApp(void);
extern void ReportCardType();
extern unsigned char ReadIPCard (char type);
extern char Savedatasql(RecordFormat Save,int flag,int tao) ;
extern void PrintferInformation (RecordFormat Save);
extern unsigned int myatoi(char *src);
extern void MoneyValue(unsigned char *OUT_Data,unsigned int Money);
extern unsigned char InitData(void);
extern unsigned char CheckTime(void);
extern void Freedom(void);
extern void UPdata_usb(void);
extern void SaveDirverNumber(void);
extern unsigned char ReturnDisplay(int messcode);

#endif
