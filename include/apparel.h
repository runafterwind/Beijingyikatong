#ifndef _APPAREL_H_
#define _APPAREL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
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
#include <popt.h>
#include <ctype.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>


/*
		版本定义:v开头表示正常标准版；
					    vx表示试用版(在正常标准版基础上)
					    vs表示由正常标准版兼容8位
*/

#define	VERSION_NUMB	"V9.1.170926" 


/* 是否需要刷卡不成功，重新刷卡*/
#define	NO_RESWIPING_CARD
#undef	NO_RESWIPING_CARD

/********************************客户定义宏*********************************/
//定义标准版本
#define	STANDARD_VERSION			1

//使用0409机器
#define  NEW0409                    0

//使用BS平台
#define  BS                         0


#define     WCDMA                   0

//交通部标准
#define 	Transport_Stander	    1


#define    SUPPORT_QR_CODE		    1//二维码
//#define     OFFLINE_TEST_QRCODE   //二维码测试
//#define    TEST_QR_CODE_SPEED
#define    QR_CODE_USE_USBHID		0


/*********************************************************************/



#define DEBUG_PRINTF

#ifdef DEBUG_PRINTF
#define DebugPrintf(format, ...) fprintf(stdout,"[%s][%s][%d]"format,__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define DebugPrintf(format, ...)
#endif



#if(STANDARD_VERSION == 0)
	#undef	STANDARD_VERSION
#endif

#if (Transport_Stander==0)
 	#undef Transport_Stander
#endif 

#if (SUPPORT_QR_CODE == 0)
	#undef SUPPORT_QR_CODE
#endif

#if (QR_CODE_USE_USBHID == 0)
	#undef QR_CODE_USE_USBHID
#endif

#if (NEW0409==0)
    #undef NEW0409
#endif 

#if (BS==0)
    #undef BS
#endif 



#ifdef   NEW0409
#define  MAXLEN     480
#else
#define  MAXLEN     320
#endif

#if (	!(defined STANDARD_VERSION)	)
	#error  "!!!!!!!!!!! Please Select One User In includes.h !!!!!!!!!!"
#endif
#define  CONFIG_BZLINUXBUS    	//linux 标准公交


////////////////////  dev/buzzer  /////
#define        BEEPL                 0
#define        BEEPH                 1

#define        LEDAL                 2
#define        LEDAH                 3

#define        LEDBL                 4
#define        LEDBH                 5

#define        RELAYL                6
#define        RELAYH                7

#define        KEYPOWERL             8
#define        KEYPOWERH             9
///////////////////////////////////////

#define 	   ERROR           		 0   
#define 	   SUCCESS         		 1  

#define 		RECORD_MAX		200000	//存储的最大数目
#define 		RECORD_CRITICAL	99850	//存储的限制数目
#define			RECORD_WARNING	95000	//存储的报警数目



/*************************************
	HAL   Degfine
*************************************/

#define		OFF_LINE_CONSUM_FILE	     "/mnt/record/cardsave.bin"
#define		SYS_PARAM_FILE               "/mnt/record/system.sys"
#define     RECORD_FILE_PATH 	         "/mnt/record/RecordErrorFile.txt"
//#define     PARM_FILE_PATH               "/mnt/record/cardlan.sys"
#define     PARM_FILE_PATH               "/mnt/record/M4"
#define     SECTION_FILE_PATH            "/mnt/record/section.sys"
#define     SECTIONUP_FILE_PATH          "/mnt/record/sectionup.sys"
#define     SECTIONDISUP_FILE_PATH       "/mnt/record/stationdisup.sys"
#define     SECTIONDISDOWN_FILE_PATH     "/mnt/record/stationdisdown.sys"
#define     BASICRATE_FILE_PATH          "/mnt/record/basicrate.sys"
#define		SAVE_CONSUM_DATA_DIRECT

#define   SECTION_KM_PATH_NAME	 "/mnt/record/M3"
//每2个站点之间的公里数所占的文件大小
#define   SECTION_POINT_SIZE		22



#define SAVE_RECORD_PATH	"/mnt/record/cardsave.bin"



extern char* mk_time (char* buff);
extern char  Wr_time (char *dt);
extern int ascii_2_hex(unsigned char *O_data, unsigned char *N_data, int len);
extern unsigned char ReadIPCard (char type);
extern unsigned char ReadKeyCard (void);

extern pthread_mutex_t m_datafile;


/***************  SDcard.c  *****************/
extern int MmcCardDetect(void);
extern int MmcCardClose(void);

extern  void * ReadGprs_Pthread (void * args);
extern  void * Readsql_Pthread (void * args);
extern  void *Dushugao_HeartBeatPacket(void *arg);
extern  void *Gps_Pthread(void *arg);

#endif
