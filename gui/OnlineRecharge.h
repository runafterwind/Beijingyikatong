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
	unsigned char head[2];			/* ��Ϣͷ��־λ */
	unsigned char In_Out_station;	/* ����վ��־ */
	unsigned char StationNum;		/* վ̨��� */
	unsigned char PathNum;			/* ·����� */
	unsigned char SectionParUp;		/* �����б�־ */
	unsigned char CheakCode_H;		/* ���ݰ�У����ֽ� */
	unsigned char CheakCode_L;		/* ���ݰ�У����ֽ�*/
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
