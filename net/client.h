#ifndef __CLIENT__H__
#define __CLIENT__H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>

#include <math.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
//#include <popt.h>
#include <ctype.h>
#include <assert.h>

#include "savesql.h"


#define TEST 1


#define MISS_SEND_HEART 0x3001
#define MISS_SEND_A2	0x3002
#define MISS_REQUEST_FIX	0x3004
#define MISS_DOWNLOAD_FIX	0x3005
#define MISS_REQUEST_MCH	0x3007
#define MISS_REQUEST_FILE_DESINFO	0x300A
#define MISS_DOWNLOAD_FILE		0X300B	
#define MISS_SEND_FILE_INFO		0x300C

#define CLIENT_VER "CL-2017.10.10-10:32"

#define MAX_TRANSFER  4096


/*	��������Ϣ�������ݰ������̶��ֶεĳ��� */
#define SYN_INFO_LEN	2
#define COMPRESS_LEN	1
#define MSG_TYPE_LEN	2
#define VERSION_LEN		1
#define POS_ID_LEN		4
#define CRC_LEN			2

/* ������Ϣ��ͬ����Ϣ���ֹ涨 */
#define SEND_HEART_SYN  	0x3001
#define SEND_A2_SYN		0x3002
#define SEND_REQUEST_FIX_SYN 	0x3004
#define SEND_DOWNLOAD_FIX_SYN	0x3005
#define SEND_REQUEST_MCH_SYN 0X3007
#define SEND_REQUEST_FILE_DES_SYN 0x300A
#define SEND_DOWNLOAD_FILE_SYN   0x300B
#define SEND_FILE_INFO_SYN		0x300C

typedef struct device_info_{
	unsigned char posid[4];		//bcd �豸id
	unsigned char Ver;			//hex �汾

	unsigned char FirVer[2];		//  bcd  : �̼��汾
	unsigned char TerParVer[2];				// -		  	
	unsigned char Line[2];						//		 ��·	
	unsigned char  BlkParVer[4];					//		 �����������汾	
	unsigned char CatpParVer[4];					// -		 ���ÿ����Ͳ����汾
	unsigned char MchCode[2];					// -		 �̻�����		
	unsigned char SamID[6];			// - bcd   SAM����
	unsigned char LinVer[2];		// 	bcd	 ��·վ����Ϣ�汾
	unsigned char InCarBlkParVer[4];				//		 ʵ�忨�����������汾
	unsigned char QRBlkVer[4];					// -		��ά��������汾
	unsigned char InQRBlkParVer[4];				//		��ά�������������汾
	unsigned char HTBlkParVer[4];				//		��ͨ�������汾
	unsigned char HTInBlkParVer[4];				//		��ͨ�����������汾
	unsigned char PriParVer[2];		//		Ʊ�۱��ļ��汾
	unsigned char MilParVer[2];		//		������ļ��汾
	unsigned char HTtwoRelParVer[2];	//	���ʿ����η�����Ϣ
	unsigned char HTWriPar[2];			//	���л�ͨ�������汾
	
}device_info;


/**/


/* ����/�������ѵĽṹ */
typedef struct message_{
	/* head */
	unsigned char len[2];		//bcd
	unsigned char syncinfo[2];	//hex
	unsigned char compress;		//hex
	/*body */
	unsigned char  type[2];		//bcd
	unsigned char  ver;			//hex
	unsigned char posid[4];		//bcd 
	unsigned char data[MAX_TRANSFER];			/*��������ܵ��������ݲ��ո�����Ϣ��body����data��ʽ*/
	/*crc16*/
	unsigned char crc[2];		//bcd
}message;


/* ���͵����� body ����data ��ʽ*/
typedef struct heart_info_{
	/*һ��Ϊbody data����*/
	unsigned char PosStatus;			//  byte : 0	
	unsigned char FirVer[2];		//  bcd  :
	unsigned char TerParVer[2];		// -
	unsigned char Line[2];				//
	unsigned char  BlkParVer[4];		//
	unsigned char CatpParVer[4];		// -
	unsigned char MchCode[2];			// -			
	unsigned char SamID[6];			// - bcd
	unsigned char LinVer[2];		// 	bcd
	unsigned char InCarBlkParVer[4];	//
	unsigned char QRBlkVer[4];			// -
	unsigned char InQRBlkParVer[4];		//
	unsigned char HTBlkParVer[4];		//
	unsigned char  HTInBlkParVer[4];		//
	unsigned char PriParVer[2];		//
	unsigned char MilParVer[2];		//
	unsigned char HTtwoRelParVer[2];	//
	unsigned char HTWriPar[2];		//
	
}__attribute__((packed)) heart_info;

/*���յ���������body ����data ��ʽ 0x3001�ķ���*/

typedef struct heart_bak_info_{
	unsigned char dattime[7];			// bcd YYYYYMMDDHHmmss
	unsigned char flag[2];				//λ��
	unsigned char buscode[4];
}heart_bak_info;



/*pos �ϴ�A2�ļ� body����data ��ʽ*/

typedef struct record_info_
{
	RecordFormat_new record;

}record_info;

/*pos ����¼�ϴ����� body���� data ��ʽ 0x3002�ķ���*/
typedef struct record_bak_info_
{
	unsigned char respond[3];
	unsigned char reserved[2];
}record_bak_info;

/*pos ��������¸��¹̼���Ϣ ���� body ���� data��ʽ ,0x3004 �ķ���*/
typedef struct updatefirm_back_info_{
	unsigned char version[2];
	unsigned char filesz[4];
	
}updatefirm_back_info;



#define TMEP_FIX_FILE_PREFIX	"tmpfirm.fex"		
#define FIX_FILE_PREFIX		"firm.fex"		//������ɵĶ��Ǵ��а汾��׺�� firm.fex.10.0.2

typedef struct firmhead_{
	unsigned char ver[2];		//�� 0x1002  ��ʾ 1.0.02
	int origisz;				//�ļ�ԭ��С
	int currsz;					//�����ش�С 
	int headsz;					//�ļ�ͷ��С
	int offset;					//����ƫ�����������ص������� offset=currsz
}firmhead;


/*0x3005 body ���� ��data ��ʽ */
typedef struct down_fir_info_{
		unsigned char offset[4];
		unsigned char length[2];
}__attribute__((packed)) down_fir_info;


/*0x3085 body ���� ��data ��ʽ */
typedef struct down_fir_back_info_{
	unsigned char offset[4];
	unsigned char length[2];
	unsigned char *fdata;				//��ȡmessg��ʱ��̬����� fdata��С
}down_fir_back_info;


/*���ع̼�������Ϣ*/
typedef struct download_process_info_{	

	char tmppath[56];  //��ʱ�ļ���
	char destpath[56]; //Ŀ���ļ���
	
	char version[2];		//���ذ汾
	int  origisz;		//������С
	int  currsz;		//�����ش�С
	int  offset;		// ���� currsz
	
}download_process_info;

/*�̼��ļ����5���ֽڱ�ʾ�̼���Ϣ*/
typedef struct fix_info_{
	unsigned char filecrc[2];
	unsigned char type;
	unsigned char vers[2];
}fix_info;





/**�����̻�ID���� ,0x3087 body����data��ʽ***/
typedef struct request_Merchant_back_info_{

	unsigned char paraVer[2];
	unsigned char agentId[4];
	unsigned char agentSpId;
	unsigned char mchntId1[6];
	unsigned char mchntSpId1[2];
	unsigned char mchntId2[6];
	unsigned char mchntSpId2[2];
	unsigned char mchniName[40];
	unsigned char storeId[6];
	unsigned char storeSpId[2];
	unsigned char storeNmae[40];
	unsigned char offSeasonMon[40];
	unsigned char offSeasonPri[4];
	unsigned char onSeasonMon[40];
	unsigned char onSeasonPri[4];
	
}request_Merchant_back_info;



/**����ƽ̨֧�ֵ��ļ�**/
#define FIL1_BLACK		 		"1:/G1"
#define FIL2_INCR_BLACK 		"1:/G4"
#define FIL3_CARD_TYPE			"1:/M2"
#define FIL4_LINE_PARA    		"1:/M3"
#define FIL5_QR_INCR_BLACK		"1:/G10"
#define FIL6_HT_INCR_BLACK		"1:/G6"
#define FIL7_HT_BLACK			"1:/G7"
#define FIL8_PRICE				"1:/M4"
#define FIL9_MILS_PARA			"1:/M5"
#define FIL10_TWO_RELE_PARA		"1:/MP"
#define	FIL11_HT_WHITE			"1:/W1"

/* M3 �ļ��ṹ */
typedef struct M3_head_{
		unsigned char upValueLen;			
		unsigned char upCount;
		unsigned char downValueLen;
		unsigned char downCount;
}M3_head;

typedef struct M3_single_{
		unsigned char staionNo;
		unsigned char mileage[2];
		unsigned char stationName[19];
}M3_single;


/*M4 �ļ��ṹ  */
typedef struct M4_head_{
		unsigned char upValueLen;			
		unsigned char upCount[2];
		unsigned char downValueLen;
		unsigned char downCount[2];
}M4_head;

typedef struct M4_single_{
		unsigned char price[2];
}M4_single;

/*M5 �ļ��ṹ  */
typedef struct M5_head_{
		unsigned char localDiscontLen;			
		unsigned char localCount[2];
		unsigned char HTDiscontLen;
		unsigned char HTCount[2];
}M5_head;

typedef struct M5_local_single_{
	unsigned char cardAttr;
	unsigned char cardType;
	unsigned char cardXX;
	unsigned char Mode;
	unsigned char BalMin[2];
	unsigned char TouZhiMax[2];
	unsigned char BalMax[4];
	unsigned char PriceMax[2];
	unsigned char inDiscount;
	unsigned char inDiscontPri[2];
	unsigned char outDiscount;
	unsigned char outDiscontPri[2];
}M5_local_single;


typedef struct M5_HT_single_{
	unsigned char issurCode1[4];
	unsigned char issurCode2[4];
	unsigned char cardType;
	unsigned char cardXX;
	unsigned char Mode;
	unsigned char TouZhiMax[2];
	unsigned char BalMax[4];
	unsigned char PriceMax[2];
	unsigned char inDiscount;
	unsigned char inDiscontPri[2];
	unsigned char outDiscount;
	unsigned char outDiscontPri[2];
	unsigned char reserved[2];
}M5_HT_single;


/*
	Mp �ļ��ṹ
*/
typedef struct MP_filinfo_{

	unsigned char line[2];
	unsigned char lineName[16];
	unsigned char corpFlag[2];
	unsigned char corpEnterpriseName[40];
	unsigned char corpCode[4];
	unsigned char corpCompanyName[40];
	unsigned char lineAttr;
	unsigned char upPriceItemNum[2];
	unsigned char downPriceItemNum[2];
	unsigned char priceVer[2];
	unsigned char ticketTrans[2];
	unsigned char defaultBasPrice[2];
	unsigned char upStationMid;
	unsigned char downStationMid;

	unsigned char mode_bj;
	unsigned char vehicleAttr;
	unsigned char modee_hl_bp;
	unsigned char onOffDir;
	unsigned char priceDiscont;
	unsigned char funcitonFlag[2];
	
}MP_filinfo;


/* ͨ�� �ļ����ع�����Ϣ�ṹ */
typedef struct downfileprocess_{
	char keyword[40];			//����ƽ̨���ļ���������ƥ��
	char tmpname[40];			//��ʱ�ļ���
	char destname[40];			//Ŀ���ļ���
	unsigned char filecrc[2]; 	//�ļ�crcУ��ֵ �������ļ�����������֤
	unsigned char ver[4];					//�汾,ʵ��ֻ�õ�2���ֽ�
	int destlen;				//Ŀ�곤��
	int offset;					//��ǰƫ����
	int status;					// 0:����  1:������
}downfileprocess;



/* 0x308A body ����data ����*/
typedef struct file_desinfo_back_{
	unsigned char filename[40];
	unsigned char version[4];
	unsigned char fileCRC[2];
	unsigned char filesz[4];

}file_desinfo_back;



/*0x300b body ���� data��ʽ*/
typedef struct down_file_info_{
	unsigned char filename[40];
	unsigned char offset[4];
	unsigned char length[2]; 
}down_file_info;

typedef struct down_file_back_info_{
	unsigned char name[40];
	unsigned char offset[4];
	unsigned char length[2];
	unsigned char * filedata;	//��ȡmessg��ʱ��̬����� fdata��С

}down_file_back_info;


/**0x300c data ����**/
typedef struct send_file_info_{
	
	unsigned char name[40];
	unsigned char ver[4];
	unsigned char fileCRC[2];
	unsigned char filesz[4];
	
}send_file_info;

/** 0x308c data ����**/
typedef struct send_file_back_{
	unsigned char name[40];
	unsigned char state;				
}send_file_back;
	

#endif

