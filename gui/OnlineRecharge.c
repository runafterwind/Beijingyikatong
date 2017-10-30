#include <apparel.h>
#include "InitSystem.h"
#include "OnlineRecharge.h"

#include "RC500.h"
#include "des.h"
#include "cpucard.h"

#include "../sound/sound.h"
#include "../Display/fbtools.h"
#include "../input/keyboard.h"

#include "../errorlog/Errorlog.h"

#include <linux/input.h>

#define  UARTDIS  	 1

#define DATE_STYLE1  1
#define DATE_STYLE2  2
#define DATE_STYLE3  3
#define DATE_STYLE4  4

/////////////////////////////////////////////////////////////////////////////////////////////////////
#if RUSHAN_BUS
extern pthread_mutex_t m_stationrecord;
extern	struct STATION_INFO_ stationtaill,stationhead;
#endif

extern unsigned char use_jiaotong_stander;
extern CardInformCPU CardLanCPU;



extern LongUnon g_DisplayMoney;
extern int mf_fd;
extern int uart4_fd;

extern FILE *nettab;

extern LongUnon DevNum,DecValue;
extern CardInform CardLan;
extern RecordFormat SaveData;
extern ShortUnon Infor;
extern ShortUnon Driver;
extern JackRegal Sector;
extern unsigned char AutoUpFlag;
extern struct card_buf  test;
extern unsigned char SnBack[4];
extern char TcpIpBuf[35];

//extern unsigned char FileOpenFlag;
extern unsigned char KeyDes[8];
static unsigned char DesKeybuf[8];
extern unsigned char SelfAddress[32];
extern FixTimerBuf FixTime;
extern Interval  retdata;
extern unsigned char OPENBEEP;			//1´ò¿ªÉùÒô 0¹Ø±ÕÉùÒô
extern unsigned char OPENPRINTF;
extern unsigned char MonthlyFlay;  //°üÔÂ±êÖ¾
extern unsigned char CardTwo;  //°üÔÂ±êÖ¾
extern CardLanSector LanSec;		//ÓÃ»§ÉÈÇø
extern Operat USBFile;
extern Operat MMCFile;
extern SectionFarPar Section,Sectionup;
extern unsigned char ConnectFlag; //ÉÏÍø±êÖ¾
extern unsigned short SectionNum; //
extern unsigned int CurIcCardNo;

extern volatile unsigned char PlayMusicFinishFlag;		//¸Ã±êÖ¾Î»ÓÃÀ´ÅÐ¶Ïµ±Ç°µÄÉùÒôÊÇ·ñÒÑ¾­²¥Íê

extern unsigned int SumMoney;
extern unsigned char showSunMoneyFLag;
extern unsigned char RepairLastRecErrorFlag;
extern char Eflag;
extern BMP_INFO bmppicture1;


unsigned char SetDes[8] = {0x82,0x26,0x00,0x36,0x82,0x42,0x27,0x79};
unsigned char stepokflag;
unsigned char UserKeyname[14];
unsigned char HttpProt[35];

unsigned char valuetype;
unsigned char stepokflag;
unsigned char Readcard;
unsigned char EnterFlag;
unsigned char Scanfok;
unsigned char updatline_err;

LongUnon HostValue,DecValue;
LongUnon HostValuej,HostValuet;
LongUnon FValue,LonDbuf,BFValue;
LongUnon Fixvalue;		//Éè¶¨¹Ì¶¨Ïû·ÑÖµÊ¹ÓÃµÄ±äÁ¿


#define UART_BUFFER_SIZE			512

#if RUSHAN_BUS
#define UART_PACKET_LENGTH		128
#elif ZHEJIANG_ANJI
#define UART_PACKET_LENGTH		128
#else
#define UART_PACKET_LENGTH		7
#endif

#define UART_PACKET_LENGTH_ONE 30


#define UART_PACKET_DELIMITER		0x7F

unsigned int UARTWriteIndex;
unsigned int UARTReadIndex;
unsigned int UARTPacketIndex;
unsigned char UARTRxBuff[UART_BUFFER_SIZE];
unsigned char UARTPacket[UART_PACKET_LENGTH];
unsigned char UARTPacket_one[UART_PACKET_LENGTH_ONE];
unsigned char UARTHartBeat[]={0x7f,0x05,0x80,0x00,0x00,0x00,0x7f,0x7f};

extern unsigned char ReadCardFirst;
extern unsigned char *CardLanBuf;

// ±£³ÖÏÔÊ¾½»Ò×ÐÅÏ¢×´Ì¬
extern unsigned char LCDKeepDisp;
extern struct timeval LCDKeepStartTime;


unsigned char freedomflag;
unsigned char connecttime;


extern int mg_fd;

extern unsigned char disflag;

struct DALI_TransferMsg RcvData;
unsigned int g_MaxPersonNumber = 0;
unsigned int g_CurrentNumber = 0;
unsigned char g_FgSetPersonMode = 0;

extern struct RechargeInfo tempRechargedata;

#ifdef QINGDAO_TONGRUAN
extern LongUnon g_DisplayMoney;
#endif

#if defined(SUPPORT_QR_CODE)
unsigned int g_SendScanMarkCnt = 0;
unsigned char g_FgQRCodeRcvAck;
//unsigned char g_QRSendCmdTimes;
unsigned char g_QRCodeRcvDataFg;
static int QRCode_fd = -1;
struct QRCode G_QRCodeInfo;
unsigned char g_FgCardLanQRCode = 0;
#endif

extern LongUnon WatcherCard;
extern LongUnon SeDriverCard;
unsigned char g_DevNoInBus = 0; //±äÁ¿ÓÃÓÚ³µÉÏÖÕ¶Ë»ú±àºÅ£¬Ò»°ã³µÉÏÓÐ3¸ö³µÔØ»ú

unsigned char g_CallStationNo;


int CheckLineCardlanBin(unsigned char num1,unsigned char num2);
int UpdateLinePara(unsigned char num1,unsigned char num2);	//¸üÐÂÏû·Ñ²ÎÊý£¬ÓëÏßÂ·Í¬²½

int ChangeLine(void)
{
	FILE* temfile;
	struct stat file_info;
	char buffer[32];
	int len,num,i;
	int ret;	
	unsigned char Dfalg,ffalg;
	char str[3];
	unsigned char test[2];
	char templinenum;
	unsigned char ch;
	int allline[100];	//ËùÓÐµÄÏßÂ·±àºÅ
	int index=0;
	int TotalLine=0;
	

	system("ls /mnt/record/ | grep \"cardlan[0-9]\" > line.txt");
	stat("line.txt" , &file_info);
	system("cat line.txt");
	if(file_info.st_size!=0){
		
		len=sizeof("cardlan000.bin");
		num=file_info.st_size/len;
		temfile=fopen("line.txt","r");
		for(i=0;i<num;i++)
		{
			memset(buffer,0,32);
			fseek(temfile,i*len,SEEK_SET);
			fread(buffer,len,1,temfile);
			if((ret=mystrncmp(buffer,"cardlan999.bin",len-1))==0)	//Õâ¸ö²»ÊÇÏßÂ·ÎÄ¼þ
									continue;
			memcpy(str,buffer+7,3);
			templinenum=(str[0]-'0')*100+(str[1]-'0')*10+(str[2]-'0');
			allline[index]=templinenum;
			index++;
			TotalLine=index;
		}
		fclose(temfile);
	}
	else{
			TextOut(45,100,"Ã»ÓÐÏßÂ·ÎÄ¼þ");
			sleep(2);
			return 0;
	}

	system("rm line.txt");
	system("sync");
	Dfalg = 1;
	while(Dfalg)
	{
		SetColor(Mcolor);
        SetTextSize(32);
        SetTextColor(Color_white);
        TextOut(65,50, "ÏßÂ·ÇÐ»»Ä£Ê½");
		index=0;
		while(1)
		{
			memset(buffer,0,32);
			sprintf(buffer,"ÏßÂ·Ñ¡Ôñ:%03dÂ·",allline[index]);
            SetTextSize(32);
			TextOut(20,105,buffer);
			 do
	        {
	           ch = updatekey();
	         }while( ch == 0xff);
			
			switch(ch){
				case SCANCODE_ESCAPE:
									TextOut(20,105," ");
									TextOut(65,50," ");
									return 0;
									break;
				case  SCANCODE_ENTER:
									test[0]=(unsigned char)(allline[index]>>8);
									test[1]=(unsigned char)allline[index];
									printf("%x test1=%02x test2=%02x\n",allline[index],test[0],test[1]);
									ret=CheckLineCardlanBin(test[0],test[1]);
									if(ret==0){

										
										ReadOrWriteFile(SETSECTIONLINE);		//Ð´ÏµÍ³²ÎÊýÎÄ¼þ	
										ret=UpdateLinePara(test[0],test[1]);
										if(ret==0){
											PlayMusic(18,0);
											TextOut(20,105,"ÏßÂ·Ñ¡Ôñ³É¹¦");
										}
										else{
											PlayMusic(19,0);
											TextOut(20,105,"ÏßÂ·Ñ¡ÔñÊ§°Ü");
										}
										sleep(2);
									}
									else{
										TextOut(20,105,"ÏßÂ·Ñ¡ÔñÊ§°Ü");
										PlayMusic(19,0);
									}	
									break;
				case 	 SCANCODE_2 :
									if(index+1<TotalLine) index++;
									break; 
				case 	 SCANCODE_8 :
									if(index-1>=0)	index--;
									break;
					 	default		:					
									break;
			}
		}
	}
	
}


/*
*************************************************************************************************************
- º¯ÊýÃû³Æ : void ValueData(unsigned char *IN,unsigned char *Out)
- º¯ÊýËµÃ÷ : ½ð¶îÏÔÊ¾×ª»»
- ÊäÈë²ÎÊý :
- Êä³ö²ÎÊý : ÎÞ
*************************************************************************************************************
*/
void ValueData(unsigned char *IN,unsigned char *Out)
{
    unsigned char i,Qflag,j;
    Qflag = 1;
    j = 0;
    while(Qflag)
    {
        switch(j)
        {
        case 0:
            if((IN[0] =='0')&&(IN[1] =='.')&&(IN[2] =='0')&&(IN[3] =='0'))
            {
                Out[0] = '0';
                Qflag = 0;
            }
            else j++;
            break;

        case 1:
            if((IN[0] =='0')&&(IN[1] =='.'))
            {
                Out[0] = IN[0];
                Out[1] = IN[2];
                if(IN[3]==0x00)Out[2]=0x30;
                else Out[2] = IN[3];
                Qflag = 0;
            }
            else j++;
            break;

        case 2:
            for(i =0; i<strlen(IN); i++)
            {
                if(IN[i] == '.')break;
            }
            if((i == strlen(IN))||((i == (strlen(IN)-1))))
            {
                if(i == strlen(IN)) {
                    strcpy(Out,IN);
                    strcat(Out,"\x30\x30");
                }
                if(i == (strlen(IN)-1)) {
                    memcpy(Out,IN,strlen(IN)-1);
                    strcat(Out,"\x30\x30");
                }
                Qflag = 0;
            }
            else j++;
            break;
        case 3:
            for(i =0 ; i<strlen(IN); i++)
            {
                if(IN[i] != '.')Out[i] = IN[i];
                else
                {
                    memcpy(Out+i,IN+i+1,2);
                    if(Out[i+1] == 0)Out[i+1] = 0x30;
                    Qflag = 0;
                    break;
                }
            }
            j++;
            break;
        default:
            Qflag = 0;
            break;
        }
    }
}

void ReportCardType()
{
	DebugPrintf("\n");
    LongUnon Dislong;
	//static int tempInt = 0;

    if(CardLan.StationDEC == 0xaa)
    {

        if(Sector.FlagValue == 2)
        {
            memcpy(Dislong.longbuf,CardLan.QCash,4);
            Dislong.i = Dislong.i + HostValue.i;
        }
        else if(Sector.FlagValue == 1)
        {
            memcpy(Dislong.longbuf,CardLan.Subsidies,4);
            Dislong.i = Dislong.i + HostValue.i;
        }
        else
        {
            Dislong.i = 0;
        }
    }
    else
    {
        if(Sector.FlagValue == 2)
        {
            memcpy(Dislong.longbuf,CardLan.QCash,4);
            Dislong.i = Dislong.i - HostValue.i;
        }
        else if(Sector.FlagValue == 1)
        {
            memcpy(Dislong.longbuf,CardLan.Subsidies,4);
            Dislong.i = Dislong.i - HostValue.i;
        }
        else
        {
            Dislong.i = 0;
        }
    }

	printf("CardLan.CardType: %d \n",CardLan.CardType);

    if((FindCardValue(Dislong.i) == 0)&& (HostValue.i != 0)&& ((Sector.FlagValue ==1)|| (Sector.FlagValue == 2)))
    {
        // ÓïÒô ¿¨Àà
#if (!defined Transport_Stander ||defined ZHAOTONG_BUS||defined YAN_AN_BUS)
#ifdef SHAOYANG_PARK
		
		PlayMusic(1, 0);
#else
        
        if(CardLan.CardType <= 40) //9
        {
    #if  XIANGXIANG_BUS
		PlayMusic(CardLan.CardType, 0);
	#else
            if(CardLan.CardType>9)
                PlayMusic(CardLan.CardType+10, 0);
            else
                PlayMusic(CardLan.CardType, 0);
	#endif
        }
        else
        {
            PlayMusic(1, 0);
        }
#endif
		// µÈ´ý¿¨ÀàÓïÒô²¥·ÅÍê±Ï
		while(!PlayMusicFinishFlag)
			usleep(100*1000);
#endif
        // ÓïÒô Óà¶î²»¶à£¬Çë³äÖµ
        PlayMusic(14, 0);
    }
    else if(CardLan.CardType <= 40) //9
    {
        // ÓïÒô	   ¿¨Àà
#ifdef SHAOYANG_PARK
		
		PlayMusic(1, 0);
/*
		StopMusic();
		SumMoney = Dislong.i;
		showSunMoneyFLag = 1;
		tempInt++;
*/
#else
	#if  XIANGXIANG_BUS
	PlayMusic(CardLan.CardType, 0);
	#else
	if(CardLan.CardType>9)
            PlayMusic(CardLan.CardType+10, 0);
        else
            PlayMusic(CardLan.CardType, 0);
	#endif
#endif

    }
    else
	{
		
		PlayMusic(1, 0);
#if 0
#ifdef SHAOYANG_PARK
		StopMusic();

		SumMoney = Dislong.i;
		showSunMoneyFLag = 1;
#else
		PlayMusic(1, 0);
#endif
#endif
    }
}

/*
*************************************************************************************************************
- º¯ÊýÃû³Æ : unsigned char ReturnDisplay(int messcode)
- º¯ÊýËµÃ÷ : Ë¢ ¿¨ÏÔÊ¾
- ÊäÈë²ÎÊý :  0 ½»Ò×³É¹¦   1 Ë¢¿¨²éÑ¯³É¹¦
- Êä³ö²ÎÊý : ÎÞ
*************************************************************************************************************
*/
unsigned char bStolenDis = 0;
unsigned int StolenAmount;

unsigned char ReturnDisplay(int messcode)
{
	DebugPrintf("messcode = %d\n", messcode);
	char DisBuf1[80];
    char DisBuf2[80];
	char disbuff[60];
	char buff[20];
    char dig[5],dig1[5];
    char diglen;
	LongUnon Dislong;

	if(disflag)
    {
         beepopen(3);
		 SetColor(Mcolor);
		 SetTextColor(Color_red);
         #ifdef NEW0409
         SetTextSize(48);
		 TextOut(0 , 90,  "´æ´¢¼ÇÂ¼¼´½«Âú");
         #else
		 SetTextSize(32);
		 TextOut(50 , 90,  "´æ´¢¼ÇÂ¼¼´½«Âú");
         #endif
         sleep(1);
    }
    //ÏÔÊ¾Í¼Æ¬
  #if 1 
    if(Eflag)
       {
       	//change_background(2);
       // Show_BMP_(0, 0);    
        messcode = 9;
        }
   #endif


	SetColor(Mcolor);
	SetTextColor(Color_white);
    memset(dig,0xff,5);

#ifdef NEW0409
    switch(messcode)
    {
		case  0:
    		SetTextSize(48);
    		TextOut(0,40, "Ïû·Ñ³É¹¦");
			if((CardTypeIC == 0x08)||(use_jiaotong_stander!=0xAA))
    			{
                    sprintf(DisBuf1,"¿¨ºÅ:%02X%02X%02X%02X",CardLan.CardCsnB[0],\
            		CardLan.CardCsnB[1],CardLan.CardCsnB[2],CardLan.CardCsnB[3]);
                    TextOut(0,95,DisBuf1);
             }
			else
            {
                memset(DisBuf2,0,sizeof(DisBuf2));
                hex_2_ascii(&CardLanCPU.appserialnumber[0], DisBuf2, 10);
                memcpy(DisBuf1,&DisBuf2[1],19);
                TextOut(0,95,DisBuf1);
             }
			
			if((Sector.FlagValue == 0)||(Sector.FlagValue == 9))
    		{
				memset(DisBuf1,0,sizeof(DisBuf1));
				sprintf(DisBuf1,"Ïû·Ñ´ÎÊý:%d´Î",HostValue.i);
				TextOut(0,150,DisBuf1);
				memset(DisBuf1,0,sizeof(DisBuf1));
				memcpy(Dislong.longbuf,CardLan.Views,4);
				Dislong.i = Dislong.i - HostValue.i;
				sprintf(DisBuf1,"Ê£Óà´ÎÊý:%d´Î",Dislong.i);
				TextOut(0,205,DisBuf1);
    		}
    		else if(Sector.FlagValue == 1)
    		{
				memset(DisBuf1,0,sizeof(DisBuf1));
				strcpy(DisBuf1,"Ïû·Ñ½ð¶î:");
				MoneyValue(DisBuf1+9,HostValue.i);
#if NEWBUS  
	            MoneyValue1(dig,HostValue.i);              
	            write(mg_fd,dig,5);            //?
#endif      
				TextOut(0,150,DisBuf1);

				memset(DisBuf1,0,sizeof(DisBuf1));
				strcpy(DisBuf1,"²¹ÌùÓà¶î:");
				memcpy(Dislong.longbuf,CardLan.Subsidies,4);
				Dislong.i = Dislong.i - HostValue.i;
				MoneyValue(DisBuf1+9,Dislong.i);
				TextOut(0,205,DisBuf1);
    		}
    		else if(Sector.FlagValue == 2)
    		{
				if(bStolenDis)
				{			
					bStolenDis = 0;
					memset(DisBuf1,0,sizeof(DisBuf1));
					strcpy(DisBuf1,"Ïû·Ñ½ð¶î:");
					MoneyValue(DisBuf1+9,HostValue.i);
#if NEWBUS  
	                MoneyValue1(dig,HostValue.i);              
	                write(mg_fd,dig,5);
#endif          
					TextOut(0,95,DisBuf1);
				
					memset(DisBuf1,0,sizeof(DisBuf1));
					strcpy(DisBuf1,"ÆäÖÐ²¹¿Û:");
					MoneyValue(DisBuf1+9,StolenAmount);
					TextOut(0,150,DisBuf1);				
				}
				else
				{
					memset(DisBuf1,0,sizeof(DisBuf1));
					strcpy(DisBuf1,"Ïû·Ñ½ð¶î:");
					#if(defined QINGDAO_TONGRUAN)
					MoneyValue(DisBuf1+9,g_DisplayMoney.i);
					#else
					MoneyValue(DisBuf1+9,HostValue.i);
					#endif
#if NEWBUS      
	                MoneyValue1(dig,HostValue.i);              
	                write(mg_fd,dig,5);
#endif          
					TextOut(0,150,DisBuf1);				
				}
		
				memset(DisBuf1,0,sizeof(DisBuf1));
				strcpy(DisBuf1,"ÏÖ½ðÓà¶î:");
				if((CardTypeIC == 0x08)||(use_jiaotong_stander!=0xAA))
					{  
                        memcpy(Dislong.longbuf,CardLan.QCash,4);                        
						Dislong.i = Dislong.i - HostValue.i;                        
					}
				else{
                    #ifdef CANGNAN_BUS
                    Dislong.i = (CardLanCPU.beforemoney[0]<<24|CardLanCPU.beforemoney[1]<<16|CardLanCPU.beforemoney[2]<<8|CardLanCPU.beforemoney[3]);    
                    #else
					Dislong.i = (CardLanCPU.beforemoney[0]<<24|CardLanCPU.beforemoney[1]<<16|CardLanCPU.beforemoney[2]<<8|CardLanCPU.beforemoney[3])-HostValue.i;
                    #endif
					use_jiaotong_stander=0;
					}
				MoneyValue(DisBuf1+9,Dislong.i);
				TextOut(0,205,DisBuf1);
    		}
    		else if(Sector.FlagValue == 8)
    		{
				memset(DisBuf1,0,sizeof(DisBuf1));
				sprintf(DisBuf1,"¿ªÊ¼ÈÕÆÚ:%02X-%02X-%02X",CardLan.SMonth[0],CardLan.SMonth[1],CardLan.SMonth[2]);
				TextOut( 0,110,DisBuf1);
				memset(DisBuf1,0,sizeof(DisBuf1));
				sprintf(DisBuf1,"½áÊøÈÕÆÚ:%02X-%02X-%02X",CardLan.EMonth[0],CardLan.EMonth[1],CardLan.EMonth[2]);
				TextOut(0,160,DisBuf1);
    		}
    		TextOut( 0,260, "Ð»Ð»Ê¹ÓÃ");
    		ReportCardType();

    		break;

		case 1:
			SetTextSize(32);
			TextOut(0,40, "Ë¢¿¨³É¹¦");
			memset(disbuff,0,sizeof(disbuff));
			sprintf(disbuff,"ÓÃ»§¿¨ºÅ:%02X%02X%02X%02X",CardLan.CardCsnB[0],\
			        CardLan.CardCsnB[1],CardLan.CardCsnB[2],CardLan.CardCsnB[3]);
			TextOut( 0,80,disbuff);

			sprintf(disbuff,"ÓÃ»§¿¨Àà:%02X",CardLan.CardType);
			TextOut( 0,120,disbuff);

			memset(disbuff,0,sizeof(disbuff));
			memcpy(LonDbuf.longbuf,CardLan.Views,4);
			sprintf(disbuff,"Ê£Óà´ÎÊý:%d´Î",LonDbuf.i);
			TextOut(0,160,disbuff);

			memset(disbuff,0,sizeof(disbuff));
			memset(buff,0,sizeof(buff));
			sprintf(disbuff,"²¹ÌùÓà¶î:");
			memcpy(LonDbuf.longbuf,CardLan.Subsidies,4);
			MoneyValue(buff,LonDbuf.i);
			strcat(disbuff,buff);
			TextOut(0,200,disbuff);

			memset(disbuff,0,sizeof(disbuff));
			memset(buff,0,sizeof(buff));
			sprintf(disbuff,"ÏÖ½ðÓà¶î:");
			memcpy(LonDbuf.longbuf,CardLan.QCash,4);
			MoneyValue(buff,LonDbuf.i);
			strcat(disbuff,buff);
			TextOut(0,240,disbuff);

			memset(disbuff,0,sizeof(disbuff));
			sprintf(disbuff,"ÓÐÐ§ÆÚ:20%02X-%02X-%02X",CardLan.Effective[1],CardLan.Effective[2],CardLan.Effective[3]);
			TextOut(0,280,disbuff);
			ReportCardType();
    		break;

		case 2:
			SetTextSize(48);
			TextOut(0,40, "Ë¢¿¨³É¹¦");
			memset(disbuff,0,sizeof(disbuff));
			switch(CardLan.CardType)
			{
        	    //case 0xcc:
        	    case OPERATOR_DRIVER_CARD:
    				memset(disbuff,0,sizeof(disbuff));
					sprintf(disbuff,"¿¨Àà:Ë¾»ú¿¨");
					TextOut( 0,95,disbuff);

					memset(disbuff,0,sizeof(disbuff));

					Dislong.i = BCDToDec(SeDriverCard.longbuf, 4);
					sprintf(disbuff,"±àºÅ:%08dºÅ",Dislong.i);
					TextOut( 0,150,disbuff);

					memset(disbuff,0,sizeof(disbuff));
					sprintf(disbuff,"ÓÐÐ§ÆÚ:20%02X-%02X-%02X",CardLan.Effective[1],CardLan.Effective[2],CardLan.Effective[3]);
					TextOut(0,205,disbuff);

					// ÓïÒô
					PlayMusic(10, 0);
				break;
		case OPERATOR_WATCHER_CARD:
				memset(disbuff,0,sizeof(disbuff));
				sprintf(disbuff,"¿¨Àà:¼àÆ±Ô±¿¨");
				TextOut( 0,95,disbuff);

				memset(disbuff,0,sizeof(disbuff));
				Dislong.i = BCDToDec(WatcherCard.longbuf, 4);
				sprintf(disbuff,"±àºÅ:%08ºÅ", Dislong.i);
				TextOut( 0,150,disbuff);

				memset(disbuff,0,sizeof(disbuff));
				sprintf(disbuff,"ÓÐÐ§ÆÚ:20%02X-%02X-%02X",CardLan.Effective[1],CardLan.Effective[2],CardLan.Effective[3]);
				TextOut(0,205,disbuff);

				// ÓïÒô
				//PlayMusic(10, 0);
			break;

		case OPERATOR_CAPTURE_CARD:
			break;
			
    			default:
					memset(disbuff,0,sizeof(disbuff));
					sprintf(disbuff,"ÓÃ»§¿¨ºÅ:%02X%02X%02X%02X",CardLan.CardCsnB[0],\
					        CardLan.CardCsnB[1],CardLan.CardCsnB[2],CardLan.CardCsnB[3]);
					TextOut( 0,95,disbuff);

					memset(disbuff,0,sizeof(disbuff));
					memset(buff,0,sizeof(buff));
					sprintf(disbuff,"ÏÖ½ðÓà¶î:");
					memcpy(LonDbuf.longbuf,CardLan.QCash,4);
					MoneyValue(buff,LonDbuf.i);
					strcat(disbuff,buff);
					TextOut(0,150,disbuff);

					memset(disbuff,0,sizeof(disbuff));
					sprintf(disbuff,"ÓÐÐ§ÆÚ:20%02X-%02X-%02X",CardLan.Effective[1],CardLan.Effective[2],CardLan.Effective[3]);
					TextOut(0,205,disbuff);
				break;
    		}
    		break;

		case 3:
		SetTextSize(48);
#ifdef CANGNAN_BUS
        if((CardLan.EnterExitFlag == 0x01)|| CardLanCPU.enterexitflag == 0x55) //ÉÏ³µ    
#else
		if((CardLan.EnterExitFlag == 0x55)|| CardLanCPU.enterexitflag == 0x55) //ÉÏ³µ
#endif
        {
			TextOut(0,25, "ÉÏ³µË¢¿¨");
			
			if(use_jiaotong_stander!=0xAA){
			sprintf(DisBuf1,"¿¨ºÅ:%02X%02X%02X%02X",CardLan.CardCsnB[0],\
        			CardLan.CardCsnB[1],CardLan.CardCsnB[2],CardLan.CardCsnB[3]);
			}
			else{
				hex_2_ascii(&CardLanCPU.appserialnumber[2], DisBuf1, 8);
			}
			
			TextOut(0,80,DisBuf1);
			if(Sector.FlagValue == 1)	
			{
				memset(DisBuf1,0,sizeof(DisBuf1));
				strcpy(DisBuf1,"Ô¤¿Û½ð¶î:");
				MoneyValue(DisBuf1+9,HostValue.i);
#if NEWBUS    
	            MoneyValue1(dig,HostValue.i);              
	            write(mg_fd,dig,5);        
#endif          
				TextOut(0,135,DisBuf1);

				memset(DisBuf1,0,sizeof(DisBuf1));
				strcpy(DisBuf1,"²¹ÌùÓà¶î:");
				memcpy(Dislong.longbuf,CardLan.Subsidies,4);
				Dislong.i = Dislong.i - HostValue.i;
				MoneyValue(DisBuf1+9,Dislong.i);
				TextOut(0,190,DisBuf1);
			}
			else if(Sector.FlagValue == 2)
			{
    			memset(DisBuf1,0,sizeof(DisBuf1));
    			if(bStolenDis)
				{
				bStolenDis = 0;
				strcpy(DisBuf1,"²¹¿Û½ð¶î:");
				}
				else
				{
					strcpy(DisBuf1,"Ô¤¿Û½ð¶î:");
				}
				MoneyValue(DisBuf1+9,HostValue.i);
#if NEWBUS      
           		MoneyValue1(dig,HostValue.i);              
            	write(mg_fd,dig,5);
#endif          
				TextOut(0,135,DisBuf1);

				memset(DisBuf1,0,sizeof(DisBuf1));
				strcpy(DisBuf1,"ÏÖ½ðÓà¶î:");
				if(use_jiaotong_stander!=0xAA)		//²»ÊÇ½»Í¨²¿µÄ¿¨
				{
					memcpy(Dislong.longbuf,CardLan.QCash,4);
					Dislong.i = Dislong.i - HostValue.i;
				}
				else{
					Dislong.i = (CardLanCPU.beforemoney[0]<<24|CardLanCPU.beforemoney[1]<<16|CardLanCPU.beforemoney[2]<<8|CardLanCPU.beforemoney[3])-HostValue.i;
					use_jiaotong_stander=0;
				}
				MoneyValue(DisBuf1+9,Dislong.i);
				TextOut(0,190,DisBuf1);
			}
    	}
    		else		//ÈôÎªÏÂ³µ
    		{
        			TextOut(0,25, "ÏÂ³µË¢¿¨");
					if(use_jiaotong_stander!=0xAA)
					{
        				sprintf(DisBuf1,"¿¨ºÅ:%02X%02X%02X%02X",CardLan.CardCsnB[0],\
                			CardLan.CardCsnB[1],CardLan.CardCsnB[2],CardLan.CardCsnB[3]);
    				}
					else
						hex_2_ascii(&CardLanCPU.appserialnumber[2], DisBuf1, 8);
					
        			TextOut(0,80,DisBuf1);
        			if(CardLan.StationDEC == 0xaa)		//?
        			{
            			if(Sector.FlagValue == 1)
            			{
            				memset(DisBuf1,0,sizeof(DisBuf1));
							strcpy(DisBuf1,"²¹»Ø½ð¶î:");
							MoneyValue(DisBuf1+9,HostValue.i);
							TextOut(0,135,DisBuf1);

							memset(DisBuf1,0,sizeof(DisBuf1));
							strcpy(DisBuf1,"²¹ÌùÓà¶î:");
							memcpy(Dislong.longbuf,CardLan.Subsidies,4);
							Dislong.i = Dislong.i + HostValue.i;
							MoneyValue(DisBuf1+9,Dislong.i);
							TextOut(0,190,DisBuf1);
            			}
	        			else if(Sector.FlagValue == 2)
	        			{
							memset(DisBuf1,0,sizeof(DisBuf1));
							strcpy(DisBuf1,"²¹»Ø½ð¶î:");
							MoneyValue(DisBuf1+9,HostValue.i);
							TextOut(0,135,DisBuf1);

							memset(DisBuf1,0,sizeof(DisBuf1));
							strcpy(DisBuf1,"ÏÖ½ðÓà¶î:");
							memcpy(Dislong.longbuf,CardLan.QCash,4);
							Dislong.i = Dislong.i + HostValue.i;
							MoneyValue(DisBuf1+9,Dislong.i);
							TextOut(0,190,DisBuf1);
            			}
        			}
        			else
        			{
            			if(Sector.FlagValue == 1)
            			{
							memset(DisBuf1,0,sizeof(DisBuf1));
							strcpy(DisBuf1,"Ïû·Ñ½ð¶î:");
							MoneyValue(DisBuf1+9,HostValue.i);
#if NEWBUS         
							MoneyValue1(dig,HostValue.i);              
							write(mg_fd,dig,5);         
#endif           
							TextOut(0,135,DisBuf1);

							memset(DisBuf1,0,sizeof(DisBuf1));
							strcpy(DisBuf1,"²¹ÌùÓà¶î:");
							memcpy(Dislong.longbuf,CardLan.Subsidies,4);
							Dislong.i = Dislong.i - HostValue.i;
							MoneyValue(DisBuf1+9,Dislong.i);
							TextOut(0,190,DisBuf1);
            			}
            			else if(Sector.FlagValue == 2)
            			{
							memset(DisBuf1,0,sizeof(DisBuf1));
							strcpy(DisBuf1,"Ïû·Ñ½ð¶î:");
							MoneyValue(DisBuf1+9,HostValue.i);
#if NEWBUS          
		                    MoneyValue1(dig,HostValue.i);              
		                    write(mg_fd,dig,5);     
#endif              
							TextOut(0,135,DisBuf1);

							memset(DisBuf1,0,sizeof(DisBuf1));
							strcpy(DisBuf1,"ÏÖ½ðÓà¶î:");
							if(use_jiaotong_stander!=0xAA)	//²»Ê¹ÓÃ½»Í¨²¿¿¨
							{
								memcpy(Dislong.longbuf,CardLan.QCash,4);
								Dislong.i = Dislong.i - HostValue.i;
							}
							else{
								Dislong.i = (CardLanCPU.beforemoney[0]<<24|CardLanCPU.beforemoney[1]<<16|CardLanCPU.beforemoney[2]<<8|CardLanCPU.beforemoney[3])-HostValue.i;
								use_jiaotong_stander=0;
							}		
							MoneyValue(DisBuf1+9,Dislong.i);
							TextOut(0,190,DisBuf1);
		    			}
				}
			}            
		ReportCardType();
      
		break;

	case 5:
		SetColor(Mcolor);
		SetTextColor(Color_red);
		SetTextSize(32);
		TextOut(100 , 50, "ÎÂÜ°ÌáÊ¾");
		TextOut(85 , 90, "Éè±¸Î´ÈÏÖ¤");
		TextOut(25  , 130,"Çë´ýÁªÍøÈÏÖ¤ºóÊ¹ÓÃ!");
		break;

	case 7:
        SetTextSize(48);
		TextOut(0,25, "Ë¢¿¨³É¹¦");
		sprintf(DisBuf1,"¿¨ºÅ:%02X%02X%02X%02X",CardLan.CardCsnB[0],\
        		CardLan.CardCsnB[1],CardLan.CardCsnB[2],CardLan.CardCsnB[3]);
		TextOut(0,80,DisBuf1);

        memset(disbuff,0,sizeof(disbuff));
		memset(buff,0,sizeof(buff));
		sprintf(disbuff,"²¹ÌùÓà¶î:");
		memcpy(LonDbuf.longbuf,CardLan.Subsidies,4);
		MoneyValue(buff,LonDbuf.i);
		strcat(disbuff,buff);
		TextOut(0,135,disbuff);

		memset(disbuff,0,sizeof(disbuff));
		memset(buff,0,sizeof(buff));
		sprintf(disbuff,"ÏÖ½ðÓà¶î:");
		memcpy(LonDbuf.longbuf,CardLan.QCash,4);
		MoneyValue(buff,LonDbuf.i);
		strcat(disbuff,buff);
		TextOut(0,190,disbuff);
        
        memset(disbuff,0,sizeof(disbuff));
		sprintf(disbuff,"ÓÐÐ§ÆÚ:20%02X-%02X-%02X",CardLan.Effective[1],CardLan.Effective[2],CardLan.Effective[3]);
		TextOut(0,235,disbuff);
		//ReportCardType();        
		PlayMusic(18, 0);     //²éÑ¯³É¹¦
        break;
    case 8:
        SetTextSize(48);
		TextOut(0,25, "Ë¢¿¨³É¹¦");
		sprintf(DisBuf1,"¿¨ºÅ:%02X%02X%02X%02X",CardLan.CardCsnB[0],\
        		CardLan.CardCsnB[1],CardLan.CardCsnB[2],CardLan.CardCsnB[3]);
		TextOut(0,80,DisBuf1);

        memset(disbuff,0,sizeof(disbuff));
		memset(buff,0,sizeof(buff));
		sprintf(disbuff,"³äÖµ½ð¶î:");
		//memcpy(LonDbuf.longbuf,CardLan.Subsidies,4);
		MoneyValue(buff,HostValue.i);
		strcat(disbuff,buff);
		TextOut(0,135,disbuff);

		memset(disbuff,0,sizeof(disbuff));
		memset(buff,0,sizeof(buff));
        if(Sector.FlagValue == 1)
        {
            memset(DisBuf1,0,sizeof(DisBuf1));
			strcpy(DisBuf1,"³äºóÓà¶î:");
			memcpy(Dislong.longbuf,CardLan.Subsidies,4);
			Dislong.i = Dislong.i + HostValue.i;
			MoneyValue(DisBuf1+9,Dislong.i);
			TextOut(0,190,DisBuf1); ;            
            }
        else
        {
            memset(DisBuf1,0,sizeof(DisBuf1));
			strcpy(DisBuf1,"³äºóÓà¶î:");
			memcpy(Dislong.longbuf,CardLan.QCash,4);
			Dislong.i = Dislong.i + HostValue.i;
			MoneyValue(DisBuf1+9,Dislong.i);
			TextOut(0,190,DisBuf1);  
            }		
        
        memset(disbuff,0,sizeof(disbuff));
		sprintf(disbuff,"ÓÐÐ§ÆÚ:20%02X-%02X-%02X",CardLan.Effective[1],CardLan.Effective[2],CardLan.Effective[3]);
		TextOut(0,235,disbuff);
		//ReportCardType();
		PlayMusic(19, 0);     //³äÖµ³É¹¦

        break;
#ifdef SUPPORT_QR_CODE
	case QR_CODE_DISPLAY_TYPE:
		SetTextSize(48);
    		TextOut(0,50, "Ö§¸¶³É¹¦");
		memset(DisBuf1,0,sizeof(DisBuf1));
		strcpy(DisBuf1,"Ïû·Ñ½ð¶î:");
		MoneyValue(DisBuf1+9,HostValue.i);
		TextOut(0,105,DisBuf1);	
		TextOut( 0,150, "Ð»Ð»Ê¹ÓÃ");
    		//ReportCardType();
    		//if (g_FgCardLanQRCode)
    		PlayMusic(27, 0);  //Ö§¸¶³É¹¦
		break;
#endif
		 case 9:
			printf("-----------------messcode = %d\n",messcode);
			Show_Bmp(0,0,&bmppicture1);
			Set_Clear_Mode(0x05);
			
            SetTextSize(32);
        memset(DisBuf1,0,sizeof(DisBuf1));
		strcpy(DisBuf1,"Ïû·Ñ½ð¶î:");
		MoneyValue(DisBuf1+9,HostValue.i);
#if NEWBUS  
	            MoneyValue1(dig,HostValue.i);              
	            write(mg_fd,dig,5);            //?
#endif      
				//TextOut(60,15,DisBuf1);
		TextOut_CMD(30,200,DisBuf1,1);
        PlayMusic(40, 0);     //³äÖµ³É¹¦,ºóÃæÔö¼Ó²¥±¨¿¨Àà
        break;

	default :
		break;
	}

#else
	switch(messcode)
	{
		case  0:
    		SetTextSize(32);
    		TextOut(100,25, "Ïû·Ñ³É¹¦");
			if((CardTypeIC == 0x08)||(use_jiaotong_stander!=0xAA))
    			{
                    sprintf(DisBuf1,"¿¨ºÅ:%02X%02X%02X%02X",CardLan.CardCsnB[0],\
            		CardLan.CardCsnB[1],CardLan.CardCsnB[2],CardLan.CardCsnB[3]);
                    TextOut(30,70,DisBuf1);
             }
			else
            {
                memset(DisBuf2,0,sizeof(DisBuf2));
                hex_2_ascii(&CardLanCPU.appserialnumber[0], DisBuf2, 10);
                memcpy(DisBuf1,&DisBuf2[1],19);
                TextOut(10,70,DisBuf1);
             }
			
			if((Sector.FlagValue == 0)||(Sector.FlagValue == 9))
    		{
				memset(DisBuf1,0,sizeof(DisBuf1));
				sprintf(DisBuf1,"Ïû·Ñ´ÎÊý:%d´Î",HostValue.i);
				TextOut(30,110,DisBuf1);
				memset(DisBuf1,0,sizeof(DisBuf1));
				memcpy(Dislong.longbuf,CardLan.Views,4);
				Dislong.i = Dislong.i - HostValue.i;
				sprintf(DisBuf1,"Ê£Óà´ÎÊý:%d´Î",Dislong.i);
				TextOut(30,150,DisBuf1);
    		}
    		else if(Sector.FlagValue == 1)
    		{
				memset(DisBuf1,0,sizeof(DisBuf1));
				strcpy(DisBuf1,"Ïû·Ñ½ð¶î:");
				MoneyValue(DisBuf1+9,HostValue.i);
#if NEWBUS  
	            MoneyValue1(dig,HostValue.i);              
	            write(mg_fd,dig,5);            //?
#endif      
				TextOut(30,110,DisBuf1);

				memset(DisBuf1,0,sizeof(DisBuf1));
				strcpy(DisBuf1,"²¹ÌùÓà¶î:");
				memcpy(Dislong.longbuf,CardLan.Subsidies,4);
				Dislong.i = Dislong.i - HostValue.i;
				MoneyValue(DisBuf1+9,Dislong.i);
				TextOut(30,150,DisBuf1);
    		}
    		else if(Sector.FlagValue == 2)
    		{
				if(bStolenDis)
				{			
					bStolenDis = 0;
					memset(DisBuf1,0,sizeof(DisBuf1));
					strcpy(DisBuf1,"Ïû·Ñ½ð¶î:");
					MoneyValue(DisBuf1+9,HostValue.i);
#if NEWBUS  
	                MoneyValue1(dig,HostValue.i);              
	                write(mg_fd,dig,5);
#endif          
					TextOut(30,70,DisBuf1);
				
					memset(DisBuf1,0,sizeof(DisBuf1));
					strcpy(DisBuf1,"ÆäÖÐ²¹¿Û:");
					MoneyValue(DisBuf1+9,StolenAmount);
					TextOut(30,110,DisBuf1);				
				}
				else
				{
					memset(DisBuf1,0,sizeof(DisBuf1));
					strcpy(DisBuf1,"Ïû·Ñ½ð¶î:");
					#if(defined QINGDAO_TONGRUAN)
					MoneyValue(DisBuf1+9,g_DisplayMoney.i);
					#else
					MoneyValue(DisBuf1+9,HostValue.i);
					#endif
#if NEWBUS      
	                MoneyValue1(dig,HostValue.i);              
	                write(mg_fd,dig,5);
#endif          
					TextOut(30,110,DisBuf1);				
				}
		
				memset(DisBuf1,0,sizeof(DisBuf1));
				strcpy(DisBuf1,"ÏÖ½ðÓà¶î:");
				if((CardTypeIC == 0x08)||(use_jiaotong_stander!=0xAA))
					{  
                        memcpy(Dislong.longbuf,CardLan.QCash,4);                        
						Dislong.i = Dislong.i - HostValue.i;                        
					}
				else{
                    #ifdef CANGNAN_BUS
                    Dislong.i = (CardLanCPU.beforemoney[0]<<24|CardLanCPU.beforemoney[1]<<16|CardLanCPU.beforemoney[2]<<8|CardLanCPU.beforemoney[3]);    
                    #else
					Dislong.i = (CardLanCPU.beforemoney[0]<<24|CardLanCPU.beforemoney[1]<<16|CardLanCPU.beforemoney[2]<<8|CardLanCPU.beforemoney[3])-HostValue.i;
                    #endif
					use_jiaotong_stander=0;
					}
				MoneyValue(DisBuf1+9,Dislong.i);
				TextOut(30,150,DisBuf1);
    		}
    		else if(Sector.FlagValue == 8)
    		{
				memset(DisBuf1,0,sizeof(DisBuf1));
				sprintf(DisBuf1,"¿ªÊ¼ÈÕÆÚ:%02X-%02X-%02X",CardLan.SMonth[0],CardLan.SMonth[1],CardLan.SMonth[2]);
				TextOut( 20,110,DisBuf1);
				memset(DisBuf1,0,sizeof(DisBuf1));
				sprintf(DisBuf1,"½áÊøÈÕÆÚ:%02X-%02X-%02X",CardLan.EMonth[0],CardLan.EMonth[1],CardLan.EMonth[2]);
				TextOut(20,150,DisBuf1);
    		}
    		TextOut( 100,200, "Ð»Ð»Ê¹ÓÃ");
    		ReportCardType();

    		break;

		case 1:
			SetTextSize(16);
			TextOut(125,35, "Ë¢¿¨³É¹¦");
			memset(disbuff,0,sizeof(disbuff));
			sprintf(disbuff,"ÓÃ»§¿¨ºÅ:%02X%02X%02X%02X",CardLan.CardCsnB[0],\
			        CardLan.CardCsnB[1],CardLan.CardCsnB[2],CardLan.CardCsnB[3]);
			TextOut( 90,65,disbuff);

			sprintf(disbuff,"ÓÃ»§¿¨Àà:%02X",CardLan.CardType);
			TextOut( 90,95,disbuff);

			memset(disbuff,0,sizeof(disbuff));
			memcpy(LonDbuf.longbuf,CardLan.Views,4);
			sprintf(disbuff,"Ê£Óà´ÎÊý:%d´Î",LonDbuf.i);
			TextOut(90,125,disbuff);

			memset(disbuff,0,sizeof(disbuff));
			memset(buff,0,sizeof(buff));
			sprintf(disbuff,"²¹ÌùÓà¶î:");
			memcpy(LonDbuf.longbuf,CardLan.Subsidies,4);
			MoneyValue(buff,LonDbuf.i);
			strcat(disbuff,buff);
			TextOut(90,155,disbuff);

			memset(disbuff,0,sizeof(disbuff));
			memset(buff,0,sizeof(buff));
			sprintf(disbuff,"ÏÖ½ðÓà¶î:");
			memcpy(LonDbuf.longbuf,CardLan.QCash,4);
			MoneyValue(buff,LonDbuf.i);
			strcat(disbuff,buff);
			TextOut(90,185,disbuff);

			memset(disbuff,0,sizeof(disbuff));
			sprintf(disbuff,"ÓÐÐ§ÆÚ:20%02X-%02X-%02X",CardLan.Effective[1],CardLan.Effective[2],CardLan.Effective[3]);
			TextOut(90,215,disbuff);
			ReportCardType();
    		break;

		case 2:
			SetTextSize(32);
			TextOut(100,35, "Ë¢¿¨³É¹¦");
			memset(disbuff,0,sizeof(disbuff));
			switch(CardLan.CardType)
			{
        	    case 0xcc:
    				memset(disbuff,0,sizeof(disbuff));
					sprintf(disbuff,"¿¨Àà:Ë¾»ú¿¨");
					TextOut( 80,75,disbuff);

					memset(disbuff,0,sizeof(disbuff));
					sprintf(disbuff,"±àºÅ:%03dºÅ",Driver.i);
					TextOut( 80,115,disbuff);

					memset(disbuff,0,sizeof(disbuff));
					sprintf(disbuff,"ÓÐÐ§ÆÚ:20%02X-%02X-%02X",CardLan.Effective[1],CardLan.Effective[2],CardLan.Effective[3]);
					TextOut(30,155,disbuff);

					// ÓïÒô
					PlayMusic(10, 0);
				break;

    			default:
					memset(disbuff,0,sizeof(disbuff));
					sprintf(disbuff,"ÓÃ»§¿¨ºÅ:%02X%02X%02X%02X",CardLan.CardCsnB[0],\
					        CardLan.CardCsnB[1],CardLan.CardCsnB[2],CardLan.CardCsnB[3]);
					TextOut( 30,75,disbuff);

					memset(disbuff,0,sizeof(disbuff));
					memset(buff,0,sizeof(buff));
					sprintf(disbuff,"ÏÖ½ðÓà¶î:");
					memcpy(LonDbuf.longbuf,CardLan.QCash,4);
					MoneyValue(buff,LonDbuf.i);
					strcat(disbuff,buff);
					TextOut(30,115,disbuff);

					memset(disbuff,0,sizeof(disbuff));
					sprintf(disbuff,"ÓÐÐ§ÆÚ:20%02X-%02X-%02X",CardLan.Effective[1],CardLan.Effective[2],CardLan.Effective[3]);
					TextOut(30,155,disbuff);
				break;
    		}
    		break;

		case 3:
		SetTextSize(32);
#ifdef CANGNAN_BUS
        if((CardLan.EnterExitFlag == 0x01)|| CardLanCPU.enterexitflag == 0x55) //ÉÏ³µ    
#else
		if((CardLan.EnterExitFlag == 0x55)|| CardLanCPU.enterexitflag == 0x55) //ÉÏ³µ
#endif
        {
			TextOut(100,25, "ÉÏ³µË¢¿¨");
			
			if(use_jiaotong_stander!=0xAA){
			sprintf(DisBuf1,"¿¨ºÅ:%02X%02X%02X%02X",CardLan.CardCsnB[0],\
        			CardLan.CardCsnB[1],CardLan.CardCsnB[2],CardLan.CardCsnB[3]);
			}
			else{
				hex_2_ascii(&CardLanCPU.appserialnumber[2], DisBuf1, 8);
			}
			
			TextOut(30,70,DisBuf1);
			if(Sector.FlagValue == 1)	
			{
				memset(DisBuf1,0,sizeof(DisBuf1));
				strcpy(DisBuf1,"Ô¤¿Û½ð¶î:");
				MoneyValue(DisBuf1+9,HostValue.i);
#if NEWBUS    
	            MoneyValue1(dig,HostValue.i);              
	            write(mg_fd,dig,5);        
#endif          
				TextOut(30,110,DisBuf1);

				memset(DisBuf1,0,sizeof(DisBuf1));
				strcpy(DisBuf1,"²¹ÌùÓà¶î:");
				memcpy(Dislong.longbuf,CardLan.Subsidies,4);
				Dislong.i = Dislong.i - HostValue.i;
				MoneyValue(DisBuf1+9,Dislong.i);
				TextOut(30,150,DisBuf1);
			}
			else if(Sector.FlagValue == 2)
			{
    			memset(DisBuf1,0,sizeof(DisBuf1));
    			if(bStolenDis)
				{
				bStolenDis = 0;
				strcpy(DisBuf1,"²¹¿Û½ð¶î:");
				}
				else
				{
					strcpy(DisBuf1,"Ô¤¿Û½ð¶î:");
				}
				MoneyValue(DisBuf1+9,HostValue.i);
#if NEWBUS      
           		MoneyValue1(dig,HostValue.i);              
            	write(mg_fd,dig,5);
#endif          
				TextOut(30,110,DisBuf1);

				memset(DisBuf1,0,sizeof(DisBuf1));
				strcpy(DisBuf1,"ÏÖ½ðÓà¶î:");
				if(use_jiaotong_stander!=0xAA)		//²»ÊÇ½»Í¨²¿µÄ¿¨
				{
					memcpy(Dislong.longbuf,CardLan.QCash,4);
					Dislong.i = Dislong.i - HostValue.i;
				}
				else{
					Dislong.i = (CardLanCPU.beforemoney[0]<<24|CardLanCPU.beforemoney[1]<<16|CardLanCPU.beforemoney[2]<<8|CardLanCPU.beforemoney[3])-HostValue.i;
					use_jiaotong_stander=0;
				}
				MoneyValue(DisBuf1+9,Dislong.i);
				TextOut(30,150,DisBuf1);
			}
    	}
    		else		//ÈôÎªÏÂ³µ
    		{
        			TextOut(100,25, "ÏÂ³µË¢¿¨");
					if(use_jiaotong_stander!=0xAA)
					{
        				sprintf(DisBuf1,"¿¨ºÅ:%02X%02X%02X%02X",CardLan.CardCsnB[0],\
                			CardLan.CardCsnB[1],CardLan.CardCsnB[2],CardLan.CardCsnB[3]);
    				}
					else
						hex_2_ascii(&CardLanCPU.appserialnumber[2], DisBuf1, 8);
					
        			TextOut(30,70,DisBuf1);
        			if(CardLan.StationDEC == 0xaa)		//?
        			{
            			if(Sector.FlagValue == 1)
            			{
            				memset(DisBuf1,0,sizeof(DisBuf1));
							strcpy(DisBuf1,"²¹»Ø½ð¶î:");
							MoneyValue(DisBuf1+9,HostValue.i);
							TextOut(30,110,DisBuf1);

							memset(DisBuf1,0,sizeof(DisBuf1));
							strcpy(DisBuf1,"²¹ÌùÓà¶î:");
							memcpy(Dislong.longbuf,CardLan.Subsidies,4);
							Dislong.i = Dislong.i + HostValue.i;
							MoneyValue(DisBuf1+9,Dislong.i);
							TextOut(30,150,DisBuf1);
            			}
	        			else if(Sector.FlagValue == 2)
	        			{
							memset(DisBuf1,0,sizeof(DisBuf1));
							strcpy(DisBuf1,"²¹»Ø½ð¶î:");
							MoneyValue(DisBuf1+9,HostValue.i);
							TextOut(30,110,DisBuf1);

							memset(DisBuf1,0,sizeof(DisBuf1));
							strcpy(DisBuf1,"ÏÖ½ðÓà¶î:");
							memcpy(Dislong.longbuf,CardLan.QCash,4);
							Dislong.i = Dislong.i + HostValue.i;
							MoneyValue(DisBuf1+9,Dislong.i);
							TextOut(30,150,DisBuf1);
            			}
        			}
        			else
        			{
            			if(Sector.FlagValue == 1)
            			{
							memset(DisBuf1,0,sizeof(DisBuf1));
							strcpy(DisBuf1,"Ïû·Ñ½ð¶î:");
							MoneyValue(DisBuf1+9,HostValue.i);
#if NEWBUS         
							MoneyValue1(dig,HostValue.i);              
							write(mg_fd,dig,5);         
#endif           
							TextOut(30,110,DisBuf1);

							memset(DisBuf1,0,sizeof(DisBuf1));
							strcpy(DisBuf1,"²¹ÌùÓà¶î:");
							memcpy(Dislong.longbuf,CardLan.Subsidies,4);
							Dislong.i = Dislong.i - HostValue.i;
							MoneyValue(DisBuf1+9,Dislong.i);
							TextOut(30,150,DisBuf1);
            			}
            			else if(Sector.FlagValue == 2)
            			{
							memset(DisBuf1,0,sizeof(DisBuf1));
							strcpy(DisBuf1,"Ïû·Ñ½ð¶î:");
							MoneyValue(DisBuf1+9,HostValue.i);
#if NEWBUS          
		                    MoneyValue1(dig,HostValue.i);              
		                    write(mg_fd,dig,5);     
#endif              
							TextOut(30,110,DisBuf1);

							memset(DisBuf1,0,sizeof(DisBuf1));
							strcpy(DisBuf1,"ÏÖ½ðÓà¶î:");
							if(use_jiaotong_stander!=0xAA)	//²»Ê¹ÓÃ½»Í¨²¿¿¨
							{
								memcpy(Dislong.longbuf,CardLan.QCash,4);
								Dislong.i = Dislong.i - HostValue.i;
							}
							else{
								Dislong.i = (CardLanCPU.beforemoney[0]<<24|CardLanCPU.beforemoney[1]<<16|CardLanCPU.beforemoney[2]<<8|CardLanCPU.beforemoney[3])-HostValue.i;
								use_jiaotong_stander=0;
							}		
							MoneyValue(DisBuf1+9,Dislong.i);
							TextOut(30,150,DisBuf1);
		    			}
				}
			}            
		ReportCardType();
      
		break;

	case 5:
		SetColor(Mcolor);
		SetTextColor(Color_red);
		SetTextSize(32);
		TextOut(100 , 50, "ÎÂÜ°ÌáÊ¾");
		TextOut(85 , 90, "Éè±¸Î´ÈÏÖ¤");
		TextOut(25  , 130,"Çë´ýÁªÍøÈÏÖ¤ºóÊ¹ÓÃ!");
		break;
	
	case 7:
        SetTextSize(32);
		TextOut(100,25, "Ë¢¿¨³É¹¦");
		sprintf(DisBuf1,"¿¨ºÅ:%02X%02X%02X%02X",CardLan.CardCsnB[0],\
        		CardLan.CardCsnB[1],CardLan.CardCsnB[2],CardLan.CardCsnB[3]);
		TextOut(30,70,DisBuf1);

        memset(disbuff,0,sizeof(disbuff));
		memset(buff,0,sizeof(buff));
		sprintf(disbuff,"²¹ÌùÓà¶î:");
		memcpy(LonDbuf.longbuf,CardLan.Subsidies,4);
		MoneyValue(buff,LonDbuf.i);
		strcat(disbuff,buff);
		TextOut(30,110,disbuff);

		memset(disbuff,0,sizeof(disbuff));
		memset(buff,0,sizeof(buff));
		sprintf(disbuff,"ÏÖ½ðÓà¶î:");
		memcpy(LonDbuf.longbuf,CardLan.QCash,4);
		MoneyValue(buff,LonDbuf.i);
		strcat(disbuff,buff);
		TextOut(30,150,disbuff);
        
        memset(disbuff,0,sizeof(disbuff));
		sprintf(disbuff,"ÓÐÐ§ÆÚ:20%02X-%02X-%02X",CardLan.Effective[1],CardLan.Effective[2],CardLan.Effective[3]);
		TextOut(30,190,disbuff);
		//ReportCardType();        
		PlayMusic(18, 0);     //²éÑ¯³É¹¦
        break;
    case 8:
        SetTextSize(32);
		TextOut(100,25, "Ë¢¿¨³É¹¦");
		sprintf(DisBuf1,"¿¨ºÅ:%02X%02X%02X%02X",CardLan.CardCsnB[0],\
        		CardLan.CardCsnB[1],CardLan.CardCsnB[2],CardLan.CardCsnB[3]);
		TextOut(30,70,DisBuf1);

        memset(disbuff,0,sizeof(disbuff));
		memset(buff,0,sizeof(buff));
		sprintf(disbuff,"³äÖµ½ð¶î:");
		//memcpy(LonDbuf.longbuf,CardLan.Subsidies,4);
		MoneyValue(buff,HostValue.i);
		strcat(disbuff,buff);
		TextOut(30,110,disbuff);

		memset(disbuff,0,sizeof(disbuff));
		memset(buff,0,sizeof(buff));
        if(Sector.FlagValue == 1)
        {
            memset(DisBuf1,0,sizeof(DisBuf1));
			strcpy(DisBuf1,"³äºóÓà¶î:");
			memcpy(Dislong.longbuf,CardLan.Subsidies,4);
			Dislong.i = Dislong.i + HostValue.i;
			MoneyValue(DisBuf1+9,Dislong.i);
			TextOut(30,150,DisBuf1); ;            
            }
        else
        {
            memset(DisBuf1,0,sizeof(DisBuf1));
			strcpy(DisBuf1,"³äºóÓà¶î:");
			memcpy(Dislong.longbuf,CardLan.QCash,4);
			Dislong.i = Dislong.i + HostValue.i;
			MoneyValue(DisBuf1+9,Dislong.i);
			TextOut(30,150,DisBuf1);  
            }		
        
        memset(disbuff,0,sizeof(disbuff));
		sprintf(disbuff,"ÓÐÐ§ÆÚ:20%02X-%02X-%02X",CardLan.Effective[1],CardLan.Effective[2],CardLan.Effective[3]);
		TextOut(30,190,disbuff);
		//ReportCardType();
		PlayMusic(19, 0);     //³äÖµ³É¹¦

        break;
#ifdef SUPPORT_QR_CODE
	case QR_CODE_DISPLAY_TYPE:
		SetTextSize(32);
    		TextOut(100,70, "Ö§¸¶³É¹¦");
		memset(DisBuf1,0,sizeof(DisBuf1));
		strcpy(DisBuf1,"Ïû·Ñ½ð¶î:");
		MoneyValue(DisBuf1+9,HostValue.i);
		TextOut(30,110,DisBuf1);	
		TextOut( 100,150, "Ð»Ð»Ê¹ÓÃ");
    		//ReportCardType();
    		//if (g_FgCardLanQRCode)
    		PlayMusic(27, 0);  //Ö§¸¶³É¹¦
		break;
#endif
		 case 9:
			printf("-----------------messcode = %d\n",messcode);
			Show_Bmp(0,0,&bmppicture1);
			Set_Clear_Mode(0x05);
			
            SetTextSize(32);
        memset(DisBuf1,0,sizeof(DisBuf1));
		strcpy(DisBuf1,"Ïû·Ñ½ð¶î:");
		MoneyValue(DisBuf1+9,HostValue.i);
#if NEWBUS  
	            MoneyValue1(dig,HostValue.i);              
	            write(mg_fd,dig,5);            //?
#endif      
				//TextOut(60,15,DisBuf1);
		TextOut_CMD(30,200,DisBuf1,1);
        PlayMusic(40, 0);     //³äÖµ³É¹¦,ºóÃæÔö¼Ó²¥±¨¿¨Àà
        break;

	default :
		break;
	}

#endif    
	Set_Clear_Mode(0);

    	// ±£³ÖÏÔÊ¾½»Ò×ÐÅÏ¢×´Ì¬
	LCDKeepDisp = 1;
	
	gettimeofday(&LCDKeepStartTime, 0);
	CardTwo = 0;

	return 0;
}




void SaveDirverNumber(void)
{

    //memcpy(Driver.intbuf,CardLan.UserNum,2);
     
    ReadOrWriteFile(DRIVER);


}



/*
*************************************************************************************************************
- º¯ÊýÃû³Æ : static int DialogPrintferProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
- º¯ÊýËµÃ÷ : ÏÔÊ¾´òÓ¡½çÃæ
- ÊäÈë²ÎÊý : ÎÞ
- Êä³ö²ÎÊý : ÎÞ
*************************************************************************************************************
*/
/*
static int DialogPrintferProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
  switch(message)
  {
        case MSG_INITDIALOG:
	{
            return(1);
	}
        case MSG_CREATE:
        {
            SetTimer(hDlg,PRINTF_TIMER1,10);
            break;
        }
	case MSG_KEYDOWN:
	{
		switch(wParam)
		{
                         case SCANCODE_ENTER:
                         case SCANCODE_BACKSPACE:
			 KillTimer (hDlg, PRINTF_TIMER1);
	    		 PrintferInformation(SaveData);
                      	 SendMessage(hDlg,MSG_CLOSE,0, 0);
                     	 break;
                     default:
                       break;
                }
	}
	break;
        case MSG_TIMER:
        {
		SendMessage(hDlg, MSG_KEYDOWN, SCANCODE_ENTER, SCANCODE_ENTER);
        }
        break;
	case MSG_CLOSE:
             KillTimer (hDlg, PRINTF_TIMER1);
	     EndDialog (hDlg, wParam);
             DestroyAllControls (hDlg);
	     return 0;
       default :
             break;
   }
   return DefaultDialogProc (hDlg, message, wParam, lParam);
}
*/
/*
*************************************************************************************************************
- º¯ÊýÃû³Æ : int PrintferPro(HWND hWnd)
- º¯ÊýËµÃ÷ : Èý¼¶²Ëµ¥£­´òÓ¡Ñ¡Ïî
- ÊäÈë²ÎÊý : ÎÞ
- Êä³ö²ÎÊý : ÎÞ
*************************************************************************************************************
*/
/*
int PrintferPro(HWND hWnd)
{
   DlgPrintfer.controls = CtrlPrintfer;
   DialogBoxIndirectParam (&DlgPrintfer, hWnd, DialogPrintferProc, 0L);
   return 0;
}
*/
/*
*************************************************************************************************************
- º¯ÊýÃû³Æ : unsigned char ReadIPCard (char type)
- º¯ÊýËµÃ÷ : IPÉèÖÃ¿¨   WIFIÁ¬½Ó¿¨
- ÊäÈë²ÎÊý : ÎÞ
- Êä³ö²ÎÊý : ÎÞ
*************************************************************************************************************
*/
unsigned char ReadIPCard (char type)
{
    int flag,t;
    int ret;
    static int  receive_len[1] = {0};
    static char receive_buf[20]= {0};
    char buff[32];
    unsigned char Keybuf[8];
    unsigned char SELFProt[32];
    DebugPrintf("KeydesIP= %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\n",\
    KeyDes[0],KeyDes[1],KeyDes[2],KeyDes[3],KeyDes[4],KeyDes[5],KeyDes[6],KeyDes[7]);  //LanSec.Zer,
		
    memset(HttpProt,0,sizeof(HttpProt));
    memset(SELFProt,0,sizeof(SELFProt));
    test.mode = KEYA;
    memset(test.key,0xFF,6);
    memset(test.rwbuf,0xff,16);
    memset(test.money,0x00,4);
    unsigned char xor1,xor2;
    int ak0;
    LongUnon Buf;
    DebugPrintf("\n");
    flag = 1;
    t = 1;
    while(flag)
    {
        switch(t)
        {
        case 1:
            ret = ioctl(mf_fd, 0xAB);
            if(ret == MI_OK)t++;
            else
            {
                ret = ioctl(mf_fd, 0xAB);
                if(ret == MI_OK)t++;
                else flag = 0;
            }
            break;
        case 2:
            ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
            read(mf_fd, receive_buf, receive_len[0]);
            memcpy(Keybuf,receive_buf,4);
            #ifdef HANGJIA_BUS
            xor1 = Keybuf[0]^Keybuf[1]^Keybuf[2]^Keybuf[3];
            xor2 = Keybuf[0]^Keybuf[1]^Keybuf[2]^Keybuf[3]^xor1;
            memcpy(Buf.longbuf,Keybuf,4);
            ak0 = ~Buf.i;
            memcpy(test.key,&ak0,4);
            test.key[4] = xor1;
			test.key[5] = xor2;
            #else
            memcpy(Keybuf+4,SnBack,4);
            DES_CARD(KeyDes,Keybuf,test.key);
            #endif
            t++;
            break;
        case 3:
            ioctl(mf_fd, WRITE_TYPE, W_CHAR);
            ret = write(mf_fd, &test, sizeof(struct card_buf));
            if(ret == MI_OK)t++;
            else flag = 0;
            break;
        case 4:
            ret = ioctl(mf_fd, RC531_AUTHENT,7);
            if(ret == MI_OK)t++;
            else flag = 0;
            break;
        case 5:
            ret = ioctl(mf_fd, RC531_READ,6);
            if(ret == MI_OK)
            {
                ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
                read(mf_fd, receive_buf, receive_len[0]);
                t++;
            }
            else flag = 0;
            break;
        case 6:
            switch(type)
            {
            case 1:
                if(receive_buf[0] == 0xAB)
                {
                    memcpy(HttpProt+33,receive_buf+1,2);
                    t++;
                }
                else flag = 0;
                break;

            case 2:
                if(receive_buf[0] == 0xAC)
                {
                    t++;
                }
                else flag = 0;
                break;

            default :
                flag = 0;
                break;
            }
            break;

        case 7:
            ret = ioctl(mf_fd, RC531_READ,4);
            if(ret == MI_OK)
            {
                ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
                read(mf_fd, receive_buf, receive_len[0]);
                memcpy(HttpProt,receive_buf,16);
                t++;
            }
            else flag = 0;
            break;
        case 8:
            ret = ioctl(mf_fd, RC531_READ,5);
            if(ret == MI_OK)
            {
                ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
                read(mf_fd, receive_buf, receive_len[0]);
                memcpy(HttpProt+16,receive_buf,16);
                t++;
            }
            else flag = 0;
            break;
        case 9:
            if(type == 1)
            {
                ret = ioctl(mf_fd, RC531_AUTHENT,11);
                if(ret == MI_OK)t++;
                else flag = 0;
            }
            else
            {
                t = 12;
            }
            break;
        case 10:
            ret = ioctl(mf_fd, RC531_READ,9);
            if(ret == MI_OK)
            {
                ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
                read(mf_fd, receive_buf, receive_len[0]);
                memcpy(SELFProt,receive_buf,16);
                t++;
            }
            else flag = 0;
            break;
        case 11:
            ret = ioctl(mf_fd, RC531_READ,10);
            if(ret == MI_OK)
            {
                ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
                read(mf_fd, receive_buf, receive_len[0]);
                memcpy(SELFProt+16,receive_buf,16);
                t++;
            }
            else flag = 0;
            break;
        case 12:
            if(type == 1)
            {
                memcpy(TcpIpBuf,HttpProt+1,34);
                memset(SelfAddress,0,sizeof(SelfAddress));
                memcpy(SelfAddress,SELFProt,32);
                //while (FileOpenFlag == 0);
                //     FileOpenFlag = 0;
                ReadOrWriteFile (MSEVERIP);
                ReadOrWriteFile (SELFIP);
                // FileOpenFlag = 1;
                beepopen(2);
                t++;
            }
            else
            {
                memcpy(buff,HttpProt+1,32);
                nettab = fopen("tabneton.bin","rb+");
                ret = fseek(nettab,0, SEEK_SET);
                ret = fwrite(buff,sizeof(unsigned char),strlen(buff),nettab);
                fclose(nettab);
                beepopen(2);
                t++;
            }
            break;
        default:
            t = 0;
            flag = 0;
            break;
        }
    }
    DebugPrintf("t=%d\n",t);
    return(t);
}

/*
*************************************************************************************************************
- º¯ÊýÃû³Æ : unsigned char ReadKeyCard (void)
- º¯ÊýËµÃ÷ : ¶ÁÊÚÈ¨¿¨
- ÊäÈë²ÎÊý : ÎÞ
- Êä³ö²ÎÊý : ÎÞ
*************************************************************************************************************
*/
unsigned char ReadKeyCard (void)
{
    int flag,t;
    int ret;
    static int  receive_len[1] = {0};
    static char receive_buf[20]= {0};
    unsigned char Keybuf[8];
    test.mode = KEYA;
    memset(test.key,0xFF,6);
    memset(test.rwbuf,0xff,16);
    memset(test.money,0x00,4);
  DebugPrintf("\n");
    flag = 1;
    t = 1;
    while(flag)
    {
        switch(t)
        {
        case 1:
            ret = ioctl(mf_fd, 0xAB);
            if(ret == MI_OK)t++;
            else
            {
                ret = ioctl(mf_fd, 0xAB);
                if(ret == MI_OK)t++;
                else flag = 0;
            }
            break;
        case 2:
            ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
            read(mf_fd, receive_buf, receive_len[0]);
            memcpy(Keybuf,receive_buf,4);
            memcpy(Keybuf+4,SnBack,4);
            DES_CARD(SetDes,Keybuf,test.key);
            t++;
            break;
        case 3:
            ioctl(mf_fd, WRITE_TYPE, W_CHAR);
            ret = write(mf_fd, &test, sizeof(struct card_buf));
            if(ret == MI_OK)t++;
            else flag = 0;
            break;

        case 4:
            ret = ioctl(mf_fd, RC531_AUTHENT,7);
            if(ret == MI_OK)t++;
            else flag = 0;
            break;

        case 5:
            ret = ioctl(mf_fd, RC531_READ,4);
            if(ret == MI_OK)
            {
                ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
                read(mf_fd, receive_buf, receive_len[0]);
                t++;
            }
            else flag = 0;
            break;
        case 6:
            ret = mystrncmp(receive_buf,"\x55\xa0\xa1\xa2",4);
            if(ret == MI_OK)
            {
                memcpy(DesKeybuf,receive_buf+4,8);
                t++;
            }
            else flag = 0;
            break;
        case 7:
            ret = ioctl(mf_fd, RC531_AUTHENT,3);
            if(ret == MI_OK)t++;
            else flag = 0;
            break;
        case 8:
            ret = ioctl(mf_fd, RC531_READ,1);
            if(ret == MI_OK)
            {
                ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
                read(mf_fd, receive_buf, receive_len[0]);
                memcpy(UserKeyname,receive_buf+2,14);
                t++;
            }
            else flag = 0;
            break;

        case 9:
            memcpy(KeyDes,DesKeybuf,8);
            beepopen(2);
            //	while (FileOpenFlag == 0);
            //FileOpenFlag = 0;
            ReadOrWriteFile (MUSERKEY);
            //FileOpenFlag = 1;
            t++;
            break;

        default:
            t = 0;
            flag = 0;
            break;
        }
    }
    //ioctl(mf_fd, RC531_HALT);
    DebugPrintf("t=%d\n",t);
    return(t);
}


/*
*************************************************************************************************************
- º¯ÊýÃû³Æ : void UPdata_usb(void)
- º¯ÊýËµÃ÷ : Êý¾Ý³õÊ¼»¯  UÅÌÉý¼¶
- ÊäÈë²ÎÊý : ÎÞ
- Êä³ö²ÎÊý : ÎÞ
*************************************************************************************************************
*/
void UPdata_usb(void)
{
    char Disbuf[50];
    unsigned char status;
    unsigned char Loop=1;
    unsigned char step=2;
    unsigned char *pr;

    status = USB_Updata();
#ifdef NEW0409
     if(status == MI_OK)
    {
		system("sync");
        pr = (unsigned char *)(&USBFile);

#if  UARTDIS
        {
            unsigned char i;
            for(i=0; i<sizeof(Operat); i++)
            {
                printf("pr%d == %d \n",i,pr[i]);
            }

        }
#endif
        while(step < TASKTATOL)
        {
            step++;
            if(*pr == 0)
            {
                pr++;
            }
            else break;
        }


        while(Loop)
        {
            switch(step)
            {
            case 3:
                if(*pr == 0xaa)
                {
                    sprintf(Disbuf,"²ÎÊýÎÄ¼þ¸üÐÂ³É¹¦!");
                }
                else
                {
                    sprintf(Disbuf,"²ÎÊýÎÄ¼þ¸üÐÂÊ§°Ü!");
                }
                break;
            case 4:
                if(*pr == 0xaa)
                {
                    sprintf(Disbuf,"¸üÐÂºÚÃûµ¥³É¹¦!");
                }
                else
                {
                    sprintf(Disbuf,"¸üÐÂºÚÃûµ¥Ê§°Ü!");
                }
                break;
            case 5:
                if(*pr == 0xaa)
                {
                    sprintf(Disbuf,"ÉèÖÃ´òÓ¡Í·ÎÄ¼þ³É¹¦!");
                }
                else
                {
                    sprintf(Disbuf,"ÉèÖÃ´òÓ¡Í·ÎÄ¼þÊ§°Ü!");
                }
                break;
            case 6:
                if(*pr == 0xaa)
                {
                    sprintf(Disbuf,"ÉèÖÃ´òÓ¡Î²ÎÄ¼þ³É¹¦!");
                }
                else
                {
                    sprintf(Disbuf,"ÉèÖÃ´òÓ¡Î²ÎÄ¼þÊ§°Ü!");
                }
                break;
            case 7:
                if(*pr == 0xaa)
                {
                    sprintf(Disbuf,"²É¼¯Êý¾Ý³É¹¦!");
                }
                else
                {
                    sprintf(Disbuf,"²É¼¯Êý¾ÝÊ§°Ü!");
                }
                break;
            case 8:
                if(*pr == 0xaa)
                {
                    sprintf(Disbuf,"¸üÐÂÓïÒôÎÄ¼þ³É¹¦!");
                }
                else
                {
                    sprintf(Disbuf,"¸üÐÂÓïÒôÎÄ¼þÊ§°Ü!");
                }
                break;
            case 9:
                if(*pr == 0xaa)
                {
                    sprintf(Disbuf,"¸ñÊ½»¯Êý¾Ý³É¹¦!");
                }
                else
                {
                    sprintf(Disbuf,"¸ñÊ½»¯Êý¾ÝÊ§°Ü!");
                }
                break;

            case 10:
                if(*pr == 0xaa)
                {
                    sprintf(Disbuf,"Éý¼¶Íê³É!");
                }
                else
                {
                    sprintf(Disbuf,"Ã»ÓÐÕÒµ½Éý¼¶ÎÄ¼þ!");
                }
                break;

            case 11:
                SetColor(Mcolor);
                SetTextSize(48);
                if(*pr == 0xaa)
                {
                    TextOut(0,50,  "ÉèÖÃÖÕ¶ËÉÈÇø³É¹¦");
                    sprintf(Disbuf,"%02d_%02d_%02d_%02d_%02d_%02d_%02d",LanSec.One,LanSec.Two,\
                            LanSec.Thr,LanSec.For,LanSec.Fiv,LanSec.Six,LanSec.Sev);
                    TextOut(0,120,Disbuf);

                    memset(Disbuf,0,sizeof(Disbuf));
                    sprintf(Disbuf,"Ó¦ÓÃÎÄ¼þ:%02X%02X",LanSec.ADFNUM[0],LanSec.ADFNUM[1]);
                    TextOut(0,190,Disbuf);
                    sleep(1);
                }
                else
                {
                    TextOut(0,75,"ÎÂÜ°ÌáÊ¾");
                    TextOut(0,120,"ÉèÖÃÖÕ¶ËÉÈÇøÊ§°Ü");
                    sleep(1);
                }
                break;

            case 12:
                SetColor(Mcolor);
                SetTextSize(48);
                if(*pr == 0xaa)
                {
                    TextOut(0,75, "ÉèÖÃÖÕ¶Ë»úºÅ³É¹¦");
                    sprintf(Disbuf,"»úºÅ:%03d",DevNum.i);
                    TextOut(0,120,Disbuf);
                    sleep(1);
                }
                else
                {
                    TextOut(0,75,"ÎÂÜ°ÌáÊ¾");
                    TextOut(0,120,"ÉèÖÃÖÕ¶Ë»úºÅÊ§°Ü");
                    sleep(1);
                }
                break;
            case 13:
                if(*pr == 0xaa)
                {
                    sprintf(Disbuf,"·Ö¶ÎÉÏÐÐ²ÎÊý¸üÐÂ³É¹¦!");
                }
                else
                {
                    sprintf(Disbuf,"·Ö¶ÎÉÏÐÐ²ÎÊý¸üÐÂÊ§°Ü!");
                }
                break;

            case 14:
                if(*pr == 0xaa)
                {
                    sprintf(Disbuf,"·Ö¶ÎÏÂÐÐ²ÎÊý¸üÐÂ³É¹¦!");
                }
                else
                {
                    sprintf(Disbuf,"·Ö¶ÎÏÂÐÐ²ÎÊý¸üÐÂÊ§°Ü!");
                }
                break;

            default :
                Loop = 0;
                break;
            }
            if((step != 11)&&(step != 12)&&(step < TASKTATOL))
            {
                SetColor(Mcolor);
                SetTextSize(48);
                TextOut(0,75,"ÎÂÜ°ÌáÊ¾");
                TextOut(0 ,120,Disbuf);
                sleep(1);
            }
            pr++;
            while(step < TASKTATOL)
            {
                step ++;
                if(*pr == 0)
                {
                    pr ++;
                }
                else break;
            }
        }
    }
    else if(status == 1)
    {
        SetColor(Mcolor);
        SetTextSize(48);
        TextOut(0,75,  "ÎÂÜ°ÌáÊ¾");
        TextOut(0 ,144,"Î´ÕÒµ½UÅÌ!");
        TextOut(0 ,210,"Ð»Ð»Ê¹ÓÃ");
        beepopen(3);
        sleep(2);

    }
    else
    {
        SetColor(Mcolor);
        SetTextSize(48);
        TextOut(0,75,"ÎÂÜ°ÌáÊ¾");
        TextOut(0 ,144,"Ã»ÓÐÕÒµ½Òýµ¼ÎÄ¼þ!");
        TextOut(0 ,210,"Ð»Ð»Ê¹ÓÃ");
        beepopen(3);
        sleep(2);
    }

    if(status == 0)
    {
        SetColor(Mcolor);
        SetTextSize(48);
        TextOut(0,50,   "ÎÂÜ°ÌáÊ¾");
        TextOut(0 ,120,"UÅÌÊý¾Ý¸üÐÂÍê³É");
        TextOut(0 ,200, "Çë°Î³öUÅÌ");
#ifdef ZHUHAI_DUSHUGAO
        TextOut(85 ,160,"ÇëÖØÆô»úÆ÷");
		while(1){ sleep(1); };
#else
        TextOut(0 ,260,"Ð»Ð»Ê¹ÓÃ");
        beepopen(2);
        sleep(2);
#endif		
    }    


#else
    if(status == MI_OK)
    {
		system("sync");
        pr = (unsigned char *)(&USBFile);

#if  UARTDIS
        {
            unsigned char i;
            for(i=0; i<sizeof(Operat); i++)
            {
                printf("pr%d == %d \n",i,pr[i]);
            }

        }
#endif
        while(step < TASKTATOL)
        {
            step++;
            if(*pr == 0)
            {
                pr++;
            }
            else break;
        }


        while(Loop)
        {
            switch(step)
            {
            case 3:
                if(*pr == 0xaa)
                {
                    sprintf(Disbuf," ²ÎÊýÎÄ¼þ¸üÐÂ³É¹¦!");
                }
                else
                {
                    sprintf(Disbuf," ²ÎÊýÎÄ¼þ¸üÐÂÊ§°Ü!");
                }
                break;
            case 4:
                if(*pr == 0xaa)
                {
                    sprintf(Disbuf,"  ¸üÐÂºÚÃûµ¥³É¹¦!");
                }
                else
                {
                    sprintf(Disbuf,"  ¸üÐÂºÚÃûµ¥Ê§°Ü!");
                }
                break;
            case 5:
                if(*pr == 0xaa)
                {
                    sprintf(Disbuf,"ÉèÖÃ´òÓ¡Í·ÎÄ¼þ³É¹¦!");
                }
                else
                {
                    sprintf(Disbuf,"ÉèÖÃ´òÓ¡Í·ÎÄ¼þÊ§°Ü!");
                }
                break;
            case 6:
                if(*pr == 0xaa)
                {
                    sprintf(Disbuf,"ÉèÖÃ´òÓ¡Î²ÎÄ¼þ³É¹¦!");
                }
                else
                {
                    sprintf(Disbuf,"ÉèÖÃ´òÓ¡Î²ÎÄ¼þÊ§°Ü!");
                }
                break;
            case 7:
                if(*pr == 0xaa)
                {
                    sprintf(Disbuf,"   ²É¼¯Êý¾Ý³É¹¦!");
                }
                else
                {
                    sprintf(Disbuf,"   ²É¼¯Êý¾ÝÊ§°Ü!");
                }
                break;
            case 8:
                if(*pr == 0xaa)
                {
                    sprintf(Disbuf," ¸üÐÂÓïÒôÎÄ¼þ³É¹¦!");
                }
                else
                {
                    sprintf(Disbuf," ¸üÐÂÓïÒôÎÄ¼þÊ§°Ü!");
                }
                break;
            case 9:
                if(*pr == 0xaa)
                {
                    sprintf(Disbuf,"  ¸ñÊ½»¯Êý¾Ý³É¹¦!");
                }
                else
                {
                    sprintf(Disbuf,"  ¸ñÊ½»¯Êý¾ÝÊ§°Ü!");
                }
                break;

            case 10:
                if(*pr == 0xaa)
                {
                    sprintf(Disbuf,"    Éý¼¶Íê³É!");
                }
                else
                {
                    sprintf(Disbuf," Ã»ÓÐÕÒµ½Éý¼¶ÎÄ¼þ!");
                }
                break;

            case 11:
                SetColor(Mcolor);
                SetTextSize(32);
                if(*pr == 0xaa)
                {
                    TextOut(55,70,  "ÉèÖÃÖÕ¶ËÉÈÇø³É¹¦");
                    sprintf(Disbuf,"%02d_%02d_%02d_%02d_%02d_%02d_%02d",LanSec.One,LanSec.Two,\
                            LanSec.Thr,LanSec.For,LanSec.Fiv,LanSec.Six,LanSec.Sev);
                    TextOut(0,120,Disbuf);

                    memset(Disbuf,0,sizeof(Disbuf));
                    sprintf(Disbuf,"Ó¦ÓÃÎÄ¼þ:%02X%02X",LanSec.ADFNUM[0],LanSec.ADFNUM[1]);
                    TextOut(0,150,Disbuf);
                    sleep(1);
                }
                else
                {
                    TextOut(100,70,"ÎÂÜ°ÌáÊ¾");
                    TextOut(55,120,"ÉèÖÃÖÕ¶ËÉÈÇøÊ§°Ü");
                    sleep(1);
                }
                break;

            case 12:
                SetColor(Mcolor);
                SetTextSize(32);
                if(*pr == 0xaa)
                {
                    TextOut(55,70, "ÉèÖÃÖÕ¶Ë»úºÅ³É¹¦");
                    sprintf(Disbuf,"»úºÅ:%03d",DevNum.i);
                    TextOut(65,120,Disbuf);
                    sleep(1);
                }
                else
                {
                    TextOut(100,70,"ÎÂÜ°ÌáÊ¾");
                    TextOut(55,120,"ÉèÖÃÖÕ¶Ë»úºÅÊ§°Ü");
                    sleep(1);
                }
                break;
            case 13:
                if(*pr == 0xaa)
                {
                    sprintf(Disbuf," ·Ö¶ÎÉÏÐÐ²ÎÊý¸üÐÂ³É¹¦!");
                }
                else
                {
                    sprintf(Disbuf," ·Ö¶ÎÉÏÐÐ²ÎÊý¸üÐÂÊ§°Ü!");
                }
                break;

            case 14:
                if(*pr == 0xaa)
                {
                    sprintf(Disbuf," ·Ö¶ÎÏÂÐÐ²ÎÊý¸üÐÂ³É¹¦!");
                }
                else
                {
                    sprintf(Disbuf," ·Ö¶ÎÏÂÐÐ²ÎÊý¸üÐÂÊ§°Ü!");
                }
                break;

            default :
                Loop = 0;
                break;
            }
            if((step != 11)&&(step != 12)&&(step < TASKTATOL))
            {
                SetColor(Mcolor);
                SetTextSize(32);
                TextOut(100,70,"ÎÂÜ°ÌáÊ¾");
                TextOut(20 ,120,Disbuf);
                sleep(1);
            }
            pr++;
            while(step < TASKTATOL)
            {
                step ++;
                if(*pr == 0)
                {
                    pr ++;
                }
                else break;
            }
        }
    }
    else if(status == 1)
    {
        SetColor(Mcolor);
        SetTextSize(32);
        TextOut(100,50,  "ÎÂÜ°ÌáÊ¾");
        TextOut(80 ,100,"Î´ÕÒµ½UÅÌ!");
        TextOut(100 ,150,"Ð»Ð»Ê¹ÓÃ");
        beepopen(3);
        sleep(2);

    }
    else
    {
        SetColor(Mcolor);
        SetTextSize(32);
        TextOut(100,50,"ÎÂÜ°ÌáÊ¾");
        TextOut(20 ,100," Ã»ÓÐÕÒµ½Òýµ¼ÎÄ¼þ!");
        TextOut(100 ,150,"Ð»Ð»Ê¹ÓÃ");
        beepopen(3);
        sleep(2);
    }

    if(status == 0)
    {
        SetColor(Mcolor);
        SetTextSize(32);
        TextOut(100,40,   "ÎÂÜ°ÌáÊ¾");
        TextOut(40 ,80,"UÅÌÊý¾Ý¸üÐÂÍê³É");
        TextOut(90 ,120, "Çë°Î³öUÅÌ");
#ifdef ZHUHAI_DUSHUGAO
        TextOut(85 ,160,"ÇëÖØÆô»úÆ÷");
		while(1){ sleep(1); };
#else
        TextOut(100 ,160,"Ð»Ð»Ê¹ÓÃ");
        beepopen(2);
        sleep(2);
#endif		
    }
#endif

    SetColor(Mcolor);
}







/*
*************************************************************************************************************
- º¯ÊýÃû³Æ : unsigned char InitData(void)
- º¯ÊýËµÃ÷ : Êý¾Ý³õÊ¼»¯  SD¿¨Éý¼¶ ¡¢ ¶ÁIPÉèÖÃ¿¨ ¡¢¶ÁÊÚÈ¨¿¨¡¢¶ÁWIFIÉèÖÃ¿¨¡¢
- ÊäÈë²ÎÊý : ÎÞ
- Êä³ö²ÎÊý : ÎÞ
*************************************************************************************************************
*/
unsigned char InitData(void)
{
    unsigned char Loop,status;
    char Disbuf[50];

    /*
        status = UPdata_SD();
        if(status == MI_OK)
        {
            SetColor(Mcolor);
            SetTextSize(16);
            TextOut( 50 , 60,"       ÎÂÜ°ÌáÊ¾");
            TextOut(50 , 90, "    ÎÄ¼þ¸üÐÂ³É¹¦ºó");
            TextOut(50 , 120,"Çë¹Ø»úÈ¡³öSD¿¨ºó½øÈëÏµÍ³");
            TextOut(50 ,150, "       Ð»Ð»Ê¹ÓÃ");

            while(1)
            {
                sleep(1);
            }
        }

    */
#ifdef NEW0409
    Loop = ioctl(mf_fd, 0xAB);
    if(Loop == 0)
    {
        char buff[32];
        unsigned char i;

        status = ReadIPCard(1);
        if(status ==  MI_OK)
        {
        	system("sync;");
            SetColor(Mcolor);
            SetTextSize(32);             
            TextOut(0 , 40, "Ë¢¿¨³É¹¦");
            TextOut(0 , 85, "IPÉèÖÃ¿¨");
            sprintf(Disbuf," ·þÎñÆ÷µØÖ·:%s\n",TcpIpBuf);
            TextOut(1 , 130, Disbuf);
            memcpy(Infor.intbuf,TcpIpBuf+32,2);
            sprintf(Disbuf," ·þÎñÆ÷¶Ë¿Ú:%05d",Infor.i);
            TextOut(1 , 175, Disbuf);
            sprintf(Disbuf," ±¾»úµØÖ·:%s\n",SelfAddress);
            TextOut(1 , 220, Disbuf);
            sprintf(Disbuf," Ä¬ÈÏÍø¹Ø:%s\n",SelfAddress+16);
            TextOut(1 , 265, Disbuf);
            ioctl(mf_fd, RC531_HALT);
            sleep(5);
            Loop = 1;
        }

        status = ReadIPCard(2);
        if(status ==  MI_OK)
        {
			system("sync;");
            SetColor(Mcolor);
            SetTextSize(32);
            TextOut(0 , 70, "           Ë¢¿¨³É¹¦");
            TextOut(0 , 115, "         WIFIIDÉèÖÃ¿¨");
            for(i = 1; i < 32; i++)
            {
                if(HttpProt[i] == 0x20)
                {
                    break;
                }
            }
            if(i != 32)
            {
                memset(buff,0,sizeof(buff));
                memcpy(buff,HttpProt+2,i-2);

                sprintf(Disbuf,"WIFIID:%s ",buff);
                TextOut(60,120, Disbuf);
                switch(HttpProt[1])
                {
                case '4':
                    TextOut(0 , 160, "Á¬½Ó·½Ê½£º GPRS");
                    break;

                case '3':
                    TextOut(0 , 160, "Á¬½Ó·½Ê½£º CDMA");
                    break;

                case '2':
                    TextOut(0 , 160, "Á¬½Ó·½Ê½£º WIFI");
                    break;

                case '1':
                default:
                    TextOut(0 , 160, "Á¬½Ó·½Ê½£º TCP/IP");
                    break;
                }
            }
            else
            {
                TextOut(0 , 160, "WIFIID ´íÎó!");

            }
            ioctl(mf_fd, RC531_HALT);
            sleep(5);
            Loop = 1;
        }
        status = ReadKeyCard();
        if(status ==  MI_OK)
        {
        	system("sync;");
            SetColor(Mcolor);
            SetTextSize(32);
            TextOut( 0 , 40, "Ë¢¿¨³É¹¦");
            TextOut( 0 , 85, "ÏµÍ³ÊÚÈ¨¿¨");
            sprintf(Disbuf,"ÓÃ»§Ãû: %s\n",UserKeyname);
            TextOut( 0 , 130, Disbuf);
            TextOut( 0 , 175,"¹§Ï²ÄúÊÚÈ¨³É¹¦!");
            ioctl(mf_fd, RC531_HALT);
            sleep(5);
            Loop = 1;
        }
        if(Loop == 1)
        {
            SetColor(Mcolor);
            SetTextSize(32);
            TextOut( 0 , 60, "»¶Ó­Ê¹ÓÃ±¾ÏµÍ³");
            TextOut( 0 , 105, "ÌáÊ¾Äú£ºÉèÖÃÍê³ÉºóÖØÆô½øÈëÏµÍ³");
            TextOut( 0 ,150, "Ð»Ð»Ê¹ÓÃ");
            while(1)
            {
                sleep(1);
            }
        }
    }
    
#else
    Loop = ioctl(mf_fd, 0xAB);
    if(Loop == 0)
    {
        char buff[32];
        unsigned char i;

        status = ReadIPCard(1);
        if(status ==  MI_OK)
        {
        	system("sync;");
            SetColor(Mcolor);
            SetTextSize(16);
            TextOut(50 , 50, "        Ë¢¿¨³É¹¦");
            TextOut(50 , 80, "        IPÉèÖÃ¿¨");
            sprintf(Disbuf," ·þÎñÆ÷µØÖ·:%s\n",TcpIpBuf);
            TextOut(50 , 110, Disbuf);
            memcpy(Infor.intbuf,TcpIpBuf+32,2);
            sprintf(Disbuf," ·þÎñÆ÷¶Ë¿Ú:%05d",Infor.i);
            TextOut(50 , 140, Disbuf);
            sprintf(Disbuf," ±¾»úµØÖ·:%s\n",SelfAddress);
            TextOut(50 , 165, Disbuf);
            sprintf(Disbuf," Ä¬ÈÏÍø¹Ø:%s\n",SelfAddress+16);
            TextOut(50 , 190, Disbuf);
            ioctl(mf_fd, RC531_HALT);
            sleep(5);
            Loop = 1;
        }

        status = ReadIPCard(2);
        if(status ==  MI_OK)
        {
			system("sync;");
            SetColor(Mcolor);
            SetTextSize(16);
            TextOut(20 , 70, "           Ë¢¿¨³É¹¦");
            TextOut(50 , 95, "         WIFIIDÉèÖÃ¿¨");
            for(i = 1; i < 32; i++)
            {
                if(HttpProt[i] == 0x20)
                {
                    break;
                }
            }
            if(i != 32)
            {
                memset(buff,0,sizeof(buff));
                memcpy(buff,HttpProt+2,i-2);

                sprintf(Disbuf,"WIFIID:%s ",buff);
                TextOut(60,120, Disbuf);
                switch(HttpProt[1])
                {
                case '4':
                    TextOut(60 , 145, "Á¬½Ó·½Ê½£º GPRS");
                    break;

                case '3':
                    TextOut(60 , 145, "Á¬½Ó·½Ê½£º CDMA");
                    break;

                case '2':
                    TextOut(60 , 145, "Á¬½Ó·½Ê½£º WIFI");
                    break;

                case '1':
                default:
                    TextOut(60 , 145, "Á¬½Ó·½Ê½£º TCP/IP");
                    break;
                }
            }
            else
            {
                TextOut(60 , 170, "WIFIID ´íÎó!");

            }
            ioctl(mf_fd, RC531_HALT);
            sleep(5);
            Loop = 1;
        }
        status = ReadKeyCard();
        if(status ==  MI_OK)
        {
        	system("sync;");
            SetColor(Mcolor);
            SetTextSize(16);
            TextOut( 20 , 50, "            Ë¢¿¨³É¹¦");
            TextOut( 20 , 80, "           ÏµÍ³ÊÚÈ¨¿¨");
            sprintf(Disbuf,"         ÓÃ»§Ãû: %s\n",UserKeyname);
            TextOut( 20 , 110, Disbuf);
            TextOut( 20 , 140,"          ¹§Ï²ÄúÊÚÈ¨³É¹¦!");
            ioctl(mf_fd, RC531_HALT);
            sleep(5);
            Loop = 1;
        }
        if(Loop == 1)
        {
            SetColor(Mcolor);
            SetTextSize(16);
            TextOut( 20 , 80, "           »¶Ó­Ê¹ÓÃ±¾ÏµÍ³");
            TextOut( 20 , 110, "   ÌáÊ¾Äú£ºÉèÖÃÍê³ÉºóÖØÆô½øÈëÏµÍ³");
            TextOut( 20 ,140, "            Ð»Ð»Ê¹ÓÃ");
            while(1)
            {
                sleep(1);
            }
        }
    }
    
#endif
    return 0;
}


/*
*************************************************************************************************************
- º¯ÊýÃû³Æ : char StrDian(char *Data)
- º¯ÊýËµÃ÷ : ÊäÈëÐ¡ÊýµãÅÐ¶Ï
- ÊäÈë²ÎÊý :
- Êä³ö²ÎÊý :
*************************************************************************************************************
*/
char StrDian(char *Data)
{
    unsigned char i,j;
    if(Data[0]=='.')return 1;
    j = 0;
    for(i=0; i<strlen(Data); i++)
    {
        if(Data[i]=='.')j++;
    }
    if(j>=2)return 1;
    else return 0;
}

/*
*************************************************************************************************************
- º¯ÊýÃû³Æ : char Input(int x,int y,char *prompt,unsigned char len,char *str,unsigned char flag,unsigned char style)
- º¯ÊýËµÃ÷ :  ¼üÅÌÊäÈëº¯Êý
- ÊäÈë²ÎÊý :
- Êä³ö²ÎÊý :
*************************************************************************************************************
*/
char Input(int x,int y,char *prompt,unsigned char len,char *str,unsigned char flag,unsigned char style)
{
    unsigned char ch;
    unsigned char plen=0,tmplen;

    char c[2];
    char csn[10];
    char tmp[100];
    char PcText[100];
    char PcTextb[100];
    char GL_USB_Tmp[100];
#ifdef NEW0409
    memset(tmp,0,sizeof(tmp));
    memset(PcText,0,sizeof(PcText));
    memset(GL_USB_Tmp,0,sizeof(GL_USB_Tmp));
    strcpy(tmp,prompt);
    strcpy(GL_USB_Tmp,prompt);
    plen = (char)strlen(prompt);
    memcpy((char *)&tmp[plen],PcText,len);
    memset(csn,0,sizeof(csn));
    if(flag == 0)
    {
        SetTextSize(16);
        TextOut(x,y,tmp);
    }
    else
    {
        SetTextSize(48);
        TextOut(x,y,tmp);
    }

    while(1)
    {
        switch(style)
        {
        case DATE_STYLE1:
            sprintf(PcText,"%-4.4sÄê%-2.2sÔÂ%-2.2sÈÕ",
                    tmp+plen,
                    tmp+plen+4,
                    tmp+plen+6);

            sprintf(PcTextb,"  %-2.2sÊ±%-2.2s·Ö%-2.2sÃë",
                    tmp+plen+8,
                    tmp+plen+10,
                    tmp+plen+12);
            SetTextSize(48);
            TextOut(x,y,PcText);
            TextOut(x,y+48,PcTextb);
            break;

        case DATE_STYLE2:

            break;

        case DATE_STYLE3:
            sprintf(PcText,"%-7.7sÔª",tmp+plen);
            if(flag == 0)
            {
                SetTextSize(16);
                TextOut(x+plen*16,y,PcText);
            }
            else
            {
                SetTextSize(48);
                TextOut(x+plen*24,y,PcText);
            }

            break;
	case DATE_STYLE4:
	  sprintf(PcText,"ÈËÊýÉèÎª:%-7.7s¸ö",tmp+plen);
                SetTextSize(48);
               // TextOut(x+plen*16,y,PcText);
               TextOut(x, y, PcText);
		break;
        default:
            if(flag == 0)
            {
                SetTextSize(16);
                TextOut(x,y,tmp);
            }
            else
            {
                SetTextSize(48);
                TextOut(x,y,tmp);
            }
            break;

        }
		
        do
        {
            ch = updatekey();
        }
        while( ch == 0xff);

        switch(ch)
        {
        #if 0
        case  SCANCODE_F1:
		if (g_FgSetPersonMode)
			g_FgSetPersonMode = 0;
		else
			g_FgSetPersonMode = 1;
		return 1;
		break;
	#endif
        case  SCANCODE_ESCAPE:
            return 1;
            break;

        case SCANCODE_ENTER:
            if(strlen(tmp) <= strlen(prompt)+len )
            {
                memcpy(str,tmp+strlen(prompt),len);
                if(StrDian(str)==MI_OK)return 0;
                else
                {
                    memset(tmp,0,sizeof(tmp));
                    memset(GL_USB_Tmp,0,sizeof(GL_USB_Tmp));
                    strcpy(tmp,prompt);
                    strcpy(GL_USB_Tmp,prompt);
                    str[0] = '\x00';
                }
            }

            break;

        case SCANCODE_BACKSPACE:
            //if(strlen(tmp) == plen)return 1;
            if(strlen(tmp) > strlen(prompt) )
            {
                tmp[strlen(tmp)-1]=0;
                GL_USB_Tmp[strlen(GL_USB_Tmp)-1]=0;
            }
            break;
        case SCANCODE_PERIOD:
            if(flag) ch='.';
            break;
        case SCANCODE_0:
            ch='0';
            break;
        case SCANCODE_1:
            ch='1';
            break;
        case SCANCODE_2:
            ch='2';
            break;
        case SCANCODE_3:
            ch='3';
            break;
        case SCANCODE_4:
            ch='4';
            break;
        case SCANCODE_5:
            ch='5';
            break;
        case SCANCODE_6:
            ch='6';
            break;
        case SCANCODE_7:
            ch='7';
            break;
        case SCANCODE_8:
            ch='8';
            break;
        case SCANCODE_9:
            ch='9';
            break;
        default :
            break;
        }

        if(isdigit(ch)||ch=='.')
        {
            tmplen = strlen(tmp);
            if( tmplen != (plen+len) )
            {
                c[0]=ch;
                c[1]=0;
                strcat(tmp,c);
                strcat(GL_USB_Tmp,"*");
            }
        }
    }
#else
    memset(tmp,0,sizeof(tmp));
    memset(PcText,0,sizeof(PcText));
    memset(GL_USB_Tmp,0,sizeof(GL_USB_Tmp));
    strcpy(tmp,prompt);
    strcpy(GL_USB_Tmp,prompt);
    plen = (char)strlen(prompt);
    memcpy((char *)&tmp[plen],PcText,len);
    memset(csn,0,sizeof(csn));
    if(flag == 0)
    {
        SetTextSize(16);
        TextOut(x,y,tmp);
    }
    else
    {
        SetTextSize(32);
        TextOut(x,y,tmp);
    }

    while(1)
    {
        switch(style)
        {
        case DATE_STYLE1:
            sprintf(PcText,"%-4.4sÄê%-2.2sÔÂ%-2.2sÈÕ",
                    tmp+plen,
                    tmp+plen+4,
                    tmp+plen+6);

            sprintf(PcTextb,"%-2.2sÊ±%-2.2s·Ö%-2.2sÃë",
                    tmp+plen+8,
                    tmp+plen+10,
                    tmp+plen+12);
            SetTextSize(32);
            TextOut(x,y,PcText);
            TextOut(x+32,y+48,PcTextb);
            break;

        case DATE_STYLE2:

            break;

        case DATE_STYLE3:
            sprintf(PcText,"%-7.7sÔª",tmp+plen);
            if(flag == 0)
            {
                SetTextSize(16);
                TextOut(x+plen*16,y,PcText);
            }
            else
            {
                SetTextSize(32);
                TextOut(x+plen*16,y,PcText);
            }

            break;
	case DATE_STYLE4:
	  sprintf(PcText,"ÈËÊýÉèÎª:%-7.7s¸ö",tmp+plen);
                SetTextSize(32);
               // TextOut(x+plen*16,y,PcText);
               TextOut(x, y, PcText);
		break;
        default:
            if(flag == 0)
            {
                SetTextSize(16);
                TextOut(x,y,tmp);
            }
            else
            {
                SetTextSize(32);
                TextOut(x,y,tmp);
            }
            break;

        }
		
        do
        {
            ch = updatekey();
        }
        while( ch == 0xff);

        switch(ch)
        {
        #if 0
        case  SCANCODE_F1:
		if (g_FgSetPersonMode)
			g_FgSetPersonMode = 0;
		else
			g_FgSetPersonMode = 1;
		return 1;
		break;
	#endif
        case  SCANCODE_ESCAPE:
            return 1;
            break;

        case SCANCODE_ENTER:
            if(strlen(tmp) <= strlen(prompt)+len )
            {
                memcpy(str,tmp+strlen(prompt),len);
                if(StrDian(str)==MI_OK)return 0;
                else
                {
                    memset(tmp,0,sizeof(tmp));
                    memset(GL_USB_Tmp,0,sizeof(GL_USB_Tmp));
                    strcpy(tmp,prompt);
                    strcpy(GL_USB_Tmp,prompt);
                    str[0] = '\x00';
                }
            }

            break;

        case SCANCODE_BACKSPACE:
            //if(strlen(tmp) == plen)return 1;
            if(strlen(tmp) > strlen(prompt) )
            {
                tmp[strlen(tmp)-1]=0;
                GL_USB_Tmp[strlen(GL_USB_Tmp)-1]=0;
            }
            break;
        case SCANCODE_PERIOD:
            if(flag) ch='.';
            break;
        case SCANCODE_0:
            ch='0';
            break;
        case SCANCODE_1:
            ch='1';
            break;
        case SCANCODE_2:
            ch='2';
            break;
        case SCANCODE_3:
            ch='3';
            break;
        case SCANCODE_4:
            ch='4';
            break;
        case SCANCODE_5:
            ch='5';
            break;
        case SCANCODE_6:
            ch='6';
            break;
        case SCANCODE_7:
            ch='7';
            break;
        case SCANCODE_8:
            ch='8';
            break;
        case SCANCODE_9:
            ch='9';
            break;
        default :
            break;
        }

        if(isdigit(ch)||ch=='.')
        {
            tmplen = strlen(tmp);
            if( tmplen != (plen+len) )
            {
                c[0]=ch;
                c[1]=0;
                strcat(tmp,c);
                strcat(GL_USB_Tmp,"*");
            }
        }
    }
#endif    
}


/*
*************************************************************************************************************
- º¯ÊýÃû³Æ : void Err_Time(unsigned char *TData)
- º¯ÊýËµÃ÷ : Ð£ÑéÊ±¼ä
- ÊäÈë²ÎÊý : ÎÞ
- Êä³ö²ÎÊý : ÎÞ
*************************************************************************************************************
*/
unsigned char Err_Time(unsigned char *TKData)
{
    unsigned char  TdFlag,k;
    unsigned char  month[]= {0,31,28,31,30,31,30,31,31,30,31,30,31};
    unsigned int TYear;
    unsigned char SaveTime[7];
    memcpy(SaveTime,TKData+1,6);
//memcpy(&Time,TKData+1,6);
    for(k = 0; k < 6; k++)
    {
        SaveTime[k] = BCD2HEX(SaveTime[k]);
    }
    TYear = (unsigned int)(SaveTime[0] + 2000);
    if(((TYear%4 == 0)&&(TYear%100!=0))||(TYear%400 == 0))
    {
        month[2]+=1;
    }
    else
    {
        month[2]=28;
    }
    TdFlag = 1;
    k = 1;
    while(TdFlag)
    {

        switch(k)
        {

        case 1:
            if((SaveTime[k] > 0)&&(SaveTime[k] < 13))k++;//ÔÂ
            else
            {
                TdFlag = 0;
            }
            break;
        case 2:
            if((SaveTime[k] > 0)&&(SaveTime[k] <= month[SaveTime[k-1]]))k++;//ÈÕ
            else
            {
                TdFlag = 0;
            }
            break;
        case 3:
            if(SaveTime[k] < 24)k++;//Ê±
            else
            {
                TdFlag = 0;
            }
            break;
        case 4:
            if(SaveTime[k] < 60)k++;//·Ö
            else
            {
                TdFlag = 0;
            }
            break;
        case 5:
            if(SaveTime[k] < 60)k++;//Ãë
            else
            {
                TdFlag = 0;
            }
            break;
        default:
            TdFlag = 0;
            break;
        }
    }

    return k;
}


/*
*************************************************************************************************************
- º¯ÊýÃû³Æ : unsigned char CheckTime(void)
- º¯ÊýËµÃ÷ : ÐÞ¸ÄÊ±¼ä
- ÊäÈë²ÎÊý : ÎÞ
- Êä³ö²ÎÊý : ÎÞ
*************************************************************************************************************
*/
unsigned char CheckTime(void)
{
    char status;
    unsigned char Dfalg;
    char InKess[30];
    char KeyDat[10];

    Dfalg = 1;
#ifdef NEW0409
    while(Dfalg)
    {
        SetColor(Mcolor);
        SetTextSize(48);
        SetTextColor(Color_white);
        TextOut(0,50, "ÐÞ¸ÄÊ±¼ä");
        memset(InKess,0,sizeof(InKess));
        if(Input(0,105,"",14,InKess,0,DATE_STYLE1) != 1)
        {
            ascii_2_hex(InKess,KeyDat,14);
            status = Err_Time(KeyDat);
            if(status == 6)
            {
                memset(InKess,0,sizeof(InKess));
                sprintf(InKess,"20%02x-%02x-%02x %02x:%02x:%02x",KeyDat[1],\
                        KeyDat[2],KeyDat[3],KeyDat[4],KeyDat[5],KeyDat[6]);
                status  = Wr_time(InKess);
                if(status == 0)
                {
                    system ("hwclock -w");
                    system ("hwclock -s");
                }
                SetColor(Mcolor);
                SetTextSize(48);
                SetTextColor(Color_white);
                TextOut(0,70, "¹§Ï²Äú!");
                TextOut(0,125, "ÐÞ¸ÄÊ±¼ä³É¹¦");
                beepopen(2);
                sleep(3);
                Dfalg = 0;
#if MULTI_FILE_STORAGE
						if(init_record()<0){
								printf("¼ÇÂ¼³õÊ¼»¯Ê§°Ü\n");
							}
							else{
								printf("¼ÇÂ¼³õÊ¼»¯³É¹¦\n");
								show_index_item();
								show_all_record_head(); 	//test
							}	
#endif

            }
            else
            {
                SetColor(Mcolor);
                SetTextSize(48);
                SetTextColor(Color_red);
                TextOut(0,70, "ÌáÊ¾Äú!");
                TextOut(0,130, "ÊäÈëÊ±¼ä¸ñÊ½´íÎó");
                TextOut(0,190, "ÐÞ¸ÄÊ±¼äÊ§°Ü");
                beepopen(3);
                sleep(2);
            }
        }
        else
        {
            Dfalg = 0;
        }
    }
#else
    while(Dfalg)
    {
        SetColor(Mcolor);
        SetTextSize(32);
        SetTextColor(Color_white);
        TextOut(85,50, "ÐÞ¸ÄÊ±¼ä");
        memset(InKess,0,sizeof(InKess));
        if(Input(20,105,"",14,InKess,0,DATE_STYLE1) != 1)
        {
            ascii_2_hex(InKess,KeyDat,14);
            status = Err_Time(KeyDat);
            if(status == 6)
            {
                memset(InKess,0,sizeof(InKess));
                sprintf(InKess,"20%02x-%02x-%02x %02x:%02x:%02x",KeyDat[1],\
                        KeyDat[2],KeyDat[3],KeyDat[4],KeyDat[5],KeyDat[6]);
                status  = Wr_time(InKess);
                if(status == 0)
                {
                    system ("hwclock -w");
                    system ("hwclock -s");
                }
                SetColor(Mcolor);
                SetTextSize(32);
                SetTextColor(Color_white);
                TextOut(110,70, "¹§Ï²Äú!");
                TextOut(55,125, "ÐÞ¸ÄÊ±¼ä³É¹¦");
                beepopen(2);
                sleep(3);
                Dfalg = 0;
#if MULTI_FILE_STORAGE
						if(init_record()<0){
								printf("¼ÇÂ¼³õÊ¼»¯Ê§°Ü\n");
							}
							else{
								printf("¼ÇÂ¼³õÊ¼»¯³É¹¦\n");
								show_index_item();
								show_all_record_head(); 	//test
							}	
#endif

            }
            else
            {
                SetColor(Mcolor);
                SetTextSize(32);
                SetTextColor(Color_red);
                TextOut(110,70, "ÌáÊ¾Äú!");
                TextOut(25,110, "ÊäÈëÊ±¼ä¸ñÊ½´íÎó");
                TextOut(55,150, "ÐÞ¸ÄÊ±¼äÊ§°Ü");
                beepopen(3);
                sleep(2);
            }
        }
        else
        {
            Dfalg = 0;
        }
    }
#endif    
    return 0;

}

/*
¼üÅÌÈ¡µ½int ÐÍµÄÈËÊý
*/
int GetNumFromKey(char *str)
{
      unsigned int i = 0;
    unsigned char len;
    char ddbuf[20];

    len = strlen(str);
    if(str > 0)
   {
   	memcpy(ddbuf, str, len);
	i = atoi(ddbuf);
    }
   return i;
}
/*
ÉèÖÃ³µÔØ¶î¶¨ÈËÊý
*/
void SetMaxPersonNum(void)
{
	unsigned char Dfalg,ffalg;
	char InKess[50]={0};
      Dfalg = 1;
#ifdef NEW0409
     while(Dfalg)
    {
	        SetColor(Mcolor);

	        SetTextSize(48);
	        SetTextColor(Color_white);
	        TextOut(0,50, "¶î¶¨ÈËÊýÉè¶¨");
           
	        memset(InKess,0,sizeof(InKess));
	        if(Input(0,105,"",7,InKess,0,DATE_STYLE4) != 1)
	        {
			SetColor(Mcolor);
			if (strlen(InKess))
			{
				g_MaxPersonNumber = GetNumFromKey(InKess); //get the number
			}
			else
				g_MaxPersonNumber = 0;
			sprintf(InKess, "ÈËÊýÉè¶¨Îª:%04d¸ö", g_MaxPersonNumber);
			SetTextSize(48);
            SetTextColor(Color_white);
            TextOut(0,70, "¹§Ï²Äú!");
            TextOut(0,130, "¶î¶¨ÈËÊýÉè¶¨³É¹¦");
			TextOut(0,190, InKess);
			ReadOrWriteFile(PERSONWRITE);
			g_CurrentNumber = 0;
            beepopen(2);
            sleep(3);
            Dfalg = 0;
	        }
		else
		{
			Dfalg = 0;
		}
    	}
#else      
    while(Dfalg)
    {
	        SetColor(Mcolor);

	        SetTextSize(32);
	        SetTextColor(Color_white);
	        TextOut(65,50, "¶î¶¨ÈËÊýÉè¶¨");
           
	        memset(InKess,0,sizeof(InKess));
	        if(Input(20,105,"",7,InKess,0,DATE_STYLE4) != 1)
	        {
			SetColor(Mcolor);
			if (strlen(InKess))
			{
				g_MaxPersonNumber = GetNumFromKey(InKess); //get the number
			}
			else
				g_MaxPersonNumber = 0;
			sprintf(InKess, "ÈËÊýÉè¶¨Îª:%04d¸ö", g_MaxPersonNumber);
			SetTextSize(32);
            SetTextColor(Color_white);
            TextOut(110,70, "¹§Ï²Äú!");
            TextOut(25,110, "¶î¶¨ÈËÊýÉè¶¨³É¹¦");
			TextOut(2,150, InKess);
			ReadOrWriteFile(PERSONWRITE);
			g_CurrentNumber = 0;
            beepopen(2);
            sleep(3);
            Dfalg = 0;
	        }
		else
		{
			Dfalg = 0;
		}
    	}
#endif     
}

/*
*************************************************************************************************************
- º¯ÊýÃû³Æ : unsigned int CheckFree(char *Valued)
- º¯ÊýËµÃ÷ : ×ª»»ÊäÈëÖµ  ASCII £­£­ £¾ INT
- ÊäÈë²ÎÊý : ÊäÈë×Ö·û´®
- Êä³ö²ÎÊý : ½âÎöÖµ
*************************************************************************************************************
*/
unsigned int CheckFree(char *Valued)
{
    //float  f;
    unsigned int i,a;
    unsigned char n,len;
    char ddbuf[20];
    char ddbuf1[20];

    len = strlen(Valued);
    for(n=0; n<len; n++)
    {
        if(Valued[n] == '+')
        {
            if(Valued[n] == '+') break;
        }
    }
    memset(ddbuf,0,sizeof(ddbuf));
    memset(ddbuf1,0,sizeof(ddbuf1));

    if(n != len)
    {
        memcpy(ddbuf1,Valued+1,len);
    }
    else
    {
        memcpy(ddbuf1,Valued,len);
    }

    sprintf(ddbuf,"%s",ddbuf1);

    i = a = 0;

#if UARTDIS
    printf(ddbuf);
#endif

    i= atoi(ddbuf)*100;

#if UARTDIS
    printf("\n CheckFree 1 i %d \n",i);
#endif

    memset(ddbuf1,0,sizeof(ddbuf1));
    memcpy(ddbuf1,ddbuf,20);


    for(n=0; n<len; n++)
    {
        if(ddbuf1[n] == '.')
        {
            memset(ddbuf,0,sizeof(ddbuf));
            memcpy(ddbuf+1,ddbuf1+n,3);
            ddbuf[0] ='0';
            a = (unsigned int)(atof(ddbuf)*100);
        }
    }

#if UARTDIS
    printf("\n CheckFree 2 i %d \n",a);
#endif

    i = i + a;

#if UARTDIS
    printf("\n CheckFree  ii %d \n",i);
#endif

    return (i);
}

void SetFixValue()	//°´¼ü×Ó¹¦ÄÜ6£¬ÉèÖÃ¹Ì¶¨Ïû·ÑÖµ
{
	char status;
    unsigned char Dfalg,ffalg;
    unsigned char upkey;
    unsigned char disnum,len, SwipeCardError, Flashshow;
    char dbff[100];
    char InKess[50];
    char buff[50];


	FValue.i = 0;
    Dfalg = 1;
#ifdef NEW0409
 while(Dfalg)
    {
        SetColor(Mcolor);
        SetTextSize(48);
        SetTextColor(Color_white);
        TextOut(0,50, "Éè¶¨¹Ì¶¨Ïû·ÑÖµ");
	    memset(InKess,0,sizeof(InKess));

		  if(Input(20,105,"½»Ò×½ð¶î:",7,InKess,1,DATE_STYLE3) != 1)
		  {
			//Flashshow = 1;
			SetColor(Mcolor);
#if UARTDIS
            printf("input  ok  %s   \n",InKess);
#endif
			
            if(strlen(InKess) != 0)
            {
                FValue.i = CheckFree(InKess);
            }
           Fixvalue.i= FValue.i; //×ÔÓÉÏû·ÑÖ

			if(Fixvalue.i!=0){
	            TextOut(0,155, "Éè¶¨³É¹¦");
	            TextOut(164,105,"           ");
	            memset(buff,0,sizeof(buff));
	            sprintf(buff,"¹Ì¶¨½ð¶î:");
	            MoneyValue(buff+9,FValue.i);
	            TextOut(0,105,buff);
				DebugPrintf("FValue.i = %u\n", FValue.i);
				beepopen(2);
                sleep(1);
				PlayMusic(25,0);
				ReadOrWriteFileB(WRFIXVALUE);
				Dfalg=0;
		   	}
			else{
				 SetTextSize(48);
				 TextOut(0,155, "¹Ø±Õ");
				 TextOut(0,105,"¹Ì¶¨Ïû·ÑÉè¶¨¹¦ÄÜ");
				 beepopen(3);
				 sleep(1);
				 PlayMusic(26,0);//ÒÑ¹Ø±Õ¹Ì¶¨Ïû·ÑÉè¶¨¹¦ÄÜ
				 ReadOrWriteFileB(WRFIXVALUE);
				 Dfalg=0;
			}	
		  } 
		  else
        {
            Dfalg = 0;
        } 
		  
		  
    }

#else
    while(Dfalg)
    {
        SetColor(Mcolor);
        SetTextSize(32);
        SetTextColor(Color_white);
        TextOut(65,50, "Éè¶¨¹Ì¶¨Ïû·ÑÖµ");
	    memset(InKess,0,sizeof(InKess));

		  if(Input(20,105,"½»Ò×½ð¶î:",7,InKess,1,DATE_STYLE3) != 1)
		  {
			//Flashshow = 1;
			SetColor(Mcolor);
#if UARTDIS
            printf("input  ok  %s   \n",InKess);
#endif
			
            if(strlen(InKess) != 0)
            {
                FValue.i = CheckFree(InKess);
            }
           Fixvalue.i= FValue.i; //×ÔÓÉÏû·ÑÖ

			if(Fixvalue.i!=0){
	            TextOut(105,155, "Éè¶¨³É¹¦");
	            TextOut(164,105,"           ");
	            memset(buff,0,sizeof(buff));
	            sprintf(buff,"¹Ì¶¨½ð¶î:");
	            MoneyValue(buff+9,FValue.i);
	            TextOut(40,105,buff);
				DebugPrintf("FValue.i = %u\n", FValue.i);
				beepopen(2);
                sleep(1);
				PlayMusic(25,0);
				ReadOrWriteFileB(WRFIXVALUE);
				Dfalg=0;
		   	}
			else{
				 SetTextSize(20);
				 TextOut(105,155, "¹Ø±Õ");
				 TextOut(20,105,"¹Ì¶¨Ïû·ÑÉè¶¨¹¦ÄÜ");
				 beepopen(3);
				 sleep(1);
				 PlayMusic(26,0);//ÒÑ¹Ø±Õ¹Ì¶¨Ïû·ÑÉè¶¨¹¦ÄÜ
				 ReadOrWriteFileB(WRFIXVALUE);
				 Dfalg=0;
			}	
		  } 
		  else
        {
            Dfalg = 0;
        } 
		  
		  
    }
#endif	
}


int PlayMusic_Yue(char * buff)
{
	int i;
	printf("buff=%s\n",buff);
	for(i=0;i<strlen(buff);i++)
	{
		if(buff[i]=='.')
		{
			break;
		}
	}
	printf("i=%d\n",i);
	//PlayMusic(16);  //Óà¶îÎª¸ÄÎªÇëË¢¿¨
	
	usleep(8000);
	if(i==1)
	{
		PlayMusic(buff[0]-17, 0);
		if ((buff[2]>'0') ||(buff[3]>'0'))
		{
			PlayMusic(42, 0);
			usleep(1000);
			PlayMusic(buff[2]-17, 0);
			PlayMusic(buff[3]-17, 0);
		}
	}
	if(i==2)
		{
		PlayMusic(buff[0]-17, 0);
		PlayMusic(43, 0);   //10ÓïÒô
		if(buff[1]!='0')
		{
			PlayMusic(buff[1]-17, 0);
		}
		
		if ((buff[3]>'0') ||(buff[4]>'0'))
		{
			PlayMusic(42, 0);  //µãÓïÒô
			usleep(1000);
			PlayMusic(buff[3]-17, 0);
			PlayMusic(buff[4]-17, 0);
		}
	}
	if(i==3)
	{
		printf("buff[0]=%d\n",buff[0]);

		PlayMusic(buff[0]-17, 0);

		PlayMusic(44, 0);   //100ÓïÒô
		usleep(1000);
#if 1
		if((buff[1]=='0')&&(buff[2]=='0'))
		{
			//PlayMusic(42, 0);  //µãÓïÒô
			//usleep(3000);
		}
		else
		{
			PlayMusic(buff[1]-17, 0);
			if(buff[1]!='0')
			{
				PlayMusic(43, 0);   //10ÓïÒô
			}
			if(buff[2]!='0')
			{
				PlayMusic(buff[2]-17, 0);
			}
		}	
		//printf("buff[4]-17=%d\n",buff[4]-17);
		if ((buff[4]>'0') ||(buff[5]>'0'))
		{
			PlayMusic(42, 0);  //µãÓïÒô
			usleep(1000);
			PlayMusic(buff[4]-17, 0);
			PlayMusic(buff[5]-17, 0);
		}
#endif
     }
	
	if(i==4)
	{
		PlayMusic(buff[0]-17, 0);
	
		PlayMusic(45, 0);		//Ç§ÓïÒô

		if((buff[1]=='0')&&(buff[2]=='0')&&(buff[3]=='0'))
		{
			//printf("µãÓïÒô\n");
			//PlayMusic(42, 0);
			//usleep(3000);
		}
		else{
			if((buff[1]=='0')&&(buff[2]=='0'))
			{
				PlayMusic(31, 0);		//0ÓïÒô
				PlayMusic(buff[3]-17, 0);
			}
			else if((buff[1]=='0')&&(buff[3]=='0'))
			{
				PlayMusic(31, 0);	//0
				PlayMusic(buff[2]-17, 0);
				PlayMusic(43, 0);	//10
			}
			else if((buff[2]=='0')&&(buff[3]=='0'))
			{
				PlayMusic(buff[1]-17, 0);
				PlayMusic(44, 0);			//100
			}
			else if(buff[1]=='0')
			{
				PlayMusic(31, 0);		//0
				PlayMusic(buff[2]-17, 0);
				PlayMusic(43, 0);		//10
				PlayMusic(buff[3]-17, 0);
			}
			else if(buff[2]=='0')
			{
				PlayMusic(buff[1]-17, 0);
				PlayMusic(44, 0);		//100
				PlayMusic(31, 0);		//0
				PlayMusic(buff[3]-17, 0);
			}
			else if(buff[3]=='0')
			{
				PlayMusic(buff[1]-17, 0);
				PlayMusic(44, 0);		//100
				PlayMusic(buff[2]-17, 0);
				PlayMusic(43, 0);		//10
			}
			else{
				PlayMusic(buff[1]-17, 0);
				PlayMusic(44, 0);
				PlayMusic(buff[2]-17, 0);
				PlayMusic(43, 0);
				PlayMusic(buff[3]-17, 0);
			}

			
		}
		if ((buff[5]>'0') ||(buff[6]>'0'))
		{
			PlayMusic(42, 0);  //µãÓïÒô
			usleep(1000);
			PlayMusic(buff[5]-17, 0);
			PlayMusic(buff[6]-17, 0);
		}
	}
	PlayMusic(47, 0);    //Ôª
	return 0;

}


/*
*************************************************************************************************************
- º¯ÊýÃû³Æ : void Freedom(void)
- º¯ÊýËµÃ÷ : ×ÔÓÉÏû·Ñ
- ÊäÈë²ÎÊý : ÎÞ
- Êä³ö²ÎÊý : ÎÞ
*************************************************************************************************************
*/

void Freedom(void)
{
    char status;
    unsigned char Dfalg,ffalg;
    unsigned char upkey;
    unsigned char disnum,len, SwipeCardError, Flashshow;
    char dbff[100];
    char InKess[50];
    char buff[50];
//	struct timeval now, lastTime;
    // LongUnon FeValue;
    //TextOut(20,105, "½»Ò×½ð¶î:"); //175
    // TextOut(105,185, "ÇëË¢¿¨");
    FValue.i = 0;
    Dfalg = 1;
#ifdef NEW0409
    while(Dfalg)
    {
        SetColor(Mcolor);
        SetTextSize(48);
        SetTextColor(Color_white);
        TextOut(0,50, "×ÔÓÉÏû·ÑÄ£Ê½");
        memset(InKess,0,sizeof(InKess));
        if(Input(20,110,"½»Ò×½ð¶î:",7,InKess,1,DATE_STYLE3) != 1)
        {
			//Flashshow = 1;
			SetColor(Mcolor);
#if UARTDIS
            printf("input  ok  %s   \n",InKess);
#endif

            if(strlen(InKess) != 0)
            {
                FValue.i = CheckFree(InKess);
            }
		
			
            DecValue.i = HostValue.i = FValue.i; //×ÔÓÉÏû·ÑÖµ
            TextOut(0,50, "×ÔÓÉÏû·ÑÄ£Ê½");
	#ifdef SUPPORT_QR_CODE
            TextOut(0,160, "ÇëË¢¿¨»òÉ¨ÂëÖ§¸¶");
	#else
	        TextOut(0,160, "ÇëË¢¿¨");
	#endif
            TextOut(164,105,"           ");
            memset(buff,0,sizeof(buff));
            sprintf(buff,"½»Ò×½ð¶î:");
            MoneyValue(buff+9,FValue.i);
            TextOut(0,105,buff);
			DebugPrintf("FValue.i = %u\n", FValue.i);
			//PlayMusic(22,0);
			// memset(buff,0,sizeof(buff));
			//MoneyValue(buff,FValue.i);
			PlayMusic(48, 0); //Æ±¼Û
			PlayMusic_Yue(buff+9);
			PlayMusic(49, 0); // ÇëË¢¿¨
            ffalg = 1;
            disnum = 0;
#ifdef SUPPORT_QR_CODE
            g_QRCodeRcvDataFg = 0;
#endif

            while(ffalg)
            {
#if UARTDIS
                printf("\n Free value:%d decvalue:%d  hostvalue:%d \n",FValue.i,DecValue.i,HostValue.i);
#endif
                disnum++;
				DecValue.i = HostValue.i = FValue.i; //×ÔÓÉÏû·ÑÖµ

                if((disnum >= 5) && Flashshow)
                {
                    disnum = 0;
					Flashshow = 0;
                    SetColor(Mcolor);
                    SetTextSize(48);
                    SetTextColor(Color_white);
                    TextOut(0,50, "×ÔÓÉÏû·ÑÄ£Ê½");
	#ifdef SUPPORT_QR_CODE
           	        TextOut(0,160, "ÇëË¢¿¨»òÉ¨ÂëÖ§¸¶");
	#else
                    TextOut(0,160, "ÇëË¢¿¨");
	#endif
                    TextOut(0,105,buff);
			
                }

#if FOSHAN_HUAYUE
				if((updatekey() == SCANCODE_ESCAPE) && !(disnum%10))
				{
					ffalg = 0;
					freedomflag = 1;
					break;
				}
#endif

FreeSwipeCardAgain:
                //status = CardReset(dbff,&len,0);
 #ifdef SUPPORT_QR_CODE
            	if (g_QRCodeRcvDataFg)
            	{
            		//g_QRCodeRcvDataFg = 0;
            		status =  QR_CODE_STATUS;
            	}
	           else
#endif
                status = CardReset(dbff,&len, 0);
				DecValue.i = HostValue.i = FValue.i;
                if(status == 0x08)
                {
					//printf("---------Ó¦Ïû·Ñ½ð¶î---:%d\n",FValue);
						
                	Flashshow = 1;

						status = FreeReadorRepairCard();

                    if(status == 0)
                    {
                        //beepopen(6);
                        //ReturnDisplay(0);
                        sleep(1);
						LEDL(0);
						LEDR(0);
						StopMusic();
						SwipeCardError = 0;
						RepairLastRecErrorFlag = 0;
#if ((defined FOSHAN_HUAYUE) ||(defined TONGCHUANG_BUS)) 
						freedomflag=1;
                        ffalg = 0;
#endif
                    }
                    else if(status == SWIPE_CARD_ERROR)
                    {
#if FOSHAN_HUAYUE
						freedomflag=1;
						ffalg = 0;
#else
                    	if(WaitForSwipeCardOrTimeOut(&SwipeCardError)  == MI_OK)
						 	goto FreeSwipeCardAgain;
#endif
                    }

                }
                else if(status == 0x20)
                {
                	Flashshow = 1;
					freedomflag=0;
					
					{
	                    status = FreeReadorRepairCard_CPU();
	                    if(status == 0)
	                    {
	                        beepopen(6);
	                        ReturnDisplay(0);
	                        sleep(2);
							freedomflag=1;
	                        ffalg = 0;
	                        // if(OPENPRINTF == 1)
	                        //	PrintferPro(hDlg);			//´òÓ¡Ð¡Æ±
	                    }
	                    else
	                    {
							freedomflag=1;
							ffalg = 0;
	                    }
					}
                    if (status==0)
                    {
                    	sleep(2);
						freedomflag=1;
                        ffalg = 0;
                    }					
                }
#ifdef SUPPORT_QR_CODE
    		else if (status == QR_CODE_STATUS)
    		{
    			Flashshow = 1;
    			status = QRCodeProcessConsume(1);
    			QRCodeScanOutPut(status);
    			if (status == QR_NO_ERROR)
    			       sleep(2);
    			//freedomflag=1;
    	                     //  ffalg = 0;
    		}
#endif
            else if(status > 1)
            {
				freedomflag=1;
				ffalg = 0;
            }

            if(ffalg !=0)
            {
                upkey = updatekey();
                if(upkey == SCANCODE_ESCAPE)
                {
                    ffalg = 0;
                }
            }
            }
        }
        else
        {
            Dfalg = 0;
        }
    }


#else
    while(Dfalg)
    {
        SetColor(Mcolor);
        SetTextSize(32);
        SetTextColor(Color_white);
        TextOut(65,50, "×ÔÓÉÏû·ÑÄ£Ê½");
        memset(InKess,0,sizeof(InKess));
        if(Input(20,105,"½»Ò×½ð¶î:",7,InKess,1,DATE_STYLE3) != 1)
        {
			//Flashshow = 1;
			SetColor(Mcolor);
#if UARTDIS
            printf("input  ok  %s   \n",InKess);
#endif

            if(strlen(InKess) != 0)
            {
                FValue.i = CheckFree(InKess);
            }
		
			
            DecValue.i = HostValue.i = FValue.i; //×ÔÓÉÏû·ÑÖµ
            TextOut(65,50, "×ÔÓÉÏû·ÑÄ£Ê½");
	#ifdef SUPPORT_QR_CODE
            TextOut(35,155, "ÇëË¢¿¨»òÉ¨ÂëÖ§¸¶");
	#else
	        TextOut(105,155, "ÇëË¢¿¨");
	#endif
            TextOut(164,105,"           ");
            memset(buff,0,sizeof(buff));
            sprintf(buff,"½»Ò×½ð¶î:");
            MoneyValue(buff+9,FValue.i);
            TextOut(40,105,buff);
			DebugPrintf("FValue.i = %u\n", FValue.i);
			//PlayMusic(22,0);
			// memset(buff,0,sizeof(buff));
			//MoneyValue(buff,FValue.i);
			PlayMusic(48, 0); //Æ±¼Û
			PlayMusic_Yue(buff+9);
			PlayMusic(49, 0); // ÇëË¢¿¨
            ffalg = 1;
            disnum = 0;
#ifdef SUPPORT_QR_CODE
            g_QRCodeRcvDataFg = 0;
#endif

            while(ffalg)
            {
#if UARTDIS
                printf("\n Free value:%d decvalue:%d  hostvalue:%d \n",FValue.i,DecValue.i,HostValue.i);
#endif
                disnum++;
				DecValue.i = HostValue.i = FValue.i; //×ÔÓÉÏû·ÑÖµ

                if((disnum >= 5) && Flashshow)
                {
                    disnum = 0;
					Flashshow = 0;
                    SetColor(Mcolor);
                    SetTextSize(32);
                    SetTextColor(Color_white);
                    TextOut(65,50, "×ÔÓÉÏû·ÑÄ£Ê½");
	#ifdef SUPPORT_QR_CODE
           	        TextOut(35,155, "ÇëË¢¿¨»òÉ¨ÂëÖ§¸¶");
	#else
                    TextOut(105,155, "ÇëË¢¿¨");
	#endif
                    TextOut(40,105,buff);
			
                }

#if FOSHAN_HUAYUE
				if((updatekey() == SCANCODE_ESCAPE) && !(disnum%10))
				{
					ffalg = 0;
					freedomflag = 1;
					break;
				}
#endif

FreeSwipeCardAgain:
                //status = CardReset(dbff,&len,0);
 #ifdef SUPPORT_QR_CODE
            	if (g_QRCodeRcvDataFg)
            	{
            		//g_QRCodeRcvDataFg = 0;
            		status =  QR_CODE_STATUS;
            	}
	           else
#endif
                status = CardReset(dbff,&len, 0);
				DecValue.i = HostValue.i = FValue.i;
                if(status == 0x08)
                {
					//printf("---------Ó¦Ïû·Ñ½ð¶î---:%d\n",FValue);
						
                	Flashshow = 1;

						status = FreeReadorRepairCard();

                    if(status == 0)
                    {
                        //beepopen(6);
                        //ReturnDisplay(0);
                        sleep(1);
						LEDL(0);
						LEDR(0);
						StopMusic();
						SwipeCardError = 0;
						RepairLastRecErrorFlag = 0;
#if ((defined FOSHAN_HUAYUE) ||(defined TONGCHUANG_BUS)) 
						freedomflag=1;
                        ffalg = 0;
#endif
                    }
                    else if(status == SWIPE_CARD_ERROR)
                    {
#if FOSHAN_HUAYUE
						freedomflag=1;
						ffalg = 0;
#else
                    	if(WaitForSwipeCardOrTimeOut(&SwipeCardError)  == MI_OK)
						 	goto FreeSwipeCardAgain;
#endif
                    }

                }
                else if(status == 0x20)
                {
                	Flashshow = 1;
					freedomflag=0;
					
					{
	                    status = FreeReadorRepairCard_CPU();
	                    if(status == 0)
	                    {
	                        beepopen(6);
	                        ReturnDisplay(0);
	                        sleep(2);
							freedomflag=1;
	                        ffalg = 0;
	                        // if(OPENPRINTF == 1)
	                        //	PrintferPro(hDlg);			//´òÓ¡Ð¡Æ±
	                    }
	                    else
	                    {
							freedomflag=1;
							ffalg = 0;
	                    }
					}
                    if (status==0)
                    {
                    	sleep(2);
						freedomflag=1;
                        ffalg = 0;
                    }					
                }
#ifdef SUPPORT_QR_CODE
    		else if (status == QR_CODE_STATUS)
    		{
    			Flashshow = 1;
    			status = QRCodeProcessConsume(1);
    			QRCodeScanOutPut(status);
    			if (status == QR_NO_ERROR)
    			       sleep(2);
    			//freedomflag=1;
    	                     //  ffalg = 0;
    		}
#endif
            else if(status > 1)
            {
				freedomflag=1;
				ffalg = 0;
            }

            if(ffalg !=0)
            {
                upkey = updatekey();
                if(upkey == SCANCODE_ESCAPE)
                {
                    ffalg = 0;
                }
            }
            }
        }
        else
        {
            Dfalg = 0;
        }
    }
#endif    
	freedomflag=1;
}


unsigned char SectionSta(unsigned char srct,unsigned char type)
{
    static unsigned char sta = 0,upd =0;
    ShortUnon  Buft;

#if UARTDIS
    printf("SectionSta() is called, srct=%d, type=%d, sta=%d, upd=%d.\n", srct, type, sta, upd);
#endif

    if(type == 0)									//´ú±íÕ¾Ì¨ÇÐ»»
    {
        if((srct <SectionNum) && (srct>=0))
        {
#if RUSHAN_BUS
			if(Section.Updown == 0)
				Section.Sationdis = srct + 1;			//ÐÞ¸ÄÎ»ÖÃ
				//Section.Sationdis = srct;	
			else				
            	Section.Sationdis = SectionNum - srct;
           
            Section.SationNow = srct + 1;
            

#else
		    Section.Sationdis = srct;				//µ±Ç°Õ¾Ì¨±àºÅ

            Section.SationNow = srct + 1;			//ÏÂÒ»Õ¾±àºÅ
#endif            
            if(sta != srct)
            {
                // ÓïÒô »»Õ¾³É¹¦
                PlayMusic(9, 0);
				printf("»»Õ¾³É¹¦SectionSta(): ÐÂÕ¾µã srct=%d, Ô­Õ¾µãsta=%d\n", srct, sta);
#if UARTDIS
                printf("SectionSta(): new station srct=%d, sta=%d\n", srct, sta);
#endif
            }
            sta = srct;					//±£´æÕ¾Ì¨±àºÅ(Áô¸øÏÂÒ»´Î»»Õ¾½øÐÐ±È½Ï)
        }
        else
        {
            srct = Section.Sationdis;
        }
    }
    else									//´ú±íÉÏÏÂÐÐÇÐ»»
    {
        if(srct < 2)					
        {
            Section.Updown = srct;
            if((Section.Updown == 0)||(Section.Enableup != 0x55))			//ÉÏÐÐ
            {
                memcpy(Buft.intbuf, Section.SationNum, sizeof(ShortUnon));
                SectionNum = Buft.i;
				printf("new sectionum=%d\n",SectionNum);
            }
            else
            {
                memcpy(Buft.intbuf, Sectionup.SationNum, sizeof(ShortUnon));
                SectionNum = Buft.i;
				printf("new sectionum=%d\n",SectionNum);
            }

            Section.SationNow = 0x01;
            Section.Sationdis = 0;
            Section.Sationkey =0;

            if(upd != srct)
            {
            	printf("»»ÐÐ³É¹¦,µ±Ç°Îªsection.updown=%d\n",Section.Updown);
                // ÓïÒô »»ÐÐ³É¹¦
                PlayMusic(9, 0);
            }
			printf("»»ÐÐ³É¹¦£¬ updown=%d\n",srct);
			printf("sectionum=%d---->\n",SectionNum);
			
            upd = srct;				//±£´æÉÏÏÂÐÐ£¬ÄÚ²¿»º³å,(Áô¸øÏÂÒ»´Î»»ÐÐ×÷Îª±È½Ï)
			//sta = 0xFF;				//ÖØÖÃÕ¾Ì¨£¬ÄÚ²¿»º³å
        }
    }

	ReadOrWriteFile(SETSECTINO_);

#if   UARTDIS
    printf("SectionSta  now = %d \n",Section.SationNow);
#endif

    return srct;

}


void SectionApp(void)
{
	DebugPrintf("\n");
    char status;
//	struct tm *tmd;
    unsigned char Dfalg;
    unsigned char upkey,len;
//  unsigned char disnum,Qflag;
//	unsigned char secold,secnow;
    unsigned char Buffer[50];
    unsigned char stationup;
    struct timeval now, lastTime;
	FILE* parafile;
	LongUnon tempbuf;
	unsigned char NandBuf[512];
	
    Section.StationOn = 0x55;
    Section.Sationkey = Section.Sationdis;	//Õ¾Ì¨±àºÅ
    stationup = Section.Updown;
    Dfalg = 1;
    CardTwo = 1;

#if RUSHAN_BUS
			//Ôö¼Ó¶Á²ÎÊýÎÄ¼þ£¬Ìî³äsection½á¹¹Ìå£¬ÀàËÆUSBÉý¼¶·Ö¶Î
	
	parafile=fopen("/mnt/record/cardlan.sys","rb+");		//Òª¸üÐÂÕâ¸öÎÄ¼þ£¬usb¸üÐÂ²ÎÊýÎÄ¼þ	
	if(parafile==NULL)
	{
		printf("can't find cardlan.sys,can't use section mode\n");
		return ;
	}
	
	memset(NandBuf,0,sizeof(NandBuf));
	fread(NandBuf,512,1,parafile);
	
	memcpy(tempbuf.longbuf,NandBuf+321,sizeof(LongUnon));
				if((tempbuf.i != 0)&&(tempbuf.i <= 0x10000))
				{
					memcpy(&Sectionup.SationNum,NandBuf+325,6);
					ReadOrWriteFile(SETSECTIONUP);
	
			   }

	memcpy(tempbuf.longbuf,NandBuf+305,sizeof(LongUnon));
    if((tempbuf.i != 0)&&(tempbuf.i <= 0x10000))
     {
                memcpy(&Section.SationNum,NandBuf+309,10);
                ReadOrWriteFile(SETSECTION);
                if(Section.Enableup != 0x55)
                {
                    memcpy(&Sectionup.SationNum,NandBuf+309,6);
                    ReadOrWriteFile (SETSECTIONUP);
                }
     }

	 

#endif

    while(Dfalg)
    {
        gettimeofday(&now,0);
        if ((now.tv_sec != lastTime.tv_sec)||(1== CardTwo))
        {
            if((LCDKeepDisp)||(1== CardTwo))
            {
                if((now.tv_sec - LCDKeepStartTime.tv_sec > LCD_KEEP_DISP_TIME)||(1== CardTwo))
                {
                    // ½áÊø½»Ò×ÐÅÏ¢ÏÔÊ¾
                    LCDKeepDisp = 0;
                    SetColor(Mcolor);
                    CardTwo = 0;
                }
            }
			
            if (LCDKeepDisp == 0)
            {
                memcpy(&lastTime, &now, sizeof(now));

                SetTextSize(32);
                SetTextColor(Color_white);
				DebugPrintf("ConnectFlag = 0x%02X\n", ConnectFlag);
                Display_signal(ConnectFlag);
                if(Section.Updown  == 0x00)
                {
                    sprintf(Buffer,"ÉÏÐÐ Õ¾ºÅ:%02d",Section.Sationdis);
                }
                else
                {
                    sprintf(Buffer,"ÏÂÐÐ Õ¾ºÅ:%02d",Section.Sationdis);
                }
                TextOut(75,25,Buffer);
                TextOut(70,70, "·Ö¶ÎÊÕ·ÑÄ£Ê½");
                TextOut(115,125, "ÇëË¢¿¨");
                SetTextSize(16);
                TextOut(85,200,mk_time(Buffer));
            }
        }


        status = CardReset(Buffer,&len,0);
        if(status == 0x08)
        {
            status = ReadCardInfor();
            if(status == 0)
            {
                status = SectionFares(); // ·Ö¶ÎÏû·Ñ
                if(status == 0)
                {
                    // ÖÐ¶ÏÓïÒô²¥·Å
                    //StopMusic();

                    //LEDR(1);
                    //beepopen(2);
                    //ReturnDisplay(3);
                    //LEDR(0);
                }
            }

        }

        if(CardTwo == 0)
        {
            upkey = updatekey();
            if(upkey != 0xff)
            {
                CardTwo = 1;
                switch(upkey)
                {

#if __KEYBOARD_1
               case SCANCODE_F4:
#elif __KEYBOARD_2
          		case SCANCODE2_CHL:
#endif

#if  UartDis
                    printf("SCANCODE_F4 \n");
#endif
                    stationup++;
                    SectionSta((stationup%2),1);
                    break;

#if __KEYBOARD_1
                case SCANCODE_F5:
#elif __KEYBOARD_2
         		case SCANCODE2_PRE:
#endif

#if  UartDis
                    printf("SCANCODE_F5 \n");
#endif
                    Section.Sationkey--;
                    Section.Sationkey = SectionSta(Section.Sationkey,0);
                    break;

#if __KEYBOARD_1
                case SCANCODE_F6:
#elif __KEYBOARD_2
        		case SCANCODE2_NEXT:
#endif

#if  UartDis
                    printf("SCANCODE_F6 \n");
#endif
                    Section.Sationkey++;
                    Section.Sationkey = SectionSta(Section.Sationkey,0);
                    break;

                case SCANCODE_ESCAPE:
                    Dfalg = 0;
                    break;

                default :
#if  UartDis
                    printf("SCANCODE  %d \n",upkey);
#endif
                    break;
                }
            }
        }


    }
    Section.StationOn = 0x00;

}
/************************************/
int CheckLineCardlanBin(unsigned char num1,unsigned char num2)			//²éÕÒÏßÂ·²ÎÊýÎÄ¼þ£¬²¢ÐÞ¸ÄÏßÂ·
{
	char buff1[30]="/mnt/record/cardlan";
	char buff2[16];	FILE *linef;
	int result;
	int linenum;
	linenum=0;
	linenum=num1;
	linenum=linenum<<8;
	linenum=linenum|num2;
	sprintf(buff2,"%03d.bin",linenum);
	strcat(buff1,buff2);
	//printf("the file is %s\n",buff1);
	if (!(access(buff1, 0))){
		linef=fopen(buff1,"rb+");
		if(linef==NULL){
			return -1;
		}
		memset(buff2,0,sizeof(buff2));
		result = fseek(linef, 0, SEEK_SET);
		result = fread(buff2,sizeof(unsigned char),8,linef);
		fclose(linef);	
		//	if(buff2[1]==0xff){	//´ú±íÓÐÐ§
		Section.Linenum[0]=num1;
		Section.Linenum[1]=num2;
		return 0;	
		//	}
	}	
	else{
	//	printf("can not find file \n");
		return -1;
	}	
	return -1;
}



int UpdateLinePara(unsigned char num1,unsigned char num2)	//¸üÐÂÏû·Ñ²ÎÊý£¬ÓëÏßÂ·Í¬²½
{
	int i;
	ShortUnon satnum;
	LongUnon Buf;
	unsigned char buff1[30]="/mnt/record/cardlan";
	unsigned char buff2[16];
	unsigned char Nandbuf[512];
	unsigned char Nandbuf_temp[512];
	FILE *linef;
	FILE *Filebuf;
	int result;
	int linenum;
	linenum=0;
	linenum=num1;
	linenum=linenum<<8;
	linenum=linenum|num2;
	sprintf(buff2,"%03d.bin",linenum);
	strcat(buff1,buff2);
	//printf("the file is %s,you are in updatelinepara\n",buff1);

	
	printf("you are in upadatalinepara-------------------->\n");
	if (!(access(buff1, 0))){
		linef = fopen(buff1,"rb+");
		if(linef==NULL){
			//printf("can not open this file\n");
			return -1;
		}
		printf("open %s ok\n",buff1);
        result = fseek(linef,0, SEEK_SET);
        result = fread(Nandbuf,sizeof(unsigned char),512,linef);
        ReadCardFirst  = Nandbuf[1];
        ReadOrWriteFile(SETCARDFIRST);
	/***¸úÐÂsection Óësectionup£¬ÀàËÆusb¸úÐÂ·Ö¶ÎÊÕ·Ñ****/



	

#if 1
	/*ÏÈ²é¿´ÊÇ·ñÓÐÏÂÐÐÓÐÐ§·Ö¶ÎÊÕ·Ñ*/
	printf("check section------------------>\n");
//	if(Nandbuf[320] == 0x55)
        {
            memcpy(Buf.longbuf,Nandbuf+321,sizeof(LongUnon));
            if((Buf.i != 0)&&(Buf.i <= 0x10000))
            {
                memcpy(&Sectionup.SationNum,Nandbuf+325,6);	//¹¹Ôìsectionup½á¹¹Ìå£¬Ç°6¸ö×Ö½Ú
				printf("check section ok------->\n");
                ReadOrWriteFile(SETSECTIONUP);

                Filebuf = fopen("/mnt/record/sectionup.sys","rb+");	//½«cardlan0x.binÎÄ¼þµÄÄÚÈÝ¿½Èëµ½sysÎÄ¼þÖÐ
                for(i=0; i<32; i++)
                {
                    memset (Nandbuf_temp,0,sizeof(Nandbuf_temp));
                    result = fseek(linef,Buf.i+i*512, SEEK_SET);
                    result = fread(Nandbuf_temp,sizeof(unsigned char),512,linef);

                    result = fseek(Filebuf,i*512, SEEK_SET);
                    result = fwrite(Nandbuf_temp,sizeof(unsigned char),512,Filebuf);
                }
                fclose(Filebuf);
                CardLanFile(SectionParup);
            }

        }
		
	/*²é¿´ÊÇ·ñÓÐÉÏÐÐÓÐÐ§·Ö¶ÎÊÕ·Ñ*/
	printf("check sectionup------------------>\n");
	//	 if(Nandbuf[304] == 0x55)
        {
            memcpy(Buf.longbuf,Nandbuf+305,sizeof(LongUnon));//¹¹Ôìsection½á¹¹Ìå,Ç°Ê®¸ö×Ö½Ú
            if((Buf.i != 0)&&(Buf.i <= 0x10000))
            {
                memcpy(&Section.SationNum,Nandbuf+309,10);
				printf("check sectionup ok---------->\n");
                ReadOrWriteFile(SETSECTION);
              //  if(Section.Enableup != 0x55)
                //{
                  //  memcpy(&Sectionup.SationNum,Nandbuf+309,6);
                    //ReadOrWriteFile (SETSECTIONUP);
                //}
                #ifdef SAVE_CONSUM_DATA_DIRECT
                Filebuf = fopen("/mnt/record/section.sys","rb+");//½«cardlan0x.binÎÄ¼þµÄÄÚÈÝ¿½Èëµ½sysÎÄ¼þ
                #else
                Filebuf = fopen("/var/run/section.sys","rb+");//½«cardlan0x.binÎÄ¼þµÄÄÚÈÝ¿½Èëµ½sysÎÄ¼þ
                #endif
                for(i=0; i<32; i++)
                {
                    memset (Nandbuf_temp,0,sizeof(Nandbuf));
                    result = fseek(linef,Buf.i+i*512, SEEK_SET);
                    result = fread(Nandbuf_temp,sizeof(unsigned char),512,linef);

                    result = fseek(Filebuf,i*512, SEEK_SET);
                    result = fwrite(Nandbuf_temp,sizeof(unsigned char),512,Filebuf);
                }
                fclose(Filebuf);
                CardLanFile(SectionPar);
             
            }

        }	   
#endif

        memset (CardLanBuf,0,sizeof(CardLanBuf));
        result = fseek(linef, 512, SEEK_SET);
        result = fread(CardLanBuf,sizeof(unsigned char),16384,linef);
        fclose(linef);
		linef = NULL;


       linef = fopen("/mnt/record/cardlan.sys","rb+");
		if(linef==NULL){
			printf("can not open this file\n");
			return -1;
		}

		printf("open cardlan.sys ok\n");
        result = fseek(linef, 0, SEEK_SET);
      	result = fwrite(CardLanBuf,sizeof(unsigned char),16384,linef);
        fclose(linef);
      /***ÐÞ¸Ä section½á¹¹Ìå*******/
//
//		Section.Enable = 0;
//		ReadOrWriteFile(SETSECTION);
//³õÊ¼»¯Õ¾µã£¬Ä¬ÈÏÎªÉÏÐÐµÄµÚÒ»¸öÕ¾

		Section.Updown=0;
		Section.SationNow=1;
		Section.Sationdis=0;
		Section.Sationkey=0;
		memcpy(satnum.intbuf,Section.SationNum,2);
		SectionNum=satnum.i;
        usleep(500000);
				
	}
	else{
		//printf("there is on sucn file\n");
		return -1;
	}	

	printf("the section.enable=%02x, sectionup.enable=%02x\n",Section.Enable,Sectionup.Enable);
	printf("the section.updown=%d\n",Section.Updown);
	printf("the section.sationnow=%d\n",Section.SationNow);
	printf("the section.sationdis=%d\n",Section.Sationdis);
	printf("the section.sationkey=%d\n",Section.Sationkey);
	return 0;

}

/************************************/
void ProcessUARTPacket(char *buffer)
{
    DBG_UART_PRINTF("ProcessUARTPacket() is called.\n");
	static int sucesstimes=0;
	static int failtimes=0;
	static unsigned char lastNum[2]={0};
	unsigned char ischageline;
	static time_t t,Now=0,Last=0;
	int status;
    int temp = 0;
    int j;
    unsigned char flag;

	time(&t);
	if(Now==0&&Last==0){
		Now=t;
		Last=Now-5;
	}
	else Now=t;
	
    if(( (DevNum.i/10000000) == 1)||((DevNum.i/10000000) == 0))
        flag = 0x01;
    else
        flag = 0x02;
    
    switch(buffer[2])
    {
    case 0x80:
        if((buffer[4] == 0x02)&&(flag == buffer[3]))
        {   
            UARTHartBeat[3]= flag;
            for(j=0;j<1;j++)
            {
            temp = write(uart4_fd,UARTHartBeat,sizeof(UARTHartBeat));
#if 0
            {
                int i;
                printf("Send uart=%03d::",temp);
                for(i = 0; i<temp; i++)
                {
                    printf("%02X",UARTHartBeat[i]);
                }
                printf("\n");
            }
#endif
            }
            }
           connecttime = 0;
        break;
    case 0x82:					//Õ¾Ì¨ÇÐ»»
    //printf("########## the packet=%x %x %x %x %x %x %x######\n",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6]);
        if((buffer[4] != Section.Updown)||(buffer[5] == 0))
        {
            // if the line is not match, for example, the terminal is just started up while the station reporter is running,
            // just call the line change routine.
            SectionSta(buffer[4], 1);	//ÇÐ»»ÉÏÏÂÐÐ
        }

        // call station change routine


        Section.Sationkey = buffer[5];
        SectionSta(Section.Sationkey, 0);
       connecttime = 0;
        break;

    case 0x83:
		//printf("########## the packet=%x %x %x %x %x %x %x######\n",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6]);
        // call the line change routine	ÉÏÏÂÐÐÇÐ»»
        SectionSta(buffer[4], 1);
        connecttime = 0;
        break;
#if Yantai_Qixia
	case 0x85:			//ÏßÂ·¸ü»»
		//printf("ready update line\n");
		printf("########## the packet=%x %x %x %x %x %x %x######\n",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6]);
		status=CheckLineCardlanBin(buffer[3],buffer[4]);
		if(Section.Linenum[0]==lastNum[0]&&Section.Linenum[1]==lastNum[1])
						ischageline=0;
		else{
				ischageline=1;
				lastNum[0]=Section.Linenum[0];
				lastNum[1]=Section.Linenum[1];
			}

		
		if(status==0){
		//	if((Section.Linenum[0]!=buffer[3]||Section.Linenum[1]!=buffer[4])||(Now-Last>=5))
			if((ischageline==1)||(Now-Last>5)&&(ischageline=0))
			{	

				PlayMusic(18,0);
				ReadOrWriteFile(SETSECTIONLINE);		//Ð´ÏµÍ³²ÎÊýÎÄ¼þ	
				UpdateLinePara(buffer[3],buffer[4]);
				//updatline_err=1;					//ÓÃÓïÒôÌæ»»
				Last=Now;
				//Err_display(27);						
			}
		
		}
		else{
		
			if(	Now-Last>=5)
			{
			 //printf("¸üÐÂÏßÂ·´íÎó¡\\n");
			 //updatline_err=2;
			 PlayMusic(19,0);
			 Last=Now;
			}
		}
		break;	
#endif
    default :
        break;
    }
 
}
/********************ÔÝÊ±ÓÐÎÊÌâ**********************/
int ispacketvalid(unsigned char *pkt,unsigned char len)
{
	
	int flag=1;	
	printf("----in ispacketvalid---\n");

	if(pkt[len+1]==0x7f)
		flag=0;
	return flag;
}


void CheckUARTPacket_one(char ch)
{	unsigned char lenth;
	int i;
	
	if(ch==UART_PACKET_DELIMITER)	//0x7fÎª°üÍ·»ò°üÎ²	
	{		
		if(UARTPacketIndex==0){
			UARTPacket_one[UARTPacketIndex++]=ch;
		}
		else{
			UARTPacket_one[UARTPacketIndex++]=ch;			//ÈôÊÇ°üÎ²
			lenth=UARTPacket_one[1];						//¼ìÑé³¤¶ÈÊÇ·ñ×¼È·
			if(ispacketvalid(UARTPacket_one,lenth)==0){
				printf("a new packet come\n");
				for(i=0;i<lenth+2;i++)
					printf("packet[%d]=%x",i,UARTPacket_one[i]);
				printf("\n");
				ProcessUARTPacket(UARTPacket_one);
				memset(UARTPacket_one,0,sizeof(UARTPacket_one));
				UARTPacketIndex=0;
			}
			else{											//¼ìÑé²»ºÏ¸ñ
				UARTPacketIndex=0;
				memset(UARTPacket_one,0,sizeof(UARTPacket_one));
		
			}
		}
	}
	else{
		#if 0
		if(UARTPacket_one[0]==UART_PACKET_DELIMITER)		//ÈôÎªÊý¾Ý°üµÄÖÐ¼ä×Ö·û
					UARTPacket_one[UARTPacketIndex++]=ch;		
		else{
			UARTPacketIndex=0;
			memset(UARTPacket_one,0,sizeof(UARTPacket_one));
		}
		#endif
		
		UARTPacket_one[UARTPacketIndex++]=ch;
		if(UARTPacketIndex>=UART_PACKET_LENGTH_ONE){	
			UARTPacketIndex=0;
			memset(UARTPacket_one,0,sizeof(UARTPacket_one));
		}
		
	}
}

/********************************************/
void CheckUARTPacket(char ch)
{
    printf("¶ÁÈëµÄÖµCH = %02x \n",ch);
    if (ch == UART_PACKET_DELIMITER)
    {
  //      printf("¶ÁÈëµÄË÷ÒýÖµUARTPacketIndex = %02x \n",UARTPacketIndex);
        if ((UARTPacketIndex == 0)
                || (UARTPacketIndex == (UART_PACKET_LENGTH - 1)) )
        {
            UARTPacket[UARTPacketIndex] = ch;
            UARTPacketIndex++;

            // Found one complete packet

            if (UARTPacketIndex == UART_PACKET_LENGTH)
                
            {
                // Call station changed routine
                ProcessUARTPacket(UARTPacket);

                UARTPacketIndex = 0;
            }
        }
        else
        {
            // Received data maybe corrupted, reset packet
            UARTPacketIndex = 0;
            UARTPacket[UARTPacketIndex] = ch;
            UARTPacketIndex++;
        }
    }
    else if (UARTPacketIndex > 0)
    {
        UARTPacket[UARTPacketIndex] = ch;
        UARTPacketIndex++;

        if (UARTPacketIndex == UART_PACKET_LENGTH)
        {
            // overflow, reset packet
            UARTPacketIndex = 0;
        }
    }
}

void SectionStaion_Uart(unsigned char *databuf)
{
//	int i;
    //unsigned char datflag = 0;
    //unsigned char Rcvuartbuf[20];

	//printf(" ---in sectionstation_uart--\n");
    if (UARTReadIndex <= UARTWriteIndex)
    {
        while (UARTReadIndex < UARTWriteIndex)
        {
        	#ifdef  ZHEJIANG_ANJI
			 CheckUARTPacket_AnJi(UARTRxBuff[UARTReadIndex]);
			 #else
		         //  CheckUARTPacket(UARTRxBuff[UARTReadIndex]);
		        CheckUARTPacket_beijing(UARTRxBuff[UARTReadIndex]);
			 #endif
           // CheckUARTPacket_one(UARTRxBuff[UARTReadIndex]);
            UARTReadIndex++;
        }
    }
    else
    {
        while (UARTReadIndex < UART_BUFFER_SIZE)
        {
	        #ifdef  ZHEJIANG_ANJI
			 CheckUARTPacket_AnJi(UARTRxBuff[UARTReadIndex]);
			 #else
		        //    CheckUARTPacket(UARTRxBuff[UARTReadIndex]);
		        CheckUARTPacket_beijing(UARTRxBuff[UARTReadIndex]);
			 #endif
            //CheckUARTPacket_one(UARTRxBuff[UARTReadIndex]);
            UARTReadIndex++;
        }

        UARTReadIndex= 0;
        while (UARTReadIndex < UARTWriteIndex)
        {
            #ifdef  ZHEJIANG_ANJI
			 CheckUARTPacket_AnJi(UARTRxBuff[UARTReadIndex]);
			 #else
		          //  CheckUARTPacket(UARTRxBuff[UARTReadIndex]);
		     CheckUARTPacket_beijing(UARTRxBuff[UARTReadIndex]);
			 #endif
           //CheckUARTPacket_one(UARTRxBuff[UARTReadIndex]);
            UARTReadIndex++;
        }
    }
}

/*±±¾©ÏîÄ¿°´¼üÓÚÆäËû*/

/*
CRC8 = 0xb8 10111000 (0x1b8 110111000)
·½³ÌÎªx^8+x^7+x^5+x^4+x^3
*/

unsigned char CalCRC(unsigned char *ptr, unsigned char len)
{
    unsigned char i; 
    unsigned char crc=0x00£» /* ¼ÆËãµÄ³õÊ¼crcÖµ */ 

    while(len--)
    {
        crc ^= *ptr++;  /* Ã¿´ÎÏÈÓëÐèÒª¼ÆËãµÄÊý¾ÝÒì»ò,¼ÆËãÍêÖ¸ÏòÏÂÒ»Êý¾Ý */  
        for (i=8; i>0; --i)   /* ÏÂÃæÕâ¶Î¼ÆËã¹ý³ÌÓë¼ÆËãÒ»¸ö×Ö½ÚcrcÒ»Ñù */  
        { 
            if (crc & 0x80)
                crc = (crc << 1) ^ 0xb8;
            else
                crc = (crc << 1);
        }
    }

    return (crc); 
}

int RetCtrlCmdStatus(char *cmd)//, unsigned char set)
{
	unsigned char sendbuf[40];
	unsigned char  len;
	memset(sendbuf, 0, sizeof(sendbuf));
	len = 0;
	if (!memcmp(cmd, CMD_FUNC_REQ, 2)) //?
	{
		sendbuf[len++] = DEV_TYPE_POS;
		len = 8;
	}
	else if (!memcmp(cmd, CMD_FUNC_DIR_NO, 2)){
		sprintf(sendbuf+len, "02%d", Section.Sationdis);
		len += 2;
		len += 1; // 0
		sendbuf[len++] = g_CallStationNo;
		len += 3; // 0
		sendbuf[len++] = Section.Updown;//0 up . 1 down
	}
	else if (!memcmp(cmd, CMD_FUNC_DATE, 2)){
		
	}


	write(uart4_fd, sendbuf, len);
	return 0;
}
void SetBusDeviceStation(unsigned char *buf)
{
	unsigned char current_station, dir, callno;
	current_station = (buf[0]-'0')*10+(buf[1]-'0');
	callno = buf[2]-'0';
	dir = buf[3]-'0';
	if(Section.Updown == 1)  //ÏÂÐÐ
	{
		if (dir == 0)  //±¨Õ¾Æ÷ÉÏÐÐ£¬so change
		{
			SectionSta(0, 1);
			 if(current_station < SectionNum)
    		{
	   			 Section.Sationdis = current_station;				//µ±Ç°Õ¾Ì¨±àºÅ
       				 Section.SationNow = current_station + 1;
			 }
			 return;
		}
		SectionSta(current_station, 0);
	}
	else
	{
		if (dir)  //±¨Õ¾Æ÷ÏÂÐÐ£¬so change
		{
			SectionSta(1, 1);
			 if(current_station < SectionNum)
    		{
	   			 Section.Sationdis = current_station;				//µ±Ç°Õ¾Ì¨±àºÅ
       				 Section.SationNow = current_station + 1;
			 }
			 return;
		}
		SectionSta(current_station, 0);
	}
}
/*

1set date 
2set time
3 set date time
*/
void SetBusDeviceTime(unsigned char *time, unsigned char set_flag)
{
	unsigned char buff[32]={0};
	char status;
	unsigned char year,month,day,hour,min,sec;
	Rd_time (buff);
	year = buff[0];
	month = buff[1];
	day = buff[2];
	hour = buff[3];
	min = buff[4];
	sec = buff[5];
	memset(buff, 0, siezof(buff));
	if (set_flag == 1)
	{
		//sprintf(buff,"20%02x-%02x-%02x %02x:%02x:%02x",Data[4],\
		//Data[5],Data[6],Data[7],Data[8],Data[9]);
	} else if (set_flag == 2) {
		
	} else if (set_flag == 3) {

	}
	
	status	= Wr_time(buff);
	if(status == 0)
	{
		system ("hwclock -w");
		system ("hwclock -s");
	}
}
void CheckUARTPacket_beijing(char ch)
{
	unsigned char len = 0;
	unsigned char cmdstr[3];
	
	if (((ch & 0xf0) == g_DevNoInBus) && (UARTPacketIndex == 0))
	{
   	    UARTPacket[UARTPacketIndex] = ch;
	    UARTPacketIndex++;
   }
   else if (UARTPacketIndex > 1)
   {
	    UARTPacket[UARTPacketIndex] = ch;
	    UARTPacketIndex++;
   }
   else
   	{
   		UARTPacketIndex = 0;
		return;
   	}
   
	if (UARTPacketIndex < 3) return;
		
	len = UARTPacket[1];
   if (UARTPacketIndex != (len+3))// rcv finish?
   		return;

	if (UARTPacket[2] != CMD_FLAG_CH)
	{
		UARTPacketIndex = 0;
		return;
	}
   

   	if (CalCRC(UARTPacket, UARTPacketIndex-1) != UARTPacket[UARTPacketIndex-1])
   	{
		UARTPacketIndex = 0;
		return;
    }
   memset(cmdstr, 0, sizeof(cmdstr));
   memcpy(cmdstr, UARTPacket+3, 2)
   if (!memcmp(cmdstr, CMD_FUNC_REQ, 2))
   {
		RetCtrlCmdStatus(CMD_FUNC_REQ);
   } else if (!memcmp(cmdstr, CMD_FUNC_DIR_NO, 2)){
		if (len > 3){//set
			SetBusDeviceStation(UARTPacket+5);
		}
		RetCtrlCmdStatus(CMD_FUNC_DIR_NO);
   } else if (!memcmp(cmdstr, CMD_FUNC_DATE, 2)) {
   		if (len > 3){
			
		}
			
		
   }

}



/*******************************************************/
#ifdef ZHEJIANG_ANJI
void CalcXorAndSum(unsigned char *in_arr, unsigned short in_len, unsigned char *xor, unsigned char *sum)
{
	unsigned short i = 0;
	unsigned char tmp_xor = 0, tmp_sum = 0;
	
	for(i=0; i<in_len; i++)
	{
		tmp_xor ^= *(in_arr+i);
		tmp_sum += *(in_arr+i);
	}
	*xor= tmp_xor;
	*sum = tmp_sum;
	
}
void ParseAnJiDataPacket(unsigned char *dat, unsigned short len)
{
	unsigned char current_station = dat[8]&0x7f;
	if ((dat[0] == 0x10)&&(dat[1] == 0))   //cmd and status check
	{
		//index 2 to 7 is the time , so pass
		if(Section.Updown == 1)  //ÏÂÐÐ
		{
			if ((dat[8] & 0x80) == 0)  //±¨Õ¾Æ÷ÉÏÐÐ£¬so change
			{
				SectionSta(0, 1);
				 if(current_station < SectionNum)
        				{
		   			 Section.Sationdis = current_station;				//µ±Ç°Õ¾Ì¨±àºÅ
           				 Section.SationNow = current_station + 1;
				 }
				 return;
			}
			SectionSta(current_station, 0);
		}
		else
		{
			if (dat[8] & 0x80)  //±¨Õ¾Æ÷ÏÂÐÐ£¬so change
			{
				SectionSta(1, 1);
				 if(current_station < SectionNum)
        				{
		   			 Section.Sationdis = current_station;				//µ±Ç°Õ¾Ì¨±àºÅ
           				 Section.SationNow = current_station + 1;
				 }
				 return;
			}
			SectionSta(current_station, 0);
		}
	}
}

unsigned char SendDriverInfoToDevice(unsigned char sign)     //0 is sign. 1 is leave 
{
	unsigned char i, buffer[64] = {0};
	unsigned char mXor, mSum;
	i = 0;
	buffer[i++] = 0xD6; //HEADER
	buffer[i++] = 0xA3;
	buffer[i++] = 0x19;   //LEN
	buffer[i++] = 0;
	buffer[i++] = 0x10;
	buffer[i++] = 0x81;
	buffer[i++] = 0x20;
	buffer[i++] = 0x20;
	buffer[i++] = 'C';
	sprintf(buffer+i, "%05d", Driver.i);
	i += 5;
	sprintf(buffer+i, "%08d", DevNum.i);
	i += 8;
	buffer[i++] = sign;
	memcpy(buffer+i, (unsigned char *)&Time, 6);
	i += 6;
	CalcXorAndSum(buffer+4, 0x19, &mXor, &mSum);

	buffer[i++] = mXor;
	buffer[i++] = mSum;
	write(uart4_fd, buffer, i);
	return i;
}

unsigned char SendErrorOrNoDataToDevice(unsigned char val) //0x80 mean no data , 0x90 mean data error
{
	unsigned char i, buffer[64] = {0};
	unsigned char mXor, mSum;
	i = 0;
	buffer[i++] = 0xD6; //HEADER
	buffer[i++] = 0xA3;
	buffer[i++] = 2;   //LEN
	buffer[i++] = 0;
	buffer[i++] = 0x10;
	buffer[i++] = val;
	CalcXorAndSum(buffer+4, 2, &mXor, &mSum);
	buffer[i++] = mXor;
	buffer[i++] = mSum;
	write(uart4_fd, buffer, i);
	return i;
}

void CheckUARTPacket_AnJi(char ch)
{
	  unsigned short len = 0;
	 unsigned char Xor,Sum;
	   printf("char = %d\n", ch);
	   if (((ch == 0xD6) && (UARTPacketIndex == 0)) ||((ch == 0xA3) && (UARTPacketIndex == 1)))
	   {
	   	    UARTPacket[UARTPacketIndex] = ch;
		    UARTPacketIndex++;
	   }
	   else if (UARTPacketIndex > 1)
	   {
		      UARTPacket[UARTPacketIndex] = ch;
		    UARTPacketIndex++;
	   }
	   else
	   	UARTPacketIndex = 0;

	   if (UARTPacketIndex > 7)
	   {
	   	 len = UARTPacket[3]*0xFF + UARTPacket[2];
		 if (len > (512+2)) 
		 {
			UARTPacketIndex = 0;
			return;
		 }
		 if (len == (UARTPacketIndex-6))
		 {
			CalcXorAndSum(UARTPacket+2, len+2, &Xor, &Sum);
			if ((Xor == UARTPacket[UARTPacketIndex-2]) && (Sum == UARTPacket[UARTPacketIndex-1]))  //data correct
			{
				ParseAnJiDataPacket(UARTPacket+4, len);
				SendDriverInfoToDevice(0); 
				UARTPacketIndex= 0;
			}
			else
				UARTPacketIndex= 0;
		 }
	   }
}
#endif

#if RUSHAN_BUS
int save_station()
{
	struct STATION_INFO_ *p,*q;
	FILE *file;
	struct stat fileInfo;
	int num,len,i;
	char tmpbuf[6];

	file=fopen("/mnt/nand1-2/app/stationrecord.bin","r+");	
	if(file!=NULL){
		printf("/mnt/nand1-2/app/stationrecord.bin ok \n");
		stat("/mnt/nand1-2/app/stationrecord.bin" , &fileInfo);
		
		len = sizeof(struct STATION_INFO_);
		num = fileInfo.st_size/len;
		p = (struct STATION_INFO_*)malloc(sizeof(struct STATION_INFO_));
		if(p!=NULL){
			Rd_time(tmpbuf);
			memcpy(p->time,tmpbuf+1,5);
			p->section_stationNow = Section.SationNow;
			p->section_Updown = Section.Updown;
			p->section_stationNum = Section.SationNum[0];
			p->sectionup_stationNum = Sectionup.SationNum[0];
			p->sectionNum = SectionNum;
			p->currentperson = g_CurrentNumber;
		}
		else{
			printf("¿Õ¼ä²»×ã£¬ÎÞ·¨±£´æ¸ÃÕ¾µãÐÅÏ¢\n");
			fclose(file);
			return -1;
		}
		pthread_mutex_lock(&m_stationrecord);	
		
		if(num>=0&&num<50){
				num++;
				stationtaill.pre->next=p;
				p->next=&stationtaill;
				p->pre=stationtaill.pre;
				stationtaill.pre=p;
		}
		else if(num==50){
				q=stationhead.next;
				stationhead.next=q->next;
				q->next->pre=&stationhead;
				free(q);
			
				stationtaill.pre->next=p;
				p->next=&stationtaill;
				p->pre=stationtaill.pre;
				stationtaill.pre=p;	
		}
		fclose(file);
		system("> /mnt/nand1-2/app/stationrecord.bin\n");	//½«ÎÄ¼þÇå¿Õ
		system("sync");
		file=fopen("/mnt/nand1-2/app/stationrecord.bin","r+");	
		if(file!=NULL){
			p=stationhead.next;
			fseek(file,0, SEEK_SET);
			while(p!=&stationtaill){
				fwrite((char *)p,len,1,file);
				p=p->next;
			}
		}
		else{
			printf("Ð´Õ¾µã¼ÇÂ¼´íÎó£¬Çå¿ÕÁ´±í\n");
			while(stationhead.next != &stationtaill){
				q = stationhead.next;
				stationhead.next = q->next;
				q->next->pre=&stationhead;
				free(q);
			}
		}	
		
		pthread_mutex_unlock(&m_stationrecord);
	}
	else{
		printf("can't open /mnt/nand1-2/app/stationrecord.bin \n");
	}
	
	fclose(file);
	return 0;
}

#endif
void ProcessUARTPacket_RUSHAN(char *buffer)	//ÈéÉ½¹«½»±¨Õ¾Æ÷Ð­Òé½âÎö
{	
	unsigned char StationData = buffer[4];
	int i;
	
	union		
		{			
			unsigned char buf[2];
			unsigned short i;
		}tmp;

	printf("in processuartpacket_rushan____\n");
	
	//for(i=0;i<UARTPacketIndex;i++)
	//	printf("packet[%d]=%02x ",i,UARTPacket[i]);
	//	printf("\n");
	//for(i=0;i<UARTPacketIndex;i++)
	//	printf("buffer[%d]=%02x ",i,buffer[i]);

	printf("stationdata=%d\n",StationData);
	
	if(StationData <= 99)
	{
		if(Section.Updown == 1){		//0-99ÎªÉÏÐÐµÄÕ¾ºÅÇø¼ä
			SectionSta(0, 1);				//ÇÐ»»ÖÁÉÏÐÐ
			//Section.Sationdis = StationData;			
			 Section.Sationdis=1;
		}
		else{
				printf("SationNum=%d\n",SectionNum);
				SectionSta(StationData-1, 0);		//ÉÏÐÐ»»Õ¾£¬»»Õ¾µÄÊ±ÄÚ²¿»á½«±àºÅ¼ÓÒ»		
			
			}
	}
	else
		{
			StationData = 0xFF-StationData; 		//255-156ÎªÏÂÐÐÕ¾ºÅÇø¼ä
			StationData--;
			if(Section.Updown == 0){
				SectionSta(1, 1);					//ÇÐ»»ÖÁÏÂÐÐ
				Section.Sationdis = StationData;
				
			}
			else{
				if(Section.Enableup != 0x55)		//ÈôÏÂÐÐ²»ÆôÓÃ
					memcpy(tmp.buf,Section.SationNum,2);
				else
					memcpy(tmp.buf,Sectionup.SationNum,2);
				
				printf("SationNum=%d\n",SectionNum);
				//SectionSta(tmp.i-StationData, 0);			//ÏÂÐÐ»»Õ¾
			
					SectionSta(tmp.i-StationData,0);
				
				//printf("tmp.i : %d,StationData: %d,Section.Enableup : %d , xxx :%d \n",tmp.i,StationData,Section.Enableup,*(unsigned short*)(Section.SationNum));			
			}
		}
	
}

void CheckUARTPacket_RUSHAN(char ch)
{	
	static unsigned char bytes = 1;
	static unsigned short len;
	unsigned short i,check;
	switch(bytes)
	//printf("Received data : 0x%x\n",ch);
	{
		case 1:
			if(ch == 0x55)
				bytes++;
			break;
		case 2:
			if(ch == 0xAA)
				bytes++;
			else
				bytes = 1;
			break;
		case 3:
			if(ch == 0x02)
				bytes++;
			else
				bytes = 1;
			break;
		case 4:
			if(ch == 0xAA)
			{
				bytes++;
				UARTPacketIndex = 0;
			}
			else
				bytes = 1;
			break;
		case 5:
			if(UARTPacketIndex < 2){
				UARTPacket[UARTPacketIndex] = ch;
				UARTPacketIndex++;
				if(2 == UARTPacketIndex)
				{
					len = (unsigned short)((UARTPacket[0]<<8)|UARTPacket[1]);
					if(len==0){
						bytes = 1;
					}
					else{
						if(len > 126){
							bytes = 1;
						}
						else
							len += 2;
					}
				}
			}
			else{
				if(len)
				{
					UARTPacket[UARTPacketIndex] = ch;
					UARTPacketIndex++;
					len--;	
	//				}
	//				else
	//				{
					if(0 == len)
					{
						check = 0x02+0xAA;
						for(i = 0; i < UARTPacketIndex-2; i++)//³ý¿ª°ü×îºóÁ½¸ö×Ö½Ú
						{
							check += UARTPacket[i];
						}
						len = (unsigned short)((UARTPacket[UARTPacketIndex-2]<<8)|\	
							UARTPacket[UARTPacketIndex-1]);
						//printf("get sum : 0x%x,0x%x\n",UARTPacket[UARTPacketIndex-2],UARTPacket[UARTPacketIndex-1]);
						//printf("check : 0x%x \n",check);
						if((check+len)==0xFFFF)
						{
							ProcessUARTPacket_RUSHAN(UARTPacket);
						}
						else
							printf("data not right \n");
						bytes = 1;
						UARTPacketIndex = 0;
					}
				}
			}
			break;
		default:
			bytes = 1;
			break;
	}
}




void SectionStaion_Uart_RUSHAN(unsigned char *databuf)
{
	printf(" ---in sectionstation_uart--\n");
    if (UARTReadIndex <= UARTWriteIndex)
    {
        while (UARTReadIndex < UARTWriteIndex)
        {
           CheckUARTPacket_RUSHAN(UARTRxBuff[UARTReadIndex]);
           // CheckUARTPacket_one(UARTRxBuff[UARTReadIndex]);
            UARTReadIndex++;
        }
    }
    else
    {
        while (UARTReadIndex < UART_BUFFER_SIZE)
        {
            CheckUARTPacket_RUSHAN(UARTRxBuff[UARTReadIndex]);
            //CheckUARTPacket_one(UARTRxBuff[UARTReadIndex]);
            UARTReadIndex++;
        }

        UARTReadIndex= 0;
        while (UARTReadIndex < UARTWriteIndex)
        {
           CheckUARTPacket_RUSHAN(UARTRxBuff[UARTReadIndex]);
           //CheckUARTPacket_one(UARTRxBuff[UARTReadIndex]);
            UARTReadIndex++;
        }
    }	
}
/*******************************************************/
unsigned char VerifyRecvPacket_485(char *data)
{
	int i;
	unsigned short sum = 0;
	
	printf("´«ÈëÊý¾Ý:");
		for(i=0;i<8;i++)
			printf("%02x ",RcvData.head[i]);
		printf("\n");

	
	for(i = 0;i<6;i++)
	{
		sum += data[i];
	}
	printf("sum = %04x:date[6]=%02x:date[7]=%02x:( sum >> 8 )=%02x:(sum&0x0f)=%02x\n",sum,data[6],data[7]
		,( sum >> 8 ),(sum&0x0f));
	if((data[6] == ( sum >> 8 )) && ((data[7]&0x0f) == (sum&0x0f))) 
		return 0;
	else
		return 1;
}

unsigned char InitStationInfo_DALI(void)
{
	DebugPrintf("\n");
	FILE *fp = NULL;
	char TempBuffer[64];
	struct DALI_TransferMsg ReadData_DALI;
	
	memset(TempBuffer, 0, sizeof(TempBuffer));
	
	if(access(SAVE_STATION_NUM_FILE, F_OK))
	{
		DebugPrintf("No dushugao file\n");
		return -1;
	}

	if((fp = fopen (SAVE_STATION_NUM_FILE, "r")) == NULL)
	{
		DebugPrintf("read dushugao.cfg failed\n");
		perror("open error\n");
		return -1;
	}

	if((fgets (TempBuffer, sizeof(struct DALI_TransferMsg), fp)) != NULL)
	{
		memcpy(&ReadData_DALI, TempBuffer, sizeof(struct DALI_TransferMsg));
		DebugPrintf("In_Out_station : 0x%02X\n", ReadData_DALI.In_Out_station);
		DebugPrintf("StationNum 	: %u\n", 	 ReadData_DALI.StationNum);
		DebugPrintf("SectionParUp 	: 0x%02X\n", ReadData_DALI.SectionParUp);

		Section.Updown = ReadData_DALI.SectionParUp == 0x00 ? 0:1; 
		Section.Sationdis = ReadData_DALI.StationNum;
		Section.SationNow = ReadData_DALI.StationNum + 1;
		CardTwo = 1;
	}

	fclose(fp);

	return 0;
}

unsigned char WriteStationNumTOFile(struct DALI_TransferMsg RcvData_DALI)
{
	int StaNum_Fd = 0;
	
	if((StaNum_Fd = open(SAVE_STATION_NUM_FILE, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) < 0)
	{
		printf("open stationNum.txt error\n");
		return -1;
	}
	
	lseek(StaNum_Fd, 0, SEEK_SET);
	
	write(StaNum_Fd, &RcvData_DALI, sizeof(struct DALI_TransferMsg));

	close(StaNum_Fd);

	return 0;
}
	



/* 	º¯ÊýÃû³Æ unsigned char DALI_ParseRecvMsg(int RevcDatalen)
 *	param: RevcDatalen ½ÓÊÕµÄÊý¾Ý³¤¶È
 */
unsigned char DALI_ParseRecvMsg(int RevcDatalen)
{
	DebugPrintf("\n");

	static unsigned char  CurStationNum = -1;
	
	int OnePacketLen = sizeof(struct DALI_TransferMsg);
	int FindStartPos = 1, tempint = 0;
	char OnePacketBuf[16];
	int copysize_1 = 0, copysize_2 = 0;

	memset(OnePacketBuf, 0, sizeof(OnePacketBuf));
	memset(&RcvData, 0, OnePacketLen);


	/* 1. ÅÐ¶Ï½ÓÊÕµÄÊý¾ÝÊÇ·ñ¹»Ò»¸öÊý¾Ý°üµÄ³¤¶È£¬²»¹»ÔòÖ±½Ó·µ»Ø 
	 * 2. Ñ¡Ôñ¿ªÍ·Îª\x3A\x3AµÄ½á¹¹ÌåÍ·
	 */
	while(FindStartPos)
	{
		DebugPrintf("UARTReadIndex = %u UARTWriteIndex = %u\n", UARTReadIndex, UARTWriteIndex);
		
		if((UARTWriteIndex > UARTReadIndex) && (UARTWriteIndex - UARTReadIndex >= OnePacketLen))
		{
			if(UARTRxBuff[UARTReadIndex] != 0x3A || UARTRxBuff[UARTReadIndex+1] != 0x3A)
				UARTReadIndex++;
			else
				FindStartPos = 0;
		}
		else if(UARTWriteIndex < UARTReadIndex)
		{
			tempint = (UART_BUFFER_SIZE - UARTReadIndex) + UARTWriteIndex;
			
			if(tempint >= OnePacketLen)
			{
				if(UARTRxBuff[UARTReadIndex%512] != 0x3A || UARTRxBuff[(UARTReadIndex+1)%512] != 0x3A)
				{
					if(UARTReadIndex >= 512)
						UARTReadIndex = 0;
					else
						UARTReadIndex++;
				}
				else
					FindStartPos = 0;
			}else
				return 0;
		}
		else
			return 0;
	}


	if(UARTReadIndex + OnePacketLen < UART_BUFFER_SIZE)
	{
		memcpy(&RcvData, UARTRxBuff+UARTReadIndex, OnePacketLen);
		UARTReadIndex += OnePacketLen;
	}
	else
	{
		copysize_1 = UART_BUFFER_SIZE - UARTReadIndex -1;
		memcpy(OnePacketBuf, UARTRxBuff+UARTReadIndex, copysize_1);
		copysize_2 = OnePacketLen - copysize_1;
		memcpy(OnePacketBuf + copysize_1, UARTRxBuff, copysize_2);
		memcpy(&RcvData, OnePacketBuf, OnePacketLen);
		UARTReadIndex = copysize_2 - 1;
	}

	
	if((RcvData.head[0] == 0x3A) && (RcvData.head[1] == 0x3A) && !VerifyRecvPacket_485(RcvData.head))
	{
		char i;
		printf("Êý¾ÝÕýÈ·:");
		for(i=0;i<8;i++)
			printf("%02x ",RcvData.head[i]);
		printf("\n");	
		DebugPrintf("In_Out_station : 0x%02X\n", RcvData.In_Out_station);
		DebugPrintf("StationNum 	: %u\n", RcvData.StationNum);
		DebugPrintf("PathNum 		: 0x%02X\n", RcvData.PathNum);
		DebugPrintf("SectionParUp 	: 0x%02X\n", RcvData.SectionParUp);
		DebugPrintf("SectionNum		: 0x%02X\n", SectionNum);
#if 1
		if(CurStationNum != RcvData.StationNum)
		{
			Section.Updown = RcvData.SectionParUp == 0x00 ? 0:1; 
			Section.Sationdis = RcvData.StationNum;
			
			//if((RcvData.StationNum - 1) < SectionNum)
				Section.SationNow = RcvData.StationNum;
			
			CardTwo = 1;
			CurStationNum = RcvData.StationNum;
			WriteStationNumTOFile(RcvData);
			system("sync");
			PlayMusic(9, 0);
		}
#endif	
	}
		
	return 0;
}

void * Readuart_Pthread (void * args)
{
	DebugPrintf("\n");
    int ret;
    //struct stat fdbuf;
    unsigned char rebuf[256];
    int copiedSize1, copiedSize2;
    int time;
    UARTReadIndex = 0;
    UARTWriteIndex = 0;
    UARTPacketIndex = 0;
    connecttime = 0;
    memset(UARTRxBuff, 0, sizeof(UARTRxBuff));
    memset(UARTPacket, 0, sizeof(UARTPacket));
	memset(UARTPacket_one,0,sizeof(UARTPacket_one));
    printf("DevNum.longbuf=%02x::%02x::%02x::%02x",DevNum.longbuf[0],DevNum.longbuf[1],DevNum.longbuf[2],DevNum.longbuf[3]);
    while(1)
    {
        ret = Uart_Readdata(uart4_fd, rebuf, sizeof(rebuf));
        if(ret > 0)
        {
        	int i;
			//for(i=0;i<ret;i++)
			//	printf("buf[%d]=%x ",i,rebuf[i]);
			//printf("\n");
			
            // write index is in front of read index, normal sequence
            DebugPrintf("\n");
            if((UARTWriteIndex + ret) <= UART_BUFFER_SIZE)
            {
                memcpy(UARTRxBuff + UARTWriteIndex, rebuf, ret);
                UARTWriteIndex += ret;
                if (UARTWriteIndex == UART_BUFFER_SIZE)
                {
                    UARTWriteIndex = 0;
                    if (UARTReadIndex == 0)
                    {
                        // buffer overflow
                        UARTReadIndex++;
                        DBG_UART_PRINTF("Readuart_Pthread(): UART RX buffer overflow!\n");
                    }
                }

            }
            else
            {
                copiedSize1 = UART_BUFFER_SIZE - UARTWriteIndex;
                copiedSize2 = ret - copiedSize1;
                memcpy(UARTRxBuff + UARTWriteIndex, rebuf, copiedSize1);

                // loop back the write index
                memcpy(UARTRxBuff, rebuf + copiedSize1, copiedSize2);
                UARTWriteIndex = copiedSize2;

                if (copiedSize2 >= UARTReadIndex)
                {
                    // buffer overflow
                    DBG_UART_PRINTF("Readuart_Pthread(): UART RX buffer overflow, UARTReadIndex = %d, copiedSize2 = %d!\n", UARTReadIndex, copiedSize2);
                    UARTReadIndex = copiedSize2 + 1;
                }
            }
            memset (rebuf,0,sizeof(rebuf));


#if	RUSHAN_BUS
			SectionStaion_Uart_RUSHAN(UARTRxBuff);
			save_station();					//±£´æÕ¾µãÐÅÏ¢
#else			
            SectionStaion_Uart(UARTRxBuff);
#endif
        }
        usleep(200000);
		
        if(Section.Enable == 0x55)
        {
            connecttime++;
    //    printf("connecttime=%d:ret=%d\n",connecttime,ret);
        if(connecttime>12)         //Á¬½Ó±¨Õ¾Æ÷³¬¹ý2·ÖÖÓÃ»ÓÐÕýÈ·Êý¾Ý£¬²úÉú´íÎóÈÕÖ¾
        { 
            connecttime = 0;
            SAVE_CARLAN_LOG(RS485_ERROR);
            }
        }
    }
}

#if defined(SUPPORT_QR_CODE)

#if QR_CODE_USE_USBHID
const unsigned char KeyTabToChr[][3] = 
{		
	{0x2a, 0x02, '!'},		
	{0x2a, 0x28, '"'},		
	{0x2a, 0x04, '#'},		
	{0x2a, 0x05, '$'},		
	{0x2a, 0x06, '%'},		
	{0x2a, 0x08, '&'},		
	{0x00, 0x28, '\''},		
	{0x2a, 0x0a, '('},		
	{0x2a, 0x0b, ')'},		
	{0x2a, 0x09, '*'},		
	{0x2a, 0x0d, '+'},		
	{0x00, 0x33, ','},		
	{0x00, 0x0c, '-'},		
	{0x00, 0x34, '.'},		
	{0x00, 0x35, '/'},		
	{0x00, 0x0b, '0'},		
	{0x00, 0x02, '1'},		
	{0x00, 0x03, '2'},		
	{0x00, 0x04, '3'},		
	{0x00, 0x05, '4'},		
	{0x00, 0x06, '5'},		
	{0x00, 0x07, '6'},		
	{0x00, 0x08, '7'},		
	{0x00, 0x09, '8'},		
	{0x00, 0x0a, '9'},		
	{0x2a, 0x27, ':'},		
	{0x00, 0x27, ';'},		
	{0x2a, 0x33, '<'},		
	{0x00, 0x0d, '='},		
	{0x2a, 0x34, '>'},		
	{0x2a, 0x35, '?'},		
	{0x2a, 0x03, '@'},		
	{0x2a, 0x1e, 'A'},		
	{0x2a, 0x30, 'B'},		
	{0x2a, 0x2e, 'C'},		
	{0x2a, 0x20, 'D'},		
	{0x2a, 0x12, 'E'},		
	{0x2a, 0x21, 'F'},		
	{0x2a, 0x22, 'G'},		
	{0x2a, 0x23, 'H'},		
	{0x2a, 0x17, 'I'},		
	{0x2a, 0x24, 'J'},		
	{0x2a, 0x25, 'K'},		
	{0x2a, 0x26, 'L'},		
	{0x2a, 0x32, 'M'},		
	{0x2a, 0x31, 'N'},		
	{0x2a, 0x18, 'O'},		
	{0x2a, 0x19, 'P'},		
	{0x2a, 0x10, 'Q'},		
	{0x2a, 0x13, 'R'},		
	{0x2a, 0x1f, 'S'},		
	{0x2a, 0x14, 'T'},		
	{0x2a, 0x16, 'U'},		
	{0x2a, 0x2f, 'V'},		
	{0x2a, 0x11, 'W'},		
	{0x2a, 0x2d, 'X'},		
	{0x2a, 0x15, 'Y'},		
	{0x2a, 0x2c, 'Z'},		
	{0x00, 0x1a, '['},		
	{0x00, 0x2b, '\\'},		
	{0x00, 0x1b, ']'},		
	{0x2a, 0x07, '^'},		
	{0x2a, 0x0c, '_'},		
	{0x00, 0x29, '`'},		
	{0x00, 0x1e, 'a'},		
	{0x00, 0x30, 'b'},		
	{0x00, 0x2e, 'c'},		
	{0x00, 0x20, 'd'},		
	{0x00, 0x12, 'e'},		
	{0x00, 0x21, 'f'},		
	{0x00, 0x22, 'g'},		
	{0x00, 0x23, 'h'},		
	{0x00, 0x17, 'i'},		
	{0x00, 0x24, 'j'},		
	{0x00, 0x25, 'k'},		
	{0x00, 0x26, 'l'},		
	{0x00, 0x32, 'm'},		
	{0x00, 0x31, 'n'},		
	{0x00, 0x18, 'o'},		
	{0x00, 0x19, 'p'},		
	{0x00, 0x10, 'q'},		
	{0x00, 0x13, 'r'},		
	{0x00, 0x1f, 's'},		
	{0x00, 0x14, 't'},		
	{0x00, 0x16, 'u'},		
	{0x00, 0x2f, 'v'},		
	{0x00, 0x11, 'w'},		
	{0x00, 0x2d, 'x'},		
	{0x00, 0x15, 'y'},		
	{0x00, 0x2c, 'z'},		
	{0x2a, 0x1a, '{'},		
	{0x2a, 0x2b, '|'},		
	{0x2a, 0x1b, '}'},		
	{0x2a, 0x29, '~'},		
	{0x00, 0x1c, '\r'},		
	{0x00, 0x6c, '\n'},
};

static unsigned char HidDevPath[100];
int GetHidDevInfo(void)	
{	
	int fd=NULL,res;	
	char buf[100];	
	char *pfile;	
	char VoidInputDevStr[4];	
	int VoidInputDev;	
	
	VoidInputDev = 0;	
	system("cat /proc/bus/input/devices | awk '/Vendor=0525 Product=a4ac/{print NR}' > /tmp/Hidinfo.bin;");			
	fd = fopen("/tmp/Hidinfo.bin", "rb");	
	if(fd == NULL)	{				
		printf("/tmp/Hidinfo can not opened \n");		
		return -1;	
	}	
	memset(VoidInputDevStr , 0 , sizeof(VoidInputDevStr));	
	res = fread(&VoidInputDevStr,1,4,fd);		
	fclose(fd);		
	VoidInputDev = atoi(VoidInputDevStr);	
	//VoidInputDev -= 0x30;	
	printf("--------VoidInputDev: %d,res:%d \n", VoidInputDev,res);			
	memset(buf,0,sizeof(buf));	
	//printf("here \n");	
	sprintf(buf, "cat /proc/bus/input/devices |awk '{if(NR==%d)print}' > /tmp/hiddev;",VoidInputDev+4);	
	printf("string : %s \n", buf);	
	system(buf);	memset(buf,0,sizeof(buf));	
	fd = fopen("/tmp/hiddev", "r");	
	if(fd == NULL)	{				
		printf("/tmp/hiddev can not opened \n");		
		return -1;	
	}	
	res = fread(buf,100,1,fd);		
	fclose(fd);	
	printf("res : %d \n",res);	
	if(res >= sizeof(buf) || strlen(buf) == 0)	{		
		printf("read kbd info error ! \n");		
		return -1;	
	}	
	while(buf[strlen(buf)-1] < 0x30 || buf[strlen(buf)-1] > 0x39)	
	{		
		buf[strlen(buf)-1] = 0;	
	}	
	pfile = buf+strlen(buf)-1;	
	while(pfile != buf)	
	{		
		if((*pfile)==0x3D)		
		{						
			break;		
		}		
		pfile--;	
	}	
	if(pfile == buf)		
		return -1;	
	if(*(pfile+1) != 0)		
		pfile += 1;	
	else		
		return -1;	
	memset(HidDevPath, 0, sizeof(HidDevPath));	
	sprintf(HidDevPath, "/dev/input/%s", pfile);	
	pfile += 5;	
	if(*pfile == 0)		
		return -1;	
	printf("******* pfile : %s \n",pfile);	
	res = 0;	
	while((*pfile) != 0)	
	{		
		res *= 10;		
		res += (*pfile - 0x30);		
		printf("*pfile :%x\n", *pfile);		
		pfile++;	
	}	
	printf("GetInputInfo----string is %s ,res:%d\n", HidDevPath,res);	
	if(res > 2)	
	{		
		memset(buf,0,sizeof(buf));		
		sprintf(buf, "mknode %s c 13 %d;", HidDevPath, res+64);		
		system(buf);	
	}	
	return res;		
}
void closeHid(int *p_fd)
{	
	if(*p_fd >0) 
	{		
		close(*p_fd);
	}	
	*p_fd = NULL;
}
unsigned char QrCodeBuffer[512]={0};
int g_QrCodeLen = 0;
static unsigned char Rcv_0x2A_Fg=0;
#if 1
unsigned char read_HidDevInput(void)
{   
	int retval;    
	int yalv; /* loop counter */    
	fd_set rfds;    
	struct timeval tv;    
	size_t read_bytes; /* how many bytes were read */    
	struct input_event ev[50];  /* the events (up to 64 at once) */	
	unsigned char codebuf = 0;
	int i;	   
	// keyvalue = 0xff;    
	tv.tv_sec = 1;                   // the rcv wait time   
	tv.tv_usec = 0;                   // 100ms    
	while(1)    
	{        
	FD_ZERO(&rfds);        
	FD_SET(QRCode_fd,&rfds);		
	//printf("select \n");        
	retval = select(QRCode_fd+1,&rfds,NULL,NULL,&tv);        
	if(retval == -1)       
	{            	
		printf("select read Hid ()  ");		
		closeHid(&QRCode_fd);			
		QRCode_fd = 0;            	
		return -1;        
	}        
	else if(retval)       
	{           
		read_bytes= read(QRCode_fd,ev,sizeof(struct input_event) * 50);           
		if((read_bytes < (int)sizeof(struct input_event))|| (read_bytes == -1))            
		{                
			printf("evtest: short read");			
			closeHid(&QRCode_fd);				
			QRCode_fd = 0;                
			return -1;           
		}			
		//printf("read_bytes : %d,\n",read_bytes);			
		codebuf = 0;			
		//codebuf[1] = 0;           
		for(yalv=0; yalv<read_bytes/sizeof(struct input_event); yalv++)            
		{               
			if((ev[yalv].type == 1)&&(ev[yalv].value == 1))                
			{                    
				if (ev[yalv].code == 0x2a)
				{
					Rcv_0x2A_Fg = 1;
					continue;
				}
				codebuf = ev[yalv].code;
				//printf("%02x ", codebuf);
				for(i=0; i<sizeof(KeyTabToChr)/3; i++)				
				{					
					if (Rcv_0x2A_Fg)					
					{			
						if(KeyTabToChr[i][0] == 0x2a && KeyTabToChr[i][1] == codebuf)						
						{		
							Rcv_0x2A_Fg = 0;
							QrCodeBuffer[g_QrCodeLen++] = KeyTabToChr[i][2];							
							break;						
						}					
					}					
					else			
					{						
						if(KeyTabToChr[i][0] == 0 && KeyTabToChr[i][1] == codebuf)						
						{							
							QrCodeBuffer[g_QrCodeLen++] = KeyTabToChr[i][2];							
							break;						
						}					
					}				
				}			
			}           
		}		
		
		if (QrCodeBuffer[g_QrCodeLen-2] == '\r' && QrCodeBuffer[g_QrCodeLen-1] == '\n')			
		{				
			printf("qrcode string = %s \n", QrCodeBuffer);				
			//memset(QrCodeBuffer, 0, sizeof(QrCodeBuffer));	
			//g_QrCodeLen = 0;
			if (g_QRCodeRcvDataFg == 0)
			{
				for (i=0; i<g_QrCodeLen-2; i++)
					if ((QrCodeBuffer[i] < '0') || (QrCodeBuffer[i] > '9'))
							break;
				if (i == (g_QrCodeLen-2))
				{
					g_FgCardLanQRCode = 0;
					if ((QrCodeBuffer[0] == '3') && ((QrCodeBuffer[1] == '0')||(QrCodeBuffer[1] == '1')))
					{
						g_FgCardLanQRCode = 1;
					}
					memset(G_QRCodeInfo.id, 0, sizeof(G_QRCodeInfo.id));
					memcpy(G_QRCodeInfo.id, QrCodeBuffer, (g_QrCodeLen-2));
					G_QRCodeInfo.length = (g_QrCodeLen-2);
					g_QRCodeRcvDataFg = 1;
				}
			}
			Rcv_0x2A_Fg = 0;
			g_QrCodeLen = 0;
			memset(QrCodeBuffer, 0, sizeof(QrCodeBuffer));
			usleep(100000);
			return 0;
		}        
			//  printf("input value 2 %d \n",keyvalue);            
			//break;        
		}        
		else       
		{		  
			//printf("Read QrCode timeout  \n");          
			return -1;       
		}   
	}    
	return(0);
}
#else
unsigned char read_HidDevInput(void)
{   
	int retval, i;    
	int yalv; /* loop counter */    
	fd_set rfds;    
	struct timeval tv;    
	size_t read_bytes; /* how many bytes were read */    
	struct input_event ev[2];  /* the events (up to 64 at once) */	
	unsigned char codebuf[2];	   
	// keyvalue = 0xff;    
	tv.tv_sec = 1;                   // the rcv wait time   
	tv.tv_usec = 0;                   // 100ms    
	while(1)    
	{        
	FD_ZERO(&rfds);        
	FD_SET(QRCode_fd,&rfds);		
	//printf("select \n");        
	retval = select(QRCode_fd+1,&rfds,NULL,NULL,&tv);        
	if(retval == -1)       
	{            	
		printf("select read Hid ()  ");		
		closeHid(&QRCode_fd);			
		QRCode_fd = 0;            	
		return -1;        
	}        
	else if(retval)       
	{           
		read_bytes= read(QRCode_fd,ev,sizeof(struct input_event) * 2);           
		if((read_bytes < (int)sizeof(struct input_event))|| (read_bytes == -1))            
		{                
			printf("evtest: short read");			
			closeHid(&QRCode_fd);				
			QRCode_fd = 0;                
			return -1;           
		}			
		//printf("read_bytes : %d,\n",read_bytes);			
		codebuf[0] = 0;			
		codebuf[1] = 0;           
		for(yalv=0; yalv<(read_bytes/sizeof(struct input_event)-1); yalv++)            
		{               
			if((ev[yalv].type == 1)&&(ev[yalv].value == 1))                
			{                    
				codebuf[yalv] = ev[yalv].code;					
				//	printf("%02x ",keyvalue);               
			}           
		}			
		if (read_bytes/sizeof(struct input_event) > 1)			
		{				
			for(i=0; i<sizeof(KeyTabToChr)/3; i++)				
			{					
				if (2 == read_bytes/sizeof(struct input_event)-1)					
				{						
					if(KeyTabToChr[i][0] == codebuf[0] && KeyTabToChr[i][1] == codebuf[1])						
					{							
						QrCodeBuffer[g_QrCodeLen++] = KeyTabToChr[i][2];							
						break;						
					}					
				}					
				else if (1 == read_bytes/sizeof(struct input_event)-1)					
				{						
					if(KeyTabToChr[i][0] == 0 && KeyTabToChr[i][1] == codebuf[0])						
					{							
						QrCodeBuffer[g_QrCodeLen++] = KeyTabToChr[i][2];							
						break;						
					}					
				}				
			}			
		}						
		if (QrCodeBuffer[g_QrCodeLen-2] == '\r' && QrCodeBuffer[g_QrCodeLen-1] == '\n')			
		{				
			printf("qrcode string = %s \n", QrCodeBuffer);				
			//memset(QrCodeBuffer, 0, sizeof(QrCodeBuffer));	
			//g_QrCodeLen = 0;
			if (g_QRCodeRcvDataFg == 0)
			{
				for (i=0; i<g_QrCodeLen-2; i++)
					if ((QrCodeBuffer[i] < '0') || (QrCodeBuffer[i] > '9'))
							break;
				if (i == (g_QrCodeLen-2))
				{
					g_FgCardLanQRCode = 0;
					if ((QrCodeBuffer[0] == '3') && ((QrCodeBuffer[1] == '0')||(QrCodeBuffer[1] == '1')))
					{
						g_FgCardLanQRCode = 1;
					}
					memset(G_QRCodeInfo.id, 0, sizeof(G_QRCodeInfo.id));
					memcpy(G_QRCodeInfo.id, QrCodeBuffer, (g_QrCodeLen-2));
					G_QRCodeInfo.length = (g_QrCodeLen-2);
					g_QRCodeRcvDataFg = 1;
				}
			}
			g_QrCodeLen = 0;
			memset(QrCodeBuffer, 0, sizeof(QrCodeBuffer));
			usleep(100000);
			return 0;
		}        
			//  printf("input value 2 %d \n",keyvalue);            
			break;        
		}        
		else       
		{		  
			//printf("Read QrCode timeout  \n");          
			return -1;       
		}   
	}    
	return(0);
}
#endif

unsigned char GetHidDevStatus(void)
{	
	unsigned int i;	
	struct stat buf;	
	static unsigned char bflg = 0;	
	if(QRCode_fd <= 0)	
	{		
		system("cat /proc/bus/input/devices |grep \"Vendor=0525 Product=a4ac\" > /tmp/hid");			
		stat("/tmp/hid", &buf);		
		if(!buf.st_size)
		{			
			bflg = 0;			
			//printf("clear over ! \n");	
			sleep(1);
			return -1;		
		}		
		else		
		{			
			if(!bflg)
			{						
				bflg = 1;								
				if(GetHidDevInfo() != -1)				
				{					
					if((QRCode_fd = open(HidDevPath, O_RDONLY)) < 0)					
					{						
						printf("Can not open %s \n", HidDevPath);						
						QRCode_fd = 0;						
						return -1;					
					}					
					printf("QRCode_fd : %d \n", QRCode_fd);
					g_QrCodeLen = 0;
					Rcv_0x2A_Fg = 0;
					memset(QrCodeBuffer, 0, sizeof(QrCodeBuffer));
				}			
			}				
		}	
	}	
	else	{		
			read_HidDevInput();					
		//printf("***keyvalue:%d---\n",keyvalue);	
	}	
	return  (0);
}

void *UsbHid_Pthread (void * args)
{
	while(1)
	{
		GetHidDevStatus();
	}
}
#endif


int read_uart0_data(int fd,char *rcv_buf,int *len)
{
	int retval;
	fd_set rfds;
	struct timeval tv;
	int ret=0,pos=0;
	tv.tv_sec = 1;//set the rcv wait time 0
	tv.tv_usec = 0;//200000us = 0.3s 100000
	pos = 0;
	if ((fd < 0) ||( NULL == rcv_buf))
    {
        printf("Read Buffer is Nc\n");
        return -1;
    }
	
	while(1)
	{
		FD_ZERO(&rfds);
		FD_SET(fd,&rfds);
		retval = select(fd+1,&rfds,NULL,NULL,&tv);
		if(retval ==-1)
		{
			perror("select()");
			return -1;
		}
		else if(retval)
		{
			ret= read(fd, rcv_buf+pos, 38); //38
			//ret= read(fd, rcv_buf+pos, 22); //20
			
			if(ret>0)
			{
				//printf("Read Buffer = %d\n", ret);
				pos += ret;
				*len = pos;
			//	if (pos >= 20)   //20
			//return 0;
			    printf("read socket ret:%d data:%s \n",ret,rcv_buf);
				
			}
			else
				return -1;
		}
		else      //timeout
		{
			return 1;
		}
	}
	return 0;
}


int CheckQR()
{
    int ret=1;
    int cnt,rlen;
    char buf[10]={0x16,0x51,0x0d,0x30,0x30,0x31,0x31,0x48,0x3f,0x21};
    char RX_buff[64] = {"\0"};
    InitUart(&QRCode_fd, "ttyC0", 115200);     
     cnt = 0;
	 do {
	 	 rlen = 0;
		 memset(RX_buff,0,sizeof(RX_buff));  
	 	 write(QRCode_fd,buf,sizeof(buf)); 
		ret = read_uart0_data(QRCode_fd, RX_buff, &rlen);
        
		if (rlen)
		{
			if((strchr(RX_buff, 'F') !=  NULL) || (strchr(RX_buff, 'V') !=  NULL)) 
				break;
		}
		usleep(500000);
		cnt++;
	   }while(cnt < 2); //30s check network status
	   if(cnt<=2)
        ret = 0;
      close(QRCode_fd);
      return ret;      
    }



void *uart0_Pthread (void * args)
{
	int i, iLen=0;
	char rx_string[256] = {0};
	//struct timeval older, newer;

	#if 1
	InitUart(&QRCode_fd, "ttyC0", 115200);
	#else
	uart0_fd = initializegps("ttyC2", 115200);
	tcflush (uart0_fd, TCIFLUSH);
	#endif
	printf("welcome use ttyC0 = %d\n", QRCode_fd);
	
	
	while (1)
	{
		iLen = 0;
		signal(SIGPIPE, SIG_IGN);  //¹Ø±ÕSIGPIPEÐÅºÅ£¬·ÀËÀ»ú
		memset(rx_string, 0, sizeof(rx_string));
		read_uart0_data(QRCode_fd, rx_string, &iLen);
		#if 0
		if (iLen)
		{
			printf("iLENGTH = %d, strings = %s\n", iLen, rx_string);
			printf("starts ....\n");
			for (i=0; i<iLen; i++)
				printf("0x%02x ", rx_string[i]);
			printf("\n uart2 end ....\n");
		}
		#endif

		if ((iLen >= 20)&&(! g_QRCodeRcvDataFg))  //total length  more than 20 bytes
		{
			if  ((rx_string[iLen-2] == 0x0d) && (rx_string[iLen-1] == 0x0a))  //rcv finish , last two byte is \r\n
			{
				for (i=0; i<iLen-2; i++)  //total bytes are digit,except last two byte.
				{
					if ((rx_string[i] < '0') || (rx_string[i] > '9'))
						break;
				}
				if (i== (iLen-2))
				{
					g_FgCardLanQRCode = 0;
					if ((rx_string[0] == '3') && ((rx_string[1] == '0')||(rx_string[1] == '1')))
					{
						g_FgCardLanQRCode = 1;
					}
					memset(G_QRCodeInfo.id, 0, sizeof(G_QRCodeInfo.id));
					memcpy(G_QRCodeInfo.id, rx_string, (iLen-2));
					G_QRCodeInfo.length = (iLen-2);
					g_QRCodeRcvDataFg = 1;
				}
			}
		}
		usleep(100000);
	}

	if (QRCode_fd) close(QRCode_fd);
}
#if 1
int QRSaveExtData(void)
{
	int ret = 1;
	RecordFormat record;

	//if (ret == 0)
	{
		memset((unsigned char *)&record, 0, sizeof(record));
		memcpy(record.RFIccsn, DevSID.longbuf, 4);
		memcpy(record.RFDtac, DevNum.longbuf, 4);
		record.RFcsn[0] = 0x20;  //2017...
		memcpy(&record.RFcsn[1], &Time, 6);
		memcpy(&record.RFrove[3], G_QRCodeInfo.tranNo.longbuf, 4);
		record.RFvalueq[0] = G_QRCodeInfo.type;
		if (g_FgCardLanQRCode)
		{
			record.RFvalueq[1] = G_QRCodeInfo.length;
			memcpy(&record.RFvalueq[2], G_QRCodeInfo.id, G_QRCodeInfo.length);
		}
		else
		{
			record.RFvalueq[1] = G_QRCodeInfo.name_len;
			memcpy(&record.RFvalueq[2], G_QRCodeInfo.name, G_QRCodeInfo.name_len);
		}
       
		record.RFflag = CARD_SPEC_QR_CODE_EXT;
        
		record.RFXor = Save_Data_Xor((unsigned char *)&record);

		ret = Savedatasql(record, 0, 0);
		if (!ret)
		{
			IncSerId();
		}
	}

	return ret;
}
#endif
#endif


