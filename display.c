//����ͷ�ļ�
#include  "apparel.h"
#include "../gui/RC500.h"
#include "../gui/cpucard.h"
#include "../gui/InitSystem.h"
#include "../gui/des.h"
#include "../gui/OnlineRecharge.h"
#include "../gui/querysql.h"
#include "../sound/sound.h"
#include "../Display/fbtools.h"
#include "../input/keyboard.h"
#include "../gui/gps.h"
#include "../update/resumedownload.h"
#include "../gui/cpucard.h"
/************************************************************/
MYBMPFILE *signal_bmp;

/***�ⲿ���ñ���***/
extern unsigned char freedomflag;
extern volatile unsigned char PlayMusicFinishFlag;
extern unsigned char PsamNum[6];
extern unsigned char PsamNum_bak2[6];
extern unsigned char Tunion;
extern unsigned char OPENBEEP;
extern unsigned char ConnectFlag; //������־
extern LongUnon AllMoney;
extern LongUnon TypeMoney;
extern LongUnon DevNum;
extern LongUnon CodeNum;
extern LongUnon  SaveNum;
extern LongUnon  TransactionNum;
extern char TcpIpBuf[35];
extern ShortUnon Infor;
extern unsigned char HttpProt[35];
extern unsigned char UserKeyname[14];
extern unsigned char SelfAddress[32];
extern int mf_fd;
extern int mg_fd;
extern unsigned char BankCardSwitch;
extern unsigned char LastCardNum[4];
extern CardInform CardLan;
extern unsigned char OPENPRINTF;
extern unsigned char *CardLanBuf;
extern SectionFarPar Section,Sectionup;
extern unsigned char ReadCardFirst;
extern CardLanSector LanSec;		//�û�����
extern int bp_fd;
extern unsigned int TotalCardCount;
extern int RecFd;
extern unsigned char Dushugao_ConnectServerFlag;
extern unsigned char GPSSIG;
extern unsigned char COMNET;
extern unsigned char G_SignInfo_PBOC;
extern unsigned char G_Consume_PBOC;		/* �ϴ�������Ϣ */
extern unsigned char G_BatchSett_PBOC;	
extern struct DALI_TransferMsg RcvData;
extern unsigned int Dci;
extern unsigned char updatline_err;
extern unsigned short SectionNum;
extern unsigned char g_FgFileOccurError;
extern unsigned char g_FgSetPersonMode;
extern unsigned int g_MaxPersonNumber;
extern unsigned int g_CurrentNumber;
extern unsigned char updataflag;
extern PIXEL _pix[76800];
extern PIXEL _pix1[76800];
extern char Eflag;
extern BMP_INFO bmppicture1;
extern LongUnon HostValue,DecValue;
extern LongUnon Fixvalue;		//�趨�̶�����ֵʹ�õı���

extern void SetMaxPersonNum(void);
extern void MakeSureCardLeave(void);

#ifdef TEST_QR_CODE_SPEED
unsigned int TotalTestTime = 0;
unsigned int TotalMsTime = 0;
unsigned int TotalTestCnt = 0;
extern int QRTest_fd;
struct timeval second_test;
#endif


/***��������***/
LongUnon  GetWord;
int myflag=0;
//��ѯ��Ƭ��Ϣ
unsigned char ReadcardFlag;
unsigned char CardTwo;                  //���±�־
unsigned char LCDKeepDisp = 0;          // ������ʾ������Ϣ״̬
unsigned char DisplaySignalStatus = 1;
unsigned char DevVersion[30];           // ��ȡ�汾��
unsigned char disflag;
unsigned int sumye,nowye=1;
unsigned char ReadDushugaoCfgError = 0;
unsigned int  CurIcCardNo = 0;
unsigned char RepairLastRecErrorFlag = 0;
unsigned char WEISHENG_DeviceAuther = 0; 	//0:�豸δ��֤ͨ��   1:�豸��֤ͨ�� 
unsigned char WEISHENG_ShowFlag = 1;
unsigned int SumMoney = 0;
volatile unsigned char showSunMoneyFLag = 0;				//0 ���������    1:������� 
volatile unsigned char showSunMoneyFinishFLag = 0;
unsigned int addrflag=0;
unsigned int Dci=0;
unsigned int firstcon=0;
unsigned int SaveNumbak=0;
int tm_offamp=0;
int thread_timer = 0;

struct timeval LCDKeepStartTime;
struct {
	unsigned char bGetCard;
	unsigned char bDisCtrl;
	unsigned char bStartInqure;
	unsigned char bStickCard;
	unsigned char Cardbuf[4];
	struct timeval tim;
}DriverCard;
struct DRIVER_SEARCH SearchNextPage;

//�߳�����
pthread_mutex_t m_sysfile = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m_datafile = PTHREAD_MUTEX_INITIALIZER; //m_socketwrite
pthread_mutex_t m_socketwrite = PTHREAD_MUTEX_INITIALIZER; //
pthread_mutex_t m_ErrorRecFile = PTHREAD_MUTEX_INITIALIZER; //
pthread_mutex_t m_ErrorList = PTHREAD_MUTEX_INITIALIZER; //
pthread_mutex_t m_DisplaySingle = PTHREAD_MUTEX_INITIALIZER; //
pthread_mutex_t m_stationrecord = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m_Blacklist = PTHREAD_MUTEX_INITIALIZER;

#ifdef	DBG_RF_TRANS_TIME
struct timeval test1,test2;
#endif


/************************************************************/


void DisConsumption(void)
{
	//struct USER_RESERCH *pUserRec;
	unsigned char count=0;
	unsigned int i,sum=0;
   
	struct USER_RESERCH pUserRec[5];
	unsigned char buf[40];
	Dci=0;
	
	memset(pUserRec,0,sizeof(struct USER_RESERCH)*5);
 //   if(upordown)
//	    count = GetUserRec(pUserRec);
 //   else
        count = GetUserRec_(pUserRec);
	if(count==0){
		//free(pUserRec);
	//	printf("--count = GetUserRec(pUserRec);\n");
		return;
	}	
	DriverCard.bDisCtrl = 0;
	LCDKeepDisp = 1;		
	SetColor(Mcolor);
	SetTextSize(16);
	TextOut(10,10, "  ����              ʱ��       ���");	
	i = 0;
	while(count--)
	{
		SetTextSize(16);
		TextOut(1,45+i*30,(pUserRec+i)->id);
		SetTextSize(16);
		TextOut(135,45+i*30,(pUserRec+i)->tim);
		TextOut(260,45+i*30,(pUserRec+i)->val);
		i++;
	}
	if(GetTotalConsumption(&i)==0)			//���㵱�콻���ܶ�
	{
		memset(buf,0,sizeof(buf));
		strcpy(buf,"���콻���ܶ�:");
		MoneyValue(buf+strlen(buf),i);
		//printf("i === %d\n",i);
		SetTextSize(16);
		TextOut(1,190,buf);
		memset(buf,0,sizeof(buf));
		strcpy(buf,"���콻���ܴ���:");
		sprintf(buf+15,"%d",Dci);
		TextOut(1,210,buf);		
	}
	else
		printf("GetTotalConsumption return 0 \n");		
    memset(buf,0,sizeof(buf));
    sumye = (TransactionNum.i -1) / 5;
    if((TransactionNum.i -1)%5>0)
        sumye++;
    sprintf(buf,"%d/%d",nowye,sumye);
    TextOut(233,200,buf);
    
	//sleep(5);
	//free(pUserRec);
	gettimeofday(&LCDKeepStartTime, 0);


	
}

void GetDevVersion(unsigned char* pver)
{
	memset(DevVersion, 0, sizeof(DevVersion));
	strcpy(DevVersion, pver);
}

// added by taeguk , Set desktop display
void GetDisInfo(char *buf,int len,unsigned int *x)
{
	printf("GetDisInfo() is called !!!!!\n");
	FILE *pfdis;
	char arry[50];
	int ret,i;
	if(len < 20)
		return;
	
	memcpy(buf, "����ˢ����", strlen("����ˢ����"));
	*x = 85;
	if(access("/var/run/dis.txt", 0)==0)
	{
		pfdis = fopen("/var/run/dis.txt","r");
		if(pfdis == NULL)
			return;
		ret = fread(arry,sizeof(unsigned char),20,pfdis);		
		if((ret) && (ret <= 20))
		{
			*x = (160 - ret*8);
			memset(buf, 0, len);
			memcpy(buf, arry, ret);
		}
		fclose(pfdis);
	}
	printf("buf = %s\n",buf);
}


char readgccdata(char *data,unsigned char type)
{

	unsigned char status;
	char buf[30];
	char buf1[10];
	char buf2[10];

	sprintf(buf,"%s",__DATE__);
	status = ( unsigned char)DateTran(buf);	//��

	memset(buf,0,sizeof(buf));
	memcpy(buf,__DATE__+4,2);
	if(buf[0] == 0x20) buf[0] = '0'; //��

	memset(buf1,0,sizeof(buf1));
	memcpy(buf1,__DATE__+7,4); //��

// memset(buf2,0,sizeof(buf1));
//  memcpy(buf2,__DATE__+12,2); //time

	switch(type)
	{
	case 0:
		sprintf(data,"%4.4s��%02d��%-2.2s��",buf1,status,buf);
		break;

	case 1:
		sprintf(data,"%4.4s_%02d_%-2.2s",buf1,status,buf);
		break;
	case 2:
		sprintf(buf2,"%s",__TIME__);
		buf2[5] = 0;
		sprintf(data,"%4.4s_%02d_%-2.2s ",buf1,status,buf);
		strcat(data,buf2);
		break;
	case 3:
		strcpy(data, VERSION_NUMB);
		break;
	default :
		break;
	}


	return 0;
}

/*
*************************************************************************************************************
- �������� : static char* mk_time (char* buff)
- ����˵�� : ��ȡʱ�����
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
char* mk_time (char* buff)
{
	time_t t;
	struct tm * tm;

	time (&t);
	tm = localtime (&t);

	sprintf(buff,"%04d-%02d-%02d  %02d:%02d:%02d",
	        tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,
	        tm->tm_hour,tm->tm_min,tm->tm_sec);
	return buff;
}



void Display_signal(unsigned char type)
{	
	//DebugPrintf("type = %u\n", type);
	DisplaySignalStatus = type;
	pthread_mutex_lock(&m_DisplaySingle);
	char *buff;	
	int i,len1,len2,MAXLen,len3;
	unsigned char ShowSignel_GPRS;
	
	int ShowLen_WEISHENG = 0;

	ShowLen_WEISHENG = strlen("���ڴ�������");
	//static unsigned char LastConnectStatus_GPS = 0, LastConnectStatus_GPRS = 0;	//��ֹ�ظ�ˢlogo����

	
    if(COMNET==2)
        len1=strlen("WIFI������");
    else
	    len1=strlen("���ڲ�����");
	
	len2 = strlen("�������ӷ�����");	
	MAXLen=len1>len2?len1:len2;	

	if((!WEISHENG_ShowFlag) && ((type == 1) || (type == 0)))
		type = 0;

	if(type == 0)
	{
		ShowSignel_GPRS = 1;
	}
	else if(type == 1)
	{
		ShowSignel_GPRS = 0;
	}
	else if(type == 10)	
	{		
		//SetTextSize(16);		
		buff = (char*)malloc(MAXLen);		
		for(i=0;i<MAXLen;i++)			
			buff[i]=0x20;				
		TextOut_fontSize(MAXLEN-MAXLen*8,10,buff, 16);	
		
        if(COMNET==2)
            TextOut_fontSize(MAXLEN-len1*8,10,"WIFI������", 16); 
        else
#if GUANGZHOU_WEISHENG
		if(WEISHENG_ShowFlag)
			TextOut_fontSize(MAXLEN-len1*8,10,"���ڲ�����", 16);
		else
			TextOut_fontSize(MAXLEN-ShowLen_WEISHENG*8,10,"���ڴ�������", 16);
#else
		    TextOut_fontSize(MAXLEN-len1*8,10,"���ڲ�����", 16);
#endif
		ShowSignel_GPRS = 0; 
		free(buff);	
	}	
	else if(type == 20)	
	{		
		//SetTextSize(16);		
		buff = (char*)malloc(MAXLen);	
		
		for(i=0; i<MAXLen; i++)			
			buff[i]=0x20;
#if GUANGZHOU_WEISHENG
		if(WEISHENG_ShowFlag)
		{
			TextOut_fontSize(MAXLEN-MAXLen*8, 10, buff, 16);		
			TextOut_fontSize(MAXLEN-len2*8, 10, "�������ӷ�����", 16);
		}
#else
		TextOut_fontSize(MAXLEN-MAXLen*8, 10, buff, 16);		
		TextOut_fontSize(MAXLEN-len2*8, 10, "�������ӷ�����", 16);
#endif
		ShowSignel_GPRS = 0;
		free(buff);	
	}	
	else if(type == 30)	
	{		
		//SetTextSize(16);		
		buff = (char*)malloc(MAXLen);	
		
		for(i=0; i<MAXLen; i++)			
			buff[i] = 0x20;	
		
		TextOut_fontSize(MAXLEN-MAXLen*8, 10, buff, 16);		
		ShowSignel_GPRS = 0;
		free(buff);	
	}
	else if(type == 40)
	{	
		//SetTextSize(16);		
		buff = (char*)malloc(MAXLen);		
		for(i=0; i<MAXLen; i++)			
			buff[i] = 0x20;	
		TextOut_fontSize(MAXLEN-MAXLen*8, 10, buff, 16);		
		TextOut_fontSize(MAXLEN-len2*8, 10, "���ӷ������ɹ�", 16);
		ShowSignel_GPRS = 1;
		free(buff);	
	}	
	else if(type == 50)
	{	
		//SetTextSize(16);		
		buff = (char*)malloc(MAXLen);		
		for(i=0; i<MAXLen; i++)			
			buff[i] = 0x20;	
#if GUANGZHOU_WEISHENG
		if(WEISHENG_ShowFlag)
		{
			TextOut_fontSize(MAXLEN-MAXLen*8, 10, buff, 16);		
			TextOut_fontSize(MAXLEN-len2*8, 10, "���ӷ�����ʧ��", 16);
		}
#else
		TextOut_fontSize(MAXLEN-MAXLen*8, 10, buff, 16);		
		TextOut_fontSize(MAXLEN-len2*8, 10, "���ӷ�����ʧ��", 16);
#endif
		ShowSignel_GPRS = 0;
		free(buff);	
	}
	else if (type == 60)
	{	
		//SetTextSize(16);
		buff = (char*)malloc(MAXLen);		
		for(i=0; i<MAXLen; i++)			
			buff[i] = 0x20;	
		TextOut_fontSize(MAXLEN-MAXLen*8, 10, buff, 16);		
		TextOut_fontSize(MAXLEN-len2*8, 10, "����ʧ������", 16);
		ShowSignel_GPRS = 0;
		free(buff);	
	}
	else if (type == 70)
	{	
		len3 = strlen("������������");
		buff = (char*)malloc(MAXLen);		
		for(i=0; i<MAXLen; i++)			
			buff[i] = 0x20;	
		TextOut_fontSize(MAXLEN-MAXLen*8, 10, buff, 16);		
		TextOut_fontSize(MAXLEN-len3*8, 10, "������������", 16);
		ShowSignel_GPRS = 0;
		free(buff);	
	}
	
	if(ShowSignel_GPRS == 1)	
	{		
		Show_BMP(10,10,"sigbmp.bmp");	
	}	
	else
	{		
#if GUANGZHOU_WEISHENG
		if(WEISHENG_ShowFlag)
		{
			//SetTextSize(32);		
			TextOut_fontSize(5,5,"	 ", 32);	
		}
#else
		//SetTextSize(32);		
		TextOut_fontSize(5,5,"	 ", 32);	
#endif
	}	

	if(GPSSIG == 0)
	{
		Show_BMP(100,10,"sigbmp.bmp");
	}
	else
	{
	    //SetTextSize(32);		
		TextOut_fontSize(50,5,"   ", 32);
	}
 
    if(Tunion)
    {
        TextOut_fontSize(120,5,"TU", 16);
        }
    else
    {
        TextOut_fontSize(120,5," ", 16);
        }
        
	pthread_mutex_unlock(&m_DisplaySingle);
}



void * main_timer (void * args)
{
	int ret;
	//struct stat fdbuf;
	//unsigned char rebuf[256];
	//int copiedSize1, copiedSize2;

	while(1)
	{
		if(thread_timer)
		{
			thread_timer--;
			if(thread_timer == 0)
				buzz_off();
			usleep(10000);
			
		}
		else
		{
			usleep(500000);
			if(tm_offamp != 0)
			{
				if(--tm_offamp == 0)
				{
					system("./StopAmp.sh;");
					w55fa93_setio(GPIO_GROUP_B, 5, 0);	//turn off Amp
				}
			}
		}
	}
}








int  LoopIsKeepingSwipeCard(void)
{
	unsigned char CheakKeepingSwipeCard = 0,  i = 0;
	
	while(CheakKeepingSwipeCard == MI_OK)
	{	
		for(i=5; i>0; i--)
		{
			if((CheakKeepingSwipeCard = ioctl(mf_fd, RC531_M1_CSN)) == MI_OK)
			{
				break;
			}
			usleep(1000);
		}
		usleep(50000);
		DebugPrintf("keeping swipe\n");
	}
	return 0;
}


/*
*************************************************************************************************************
- �������� : void * TimerTask(void *arg)
- ����˵�� : ����ִ�ж�ʱ��ѯ������
		   1. 3G�������ݶԽ�
		   2. ��ʱ�򱣴����ļ�¼�ļ�
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
void * TimerTask(void *arg)
{
	DebugPrintf("\n");
	int TimeCount = 0;

	while(1)
	{
 		//����ˢ�������¼������ 2s ��ѯһ��
		if(TimeCount > 10)
		{
			Timer_PollErrorRecordList();
			TimeCount = 0;
		}
		TimeCount++;
		usleep(100 * 1000);
	}
	return;
}


/*
*************************************************************************************************************
- �������� : void * PlaySumMoney(void *arg)
- ����˵�� : ����ִ�в����������
		   1. 3G�������ݶԽ�
		   2. ��ʱ�򱣴����ļ�¼�ļ�
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
void * PlaySumMoney(void *arg)
{
	SumMoney = 0;

	while(1)
	{
		if(showSunMoneyFLag)
		{
			showSunMoneyFinishFLag = 1;
			PlaySum(SumMoney);
			showSunMoneyFLag = 0;
			showSunMoneyFinishFLag = 0;
		}
		else
		{
			usleep(200 * 1000);
		}
	}
	return ;
}






unsigned char FindCardInListAndReadSectorData(void)
{
	DebugPrintf("\n");

	unsigned char status = 0;
	return 0;
	
	if(SeekErrorRecordList(CardLan.UserIcNo))
	{
		status = ReadPartCardSectorData();
		RepairLastRecErrorFlag = 1;
	}
	else
	{

		status = ReadCardInfor();
		RepairLastRecErrorFlag = 0;
	}

	DebugPrintf("RepairLastRecErrorFlag = %02X status = 0x%02X\n", RepairLastRecErrorFlag, status);	
//	printf("RepairLastRecErrorFlag = %02X status = 0x%02X\n", RepairLastRecErrorFlag, status);

	return status;

}


int WaitForSwipeCardOrTimeOut(unsigned char *SwipeCardError)
{
	DebugPrintf("\n");
	int count = 0, ReturnValue = 0;
	time_t StartTimeSec, EndTime = 0;
	
	time(&StartTimeSec);
	time(&EndTime);
	
	*SwipeCardError = 1;
	
	while((ioctl(mf_fd, RC531_M1_CSN) != MI_OK) && ((EndTime - StartTimeSec) < 10))
	{
    //    printf("run here ,no card:count=%d\n",count);
		if(count%100 == 0)
		{
			time(&EndTime);
		}

		if((count%500 == 0) && (PlayMusicFinishFlag))
			PlayMusic(15, 0);
		
		usleep(1000);
		count++;
	}
	
	if((EndTime - StartTimeSec) >= 10)		//��ʱû��Ѱ����
	{
		beepopen(11);
		LEDL(0);
		LEDR(0);
		*SwipeCardError = 0;
		RepairLastRecErrorFlag = 0;
		ShowSwipeCardStatus(0);
		StopMusic();
		return MI_FAIL;
	}
	
	return MI_OK;
}


int CheckRecordFull(void)
{
	DebugPrintf("\n");
	struct stat file_info,file_info1;
	off_t file_size;
	int ret = 0;
    int status;
	ret = stat(SAVE_RECORD_PATH, &file_info);
	if(ret == -1)
	{
		Err_display(19);
        ret = 5;
		return ret;
	}
	file_size = file_info.st_size/72;
	//printf("�����̿ռ�:cardsave.bin is %ld\n",file_size);
	if(file_size >= RECORD_CRITICAL)
	{
		ret = 2;
		Err_display(21);
        
	}
	else if(file_size >= RECORD_WARNING)
	{
		ret = 1;
		disflag = 1;
	}
	else
		disflag = 0;

	system("ls /mnt/record/ 2>/tmp/error_log.txt");
	status=stat("/tmp/error_log.txt",&file_info1);
	if((file_info1.st_size>0)||(status==-1))
		{
		ret = 3;
        g_FgFileOccurError = 1;
		Err_display(25);
		}
	
	if (g_MaxPersonNumber)
	{
		if (g_CurrentNumber >= g_MaxPersonNumber)
		{
			printf("g_CurrentNumber = %d\n", g_CurrentNumber);
			ret = 4;
			Err_display(26);
		}
	}
	//printf("the ret of checkfull is %d\n",ret);
	
	return ret;
}

//extern void RSA_TEST(void);
extern int M1CardSwipeStatus(void);
int main(int argc,const char** argv)
{
	static int textcmd=1;
	pthread_t pb;
	pthread_t pc;
	pthread_t pd;
	pthread_t pe;
	pthread_t pf;
    pthread_t pg;
    pthread_t ph;
	pthread_t pj;
	pthread_t pz;
	pthread_t pcc;
    pthread_t pa;
	unsigned char SwipeCardError = 0,lastRecordCount = 0;
	unsigned int i,dis_len,ret;
    int ret1;
	unsigned char status,len, stationup, upkey,upkey2, ReturnFlag = 0;
	char buff[30], Buffer[100],buff1[100];
    ShortUnon value; 
	DebugPrintf("\n");

	static unsigned char LastDisplayStatus = -1;	//��ʾ����״̬����ֹһֱˢ����������

	struct timeval now, lastTime;
	struct timeval first, second;
	
	pthread_attr_t attr;
	struct sched_param param;
	struct stat verStat;    
    
	InitDisplay();// ��ʾ��ʼ��
    DebugPrintf("\n");
	if(access("/var/run/back.bmp",0)==0)
        {
           // load_BMP("back.bmp",&_pix);
           //load_back_Img(0,0,background2,"back.bmp");
           ret=Load_Bmp_Img("/var/run/back.bmp",&bmppicture1);
		   if(ret!=0){
		   		printf("����ͼƬʧ��\n");
				free(bmppicture1.data);
				memset(&bmppicture1,0,sizeof(bmppicture1));
		   	}
           else{
		   	Eflag = 1;
           	}
        }
    else
	    {
	    	Eflag = 0;
			printf("ͼƬ������\n");
    	}
	
    if(access("/var/run/logo.bmp",0)==0)
        {
           // load_BMP("logo.bmp",&_pix1);
           //load_back_Img(0,0,background1,"logo.bmp");
           //Bflag = 1;
           init_backgroud();
        }
    else
		{
			textcmd=1;
			printf("����ͼƬ������\n");
    	}
	
	

    #ifdef NEW0409
	SetTextSize(48);
	SetTextColor(Color_white);
	TextOut(0,120, "���ڼ��س���...");
	//SetTextSize(48);
    #else
	SetTextSize(32);
	SetTextColor(Color_white);
	TextOut(60,120, "���ڼ��س���...");
	SetTextSize(16);
    #endif
	sprintf(Buffer,"�汾�ţ�");


	readgccdata(buff,3);
	GetDevVersion(buff);
	strcat(Buffer,buff);
	TextOut(0,180,Buffer);
    sleep(1);
    /***��ʼ�������ж��豸�����Լ���ʾ***/
	InitSystem();// ϵͳ���ݳ�ʼ��
	/******/
	InitData(); // SD��������ϵͳ��Ȩ��IP����
	Tunion = 0;
    /***
     ��Ҫ��Բ�ͬ��PSAM�����в���
    (��Ա���PSAM���ͽ�ͨ����PSAM���ֱ���м�⣬���߶�����ʱ����ΪPSAM��ͨ��)
    ***/
	//ret = InitPsam();	//PSAM����ʼ��
	ret = 0;
	memcpy(PsamNum_bak2,PsamNum,6);
    memset(Buffer,0,sizeof(Buffer));
    sprintf(Buffer,"�ն˻�:%08d",DevNum.i);
    Tunion = 1;
	if(ret != 0)
	{
		SetColor(Mcolor);
		SetTextColor(Color_white);
		SetTextSize(32);
		TextOut(100,35, "��ܰ��ʾ");
		TextOut(50,75,  "PSAM����λʧ��");
		TextOut(65,115, "����PSAM��");
        beepopen(3);
        Tunion = 0;
		beepopen(3);
		while(1)
		{
			sleep(2);
			beepopen(3);
		}    	
	}	


	ConnectFlag = 1;
	freedomflag = 1;
    GPSSIG = 1;    
	stationup = Section.Updown;
	//sem_init(&sem_sound, 0, 0);
	my_sem_init(&sem_sound, 0);
	//GetInputInfo();
	pthread_mutex_init(&m_sysfile,NULL);
	pthread_mutex_init(&m_datafile,NULL);
	pthread_mutex_init(&m_socketwrite,NULL);
	pthread_mutex_init(&m_ErrorRecFile,NULL);
	pthread_mutex_init(&m_ErrorList,NULL);
	pthread_mutex_init(&m_DisplaySingle,NULL);
    pthread_mutex_init(&m_Blacklist,NULL);

	#if RUSHAN_BUS
	pthread_mutex_init(&m_stationrecord,NULL);
	#endif
	
	// set thread priority
	pthread_attr_init(&attr);
	//pthread_attr_setschedpolicy(&attr, SCHED_RR);
	//param.sched_priority = 10;
	pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
	param.sched_priority = sched_get_priority_min(SCHED_FIFO);
	pthread_attr_setschedparam(&attr, &param);
	//pthread_create ( &pc, NULL, Sund_Pthread,"AVBC");				//�򿪼�дGPRS����
	pthread_create(&pc, &attr, Sund_Pthread, "AVBC");


	param.sched_priority = sched_get_priority_max(SCHED_OTHER);
	pthread_attr_setschedparam(&attr, &param);

    pthread_create ( &pg, &attr, Gps_Pthread,"AVBC");
    pthread_create ( &ph, &attr, sendGps_Pthread,"AVBC");

	pthread_create ( &pf, &attr, TimerTask, "AVBC");  
	pthread_create ( &pb, &attr, ReadGprs_Pthread, "AVBC");		//GPRS��ȡ����
	pthread_create ( &pd, &attr, Readsql_Pthread, "AVBC");		//�Զ��ϴ�����
#ifdef SUPPORT_QR_CODE
#if QR_CODE_USE_USBHID
	pthread_create(&pcc, &attr, UsbHid_Pthread, "AVBC");
#else
	pthread_create(&pcc, &attr, uart0_Pthread, "AVBC");
#endif
#endif

    pthread_create ( &pe, &attr, Readuart_Pthread,"AVBC"); 		// �������ݶԽӣ����ڱ�վ��

	memset(buff, 0, sizeof(buff)); 
	GetDisInfo(buff,sizeof(buff),&dis_len);
    /***
    ��ʾ��Ϣ��Ҫ�����������»���
    ***/
	CardTwo = 1;
	while(1)
	{	

		gettimeofday(&now, 0);

		if ((!RepairLastRecErrorFlag) &&(!SwipeCardError) && (((ReadcardFlag == 0) && ((now.tv_sec != lastTime.tv_sec) || (SaveNum.i != lastRecordCount)))||(1== CardTwo)))

        {		
        	gettimeofday(&now, 0);

			if(updatline_err==1)
				Err_display(27);
			else if(updatline_err==2)
				Err_display(28);

            if ((LCDKeepDisp)||(1== CardTwo))        
            {
#if 1
          if((((now.tv_sec - LCDKeepStartTime.tv_sec)*1000000 + (now.tv_usec-LCDKeepStartTime.tv_usec)) > 300000) || (1== CardTwo))    
#else
	    if((now.tv_sec - LCDKeepStartTime.tv_sec > LCD_KEEP_DISP_TIME) || (1== CardTwo))
#endif
		{
                	// ����������Ϣ��ʾ
                	ShowSwipeCardStatus(0);
                }
            }
                       
            if(LCDKeepDisp == 0)
            {
                memcpy(&lastTime, &now, sizeof(now));
                lastRecordCount = TransactionNum.i;

                ReadcardFlag = 0;
                SetTextColor(Color_white);
              #ifdef NEW0409
                SetTextSize(48);
               #else
                SetTextSize(24);
               #endif


                Display_signal(ConnectFlag);
                
                {
                    memset(Buffer,0,sizeof(Buffer));
					/*��ʾ��·����վ��*/
					memcpy(Buffer,filemp.linename,16);
                    /***
                  			  ���޸���ʾվ��
                  			  ***/
                    if(Section.Updown  == 0x00)
                    {
                        sprintf(Buffer+16,"   ���� վ��:%02d",Section.Sationdis);
                      //	sprintf(Buffer,"���� վ��:%02d",Section.Sationkey);
                     // sprintf(Buffer,"����");    //������ʾ
                    }
                    else
                    {
                        sprintf(Buffer+16,"   ���� վ��:%02d",Section.Sationdis);
                      // sprintf(Buffer,"���� վ��:%02d",Section.Sationkey);
                    }
                    #ifdef NEW0409
                        TextOut(0,75, Buffer);
                    #else
                        TextOut(100,25,Buffer);
                    #endif
                }

    	         #ifdef NEW0409
                    TextOut(0,144, buff);
                 #else    
                  //  TextOut(dis_len,70, buff);
                    /***
                    �޸���ʾƱ�ۻ���,���ӷ�������������ʽƱ������ʾ
                    ***/
                    unsigned char temp[20];           
                 // AnalysisSheetcheck();                
             
                 //   printf("�۸� = %d �̶��۸�:=%d \n",HostValue.i,Fixvalue.i);
        			memset(temp,0,sizeof(temp));
                    strcpy(temp,"Ʊ��:");
                   // value.i = 0;
                  //  memcpy(value.intbuf,flc0005.gbasicpice,2);
        			MoneyValue(temp+5,filemp.defaultbaseprice.i);
        			TextOut(0,70,temp);
                 #endif
				
				GetSectionKMFromPara(0,Section.Sationdis);
                #ifdef NEW0409
                  TextOut(0,210, "��ˢ��"); 
                #else
                  TextOut(0,110, GetOnOffInfo.On_Name);
                #endif                

                SetTextSize(16);               

                i = TransactionNum.i - 1;

                sprintf(Buffer,"�ܽ��״���:%06d", i);

                #ifdef NEW0409
                    TextOut(10,295, Buffer);
                #else 
                    TextOut(100,160,Buffer);
                #endif
                
#if 1
                memset(buff1, 0, sizeof(buff1));
                memset(Buffer, 0, sizeof(Buffer));
                if(Tunion)
               {
                sprintf(buff1,"%02x%02x%02x%02x%02x%02x",
                    PsamNum_bak2[0],PsamNum_bak2[1],PsamNum_bak2[2],PsamNum_bak2[3],PsamNum_bak2[4],PsamNum_bak2[5]);
                 memcpy(Buffer,"�ն˻�:",strlen("�ն˻�:"));
                strcat(Buffer,buff1);
                TextOut(20,185, Buffer); 
                memset(Buffer, 0, sizeof(Buffer));
                sprintf(Buffer,"�� ��:%08d",DevNum.i);
                      TextOut(180,185, Buffer);                
                }
                else
                {
                    sprintf(Buffer,"�ն˻�:%08d",DevNum.i);                    
                      TextOut(100,185, Buffer);
                   
                    }
                              
#else
                sprintf(Buffer,"�ն˻�:%08d",DevNum.i);
                #ifdef NEW0409
                  TextOut(345,295, Buffer);  
                #else
                  TextOut(100,185, Buffer);
                #endif
#endif

                 #ifdef NEW0409
                  TextOut(0,10,mk_time(Buffer)); 
                #else
                  TextOut(85,210,mk_time(Buffer));
                #endif                
                

            }
        }
		else
		{
		//	DebugPrintf("RepairLastRecErrorFlag = [0x%02X] SwipeCardError[0x%02X] ReadcardFlag[0x%02X]\n", RepairLastRecErrorFlag, SwipeCardError, ReadcardFlag);
		//	DebugPrintf("SaveNum.i[0x%02X]  lastRecordCount[0x%02X] CardTwo[0x%02X]\n",SaveNum.i,lastRecordCount,CardTwo);
		}



		ReadCardFirst = 0;
		gettimeofday(&first,0);
       // printf("#1#��ʼ��ʱ, time = [%d.%06d]\n", first.tv_sec, first.tv_usec);

SwipeCardAgain:

#ifdef SUPPORT_QR_CODE
	if (g_QRCodeRcvDataFg)
	{
		//g_QRCodeRcvDataFg = 0;
		status =  QR_CODE_STATUS;
	}
	else
#endif

        status = CardReset(Buffer,&len, 0);  		       
  		if(status == 0x08 || SwipeCardError)           
		{
			if(FindCardInListAndReadSectorData() == MI_OK)

			{
				DebugPrintf("status = 0x08\n");
				DebugPrintf("status = 0x%02X\n", status);
				
				if(ReadcardFlag == 0)
				{
					switch(CardLan.CardType)
					{
					case 0xd0:
						ioctl(mf_fd, RC531_HALT);
						ShowSwipeCardStatus(4);
						sleep(8);
						UPdata_usb();

						break;
					case 0xcc:
						ioctl(mf_fd, RC531_HALT);
						status = DiverCard();
						if(status == 0)
						{
							beepopen(2);
							ReturnDisplay(2);
						}

						break;
					default:
						
                        ret1 =CheckRecordFull();                      
					    if(ret1>1)
                           break;
                        DebugPrintf("Section.Enable  = %02x\n",Section.Enable );
						if(DriverCard.bGetCard ){
							memset(&DriverCard, 0,sizeof(DriverCard));
						}	
						memcpy(GetWord.longbuf, Sector.STwoOne, 4);

						M1CardSwipeStatus();
						#if 0
						if(Section.Enable!=0x55)
						{						
						
							status = ReadorRepairCard(); 	//��������							
							
							DebugPrintf("status = 0x%02X\n", status);
							if(status == 0)
							{	
								if (g_MaxPersonNumber)
									g_CurrentNumber++;
                                Save_RC531_NO_Handle();
								SwipeCardError = 0;
							}
							else if(SwipeCardError || (status == SWIPE_CARD_ERROR)||(status && RepairLastRecErrorFlag))
							{
								if(WaitForSwipeCardOrTimeOut(&SwipeCardError)  == MI_OK)
								 	goto SwipeCardAgain;
								printf("SwipeCardError = 0x%02X\n", SwipeCardError);
							}
                     
							else if(!RepairLastRecErrorFlag)
							{
								SwipeCardError = 0;
							}
						}
						else
						{						
							
							status = SectionFares();
							if(status ==MI_OK) //�ֶ�����
							{
								if (g_MaxPersonNumber)
									g_CurrentNumber++;
								DebugPrintf("\n");
                                Save_RC531_NO_Handle();
								SwipeCardError = 0;
							}								
							else if( SwipeCardError || (status == SWIPE_CARD_ERROR) || (status && RepairLastRecErrorFlag))
							{
								if(WaitForSwipeCardOrTimeOut(&SwipeCardError)  == MI_OK)
								 	goto SwipeCardAgain;
							}
							else if(!RepairLastRecErrorFlag)
							{
								SwipeCardError = 0;
							}
							DebugPrintf("status = 0x%02X\n", status);
						}
					#endif
						break;
					}
				}
				else
				{
					// F2 ��ѯ��Ƭ��Ϣ
					ReadcardFlag = 0;
					status = RepairCardInfor(); 			//�޸�
					if(status == MI_OK)
					{
						ioctl(mf_fd, RC531_HALT);
						beepopen(2);
						ReturnDisplay(1);
					}
				}
			}
			else
			{
				DebugPrintf("SwipeCardError = 0x%02X\n", SwipeCardError);
				
				if(SwipeCardError)
				{
					PlayMusic(15, 0);
					
					if(WaitForSwipeCardOrTimeOut(&SwipeCardError)  == MI_OK)
					 	goto SwipeCardAgain;
				}
			}
			gettimeofday(&second, NULL);
            printf("#1#��ʼ��ʱ, time = [%d.%06d]\n", first.tv_sec, first.tv_usec);
            printf("#1#������ʱ, time = [%d.%06d]\n", second.tv_sec, second.tv_usec);
			printf("#2#���׺�ʱ, time = [%d.%06d]\n", (second.tv_sec - first.tv_sec)*1000000 + (second.tv_usec-first.tv_usec),((second.tv_sec - first.tv_sec)*1000000 + (second.tv_usec-first.tv_usec))/1000);

		}

/*---------------------------------------------------start here--------------------------------*/
		else if((status == 0x20)/*&&(Section.Enable != 0x55)*/)
		{
			DBG_PRINTF("CardReset() returns 0x20.\n");			
		//	if(CheckRecordFull()!=1)        //��ѯ���д�Ͳ��ܼ���ˢ��
		    ret1 =CheckRecordFull();
			if(ret1>1)
                  continue;
			
			if(ReadcardFlag == 0)
			{       				
				//gettimeofday(&first,0);
				{
                      
					if(Section.Enable != 0x55)
					{
						status = ReadorRepairCard_CPU();		//һƱ��
						if(status == 0)
						{
							if (g_MaxPersonNumber)
								g_CurrentNumber++;
							LEDR(1);
							beepopen(2);
							ReturnDisplay(0);
							LEDR(0);
                            Save_RC531_NO_Handle();
						}						
					}
					
					else
					{
						status = ComplexConsumption();	     //�ֶ�
						if(status == 0)
						{
							if (g_MaxPersonNumber)
								g_CurrentNumber++;
							LEDR(1);
							beepopen(2);
							ReturnDisplay(3);
							LEDR(0);
                            Save_RC531_NO_Handle();
						}						
					}
				}	
			
				MakeSureCardLeave();

        gettimeofday(&second, NULL);
        printf("#1#��ʼ��ʱ, time = [%d.%06d]\n", first.tv_sec, first.tv_usec);
        printf("#1#������ʱ, time = [%d.%06d]\n", second.tv_sec, second.tv_usec);
		printf("#2#���׺�ʱ, time = [%d.%06d]\n", (second.tv_sec - first.tv_sec)*1000000 + (second.tv_usec-first.tv_usec),((second.tv_sec - first.tv_sec)*1000000 + (second.tv_usec-first.tv_usec))/1000);
			}
			else
			{
				ReadcardFlag = 0;
				status = ReadCardInfor_CPU();
				if(status == 0)
				{
					if (g_MaxPersonNumber)
						g_CurrentNumber++;
					ioctl(mf_fd, RC531_HALT);
					beepopen(2);
					ReturnDisplay(2);
				}
			}
		}	
#ifdef SUPPORT_QR_CODE
		else if (status == QR_CODE_STATUS)
		{
			//RSA_TEST();
			#ifdef OFFLINE_TEST_QRCODE
			beepopen(2);
			Sector.FlagValue = 2; 
			CardLan.CardType = 1;
			HostValue.i = 0;
			memset(CardLan.QCash, 0, 4); 
			memset(CardLan.CardCsnB, 0, 4); 
			//AnalysisSheet(1);
			//CardLan.CardType = 1;
			//memset(CardLan.Views, 0, 4);
			ReturnDisplay(0);
			sleep(1);
			g_QRCodeRcvDataFg = 0;
			#else
			status = QRCodeProcessConsume(0);
			QRCodeScanOutPut(status);			
			#endif
			#ifdef TEST_QR_CODE_SPEED
			   gettimeofday(&second_test, NULL);
          		//  printf("#QR#��ʼ��ʱ, time = [%d.%06d]\n", first.tv_sec, first.tv_usec);
           		// printf("#QR#������ʱ, time = [%d.%06d]\n", second.tv_sec, second.tv_usec);
			//printf("#QR#���׺�ʱ, time = [%04d]ms\n", (second.tv_sec - first.tv_sec)*1000  + (second.tv_usec-first.tv_usec)/1000);
			TotalTestCnt++;
			//TotalTestTime = (second_test.tv_sec - first.tv_sec)*1000  + (second_test.tv_usec-first.tv_usec)/1000;
			TotalTestTime = second_test.tv_sec;
			TotalMsTime = second_test.tv_usec/1000;
			memset(buff1, 0, sizeof(buff1));
			sprintf(buff1, "%d, %04ds, %04dms\r\n", TotalTestCnt, TotalTestTime, TotalMsTime);
			QRTest_fd = open("/mnt/nand1-2/app/qr_speed.txt",O_SYNC|O_RDWR|O_APPEND);
			if (QRTest_fd > 0)
			{
				write(QRTest_fd, buff1, strlen(buff1));
				close(QRTest_fd);
			}
			QRTest_fd = -1;
			#endif

		}
#endif
		else if(status > 1)
		{
			DBG_PRINTF("CardReset() returns %d.\n", status);
		}

		if(CardTwo == 0)
		{
			upkey2 = updatekey_2();
			
			if(upkey2 != 0xff)
			{
				switch(upkey2)
				{
					case SCANCODE2_OK:
						printf("SCANCODE2_OK \n");
            			nowye=1;
						break;
					case SCANCODE2_C:
						printf("SCANCODE2_C \n");
            			nowye=1;
						break;
					case SCANCODE2_UP:
						printf("SCANCODE2_UP \n");
            			if((TransactionNum.i-1)!=0)
						{
							myflag=0;
							nowye--;
							if(nowye<=0)
						    	nowye=1;
							DisConsumption();	//��ʾ������Ϣ
            			}
                        else
						{
							myflag=0;
							nowye=1;
						}
						break;
					case SCANCODE2_DOWN:	
						printf("SCANCODE2_DOWN \n"); 
                       	if((TransactionNum.i-1)!=0)
						{
							DisConsumption();
							nowye++;
                            if(nowye>=sumye)
                             nowye = sumye;
			                    }
                         else
                         {
                           		 myflag=0;
                           		 nowye=1;
                         }
						break;
					default :
						break;
				}
			}
			//gettimeofday(&first, NULL);
			//printf("#3#main, time = [%d.%06d]\n", first.tv_sec, first.tv_usec);
            /***
            ��Ҫʹ�ü����л���·������Э�����
            ***/
			upkey = updatekey();
			//printf("_____++++++___the keycode :%03d ___++++++_____\n",upkey);
			if(upkey != 0xff)
			{
			  
				if(ReadcardFlag == 0)
				{
					switch(upkey)
					{
#if __KEYBOARD_1
					case SCANCODE_PERIOD:
#elif __KEYBOARD_2
					case SCANCODE2_FUN1:
#endif
						PlayMusic(21,0);
						SetMaxPersonNum();
						CardTwo =1;
						break;
#if __KEYBOARD_1
					case SCANCODE_F1:
#elif	__KEYBOARD_2
					case SCANCODE_F3:
#endif						

#if  UartDis
						printf("SCANCODE_F1 \n");
#endif
						#if 0
						g_FgSetPersonMode = 0; //first from zore
						line_starts:
							Freedom();
						if (g_FgSetPersonMode)
						{
							SetMaxPersonNum();
							if (!g_FgSetPersonMode) goto line_starts;
						}
						#else
						//��������
						PlayMusic(23, 0);
						Freedom();
						#endif
                        			CardTwo =1;
						break;

#if __KEYBOARD_1
					case SCANCODE_F2:
#elif __KEYBOARD_2
					case SCANCODE2_FUN2:
#endif						

#if  UartDis
						printf("SCANCODE_F2 \n");
#endif
			   			PlayMusic(20, 0);
						ReadcardFlag = 1;
						SetColor(Mcolor);
                        #ifdef NEW0409
                        SetTextSize(48);
						SetTextColor(Color_white);
						TextOut(0,70, "��ˢ��");
						TextOut(0,125,"��ѯ��Ƭ��Ϣ ");
                        #else
						SetTextSize(32);
						SetTextColor(Color_white);
						TextOut(110,70, "��ˢ��");
						TextOut(65,125,"��ѯ��Ƭ��Ϣ ");
                        #endif
						break;
#if __KEYBOARD_1
					case SCANCODE_F3:
#elif	__KEYBOARD_2
					case SCANCODE2_FUN3:
#endif

#if  UartDis
						printf("SCANCODE_F3 \n");
#endif
						PlayMusic(24, 0);
						CheckTime();
						CardTwo =1;
						break;

#if __KEYBOARD_1
					case SCANCODE_F4:
#elif __KEYBOARD_2
					case SCANCODE2_CHL:
#endif						

						// �л�������
#if  UartDis
						printf("SCANCODE_F4 \n");
#endif

#if RUSHAN_BUS
						if(Section.Enable == 0x55)
						{
							stationup++;
							//SectionSta((stationup%2),1);
							if(Section.Updown==0){
								SectionSta(1,1);
							#if 0	
								if(Section.Enableup != 0x55)
									Section.Sationdis = Section.SationNum[0];
								else
									Section.Sationdis = Sectionup.SationNum[0];
							#endif
								Section.Sationkey=SectionNum;//����
								Section.Sationdis=SectionNum;
								
								//Section.Sationkey = Section.Sationdis;
							}
							else
							{
								SectionSta(0,1);
								Section.Sationdis = 1;
								Section.Sationkey=1;
								
								//printf("Section.Sationdis: %d\n",Section.Sationdis);
							}
						}		

#else
						if(Section.Enable == 0x55)
						{
							stationup++;					//��ż�ֱ����������
							SectionSta((stationup%2),1);
						}
						else
						{
						 //   SetColor(Mcolor);
						//	SectionApp();
						//	CardTwo =1;
						}
#endif						
						break;
#if __KEYBOARD_1

					case SCANCODE_F5:
#elif __KEYBOARD_2
					case SCANCODE2_PRE:
#endif
						// �л�վ̨����һվ
#if  UartDis
						printf("SCANCODE_F5 \n");
#endif

#if RUSHAN_BUS
					if(Section.Enable == 0x55)
						{
							if(Section.Updown==0){
								if(Section.Sationkey>1)
									Section.Sationkey--;
								SectionSta(Section.Sationkey-1,0);
							}
							else{
								if(Section.Sationkey<SectionNum)
									Section.Sationkey++;
								SectionSta(SectionNum-Section.Sationkey,0);
							}							
						}

#else
						if(Section.Enable == 0x55)
						{
							Section.Sationkey--;
							Section.Sationkey = SectionSta(Section.Sationkey,0);
						}
#endif

						break;

#if __KEYBOARD_1
					case SCANCODE_F6:
#elif __KEYBOARD_2
					case SCANCODE2_NEXT:
#endif

						// �л�վ̨����һվ
#if  UartDis
						printf("SCANCODE_F6 \n");
#endif

#if RUSHAN_BUS
					if(Section.Enable == 0x55)
						{
							if(Section.Updown==0){
								if(Section.Sationkey<SectionNum)
							
									Section.Sationkey++;
							
								SectionSta(Section.Sationkey-1,0);
							}
							else{
								if(Section.Sationkey>1)
							
									Section.Sationkey--;
								SectionSta(SectionNum-Section.Sationkey,0);
							}							
						}
#else
						if(Section.Enable == 0x55)
						{
							Section.Sationkey++;
							Section.Sationkey = SectionSta(Section.Sationkey,0);
						}
#endif						
						break;

#if Yantai_Qixia
				case SCANCODE2_FUN5:
						//�л���·
						ReadcardFlag = 0;
						ChangeLine();
						CardTwo =1;
						break;
#endif


				/*�趨�̶�����ֵ*/


					case SCANCODE2_FUN6:
						SetFixValue();
						CardTwo =1;
						break;

					case SCANCODE_ESCAPE:
					
						ReadcardFlag = 0;
						SetColor(Mcolor);
						CardTwo =1;
						break;
					case SCANCODE_ENTER:
                        #ifdef NEW0409
                        SetColor(Mcolor);
						SetTextSize(48);
						SetTextColor(Color_white);
						
						memset(Buffer, 0, sizeof(Buffer));
						stat("/mnt/nand1-1/conprog.bin", &verStat);										
						sprintf(Buffer,"�ں˰汾:%u", verStat.st_size);
						TextOut(0,35, Buffer);

						memset(Buffer, 0, sizeof(Buffer));
						stat("/mnt/nand1-1/drivers/typea.ko", &verStat);										
						sprintf(Buffer,"��ͷ�汾:%u", verStat.st_size);
						TextOut(0,90, Buffer);

						memset(Buffer, 0, sizeof(Buffer));
						stat("/mnt/nand1-2/app/armok", &verStat);										
						sprintf(Buffer,"Ӧ�ð汾:%u", verStat.st_size);
						TextOut(0,155, Buffer);
                        #else
						SetColor(Mcolor);
						SetTextSize(32);
						SetTextColor(Color_white);
						
						memset(Buffer, 0, sizeof(Buffer));
						stat("/mnt/nand1-1/conprog.bin", &verStat);										
						sprintf(Buffer,"�ں˰汾:%u", verStat.st_size);
						TextOut(2,35, Buffer);

						memset(Buffer, 0, sizeof(Buffer));
						stat("/mnt/nand1-1/drivers/typea.ko", &verStat);										
						sprintf(Buffer,"��ͷ�汾:%u", verStat.st_size);
						TextOut(2,70, Buffer);

						memset(Buffer, 0, sizeof(Buffer));
						stat("/mnt/nand1-2/app/armok", &verStat);										
						sprintf(Buffer,"Ӧ�ð汾:%u", verStat.st_size);
						TextOut(2,105, Buffer);
                        #endif
						while(1)
						{
							upkey = updatekey();
							if(upkey!=0xff)
								break;
						}
						SetColor(Mcolor);
						break;

					default :
#if  UartDis
						printf("SCANCODE  %d \n",upkey);
#endif
						CardTwo =1;
						break;
					}
				}
				else
				{
					if( upkey == SCANCODE_ESCAPE)
					{
						ReadcardFlag = 0;
						SetColor(Mcolor);
						CardTwo =1;
					}
				}
			}
		}
		//gettimeofday(&first, NULL);
		//printf("#4#main, time = [%d.%06d]\n", first.tv_sec, first.tv_usec);

		usleep(10000);
	}	//while(1)

	//reLond_BMP(signal_bmp);

	free(CardLanBuf);
	CloseDisplay();
	closekey();
    close_pthread();
	pthread_join (pd, NULL);
	pthread_join (pb, NULL);
	pthread_join (pc, NULL);
	pthread_join (pe, NULL);
    pthread_join (pf, NULL);
    pthread_join (pg, NULL);
    pthread_join (ph, NULL);
    pthread_join (pa, NULL);
	#ifdef SUPPORT_QR_CODE
	pthread_join(pcc, NULL);
	#endif


	return 0;

}




// ��ʾ˾����ѯ����
extern int GetUserRec(struct USER_RESERCH *pUserVal);
extern int GetTotalConsumption(unsigned int *ptotal);
extern struct DRIVER_SEARCH SearchNextPage;



