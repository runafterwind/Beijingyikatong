
#include <apparel.h>

#ifdef CANGNAN_BUS
#include "RC500.h"
#include "InitSystem.h"
#include "OnlineRecharge.h"
#include "cpucard.h"
#include "../bzdes/stades.h"
#include "../bzdes/mac.h"
#include "../Display/fbtools.h"
#include "../sound/sound.h"


#define   APP_PRINTF    0
#define   KEYTEST       0

extern unsigned char PsamNum_bak1[6];
extern unsigned char PsamNum_bak2[6];
extern int mf_fd;
extern CardInform CardLan;
extern SysTime Time;
extern LongUnon DevNum,DecValue;
extern LongUnon DevSID;
extern LongUnon HostValue;
extern unsigned char PsamNum[6]; //终端机号
extern unsigned char SnBack[4]; //密钥
extern unsigned char KeyDes[8];	//密钥
extern unsigned char AutoUpFlag;
extern JackRegal Sector;
extern CardLanSector LanSec;		//用户扇区
extern SectionFarPar Section,Sectionup;
extern unsigned char SRCPUCardBuf[1024];
static LongUnon OldCash,OldCi;
static int  receive_len[1] = {0};
static char receive_buf[128]= {0};
extern RouteJack   TypeTime;
extern unsigned char PsamKeyIndex;

CpuMoney cpu_money;
FILE *CWhiteFile;
st_BlackFile BlackListFilezhujian;
LongUnon Buf1,DevSIDCPU,Valueq;
unsigned char CityNum[2]= {0x32,0x50};

#ifdef SAVE_CONSUM_DATA_DIRECT
#define CITYUNION_BL_FILEZHUJIAN 		"/mnt/record/Blacklistzhujian.sys"          //交通部黑名单
#else
#define CITYUNION_BL_FILEZHUJIAN 		"/var/run/Blacklistzhujian.sys"          //交通部黑名单
#endif

unsigned char DebitKey[] =  {0xD2,0x3C,0xDE,0x7A,0x93,0xE1,0xAE,0x70,0x25,0x6A,0x98,0xA8,0xA6,0xD4,0x1D,0xE7};
//消费密钥

unsigned char CrebitKey[] = {0xF6,0x91,0xA3,0xBB,0x81,0xED,0xC3,0xDB,0xE6,0x67,0x1C,0x51,0xD4,0xF0,0xF5,0x7A};
//圈存密钥   57415443484441544154696D65434F53

unsigned char MmasterKey[] = {0x57,0x41,0x54,0x43,0x48,0x44,0x41,0x54,0x41,0x54,0x69,0x6D,0x65,0x43,0x4F,0x53};
//主控密钥

unsigned char MaintainKey_DF01[] = {0xE4,0x1B,0xBB,0xF9,0xDF,0xBF,0x50,0x6B,0xAA,0xE8,0x97,0x79,0xE2,0x91,0x76,0x83};
//  3F01维护密钥

unsigned char NameDF01[]= {0xA0,0x00,0x00,0x00,0x03,0x86,0x98,0x07,0x01};
//DF01 名称

unsigned char Pinkey[]= {0x54,0x43,0x50,0x53};
//PIN KEY

unsigned char GetChall4[] = {0x00,0x84,0x00,0x00,0x04};
//取随机数 4

unsigned char GetChall8[] = {0x00,0x84,0x00,0x00,0x08};
//取随机数 8


unsigned char use_jiaotong_stander;

CardInformCPU CardLanCPU;

extern unsigned char bStolenDis;
extern unsigned char StolenAmount;
extern Interval  retdata;	//时间间隔
extern RecordFormat SaveData;

extern LongUnon SaveNum;                       //数据存储 存储记录的数量
extern LongUnon SaveNumBs;                     //数据存储
extern LongUnon SaveNumBc;                     //数据存储
extern LongUnon CodeNum; 		                // 已上传记录数
extern pthread_mutex_t m_datafile;
extern unsigned char Senddata[72];
extern unsigned char SendeghitDat[512];
extern LongUnon LBuf;
extern FILE *Datafile;
extern unsigned char SavedataErr ;
extern unsigned long long big_data;
extern LongUnon IDaddr;    					// 数据库 ID 号
extern int sockfd; 

extern time_t utc_time;
extern ShortUnon Driver;
extern unsigned char G_START[8];  				// 协议字
extern unsigned char G_STARTW[8]; 				// 协议字

#if defined(SUPPORT_QR_CODE)
extern unsigned int g_SendScanMarkCnt;
extern unsigned char g_FgQRCodeRcvAck;
//unsigned char g_QRSendCmdTimes;
extern unsigned char g_QRCodeRcvDataFg;

extern struct QRCode G_QRCodeInfo;
extern unsigned char g_FgCardLanQRCode;
#endif


typedef struct {
	unsigned char Type; //1:DDF,2:ADF
	unsigned char DFNameExist;//0-non exist;1-exist.
	unsigned char DFNameLen;
	unsigned char DFName[16]; //5-16,ADF or DDF name according to Type.
	unsigned char AppLabelExist;
	unsigned char AppLabelLen;
	unsigned char AppLabel[16];
	unsigned char PreferNameExist;
	unsigned char PreferNameLen;
	unsigned char PreferName[16];
	unsigned char PriorityExist;
	unsigned char Priority; //tag'87'
	unsigned char DirDiscretExist;
	unsigned char DirDiscretLen;
	unsigned char DirDiscret[222];
}RECORD_PPSE;

typedef union
{
	unsigned char buff[8];
	unsigned long long  i;
}BigUnion;

BigUnion BigData;


unsigned short Union_Card[]=
{
	0x0071,0x0130,0x1110,0x1120,0x1130,0x1150,0x1170,0x1210,
    0x1250,0x1251,0x1362,0x1380,0x2000,0x2140,0x2142,0x2144,
    0x2153,0x2154,0x2155,0x2230,0x2253,0x2260,0x2320,0x3000,
    0x3120,0x3130,0x3131,0x3140,0x3150,0x3160,0x3180,0x3210,
    0x3220,0x3250,0x3300,0x3320,0x3350,0x3410,0x3500,0x3511,
    0x3620,0x3622,0x3640,0x4102,0x4250,0x4331,0x4420,0x4500,
    0x4501,0x4511,0x4620,0x4630,0x4730,0x5190,0x5240,0x5580,
    0x5630,0x5720,0x6150,0x6217,0x6374,0x6430,0x6500,0x6710,
    0x7120,0x7121,0x7140,0x7190,0x7300,0x7309,0x7441,0x8340,
    0x9001
};



unsigned char SelectAppDF(char *DFname,char *Recvdata,unsigned  char *namelen);
unsigned char SecurityCertificate_zhujian(void);


void printbuf(char *buffer, int length)
{
	int i;

	for (i = 0; i < length; i++)
	{
		printf("%02X ", *(buffer + i));
		if ((i + 1) % 8 == 0) printf(" ");
		if ((i + 1) % 16 == 0) printf("\n");
	}
	printf("\n");
}


int MatchTermAID(unsigned char * aid,unsigned char aidLen)
{
	int i = -1;

	DBG_PRINTF("MatchTermAID() is called. AID:\n");
	menu_print(aid, aidLen);
		
    if(!memcmp(aid,"\xA0\x00\x00\x06\x32\x01\x01\x05",8))
    {
    	DBG_PRINTF("MatchTermAID(): AID matched!.\n");
    	return 1;
    }

		
	
	return i;
}

unsigned char SelectAID(unsigned char *input,unsigned char *inlen,unsigned char *AID,unsigned char *outlen)
{
	unsigned char DFName[16],DFNameLen;
	unsigned char state[2]={0};
	unsigned char j,t,k;
	int index,indexADF,len,lenAEF,lenFCI,matchNum;
	int indexRecord,indexEntry,lenRecord,lenEntry,indexFCI;
	int indexIssuerDiscret,lenIssuerDiscret;
	int indexPSEAID,lenPSEAID;	
	RECORD_PPSE recordPSE;
	int i, length;	
	
    state[0] = input[*inlen-2];
    state[1] = input[*inlen-1];
	DBG_PRINTF("SelectAID(): state = %02X%02X\n", state[0], state[1]);
	
	if(state[0]==0x90 && state[1]==0x00)   //DF select found ok.
	{
		memset((unsigned char *)&recordPSE,0,sizeof(RECORD_PPSE));
		index=0;
		if(input[index]!=0x6F)	return 1;
		index++;
		if(input[index]<=127)
		{
			lenRecord=input[index];
			index++;
		}
		else
		{
			lenRecord=0;
			t=input[index]&0x7F;
			for(j=1;j<=t;j++)
			{
				lenRecord=lenRecord*256+input[index+j];
			}
			index+=t+1;
		}
        DBG_PRINTF("SelectAID(): lenRecord = %02X\n", lenRecord);
		indexRecord=index;
		while(index<indexRecord+lenRecord)
		{
			if(index>=(*inlen)) return 1;
			if(input[index]==0xff || input[index]==0x00) {index++;continue;}
			else if(input[index]==0x84)
			{
				DBG_PRINTF("SelectAID(): found tag: %02X\n", input[index]);

				if(recordPSE.DFNameExist==1) return 1;
				index++;
				recordPSE.DFNameLen=input[index];
				index++;
				memcpy(recordPSE.DFName,input+index,recordPSE.DFNameLen);
				index+=recordPSE.DFNameLen;
				recordPSE.DFNameExist=1;
				recordPSE.Type=2;
			}
			else if(input[index]==0xA5)
			{
				DBG_PRINTF("SelectAID(): found tag: %02X\n", SRCPUCardBuf[index]);
			
				index++;
				if(input[index]<=127)
				{
					lenFCI=input[index];
					index++;
				}
				else
				{
					lenFCI=0;
					t=input[index]&0x7F;
					for(j=1;j<=t;j++)
					{
						lenFCI=lenFCI*256+input[index+j];
					}
					index+=t+1;
				}
				indexFCI=index;

				while(index<indexFCI+lenFCI)
				{
					if(index>=(*inlen)) return 1;
					if(input[index]==0xff || input[index]==0x00) {index++;continue;}
					else if(!memcmp(input+index,"\xBF\x0C",2))
					{
					DBG_QPBOC_PRINTF("SelectAID(): found tag: %02X%02X\n", input[index], input[index + 1]);
					
						
						if(recordPSE.DirDiscretExist==1) return 1;
						index+=2;
						if(input[index]<=127)
						{
							lenIssuerDiscret=input[index];
							index++;
						}
						else
						{
							lenIssuerDiscret=0;
							t=input[index]&0x7F;
							for(j=1;j<=t;j++)
							{
								lenIssuerDiscret=lenIssuerDiscret*256+input[index+j];
							}
							index+=t+1;
						}
						recordPSE.DirDiscretLen=lenIssuerDiscret;
						memcpy(recordPSE.DirDiscret,input+index,lenIssuerDiscret);
						index+=lenIssuerDiscret;
						recordPSE.DirDiscretExist=1;
					}
					else  //other unknown TLV data
					{
						k=input[index];
						if((k&0x1F)==0x1F)	index++;
						index++;
						if(input[index]<=127)
						{
							len=input[index];
							index++;
						}
						else
						{
							len=0;
							t=input[index]&0x7F;
							for(j=1;j<=t;j++)
								len=len*256+input[index+j];
							index+=t+1;
						}
						index+=len;
					}
				}
				if(index!=indexFCI+lenFCI)
					return 1;
               
			}
			else 
			{
				k=input[index];
				if((k&0x1F)==0x1F) index++;
				index++;
				if(input[index]<=127)
				{
					len=input[index];
					index++;
				}
				else
				{
					len=0;
					t=input[index]&0x7F;
					for(j=1;j<=t;j++)
					{
						len=len*256+input[index+j];
					}
					index+=t+1;
				}
				index+=len;
			}
		}

		
		if(index!=indexRecord+lenRecord)
		{
			 return 1;
		}
	}
	else
	{
        return 1;
        }

	DBG_PRINTF("SelectAID(): recordPSE:\n");
	length = (int)&recordPSE.DirDiscret[0] - (int)&recordPSE;
	menu_print((char *)&recordPSE, length);	

    index=0;

	while(index<recordPSE.DirDiscretLen)
	{
		if(recordPSE.DirDiscretExist!=1) return 1;		
		if(recordPSE.DirDiscret[index]==0xFF || recordPSE.DirDiscret[index]==0x00) {index++;continue;}
		else if(recordPSE.DirDiscret[index]==0x61)
		{
			DBG_QPBOC_PRINTF("TERMAPP_SelectDDF(): found tag: %02X\n", recordPSE.DirDiscret[index]);
		
			 index++;
			 if(recordPSE.DirDiscret[index]<=127)
			 {
			 	  lenPSEAID=recordPSE.DirDiscret[index];
				  index++;
			 }
			 else
			 {
			     lenPSEAID=0;
				 t=recordPSE.DirDiscret[index]&0x7F;
				 for(j=1;j<=t;j++)
				 {
				 	lenPSEAID=lenPSEAID*256+recordPSE.DirDiscret[index+j];
				 }
				 index+=t+1;
			 }
			 indexPSEAID=index;

			 while(index<indexPSEAID+lenPSEAID)
			 {
			     if(index>=recordPSE.DirDiscretLen) return 1;
				 if(recordPSE.DirDiscret[index]==0xFF ||recordPSE.DirDiscret[index]==0x00) {index++;continue;}
				 else if(recordPSE.DirDiscret[index]==0x4f)
				 {
					DBG_QPBOC_PRINTF("TERMAPP_SelectDDF(): found tag: %02X\n", recordPSE.DirDiscret[index]);
				 
					index++;
					recordPSE.DFNameLen=recordPSE.DirDiscret[index];
					index++;
					memcpy(recordPSE.DFName,recordPSE.DirDiscret+index,recordPSE.DFNameLen);
					index+=recordPSE.DFNameLen;
					recordPSE.DFNameExist=1;
					recordPSE.Type=2;
				 }
				 else if(recordPSE.DirDiscret[index]==0x50)
				 {
					DBG_QPBOC_PRINTF("TERMAPP_SelectDDF(): found tag: %02X\n", recordPSE.DirDiscret[index]);
				 
				 	index++;
					recordPSE.AppLabelLen=recordPSE.DirDiscret[index];
					index++;
					memcpy(recordPSE.AppLabel,recordPSE.DirDiscret+index,recordPSE.AppLabelLen);
					index+=recordPSE.AppLabelLen;
					recordPSE.AppLabelExist=1;
				 }
				 else if(!memcmp(recordPSE.DirDiscret+index,"\x9F\x12",2))
				 {
					DBG_QPBOC_PRINTF("TERMAPP_ReadSelectRetData(): found tag: %02X%02X\n", recordPSE.DirDiscret[index], recordPSE.DirDiscret[index + 1]);
				 
				 	index+=2;
					recordPSE.PreferNameLen=recordPSE.DirDiscret[index];
					index++;
					memcpy(recordPSE.PreferName,recordPSE.DirDiscret+index,recordPSE.PreferNameLen);
					index+=recordPSE.PreferNameLen;
					recordPSE.PreferNameExist=1;
				 }
				 else if(recordPSE.DirDiscret[index]==0x87)
				 {
					DBG_QPBOC_PRINTF("TERMAPP_SelectDDF(): found tag: %02X\n", recordPSE.DirDiscret[index]);

					index+=2;
					recordPSE.Priority=recordPSE.DirDiscret[index];
					index++;
					recordPSE.PriorityExist=1;
				 }
				 else
				 {
				 	k=recordPSE.DirDiscret[index];
					if((k&0x1F)==0x1F) index++;
					index++;
					k=recordPSE.DirDiscret[index];
					index+=k+1;
				 }
			 }
			if(index!=indexPSEAID+lenPSEAID)
			 	return 1;
			if(recordPSE.Type==2 && recordPSE.DFNameExist==1)
			{
				matchNum=MatchTermAID(recordPSE.DFName,recordPSE.DFNameLen);	
				if(matchNum < 0)
				{
					continue;
				}
                else
                   {
                    memcpy(AID,recordPSE.DFName,recordPSE.DFNameLen);
                   *outlen = recordPSE.DFNameLen;
                    return 0;
                    }
				
				
			}
			else
			{
				return 1;
			}
		}
		else
		{
			k=recordPSE.DirDiscret[index];
			if((k&0x1F)==0x1F) index++;
			index++;
			k=recordPSE.DirDiscret[index];
			index+=k+1;
		}
	}

	if(index!=recordPSE.DirDiscretLen)
		return 1;    

    
		
	return 1;
}


void MoneyChange(unsigned int Monei,unsigned char *Moneo)
{
	LongUnon Lbuf;
	unsigned char i;

	Lbuf.i = Monei;
	for(i=0; i<4; i++)
	{
		Moneo[i] = Lbuf.longbuf[3-i];
	}
}


/*
*************************************************************************************************************
- 函数名称 : char Savedatasql(RecordFormat Save)
- 函数说明 : 保存数据
- 入口参数：
- 输出参数 :
*************************************************************************************************************
*/
char Savedatasql_Zhujian(RecordFormat Save,int flag,int tao)  //flag 0:消费数据 1:银行卡EMV数据  tao:0 正常数据 1:逃票数据
{
	int result;
	unsigned int Saddrs;
	unsigned char loop;
	unsigned char DXOR,t,status = 1;
	unsigned char dbuff[8];
	unsigned char dtatabuf[72];
    unsigned char Timebuf[8];
	unsigned char *saddr;


	if(SaveNum.i > 100000)
	{
    	FindSavedata();
	}

	printf("Enter Savedatasql !SavedataErr=%d\n",SavedataErr);

	if((SavedataErr == 0)||(SavedataErr == 2))
	{
    	pthread_mutex_lock(&m_datafile);
#ifdef	SAVE_CONSUM_DATA_DIRECT
		Datafile = open(OFF_LINE_CONSUM_FILE,O_RDWR|O_SYNC);
#else
    	Datafile = fopen(OFF_LINE_CONSUM_FILE,"rb+");
#endif
    	if(Datafile < 0)
    	{
        		printf("open cardsave.bin Err\n");
		//Datafile = open(OFF_LINE_CONSUM_FILE,O_RDWR|O_SYNC);
        		return 1;
    	}
		printf("open cardsave.bin\n");
    	loop = 1;
		
    	while(loop)
    	{
    		memset(dtatabuf,0,sizeof(dtatabuf));
    		Saddrs = (unsigned int)((SaveNum.i -1) * 72);
#ifdef	SAVE_CONSUM_DATA_DIRECT
			result = lseek(Datafile,Saddrs, SEEK_SET);
			result = read(Datafile, dtatabuf, sizeof(dtatabuf));
#else
    		result = fseek(Datafile,Saddrs, SEEK_SET);
    		result = fread(dtatabuf,sizeof(unsigned char),sizeof(dtatabuf),Datafile);
#endif			
    		for(t=0; t<8; t++)
    		{
        		memset(dbuff,0,sizeof(dbuff));
        		memcpy(dbuff,dtatabuf+t*8,8);
        		if(mystrncmp(dbuff,"\x00\x00\x00\x00\x00\x00\x00\x00",8) != 0)
        		{
        			t= 0;
        			break;
        		}
    		}
    		if(t >= 8)
    		{
        		loop = 0;
        		break;
    		}
    		else
    		{
#if SqlOPEN
        		printf(" SaveNum  tt  == %d \n",t);
#endif
        		SaveNum.i += 1;
        		if(SaveNum.i > 200000)
        		{
        			printf("SavedataErr error\n");
					SavedataErr = 1;
					status = 1;
					loop = 0;
					break;
        		}
    		}

#if   SqlOPEN
    		printf("Savedatasql 1 == %d \n",SaveNum.i);
#endif
    	}

    	loop = 0;
	
    	if(SavedataErr == 0)
    	{
    		for(loop = 5; loop>0; )
    		{
			memset(dtatabuf,0,sizeof(dtatabuf));
			saddr = (unsigned char *)&Save;
			memcpy(dtatabuf,saddr,64);
			if(1)   //EMV数据增加终端机号与交易时间
			{

            #ifdef SUPPORT_QR_CODE	
               Rd_time (Timebuf + 1);
        		Timebuf[0] = 0x20;
        	 if (Sector.FlagValue == QR_CODE_WECHAT_TYPE || Sector.FlagValue == QR_CODE_ALIPAY_TYPE)
        	 {
        		memcpy(dtatabuf+64,Timebuf,4);
                dtatabuf[68]=0x0;
				dtatabuf[69]=0x0;
				dtatabuf[70]=0x0;
        	 }
             else
             {
                memcpy(dtatabuf+64,CardLan.Effective,4);
                dtatabuf[68]=0x0;
				dtatabuf[69]=0x0;
				dtatabuf[70]=0x0;
                }
        	// memset(SaveData.RFcsn, 0,4);
            #else               
    			memcpy(dtatabuf+64,CardLan.Effective,4);
                dtatabuf[68]=0x0;
				dtatabuf[69]=0x0;
				dtatabuf[70]=0x0;
               #endif                 
			}
			else
			{
				dtatabuf[64]=0x0;
				dtatabuf[65]=0x0;
				dtatabuf[66]=0x0;
				dtatabuf[67]=0x0;
				dtatabuf[68]=0x0;
				dtatabuf[69]=0x0;
				dtatabuf[70]=0x0;
			}
			dtatabuf[71] = 0x55;

#ifdef	SAVE_CONSUM_DATA_DIRECT				
    		result = lseek(Datafile,Saddrs, SEEK_SET);
    		result = write(Datafile,dtatabuf,sizeof(dtatabuf));				
#else
    		result = fseek(Datafile,Saddrs, SEEK_SET);
    		result = fwrite(dtatabuf,sizeof(unsigned char),sizeof(dtatabuf),Datafile);
#endif
		
			memset(dtatabuf,0,sizeof(dtatabuf));
#ifdef	SAVE_CONSUM_DATA_DIRECT	
			result = lseek(Datafile,Saddrs, SEEK_SET);
			result = read(Datafile,dtatabuf,sizeof(dtatabuf));
#else
			result = fseek(Datafile,Saddrs, SEEK_SET);
			result = fread(dtatabuf,sizeof(unsigned char),sizeof(dtatabuf),Datafile);
#endif				

			DXOR  = Save_Data_Xor(dtatabuf);
			if(DXOR == dtatabuf[63])
			{
				break;
			}
			else
			{
				loop --;
#if   SqlOPEN
				printf("\n\n save data  loop=%d \n\n",loop);
#endif
    		}
		}
	}
#ifdef	SAVE_CONSUM_DATA_DIRECT	
	close(Datafile);
#else
    	fclose_nosync(Datafile);
#endif
    	pthread_mutex_unlock(&m_datafile);

    	if(loop == 0)
    	{
			DebugPrintf("SavedataErr [%02X] status [%02X]\n",SavedataErr,  status);
			SavedataErr = 1;
			status = 1;
    	}
    	else
    	{
    		SaveNum.i += 1;
    		ReadOrWriteFile(WSDATA);
        	status = 0;
    	}
	}
	else
	{
    	status = 1;
	}

	return status;
}



char SaveCardData_Zhujian(int sepc, int Mode, int OperationFlag)
{
    DebugPrintf("\n");
	int i;
	LongUnon LBufVal;
	char status;
    unsigned char Timebuf[8];
	unsigned char *strcharr;
	unsigned char buffin[8];
	unsigned char buffino[8];
	unsigned char buffout[6];


 //   printf("use_jiaotong_stander=%02x\n",use_jiaotong_stander);
	if( OperationFlag & GET_RECORD )
	{
		//---start
   		memset(&SaveData,0,sizeof(RecordFormat));
		//---0
		if((CardTypeIC == 0x08)||(use_jiaotong_stander == 0x00))
	        memcpy(SaveData.RFIccsn,CardLan.UserIcNo,4);
		else
			memcpy(SaveData.RFIccsn,CardLanCPU.CSN,4);
		//---2
		if((CardTypeIC == 0x08)||(use_jiaotong_stander == 0x00))
		{
            
			bcd_to_bin(CardLan.CardCsnB, 4);
			BigData.i = big_data;
			memcpy(SaveData.RFcsn,BigData.buff,8);
		}
		else
		{
           
			bcd_to_bin(CardLan.Appcsn, 8);
            BigData.i = 0;
			BigData.i = big_data; 
            DebugPrintf("big_data:%lld\n",big_data);
			memcpy(SaveData.RFcsn,BigData.buff,8);
		}

	 #ifdef SUPPORT_QR_CODE	
	 if (Sector.FlagValue == QR_CODE_WECHAT_TYPE || Sector.FlagValue == QR_CODE_ALIPAY_TYPE)
	 {
		memcpy(SaveData.RFcsn, G_QRCodeInfo.tranNo.longbuf, 4);
	 }
	// memset(SaveData.RFcsn, 0,4);
	 #endif

        
		//---3
		if(CardTypeIC == 0x08)
	        memcpy(SaveData.RFrove,DevSID.longbuf,4);
		else
			{
                if(use_jiaotong_stander==0)
                    Chang4to4(DevSID.longbuf, SaveData.RFrove);
                else
                    memcpy(SaveData.RFrove,CardLanCPU.PSAMOfflineSN,4);
          }
        
    		memcpy(LBufVal.longbuf, DevSID.longbuf, 4);
		//---4
		if((CardTypeIC == 0x08)||(use_jiaotong_stander == 0x00))  	
    			SaveData.RFtype = CardLan.CardType;
		else
			//SaveData.RFtype = CardLanCPU.cardtype;
			SaveData.RFtype = 0;     //客户要求
		//---5
        		SaveData.RFXFtype = 0x00;

	//	printf("CardLan.StationDEC=%02x\n",CardLan.StationDEC);
        		if(CardLan.StationDEC != 0xaa)
        		{
            		if(Sector.FlagValue == 0)
            		{
        				memcpy(SaveData.RFvalueq,CardLan.Views,3);
        				memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
        				memcpy(LBufVal.longbuf,CardLan.Views,4);
        				LBufVal.i = LBufVal.i - HostValue.i;
        				memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
        				memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
            		}
        			else if(Sector.FlagValue == 1)
        			{
        				memcpy(SaveData.RFvalueq,CardLan.Subsidies,3);
        				memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
        				memcpy(LBufVal.longbuf,CardLan.Subsidies,4);
        				LBufVal.i = LBufVal.i - HostValue.i;
        				memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
        				memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
        			}
            		else if(Sector.FlagValue == 2)
            		{
						
		        		memcpy(SaveData.RFvalueq,CardLan.QCash,3);
						
        				memcpy(SaveData.RFvaluej,HostValue.longbuf,3);						
    					memcpy(LBufVal.longbuf,CardLan.QCash,4);
    					LBufVal.i = LBufVal.i - HostValue.i;
    					memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
						
						
        				memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
            		}
            		#if SUPPORT_QR_CODE
            		else if (Sector.FlagValue == QR_CODE_WECHAT_TYPE || Sector.FlagValue == QR_CODE_ALIPAY_TYPE)
            		{
                            printf("aaaaaaaaaaaaaaaaaaaaa\n");
            				memset(SaveData.RFvalueq,0,4);
            				memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
            				memset(SaveData.RFvalueh,0,4);
            				memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
            		}
            		#endif                    
            		
        		}
        		else
        		{
            		if(Sector.FlagValue == 1)
            		{
        				memcpy(SaveData.RFvalueq,CardLan.Subsidies,3);
        				memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
        				memcpy(LBufVal.longbuf,CardLan.Subsidies,4);
        				LBufVal.i = LBufVal.i + HostValue.i;
        				memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
        				memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
            		}
            		else if(Sector.FlagValue == 2)
            		{
        				memcpy(SaveData.RFvalueq,CardLan.QCash,3);
        				memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
        				memcpy(LBufVal.longbuf,CardLan.QCash,4);
        				LBufVal.i = LBufVal.i + HostValue.i;
        				memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
        				memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
            		}
        		}

	//	printf("utc_time=%d\n",utc_time);
	
		SaveData.RFtime[0] = (utc_time&0xFF)>>0;
		SaveData.RFtime[1] = (utc_time&0xFF00)>>8;
		SaveData.RFtime[2] = (utc_time&0xFF0000)>>16;
		SaveData.RFtime[3] = (utc_time&0xFF000000)>>24;     


        SaveData.RFtran = 0x01;//公交		
	    memset(SaveData.RFMoneyJF,0,sizeof(SaveData.RFMoneyJF));
    #ifdef SUPPORT_QR_CODE	
       Rd_time (Timebuf + 1);
		Timebuf[0] = 0x20;
	 if (Sector.FlagValue == QR_CODE_WECHAT_TYPE || Sector.FlagValue == QR_CODE_ALIPAY_TYPE)
	 {
		memcpy(SaveData.RFMoneyJF,Timebuf,4);
	 }
     else
     {
        memcpy(SaveData.RFMoneyJF,CardLan.EnableH,4);
        }
	// memset(SaveData.RFcsn, 0,4);
    #else
       memcpy(SaveData.RFMoneyJF,CardLan.EnableH,4); 
	 #endif            
    					
  
		if((CardTypeIC == 0x08)||(use_jiaotong_stander == 0x00))
			memset(SaveData.RFMoneyJFbuf,0,sizeof(SaveData.RFMoneyJFbuf));
		else
			memcpy(SaveData.RFMoneyJFbuf,CardLan.CityId,2);
        
		if((CardTypeIC == 0x08)||(use_jiaotong_stander == 0x00))
			memcpy(SaveData.RFtac, CardLan.ViewMoney,3);
		else
		{
			SaveData.RFtac[0] = (((CardLanCPU.offlineSN[0]<<8|CardLanCPU.offlineSN[1])+1)&0xFF)>>0;
			SaveData.RFtac[1] = (((CardLanCPU.offlineSN[0]<<8|CardLanCPU.offlineSN[1])+1)&0xFF00)>>8;
			SaveData.RFtac[2] = 0x00;
		}
		memcpy(LBufVal.longbuf, CardLan.ViewMoney, 4);

        		if( CardLan.StationDEC == 0x55 )
        		{
            		SaveData.RFpurse = Sector.FlagValue;
        		}
        		else if(CardLan.StationDEC == 0xaa)
        		{
            		SaveData.RFpurse = Sector.FlagValue + 3;
        		}
        		else
        		{
            		SaveData.RFpurse = Sector.FlagValue;
        		}

    		SaveData.RFtimeno = TypeTime.TimeNum;
			
		if((CardTypeIC == 0x08)||(use_jiaotong_stander == 0x00))
    			memcpy(SaveData.RFderno,DevNum.longbuf,4);
		else
			memcpy(SaveData.RFderno,&PsamNum[2],4);
		if((CardTypeIC == 0x08)||(use_jiaotong_stander == 0x00))
    			SaveData.RFEnterOrExit = CardLan.EnterExitFlag;
		else
			SaveData.RFEnterOrExit = CardLanCPU.enterexitflag;

        		if(CardLan.EnterExitFlag == 0x01||CardLanCPU.enterexitflag == 0x55)
        		{
					SaveData.RFcarp = Section.Updown;
					SaveData.RFStationID = Section.SationNow - 1;
        		}
        		else
        		{
					
						SaveData.RFcarp = CardLan.StationOn;
						if((Section.SationNow == 1)&&(CardLan.StationOn == 0)&&(CardLan.StationOn != Section.Updown))
						{
			                			SaveData.RFStationID = Section.SationNum[0] - 1;
			            		}
			            		else if((Section.SationNow == 1)&&(CardLan.StationOn == 1)&&(CardLan.StationOn != Section.Updown))
			            		{
			                			SaveData.RFStationID = Sectionup.SationNum[0] -1;
			            		}
			            		else
			            		{
			                			SaveData.RFStationID = Section.SationNow - 1;
			            		}
					
				
        		}
				
		memcpy(SaveData.RFoperator,PsamNum,2);
		memcpy(&SaveData.RFoperator[2],Driver.intbuf,2);
		SaveData.RFflag = sepc;

		SaveData.RFspare = (unsigned char)Mode;	// (定额:0 自由:1 分段:2 计时:3)
		
		if(SaveData.RFflag == 1)					//wsl
		{            
          memcpy(SaveData.RFDtac,CardLan.CardTac,4);            
		}
		else
		{
			//设置des加密密码
			memcpy(buffin,SaveData.RFrove,4);
			memcpy(buffin+4,SaveData.RFderno,4);
			
			memcpy(buffino,CardLan.UserIcNo,4);
			memcpy(buffino+4,SnBack,4);
			DES_CARD(buffin,buffino,buffout);
			memcpy(SaveData.RFDtac,buffout+1,4);
		}
	//	printf("SaveData.RFpurse=%d\n",SaveData.RFpurse);
		strcharr = (unsigned char *)(&SaveData);
		SaveData.RFXor = Save_Data_Xor(strcharr);

		DBG_PRINTF("保存的数据:\n");
		for(i=0;i<64;i++)
			DBG_PRINTF("%02X ",strcharr[i]);
		DBG_PRINTF("\n");

	}	

	if( OperationFlag & SAVE_RECORD )
	{

		status = Savedatasql_Zhujian(SaveData,0,0);
		IncTransactionNum();

	//	printf("####SAVE_RECORD status = 0x%02X\n", status);
	}

    	return  status;
}

unsigned char CardKey(char *keydata)
{
	unsigned char buff[16];
	unsigned char buf1[16];
	//unsigned char keybuff[16];

	//SnBack
	// KeyDes
	memcpy(buff,KeyDes,8);
	Not(buff+8,KeyDes,8);

	memcpy(buf1,CardLan.UserIcNo,4);
	memcpy(buf1+4,SnBack,4);
	Not(buf1+8,buf1,8);

	RunDes(0,0,buf1,keydata,8,buff,16);
	RunDes(0,0,buf1+8,keydata+8,8,buff,16);
	return 0;
}



unsigned char Check_CardDate()
{
    unsigned status = 0;
    unsigned int start_time,end_time;
    
    start_time = CardLanCPU.appstarttime[0]<<24|CardLanCPU.appstarttime[1]<<16|CardLanCPU.appstarttime[2]<<8|CardLanCPU.appstarttime[3];
	end_time = CardLanCPU.appendtime[0]<<24|CardLanCPU.appendtime[1]<<16|CardLanCPU.appendtime[2]<<8|CardLanCPU.appendtime[3];
    if((start_time==0xffffffff)||(end_time==0xffffffff)||(start_time==0x0)||(end_time==0x0))
    {
        status = 1;
        }

    return status;
    }


/*
*********************************************************************
函数:
unsigned char SelectAppDF(char *DFname,char *Recvdata,unsigned  char *namelen)
功能:
输入:
      DFname
      Recvdata
      namelen

输出:
	status  0 成功  1 失败
*********************************************************************
*/

unsigned char SelectAppDF(char *DFname,char *Recvdata,unsigned  char *namelen)
{
	int result;
	char Send[256];
	unsigned char len;
	unsigned char selfileDF[]= {0x00,0xa4,0x04,0x00}; //,0x09,0xA0,0x00,0x00,0x00,0x03,0x86,0x98,0x07,0x01,0x00};
	unsigned char selfileDFFci[]= {0x00,0xa4,0x00,0x00}; //,0x09,0xA0,0x00,0x00,0x00,0x03,0x86,0x98,0x07,0x01,0x00};
    char typesa[5];
	//选择应用 DF01  电信应用(3F01 -- 3F06)

/*
	memcpy(Send,"\x00\xb0\x85\x00\x00",5);

    result = write(mf_fd,Send,5);
    if(result == MI_OK)
    {
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);  
	
	    if(receive_len[0] > 2) {
				
		if (CityNum[0] == receive_buf[2] && 
				CityNum[1] == receive_buf[3]) {
			CardLan.CardType = receive_buf[17]; //卡类

			CardLan.CardTypebak = CardLan.CardType; 
			sprintf(typesa,"%02x",CardLan.CardType);
			CardLan.CardType=atoi(typesa);
			CardLan.CardType = 5;           //测试使用卡类1
			//read_05_maintype = receive_buf[16];

			//if (CardLan.CardType <= 20) {
				
				//CardLan.CardType = native_cpu_type_switch(1,CardLan.CardType);
			//}
		} 

	}
             }

*/

    CardLan.CardType = 5;           //测试使用卡类1
	DBG_PRINTF("SelectAppDF() is called.\n");
    DBG_PRINTF("DFName=%s\n",DFname);
	memset(Send,0,sizeof(Send));

	if(*namelen == 0x02)
	{
		memcpy(Send,selfileDFFci,sizeof(selfileDFFci));
	}
	else
	{
		memcpy(Send,selfileDF,sizeof(selfileDF));
	}
	Send[4] = *namelen;
	len = *namelen;
	memcpy(Send+5,DFname,len);
	len = len + 6;
    memset(receive_buf,0,sizeof(receive_buf));
	result = write(mf_fd,Send,len);
	if(result == MI_OK)
	{
		ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
		result = read(mf_fd, receive_buf, receive_len[0]);
		if(receive_len[0] >= 2)
		{
			memcpy(Recvdata,receive_buf,receive_len[0]);
			*namelen = receive_len[0];
			result = 0;
		}
		else
		{
			result = 1;
		}
#if 1
		{
			unsigned char i;
            DBG_PRINTF("selfileDF write data = %d\n",len);
			for(i =0 ; i < len; i++)
			{
				DBG_PRINTF("%02X ",Send[i]);
			}
			DBG_PRINTF("\n");            
			DBG_PRINTF("selfileDF get data = %d\n",receive_len[0]);
			for(i =0 ; i < receive_len[0]; i++)
			{
				DBG_PRINTF("%02X ",Recvdata[i]);
			}
			DBG_PRINTF("\n");
		}
#endif
	}

	return result;

}


/*
*********************************************************************
函数:unsigned char UpdataBin(char *BinName,char *data,unsigned short datastart,unsigned char datalen,char *Key,unsigned char keylen)
输入:
     BinName   文件名
     data      写入的数据
     datastart 数据的开始地址
     datalen   写入的数据长度
     Key       更新数据的密钥
     keylen    密钥长度
输出:
   status  0 成功  1 失败
*********************************************************************
*/
unsigned char UpdataBin(char BinName,char *data,unsigned char datastart,unsigned char datalen,char *Key,unsigned char keylen)
{
	int result;
	unsigned char flag,t,status;
	unsigned char buff1[8],buff2[8];
	unsigned char Send[256],Recv[256];
	// unsigned char SelectFile[] = {0x00,0xa4,0x00,0x00,0x02};


	if(datalen > 122) return 1;

	flag = 1;
	t = 1;

	while(flag)
	{
		switch(t)
		{
		case 2:
			result = write(mf_fd,GetChall4, sizeof(GetChall4));
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
					memcpy(Recv,receive_buf,receive_len[0]);
					t++;
				}
				else
				{
					flag = 0;
				}
#if APP_PRINTF
				{
					unsigned char i;

					printf("GetChall4 get data = %d\n",receive_len[0]);
					for(i =0 ; i < receive_len[0]; i ++)
					{
						printf("%02X ",receive_buf[i]);
					}
					printf("\n");
				}
#endif

			}
			else flag = 0;
			break;

		case 3:
			memset(buff1,0,sizeof(buff1));
			memset(Send,0,sizeof(Send));
			memcpy(buff1,Recv,4);
			Send[0] = 0x04;
			Send[1] = 0xd6;
			Send[2] = BinName|0x80;
			Send[3] = datastart;
			Send[4] = datalen+4;
			memcpy(Send+5,data,datalen);
			status = MacAnyLength(buff1,Send,buff2,(datalen+5),Key,keylen);
			if(status == MI_OK)
			{
				memcpy(Send+datalen+5,buff2,4);
				result = write(mf_fd,Send,(datalen+9));
				if(result == MI_OK)
				{
					ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
					result = read(mf_fd, receive_buf, receive_len[0]);
					if((receive_len[0]==0x02)&&(receive_buf[0]==0x90)&&(receive_buf[1]==0x00))
					{
						t++;
					}
					else
					{
						flag = 0;
					}

#if APP_PRINTF
					{
						unsigned char i;

						printf("UpdataBin get data = %d\n",receive_len[0]);
						for(i =0 ; i < receive_len[0]; i ++)
						{
							printf("%02X ",receive_buf[i]);
						}
						printf("\n");
					}
#endif

				}
			}
			else flag = 0;
			break;

		default :
			t = 0;
			flag = 0;
			break;
		}
	}
	return t;
}


unsigned char SecondDebitChas(unsigned int Money,char *Key,unsigned char Keylen)
{
	int result;
	unsigned short ic;
	unsigned char Timebuf[8],Mbuff[6];
	char Send[128],Recv[128];
	char buff2[8];
	unsigned char flag,t,status,len,Qflag,ErrNumo;
	//char *UserPIN,unsigned char pinlen


	//unsigned char selfileDF[]={0x00,0xa4,0x04,0x00,0x09,0xA0,0x00,0x00,0x00,0x03,0x86,0x98,0x07,0x01,0x00};
	//选择应用 DF01  电信应用(DF01 -- DF06)


	//   unsigned char VerPIN[]= {0x00,0x20,0x00,0x00}; //,0x04,0x54,0x43,0x50,0x53;
	//校验PIN

	char InitChas[]= {0x80,0x50,0x01,0x02,0x0b}; //,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x0f;
	// 消费初始化  KEY:0x00  金额:0.01元 终端机号:1号

	char DebitChas[]= {0x80,0x54,0x01,0x00,0x0f}; //,0x00,0x00,0x00,0x01,0x20,0x12,0x09,0x24,0x15,0x26,0x00; //+ MAC1
	// 消费   交易序号: 1 交易时间:20120924152600 + MAC1


	//unsigned char InitLoad[]={0x80,0x50,0x00,0x02,0x0b};//,0x01,0x00,0x00,0x00,0xff,0x00,0x00,0x00,0x00,0x00,0x01,0x10;
	// 圈存初始化 KEY:0x01  金额:2.25元 终端机号:1号

	//unsigned char CrebitChas[]={0x80,0x52,0x00,0x00,0x0b};//,0x20,0x12,0x09,0x24,0x15,0x26,0x00; //+ MAC2
	// 圈存  交易时间:20120924152600 + MAC2

	char SamMac1[]= {0x80,0x70,0x00,0x00,0x24};
	char SamMac2[]= {0x80,0x72,0x00,0x00,0x04};
	LongUnon Buf;
	SysTime BakTime;

#if APP_PRINTF
	printf("SecondDebitChas start  Money == %d \n",Money);
#endif
	LEDL(1);
	beepopen(10);
	SetColor(Mcolor);
	SetTextColor(Color_red);
	SetTextSize(32);
	TextOut(100 , 50, "温馨提示");
	TextOut(100 , 90, "刷卡错误");
	TextOut(85  , 130,"请重新刷卡");

	ErrNumo = 0;
	Qflag = 1;
	ic = 0;

	memcpy(&BakTime, &Time, sizeof(Time));		
	while(Qflag)
	{
		SetTextColor(Color_red);
		SetTextSize(32);
		TextOut(100 , 50, "温馨提示");
		TextOut(100 , 90, "刷卡错误");
		TextOut(85  , 130,"请重新刷卡");

		Rd_time (Timebuf+1);

		if(Time.sec != Timebuf[6] || Time.min != Timebuf[5])
			{
				Timebuf[0] = 0x20;
				Time.year = Timebuf[1];
				Time.month = Timebuf[2];
				Time.day = Timebuf[3];
				Time.hour = Timebuf[4];
				Time.min = Timebuf[5];
				Time.sec = Timebuf[6];

				ic++;
				if(ic>=28)
				{
					Qflag =0;
		//			IncSerId(); 
		//			SaveCardData(3);
					break;
				}
				if((ic>3)&&(ic%4 == 0))
				{
					PlayMusic(13,0);
				}

			}
		flag = 1;
		t = 1;
		while(flag)
		{
			switch(t)
			{
				case 1:
					status =  CardReset(Recv,&len,1);

					if(status == MI_OK)
					{
						t++;
					}
					else
					{
						//IncSerId();
						//SaveCardData(3);   //保存普通灰记录
						flag = 0;
						//Qflag =0;
					}
					break;

				case 2:
					//  memcpy(Send,LanSec.ADFNUM,2);
					//  len = 2;
#if 0					
					if(memcmp(LanSec.ADFNUM,"\x3f\x01",2) == 0)
					{
						memcpy(Send,NameDF01,sizeof(NameDF01));
						len = sizeof(NameDF01);
					}
					else
					{
						memcpy(Send,LanSec.ADFNUM,2);
						len = sizeof(LanSec.ADFNUM);
					}

					len = sizeof(AID_MAIN);
					memcpy(Send,AID_MAIN,len);
#endif
                    memcpy(Send,"\xA0\x00\x00\x00\x03\x86\x98\x07\x01",9);
                    len = 9;
					status = SelectAppDF(Send,Recv,&len);
					if(status == MI_OK)t++;
					else 
					{
					//	IncSerId();
					//	SaveCardData(3);   //保存普通灰记录
						flag = 0;
						Err_display(40);
						Qflag =0;
					}
					break;

				case 3: //判断卡号是否相同
#if 1
					Send[0] = 0x00;
					Send[1] = 0xb0;
					Send[2] = 0x95;  
					Send[3] = 0x00;
					Send[4] = 0x1e;//
#if APP_PRINTF
					printf("ReadBin 0015 data  ");
#endif
					result = write(mf_fd,Send,5);                     
					if (result == MI_OK )
					{
                        ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				        result = read(mf_fd, receive_buf, receive_len[0]);
                     if((receive_len[0] >= 2) &&((receive_buf[receive_len[0]-2] == 0x90) &&(receive_buf[receive_len[0]-1] == 0x00)))
					{

#if APP_PRINTF
                    unsigned char i;
					DBG_PRINTF("get data = %d\n",receive_len[0]);
					for(i =0 ; i < receive_len[0]; i ++)
					{
						DBG_PRINTF("%02X ",receive_buf[i]);
					}
					DBG_PRINTF("\n");
						
					
#endif								
						if (memcmp(&CardLan.Appcsn[0],receive_buf+12,sizeof(CardLan.Appcsn))) 
						{
						//	IncSerId();
						//	SaveCardData(3);   //保存普通灰记录
							flag = 0;	
							Qflag = 0;
							Err_display(63);							
							break;
						}
                       }
					}
					/*verify cash*/      //读取余额
					Send[0] = 0x80;
					Send[1] = 0x5c;
					Send[2] = 0x00;
					Send[3] = 0x02;		//EP
					Send[4] = 0x04;

					result = write(mf_fd,Send,5);
                    if(result==MI_OK)
				    {
                        ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				        result = read(mf_fd, receive_buf, receive_len[0]);
                     if((receive_len[0] >= 2) &&((receive_buf[receive_len[0]-2] == 0x90) &&(receive_buf[receive_len[0]-1] == 0x00)))
					{
						LongUnon OldCardCash;
						LongUnon t1, t3;

						
						MoneyChange(OldCash.i , t1.longbuf);
						t3.i = t1.i - HostValue.i; 
						MoneyChange(t3.i , OldCardCash.longbuf);
#if APP_PRINTF
						printf("HostValue.i = %x\n", HostValue.i);
						printbuf(OldCash.longbuf, 4);
						printbuf(OldCardCash.longbuf, 4);

						printf("SecondDebitChas() %d\n", __LINE__);
						printf("cash = ");
						printbuf(receive_buf, receive_len[0]);
                       
                        
						printf("OldCash.longbuf = ");
						printbuf(OldCash.longbuf, 4);

						printf("HostValue.i = %x\n", HostValue.i);
						printf("OldCardCash.i = %x\n", OldCardCash.i);
#endif								
					if (memcmp(&OldCardCash.longbuf[0], &receive_buf[0], sizeof(OldCardCash.longbuf))) 
					{
						flag = 0;	
						Qflag = 0;
						Err_display(55);
						break;
					}
					else
					{
						t++;
					}
				  }
                }
				else 
				{
				//	IncSerId();
				//	SaveCardData(3);   //保存普通灰记录
					flag = 0;
					Qflag =0;
					Err_display(65);
				}
#endif
				break;
				case 4:
					OldCi.i++;
					memset(Send,0,sizeof(Send));
					Send[0] = 0x80;
					Send[1] = 0x5a; //
					Send[2] = 0x00; //
					Send[3] = 0x06; // 
					Send[4] = 0x02;
					Send[5] = OldCi.longbuf[1];
					Send[6] = OldCi.longbuf[0];
					Send[7] = 0x08;
#if APP_PRINTF
					{
						unsigned char i;
						printf("Get Transaction Prove Send =\n");
						for(i =0 ; i < 8; i ++)
						{
							printf("%02X ",Send[i]);
						}
						printf("\n");
					}
#endif
					result = write(mf_fd,Send,8);
					if(result == MI_OK)
					{
                        ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				        result = read(mf_fd, receive_buf, receive_len[0]);
						if(receive_len[0] >= 2)
						{
							if((receive_buf[receive_len[0]-2] == 0x90)&&(receive_buf[receive_len[0]-1] == 0x00))
							{
								memcpy(Recv,receive_buf,receive_len[0]); 
#if APP_PRINTF
								printf("SecondDebitChas() %d\n", __LINE__);
								printf("receive_buf = ");
								printbuf(receive_buf, receive_len[0]);
#endif								

								memcpy(&Time, &BakTime, sizeof(Time));	//上次扣款成功, 还原上一次交易时间
								if(receive_len[0] > 6)
								{
									memcpy(CardLan.CardTac,Recv+4,4); //TAC
									memcpy(Recv+4,receive_buf,4);
									t++;
								}
								else
								{
									memcpy(CardLan.CardTac,Recv,4); //TAC
									t++;
								}
							}
							else
							{
								flag = 0;
								Qflag =0;
						//		IncSerId();
								Err_display(56);
						//		SaveCardData(3);   //保存普通灰记录
								//灰记录
							}
						}
						else
						{
							flag = 0;
							Qflag =0;
							Err_display(56);
						//	IncSerId();
						//	SaveCardData(3);   //保存普通灰记录
							//灰记录
						}
#if APP_PRINTF
						{
							unsigned char i;

							printf("Get Transaction Prove data = %d\n",receive_len[0]);
							for(i =0 ; i < receive_len[0]; i ++)
							{
								printf("%02X ",receive_buf[i]);
							}
							printf("\n");
						}
#endif
					}
					else 
					{
						flag = 0;
						Qflag =0;
						Err_display(56);
				//		IncSerId();
				//		SaveCardData(3);   //保存普通灰记录
						//灰记录
					}
					break;
				case 5:
					flag = 0;
					t = 0;
					break;

				default:
					flag = 0;
					break;
			}
		}

		if (t == 0)
		{
			StopMusic();
		}

		if((t == 0)||(t==10))
		{
			Qflag = 0;
			break;
		}
	}


#if APP_PRINTF
	printf("SecondDebitChas   End == %d \n",t);
#endif

	LEDL(0);
	beepopen(11);
	SetColor(Mcolor);
	return t;
}


unsigned char SecondDebitChas_complex(unsigned int Money,char *Key,unsigned char Keylen)
{
	
	int result,i;
	unsigned char Timebuf[8];
	char Send[128],Recv[128];
	char Mbuff[4],buff2[8],buff1[8];
	unsigned char flag,t,status,len,ErrNumo,Qflag,ic;

	char InitChas[]= {0x80,0x50,0x03,0x02,0x0b}; //,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x0f;
	// 消费初始化  KEY:0x00  金额:0.01元 终端机号:1号
	char DebitChas[]= {0x80,0x54,0x01,0x00,0x0f}; //,0x00,0x00,0x00,0x01,0x20,0x12,0x09,0x24,0x15,0x26,0x00; //+ MAC1
	// 消费   交易序号: 1 交易时间:20120924152600 + MAC1
	char SamMac1[]= {0x80,0x70,0x00,0x00,0x24};
	char SamMac2[]= {0x80,0x72,0x00,0x00,0x04};
	char update_rec_1703[]= {0x80,0xDC,0x03,0xB8,0x3e/*,0x03,0x2c,0x00*/};
	LongUnon Buf;


#if APP_PRINTF
	printf("SecondDebitCmdChas start  Money == %d \n",Money);
#endif
	LEDL(1);
	beepopen(10);
	SetColor(Mcolor);
	SetTextColor(Color_red);
	SetTextSize(32);
	TextOut(100 , 50, "温馨提示");
	TextOut(100 , 90, "刷卡错误");
	TextOut(85  , 130,"请重新刷卡");
#if 0
#if _F26
	PlayMusic(13);
#else
	PlayMusic(15);
#endif
#endif

	ErrNumo = 0;
	Qflag = 1;
	ic = 0;
	while(Qflag)
	{
		SetTextColor(Color_red);
		SetTextSize(32);
		TextOut(100 , 50, "温馨提示");
		TextOut(100 , 90, "刷卡错误");
		TextOut(85  , 130,"请重新刷卡");

		Rd_time (Timebuf+1);

		if(Time.sec != Timebuf[6] || Time.min != Timebuf[5])


		{
			Timebuf[0] = 0x20;
			Time.year = Timebuf[1];
			Time.month = Timebuf[2];
			Time.day = Timebuf[3];
			Time.hour = Timebuf[4];
			Time.min = Timebuf[5];
			Time.sec = Timebuf[6];

			ic++;
			if(ic>=28)
			{
				Qflag =0;
				break;
			}

			if((ic>3)&&(ic%4 == 0))
			{
				PlayMusic(15,0);
			}

		}

		flag = 1;
		t = 1;
		while(flag)
		{
            
#if APP_PRINTF
			printf("t=%d,%d,%s\n",t,__LINE__,__FUNCTION__);
#endif
		
			switch(t)
			{

				case 1:
			status = CardReset(Recv,&len,1);
					if(status == MI_OK)
					{
						t++;
					}
					else
					{
						flag = 0;
					}
					break;

				case 2:
					//  memcpy(Send,LanSec.ADFNUM,2);
					//  len = 2;
					memcpy(Send,"\xA0\x00\x00\x00\x03\x86\x98\x07\x01",9);
                    len = 9;					

					status = SelectAppDF(Send,Recv,&len);
					if(status == MI_OK) {
                        t++;
					}
					else flag = 0;
					break;

				case 3: //Get Transaction Prove
				t++;
				/*
					OldCi.i++;
					memset(Send,0,sizeof(Send));
					Send[0] = 0x80;
					Send[1] = 0x5a; //
					Send[2] = 0x00; //
					Send[3] = 0x09; // 0018
					Send[4] = 0x02;
					Send[5] = OldCi.longbuf[1];
					Send[6] = OldCi.longbuf[0];
					Send[7] = 0x08;

#if APP_PRINTF
					{
						
						printf("Get Transaction Prove Send =\n");
						for(i =0 ; i < 8; i ++)
						{
							printf("%02X ",Send[i]);
						}
						printf("\n");
					}
#endif


					result = write(mf_fd,Send,8);
					if(result == MI_OK)
					{
						ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
						result = read(mf_fd, receive_buf, receive_len[0]);
						if(receive_len[0] >= 2)
						{
							if((receive_buf[receive_len[0]-2] == 0x90)&&(receive_buf[receive_len[0]-1] == 0x00))
							{
								memcpy(Recv,receive_buf,receive_len[0]); //上次交易终端机号
								if(receive_len[0] > 6)
								{
									memcpy(CardLan.CardTac,Recv+4,4); //TAC
									memcpy(Recv+4,receive_buf,4);
									t+=6;
								}
								else
								{
									memcpy(CardLan.CardTac,Recv,4); //TAC
									t+=7;
								}
							}
							else
							{
								t++;
							}
						}
						else
						{
							flag = 0;
						}

#if APP_PRINTF
						{					

							printf("Get Transaction Prove data = %d\n",receive_len[0]);
							for(i =0 ; i < receive_len[0]; i ++)
							{
								printf("%02X ",receive_buf[i]);
							}
							printf("\n");
						}
#endif
					}
					else flag = 0;
					*/
					break;

				case 4:
                    t++;
                    /*
					status = SecurityCertificate_zhujian();
					if(status == MI_OK)
					{
						t++;
					}
					else
					{
						flag = 0;
					}
					*/
					break;


				case 5:
					memset(Mbuff,0,sizeof(Mbuff));
					memset(Send,0,sizeof(Send));
					MoneyChange(Money,Mbuff);
					memcpy(Send,InitChas,sizeof(InitChas)); //命令头
					Send[5] = Key[0]; //密钥类型
					memcpy(Send+6,Mbuff,4); //消费金额
					memcpy(Send+10,PsamNum,6); //终端机号
					Send[16] = 0x0f; //
					len = 17;
					result = write(mf_fd,Send,len);
					if(result == MI_OK)
					{
						ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
						result = read(mf_fd, receive_buf, receive_len[0]);

						memcpy(Buf.longbuf,CardLan.CAppCash,4);						

							if(receive_len[0] > 2)
							{
								t++;
								memcpy(Recv,receive_buf,receive_len[0]);
							}
							else if((receive_buf[0] == 0x94)&&(receive_buf[1] == 0x01))
							{
								// 余额不足
								Err_display(12);
								flag = 0;
							}
							else
							{
								Err_display(6);
								flag = 0;
							}

#if APP_PRINTF
						{
							
							printf("InitChas get data = %d\n",receive_len[0]);
							for(i =0 ; i < receive_len[0]; i ++)
							{
								printf("%02X",receive_buf[i]);
							}
							printf("\n");
						}
#endif
					}
					else
					{
						Err_display(6);
						flag = 0;
					}
					break;

				case 6:
					memset(Send,0,sizeof(Send));
					memcpy(Send,SamMac1,5);
					memcpy(Send+5,Recv+11,4); //伪随机数
					memcpy(Send+9,Recv+4,2); //伪随机数
					memcpy(Send+11,Mbuff,4); //伪随机数
					Send[15] = 0x09;
					memcpy(Send+16,Timebuf,7);
					memcpy(Send+23,Recv+9,2);
					memcpy(Send+25,CardLan.Appcsn,8);
					memcpy(Send+33,CardLan.CityId,2);
					Send[35] = 0xff;
					len = 41;
					memset(Recv,0,sizeof(Recv));
					status = PsamCos(Send,Recv,&len);
					if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
					{
						t ++;
						memset(buff2,0,sizeof(buff2));
						memset(Buf.longbuf,0,4);
						memcpy(Buf.longbuf,Recv,4);
						MoneyChange(Buf.i,buff2);
						memcpy(DevSIDCPU.longbuf,buff2,4);
					}
					else
					{
						Err_display(60);
						flag = 0;
					}
					break;

				case 7:
                    t++;
                    /*
					memset(Send,0,sizeof(Send));
					memcpy(Send,update_rec_1703,sizeof(update_rec_1703));
                    Send[5] = 0x03;                        //复合记录标识
                    Send[6] = 0x3e;                        // 记录长度
                    Send[7] = 0x00;                        // 应用锁定标识
    				Send[8] = CardLan.EnterExitFlag;       //进出标志
    				memcpy(Send+9,CardLan.CityId,2);      //城市代码
                    memcpy(Send+11,Timebuf,7);             //交易时间 
                    //18-19 线路号 20-22 车辆号 23-26 终端号 
                    memset(buff1,0,sizeof(buff1));
    				sprintf(buff1,"%08d",DevNum.i);
    				ascii_2_hex(buff1, CardLan.CAppTermNo+2, 8);//下车终端编号
    				memcpy(Send+23,CardLan.CAppTermNo+2,4);
                    if(Section.Updown==0)
                        Send[27] = 0;
                    else
                        Send[27] = 1;
                    Send[28] = Section.SationNow;
                    Send[29] = CardLan.CAppCash[2];
                    Send[30] = CardLan.CAppCash[3];
    				memcpy(Send+31,PsamNum,6);             //PSAM卡号				
				
				    len = 67;
                    
#if APP_PRINTF
				{      
   					printf("update 1703(%d)",__LINE__);
					for(i =0 ; i < 67; i ++)
					{
						printf("%02X ",Send[i]);
					}
					printf("\n");
				}
#endif				
                if(result == MI_OK)
					{
						ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
						result = read(mf_fd, receive_buf, receive_len[0]);
						if(receive_len[0] >= 2)
						{
							if((receive_buf[receive_len[0]-2] == 0x90)&&(receive_buf[receive_len[0]-1] == 0x00))
							{
								t++;
								memcpy(Recv,receive_buf,receive_len[0]);
							}
							else
							{
								Err_display(56);
								flag = 0;
							}
						}
						else
						{
							Err_display(56);
							flag = 0;
						}
#if APP_PRINTF
				    printf("Updata 1703 get data = %d\n",receive_len[0]);
				   for(i =0 ; i < receive_len[0]; i ++)
					{
						printf("%02X ",receive_buf[i]);
					}
					printf("\n");
#endif
                        
					}
					else
					{
						Err_display(56);
						flag = 0;
					}
					*/
					break;

				case 8:
					memset(Send,0,sizeof(Send));
					memcpy(Send,DebitChas,sizeof(DebitChas));
					memcpy(Send+5,Recv,4);//终端交易序号
					memcpy(Send+9,Timebuf,7); //时间
					memcpy(Send+16,Recv+4,4); //MAC1
					Send[20]= 0x08;          //
					result = write(mf_fd,Send,21);
					if(result == MI_OK)
					{
						ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
						read(mf_fd, receive_buf, receive_len[0]);
						if(receive_len[0] >= 2)
						{
							if((receive_buf[receive_len[0]-2] == 0x90)&&(receive_buf[receive_len[0]-1] == 0x00))
							{
								t++;
								memcpy(Recv,receive_buf,receive_len[0]);
								memcpy(CardLan.CardTac,Recv,4); //TAC
							}
							else if((receive_buf[receive_len[0]-2] == 0x93)&&(receive_buf[receive_len[0]-1] == 0x02))
							{
								Err_display(23);
								flag = 0;
							}
							else
							{
								ErrNumo++;
								if(ErrNumo>3)
								{
									t = 20;
									Err_display(23);
								}
								flag = 0;
							}
						}
						else
						{
							flag = 0;
						}
#if APP_PRINTF
				    printf("DebitChas get data = %d\n",receive_len[0]);
					for(i =0 ; i < receive_len[0]; i ++)
					{
						printf("%02X ",receive_buf[i]);
					}
					printf("\n");
#endif      
                    }
					else
					{
						flag = 0;
					}
					break;

				case 9:
					memset(Send,0,sizeof(Send));
					memcpy(Send,SamMac2,5);
					memcpy(Send+5,Recv+4,4); //伪随机数
					len = 9;
					memset(Recv,0,sizeof(Recv));
					status = PsamCos(Send,Recv,&len);
					if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
					{
						t ++;
					}
					else
					{
						Err_display(61);
						flag = 0;
					}
					break;

				case 10:
					t = 0;
					flag =0 ;
					break;

				default :
					flag =0 ;
					break;

			}
		}

		if (t == 0)
		{
			StopMusic();
		}
		
		if((t == 0)||(t==20))
		{
			Qflag = 0;
			break;
		}
	}

#if APP_PRINTF
	printf("SecondDebitCmdChas   End == %d \n",t);
#endif

	return t;
}



/*
*********************************************************************
函数:unsigned char Card_DebitChas(unsigned int Money,char *Key,unsigned char Keylen,char *UserPIN,unsigned char pinlen)
功能:
输入:
		 Money 消费金额
		 Key  消费密钥 Key[0] 密钥类型	Key1 -- Keyn 为密钥
		 Keylen  密钥长度
		 UserPIN 用户PIN口令
		 pinlen  口令长度

输出:
	status  0 成功  1 失败
*********************************************************************
*/


unsigned char Card_DebitChas_complex(unsigned int Money,char *Key,unsigned char Keylen)
{
	int result;
	unsigned char Timebuf[8];
	char Send[128],Recv[128];
	char Mbuff[4],buff2[8],buff1[8];
	unsigned char flag,t,status,len;
    unsigned char i;
    
	char InitChas[]= {0x80,0x50,0x03,0x02,0x0b}; //,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x0f;
	// 消费初始化  KEY:0x00  金额:0.01元 终端机号:1号
	char DebitChas[]= {0x80,0x54,0x01,0x00,0x0f}; //,0x00,0x00,0x00,0x01,0x20,0x12,0x09,0x24,0x15,0x26,0x00; //+ MAC1
	// 消费   交易序号: 1 交易时间:20120924152600 + MAC1
	char SamMac1[]= {0x80,0x70,0x00,0x00,0x24};
	char SamMac2[]= {0x80,0x72,0x00,0x00,0x04};
	char update_rec_1703[]= {0x80,0xDC,0x03,0xB8,0x3e/*,0x03,0x2c,0x00*/};
	LongUnon Buf;



	Timebuf[0] = 0x20;
	Timebuf[1] = Time.year;
	Timebuf[2] = Time.month;
	Timebuf[3] = Time.day;
	Timebuf[4] = Time.hour;
	Timebuf[5] = Time.min;
	Timebuf[6] = Time.sec;

	flag = 1;
	t = 1;
	while(flag)
	{
		switch(t)
		{
			case 1:
				memset(Mbuff,0,sizeof(Mbuff));
				memset(Send,0,sizeof(Send));
				MoneyChange(Money,Mbuff);
				memcpy(Send,InitChas,sizeof(InitChas)); //命令头
				Send[5] = Key[0]; //密钥类型
				memcpy(Send+6,Mbuff,4); //消费金额
				memcpy(Send+10,PsamNum,6); //终端机号
				Send[16] = 0x0f; //
				len = 17;
				result = write(mf_fd,Send,len);
#if APP_PRINTF
				{
					
					printf("80-50(%d)",__LINE__);
					for(i =0 ; i < 35; i ++)
					{
						printf("%02X ",Send[i]);
					}
					printf("\n");
				}
#endif
				if(result == MI_OK)
				{
					ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
					result = read(mf_fd, receive_buf, receive_len[0]);
					if(receive_len[0] > 2)
					{
						t++;
						OldCi.i = 0;
						OldCash.i = 0;
						memcpy(Recv,receive_buf,receive_len[0]);
						memcpy(OldCash.longbuf,Recv,4);
						MoneyChange(OldCash.i,CardLan.QCash);
						memset(CardLan.ViewMoney,0,sizeof(CardLan.ViewMoney));
						CardLan.ViewMoney[0] = Recv[5];
						CardLan.ViewMoney[1] = Recv[4];
						OldCi.longbuf[0] = Recv[5];
						OldCi.longbuf[1] = Recv[4];
					}
					else if((receive_buf[0] == 0x94)&&(receive_buf[1] == 0x01))
					{
						Err_display(12);
						flag = 0;
					}
					else
					{
						Err_display(6);
						flag = 0;
					}

#if APP_PRINTF
				{
					
					printf("InitChas get data = %d\n",receive_len[0]);
					for(i =0 ; i < receive_len[0]; i ++)
					{
						printf("%02X ",receive_buf[i]);
					}
					printf("\n");
				}
#endif

				}
				else
				{
					Err_display(6);
					flag = 0;
				}
				break;

			case 2:
				memset(Send,0,sizeof(Send));
				memcpy(Send,SamMac1,5);
				memcpy(Send+5,Recv+11,4); //伪随机数
				memcpy(Send+9,Recv+4,2); //伪随机数
				memcpy(Send+11,Mbuff,4); //伪随机数
				Send[15] = 0x09;
				memcpy(Send+16,Timebuf,7);
				memcpy(Send+23,Recv+9,2);
				memcpy(Send+25,CardLan.Appcsn,8);
				memcpy(Send+33,CardLan.CityId,2);
				Send[35] = 0xff;
				len = 41;
				memset(Recv,0,sizeof(Recv));
				status = PsamCos(Send,Recv,&len);
				if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
				{
					t ++;
					memset(buff2,0,sizeof(buff2));
					memset(Buf.longbuf,0,4);
					memcpy(Buf.longbuf,Recv,4);
					MoneyChange(Buf.i,buff2);
					memcpy(DevSIDCPU.longbuf,buff2,4);
				}
				else
				{
					Err_display(60);
					flag = 0;
				}
				break;

			case 3:
				memset(Send,0,sizeof(Send));
				memcpy(Send,update_rec_1703,sizeof(update_rec_1703));
                Send[5] = 0x03;                        //复合记录标识
                Send[6] = 0x3e;                        // 记录长度
                Send[7] = 0x00;                        // 应用锁定标识
				Send[8] = CardLan.EnterExitFlag;       //进出标志
				memcpy(Send+9,CardLan.CityId,2);      //城市代码
                memcpy(Send+11,Timebuf,7);             //交易时间 
                //18-19 线路号 20-22 车辆号 23-26 终端号 
                memset(buff1,0,sizeof(buff1));
				sprintf(buff1,"%08d",DevNum.i);
				ascii_2_hex(buff1, CardLan.CAppTermNo+2, 8);//下车终端编号
				memcpy(Send+23,CardLan.CAppTermNo+2,4);
                if(Section.Updown==0)
                    Send[27] = 0;
                else
                    Send[27] = 1;
                Send[28] = Section.SationNow;
                Send[29] = CardLan.CAppCash[2];
                Send[30] = CardLan.CAppCash[3];
				memcpy(Send+31,PsamNum,6);             //PSAM卡号				
				
				len = 67;
				result = write(mf_fd,Send,len);
#if APP_PRINTF
				{
                    printf("预扣金额:");
                    for(i =0 ; i < 4; i ++)
					{
						printf("%02X ",CardLan.CAppCash[i]);
					}
					printf("\n");                    
                    
					
					printf("update 1703(%d)",__LINE__);
					for(i =0 ; i < 67; i ++)
					{
						printf("%02X ",Send[i]);
					}
					printf("\n");
				}
#endif
				if(result == MI_OK)
				{
					ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
					result = read(mf_fd, receive_buf, receive_len[0]);
					if(receive_len[0] >= 2)
					{
						if((receive_buf[receive_len[0]-2] == 0x90)&&(receive_buf[receive_len[0]-1] == 0x00))
						{
							t++;
						}
						else
						{
							Err_display(56);
							flag = 0;
						}
					}
					else
					{
						Err_display(56);
						flag = 0;
					}
#if APP_PRINTF
				printf("Updata 1703 get data = %d\n",receive_len[0]);
					for(i =0 ; i < receive_len[0]; i ++)
					{
						printf("%02X ",receive_buf[i]);
					}
					printf("\n");
#endif

				}
				else
				{
					Err_display(56);
					flag = 0;
				}
				break;

			case 4:
				memset(Send,0,sizeof(Send));
				memcpy(Send,DebitChas,sizeof(DebitChas));
				memcpy(Send+5,Recv,4);//终端交易序号
				memcpy(Send+9,Timebuf,7); //时间
				memcpy(Send+16,Recv+4,4); //MAC1
				Send[20]= 0x08;          //
				result = write(mf_fd,Send,21);
				if(result == MI_OK)
				{
					ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
					read(mf_fd, receive_buf, receive_len[0]);
					if(receive_len[0] >= 2)
					{
						if((receive_buf[receive_len[0]-2] == 0x90)&&(receive_buf[receive_len[0]-1] == 0x00))
						{
							t++;
							memcpy(Recv,receive_buf,receive_len[0]);
							memcpy(CardLan.CardTac,Recv,4); //TAC
						}
						else if((receive_buf[receive_len[0]-2] == 0x93)&&(receive_buf[receive_len[0]-1] == 0x02))
						{
							Err_display(23);
							flag = 0;
						}
						else
						{
							printf("MRecv f3No:%02X%02X",receive_buf[receive_len[0]-2],receive_buf[receive_len[0]-1]);
							Err_display(6);
							flag = 0;	
						}
#if APP_PRINTF
				    printf("DebitChas get data = %d\n",receive_len[0]);
					for(i =0 ; i < receive_len[0]; i ++)
					{
						printf("%02X ",receive_buf[i]);
					}
					printf("\n");
#endif                        
					}
					else
					{
						printf("MRecv f3No:0000");
						status = SecondDebitChas_complex(Money,Key,Keylen);
						if(status == MI_OK)
						{
							t++;
						}
						else
						{
							flag = 0;
						}
					}
				}
				else
				{
					status = SecondDebitChas_complex(Money,Key,Keylen);
					if(status == MI_OK)
					{
						t++;
					}
					else
					{
						flag = 0;
					}
				}
				break;

			case 5:
				t = 0;
				flag =0 ;
				break;

			default :
				flag =0 ;
				break;

		}
	}


#if APP_PRINTF
	printf("Card_DebitComChas   End == %d \n",t);
#endif

	return t;
}


unsigned char TopUpCardInfor_CPU(int type)
{
	unsigned char status = 1;
	unsigned char keybuff[20];
	keybuff[0] = 0x01;
    DBG_PRINTF("TopUpCardInfor_CPU() is called.\n");
#if KEYTEST

	memcpy(keybuff+1,MmasterKey,16);

#else
	CardKey(keybuff+1);

#endif

//   printf("CardKey: \n");


	if(type == 0) {
		//status  = Card_DebitChas(HostValue.i,keybuff,16);
		//因为引入补贴钱包(0x17文件)，所以定额消费也按复合消费处理
		CardLan.StationDEC = 0x55;
		CardLan.EnterExitFlag = 0;
		status  = Card_DebitChas_complex(HostValue.i,keybuff,16);
	}
	else if(type == 1) {
		status  = Card_DebitChas_complex(HostValue.i,keybuff,16);
	}
	else if(type == 2)
		status  = Card_DebitChas(HostValue.i,keybuff,16);
	else {
		printf("impossible at TopUpCardInfor_CPU\n");
	}
	return status;
}


unsigned char ReadCardInfor_CPU(void)
{
	int result;
	char Recv[256];
	char binname[20];
	unsigned char flag,t,status,len;
	unsigned char QuerrChas[]= {0x80,0x5c,0x00,0x02,0x04};
	unsigned char verify_pin[]= {0x00,0x20,0x00,0x00,0x02,0x12,0x34};
    unsigned char PPSE[]={"2PAY.SYS.DDF01"};
	LongUnon Value;   
    char tempbuf[8],tempbuf1[8];   
    char data[50],plen;
    unsigned char i;    
    char typesa[5];
    char read_rec_1703[]= {0x00,0xB2,0x03,0xB8,0x3e};
	DBG_PRINTF("ReadCardInfor_CPU() is called.\n");

	flag = 1;
    t = 0;
	while(flag)
	{
        DBG_PRINTF("use_jiaotong_stander=%02x\n",use_jiaotong_stander);
        DBG_PRINTF("ReadCardInfor_CPU	 End == %d \n",t);
		switch(t)
		{
        case 0:
        #if Transport_Stander            
                len = strlen(PPSE);
                status = SelectAppDF(PPSE,Recv,&len);  		
                status = SelectAID(Recv, &len, binname, &plen);
        #else
                memcpy(binname,"\xA0\x00\x00\x00\x03\x86\x98\x07\x01",9);
                len = 9;
                status = SelectAppDF(binname,Recv,&len); 
		#endif
                DBG_PRINTF("status = %d:line=%d\n",status,__LINE__);     
                if(status == MI_OK)
                {                    
                        len = plen; 
    					memcpy(PsamNum,PsamNum_bak2,sizeof(PsamNum_bak2));					
    					DBG_PRINTF("使用住建部cpu卡流程00:line=%d\n",__LINE__);
                        use_jiaotong_stander=0x55;
                        t+=2;
                    }
                else
                {
                    /*
                    memcpy(binname,"\xA0\x00\x00\x06\x32\x01\x01\x05",8);
					len=8;
                    memcpy(PsamNum,PsamNum_bak2,sizeof(PsamNum_bak2));					
					DBG_PRINTF("使用交通部cpu卡流程00:line=%d\n",__LINE__);
                    use_jiaotong_stander=0x55;
                    t++;
                    */
                    flag = 0;
                }
            


            break;
            
    	case 1:            			
 			
#if Transport_Stander	
          
			if((binname[0]==0x3f)&&(binname[1]==0x01))	//先选择0x3f 0x01应用,不成功再选择交通部应用
			{
				status=SelectAppDF(binname,Recv,&len);
				if(status!=MI_OK){
					memcpy(binname,"\xA0\x00\x00\x06\x32\x01\x01\x05",8);
					len=8;
					status=SelectAppDF(binname,Recv,&len);
					memcpy(PsamNum,PsamNum_bak2,sizeof(PsamNum_bak2));					
					DBG_PRINTF("使用交通部cpu卡流程01:line=%d\n",__LINE__);
				}
				else{
					memcpy(PsamNum,PsamNum_bak1,sizeof(PsamNum));					
					DBG_PRINTF("使用卡联cpu卡流程01:line=%d\n",__LINE__);		
				}
			}
			else{                  
					status=SelectAppDF(binname,Recv,&len);
                    			if(status != MI_OK)
				                    {
				                        len = 2;
				                        binname[0] = 0x3f;
				                        binname[1] = 0x01;
				                        status=SelectAppDF(binname,Recv,&len);
				                        memcpy(PsamNum,PsamNum_bak1,sizeof(PsamNum));
				                        DBG_PRINTF("使用卡联cpu卡流程02:line=%d\n",__LINE__);
				                        }
                    			else 
			                        {
			                            if((receive_buf[len-2]==0x90)&&(receive_buf[len-1]==0x00))
			                           {
			                            memcpy(PsamNum,PsamNum_bak2,sizeof(PsamNum_bak2));					
								        DBG_PRINTF("使用交通部流程02:line=%d\n",__LINE__);
			                            }
		                            else if((receive_buf[len-2]==0x62)&&(receive_buf[len-1]==0x83))
		                                {
		                                    DBG_PRINTF("出错6283\n");
		                                     use_jiaotong_stander=0x0;
		                                     Err_display(47);
		        				            flag=0; 
		                                    break;
		                                    }
		                            else if((receive_buf[len-2]==0x6A)&&(receive_buf[len-1]==0x82))
		                            {
		                                    DBG_PRINTF("出错6A82\n");
		                                     use_jiaotong_stander=0x0;
		                                    Err_display(52);
		        				            flag=0;
		                                    break;
		                                }
                               }            
                    
			}
#else		
			status = SelectAppDF(binname,Recv,&len);
#endif

            memset(tempbuf,0,8);           
            memcpy(tempbuf,&receive_buf[4],8);
			if((status==MI_OK)&&((binname[0]==0x3f)&&(binname[1]==0x01))&& 
                ((receive_buf[3]==0x03)||(receive_buf[3]==0x09)))
			{
				t++;
			}
		    else if((status==MI_OK)&&((mystrncmp(binname,"\xA0\x00\x00\x06\x32\x01\x01\x05",8)==0)||
                (mystrncmp(tempbuf,"\xA0\x00\x00\x06\x32\x01\x01\x05",8)==0)))
            
            {
				//交通部规范
				memcpy(PsamNum,PsamNum_bak2,sizeof(PsamNum_bak2));
				t=10;
				use_jiaotong_stander=0x55;
			}
            if(status!=MI_OK)
            {
                Err_display(30);
				flag=0;
				}

			break;	



		case 2: // 读 00015 卡片基本信息文件
			memset(binname,0,sizeof(binname));
			binname[0] = 0x00;
			binname[1] = 0xb0;
			binname[2] = 0x95;  // 00015
			binname[3] = 0x00;
			binname[4] = 0x1e;//
			result = write(mf_fd,binname,5);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);                
				if(receive_len[0] > 2)
				{
					t++;
					CardLan.Appstate = receive_buf[8];
                    memcpy(CardLan.CityId,receive_buf+2,2);         //城市代码
                    memcpy(CardLan.Appfalw,receive_buf+10,2);  //互通标识
                   // memcpy(CardLan.Appcsn,receive_buf+10,8);  //应用序列号
                    memcpy(CardLan.Appcsn,receive_buf+12,8);  //应用序列号
                    memcpy(CardLanCPU.appserialnumber+2,CardLan.Appcsn,8);
                    memcpy(CardLan.CardCsnB,CardLan.Appcsn+4,4);
                    memcpy(CardLan.EnableH,receive_buf+20,4); //启用
					memcpy(CardLan.Effective,receive_buf+24,4); //有效
					
				}
				else
				{
					flag = 0;
				}
				{
					

					DBG_PRINTF("ReadBin 0015 data = %d\n",receive_len[0]);
					for(i =0 ; i < receive_len[0]; i ++)
					{
						DBG_PRINTF("%02X ",receive_buf[i]);
					}
					DBG_PRINTF("\n");
				}

			}
			else flag = 0;
			break;

     

        case 3:
            t++;
            /*
    		memset(binname,0,sizeof(binname));
			binname[0] = 0x00;
			binname[1] = 0xb0;
			binname[2] = 0x96;  // 00016
			binname[3] = 0x00;
			binname[4] = 0x02;
			result = write(mf_fd,binname,5);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
					CardLan.CardType = receive_buf[16]; //卡类		
					sprintf(typesa,"%02x",CardLan.CardType);
					CardLan.CardType = atoi(typesa);
                    CardLan.CardType = 1;
                    //t++;
                    t+=2;
				}
				else
				{
					flag = 0;
				}

#if 1
				{
					DBG_PRINTF("ReadBin 0005 data = %d\n",receive_len[0]);
					for(i =0 ; i < receive_len[0]; i ++)
					{
						DBG_PRINTF("%02X ",receive_buf[i]);
					}
					DBG_PRINTF("\n");
				}
#endif

			}
			else flag = 0;
        */
        break;
            
        
		case 4: // 读 00018 交易记录文件 最后一条
            t++;
            #if 0
            memset(binname,0,sizeof(binname));
			binname[0] = 0x00;
			binname[1] = 0xb2; //
			binname[2] = 0x01; //
			binname[3] = 0xc4; // 0018
			binname[4] = 0x00;
			result = write(mf_fd,binname,5);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
					t++;
					CardLan.OldTransType = receive_buf[9]; //上次交易类型
					memcpy(CardLan.OldTermNo,receive_buf+12,4); //上次交易终端机号
					memcpy(CardLan.OldTime,receive_buf+17,6); //日期时间
				}
				else
				{
					flag = 0;
				}				
					

				DBG_PRINTF("Readcord 0018 data = %d\n",receive_len[0]);
				for(i =0 ; i < receive_len[0]; i ++)
				{
					DBG_PRINTF("%02X ",receive_buf[i]);
				}
				DBG_PRINTF("\n");
				

			}
			else flag = 0;
            #endif
			break;


			case 5:
				memset(binname,0,sizeof(binname));
				memcpy(binname,read_rec_1703,sizeof(read_rec_1703));
				len = sizeof(read_rec_1703);
				result = write(mf_fd,binname,len);
				if(result == MI_OK)
				{
                    ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				    result = read(mf_fd, receive_buf, receive_len[0]);
					if(receive_len[0] > 2)
					{
						t++;
						CardLan.CAppstate = receive_buf[2];
						CardLan.EnterExitFlag = receive_buf[3];
                        memcpy(CardLan.CAppTime,receive_buf+7,6);
						memcpy(CardLan.CAppTermNo+2,receive_buf+18,4);
                        memset(CardLan.CAppCash,0,4);
						memcpy(CardLan.CAppCash+2,receive_buf+24,2);						
						CardLan.StationID = receive_buf[23];
						CardLan.EnterCarCi = 0;
					}
					else if((receive_buf[0] == 0x67)&&(receive_buf[1] == 0x00))
					{
						Err_display(54);
						flag = 0;
					}
					else
					{
						Err_display(53);
						flag = 0;
					}
				DBG_PRINTF("Readcord 1703 data = %d\n",receive_len[0]);
					for(i =0 ; i < receive_len[0]; i ++)
					{
						DBG_PRINTF("%02X ",receive_buf[i]);
					}
					DBG_PRINTF("\n");
				}
				else
				{
					Err_display(53);
					flag = 0;
				}
				break;

            case 6:// 查询余额
				//printf("查询余额\n");
				result = write(mf_fd,"\x80\x5c\x00\x02\x04",5);
				if(result == MI_OK)
				{
					ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
					result = read(mf_fd, receive_buf, receive_len[0]);
					if(receive_len[0] > 2)
					{
						t++;
                        Value.i = 0;
						memcpy(Value.longbuf,receive_buf,4);
                        MoneyChange(Value.i, CardLan.QCash);
                        Valueq.i = receive_buf[0]<<24|receive_buf[1]<<16|receive_buf[2]<<8|receive_buf[3];
						printf("\n交易前余额:%d分\n",receive_buf[0]<<24|receive_buf[1]<<16|receive_buf[2]<<8|receive_buf[3]);
					}
					else
					{
						printf("查询余额 SW1=%02X  SW2=%02X\n",receive_buf[0],receive_buf[1]);
						flag = 0;
					}
				}
				else {
					flag = 0;
					use_jiaotong_stander=0;
					}
				break;                

		case 7:
			flag = 0;
			t  = 0;
			break;
			

#if Transport_Stander

		case 10:			// 读 0015 卡片基本信息文件
			printf("开始读交通部cpu卡  step 10 \n");

			memset(binname,0,sizeof(binname));
			binname[0] = 0x00;
			binname[1] = 0xb0;
			binname[2] = 0x95;  // 00015
			binname[3] = 0x00;
			binname[4] = 0x00;
			result = write(mf_fd,binname,5);
			//printf("在读0015文件没死掉\n");
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
				DBG_DATA_PRINTF("0015文件返回:");
					menu_print(receive_buf, 30);
					t++;
					memcpy(CardLanCPU.issuerlabel,receive_buf,8);	//发卡机构
					CardLanCPU.apptypelabel = receive_buf[8];		//应用类型
					CardLanCPU.issuerappversion = receive_buf[9];
					memcpy(CardLanCPU.appserialnumber,&receive_buf[10],10);//应用序列号
					memcpy(CardLanCPU.appstarttime,&receive_buf[20],4);
					memcpy(CardLanCPU.appendtime,&receive_buf[24],4);
				}
				else
				{
					printf("0015 SW1=%02X  SW2=%02X\n",receive_buf[0],receive_buf[1]);
					Err_display(41);
					use_jiaotong_stander=0;
					flag = 0;
				}
			}
			else 
				{
					Err_display(41);
					use_jiaotong_stander=0;
					flag = 0;
			 }
			break;
		case 11:		
			//检测是否是白名单
			//在白名单文件中查询发卡机标识				
			//printf("判断白名单\n");
#if 1			//交通部测试暂时关掉
            status  = Card_White_Cpu_jiaotong();
			if(status == MI_OK)t++;
			else
			{
				Err_display(65);
				use_jiaotong_stander=0;
				flag = 0;
			}
#else
            t++;
#endif
			break;
		case 12:
			//判断发卡机构应用版本
			if(CardLanCPU.issuerappversion == 0x0)
			
			{
				Err_display(45);
				use_jiaotong_stander=0;
				flag = 0;
			}
			else
				t++;
			break;

        case 13:
            if(Check_CardDate()!=0)
            {
                Err_display(48);
                use_jiaotong_stander=0;
				flag = 0;                   
                }
            else
                t++;
            break;
            
		case 14:
			//判断卡片启用日期和截止日期
			//printf("判断日期\n");    
			status = Card_JudgeDate_jiaotong();
			if(status == 2)
				{
					Err_display(1);
					use_jiaotong_stander=0;
					flag = 0;
				}
            else if(status == 1)
            {
                Err_display(49);
				use_jiaotong_stander=0;
				flag = 0;
                }
			else
				t++;
			break;
			
		case 15: // 读 00017 管理信息文件
			//printf("读00017文件\n");
			memset(binname,0,sizeof(binname));
			binname[0] = 0x00;
			binname[1] = 0xb0;
			binname[2] = 0x97;  // 00017
			binname[3] = 0x00;
			binname[4] = 0x00;
			result = write(mf_fd,binname,5);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
				DBG_DATA_PRINTF("0017文件返回:");
					menu_print(receive_buf, 15);	
					t ++;
					memcpy(CardLanCPU.countrycode,receive_buf,4);	
					memcpy(CardLanCPU.provincecode,&receive_buf[4],2);
					memcpy(CardLanCPU.citycode,&receive_buf[6],2);
					memcpy(CardLanCPU.unioncardtype,&receive_buf[8],2);
					CardLanCPU.cardtype = receive_buf[10];
					memcpy(CardLanCPU.settlenumber,&receive_buf[11],4);
				}
				else
				{
					printf("0017 SW1=%02X  SW2=%02X\n",receive_buf[0],receive_buf[1]);
					Err_display(42);
					use_jiaotong_stander=0;
					flag = 0;
				}
			}
			else 
				{
				Err_display(42);
				use_jiaotong_stander=0;
				flag = 0;
				}
			break;
			
		case 16:
			//卡片互联互通判断
			//printf("读互联互通\n");
			
		if(memcmp(CardLanCPU.citycode,Yanzhou_Card.CityCode,2)==0)		//这里很个性
		{
			CardLanCPU.IsLocalCard = 1;
			t++;
		}
		else

		{
#if 0


			status = CheckCityUnion();
			if(status == MI_OK)
			{
				if(memcmp(CardLanCPU.unioncardtype,"\x00\x01",2)==0)	//这里需定制
				{
					CardLanCPU.IsLocalCard = 2;				//这里需定制
					t ++;
				}
				else
				{
					Err_display(65);
					use_jiaotong_stander=0;
					flag = 0;
				}
			}
			else 
			{
				Err_display(29);
				use_jiaotong_stander=0;
				flag = 0;
			}
#else
        if(memcmp(CardLanCPU.unioncardtype,"\x00\x01",2)==0)	//这里需定制
				{
					CardLanCPU.IsLocalCard = 2;				//这里需定制
					t ++;
				}
				else
				{
					Err_display(65);
					use_jiaotong_stander=0;
					flag = 0;
				}
#endif            
		}

		break;
		case 17://判断所支持的卡类
			//printf("判断支持卡类\n");
			status  = SupportType_Cpu_jiaotong();
			if(status == MI_OK)
				t++;
			else
			{
				Err_display(11);
				use_jiaotong_stander=0;
				flag = 0;
			}
			break;
		case 18:
			//判断卡片是否是黑名单卡
			//printf("判断黑名单\n");
			status  = Card_JudgeCsn_Cpu_jiaotong();
			if(status == MI_OK)t++;
			else
			{
                //是黑名单,则临时锁定应用
                    GET_MAC();
				Err_display(10);
				use_jiaotong_stander=0;
				flag = 0;
			}
			break;
		case 19:
			 // 读 00018 交易记录文件 最后一条
			//printf("读00018文件\n");
			memset(binname,0,sizeof(binname));
			binname[0] = 0x00;
			binname[1] = 0xb2; 
			binname[2] = 0x01; 
			binname[3] = 0xC4; // 0018
			binname[4] = 0x00;
			result = write(mf_fd,binname,5);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
				    DBG_DATA_PRINTF("0018记录文件返回:");
					menu_print(receive_buf, 23);	
					t++;
					memcpy(CardLanCPU.tradenumber,receive_buf,2);
					memcpy(CardLanCPU.overdraftlimit,&receive_buf[2],3);
					memcpy(CardLanCPU.trademoney,&receive_buf[5],4);
					CardLanCPU.tradetype = receive_buf[9];
					memcpy(CardLanCPU.deviceNO,&receive_buf[10],6);
					memcpy(CardLanCPU.tradedate,&receive_buf[16],4);
					memcpy(CardLanCPU.tradetime,&receive_buf[20],3);
				}
				else
				{
					printf("0018 SW1=%02X  SW2=%02X\n",receive_buf[0],receive_buf[1]);
					Err_display(43);
					use_jiaotong_stander=0;
					flag = 0;
				}
			}
			else 
				{
					Err_display(43);
					use_jiaotong_stander=0;
					flag = 0;
				}
			break;
		case 20:
			 // 读 0001A 公共汽电车应用信息记录
			//printf("读0001A文件\n");
		//	if(Section.Enable!=0x55)            //一票制不读1A文件
        //        t++;
        //    else
            {
    			memset(binname,0,sizeof(binname));
    			binname[0] = 0x00;
    			binname[1] = 0xb2; 
    			binname[2] = 0x02; 
    			binname[3] = 0xd4; // 001A
    			binname[4] = 0x00;
    			result = write(mf_fd,binname,5);
    			if(result == MI_OK)
    			{
    				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
    				result = read(mf_fd, receive_buf, receive_len[0]);
    				if(receive_len[0] > 2)
    				{
    				    DBG_DATA_PRINTF("001A记录文件返回:");
    					menu_print(receive_buf, receive_len[0]);	
    					t++;
    					CardLanCPU.applockflag = receive_buf[5];
    					memcpy(CardLanCPU.tradeserialnumber,&receive_buf[6],8);
    					CardLanCPU.tradestate = receive_buf[14];
    					memcpy(CardLanCPU.getoncitycode,&receive_buf[15],2);
    					memcpy(CardLanCPU.getonissuerlabel,&receive_buf[17],8);
    					memcpy(CardLanCPU.getonoperatorcode,&receive_buf[25],2);
    					memcpy(CardLanCPU.getonline,&receive_buf[27],2);
    					CardLanCPU.getonstation = receive_buf[29];
    					memcpy(CardLanCPU.getonbus,&receive_buf[30],8);
    					memcpy(CardLanCPU.getondevice,&receive_buf[38],8);
    					memcpy(CardLanCPU.getontime,&receive_buf[46],7);
    					memcpy(CardLanCPU.markamount,&receive_buf[53],4);
    					CardLanCPU.directionflag = receive_buf[57];
    					memcpy(CardLanCPU.getoffcitycode,&receive_buf[58],2);
    					memcpy(CardLanCPU.getoffissuerlabel,&receive_buf[60],8);
    					memcpy(CardLanCPU.getoffoperatorcode,&receive_buf[68],2);
    					memcpy(CardLanCPU.getoffline,&receive_buf[70],2);
    					CardLanCPU.getoffstation = receive_buf[72];
    					memcpy(CardLanCPU.getoffbus,&receive_buf[73],8);
    					memcpy(CardLanCPU.getoffdevice,&receive_buf[81],8);
    					memcpy(CardLanCPU.getofftime,&receive_buf[89],7);
    					memcpy(CardLanCPU.tradeamount,&receive_buf[96],4);
    					memcpy(CardLanCPU.ridedistance,&receive_buf[100],2);
    				}
    				else
    				{
    					printf("001E SW1=%02X  SW2=%02X\n",receive_buf[0],receive_buf[1]);
    					Err_display(44);
    					use_jiaotong_stander=0;
    					flag = 0;
    				}
    			}
    			else 
    				{
    					Err_display(44);
    					use_jiaotong_stander=0;
    					flag = 0;
    				}
                }
             
             if(CardLanCPU.applockflag)
              {
                Err_display(53);
    			use_jiaotong_stander=0;
    			flag = 0;
                }
             
			break;
            
            case 21:
            // 读 1E01 循环记录文件
			//printf("读1E01文件\n");
			//不需要读，只需要更新
			t++;
#if 0			
			memset(binname,0,sizeof(binname));
			binname[0] = 0x00;
			binname[1] = 0xb2; 
			binname[2] = 0x01; 
			binname[3] = 0xF4; // 001A
			binname[4] = 0x00;
			result = write(mf_fd,binname,5);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
				    DBG_DATA_PRINTF("1E01记录文件返回:");
					menu_print(receive_buf, receive_len[0]);	
					t++;   
					CardLanCPU.tradetype1E = receive_buf[0];
                    memcpy(CardLanCPU.deviceNO1E,&receive_buf[1],8);
                    CardLanCPU.industrycode1E = receive_buf[9];
                    memcpy(CardLanCPU.line1E,&receive_buf[10],2);
                    memcpy(CardLanCPU.station1E,&receive_buf[12],2);
                    memcpy(CardLanCPU.operatorcode,&receive_buf[14],2);                    
                    memcpy(CardLanCPU.trademoney1E,&receive_buf[18],2);
                    memcpy(CardLanCPU.tradeaftermoney1E,&receive_buf[20],2);
                    memcpy(CardLanCPU.tradetime1E,&receive_buf[22],7);
                    memcpy(CardLanCPU.acceptorcitycode,&receive_buf[29],2);
                     memcpy(CardLanCPU.acceptorissuerlabel,&receive_buf[31],8);
                    
				}
				else
				{
					printf("001A SW1=%02X  SW2=%02X\n",receive_buf[0],receive_buf[1]);
					Err_display(44);
					use_jiaotong_stander=0;
					flag = 0;
				}
			}
			else 
				{
					Err_display(44);
					use_jiaotong_stander=0;
					flag = 0;
				}                
#endif

                break;
            
			case 22:// 查询余额
				//printf("查询余额\n");
				result = write(mf_fd,"\x80\x5c\x03\x02\x04",5);
				if(result == MI_OK)
				{
					ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
					result = read(mf_fd, receive_buf, receive_len[0]);
					if(receive_len[0] > 2)
					{
						t++;
						memcpy(CardLanCPU.beforemoney,receive_buf,4);
						printf("\n交易前余额:%d分\n",receive_buf[0]<<24|receive_buf[1]<<16|receive_buf[2]<<8|receive_buf[3]);
					}
					else
					{
						printf("查询余额 SW1=%02X  SW2=%02X\n",receive_buf[0],receive_buf[1]);
						flag = 0;
					}
				}
				else {
					flag = 0;
					use_jiaotong_stander=0;
					}
				break;
			case 23:
				flag = 0;
				t  = 0;
				break;  
#endif
        
		default:
			flag = 0;
			break;
		}
	}


	


	return t;
}


int half_search_zhujian(BlackItem dest, int *find)
{
	int low,high,mid,val;
	low = 0;
	*find = 0;
	low = 0;
	mid = 0;
  
	if((BlackListFilezhujian.count == 0)||(BlackListFilezhujian.buf==NULL))
		return 0;

	high = BlackListFilezhujian.count - 1;

	DBG_PRINTF("high= %d\n", high);
	DBG_PRINTF("dest: %02X%02X%02X%02X%02X%02X%02X%02X\n", dest.dat[0],dest.dat[1],\
		dest.dat[2],dest.dat[3],dest.dat[4],dest.dat[5],dest.dat[6],dest.dat[7]);
	
	//printf("正在折半查找黑名单\n");
	while (low <= high)
	{
		mid = (low + high)/ 2;

		DBG_PRINTF("BlackListFile.buf[mid].dat: %02X%02X%02X%02X%02X%02X%02X%02X\n", BlackListFilezhujian.buf[mid].dat[0],BlackListFilezhujian.buf[mid].dat[1],\
		BlackListFilezhujian.buf[mid].dat[2],BlackListFilezhujian.buf[mid].dat[3],BlackListFilezhujian.buf[mid].dat[4],BlackListFilezhujian.buf[mid].dat[5],BlackListFilezhujian.buf[mid].dat[6],BlackListFilezhujian.buf[mid].dat[7]);
		
		//printf("dest %p buf %p \n",dest.dat,BlackListFile.buf[mid].dat);
		val = memcmp(dest.dat, BlackListFilezhujian.buf[mid].dat, 10 );//izeof(BlackListFile.buf[0]));
		//  val= 1;	
		//printf("使用memcmp没死掉\n");
		if (val == 0)
		{
			unsigned char src[10];
			memcpy(src, BlackListFilezhujian.buf[mid].dat, sizeof(BlackListFilezhujian.buf[0]));
			DBG_PRINTF("src: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n", src[0],src[1],\
				src[2],src[3],src[4],src[5],src[6],src[7],src[8],src[9]);

			*find = 1;
			low = mid;
			break;
		}
		else if (val > 0)
			low = mid + 1;	//dest > src
		else
			high = mid - 1;
	}

	DBG_PRINTF("low= %d find=%d\n", low, *find);
   
//	printf("half_serach结束\n");
	return low;
}


void update_sortfile_zhujian(BlackItem dat, unsigned char cmd)
{
	int find,count;
	int i,pos;

	count = BlackListFilezhujian.count;
	i = 0;
	pos = half_search_zhujian(dat, &find);

	if (cmd == 0 && !find)
	{ /*update*/
		for(i=BlackListFilezhujian.count; i>pos; i--)
		{
			BlackListFilezhujian.buf[i] = BlackListFilezhujian.buf[i-1];
		}
		BlackListFilezhujian.buf[pos] = dat;
		BlackListFilezhujian.count++;
	}
	else if (cmd == 1 && find && (BlackListFilezhujian.count > 0))
	{ /*enbale*/
		i=pos;
		while(i < (BlackListFilezhujian.count-1))
		{
			BlackListFilezhujian.buf[i] = BlackListFilezhujian.buf[i+1];
			i++;
		}
		memset( BlackListFilezhujian.buf[BlackListFilezhujian.count-1].dat, 0x00, sizeof(BlackListFilezhujian.buf[0]));
		BlackListFilezhujian.count--;
	}
}

void SavetBlackListBuff_zhujian()
{
	FILE *stream;
	
	stream= fopen(CITYUNION_BL_FILEBAK,"wb+");
	fseek(stream, 0, SEEK_SET);
	fwrite(BlackListFilezhujian.buf, sizeof(unsigned char), BlackListFilezhujian.count * sizeof(BlackListFilezhujian.buf[0]), stream);

	printf("BlackListFile.count %d\n", BlackListFilezhujian.count);
	printf("sizeof(BlackListFile.buf[0] %d\n", sizeof(BlackListFilezhujian.buf[0]) );
	printf("SavetBlackListBuff(): %d\n", BlackListFilezhujian.count * sizeof(BlackListFilezhujian.buf[0]));

	fclose_nosync(stream);	
}


void InitBlackListBuff_zhujian()
{

		FILE *fp;
		int i,j;
	BlackListFilezhujian.buf = NULL;
	BlackListFilezhujian.count = 0;
	
	//BlackListFile.buf = (BlackItem *)malloc(MAX_BLACK_CONNT * sizeof(BlackListFile.buf[0]));
	BlackListFilezhujian.buf = (BlackItem *)malloc(MAX_BLACK_CONNT * sizeof(BlackItem));
	if(BlackListFilezhujian.buf == NULL) return;
	memset(BlackListFilezhujian.buf, 0x00, (MAX_BLACK_CONNT * sizeof(BlackListFilezhujian.buf[0])));

	fp = fopen(CITYUNION_BL_FILEZHUJIAN, "rb");
	if(fp == NULL)
	{	
		printf("Can't open the Blacklist.sys\n");
		return;
	}

	fseek(fp,0,SEEK_END);
	BlackListFilezhujian.count = ftell(fp) / sizeof(BlackListFilezhujian.buf[0]);
	fseek(fp,0,SEEK_SET);
	fread(BlackListFilezhujian.buf, sizeof(BlackItem), BlackListFilezhujian.count, fp);
	fclose_nosync(fp);
#if DEBUG	
	for(i=0;i<BlackListFilezhujian.count;i++)
	{
		printf("BlackListFile.buf[%d].dat: %02X%02X%02X%02X%02X%02X%02X%02X\n", i,BlackListFilezhujian.buf[i].dat[0],BlackListFilezhujian.buf[i].dat[1],\
		BlackListFilezhujian.buf[i].dat[2],BlackListFilezhujian.buf[i].dat[3],BlackListFilezhujian.buf[i].dat[4],BlackListFilezhujian.buf[i].dat[5],BlackListFilezhujian.buf[i].dat[6],BlackListFilezhujian.buf[i].dat[7]);
	}		
#endif
}

unsigned char GreyLockCard(unsigned int Money,char *Key,unsigned char Keylen,char alarm)
{
	int result;
	char Send[128],Recv[128];
	char Mbuff[4],RandomNum[4];
	unsigned char flag,t,status,len;

	char InitChas[]= {0x80,0x50,0x01,0x02,0x0b}; //,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x0f;
	// 消费初始化  KEY:0x00  金额:0.01元 终端机号:1号
	char InitSamMac[]= {0x80,0x1a,0x45,0x01,0x10};
	char CountSamMac[]= {0x80,0xfa,0x05,0x00,0x10};
	char GreyLock[]= {0x84,0x1e,0x00,0x00,0x04};

	flag = 1;
	t = 1;
#if APP_PRINTF
	printf("Enter %s", __FUNCTION__);
#endif
	while(flag)
	{
		switch(t)
		{
			case 1:
				memset(Mbuff,0,sizeof(Mbuff));
				memset(Send,0,sizeof(Send));
				MoneyChange(Money,Mbuff);
				memcpy(Send,InitChas,sizeof(InitChas)); //命令头
				Send[5] = Key[0]; //密钥类型
				memcpy(Send+6,Mbuff,4); //消费金额
				memcpy(Send+10,PsamNum,6); //终端机号
				Send[16] = 0x0f; //
				len = 17;

#if __DEBUG
				printf("InitChas : %d\n",__LINE__);
#endif
				result = write(mf_fd,Send,len);
				if(result == MI_OK)
				{
                    ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				    result = read(mf_fd, receive_buf, receive_len[0]); 
					if(receive_len[0] > 2)
					{
						t++;
						OldCi.i = 0;
						OldCash.i = 0;
						memcpy(Recv,receive_buf,receive_len[0]);
						memcpy(OldCash.longbuf,Recv,4);
						MoneyChange(OldCash.i,CardLan.QCash);
						memset(CardLan.ViewMoney,0,sizeof(CardLan.ViewMoney));
						CardLan.ViewMoney[0] = Recv[5];
						CardLan.ViewMoney[1] = Recv[4];
						OldCi.longbuf[0] = Recv[5];
						OldCi.longbuf[1] = Recv[4];
						CardLan.CountType = Recv[10];
#if 0
						memcpy(Buf.longbuf,CardLan.CAppCash,4);
						if (is_max_money_exceed(Buf.i)) {
							Err_display(REMONEY_ALARM_ERR);
							flag = 0;
						}
#endif
					}
					else if((receive_buf[0] == 0x94)&&(receive_buf[1] == 0x01))
					{
						// 余额不足
						Err_display(12);
						flag = 0;
					}
					else
					{
#if _F26
					if (alarm)
#endif
						Err_display(6);
						flag = 0;
					}

#if APP_PRINTF
					{
						unsigned char i;
						printf("InitChas get data(%d) = %d\n",__LINE__,receive_len[0]);
						for(i =0 ; i < receive_len[0]; i ++)
						{
							printf("%02X",receive_buf[i]);
						}
						printf("\n");
					}
#endif
				}
				else
				{
#if _F26
					if (alarm)
#endif
					Err_display(6);
					flag = 0;
				}
				break;

			case 2:

#if __DEBUG
				printf("GetChall4 : %d\n",__LINE__);
#endif				
                result= write(mf_fd,GetChall4,sizeof(GetChall4));
				if(result == MI_OK)
				{
                    ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				    result = read(mf_fd, receive_buf, receive_len[0]); 
					if(receive_len[0] >= 2)
					{
						if((receive_buf[receive_len[0]-2] == 0x90)&&(receive_buf[receive_len[0]-1] == 0x00))
						{
							t++;
							memcpy(RandomNum,receive_buf,4);
						}
						else
						{
#if _F26
					if (alarm)
#endif
							Err_display(6);
							flag = 0;
						}
					}
					else
					{
#if _F26
					if (alarm)
#endif
						Err_display(6);
						flag = 0;
					}

#if APP_PRINTF
					{
						unsigned char i;
						printf("GetChall4 get data = %d\n",receive_len[0]);
						for(i =0 ; i < receive_len[0]; i ++)
						{
							printf("%02X",receive_buf[i]);
						}
						printf("\n");
					}
#endif


				}
				else
				{
#if _F26
					if (alarm)
#endif
					Err_display(6);
					flag = 0;
				}
				break;

			case 3:
				memset(Send,0,sizeof(Send));
				memcpy(Send,InitSamMac,5);
				memcpy(Send+5,CardLan.Appcsn,8); //
				memcpy(Send+13,CardLan.CityId,2); //
				memcpy(Send+15,"\xff\x00\x00\x00\x00\x00",6); //
				len = 21;
				memset(Recv,0,sizeof(Recv));
#if __DEBUG
				printf("InitSamMac : %d\n",__LINE__);
#endif
				status = PsamCos(Send,Recv,&len);
				if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
				{
					t ++;
				}
				else
				{
					Err_display(6);
					flag = 0;
				}
				break;

			case 4:
				memset(Send,0,sizeof(Send));
				memcpy(Send,CountSamMac,5);
				memcpy(Send+5,RandomNum,4); //
				memcpy(Send+9,"\x00\x00\x00\x00\x84\x1e\x00\x00\x04\x80\x00\x00",12); //
				len = 21;
				memset(Recv,0,sizeof(Recv));

#if __DEBUG
				printf("CountSamMac : %d\n",__LINE__);
#endif
				status = PsamCos(Send,Recv,&len);
				if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
				{
					t++;
				}
				else
				{
#if _F26
					if (alarm)
#endif
					Err_display(6);
					flag = 0;
				}
				break;

			case 5:
				memset(Send,0,sizeof(Send));
				memcpy(Send,GreyLock,sizeof(GreyLock));
				memcpy(Send+5,Recv,4);//

#if __DEBUG
				printf("GreyLock : %d\n",__LINE__);
#endif
				result = write(mf_fd,Send,9);
				if(result == MI_OK)
				{
                    ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				    result = read(mf_fd, receive_buf, receive_len[0]); 
					if(receive_len[0] >= 2)
					{
						if((receive_buf[receive_len[0]-2] == 0x90)&&(receive_buf[receive_len[0]-1] == 0x00))
						{
							t++;
						}
						else
						{
#if _F26
					if (alarm)
#endif
							Err_display(6);
							flag = 0;
						}
					}
					else
					{
#if _F26
					if (alarm)
#endif
						Err_display(6);
						flag = 0;
					}

#if APP_PRINTF
					{
						unsigned char i;
						printf("GreyLock get data = %d\n",receive_len[0]);
						for(i =0 ; i < receive_len[0]; i ++)
						{
							printf("%02X",receive_buf[i]);
						}
						printf("\n");
					}
#endif

				}
				else
				{
#if _F26
					if (alarm)
#endif
					Err_display(6);
					flag = 0;
				}
				break;

			case 6:
				t = 0;
				flag =0;
				break;

			default :
				flag =0 ;
				break;
		}
	}


#if APP_PRINTF
	printf("GreyLockCard   End == %d \n",t);
#endif

	return t;
}







unsigned char SecurityCertificate_zhujian(void)
{
	int result;
	unsigned char flag = 1;
	unsigned char t = 1;
	unsigned char status,len;
	char Send[128],Recv[128];
	char GetUserScn[] = {0x80,0xca,0x00,0x00,0x09};


	while(flag)
	{
		switch(t)
		{
		case 1:
			len = sizeof(GetUserScn);
			result = write(mf_fd,GetUserScn,len);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
					t++;
					memcpy(Recv,receive_buf,9);
				}
				else
				{
					if(receive_len[0]>=2)
					{

						printf("Recv No:%02X%02X",receive_buf[receive_len[0]-2],receive_buf[receive_len[0]-1]);
					}
					else
					{
						printf("Recv No:0000");
					}
					//Err_display(52);                  
					flag = 0;
				}

#if APP_PRINTF
				{
					unsigned char i;
					printf("GetUserScn get data = %d\n",receive_len[0]);
					for(i =0 ; i < receive_len[0]; i ++)
					{
						printf("%02X",receive_buf[i]);
					}
					printf("\n");
				}
#endif
			}
			else
			{
				//Err_display(52);
				flag = 0;
			}
			break;


		case 2:
			memset(Send,0,sizeof(Send));
			len = sizeof(GetUserScn);
			memcpy(Send,GetUserScn,len);
			memcpy(Send+len,Recv,9);
			len = len + 9;
			memset(Recv,0,sizeof(Recv));
			status = PsamCos(Send,Recv,&len);
			if((status == MI_OK)&&(Recv[len -2] == 0x90)&&(Recv[len -1] == 0x00))
			{
				t++;
			}
			else
			{
				Err_display(32);
				flag = 0;
			}
			break;

		default :
            t=0;
			flag = 0;
			break;
		}
	}



	DBG_PRINTF("SecurityCertificate	 End == %d \n",t);



	return t;

}



/*
*************************************************************************************************************
- 函数名称 : unsigned char Permissions (HWND hDlg)
- 函数说明 : 权限处理
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
unsigned char Permissions_zhujian (void)
{
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char status;

	DBG_PRINTF("Permissions() is called.\n");

	while(Loop)
	{
		switch (step)
		{
		case 1:
			status = TimeRange();				//当前有没有合法的消费时间段
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


		case 2:		//判断卡片是否是黑名单卡 33表示卡是黑名单
			status  = Card_JudgeCsn_zhujian();
			printf("Card_JudgeCsn(%d) = %d,CardTypeIC = %d\n",__LINE__, status,CardTypeIC);

			if(status == MI_OK)step++;
			else
			{			
				Loop = 0;
			}
			break;

		case 3:	//判断卡片类别
			status  = SupportType();
			if(status == MI_OK)step++;
			else
			{
				Err_display(11);
				Loop = 0;
			}
			break;

		case 4:
            step++;
            /*
			if(SavedataErr == 0)step++;
			else if(SavedataErr == 2)
			{
				Err_display(21);
				Loop = 0;
			}
			else
			{
				Err_display(20);
				Loop = 0;
			}
			*/
			break;

		case 5:
			status = CardDiscounts(CardLan.CardType,0);
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




unsigned char CityunionWhiitelist(unsigned char *CityNumber,unsigned char cmd)
{
    /*
    int result;
    unsigned int city;
    unsigned int CardByte,CardBit;
    unsigned char NumBuf,i,status;
    unsigned char FileBuf[8];
    memset(FileBuf,0,sizeof(FileBuf));
    sprintf(FileBuf,"%02x%02x",CityNumber[0],CityNumber[1]);
    city = atoi(FileBuf);
    CardByte = city/8;
    CardBit = city%8;
    memset(FileBuf,0,sizeof(FileBuf));
    CWhiteFile= fopen("/mnt/record/CWhitelist.sys","rb+");
    result = fseek(CWhiteFile,CardByte, SEEK_SET);
    result = fread(FileBuf,sizeof(unsigned char),2,CWhiteFile);
    switch(cmd)
    {
    case 0:										//下载
        NumBuf = 0x01;
        NumBuf = NumBuf << CardBit;
        FileBuf[0] = FileBuf[0] | NumBuf;
        result = fseek(CWhiteFile,CardByte, SEEK_SET);
        result = fwrite(FileBuf,sizeof(unsigned char),2,CWhiteFile);
        status = 0;
        break;
    case 1:
        NumBuf = 0x01;
        NumBuf = NumBuf << CardBit;
        if((NumBuf & FileBuf[0]))
        {
            NumBuf = 0xFE;
            for(i = 0; i < CardBit; i++) NumBuf = (NumBuf << 1)|0x01;
            FileBuf[0] = FileBuf[0] & NumBuf;
            result = fseek(CWhiteFile,CardByte, SEEK_SET);
            result = fwrite(FileBuf,sizeof(unsigned char),2,CWhiteFile);
        }				//解挂
        status = 0;
        break;
    case 2:
        status = FileBuf[0] >> CardBit;
        status = status&0x01;
        break;
    }
    fclose(CWhiteFile);
    return(status);
    */
    unsigned char status=1;
	unsigned char i;
	unsigned short CityCode;

	CityCode = CityNumber[0]<<8|CityNumber[1];
	for(i=0;i<sizeof(Union_Card);i++)
	{
		if(CityCode == Union_Card[i])
		{
			status = 0;
			break;
		}
	}
	return status;
}


unsigned char CheckCityUnionZHUJIAN(void)
{

	unsigned char flag = 1;
	unsigned char t = 1;
	unsigned char status;
   DBG_PRINTF("CheckCityUnionZHUJIAN() is called.\n");

	while(flag)
	{
		switch(t)
		{
			case 1:
				status = CityunionWhiitelist(CardLan.Appfalw,2);
				if(status != MI_OK)
				{
				    Err_display(29);
					flag = 0;
				}
                else
                    t++;
				break;

			case 2:
				if(CardLan.Appstate!= MI_OK)
				{
					t++;
				}
				else
				{
					Err_display(45);
					flag = 0;
				}
				break;

			case 3:		//有效日期是否可行
				status = Card_JudgeDate();
				if(status == MI_OK)
				{
					t++;
				}
				else
				{
#if 1
					if (status == 2) {
						printf("11111111\n");
						Err_display(49);
						flag= 0;
					} else if (status == 1) {
						Err_display(1);
						flag= 0;
					} else
#endif
					Err_display(1);
					flag= 0;
				}
				break;

			case 4:
				flag = 0;
				t = 0;
				break;

			default :
				flag = 0;
				break;

		}
	}

#if APP_PRINTF
	printf("CheckCityUnion	 End == %d \n",t);
#endif

	return t;
}

unsigned char CheckLocalApp(void)
{
	unsigned char flag = 1;
	unsigned char t = 1;
	unsigned char status;
    DBG_PRINTF("CheckLocalApp() is called.\n");
    
	while(flag)
	{
		switch(t)
		{
			case 1:

				if(CardLan.Appstate!= MI_OK)
				{
					t++;
				}
				else
				{
					Err_display(45);
					flag = 0;
				}
				break;

			case 2:		//有效日期是否可行
				status = Card_JudgeDate();
				if(status == MI_OK)
				{
					t++;
				}
				else
				{

				if (status == 2) {
					printf("22222222\n");
					Err_display(49);
					flag= 0;
				} else if (status == 1) {
					Err_display(1);
					flag= 0;
				} else
					Err_display(1);
					flag= 0;
				}
				break;

			case 3:
				flag = 0;
				t = 0;
				break;

			default :
				flag = 0;
				break;

		}
	}

#if APP_PRINTF

	printf("CheckLocalApp	 End == %d \n",t);

#endif

	return t;
}



/*
*************************************************************************************************************
- 函数名称 : unsigned char ReadorRepairCard (HWND hDlg,int mode)
- 函数说明 : 用户卡读及修复卡片
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
unsigned char ReadorRepairCard_CPU(void)
{
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char status;
	LongUnon Buf;
	unsigned int beforeMoney;
    unsigned char keybuff[16];
    char buf[2]={0x32,0x50};
	DBG_PRINTF("ReadorRepairCard_CPU() is called.\n");

	while(Loop)
	{
        DBG_PRINTF("the step 0f readorrepaircard is :%d\n",step);
		switch(step)
		{
		case 1:
			status = ReadCardInfor_CPU();
			if(status == MI_OK)
			{
#if Transport_Stander
                if(Tunion==0)
                    Loop = 0;
                else
                {
    				if(use_jiaotong_stander==0x55)
    					step=10;
    				else
    					step++;
                 }
#else
				step ++;
#endif
			}
			else
			{	
				Loop = 0;
			}
			break;

		case 2:		//有效日期是否可行             
				if(memcmp(buf,CardLan.CityId,2) == 0)
				{
					status = CheckLocalApp();
					if(status == MI_OK) step ++;
					else Loop = 0;
				}
				else
				{
					status = CheckCityUnionZHUJIAN();
					if(status == MI_OK) step ++;
					else Loop = 0;
				}
				
			break;
            
        case 3:  
        	status = SecurityCertificate_zhujian();
        		if(status == MI_OK)
        		{
        			step ++;
        		}
        		else
        		{
        			Loop = 0;
        		}

				break;
            

		case 4:
			status = Permissions_zhujian();				//权限
			if(status == MI_OK)
                {
                    step ++;
             }
			else if(status == 2)
				{
					keybuff[0] = PsamKeyIndex;
					status = GreyLockCard(0,keybuff,16,0);
					if(status == 0)
					{

						Sector.FlagValue = 0;
						HostValue.i= 0;
						DecValue.i= 0;
					//	IncSerId();
					//	SaveCardData(10);
					}
					Err_display(10);
					Loop = 0;
				}
				else
				{
					Loop = 0;
				}            
			
			break;

		case 5:
			HostValue.i = DecValue.i = 0;
			status = AnalysisSheet(1);				//查消费表
			if(status == MI_OK) step ++;
			else
			{
				Err_display(16);               
				ioctl(mf_fd, RC531_HALT);
				Loop = 0;
			}
			break;

		case 6:								//扣现金
           
            Sector.FlagValue = 2;
            
			memcpy(Buf.longbuf,CardLan.QCash,4);
			if(Buf.i >= HostValue.i)
			{
				Sector.FlagValue = 2;
				step++;
			}
			else
			{
				Err_display(12);              
				ioctl(mf_fd, RC531_HALT);
				Loop = 0;
			}
			
			break;

		case 7:
			status = TopUpCardInfor_CPU(2);
		//	status = TopUpCardInfor_CPU(0);
			if(status == MI_OK)
			{
				step++;
				IncSerId();
			}
			else
			{
				// Err_display(6);
				Loop = 0;
			}
			break;

		case 8:
#if Transport_Stander
            status = SaveCardData_jiaotong(CARD_SPEC_CPU_PBCO20, CONSUME_MODE_PRESET, GET_RECORD | SAVE_RECORD); //保存数据  定额
#else
            #ifdef CANGNAN_BUS
            use_jiaotong_stander = 0xAA;
            status = SaveCardData_Zhujian(CARD_SPEC_CPU_PBCO20, CONSUME_MODE_PRESET, GET_RECORD | SAVE_RECORD); //保存数据  定额
            #else
			status = SaveCardData(CARD_SPEC_CPU_PBCO20, CONSUME_MODE_PRESET, GET_RECORD | SAVE_RECORD); //保存数据  定额
			#endif
#endif
            if(status == MI_OK) step ++;
			else
			{
				Err_display(20);
				Loop = 0;
			}
			break;

		case 9:
			AutoUpFlag = 0x55;  
			step = 0;
			Loop = 0;
			break;
			
#if Transport_Stander

		case 10:
			printf("开始交通部流程\n");
			status = Permissions_jiaotong(0);
			if(status == MI_OK) step ++;
			else{
				Loop = 0;
				use_jiaotong_stander=0;
			}
			break;
			
		case 11://查消费表
			//printf("step 11---------->\n");
			HostValue.i = DecValue.i = 0;
			status = AnalysisSheet_Cpu_jiaotong(1);	
			if(status == MI_OK) 
			{				
				step ++;
			}
			else
			{
				use_jiaotong_stander=0;
				Err_display(16);
				ioctl(mf_fd, RC531_HALT);
				Loop = 0;
			}
			break;
			
		case 12://判断卡余额是否支持本次消费
		    
			//printf("step 12------->\n");
			beforeMoney=CardLanCPU.beforemoney[0]<<24|CardLanCPU.beforemoney[1]<<16|CardLanCPU.beforemoney[2]<<8|CardLanCPU.beforemoney[3];
			printf("beforeMoney=%d\n,HostValue.i=%d\n",beforeMoney,HostValue.i);
            if(beforeMoney>100000)
            {
                use_jiaotong_stander=0;
				Err_display(64);
				ioctl(mf_fd, RC531_HALT);
                Loop = 0;
                }
            else if(beforeMoney >= HostValue.i)
			{
				Sector.FlagValue = 2;
				step++;
			}
			else
			{
				use_jiaotong_stander=0;
				Err_display(12);
				ioctl(mf_fd, RC531_HALT);
				Loop = 0;
			}
			break;
			
		case 13:
			//电子钱包进行交易扣款
			//printf("step 13------------>\n");
			status = TopUpCardInfor_CPU_jiaotong(1);
			if(status == MI_OK)
			{
				step++;
				IncSerId();
                ioctl(mf_fd,DO_TYPEA_M1);	
                ioctl(mf_fd, RC531_HALT);
			}
			else
                {
                    switch(status)
                    {
                        case 0xAA:
                            Err_display(40);
                            break;
                        case 0x02:
                            Err_display(31);
                            break;                     
                        default:
                            Err_display(54);
                            break;
                        }
                    use_jiaotong_stander=0;
                    ioctl(mf_fd, RC531_HALT);
				    Loop = 0;                    
                    }
			break;
		case 14:
			
			CardLanCPU.aftermoney[0] = ((beforeMoney-HostValue.i)&0xFF000000)>>24;
			CardLanCPU.aftermoney[1] = ((beforeMoney-HostValue.i)&0xFF0000)>>16;
			CardLanCPU.aftermoney[2] = ((beforeMoney-HostValue.i)&0xFF00)>>8;
			CardLanCPU.aftermoney[3] = ((beforeMoney-HostValue.i)&0xFF)>>0;
            CardLan.CardType = GetCardType();   //用于播报语音
			status = SaveCardData_jiaotong(CARD_SPEC_CPU_PBCO20, CONSUME_MODE_PRESET, GET_RECORD | SAVE_RECORD); //保存数据  定额
			if(status == MI_OK) step ++;
			else
			{	use_jiaotong_stander=0;
				Err_display(20);
				Loop = 0;
			}
			break;

		case 15:            
			AutoUpFlag = 0x55;
			use_jiaotong_stander=0xAA;		//用于控制显示
			step = 0;
			Loop = 0;
			break;
#endif 
		default:
			Loop = 0;
			break;
		}
	}
	
	return step;
}




/*
*************************************************************************************************************
- 函数名称 : unsigned char ReadorRepairCard (HWND hDlg,int mode)
- 函数说明 : 用户卡读及修复卡片
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
unsigned char FreeReadorRepairCard_CPU(void)
{
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char status;
	LongUnon Buf;
    unsigned int beforeMoney;

	while(Loop)
	{
		switch(step)
		{
		case 1:
			status = ReadCardInfor_CPU();
			if(status == MI_OK)
			{
#if Transport_Stander
				if(use_jiaotong_stander==0x55)
					step=10;
				else
					step++;
#else
				step ++;
#endif
			}
			else
			{
				Loop = 0;
			}
			break;

		case 2:		//有效日期是否可行
			status = Card_JudgeDate();
			if(status == MI_OK)
			{
				step++;
			}
			else
			{
				Err_display(1);
                Err_save(CARD_SPEC_CPU_PBCO20,1);
				Loop = 0;
			}
			break;

		case 3:
			status = Permissions(0);				//权限
			if(status == MI_OK) step ++;
			else Loop = 0;
			break;

		case 4:
			status = AnalysisSheet(0);				//查消费表
			if(status == MI_OK) step ++;
			else
			{
				Err_display(16);
                Err_save(CARD_SPEC_CPU_PBCO20,16);
				ioctl(mf_fd, RC531_HALT);
				Loop = 0;
			}
			break;

		case 5:								//扣现金			
			memcpy(Buf.longbuf,CardLan.QCash,4);
			if(Buf.i >= HostValue.i)
			{
				Sector.FlagValue = 2;
				step++;
			}
			else
			{
				Err_display(12);
                Err_save(CARD_SPEC_CPU_PBCO20,12);
				ioctl(mf_fd, RC531_HALT);
				Loop = 0;
			}
			break;

		case 6:
			status = TopUpCardInfor_CPU(0);
			if(status == MI_OK)
			{
				IncSerId();
				step++;
			}
			else Loop = 0;
			break;

		case 7:
#if Transport_Stander
            status = SaveCardData_jiaotong(CARD_SPEC_CPU_PBCO20, CONSUME_MODE_FLEXIBLE, GET_RECORD | SAVE_RECORD); //保存数据  定额
#else
			status = SaveCardData(CARD_SPEC_CPU_PBCO20, CONSUME_MODE_FLEXIBLE, GET_RECORD | SAVE_RECORD); //保存数据  定额
#endif
			if(status == MI_OK) step ++;
			else
			{
				Err_display(20);
				Loop = 0;
			}
			break;

		case 8:
			AutoUpFlag = 0x55;
			step = 0;
			Loop = 0;
			break;
            
#if Transport_Stander

		case 10:
			printf("开始交通部流程\n");
			status = Permissions_jiaotong(0);
			if(status == MI_OK) step ++;
			else{
				Loop = 0;
				use_jiaotong_stander=0;
			}
			break;
			
		case 11://查消费表
			//printf("step 11---------->\n");			
			status = AnalysisSheet_Cpu_jiaotong(1);	
			if(status == MI_OK) 
			{				
				step ++;
			}
			else
			{
				use_jiaotong_stander=0;
				Err_display(16);
				ioctl(mf_fd, RC531_HALT);
				Loop = 0;
			}
			break;
			
		case 12://判断卡余额是否支持本次消费
		    
			//printf("step 12------->\n");
			beforeMoney=CardLanCPU.beforemoney[0]<<24|CardLanCPU.beforemoney[1]<<16|CardLanCPU.beforemoney[2]<<8|CardLanCPU.beforemoney[3];
			printf("beforeMoney=%d\n,HostValue.i=%d\n",beforeMoney,HostValue.i);
            if(beforeMoney>100000)
            {
                use_jiaotong_stander=0;
				Err_display(64);
				ioctl(mf_fd, RC531_HALT);
                }
            else if(beforeMoney >= HostValue.i)
			{
				Sector.FlagValue = 2;
				step++;
			}
			else
			{
				use_jiaotong_stander=0;
				Err_display(12);
				ioctl(mf_fd, RC531_HALT);
				Loop = 0;
			}
			break;
			
		case 13:
			//电子钱包进行交易扣款
			//printf("step 13------------>\n");
			status = TopUpCardInfor_CPU_jiaotong(1);
			if(status == MI_OK)
			{
				step++;
				IncSerId();
			}
			else if(status == 0xAA) 	//防插拔处理的结果
			{
				//此处没有保存不明确状态的扣款记录
				use_jiaotong_stander=0;
				Err_display(40);
				ioctl(mf_fd, RC531_HALT);
				Loop = 0;
			}
			else
			{	use_jiaotong_stander=0;
				Err_display(4);
				Loop = 0;
			}
			break;
		case 14:
			
			CardLanCPU.aftermoney[0] = ((beforeMoney-HostValue.i)&0xFF000000)>>24;
			CardLanCPU.aftermoney[1] = ((beforeMoney-HostValue.i)&0xFF0000)>>16;
			CardLanCPU.aftermoney[2] = ((beforeMoney-HostValue.i)&0xFF00)>>8;
			CardLanCPU.aftermoney[3] = ((beforeMoney-HostValue.i)&0xFF)>>0;
			status = SaveCardData_jiaotong(CARD_SPEC_CPU_PBCO20, CONSUME_MODE_FLEXIBLE, GET_RECORD | SAVE_RECORD); //保存数据  定额
			if(status == MI_OK) step ++;
			else
			{	use_jiaotong_stander=0;
				Err_display(20);
				Loop = 0;
			}
			break;

		case 15:
            CardLan.CardType = CardLanCPU.cardtype;   //用于播报语音
			AutoUpFlag = 0x55;
			use_jiaotong_stander=0xAA;		//用于控制显示
			step = 0;
			Loop = 0;
			break;
#endif 
		default:
			step = 0;
			Loop = 0;
			break;
		}
	}

	return step;
}




unsigned char ExitCarProgram_CPU(unsigned char type)
{
	unsigned char step = 1;
	unsigned char Loop = 1;
	unsigned char status;
	LongUnon Buf;

	if(type == 0)
	{
		while(Loop)
		{
			switch(step)
			{
				case 1:
					status = SectionSheet();
					if(status == MI_OK) step++;
					else
					{
						Err_display(16);
						Loop = 0;
					}
					break;

				case 2:
					status = AnalysisSheet(0);				//查消费表
					if(status == MI_OK) step ++;
					else
					{
						Err_display(16);
						Loop = 0;
					}
					break;

				case 3:
					if(Section.Updoor == 0x02)
					{
						if((CardLan.EnterCarCi == 0)||(CardLan.EnterCarCi >= 100)) CardLan.EnterCarCi = 1;
						HostValue.i = (unsigned int)( HostValue.i * CardLan.EnterCarCi);
						DecValue.i = HostValue.i;
					}
					else
					{
						CardLan.EnterCarCi = 1;
						DecValue.i = HostValue.i;
					}
					CardLan.StationID= 0;
					CardLan.EnterCarCi = 0;
					CardLan.EnterExitFlag = 0;
					memset(CardLan.CAppCash,0,sizeof(CardLan.CAppCash));
					step++;
					break;

				default :
					step =0;
					Loop =0;
					break;
			}
		}
	}
	else
	{
		memcpy(Buf.longbuf,CardLan.CAppCash,4);
		MoneyChange(Buf.i,Buf.longbuf);
		HostValue.i = DecValue.i = Buf.i;

		status = AnalysisSheet(0);				//查消费表
		if(status == MI_OK)
		{
			CardLan.StationID= 0;
			CardLan.EnterCarCi = 0;
			CardLan.EnterExitFlag = 0;
			memset(CardLan.CAppCash,0,sizeof(CardLan.CAppCash));
			step = 0;
		}
		else
		{
			Err_display(16);
			Loop = 0;
		}
	}

#if  RC500printf
	printf("ExitCarProgram = %d\n",step);
#endif

	return step;
}




unsigned char EnterCarProgram_CPU(void)
{
	unsigned char step = 1;
	unsigned char Loop = 1;
	LongUnon Buf,Value;

#if RC500printf
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
				memcpy(Buf.longbuf,Section.DeductMoney,4);	//预扣金额 分段
				Buf.i = SectionDiscountRate(Buf.i,CardLan.CardType);
				step++;
				break;

			case 2:
				if((memcmp(CardLan.CAppTermNo,PsamNum,6) == 0)&&(Section.SationNow == CardLan.StationID)&&(CardLan.EnterExitFlag == 0x01)&&(Section.Updoor == 0x01))
				{
					if(CardLan.EnterCarCi >= 100)CardLan.EnterCarCi = 0;
					CardLan.EnterCarCi ++;
					CardLan.EnterExitFlag = 0x01;
					CardLan.StationID = Section.SationNow;

					memcpy(Value.longbuf,CardLan.CAppCash,4);
					MoneyChange(Value.i,Value.longbuf);
					Value.i = Value.i + Buf.i;
					MoneyChange(Value.i,CardLan.CAppCash);
				}
				else
				{
					CardLan.EnterCarCi = 1;
					CardLan.EnterExitFlag = 0x01;
					CardLan.StationID = Section.SationNow;

					MoneyChange(Buf.i,CardLan.CAppCash);
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








unsigned char ComplexConsumption()
{
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char status;
	unsigned int beforeMoney;
    unsigned int markamount;
    unsigned char keybuff[16];
	char buf[2]={0x32,0x50};
    LongUnon temp;
	DBG_PRINTF("ComplexConsumption() is called.\n");
	
	while(Loop)
	{
		switch(step)
		{
		case 1:
				status = ReadCardInfor_CPU();
				if(status == MI_OK)
				{
					step ++;
				}
				else
				{
					Loop = 0;
				}
				break;

			case 2:		//有效日期是否可行
			    step++;
                
				if(memcmp(buf,CardLan.CityId,2) == 0)
				{
					status = CheckLocalApp();
					if(status == MI_OK) step ++;
					else Loop = 0;
				}
				else
				{
					status = CheckCityUnionZHUJIAN();
					if(status == MI_OK) step ++;
					else Loop = 0;
				}
				
				break;

			case 3:
				status = SecurityCertificate_zhujian();
				if(status == MI_OK)
				{
					step ++;
				}
				else
				{
					Loop = 0;
				}
				break;

			case 4:
				status = Permissions_zhujian(); 			//权限
				if(status == MI_OK) step++;
				else if(status == 2)
				{
					keybuff[0] = PsamKeyIndex;
					status = GreyLockCard(0,keybuff,16,0);
					if(status == 0)
					{
						Sector.FlagValue = 0;
						HostValue.i= 0;
						DecValue.i= 0;				               
					//	IncSerId();
					//	SaveCardData(11);
					}
					Err_display(10);
					Loop = 0;
				}
				else
				{
					Loop = 0;
				}
				break;

        	case 5:	//判断上下车
        			status = IsGetOnOff_ZHUJIAN();
        			if(status == MI_OK) step ++;
        			else
        			{
        				//Err_display(16);
        				ioctl(mf_fd, RC531_HALT);
        				use_jiaotong_stander=0;
        				Loop = 0;
        			}
        			break;

        	case 6:
                    temp.i = 0;
                    memcpy(temp.longbuf,CardLan.QCash,4);        			
                    markamount = CardLan.CAppCash[0]<<24|CardLan.CAppCash[1]<<16|CardLan.CAppCash[2]<<8|CardLan.CAppCash[3];
                    printf("temp.i=%d,HostValue.i=%d,markamount=%d\n",temp.i,HostValue.i,markamount);
                    if(temp.i>100000)
                    {
                        use_jiaotong_stander=0;
        				Err_display(64);
        				ioctl(mf_fd, RC531_HALT);
                        Loop = 0;
                        }
                    else
                    {
                        if((temp.i >= HostValue.i)&&(temp.i>=markamount))
            			{
            				Sector.FlagValue = 2;
            				step++;
            			}            
            			else
            			{
            				Err_display(12);
            				ioctl(mf_fd, RC531_HALT);
            				use_jiaotong_stander=0;
            				Loop = 0;
            			}
                        }
        			break;


/*
			case 5:
				HostValue.i = DecValue.i = 0;
                 Sector.FlagValue = 2;
				switch(Section.Updoor)
				{
					case 1:
						if(CardLan.EnterExitFlag == 0x00)
						{
							status = EnterCarProgram_CPU();
							if(status == MI_OK)
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
							Err_display(30);  //没有上车
							Loop = 0;
						}
						break;

					case 2:                        
						if(CardLan.EnterExitFlag == 0x01) 							  //出车
						{
							status = OverTimeEnter();
							if(0 == status)								  //标志是属于出车，但考虑到上次没打卡下车现象，超出这个时间
							{   //还是当进车处理,终端机号不对当进车处理
								if((CardLan.StationID<= Section.SationNum[0])&&(CardLan.StationID != 0))
								{
									status = ExitCarProgram_CPU(0);
									if(status == MI_OK)
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
									status = ExitCarProgram_CPU(1);
									if(status == MI_OK)
									{
										step ++;
									}
									else
									{
										Loop = 0;
									}
								}
							}
							else if(1 == status)
							{
								status = ExitCarProgram_CPU(1);
								if(status == MI_OK)
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
								Err_display(30);  //没有上车
								Loop = 0;
							}
						}
						else
						{
							Err_display(30);  //没有上车
							Loop = 0;
						}
						break;

					default:
						if(CardLan.EnterExitFlag == 0x01)								//出车
						{
							status = OverTimeEnter();
							if(0 == status)									//标志是属于出车，但考虑到上次没打卡下车现象，超出这个时间
							{   //还是当进车处理,终端机号不对当进车处理
								if((CardLan.StationID<= Section.SationNum[0])&&(CardLan.StationID != 0))
								{
									status =  ExitCarProgram_CPU(0);                                    
									if(status == MI_OK)
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
									status = ExitCarProgram_CPU(1);                                    
									if(status == MI_OK)
									{
										step ++;
									}
									else
									{
										Loop = 0;
									}
								}
							}
							else if(1 == status)
							{
								status = ExitCarProgram_CPU(1);                                
								if(status == MI_OK)
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
								Err_display(30);  //没有上车
								Loop = 0;
							}
						}
						else
						{
							status = EnterCarProgram_CPU();
							if(status == MI_OK)
							{
								step ++;
							}
							else
							{
								Loop = 0;
							}
						}
						break;
				}
				break;
        */
		case 7:
			status = TopUpCardInfor_CPU(1);
			if(status == MI_OK)
			{
				step++;
				IncSerId();
			}
			else
			{
				// Err_display(6);
				Loop = 0;
			}
			break;

		case 8:	
#if Transport_Stander
            status = SaveCardData_jiaotong(CARD_SPEC_CPU_PBCO20, CONSUME_MODE_SECTIONAL, GET_RECORD | SAVE_RECORD); //保存数据  分段
#else            
			status = SaveCardData(CARD_SPEC_CPU_PBCO20, CONSUME_MODE_SECTIONAL, GET_RECORD | SAVE_RECORD); //保存数据  分段
#endif			
			if(status == MI_OK) step ++;
			else
			{
				Err_display(20);
				Loop = 0;
			}
			break;

		case 9:
			AutoUpFlag = 0x55;
			step = 0;
			Loop = 0;
			break;
            
#if Transport_Stander
		case 10:
			printf("开始交通部分段流程\n");
			if(CardLanCPU.applockflag != 0x00)
			{
				Err_display(5);
				use_jiaotong_stander=0;
				Loop = 0;
			}
			else
				step ++;
			break;
			
		case 11:
			step++;
			break;
			
		case 12:
			status=Permissions_jiaotong(0);
			if(status == MI_OK) step ++;
			else {
				Loop = 0;
				use_jiaotong_stander=0;
				}
			break;
			
		case 13:	//判断上下车
			status = IsGetOnOff();
			if(status == MI_OK) step ++;
			else
			{
				//Err_display(16);
				ioctl(mf_fd, RC531_HALT);
				use_jiaotong_stander=0;
				Loop = 0;
			}
			break;

		case 14:
			beforeMoney=CardLanCPU.beforemoney[0]<<24|CardLanCPU.beforemoney[1]<<16|CardLanCPU.beforemoney[2]<<8|CardLanCPU.beforemoney[3];
            markamount = CardLanCPU.markamount[0]<<24|CardLanCPU.markamount[1]<<16|CardLanCPU.markamount[2]<<8|CardLanCPU.markamount[3];
            printf("beforeMoney=%d,HostValue.i=%d,markamount=%d\n",beforeMoney,HostValue.i,markamount);
            if(beforeMoney>100000)
            {
                use_jiaotong_stander=0;
				Err_display(64);
				ioctl(mf_fd, RC531_HALT);
                Loop = 0;
                }
            else
            {
                if((beforeMoney >= HostValue.i)&&(beforeMoney>=markamount))
    			{
    				Sector.FlagValue = 2;
    				step++;
    			}            
    			else
    			{
    				Err_display(12);
    				ioctl(mf_fd, RC531_HALT);
    				use_jiaotong_stander=0;
    				Loop = 0;
    			}
                }
			break;

		case 15:
			status = TopUpCardInfor_CPU_jiaotong(1);
			if(status == MI_OK)
			{
				step++;
				IncSerId();
			}
            else
            {
                switch(status)
                {
                    case 0xAA:
                        Err_display(40);
                        break;
                    case 0x02:
                        Err_display(31);
                        break;
                    default:
                        Err_display(4);
                        break;
                    }
                use_jiaotong_stander=0;
                ioctl(mf_fd, RC531_HALT);
			    Loop = 0;                  
                
                }
            
			break;

		case 16:
			printf("in step -----16\n");
			CardLanCPU.aftermoney[0] = ((beforeMoney-HostValue.i)&0xFF000000)>>24;
			CardLanCPU.aftermoney[1] = ((beforeMoney-HostValue.i)&0xFF0000)>>16;
			CardLanCPU.aftermoney[2] = ((beforeMoney-HostValue.i)&0xFF00)>>8;
			CardLanCPU.aftermoney[3] = ((beforeMoney-HostValue.i)&0xFF)>>0;	
			status = SaveCardData_jiaotong(CARD_SPEC_CPU_PBCO20, CONSUME_MODE_SECTIONAL, GET_RECORD | SAVE_RECORD); //保存数据  分段
			if(status == MI_OK) step ++;
			else
			{
				Err_display(20);
				use_jiaotong_stander=0;
				Loop = 0;
			}
			break;

		case 17:
			//printf("in step ----17\n");
			CardLan.CardType = CardLanCPU.cardtype;   //用于播报语音
			AutoUpFlag = 0x55;
			use_jiaotong_stander=0xAA;
			step = 0;
			Loop = 0;
			break;
#endif
		default:
			Loop = 0;
			break;
		}
	}

	return step;
}

/*
*************************************************************************************************************
- 函数名称 :int SqlCheckNewDat (unsigned int Dtype)
- 函数说明 : 查询是否有数据要上传
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
int SqlCheckNewDat_Zhujian (unsigned int Dtype)
{
    int result;
    int Saddr;
    int status;
    char tempbuf[4];
	DebugPrintf("SaveNumBc = %u CodeNum = %u\n", SaveNumBc.i, CodeNum.i);
    if(SaveNumBc.i > CodeNum.i)
    {
        // while(savedataflag);
        //     savedataflag = 1;
        pthread_mutex_lock(&m_datafile);

        Saddr = (int)((CodeNum.i-1) * 72);
        memset(Senddata, 0, sizeof(Senddata));

		Datafile = open(OFF_LINE_CONSUM_FILE, O_SYNC|O_RDWR);
		result = lseek(Datafile, Saddr, SEEK_SET);
		result = read(Datafile, Senddata, 72);

       if(Senddata[61]==0)
            memcpy(IDaddr.longbuf, Senddata+16, 4);
       else
       {
            memcpy(tempbuf, Senddata+16, 4);
            Chang4to4(tempbuf, IDaddr.longbuf);
            
        }
		status = 0;
		close(Datafile);

        pthread_mutex_unlock(&m_datafile);

        // savedataflag = 0;

    }
    else
    {
        status = 2;
    }
    return  status;
}

/*
*************************************************************************************************************
- 函数名称 : char WaveCOmPc(unsigned char *Date)
- 函数说明 : 发送自动上传数据
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
char WaveCOmPc_Zhujian(unsigned char *Date)
{
    char status;
    unsigned char Scheme[300];
    unsigned char i,ch;
    unsigned char sendOut[300];
    unsigned char AusendOut[300];
    unsigned char  Len = 74;

    memset(Scheme,0,sizeof(Scheme));
    Scheme[0] = 0x37;
    Scheme[1] = 0x00;
    memcpy(Scheme+2,Date,72);

    memset(sendOut,0,sizeof(sendOut));
    sendOut[0] = ST_RX;
    sendOut[1] = Len;
    ch =  Scheme[0];
    for(i = 1; i < Len; i++)
    {
        ch ^= 	Scheme[i];
    }
    sendOut[2] = ch;
    memcpy(sendOut + 3,Scheme,Len);
    sendOut[i + 3] = ST_END;

    memset(Scheme,0,sizeof(Scheme));
    hex_2_ascii(sendOut,Scheme,sendOut[1] + 4);
    memset(AusendOut,0,sizeof(AusendOut));

    sprintf(G_STARTW,"TT%02X%02X%02X",IDaddr.longbuf[0],IDaddr.longbuf[1],IDaddr.longbuf[2]);
    strcat(AusendOut,G_STARTW);
    strcat(AusendOut,Scheme);

#if GPRSPR
    printf("AUTO Send = %s\n",AusendOut);
#endif
    status = write_datas_gprs(sockfd,AusendOut,strlen(AusendOut));
    return  status;

}

/*
**************************************************************
- 函数名称 : unsigned char ReadEightDat (unsigned char *StartDat)
- 函数说明 :完全采集   每次发送8条数据
- 入口参数： 数据
- 输出参数 :
**************************************************************
*/
unsigned char ReadEightDat_Zhujian (unsigned char *StartDat)
{
    int result;
    unsigned int i,Saddr,nu;

    memset(SendeghitDat,0xff,sizeof(SendeghitDat));
    memcpy(LBuf.longbuf,StartDat,4);

    if(SaveNumBs.i >= CodeNum.i)
    {
        //while(savedataflag);
        //    savedataflag =1;
        pthread_mutex_lock(&m_datafile);

		Datafile = open(OFF_LINE_CONSUM_FILE,O_SYNC|O_RDWR);

        Saddr = (LBuf.i-1)*72;
        if(SaveNumBs.i >= (LBuf.i + 8))
        {
            for(i = 0; i<8; i++)
            {

				result = lseek(Datafile,Saddr+i*72, SEEK_SET);
				result = read(Datafile,SendeghitDat+i*72,72);

			
            }
            // status = 0;

			close(Datafile);

            pthread_mutex_unlock(&m_datafile);
            //  savedataflag = 0;
            return 0;

        }
        else
        {
            nu = SaveNumBs.i - LBuf.i;
            for(i = 0; i< nu; i++)
            {

				result = lseek(Datafile,Saddr+i*72, SEEK_SET);
				result = read(Datafile,SendeghitDat+i*72,72);
		
            }            

			close(Datafile);            
            pthread_mutex_unlock(&m_datafile);
            return 0;
        }
    }
    else
    {
        return -1;
    }
}

#endif
