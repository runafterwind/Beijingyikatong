#ifndef		__ERROR_LOG_H__
#define		__ERROR_LOG_H__



#define		CARD_OUT_OF_TIME		0x0001    //��Ƭ�Ѿ�����Ч��
#define		CARD_FORNAT_ERROR		0x0002    //��Ƭ��ʽ����,��Ǳ�ϵͳ��Ƭ
#define		WALLET_FORMAT_ERROR		0x0003    // Ǯ����ʽ����
#define		RC531_READ_WRITE_ERR	0x0004	  // RC531 ��д����
#define		PROCESS_ID_ERROR		0x0005    	// ��Ƭ��ʽ���̱�־����
#define		CASH_WALLET_ERROR		0x0006    //  �ֽ�Ǯ������
#define		REPARE_CARD_ERROR		0x0007    /*�޸�������*/
#define		ERROR_SAVE_RETURN		0x0008	  /* ���淵��ֵ*/
#define		TIME_NOT_IN_SUPPLY		0x0009    //  ����ʱ�䲻������ʱ����
#define		CARD_USER_NOT_ALLOWED	0x000A    //  �˿�ƬΪ��������Ƭ
#define		SYSTEM_NOT_SUPPORT		0x000B    /*  ϵͳ��֧��*/
#define		YOUR_MONEY_NOT_ENOUGHT	0x000C    //  ���Ľ��׽���
#define		EXCEED_MAX_TIME_LIMIT	0x000D    //  ��ǰʱ�����ѽ���������Ѷ��
#define		EXCEED_MAX_DAILY_LIMIT	0x000E	  //  �������Ѷ�ȳ���������Ѷ��
#define		EXCEED_MAX_MONTH_LIMIT	0x000F    //  �������ѽ����������������Ѷ��
#define		PARAM_TABLE_ERROR		0x0010    /*  ���������*/
#define		OVER_BOOTED_MONTH		0x0011    //  ����ʱ���ѹ�
#define		EXCEED_MAX_DAILY_COUNT	0x0012    //  ���콻�״�����������������ƴ���
//0x0013    //  ����
#define		MEMORY_ERROR			0x0014    //  �ڴ�ָ�����
#define		RECORD_MEMORY_FULL		0x0015    //  ��¼�ڴ�����
#define		TIME_NOW_NOT_ALLOWED	0x0016    /* ����������ʹ��ʱ����*/
#define		CHECK_CONSUM_FAIL		0x0017    //   ��֤�����ܳ�ʧ��
//0x0018    //   ����
//   .	     ����
//   .         ����
//   .         ����
#define		STATION_ERROR			0x0032    //  	վ�����
#define		CARD_ALREADY_EXIT		0x0033    //   �˿��Ѿ��³�
#define		EXCEED_TIME_GAP			0x0034	  // �ϳ�ˢ�����³�ˢ�����ʱ���������ʱ��
#define		HAVE_NO_ABOARD_FLAG		0x0035	  // û���ϳ���־
#define		SHEET_RATION_ERR		0x0036	  // ���ʲ�������


// 2014-02-18 �����ӵ���־
#define		GPRS_DOWNLOAD_PARAM_OK   0x0037 /*GPRS���ز����ɹ�*/
#define		GPRS_DOWNLOAD_PARAM_FAIL 0x0038 /*GPRS���ز���ʧ��*/
#define		U_DISK_DOWN_PARAM_OK	 0x0039 /*u�����ز����ɹ�*/
#define		U_DISK_DOWN_PARAM_FAIL	 0x003A	/*u�����ز���ʧ��*/
#define		GPRS_UPDATE_OK			 0x003B /*Զ�������ɹ�*/
#define		GPRS_UPDATE_FAIL	     0x003C /*Զ������ʧ��*/
#define 	U_DISK_UPDATE_OK		 0x003D /*u�������ɹ�*/
#define	    U_DISK_UPDATE_FAIL		 0x003E /*u������ʧ��*/
#define		SIM_CARD_NOT_IN_POS      0x003F /*�ֻ����ɶ�*/
#define		USB_FORMAT_DATA			 0x0040 /*u�̸�ʽ������*/
#define		GPRS_FORMAT_DATA		 0x0041 /*GPRS��ʽ������*/
#define		USB_RM_LIMI_USER         0x0042 /*u�̸�ʽ��������*/
#define		GPRS_RM_LIMI_USER        0x0043 /*GPRS��ʽ��������*/
#define		USB_SET_SECTOR			 0x0044 /*u����������*/
#define		GPRS_SET_SECTOR			 0x0045 /*GPRS��������*/

#define		USB_RESET_TERM_NUM		 0x0046 /*u���޸��ն˻���*/
#define		GPRS_RESET_TERM_NUM		 0x0047 /*GPRS�޸��ն˻���*/
#define		RS485_ERROR				 0x0048 /* 485 ��վ����*/



void SaveLog(unsigned int type,void *pfile,unsigned int line);
extern unsigned int SaveRet;

#define		SAVE_CARLAN_LOG(type) SaveLog(type,__FILE__,__LINE__)
#define		GET_FUNC_RET(ret)     (SaveRet = (ret))




/*վ�����*/
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
/*�ϳ�ˢ�����³�ˢ�����ʱ���������ʱ��*/
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
/* û���ϳ���־*/
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
/*���ʲ�������*/
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
/* 2014-02-18 �����Ĵ�����־*/
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


/*���ӱ�վ������*/
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

