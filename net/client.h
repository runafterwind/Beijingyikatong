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


/*	发送类消息定义数据包各个固定字段的长度 */
#define SYN_INFO_LEN	2
#define COMPRESS_LEN	1
#define MSG_TYPE_LEN	2
#define VERSION_LEN		1
#define POS_ID_LEN		4
#define CRC_LEN			2

/* 发送消息的同步信息部分规定 */
#define SEND_HEART_SYN  	0x3001
#define SEND_A2_SYN		0x3002
#define SEND_REQUEST_FIX_SYN 	0x3004
#define SEND_DOWNLOAD_FIX_SYN	0x3005
#define SEND_REQUEST_MCH_SYN 0X3007
#define SEND_REQUEST_FILE_DES_SYN 0x300A
#define SEND_DOWNLOAD_FILE_SYN   0x300B
#define SEND_FILE_INFO_SYN		0x300C

typedef struct device_info_{
	unsigned char posid[4];		//bcd 设备id
	unsigned char Ver;			//hex 版本

	unsigned char FirVer[2];		//  bcd  : 固件版本
	unsigned char TerParVer[2];				// -		  	
	unsigned char Line[2];						//		 线路	
	unsigned char  BlkParVer[4];					//		 黑名单参数版本	
	unsigned char CatpParVer[4];					// -		 可用卡类型参数版本
	unsigned char MchCode[2];					// -		 商户简码		
	unsigned char SamID[6];			// - bcd   SAM卡号
	unsigned char LinVer[2];		// 	bcd	 线路站点信息版本
	unsigned char InCarBlkParVer[4];				//		 实体卡增量黑名单版本
	unsigned char QRBlkVer[4];					// -		二维码黑名单版本
	unsigned char InQRBlkParVer[4];				//		二维码增量黑名单版本
	unsigned char HTBlkParVer[4];				//		互通黑名单版本
	unsigned char HTInBlkParVer[4];				//		互通增量黑名单版本
	unsigned char PriParVer[2];		//		票价表文件版本
	unsigned char MilParVer[2];		//		则扣率文件版本
	unsigned char HTtwoRelParVer[2];	//	费率卡二次发行信息
	unsigned char HTWriPar[2];			//	城市互通白名单版本
	
}device_info;


/**/


/* 发送/接收消费的结构 */
typedef struct message_{
	/* head */
	unsigned char len[2];		//bcd
	unsigned char syncinfo[2];	//hex
	unsigned char compress;		//hex
	/*body */
	unsigned char  type[2];		//bcd
	unsigned char  ver;			//hex
	unsigned char posid[4];		//bcd 
	unsigned char data[MAX_TRANSFER];			/*发送与接受的数据内容参照各类信息的body部分data格式*/
	/*crc16*/
	unsigned char crc[2];		//bcd
}message;


/* 发送的心跳 body 部分data 格式*/
typedef struct heart_info_{
	/*一下为body data部分*/
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

/*接收的心跳返回body 部分data 格式 0x3001的返回*/

typedef struct heart_bak_info_{
	unsigned char dattime[7];			// bcd YYYYYMMDDHHmmss
	unsigned char flag[2];				//位域
	unsigned char buscode[4];
}heart_bak_info;



/*pos 上传A2文件 body部分data 格式*/

typedef struct record_info_
{
	RecordFormat_new record;

}record_info;

/*pos 机记录上传返回 body部分 data 格式 0x3002的返回*/
typedef struct record_bak_info_
{
	unsigned char respond[3];
	unsigned char reserved[2];
}record_bak_info;

/*pos 机请求更新更新固件信息 返回 body 部分 data格式 ,0x3004 的返回*/
typedef struct updatefirm_back_info_{
	unsigned char version[2];
	unsigned char filesz[4];
	
}updatefirm_back_info;



#define TMEP_FIX_FILE_PREFIX	"tmpfirm.fex"		
#define FIX_FILE_PREFIX		"firm.fex"		//最后生成的都是带有版本后缀如 firm.fex.10.0.2

typedef struct firmhead_{
	unsigned char ver[2];		//如 0x1002  表示 1.0.02
	int origisz;				//文件原大小
	int currsz;					//已下载大小 
	int headsz;					//文件头大小
	int offset;					//下载偏移量，已下载的数据量 offset=currsz
}firmhead;


/*0x3005 body 部分 的data 格式 */
typedef struct down_fir_info_{
		unsigned char offset[4];
		unsigned char length[2];
}__attribute__((packed)) down_fir_info;


/*0x3085 body 部分 的data 格式 */
typedef struct down_fir_back_info_{
	unsigned char offset[4];
	unsigned char length[2];
	unsigned char *fdata;				//获取messg的时候动态分配的 fdata大小
}down_fir_back_info;


/*下载固件过程信息*/
typedef struct download_process_info_{	

	char tmppath[56];  //临时文件名
	char destpath[56]; //目标文件名
	
	char version[2];		//下载版本
	int  origisz;		//期望大小
	int  currsz;		//已下载大小
	int  offset;		// 等于 currsz
	
}download_process_info;

/*固件文件最后5个字节表示固件信息*/
typedef struct fix_info_{
	unsigned char filecrc[2];
	unsigned char type;
	unsigned char vers[2];
}fix_info;





/**请求商户ID返回 ,0x3087 body部分data格式***/
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



/**服务平台支持的文件**/
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

/* M3 文件结构 */
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


/*M4 文件结构  */
typedef struct M4_head_{
		unsigned char upValueLen;			
		unsigned char upCount[2];
		unsigned char downValueLen;
		unsigned char downCount[2];
}M4_head;

typedef struct M4_single_{
		unsigned char price[2];
}M4_single;

/*M5 文件结构  */
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
	Mp 文件结构
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


/* 通用 文件下载过程信息结构 */
typedef struct downfileprocess_{
	char keyword[40];			//服务平台的文件名，用于匹配
	char tmpname[40];			//临时文件名
	char destname[40];			//目标文件名
	unsigned char filecrc[2]; 	//文件crc校验值 ，整个文件下完后计算验证
	unsigned char ver[4];					//版本,实际只用到2个字节
	int destlen;				//目标长度
	int offset;					//当前偏移量
	int status;					// 0:空闲  1:进行中
}downfileprocess;



/* 0x308A body 部分data 部分*/
typedef struct file_desinfo_back_{
	unsigned char filename[40];
	unsigned char version[4];
	unsigned char fileCRC[2];
	unsigned char filesz[4];

}file_desinfo_back;



/*0x300b body 部分 data格式*/
typedef struct down_file_info_{
	unsigned char filename[40];
	unsigned char offset[4];
	unsigned char length[2]; 
}down_file_info;

typedef struct down_file_back_info_{
	unsigned char name[40];
	unsigned char offset[4];
	unsigned char length[2];
	unsigned char * filedata;	//获取messg的时候动态分配的 fdata大小

}down_file_back_info;


/**0x300c data 部分**/
typedef struct send_file_info_{
	
	unsigned char name[40];
	unsigned char ver[4];
	unsigned char fileCRC[2];
	unsigned char filesz[4];
	
}send_file_info;

/** 0x308c data 部分**/
typedef struct send_file_back_{
	unsigned char name[40];
	unsigned char state;				
}send_file_back;
	

#endif

