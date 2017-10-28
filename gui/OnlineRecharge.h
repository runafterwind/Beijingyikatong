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
