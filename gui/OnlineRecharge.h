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

#define 	EXT_KEY_ADDR		0
#define		BUS_DEV1_ADDR		1
#define		BUS_DEV2_ADDR		2
#define		BUS_DEV3_ADDR		3
#define		BUS_GPS_ADDR		0x0F

#define		DEV_TYPE_KEY		0
#define		DEV_TYPE_POS		1
#define		DEV_TYPE_GPS		0X0F




#define		CMD_FLAG_CH			'@'


/*�豸ѯ��*/
#define		CMD_FUNC_REQ		"00"


/*��ѯ/���õ�ǰվ̨���ڷ���*/
#define		CMD_FUNC_DIR_NO		"01"

/*��ѯ/���û�������*/
#define		CMD_FUNC_DATE		"02"

/*��ѯ/���û���ʱ��*/
#define		CMD_FUNC_TIME		"03"

/*��ѯ/���û���ʱ��������*/
#define		CMD_FUNC_TIME_DATE		"08"

/*��ѯ/���û�����·��*/
#define		CMD_FUNC_ROAD_NO		"10"

/*�����ն˻���������*/
#define		CMD_FUNC_VOL_UP		"18"

/*�����ն˻�������С*/
#define		CMD_FUNC_VOL_DOWN		"19"

/*����ն˻��������*/
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
