#ifndef		__ERROR_LOG_H__
#define		__ERROR_LOG_H__



#define		CARD_OUT_OF_TIME		0x0001    //卡片已经过有效期
#define		CARD_FORNAT_ERROR		0x0002    //卡片格式错误,或非本系统卡片
#define		WALLET_FORMAT_ERROR		0x0003    // 钱包格式错误
#define		RC531_READ_WRITE_ERR	0x0004	  // RC531 读写错误
#define		PROCESS_ID_ERROR		0x0005    	// 卡片格式进程标志错误
#define		CASH_WALLET_ERROR		0x0006    //  现金钱包错误
#define		REPARE_CARD_ERROR		0x0007    /*修复卡错误*/
#define		ERROR_SAVE_RETURN		0x0008	  /* 保存返回值*/
#define		TIME_NOT_IN_SUPPLY		0x0009    //  现在时间不在消费时段内
#define		CARD_USER_NOT_ALLOWED	0x000A    //  此卡片为黑名单卡片
#define		SYSTEM_NOT_SUPPORT		0x000B    /*  系统部支持*/
#define		YOUR_MONEY_NOT_ENOUGHT	0x000C    //  您的交易金额不够
#define		EXCEED_MAX_TIME_LIMIT	0x000D    //  当前时段消费金额超过最大消费额度
#define		EXCEED_MAX_DAILY_LIMIT	0x000E	  //  当天消费额度超过最大消费额度
#define		EXCEED_MAX_MONTH_LIMIT	0x000F    //  当月消费金额总数超过最大消费额度
#define		PARAM_TABLE_ERROR		0x0010    /*  参数表错误*/
#define		OVER_BOOTED_MONTH		0x0011    //  包月时间已过
#define		EXCEED_MAX_DAILY_COUNT	0x0012    //  当天交易次数总数超过最大限制次数
//0x0013    //  保留
#define		MEMORY_ERROR			0x0014    //  内存指针错误
#define		RECORD_MEMORY_FULL		0x0015    //  记录内存已满
#define		TIME_NOW_NOT_ALLOWED	0x0016    /* 本卡在限制使用时间内*/
#define		CHECK_CONSUM_FAIL		0x0017    //   验证消费密匙失败
//0x0018    //   保留
//   .	     保留
//   .         保留
//   .         保留
#define		STATION_ERROR			0x0032    //  	站点错误
#define		CARD_ALREADY_EXIT		0x0033    //   此卡已经下车
#define		EXCEED_TIME_GAP			0x0034	  // 上车刷卡到下车刷卡间隔时间大于上限时间
#define		HAVE_NO_ABOARD_FLAG		0x0035	  // 没有上车标志
#define		SHEET_RATION_ERR		0x0036	  // 折率参数错误


// 2014-02-18 新增加的日志
#define		GPRS_DOWNLOAD_PARAM_OK   0x0037 /*GPRS下载参数成功*/
#define		GPRS_DOWNLOAD_PARAM_FAIL 0x0038 /*GPRS下载参数失败*/
#define		U_DISK_DOWN_PARAM_OK	 0x0039 /*u盘下载参数成功*/
#define		U_DISK_DOWN_PARAM_FAIL	 0x003A	/*u盘下载参数失败*/
#define		GPRS_UPDATE_OK			 0x003B /*远程升级成功*/
#define		GPRS_UPDATE_FAIL	     0x003C /*远程升级失败*/
#define 	U_DISK_UPDATE_OK		 0x003D /*u盘升级成功*/
#define	    U_DISK_UPDATE_FAIL		 0x003E /*u盘升级失败*/
#define		SIM_CARD_NOT_IN_POS      0x003F /*手机卡松动*/
#define		USB_FORMAT_DATA			 0x0040 /*u盘格式化数据*/
#define		GPRS_FORMAT_DATA		 0x0041 /*GPRS格式化数据*/
#define		USB_RM_LIMI_USER         0x0042 /*u盘格式化黑名单*/
#define		GPRS_RM_LIMI_USER        0x0043 /*GPRS格式化黑名单*/
#define		USB_SET_SECTOR			 0x0044 /*u盘设置扇区*/
#define		GPRS_SET_SECTOR			 0x0045 /*GPRS设置扇区*/

#define		USB_RESET_TERM_NUM		 0x0046 /*u盘修改终端机号*/
#define		GPRS_RESET_TERM_NUM		 0x0047 /*GPRS修改终端机号*/
#define		RS485_ERROR				 0x0048 /* 485 报站出错*/



void SaveLog(unsigned int type,void *pfile,unsigned int line);
extern unsigned int SaveRet;

#define		SAVE_CARLAN_LOG(type) SaveLog(type,__FILE__,__LINE__)
#define		GET_FUNC_RET(ret)     (SaveRet = (ret))




/*站点错误*/
struct STATION_ERROR_
{
	unsigned short error_type;
	unsigned char time[5];
	unsigned char card_stationid;
	unsigned char card_stationOn;
	unsigned char section_stationNow;
	unsigned char section_Updown;
	unsigned char section_stationNum;
	unsigned char sectionup_stationNum;
	unsigned char sectionNum;
	unsigned char card_id[4];
	unsigned char file[4];
	unsigned short line;

	unsigned char space[4];
};
/*上车刷卡到下车刷卡间隔时间大于上限时间*/
struct SWIPING_TIME_
{
	unsigned short error_type;
	unsigned char time[5];
	unsigned char space[1];
	unsigned short section_DeductTime;		
	unsigned short line;
	unsigned int DevNum_i;
	unsigned int CardLan_OldTermNo;	
	unsigned char card_id[4];
	unsigned char file[4];		
};
/* 没有上车标志*/
struct HAVE_NOT_ABOARD_
{
	unsigned short error_type;
	unsigned char time[5];
	unsigned char card_stationid;
	unsigned char card_stationOn;
	unsigned char card_EnterExitFlag;
	unsigned char card_id[4];
	unsigned char file[4];	
	unsigned short line;
	
	unsigned char space[8];	
};
/*折率参数错误*/
struct SHEET_RATION_ERR_
{
	unsigned short error_type;
	unsigned char time[5];
	unsigned char space1[1];
	unsigned int status;
	unsigned char card_id[4];
	unsigned char file[4];
	unsigned short line;

	unsigned char space[6];
};
/* 2014-02-18 新增的错误日志*/
struct USER_ACTION_RECORD_
{
	unsigned short error_type;
	unsigned char time[5];
	unsigned char space1[1];
	unsigned int term_id;
	union{
		unsigned int old_term_id;
		unsigned char card_id[4];
	}tmp_4byte;
	unsigned char file[4];
	unsigned short line;

	unsigned char space[6];

};


/*连接报站器错误*/
struct UART_ANNOUNCER_ERR_
{
	unsigned short error_type;
	unsigned char time[6];
	unsigned char section_stationNow;
	unsigned char section_Updown;
	unsigned char section_stationNum;
	unsigned char sectionup_stationNum;
	unsigned char sectionNum;
	unsigned char file[4];
	unsigned short line;

	unsigned char space[9];
};






#endif

