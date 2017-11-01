#include <apparel.h>
#include "RC500.h"
#include "cpucard.h"
#include "InitSystem.h"
#include "des.h"
#include "OnlineRecharge.h"
#include "../Display/fbtools.h"
#include "../sound/sound.h"

// ������ϪPBOC���ڿ�M1�����û���������
#define YUXI_PBOC_USER_INFO_SECTOR	1

extern unsigned char updatline_err;
extern int mf_fd;
extern unsigned char DevSerialID[4];
extern unsigned char DayMaxCount;
extern unsigned char AutoUpFlag;
extern unsigned char SavedataErr;
extern unsigned char CardTwo;  //���±�־
extern unsigned char ReadCardFirst;

extern RouteJack TypeTime;
extern CardLanSector LanSec;   //�û�����
extern FixTimerBuf FixTime;
extern LongUnon DevSID;
extern LongUnon TransactionNum;
extern LongUnon DecValue;
extern LongUnon HostValue;
extern LongUnon Fixvalue;
extern LongUnon DevNum,DayMaxMoney;	//�ն˻�����
extern unsigned short SectionNum;
extern RecordFormat SaveData;
extern unsigned char bStolenDis ;
extern unsigned char StolenAmount;
extern unsigned char RepairLastRecErrorFlag;
extern pthread_mutex_t m_ErrorRecFile; //
extern pthread_mutex_t m_ErrorList; //

extern int mf_fd;
extern int bp_fd;
extern unsigned char LCDKeepDisp;
extern unsigned char  ReadcardFlag;

static LongUnon JackArm,Test,Buf;
unsigned char SnBack[4] = {0x26,0x91,0x13,0x00};
unsigned char XFBuf1[16],XFBuf2[16];
unsigned char BFlaglessfive;
unsigned char KeyDes[8];	//��Կ
struct card_buf  test;
CardInform CardLan;
JackRegal Sector;
JackValue MoneyNo;
SysTime Time;
SectionFarPar Section,Sectionup;
unsigned char CardTypeIC;
struct ErrorRecordLsit *pErrorRecHead = NULL;

static unsigned int readcardnum = 0;
unsigned char UseForTest = 0;
char G_RC531_NO[16];
unsigned char G_GET531_flag = 0;	/* ��ȡRC531���кŵı�ʶ 0:δ��ȡ����1:��ȡ�� */
unsigned char G_Read531fg = 0;
char StuToOrd = 0;//0-����ѧ����ת��ͨ������,1-��ѧ��������ͨ������

extern unsigned char g_FgWriteNoDiscount;
extern unsigned char g_FgNoDiscount;

#ifdef QINGDAO_TONGRUAN
LongUnon g_DisplayMoney;
unsigned char g_SepcialDiscount = 0;
LongUnon g_DiscountOfMoney;
unsigned char g_FindTheDiscount = 0;
#endif

extern struct RechargeInfo tempRechargedata;


extern CardInformCPU CardLanCPU;

unsigned char g_ReadSevIndex = 0;
unsigned char g_FlagMonCard = 0;

LongUnon g_YuChongBackUp;
unsigned char g_FgOprationSuccess;

struct UserCard_Struct UserCard;
struct UserCard_Key	UserKey;
struct OperatCard_Struct OperCard;
LongUnon WatcherCard;
LongUnon SeDriverCard;
ShortUnon RegisterMoney;
unsigned char g_UseRegisterMoney;
CardRate_local LocalCardRate[CARD_NUMBER];
FileM5_T CardConParam[CARD_NUMBER];

extern unsigned char PsamNum[6];
extern unsigned char *StationdisupParBuf;
extern unsigned char *StationdisdownParBuf;
struct GetOnOff_Struct GetOnOffInfo;
unsigned char g_CardCurrentMode = USECARD_MODE;


#if 1
void print_debug(unsigned char *tips,const unsigned char *data , const unsigned int dataLen)
{
	unsigned int i;

	printf("%s\n",tips);
	for(i=0;i<dataLen;i++)
		printf("%02x ",data[i]);
	printf("\n");
}

/*****************************************
ASCII �� HEX��ת������
��ڲ����� O_data: ת�����ݵ����ָ��
N_data: ת���������ݵ����ָ��
len : ��Ҫת���ĳ���
���ز�����-1: ת��ʧ��
������ת�������ݳ���
ע�⣺O_data[]�����е�������ת�������лᱻ�޸ġ�
****************************************/
int ascii_2_hex(unsigned char *O_data, unsigned char *N_data, int len)
{
	int i,j,tmp_len;
	unsigned char tmpData;
	unsigned char *O_buf = O_data;
	unsigned char *N_buf = N_data;
	for(i = 0; i < len; i++)
	{
		if ((O_buf[i] >= '0') && (O_buf[i] <= '9'))
		{
			tmpData = O_buf[i] - '0';
		}
		else if ((O_buf[i] >= 'A') && (O_buf[i] <= 'F')) //A....F
		{
			tmpData = O_buf[i] - 0x37;
		}
		else if((O_buf[i] >= 'a') && (O_buf[i] <= 'f')) //a....f
		{
			tmpData = O_buf[i] - 0x57;
		}
		else
		{
			//Uart_Printf(0,"i = %02X \r\n",i);
			return 0xff;
		}
		O_buf[i] = tmpData;
	}
	for(tmp_len = 0,j = 0; j < i; j+=2)
	{
		N_buf[tmp_len++] = (O_buf[j]<<4) | O_buf[j+1];
	}
	return tmp_len;
}

void myitoa(char *s, unsigned int n)
{
	char tmp, *str;
	if (!n) {
		*s++=0x30;
		*s=0;
		return;
	}
	for (str=s; n; *s++='0'+ n%10, n/=10);
	for (*s--=0,n=strlen(str); n/2; n-=2,\
	        tmp=*str,*str++=*s,*s--=tmp);
}
/*
*************************************************************************************************************
- �������� : void MoneyValue(INT8U *OUT_Data,unsigned int Money)
- ����˵�� : ת���ɽ�Ǯ��ʽ
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
void MoneyValue(unsigned char *OUT_Data,unsigned int Money)
{
	unsigned char wa,wb,wc,i;
	if(Money > 2147483647)
	{
		Money = ~ Money;
		Money ++;
		myitoa(OUT_Data, Money);
		if(Money < 10)
		{
			wa =  OUT_Data[0];
			OUT_Data[0] = '-';
			OUT_Data[1] = '0';
			OUT_Data[2] = '.';
			OUT_Data[3] = '0';
			OUT_Data[4] =  wa;
		}
		else if((Money >=10)&&(Money < 100))
		{
			wa =  OUT_Data[0];
			wb =  OUT_Data[1];
			OUT_Data[0] = '-';
			OUT_Data[1] = '0';
			OUT_Data[2] = '.';
			OUT_Data[3] =  wa;
			OUT_Data[4] =  wb;
		}
		else
		{
			wa = strlen(OUT_Data);
			wb =  OUT_Data[wa-2];
			wc =  OUT_Data[wa-1];
			OUT_Data[wa-2] = '.';
			OUT_Data[wa-1] =  wb;
			OUT_Data[wa]   =  wc;
			for(i = wa+1; i > 0; i--)
			{
				OUT_Data[i] = OUT_Data[i-1];
			}
			OUT_Data[0] = '-';
		}
		strcat(OUT_Data,"Ԫ");
	}
	else
	{
		myitoa(OUT_Data, Money);
		if(Money < 10)
		{
			wa =  OUT_Data[0];
			OUT_Data[0] = '0';
			OUT_Data[1] = '.';
			OUT_Data[2] = '0';
			OUT_Data[3] =  wa;
		}
		else if((Money >=10)&&(Money < 100))
		{
			wa =  OUT_Data[0];
			wb =  OUT_Data[1];
			OUT_Data[0] = '0';
			OUT_Data[1] = '.';
			OUT_Data[2] =  wa;
			OUT_Data[3] =  wb;
		}
		else
		{
			wa = strlen(OUT_Data);
			wb =  OUT_Data[wa-2];
			wc =  OUT_Data[wa-1];
			OUT_Data[wa-2] = '.';
			OUT_Data[wa-1] =  wb;
			OUT_Data[wa]   =  wc;
		}
		strcat(OUT_Data,"Ԫ");
	}
}

void MoneyValue1(unsigned char *OUT_Data,unsigned int Money)
{
	unsigned char wa,wb,wc,i;
	if(Money > 2147483647)
	{
		Money = ~ Money;
		Money ++;
		myitoa(OUT_Data, Money);
		if(Money < 10)
		{
			wa =  OUT_Data[0];
			OUT_Data[0] = '-';
			OUT_Data[1] = '0';
			OUT_Data[2] = '.';
			OUT_Data[3] = '0';
			OUT_Data[4] =  wa;
		}
		else if((Money >=10)&&(Money < 100))
		{
			wa =  OUT_Data[0];
			wb =  OUT_Data[1];
			OUT_Data[0] = '-';
			OUT_Data[1] = '0';
			OUT_Data[2] = '.';
			OUT_Data[3] =  wa;
			OUT_Data[4] =  wb;
		}
		else
		{
			wa = strlen(OUT_Data);
			wb =  OUT_Data[wa-2];
			wc =  OUT_Data[wa-1];
			OUT_Data[wa-2] = '.';
			OUT_Data[wa-1] =  wb;
			OUT_Data[wa]   =  wc;
			for(i = wa+1; i > 0; i--)
			{
				OUT_Data[i] = OUT_Data[i-1];
			}
			OUT_Data[0] = '-';
		}
	}
	else
	{
		myitoa(OUT_Data, Money);
		if(Money < 10)
		{
			wa =  OUT_Data[0];
			OUT_Data[0] = '0';
			OUT_Data[1] = '.';
			OUT_Data[2] = '0';
			OUT_Data[3] =  wa;
		}
		else if((Money >=10)&&(Money < 100))
		{
			wa =  OUT_Data[0];
			wb =  OUT_Data[1];
			OUT_Data[0] = '0';
			OUT_Data[1] = '.';
			OUT_Data[2] =  wa;
			OUT_Data[3] =  wb;
		}
		else
		{
			wa = strlen(OUT_Data);
			wb =  OUT_Data[wa-2];
			wc =  OUT_Data[wa-1];
			OUT_Data[wa-2] = '.';
			OUT_Data[wa-1] =  wb;
			OUT_Data[wa]   =  wc;
		}
	}
}

/*
*************************************************************************************************************
- �������� : void Err_save (unsigned char number)
- ����˵�� : ˢ������󱣴�����¼
- ������� : ������
- ������� : ��
*************************************************************************************************************
*/
void Err_save(unsigned char cardtype,unsigned char number)
{
#if 0
        
    unsigned char status;
    IncSerId();
	status = SaveCardData_error(cardtype,number, GET_RECORD | SAVE_RECORD); //��������  ����
	if(status != MI_OK) 
	{
		Err_display(20);

	}
    AutoUpFlag = 0x55;
#endif
}


/*
*************************************************************************************************************
- �������� : void Err_display (HWND hDlg,unsigned char Mode)
- ����˵�� :
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
void Err_display(unsigned int messcode)
{
	int musicIndex = 0;

	DBG_RC500_PRINTF("Err_display() is called, code = %d.\n", messcode);

	LEDL(1);
	SetColor(Mcolor);
	SetTextColor(Color_red);
#ifdef NEW0409  
    char x1 = 0;
    char y1[]={75,144,210};   
    char y2[]={50,110,170,230};
	SetTextSize(48);
switch(messcode)
	{
	case 1:
		TextOut(x1 , *(y1+0), "��Ƭ����");
		TextOut(x1 , *(y1+1),  "��Ƭ��Ч���ѹ���");
		TextOut(x1 , *(y1+2),"ллʹ��");
		break;
	case 2:
		TextOut( x1 , *(y2+0), "��Ƭ����");
		TextOut( x1 , *(y2+1), "��Ƭ��ʽ����");//
		TextOut( x1 , *(y2+2), "��Ǳ�ϵͳ��Ƭ");
		TextOut( x1 , *(y2+3),"ллʹ��");
		break;
	case 3:
		TextOut( x1 , *(y1+0), "��Ƭ����");
		TextOut( x1 , *(y1+1), "Ǯ����ʽ����");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;
	case 4:
		TextOut( x1 , *(y1+0), "��Ƭ����");
		TextOut( x1 , *(y1+1), "MAC����");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;
	case 5:
		TextOut( x1 , *(y1+0), "��Ƭ����");
		TextOut( x1 , *(y1+1), "��Ƭ��ʽ���̱�־����");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;
	case 6:
		TextOut( x1 , *(y1+0), "��Ƭ����");
		TextOut( x1 , *(y1+1),"�ֽ�Ǯ������");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;
	case 7:
		TextOut( x1 , *(y1+0), "������");
		TextOut( x1 , *(y1+1), "��Ƭ������");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;

	case 9:
		TextOut( x1 , *(y2+0), "ʱ�δ���");
		TextOut( x1 , *(y2+1), "����ʱ�䲻");//������ʱ����
		TextOut( x1 , *(y2+2),"������ʱ����");
		TextOut( x1 , *(y2+3),"ллʹ��");
		break;
	case 10:
		TextOut( x1 , *(y1+0), "��Ƭ����");
		TextOut( x1 , *(y1+1), "�˿�ƬΪ��������Ƭ");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;
	case 11:
		TextOut( x1 , *(y2+0), "��Ƭ����");
		TextOut( x1 , *(y2+1), "ϵͳ��֧��");//
		TextOut( x1 , *(y2+2),"�˿�������");//
		TextOut( x1 , *(y2+3),"ллʹ��");//
		break;
	case 12:
		TextOut( x1 , *(y2+0), "����ʧ��");
		TextOut( x1 , *(y2+1),  "���Ŀ�Ƭ����");
		TextOut( x1 , *(y2+2), "֧���������ѽ��");
		TextOut( x1 , *(y2+3),"ллʹ��");
		break;
	case 13:
		TextOut( x1 , *(y2+0), "����ʧ��");
		TextOut( x1 , *(y2+1),  "��ǰʱ�����ѽ��");
		TextOut( x1 , *(y2+2), "����������Ѷ��");
		TextOut( x1 , *(y2+3),"ллʹ��");
		break;
	case 14:
		TextOut( x1 , *(y2+0), "����ʧ��");
		TextOut( x1 , *(y2+1),  "�������ѽ������");
		TextOut( x1 , *(y2+2), "����������Ѷ��");
		TextOut( x1 , *(y2+3),"ллʹ��");
		break;

	case 15:
		TextOut( x1 , *(y1+0), "����ʧ��");
		TextOut( x1 , *(y2+1), "�������ѽ������");
		TextOut( x1 , *(y2+2), "����������Ѷ��");
		TextOut( x1 , *(y2+3),"ллʹ��");
		break;

	case 16:
		TextOut( x1 , *(y1+0), "����ʧ��");
		TextOut( x1 , *(y1+1), "���������");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;

	case 17:
		TextOut( x1 , *(y1+0), "����ʧ��");
		TextOut( x1 , *(y1+1), "����ʱ�伺��");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;

	case 18:
		TextOut( x1 , *(y2+0), "����ʧ��");
		TextOut( x1 , *(y2+1), "���콻�״�������");
		TextOut( x1 , *(y2+2), "����������ƴ���");
		TextOut( x1 , *(y2+3),"ллʹ��");
		break;
	case 19:
		TextOut( x1 , *(y1+0),"����ʧ��");
		TextOut( x1 , *(y1+1),"�洢�ļ�·������");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;
	case 20:
		TextOut( x1 , *(y1+0), "����ʧ��");
		TextOut( x1 , *(y1+1), "�ڴ�ָ�����");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;

	case 21:
		TextOut( x1 , *(y1+0), "����ʧ��");
		TextOut( x1 , *(y1+1), "��¼�ڴ�����");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;

	case 22:
		TextOut( x1 , *(y1+0),"����ʧ��");
		TextOut( x1 , *(y1+1), "����������ʹ��ʱ����");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;


	case 23:
		TextOut( x1 , *(y1+0),"����ʧ��");
		TextOut( x1 , *(y1+1), "��֤������Կʧ��");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;
	case 24:
		TextOut( x1 , *(y1+0), "��ܰ��ʾ");
		TextOut( x1 , *(y1+1), "ˢ������");
		TextOut( x1 , *(y1+2),"������ˢ��");	
		break;	

	case 25:
		TextOut( x1 , *(y1+0),"����ʧ��");
		TextOut( x1 , *(y1+1), "�洢�ļ�Ŀ¼����");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;

	case 26:
		TextOut( x1 , *(y1+0),"����ʧ��");
		TextOut( x1 , *(y1+1), "��Ա����");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;
	case 27: 
			TextOut(x1,100, "��·���³ɹ�");
			//beepopen(2);
			printf("now the line is %d\n",Section.Linenum);
			updatline_err=0;
			break;
	case 28:			
		TextOut(x1,100, "��·����ʧ��");
		//beepopen(2);
		updatline_err=0;
			break;

	case 29:
		TextOut( x1 , *(y1+0),"����ʧ��");
		TextOut( x1 , *(y1+1), "�ݲ�֧�ִ˳�������");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;

	case 30:
		TextOut( x1 , *(y1+0), "��Ƭ����");
		TextOut( x1 , *(y1+1),  "δ��ͨ������ͨӦ��");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;
    case 31:
		TextOut( x1 , *(y1+0), "��Ƭ����");
		TextOut( x1 , *(y1+1), "��Կ�汾����");
		TextOut( x1 , *(y1+2),"ллʹ��");
        break;     

    case 32:
		TextOut( x1 , *(y1+0), "��Ƭ����");
		TextOut( x1 , *(y1+1), "��ȫ��֤����");
		TextOut( x1 , *(y1+2),"ллʹ��");
        break;          

	case 40:
		TextOut( x1 , *(y1+0), "��ܰ��ʾ");
		TextOut( x1 , *(y1+1),  "����״̬��ȷ��");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;
		
	 case 41:
        TextOut( x1 , *(y1+0),"����ʧ��");
        TextOut( x1 , *(y1+1), "����Ƭ������Ϣʧ��");
        TextOut( x1 , *(y1+2),"ллʹ��");
        break;

	case 42:
        TextOut( x1 , *(y1+0),"����ʧ��");
        TextOut( x1 , *(y1+1), "����Ƭ������Ϣʧ��");
        TextOut( x1 , *(y1+2),"ллʹ��");
        break;	
	case 43:
		TextOut( x1 , *(y1+0),"����ʧ��");
		TextOut( x1 , *(y1+1), "��������ϸ�ļ�ʧ��");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;
	case 44:
		TextOut( x1 , *(y1+0), "����ʧ��");
		TextOut( x1 , *(y1+1), "��¼�ļ���ʧ��");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;
		
	case 45:
		TextOut( x1 , *(y1+0), "��Ƭ����");
		TextOut( x1 , *(y1+1), "��ƬӦ��δ��ͨ");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;
        
		
	case 46:
		TextOut( x1 , *(y1+0), "��Ƭ����");
		TextOut( x1 , *(y1+1),  "ѭ����¼��ʧ��");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;	

    case 47:
        TextOut( x1 , *(y1+0), "��Ƭ����");
		TextOut( x1 , *(y1+1),  "����Ǯ��Ӧ������");
		TextOut( x1 , *(y1+2),"ллʹ��");
        break;

    case 48:
        TextOut( x1 , *(y1+0), "��Ƭ����");
		TextOut( x1 , *(y1+1),  "��Ƭ���ڸ�ʽ�쳣");
		TextOut( x1 , *(y1+2),"ллʹ��");
        break; 
        
    case 49:
        TextOut( x1 , *(y1+0), "��Ƭ����");
		TextOut( x1 , *(y1+1),  "��Ƭδ����������");
		TextOut( x1 , *(y1+2),"ллʹ��");
        break; 
        
	case 50:
		TextOut( x1 , *(y1+0),"����ʧ��");
		TextOut( x1 , *(y1+1), "û���ϳ���־");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;

	case 51:
		TextOut( x1 , *(y1+0),"����ʧ��");
		TextOut( x1 , *(y1+1), "�˿����³�");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;
        
    case 52:
        TextOut( x1 , *(y1+0), "��Ƭ����");
		TextOut( x1 , *(y1+1),  "�޻�����ͨӦ��");
		TextOut( x1 , *(y1+2),"ллʹ��");
        break;  

    case 53:
        TextOut( x1 , *(y1+0), "��Ƭ����");
		TextOut( x1 , *(y1+1),  "��ҵӦ��������");
		TextOut( x1 , *(y1+2),"ллʹ��");
        break;
        
    case 54:
        TextOut( x1 , *(y1+0), "����ʧ��");
		TextOut( x1 , *(y1+1),  " �ϴν���ʧ��");
		TextOut( x1 , *(y1+2),"ллʹ��");
        break;
        
    case 55:
		TextOut( x1 , *(y1+0),"����ʧ��");
		TextOut( x1 , *(y1+1), "���۳�ʧ��");
		TextOut( x1 , *(y1+2),"ллʹ��");        
        break;

    case 56:
		TextOut( x1 , *(y1+0),"����ʧ��");
		TextOut( x1 , *(y1+1), "��ȡTACʧ��");
		TextOut( x1 , *(y1+2),"ллʹ��");      
        break;  

    case 57:
		TextOut( x1 , *(y1+0),"����ʧ��");
		TextOut( x1 , *(y1+1), "��ȡ���ʧ��");
		TextOut( x1 , *(y1+2),"ллʹ��");
        break;
        
	case 60:
		TextOut( x1 , *(y1+0),"����ʧ��");
		TextOut( x1 , *(y1+1), "�����ֽ��ױ��ܾ�");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;

	case 61:
		TextOut( x1 , *(y1+0),"����ʧ��");
		TextOut( x1 , *(y1+1), "�ѻ�������֤ʧ��!");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;
        
	case 62:
		TextOut( x1 , *(y1+0),"����ʧ��");
		TextOut( x1 , *(y1+1), "ȡ����ʧ��!");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break; 
        
	case 63:
		TextOut( x1 , *(y1+0),"����ʧ��");
		TextOut( x1 , *(y1+1), "�����ֽ��ױ���ֹ");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break; 
        
    case 64:
        TextOut( x1 , *(y1+0),"����ʧ��");
		TextOut( x1 , *(y1+1),"����!");
		TextOut( x1 , *(y1+2),"ллʹ��");
        break;        

    case 65:
		TextOut( x1 , *(y1+0),"����ʧ��");
		TextOut( x1 , *(y1+1),"�˿��ǻ�ͨ��");
		TextOut( x1 , *(y1+2),"ллʹ��");   
        break;
	case 66:
		TextOut( x1 , *(y1+0), "��ܰ��ʾ");
		TextOut( x1 , *(y1+1),"������δ����");
		TextOut( x1 , *(y1+2),"���Ժ�����");
		//TextOut(100 , 50, "Ʊ������");
		//TextOut(35 , 90,  "Ʊ����Ч���ѹ���");
		//TextOut(100 , 130,"ллʹ��");
		break;
	case 67:
		TextOut( x1 , *(y1+0),"����ʧ��");
		TextOut( x1 , *(y1+1), "���Ӻ�̨��Ӧ��");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;
	case 68:
		TextOut( x1 , *(y1+0),"����ʧ��");
		TextOut( x1 , *(y1+1),"��Ʊ��֤����");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;
	case 69:
		TextOut( x1 , *(y1+0),"����ʧ��");
		//TextOut( 65 , 90, "��Ʊ��֤����");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;
	case 70:
		TextOut( x1 , *(y1+0),"����ʧ��");
		TextOut( x1 , *(y1+1),"���ѳ�ʱ�������ֻ���ȷ��");
		TextOut( x1 , *(y1+2),"ллʹ��");
		break;
	default:
		break;
	}    
#else
	SetTextSize(32);
	switch(messcode)
	{
	case 1:
		TextOut(100 , 50, "��Ƭ����");
		TextOut(35 , 90,  "��Ƭ��Ч���ѹ���");
		TextOut(100 , 130,"ллʹ��");
		break;
	case 2:
		TextOut( 100 , 30, "��Ƭ����");
		TextOut( 65  , 70, "��Ƭ��ʽ����");//
		TextOut( 50 , 110, "��Ǳ�ϵͳ��Ƭ");
		TextOut( 100 , 150,"ллʹ��");
		break;
	case 3:
		TextOut(100 , 50, "��Ƭ����");
		TextOut( 65 , 90, "Ǯ����ʽ����");
		TextOut(100 , 130,"ллʹ��");
		break;
	case 4:
		TextOut(100 , 50, "��Ƭ����");
		TextOut( 65 , 90, "   MAC����");
		TextOut(100 , 130,"ллʹ��");
		break;
	case 5:
		TextOut(100 , 50, "��Ƭ����");
		TextOut(10 , 90,  "��Ƭ��ʽ���̱�־����");
		TextOut(100 , 130,"ллʹ��");
		break;
	case 6:
		TextOut(100 , 50, "��Ƭ����");
		TextOut( 65 , 90, "�ֽ�Ǯ������");
		TextOut(100 , 130,"ллʹ��");
		break;
	case 7:
		TextOut(100 , 50, "������");
		TextOut( 65 , 90, "��Ƭ������");
		TextOut(100 , 130,"ллʹ��");
		break;

	case 9:
		TextOut(100 , 30,  "ʱ�δ���");
		TextOut( 85 , 70,  "����ʱ�䲻");//������ʱ����
		TextOut( 70 , 110, "������ʱ����");
		TextOut(100 , 150, "ллʹ��");
		break;
	case 10:
		TextOut( 100 , 50, "��Ƭ����");
		TextOut( 25 , 90,  "�˿�ƬΪ��������Ƭ");
		TextOut( 100 , 130,"ллʹ��");
		break;
	case 11:
		TextOut( 100 , 30, "��Ƭ����");
		TextOut( 85 , 70, "ϵͳ��֧��");//
		TextOut( 85 , 110,"�˿�������");//
		TextOut(100 , 150,"ллʹ��");//
		break;
	case 12:
		TextOut( 100 , 30, "����ʧ��");
		TextOut( 35 , 70,  "���Ŀ�Ƭ����");
		TextOut( 35 , 110, "֧���������ѽ��");
		TextOut( 100 , 150,"ллʹ��");
		break;
	case 13:
		TextOut( 100 , 30, "����ʧ��");
		TextOut( 35 , 70,  "��ǰʱ�����ѽ��");
		TextOut( 35 , 110, "����������Ѷ��");
		TextOut( 100 , 150,"ллʹ��");
		break;
	case 14:
		TextOut( 100 , 30, "����ʧ��");
		TextOut( 35 , 70,  "�������ѽ������");
		TextOut( 35 , 110, "����������Ѷ��");
		TextOut( 100 , 150,"ллʹ��");
		break;

	case 15:
		TextOut( 100 , 30, "����ʧ��");
		TextOut( 35 , 70,  "�������ѽ������");
		TextOut( 35 , 110, "����������Ѷ��");
		TextOut( 100 , 150,"ллʹ��");
		break;

	case 16:
		TextOut( 100 , 50, "����ʧ��");
		TextOut( 85 , 90, "���������");
		TextOut( 100 , 130,"ллʹ��");
		break;

	case 17:
		TextOut( 100 , 50, "����ʧ��");
		TextOut( 70 , 90, "����ʱ�伺��");
		TextOut( 100 , 130,"ллʹ��");
		break;

	case 18:
		TextOut( 100 , 30, "����ʧ��");
		TextOut( 35 ,  70, "���콻�״�������");
		TextOut( 35 , 110, "����������ƴ���");
		TextOut( 100 , 150,"ллʹ��");
		break;
	case 19:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 35 , 90, "�洢�ļ�·������");
		TextOut( 100 ,130,"ллʹ��");
		break;
	case 20:
		TextOut( 100 , 50, "����ʧ��");
		TextOut( 70 , 90, "�ڴ�ָ�����");
		TextOut( 100 ,130,"ллʹ��");
		break;

	case 21:
		TextOut( 100 , 50, "����ʧ��");
		TextOut( 70 , 90, "��¼�ڴ�����");
		TextOut( 100 ,130,"ллʹ��");
		break;

	case 22:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 5 , 90, "����������ʹ��ʱ����");
		TextOut( 100 ,130,"ллʹ��");
		break;


	case 23:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 35 , 90, "��֤������Կʧ��");
		TextOut( 100 ,130,"ллʹ��");
		break;
	case 24:
		TextOut(100 , 50, "��ܰ��ʾ");
		TextOut(100 , 90, "ˢ������");
		TextOut(85  , 130,"������ˢ��");	
		break;	

	case 25:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 35 , 90, "�洢�ļ�Ŀ¼����");
		TextOut( 100 ,130,"ллʹ��");
		break;

	case 26:
		TextOut( 100 , 50, "����ʧ��");
		TextOut(100 , 90, "��Ա����");
		TextOut( 100 , 130,"ллʹ��");
		break;
	case 27: 
			TextOut(80,100, "��·���³ɹ�");
			//beepopen(2);
			printf("now the line is %d\n",Section.Linenum);
			updatline_err=0;
			break;
	case 28:			
		TextOut(80,100, "��·����ʧ��");
		//beepopen(2);
		updatline_err=0;
			break;

	case 29:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 25 , 90, "�ݲ�֧�ִ˳�������");
		TextOut(100 , 130,"ллʹ��");
		break;

	case 30:
		TextOut(100 , 50, "��Ƭ����");
		TextOut(25 , 90,  "δ��ͨ������ͨӦ��");
		TextOut(100 , 130,"ллʹ��");
		break;
    case 31:
		TextOut(100 , 50, "��Ƭ����");
		TextOut( 65 , 90, "��Կ�汾����");
		TextOut(100 , 130,"ллʹ��");
        break;     

    case 32:
		TextOut(100 , 50, "��Ƭ����");
		TextOut( 65 , 90, "��ȫ��֤����");
		TextOut(100 , 130,"ллʹ��");
        break;          

	case 40:
		TextOut(100 , 50, "��ܰ��ʾ");
		TextOut(50 , 90,  "����״̬��ȷ��");
		TextOut(100 , 130,"ллʹ��");
		break;
		
	 case 41:
        TextOut( 100 , 50,"����ʧ��");
        TextOut( 20 , 90, "����Ƭ������Ϣʧ��");
        TextOut( 100 ,130,"ллʹ��");
        break;

	case 42:
        TextOut( 100 , 50,"����ʧ��");
        TextOut( 20 , 90, "����Ƭ������Ϣʧ��");
        TextOut( 100 ,130,"ллʹ��");
        break;	
	case 43:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 20 , 90, "��������ϸ�ļ�ʧ��");
		TextOut( 100 ,130,"ллʹ��");
		break;
	case 44:
		TextOut( 100 , 50, "����ʧ��");
		TextOut( 35 , 90, "��¼�ļ���ʧ��");
		TextOut( 100 ,130,"ллʹ��");
		break;
		
	case 45:
		TextOut( 100 , 50, "��Ƭ����");
		TextOut(50 , 90,  "��ƬӦ��δ��ͨ");
		TextOut( 100 , 150,"ллʹ��");
		break;
        
		
	case 46:
		TextOut( 100 , 50, "��Ƭ����");
		TextOut(35 , 90,  "ѭ����¼��ʧ��");
		TextOut( 100 , 150,"ллʹ��");
		break;	

    case 47:
        TextOut( 100 , 50, "��Ƭ����");
		TextOut(35 , 90,  "����Ǯ��Ӧ������");
		TextOut( 100 , 150,"ллʹ��");
        break;

    case 48:
        TextOut( 100 , 50, "��Ƭ����");
		TextOut(35 , 90,  "��Ƭ���ڸ�ʽ�쳣");
		TextOut( 100 , 150,"ллʹ��");
        break; 
        
    case 49:
        TextOut( 100 , 50, "��Ƭ����");
		TextOut(35 , 90,  "��Ƭδ����������");
		TextOut( 100 , 150,"ллʹ��");
        break; 
        
	case 50:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 65 , 90, "û���ϳ���־");
		TextOut( 100 ,130,"ллʹ��");
		break;

	case 51:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 85 , 90, "�˿����³�");
		TextOut( 100 ,130,"ллʹ��");
		break;
        
    case 52:
        TextOut( 100 , 50, "��Ƭ����");
		TextOut(50 , 90,  "�޻�����ͨӦ��");
		TextOut( 100 , 150,"ллʹ��");
        break;  

    case 53:
        TextOut( 100 , 50, "��Ƭ����");
		TextOut(50 , 90,  "��ҵӦ��������");
		TextOut( 100 , 150,"ллʹ��");
        break;
        
    case 54:
        TextOut( 100 , 50, "����ʧ��");
		TextOut(50 , 90,  " �ϴν���ʧ��");
		TextOut( 100 , 130,"ллʹ��");
        break;
        
    case 55:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 65 , 90, "���۳�ʧ��");
		TextOut( 100 ,130,"ллʹ��");        
        break;

    case 56:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 65 , 90, "��ȡTACʧ��");
		TextOut( 100 ,130,"ллʹ��");      
        break;  

    case 57:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 65 , 90, "��ȡ���ʧ��");
		TextOut( 100 ,130,"ллʹ��");
        break;
        
	case 60:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 10 , 90, "�����ֽ��ױ��ܾ�");
		TextOut( 100 ,130,"ллʹ��");
		break;

	case 61:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 10 , 90, "�ѻ�������֤ʧ��!");
		TextOut( 100 ,130,"ллʹ��");
		break;
        
	case 62:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 10 , 90, "   ȡ����ʧ��!");
		TextOut( 100 ,130,"ллʹ��");
		break; 
        
	case 63:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 10 , 90, "�����ֽ��ױ���ֹ");
		TextOut( 100 ,130,"ллʹ��");
		break; 
        
    case 64:
        TextOut( 100 , 50,"����ʧ��");
		TextOut( 35 , 90, "    ����!");
		TextOut( 100 ,130,"ллʹ��");
        break;        

    case 65:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 10 , 90, "   �˿��ǻ�ͨ��");
		TextOut( 100 ,130,"ллʹ��");   
        break;
	case 66:
		TextOut(100,50, "��ܰ��ʾ");
		TextOut(62,100,"������δ����");
		TextOut(90,150,"���Ժ�����");
		//TextOut(100 , 50, "Ʊ������");
		//TextOut(35 , 90,  "Ʊ����Ч���ѹ���");
		//TextOut(100 , 130,"ллʹ��");
		break;
	case 67:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 42 , 90, "���Ӻ�̨��Ӧ��");
		TextOut( 100 ,130,"ллʹ��");
		break;
	case 68:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 65 , 90, "��Ʊ��֤����");
		TextOut( 100 ,130,"ллʹ��");
		break;
	case 69:
		TextOut( 100 , 50,"����ʧ��");
		//TextOut( 65 , 90, "��Ʊ��֤����");
		TextOut( 100 ,130,"ллʹ��");
		break;
	case 70:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 40 , 90, "���ѳ�ʱ�������ֻ���ȷ��");
		TextOut( 100 ,130,"ллʹ��");
		break;

 	case 71:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 65 , 90, "��Ƭδ����");
		TextOut( 100 ,130,"ллʹ��");
		break;   

    case 72:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 40 , 90, "���¹��������ļ�ʧ��");
		TextOut( 100 ,130,"ллʹ��");
		break;  

    case 73:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 40 , 90, "���»�����Ϣ�ļ�ʧ��");
		TextOut( 100 ,130,"ллʹ��");
		break;     

    case 74:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 40 , 90, "�����ؿ������ļ�ʧ��");
		TextOut( 100 ,130,"ллʹ��");
		break;     

    case 75:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 40 , 90, "��ͨ���������ļ�ʧ��");
		TextOut( 100 ,130,"ллʹ��");
		break;  

    case 76:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 40 , 90, "����ͨ�������ļ�ʧ��");
		TextOut( 100 ,130,"ллʹ��");
		break;  

    case 77:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 40 , 90, "���������ļ�ʧ��");
		TextOut( 100 ,130,"ллʹ��");
		break;    

    case 78:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 40 , 90, "�����в����ļ�ʧ��");
		TextOut( 100 ,130,"ллʹ��");
		break;          

    case 79:
		TextOut( 100 , 50,"����ʧ��");
		TextOut( 40 , 90, "�����в����ļ�ʧ��");
		TextOut( 100 ,130,"ллʹ��");
		break;          
	default:
		break;
	}
#endif    

	if(messcode!=27) 
			beepopen(3);
	else 
			beepopen(2);	
	
		switch(messcode)
		{            
        case 1:
            //����       ��Ƭ�ѹ���
            musicIndex = 40;
            break;
        case 2:
        case 3:
        case 5:
            // ����       ��ʽ����
            musicIndex =  41;
			break;
        case 4:        
            // ����       ���Ѵ���
            musicIndex =  41;
            break;
        case 6:
            // ����       �������
            musicIndex =  43;
            break;
        case 7:
        case 64:
            // ����       ����
            musicIndex =  64;
            break;    
        case 9:
            // ����       ����ʱ�δ���
            musicIndex =  44;
            break;              
		case 10:
			// ����       ��������
            musicIndex =  45;
			break;
        case 11:
            // ����       ��֧�ִ˿���
            musicIndex =  46;
            break;
        case 12:
			// ����       ����
			musicIndex =  13;
			break;
		case 13:
		case 14:
		case 15:
        case 18:
			// ����       ���ѳ���
			musicIndex =  17;
			break;
        case 16:
			// ����       ��������
			musicIndex =  48;
			break;
        case 17:
			// ����       �����ѹ���
			musicIndex =  49;
			break;
        case 19:
        case 25: 
			// ����       �����¼����
			musicIndex =  50;
			break;    
        case 21:
			// ����       �ڴ�����
			musicIndex =  51;
			break;    
		case 22:
			// ����       �����ظ�ˢ��
			musicIndex =  16;
			break;
        case 23:
			// ����       ������Կʧ��
			musicIndex =  52;
			break;    
		case 24:
        case 60:
        case 63:
        case 54:     
        case 72:
        case 73:
            //����        ˢ�����ɹ�������ˢ��
			musicIndex = 15;
			break;			
        case 26:
            //����       ��Ա����
			musicIndex = 19;
			break;
        case 29:
        case 65:    
            //����       �˿��ǻ�ͨ��
			musicIndex = 65;
			break;
        case 30:
            //����       δ��ͨ��ͨӦ��
			musicIndex = 53;
			break;
        case 31:
            //����       ��Կ�汾����
			musicIndex = 54;
			break;
        case 32:
        case 71:
            //����       �Ƿ���
            musicIndex = 12;
            break;
        case 41:
        case 42:
        case 43:
        case 44:
        case 46:
        case 74:    
            //����       ����Ƭ�ļ�ʧ��
			musicIndex = 55;
			break;        
        case 45:
            //����       ��ƬӦ��δ��ͨ
			musicIndex = 56;
			break;    
        case 47:
        case 53:
            //����       Ӧ��������
			musicIndex = 61;
			break;
        case 48:
            //����       ���ڸ�ʽ�쳣
			musicIndex = 57;
			break;
        case 49:
            //����       δ����������
			musicIndex = 58;
			break;            
        case 50:
            //����       �˿�δ�ϳ�
			musicIndex = 59;
			break;         
        case 51:
            //����       �˿����³�
			musicIndex = 60;
			break;           
        case 61:
            //����       �ѻ���֤ʧ��
			musicIndex = 62;
			break;                
        case 62:
            //����       ȡ����ʧ��
			musicIndex = 63;
			break;         
            
	          
#ifdef SUPPORT_QR_CODE
        case 69:
		musicIndex = 28;
		break;
	case 70:
		musicIndex = 29;
		break;
#endif

		default :
			break;
		}
     PlayMusic(musicIndex, 0);

	ioctl(mf_fd, RC531_HALT);
	sleep(1);

	LEDL(0);

	SetColor(Mcolor);
}



/*
*************************************************************************************************************
- �������� : unsigned char  MoneyYes(unsigned char *Date)
- ����˵�� : �õ���Ƭ��Ϣ
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/

//�жϵ�ǰ�Ŀ�Ƭ�Ƿ���������ֵ��
unsigned char  MoneyYes(unsigned char *Date)
{
	unsigned char  status = 1;
	unsigned int ak0,ak1,ak2;
	LongUnon Buf;

	memcpy(Buf.longbuf,Date  ,4);
	ak0 = Buf.i;
	memcpy(Buf.longbuf,Date+4,4);
	ak1 = Buf.i;
	memcpy(Buf.longbuf,Date+8,4);
	ak2 = Buf.i;
	if((ak0 == ak2)&&(ak0 == ~ak1)&&(Date[12] == Date[14])&&(Date[13] == Date[15])&&((Date[12]^Date[13]) == 0xff)&&(ak0 < 0x7fffffff))status = 0;
	return status;
}


/*
*************************************************************************************************************
- �������� : unsigned char JudgeCsn(void)
- ����˵�� : �жϿ�Ƭ����ĺϷ�����
- ������� : ��
- ������� : �������
			0����ʾ��Ƭ���Ǻ�����
			1����ʾ��Ƭ�Ѿ��Ǻ�����
*************************************************************************************************************
*/
unsigned char Card_JudgeCsn(void)
{
	unsigned int   Nuo;
	unsigned char  status;
	unsigned char  cardbuf[5];
	unsigned char  CJCbuf[12];
	memset(CJCbuf,0,sizeof(CJCbuf));
	memset(cardbuf,0,sizeof(cardbuf));
	memcpy(cardbuf,CardLan.CardCsnB,4);
	hex_2_ascii(cardbuf,CJCbuf,4);
	Nuo = myatoi(CJCbuf);
    DebugPrintf("�ȽϿ���:%d\n",Nuo);
	status = Number(Nuo,2);
	return status;
}



unsigned char Card_JudgeCsn1(void)
{
	unsigned int   Nuo;
	unsigned char  status;
	unsigned char  cardbuf[5];
	unsigned char  CJCbuf[12];
    BkBlackItem src;
	int find;

	memset(CJCbuf,0,sizeof(CJCbuf));
	memcpy(CJCbuf,CardLan.CardCsnB_blk,8);
	{
		
		memcpy(src.dat, CJCbuf, sizeof(BkBlackItem));
		
		half_search_bank(src, &find);
		status = find;
		DBG_PRINTF("Card_JudgeCsn() find=%d\n", find);
		DBG_PRINTF("Card_JudgeCsn() status=%d\n", status);
	}
	return status;
}



/*
*************************************************************************************************************
- �������� : unsigned char Permissions (HWND hDlg)
- ����˵�� : Ȩ�޴���
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char Permissions (unsigned char Mode)
{
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char status;
	struct timeval first, second;
//	gettimeofday(&first,0);
//	printf("#1#Permissions [%d.%06d]\n", first.tv_sec, first.tv_usec);
    DebugPrintf("\n");
	DBG_RC500_PRINTF("Permissions() is called.\n");

	while(Loop)
	{
		switch (step)
		{
		case 1:
			status = TimeRange();				//��ǰ��û�кϷ�������ʱ���
			if(status != MI_OK)
			{
				TypeTime.TimeNum = status;
				step++;
#if DBG_RC500
				printf("TypeTime = %d\n",TypeTime.TimeNum);
#endif
			}
			else
			{
				Err_display(9);
#ifndef CONFIG_LINUXBUS8BIT                
                Err_save(CARD_SPEC_M1_LINUX,9);
#endif
				Loop = 0;
			}
			gettimeofday(&first,0);
			DebugPrintf("#1#Permissions [%d.%06d]\n", first.tv_sec, first.tv_usec);

			break;
		case 2:		//�жϿ�Ƭ�Ƿ��Ǻ������� 33��ʾ���Ǻ�����
		#ifdef SUPPORT_QR_CODE
			if (CardLan.CardType == QR_CODE_TYPE)
			{
				step++;
				break;
			}
		#endif

			status  = Card_JudgeCsn();
			//status = MI_OK;
			if(status == MI_OK)step++;
			else
			{
				Err_display(10);
#ifndef CONFIG_LINUXBUS8BIT                  
                Err_save(CARD_SPEC_M1_LINUX,10);
#endif
				Loop = 0;
			}
			gettimeofday(&first,0);
			printf("#2#Permissions [%d.%06d]\n", first.tv_sec, first.tv_usec);

			break;

		case 3:	//�жϿ�Ƭ���
			status  = SupportType();
			if(status == MI_OK)step++;
			else
			{
				Err_display(11);
#ifndef CONFIG_LINUXBUS8BIT                  
                Err_save(CARD_SPEC_M1_LINUX,11);
#endif
				Loop = 0;
			}
			//gettimeofday(&first,0);
			//printf("#3#Permissions [%d.%06d]\n", first.tv_sec, first.tv_usec);

			break;

		case 4:
			if(SavedataErr == 0)
				step++;
			else if(SavedataErr == 2)
			{
				Err_display(21);
#ifndef CONFIG_LINUXBUS8BIT                  
                Err_save(CARD_SPEC_M1_LINUX,21);
#endif
				Loop = 0;
			}
			else
			{
				Err_display(20);
#ifndef CONFIG_LINUXBUS8BIT                  
                Err_save(CARD_SPEC_M1_LINUX,20);
#endif
				Loop = 0;
			}
			break;

		case 5:
#ifdef SUPPORT_QR_CODE
			if (CardLan.CardType == QR_CODE_TYPE)  //��ΪҪ֪����һ��ˢ��ά���ʱ�䣬�ⲽҲҪ������Ҳ���Բ��������Ա���һ��ʱ��
			{
				step++;
				break;
			}
#endif

			status = CardDiscounts(CardLan.CardType,Mode);
			if(status != MI_OK)step++;
			else
			{
				Err_display(22);
#ifndef CONFIG_LINUXBUS8BIT                  
                Err_save(CARD_SPEC_M1_LINUX,22);
#endif
				Loop = 0;
			}
			//gettimeofday(&first,0);
			//printf("#4#Permissions [%d.%06d]\n", first.tv_sec, first.tv_usec);

			break;

		default:
			step = 0;
			Loop = 0;
			break;
		}
	}

	gettimeofday(&first,0);
	DebugPrintf("#6#Permissions [%d.%06d]\n", first.tv_sec, first.tv_usec);
	printf("the permision step is %d\n",step);
	return(step);
}




/*
*************************************************************************************************************
- �������� : void IncSerId (void)
- ����˵�� :
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
void IncSerId (void)
{
	DebugPrintf("#### DevSID.i = 0x%02X\n", DevSID.i);
	DevSID.i ++;
	//while (FileOpenFlag == 0);
	// FileOpenFlag = 0;
//	ReadOrWriteFile (DEVSERIALID);
	ReadOrWriteFile(DEVSERIALID);
	// FileOpenFlag = 1;
}

/*
*************************************************************************************************************
- �������� : void IncTransactionNum (void)
- ����˵�� : �����ܽ��״���
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
void IncTransactionNum (void)
{
	DebugPrintf("#### TransactionNum.i = 0x%02X\n", TransactionNum.i);
	
	if(SaveData.RFXFtype != 6)
		TransactionNum.i++;

	ReadOrWriteFile(WSDATA);

}

unsigned char DiverCard(void)
{
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char status;

	while(Loop)
	{
		switch (step)
		{
		case 1: 						//��Ч�����Ƿ����
			status = Card_JudgeDate();
			if(status == MI_OK)
			{
				step++;
			}
			else
			{
				Err_display(1);
                Err_save(CARD_SPEC_M1_LINUX,1);
				Loop = 0;
			}
			break;

		case 2:		//�жϿ�Ƭ�Ƿ��Ǻ������� 33��ʾ���Ǻ�����
			status  = Card_JudgeCsn();
			if(status == MI_OK)step++;
			else
			{
				Err_display(10);
                Err_save(CARD_SPEC_M1_LINUX,10);
				Loop = 0;
			}
			break;

		case 3:
			if(SavedataErr == 0)
				step++;
			else if(SavedataErr == 2)
			{
				Err_display(21);
                Err_save(CARD_SPEC_M1_LINUX,21);
				Loop = 0;
			}
			else
			{
				Err_display(20);
                Err_save(CARD_SPEC_M1_LINUX,20);
				Loop = 0;
			}
			break;

		case 4:
			SaveDirverNumber();
			IncSerId();
			step++;
			break;

		case 5:
			SaveDirverData();
			#if 0//def ZHEJIANG_ANJI
			SendDriverInfoToDevice(0);     //0 is sign. 1 is leave 
			#endif
			if(status == MI_OK)step++;
			else
			{
				Loop = 0;
			}
			break;


		default:
			step = 0;
			Loop = 0;
			break;
		}
	}

	return(step);
}


int CheckReaderFunc(void)
{	
	int ret;
	char RC531_No[16];
    char i;

        memset(RC531_No, 0, sizeof(RC531_No));
    	ret = ioctl(mf_fd, 0xB5, RC531_No);
        DebugPrintf("G_GET531_flag=%d\n",G_GET531_flag);
     
    	if((RC531_No[0]!=0)||(RC531_No[1]!=0)||(RC531_No[2]!=0))
    	{	
    	    
			return 0;
    		
    	}
    	else
    		{
                return 1;
           }

	
}

int HandleLowVoltsFunc(void)
{	
	int ret;
	char RC531_No[16];
    char i;
  //  for(i=0;i<1;i++)
//	{
        memset(RC531_No, 0, sizeof(RC531_No));
    	ret = ioctl(mf_fd, 0xB5, RC531_No);
        DebugPrintf("G_GET531_flag=%d\n",G_GET531_flag);
       // DebugPrintChar("RC531_No", RC531_No, 16);
      //  DebugPrintChar("G_RC531_NO", G_RC531_NO, 16);
    	if(G_GET531_flag)
    	{	
    	    if(memcmp(RC531_No, G_RC531_NO, 16))
    		{
              //  if(i!=1)
              //      continue;
    			close(mf_fd);
                w55fa93_setio(GPIO_GROUP_B, 3, 1);
                usleep(500000);
                 w55fa93_setio(GPIO_GROUP_B, 3, 0);
                 usleep(500000);
    			mf_fd = open("/dev/typea", O_RDWR);
    			
    			printf("Handle low volts\n");
                DebugPrintf("\n");
			G_GET531_flag = 0;
			return 1;
    		}
    	}
    	else
    		{
                memcpy(G_RC531_NO, RC531_No, 16);
			    G_Read531fg = 1;
                DebugPrintf("\n");
           }
   //     }
	return 0;
}



int Init_RC531_No(void)
{
	FILE *RC531_fd;

	int ret = 0;
#if 1//def SHENGKE_TIANGUAN
return 0;
#endif

	if(access(RC531_NO_PATH, F_OK))
	{
		return 0;
	}
	
	if((RC531_fd = fopen(RC531_NO_PATH, "r+")) != 0)
	{
		if ((ret = fread(G_RC531_NO,  1, 16, RC531_fd)) == 16)
		//if((fgets (G_RC531_NO, 20, RC531_fd)) != NULL)
		{
			DebugPrintChar("RC531", G_RC531_NO, 16);
			G_GET531_flag = 1;
		}
	}

	fclose(RC531_fd);
	return 0;
}

int Save_RC531_NO_Handle(void)
{
	FILE *RC531_fd;
	char TempBuffer[16];
    return 0;
#if 1//def SHENGKE_TIANGUAN
return 0;
#endif

	if(G_GET531_flag || !G_Read531fg)
		return 0;
	
	if((RC531_fd = fopen(RC531_NO_PATH, "w+")) != 0)
	{
		DebugPrintChar("RC531", G_RC531_NO, 16);
		fwrite(G_RC531_NO, sizeof(unsigned char), 16, RC531_fd);
		G_GET531_flag = 1;
	}

	fclose(RC531_fd);
	system("sync");
	return 0;
}


unsigned char CardReset(char *data,unsigned char *plen,unsigned char type)
{
	static int	receive_len[1] = {0};
	static char receive_buf[256]= {0};
	int result;
	unsigned char Loop,step;
	//  struct stat fdbuf;
	int i;

//	DBG_PRINTF("CardReset is called \n");

	
#if 0 // DBG_RC500
	{
		struct timeval now;
		gettimeofday(&now,0);
		DBG_RC500_PRINTF("CardReset() is called, type = %d, time = %ld\"%06ld.\n", type, now.tv_sec, now.tv_usec);
	}
#endif

	Loop = 1;
	step = 1;
	CardTypeIC = 0;

	//  if((CardTwo == 0)||(type ==1))
	// {
	//     step = 1;
	// }
	// else
	// {
	//      step = 2;
	// }
	while(Loop)
	{
       
		switch(step)
		{

		case 0:
		//	HandleLowVoltsFunc();
			step++;
			break;
            
		case 1:

			for(i=0; i<5; i++)
			{
				result = ioctl(mf_fd, RC531_M1_CSN);
				if(result == MI_OK)
				{
					step++;
					break;
				}
				usleep(1000);
			}
			if(i==5) Loop = 0;
			break;

		case 2:
			CardTwo = 1;
			readcardnum =0;
			ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
			read(mf_fd, receive_buf, receive_len[0]);

#if 1
			{
				unsigned char i;
				printf("read RESET = %d\n",receive_len[0]);
				for(i =0 ; i < receive_len[0]; i ++)
				{
					printf("%02X ",receive_buf[i]);
				}
				printf("\n");
			}
#endif

			//ReadCardFirst: 0x55 - ����M1��,��������CPU��
			if(((receive_buf[4]&0x20)&&(ReadCardFirst != 0x55))||(((receive_buf[4]&0x08)!=0x08)&&(receive_buf[4]&0x20))||((receive_buf[4]&0x28)==0x28))
			{
				CardTypeIC = 0x20;
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(type == 0)
				{
					memset(&CardLan,0,sizeof(CardInform));
					memcpy(CardLan.UserIcNo,receive_buf,4);
                    memset(&CardLanCPU,0,sizeof(CardInformCPU));
                    memcpy(CardLanCPU.CSN, receive_buf ,4);
					step++;
				}
				else
				{
                    memset(&CardLanCPU,0,sizeof(CardInformCPU));
                    memcpy(CardLanCPU.CSN, receive_buf ,4);
					if(mystrncmp(CardLan.UserIcNo,receive_buf,4) == 0)
					{
						step++;
					}
					else
					{
						Loop = 0;
					}
				}

			}
			else if(receive_buf[4]&0x08)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(type == 0)
				{
					memset(&CardLan,0,sizeof(CardInform));
					memcpy(CardLan.UserIcNo,receive_buf,4);
                    CardLanCPU.IsLocalCard = 0;
					memcpy(data,receive_buf,receive_len[0]);
					*plen = receive_len[0];
					step = 0x08;
				}
				else
				{
					if(mystrncmp(CardLan.UserIcNo,receive_buf,4) == 0)
					{
						step = 0;
					}
					else
					{
						Loop = 0;
					}
				}
				CardTypeIC = 0x08;
				Loop = 0;
			}
			else
			{
				Loop = 0;
			}
			break;

		case 3:
			ioctl(mf_fd, WRITE_TYPE, W_CPU);
			result = ioctl(mf_fd, TYPEA_CPU_REST);
			if(result == MI_OK)
			{
				if(type == 0)
				{
					step++;
				}
				else
				{
					step = 0;
					Loop = 0;
				}
				// step = 0x20;
				// Loop = 0;
			}
			else Loop = 0;
			break;


		case 4:
			ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
			result = read(mf_fd, receive_buf, receive_len[0]);
			memcpy(data,receive_buf,receive_len[0]);
			*plen = receive_len[0];
			step = 0x20;
			Loop = 0;

#if DBG_RC500
			{
				unsigned char i;
				printf("TYPEA CPU RESET = %d\n",receive_len[0]);
				for(i =0 ; i < receive_len[0]; i ++)
				{
					printf("%02X ",receive_buf[i]);
				}
				printf("\n");
			}
#endif

			break;

		default :
			Loop = 0;
			break;
		}
	}

	readcardnum++;
   // printf("readcardnum=%d\n",readcardnum);
	if((step!=0x08)&&(step!=0x20))
	{
		if(readcardnum > 100000)
		{
			close(mf_fd);
			mf_fd=open("/dev/typea",O_RDWR);
			if(mf_fd<0)
			{
				printf("Can't open /dev/typea \n");
				// close(mf_fd);
				return -2;
			}
			ioctl(mf_fd,DO_TYPEA_M1);
			readcardnum = 0;

#if  0 // DBG_RC500
			{
				struct timeval now;
				gettimeofday(&now,0);
				printf("\n\n %d DO_TYPEA_M1_0  time = %ld\"%06ld  \n\n",readcardnum,now.tv_sec, now.tv_usec);

			}
#endif
		}
		else if(readcardnum%1000==0)
		{   


			#if 1//ndef SHENGKE_TIANGUAN
            	//	if (HandleLowVoltsFunc())
				ioctl(mf_fd,DO_TYPEA_M1);

             #endif

#if 0
			{
				struct timeval now;
				gettimeofday(&now,0);
				printf("\n\n %d DO_TYPEA_M1_1  time = %ld\"%06ld  \n\n",readcardnum,now.tv_sec, now.tv_usec);

			}
#endif

		}
	}

	return step;
}
#ifdef	DBG_RF_TRANS_TIME
extern struct timeval test1,test2;
#endif
unsigned char CardReset_M1(char *data,unsigned char *plen,unsigned char type)
{
	static int	receive_len[1] = {0};
	static char receive_buf[256]= {0};
	int result;
	unsigned char Loop,step;
	//  struct stat fdbuf;
	int i;
	//DBG_PRINTF("CardReset_M1() is called.\n");

#if 0 // DBG_RC500
	{
		struct timeval now;
		gettimeofday(&now,0);
		DBG_RC500_PRINTF("CardReset() is called, type = %d, time = %ld\"%06ld.\n", type, now.tv_sec, now.tv_usec);
	}
#endif

	Loop = 1;
//	step = 0;
    step = 1;
	CardTypeIC = 0;

	while(Loop)
	{
		switch(step)
		{
	//	case 0:
	//		
	//		step++;
	//		break;
            
		case 1:

#if 1
			for(i=0; i<5; i++)
			{
				result = ioctl(mf_fd, RC531_M1_CSN);		//�����õ�CSN�Ϳ�Ƭ����
				if(result == MI_OK)
				{
					step++;
					break;
				}
				usleep(1000);
			}
			if(i==5) Loop = 0;
			break;
#else
			result = ioctl(mf_fd, 0xAB);				//�����õ�CSN�Ϳ�Ƭ����,����RC531_M1_CSN��ֵ
			if(result == MI_OK)
			{
				//gettimeofday(&test2,0);
				//printf("Wirte AB command time is Microsecond : %d, Millisecond : %d \n",(test2.tv_sec - test1.tv_sec)*1000000 + (test2.tv_usec-test1.tv_usec),((test2.tv_sec - test1.tv_sec)*1000000 + (test2.tv_usec-test1.tv_usec))/1000);				
				step++;
			}
			else
			{
				Loop = 0;
			}
			break;
#endif
		case 2:
			//CardTwo = 1;
			readcardnum =0;
			ioctl(mf_fd, FIFO_RCV_LEN, receive_len);	//��ȡ��Ƭ�����ֽ���
			read(mf_fd, receive_buf, receive_len[0]);

#ifdef DEBUG_PRINTF
			unsigned char i;
			DebugPrintf("read RESET_LEN [%d]  ",receive_len[0]);
			for(i =0 ; i < receive_len[0]; i ++)
			{
				printf("%02X ",receive_buf[i]);
			}
			printf("\n");
#endif
            
			if((receive_buf[4]&0x20) || (receive_buf[4]&0x08))
			{

				//ReadCardFirst: 0x55 - ����M1��,��������CPU��
				if(((receive_buf[4]&0x20)&&(ReadCardFirst != 0x55))||(((receive_buf[4]&0x08)!=0x08)&&(receive_buf[4]&0x20)))
				{
					#if ((defined  QINGDAO_TONGRUAN)||(defined YUNCHENG_BUS))
					CardTypeIC = 0x08;
					#else
					CardTypeIC = 0x20;
					#endif
				}
				else if (receive_buf[4]&0x08)
				{
					CardTypeIC = 0x08;
				}
				else
				{
					CardTypeIC = receive_buf[4];
				}
				
				step = CardTypeIC;		//��ÿ���
                CardLanCPU.IsLocalCard = 0;
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);			//��ȡ��Ƭ�����ֽ���
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(type == 0)
				{
					memset(&CardLan,0,sizeof(CardInform));
					memcpy(CardLan.UserIcNo, receive_buf ,4);
					memcpy(data,receive_buf,receive_len[0]);
					*plen = receive_len[0];
				}
				else if(mystrncmp(CardLan.UserIcNo,receive_buf,4) == 0)
				{
					step = 0;
				}
			}
			Loop = 0;

			break;

		default :
			Loop = 0;
			break;
		}
	}

	readcardnum++;
  // printf("readcardnum=%d\n",readcardnum);
	if((step!=0x08)&&(step!=0x20))
	{
		if(readcardnum > 100000)
		{
			close(mf_fd);
			mf_fd=open("/dev/typea",O_RDWR);
			if(mf_fd<0)
			{
				printf("Can't open /dev/typea \n");
				// close(mf_fd);
				return -2;
			}
			ioctl(mf_fd,DO_TYPEA_M1);
			readcardnum = 0;

#if  0 // DBG_RC500
			{
				struct timeval now;
				gettimeofday(&now,0);
				printf("\n\n %d DO_TYPEA_M1_0  time = %ld\"%06ld  \n\n",readcardnum,now.tv_sec, now.tv_usec);

			}
#endif
		}
		else if(readcardnum%1000==0)
		{


			#if 1//ndef SHENGKE_TIANGUAN


           	//	if (HandleLowVoltsFunc())
				ioctl(mf_fd,DO_TYPEA_M1);

			#endif

            
#if 0
			{
				struct timeval now;
				gettimeofday(&now,0);
				printf("\n\n %d DO_TYPEA_M1_1  time = %ld\"%06ld  \n\n",readcardnum,now.tv_sec, now.tv_usec);

			}
#endif

		}
	}

	return step;
}

void MakeSureCardLeave(void)
{
	unsigned int status,count;
	unsigned int error_cnt = 0;
	count = 0;
	ioctl(mf_fd,DO_TYPEA_M1);		
	while(1)
	{
		status = CardReset_M1(0,0,1);
	//	printf("MakeSureCardLeave status : %d \n", status);
		if(status != 1)
		{
			error_cnt++;
			count = 0;

		}
		else
			count++;
		if(count >= 1)
			break;
        
#ifdef Transport_Stander
		if (error_cnt > 30) break;
#else
        if (error_cnt > 10) break;
#endif

		usleep(100000);
	}
}




unsigned char ResetM1FormCPU(void)
{
	ioctl(mf_fd,DO_TYPEA_M1);
	return CardReset_M1(0,0,1);
}





/****************************************************************************
* Function:	TypeAPiccSendAPDU
* Input:		Command to be sent
* Output:		Response data
****************************************************************************/
unsigned char TypeAPiccSendAPDU(uint8_t  *Send,uint8_t *Rcvdata,uint8_t Slen, unsigned int *Rlen)
{
	unsigned char status = 0xFF;
	int	receive_len[1] = {0};
	unsigned char receive_buf[256]= {0};

	DBG_RC500_PRINTF("TypeAPiccSendAPDU() is called.\n");

	*Rlen = 0;
	if (mf_fd != 0)
	{
		status = write(mf_fd, Send, Slen);
		DBG_RC500_PRINTF("TypeAPiccSendAPDU(): write status = %d.\n", status);
		if (status == MI_OK)
		{
			status = ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
			DBG_RC500_PRINTF("TypeAPiccSendAPDU(): read length status = %d, receive_len = %d.\n", status, receive_len[0]);
			if(receive_len[0] > 0)
			{
				memset(receive_buf, 0, sizeof(receive_buf));
				status = read(mf_fd, receive_buf, receive_len[0]);
				//DBG_PRINTF("TypeAPiccSendAPDU(): read status = %d, receive_len = %d.\n", status, receive_len[0]);
				memcpy(Rcvdata, receive_buf, receive_len[0]);
				*Rlen = receive_len[0];
			}
		}
	}

	return status;
}

#if Transport_Stander

//���ݲ�ͬ�ͻ����壬ת����Ӧ����
unsigned char GetCardType()
{
    unsigned char card_type=0;
    
    switch(CardLanCPU.cardtype)
			{
				case 0x01://��ͨ��			
#if((defined YAN_AN_BUS) ||(defined NINGXIA_GUYUAN)||(defined DIQING_BUS))
                    card_type = 1;
#elif  YUXI_BUS
					card_type = 7;
#elif PUER_BUS  
                    card_type = 8;  
#elif ZHAOTONG_BUS 
                    card_type = 8; 
#elif LIJIANG_BUS
                    card_type = 5;
#elif LIUKU_BUS
                    card_type = 9;
#elif LANPING_BUS  
                    card_type = 1;

#endif
					break;
				case 0x02://ѧ����
#if((defined YAN_AN_BUS) ||(defined NINGXIA_GUYUAN)||(defined DIQING_BUS))
                    card_type = 2;
#elif  YUXI_BUS
					card_type = 1;
#elif  PUER_BUS 
                    card_type = 2; 
#elif ZHAOTONG_BUS 
                    card_type = 2; 
#elif LIJIANG_BUS
                    card_type = 1;
#elif LIUKU_BUS
                    card_type = 1;
#elif LANPING_BUS
                    card_type = 2; 
#endif
					break;
				case 0x03://���˿�					
#if((defined YAN_AN_BUS) ||(defined NINGXIA_GUYUAN)||(defined DIQING_BUS))
                    card_type = 3;
#elif  YUXI_BUS
					card_type = 2;
#elif  PUER_BUS 
                    card_type = 3; 
#elif ZHAOTONG_BUS 
                    card_type = 3; 
#elif LIJIANG_BUS
                    card_type = 2;
#elif LIUKU_BUS
                    card_type = 2;
#elif LANPING_BUS
                    card_type = 3;
#endif                    
					break;

                case 0x04://�ذ��м��˿�
#if((defined YAN_AN_BUS) ||(defined NINGXIA_GUYUAN)||(defined DIQING_BUS))
                    card_type = 4;
#elif  YUXI_BUS
					card_type = 7;
#elif  PUER_BUS
                    card_type = 1;   
#elif  ZHAOTONG_BUS
                    card_type = 8;
#elif LIJIANG_BUS
                    card_type = 5;
#elif LIUKU_BUS
                    card_type = 9;
#elif LANPING_BUS
                    card_type = 3;
#endif                        
                    break;

                case 0x05://���˿�
#if((defined YAN_AN_BUS) ||(defined NINGXIA_GUYUAN)||(defined DIQING_BUS))
                    card_type = 5;
#elif  YUXI_BUS
					card_type = 2;
#elif  PUER_BUS
                    card_type = 3; 
#elif ZHAOTONG_BUS 
                    card_type = 3; 
#elif LIJIANG_BUS
                    card_type = 4;
#elif LIUKU_BUS
                    card_type = 8;
#elif LANPING_BUS
                    card_type = 3;
#endif                    
                    break;

                case 0x06:
#if((defined YAN_AN_BUS) ||(defined NINGXIA_GUYUAN)||(defined DIQING_BUS))
                    card_type = 6;
#elif  YUXI_BUS
					card_type = 3;
#elif  PUER_BUS
                    card_type = 1; 
#elif ZHAOTONG_BUS 
                    card_type = 8; 
#elif LIJIANG_BUS
                    card_type = 5;
#elif LIUKU_BUS
                    card_type = 9;
#elif LANPING_BUS
                    card_type = 1;
#endif                    
                    break;

            default:
#if((defined YAN_AN_BUS) ||(defined NINGXIA_GUYUAN)||(defined DIQING_BUS))
                    card_type = 1;
#elif  YUXI_BUS
					card_type = 7;
#elif  PUER_BUS
                    card_type = 1; 
#elif  ZHAOTONG_BUS
                    card_type= 8;
#elif LIJIANG_BUS
                    card_type = 5;
#elif LIUKU_BUS
                    card_type = 9;
#elif LANPING_BUS
                    card_type = 1;
#endif                    
             
                break;


                    
			}
    return card_type;
    }


unsigned char Permissions_jiaotong(unsigned char Mode)
{

	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char status;
	unsigned char card_type;

	while(Loop)
	{
		switch (step)
		{
		case 1:		
			//��ǰ��û�кϷ�������ʱ���
			status = TimeRange();
			if(status != MI_OK)
			{
				TypeTime.TimeNum = status;
				step++;
			}
			else
			{
				Err_display(9);
				Loop = 0;
			}
			break;	

		case 2:
			card_type = GetCardType();
			status = CardDiscounts_Cpu_jiaotong(card_type,Mode);
			if(status != MI_OK)step++;
			else
			{
				Err_display(22);
				Loop = 0;
			}
			break;

		default:
			step = 0;
			Loop = 0;
			break;
		}
	}

	return(step);
}

unsigned char Card_JudgeDate_jiaotong(unsigned char *starttime,unsigned char *endtime)
{
	
		unsigned char status = 1;
		unsigned char t;
		unsigned char buff[7];
		unsigned int start_time,end_time,this_time;
	
		for(t = 0; t<3; t++)
		{
			memset(buff,0,sizeof(buff));
			Rd_time (buff);
			Time.year = buff[0];
			Time.month = buff[1];
			Time.day = buff[2];
			Time.hour = buff[3];
			Time.min = buff[4];
			Time.sec = buff[5];
	
			this_time = 0x20<<24|buff[0]<<16|buff[1]<<8|buff[2];
			start_time = starttime[0]<<24|starttime[1]<<16|starttime[2]<<8|starttime[3];
			end_time = endtime[0]<<24|endtime[1]<<16|endtime[2]<<8|endtime[3];
			DBG_DATA_PRINTF("%x \n", this_time);
			DBG_DATA_PRINTF("%x \n", start_time);
			DBG_DATA_PRINTF("%x \n", end_time);
			if(this_time>=start_time && this_time<=end_time)
			{
				status = 0;
				break;
			}
            if(start_time> this_time)
            {
                status = 1;
                break;
                }
            else if(end_time<this_time)
            {
                status = 2;
                break;
                }
            
		}
			return status;
}

#endif 

/*
*************************************************************************************************************
- �������� : void JudgeDate(void)
- ����˵�� : �жϿ�Ƭ���ڵĺϷ�����
- ������� : ��
- ������� : �������
*************************************************************************************************************
*/
unsigned char Card_JudgeDate(void)
{
	unsigned char status = 1;
	unsigned char i,t;
	unsigned char buff[7];
	LongUnon JackArm,Test,Buf;

	DBG_RC500_PRINTF("Card_JudgeDate() is called.\n");

	for(t = 0; t<5; t++)
	{
		memset(buff,0,sizeof(buff));
		Rd_time (buff);
		Time.year = buff[0];
		Time.month = buff[1];
		Time.day = buff[2];
		Time.hour = buff[3];
		Time.min = buff[4];
		Time.sec = buff[5];

		Buf.longbuf[0] = Time.day;
		Buf.longbuf[1] = Time.month;
		Buf.longbuf[2] = Time.year;
		Buf.longbuf[3] = 0x20;

		if (CardLan.CardType == OPERATOR_CARD)
		{
			for(i = 0; i< 4 ; i++)Test.longbuf[i] = OperCard.OScOne.IssueDate[3-i];          //CardLan.EnableH[3 - i];
			for(i = 0; i< 4 ; i++)JackArm.longbuf[i] = OperCard.OScOne.InvalidDate[3-i];//CardLan.Effective[3 - i];
		}
		else
		{
			for(i = 0; i< 4 ; i++)Test.longbuf[i] = UserCard.ScOne.IssueDate[3-i];          //CardLan.EnableH[3 - i];
			for(i = 0; i< 4 ; i++)JackArm.longbuf[i] = UserCard.ScOne.InvalidDate[3-i];//CardLan.Effective[3 - i];
		}

		
		if((Buf.i >= Test.i)&&(Buf.i <= JackArm.i)) status = 0;
		else if(Buf.i < Test.i)status = 1;
		else if(Buf.i > JackArm.i)status = 2;
		if(status == 0)break;
	}

	return status;
}


/*
*************************************************************************************************************
- �������� : unsigned char CheakSertorFormatIsRight(char *SectorBuf, unsigned char type)
- ����˵�� : �������ĸ�ʽ����У�飬��ȷ����������ʽ������ȷ��
- ������� : SectorBuf : ��������		type: У������
- ������� : ��
*************************************************************************************************************
*/
unsigned char CheakSertorFormatIsRight(const unsigned char *SectorBuf, unsigned char type, unsigned char Flag)
{
	DebugPrintf("type = 0x%02X\n", type);
	unsigned int ak0,ak1,ak2;
	unsigned char status = 1, i,XOR;
	LongUnon Buf;
	
	if(type == CHEAK_BIT_FORMAT)
	{

		XOR = SectorBuf[0];
		for(i = 1; i <15; i++)
		{
			XOR ^= SectorBuf[i];
		}
		
		if(XOR == SectorBuf[15])							//У��λ�ɹ�
		{
			if(Flag)
				status = 0;
			else if((SectorBuf[0] >= 1)&&(SectorBuf[0] <=3))
				status = 0;
		}
	}
	else
	{
		
		memcpy(Buf.longbuf, SectorBuf, 4);
		ak0 = Buf.i;
		memcpy(Buf.longbuf, SectorBuf+4, 4);
		ak1 = Buf.i;
		memcpy(Buf.longbuf, SectorBuf+8, 4);
		ak2 = Buf.i;
		if((ak0 == ak2) && (ak0 == ~ak1) && (SectorBuf[12] == SectorBuf[14]) && (SectorBuf[13] == SectorBuf[15]) \
				&& ((SectorBuf[12]^SectorBuf[13]) == 0xff) && (ak0 < 0x7fffffff))
			status = 0;			

	}
	
	DebugPrintf("status = 0x%02X\n", status);
	
	return status;

}


unsigned char CheakIsNeedWriteKey(unsigned char SectorNo, struct card_buf* KeyInfo, unsigned char VerifyFlag)
{
	unsigned char TempBuf[8], ReturnFlag = 0;
	static unsigned char LastKeyType = 0; 
	
	DebugPrintf("LastKeyType = 0x%02X  SectorNo = 0x%02X\n ",LastKeyType, SectorNo);
	
	if((SectorNo < 8 && LastKeyType == 2) || (SectorNo >= 8 && LastKeyType == 1) || (!LastKeyType) || (VerifyFlag == 1))
	{
#ifdef GUANGZHOU_WEISHENG		
		if(0)	//�ô���У�͵ı�׼��У��������Կ��һ��
#else			
	//	if(SectorNo >= 8)
	    if(0)
#endif			
		{
			DebugPrintf("KEYA \n");
			LastKeyType = 2;
			
			KeyInfo->mode = KEYA;
			memset(KeyInfo->key, 0xFF, 6);
			memset(KeyInfo->rwbuf, 0xff, 16);
		}
		else
		{
			DebugPrintf("KEYB \n");
			LastKeyType = 1;
			KeyInfo->mode = KEYB;
			memset(KeyInfo->key, 0xFF, 6);
			memset(KeyInfo->rwbuf, 0xff, 16);
			memcpy(TempBuf, CardLan.UserIcNo, 4);
			memcpy(TempBuf+4, SnBack, 4);
			DES_CARD(KeyDes, TempBuf, KeyInfo->key);
		}
		
		return 0x01;
	}	
	
	
	return MI_OK;
}


/*
*************************************************************************************************************
- �������� : unsigned char WriteOneSertorDataToCard(unsigned char *SectorBuf, unsigned char SectorNo, unsigned char BlockNo, unsigned char VerifyFlag)
- ����˵�� : ������д�ؿ���
- ������� : SectorBuf: Ҫд�������	SectorNo:д���Ǹ�����  BlockNo: д��������ĵڼ���
- ������� : ��
*************************************************************************************************************
*/
unsigned char WriteOneSertorDataToCard(const unsigned char *SectorBuf, unsigned char SectorNo, unsigned char BlockNo, unsigned char VerifyFlag)
{
	DebugPrintf("SectorNo = 0x%02X BlockNo = 0x%02X\n",SectorNo, BlockNo);
	unsigned char Loop = 1, step = 1, ReturnValue = 0;
	unsigned char TempBuf[8],ReadBuf[16];
	struct card_buf KeyInfo;
	unsigned char authentime=0;
	memset(&KeyInfo, 0, sizeof(struct card_buf));
	int i, bufIndex;
	
	KeyInfo.mode = KEYA;
	memset(KeyInfo.rwbuf, 0xff, 16);
	switch(SectorNo)
	{
		case 1:
			//memset(KeyInfo.key, UserKey.ScOne, 6);
			memset(KeyInfo.key, CardLan.UserIcNo, 4);
			memset(KeyInfo.key+4, CardLan.UserIcNo, 2);
			break;
		case 2:
			memset(KeyInfo.key, UserKey.ScTwo, 6);
			break;
		case 4:
			memset(KeyInfo.key, UserKey.ScFour, 6);
			break;
		case 5:
			memset(KeyInfo.key, UserKey.ScFive, 6);
			break;
		case 6:
			memset(KeyInfo.key, UserKey.ScSix, 6);
			break;
		case 8:
			memset(KeyInfo.key, UserKey.ScEigth, 6);
			break;
		case 9:
			memset(KeyInfo.key, UserKey.ScNine, 6);
			break;
		case 15:
			memset(KeyInfo.key, UserKey.ScFifteen, 6);
			break;
		default:
			return -1;  //can't find the sector
			break;
	}

	while(Loop)
	{
		switch(step)
		{
			case 1:
			ioctl(mf_fd, WRITE_TYPE, W_CHAR);
		
			if(write(mf_fd, &KeyInfo, sizeof(struct card_buf)) == MI_OK)
				step++;
			else 
				Loop = 0;
			break;
		case 2:
			if(ioctl(mf_fd, RC531_AUTHENT, (4*SectorNo + 3)) == MI_OK)
				step++;
			else 
			{                 
    			Loop = 0;
             }
			break;
	#if 0
			case 3:
				if(ioctl(mf_fd, WRITE_TYPE,W_CARD) == MI_OK)
					step++;
				else
					Loop = 0;
				break;
				
			case 4:
				if(ioctl(mf_fd, RC531_WRITE, (4*SectorNo + BlockNo)) == MI_OK)
				{
					if((ReturnValue = write(mf_fd, SectorBuf, 16)) == MI_OK)
						step++;
					else 
						Loop = 0;
				}
				else 
					Loop = 0;
				break;
	#else
			case 3:
				bufIndex = 0;
				for(i=0; i<3; i++)
				{
					if ((BlockNo & (SC_ZERO<<i)) == 0)  continue;
					if(ioctl(mf_fd, WRITE_TYPE, W_CARD) == MI_OK)
					{
						if(ioctl(mf_fd, RC531_WRITE, (4*SectorNo + i)) == MI_OK)
						{
							if((ReturnValue = write(mf_fd, SectorBuf+bufIndex*16, 16)) == MI_OK)
							{
								bufIndex++;
								step++;
							}
							else 
							{
								Loop = 0;
								break;
							}
						}
						else 
						{
							Loop = 0;
							break;
						}
					}
					else
					{
						Loop = 0;
						break;
					}
				}
				break;
	
	#endif
			default:
				step = 0;
				Loop = 0;
		}
	}

	//if(UseForTest)
	//	return 0x04;

	

	if(step >= 3)
	{
		printf("WriteOneSertorDataToCard [%02X %02X] step = 0x%02X\n", SectorNo, BlockNo, step);
		ReturnValue = (SC_ZERO<<i);
		step |= (ReturnValue<<4);
	}
	
	return step;
}



/*
*************************************************************************************************************
- �������� : int GetCurWallentSertor(void)
- ����˵�� : ��ȡ��ǰǮ���Ĳ�������
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char  GetCurWallentSertor(unsigned char CopyFlag, char *SectorBuf, int *SectorNum)
{
	
	unsigned char WallentType = Sector.SFivZero[14] & 0xf0;

	DebugPrintf("type = 0x%02X\n", CopyFlag);

	switch(WallentType)
	{
		case 0x10:
			*SectorNum = LanSec.Two;
			if(SectorBuf)
			{	
				if(CopyFlag == 1)
					memcpy(SectorBuf, Sector.STwoTwo, 16);
				else if(CopyFlag == 2)
					memcpy(SectorBuf, Sector.STwoOne, 16);
			}
			break;
		case 0x20:
			*SectorNum = LanSec.Thr;
			if(SectorBuf)
			{
				if(CopyFlag == 1)
					memcpy(SectorBuf, Sector.SThrTwo, 16);
				else if(CopyFlag == 2)
					memcpy(SectorBuf, Sector.SThrOne, 16);
			}
 			break;
		default:
			 if(CardLan.MonthOrCountflag == 2 || g_FlagMonCard == 2)   //Ԥ��ģ����ǿ�������Ǯ��
			 {
				*SectorNum = LanSec.Two;
				if(SectorBuf)
				{	
					if(CopyFlag == 1)
						memcpy(SectorBuf, Sector.STwoTwo, 16);
					else if(CopyFlag == 2)
						memcpy(SectorBuf, Sector.STwoOne, 16);
				}
			 }
			 else
			 {
				*SectorNum = LanSec.For;
				if(SectorBuf)
				{
					if(CopyFlag == 1)
						memcpy(SectorBuf, Sector.SForTwo, 16);
					else if(CopyFlag == 2)
						memcpy(SectorBuf, Sector.SForOne, 16);
				}
			 }
 			break;
	}

	
	return 0;
}



/*
*************************************************************************************************************
- �������� : int CopyWallentSertorToAnother(unsigned char WallentType, unsigned char type)
- ����˵�� : ��ȡ��ǰǮ���Ĳ�������
- ������� : WallentType Ǯ���Ĳ�������  type : �ǲ�������������   ���� �������������� 
- ������� : ��
*************************************************************************************************************
*/
int CopyWallentSertorToAnother(unsigned char type)
{
	DebugPrintf("\n");
	unsigned char WallentType = Sector.SFivZero[14] & 0xf0;
	#if 1
	//printf("CALL CopyWallentSertorToAnother \n");
	if(type)
	{
		memcpy(Sector.STwoOne, Sector.STwoTwo, 16);
		memcpy(Sector.SThrOne, Sector.SThrTwo, 16);
		memcpy(Sector.SForOne, Sector.SForTwo, 16);

	}
	else
	{
		memcpy(Sector.STwoTwo, Sector.STwoOne, 16);
		memcpy(Sector.SThrTwo, Sector.SThrOne, 16);
		memcpy(Sector.SForTwo, Sector.SForOne, 16);
	}
	#else
	if(type)
	{
		switch(WallentType)
		{
			case 0x10:
				memcpy(Sector.STwoOne, Sector.STwoTwo, 16);
				break;
			case 0x20:
				memcpy(Sector.SThrOne, Sector.SThrTwo, 16);
				break;
			default:
				memcpy(Sector.SForOne, Sector.SForTwo, 16);
				break;
		}
	}
	else
	{
		switch(WallentType)
		{
			case 0x10:
				memcpy(Sector.STwoTwo, Sector.STwoOne, 16);
				break;
			case 0x20:
				memcpy(Sector.SThrTwo, Sector.SThrOne, 16);
				break;
			default:
				memcpy(Sector.SForTwo, Sector.SForOne, 16);
				break;
		}
	}
	#endif
	return 0;
	
}


unsigned char  FreeErrorRecordList(void)
{
	DebugPrintf("\n");
	struct ErrorRecordLsit *p = NULL, *q = NULL;
	
	p = pErrorRecHead;
	while(p->next != NULL)
	{
		q = p->next;
		p->next = q->next;
		free(q);
	}
	
	free(pErrorRecHead);
}


/*
*************************************************************************************************************
- �������� :unsigned char  VerifySaveDataIsRight(RecordFormat SaveDataInfo)
- ����˵�� :У�鱣��������Ƿ���ȷ
- ������� :��
- ������� :��
*************************************************************************************************************
*/
unsigned char  VerifySaveDataIsRight(RecordFormat SaveDataInfo)
{
	unsigned char TempSaveBuf[72], DXOR;

	memset(TempSaveBuf, 0, sizeof(TempSaveBuf));
	memcpy(TempSaveBuf, &SaveDataInfo, sizeof(RecordFormat));

    DXOR = Save_Data_Xor(TempSaveBuf);

    if(DXOR == TempSaveBuf[63])
   	{
		return MI_OK;
	}

	printf("VerifySaveDataIsRight error\n");
	
	return MI_FAIL;
}


/*
*************************************************************************************************************
- �������� :int DeleteOneErrorRecordFromList(struct ErrorRecordLsit *pErrorRec)
- ����˵�� :��������ɾ��һ������
- ������� :pErrorRec : Ҫɾ�������ݽṹ��
- ������� :��
*************************************************************************************************************
*/
int DeleteOneErrorRecordFromList(unsigned char *CardNo, unsigned char RecordType)
{
	DebugPrintf("\n");
	struct ErrorRecordLsit *p = pErrorRecHead, *q = NULL;
	struct UesForDisplayData DisplayData;
	char * pSavedataInfo = NULL;
    int transmode;
	
	memset(&DisplayData, 0, sizeof(struct UesForDisplayData));

	pthread_mutex_lock(&m_ErrorList);
	
	while(p->next)
	{
		if(strncmp(p->next->CardNo, CardNo, 4) == 0)
		{
			DebugPrintf("####p->next->CardNo = %02X%02X%02X%02X\n", CardNo[0], CardNo[1], CardNo[2], CardNo[3]);
			q = p->next;
			p->next = q->next;
			break;
 		}
		p = p->next;
	}
	pthread_mutex_unlock(&m_ErrorList);
	
	memset(&SaveData, 0, sizeof(RecordFormat));	
	memcpy(&SaveData, &q->SaveRecordBuf, sizeof(RecordFormat));

	if(RecordType)
	{
		SaveData.RFXFtype = 0x07;
		pSavedataInfo = (unsigned char *)(&SaveData);
		SaveData.RFXor = Save_Data_Xor(pSavedataInfo);
	}
	else
	{
		//��������������Ļ��ʾ
		memcpy(&CardLan, &q->DisplayData.cardlan, sizeof(CardInform));
		HostValue.i = q->DisplayData.HostValue.i;
		Sector.FlagValue = q->DisplayData.FlagValue;
		bStolenDis = q->DisplayData.bStolenDis;
		StolenAmount = 	q->DisplayData.StolenAmount;
	}

    if(Section.Enable!=0x55)
        transmode = 0;
    else
        transmode = 2;
#ifdef Transport_Stander
    if(SaveCardData_jiaotong(CARD_SPEC_M1_LINUX, transmode, SAVE_RECORD) != MI_OK)
#else
	if(SaveCardData(CARD_SPEC_M1_LINUX, transmode, SAVE_RECORD) != MI_OK)
#endif        
	{
		return -1;
	}
	AutoUpFlag = 0x55;
	free(q);

	return 0;
}


/*
*************************************************************************************************************
- �������� :int AddOneErrorRecordtoList(const struct ErrorRecordLsit *pErrorRec)
- ����˵�� :����һ�����ݵ�������
- ������� :pErrorRec : Ҫɾ�������ݽṹ��
- ������� :��
*************************************************************************************************************
*/
unsigned char AddOneErrorRecordtoList(const struct ErrorRecordLsit *pErrorRec)
{
	DebugPrintf("\n");
	
	struct ErrorRecordLsit *p = pErrorRecHead, *New = NULL;

	New = (struct ErrorRecordLsit *)malloc(sizeof(struct ErrorRecordLsit));
	
	memcpy(New, pErrorRec, sizeof(struct ErrorRecordLsit));
	New->next = NULL;
	
	DebugPrintf("pErrorRec->CardNo %02X%02X%02X%02X\n", New->CardNo[0], New->CardNo[1],New->CardNo[2],New->CardNo[3]);
	DebugPrintf("pErrorRec->RecordTime = %ld\n", New->RecordTime);
	
	pthread_mutex_lock(&m_ErrorList);

	while(p->next)
	{
		if(	strncmp(p->CardNo, pErrorRec->CardNo, 4) == 0)
		{
			free(New);
			pthread_mutex_unlock(&m_ErrorList);
			return 0;
		}
		p = p->next;
	}
	p->next = New;

	pthread_mutex_unlock(&m_ErrorList);
	return 0;
		
}

/*
*************************************************************************************************************
- �������� :void ReadErrorRecordFile(FILE *RecErrFp)
- ����˵�� :����ʱ����ȡ�����ļ������л�ȡ����һ�ιػ�֮ǰ�Ĵ����¼
- ������� :��
- ������� :��
*************************************************************************************************************
*/
unsigned char ReadErrorRecordFile(unsigned char FreeListFlag)
{
	DebugPrintf("\n");
	int CurCount = 0, i, RecHead = 0, RecFd, j, VerifyFlag = 0;
	char TempSaveBuf[128];
	struct ErrorRecordLsit New;

	if(FreeListFlag)
		FreeErrorRecordList();

	if(access(RECORD_FILE_PATH, F_OK) == 0)
	{
		if((RecFd = open(RECORD_FILE_PATH, O_RDONLY)) < 0)
		{
			perror("Verify savedata error\n");
			close(bp_fd);
			close(mf_fd);
			exit(-1);
		}
		printf("open RecordErrorFile.txt ok!\n");
	}
	else
	{
		printf("Have no RecordErrorFile.txt\n");
		return MI_OK;
	}
	
	lseek(RecFd, 0, SEEK_SET);
	read(RecFd, &RecHead, sizeof(int));
	printf("RecHead = %d\n",RecHead);
		
	if( RecHead == 0)
	{
		RecHead = 0;
		lseek(RecFd, 0,SEEK_SET);
		write(RecFd, &RecHead, sizeof(int));
	}
	else
	{
		lseek(RecFd, 0,SEEK_SET);
		while(CurCount < RecHead)
		{
			//���������ӵ�������
			i = sizeof(int) + CurCount*sizeof(struct ErrorRecordLsit);
			for(j = 3; j > 0; j--)
			{

				lseek(RecFd, i, SEEK_SET);
				read(RecFd, &New, sizeof(struct ErrorRecordLsit));
				DebugPrintf("####New.CardNo = %02x%02x%02x%02x\n", New.CardNo[0], New.CardNo[1], New.CardNo[2], New.CardNo[3]);
				//У�����ݰ�������ԵĻ���������ȥ�ļ�����ȥ��ȡ

				if(VerifySaveDataIsRight(New.SaveRecordBuf) == MI_OK)
				{
					AddOneErrorRecordtoList(&New);
					break;
				}
			}
			CurCount++;	
		}
		
		if(CurCount == RecHead)
			return MI_OK;
	}

	printf("ReadErrorRecordFile [0x%02X\n]", j);
	return MI_FAIL;
}




/*
*************************************************************************************************************
- �������� : unsigned char VerifyListSavedataIsRight(void)
- ����˵�� :У�鵱ǰ�����Ҫ��������������Ƿ�ȫ����ȷ���������ȷ����ȥ�ļ������ȡ
- ������� : BackupCompleteFlag : ���ݱ�־
- ������� : ��
*************************************************************************************************************
*/
unsigned char VerifyListSavedataIsRight(void)
{
	DebugPrintf("\n");
	struct ErrorRecordLsit *p = pErrorRecHead;
	pthread_mutex_lock(&m_ErrorList);
	while(p->next)
	{
	//	printf("##2##New.CardNo = %02x%02x%02x%02x\n", p->next->CardNo[0],p->next->CardNo[1], p->next->CardNo[2], p->next->CardNo[3]);

		if(VerifySaveDataIsRight(p->next->SaveRecordBuf) == MI_FAIL)
		{
			DebugPrintf("VerifyListSavedataIsRight one savedata error\n");
			if(ReadErrorRecordFile(1) == MI_FAIL)
			{
				printf("ReadErrorRecordFile save data error\n");

				pthread_mutex_unlock(&m_ErrorList);

				return MI_FAIL;
			}
		}
		p = p->next;
	}
	pthread_mutex_unlock(&m_ErrorList);

	return MI_OK;
}


/*
*************************************************************************************************************
- �������� : int CheakBackupIsCompleteAndRepair(unsigned char BackupCompleteFlag)
- ����˵�� : �鿴��һ���Ƿ񱸷���ɣ����û����� ��
		   1.���������ɣ��򽫱���ȥд��������
		   2.���û�б�����ɣ��򽫲���ȥ������������
		   3.�ڲ�������֮�󣬽���־λ��ΪĬ�ϱ�����
- ������� : BackupCompleteFlag : ���ݱ�־
- ������� : ��
*************************************************************************************************************
*/

int CheakBackupIsCompleteAndRepair(unsigned char BackupCompleteFlag)
{
	DebugPrintf("\n");
	unsigned char TempBuf[16], TempBuf_1[16],Xor;
	int SectorNum = 0, S5B1Flag = 0, S4B0Flag = 0, i;
	unsigned char check_backup = 1;
	printf("start BackupCompleteFlag = 0x%02X  [0x%02X]\n", Sector.SOneZero[5], RepairLastRecErrorFlag);

	if(((BackupCompleteFlag == BACKUP_COMPLETE)||(BackupCompleteFlag == 0)) && (!RepairLastRecErrorFlag))
	{
		//�Ե�������0��͵�������1�����У��
		#ifndef CANGNAN_BUS
		memcpy(TempBuf, Sector.SFivOne, 16);
		if(CheakSertorFormatIsRight(TempBuf, CHEAK_BIT_FORMAT, 0) == MI_OK)
		{
			S5B1Flag = 1;
		}
		else
		{
			memcpy(TempBuf, Sector.SFivZero, 16);
			
			if(CheakSertorFormatIsRight(TempBuf, CHEAK_BIT_FORMAT, 0) == MI_OK)
			{
				S5B1Flag = 0;
			}
			else
			{
				printf("S5B0 S5B1 error\n");
				return -1;
			}
		}
		
		if(memcmp(Sector.SFivOne, Sector.SFivZero, 16) != 0)
		{
			if(S5B1Flag)
			{
				memcpy(Sector.SFivZero, Sector.SFivOne, 16);
				if(WriteOneSertorDataToCard(TempBuf, LanSec.Fiv, 0, WRITE_KEY) != MI_OK)
					return -1;
			}
			else
			{
				memcpy(Sector.SFivOne, Sector.SFivZero, 16);
				if(WriteOneSertorDataToCard(TempBuf, LanSec.Fiv, 1, WRITE_KEY) != MI_OK)
					return -1;
			}
		}
        #endif
	//	printf("####[zero: %02X %02X one: %02X %02X]\n", \
	//		Sector.SFivZero[1], Sector.SFivZero[2],Sector.SFivOne[1], Sector.SFivOne[2]);

LINE_BACK:
		//�Ե�������1��͵�������2�����У��
		GetCurWallentSertor(GET_BACKUP_SERTOR, TempBuf, &SectorNum);
		//GetCurWallentSertor(GET_OPERATION_SERTOR, TempBuf_1, &SectorNum);

		if((CheakSertorFormatIsRight(TempBuf, WALLET_FORMAT, 0) == MI_OK) && (g_FgOprationSuccess == 0))
		{
			//if(memcmp(TempBuf, TempBuf_1, 16) != 0)
			if ((memcmp(Sector.STwoTwo, Sector.STwoOne, 16) != 0)||(memcmp(Sector.SThrTwo, Sector.SThrOne, 16) != 0)||(memcmp(Sector.SForTwo, Sector.SForOne, 16) != 0))
			{
				CopyWallentSertorToAnother(COPY_BACKUP_TO_OPERATION);
				//printf("===ye====error from MI_OK \n");
				if(WriteOneSertorDataToCard(Sector.STwoTwo, LanSec.Two, 1, VERIFY_KEY) != MI_OK)
					return -1;
				if(WriteOneSertorDataToCard(Sector.SThrTwo, LanSec.Thr, 1, VERIFY_KEY) != MI_OK)
					return -1;
				if(WriteOneSertorDataToCard(Sector.SForTwo, LanSec.For, 1, VERIFY_KEY) != MI_OK)
					return -1;
			}
		}
		else
		{

			GetCurWallentSertor(GET_OPERATION_SERTOR, TempBuf, &SectorNum);
			
			if(CheakSertorFormatIsRight(TempBuf, WALLET_FORMAT, 0) == MI_OK)
			{
				//if(memcmp(TempBuf, TempBuf_1, 16) != 0)
				//printf("===ye====error from no  MI_OK \n");
				if ((memcmp(Sector.STwoTwo, Sector.STwoOne, 16) != 0)||(memcmp(Sector.SThrTwo, Sector.SThrOne, 16) != 0)||(memcmp(Sector.SForTwo, Sector.SForOne, 16) != 0))
				{
					CopyWallentSertorToAnother(COPY_OPERATION_TO_BACKUP);
					
					if(WriteOneSertorDataToCard(Sector.STwoOne, LanSec.Two, 2, VERIFY_KEY) != MI_OK)
						return -1;
					if(WriteOneSertorDataToCard(Sector.SThrOne, LanSec.Thr, 2, VERIFY_KEY) != MI_OK)
						return -1;
					if(WriteOneSertorDataToCard(Sector.SForOne, LanSec.For, 2, VERIFY_KEY) != MI_OK)
						return -1;
				}
			}
			else
			{
				printf("S4B1 S4B2 error\n");
				if (check_backup && g_FgOprationSuccess)  //�ȼ������������������Ǯ����ʽ���ԣ��ټ�鱸����
				{
					check_backup = 0;
					g_FgOprationSuccess = 0;
					goto  LINE_BACK;
				}
				return -1;
			}
		}

		//����������0�鿽������2����0�����У��
		memcpy(TempBuf, Sector.SForZero, 16);
		
		if(CheakSertorFormatIsRight(TempBuf, CHEAK_BIT_FORMAT, 1) == MI_OK)
		{
			S4B0Flag = 1;
		}
		else
		{
			memcpy(TempBuf, Sector.STwoZero, 16);
			
			if(CheakSertorFormatIsRight(TempBuf, CHEAK_BIT_FORMAT, 1) == MI_OK)
			{
				S4B0Flag = 0;
			}
			else
			{
				printf("S2B0 S4B0 error\n");
				return -1;
			}
		}
		
		if(memcmp(Sector.STwoZero, Sector.SForZero, 16) != 0)
		{
			if(S4B0Flag)
			{
				memcpy(Sector.STwoZero, Sector.SForZero, 16);
				if(WriteOneSertorDataToCard(Sector.SForZero, LanSec.Two, 0, VERIFY_KEY) != MI_OK)
					return -1;
			}
			else
			{
				memcpy(Sector.SForZero, Sector.STwoZero, 16);
				if(WriteOneSertorDataToCard(Sector.STwoZero, LanSec.For, 0, VERIFY_KEY) != MI_OK)
					return -1;
			}
		}		
		
	}
	else
	{
		//���ݲ�����������£����Բ�����Ϊ�����������ȥУ�������ֱ�ӷ���
		//У���5����0�� ������
		#ifndef CANGNAN_BUS
		memcpy(TempBuf, Sector.SFivZero, 16);
		if(CheakSertorFormatIsRight(TempBuf, CHEAK_BIT_FORMAT, 0) == MI_OK)
		{
			memcpy(Sector.SFivOne, Sector.SFivZero, 16);
			if(WriteOneSertorDataToCard(Sector.SFivZero, LanSec.Fiv, 1, WRITE_KEY) != MI_OK)
				return -1;
		}
		else
		{
			printf("S5B0 error\n");
			return -1;
		}
        #endif    
		//У���4����1�� ������
		GetCurWallentSertor(GET_OPERATION_SERTOR, TempBuf, &SectorNum);
		DebugPrintf("SectorNum = 0x%02X\n", SectorNum);
		
		if(CheakSertorFormatIsRight(TempBuf, WALLET_FORMAT, 0) == MI_OK)
		{
			CopyWallentSertorToAnother(COPY_OPERATION_TO_BACKUP);
			if(WriteOneSertorDataToCard(Sector.STwoOne, LanSec.Two, 2, VERIFY_KEY) != MI_OK)
				return -1;
			if(WriteOneSertorDataToCard(Sector.SThrOne, LanSec.Thr, 2, VERIFY_KEY) != MI_OK)
				return -1;
			if(WriteOneSertorDataToCard(Sector.SForOne, LanSec.For, 2, VERIFY_KEY) != MI_OK)
				return -1;
		}
		else
		{
			printf("S4B1 error\n");
			return -1;
		}

		//��2������0��Ĳ���
		memcpy(TempBuf, Sector.STwoZero, 16);
		
		if(CheakSertorFormatIsRight(TempBuf, CHEAK_BIT_FORMAT, 1) == MI_OK)
		{
			memcpy(Sector.SForZero ,Sector.STwoZero, 16);
			if(WriteOneSertorDataToCard(Sector.STwoZero, LanSec.For, 0, VERIFY_KEY) != MI_OK)
				return -1;
		}
		else
		{
			printf("S2B0 error\n");
			return -1;
		}

		//�����ݱ�־��Ϊ��Ǯ������Ϊ��
		Sector.SOneZero[5] = BACKUP_COMPLETE;
		if(WriteOneSertorDataToCard(Sector.SOneZero, LanSec.One, 0, VERIFY_KEY) != MI_OK)
			return -1;

	}
    
	if(RepairLastRecErrorFlag) 
	{
		VerifyListSavedataIsRight();
		DeleteOneErrorRecordFromList(CardLan.UserIcNo, 0);
		WriteAllErrorRecordToFile();
		return RepairLastRecErrorFlag;
	}
	
	printf("end BackupCompleteFlag = 0x%02X\n", Sector.SOneZero[5]);
	return MI_OK;	
}



/*
*************************************************************************************************************
- �������� : unsigned char ReadOneSectorDataFromCard(unsigned char *SectorBuf, unsigned char SectorNo, unsigned char BlockNo)
- ����˵�� : �ӿ��ж�ȡһ������������
- ������� : SectorBuf: Ҫ����������	BufLen: ����������  SectorNo:�����Ǹ�����  VerifyFlag:У���־
- ������� : ��
- ע������ : BlockNo ������ʾ������飬��BIT2~BIT0 ��ʾ2~0��
*************************************************************************************************************
*/
unsigned char ReadOneSectorDataFromCard(unsigned char *SectorBuf, unsigned char BufLen ,
		unsigned char SectorNo, unsigned char BlockNo, unsigned char VerifyFlag)
{	
	DebugPrintf("SectorNo = 0x%02X BlockNo = 0x%02X\n",SectorNo, BlockNo);
	unsigned char Loop = 1, step = 1, ReturnValue = 0;
	unsigned long ReadLen = 0;
	unsigned char TempBuf[8],ReadBuf[16];
	unsigned char authentime=0;
	struct card_buf KeyInfo;
	static unsigned char LastKeyType = 0; 
	int i, bufIndex = 0;
	memset(&KeyInfo, 0, sizeof(struct card_buf));
	
	memset(ReadBuf, 0, sizeof(ReadBuf));

	if(SectorBuf == NULL)
	{
		printf("SectorBuf is NULL\n");
		return -1;
	}
	
	KeyInfo.mode = KEYA;
	memset(KeyInfo.rwbuf, 0xff, 16);

	switch(SectorNo)
	{
		case 0:
			KeyInfo.key[0] = 0xa0;
			KeyInfo.key[1] = 0xa1;
			KeyInfo.key[2] = 0xa2;
			KeyInfo.key[3] = 0xa3;
			KeyInfo.key[4] = 0xa4;
			KeyInfo.key[5] = 0xa5;
			break;
		case 1:
			memcpy(KeyInfo.key, CardLan.UserIcNo, 4);
			//memset(KeyInfo.key+4, CardLan.UserIcNo, 2);
			KeyInfo.key[4] = ~CardLan.UserIcNo[0];
			KeyInfo.key[5] = ~CardLan.UserIcNo[1];
			break;
		case 2:
			memset(KeyInfo.key, UserKey.ScTwo, 6);
			break;
		case 4:
			memset(KeyInfo.key, UserKey.ScFour, 6);
			break;
		case 5:
			memset(KeyInfo.key, UserKey.ScFive, 6);
			break;
		case 6:
			memset(KeyInfo.key, UserKey.ScSix, 6);
			break;
		case 8:
			memset(KeyInfo.key, UserKey.ScEigth, 6);
			break;
		case 9:
			memset(KeyInfo.key, UserKey.ScNine, 6);
			break;
		case 15:
			memset(KeyInfo.key, UserKey.ScFifteen, 6);
			break;
		default:
			return -1;  //can't find the sector
			break;
	}


	while(Loop)
	{
		switch(step)
		{
		case 1:
			ioctl(mf_fd, WRITE_TYPE, W_CHAR);

			if(write(mf_fd, &KeyInfo, sizeof(struct card_buf)) == MI_OK)
				step++;
			else 
				Loop = 0;
			break;
		case 2:
			if(ioctl(mf_fd, RC531_AUTHENT, (4*SectorNo + 3)) == MI_OK)
				step++;
			else 
			{
                
    		    Loop = 0;
             }
			break;

		case 3:
			bufIndex = 0;
			for(i=0; i<3; i++)
			{
				if ((BlockNo & (SC_ZERO<<i)) == 0)  continue;
				if(ioctl(mf_fd, RC531_READ,(4*SectorNo + i)) != MI_OK)
				{
					Loop = 0;
					break;
				}
				
				if(ioctl(mf_fd, FIFO_RCV_LEN, &ReadLen) != MI_OK)
				{
					Loop = 0;
					break;
				}
				
				if(read(mf_fd, ReadBuf, ReadLen) >= 0)
				{
					if(ReadBuf)
					{
						memcpy(SectorBuf+bufIndex*16, ReadBuf, ReadLen);
						step++;
						bufIndex++;
					}
					else
					{
						DebugPrintf("read error\n");
						Loop = 0;
						break;
					}
				}
				else
				{
					//DebugPrintf("ReturnValue = 0x%02X\n", ReturnValue);
					Loop = 0;
				}
			}
			break;
			
		default:
            
			step = 0;
			Loop = 0;
			break;
		
		}
	}
	
	if (!step)
	{
		printf("read sector = %d read len =%d :\n", SectorNo, bufIndex*16);
		for(i=0; i<bufIndex*16; i++)
			printf("0x%02x ", SectorBuf[i]);
		printf("\n");
	}
	else
		printf("ReadOneSectorDataFromCard ret = %d \n", step);
	return step;
}



/*
*************************************************************************************************************
- �������� :int WriteAllErrorRecordToFile(void)
- ����˵�� :�����������еļ�¼д�뵽�ļ���
- ������� :��
- ������� :��
*************************************************************************************************************
*/
int WriteAllErrorRecordToFile(void)
{
	DebugPrintf("\n");
	char SaveBuffer[4096];
	int ReturnValue = 0, offset = 0, fd = 0, i, count = 0;
	struct ErrorRecordLsit TempSavedata, *p = pErrorRecHead;

	memset(SaveBuffer, 0, sizeof(SaveBuffer));
	
	pthread_mutex_lock(&m_ErrorRecFile);

#if 0
	if(p->next == NULL)
	{
		if(access(RECORD_FILE_PATH, F_OK) == 0)
			unlink("RecordErrorFile.txt");
			
		pthread_mutex_unlock(&m_ErrorRecFile);
		DebugPrintf("Have no RecordErrorFile.txt\n");
		return MI_OK;
	}
#endif

	if((fd = open(RECORD_FILE_PATH,  O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) < 0)
	{
		perror("O_WRONLY RecordErrorFile.txt error\n");
		exit(-1);
	} 
	
	while(p->next != NULL)
	{
		p = p->next;
		offset = sizeof(int) + (count * sizeof(struct ErrorRecordLsit));
		
		if(VerifySaveDataIsRight(p->SaveRecordBuf) == MI_OK)
		{
			for(i = 0; i < 5; i++)
			{
				DebugPrintf("offset = %02d\n", offset);
				lseek(fd, offset, SEEK_SET);
				write(fd, p, sizeof(struct ErrorRecordLsit));
				//Savedatasql(RecordFormat Save)
				usleep(100*1000);
				
				lseek(fd, offset, SEEK_SET);

				memset(&TempSavedata, 0, sizeof(struct ErrorRecordLsit));
				read(fd, &TempSavedata, sizeof(struct ErrorRecordLsit));

				if(VerifySaveDataIsRight(TempSavedata.SaveRecordBuf) == MI_OK)
					break;	
			}

			if(i == 5)
			{
				printf("WriteAllErrorRecordToFile write error\n");
				return MI_FAIL;
			}
			
			count++;
		}
		else
		{
			printf("verify save data error\n");
			return MI_FAIL;
		}
	}

	lseek(fd, 0, SEEK_SET);

	write(fd, &count, sizeof(int));
	close(fd);

	system("sync");
	
	pthread_mutex_unlock(&m_ErrorRecFile);
	
	return MI_OK;
}


void PrintCharDebug(char *name, char *data, int len)
{
	int i = 0;
	printf("%s :", name);
	for(i=0; i<len; i++)
	{
		printf("%c", data[i]);
	}
	printf("\n");
}

int GetClassNameLen(char *TempBuf, char *ClassName)
{
	int i = 0, j = 0, temp = 0;

	for(j=0; j<8; j++)
	{
		if(TempBuf[j] != 0)
		{
			ClassName[i++] = TempBuf[j];		
		}
	}
	ClassName[i] = '\0';
	return 0;
}


/*
*************************************************************************************************************
- �������� :int SeekRecordList(struct RecordCardTime *pTime )
- ����˵�� :��ѯ���������Ƿ�����ͬ�Ŀ���
- ������� :��
- ������� :��
*************************************************************************************************************
*/
int Timer_PollErrorRecordList(void)
{
	//DebugPrintf("\n");
	time_t NowTime;
	unsigned char WriteFlag = 0;
	struct ErrorRecordLsit *p = pErrorRecHead, *q = NULL;

	time(&NowTime);

	pthread_mutex_lock(&m_ErrorList);

	while(p->next)
	{
		DebugPrintf("NowTime = %u p->next->RecordTime = %u\n",NowTime,  p->next->RecordTime);
		if( NowTime - p->next->RecordTime > WRITE_RRROR_ECORD_INTERVAL)
		{
			//����ָ����ʱ�䣬������д��
			q = p->next;
			WriteFlag = 1;
			break;
		}
		else
		{
			p = p->next;
		}
	}
	pthread_mutex_unlock(&m_ErrorList);

	if(WriteFlag)
	{
		DebugPrintf("####### Timer_PollErrorRecordList[%02X]\n", WriteFlag);
		VerifyListSavedataIsRight();
		DeleteOneErrorRecordFromList(q->CardNo, 1);
		WriteAllErrorRecordToFile();
	}
	return 0;
}


/*
*************************************************************************************************************
- �������� :int InitErrorRecordList(void)
- ����˵�� :��ʼ����¼�����¼����
- ������� :��
- ������� :��
*************************************************************************************************************
*/
int InitErrorRecordList(void)
{
	pErrorRecHead = (struct ErrorRecordLsit *)malloc(sizeof(struct ErrorRecordLsit));  //����ͷ�ڵ㡣 

	if(pErrorRecHead == NULL)
	{
		printf("create head failed\n");
		return -1;
	}

	memset(pErrorRecHead, 0, sizeof(struct ErrorRecordLsit));
	pErrorRecHead->next = NULL;

	return 0;
}




/*
*************************************************************************************************************
- �������� :int SeekRecordList(struct RecordCardTime *pTime )
- ����˵�� :��ѯ���������Ƿ�����ͬ�Ŀ���
- ������� :��
- ������� :��
*************************************************************************************************************
*/
int SeekErrorRecordList(unsigned char *CardNo)
{
	DebugPrintf("\n");
	int i;
	struct ErrorRecordLsit *p = pErrorRecHead;

	if(CardNo == NULL)
		return -1;
	
	pthread_mutex_lock(&m_ErrorList);
	while(p->next)
	{
		DebugPrintf("\n");
		
		if(strncmp(p->next->CardNo, CardNo, 4) == 0)
		{
			memcpy(Sector.SOneZero, p->SectorOneData, 16);
			
			pthread_mutex_unlock(&m_ErrorList);
			
			return 1;
		}
		else
		{
			p = p->next;
		}
	}
	pthread_mutex_unlock(&m_ErrorList);
	return 0;
}

unsigned char ReadPartCardSectorData(void)
{
	DebugPrintf("\n");
	int LoopFlag = 1,StepFlag = 1, i;
	char receive_buf[20];
	unsigned char ReciveBufLen = 0;
	int SectorNum;
	LongUnon moneytest;

	memset(receive_buf, 0, sizeof(receive_buf));
	ReciveBufLen = sizeof(receive_buf);
	
	while(LoopFlag)
	{
		DebugPrintf("StepFlag = 0x%02X\n", StepFlag);
		switch(StepFlag)
		{
		case 1:
			if(ReadOneSectorDataFromCard(receive_buf, ReciveBufLen, LanSec.Fiv, 0, WRITE_KEY) == MI_OK)
			{
				memcpy(Sector.SFivZero,receive_buf,16);
				StepFlag++;
			}
			else
			{
				LoopFlag = 0;
			}
			break;
		case 2:
			if(ReadOneSectorDataFromCard(receive_buf, ReciveBufLen, LanSec.Two, 0, VERIFY_KEY) == MI_OK)
			{
				memcpy(Sector.STwoZero, receive_buf,16);
				StepFlag++;
			}
			else
			{
				LoopFlag = 0;
			}
			break;

		case 3:
			GetCurWallentSertor(GET_NO_SERTOR, NULL, &SectorNum);

			DebugPrintf("SectorNum = 0x%02X\n", SectorNum);

			if(ReadOneSectorDataFromCard(receive_buf, ReciveBufLen, SectorNum, 1, VERIFY_KEY) == MI_OK)
			{
				if(SectorNum == LanSec.Two)
					memcpy(Sector.STwoOne,receive_buf,16);
				else if(SectorNum == LanSec.Thr)
					memcpy(Sector.SThrOne,receive_buf,16);
				else 
					memcpy(Sector.SForOne,receive_buf,16);

				memcpy(moneytest.longbuf, receive_buf, 4);
				DebugPrintf("##1## moneytest = %u\n", moneytest.i);
				
				StepFlag++;
			}
			else
			{
				LoopFlag = 0;
			}
			break;
#if 0			
		case 4:
			if(ReadOneSectorDataFromCard(receive_buf, ReciveBufLen, LanSec.One, 0, WRITE_KEY) == MI_OK)
			{
				memcpy(Sector.SOneZero,receive_buf,16);

				StepFlag++;
			}
			else
			{
				LoopFlag = 0;
			}
			break;

		case 4:
			memset(Sector.SNinZero, 0, 16);
			StepFlag++;
			break;
#endif
			
		default:
			LoopFlag = 0;
			StepFlag = 0;
		}
	}
	
	printf("[%02X] [%02X %02X] start\n", StepFlag, Sector.SFivZero[1], Sector.SFivZero[2]);

	return StepFlag;
}

unsigned char ShowSwipeCardStatus(unsigned char type)
{
	switch(type)
	{
		case 0:		//����
	        LCDKeepDisp = 0;
	        SetColor(Mcolor);
			CardTwo =0;
			ReadcardFlag = 0;
			break;
		case 1:		//ˢ��������ʾ
			SetColor(Mcolor);
			SetTextColor(Color_red);
            #ifdef NEW0409
    		SetTextSize(48);
    		TextOut(0 , 75, "��ܰ��ʾ");
    		TextOut(0 , 144, "ˢ������");
    		TextOut(0	, 210,"������ˢ��");
            #else
    		SetTextSize(32);
    		TextOut(100 , 50, "��ܰ��ʾ");
    		TextOut(100 , 90, "ˢ������");
    		TextOut(85	, 130,"������ˢ��");
            #endif

			LEDL(1);
			beepopen(10);
			PlayMusic(15, 0);
			sleep(1);
			
			break;
		case 2:		//һƱ������
			LEDR(1);
			beepopen(2);
			ReturnDisplay(0);
			LEDR(0);
			LEDL(0);
			break;
		case 3:		//�ֶ�����
			LEDR(1);
			beepopen(2);
			ReturnDisplay(3);
			LEDL(0);
			LEDR(0);
			break;
		case 4:		//���³���
			beepopen(2);
			SetColor(Mcolor);
            #ifdef NEW0409
            SetTextSize(48);
			SetTextColor(Color_white);
			TextOut(0,75, "��ܰ��ʾ");
			TextOut(0,144,"���������ⲿ�洢����");
			TextOut(0,210,"���Ժ�");
            #else
			SetTextSize(32);
			SetTextColor(Color_white);
			TextOut(100,50, "��ܰ��ʾ");
			TextOut(30,100,"���������ⲿ�洢����");
			TextOut(110,150,"���Ժ�");
            #endif
			break;
		case 5:		//�����ˢ���ɹ�
			LEDR(1);
			beepopen(2);
			ReturnDisplay(4);
			PlayMusic(1, 0);
			LEDR(0);
			break;
		case 6:		//����ά��
			LEDR(1);
			beepopen(2);
			ReturnDisplay(5);
			LEDL(0);
			LEDR(0);
			break;
		case 7:
			LEDR(1);
			beepopen(2);
			ReturnDisplay(6);
			LEDL(0);
			LEDR(0);
			break;
		case 8:
			LEDR(1);
			beepopen(2);
			ReturnDisplay(8);
			LEDL(0);
			LEDR(0);
			break;  
                 
		default:
			printf("error show\n");
			break;
	
	}

	return MI_OK;
}

#if defined(CONFIG_BZLINUXBUS)
/*
*************************************************************************************************************
- �������� : unsigned char ReadCardInfor (void)
- ����˵�� : �û��������޸���Ƭ
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char ReadCardInfor (void)
{
	DebugPrintf("\n");
	int LoopFlag = 1,StepFlag = 1,ret, i;
	char receive_buf[20];
	unsigned char ReciveBufLen = 0;
	unsigned char Keybuf[8];
	LongUnon moneytest;
	char buf[20],len,type;
#if DBG_RC500
	{
		struct timeval now;
		gettimeofday(&now,0);
		DBG_RC500_PRINTF("ReadCardInfor() is called, time = %ld\"%06ld.\n", now.tv_sec, now.tv_usec);
	}
#endif

	memset(receive_buf, 0, sizeof(receive_buf));
	ReciveBufLen = sizeof(receive_buf);
	
	while(LoopFlag)
	{
		DebugPrintf("StepFlag = 0x%02X\n", StepFlag);
		switch(StepFlag)
		{
		case 1:
			if(ReadOneSectorDataFromCard(receive_buf, ReciveBufLen, LanSec.One, 0, WRITE_KEY) == MI_OK)
			{
				memcpy(Sector.SOneZero, receive_buf, 16);
				memcpy(CardLan.UserNum, receive_buf, 2); //�û����
				memcpy(CardLan.SMonth, receive_buf+9, 3);
				memcpy(CardLan.EMonth, receive_buf+12, 3);

				CardLan.MonthOrCountflag = receive_buf[8];

				StepFlag++;
			}
			else
			{
				LoopFlag = 0;
			}
			break;
		case 2:
			if(ReadOneSectorDataFromCard(receive_buf, ReciveBufLen, LanSec.One, 1, READ_ONLY) == MI_OK)
			{
				memcpy(Sector.SOneOne,receive_buf,16);
				memcpy(CardLan.CityId, receive_buf,2);
				memcpy(CardLan.AppId, receive_buf+2,2);
				memcpy(CardLan.CardCsnB, receive_buf+4,4);
				memcpy(CardLan.CardId, receive_buf+8,4);
				
				CardLan.CardType = receive_buf[12];
				CardLan.Pwdflag = receive_buf[14];
				
				StepFlag++;
			}
			else
			{
				LoopFlag = 0;
			}
			break;
		case 3:
			if(ReadOneSectorDataFromCard(receive_buf, ReciveBufLen, LanSec.One, 2, READ_ONLY) == MI_OK)
			{
				memcpy(Sector.SOneTwo, receive_buf,16);
				memcpy(CardLan.EnableH, receive_buf+4,4);
				memcpy(CardLan.Effective, receive_buf+8,4);
				memcpy(CardLan.UserWord, receive_buf+12,3);
				
				StepFlag++;
			}
			else
			{
				LoopFlag = 0;
			}
			break;

		case 4:
			if(ReadOneSectorDataFromCard(receive_buf, ReciveBufLen, LanSec.Two, 0, VERIFY_KEY) == MI_OK)
			{
				memcpy(Sector.STwoZero,receive_buf,16);
				StepFlag++;
			}
			else
			{
				LoopFlag = 0;
			}
			break;
		case 5:
			if(ReadOneSectorDataFromCard(receive_buf, ReciveBufLen, LanSec.Two, 1, READ_ONLY) == MI_OK)
			{
				memcpy(Sector.STwoOne,receive_buf,16);
				StepFlag++;
			
			}
			else
			{
				LoopFlag = 0;
			}
			break;

		case 6:
			if(ReadOneSectorDataFromCard(receive_buf, ReciveBufLen, LanSec.Two, 2, READ_ONLY) == MI_OK)
			{
				memcpy(Sector.STwoTwo,receive_buf,16);
				StepFlag++;
			}
			else
			{
				LoopFlag = 0;
			}
			break;

		case 7:
			if(ReadOneSectorDataFromCard(receive_buf, ReciveBufLen, LanSec.Thr, 0, VERIFY_KEY) == MI_OK)
			{
				memcpy(Sector.SThrZero,receive_buf,16);
				StepFlag++;
			}
			else
			{
				LoopFlag = 0;
			}
			break;

		case 8:
			if(ReadOneSectorDataFromCard(receive_buf, ReciveBufLen, LanSec.Thr, 1, READ_ONLY) == MI_OK)
			{
				memcpy(Sector.SThrOne,receive_buf,16);
				StepFlag++;
			}
			else
			{
				LoopFlag = 0;
			}
			break;

		case 9:
			if(ReadOneSectorDataFromCard(receive_buf, ReciveBufLen, LanSec.Thr, 2, READ_ONLY) == MI_OK)
			{
				memcpy(Sector.SThrTwo,receive_buf,16);
				StepFlag++;
			}
			else
			{
				LoopFlag = 0;
			}
			break;

		case 10:
			if(ReadOneSectorDataFromCard(receive_buf, ReciveBufLen, LanSec.For, 0, VERIFY_KEY) == MI_OK)
			{
				memcpy(Sector.SForZero,receive_buf,16);
				StepFlag++;
			}
			else
			{
				LoopFlag = 0;
			}
			break;

		case 11:
			if(ReadOneSectorDataFromCard(receive_buf, ReciveBufLen, LanSec.For, 1, READ_ONLY) == MI_OK)
			{
				memcpy(Sector.SForOne,receive_buf,16);
				memcpy(moneytest.longbuf, receive_buf, 4);
				DebugPrintf("##2## moneytest = %u\n", moneytest.i);

				StepFlag++;
			}
			else
			{
				LoopFlag = 0;
			}
			break;

		case 12:
			if(ReadOneSectorDataFromCard(receive_buf, ReciveBufLen, LanSec.For, 2, READ_ONLY) == MI_OK)
			{
				memcpy(Sector.SForTwo,receive_buf,16);
				StepFlag++;
			}
			else
			{
				LoopFlag = 0;
			}
			break;

		case 13:
			if(ReadOneSectorDataFromCard(receive_buf, ReciveBufLen, LanSec.Fiv, 0, VERIFY_KEY) == MI_OK)
			{
				memcpy(Sector.SFivZero,receive_buf,16);

				StepFlag++;
			}
			else
			{
				LoopFlag = 0;
			}
			break;

		case 14:
			if(ReadOneSectorDataFromCard(receive_buf, ReciveBufLen, LanSec.Fiv, 1, READ_ONLY) == MI_OK)
			{
				memcpy(Sector.SFivOne, receive_buf, 16);
				StepFlag++;
			}
			else
			{
				LoopFlag = 0;
			}
			break;

		case 15:
			if(ReadOneSectorDataFromCard(receive_buf, ReciveBufLen, LanSec.Fiv, 2, READ_ONLY) == MI_OK)
			{
				memcpy(Sector.SFivTwo,receive_buf,16);
				g_FlagMonCard = Sector.SFivTwo[14];   //�¼ӵİ��±��

            //    StepFlag = StepFlag+2;

				StepFlag++;

			}
			else
			{
				LoopFlag = 0;
			}
			break;

	        case 16: //modify by ye ��ȡת���Ż����һ�δ򿨵�ʱ��
	        		g_FgNoDiscount = 0;
			if(ReadOneSectorDataFromCard(receive_buf, ReciveBufLen, 8, 0, READ_ONLY) == MI_OK)
			{
				memcpy(Sector.SEigZero,receive_buf,16);
				StepFlag++;
			}
			else
			{
				//LoopFlag = 0;
				g_FgNoDiscount = 1;     //��8������Կ��֤ʧ�ܲ�����ʹ�û����Ż�
                			ret = ioctl(mf_fd, 0xAB);
				if(ret != MI_OK)
				{
					ret = ioctl(mf_fd, 0xAB);
					if(ret != MI_OK)
						LoopFlag = 0;
				}
		                else
		                   StepFlag++;
			}
			break;	
#if 0			
		case 16:
			if(ReadOneSectorDataFromCard(receive_buf, ReciveBufLen, 9, 0, VERIFY_KEY) == MI_OK)
			{
				memcpy(Sector.SNinZero,receive_buf,16);
				StepFlag++;
			}
			else
			{
				LoopFlag = 0;
			}
			break;
#endif			
		default:
			LoopFlag = 0;
			StepFlag = 0;
			break;
		
		}
	}
	
	printf("[#1][%02X] [%02X %02X] ", StepFlag, Sector.SFivZero[1], Sector.SFivZero[2]);
	printf("##[%02X %02X] start\n", Sector.SFivOne[1], Sector.SFivOne[2]);
	printf("Sector.SFivZero[3] = 0x%02X\n", Sector.SFivZero[3]);
	printf("Sector.SFivOne[3] = 0x%02X\n", Sector.SFivOne[3]);


#if DBG_RC500
	printf("ReadCardInfor Out : %d \n",StepFlag);
#endif
	return StepFlag;
}



/************************************************************************
�õ�M1��Ƭ��Ϣ
************************************************************************/
/*
unsigned char ReadCardInfor(void)
{
int flag,t;
int ret;
static int  receive_len[1] ={0};
static char receive_buf[20]={0};
unsigned char Keybuf[8];

	test.mode = KEYA;
	memset(test.key,0xFF,6);
	memset(test.rwbuf,0xff,16);
	memset(test.money,0x00,4);
        memset(&CardLan,0,sizeof(CardInform));
	flag = 1;
	t = 2;
	while(flag)
	{
		switch(t)
		{
		case 2:
			ret = ioctl(mf_fd, RC531_M1_CSN);
                        if(ret == MI_OK)t++;
                        else flag = 0;
			break;
		case 3:
			ret = ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
			if(ret == MI_OK)t++;
                        else flag = 0;
			break;
		case 4:
			ret = read(mf_fd, receive_buf, receive_len[0]);
			if(ret == MI_OK)t++;
                        else flag = 0;
			break;

		case 5:
                        memcpy(Keybuf,receive_buf,4);
                        memcpy(CardLan.UserIcNo,receive_buf,4);
			memcpy(Keybuf+4,SnBack,4);
                        DES_CARD(KeyDes,Keybuf,test.key);
			t++;
			break;

		case 6:
			ioctl(mf_fd, WRITE_TYPE, W_CHAR);
                        ret = write(mf_fd, &test, sizeof(struct card_buf));
			if(ret == MI_OK)t++;
                        else flag = 0;
			break;
		case 7:
			ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.One + 3));//ֻ����Ϣ����
                        if(ret == MI_OK)t++;
			else flag = 0;
			break;
		case 8:
                        ret = ioctl(mf_fd, RC531_READ,(4*LanSec.One+ 1));
                        if(ret == MI_OK)
			{
				ret = ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				ret = read(mf_fd, receive_buf, receive_len[0]);
				if(ret >= 0)
				{
			         memcpy(CardLan.CityId,receive_buf,2);	  //���д���  HEX
				 CardLan.CardType =  receive_buf[12]; 		  //������Ϣ       HEX
				 memcpy(CardLan.CardCsnB, receive_buf+4 , 4); //�û����� BCD
                                 memcpy(CardLan.CardId, receive_buf+8 , 4); //����֤��
				 t++;
				}
				else
				{
					flag = 0;
				}
			}
                        else flag = 0;
                        break;
                case 9:
                        ret = ioctl(mf_fd, RC531_READ,(4*LanSec.One + 2));
                        if(ret == MI_OK)
			{
				ret = ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				ret = read(mf_fd, receive_buf, receive_len[0]);
				if(ret >= 0)
				{
					memcpy(CardLan.EnableH,  receive_buf + 4,4);//�������� HEX
					memcpy(CardLan.Effective,receive_buf + 8,4);//��Ч���� HEX
					memcpy(CardLan.UserWord,receive_buf + 12,3);//��Ч���� HEX
					t++;
				}
				else
				{
					flag = 0;
				}
			}
                        else flag = 0;
                        break;

		case 10:
			ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.Two + 3));	 //����Ǯ������
                        if(ret == MI_OK)t++;
			else flag = 0;
			break;
		case 11:
			ret = ioctl(mf_fd, RC531_READ,(4*LanSec.Two + 1));
			if(ret == MI_OK)t++;
			else flag = 0;
			break;
		case 12:
			ret = ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
			ret = read(mf_fd, receive_buf, receive_len[0]);
			if(ret >= 0)
			{
				 ret = MoneyYes(receive_buf);
				 if(ret == MI_OK)t++;
				 else flag = 0;

			}
			else flag = 0;
			break;
		case 13:
			memcpy(CardLan.Views,receive_buf,4);//����Ǯ�� HEX
			ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.Thr + 3));//����Ǯ������
                        if(ret == MI_OK)t++;
			else flag = 0;
			break;
		case 14:
			ret = ioctl(mf_fd, RC531_READ,(4*LanSec.Thr + 1));
			if(ret == MI_OK)t++;
			else flag = 0;
			break;

		case 15:
			ret = ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
			ret = read(mf_fd, receive_buf, receive_len[0]);
			if(ret >= 0)
			{
				 ret = MoneyYes(receive_buf);
				 if(ret == MI_OK)t++;
				 else flag = 0;

			}
			else flag = 0;
			break;
		case 16:
			memcpy(CardLan.Subsidies,receive_buf,4);//����Ǯ�� HEX
			ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.For + 3));	 //�ֽ�Ǯ������
                        if(ret == MI_OK)t++;
			else flag = 0;
			break;
		case 17:
			ret = ioctl(mf_fd, RC531_READ,(4*LanSec.For + 1));
			if(ret == MI_OK)t++;
			else flag = 0;
			break;

		case 18:
			ret = ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
			ret = read(mf_fd, receive_buf, receive_len[0]);
			if(ret >= 0)
			{
				 ret = MoneyYes(receive_buf);
				 if(ret == MI_OK)t++;
				 else flag = 0;

			}
			else flag = 0;
			break;

		case 19:
			memcpy(CardLan.QCash,receive_buf,4);//�ֽ�Ǯ�� HEX
			ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.Fiv + 3));	 //���޸�����
                        if(ret == MI_OK)t++;
			else flag = 0;
			break;

		case 20:
			ret = ioctl(mf_fd, RC531_READ,(4*LanSec.Fiv + 1));
                        if(ret == MI_OK)
			{
				ret = ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				ret = read(mf_fd, receive_buf, receive_len[0]);
				if(ret >= 0)
				{
				memcpy(CardLan.OldTime, receive_buf, 6);         //�ϴν���ʱ��   HEX
				CardLan.Period       = receive_buf[6];           //�ϴν��׵�ʱ�� HEX
				CardLan.ViewsSection = receive_buf[7];           //�������Ѵ��� HEX
				CardLan.DaySection   = receive_buf[8];  	 //�������Ѵ��� HEX
				memcpy(CardLan.MonthSection, receive_buf + 9, 2);//�������Ѵ��� HEX
                                CardLan.EnterExitFlag = receive_buf[11];
				memcpy(CardLan.MoneyJack, receive_buf + 12,4);   //��������
				memcpy(Sector.SFivOne,receive_buf,16);
				t++;
				}
				else
				{
					flag = 0;
				}
			}
                        else flag = 0;
			break;
		case 21:
			ret = ioctl(mf_fd, RC531_READ,(4*LanSec.Fiv + 2));
                        if(ret == MI_OK)
			{
				ret = ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				ret = read(mf_fd, receive_buf, receive_len[0]);
				if(ret >= 0)
				{
				memcpy(CardLan.ViewsValue, receive_buf    , 4);//�������ѽ�� HEX
				memcpy(CardLan.DayValue,   receive_buf + 4, 4);//�������ѽ�� HEX
				memcpy(CardLan.MonthValue, receive_buf + 8, 4);//�������ѽ�� HEX
				receive_buf[12]++;
				if(receive_buf[12] >= 3)receive_buf[12] = 0;
			        CardLan.WriteRecord = receive_buf[12];
				memcpy(CardLan.ViewMoney, receive_buf + 13,3);//�ۼƽ��״��� HEX
				memcpy(Sector.SFivTwo,receive_buf,16);
				t++;
				}
				else
				{
					flag = 0;
				}
			}
                        else flag = 0;
			break;

		default:
			flag = 0;
			break;
		}
	}

	if(t != 22)return t;
	return 0;
}

*/


/*
*************************************************************************************************************
- �������� : unsigned char CheckS5B0Light (unsigned char Pstatus)
- ����˵�� : S5B0��ȷ��
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char CheckS5B0Light(void)
{
	unsigned char i,XOR;
	unsigned char status = 1;

	XOR = Sector.SFivZero[0];
	for(i = 1; i <15; i++)
	{
		XOR ^= Sector.SFivZero[i];
	}
	if(XOR == Sector.SFivZero[15])							//У��λ�ɹ�
	{
		if((Sector.SFivZero[0] >= 1)&&(Sector.SFivZero[0] <=3))
		{
			if(Sector.SFivZero[4] == ProEnd)
			{
				status = 0;
			}
		}
	}

	return(status);
}
/*
*************************************************************************************************************
- �������� : unsigned char CheckS5B1Light (unsigned char Pstatus)
- ����˵�� : S5B1��ȷ��
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char CheckS5B1Light(void)
{
	unsigned char i,XOR;
	unsigned char status = 1;

	XOR = Sector.SFivOne[0];
	for(i = 1; i <15; i++)
	{
		XOR ^= Sector.SFivOne[i];
	}
	if(XOR == Sector.SFivOne[15])							//У��λ�ɹ�
	{
		if((Sector.SFivOne[0] >= 1)&&(Sector.SFivOne[0] <=3))
		{
			if(Sector.SFivOne[4] == ProEnd)	 //12.3�޸� һ����Ҫ��ɲ�������ȷ
			{
				status = 0;
			}
		}
	}
	return(status);
}
/*
*************************************************************************************************************
- �������� : unsigned char CheckS5B0ORS5B1 (void)
- ����˵�� : S5B1 == S5B0
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char CheckS5B0ORS5B1(void)
{
	unsigned char i;
	unsigned char status = 1;
	for(i = 0; i < 16; i++)
	{
		if(Sector.SFivOne[i] != Sector.SFivZero[i]) break;
	}
	if(i == 16) status = 0;
	return(status);
}

/*
*************************************************************************************************************
- �������� : unsigned char ResumeTypeThirteen(void)
- ����˵�� : SxB2 -> SxB1
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char ResumeTypeThirteen(void)
{
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char SectorNum;
	int ret;

	CardLan.OldTransType = Sector.SFivZero[14] & 0xf0;
	if(CardLan.OldTransType == 0x10)
	{
		SectorNum = 4*LanSec.Two;
	}
	else if(CardLan.OldTransType == 0x20)
	{
		SectorNum = 4*LanSec.Thr;
	}
	else if(CardLan.OldTransType == 0x30)
	{
		SectorNum = 4*LanSec.For;
	}
	else
	{
		return 1;
	}

	while(Loop)
	{
		switch(step)
		{
		case 1:
			ret = ioctl(mf_fd, RC531_AUTHENT,(SectorNum+ 3));
			if(ret == MI_OK)step++;
			else Loop = 0;
			break;

		case 2:
			ret = ioctl(mf_fd,RC531_RESTORE,(SectorNum + 2));
			if(ret == MI_OK)step++;
			else Loop = 0;
			break;

		case 3:
			ret = ioctl(mf_fd,RC531_TRANSFER,(SectorNum + 1));
			if(ret== MI_OK)step++;
			else Loop = 0;
			break;

		default:
			Loop = 0;
			step = 0;
			break;
		}
	}

#if DBG_RC500
	printf("Resume 13 = %d\n",step);
#endif
	return(step);
}


/*�Ѹ���
*************************************************************************************************************
- �������� : unsigned char ResumeTypeSev (void)
- ����˵�� : �޸���Ƭ-��SxB1�����ݸ��Ƶ�SxB2��
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/

unsigned char ResumeTypeSev (void)
{
	unsigned char Loop = 1;
	unsigned char step = 1;
	static  char receive_buf[20]= {0};
	int ret;

	while(Loop)
	{
		switch(step)
		{
		case 1:
			ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.For + 3));
			if(ret == MI_OK)step++;
			else Loop = 0;
			break;
		case 2:
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			ret = ioctl(mf_fd, RC531_WRITE,4*LanSec.For);
			if(ret == MI_OK)
			{
				memcpy(Sector.SForZero,Sector.STwoZero,16);
				memcpy(receive_buf,Sector.SForZero,16);
				ret = write(mf_fd, receive_buf, 16);
				if(ret == MI_OK)step++;
				else Loop = 0;
			}
			else Loop = 0;
			break;
		default:
			Loop = 0;
			step = 0;
			break;
		}
	}

#if DBG_RC500
	printf("Resume12 = %d\n",step);
#endif
	return(step);
}

/*
*************************************************************************************************************
- �������� : unsigned char ResumeTypeSix (void)
- ����˵�� : �޸���Ƭ-��SxB1�����ݸ��Ƶ�SxB2��
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char ResumeTypeSix (void)
{
	LongUnon TempMoney;
	
	memcpy(TempMoney.longbuf,Sector.SForOne,4);
	DebugPrintf("TempMoney.i = %u\n", TempMoney.i);
	memcpy(TempMoney.longbuf,Sector.SForTwo,4);
	DebugPrintf("TempMoney.i = %u\n", TempMoney.i);
	DebugPrintf("CardLan.OldTransType = 0x%02X\n", CardLan.OldTransType);
	DebugPrintf("HostValue.i = %u\n", HostValue.i);


	static char receive_buf[20]= {0};
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char SectorNum;
	unsigned char rbuf[16];
	int ret;

	//LongUnon TempMoney;
//   DBG_RC500_PRINTF("ResumeTypeSix() is called.\n");

	
	CardLan.OldTransType = Sector.SFivZero[14] & 0xf0;
	
	if(CardLan.OldTransType == 0x10)
	{
		SectorNum = 4*LanSec.Two;
		memcpy(Sector.STwoTwo,Sector.STwoOne,16);
		memcpy(rbuf,Sector.STwoOne,16);
		//memcpy(Sector.STwoOne,Sector.STwoTwo,16);
		//memcpy(rbuf,Sector.STwoTwo,16);
	}
	else if(CardLan.OldTransType == 0x20)
	{
		SectorNum = 4*LanSec.Thr;
		memcpy(Sector.SThrTwo,Sector.SThrOne,16);
		//memcpy(Sector.SThrOne,Sector.SThrTwo,16);
		memcpy(rbuf,Sector.SThrOne,16);
		//memcpy(rbuf,Sector.SThrTwo,16);

	}
	else
	{
		SectorNum = 4*LanSec.For;
		memcpy(Sector.SForTwo,Sector.SForOne,16);
		//memcpy(Sector.SForOne,Sector.SForTwo,16);
			
		memcpy(rbuf,Sector.SForOne,16);
		//memcpy(rbuf,Sector.SForTwo,16);
	}

	while(Loop)
	{
		switch(step)
		{
		case 1:
			ret = ioctl(mf_fd, RC531_AUTHENT,(SectorNum+ 3));
			if(ret == MI_OK)step++;
			else Loop = 0;
			break;

		case 2:
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			ret = ioctl(mf_fd, RC531_WRITE,(SectorNum+2));
			//ret = ioctl(mf_fd, RC531_WRITE,(SectorNum+1));
			if(ret == MI_OK)
			{
				memcpy(receive_buf,rbuf,16);
				ret = write(mf_fd, receive_buf, 16);
				if(ret == MI_OK)step++;
				else Loop = 0;
			}
			else Loop = 0;
			break;

			////�޴��޶�ģ�ֻȡS2B0 ����S4B0
		case 3:
			ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.For + 3));
			if(ret == MI_OK)step++;
			else Loop = 0;
			break;

		case 4:
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			ret = ioctl(mf_fd, RC531_WRITE,4*LanSec.For);
			if(ret == MI_OK)
			{
				memcpy(Sector.SForZero,Sector.STwoZero,16);
				memcpy(receive_buf,Sector.STwoZero,16);
				ret = write(mf_fd, receive_buf, 16);
				if(ret == MI_OK)step++;
				else Loop = 0;
			}
			else Loop = 0;
			break;

		default:
			Loop = 0;
			step = 0;
			break;
		}
	}

#if DBG_RC500
	printf("Resume 10 11 = %d\n",step);
#endif
	return(step);
}
/*
*************************************************************************************************************
- �������� : unsigned char ResumeTypeFive (void)
- ����˵�� : �޸���Ƭ-��S5B0�����ݸ��Ƶ�S5B1�У���SxB1�����ݸ��Ƶ�SxB2��
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char ResumeTypeFive (void)
{
	static char receive_buf[20]= {0};
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char SectorNum;
	unsigned char rbuf[16];
	int ret;

	DBG_RC500_PRINTF("ResumeTypeFive() is called.\n");

	CardLan.OldTransType = Sector.SFivZero[14] & 0xf0;
	if(CardLan.OldTransType == 0x10)
	{
		SectorNum = 4*LanSec.Two;
		memcpy(Sector.STwoTwo,Sector.STwoOne,16);
		memcpy(rbuf,Sector.STwoOne,16);
	}
	else if(CardLan.OldTransType == 0x20)
	{
		SectorNum = 4*LanSec.Thr;
		memcpy(Sector.SThrTwo,Sector.SThrOne,16);
		memcpy(rbuf,Sector.SThrOne,16);
	}
	else
	{
		SectorNum = 4*LanSec.For;
		memcpy(Sector.SForTwo,Sector.SForOne,16);
		memcpy(rbuf,Sector.SForOne,16);
	}

	while(Loop)
	{
		switch(step)
		{
		case 1:
			ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.Fiv + 3));
			if(ret == MI_OK)step++;
			else Loop = 0;
			break;

		case 2:
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			ret = ioctl(mf_fd, RC531_WRITE,(4*LanSec.Fiv+1));
			if(ret == MI_OK)
			{
				memcpy(Sector.SFivOne,Sector.SFivZero,16);
				memcpy(receive_buf,Sector.SFivZero,16);
				ret = write(mf_fd, receive_buf, 16);
				if(ret == MI_OK)step++;
				else Loop = 0;
			}
			else Loop = 0;
			break;

		case 3:
			ret = ioctl(mf_fd, RC531_AUTHENT,SectorNum+3);
			if(ret == MI_OK)step++;
			else Loop = 0;
			break;

		case 4:
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			ret = ioctl(mf_fd, RC531_WRITE,(SectorNum+2));
			if(ret == MI_OK)
			{
				memcpy(receive_buf,rbuf,16);
				ret = write(mf_fd,receive_buf,16);
				if(ret == MI_OK)step++;
				else Loop = 0;
			}
			else Loop = 0;
			break;

			/* //�޴��޶�ģ�ֻȡS2B0 ����S4B0
				   case 5:
					        ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.For + 3));
			                if(ret == MI_OK)step++;
			                else Loop = 0;
					        break;

			       case 6:
			                ioctl(mf_fd, WRITE_TYPE,W_CARD);
			                ret = ioctl(mf_fd, RC531_WRITE,4*LanSec.For);
			                if(ret == MI_OK)
					          {
			                        memcpy(Sector.SForZero,Sector.STwoZero,16);
			                        memcpy(receive_buf,Sector.SForZero,16);
						            ret = write(mf_fd, receive_buf, 16);
			                        if(ret == MI_OK)step++;
			                        else Loop = 0;
					          }
			                else Loop = 0;
			                break;
			*/
		default:
			Loop = 0;
			step = 0;
			break;
		}
	}

#if DBG_RC500
	printf("Resume89 = %d\n",step);
#endif
	return(step);
}
/*�Ѹ���
*************************************************************************************************************
- �������� : unsigned char ResumeTypeFour (void)
- ����˵�� : �޸���Ƭ-��S5B1�еĽ��״�����ָ���1�����ױ�־��Ϊ������ɣ�
             ��дS5B0����S5B0�����ݸ��Ƶ�S5B1�У���SxB1�����ݸ��Ƶ�SxB2
             S2B0 --> S4B0
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char ResumeTypeFour (void)
{
	DebugPrintf("\n");
	static char receive_buf[20]= {0};
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char Xor,i;
	unsigned char SectorNum;
	unsigned char rbuf[16];
	int ret;

	DBG_RC500_PRINTF("ResumeTypeFour() is called.\n");

	CardLan.OldTransType = Sector.SFivOne[14] & 0xf0;
#if 0	
	if(CardLan.OldTransType == 0x10)
	{
		SectorNum = 4*LanSec.Two;
		memcpy(Sector.STwoTwo,Sector.STwoOne,16);
		memcpy(rbuf,Sector.STwoOne,16);
	}
	else if(CardLan.OldTransType == 0x20)
	{
		SectorNum = 4*LanSec.Thr;
		memcpy(Sector.SThrTwo,Sector.SThrOne,16);
		memcpy(rbuf,Sector.SThrOne,16);
	}
	else
	{
		SectorNum = 4*LanSec.For;
		memcpy(Sector.SForTwo,Sector.SForOne,16);
		memcpy(rbuf,Sector.SForOne,16);
	}
#endif
	if(CardLan.OldTransType == 0x10)
	{
		SectorNum = 4*LanSec.Two;
		//memcpy(Sector.STwoTwo,Sector.STwoOne,16);
		//memcpy(rbuf,Sector.STwoOne,16);
		memcpy(Sector.STwoOne,Sector.STwoTwo,16);
		memcpy(rbuf,Sector.STwoTwo,16);
	}
	else if(CardLan.OldTransType == 0x20)
	{
		SectorNum = 4*LanSec.Thr;
		//memcpy(Sector.SThrTwo,Sector.SThrOne,16);
		memcpy(Sector.SThrOne,Sector.SThrTwo,16);
		//memcpy(rbuf,Sector.SThrOne,16);
		memcpy(rbuf,Sector.SThrTwo,16);

	}
	else
	{
		SectorNum = 4*LanSec.For;
		//memcpy(Sector.SForTwo,Sector.SForOne,16);
		memcpy(Sector.SForOne,Sector.SForTwo,16);
			
		//memcpy(rbuf,Sector.SForOne,16);
		memcpy(rbuf,Sector.SForTwo,16);
	}
	
	while(Loop)
	{
		switch(step)
		{
		case 1:
			ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.Fiv + 3));
			if(ret == MI_OK)step++;
			else Loop = 0;
			break;

		case 2:
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			ret = ioctl(mf_fd, RC531_WRITE,(4*LanSec.Fiv));
			if(ret == MI_OK)
			{
				memcpy(Sector.SFivOne,Sector.SFivZero,16);
				Sector.SFivOne[4] = ProEnd;
				Xor = Sector.SFivOne[0];
				for(i = 1; i < 15 ; i ++)
				{
					Xor ^= Sector.SFivOne[i];
				}
				Sector.SFivOne[15] = Xor;

				memcpy(Sector.SFivZero,Sector.SFivOne,16);
				memcpy(receive_buf,Sector.SFivOne,16);
				ret = write(mf_fd, receive_buf, 16);
				if(ret == MI_OK)step++;
				else Loop = 0;
			}
			else Loop = 0;
			break;

		case 3:
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			ret = ioctl(mf_fd, RC531_WRITE,(4*LanSec.Fiv+1));
			if(ret == MI_OK)
			{
				memcpy(receive_buf,Sector.SFivZero,16);
				ret = write(mf_fd, receive_buf, 16);
				if(ret == MI_OK)step++;
				else Loop = 0;
			}
			else Loop = 0;
			break;

		case 4:
			ret = ioctl(mf_fd, RC531_AUTHENT,SectorNum+3);
			if(ret == MI_OK)step++;
			else Loop = 0;
			break;

		case 5:
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			//ret = ioctl(mf_fd, RC531_WRITE,SectorNum + 2);
			ret = ioctl(mf_fd, RC531_WRITE,SectorNum + 1);
			if(ret == MI_OK)
			{
				memcpy(receive_buf,rbuf,16);
				ret = write(mf_fd, receive_buf, 16);
				if(ret == MI_OK)step++;
				else Loop = 0;
			}
			else Loop = 0;
			break;

			/* ////�޴��޶�ģ�ֻȡS2B0 ����S4B0
				  case 6:
					        ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.For + 3));
			                if(ret == MI_OK)step++;
			                else Loop = 0;
					         break;

			      case 7:
			                ioctl(mf_fd, WRITE_TYPE,W_CARD);
			                ret = ioctl(mf_fd, RC531_WRITE,4*LanSec.For);
			                if(ret == MI_OK)
					          {
			                        memcpy(Sector.SForZero,Sector.STwoZero,16);
			                        memcpy(receive_buf,Sector.STwoZero,16);
						            ret = write(mf_fd, receive_buf, 16);
			                        if(ret == MI_OK)step++;
			                        else Loop = 0;
					          }
			                else Loop = 0;
			                break;
			*/
		default:
			Loop = 0;
			step = 0;
			break;
		}
	}

#if DBG_RC500
	printf("Resume7 = %d\n",step);
#endif
	return(step);
}

/*
*************************************************************************************************************
- �������� : unsigned char ResumeTypeThree (void)
- ����˵�� : �޸���Ƭ-��S5B0�еĽ��ױ�־Ϊ������ɣ���S5B0�����ݸ��Ƶ�S5B1�У���SxB1�����ݸ��Ƶ�SxB2
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
/*
unsigned char ResumeTypeThree (void)
{
   unsigned char Loop = 1;
   unsigned char step = 1;
   unsigned char ret,Xor,i;
   static char receive_buf[20]={0};
   unsigned char SectorNum;
   CardLan.OldTransType = Sector.SFivOne[14] & 0xf0;
   if(CardLan.OldTransType == 0x10)
   	SectorNum = 4*LanSec.Two;
   else if(CardLan.OldTransType == 0x20)
   	SectorNum = 4*LanSec.Thr;
   else
        SectorNum = 4*LanSec.For;
   while(Loop)
   {
      switch(step)
      {

	   case 1:
		ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.Fiv + 3));
                if(ret == MI_OK)step++;
                else Loop = 0;
		break;
           case 2:
                ioctl(mf_fd, WRITE_TYPE,W_CARD);
                ret = ioctl(mf_fd, RC531_WRITE,(4*LanSec.Fiv));
                if(ret == MI_OK)
		{
			Sector.SFivZero[4] = ProEnd;
			Xor = Sector.SFivZero[0];
			for(i = 1; i < 15 ;i ++)
                        {
                             Xor ^= Sector.SFivZero[i];
                        }
                        Sector.SFivZero[15] = Xor;
      			memcpy(Sector.SFivOne,Sector.SFivZero,16);
                        memcpy(receive_buf,Sector.SFivZero,16);
			ret = write(mf_fd, receive_buf, 16);
                        if(ret == MI_OK)step++;
                        else Loop = 0;
		}
                else Loop = 0;
		//ret = ioctl(mf_fd, RC531_READ,(4*LanSec.Fiv));
                break;
           case 3:
                ioctl(mf_fd, WRITE_TYPE,W_CARD);
                ret = ioctl(mf_fd, RC531_WRITE,(4*LanSec.Fiv+1));
                if(ret == MI_OK)
		{
                        memcpy(receive_buf,Sector.SFivZero,16);
			ret = write(mf_fd, receive_buf, 16);
                        if(ret == MI_OK)step++;
                        else Loop = 0;
		}
                else Loop = 0;
		//ret = ioctl(mf_fd, RC531_READ,(4*LanSec.Fiv+1));
                break;
	   case 4:
		ret = ioctl(mf_fd, RC531_AUTHENT,(SectorNum + 3));
                if(ret == MI_OK)step++;
                else Loop = 0;
		break;
           case 5:
                ioctl(mf_fd, WRITE_TYPE,W_CARD);
                ret = ioctl(mf_fd, RC531_WRITE,(SectorNum+2));
                if(ret == MI_OK)
		{
      			memcpy(XFBuf2,XFBuf1,16);
                        memcpy(receive_buf,XFBuf1,16);
			ret = write(mf_fd, receive_buf, 16);
                        if(ret == MI_OK)step++;
                        else Loop = 0;
		}
                else Loop = 0;
		// ret = ioctl(mf_fd, RC531_READ,(SectorNum+2));
                break;
           case 6:
                CardLan.OldTransType = Sector.SFivOne[14] & 0xf0;
   		if(CardLan.OldTransType == 0x10)
                {
   			memcpy(Sector.STwoOne,XFBuf1,16);
   			memcpy(Sector.STwoTwo,XFBuf1,16);
                }
   		else if(CardLan.OldTransType == 0x20)
                {
   			memcpy(Sector.SThrOne,XFBuf1,16);
   			memcpy(Sector.SThrTwo,XFBuf1,16);
                }
   		else
                {
   			memcpy(Sector.SForOne,XFBuf1,16);
   			memcpy(Sector.SForTwo,XFBuf1,16);
                }
                step++;
                break;
	   case 7:
		ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.For + 3));
                if(ret == MI_OK)step++;
                else Loop = 0;
		break;
           case 8:
                ioctl(mf_fd, WRITE_TYPE,W_CARD);
                ret = ioctl(mf_fd, RC531_WRITE,4*LanSec.For);
                if(ret == MI_OK)
		{
                        memcpy(Sector.SForZero,Sector.STwoZero,16);
                        memcpy(receive_buf,Sector.SForZero,16);
			ret = write(mf_fd, receive_buf, 16);
                        if(ret == MI_OK)step++;
                        else Loop = 0;
		}
                else Loop = 0;
		//ret = ioctl(mf_fd, RC531_READ,4*LanSec.For);
                break;
           default:
                Loop = 0;
                step = 0;
                break;
      }
   }
#if DBG_RC500
   printf("Resume6 = %d\n",step);
#endif
   return(step);
}
*/

/*
*************************************************************************************************************
- �������� : unsigned char ResumeTypeFourNo (void)
- ����˵�� : �޸���Ƭ
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
/*
unsigned char ResumeTypeFourNo (void)
{
   unsigned char Loop = 1;
   unsigned char step = 1;
   unsigned char ret;
   static char receive_buf[20]={0};
   unsigned char SectorNum;
   CardLan.OldTransType = Sector.SFivOne[14] & 0xf0;
   if(CardLan.OldTransType == 0x10)
   	SectorNum = 4*LanSec.Two;
   else if(CardLan.OldTransType == 0x20)
   	SectorNum = 4*LanSec.Thr;
   else
        SectorNum = 4*LanSec.For;
   while(Loop)
   {

      switch(step)
      {
	  case 1:
		ret = ioctl(mf_fd, RC531_AUTHENT,(SectorNum + 3));
                if(ret == MI_OK)step++;
                else Loop = 0;
		break;
           case 2:
                ioctl(mf_fd, WRITE_TYPE,W_CARD);
                ret = ioctl(mf_fd, RC531_WRITE,(SectorNum+1));
                if(ret == MI_OK)
		{
      			memcpy(XFBuf1,XFBuf2,16);
                        memcpy(receive_buf,XFBuf1,16);
			ret = write(mf_fd, receive_buf, 16);
                        if(ret == MI_OK)step++;
                        else Loop = 0;
		}
                else Loop = 0;
		//ret = ioctl(mf_fd, RC531_READ,(SectorNum+1));
                break;
	   case 3:
		ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.Fiv + 3));
                if(ret == MI_OK)step++;
                else Loop = 0;
		break;
           case 4:
                ioctl(mf_fd, WRITE_TYPE,W_CARD);
                ret = ioctl(mf_fd, RC531_WRITE,(4*LanSec.Fiv));
                if(ret == MI_OK)
		{
      			memcpy(Sector.SFivZero,Sector.SFivOne,16);
                        memcpy(receive_buf,Sector.SFivZero,16);
			ret = write(mf_fd, receive_buf, 16);
                        if(ret == MI_OK)step++;
                        else Loop = 0;
		}
                else Loop = 0;
		//ret = ioctl(mf_fd, RC531_READ,(4*LanSec.Fiv));
                break;
           case 5:
                CardLan.OldTransType = Sector.SFivOne[14] & 0xf0;
   		if(CardLan.OldTransType == 0x10)
                {
   			memcpy(Sector.STwoOne,XFBuf1,16);
   			memcpy(Sector.STwoTwo,XFBuf1,16);
                }
   		else if(CardLan.OldTransType == 0x20)
                {
   			memcpy(Sector.SThrOne,XFBuf1,16);
   			memcpy(Sector.SThrTwo,XFBuf1,16);
                }
   		else
                {
   			memcpy(Sector.SForOne,XFBuf1,16);
   			memcpy(Sector.SForTwo,XFBuf1,16);
                }
                step++;
                break;
	   case 6:
		ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.Two + 3));
                if(ret == MI_OK)step++;
                else Loop = 0;
		break;
           case 7:
                ioctl(mf_fd, WRITE_TYPE,W_CARD);
                ret = ioctl(mf_fd, RC531_WRITE,4*LanSec.Two);
                if(ret == MI_OK)
		{
                        memcpy(Sector.STwoZero,Sector.SForZero,16);
                        memcpy(receive_buf,Sector.STwoZero,16);
			ret = write(mf_fd, receive_buf, 16);
                        if(ret == MI_OK)step++;
                        else Loop = 0;
		}
                else Loop = 0;
		//ret = ioctl(mf_fd, RC531_READ,4*LanSec.Two);
                break;
           default:
                Loop = 0;
                step = 0;
                break;
      }
   }
#if DBG_RC500
   printf("Resume5 = %d\n",step);
#endif
   return(step);
}
*/

/*
*************************************************************************************************************
- �������� : unsigned char ResumeTypeTwo (void)
- ����˵�� :  // S4B0-->S2B0 , SxB2--> SxB1 ��S5B1-->S5B0
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char ResumeTypeTwo (void)
{
	static char receive_buf[20]= {0};
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char SectorNum;
	unsigned char rbuf[16];
	int ret;

	DBG_RC500_PRINTF("ResumeTypeTwo() is called.\n");

	CardLan.OldTransType = Sector.SFivOne[14] & 0xf0;
	if(CardLan.OldTransType == 0x10)
	{
		SectorNum = 4*LanSec.Two;
		memcpy(Sector.STwoOne,Sector.STwoTwo,16);
		memcpy(rbuf,Sector.STwoTwo,16);
	}
	else if(CardLan.OldTransType == 0x20)
	{
		SectorNum = 4*LanSec.Thr;
		memcpy(Sector.SThrOne,Sector.SThrTwo,16);
		memcpy(rbuf,Sector.SThrTwo,16);
	}
	else
	{
		SectorNum = 4*LanSec.For;
		memcpy(Sector.SForOne,Sector.SForTwo,16);
		memcpy(rbuf,Sector.SForTwo,16);
	}

	while(Loop)
	{
		switch(step)
		{
		case 1:
			ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.Two + 3));
			if(ret == MI_OK)step++;
			else Loop = 0;
			break;

		case 2: //S4B0 -->S2B0
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			ret = ioctl(mf_fd, RC531_WRITE,4*LanSec.Two);
			if(ret == MI_OK)
			{
				memcpy(Sector.STwoZero,Sector.SForZero,16);
				memcpy(receive_buf,Sector.STwoZero,16);
				ret = write(mf_fd, receive_buf, 16);
				if(ret == MI_OK)step++;
				else Loop = 0;
			}
			else Loop = 0;
			break;

		case 3:
			ret = ioctl(mf_fd, RC531_AUTHENT,(SectorNum + 3));
			if(ret == MI_OK)step++;
			else Loop = 0;
			break;

		case 4:  //SXB2 -->SXB1
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			ret = ioctl(mf_fd, RC531_WRITE,(SectorNum+1));
			if(ret == MI_OK)
			{
				memcpy(receive_buf,rbuf,16);
				ret = write(mf_fd, receive_buf,16);
				if(ret == MI_OK)step++;
				else Loop = 0;
			}
			else Loop = 0;
			break;

		case 5:
			ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.Fiv + 3));
			if(ret == MI_OK)step++;
			else Loop = 0;
			break;

		case 6:  //S5B1 -->S5B0
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			ret = ioctl(mf_fd, RC531_WRITE,(4*LanSec.Fiv));
			if(ret == MI_OK)
			{
				memcpy(Sector.SFivZero,Sector.SFivOne,16);
				memcpy(receive_buf,Sector.SFivOne,16);
				ret = write(mf_fd, receive_buf, 16);
				if(ret == MI_OK)step++;
				else Loop = 0;
			}
			else Loop = 0;
			break;

		default:
			Loop = 0;
			step = 0;
			break;
		}
	}

#if DBG_RC500
	printf("Resume 5 = %d\n",step);
#endif

	return(step);
}
/*OK
*************************************************************************************************************
- �������� : unsigned char ResumeTypeOne (void)
- ����˵�� : �޸���Ƭ-��S5B1�����ݸ��Ƶ�S5B0��
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char ResumeTypeOne (void)
{
	static char receive_buf[20]= {0};
	unsigned char Loop = 1;
	unsigned char step = 1;
	int ret;

	DBG_RC500_PRINTF("ResumeTypeOne() is called.\n");

	while(Loop)
	{
		switch(step)
		{
		case 1:
			ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.Two+ 3));
			if(ret == MI_OK)step++;
			else Loop = 0;
			break;

		case 2:
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			ret = ioctl(mf_fd, RC531_WRITE,4*LanSec.Two);
			if(ret == MI_OK)
			{
				memcpy(Sector.STwoZero,Sector.SForZero,16);
				memcpy(receive_buf,Sector.SForZero,16);
				ret = write(mf_fd, receive_buf, 16);
				if(ret == MI_OK)step++;
				else Loop = 0;
			}
			else Loop = 0;
			break;

		case 3:
			ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.Fiv + 3));
			if(ret == MI_OK)step++;
			else Loop = 0;
			break;

		case 4:
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			ret = ioctl(mf_fd, RC531_WRITE,4*LanSec.Fiv);
			if(ret == MI_OK)
			{
				memcpy(Sector.SFivZero,Sector.SFivOne,16);
				memcpy(receive_buf,Sector.SFivOne,16);
				ret = write(mf_fd, receive_buf, 16);
				if(ret == MI_OK)step++;
				else Loop = 0;
			}
			else Loop = 0;
			break;

		default:
			Loop = 0;
			step = 0;
			break;
		}
	}
#if DBG_RC500
	printf("Resume 3 = %d\n",step);
#endif
	return(step);
}


/*
*************************************************************************************************************
- �������� : unsigned char ResumeTypeOneOther (void)
- ����˵�� : �޸���Ƭ-��S5B1�����ݸ��Ƶ�S5B0��
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char ResumeTypeOneOther (void)
{
	static char receive_buf[20]= {0};
	unsigned char Loop = 1;
	unsigned char step = 1;
	int ret;

	DBG_RC500_PRINTF("ResumeTypeOneOther() is called.\n");

	while(Loop)
	{
		switch(step)
		{
			////�޴��޶�ģ�ֻȡS2B0 ����S4B0
		case 1:
			ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.For + 3));
			if(ret == MI_OK)step++;
			else Loop = 0;
			break;
		case 2:
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			ret = ioctl(mf_fd, RC531_WRITE,4*LanSec.For);
			if(ret == MI_OK)
			{
				memcpy(Sector.SForZero,Sector.STwoZero,16);
				memcpy(receive_buf,Sector.STwoZero,16);
				ret = write(mf_fd, receive_buf, 16);
				if(ret == MI_OK)step++;
				else Loop = 0;
			}
			else Loop = 0;
			break;


		case 3:
			ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.Fiv + 3));
			if(ret == MI_OK)step++;
			else Loop = 0;
			break;

		case 4:
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			ret = ioctl(mf_fd, RC531_WRITE,(4*LanSec.Fiv + 1));
			if(ret == MI_OK)
			{
				memcpy(Sector.SFivOne,Sector.SFivZero,16);
				memcpy(receive_buf,Sector.SFivOne,16);
				ret = write(mf_fd, receive_buf, 16);
				if(ret == MI_OK)step++;
				else Loop = 0;
			}
			else Loop = 0;
			break;

		default:
			Loop = 0;
			step = 0;
			break;
		}
	}

#if DBG_RC500
	printf("Resume34 = %d\n",step);
#endif
	return(step);
}

/*
*************************************************************************************************************
- �������� : unsigned char ResumeType (unsigned char Mode)
- ����˵�� : �޸���Ƭ��Ϣ
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char ResumeType (unsigned char Mode)
{
	DebugPrintf("Mode = 0x%02X\n", Mode);
	unsigned char status = 1;

	DBG_RC500_PRINTF("ResumeType() is called, mode = %d.\n", Mode);

	switch(Mode)
	{
	case 3:	//��S5B1�����ݸ��Ƶ�S5B0�� S4B0 --> S2B0
		status = ResumeTypeOne();
		break;

	case 4://��S5B0�����ݸ��Ƶ�S5B1�� S2B0 --> S4B0
		status = ResumeTypeOneOther();
		break;

	case 5:	// S4B0-->S2B0 , SxB2--> SxB1 ��S5B1-->S5B0
		status = ResumeTypeTwo();
		break;

		//case 6:	//��S5B0�еĽ��ױ�־Ϊ������ɣ���S5B0�����ݸ��Ƶ�S5B1�У���SxB1�����ݸ��Ƶ�SxB2
		//       status = ResumeTypeThree();
		//       break;

	case 7:	//��S5B1�еĽ��״�����ָ���1�����ױ�־��Ϊ������ɣ���дS5B0��
		//��S5B0�����ݸ��Ƶ�S5B1�У���SxB1�����ݸ��Ƶ�SxB2  S2B0 --> S4B0
		status = ResumeTypeFour();
		break;

	case 8:	//��S5B0�����ݸ��Ƶ�S5B1�У���SxB1�����ݸ��Ƶ�SxB2�� S2B0 --> S4B0
	case 9:
		status = ResumeTypeFive();
		break;

	case 10://��SxB1�����ݸ��Ƶ�SxB2�� S2B0 --> S4B0
	case 11:
		status = ResumeTypeSix();
		break;

	case 12://���޴ο鸴�Ƶ������޴ο���  S2B0 ->S4B0
		status = ResumeTypeSev();
		break;

	case 13://��SxB2�����ݸ��Ƶ�SxB1��
		status = ResumeTypeThirteen();
		break;

	default :
		break;
		// case 40:
		//       status = ResumeTypeFourNo();
		//       break;
	}

	return(status);
}
/*
*************************************************************************************************************
- �������� : unsigned char CheckSXB1ORSXB2 (void)
- ����˵�� : SXB1 == SXB0
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char CheckSXB1ORSXB2(void)
{
	unsigned char i;
	unsigned char status = 1;
	for(i = 0; i < 16; i++)
	{
		//printf("XFBuf1[0x%02X] XFBuf2[0x%02X]\n", XFBuf1[i], XFBuf2[i]);
		if(XFBuf1[i] != XFBuf2[i]) break;
		
	}
	if(i == 16) status = 0;

	DebugPrintf("i = %d status = %d\n", i, status);

	return(status);
}

/*
*************************************************************************************************************
- �������� : unsigned char CheckS2B0ORS4B0 (void)
- ����˵�� : S2B0 == S4B0
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/

unsigned char CheckS2B0ORS4B0(void)
{
	unsigned char i;
	unsigned char status = 1;
	for(i = 0; i < 16; i++)
	{
		if(Sector.STwoZero[i] != Sector.SForZero[i]) break;
	}
	if(i == 16) status = 0;
	return(status);
}

/*OK
*************************************************************************************************************
- �������� : unsigned char ProcessFunOne (HWND hDlg)
- ����˵�� : ����1
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char ProcessFunOne (void)
{
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char status;
	unsigned char Xor,i;

	DBG_RC500_PRINTF("ProcessFunOne() is called.\n");

	while(Loop)
	{
		switch (step)
		{
		case 1:			
			status = CheckS5B1Light();				//S5B1
			if(status == MI_OK){
				memcpy(Sector.SFivZero,Sector.SFivOne,16);
				step++;
			}
			else
			{				
#if 1
				Err_display(2);
                Err_save(CARD_SPEC_M1_LINUX,2);
				Loop = 0;			
#else
				memcpy(Sector.SFivOne,Sector.SFivZero,16);
				Sector.SFivOne[4] = ProEnd;
				Xor = Sector.SFivOne[0];
				for(i = 1; i < 15 ; i ++)
				{
					Xor ^= Sector.SFivOne[i];
				}
				Sector.SFivOne[15] = Xor;
				memcpy(Sector.SFivZero,Sector.SFivOne,16);	
				step++;
#endif
			}
			break;

		case 2:
			CardLan.OldTransType = Sector.SFivOne[14]&0xf0;	//�˴����õ�һ��������
			if(CardLan.OldTransType == 0x10) 			  //����
			{
				memcpy(XFBuf1,Sector.STwoOne,16);
				memcpy(XFBuf2,Sector.STwoTwo,16);
			}
			else if(CardLan.OldTransType == 0x20) 			//����
			{
				memcpy(XFBuf1,Sector.SThrOne,16);
				memcpy(XFBuf2,Sector.SThrTwo,16);
			}
			else
			{
				memcpy(XFBuf1,Sector.SForOne,16);
				memcpy(XFBuf2,Sector.SForTwo,16);
			}
			step++;
			break;

		case 3:
			status = MoneyYes(XFBuf1);				//SXB1
			if(status == MI_OK) step++;
			else
			{
				status = MoneyYes(XFBuf2);			//SXB2
				if(status == MI_OK)
				{
					status = ResumeType(5);	//�п�����д��Ǯ��ʱ��ʧ�ܣ�������ʧ�ܣ������޸��޴ο�/Ǯ��1��/����0��
					if(status == MI_OK)step = 0;
					Loop = 0;
				}
				else
				{
					Err_display(3);
                    Err_save(CARD_SPEC_M1_LINUX,3);
					Loop = 0;
				}
			}
			break;

		case 4:
			status = MoneyYes(XFBuf2);				// SXB2
			if(status == MI_OK) step++;
			else
			{
				Err_display(3);
                Err_save(CARD_SPEC_M1_LINUX,3);
				Loop = 0;
			}
			break;

		case 5:
			status = CheckSXB1ORSXB2();				//CK SXB1 = or !=  SXB2
			if(status == MI_OK) step++;
			else
			{
				status = ResumeType(7); //�۷����д���̱��ʧ��
				if(status == MI_OK)
					step = 0;
				Loop = 0;
			}
			break;

		case 6:
			status = ResumeType(3); //
			if(status == MI_OK) step++;
			else  Loop = 0;
			break;

		default:
			step = 0;
			Loop = 0;
			break;
		}
	}

#if DBG_RC500
	printf("ProcessFunOne = %d",step);
#endif
	return(step);
}
/*OK
*************************************************************************************************************
- �������� : unsigned char ProcessFunTwo (HWND hDlg)
- ����˵�� : ����2
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char ProcessFunTwo (void)
{
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char status;

	DBG_RC500_PRINTF("ProcessFunTwo() is called.\n");

	while(Loop)
	{
		switch (step)
		{

		case 1:
			CardLan.OldTransType = Sector.SFivZero[14] & 0xf0;
			if(CardLan.OldTransType == 0x10) 			//����
			{
				memcpy(XFBuf1,Sector.STwoOne,16);
				memcpy(XFBuf2,Sector.STwoTwo,16);
			}
			else if(CardLan.OldTransType == 0x20) 			//����
			{
				memcpy(XFBuf1,Sector.SThrOne,16);
				memcpy(XFBuf2,Sector.SThrTwo,16);
			}
			else
			{
				memcpy(XFBuf1,Sector.SForOne,16);
				memcpy(XFBuf2,Sector.SForTwo,16);
			}
			step++;
			break;

		case 2:
			status = MoneyYes(XFBuf1);	//SXB1
			if(status == MI_OK) step++;
			else
			{
				Err_display(3);
                Err_save(CARD_SPEC_M1_LINUX,3);
				Loop = 0;
			}
			break;

		case 3:
			status = MoneyYes(XFBuf2);	//SXB2
			if(status == MI_OK) step++;
			else
			{
				status = ResumeType(8); //S5B0 --> S5B1  SXB1 --> SXB2
				if(status == MI_OK) step = 0;
				Loop = 0;
			}
			break;

		case 4:
			status = CheckSXB1ORSXB2();		//SXB2
			if(status == MI_OK) step++;
			else
			{
				status = ResumeType(8);//S5B0 --> S5B1  SXB1 --> SXB2
				if(status == MI_OK) step = 0;
				Loop = 0;
			}
			break;
		case 5:
			status = ResumeType(4);//S5B0 --> S5B1
			if(status == MI_OK) step++;
			else  Loop = 0;
			break;

		default:
			step = 0;
			Loop = 0;
			break;

		}
	}

#if DBG_RC500
	printf("ProcessFunTwo = %d",step);
#endif
	return(step);
}




/*OK
*************************************************************************************************************
- �������� : unsigned char RepairCardInfor (void)
- ����˵�� : �޸���Ƭ��Ϣ
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char RepairCardInfor (void)
{
	struct timeval first, second;
	
	gettimeofday(&first,0);
	DebugPrintf("#1#RepiraCardInfor [%d.%06d]\n", first.tv_sec, first.tv_usec);

	DebugPrintf("\n");
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char ReturnValue = 0;
	LongUnon TempMoney;

	if(RepairLastRecErrorFlag)
		step = 2;
	
#if 	DBG_RC500	
	unsigned char i;
#endif

#if DBG_RC500
	{
		struct timeval now;
		gettimeofday(&now,NULL);
		DBG_RC500_PRINTF("RepairCardInfor() is called, time = %ld\"%06ld.\n", now.tv_sec, now.tv_usec);
	}
#endif

	while(Loop)
	{
		switch (step)
		{
		case 1:							//��Ч�����Ƿ����
			DebugPrintf("\n��Ƭ��������%02x %02x %02x %02x\n",CardLan.EnableH[0],CardLan.EnableH[1],CardLan.EnableH[2],CardLan.EnableH[3]);
			DebugPrintf("\n��Ƭ��Ч����%02x %02x %02x %02x\n",CardLan.Effective[0],CardLan.Effective[1],CardLan.Effective[2],CardLan.Effective[3]);
			DebugPrintf("\n������:%02X...\n",CardLan.CardType);
			if(Card_JudgeDate() == MI_OK)
			{
				step++;
			}
			else
			{
				#ifdef ShenMuBUS
					if(CardLan.CardType == 0x02)//ѧ����
					{
						CardLan.CardType = 0x08;
						StuToOrd = 1;
						step ++;
					}
					else
					{
				#endif
						Err_display(1);
                        Err_save(CARD_SPEC_M1_LINUX,1);
						Loop = 0;
				#ifdef ShenMuBUS
					}
				#endif
			}
			break;
			
#ifndef ADD_MONEY_SUPPORT
		case 2:
			//Sector.SOneZero[5] ���ֽ������ж��Ƿ��Ѿ��������
			//g_YuChongBackUp.i= 0;
			g_FgOprationSuccess = 0;
			 if(CardLan.MonthOrCountflag == 2 || g_FlagMonCard == 2)
			 {
				if ((Sector.STwoOne[3] == 0x7F) && (Sector.STwoTwo[3] != 0x7F))  //����ط��ж�Ԥ������������Ѿ�д���ɹ�������������дʧ��
				{
					g_FgOprationSuccess = 1;
					//memcpy(g_YuChongBackUp.longbuf, Sector.STwoOne, 3);   //ͷ�����ֽ��ǿ����Ĵ��������ĸ��ֽ����ű�־Ϊ  0x7F
					//g_YuChongBackUp.i += 1;   //��Ϊ��������1����ʵ��û�����ѳɹ������Լ�1
				}
			 }
			if ((ReturnValue = CheakBackupIsCompleteAndRepair(Sector.SOneZero[5])) == MI_OK)
			{
				step++;
			}
			else if(ReturnValue == 1)
			{
				RepairLastRecErrorFlag = 0;
				return 0xFF;
			}
			else
			{
				Loop = 0;
			}
			break;
#endif
		default:
			Loop = 0;
			step = 0;
			break;
		}
	}
	
	if(step == 0)
	{
		if(CardLan.MonthOrCountflag == 2 || g_FlagMonCard == 2)  //�����
		{
			Sector.STwoOne[3] = 0;
			Sector.STwoTwo[3] = 0;
		}
		memcpy(CardLan.Views,Sector.STwoOne,4);
		memcpy(CardLan.Subsidies,Sector.SThrOne,4);
		memcpy(CardLan.QCash,Sector.SForOne,4);
		memcpy(CardLan.ViewsValue,Sector.STwoZero+4,3);
		memcpy(CardLan.DayValue,Sector.STwoZero+7,3);
		memcpy(CardLan.MonthValue,Sector.STwoZero+10,4);
		CardLan.Period = Sector.STwoZero[14];
		memcpy(CardLan.OldTime,Sector.SFivZero+8,6);
		memcpy(CardLan.NoDiscountTime, Sector.SEigZero, 6);
		#ifdef SHENGKE_TIANGUAN
		g_FgDiscntRegTime = Sector.SEigZero[6];
		#else
		memcpy(CardLan.FirstTimeDiscount, Sector.SEigZero+6, 6);
		#endif
		CardLan.OldTransType = Sector.SFivOne[14] & 0xf0;
		CardLan.EnterExitFlag = Sector.SFivZero[3];			//������־
		DebugPrintf("CardLan.EnterExitFlag = 0x%02X\n", Sector.SFivZero[3]);
		DebugPrintf("CardLan.EnterExitFlag = 0x%02X\n", Sector.SFivOne[3]);
		//CardLan.StationID = Sector.SFivTwo[0];
		
		CardLan.StationID = Sector.SFivZero[5];
		
		//CardLan.EnterCarCi = Sector.SFivTwo[1];
		//CardLan.StationOn = Sector.SFivTwo[2];
		
		CardLan.EnterCarCi = Sector.SFivZero[6];
		CardLan.StationOn = Sector.SFivZero[7];
		DebugPrintf("####CardLan.EnterCarCi = 0x%02X\n", Sector.SFivZero[6]);
		memcpy(CardLan.OldTermNo,Sector.SFivTwo+4,4);
		
	}

	memcpy(TempMoney.longbuf, CardLan.QCash, 4);
	DebugPrintf("CardLan.QCash = 0x%02X\n", TempMoney.i);
	memcpy(TempMoney.longbuf, CardLan.Subsidies, 4);
	DebugPrintf("CardLan.Subsidies = 0x%02X\n", TempMoney.i);

	gettimeofday(&second,NULL);
	DebugPrintf("#2#RepiraCardInfor [%d.%06d]\n", second.tv_sec, second.tv_usec);
	
	DebugPrintf("step = 0x%02X\n", step);
#if DBG_RC500
	printf("RepairCardInforstep = %d\n",step);
#endif

	return(step);
}
unsigned char OnlyExchangeTimeWrite(void) //use by yue
{
	char receive_buf[16] = {0};
	int i;
	unsigned char Xor = 0, ret = 1;
	Sector.SFivZero[4] = ProEnd;
	memcpy(Sector.SFivZero+8, &Time, 6);
	Sector.SFivZero[14] = ((Sector.FlagValue+1) << 4)|0x03;
	Xor = Sector.SFivZero[0];
	for(i = 1; i < 15 ; i ++)
	{
		Xor ^= Sector.SFivZero[i];
	}
	Sector.SFivZero[15] = Xor;

	memcpy(receive_buf, Sector.SFivZero, 16);
	
	ret = WriteOneSertorDataToCard(receive_buf, LanSec.Fiv, 0, VERIFY_KEY);
	ret += WriteOneSertorDataToCard(receive_buf, LanSec.Fiv, 1, VERIFY_KEY);

	return ret;
	
}
/*
*************************************************************************************************************
- �������� : unsigned char WriteS5B0DatOne (void)
- ����˵�� :
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char WriteS5B0DatOne (unsigned char mode)
{
	static char receive_buf[20]= {0};
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char Xor,i;
//    unsigned char timemoth,Smoth;
	ShortUnon Buf;
	int status;

#if DBG_RC500
	{
		struct timeval now;
		gettimeofday(&now,0);
		DBG_RC500_PRINTF("WriteS5B0DatOne() is called, time = %ld\"%06ld.\n", now.tv_sec, now.tv_usec);
	}
#endif

	while(Loop)
	{
		switch (step)
		{
		case 1:
			Sector.SFivZero[4] = ProEnd;
			Sector.SFivZero[0] ++;
			if(Sector.SFivZero[0] > 3) Sector.SFivZero[0] = 1;

			memset(Buf.intbuf, 0, sizeof(ShortUnon));
			memset(CardLan.ViewMoney, 0, sizeof(CardLan.ViewMoney));
			
			memcpy(Buf.intbuf, Sector.SFivZero+1, 2);
			Buf.i++;
			DebugPrintf("####Buf.i = 0x%02X\n", Buf.i);
			
			memcpy(Sector.SFivZero+1, Buf.intbuf, 2);			
			memcpy(CardLan.ViewMoney, Buf.intbuf, 2);
			
			Sector.SFivZero[3] = CardLan.EnterExitFlag; //CardLan.StationOn
			Sector.SFivZero[5] = CardLan.StationID;
			DebugPrintf("Sector.SFivZero[5] = 0x%02X\n", Sector.SFivZero[5]);
			Sector.SFivZero[6] = CardLan.EnterCarCi;
			Sector.SFivZero[7] = Section.Updown;
			
			memcpy(Sector.SFivZero+8, &Time, 6);

			Sector.SFivZero[14] = ((Sector.FlagValue+1) << 4)|mode;

			Xor = Sector.SFivZero[0];
			for(i = 1; i < 15 ; i ++)
			{
				Xor ^= Sector.SFivZero[i];
			}
			Sector.SFivZero[15] = Xor;

			memcpy(receive_buf, Sector.SFivZero, 16);
			
			if(WriteOneSertorDataToCard(receive_buf, LanSec.Fiv, 0, VERIFY_KEY) == MI_OK)
				step++;
			else
				Loop = 0;
			break;

#if 0			
			status = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.Fiv + 3));
			if(status == MI_OK)step++;
			else Loop = 0;
			break;

		case 2:
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			status = ioctl(mf_fd, RC531_WRITE,(4*LanSec.Fiv + 1));
			if(status == MI_OK)
			{
				Sector.SFivOne[14] = ((Sector.FlagValue+1)<<4)| mode;
				//Sector.SFivOne[3] = CardLan.EnterExitFlag;
				Xor = Sector.SFivOne[0];
				for(i = 1; i < 15 ; i ++)
				{
					Xor ^= Sector.SFivOne[i];
				}
				Sector.SFivOne[15] = Xor;
				memcpy(receive_buf,Sector.SFivOne,16);
				status = write(mf_fd, receive_buf, 16);
				if(status == MI_OK)step++;
				else Loop = 0;
			}
			else Loop = 0;
			break;

		case 3:
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			status = ioctl(mf_fd, RC531_WRITE,4*LanSec.Fiv);
			if(status == MI_OK)
			{
				Sector.SFivZero[4] = ProStart;
				Sector.SFivZero[0] ++;
				if(Sector.SFivZero[0] > 3) Sector.SFivZero[0] = 1;

				memset(Buf.intbuf,0,sizeof(ShortUnon));
				memcpy(Buf.intbuf,Sector.SFivZero+1,2);
				Buf.i++;
				memcpy(Sector.SFivZero+1,Buf.intbuf,2);

				memset(CardLan.ViewMoney,0,sizeof(CardLan.ViewMoney));
				memcpy(CardLan.ViewMoney,Sector.SFivZero+1,2);
				Sector.SFivZero[3] = CardLan.EnterExitFlag; //CardLan.StationOn
				// memcpy(Sector.SFivZero+5,DevNum.longbuf,3);
				memcpy(Sector.SFivZero+8,&Time,6);

				Sector.SFivZero[14] = ((Sector.FlagValue+1)<<4)|mode;

				Xor = Sector.SFivZero[0];
				for(i = 1; i < 15 ; i ++)
				{
					Xor ^= Sector.SFivZero[i];
				}
				Sector.SFivZero[15] = Xor;

				memcpy(receive_buf,Sector.SFivZero,16);
				status = write(mf_fd, receive_buf, 16);
				if(status == MI_OK)step++;
				else Loop = 0;
			}
			else Loop = 0;
			break;
			/*
			case 4:
			   if((Section.Enable == 0x55)||(Section.StationOn == 0x55))
			   {
			       ioctl(mf_fd, WRITE_TYPE,W_CARD);
			       status = ioctl(mf_fd, RC531_WRITE,4*LanSec.Fiv+2);
			       if(status == MI_OK)
			       {
			           Sector.SFivTwo[0] = CardLan.StationID;
			           Sector.SFivTwo[1] = CardLan.EnterCarCi;
			           memcpy(Sector.SFivTwo+4,DevNum.longbuf,4);
			           memcpy(receive_buf,Sector.SFivTwo,16);
			           status = write(mf_fd, receive_buf, 16);
			           if(status == MI_OK)step++;
			           else Loop = 0;
			       }
			       else Loop = 0;
			   }
			   else
			   {
			       step++;
			   }
			   break;

			           case 5:
			               if(CardLan.TransCiFlag != 0)
			               {
			                   status = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.Sev + 3));
			                   if(status == MI_OK)step++;
			                   else Loop = 0;
			               }
			               else
			               {
			                   step = 100;
			               }
			               break;

			           case 6:
			               timemoth = BCD2HEX(Time.month);
			               if((timemoth>=1)&&(timemoth<=4)) Smoth = 0;
			               else if((timemoth>=5)&&(timemoth<=8)) Smoth = 1;
			               else if((timemoth>=9)&&(timemoth<=12)) Smoth = 2;
			               else return 1;
			               ioctl(mf_fd, WRITE_TYPE,W_CARD);
			               status = ioctl(mf_fd, RC531_WRITE,(4*LanSec.Sev+ Smoth));
			               if(status == MI_OK)
			               {
			                   memcpy(receive_buf,Sector.SSevnBuf,16);
			                   status = write(mf_fd, receive_buf, 16);
			                   if(status == MI_OK)step++;
			                   else Loop = 0;
			               }
			               else Loop = 0;
			               break;
			   */
#endif			   	
		default:
			Loop = 0;
			step = 0;
			break;
		}
	}
	

	DebugPrintf("WriteS5B0DatOne = %d\n",step);


	return(step);
}

/*
*************************************************************************************************************
- �������� : unsigned char WriteS2B0Dat (void)
- ����˵�� :
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char WriteS2B0Dat(void)
{
	static char receive_buf[20]= {0};
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char XOR,i;
	int status;

	DBG_RC500_PRINTF("WriteS2B0Dat() is called.\n");


	while(Loop)
	{
		switch (step)
		{
		case 1:
			XOR = Sector.STwoZero[0];
			for(i = 1; i <15; i++)
			{
				XOR ^= Sector.STwoZero[i];
			}
			Sector.STwoZero[15] = XOR;  //����У��λ

			memcpy(Sector.SForZero,Sector.STwoZero,16);
			memcpy(receive_buf,Sector.STwoZero,16);

			if(WriteOneSertorDataToCard(receive_buf, LanSec.Two, 0, VERIFY_KEY) == MI_OK)
				step++;
			else
				Loop = 0;
			break;

#if 0			
			status = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.Two + 3));
			if(status == MI_OK)step++;
			else Loop = 0;
			break;

		case 2:
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			status = ioctl(mf_fd, RC531_WRITE,4*LanSec.Two);
			if(status == MI_OK)
			{
				XOR = Sector.STwoZero[0];
				for(i = 1; i <15; i++)
				{
					XOR ^= Sector.STwoZero[i];
				}
				Sector.STwoZero[15] = XOR;  //����У��λ
				memcpy(Sector.SForZero,Sector.STwoZero,16);

				memcpy(receive_buf,Sector.STwoZero,16);
				status = write(mf_fd, receive_buf, 16);
				if(status == MI_OK)step++;
				else Loop = 0;
			}
			else Loop = 0;
			break;
#endif
		default:
			Loop = 0;
			step = 0;
			break;
		}
	}
#if DBG_RC500
	printf("WriteS2B0Dat = %d\n",step);
#endif
	return(step);
}

/*
*************************************************************************************************************
- �������� : unsigned char WriteS4B0Dat (void)
- ����˵�� :
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/

unsigned char WriteS4B0Dat(void)
{
	static char receive_buf[20]= {0};
	unsigned char Loop = 1;
	unsigned char step = 1;
	int status;

	DBG_RC500_PRINTF("WriteS4B0Dat() is called.\n");

	while(Loop)
	{
		switch (step)
		{
		case 1:
			memcpy(receive_buf,Sector.STwoZero,16);
			
			if(WriteOneSertorDataToCard(receive_buf, LanSec.For, 0, VERIFY_KEY) == MI_OK)
				step++;
			else
				Loop = 0;
			break;

#if 0
			status = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.For + 3));
			if(status == MI_OK)step++;
			else Loop = 0;
			break;
		case 2:
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			status = ioctl(mf_fd, RC531_WRITE,4*LanSec.For);
			if(status == MI_OK)
			{
				memcpy(receive_buf,Sector.SForZero,16);
				status = write(mf_fd, receive_buf, 16);
				if(status == MI_OK)step++;
				else Loop = 0;
			}
			else Loop = 0;
			break;
#endif	
		default:
			Loop = 0;
			step = 0;
			break;
		
		}
	}

#if DBG_RC500
	printf("WriteS4B0Dat = %d\n",step);
#endif
	return(step);
}


/*
*************************************************************************************************************
- �������� : unsigned char TopUpMokey (void)
- ����˵�� :
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char TopUpMokey (unsigned char type)
{
	DebugPrintf("\n");
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char SetorNum;
	int status, j;
	int  receive_len[1] = {0} ;
	unsigned char receive_buf[128]= {0};
	unsigned int uMoney;

	LongUnon TempMoney;

#if DBG_RC500
	{	struct timeval now;
		gettimeofday(&now,0);
		DBG_RC500_PRINTF("TopUpMokey() is called, time = %ld\"%06ld.\n", now.tv_sec, now.tv_usec);
	}
#endif

	memset(receive_buf, 0, sizeof(receive_buf));

	if(Sector.FlagValue == 0)
	{
		memcpy(receive_buf, Sector.STwoOne, 16);
		SetorNum = LanSec.Two;
	}
	else if(Sector.FlagValue == 1)
	{
		memcpy(receive_buf, Sector.SThrOne, 16);
		SetorNum = LanSec.Thr;
	}
	else
	{
		memcpy(receive_buf, Sector.SForOne, 16);
		SetorNum = LanSec.For;
	}
	
//	memcpy(test.money,HostValue.longbuf,4);

	

	while(Loop)
	{
		switch (step)
		{
		case 1:
			//memcpy(receive_buf, Sector.SForOne, 16);
			//GetCurWallentSertor(GET_BACKUP_SERTOR, receive_buf, &SetorNum);
			
			DebugPrintf("SetorNum = 0x%02X\n", SetorNum);

			memcpy(TempMoney.longbuf, receive_buf, 4);

			if(type == 0)
				TempMoney.i = TempMoney.i - HostValue.i;
			else
			{
#ifdef ADD_MONEY_SUPPORT                
                if(tempRechargedata.SubsidyType == 1)
				    TempMoney.i = TempMoney.i + HostValue.i;
                else
                    TempMoney.i = HostValue.i;
#endif           
				TempMoney.i = TempMoney.i + HostValue.i;
             }

			DebugPrintf("TempMoney.i = %u\n", TempMoney.i);
			DebugPrintf("HostValue.i = %u\n", HostValue.i);
			if((CardLan.TransCiFlag != 0)&&(Sector.FlagValue == 0))   //������Ԥ��ı�ǣ��ڶ���ˢ��������˱�־λ��д7f
				TempMoney.longbuf[3] = 0x7F;
			
			memcpy(receive_buf, TempMoney.longbuf, 4);
			memcpy(receive_buf + 8, &TempMoney.i, 4);

			uMoney = ~TempMoney.i;
			memcpy(receive_buf + 4, &uMoney, 4);

			if(WriteOneSertorDataToCard(receive_buf, SetorNum, 1, VERIFY_KEY) == MI_OK)
				step++;
			else 
				Loop = 0;
			break;

			
#if 0	
			//ioctl(mf_fd, WRITE_TYPE,W_CHAR);
			//status	 = write(mf_fd,&test,sizeof(struct card_buf));
			status = MI_OK;
			if(status == MI_OK)step++;
			else Loop = 0;
			break;

		case 2:
			status = ioctl(mf_fd, RC531_AUTHENT,(SetorNum + 3));
			if(status == MI_OK)step++;
			else Loop = 0;
			break;

		case 3:
			memcpy(receive_buf, Sector.SForOne, 16);

			memcpy(TempMoney.longbuf, receive_buf, 4);

			DebugPrintf("TempMoney.i = %u\n", TempMoney.i);
			if(type == 0)
			{
				TempMoney.i = TempMoney.i - HostValue.i;
			}
			else
			{
				TempMoney.i = TempMoney.i + HostValue.i;
			}
			DebugPrintf("TempMoney.i = %u\n", TempMoney.i);

			memcpy(receive_buf, TempMoney.longbuf, 4);
			memcpy(receive_buf + 8, &TempMoney.i, 4);

			uMoney = ~TempMoney.i;
			memcpy(receive_buf + 4, &uMoney, 4);

			if(WriteOneSertorDataToCard(receive_buf, SetorNum, 1, VERIFY_KEY) == MI_OK)
				step++;
			else 
				Loop = 0;

			status = ioctl(mf_fd, WRITE_TYPE, W_CARD);
			DebugPrintf("status = %u\n", status);
			status = ioctl(mf_fd, RC531_WRITE, SetorNum + 1);
			DebugPrintf("status = %u\n", status);

			if(status == MI_OK)
			{
				status = write(mf_fd, receive_buf, 16);
				DebugPrintf("status = %u\n", status);
				if(status == MI_OK)step++;
				else Loop = 0;
			}
		
			if(type == 0)
			{
				status = ioctl(mf_fd, RC531_DEC,(SetorNum + 1));
			}
			else
			{
				status = ioctl(mf_fd, RC531_INC,(SetorNum + 1));
			}

        case 4:
            status = ioctl(mf_fd, RC531_READ,(SetorNum + 1));
            if(status == MI_OK)
            {
                status = ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
                status = read(mf_fd, receive_buf, receive_len[0]);
                if(status >= 0)
                {
                    memcpy(rbuf,receive_buf,16);
                }
            }
            step++;
            break;

			
			break;
#endif

		default:
			Loop = 0;
			step = 0;
			break;
		}
	}
	DebugPrintf("step = %u\n", step);
#if DBG_RC500
	printf("TopUpMokey = %d\n",step);
#endif
	return(step);
}

/*
*************************************************************************************************************
- �������� : unsigned char WriteFlag (void)
- ����˵�� :
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char WriteCiFlag (void)
{
	static char receive_buf[20]= {0};
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char timemoth,Smoth;
	int status;

#if DBG_RC500
	{
		struct timeval now;
		gettimeofday(&now,0);
		DBG_RC500_PRINTF("WriteCiFlag() is called, time = %ld\"%06ld.\n", now.tv_sec, now.tv_usec);
	}
#endif

	while(Loop)
	{
		switch (step)
		{
		case 1:
			timemoth = BCD2HEX(Time.month);
			
			if((timemoth>=1)&&(timemoth<=4)) 
				Smoth = 0;
			else if((timemoth>=5)&&(timemoth<=8)) 
				Smoth = 1;
			else if((timemoth>=9)&&(timemoth<=12)) 
				Smoth = 2;
			else 
				return 1;
			
			memcpy(receive_buf, Sector.SSevnBuf, 16);
			
			if(WriteOneSertorDataToCard(receive_buf, LanSec.Sev, Smoth, VERIFY_KEY) == MI_OK)
				step++;
			else
				Loop = 0;
			break;
#if 0			
			status = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.Sev + 3));
			if(status == MI_OK)step++;
			else Loop = 0;
			break;

		case 2:
			timemoth = BCD2HEX(Time.month);
			if((timemoth>=1)&&(timemoth<=4)) Smoth = 0;
			else if((timemoth>=5)&&(timemoth<=8)) Smoth = 1;
			else if((timemoth>=9)&&(timemoth<=12)) Smoth = 2;
			else return 1;
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			status = ioctl(mf_fd, RC531_WRITE,(4*LanSec.Sev+ Smoth));
			if(status == MI_OK)
			{
				memcpy(receive_buf,Sector.SSevnBuf,16);
				status = write(mf_fd, receive_buf, 16);
				if(status == MI_OK)step++;
				else Loop = 0;
			}
			else Loop = 0;
			break;
#endif
		default:
			Loop = 0;
			step = 0;
			break;
		}
	}

#if DBG_RC500
	printf("WriteCiFlag = %d\n",step);
#endif

	return(step);
}


/*
*************************************************************************************************************
- �������� : unsigned char WriteFlag (void)
- ����˵�� :
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char WriteFlag (unsigned char mode)
{
	static char receive_buf[20]= {0};
	unsigned char Loop = 1;
	unsigned char step = 1;
	// unsigned char Xor,i;
	unsigned char SECTOR;
	int  status;

#if DBG_RC500
	{
		struct timeval now;
		gettimeofday(&now,0);
		DBG_RC500_PRINTF("WriteFlag() is called, time = %ld\"%06ld.\n", now.tv_sec, now.tv_usec);
	}
#endif

	SECTOR = Sector.SFivZero[0]-1;
	if(SECTOR > 2) SECTOR  = 0;


	while(Loop)
	{
		switch (step)
		{
		case 1:
			receive_buf[0] = Time.month;
			receive_buf[1] = Time.day;
			receive_buf[2] = Time.hour;
			receive_buf[3] = Time.min;

			if(Sector.FlagValue == 0)
			{
				memcpy(receive_buf+4,CardLan.Views,4);
			}
			else if(Sector.FlagValue == 1)
			{
				memcpy(receive_buf+4,CardLan.Subsidies,4);
			}
			else if(Sector.FlagValue == 2)
			{
				memcpy(receive_buf+4,CardLan.QCash,4);
			}
			else
			{
				memcpy(receive_buf+4,"\x00\x00\x00\x00",4);
			}
			memcpy(receive_buf+8,HostValue.longbuf,3);

			receive_buf[11] = ((Sector.FlagValue+1)<<4)|mode;
			memcpy(receive_buf+12,DevNum.longbuf,4);
		
			if(WriteOneSertorDataToCard(receive_buf, LanSec.Six, SECTOR, VERIFY_KEY) == MI_OK)
				step++;
			else
				Loop = 0;
			break;

#if 0			
			status = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.Six + 3));
			if(status == MI_OK)step++;
			else Loop = 0;
			break;

		case 2:
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			status = ioctl(mf_fd, RC531_WRITE,4*LanSec.Six+SECTOR);
			if(status == MI_OK)
			{
				receive_buf[0] = Time.month;
				receive_buf[1] = Time.day;
				receive_buf[2] = Time.hour;
				receive_buf[3] = Time.min;

				if(Sector.FlagValue == 0)
				{
					memcpy(receive_buf+4,CardLan.Views,4);
				}
				else if(Sector.FlagValue == 1)
				{
					memcpy(receive_buf+4,CardLan.Subsidies,4);
				}
				else if(Sector.FlagValue == 2)
				{
					memcpy(receive_buf+4,CardLan.QCash,4);
				}
				else
				{
					memcpy(receive_buf+4,"\x00\x00\x00\x00",4);
				}
				memcpy(receive_buf+8,HostValue.longbuf,3);

				receive_buf[11] = ((Sector.FlagValue+1)<<4)|mode;
				memcpy(receive_buf+12,DevNum.longbuf,4);
				status = write(mf_fd, receive_buf,16);
				if(status == MI_OK)step++;
				else Loop = 0;
			}
			else Loop = 0;
			break;
#endif
		default:
			Loop = 0;
			step = 0;
			break;
		}
	}
#if DBG_RC500
	{
		struct timeval now;
		gettimeofday(&now,0);
		DBG_RC500_PRINTF("WriteFlag(): time = %ld\"%06ld.\n", now.tv_sec, now.tv_usec);
	}
#endif
	return(step);
}
/*********************************************************************************
	added by taeguk 
Function description :  Make sure cash-sector write ok
**********************************************************************************/
unsigned char MakeSureCashSecOk(unsigned char type)
{
	static int receive_len[1]= {0};
	static char receive_buf[20]= {0};
	unsigned char SetorNum;
	LongUnon OldValue,Buf;
	int status = 1;
	if(Sector.FlagValue == 0)
	{
		SetorNum = 4*LanSec.Two;
		memcpy(Buf.longbuf,CardLan.Views,sizeof(LongUnon));
		if(type == 0)
		{
			OldValue.i = Buf.i - HostValue.i;
		}
		else
		{
			OldValue.i = Buf.i + HostValue.i;
		}
	}
	else if(Sector.FlagValue == 1)
	{
		SetorNum = 4*LanSec.Thr;
		memcpy(Buf.longbuf,CardLan.Subsidies,sizeof(LongUnon));
		if(type == 0)
		{
			OldValue.i = Buf.i - HostValue.i;
		}
		else
		{
			OldValue.i = Buf.i + HostValue.i;
		}
	}
	else
	{
		SetorNum = 4*LanSec.For;
		memcpy(Buf.longbuf,CardLan.QCash,sizeof(LongUnon));
		if(type == 0)
		{
			OldValue.i = Buf.i - HostValue.i;
		}
		else
		{
			OldValue.i = Buf.i + HostValue.i;
		}
	}	
	status = ioctl(mf_fd, RC531_AUTHENT,(SetorNum + 3));
	if(status != MI_OK)	return 1;
	status = ioctl(mf_fd, RC531_READ,(SetorNum+1));
	if(status != MI_OK)	return 2;
	status = ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
	status = read(mf_fd, receive_buf, receive_len[0]);
	if(status >= 0)
		memcpy(Buf.longbuf,receive_buf,4);		
	else
		return 3;
	if(OldValue.i != Buf.i)	
	{
		status = TopUpMokey(type);
		if(status != MI_OK)	return 4;		
	}
	return MI_OK;
}

/*
*************************************************************************************************************
- �������� : unsigned char WriteS5B0DatTwo (void)
- ����˵�� :
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char WriteFlagRstart(unsigned char type)
{
	DebugPrintf("\n");
	static int receive_len[1]= {0};
	static char receive_buf[20]= {0};
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char Qflag = 1;
	unsigned char Xor,i;
	unsigned char Timebuf[8];//Keybuf[8],
	unsigned char nbuf[20],len,ic=0;
	unsigned char SetorNum;
	LongUnon OldValue,Buf;
	int status = 1;
	unsigned char Keybuf[8];

	if(Sector.FlagValue == 0)
	{
		SetorNum = 4*LanSec.Two;
		memcpy(Buf.longbuf,CardLan.Views,sizeof(LongUnon));
		if(type == 0)
		{
			OldValue.i = Buf.i - HostValue.i;
		}
		else
		{
			OldValue.i = Buf.i + HostValue.i;
		}
	}
	else if(Sector.FlagValue == 1)
	{
		SetorNum = 4*LanSec.Thr;
		memcpy(Buf.longbuf,CardLan.Subsidies,sizeof(LongUnon));
		if(type == 0)
		{
			OldValue.i = Buf.i - HostValue.i;
		}
		else
		{
			OldValue.i = Buf.i + HostValue.i;
		}
	}
	else
	{
		SetorNum = 4*LanSec.For;
		memcpy(Buf.longbuf,CardLan.QCash,sizeof(LongUnon));
		if(type == 0)
		{
			OldValue.i = Buf.i - HostValue.i;
		}
		else
		{
			OldValue.i = Buf.i + HostValue.i;
		}
	}

	SetColor(Mcolor);
	SetTextColor(Color_red);
    #ifdef NEW0409
	SetTextSize(48);
	TextOut(0 , 75, "��ܰ��ʾ");
	TextOut(0 , 144, "ˢ������");
	TextOut(0	, 210,"������ˢ��");
    #else
	SetTextSize(32);
	TextOut(100 , 50, "��ܰ��ʾ");
	TextOut(100 , 90, "ˢ������");
	TextOut(85	, 130,"������ˢ��");
    #endif

	LEDL(1);
	beepopen(10);
	PlayMusic(15, 1);
	return SWIPE_CARD_ERROR;

	while(Qflag)
	{
		Rd_time (Timebuf+1);
		if(Time.sec != Timebuf[6])
		{
			Timebuf[0] = 0x20;
			Time.year = Timebuf[1];
			Time.month = Timebuf[2];
			Time.day = Timebuf[3];
			Time.hour = Timebuf[4];
			Time.min = Timebuf[5];
			Time.sec = Timebuf[6];

			ic++;
			if(ic>30)
			{
				Qflag =0;
				break;
			}

			if((ic>2)&&(ic%3 == 0))
			{
				PlayMusic(15, 0);   //����ʱ�ر�
			//printf("���Ŵ�������\n");
			}
		}

		Loop = 1;
		step = 1;

		while(Loop)
		{
			switch (step)
			{

			case 1:
				status = CardReset(nbuf, &len, 1);
				if(status == MI_OK)
				{
					step++;
				}
				else
				{
					Loop = 0;
				}
				break;

			case 2:
			/*	test.mode = KEYB;
				memset(test.key,0xFF,6);
				memset(test.rwbuf,0xff,16);
				memcpy(Keybuf,CardLan.UserIcNo,4);
				memcpy(Keybuf+4,SnBack,4);
				DES_CARD(KeyDes,Keybuf,test.key);*/
				step++;
				break;

			case 3:
				ioctl(mf_fd, WRITE_TYPE, W_CHAR);
				status = write(mf_fd, &test, sizeof(struct card_buf));
				if(status == MI_OK)step++;
				else Loop = 0;
				break;

			case 4:
				status = ioctl(mf_fd, RC531_AUTHENT,(SetorNum + 3));
				if(status == MI_OK)step++;
				else Loop = 0;
				break;

			case 5:
				status = ioctl(mf_fd, RC531_READ,(SetorNum+1));
				if(status == MI_OK)
				{
					status = ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
					status = read(mf_fd, receive_buf, receive_len[0]);
					if(status >= 0)
					{
                        if(MoneyYes(receive_buf) != MI_OK){
				            ioctl(mf_fd, WRITE_TYPE,W_CARD);
				            status = ioctl(mf_fd, RC531_WRITE,SetorNum+1);
				            status |= write(mf_fd, Sector.SForTwo, 16);
							if(status != MI_OK)
								Loop = 0;
							else
								memcpy(receive_buf, Sector.SForTwo, 16);
						}
						memcpy(Buf.longbuf,receive_buf,4);
						step++;
					}
					else
					{
						Loop = 0;
					}
				}
				else Loop = 0;
				break;

			case 6:
				if(OldValue.i == Buf.i)
				{
					step++;
				}
				else
				{
					status = TopUpMokey(type);
					if(status == MI_OK)step--;
					else Loop = 0;

					/*
					    memcpy(test.money,HostValue.longbuf,4);
					    ioctl(mf_fd, WRITE_TYPE, W_CHAR);
					    status = write(mf_fd, &test, sizeof(struct card_buf));
					    if(type == 0)
					    {

					        status = ioctl(mf_fd, RC531_DEC,(SetorNum + 1));
					    }
					    else
					    {
					        status = ioctl(mf_fd, RC531_INC,(SetorNum + 1));
					    }
					    if(status == MI_OK)step++;
					    else Loop = 0;
					*/
				}
				break;



			case 7:
				status = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.Fiv + 3));
				if(status == MI_OK)step++;
				else Loop = 0;
				break;

			case 8:
				ioctl(mf_fd, WRITE_TYPE,W_CARD);
				status = ioctl(mf_fd, RC531_WRITE,4*LanSec.Fiv);
				if(status == MI_OK)
				{
					Sector.SFivZero[4] = ProEnd;
					Xor = Sector.SFivZero[0];
					for(i = 1; i < 15 ; i ++)
					{
						Xor ^= Sector.SFivZero[i];
					}
					Sector.SFivZero[15] = Xor;
					memcpy(Sector.SFivOne,Sector.SFivZero,16);
					memcpy(receive_buf,Sector.SFivZero,16);
					status = write(mf_fd, receive_buf, 16);
					if(status == MI_OK)step++;
					else Loop = 0;
				}
				else Loop = 0;
				break;

			case 9:
				ioctl(mf_fd, WRITE_TYPE,W_CARD);
				status = ioctl(mf_fd, RC531_WRITE,4*LanSec.Fiv+1);
				if(status == MI_OK)
				{
					memcpy(receive_buf,Sector.SFivZero,16);
					status = write(mf_fd, receive_buf, 16);
					if(status == MI_OK)step++;
					else Loop = 0;
				}
				else Loop = 0;
				break;

			case 10:
				// if((Section.Enable == 0x55)||(Section.StationOn == 0x55))
				// {
				ioctl(mf_fd, WRITE_TYPE,W_CARD);
				status = ioctl(mf_fd, RC531_WRITE,4*LanSec.Fiv+2);
				if(status == MI_OK)
				{
					//Sector.SFivTwo[0] = CardLan.StationID;
					//Sector.SFivTwo[1] = CardLan.EnterCarCi;
					//Sector.SFivTwo[2] = Section.Updown; //
					memcpy(Sector.SFivTwo+4,DevNum.longbuf,4);
					memcpy(receive_buf,Sector.SFivTwo,16);
					status = write(mf_fd, receive_buf, 16);
					if(status == MI_OK)step++;
					else Loop = 0;
				}
				else Loop = 0;
				// }
				// else
				// {
				//     step++;
				// }
				break;

			default:
				Loop = 0;
				step = 0;
				break;
			}
		}

		if(step == 0)
		{
			Qflag = 0;	
			break;
		}
	}
	LEDL(0);
	beepopen(11);

#if DBG_RC500
	printf("WriteFlagRstart = %d\n",step);
#endif

	return(step);
}





/*
*************************************************************************************************************
- �������� : unsigned char WriteS5B0DatTwo (void)
- ����˵�� :
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char WriteS5B0DatTwo (unsigned char type)
{
	DebugPrintf("\n");
	static char receive_buf[20]= {0};
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char Xor,i;
	int status;
	
	memset(receive_buf, 0, sizeof(receive_buf));
		
#if DBG_RC500
	{
		struct timeval now;
		gettimeofday(&now,0);
		DBG_RC500_PRINTF("WriteS5B0DatTwo() is called, time = %ld\"%06ld.\n", now.tv_sec, now.tv_usec);
	}
#endif
	DebugPrintf("Sector.SFivZero[3] = 0x%02X\n", Sector.SFivZero[3]);
	while(Loop)
	{
		switch (step)
		{
		case 1:

			memcpy(receive_buf,Sector.SFivZero,16);
			DebugPrintf("Sector.SFivOne[3] = 0x%02X\n", receive_buf[3]);
			
			if(WriteOneSertorDataToCard(receive_buf, LanSec.Fiv, 1, VERIFY_KEY) == MI_OK)
				step++;
			else
				Loop = 0;
			break;
#if 0	
			status = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.Fiv + 3));
			if(status == MI_OK)
				step++;
			else 
				Loop = 0;
			break;

		case 2:

			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			if(ioctl(mf_fd, RC531_WRITE, 4*LanSec.Fiv) == MI_OK)
			{
		
				Sector.SFivZero[4] = ProEnd;
				Xor = Sector.SFivZero[0];
				for(i = 1; i < 15 ; i ++)
				{
					Xor ^= Sector.SFivZero[i];
				}
				Sector.SFivZero[15] = Xor;

				memcpy(Sector.SFivOne, Sector.SFivZero, 16);
				memcpy(receive_buf, Sector.SFivZero, 16);

				if(write(mf_fd, receive_buf, 16) == MI_OK)

					step++;
				else 
					Loop = 0;
			}
			else 
				Loop = 0;
			break;

		case 3:
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			
			if(ioctl(mf_fd, RC531_WRITE,4*LanSec.Fiv+1) == MI_OK)
			{
				memcpy(receive_buf,Sector.SFivZero,16);
				DebugPrintf("Sector.SFivZero = 0x%02X\n", Sector.SFivZero[3]);
				if(write(mf_fd, receive_buf, 16) == MI_OK)
					step++;
				else 
					Loop = 0;
			}
			else 
				Loop = 0;
			break;

		case 4:
			ioctl(mf_fd, WRITE_TYPE,W_CARD);
			
			if(ioctl(mf_fd, RC531_WRITE,4*LanSec.Fiv+2) == MI_OK)
			{
				Sector.SFivTwo[0] = CardLan.StationID;
				Sector.SFivTwo[1] = CardLan.EnterCarCi;
				Sector.SFivTwo[2] = Section.Updown; //
				memcpy(Sector.SFivTwo+4, DevNum.longbuf, 4);

				DebugPrintf("DevNum.i = %u\n", DevNum.i);
				
				memcpy(receive_buf, Sector.SFivTwo, 16);
				
				if(write(mf_fd, receive_buf, 16) == MI_OK)
					step++;
				else 
					Loop = 0;
			}
			else 
				Loop = 0;
			break;
		
		case 5:
			status = MakeSureCashSecOk(type);
			if(status == MI_OK) step++;
			else Loop = 0;
			break;
#endif			
		default:
			Loop = 0;
			step = 0;
			break;
		}
	}

#if 0	
#ifndef	NO_RESWIPING_CARD
	if(step != 0)
	{	
		DebugPrintf("step = %d\n", step);
		step = WriteFlagRstart(type);
		DebugPrintf("step = 0x%02X\n", step);
	}
#endif
#endif
	DebugPrintf("WriteS5B0DatTwo = %d\n",step);
#if DBG_RC500
	printf("WriteS5B0DatTwo = %d\n",step);
#endif
	return(step);
}
/*
*************************************************************************************************************
- �������� : unsigned char CopySXB1ToSXB2 (void)
- ����˵�� :
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char CopySXB1ToSXB2 (void)
{
	unsigned char Loop = 1;
	unsigned char step = 1;
	int status, SectorNum;
	unsigned char TempBuf[16];

#if DBG_RC500
	{
		struct timeval now;

		gettimeofday(&now,0);
		DBG_RC500_PRINTF("CopySXB1ToSXB2() is called, time = %ld\"%06ld.\n", now.tv_sec, now.tv_usec);
	}
#endif
#if 1
	if(Sector.FlagValue == 0)
	{
		SectorNum = LanSec.Two;
	}
	else if(Sector.FlagValue == 1)
	{
		SectorNum = LanSec.Thr;
	}
	else
	{
		SectorNum = LanSec.For;
	}
#endif

	//GetCurWallentSertor(GET_NO_SERTOR, NULL, &SectorNum);

	while(Loop)
	{
		switch (step)
		{
		case 1:
			status = ioctl(mf_fd, RC531_AUTHENT,(4*SectorNum + 3));
			if(status == MI_OK)step++;
			else Loop = 0;
			break;

		case 2:
			status = ioctl(mf_fd,RC531_RESTORE,(4*SectorNum + 1));//ֻ����Ϣ����
			if(status == MI_OK)step++;
			else Loop = 0;
			break;

		case 3:
			status = ioctl(mf_fd,RC531_TRANSFER,(4*SectorNum + 2));//ֻ����Ϣ����
			if(status== MI_OK)step++;
			else Loop = 0;
			break;

		default:
			Loop = 0;
			step = 0;
			break;
		}
	}

#if DBG_RC500
	printf("CopySXB1ToSXB2 = %d\n",step);
#endif
	return(step);

}




unsigned char WriteBlockToS5B2(void)
{
	char receive_buf[16] = {0};
	unsigned char ReturnFlag = 0;

	DebugPrintf("####CardLan.StationID = 0x%02X\n", CardLan.StationID);
	//Sector.SFivTwo[0] = CardLan.StationID;
	//Sector.SFivTwo[1] = CardLan.EnterCarCi;
	//Sector.SFivTwo[2] = Section.Updown; 
	if (CardLan.MonthOrCountflag == 2 || g_FlagMonCard == 2)
		Sector.SFivTwo[14] = 2;
	//else if (CardLan.MonthOrCountflag == 1 || g_FlagMonCard == 1)
	//	Sector.SFivTwo[14] = 1;
	memcpy(Sector.SFivTwo+4, DevNum.longbuf, 4);
	memcpy(receive_buf, Sector.SFivTwo, 16);

	if(WriteOneSertorDataToCard(receive_buf, LanSec.Fiv, 2, VERIFY_KEY) == MI_OK)
		ReturnFlag =  MI_OK;	
	else
		ReturnFlag =  MI_FAIL;
	
	DebugPrintf("step = 0x%02X\n", ReturnFlag);
	return ReturnFlag;
}

unsigned char WriteNoDiscountTime(void)
{
	char receive_buf[16] = {0};
	unsigned char ReturnFlag = 0;
	#ifdef SHENGKE_TIANGUAN
	if (g_FgWriteNoDiscount == 1)
	{
		memcpy(Sector.SEigZero, CardLan.NoDiscountTime, 6);
		Sector.SEigZero[6] = g_FgDiscntRegTime;
	}
	else if (g_FgWriteNoDiscount ==2)
	{
		Sector.SEigZero[6] = 0; //clear g_FgDiscntRegTime
	}
	#else
	if (g_FgWriteNoDiscount == 1)
		memcpy(Sector.SEigZero, CardLan.NoDiscountTime, 6);
	else if (g_FgWriteNoDiscount == 2) 
		memcpy(Sector.SEigZero+6, CardLan.FirstTimeDiscount, 6);
	#endif
	memcpy(receive_buf, Sector.SEigZero, 16);
	if(WriteOneSertorDataToCard(receive_buf, 8, 0, VERIFY_KEY) == MI_OK)
		ReturnFlag =  MI_OK;	
	else
		ReturnFlag =  MI_FAIL;
	
	return ReturnFlag;
}


/*
*************************************************************************************************************
- �������� : unsigned char TopUpCardInfor (HWND hDlg)
- ����˵�� : ��ֵ���
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char TopUpCardInfor (unsigned char type) // 0 Ϊ�۷�  1Ϊ��ֵ
{
	DebugPrintf("\n");
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char ReturnValue;
	int i,transmode;
    
	struct timeval first, second;
	
	gettimeofday(&first,0);
	//DebugPrintf("#1#TopUpCardInfor [%d.%d]\n", first.tv_sec, first.tv_usec);
    printf("#1#TopUpCardInfor [%d.%d]\n", first.tv_sec, first.tv_usec);

	time_t NowTime;
	struct ErrorRecordLsit pErrorRec;

#if DBG_RC500
	{
		struct timeval now;

		gettimeofday(&now,0);
		DBG_RC500_PRINTF("TopUpCardInfor() is called, time = %ld\"%06ld.\n", now.tv_sec, now.tv_usec);
	}
#endif

	while(Loop)
	{
		switch (step)
		{
		case 1:
			if(type == 0)
			{
				ReturnValue = WriteS5B0DatOne(0x03);		//дS5B0��־��ʼ 3 ����
			}
			else
			{
				ReturnValue = WriteS5B0DatOne(0x04); 	//дS5B0��־��ʼ 4 ����
			}
			if(ReturnValue == MI_OK) 
				step ++;
			else 
				Loop = 0;
			break;

		case 2:
			if(CardLan.CiMoneyFlag == 0x01)
			{
				if(WriteS2B0Dat() == MI_OK) //д�޴ζ���Ϣ
					step ++;
				else 
					Loop = 0;
			}
			else
			{
				step ++;
			}
			break;
	//��Ǯ��д�������������������ȿ�Ǯ��д����������ԭ�����෴��		
		case 3:
			if(TopUpMokey(type) == MI_OK)// ��Ǯ
				step ++;
			else 
				Loop = 0;
			break;
		case 4:
			if(((CardLan.TransCiFlag != 0)&&(Sector.FlagValue == 0)) || (g_FgOprationSuccess==2))
			{
				if(WriteCiFlag() == MI_OK) 
					step ++;
				else 
					Loop = 0;
			}
			else
			{
				step ++;
			}
			break;
		case 5:
			if(WriteBlockToS5B2() == MI_OK)
				step++;
			else
				Loop = 0;
			break;
		case 6: 
			if((g_FgWriteNoDiscount))
			{
				if(WriteNoDiscountTime() == MI_OK)
					step++;
				else
					Loop = 0;
				g_FgWriteNoDiscount = 0;
			}
			else
				step++;
			break;
			
		case 7: //6
			//��������־д�뵽����
			Sector.SOneZero[5] = OPERATION_COMPLETE;
			if (CardLan.MonthOrCountflag == 2 || g_FlagMonCard == 2)
				Sector.SOneZero[8] = 2;
			//else if (CardLan.MonthOrCountflag == 1 || g_FlagMonCard == 1)
			//	Sector.SOneZero[8] = 1;
			UseForTest = 0;
			if((ReturnValue = WriteOneSertorDataToCard(Sector.SOneZero, LanSec.One, 0, VERIFY_KEY)) == MI_OK)
			{
				step ++;
				DebugPrintf("[operation success]\n");
			}
			else if(ReturnValue > 0 && ReturnValue < 3)
			{
				printf("д���־���ɹ�ReturnValue = %d\n", ReturnValue);
				Loop = 0;
			}
			else
			{
				ShowSwipeCardStatus(1);

				IncSerId();
                if(Section.Enable!=0x55)
                    transmode = 0;
                else
                    transmode = 2;
#ifdef Transport_Stander
                if(SaveCardData_jiaotong(CARD_SPEC_M1_LINUX, transmode, GET_RECORD) == MI_OK)
#else
                if(SaveCardData(CARD_SPEC_M1_LINUX, transmode, GET_RECORD) == MI_OK)
#endif                    
				{
					DebugPrintf("\n");
					memset(&pErrorRec, 0, sizeof(pErrorRec));
					pErrorRec.next = NULL;

					time(&NowTime);
					pErrorRec.RecordTime = NowTime;
					memcpy(pErrorRec.CardNo, CardLan.UserIcNo, 4);
					memcpy(&pErrorRec.SaveRecordBuf, &SaveData, sizeof(RecordFormat));

					//��������������Ļ��ʾ
					memcpy(&pErrorRec.DisplayData.cardlan, &CardLan, sizeof(CardInform));
					memcpy(pErrorRec.SectorOneData, Sector.SOneZero, 16);
	
					pErrorRec.DisplayData.bStolenDis = bStolenDis;
					pErrorRec.DisplayData.FlagValue = Sector.FlagValue;
					pErrorRec.DisplayData.HostValue.i = HostValue.i;
					pErrorRec.DisplayData.StolenAmount = StolenAmount;

					AddOneErrorRecordtoList(&pErrorRec);

					WriteAllErrorRecordToFile();
					UseForTest = 0;
					printf("return SWIPE_CARD_ERROR\n");
					return SWIPE_CARD_ERROR;
					
				}
				else
				{
					DevSID.i--;		/* ��������ʧ����Ҫ�ڱ���һ�� */
					DebugPrintf("save data failed \n");
				}

				

			}
			UseForTest = 0;
			break;
#if 0 
		case 5:
			status = WriteS5B0DatTwo(type);	//дS5B0��ɣ���S5B0���Ƶ�S5B1
			if(status == MI_OK) step ++;
			else Loop = 0;
				//gettimeofday(&test2,0);
				//printf("WriteS5B0DatTwo time is Microsecond : %d, Millisecond : %d \n",(test2.tv_sec - test1.tv_sec)*1000000 + (test2.tv_usec-test1.tv_usec),((test2.tv_sec - test1.tv_sec)*1000000 + (test2.tv_usec-test1.tv_usec))/1000);																									
			break;

		case 6:
			status = CopySXB1ToSXB2();	//Ǯ�� ����
			if(status == MI_OK) step ++;
			else Loop = 0;
				//gettimeofday(&test2,0);
				//printf("CopySXB1ToSXB2 time is Microsecond : %d, Millisecond : %d \n",(test2.tv_sec - test1.tv_sec)*1000000 + (test2.tv_usec-test1.tv_usec),((test2.tv_sec - test1.tv_sec)*1000000 + (test2.tv_usec-test1.tv_usec))/1000);																												
			break;

		case 7:
			if(CardLan.CiMoneyFlag == 0x01)
			{
				status = WriteS4B0Dat();		//�޴��޶� ����
				if(status == MI_OK) step ++;
				else Loop = 0;
			}
			else
			{
				step++;
			}
				//gettimeofday(&test2,0);
				//printf("WriteS4B0Dat time is Microsecond : %d, Millisecond : %d \n",(test2.tv_sec - test1.tv_sec)*1000000 + (test2.tv_usec-test1.tv_usec),((test2.tv_sec - test1.tv_sec)*1000000 + (test2.tv_usec-test1.tv_usec))/1000);																															
			break;

		case 8:
			if(type == 0)
			{
				status = WriteFlag(0x03);			//д�����׼�¼
			}
			else
			{
				status = WriteFlag(0x04);			 //д�����׼�¼4 ����
			}
			if(status == MI_OK) step ++;
			else Loop = 0;
				//gettimeofday(&test2,0);
				//printf("WriteFlag time is Microsecond : %d, Millisecond : %d \n",(test2.tv_sec - test1.tv_sec)*1000000 + (test2.tv_usec-test1.tv_usec),((test2.tv_sec - test1.tv_sec)*1000000 + (test2.tv_usec-test1.tv_usec))/1000);																																		
			break;	
#endif
		default:
			Loop = 0;
			step = 0;
			break;
		}
	}
	DebugPrintf("TopUpCardInfor = %d\n",step);
	gettimeofday(&second,0);
	//DebugPrintf("#2#TopUpCardInfor [%d.%d]\n", second.tv_sec, second.tv_usec);
     printf("#2#TopUpCardInfor [%d.%d]\n", second.tv_sec, second.tv_usec);   
#if DBG_RC500
	printf("TopUpCardInfor = %d\n",step);
#endif
	// transaction is regarded as successful if the electronic purse is changed.
	return step;
}


unsigned char BackupCurrentWallet(unsigned char type)
{
	DebugPrintf("\n");
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char status, ReturnValue = 0;
	
	DebugPrintf("Sector.FlagValue = %d \n", Sector.FlagValue);
	DebugPrintf("Sector.SFivZero[3] = 0x%02X\n", Sector.SFivZero[3]);

	while(Loop)
	{
		switch (step)
		{
		case 1:
			if(CopySXB1ToSXB2() == MI_OK) //Ǯ�� ����
				step++;
			else 
				Loop = 0;
			break;

		case 2:
			if(CardLan.CiMoneyFlag == 0x01)
			{
				if(WriteS4B0Dat() == MI_OK) //�޴��޶� ����
					step++;
				else 
					Loop = 0;
			}
			else
			{
				step++;
			}
			break;
		case 3:
			if(WriteS5B0DatTwo(type) == MI_OK) //дS5B0��ɣ���S5B0���Ƶ�S5B1
				step ++;
			else 
				Loop = 0; 
			break;
		case 4:
			if(type == 0)
				ReturnValue = WriteFlag(0x03);			//д�����׼�¼
			else
				ReturnValue = WriteFlag(0x04);			 //д�����׼�¼4 ����
			
			if(ReturnValue == MI_OK) 
				step ++;
			else 
				Loop = 0;
			break;	

		case 5:
			Sector.SOneZero[5] = BACKUP_COMPLETE;
			if (CardLan.MonthOrCountflag == 2 || g_FlagMonCard == 2)
				Sector.SOneZero[8] = 2;
			//else if (CardLan.MonthOrCountflag == 1 || g_FlagMonCard == 1)
			//	Sector.SOneZero[8] = 1;
			if(WriteOneSertorDataToCard(Sector.SOneZero, LanSec.One, 0, VERIFY_KEY) == MI_OK)
				step ++;
			else
				Loop = 0;
			break;
			
		default:
			Loop = 0;
			step = 0;
			break;
	
		}
	}
	DebugPrintf("step = 0x%02X\n", step);

	return 0;
}

/*
*************************************************************************************************************
- �������� : unsigned char ReadSectorSix(void)
- ����˵�� : �ϴν��׼�¼
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
/*
unsigned char ReadSectorSix(void)
{
 int ret;
 unsigned char Loop = 1;
 unsigned char step = 1;
 static int  receive_len[1] ={0};
 static char receive_buf[20]={0};
    while(Loop)
    {
         switch (step)
	 {
		case 1:
			ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.Six + 3));
                        if(ret == MI_OK)step++;
                        else Loop = 0;
			break;
		case 2:
                        if((CardLan.OldTransType == 0)||(CardLan.OldTransType > 3))
				CardLan.OldTransType = 1;
                        ret = ioctl(mf_fd, RC531_READ,(4*LanSec.Six + CardLan.OldTransType - 1));
                        if(ret == MI_OK)
			{
				ret = ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				ret = read(mf_fd, receive_buf, receive_len[0]);
				if(ret >= 0)
				{
					memcpy(CardLan.OldTime,receive_buf,4);
					memcpy(CardLan.OldTermNo,receive_buf+12,3);
 					step++;
				}
				else
				{
					Loop = 0;
				}
			}
                        else Loop = 0;
                        break;
		case 3:
                        step = 0;
                        Loop = 0;
                	break;
	        default:
                        step = 0;
                        Loop = 0;
                	break;
	}
#if DBG_RC500
    printf("sixstep = %d\r\n",step);
#endif
    }
    return(step);
}
*/
/*
unsigned char TimeWriecarad(void)
{
    unsigned char Loop = 1;
    unsigned char step = 1;
    unsigned char status;

    while(Loop)
    {
        switch (step)
        {
        case 1:
            status = WriteS5B0DatOne();			//����־�ÿ�ʼ
            if(status == MI_OK) step ++;
            else Loop = 0;
            break;

        case 2:
            status = WriteS5B0DatTwo();			//��S5B0���Ƶ�S5B1
            if(status == MI_OK) step ++;
            else Loop = 0;
            break;

        case 3:
            status = WriteFlag();				//����־�����
            if(status == MI_OK) step ++;
            else Loop = 0;
            break;

        default:
            Loop = 0;
            step = 0;
            break;
        }
    }
#if DBG_RC500
    printf("TopUpCardInfor = %d\n",step);
#endif
    if(step >= 1) step = 0;
    return(step);
}
*/


unsigned char Timediff(void)
{
	unsigned char status = 1;
	LongUnon Timea,Timeb,Timec;

	Timea.i = Timeb.i = Timec.i = 0;

	Timea.longbuf[0] = Time.day;
	Timea.longbuf[1] = Time.month;
	Timea.longbuf[2] = Time.year;

	Timeb.longbuf[0] = CardLan.SMonth[2];
	Timeb.longbuf[1] = CardLan.SMonth[1];
	Timeb.longbuf[2] = CardLan.SMonth[0];

	Timec.longbuf[0] = CardLan.EMonth[2];
	Timec.longbuf[1] = CardLan.EMonth[1];
	Timec.longbuf[2] = CardLan.EMonth[0];

	if((Timea.i < Timec.i)&&(Timea.i >= Timeb.i))
	{
		status = 0;
	}
	return status;
}


/*
*************************************************************************************************************
- �������� : unsigned char ReadorRepairCard (HWND hDlg,int mode)
- ����˵�� : �û��������޸���Ƭ
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char YueConsume(void)
{
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char status = 1;


	while(Loop)
	{
		switch(step)
		{
		case 1:
			status = Timediff();
			if(status == MI_OK)
			{
				Sector.FlagValue = 8; // ʱ�����
				step++;
			}
			else
			{
				Err_display(17);
                Err_save(CARD_SPEC_M1_LINUX,17);
				Loop = 0;
			}
			break;
#if 0    //ye mask �޸İ���ʵ��ʱ�����ж�
		case 2:
			step++;  //д���������Σ�
			/*
			     status = TimeWriecarad();
			     if(status == MI_OK)
			      {
			          step++;
			      }
			     else
			       {
			        Loop = 0;
			       }
			*/
			break;
#else
		case 2:
			status = OnlyExchangeTimeWrite();
			status += WriteBlockToS5B2();
			if(!status)
				step++;
			else
				Loop = 0;
			
			break;
#endif
		case 3:
			IncSerId();
			step++;
			break;

		case 4:
			status = SaveCardData(CARD_SPEC_M1_LINUX, CONSUME_MODE_PRESET, GET_RECORD | SAVE_RECORD); //��������  ����
			if(status == MI_OK) step++;
			else
			{
				Err_display(20);
				Loop = 0;
			}
			AutoUpFlag = 0x55;
			break;

		default:
			Loop = 0;
			step = 0;
			break;
		}
	}

#if DBG_RC500
	printf("YueConsume = %d\n",step);
#endif

	return step;
}


/*
*************************************************************************************************************
- �������� : unsigned char ReadorRepairCard (HWND hDlg,int mode)
- ����˵�� : �û��������޸���Ƭ
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char ReadcardSev(void)
{
	int flag,t,ret;
	static int  receive_len[1] = {0};
	//static char receive_buf[20]= {0};
	unsigned char rev_buf1[20]={0}, rev_buf2[20]={0}, rev_buf3[20]={0};
	unsigned char cmpbuf[60];
	unsigned char timemoth,Smoth;
	LongUnon uBuf,uValue;
	unsigned char  bigger=0;
	int i;
	timemoth = BCD2HEX(Time.month);
	if((timemoth>=1)&&(timemoth<=4)) Smoth = 0;
	else if((timemoth>=5)&&(timemoth<=8)) Smoth = 1;
	else if((timemoth>=9)&&(timemoth<=12)) Smoth = 2;
	else return 1;
	CardLan.TransCiFlag = 0;
	memset(CardLan.TimesTransfer,0,4);
	
	flag =1;
	t =1;
	while(flag)
	{
		switch(t)
		{
		case 1:
			ret = ioctl(mf_fd, RC531_AUTHENT,(4*LanSec.Sev+ 3));
			if(ret == MI_OK)t++;
			else flag = 0;
			break;

		case 2:
			ret = ioctl(mf_fd, RC531_READ,(4*LanSec.Sev + 0));
			if(ret == MI_OK)
			{
				ret = ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				ret = read(mf_fd, rev_buf1, receive_len[0]);
				if(ret >= 0)
				{
					t++;
				}
				else
				{
					flag = 0;
				}
			}
			else flag = 0;
			break;
		case 3:
			ret = ioctl(mf_fd, RC531_READ,(4*LanSec.Sev + 1));
			if(ret == MI_OK)
			{
				ret = ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				ret = read(mf_fd, rev_buf2, receive_len[0]);
				if(ret >= 0)
				{
					t++;
				}
				else
				{
					flag = 0;
				}
			}
			else flag = 0;
			break;
			
		case 4:
			ret = ioctl(mf_fd, RC531_READ,(4*LanSec.Sev + 2));
			if(ret == MI_OK)
			{
				ret = ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				ret = read(mf_fd, rev_buf3, receive_len[0]);
				if(ret >= 0)
				{
					t++;
				}
				else
				{
					flag = 0;
				}
			}
			else flag = 0;
			break;
		case 5:
			if (Smoth == 0)
				memcpy(Sector.SSevnBuf, rev_buf1, 16);
			else if (Smoth == 1)
				memcpy(Sector.SSevnBuf, rev_buf2, 16);
			else
				memcpy(Sector.SSevnBuf, rev_buf3, 16);
			
			memset(uValue.longbuf,0,4);
			uValue.longbuf[3] = 0xff;
			uValue.longbuf[1] = timemoth;
			uValue.longbuf[0] = Time.year;
			g_ReadSevIndex = 0;
			if((timemoth == 1)||(timemoth == 5)||(timemoth == 9))
			{
				memcpy(uBuf.longbuf,Sector.SSevnBuf,4);
				memcpy(Sector.SSevnBuf,uValue.longbuf,4);
				g_ReadSevIndex = 0;
			}
			else if((timemoth == 2)||(timemoth == 6)||(timemoth == 10))
			{
				memcpy(uBuf.longbuf,Sector.SSevnBuf+4,4);
				memcpy(Sector.SSevnBuf+4,uValue.longbuf,4);
				g_ReadSevIndex = 4;
			}
			else if((timemoth == 3)||(timemoth == 7)||(timemoth == 11))
			{
				memcpy(uBuf.longbuf,Sector.SSevnBuf+8,4);
				memcpy(Sector.SSevnBuf+8,uValue.longbuf,4);
				g_ReadSevIndex = 8;
			}
			else if((timemoth == 4)||(timemoth == 8)||(timemoth == 12))
			{
				memcpy(uBuf.longbuf,Sector.SSevnBuf+12,4);
				memcpy(Sector.SSevnBuf+12,uValue.longbuf,4);
				g_ReadSevIndex = 12;
			}
			if ((uBuf.longbuf[3] != 0) && (uBuf.longbuf[3] != 0xFF) && (uBuf.longbuf[3] != Time.year))// ���ǽ���Ԥ��ģ�ȡ��Ԥ��
			{
				//flag = 0;
				return 50;
				//break;
			}
			else if (uBuf.longbuf[3] != 0xFF)
				uBuf.longbuf[3] = 0;   //clear the year .......ex. 17 18 19...
			
			if (Smoth == 0)
				memcpy(rev_buf1, Sector.SSevnBuf, 16);
			else if (Smoth == 1)
				memcpy(rev_buf2, Sector.SSevnBuf, 16);
			else
				memcpy(rev_buf3, Sector.SSevnBuf, 16);
			
			memcpy(cmpbuf, rev_buf1, 16);
			memcpy(cmpbuf+16, rev_buf2, 16);
			memcpy(cmpbuf+32, rev_buf3, 16);
			
			for(i=0; i<12; i++)
			{
				if (cmpbuf[i*4+3] != 0xff)  continue;
				if( (cmpbuf[i*4] > Time.year) && cmpbuf[i*4+1])
				{
					bigger = 13; //error set to 13
					break;
				}
				else if (cmpbuf[i*4] < Time.year)
				{
					continue;
				}
				
				 if (cmpbuf[i*4+1] > bigger)
				{
					bigger = cmpbuf[i*4+1];
				}
				//printf("===the 111 = %d\n", cmpbuf[i*4+1]);
			}
			//printf("===the bigger moth = %d, current %d\n", bigger, timemoth);
			if (bigger > timemoth)
			{
				flag = 0;  //�ҵ�����ֵ�ȵ�ǰ���·ݴ󣬲���ˢ��
				CardLan.TransCiFlag = 0;
				//break;
				return 51;
			}
			
			if(uBuf.longbuf[3] == 0xff)
			{
				uValue.longbuf[3] = 0xff;
				uValue.longbuf[1] = timemoth;
				uValue.longbuf[0] = Time.year;
				if(uValue.i == uBuf.i) 
				{
					CardLan.TransCiFlag = 0;
				}
				else {
					CardLan.TransCiFlag = 0;  //haixing gongjiao modify by 20161203 old is 1
					memset(CardLan.TimesTransfer,0,4);
					flag = 0;
					//break; //haixing gongjiao modify by 20161203 goto cishu exchange
					return 52;
				}
				//printf("===CardLan.TransCiFlag =%d  uValue.i =%d  uBuf.i = %d\n" , CardLan.TransCiFlag, uValue.i ,  uBuf.i);
				t++;
			}
			else if((uBuf.i <= 65535) && (uBuf.i > 0))
			{
				memcpy(CardLan.TimesTransfer,uBuf.longbuf,3);
				CardLan.TransCiFlag = 1;
				t++;
			}
			else
			{
				flag = 0;
				return 56;
			}

			break;
		case 6:
			t = 0;
			flag = 0;
			break;

		default :
			flag = 0;
			break;
		}
	}


#if DBG_RC500
	printf("ReadcardSev = %d\n",t);
#endif


	return t;
}

/*
*************************************************************************************************************
- �������� : unsigned char TopUpCardInfor (HWND hDlg)
- ����˵�� : ��ֵ���
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char TopUpCardCi (unsigned char type) // 0 Ϊ�۷�  1Ϊ��ֵ
{
	DebugPrintf("\n");
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char status,ReturnValue;
    struct ErrorRecordLsit pErrorRec;
    time_t NowTime;
#if DBG_RC500
	{
		struct timeval now;

		gettimeofday(&now,0);
		DBG_RC500_PRINTF("TopUpCardInfor() is called, time = %ld\"%06ld.\n", now.tv_sec, now.tv_usec);
	}
#endif

	while(Loop)
	{
		switch (step)
		{
		case 1:
			if(type == 0)
			{
				status = WriteS5B0DatOne(0x03);		//дS5B0��־��ʼ
			}
			else
			{
				status = WriteS5B0DatOne(0x01); 	//дS5B0��־��ʼ
			}
			if(status == MI_OK) step ++;
			else Loop = 0;
			break;

		case 2:
			status = TopUpMokey(type);			// ��Ǯ
			if(status == MI_OK) step ++;
			else Loop = 0;
			break;

		case 3:
			status = WriteS5B0DatTwo(type);	//дS5B0��ɣ���S5B0���Ƶ�S5B1
			if(status == MI_OK) step ++;
			else Loop = 0;
			break;
            
        case 4: //6
			//��������־д�뵽����
			Sector.SOneZero[5] = OPERATION_COMPLETE;
			UseForTest = 0;
			if((ReturnValue = WriteOneSertorDataToCard(Sector.SOneZero, LanSec.One, 0, VERIFY_KEY)) == MI_OK)
			{
				step ++;
				DebugPrintf("[operation success]\n");
			}
			else if(ReturnValue > 0 && ReturnValue < 3)
			{
				printf("д���־���ɹ�ReturnValue = %d\n", ReturnValue);
				Loop = 0;
			}
			else
			{
				ShowSwipeCardStatus(1);

				IncSerId();
				if(SaveCardData(CARD_SPEC_M1_LINUX, CONSUME_MODE_PRESET, GET_RECORD) == MI_OK)
				{
					DebugPrintf("\n");
					memset(&pErrorRec, 0, sizeof(pErrorRec));
					pErrorRec.next = NULL;

					time(&NowTime);
					pErrorRec.RecordTime = NowTime;
					memcpy(pErrorRec.CardNo, CardLan.UserIcNo, 4);
					memcpy(&pErrorRec.SaveRecordBuf, &SaveData, sizeof(RecordFormat));

					//��������������Ļ��ʾ
					memcpy(&pErrorRec.DisplayData.cardlan, &CardLan, sizeof(CardInform));
					memcpy(pErrorRec.SectorOneData, Sector.SOneZero, 16);
	
					pErrorRec.DisplayData.bStolenDis = bStolenDis;
					pErrorRec.DisplayData.FlagValue = Sector.FlagValue;
					pErrorRec.DisplayData.HostValue.i = HostValue.i;
					pErrorRec.DisplayData.StolenAmount = StolenAmount;

					AddOneErrorRecordtoList(&pErrorRec);

					WriteAllErrorRecordToFile();
					UseForTest = 0;
					printf("return SWIPE_CARD_ERROR\n");
					return SWIPE_CARD_ERROR;
					
				}
				else
				{
					DevSID.i--;		/* ��������ʧ����Ҫ�ڱ���һ�� */
					DebugPrintf("save data failed \n");
				}

				

			}
			UseForTest = 0;
			break;            

		case 5:
			status = CopySXB1ToSXB2();	//Ǯ�� ����
			if(status == MI_OK) step ++;
			else Loop = 0;
			break;
            
        case 6:
			Sector.SOneZero[5] = BACKUP_COMPLETE;
			if(WriteOneSertorDataToCard(Sector.SOneZero, LanSec.One, 0, VERIFY_KEY) == MI_OK)
				step ++;
			else
				Loop = 0;
			break;
            

		case 7:
			step = 0;
			Loop = 0;
			break;

		default:
			Loop = 0;
			break;
		}
	}

#if DBG_RC500
	printf("TopUpCardCi = %d \n",step);
#endif
	// transaction is regarded as successful if the electronic purse is changed.
	if(step >= 3) step = 0;
	return(step);
}

/*
*************************************************************************************************************
- �������� : unsigned char ReadorRepairCard (HWND hDlg,int mode)
- ����˵�� : �û��������޸���Ƭ
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char CiConsume(void)
{
	DebugPrintf("\n");
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char status = 1;
	LongUnon bValue,aValue,Buf,OldValue;
	unsigned char timemoth;
	timemoth = BCD2HEX(Time.month);
	unsigned char testcmp[4] = {Time.year, timemoth, 0x00, 0xff};

	while(Loop)
	{
		switch(step)
		{
		case 1:
			status = ReadcardSev();
			if(status == MI_OK)
			{
				step++;
			}
			else
			{
				if (g_FgOprationSuccess)   //�������Ϊ:�������Ѿ�д�ɹ������ǵ�������û��д�ɹ�����Ҳ����ԭ��Ԥ�������
				{
					step = 4;
					g_FgOprationSuccess = 2;
					memcpy(Sector.SSevnBuf+g_ReadSevIndex, testcmp, 4);
				}
				else
				{
					return status;
					//Loop = 0;
				}
			}
			break;

		case 2:
			Sector.FlagValue = 0; // ѡ�����Ǯ��
			DecValue.i = HostValue.i=1;
			//printf("==old = %d, this =%d \n", CardLan.OldTime[1], Time.month);
			//printf("===ddd %x, %x, %x, %x\n", Sector.SSevnBuf[0+g_ReadSevIndex], Sector.SSevnBuf[1+g_ReadSevIndex], Sector.SSevnBuf[2+g_ReadSevIndex], Sector.SSevnBuf[3+g_ReadSevIndex]);
			//printf("===ccc %x, %x, %x, %x\n", testcmp[0], testcmp[1], testcmp[2], testcmp[3]);
			if (memcmp(Sector.SSevnBuf+g_ReadSevIndex, testcmp, 4))
			{
				step++;
			}
			else
			{
				step = 4;
			}
			break;

		case 3:
			status = YesOrNoCiShu();
			if(status == MI_OK)
			{
				step++;
			}
			else
			{
				Loop = 0;
			}
			break;
		case 4:
			memcpy(aValue.longbuf,CardLan.Views,4);
			memcpy(bValue.longbuf,CardLan.TimesTransfer,3);
			//printf("===ye====Views %d, Transfer %d, ci %d\n", aValue.i, bValue.i, CardLan.TransCiFlag);
			
			if(CardLan.TransCiFlag != 0)
			{
				if(bValue.i != 0)
				{
					#if 1
					DecValue.i = HostValue.i=1;
					memcpy(Sector.STwoOne, bValue.longbuf,4);
					status = TopUpCardInfor(0);
					if(status == MI_OK)
					{
						memcpy(CardLan.Views, bValue.longbuf,4);
						step++;
					}
					else
					{
						Loop = 0;
					}
					#else
					if(aValue.i > bValue.i) {

						HostValue.i = aValue.i - bValue.i -1;
						DecValue.i = HostValue.i ;
						status = TopUpCardInfor(0);
						if(status == MI_OK)
						{
							memcpy(CardLan.Views,bValue.longbuf,4);
							DecValue.i = HostValue.i =1 ;
							step++;
						}
						else Loop = 0;

					}
					else
					{
						HostValue.i = bValue.i - aValue.i -1 ;
						DecValue.i = HostValue.i ;
						status = TopUpCardInfor(1);
						if(status == MI_OK)
						{
							memcpy(CardLan.Views,bValue.longbuf,4);
							DecValue.i = HostValue.i =1 ;
							step++;
						}
						else Loop = 0;
					}
					#endif
				}
				else if((aValue.i != 0)&&(bValue.i == 0))
				{
					DecValue.i = HostValue.i = aValue.i;
					status = TopUpCardCi(0);
					if(status == MI_OK)
					{
						IncSerId();
						Sector.FlagValue = 10; // ʱ�����
						status = SaveCardData(CARD_SPEC_M1_LINUX, CONSUME_MODE_PRESET, GET_RECORD | SAVE_RECORD); //��������  ����
					}
                    			else
					{
			                        Loop = 0;                        
					}
				}
				else
				{   
                   			// Err_display(12);
					//Loop = 0;
					return 55;
				}
			}
			else
			{
				if(aValue.i > 0)
				{
					DecValue.i = HostValue.i=1;
					status = TopUpCardInfor(0);
					if(status == MI_OK)
					{
						step++;
					}
					else 
					{
						//Loop = 0;
						return status;
					}
				}
				else
				{
					//Loop = 0;
					return 55;
				}

			}
			break;
		case 5:
			IncSerId();
			BackupCurrentWallet(0);
			step++;
			break;

		case 6:
			Sector.FlagValue = 9; // ʱ�����
			status = SaveCardData(CARD_SPEC_M1_LINUX, CONSUME_MODE_PRESET, GET_RECORD | SAVE_RECORD); //��������  ����
			if(status == MI_OK) step++;
			else
			{
				Err_display(20);
				Loop = 0;
			}
			AutoUpFlag = 0x55;
			break;

		case 7:
			Loop = 0;
			step = 0;
			break;

		default:
			Loop = 0;
			break;
		}
	}

//#if DBG_RC500
	DebugPrintf("CiConsume = %d\n",step);
//#endif

	return step;
}

unsigned char CheakSubsidyIsValid(void)
{
	DebugPrintf("\n");
	time_t SubsidyValidTime, Now;
	time(&Now);

	memcpy(&SubsidyValidTime, Sector.SFivTwo+10, 4);
	
	DebugPrintf("Now = %d SubsidyValidTime = %d\n", Now, SubsidyValidTime);
	
	if(SubsidyValidTime > Now)
		return MI_OK;
	else 
		return MI_FAIL;
}

/*
*************************************************************************************************************
- �������� : unsigned char ReadorRepairCard (HWND hDlg,int mode)
- ����˵�� : �û��������޸���Ƭ
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char  ReadorRepairCard (void)
{
	DebugPrintf("\n");
	unsigned char Loop = 1,step = 1, status, FlagM = 0;
	struct timeval first, second;
	LongUnon TempMoney;

	struct timeval start, end;
	
	DBG_RC500_PRINTF("ReadorRepairCard() is called.\n");

	BFlaglessfive = 0;
	while(Loop)
	{
		DebugPrintf("step = 0x%02X\n", step);
		switch (step)
		{
		case 1:
#ifdef QINGDAO_TONGRUAN
			g_FindTheDiscount = 0;
#endif
			if((status = RepairCardInfor()) == MI_OK) 
			{
#ifdef SHAOYANG_PARK

				memcpy(Test.longbuf,CardLan.Subsidies,4);
				memcpy(Buf.longbuf,CardLan.QCash,4);

				DebugPrintf("Test.i = %u  Buf.i = %u\n",Test.i, Buf.i);
				
				/* ������1000Ԫ��ʱ�򣬲���ˢ */
				if((Test.i > 1000*100) || (Buf.i > 1000*100))
				{
					Err_display(7);
                    			Err_save(CARD_SPEC_M1_LINUX,7);
					Loop = 0;
				}
				else
					step++;

#else
				step++;
#endif
			}
			else if(status == 0xFF){
				step = 0xFE;				//ֱ�ӵ����
				ShowSwipeCardStatus(2);
			}else
				Loop = 0;

			
			break;

		case 2:
			if(Permissions(0) == MI_OK) 	//Ȩ��
			{
				step ++;
			}
			else 
				Loop = 0;
			break;

		case 3:
			
			if(Fixvalue.i!=0){
				HostValue.i=DecValue.i=Fixvalue.i;
			}
			else
				HostValue.i = DecValue.i = 0;
#if HAIXING_BUS
			CardLan.MonthOrCountflag = 2;
#endif
			CardLan.TransCiFlag = 0;  //clear the flag
			//printf(" == [ye]==MonthOrCountflag = %d, other = %d\n", CardLan.MonthOrCountflag, g_FlagMonCard);
#if 0//def  XIANGXIANG_BUS
            CardLan.MonthOrCountflag = 0;
#endif
			if(CardLan.MonthOrCountflag != 0 || g_FlagMonCard == 2)
			{
				if(CardLan.MonthOrCountflag == 1/* || g_FlagMonCard ==1*/)
				{
					if(YueConsume() == MI_OK){
						step = 0;
						ShowSwipeCardStatus(2);
					}else 
						Loop = 0;
				}
				else if(CardLan.MonthOrCountflag == 2 || g_FlagMonCard == 2)
				{
					if((status = CiConsume()) == MI_OK){ 
						step = 0;
						ShowSwipeCardStatus(2);
					}else {
						if(status == SWIPE_CARD_ERROR)
							return SWIPE_CARD_ERROR;
						else
						{
							if (status < 50)
							{
								Err_display(24);
								Loop = 0;
							}
							else// if (status == 55)
							{
								//Err_display(12);
								//Loop = 0;
								step +=2;
							}
						}
					}
				}
				else 
					step ++;
			}
			else
			{
				step ++;
			}
			break;

		case 4:	//�۴�
#if((defined QINGDAO_TONGRUAN))//#ifdef QINGDAO_TONGRUAN
			g_DisplayMoney.i = 0;
			YesOrNoCiShu();
			
			status = GetDiscountFromPara(CardLan.CardType);
			//printf("GetDiscount status = %d\n", status);
			if (!status)
			{
				g_FindTheDiscount = 1;
			}
			else
			{
				g_FindTheDiscount = 0;
			}
			step++;
#else //û�д�������
			memcpy(JackArm.longbuf,CardLan.Views,4);
			DebugPrintf("===JackArm.i = 0x%02X\n", JackArm.i);			
			if(JackArm.i > 0)
			{
				HostValue.i = DecValue.i = 1;
				Sector.FlagValue = 0;
				if(YesOrNoCiShu() != 0)
					step++;
				else
					step = 8;
				
			}
			else 
				step++;
#endif
			break;

		case 5:
			if(Fixvalue.i==0){
				if(AnalysisSheet(1) == MI_OK) //�����ѱ�
					step++;
				else
				{
					Err_display(16);
	                			Err_save(CARD_SPEC_M1_LINUX,16);
					ioctl(mf_fd, RC531_HALT);
					Loop = 0;
				}

			}
			else{
				HostValue.i=DecValue.i=Fixvalue.i;
				if(AnalysisSheet(0) == MI_OK) //�����ѱ�
					step++;
				else
				{
					Err_display(16);
	                			Err_save(CARD_SPEC_M1_LINUX,16);
					ioctl(mf_fd, RC531_HALT);
					Loop = 0;
				}
			}
			break;

		case 6:								//���ֽ�
#if RUSHAN_BUS
			if(CardLan.CardType==0x06)
			{
				HostValue.i = DecValue.i = 0;
			}
#endif	
#ifdef PUNINGWANCHENG
            if(CardLan.CardType==0x02)
                HostValue.i = HostValue.i - 100;
#endif     
			memcpy(Test.longbuf,CardLan.Subsidies,4);
			memcpy(Buf.longbuf,CardLan.QCash,4);
#if NINGXIA_TONGXIN //ֻ�����ֽ�Ǯ��
			if(Buf.i >= HostValue.i)
			{
				if(Buf.i <= HostValue.i*5) 
					BFlaglessfive = 1;
				
				Sector.FlagValue = 2;
				step++;
			}
			else
			{
				Err_display(12);
               	Err_save(CARD_SPEC_M1_LINUX,12);
				ioctl(mf_fd, RC531_HALT);
				Loop = 0;
			}
#else
			if((Test.i >= HostValue.i)&&(Test.i !=0 ))
			{
				Sector.FlagValue = 1;
				step++;
			}
			else if(Buf.i >= HostValue.i)
			{
				if(Buf.i <= HostValue.i*5) 
					BFlaglessfive = 1;
				
				Sector.FlagValue = 2;
				step++;
			}
			else
			{
				Err_display(12);
               	Err_save(CARD_SPEC_M1_LINUX,12);
				ioctl(mf_fd, RC531_HALT);
				Loop = 0;
			}
#endif
			break;

		case 7:										//���ֽ�
			status = YesOrNoMoney();
			if(status == MI_OK) step ++;
			else if(status == 0x01)					//����ʱ�����ѽ��
			{
				Err_display(13);
                Err_save(CARD_SPEC_M1_LINUX,13);
				ioctl(mf_fd, RC531_HALT);
				Loop = 0;
			}
			else if(status == 0x02)					//�����������ѽ��
			{
				Err_display(14);
                Err_save(CARD_SPEC_M1_LINUX,14);
				ioctl(mf_fd, RC531_HALT);
				Loop = 0;
			}
			else									//�����������ѽ��
			{
				Err_display(15);
                Err_save(CARD_SPEC_M1_LINUX,15);
				ioctl(mf_fd, RC531_HALT);
				Loop = 0;
			}	
			break;

		case 8:
			if((status = TopUpCardInfor(0)) == MI_OK)
				step++;
			else if(status == SWIPE_CARD_ERROR)
				return SWIPE_CARD_ERROR;
			else 
				Loop = 0;
			break;
		case 9:
			BackupCurrentWallet(0);
			ShowSwipeCardStatus(2);
			step++;
			break;

		case 10:
			IncSerId();
#ifdef Transport_Stander
            status = SaveCardData_jiaotong(CARD_SPEC_M1_LINUX, CONSUME_MODE_PRESET, GET_RECORD | SAVE_RECORD); //�������� �ֶ�
            
#else        
            #ifdef CANGNAN_BUS
            status = SaveCardData_Zhujian(CARD_SPEC_M1_LINUX, CONSUME_MODE_PRESET, GET_RECORD | SAVE_RECORD); //�������� �ֶ�
            #else
			status = SaveCardData(CARD_SPEC_M1_LINUX, CONSUME_MODE_PRESET, GET_RECORD | SAVE_RECORD); //�������� �ֶ�
			#endif
#endif	     
            if(status == MI_OK) 
				step ++;
			else
			{
				Err_display(20);
				Loop = 0;
			}
            g_FgNoDiscount = 0;
			AutoUpFlag = 0x55;
			break;
			
		default:
			step = 0;
			Loop = 0;
			break;
		}
	}
	
	memcpy(TempMoney.longbuf, CardLan.QCash, 4);

	DebugPrintf("###############################[%02X] [zero: %02X %02X one: %02X %02X] [%u]\n", \
			step, Sector.SFivZero[1], Sector.SFivZero[2],Sector.SFivOne[1], Sector.SFivOne[2], TempMoney.i);

	ioctl(mf_fd, RC531_HALT);


#if DBG_RC500
	printf("ReadorRepairCard = %d\n",step);
#endif

	return(step);
}


/*
*************************************************************************************************************
- �������� : unsigned char ReadorRepairCard (HWND hDlg,int mode)
- ����˵�� : �û��������޸���Ƭ
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char FreeReadorRepairCard(void)
{
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char status = 1;

	DBG_RC500_PRINTF("FreeReadorRepairCard() is called.\n");

	BFlaglessfive = 0;
	while(Loop)
	{
		switch (step)
		{
		case 1:								//����
			if(FindCardInListAndReadSectorData() == MI_OK)
				step ++;
			else
				Loop = 0;
#if 0
			status = ReadCardInfor();
			if(status == MI_OK) step ++;
			else
			{
				Loop = 0;
			}
#endif
			break;

		case 2:
			if((status = RepairCardInfor()) == MI_OK) 
				step++;
			else if(status == 0xFF){
				step = 0xFE;				//ֱ�ӵ����
				ShowSwipeCardStatus(2);
			}else
				Loop = 0;
			break;

#if 0			
			status = RepairCardInfor();				//�޸�
			if(status == MI_OK) step++;
			else Loop = 0;
			break;
#endif
		case 3:
			if(Permissions(0) == MI_OK) 	//Ȩ��
				step ++;
			else 
				Loop = 0;
			break;

		case 4:
			
#ifdef FOSHAN_HUAYUE 
			HostValue.i = FValue.i;
			step++;

#else
			if(AnalysisSheet(0) == MI_OK) //�����ѱ�
				step++;
			else
			{
				Err_display(16);
                Err_save(CARD_SPEC_M1_LINUX,16);
				ioctl(mf_fd, RC531_HALT);
				Loop = 0;
			}
#endif
			break;

		case 5:								//���ֽ�
			memcpy(Test.longbuf,CardLan.Subsidies,4);
			memcpy(Buf.longbuf,CardLan.QCash,4);
			#if NINGXIA_TONGXIN //ֻ�����ֽ�Ǯ��
			if(Buf.i >= HostValue.i)
			{
				if(Buf.i <= HostValue.i*5) BFlaglessfive = 1;
				Sector.FlagValue = 2;
				step++;
			}
			else
			{
				Err_display(12);
                Err_save(CARD_SPEC_M1_LINUX,12);
				ioctl(mf_fd, RC531_HALT);
				Loop = 0;
			}
			#else
			if((Test.i >= HostValue.i)&&(Test.i !=0 ))
			{
				Sector.FlagValue = 1;
				step++;
			}
			else if(Buf.i >= HostValue.i)
			{
				if(Buf.i <= HostValue.i*5) BFlaglessfive = 1;
				Sector.FlagValue = 2;
				step++;
			}
			else
			{
				Err_display(12);
                Err_save(CARD_SPEC_M1_LINUX,12);
				ioctl(mf_fd, RC531_HALT);
				Loop = 0;
			}
			#endif
			break;

		case 6:								//���ֽ�
			status = YesOrNoMoney();
			if(status == MI_OK) step ++;
			else if(status == 0x01)					//����ʱ�����ѽ��
			{
				Err_display(13);
                Err_save(CARD_SPEC_M1_LINUX,13);
				ioctl(mf_fd, RC531_HALT);
				Loop = 0;
			}
			else if(status == 0x02)					//�����������ѽ��
			{
				Err_display(14);
                Err_save(CARD_SPEC_M1_LINUX,14);
				ioctl(mf_fd, RC531_HALT);
				Loop = 0;
			}
			else							//�����������ѽ��
			{
				Err_display(15);
                Err_save(CARD_SPEC_M1_LINUX,15);
				ioctl(mf_fd, RC531_HALT);
				Loop = 0;
			}
			break;

		case 7:
			if((status = TopUpCardInfor(0)) == MI_OK)
				step++;
			else if(status == SWIPE_CARD_ERROR)
				return SWIPE_CARD_ERROR;
			else 
				Loop = 0;
			break;

		case 8:
			BackupCurrentWallet(0);
			ShowSwipeCardStatus(2);
			step++;
			break;

		case 9:
			IncSerId();
			status = SaveCardData(CARD_SPEC_M1_LINUX, CONSUME_MODE_FLEXIBLE, GET_RECORD | SAVE_RECORD); //��������  ����
			if(status == MI_OK) 
				step ++;
			else
			{
				Err_display(20);
				Loop = 0;
			}
			AutoUpFlag = 0x55;
			break;
#if 0
		case 8:
			status = SaveCardData(CARD_SPEC_M1_LINUX, CONSUME_MODE_FLEXIBLE, GET_RECORD | SAVE_RECORD); //�������� ����
			if(status == MI_OK) step ++;
			else
			{
				Err_display(20);
				Loop = 0;
			}
			AutoUpFlag = 0x55;
			break;
#endif
		default:
			step = 0;
			Loop = 0;
			break;
		}
	}

	ioctl(mf_fd, RC531_HALT);

#if DBG_RC500
	printf("FreeReadorRepairCard = %d\n",step);
#endif
	return(step);
}



unsigned char ExitCarProgram(void)
{
	DebugPrintf("\n");
	unsigned char step = 1;
	unsigned char Loop = 1;
	unsigned char status;
	ShortUnon Bufs;

	DBG_RC500_PRINTF("ExitCarProgram() is called.\n");

	while(Loop)
	{
		switch(step)
		{
		case 1:
			status = SectionSheet();
			if(status == MI_OK) step++;
			else if(status == 1)
			{
				Err_display(16);
                Err_save(CARD_SPEC_M1_LINUX,16);
				Loop = 0;
			}
			else
			{
				Err_display(50);  //û���ϳ�
				Err_save(CARD_SPEC_M1_LINUX,50);
				Loop = 0;
			}
			break;

		case 2:
			//  status = AnalysisSheet(0);				//�����ѱ�
			// if(status == MI_OK) step ++;
			//else
			//{
			//    Err_display(16);
			//    Loop = 0;
			//}
			HostValue.i = SectionDiscountRate(HostValue.i,CardLan.CardType);
			DebugPrintf("HostValue.i = 0x%02X\n", HostValue.i);
			step ++;
			break;

		case 3:
			DebugPrintf("Section.Updoor = 0x%02X\n", Section.Updoor);
			if(Section.Updoor == 0x02)
			{
				if((CardLan.EnterCarCi == 0)||(CardLan.EnterCarCi >= 100)) CardLan.EnterCarCi = 1;

				HostValue.i = (unsigned int)( HostValue.i * CardLan.EnterCarCi);
				DecValue.i = HostValue.i;

				if(CardLan.StationOn)
				{
					memcpy(Buf.longbuf,Sectionup.DeductMoney,4);	//Ԥ�۽�� �ֶ�
				}
				else
				{
					memcpy(Buf.longbuf,Section.DeductMoney,4);	//Ԥ�۽�� �ֶ�
				}

				// memcpy(Buf.longbuf,Section.DeductMoney,4); //

			}
			else
			{
				CardLan.EnterCarCi = 1;
				DecValue.i = HostValue.i;
				//memcpy(Buf.longbuf,Section.DeductMoney,4); //

				if(CardLan.StationOn)
				{
					memcpy(Buf.longbuf,Sectionup.DeductMoney,4);	//Ԥ�۽�� �ֶ�
				}
				else
				{
					memcpy(Buf.longbuf,Section.DeductMoney,4);	//Ԥ�۽�� �ֶ�
				}
			}

			Buf.i = SectionDiscountRate(Buf.i,CardLan.CardType);

			Buf.i = (unsigned int)(Buf.i * CardLan.EnterCarCi);
#ifdef PUNINGWANCHENG
            if(CardLan.CardType==0x02)
                HostValue.i = HostValue.i - 100;
#endif                
			if(Buf.i >= HostValue.i)									//AA��ֵ
			{
				HostValue.i = Buf.i - HostValue.i;
				CardLan.StationDEC = 0xAA;
			}
			else														//55��ֵ
			{
				HostValue.i = HostValue.i - Buf.i;
				CardLan.StationDEC = 0x55;
			}
			memset(Bufs.intbuf,0,sizeof(ShortUnon));

			if(CardTypeIC == 0x08)
			{
				memcpy(Bufs.intbuf,Sector.SFivZero+1,2);
				Bufs.i--;
				memcpy(Sector.SFivZero+1,Bufs.intbuf,2);
			}
			else
			{
				memcpy(Bufs.intbuf,CardLan.ViewMoney,2);
				Bufs.i--;
				memcpy(CardLan.ViewMoney,Bufs.intbuf,2);						
			}			

			CardLan.StationID= 0;
			CardLan.EnterCarCi = 0;
			CardLan.EnterExitFlag = 0;
			step++;
			break;

		default :
			step =0;
			Loop =0;
			break;
		}
	}
	DebugPrintf("CardLan.EnterExitFlag = 0x%02X\n", CardLan.EnterExitFlag);
	DebugPrintf("step = %d\n",step);
#if  DBG_RC500
	printf("ExitCarProgram = %d\n",step);
#endif


	return step;
}


unsigned char EnterCarProgram(void)
{
	DebugPrintf("\n");
	unsigned char step = 1;
	unsigned char Loop = 1;
	unsigned char status;
	ShortUnon Bufs;

#if DBG_RC500
	{
		struct timeval now;

		gettimeofday(&now,0);
		DBG_RC500_PRINTF("EnterCarProgram() is called, time = %ld\"%06ld.\n", now.tv_sec, now.tv_usec);
	}
#endif

	while(Loop)
	{
		switch(step)
		{
		case 1:
			if(Section.Updown)			//  1Ϊ����	
			{
				memcpy(HostValue.longbuf,Sectionup.DeductMoney,4);	//Ԥ�۽�� �ֶ�
			}
			else	// 0Ϊ����
			{
				memcpy(HostValue.longbuf,Section.DeductMoney,4);	//Ԥ�۽�� �ֶ�
			}
			HostValue.i = SectionDiscountRate(HostValue.i,CardLan.CardType);
			DebugPrintf("HostValue.i = 0x%02X\n", HostValue.i);
			step++;
			break;

		case 2:
			memset(Buf.longbuf,0,4);
			memcpy(Buf.longbuf,CardLan.OldTermNo,4);

			DebugPrintf("Buf.i = 0x%02X DevNum.i 0x%02X\n",Buf.i, DevNum.i);
			DebugPrintf("Section.SationNow = 0x%02X CardLan.StationID = 0x%02X\n",Section.SationNow, CardLan.StationID);
			DebugPrintf("CardLan.EnterExitFlag = 0x%02X\n",CardLan.EnterExitFlag);
			DebugPrintf("Section.Updoor = 0x%02X\n",Section.Updoor);
			DebugPrintf("Section.Updown = 0x%02X  CardLan.StationOn = 0x%02X\n",Section.Updown,CardLan.StationOn);
			
			if((Buf.i == DevNum.i)&&(Section.SationNow == CardLan.StationID)&&(CardLan.EnterExitFlag == 0x55)&&(Section.Updoor == 0x01)&&(Section.Updown == CardLan.StationOn))
			{
				status = OverTimeEnter();
				if(status != 0)
				{
					if(CardLan.EnterCarCi >= 100) CardLan.EnterCarCi = 0;
					CardLan.EnterCarCi ++;
					CardLan.EnterExitFlag = 0x55;

					memset(Bufs.intbuf,0,sizeof(ShortUnon));
					if(CardTypeIC == 0x08)
					{
						memcpy(Bufs.intbuf,Sector.SFivZero+1,2);
						Bufs.i--;
						memcpy(Sector.SFivZero+1,Bufs.intbuf,2);
					}
					else
					{
						memcpy(Bufs.intbuf,CardLan.ViewMoney,2);
						Bufs.i--;
						memcpy(CardLan.ViewMoney,Bufs.intbuf,2);						
					}

				}
				else
				{
					CardLan.EnterCarCi = 1;
					CardLan.EnterExitFlag = 0x55;
				}
			}
			else
			{
				CardLan.EnterCarCi = 1;
				CardLan.EnterExitFlag = 0x55;
				CardLan.StationID = Section.SationNow;
			}
			
			step++;
			break;

		default :
			step = 0;
			Loop = 0;
			break;
		}
	}

	return step;
}

//SectionSheet
unsigned char SectionFares(void)
{
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char status = 1;

#if DBG_RC500
	{
		struct timeval now;

		gettimeofday(&now,0);
		DBG_RC500_PRINTF("SectionFares() is called, time = %ld\"%06ld.\n", now.tv_sec, now.tv_usec);
	}
#endif
	CardLan.StationDEC  = 0x55;

	while(Loop)
	{
		switch (step)
		{
		case 1:
			if((status = RepairCardInfor()) == MI_OK) 	//�޸�
				step++;
			else if(status == 0xFF){
				step = 0xFE;		//ֱ�ӵ����
				ShowSwipeCardStatus(3);
			}else
				Loop = 0;
			break;

		case 2:
			if(Permissions(2) == MI_OK) 	//Ȩ��
				step++;
			else 
				Loop = 0;
			break;

		case 3:
			HostValue.i = DecValue.i = 0;
			DebugPrintf("Section.Updoor = 0x%02X\n", Section.Updoor);
			switch(Section.Updoor)
			{
			case 1:									//ǰ��
				if(EnterCarProgram() == MI_OK)
					step ++;
				else
					Loop = 0;
				break;

			case 2:									//����
				DebugPrintf("CardLan.EnterExitFlag = 0x%02X\n", CardLan.EnterExitFlag );
				DebugPrintf("SectionNum		: 0x%02X\n", SectionNum);
				if(CardLan.EnterExitFlag == 0x55) 							  //����
				{
					if(OverTimeEnter() != 0)								  //��־�����ڳ����������ǵ��ϴ�û���³����󣬳������ʱ��
					{	//���ǵ���������,�ն˻��Ų��Ե���������
						if(((CardLan.StationID<= SectionNum)&&(CardLan.StationID != 0))||((Section.SationNow == 1)&&(CardLan.StationID!=0)))
						{
							if(ExitCarProgram() == MI_OK)
							{
								step ++;
							}
							else
							{
								Loop = 0;
							}
						}
						else
						{
							Err_display(50);  //û���ϳ�
							Err_save(CARD_SPEC_M1_LINUX,50);
							Loop = 0;
						}
					}
					else
					{
						Err_display(50);  //û���ϳ�
						Err_save(CARD_SPEC_M1_LINUX,50);
						Loop = 0;
					}
				}
				else
				{
					if(memcmp(DevNum.longbuf,CardLan.OldTermNo,4) == 0)
					{
						Err_display(51);  //�Ѿ��³�
						Err_save(CARD_SPEC_M1_LINUX,51);
					}
					else
					{
						Err_display(50);  //û���ϳ�
						Err_save(CARD_SPEC_M1_LINUX,50);
					}
					Loop = 0;
				}
				break;

			default:
				DebugPrintf("CardLan.EnterExitFlag = 0x%02X\n", CardLan.EnterExitFlag );
				if(CardLan.EnterExitFlag == 0x55)								//����
				{
					if(OverTimeEnter() != 0)									//��־�����ڳ����������ǵ��ϴ�û���³����󣬳������ʱ��
					{	//���ǵ���������,�ն˻��Ų��Ե���������
						DebugPrintf("CardLan.StationID = %u SectionNum = %u\n", CardLan.StationID, SectionNum);
						DebugPrintf("Section.SationNow = %u\n", Section.SationNow);
						
						if(((CardLan.StationID<= SectionNum)&&(CardLan.StationID != 0))||((Section.SationNow == 1)&&(CardLan.StationID!=0)))
						{
							if(ExitCarProgram() == MI_OK)
								step ++;
							else
								Loop = 0;
						}
						else
						{
							if(EnterCarProgram() == MI_OK)
								step ++;
							else
								Loop = 0;
						}
					}
					else
					{
						if(EnterCarProgram() == MI_OK)
							step ++;
						else
							Loop = 0;
					}
				}
				else
				{
					if(EnterCarProgram() == MI_OK)
						step ++;
					else
						Loop = 0;
				}
				break;
			}
			break;


		case 4: 							//���ֽ�
			memcpy(Test.longbuf,CardLan.Subsidies,4);
			memcpy(Buf.longbuf,CardLan.QCash,4);
			DebugPrintf("Test.i = 0x%02X\n", Test.i);
			DebugPrintf("HostValue.i = 0x%02X\n", HostValue.i);
			DebugPrintf("Buf.i = 0x%02X\n", Buf.i);
			DebugPrintf("CardLan.EnterExitFlag = 0x%02X\n", CardLan.EnterExitFlag );

			if(CardLan.StationDEC == 0x55)
			{
				if((Test.i >= HostValue.i)&&(Test.i !=0 ))
				{
					Sector.FlagValue = 1;
					step++;
				}
				else if(Buf.i >= HostValue.i)
				{
					if(Buf.i <= HostValue.i*5) BFlaglessfive = 1;
					Sector.FlagValue = 2;
					step++;
				}
				else
				{
					Err_display(12);
                    Err_save(CARD_SPEC_M1_LINUX,12);
					ioctl(mf_fd, RC531_HALT);
					Loop = 0;
				}

			}
			else
			{
				if(CardLan.OldTransType == 0x10)
				{
					Sector.FlagValue = 0;
				}
				else if(CardLan.OldTransType == 0x20)
				{
					Sector.FlagValue = 1;
				}
				else
				{
					Sector.FlagValue = 2;
				}
				step++;
			}
			break;

		case 5: 							//���ֽ�
			DebugPrintf("CardLan.EnterExitFlag = 0x%02X\n", CardLan.EnterExitFlag );

			if(CardLan.StationDEC == 0x55)
			{
				if(YesOrNoMoney() == MI_OK) 
					step ++;
				else if(status == 0x01) 				//����ʱ�����ѽ��
				{
					Err_display(13);
                    Err_save(CARD_SPEC_M1_LINUX,13);
					ioctl(mf_fd, RC531_HALT);
					Loop = 0;
				}
				else if(status == 0x02) 				//�����������ѽ��
				{
					Err_display(14);
                    Err_save(CARD_SPEC_M1_LINUX,14);
					ioctl(mf_fd, RC531_HALT);
					Loop = 0;
				}
				else							//�����������ѽ��
				{
					Err_display(15);
                    Err_save(CARD_SPEC_M1_LINUX,15);
					ioctl(mf_fd, RC531_HALT);
					Loop = 0;
				}
			}
			else
			{
				step++;
			}
			break;

		case 6:
			if(CardLan.StationDEC ==0x55)
				status = TopUpCardInfor(0);
			else
				status = TopUpCardInfor(1);

			if(status == MI_OK)
			{
				step++;
			}
			else if(status == SWIPE_CARD_ERROR)
			{
				return SWIPE_CARD_ERROR;
			}
			else 
				Loop = 0;
			DebugPrintf("Sector.SFivZero[3] = 0x%02X\n", Sector.SFivZero[3]);

			break;

		case 7:
			if(CardLan.StationDEC ==0x55)
				BackupCurrentWallet(0);
			else
				BackupCurrentWallet(1);
			//DebugPrintf("1 ########################\n");
			ShowSwipeCardStatus(3);
			//DebugPrintf("2 ########################\n");
			step++;
			break;
		case 8:
			
			IncSerId();
#ifdef Transport_Stander
            status = SaveCardData_jiaotong(CARD_SPEC_M1_LINUX, CONSUME_MODE_SECTIONAL, GET_RECORD | SAVE_RECORD); //�������� �ֶ�
#else        
			status = SaveCardData(CARD_SPEC_M1_LINUX, CONSUME_MODE_SECTIONAL, GET_RECORD | SAVE_RECORD); //�������� �ֶ�
#endif			
			if(status == MI_OK) 
				step ++;
			else
			{
				Err_display(20);
				Loop = 0;
			}
			AutoUpFlag = 0x55;
			break;
		default:
			step = 0;
			Loop = 0;
			break;
		}
	}

	ioctl(mf_fd, RC531_HALT);
	printf("SectionFares = %d\n",step);

#if DBG_RC500
	printf("SectionFares = %d\n",step);
#endif
	return step;
}


#endif





//#elif  (defined(CONFIG_LINUXBUS8BIT) || defined(Stander_Compatible_8Bit))





int complexDateDeal()
{

	unsigned char Loop = 1;
	unsigned char ret = 1;
	unsigned char status = 1;
	LongUnon Buf,Buf2;
	int true_money;


	CardLan.StationDEC  = 0x55;

	DBG_PRINTF("Section.Updoor %d\n",Section.Updoor);
	HostValue.i = DecValue.i = 0;

	switch(Section.Updoor)
	{
	case 1:

		status = EnterCarProgram();
		if(status == MI_OK) {
			ret = 0;
		}
		else {
			ret = -1;
		}

		memcpy(Buf.longbuf,CardLan.QCash,4);
		if(Buf.i < HostValue.i) {
			Err_display(12);  //����
			Err_save(CARD_SPEC_M1_LINUX,12);
			ret = -1;
			break;
		}

		break;

	case 2:
		if(CardLan.EnterExitFlag == 0x55) 							  //����
		{
			if(OverTimeEnter() != 0)
			{
				if(((CardLan.StationID<= SectionNum)&&(CardLan.StationID != 0))||((Section.SationNow == 1)&&(CardLan.StationID!=0)))
				{
					status = ExitCarProgram();
					if(status == MI_OK)
					{
						ret = 0;
					}
					else
					{
						ret = -1;
					}
				}
				else
				{
					Err_display(50);  //û���ϳ�
					Err_save(CARD_SPEC_M1_LINUX,50);
					ret = -1;
				}
			}
			else
			{
				Err_display(50);  //û���ϳ�
				Err_save(CARD_SPEC_M1_LINUX,50);
				ret = -1;

			}
		}
		else
		{
			if(memcmp(DevNum.longbuf,CardLan.OldTermNo,4) == 0)
			{
				Err_display(51);  //�Ѿ��³�
				Err_save(CARD_SPEC_M1_LINUX,51);
			}
			else
			{
				Err_display(50);  //û���ϳ�
				Err_save(CARD_SPEC_M1_LINUX,50);
			}
			ret = -1;
		}
		break;

	default:

		DBG_PRINTF("CardLan.EnterExitFlag is %02X\n",CardLan.EnterExitFlag);
		if(CardLan.EnterExitFlag == 0x55)								//����
		{
			if(OverTimeEnter() != 0)
			{
				if(((CardLan.StationID<= SectionNum)&&(CardLan.StationID != 0))||((Section.SationNow == 1)&&(CardLan.StationID!=0)))
				{
					status =  ExitCarProgram();
					if(status == MI_OK)
					{
						ret = 0;
					}
					else
					{
						ret = -1;
					}
				}
				else //���ϳ���
				{
					status = EnterCarProgram();
					if(status == MI_OK)
					{
						ret = 0;
						memcpy(Buf.longbuf,CardLan.QCash,4);
						if(Buf.i < HostValue.i) {
							Err_display(12);  //����
							Err_save(CARD_SPEC_M1_LINUX,12);
							ret = -1;
						}
					}
					else
					{
						ret = -1;
					}


				}
			}
			else
			{
				status = EnterCarProgram();
				if(status == MI_OK)
				{
					ret = 0;
					memcpy(Buf.longbuf,CardLan.QCash,4);
					if(Buf.i < HostValue.i) {
						Err_display(12);  //����
						Err_save(CARD_SPEC_M1_LINUX,12);
						ret = -1;
					}
				}
				else
				{
					ret = -1;
				}

			}
		}
		else
		{
			status = EnterCarProgram();
			DBG_PRINTF("after EnterCarProgram HostValue.i %d\n",HostValue.i);
			if(status == MI_OK)
			{
				ret = 0;
				memcpy(Buf.longbuf,CardLan.QCash,4);
				if(Buf.i < HostValue.i) {
					Err_display(12);  //����
					Err_save(CARD_SPEC_M1_LINUX,12);
					ret = -1;
				}
			}
			else
			{
				ret = -1;
			}
		}

		break;
	}


	if(ret != 0){
		return ret;
	}



	if(CardLan.StationDEC == 0x55)
	{
		status = YesOrNoMoney_cpu();
		if(status == MI_OK) {
			ret = 0;
		}
		else if(status == 0x01) 				//����ʱ�����ѽ��
		{
			Err_display(13);
            Err_save(CARD_SPEC_M1_LINUX,13);
			ret = -1;
		}
		else if(status == 0x02) 				//�����������ѽ��
		{
			Err_display(14);
            Err_save(CARD_SPEC_M1_LINUX,14);
			ret = -1;
		}
		else							//�����������ѽ��
		{
			Err_display(15);
            Err_save(CARD_SPEC_M1_LINUX,15);
			ret = -1;
		}
	}

	Sector.FlagValue = 2;  //�ֽ���

	return ret;
}



#ifdef SUPPORT_QR_CODE
extern unsigned char g_QRCodeRcvDataFg;
//extern unsigned char ConnectFlag; 	
unsigned char QRCode_OldTime[6]={0};

extern struct QRCode G_QRCodeInfo;
extern unsigned char g_FgCardLanQRCode;
extern int QRSaveExtData(void);
unsigned char QRCodeScanOutPut(enum OutPut_Status out_sta)
{
	unsigned char status = 1;


	switch(out_sta)
	{
		case QR_NO_ERROR:
			Sector.FlagValue = G_QRCodeInfo.type;
			ReturnDisplay(QR_CODE_DISPLAY_TYPE);
            #ifdef CANGNAN_BUS
                  status = SaveCardData_Zhujian(CARD_SPEC_QR_CODE, CONSUME_MODE_QRCODE, GET_RECORD | SAVE_RECORD);   
            #elif Transport_Stander
                  status = SaveCardData_jiaotong(CARD_SPEC_QR_CODE, CONSUME_MODE_QRCODE, GET_RECORD | SAVE_RECORD);         
            #else
				  status = SaveCardData(CARD_SPEC_QR_CODE, CONSUME_MODE_QRCODE, GET_RECORD | SAVE_RECORD); 
            #endif
           
			if (!status)
			{
				//IncSerId();
				status = QRSaveExtData();
			}
			break;
		case QR_ERROR_OFFLINE:
			Err_display(66);
			break;
			
		case QR_ERROR_NO_RESPOND:
			Err_display(67);
			break;
			
		case QR_SCAN_AGAIN:
			Err_display(70);
			break;
			
		case QR_NO_SUPPOR_TRANSACTION:
		case QR_SUPPORT_WECHAT_ONLY:
		case QR_TRANSACTION_FAIL:
		default:
			Err_display(69);
			break;
			
	}
	memcpy(QRCode_OldTime, (unsigned char *)&Time, 6);	//save time
	g_QRCodeRcvDataFg = 0;	 //clear the flag, waiting for next qrcode scanning
	g_FgCardLanQRCode= 0;
	return status;
}


void usSleep(unsigned int nusecs)
{
    struct timeval    tval;
 
    tval.tv_sec = nusecs / 1000000;
    tval.tv_usec = nusecs % 1000000;
    select(0, NULL, NULL, NULL, &tval);
}


/*
����ʱ��7���ֽ�BCD�루20170322163602�����ն˻��ţ����ֽ�BCD����
�۸� ����4�ֽ�BCD�� ��ά�볤�ȣ�һ���ֽ�HEX������ά��ţ�N���ַ�����
*/

unsigned char QRCodeSendToServer(void)
{
	unsigned char buffer[256];
	int i=0;
	
	buffer[i++] = QR_CODE_CMD_REQ;
	/*time
	buffer[i++] = 0x20; ///2017 .......
	memcpy(buffer+i, (unsigned char *)&Time, 6);
	i += 6;
	*/
	memcpy(buffer+i, DevNum.longbuf, 4);
	i += 4;
	memcpy(buffer+i, HostValue.longbuf, 4);
	i += 4;
	buffer[i++] = G_QRCodeInfo.length;
	memcpy(buffer+i, G_QRCodeInfo.id, G_QRCodeInfo.length);
	i += G_QRCodeInfo.length;
	//printf("QRCodeSendToServer len = %d\n", i);

/*
	#ifdef BS
    return QR_SendCmdPc(buffer, i);
    #else
	return SendCmdPc(buffer, i);
    #endif
  */
  	
  
}
#ifdef TEST_QR_CODE_SPEED
extern struct timeval second_test;
#endif
enum OutPut_Status QRCodeProcessConsume(unsigned char freed)
{
	DebugPrintf("\n");
	unsigned char Loop = 1,step = 1;
	unsigned char sendtimes, i=0, flash=0;
	char disp_buf[32] = "���Ժ�";
	char *str_dian = "...";
	unsigned char str_len = strlen("���Ժ�");
	//struct timeval first, second;
	LongUnon TempMoney;

	//if(ConnectFlag /*&& (!g_FgCardLanQRCode)*/)  return QR_ERROR_OFFLINE;	//check network first
	if(is_net_connect()!=1 || is_server_connect()!=1) return QR_ERROR_OFFLINE;
	CardLan.CardType = QR_CODE_TYPE;
	
	while(Loop)
	{
		switch (step)
		{
		case 1:
			if(Permissions(0) == MI_OK) 	//Ȩ��
			{
				step ++;
			}
			else 
				Loop = 0;
			break;
		case 2:
			if((Fixvalue.i==0) && (freed == 0)){
				HostValue.i = 0;
				if(AnalysisSheet(1) == MI_OK) //�����ѱ�
					step++;
				else
				{
					Err_display(16);
								Err_save(CARD_SPEC_QR_CODE,16);
					ioctl(mf_fd, RC531_HALT);
					Loop = 0;
				}

			}
			else{
				if(AnalysisSheet(0) == MI_OK) //�����ѱ�
					step++;
				else
				{
					Err_display(16);
								Err_save(CARD_SPEC_QR_CODE,16);
					ioctl(mf_fd, RC531_HALT);
					Loop = 0;
				}
			}
			break;
		case 3:
            /*
			if (g_FgCardLanQRCode)
			{
				beepopen(1);
				return QR_NO_ERROR;
			}
			*/
			#if 0
			beepopen(1);
			PlayMusic(30, 0);
			sleep(5);
			#else
#ifdef NEW0409
            SetColor(Mcolor);
			SetTextSize(48);
			SetTextColor(Color_white);
			TextOut(0,50, "��ܰ��ʾ");
			TextOut(0,120,"������������");
			TextOut(0,190,"���Ժ�");
			beepopen(1);
#else
			SetColor(Mcolor);
			SetTextSize(32);
			SetTextColor(Color_white);
			TextOut(100,50, "��ܰ��ʾ");
			TextOut(60,100,"������������");
			TextOut(110,150,"���Ժ�");
			beepopen(1);
#endif            
			#endif
			QRCodeSendToServer();
			#ifdef TEST_QR_CODE_SPEED
			gettimeofday(&second_test, NULL);
			#endif
			g_FgQRCodeRcvAck = 0;  //clear the important flag
			g_SendScanMarkCnt = QR_CODE_WAITE_Nx10MS;
			sendtimes = QR_CODE_SEND_CMD_TIMES;
			step++;
			break;
			
		case 4:
			flash= 0;
			for(;;)
			{
				if (g_FgQRCodeRcvAck) //rcv ack and no error 
				{
					g_FgQRCodeRcvAck = 0;
					g_SendScanMarkCnt = 0;
					return G_QRCodeInfo.status;

				}
				g_SendScanMarkCnt--;
				if ((g_SendScanMarkCnt%10) == 0) 
				{
					flash++;
					if (flash > 3) flash = 0;
					if (flash ==0) 
						strncpy(disp_buf+str_len, "   ", 3);
					else
						strncpy(disp_buf+str_len, str_dian, flash);
#ifdef NEW0409
        			TextOut(0,190,disp_buf);
#else
					TextOut(110,150,disp_buf);
#endif
				}
				if (g_SendScanMarkCnt == 0) 
				{
					sendtimes--;
					if (sendtimes)
					{
						QRCodeSendToServer();
						g_FgQRCodeRcvAck = 0;  //clear the important flag
						g_SendScanMarkCnt = QR_CODE_WAITE_Nx10MS;
						continue;
					}
					return QR_ERROR_NO_RESPOND;
				}
		//		if(ConnectFlag) 
				if(is_net_connect()!=1 || is_server_connect()!=1)
				{
					g_SendScanMarkCnt = 0;
					return QR_ERROR_OFFLINE;
				}
				QRCode_1msDelay(1);
			}
			Loop = 0;
			break;
		default:
			break;
		}
	}
	return 0xff;
}


#endif

#endif
#if 0
unsigned char ReadCardZeroSector(void)
{
	unsigned char Loop = 1, step = 1, ReturnValue = 0;
	unsigned long ReadLen = 0;
	unsigned char TempBuf[8],ReadBuf[16];
	unsigned char authentime=0;
	struct card_buf KeyInfo;
	unsigned char SectorNo = 0;

	memset(&KeyInfo, 0, sizeof(struct card_buf));
	
	memset(ReadBuf, 0, sizeof(ReadBuf));

	KeyInfo->mode = KEYA;
	memset(KeyInfo->key, 0xFF, 6);   //6 FF
	memset(KeyInfo->rwbuf, 0xff, 16);

	while(Loop)
	{
		switch(step)
		{
		case 1:
			ioctl(mf_fd, WRITE_TYPE, W_CHAR);

			if(write(mf_fd, &KeyInfo, sizeof(struct card_buf)) == MI_OK)
				step++;
			else 
				Loop = 0;
			break;
		case 2:
			if(ioctl(mf_fd, RC531_AUTHENT, (4*SectorNo + 3)) == MI_OK)
				step++;
			else 
			{
                
    		    Loop = 0;
             }
			break;

		case 3:
			if(ioctl(mf_fd, RC531_READ,(4*SectorNo + 0)) != MI_OK)
			{
				Loop = 0;
				break;
			}
			
			if(ioctl(mf_fd, FIFO_RCV_LEN, &ReadLen) != MI_OK)
			{
				Loop = 0;
				break;
			}
			
			if(read(mf_fd, ReadBuf, ReadLen) >= 0)
			{
				if(ReadBuf)
				{
					memcpy(UserCard.ScZero.VendorSpace, ReadBuf, ReadLen);
					step++;
				}
				else
				{
					DebugPrintf("read error\n");
					Loop = 0;
				}
			}
			else
			{
				//DebugPrintf("ReturnValue = 0x%02X\n", ReturnValue);
				Loop = 0;
			}
			break;
			
		case 4:
			if(ioctl(mf_fd, RC531_READ,(4*SectorNo + 1)) != MI_OK)
			{
				Loop = 0;
				break;
			}
			
			if(ioctl(mf_fd, FIFO_RCV_LEN, &ReadLen) != MI_OK)
			{
				Loop = 0;
				break;
			}
			
			if(read(mf_fd, ReadBuf, ReadLen) >= 0)
			{
				if(ReadBuf)
				{
					memcpy((unsigned char *)&UserCard.ScZero.BOne.Version, ReadBuf, ReadLen);
					step++;
				}
				else
				{
					DebugPrintf("read error\n");
					Loop = 0;
				}
			}
			else
			{
				//DebugPrintf("ReturnValue = 0x%02X\n", ReturnValue);
				Loop = 0;
			}
			break;

		case 5:
			if(ioctl(mf_fd, RC531_READ,(4*SectorNo + 2)) != MI_OK)
			{
				Loop = 0;
				break;
			}
			
			if(ioctl(mf_fd, FIFO_RCV_LEN, &ReadLen) != MI_OK)
			{
				Loop = 0;
				break;
			}
			
			if(read(mf_fd, ReadBuf, ReadLen) >= 0)
			{
				if(ReadBuf)
				{
					memcpy(UserCard.ScZero.DirZoom2, ReadBuf, ReadLen);
					step++;
				}
				else
				{
					DebugPrintf("read error\n");
					Loop = 0;
				}
			}
			else
			{
				//DebugPrintf("ReturnValue = 0x%02X\n", ReturnValue);
				Loop = 0;
			}
			break;
			
		default:
            
			step = 0;
			Loop = 0;
			break;
		
		}
	}

	return step;
}
#endif

/*��Ӫ��λ����*/
#define COMP_NUMBER		15
const unsigned char CompCode[COMP_NUMBER][5] = {
	"00000",
	"00010",
	"00020",
	"00030",
	"00040",
	"00050",
	"00060",
	"00070",
	"00080",
	"01000",
	"01002",
	"01001",
	"01002",
	"01003",
	"01004"
};


void CalcM1CardKey(void)
{
	unsigned char step = 1, loop = 1;
	unsigned char buffer[32];
	unsigned char rcvbuf[128],  rcvlen;
	unsigned char sectorFg[8] = {0x01, 0x10, 0x12, 0x04, 0x20, 0x03, 0x03, 0x30};  //������ʶ

	while(loop)
	{
		switch (step)
		{
			case 1:
				memset(buffer, 0, sizeof(buffer));
				buffer[0] = 0;
				buffer[1] = 0xA4;
				buffer[2] = 0;
				buffer[3] = 0;
				buffer[4] = 2;
				buffer[5] = 0x10;
				buffer[6] = 0x03;
				rcvlen = 7;
				if (PsamCos(buffer, rcvbuf, &rcvlen) == MI_OK)
				{
					if (rcvlen > 0 && rcvbuf[0] == 0x61 && rcvbuf[1] == 0x09)
						step++;
					else
						loop = 0;
				}
				else
					loop = 0;
				break;
			case 2:
				memset(buffer, 0, sizeof(buffer));
				buffer[0] = 0x80;
				buffer[1] = 0xFC;
				buffer[2] = 0;
				buffer[3] = 1;
				buffer[4] = 20;  //len
				memcpy(buffer+5, UserCard.ScOne.CityNum, 2);
				memcpy(buffer+7, CardLan.UserIcNo, 4);
				memcpy(buffer+11, UserCard.ScOne.IssueNum+2, 2);
				memcpy(buffer+13, UserCard.ScOne.AuthCode, 4);
				memcpy(buffer+17, sectorFg, 8);
				rcvlen = 25;
				if (PsamCos(buffer, rcvbuf, &rcvlen) == MI_OK)
				{
					if (rcvlen > 0 && rcvbuf[0] == 0x61)
					{
						
						step++;
					}
					else
						loop = 0;
				}
				else
					loop = 0;
				break;
			#if 0
			case 2:
				memset(buffer, 0, sizeof(buffer));
				buffer[0] = 0;
				buffer[1] = 0xC0;
				buffer[2] = 0;
				buffer[3] = 0;
				buffer[4] = 8*6;
				rcvlen = 53;
				if (PsamCos(buffer, rcvbuf, &rcvlen) == MI_OK)
				{
					if (rcvlen > 0 && rcvbuf[0] == 0x61 && rcvbuf[1] == 0x09)
						step++;
					else
						loop = 0;
				}
				else
					loop = 0;
				break;
			#endif
			default:
			step = 0;
			loop = 0;
				break;
		}
	}
}

unsigned char ReadM1CardType(void)
{
	unsigned char buffer[48]={0};
	unsigned char type = 0;
	
	if (ReadOneSectorDataFromCard(buffer, 48, 0, SC_ZERO|SC_ONE|SC_TWO, VERIFY_KEY) == MI_OK)
		memcpy(OperCard.OScOne.CityNum, buffer, 48);
	memset(buffer, 0, sizeof(buffer));
	if (ReadOneSectorDataFromCard(buffer, 48, 1, SC_ZERO|SC_ONE|SC_TWO, VERIFY_KEY) == MI_OK)
	{
		type = buffer[12];
		if (type == OPERATOR_CARD)
			memcpy(OperCard.OScOne.CityNum, buffer, 48);
		else
			memcpy(UserCard.ScOne.CityNum, buffer, 48);   //copy 3 block
	}
	else
		return 0;

	return type;
}
#if 1
/*
����ֵ: -1 ��û���ҵ�����
-2 �˿��ڱ���·��ֹʹ��
-3 ���С�ڹ涨����Сֵ������ڹ涨�����ֵ
-4 ����͸֧���

*/
int FindCardTypeAmount(unsigned char logictype, unsigned short balance, unsigned short time_inter, unsigned char *beepmode)
{
	int i;
	unsigned char mode;
	ShortUnon tmp;
	for(i=0; i<CARD_NUMBER; i++)
	{
		if(logictype == CardConParam[i].logiccardtype)
			break;
	}
	if (i == CARD_NUMBER) //can't find the card's parameter
	{
		if (flc0005.glocalnodefinecard == 0)
			return -1; 
		else if (flc0005.glocalnodefinecard == 0x01){
				
		} else if (flc0005.glocalnodefinecard == 0x02) {
			
		}
	}

	mode = CardConParam[i].consumemode & 0xf0;
	if (mode != 1 && mode != 2) {
		return -2; 	  // �˿��ڱ���·��ֹʹ��
	}

	if (balance < CardConParam[i].minblancelimit.i || balance > CardConParam[i].maxblancelimit.i) 
	{
		printf("balance is error  bal = %u, limit = %u, max = %u \n", balance, CardConParam[i].minblancelimit.i, CardConParam[i].maxblancelimit.i);
		return -3;
	}
	
	*beepmode = CardConParam[i].consumemode & 0x0f;

	memcpy(tmp.intbuf, flc0005.gbupiaolimittime, 2);
	if (time_inter > tmp.i)   // �����
	{
		HostValue.i = (HostValue.i*CardConParam[i].outdiscontrate)/100;
	}
	else  // ��
	{
		HostValue.i = (HostValue.i*CardConParam[i].indiscontrate)/100;
	}

	if (HostValue.i > CardConParam[i].maxdebit.i)
		HostValue.i = CardConParam[i].maxdebit.i;
	if (HostValue.i > balance)
	{
		tmp.i = HostValue.i - balance; // ͸֧�޶�
		if (tmp.i > CardConParam[i].overdraw.i)
			return -4;
	}
	
#if 0
	switch(LocalCardRate[i].cardattr)
	{
		case 0x01: // ��ͨ��ֵ��
			break;
		case 0x02: // �ƴο�
			break;
		case 0x03: // ���ڿ�
			break;
		case 0x04: // ���ⴢֵ��
			break;
		default:
			break;
	}
#endif
	

}

#if 0
unsigned char *SectionFileP = NULL;

int InitSectionInfomation(void)
{
	FILE *fp = NULL;
	struct stat fileInfo;
	stat(SECTION_PATH_NAME , &fileInfo);
	
	SectionFileP = (unsigned char *)malloc(fileInfo.st_size);
	if (SectionFileP == NULL)
	{
		printf("malloc mem error \n");
		return -2;
	}
	
	fp = fopen(SECTION_PATH_NAME, "r");
	if (NULL == fp) 
	{
		printf("open file error \n");
		return -1;
	}

	fread(SectionFileP, 1, fileInfo.st_size, fp);


	fclose(SECTION_PATH_NAME);
	
}

// onstation �ϳ�վ̨��� offstation �³�վ̨���
unsigned int GetSectionAmontFromPara(unsigned char onstation, unsigned char offstation)
{
	unsigned char maxStation;
	unsigned int totalNm, exitCarToEndNm, index;
	unsigned char tmpNm;
	LongUnon dat;
	// off > on
	totalNm = maxStation*(maxStation-1)/2;
	tmpNm = offstation - onstation;
	exitCarToEndNm = tmpNm*(tmpNm+1);
	index = totalNm - exitCarToEndNm + tmpNm;
	//index -= 1; //?
	index *= SECTION_POINT_SIZE ;

	if (Section.Updown == 0)
		memcpy(dat.longbuf, StationdisupParBuf+index, SECTION_POINT_SIZE);
	else
		memcpy(dat.longbuf, StationdisdownParBuf+index, SECTION_POINT_SIZE);

	return dat.i;
	
	//on < off
	
	
}
#endif

// onstation �ϳ�վ̨��� offstation �³�վ̨���
//ONSTATION Ϊ0ʱOFFSTATION ����Ϊ��ǰվ���
unsigned int GetSectionKMFromPara(unsigned char onstation, unsigned char offstation)
{
	//LongUnon datOn[22], datOff[22];
	//memcpy(datOn, 0, sizeof(datOn));
	//memcpy(datOff, 0, sizeof(datOff));
	memset(&GetOnOffInfo, 0, sizeof(GetOnOffInfo));
	if (Section.Updown == 0)
	{
		if (onstation)
			memcpy(&GetOnOffInfo.On_StationNo, StationdisupParBuf+onstation*SECTION_POINT_SIZE, SECTION_POINT_SIZE);
		memcpy(&GetOnOffInfo.Off_StationNo, StationdisupParBuf+offstation*SECTION_POINT_SIZE, SECTION_POINT_SIZE);
	}
	else
	{
		if (onstation)
			memcpy(&GetOnOffInfo.On_StationNo, StationdisdownParBuf+onstation*SECTION_POINT_SIZE, SECTION_POINT_SIZE);
		memcpy(&GetOnOffInfo.Off_StationNo, StationdisdownParBuf+offstation*SECTION_POINT_SIZE, SECTION_POINT_SIZE);
	}
	//if (GetOnOffInfo.On_StationNo == onstation && GetOnOffInfo.Off_StationNo == offstation)
		GetOnOffInfo.Real_Mileage.i = GetOnOffInfo.Off_Mileage.i - GetOnOffInfo.On_Mileage.i;
	
	return GetOnOffInfo.Real_Mileage.i;
}



#endif

#define   NEED_UPDATE_WALLET	0x01
#define   NEED_UPDATE_STATUS	0x02
int SectionInOutCarProc(void)
{
		unsigned char ret = 1;
		unsigned char buffer[48];
		unsigned char money = 0;

		switch(Section.Updoor)
		{
			case 1:	//ǰ��
				ret = 0;
				if (UserCard.ScFive.BusRegFlag == 0x18)  //�ϴ��³�û��ˢ��
				{
					if (UserCard.ScSix.GetOnBusStatus[0] == 0x0A)
					{
						//�۳���ע���
						g_UseRegisterMoney = 1;
						ret = NEED_UPDATE_WALLET;
					}
				}
				
				memcpy(UserCard.ScSix.GetOnTime, &Time, 5); //save get on time 
				UserCard.ScSix.GetOnStation = Section.SationNow;
				if (Section.Updown == 0)  //shang 
					UserCard.ScSix.DirFlag = 0xAB;
				else
					UserCard.ScSix.DirFlag = 0xBA;
				// ������ע����·�ţ�������
				memcpy(UserCard.ScSix.RoadNum, flc0005.glinenum, 2);
				UserCard.ScFive.BusRegFlag = 0x18;  // ��¼�����ļ�
				UserCard.ScSix.GetOnBusStatus[0] = 0x0A;
				CardLan.EnterExitFlag = 0x55;
				//if (WriteOneSertorDataToCard(&UserCard.ScSix.GetOnTime, 6, SC_ZERO|SC_ONE, VERIFY_KEY) == MI_OK)
				//	ret = 0;
				ret |= NEED_UPDATE_STATUS;
				break;
				
			case 2:	//����
				if (UserCard.ScFive.BusRegFlag == 0x18) 
				{
					UserCard.ScFive.BusRegFlag = 0;
					if (UserCard.ScSix.GetOnBusStatus[0] == 0x0A)
					{
						//���Ҳ�������Ľ��
						//memcpy(HostValue.longbuf, , 2);
						//memset(UserCard.ScSix.GetOnTime, 0, 32);
						
						CardLan.EnterExitFlag = 0;  // ����
						ret = (NEED_UPDATE_WALLET|NEED_UPDATE_STATUS);
					}
					else
					{
						return -1; //��¼�ϳ���������û�й����ļ�
					}
				}
				else // �ϳ�û��ˢ��
				{
					return -2;
				}
				break;
				
			default:	//ǰ����
				memcpy(buffer, PsamNum, 3);
				buffer[2] &= 0xF0;
				// �жϹ�����Ӫ�� �������if �������ϵ
				// memcmp();
				if (UserCard.ScFive.BusRegFlag == 0x18)  //��־λΪ�ϳ�����ʱ��ˢ�³���
				{
					UserCard.ScFive.BusRegFlag = 0;
					if (UserCard.ScSix.GetOnBusStatus[0] == 0x0A)
					{
						//���Ҳ�������Ľ��
						//memset(UserCard.ScSix.GetOnTime, 0, 32);//sizeof(UserCard.ScSix));  //set 0
						CardLan.EnterExitFlag = 0;
						ret = (NEED_UPDATE_WALLET|NEED_UPDATE_STATUS);
					}
					else
					{
						return -3;  //error
					}
					
				}
				else // ��־λΪ�³�����Ҫˢ���ϳ�
				{
					memcpy(UserCard.ScSix.GetOnTime, &Time, 5); //save get on time 
					UserCard.ScSix.GetOnStation = Section.SationNow;
					if (Section.Updown == 0)  //shang 
						UserCard.ScSix.DirFlag = 0xAB;
					else
						UserCard.ScSix.DirFlag = 0xBA;
					// ������ע����·�ţ�������
					memcpy(UserCard.ScSix.RoadNum, flc0005.glinenum, 2);
					UserCard.ScFive.BusRegFlag = 0x18;  // ��¼�����ļ�
					UserCard.ScSix.GetOnBusStatus[0] = 0x0A;
					CardLan.EnterExitFlag = 0x55;
					//if (WriteOneSertorDataToCard(&UserCard.ScSix.BZero.GetOnTime, 6, SC_ZERO|SC_ONE, VERIFY_KEY) == MI_OK)
					ret = NEED_UPDATE_STATUS;
				}
				break;
		}
		
	return ret;
}



int UserCardTransStatus(void)
{
	unsigned char step = 1, loop = 1;
	unsigned char status = 1, onlyreg = 0;
	unsigned char buffer[64];
	unsigned char recSector = 0, recBlock = 0;
	LongUnon tmpcnt, tmpval, oldmoney;
	unsigned int agamoney;
	int ret;
	
	onlyreg = 0;
	g_UseRegisterMoney = 0;
	
	while(loop)
	{
		switch(step)
		{
			case 1:
				if (UserCard.ScOne.CardStatus != 0x02)
				{
					Err_display(11);
					loop = 0;
					break;
				}
				status = Card_JudgeDate();
				if(status == MI_OK)
				{
					step++;
				}
				else
				{
					Err_display(1);
					loop = 0;
				}
				break;
			case 2:
				if (ReadOneSectorDataFromCard(buffer, 48, 5, SC_ZERO|SC_TWO, VERIFY_KEY) == MI_OK)
				{
					memcpy((unsigned char *)&UserCard.ScFive.BusRegFlag, buffer, 32);
					if (UserCard.ScFive.BusRegFlag == 0xA5)  //������
					{
						Err_display(10);
						loop = 0;
						break;
					}
					else if(UserCard.ScFive.BusRegFlag == 0)
					{
						// ����¼�������ڴ���Ҫ�����·��ĺ������жϣ�����Ǻ������Ѵ˱�����ֵA5
						step++;
					}
					else  //�Ƿ���
					{
						Err_display(11);
						loop = 0;
						break;
					}
				}
				else 
				{
					loop = 0;
				}
				break;
			case 3:
				if (UserCard.ScOne.CardType == EMPLOYEE_CARD)
				{
					//Ա���� ������STEP = X;
				}
				else
					step++;
				break;
			case 4:
				if (ReadOneSectorDataFromCard(buffer, 48, 4, SC_ZERO|SC_ONE|SC_TWO, VERIFY_KEY) == MI_OK)
				{
					memcpy(UserCard.ScFour.VaildDate, buffer, 48);
					if (UserCard.ScFour.TopupPara & 0xf0 == 0)   //��ͨ��
					{
						step++;
					}
					else //if (UserCard.ScFour.TopupPara & 0xf0 == 0x10)  //��Ʊ
					{
						loop = 0;
					}
					
				}
				else 
				{
					loop = 0;
				}
				break;
				
			case 5:
				memcpy(tmpcnt.longbuf, UserCard.ScFive.AgaTransCnt, 4);
				memcpy(tmpval.longbuf, UserCard.ScFive.TransCnt1, 4);
				agamoney = ~tmpcnt.i;
				if ((memcmp(UserCard.ScFive.TransCnt2, UserCard.ScFive.TransCnt1, 4)==0) && (agamoney == tmpval.i))  //�ۼƴ����Ϸ�
				{
					if (memcmp(UserCard.ScFive.TransCntBak, UserCard.ScFive.TransCnt1, 4))
					{
						memcpy(UserCard.ScFive.TransCntBak, UserCard.ScFive.TransCnt1, 4); 
					}
				}
				else //�ۼƴ������Ϸ�
				{
					memcpy(UserCard.ScFive.TransCnt1, UserCard.ScFive.TransCntBak, 4);   
					memcpy(UserCard.ScFive.TransCnt2, UserCard.ScFive.TransCntBak, 4);   
					memcpy(tmpcnt.longbuf, UserCard.ScFive.TransCntBak, 4);
					tmpval.i = ~tmpcnt.i;
					memcpy(UserCard.ScFive.AgaTransCnt, tmpval.longbuf, 4);
				}

				step++;
				
				break;
				
			/*
				���￪ʼ����һƱ����ֶ�
			*/
			
			case 6:
				if(Section.Enable != 0x55)  //����
				{
					step = 9;
				}
				else
				{
					if (ReadOneSectorDataFromCard(buffer, 48, 6, SC_ZERO|SC_ONE, VERIFY_KEY) == MI_OK)
					{
						memcpy(UserCard.ScSix.GetOnTime, buffer, 32);
						memcpy(RegisterMoney.intbuf, UserCard.ScSix.OnTEndVal, 2);
						CardLan.StationID = UserCard.ScSix.GetOnStation; //�ϳ����
						if (UserCard.ScSix.DirFlag == 0xAB)
						{
							CardLan.StationOn = 0;  //����
						}
						else 
						{
							CardLan.StationOn = 1;  
						}
						
						// �������³�
						ret = SectionInOutCarProc();
						if (ret & NEED_UPDATE_WALLET)
						{ 
							step++; 
						}
						else if (ret & NEED_UPDATE_STATUS)
						{
							onlyreg = 1;
							step = 10;  // only update get on register 
						}
						else// if ( ret < 0)  error 
						{
							loop = 0;
						}
						

					}
					else
						loop = 0;
				}
				break;

			case 7:
				if (g_UseRegisterMoney)   // �Ѿ�ȷ������Ʊ���쳣���۳���ע���
				{
					step = 9;
					break;
				}
				//�ж���·��
				memcpy(buffer, PsamNum, 3);   //5 + 0
				UserCard.ScFive.OperCode[2] &= 0xF0;

				if (!memcmp(flc0005.glinenum, UserCard.ScSix.RoadNum, 2) && (flc0005.glinenum == UserCard.ScSix.DirFlag)/* && !memcmp(CardLanCPU.getonline, UserCard.ScSix.BZero.BusNum, 3)*/)
				{
					//����վ������
					SectionSheet();
				}
				else // �쳣���
				{
					//�۳���ע�Ľ��
					g_UseRegisterMoney = 1;
				}
				memset(UserCard.ScSix.GetOnTime, 0, 32);  // set to zero  ��ʱ��ˢ���³�
				
				step = 9;
				break;
				
				
			case 9:
				if (ReadOneSectorDataFromCard(buffer, 48, 2, SC_ZERO|SC_ONE|SC_TWO, VERIFY_KEY) == MI_OK)
				{
					memcpy(UserCard.ScTwo.Reserve1, buffer, 48);

					memcpy(tmpcnt.longbuf, UserCard.ScTwo.Wallet1, 4);
					memcpy(tmpval.longbuf, UserCard.ScTwo.AgaWallet, 4);
					agamoney = ~tmpval.i;
					if ((memcmp( UserCard.ScTwo.Wallet1,  UserCard.ScTwo.Wallet2, 4)==0) && (agamoney == tmpcnt.i))  //�ж���Ǯ���Ϸ���
					{
						memcpy(tmpcnt.longbuf, UserCard.ScTwo.Wallet1Bak, 4);
						memcpy(tmpval.longbuf, UserCard.ScTwo.AgaWalletBak, 4);
						agamoney = ~tmpval.i;
						if ((memcmp( UserCard.ScTwo.Wallet1Bak,  UserCard.ScTwo.Wallet2Bak, 4)==0) && (agamoney == tmpcnt.i))  //����Ǯ���Ϸ�
						{
							step++;
						}
						else
						{
							memcpy(UserCard.ScTwo.Wallet1Bak, UserCard.ScTwo.Wallet1, 16);  //��Ǯ������������
							step++;
						}
					}
					else
					{
						memcpy(tmpcnt.longbuf, UserCard.ScTwo.Wallet1Bak, 4);
						memcpy(tmpval.longbuf, UserCard.ScTwo.AgaWalletBak, 4);
						agamoney = ~tmpval.i;
						if ((memcmp( UserCard.ScTwo.Wallet1Bak,  UserCard.ScTwo.Wallet2Bak, 4)==0) && (agamoney == tmpcnt.i))  //����Ǯ���Ϸ�
						{
							memcpy(UserCard.ScTwo.Wallet1, UserCard.ScTwo.Wallet1Bak, 16);  //���ݿ�������Ǯ��
							step++;
						}
						else
						{
							loop = 0;  //��Ǯ���뱸��Ǯ��������
						}
					}
				}
				else 
				{
					loop = 0;
				}
				break;
				
			case 10:
				tmpcnt.i =0;
				tmpval.i = 0;
				memcpy(tmpcnt.longbuf, UserCard.ScTwo.Wallet1, 4);
				memcpy(tmpval.longbuf, UserCard.ScTwo.Balance, 3);   //��ֵ�����
				if (tmpcnt.i > tmpval.i) 
				{
					// �����ˣ���Ҫ��Ӧ����ʾ
					loop = 0;
				}
				else 
				{
					memcpy(tmpcnt.longbuf, UserCard.ScFive.TransCnt1, 4);
					tmpcnt.i++;   //��¼��1
					memcpy(UserCard.ScFive.TransCnt1, tmpcnt.longbuf, 4);
					memcpy(UserCard.ScFive.TransCnt2, tmpcnt.longbuf, 4);
					memcpy(UserCard.ScFive.TransCntBak, tmpcnt.longbuf, 4);
					tmpval.i = ~tmpcnt.i;
					memcpy(UserCard.ScFive.AgaTransCnt, tmpval.longbuf, 4);

					memcpy(UserCard.ScFive.TransTime, &Time.year, 5);
					if (UserCard.ScFive.TransRegFlag == 0)
						UserCard.ScFive.TransRegFlag = 33;
					else
					{
						UserCard.ScFive.TransRegFlag++;
						if (UserCard.ScFive.TransRegFlag > 38)
							UserCard.ScFive.TransRegFlag = 33;
					}
					//��Ӫ�̴���
					memcpy(UserCard.ScFive.OperCode, PsamNum, 3);   //5 + 0
					UserCard.ScFive.OperCode[2] &= 0xF0;
					if (WriteOneSertorDataToCard(&UserCard.ScFive.BusRegFlag, 5, SC_ZERO|SC_TWO, VERIFY_KEY) == MI_OK)
					{
						if (onlyreg) step += 2;
						else step++;
					}
					else
					{
						// д��¼������
						loop = 0;
					}
				}
				break;

				
			case 11:
				if(Section.Enable == 0x55)   // �ֶ���Ҫ�����Ż�??
				{
					if (g_UseRegisterMoney)
					{
						HostValue.i = RegisterMoney.i;
					}
				}
				else {
				// ������ͨ��������Ҷ�Ӧ�Ľ��Żݵ�
				// �������ֵ��HostValue.i
				//DecValue.i   //������ԭ��
			
					}
				
				memcpy(tmpval.longbuf, UserCard.ScTwo.Wallet1, 4);
				oldmoney.i = tmpval.i;
				if (tmpval.i >= HostValue.i)
				{
					tmpval.i -= HostValue.i;
					memcpy(UserCard.ScTwo.Wallet1, tmpval.longbuf, 4);
					memcpy(UserCard.ScTwo.Wallet2, tmpval.longbuf, 4);
					memcpy(UserCard.ScTwo.Wallet1Bak, tmpval.longbuf, 4);
					memcpy(UserCard.ScTwo.Wallet2Bak, tmpval.longbuf, 4);
					tmpcnt.i = ~tmpval.i;
					memcpy(UserCard.ScTwo.AgaWallet, tmpcnt.longbuf, 4);
					memcpy(UserCard.ScTwo.AgaWalletBak, tmpcnt.longbuf, 4);
					status = WriteOneSertorDataToCard(UserCard.ScTwo.Wallet1, 2, SC_ONE|SC_TWO, VERIFY_KEY);

					if (status > 3)
					{
						if ((status & 0x20 == 0) && (status & 0x40))
							step++; // д��Ǯ���ɹ�������Ǯ��ʧ�ܣ������ɹ�����
						else
						{
							//д��Ǯ��ʧ��
							loop = 0;
						}
					} 
					else if (status){
						loop = 0;
					}
					else step++;
				}	
				else{
					//����
					Err_display(12);
					loop = 0;
				}
		
				break;
				
			case 12:
				if (UserCard.ScFive.TransRegFlag < 35)
				{
					recSector = 8;
					if (UserCard.ScFive.TransRegFlag == 33)
						recBlock = SC_ONE;
					else
						recBlock = SC_TWO;
				}
				else 
				{
					recSector = 9;
					if (UserCard.ScFive.TransRegFlag == 36)
						recBlock = SC_ZERO;
					if (UserCard.ScFive.TransRegFlag == 37)
						recBlock = SC_ONE;
					else
						recBlock = SC_TWO;
				}
				memcpy(UserCard.TsRecord.TransTime, &Time.year, 5);
				memcpy(UserCard.TsRecord.OldMoney, oldmoney.longbuf, 3);
				memcpy(UserCard.TsRecord.TransMoney, HostValue.longbuf, 2);
				UserCard.TsRecord.TransType = Sector.FlagValue; //��������
				UserCard.TsRecord.SystemCode = 0x01;
				memcpy(UserCard.TsRecord.SystemInfo, flc0005.glinenum, 2); //��2���ֽ������³���־
				UserCard.TsRecord.SystemInfo[2] = UserCard.ScSix.GetOnStation;
				UserCard.TsRecord.SystemInfo[3] = Section.SationNow;
				
				WriteOneSertorDataToCard(UserCard.TsRecord.TransTime, recSector, recBlock, VERIFY_KEY);
				if (Section.Enable == 0x55)  // �ֶΣ���Ҫ��¼���³�״̬
					WriteOneSertorDataToCard(&UserCard.ScSix.GetOnTime, 6, SC_ZERO|SC_ONE, VERIFY_KEY);
				step++;
				break;
			
			case 13:
				// дˢ����¼
				break;

				
			default:
				loop = 0;
				step = 0;
				break;
		}
	}	

	return step;
}

int OperCardTransStatus(void)
{
	unsigned char step = 1, loop = 1;
	unsigned char status = 1;
	unsigned char buffer[64];

	while(loop)
	{
		switch(step)
		{
			case 1:
				if (OperCard.OScOne.CardStatus != 0x02)
				{
					Err_display(11);
					loop = 0;
					break;
				}
				status = Card_JudgeDate();
				if(status == MI_OK)
				{
					step++;
				}
				else
				{
					Err_display(1);
					loop = 0;
				}
				break;
			case 2:
				if (ReadOneSectorDataFromCard(buffer, 48, 5, SC_ZERO|SC_TWO, VERIFY_KEY) == MI_OK)
				{
					memcpy((unsigned char *)&OperCard.OScFive.BusRegFlag, buffer, 32);
					if (OperCard.OScFive.BusRegFlag == 0xA5)  //������
					{
						Err_display(10);
						loop = 0;
						break;
					}
					else if(OperCard.OScFive.BusRegFlag == 0)
					{
						// ����¼�������ڴ���Ҫ�����·��ĺ������жϣ�����Ǻ������Ѵ˱�����ֵA5
						step++;
					}
					else  //�Ƿ���
					{
						Err_display(11);
						loop = 0;
						break;
					}
				}
				else 
				{
					loop = 0;
				}
				break;
				
			case 3:
				if (ReadOneSectorDataFromCard(buffer, 48, 2, SC_ZERO, VERIFY_KEY) == MI_OK)
				{
					if (OperCard.Un.OScTwo.OperType == OPERATOR_CAPTURE_CARD) //�ɼ���
					{
						// ���ɼ�����
						step = 5;
					}
					else if (OperCard.Un.OScTwo.OperType == OPERATOR_DRIVER_CARD)  // ˾����
					{
						CardLan.CardType = OPERATOR_DRIVER_CARD;
						memcpy(SeDriverCard.longbuf, OperCard.Un.OScTwo.DriverID, 4);
						step++;
					}
					else if (OperCard.Un.OScTwo.OperType == OPERATOR_WATCHER_CARD) // ��ƱԱ��
					{
						CardLan.CardType = OPERATOR_WATCHER_CARD;
						memcpy(WatcherCard.longbuf, OperCard.Un.OScTwo.DriverID, 4);
						step++;
					}
					else
					{
						Err_display(2);  // 
						loop = 0;
					}
				}
				break;
			case 4:
				SaveDirverNumber();
				beepopen(2);
				ReturnDisplay(2);
				break;
			default:
				step = 0;
				loop = 0;
				break;
			
		}
	}
}


int M1CardSwipeStatus(void)
{
	unsigned char cardtype;
	int ret = -1;
	cardtype = ReadM1CardType();

	if (cardtype == 0) return -1;

	CardLan.CardType = cardtype;
	
	switch(cardtype)
	{
		case OPERATOR_CARD:    //����
			if (g_CardCurrentMode == OPERATOR_MODE)
				OperCardTransStatus();
			break;

		default:
			if (g_CardCurrentMode == USECARD_MODE)
				ret = UserCardTransStatus();
			break;
	}


	
}











