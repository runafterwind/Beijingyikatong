#include <apparel.h>

#ifndef CANGNAN_BUS
#include "RC500.h"
#include "InitSystem.h"
#include "OnlineRecharge.h"
#include "cpucard.h"
#include "../bzdes/stades.h"
#include "../bzdes/mac.h"
#include "../Display/fbtools.h"
#include "../sound/sound.h"


#define   APP_PRINTF    1
#define   KEYTEST       0

/***命令数组***/
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

unsigned char GetChall8[] = {0x00,0x88,0x0,0x01,0x08};
//取随机数 8

unsigned char QuerrChas[]= {0x80,0x5c,0x00,0x02,0x04};
//查询余额

unsigned char ReadFile04[]={0x00,0xb0,0x84,0x00,0x3c};
//读取住建部0004文件

unsigned char ReadFiley05[]={0x00,0xb0,0x85,0x00,0x20};
//读取住建部用户卡0005文件

unsigned char ReadFileg05[]={0x00,0xb0,0x85,0x00,0x32};
//读取住建部管理卡0005文件

unsigned char ReadFile14[]={0x00,0xb0,0x94,0x00,0x18};
//读取住建部用户卡0014文件 

unsigned char PPSE[]={"2PAY.SYS.DDF01"};
//交通部应用支付环境

unsigned char localissuerlabel[]={0x01,0x01,0x10,0x00,0xff,0xff,0xff,0xff};
//本地发卡机构标识

unsigned char PrivateAID[]={0x91,0x56,0x00,0x00,0x14,0x4d,0x4f,0x54,0x2e,0x42,0x4d,0x41,0x43,0x30,0x30,0x31};
//本地私有应用

unsigned char TunionAID[]= {0xA0,0x00,0x00,0x00,0x06,0x32,0x01,0x01,0x05};
//交通部AID


/***变量定义***/
CpuMoney cpu_money;
unsigned char use_jiaotong_stander;
CardInformCPU CardLanCPU;
LongUnon Buf1,DevSIDCPU,Valueq;
FLC0005 flc0005;
FLC0006 flc0006;
FLC0007 flc0007;
FLC0008 flc0008;
FLC0009 flc0009;
FileM4 filem4;
FileM5 filem5;
FileMP filemp;

CardRate_local localcard[13];
CardRate_remot remotcard[13];


/***外变量定义***/
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
unsigned char SRCPUCardBuf[1024];
static LongUnon OldCash,OldCi;
static int  receive_len[1] = {0};
static char receive_buf[1024]= {0};
extern ShortUnon Infor;
extern Interval  retdata;	//时间间隔
extern pthread_mutex_t m_Blacklist;
extern unsigned char PsamKeyIndex;	
extern unsigned char bStolenDis;
extern unsigned int StolenAmount;
extern YanZhouCard Yanzhou_Card;
extern unsigned char Tunion;
extern unsigned char PsamNum_bak2[6];
extern unsigned char *CardLanBuf;

unsigned short Union_Card[]=
{
	0x3630,0x1000,0x3910,0x3970,0x3930,0x3950,0x3990,0x3940,
	0x4010,0x4050,0x4030,0x3918,0x1210,0x1270,0x1210,0x1340,
	0x1380,0x1460,0x1240,0x1260,0x1410,0x1430,0x1480,0x1310,
	0x1270,0x2410,0x2420,0x2411,0x2450,0x3010,0x3120,0x3140,
	0x3020,0x3160,0x3040,0x3060,0x3050,0x3180,0x3110,0x3080,
	0x3030,0x3070,0x1920,0x4540,0x4730,0x7950,0x1121,0x7410,
	0x7510,0x7360,0x7380,0x7530,0x7340,0x7450,0x7430,0x7470,
	0x7580,0x7490,0x7570,0x7550,0x7540,0x7560,0x3350,0x6900,
	0x3250,0x1001,0x2720,0x8040,0x8740
};




unsigned char SelectAppDF(char *DFname,char *Recvdata,unsigned  char *namelen);


void menu_print(char *buffer, int length)
{
	int i;

	for (i = 0; i < length; i++)
	{
		DBG_DATA_PRINTF("%02X ", *(buffer + i));
		if ((i + 1) % 8 == 0) DBG_DATA_PRINTF(" ");
		if ((i + 1) % 16 == 0) DBG_DATA_PRINTF("\n");
	}
	DBG_DATA_PRINTF("\n");
}


extern CardInformCPU CardLanCPU;
unsigned char IsOverTime_Cpu_jiaotong(void)
{
	unsigned char statusFlag = 0;
	unsigned char status;
	unsigned char buff[6];
	
		
		
			memcpy(buff,&CardLanCPU.tradedate[1],3);
			memcpy(&buff[3],CardLanCPU.tradetime,3);
			status = Date_MinLooK(buff, (unsigned char *)&Time);	
	
		if(status == MI_OK)
		{
			memcpy(Infor.intbuf,CardLanCPU.gbupiaolimittime,2);
			printf("------ time gap is : %d ------\n", Infor.i);
			if(((retdata.date*1440) + retdata.min) > Infor.i)		//大于时间间隔
			{
				statusFlag = 1;
			}
		}
		return statusFlag;

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


//SectionSheet
unsigned char IsOverTime_Cpu_zhujian(void)
{
	unsigned char statusFlag = 0;
	unsigned char status;
	unsigned char buff[6];
	ShortUnon Infor;
		
		
			memcpy(buff,&CardLanCPU.ygetontime[0],5);			
			status = Date_MinLooK(buff, (unsigned char *)&Time);		
	
		if(status == MI_OK)
		{
			memcpy(Infor.intbuf,CardLanCPU.gbupiaolimittime,2);
			printf("------ time gap is : %d ------\n", Infor.i);
			if(((retdata.date*1440) + retdata.min) > Infor.i)		//大于时间间隔
			{
				statusFlag = 1;
			}
		}
		return statusFlag;

}
#if 0
unsigned char IsGetOnOff()	//上下车判断
{
	unsigned char ret;
	unsigned char buff1[20],buff2[20];

	printf("1.前后门定义Section.Updoor=%d\n",Section.Updoor);
    bStolenDis = 0;
	switch(Section.Updoor)
	{
		case 1://前门(只上车)
			if(CardLanCPU.tradestate == 0x01)
			{
				//判断是否是同一辆车
				memset(buff1,0,sizeof(buff1));
				memset(buff2,0,sizeof(buff2));
				sprintf(buff1,"%016d",DevNum.i);
            	sprintf(buff2,"%02x%02x%02x%02x%02x%02x%02x%02x",CardLanCPU.getondevice[0],CardLanCPU.getondevice[1],
					CardLanCPU.getondevice[2],CardLanCPU.getondevice[3],CardLanCPU.getondevice[4],CardLanCPU.getondevice[5],
					CardLanCPU.getondevice[6],CardLanCPU.getondevice[7]);	
			    if(memcmp(&buff1[9],&buff2[9],7)==0)
			{
					if(IsOverTime_Cpu_jiaotong())//判断有没有超时逃票情况
					{
						bStolenDis = 1;//逃票了
						StolenAmount = CardLanCPU.markamount[0]<<24|CardLanCPU.markamount[1]<<16|CardLanCPU.markamount[2]<<8|CardLanCPU.markamount[3];
						HostValue.i = StolenAmount;
					}
				}
				else
				{
					bStolenDis = 1;//逃票了
					StolenAmount = CardLanCPU.markamount[0]<<24|CardLanCPU.markamount[1]<<16|CardLanCPU.markamount[2]<<8|CardLanCPU.markamount[3];
					HostValue.i = StolenAmount;
				}
				CardLanCPU.tradestate = 0x02;
			}
			break;
		
		case 2://后门(只下车)(1.正常下车 2.同一车上车了但没下车 3.别的车上车此车下车)
			if(CardLanCPU.tradestate == 0x00 || CardLanCPU.tradestate == 0x02) 
			{
				Err_display(44);  //没有上车
				return 1;
			}
			else
			{
				//判断是否是同一辆车
				memset(buff1,0,sizeof(buff1));
				memset(buff2,0,sizeof(buff2));
				sprintf(buff1,"%016d",DevNum.i);
				sprintf(buff2,"%02x%02x%02x%02x%02x%02x%02x%02x",CardLanCPU.getondevice[0],CardLanCPU.getondevice[1],
					CardLanCPU.getondevice[2],CardLanCPU.getondevice[3],CardLanCPU.getondevice[4],CardLanCPU.getondevice[5],
					CardLanCPU.getondevice[6],CardLanCPU.getondevice[7]);
				if(memcmp(&buff1[9],&buff2[9],7)==0)
				{
					if(IsOverTime_Cpu_jiaotong())
					{
						bStolenDis = 1;//逃票了
						StolenAmount = CardLanCPU.markamount[0]<<24|CardLanCPU.markamount[1]<<16|CardLanCPU.markamount[2]<<8|CardLanCPU.markamount[3];
					}
				}
				else
				{
					bStolenDis = 1;//逃票了
					StolenAmount = CardLanCPU.markamount[0]<<24|CardLanCPU.markamount[1]<<16|CardLanCPU.markamount[2]<<8|CardLanCPU.markamount[3];
				}
			}
			break;

		default://同门上下车
			if(CardLanCPU.tradestate == 0x01)
			{
				//判断是否是同一辆车
				memset(buff1,0,sizeof(buff1));
				memset(buff2,0,sizeof(buff2));
				sprintf(buff1,"%016d",DevNum.i);
				sprintf(buff2,"%02x%02x%02x%02x%02x%02x%02x%02x",CardLanCPU.getondevice[0],CardLanCPU.getondevice[1],
					CardLanCPU.getondevice[2],CardLanCPU.getondevice[3],CardLanCPU.getondevice[4],CardLanCPU.getondevice[5],
					CardLanCPU.getondevice[6],CardLanCPU.getondevice[7]);
				printf("\nbuff1=%s\nbuff2=%s\n",buff1,buff2);
				if(memcmp(&buff1[8],&buff2[8],8)==0)
				{
					if(IsOverTime_Cpu_jiaotong())
					{
						bStolenDis = 1;//逃票了
						StolenAmount = CardLanCPU.markamount[0]<<24|CardLanCPU.markamount[1]<<16|CardLanCPU.markamount[2]<<8|CardLanCPU.markamount[3];
						HostValue.i = StolenAmount;
						CardLanCPU.tradestate = 0x02;
						printf("\n1.StolenAmount=%d\n",StolenAmount);
					}
				}
				else
				{
					bStolenDis = 1;//逃票了
					StolenAmount = CardLanCPU.markamount[0]<<24|CardLanCPU.markamount[1]<<16|CardLanCPU.markamount[2]<<8|CardLanCPU.markamount[3];
					HostValue.i = StolenAmount;
					CardLanCPU.tradestate = 0x02;
					printf("\n2.StolenAmount=%d\n",StolenAmount);
				}
			}
			break;	
	}

	 if(StolenAmount>5000)
    {
        StolenAmount = 1;
        HostValue.i = 1;
    }
	if(CardLanCPU.tradestate == 0x01)		                                 //代表已经上车,现在正下车
	{
		ret = SectionSheet_CPU();
		if(ret == 0)
		{
			ret = AnalysisSheet_Cpu_jiaotong(0);                            //需要重新修正该函数
			if(ret == 0)
			{
				//价格进行打折
				
	 			if(bStolenDis == 1)
					HostValue.i += StolenAmount;

				CardLanCPU.thismoney[0] = (HostValue.i&0xFF000000)>>24;
				CardLanCPU.thismoney[1] = (HostValue.i&0xFF0000)>>16;
				CardLanCPU.thismoney[2] = (HostValue.i&0xFF00)>>8;
				CardLanCPU.thismoney[3] = (HostValue.i&0xFF)>>0;            //本次交易金额
				//下车流程
				CardLanCPU.enterexitflag = 0x00;                            //下车
				CardLanCPU.tradestate = 0x02;                               //交易状态，代表已经下车
				if(Section.Updown  == 0x00)
					CardLanCPU.directionflag = 0xAB;                        //上行
				else
					CardLanCPU.directionflag = 0xBA;                        //下行
				
				memcpy(CardLanCPU.getoffcitycode,Yanzhou_Card.CityCode,2);  //下车城市代码
				memcpy(CardLanCPU.getoffissuerlabel,Yanzhou_Card.IssuerLabel,8);//下车机构标示
				memcpy(CardLanCPU.getoffoperatorcode,Yanzhou_Card.OperatorCode,2);//下车运营商代码
				memcpy(CardLanCPU.getoffline,CardLanCPU.glinenum,2);        //下车线路号
				
				CardLanCPU.getoffstation = Section.SationNow;               //下车站点
				memcpy(CardLanCPU.getoffbus+2,PsamNum_bak2,6);              //下车车辆号
				
				memset(buff1,0,sizeof(buff1));
				sprintf(buff1,"%016d",DevNum.i);
				ascii_2_hex(buff1, CardLanCPU.getoffdevice, 16);            //下车终端编号

				//memcpy(CardLanCPU.getoffdevice,PsamNum,6);
				//CardLanCPU.getoffdevice[6] = 0xFF;
				//CardLanCPU.getoffdevice[7] = 0xFF;//下车终端编号
				CardLanCPU.getofftime[0] = 0x20;
				CardLanCPU.getofftime[1] = Time.year;
				CardLanCPU.getofftime[2] = Time.month;
				CardLanCPU.getofftime[3] = Time.day;
				CardLanCPU.getofftime[4] = Time.hour;
				CardLanCPU.getofftime[5] = Time.min;
				CardLanCPU.getofftime[6] = Time.sec;//下车时间
				memcpy(CardLanCPU.tradeamount,CardLanCPU.thismoney,4);      //交易金额
				memcpy(CardLanCPU.ridedistance,"\x00\x00",2);               //乘车里程       //待修改
			}
			else
			{
				Err_display(16);
			}
		}
		else
		{
			Err_display(16);
		}
	}
	else		//代表刚上车
	{
		CardLanCPU.enterexitflag = 0x55;                                    //上车
		CardLanCPU.tradestate = 0x01;                                       //交易状态，代表已经上车
		if(Section.Updown  == 0x00)
			CardLanCPU.directionflag = 0xAB;                                //上行
		else
			CardLanCPU.directionflag = 0xBA;                                //下行

	//与扬州卡有关的信息已被屏蔽
		memcpy(CardLanCPU.getoncitycode,Yanzhou_Card.CityCode,2);           //上车城市代码
		memcpy(CardLanCPU.getonissuerlabel,Yanzhou_Card.IssuerLabel,8);     //上车机构标示
		memcpy(CardLanCPU.getonoperatorcode,Yanzhou_Card.OperatorCode,2);   //上车运营商代码
		memcpy(CardLanCPU.getonline,CardLanCPU.glinenum,2);                 //上车线路号
		
		CardLanCPU.getonstation = Section.SationNow;                        //上车站点
		memcpy(CardLanCPU.getonbus+2,PsamNum_bak2,6);                       //上车车辆号

		memset(buff1,0,sizeof(buff1));
		sprintf(buff1,"%016d",DevNum.i);
		ascii_2_hex(buff1, CardLanCPU.getondevice, 16);                     //上车终端编号
		printf("CardLanCPU.getondevice=%02x%02x%02x%02x%02x%02x%02x%02x\n",CardLanCPU.getondevice[0],CardLanCPU.getondevice[1],
			CardLanCPU.getondevice[2],CardLanCPU.getondevice[3],CardLanCPU.getondevice[4],CardLanCPU.getondevice[5],CardLanCPU.getondevice[6],
			CardLanCPU.getondevice[7]);
		printf("HostValue.i 0= %d\n",HostValue.i);
		//memcpy(CardLanCPU.getondevice,PsamNum,6);
		//CardLanCPU.getondevice[6] = 0xFF;
		//CardLanCPU.getondevice[7] = 0xFF;//上车终端编号
		CardLanCPU.getontime[0] = 0x20;
		CardLanCPU.getontime[1] = Time.year;
		CardLanCPU.getontime[2] = Time.month;
		CardLanCPU.getontime[3] = Time.day;
		CardLanCPU.getontime[4] = Time.hour;
		CardLanCPU.getontime[5] = Time.min;
		CardLanCPU.getontime[6] = Time.sec;//上车时间

		//填充用于套票追踪的金额信息
		if(Section.Updown)
		{
			CardLanCPU.markamount[0] = Sectionup.DeductMoney[3];
			CardLanCPU.markamount[1] = Sectionup.DeductMoney[2];
			CardLanCPU.markamount[2] = Sectionup.DeductMoney[1];
			CardLanCPU.markamount[3] = Sectionup.DeductMoney[0];//标注金额
		}
		else
		{
			CardLanCPU.markamount[0] = Section.DeductMoney[3];
			CardLanCPU.markamount[1] = Section.DeductMoney[2];
			CardLanCPU.markamount[2] = Section.DeductMoney[1];
			CardLanCPU.markamount[3] = Section.DeductMoney[0];//标注金额
		}
		
		if(bStolenDis == 1)
		{
			CardLanCPU.thismoney[0] = (HostValue.i&0xFF000000)>>24;
			CardLanCPU.thismoney[1] = (HostValue.i&0xFF0000)>>16;
			CardLanCPU.thismoney[2] = (HostValue.i&0xFF00)>>8;
			CardLanCPU.thismoney[3] = (HostValue.i&0xFF)>>0;//本次交易金额
		}
		else
		{
			HostValue.i = 0;
			DecValue.i = 0;
			memcpy(CardLanCPU.thismoney,"\x00\x00\x00\x00",4);//本次交易金额
		}
        printf("HostValue.i 1= %d\n",HostValue.i);
		ret = 0;
	}

	return ret;
}
#endif
unsigned char IsGetOnOff()	//上下车判断
{
	unsigned char ret;
	unsigned char buff1[20],buff2[20];

	printf("1.前后门定义Section.Updoor=%d\n",Section.Updoor);
    bStolenDis = 0;
	switch(Section.Updoor)
	{
		case 1://前门(只上车)
			if(CardLanCPU.tradestate == 0x01)
			{
				//判断是否是同一辆车
				memset(buff1,0,sizeof(buff1));
				memset(buff2,0,sizeof(buff2));			
			    sprintf(buff1,"%02x%02x%02x%02x%02x%02x",PsamNum_bak2[0],PsamNum_bak2[1],PsamNum_bak2[2],PsamNum_bak2[3],PsamNum_bak2[4],PsamNum_bak2[5]);
				sprintf(buff2,"%02x%02x%02x%02x%02x%02x",CardLanCPU.getondevice[2],CardLanCPU.getondevice[3],CardLanCPU.getondevice[4],CardLanCPU.getondevice[5],
					CardLanCPU.getondevice[6],CardLanCPU.getondevice[7]);
				if((memcmp(&buff1[0],&buff2[0],12)!=0)&&(Section.Updown==CardLanCPU.ydirectionflag)&&
                    (memcpy(CardLanCPU.glinenum,CardLanCPU.ygetonlinenum,2)))
				{
					if(IsOverTime_Cpu_jiaotong())//判断有没有超时逃票情况
					{
						bStolenDis = 1;//逃票了
						StolenAmount = CardLanCPU.markamount[0]<<24|CardLanCPU.markamount[1]<<16|CardLanCPU.markamount[2]<<8|CardLanCPU.markamount[3];
						HostValue.i = StolenAmount;
					}
				}				
                else if((Section.Updown!=CardLanCPU.ydirectionflag)&&(memcpy(CardLanCPU.glinenum,CardLanCPU.ygetonlinenum,2))&&
                    (IsOverTime_Cpu_jiaotong()==0))   
                  { 
                    switch(CardLanCPU.gruleofupanddowm)     //判定补票规则
                    {
                        case 0x01:
                            bStolenDis = 1;//逃票了
    						StolenAmount = CardLanCPU.ymarkamount[0]<<8|CardLanCPU.ymarkamount[1];
    						HostValue.i = StolenAmount;
    						                          
                            break;
                        case 0x02:
                            bStolenDis = 2;//逃票了
						    StolenAmount = 0xff;                //需要修改成查询远端票价
						    HostValue.i = StolenAmount;
						    

                            break;

                         case 0x03:
                             bStolenDis = 2;//逃票了
						    StolenAmount = 0xff;                //需要修改成查询近端票价
						    HostValue.i = StolenAmount;
						    

                            break;
                            
                         default:
                            break;
                        }
    				
                    
                    }	
                
				CardLanCPU.tradestate = 0x02;
			}
			break;
		
		case 2://后门(只下车)(1.正常下车 2.同一车上车了但没下车 3.别的车上车此车下车)
			if(CardLanCPU.tradestate == 0x00 || CardLanCPU.tradestate == 0x02) 
			{
				Err_display(44);  //没有上车
				return 1;
			}
			else
			{
                //判断是否是同一辆车
				memset(buff1,0,sizeof(buff1));
				memset(buff2,0,sizeof(buff2));			
			    sprintf(buff1,"%02x%02x%02x%02x%02x%02x",PsamNum_bak2[0],PsamNum_bak2[1],PsamNum_bak2[2],PsamNum_bak2[3],PsamNum_bak2[4],PsamNum_bak2[5]);
				sprintf(buff2,"%02x%02x%02x%02x%02x%02x",CardLanCPU.getondevice[2],CardLanCPU.getondevice[3],CardLanCPU.getondevice[4],CardLanCPU.getondevice[5],
					CardLanCPU.getondevice[6],CardLanCPU.getondevice[7]);
				if((memcmp(&buff1[0],&buff2[0],12)!=0)&&(Section.Updown==CardLanCPU.ydirectionflag)&&
                    (memcpy(CardLanCPU.glinenum,CardLanCPU.ygetonlinenum,2)))
				{
					if(IsOverTime_Cpu_jiaotong())//判断有没有超时逃票情况
					{
						bStolenDis = 1;//逃票了
						StolenAmount = CardLanCPU.markamount[0]<<24|CardLanCPU.markamount[1]<<16|CardLanCPU.markamount[2]<<8|CardLanCPU.markamount[3];
						HostValue.i = StolenAmount;
					}
				}				
                else if((Section.Updown!=CardLanCPU.ydirectionflag)&&(memcpy(CardLanCPU.glinenum,CardLanCPU.ygetonlinenum,2))&&
                    (IsOverTime_Cpu_jiaotong()==0))   
                  { 
                    switch(CardLanCPU.gruleofupanddowm)     //判定补票规则
                    {
                        case 0x01:
                            bStolenDis = 1;//逃票了
    						StolenAmount = CardLanCPU.ymarkamount[0]<<8|CardLanCPU.ymarkamount[1];
    						HostValue.i = StolenAmount;
    						                          
                            break;
                        case 0x02:
                            bStolenDis = 2;//逃票了
						    StolenAmount = 0xff;                //需要修改成查询远端票价
						    HostValue.i = StolenAmount;
						    

                            break;

                         case 0x03:
                             bStolenDis = 2;//逃票了
						    StolenAmount = 0xff;                //需要修改成查询近端票价
						    HostValue.i = StolenAmount;
						    

                            break;
                            
                         default:
                            break;
                        }
    				
                    
                    }	
                
			}
			break;

		default://同门上下车
			if(CardLanCPU.tradestate == 0x01)
			{
				 //判断是否是同一辆车
				memset(buff1,0,sizeof(buff1));
				memset(buff2,0,sizeof(buff2));			
			    sprintf(buff1,"%02x%02x%02x%02x%02x%02x",PsamNum_bak2[0],PsamNum_bak2[1],PsamNum_bak2[2],PsamNum_bak2[3],PsamNum_bak2[4],PsamNum_bak2[5]);
				sprintf(buff2,"%02x%02x%02x%02x%02x%02x",CardLanCPU.getondevice[2],CardLanCPU.getondevice[3],CardLanCPU.getondevice[4],CardLanCPU.getondevice[5],
					CardLanCPU.getondevice[6],CardLanCPU.getondevice[7]);
				if((memcmp(&buff1[0],&buff2[0],12)!=0)&&(Section.Updown==CardLanCPU.ydirectionflag)&&
                    (memcpy(CardLanCPU.glinenum,CardLanCPU.ygetonlinenum,2)))
				{
					if(IsOverTime_Cpu_jiaotong())//判断有没有超时逃票情况
					{
						bStolenDis = 1;//逃票了
						StolenAmount = CardLanCPU.markamount[0]<<24|CardLanCPU.markamount[1]<<16|CardLanCPU.markamount[2]<<8|CardLanCPU.markamount[3];
						HostValue.i = StolenAmount;
					}
				}				
                else if((Section.Updown!=CardLanCPU.ydirectionflag)&&(memcpy(CardLanCPU.glinenum,CardLanCPU.ygetonlinenum,2))&&
                    (IsOverTime_Cpu_jiaotong()==0))   
                  { 
                    switch(CardLanCPU.gruleofupanddowm)     //判定补票规则
                    {
                        case 0x01:
                            bStolenDis = 1;//逃票了
    						StolenAmount = CardLanCPU.ymarkamount[0]<<8|CardLanCPU.ymarkamount[1];
    						HostValue.i = StolenAmount;    						                          
                            break;
                            
                        case 0x02:
                            bStolenDis = 2;//逃票了
						    StolenAmount = 0xff;                //需要修改成查询远端票价
						    HostValue.i = StolenAmount;
                            break;

                         case 0x03:
                             bStolenDis = 2;//逃票了
						    StolenAmount = 0xff;                //需要修改成查询近端票价
						    HostValue.i = StolenAmount;					    
                            break;
                            
                         default:
                            break;
                        }                    
                    }	
                    CardLanCPU.tradestate = 0x02;
			}
			break;	
	}

	 if(StolenAmount>5000)
    {
        StolenAmount = 1;
        HostValue.i = 1;
    }
	if(CardLanCPU.tradestate == 0x01)		                                 //代表已经上车,现在正下车
	{
		ret = SectionSheet_CPU();
		if(ret == 0)
		{
			ret = AnalysisSheet_Cpu_jiaotong(0);                            //需要重新修正该函数
			if(ret == 0)
			{
				//价格进行打折
				
	 			if(bStolenDis == 1)
					HostValue.i += StolenAmount;

				CardLanCPU.thismoney[0] = (HostValue.i&0xFF000000)>>24;
				CardLanCPU.thismoney[1] = (HostValue.i&0xFF0000)>>16;
				CardLanCPU.thismoney[2] = (HostValue.i&0xFF00)>>8;
				CardLanCPU.thismoney[3] = (HostValue.i&0xFF)>>0;            //本次交易金额
				//下车流程
				CardLanCPU.enterexitflag = 0x00;                            //下车
				CardLanCPU.tradestate = 0x02;                               //交易状态，代表已经下车
				if(Section.Updown  == 0x00)
					CardLanCPU.directionflag = 0xAB;                        //上行
				else
					CardLanCPU.directionflag = 0xBA;                        //下行
				
				memcpy(CardLanCPU.getoffcitycode,Yanzhou_Card.CityCode,2);  //下车城市代码
				memcpy(CardLanCPU.getoffissuerlabel,Yanzhou_Card.IssuerLabel,8);//下车机构标示
				memcpy(CardLanCPU.getoffoperatorcode,Yanzhou_Card.OperatorCode,2);//下车运营商代码
				memcpy(CardLanCPU.getoffline,CardLanCPU.glinenum,2);        //下车线路号
				
				CardLanCPU.getoffstation = Section.SationNow;               //下车站点
				memcpy(CardLanCPU.getoffbus+2,PsamNum_bak2,6);              //下车车辆号
				
				memset(buff1,0,sizeof(buff1));
				sprintf(buff1,"%016d",DevNum.i);
				ascii_2_hex(buff1, CardLanCPU.getoffdevice, 16);            //下车终端编号

				//memcpy(CardLanCPU.getoffdevice,PsamNum,6);
				//CardLanCPU.getoffdevice[6] = 0xFF;
				//CardLanCPU.getoffdevice[7] = 0xFF;//下车终端编号
				CardLanCPU.getofftime[0] = 0x20;
				CardLanCPU.getofftime[1] = Time.year;
				CardLanCPU.getofftime[2] = Time.month;
				CardLanCPU.getofftime[3] = Time.day;
				CardLanCPU.getofftime[4] = Time.hour;
				CardLanCPU.getofftime[5] = Time.min;
				CardLanCPU.getofftime[6] = Time.sec;//下车时间
				memcpy(CardLanCPU.tradeamount,CardLanCPU.thismoney,4);      //交易金额
				memcpy(CardLanCPU.ridedistance,"\x00\x00",2);               //乘车里程       //待修改
			}
			else
			{
				Err_display(16);
			}
		}
		else
		{
			Err_display(16);
		}
	}
	else		//代表刚上车
	{
		CardLanCPU.enterexitflag = 0x55;                                    //上车
		CardLanCPU.tradestate = 0x01;                                       //交易状态，代表已经上车
		if(Section.Updown  == 0x00)
			CardLanCPU.directionflag = 0xAB;                                //上行
		else
			CardLanCPU.directionflag = 0xBA;                                //下行

	//与扬州卡有关的信息已被屏蔽
		memcpy(CardLanCPU.getoncitycode,Yanzhou_Card.CityCode,2);           //上车城市代码
		memcpy(CardLanCPU.getonissuerlabel,Yanzhou_Card.IssuerLabel,8);     //上车机构标示
		memcpy(CardLanCPU.getonoperatorcode,Yanzhou_Card.OperatorCode,2);   //上车运营商代码
		memcpy(CardLanCPU.getonline,CardLanCPU.glinenum,2);                 //上车线路号
		
		CardLanCPU.getonstation = Section.SationNow;                        //上车站点
		memcpy(CardLanCPU.getonbus+2,PsamNum_bak2,6);                       //上车车辆号

		memset(buff1,0,sizeof(buff1));
		sprintf(buff1,"%016d",DevNum.i);
		ascii_2_hex(buff1, CardLanCPU.getondevice, 16);                     //上车终端编号
		printf("CardLanCPU.getondevice=%02x%02x%02x%02x%02x%02x%02x%02x\n",CardLanCPU.getondevice[0],CardLanCPU.getondevice[1],
			CardLanCPU.getondevice[2],CardLanCPU.getondevice[3],CardLanCPU.getondevice[4],CardLanCPU.getondevice[5],CardLanCPU.getondevice[6],
			CardLanCPU.getondevice[7]);
		printf("HostValue.i 0= %d\n",HostValue.i);
		//memcpy(CardLanCPU.getondevice,PsamNum,6);
		//CardLanCPU.getondevice[6] = 0xFF;
		//CardLanCPU.getondevice[7] = 0xFF;//上车终端编号
		CardLanCPU.getontime[0] = 0x20;
		CardLanCPU.getontime[1] = Time.year;
		CardLanCPU.getontime[2] = Time.month;
		CardLanCPU.getontime[3] = Time.day;
		CardLanCPU.getontime[4] = Time.hour;
		CardLanCPU.getontime[5] = Time.min;
		CardLanCPU.getontime[6] = Time.sec;//上车时间

		//填充用于套票追踪的金额信息
		if(Section.Updown)
		{
			CardLanCPU.markamount[0] = Sectionup.DeductMoney[3];
			CardLanCPU.markamount[1] = Sectionup.DeductMoney[2];
			CardLanCPU.markamount[2] = Sectionup.DeductMoney[1];
			CardLanCPU.markamount[3] = Sectionup.DeductMoney[0];//标注金额
		}
		else
		{
			CardLanCPU.markamount[0] = Section.DeductMoney[3];
			CardLanCPU.markamount[1] = Section.DeductMoney[2];
			CardLanCPU.markamount[2] = Section.DeductMoney[1];
			CardLanCPU.markamount[3] = Section.DeductMoney[0];//标注金额
		}
		
		if(bStolenDis == 1)
		{
			CardLanCPU.thismoney[0] = (HostValue.i&0xFF000000)>>24;
			CardLanCPU.thismoney[1] = (HostValue.i&0xFF0000)>>16;
			CardLanCPU.thismoney[2] = (HostValue.i&0xFF00)>>8;
			CardLanCPU.thismoney[3] = (HostValue.i&0xFF)>>0;//本次交易金额
		}
		else
		{
			HostValue.i = 0;
			DecValue.i = 0;
			memcpy(CardLanCPU.thismoney,"\x00\x00\x00\x00",4);//本次交易金额
		}
        printf("HostValue.i 1= %d\n",HostValue.i);
		ret = 0;
	}

	return ret;
}


unsigned char IsGetOnOff_zhujian()	//上下车判断
{
	unsigned char ret;
	unsigned char buff1[20],buff2[20];
    LongUnon ci;

	printf("1.前后门定义Section.Updoor=%d\n",Section.Updoor);
    bStolenDis = 0;
    Section.Updoor= 0xff;
	switch(Section.Updoor)
	{
		case 1:
        case 2:
            break;

		default://同门上下车
			if(CardLanCPU.ytradestatus == 0x20)
			{                
				//判断是否是同一辆车
				memset(buff1,0,sizeof(buff1));
				memset(buff2,0,sizeof(buff2));
				sprintf(buff1,"%02x%02x%02x",PsamNum_bak2[0],PsamNum_bak2[1],PsamNum_bak2[2]&0xf0);
				sprintf(buff2,"%02x%02x%02x",CardLanCPU.ygetonvehiclnum[0],CardLanCPU.ygetonvehiclnum[1],CardLanCPU.ygetonvehiclnum[2]);
                printf("\nbuff1=%s\nbuff2=%s\n",buff1,buff2);
                //不同车辆，同方向，同线路，超时算逃票
				if((memcmp(&buff1[0],&buff2[0],3)!=0)&&(Section.Updown==CardLanCPU.ydirectionflag)&&
                    (memcpy(CardLanCPU.glinenum,CardLanCPU.ygetonlinenum,2)))                               
				{
					if(IsOverTime_Cpu_zhujian())
					{
						bStolenDis = 1;//逃票了
						StolenAmount = CardLanCPU.ymarkamount[0]<<8|CardLanCPU.ymarkamount[1];
						HostValue.i = StolenAmount;
						CardLanCPU.ytradestatus= 0x20;
						printf("\n1.StolenAmount=%d\n",StolenAmount);
					}
				}
                else if((Section.Updown!=CardLanCPU.ydirectionflag)&&(memcpy(CardLanCPU.glinenum,CardLanCPU.ygetonlinenum,2))&&
                    (IsOverTime_Cpu_zhujian()==0))   
                  { 
                    switch(CardLanCPU.gruleofupanddowm)     //判定补票规则
                    {
                        case 0x01:
                            bStolenDis = 1;//逃票了
    						StolenAmount = CardLanCPU.ymarkamount[0]<<8|CardLanCPU.ymarkamount[1];
    						HostValue.i = StolenAmount;
    						CardLanCPU.ytradestatus= 0x20;                            
                            break;
                        case 0x02:
                            bStolenDis = 2;//逃票了
						    StolenAmount = 0xff;                //需要修改成查询远端票价
						    HostValue.i = StolenAmount;
						    CardLanCPU.ytradestatus= 0x20;

                            break;

                         case 0x03:
                             bStolenDis = 2;//逃票了
						    StolenAmount = 0xff;                //需要修改成查询近端票价
						    HostValue.i = StolenAmount;
						    CardLanCPU.ytradestatus= 0x20;

                            break;
                            
                         default:
                            break;
                        }
    				
                    
                    }			
			
				
			}
			break;	
	}

	
	if(CardLanCPU.ytradestatus == 0x20)		//代表已经上车,现在正下车
	{
		ret = SectionSheet_CPU();
		if(ret == 0)
		{
			ret = AnalysisSheet_Cpu_jiaotong(0);           //查询参数需要重新修改函数
			if(ret == 0)
			{
				//价格进行打折
				
	 			if(bStolenDis == 1)
					HostValue.i += StolenAmount;

				CardLanCPU.thismoney[0] = (HostValue.i&0xFF000000)>>24;
				CardLanCPU.thismoney[1] = (HostValue.i&0xFF0000)>>16;
				CardLanCPU.thismoney[2] = (HostValue.i&0xFF00)>>8;
				CardLanCPU.thismoney[3] = (HostValue.i&0xFF)>>0;//本次交易金额
				//下车流程
				//组织用于更新0005文件的数据
				memcpy(ci.longbuf,CardLanCPU.ycardtransnum,4);
                ci.i++;
                memcpy(&CardLanCPU.ydata0005[1],ci.longbuf,4);              //卡累计交易次数+1
				CardLanCPU.ydata0005[9] = 0x30;                             //修改上下车状态 下车
                CardLanCPU.ydata0005[10]=Time.year;                         //本交交易时间
                CardLanCPU.ydata0005[11]=Time.month;
                CardLanCPU.ydata0005[12]=Time.day;
                CardLanCPU.ydata0005[13]=Time.hour;
                CardLanCPU.ydata0005[14]=Time.min;
                CardLanCPU.ydata0005[15]=HEX2BCD(PsamNum_bak2[0]);
                CardLanCPU.ydata0005[16]=HEX2BCD(PsamNum_bak2[1]);
                CardLanCPU.ydata0005[17]=HEX2BCD(PsamNum_bak2[2])&0xf0;     //更新本次运营商代码
                
			}
			else
			{
				Err_display(16);
			}
		}
		else
		{
			Err_display(16);
		}
	}
	else		//代表刚上车
	{
        //组织用于更新0005文件的数据
		memcpy(ci.longbuf,CardLanCPU.ycardtransnum,4);
        ci.i++;
        memcpy(&CardLanCPU.ydata0005[1],ci.longbuf,4);              //卡累计交易次数+1
		CardLanCPU.ydata0005[9] = 0x20;                             //修改上下车状态  上车
        CardLanCPU.ydata0005[10]=Time.year;                         //本交交易时间
        CardLanCPU.ydata0005[11]=Time.month;
        CardLanCPU.ydata0005[12]=Time.day;
        CardLanCPU.ydata0005[13]=Time.hour;
        CardLanCPU.ydata0005[14]=Time.min;
        CardLanCPU.ydata0005[15]=HEX2BCD(PsamNum_bak2[0]);
        CardLanCPU.ydata0005[16]=HEX2BCD(PsamNum_bak2[1]);
        CardLanCPU.ydata0005[17]=HEX2BCD(PsamNum_bak2[2])&0xf0;     //更新本次运营商代码

        //组织用于更新0014文件的数据
		CardLanCPU.ygetontime[1] = Time.year;                       //上车时间
		CardLanCPU.ygetontime[2] = Time.month;                      
		CardLanCPU.ygetontime[3] = Time.day;
		CardLanCPU.ygetontime[4] = Time.hour;
		CardLanCPU.ygetontime[5] = Time.min;
        CardLanCPU.ygetonstation = Section.SationNow;               //上车站点
        if(Section.Updown  == 0x00)                                 //方向标识
			CardLanCPU.ydirectionflag = 0xAB;                       //上行
		else
			CardLanCPU.ydirectionflag = 0xBA;                       //下行
		CardLanCPU.ymarkamount[0] = Sectionup.DeductMoney[1];       //标注金额
		CardLanCPU.ymarkamount[1] = Sectionup.DeductMoney[0];
        memcpy(CardLanCPU.ygetonlinenum,CardLanCPU.glinenum,2);     //上车线路号    
        CardLanCPU.ygetonvehiclnum[0]=HEX2BCD(PsamNum_bak2[0]);
        CardLanCPU.ygetonvehiclnum[1]=HEX2BCD(PsamNum_bak2[1]);
        CardLanCPU.ygetonvehiclnum[2]=HEX2BCD(PsamNum_bak2[2])&0xf0;     //上车车辆号	
        	
#if APP_PRINTF
         int i;
		printf("预扣金额:");
       
                    for(i =0 ; i < 2; i ++)
					{
						printf("%02X ",CardLanCPU.ymarkamount[i]);
					}
					printf("\n"); 
        printf("参数预扣金额:");
       
                    for(i =0 ; i < 2; i ++)
					{
						printf("%02X ",Sectionup.DeductMoney[i]);
					}
					printf("\n");  
#endif                    
		if(bStolenDis == 1)
		{
			CardLanCPU.thismoney[0] = (HostValue.i&0xFF000000)>>24;
			CardLanCPU.thismoney[1] = (HostValue.i&0xFF0000)>>16;
			CardLanCPU.thismoney[2] = (HostValue.i&0xFF00)>>8;
			CardLanCPU.thismoney[3] = (HostValue.i&0xFF)>>0;//本次交易金额
		}
		else
		{
			HostValue.i = 0;
			DecValue.i = 0;
		//	memcpy(CardLan.CAppCash,"\x00\x00\x00\x00",4);//本次交易金额
		}
        printf("HostValue.i 1= %d\n",HostValue.i);
		ret = 0;
	}

	return ret;
}



unsigned char CheckCityUnion(void)
{

	unsigned char status=1;
	unsigned char i;
	unsigned short CityCode;

	CityCode = CardLanCPU.citycode[0]<<8|CardLanCPU.citycode[1];
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

//防插拔处理
unsigned char SecondDebitChas_jiaotong(unsigned int Money,char *Key,unsigned char Keylen)
{
		int result;
		unsigned short ic;
		unsigned char Timebuf[8];
		unsigned char Send[256],Recv[256];
		unsigned char flag,t,status,len,Qflag;
        CardInformCPU tempbuf;
        unsigned int beforeMoney;
        
        DBG_DATA_PRINTF("初始化后的ATC3:%02x %02x\n",CardLanCPU.offlineSN[0],CardLanCPU.offlineSN[1]);
        memcpy(&tempbuf.IsLocalCard,&CardLanCPU.IsLocalCard,sizeof(CardInformCPU));             //备份交通部卡信息
        
		LEDL(1);
		beepopen(10);
		PlayMusic(15, 0);
	
		SetColor(Mcolor);
		SetTextColor(Color_red);
        #ifdef NEW0409
		SetTextSize(48);
		TextOut(0 , 75, "温馨提示");
		TextOut(0 , 144, "刷卡错误");
		TextOut(0	, 210,"请重新刷卡");
        #else
		SetTextSize(32);
		TextOut(100 , 50, "温馨提示");
		TextOut(100 , 90, "刷卡错误");
		TextOut(85	, 130,"请重新刷卡");
        #endif
		flag = 1;
		t = 1;
		Qflag = 1;
		ic = 0;
	while(Qflag)
	{
		Rd_time (Timebuf+1);
		if(Time.sec != Timebuf[6])
		{
			ic++;
			if(ic>30)
			{
				Qflag =0;
				break;
			}
			if((ic%2))
				beepopen(10);
			else
				beepopen(11);
			Timebuf[0] = 0x20;
			Time.year = Timebuf[1];
			Time.month = Timebuf[2];
			Time.day = Timebuf[3];
			Time.hour = Timebuf[4];
			Time.min = Timebuf[5];
			Time.sec = Timebuf[6];
		}
		
		flag = 1;
		t = 1;
		while(flag)
		{
			switch(t)
			{
			case 1://卡复位并判定是不是同一张卡
				status =  CardReset(Recv,&len,1);
				if(status == MI_OK) t++;
				else flag = 0;
				break;

			case 2://选应用目录
				len = 8;
				status = SelectAppDF("\xA0\x00\x00\x06\x32\x01\x01\x05",Recv,&len);
				if(status == MI_OK) 
					t++;
				else 
					flag = 0;
				break;
			case 3: //取交易证明
			    DBG_DATA_PRINTF("初始化后的ATC:%02x %02x\n",CardLanCPU.offlineSN[0],CardLanCPU.offlineSN[1]);
                 memcpy(&CardLanCPU.IsLocalCard,&tempbuf.IsLocalCard,sizeof(CardInformCPU)); 
				memset(Send,0,sizeof(Send));
				Send[0] = 0x80;
				Send[1] = 0x5A;
				Send[2] = 0x00; 
				Send[3] = 0x09; 
				Send[4] = 0x02;
				Send[5] = ((CardLanCPU.offlineSN[0]<<8|CardLanCPU.offlineSN[1] + 1)&0xFF00)>>8;
				Send[6] = ((CardLanCPU.offlineSN[0]<<8|CardLanCPU.offlineSN[1] + 1)&0xFF)>>0;
				Send[7] = 0x08;
                DBG_DATA_PRINTF("取ATC发送:");
		        menu_print(Send, 8);
				result = write(mf_fd,Send,8);
                DBG_DATA_PRINTF("取ATC发送结果1:%02x\n",result);
				if(result == MI_OK)
				{
					ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
					result = read(mf_fd, receive_buf, receive_len[0]);
                    DBG_DATA_PRINTF("取ATC发送结果2:%02x\n",receive_len[0]);
					if(receive_len[0] > 2)
					{
						//交易是成功的
						flag = 0;
						t = 0;
                        Qflag = 0;
						memcpy(CardLanCPU.MAC2,receive_buf,4);
						memcpy(CardLanCPU.TAC,&receive_buf[4],4);
					}
					else if(receive_len[0] == 2)
					{
                        DBG_DATA_PRINTF("取TAC返回:%02x %02x\n",receive_buf[0],receive_buf[1]);						//交易未成功，需要重新消费
						
						t++;
					}					
				}
				else flag = 0;
				break;

            case 4://复合应用消费初始化
			memset(Send,0,sizeof(Send));
			memcpy(Send,"\x80\x50\x03\x02\x0B",5); 		//命令头
			Send[5] = PsamKeyIndex; 					//密钥索引
			Send[6] = (Money&0xFF000000)>>24;
			Send[7] = (Money&0xFF0000)>>16;
			Send[8] = (Money&0xFF00)>>8;
			Send[9] = Money&0xFF;						//交易金额
			memcpy(&Send[10],PsamNum,6); 				//终端机号
			Send[16] = 0x0f; 							//Le
			len = 17;
            DBG_DATA_PRINTF("复合应用消费初始化发送:");
		    menu_print(Send, len);
			result = write(mf_fd,Send,len);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
					t++;
					DBG_DATA_PRINTF("复合应用消费初始化返回:");
					menu_print(receive_buf, receive_len[0]);
					memcpy(CardLanCPU.offlineSN,&receive_buf[4],2);
					memcpy(CardLanCPU.overdraftAmount,&receive_buf[6],3);
					CardLanCPU.keyVersion = receive_buf[9];
					CardLanCPU.arithmeticLabel = receive_buf[10];
					memcpy(CardLanCPU.PRandom,&receive_buf[11],4);
				}
				else
				{
					printf("复合应用消费初始化SW1=%02X   SW2=%02X\n",receive_buf[0],receive_buf[1]);
					flag = 0;
				}
			}
			else flag = 0;
			break;
            
			case 5://PSAM卡生成MAC1
			 DBG_DATA_PRINTF("重刷初始化后的ATC1:%02x %02x\n",CardLanCPU.offlineSN[0],CardLanCPU.offlineSN[1]);
			memset(Send,0,sizeof(Send));
			memcpy(Send,"\x80\x70\x00\x00\x24",5);			//命令头
			memcpy(&Send[5],CardLanCPU.PRandom,4);			//用户卡片随机数
			memcpy(&Send[9],CardLanCPU.offlineSN,2);			//用户卡脱机交易序号
			Send[11] = (Money&0xFF000000)>>24;
			Send[12] = (Money&0xFF0000)>>16;
			Send[13] = (Money&0xFF00)>>8;
			Send[14] = Money&0xFF;							//交易金额
			Send[15] = 0x09;								//交易类型
			memcpy(&Send[16],Timebuf,7);					//交易日期时间
			Send[23] = CardLanCPU.keyVersion;					//密钥版本号
			Send[24] = CardLanCPU.arithmeticLabel;				//密钥算法标识
			memcpy(&Send[25],&CardLanCPU.appserialnumber[2],8);//用户卡号
			memcpy(&Send[33],CardLanCPU.issuerlabel,8);		//发卡机构编码
			len = 41;
			memset(Recv,0,sizeof(Recv));

            DBG_DATA_PRINTF("生成MAC1发送:");
		    menu_print(Send, len);
            status = PsamCos(Send,Recv,&len);
            
			DBG_DATA_PRINTF("生成MAC1返回:");
		    menu_print(Recv, len);
	
            if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
			{
				t++;
		//		printf("PSAM卡计算MAC1返回:");
				menu_print(Recv, len);
				memcpy(CardLanCPU.PSAMOfflineSN,Recv,4);
				memcpy(CardLanCPU.MAC1,&Recv[4],4);
			}
			else flag = 0;
		
			break;          
			
			case 6://用户卡更新复合应用记录文件
			if(Section.Enable != 0x55)
                t++;
            else
            {
    			memset(Send,0,sizeof(Send));
    			memcpy(Send,"\x80\xDC\x02\xD0\x80",5);
    			memcpy(&Send[5],"\x27\x02\x7D\x01\x01",5);
    			
    			Send[10] = CardLanCPU.applockflag;
    			memcpy(&Send[11],CardLanCPU.tradeserialnumber,8);
    			
    			Send[19] = CardLanCPU.tradestate;
    			memcpy(&Send[20],CardLanCPU.getoncitycode,2);
    			memcpy(&Send[22],CardLanCPU.getonissuerlabel,8);
    			memcpy(&Send[30],CardLanCPU.getonoperatorcode,2);
    			memcpy(&Send[32],CardLanCPU.getonline,2);
    			
    			Send[34] = CardLanCPU.getonstation;
    			memcpy(&Send[35],CardLanCPU.getonbus,8);
    			memcpy(&Send[43],CardLanCPU.getondevice,8);
    			memcpy(&Send[51],CardLanCPU.getontime,7);
    			memcpy(&Send[58],CardLanCPU.markamount,4);
    			
    			Send[62] = CardLanCPU.directionflag;
    			memcpy(&Send[63],CardLanCPU.getoffcitycode,2);
    			memcpy(&Send[65],CardLanCPU.getoffissuerlabel,8);
    			memcpy(&Send[73],CardLanCPU.getoffoperatorcode,2);
    			memcpy(&Send[75],CardLanCPU.getoffline,2);
    			
    			Send[77] = CardLanCPU.getoffstation;
    			memcpy(&Send[78],CardLanCPU.getoffbus,8);
    			memcpy(&Send[86],CardLanCPU.getoffdevice,8);
    			memcpy(&Send[94],CardLanCPU.getofftime,7);
    			memcpy(&Send[101],CardLanCPU.tradeamount,4);
    			memcpy(&Send[105],CardLanCPU.ridedistance,2);
    			
    			len = 133;
    			result = write(mf_fd,Send,len);
                DBG_DATA_PRINTF("更新复合记录发送:");
    		    menu_print(Send, len);
    			if(result == MI_OK)
    			{
    				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
    				result = read(mf_fd, receive_buf, receive_len[0]);
    				if(receive_buf[0]==0x90 && receive_buf[1]==0x00)
    				{
    					t++;
    					DBG_DATA_PRINTF("更新复合应用数据缓存返回:");
    					menu_print(receive_buf, receive_len[0]);
    				}
    				else
    				{
    					printf("更新复合应用数据缓存SW1=%02X   SW2=%02X\n",receive_buf[0],receive_buf[1]);
    					flag = 0;
    				}
    			}
    			else flag = 0;
                }
			break;

            case 7:
                memset(Send,0,sizeof(Send));
    			memcpy(Send,"\x80\xDC\x00\xF0\x30",5);
                if(Section.Enable!=0x55)                     //交易类型
        			Send[5] = 0x06;
                else
                {
                    if(CardLanCPU.enterexitflag == 0x55)
                        Send[5] = 0x03;
                    else
                        Send[5] = 0x04;
                    }
                Send[6] = 0;
                Send[7] = 0;
                memcpy(&Send[8],PsamNum,6);                  //终端编号
                Send[14] = 0x02;                             //行业代码
                memcpy(&Send[15],Yanzhou_Card.LineNO,2);     //线路号
                if(Section.Enable!=0x55)                     //站点号
        			{
                        Send[17] = 0x00;
                        Send[18] = 0x00;
                    }
                else
                {
                    if(CardLanCPU.enterexitflag == 0x55)
                        {
                            Send[17] = 0x00;
                            Send[18] = CardLanCPU.getonstation;
                        }
                    else
                        {
                            Send[17] = 0x00;
                            Send[18] = CardLanCPU.getoffstation;
                        }
                    }
                memcpy(&Send[19],Yanzhou_Card.OperatorCode,2);
                Send[21] = 0;
                Send[22] = (Money&0xFF000000)>>24;
			    Send[23] = (Money&0xFF0000)>>16;
			    Send[24] = (Money&0xFF00)>>8;
			    Send[25] = Money&0xFF;							//交易金额
			    beforeMoney=CardLanCPU.beforemoney[0]<<24|CardLanCPU.beforemoney[1]<<16|CardLanCPU.beforemoney[2]<<8|CardLanCPU.beforemoney[3];                               					
                //交易后余额    
                Send[26] = ((beforeMoney-HostValue.i)&0xFF000000)>>24;
			    Send[27] = ((beforeMoney-HostValue.i)&0xFF0000)>>16;
			    Send[28] = ((beforeMoney-HostValue.i)&0xFF00)>>8;
			    Send[29] = ((beforeMoney-HostValue.i)&0xFF)>>0;
			    memcpy(&Send[30],Timebuf,7);                    //交易时间
			    memcpy(&Send[37],&Yanzhou_Card.CityCode[0],2);  //受理方城市代码
			    memcpy(&Send[39],&Yanzhou_Card.IssuerLabel,8);  //受理方机构标识			    
			    len = 53;
    			result = write(mf_fd,Send,len);
                DBG_DATA_PRINTF("更新交易信息记录发送:");
    		    menu_print(Send, len);
    			if(result == MI_OK)
    			{
    				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
    				result = read(mf_fd, receive_buf, receive_len[0]);
    				if(receive_buf[0]==0x90 && receive_buf[1]==0x00)
    				{
    					t++;
    					DBG_DATA_PRINTF("更新交易信息记录发送:");
    					menu_print(receive_buf, receive_len[0]);
    				}
    				else
    				{
    					printf("更新交易信息记录发送:SW1=%02X   SW2=%02X\n",receive_buf[0],receive_buf[1]);
    					flag = 0;
    				}
    			}
    			else flag = 0;
                
                
                
                break;

			case 8://执行扣款
			 DBG_DATA_PRINTF("初始化后的ATC2:%02x %02x\n",CardLanCPU.offlineSN[0],CardLanCPU.offlineSN[1]);
			memset(Send,0,sizeof(Send));
			memcpy(Send,"\x80\x54\x01\x00\x0F",5); 		//命令头
			memcpy(&Send[5],CardLanCPU.PSAMOfflineSN,4);	//终端交易序号
			memcpy(&Send[9],Timebuf,7); 				//终端交易日期时间
			memcpy(&Send[16],CardLanCPU.MAC1,4);		//MAC1
			
			Send[20] = 0x08; 							//Le
			len = 21;
            DBG_DATA_PRINTF("执行扣款发送:");
		    menu_print(Send, len);
			result = write(mf_fd,Send,len);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
					t++;
					DBG_DATA_PRINTF("执行扣款返回:");
					menu_print(receive_buf, receive_len[0]);
					memcpy(CardLanCPU.TAC,receive_buf,4);
					memcpy(CardLanCPU.MAC2,&receive_buf[4],4);
				}
				else if(receive_len[0] == 2)
				{
					printf("执行扣款SW1=%02X   SW2=%02X\n",receive_buf[0],receive_buf[1]);
					flag = 0;
				}				
			}
			else                //flag = 0;
			{	
                t = 0x01;
                flag=0;
                                   
                }
			
			break;
            
            case 9:
			flag = 0;
            t=0;
            Qflag = 0;
			break;


                
			}
		}

	}
	
	LEDL(0);
	beepopen(11);
	return t;
}

unsigned char Card_DebitChas_jiaotong(unsigned int Money,char *Key,unsigned char Keylen)
{
	
	int result;
	unsigned char Timebuf[8];
	unsigned char buff1[20];
	unsigned char Send[128],Recv[128];
	unsigned char flag,t,status,len,i;


	Timebuf[0] = 0x20;
	Timebuf[1] = Time.year;
	Timebuf[2] = Time.month;
	Timebuf[3] = Time.day;
	Timebuf[4] = Time.hour;
	Timebuf[5] = Time.min;
	Timebuf[6] = Time.sec;

	flag = 1;
	t = 1;
	printf("in card_debitchas_jiaotong\n");
	while(flag)
	{
		switch(t)
		{
		case 1://消费初始化
			memset(Send,0,sizeof(Send));
			memcpy(Send,"\x80\x50\x01\x02\x0B",5); 		//命令头
			Send[5] = PsamKeyIndex; 					//密钥索引
			Send[6] = (Money&0xFF000000)>>24;
			Send[7] = (Money&0xFF0000)>>16;
			Send[8] = (Money&0xFF00)>>8;
			Send[9] = Money&0xFF;						//交易金额
			memcpy(&Send[10],PsamNum,6); 				//终端机号
			Send[16] = 0x0f; 							//Le
			len = 17;
			result = write(mf_fd,Send,len);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
					t++;
					printf("消费初始化返回:");
					menu_print(receive_buf, receive_len[0]);
					memcpy(CardLanCPU.offlineSN,&receive_buf[4],2);
					memcpy(CardLanCPU.overdraftAmount,&receive_buf[6],3);
					CardLanCPU.keyVersion = receive_buf[9];
					CardLanCPU.arithmeticLabel = receive_buf[10];
					memcpy(CardLanCPU.PRandom,&receive_buf[11],4);
				}
				else
				{
					printf("消费初始化SW1=%02X   SW2=%02X\n",receive_buf[0],receive_buf[1]);
					flag = 0;
				}
			}
			else flag = 0;
			break;
		case 2://PSAM卡生成MAC1
			memset(Send,0,sizeof(Send));
			memcpy(Send,"\x80\x70\x00\x00\x24",5);			//命令头
			memcpy(&Send[5],CardLanCPU.PRandom,4);			//用户卡片随机数
			memcpy(&Send[9],CardLanCPU.offlineSN,2);			//用户卡脱机交易序号
			Send[11] = (Money&0xFF000000)>>24;
			Send[12] = (Money&0xFF0000)>>16;
			Send[13] = (Money&0xFF00)>>8;
			Send[14] = Money&0xFF;							//交易金额
			Send[15] = 0x06;								//交易类型
			memcpy(&Send[16],Timebuf,7);					//交易日期时间
			Send[23] = CardLanCPU.keyVersion;					//密钥版本号
			Send[24] = CardLanCPU.arithmeticLabel;				//密钥算法标识
			memcpy(&Send[25],&CardLanCPU.appserialnumber[2],8);//用户卡号
			memcpy(&Send[33],CardLanCPU.issuerlabel,8);		//发卡机构编码
			len = 41;
			memset(Recv,0,sizeof(Recv));
            		status = PsamCos(Send,Recv,&len);
            		if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
			{
				t++;
				//printf("PSAM卡计算MAC1返回:");
				menu_print(Recv, len);
				memcpy(CardLanCPU.PSAMOfflineSN,Recv,4);
				memcpy(CardLanCPU.MAC1,&Recv[4],4);
			}
			else flag = 0;
			break;
		
		case 3://执行扣款
			memset(Send,0,sizeof(Send));
			memcpy(Send,"\x80\x54\x01\x00\x0F",5); 		//命令头
			memcpy(&Send[5],CardLanCPU.PSAMOfflineSN,4);	//终端交易序号
			memcpy(&Send[9],Timebuf,7); 				//终端交易日期时间
			memcpy(&Send[16],CardLanCPU.MAC1,4);		//MAC1
			Send[20] = 0x08; 							//Le
			len = 21;
			result = write(mf_fd,Send,len);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
					t++;
					//printf("执行扣款返回:");
					menu_print(receive_buf, receive_len[0]);
					memcpy(CardLanCPU.TAC,receive_buf,4);
					memcpy(CardLanCPU.MAC2,&receive_buf[4],4);
				}
				else if(receive_len[0] == 2)
				{
					printf("执行扣款SW1=%02X   SW2=%02X\n",receive_buf[0],receive_buf[1]);
					flag = 0;
				}
				else
				{
					//防插拔处理
					status = SecondDebitChas_jiaotong(Money,Key,Keylen);
					if(status == 0)
						t ++;
					else if(status == 0xFF)
						t = 1;
					else
						t = 0xAA;
				}
			}
			else flag = 0;
			break;
		case 4://PSAM卡验证MAC2
			memset(Send,0,sizeof(Send));
			memcpy(Send,"\x80\x72\x00\x00\x04",5);		//命令头
			memcpy(&Send[5],CardLanCPU.MAC2,4);		//MAC2
			len = 9;
			for(i=0;i<2;i++)
			{
				status = PsamCos(Send,Recv,&len);
				if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
				{
				//	printf("PSAM卡验证MAC2  SW1=%02X  SW2=%02X\n",Recv[len-2],Recv[len-1]);
					break;
				}
			}
			t++;
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
	return t;
	
}


//复合消费交易流程
unsigned char Card_DebitChas_complex_jiaotong(unsigned int Money,char *Key,unsigned char Keylen)
{
	int result;
	unsigned char Timebuf[8];
	unsigned char Send[256],Recv[256];
	unsigned char flag,t,status,len,i;
    unsigned int beforeMoney;
    char selectapp[]  = {0x00,0xa4,0x00,0x00,0x02,0x80,0x11};
    
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
			case 1://复合应用消费初始化
			memset(Send,0,sizeof(Send));
			memcpy(Send,"\x80\x50\x03\x02\x0B",5); 		//命令头
			Send[5] = PsamKeyIndex; 					//密钥索引
			Send[6] = (Money&0xFF000000)>>24;
			Send[7] = (Money&0xFF0000)>>16;
			Send[8] = (Money&0xFF00)>>8;
			Send[9] = Money&0xFF;						//交易金额
			memcpy(&Send[10],PsamNum,6); 				//终端机号
			Send[16] = 0x0f; 							//Le
			len = 17;
            DBG_DATA_PRINTF("复合应用消费初始化发送:");
		    menu_print(Send, len);
			result = write(mf_fd,Send,len);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
					t++;
					DBG_DATA_PRINTF("复合应用消费初始化返回:");
					menu_print(receive_buf, receive_len[0]);
					memcpy(CardLanCPU.offlineSN,&receive_buf[4],2);
					memcpy(CardLanCPU.overdraftAmount,&receive_buf[6],3);
					CardLanCPU.keyVersion = receive_buf[9];
					CardLanCPU.arithmeticLabel = receive_buf[10];
					memcpy(CardLanCPU.PRandom,&receive_buf[11],4);
				}
				else
				{
					printf("复合应用消费初始化SW1=%02X   SW2=%02X\n",receive_buf[0],receive_buf[1]);
					flag = 0;
				}
			}
			else flag = 0;
			break;
            
			case 2://PSAM卡生成MAC1
                memset(Recv,0,sizeof(Recv));
                len = sizeof(selectapp);
                status = PsamCos(selectapp,Recv,&len);
                if((status == MI_OK)&&(Recv[len-2]== 0x90)&&(Recv[len-1]== 0x00))
                {
    			    DBG_DATA_PRINTF("初始化后的ATC1:%02x %02x\n",CardLanCPU.offlineSN[0],CardLanCPU.offlineSN[1]);
        			memset(Send,0,sizeof(Send));
        			memcpy(Send,"\x80\x70\x00\x00\x24",5);			//命令头
        			memcpy(&Send[5],CardLanCPU.PRandom,4);			//用户卡片随机数
        			memcpy(&Send[9],CardLanCPU.offlineSN,2);			//用户卡脱机交易序号
        			Send[11] = (Money&0xFF000000)>>24;
        			Send[12] = (Money&0xFF0000)>>16;
        			Send[13] = (Money&0xFF00)>>8;
        			Send[14] = Money&0xFF;							//交易金额
        			Send[15] = 0x09;								//交易类型
        			memcpy(&Send[16],Timebuf,7);					//交易日期时间
        			Send[23] = CardLanCPU.keyVersion;					//密钥版本号
        			Send[24] = CardLanCPU.arithmeticLabel;				//密钥算法标识
        			memcpy(&Send[25],&CardLanCPU.appserialnumber[2],8);//用户卡号
        			memcpy(&Send[33],CardLanCPU.issuerlabel,8);		//发卡机构编码
        			len = 41;
        			memset(Recv,0,sizeof(Recv));

                    DBG_DATA_PRINTF("生成MAC1发送:");
        		    menu_print(Send, len);
                    status = PsamCos(Send,Recv,&len);
                    
        			DBG_DATA_PRINTF("生成MAC1返回:");
        		    menu_print(Recv, len);
        	
                    if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
        			{
        				t++;
        		//		printf("PSAM卡计算MAC1返回:");
        				menu_print(Recv, len);
        				memcpy(CardLanCPU.PSAMOfflineSN,Recv,4);
        				memcpy(CardLanCPU.MAC1,&Recv[4],4);
        			}
        			else flag = 0;
                 }
                else
                    flag = 0;
			break;          
			
			case 3://用户卡更新复合应用记录文件
			if(Section.Enable != 0x55)
                t++;
            else
            {
    			memset(Send,0,sizeof(Send));
    			memcpy(Send,"\x80\xDC\x02\xD0\x80",5);
    			memcpy(&Send[5],"\x27\x02\x7D\x01\x01",5);
    			
    			Send[10] = CardLanCPU.applockflag;
    			memcpy(&Send[11],CardLanCPU.tradeserialnumber,8);
    			
    			Send[19] = CardLanCPU.tradestate;
    			memcpy(&Send[20],CardLanCPU.getoncitycode,2);
    			memcpy(&Send[22],CardLanCPU.getonissuerlabel,8);
    			memcpy(&Send[30],CardLanCPU.getonoperatorcode,2);
    			memcpy(&Send[32],CardLanCPU.getonline,2);
    			
    			Send[34] = CardLanCPU.getonstation;
    			memcpy(&Send[35],CardLanCPU.getonbus,8);
    			memcpy(&Send[43],CardLanCPU.getondevice,8);
    			memcpy(&Send[51],CardLanCPU.getontime,7);
    			memcpy(&Send[58],CardLanCPU.markamount,4);
    			
    			Send[62] = CardLanCPU.directionflag;
    			memcpy(&Send[63],CardLanCPU.getoffcitycode,2);
    			memcpy(&Send[65],CardLanCPU.getoffissuerlabel,8);
    			memcpy(&Send[73],CardLanCPU.getoffoperatorcode,2);
    			memcpy(&Send[75],CardLanCPU.getoffline,2);
    			
    			Send[77] = CardLanCPU.getoffstation;
    			memcpy(&Send[78],CardLanCPU.getoffbus,8);
    			memcpy(&Send[86],CardLanCPU.getoffdevice,8);
    			memcpy(&Send[94],CardLanCPU.getofftime,7);
    			memcpy(&Send[101],CardLanCPU.tradeamount,4);
    			memcpy(&Send[105],CardLanCPU.ridedistance,2);
    			
    			len = 133;
    			result = write(mf_fd,Send,len);
                DBG_DATA_PRINTF("更新复合记录发送:");
    		    menu_print(Send, len);
    			if(result == MI_OK)
    			{
    				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
    				result = read(mf_fd, receive_buf, receive_len[0]);
    				if(receive_buf[0]==0x90 && receive_buf[1]==0x00)
    				{
    					t++;
    					DBG_DATA_PRINTF("更新复合应用数据缓存返回:");
    					menu_print(receive_buf, receive_len[0]);
    				}
    				else
    				{
    					printf("更新复合应用数据缓存SW1=%02X   SW2=%02X\n",receive_buf[0],receive_buf[1]);
    					flag = 0;
    				}
    			}
    			else flag = 0;
                }
			break;

            case 4:
                memset(Send,0,sizeof(Send));
    			memcpy(Send,"\x80\xDC\x00\xF0\x30",5);
                if(Section.Enable!=0x55)                     //交易类型
        			Send[5] = 0x06;
                else
                {
                    if(CardLanCPU.enterexitflag == 0x55)
                        Send[5] = 0x03;
                    else
                        Send[5] = 0x04;
                    }
                Send[6] = 0;
                Send[7] = 0;
                memcpy(&Send[8],PsamNum,6);                  //终端编号
                Send[14] = 0x02;                             //行业代码
                memcpy(&Send[15],Yanzhou_Card.LineNO,2);     //线路号
                if(Section.Enable!=0x55)                     //站点号
        			{
                        Send[17] = 0x00;
                        Send[18] = 0x00;
                    }
                else
                {
                    if(CardLanCPU.enterexitflag == 0x55)
                        {
                            Send[17] = 0x00;
                            Send[18] = CardLanCPU.getonstation;
                        }
                    else
                        {
                            Send[17] = 0x00;
                            Send[18] = CardLanCPU.getoffstation;
                        }
                    }
                memcpy(&Send[19],Yanzhou_Card.OperatorCode,2);
                Send[21] = 0;
                Send[22] = (Money&0xFF000000)>>24;
			    Send[23] = (Money&0xFF0000)>>16;
			    Send[24] = (Money&0xFF00)>>8;
			    Send[25] = Money&0xFF;							//交易金额
			    beforeMoney=CardLanCPU.beforemoney[0]<<24|CardLanCPU.beforemoney[1]<<16|CardLanCPU.beforemoney[2]<<8|CardLanCPU.beforemoney[3];                               					
                //交易后余额    
                Send[26] = ((beforeMoney-HostValue.i)&0xFF000000)>>24;
			    Send[27] = ((beforeMoney-HostValue.i)&0xFF0000)>>16;
			    Send[28] = ((beforeMoney-HostValue.i)&0xFF00)>>8;
			    Send[29] = ((beforeMoney-HostValue.i)&0xFF)>>0;
			    memcpy(&Send[30],Timebuf,7);                    //交易时间
			    memcpy(&Send[37],&Yanzhou_Card.CityCode[0],2);  //受理方城市代码
			    memcpy(&Send[39],&Yanzhou_Card.IssuerLabel,8);  //受理方机构标识			    
			    len = 53;
    			result = write(mf_fd,Send,len);
                DBG_DATA_PRINTF("更新交易信息记录发送:");
    		    menu_print(Send, len);
    			if(result == MI_OK)
    			{
    				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
    				result = read(mf_fd, receive_buf, receive_len[0]);
    				if(receive_buf[0]==0x90 && receive_buf[1]==0x00)
    				{
    					t++;
    					DBG_DATA_PRINTF("更新交易信息记录发送:");
    					menu_print(receive_buf, receive_len[0]);
    				}
    				else
    				{
    					printf("更新交易信息记录发送:SW1=%02X   SW2=%02X\n",receive_buf[0],receive_buf[1]);
    					flag = 0;
    				}
    			}
    			else flag = 0;
                
                
                
                break;

			case 5://执行扣款
			 DBG_DATA_PRINTF("初始化后的ATC2:%02x %02x\n",CardLanCPU.offlineSN[0],CardLanCPU.offlineSN[1]);
			memset(Send,0,sizeof(Send));
			memcpy(Send,"\x80\x54\x01\x00\x0F",5); 		//命令头
			memcpy(&Send[5],CardLanCPU.PSAMOfflineSN,4);	//终端交易序号
			memcpy(&Send[9],Timebuf,7); 				//终端交易日期时间
			memcpy(&Send[16],CardLanCPU.MAC1,4);		//MAC1
			
			Send[20] = 0x08; 							//Le
			len = 21;
            DBG_DATA_PRINTF("执行扣款发送:");
		    menu_print(Send, len);
			result = write(mf_fd,Send,len);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
					t++;
					DBG_DATA_PRINTF("执行扣款返回:");
					menu_print(receive_buf, receive_len[0]);
					memcpy(CardLanCPU.TAC,receive_buf,4);
					memcpy(CardLanCPU.MAC2,&receive_buf[4],4);
				}
				else if(receive_len[0] == 2)
				{
					printf("执行扣款SW1=%02X   SW2=%02X\n",receive_buf[0],receive_buf[1]);
					flag = 0;
				}
				else
				{
					//防插拔处理
					status = SecondDebitChas_jiaotong(Money,Key,Keylen);
					if(status == 0)
						t ++;
					else if(status == 0xFF)
						{
                            t = 1;
                            flag=0;
                            
                       }
					else
						{
                            t = 0xAA;
                            flag=0;
                       }
				}
			}
			else 
                //flag = 0;
			{
                    
					status = SecondDebitChas_jiaotong(Money,Key,Keylen);
                    DBG_DATA_PRINTF("重新刷卡返回:%02x\n",status);
					if(status == 0)
						t ++;
					else if(status == 0xFF)
						{
                            t = 1;
                            flag=0;
                            
                       }
					else
						{
                            t = 0xAA;
                            flag=0;
                       }                
                }
			
			break;
            
            case 6://PSAM卡验证MAC2
			memset(Send,0,sizeof(Send));
			memcpy(Send,"\x80\x72\x00\x00\x04",5);		//命令头
			memcpy(&Send[5],CardLanCPU.MAC2,4);		//MAC2
			len = 9;
			for(i=0;i<2;i++)
			{
				status = PsamCos(Send,Recv,&len);
				if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
				{
					printf("PSAM卡验证MAC2  SW1=%02X  SW2=%02X\n",Recv[len-2],Recv[len-1]);
					break;
				}
			}
			t++;
			break;
		           
		default:
			flag=0;
			t=0;
			break;
		}
	}
	return t;
}


//快速消费交易流程
unsigned char Card_DebitChas_Quickpass(unsigned int Money,char *Key,unsigned char Keylen)
{
	int result;
	unsigned char Timebuf[8];
	unsigned char Send[256],Recv[256],Send1[256];
	unsigned char flag,t,status,len,i;
    unsigned int beforeMoney;
    LongUnon Rand;
	Timebuf[0] = 0x20;
	Timebuf[1] = Time.year;
	Timebuf[2] = Time.month;
	Timebuf[3] = Time.day;
	Timebuf[4] = Time.hour;
	Timebuf[5] = Time.min;
	Timebuf[6] = Time.sec;
	srand(time(0));
    rand();
	flag = 1;
	t = 1;
	while(flag)
	{
			switch(t)
		{
			case 1://用户卡内部认证
			memset(CardLanCPU.random,0,sizeof(CardLanCPU.random));
            Rand.i = 0;
            srand(time(0));
            Rand.i = rand();
            memcpy(CardLanCPU.random,Rand.longbuf,4);
            memset(Send,0,sizeof(Send));
			memcpy(Send,GetChall8,5);			//命令头
			memcpy(&Send[5],CardLanCPU.random,8);
            len = 13;
            result = write(mf_fd,Send,len);
            DBG_DATA_PRINTF("卡片内部认证发送:");
		    menu_print(Send, len);
            if(result == MI_OK)
    			{
    				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
    				result = read(mf_fd, receive_buf, receive_len[0]);
    				if(receive_buf[receive_len[0]-2]==0x90 && receive_buf[receive_len[0]-1]==0x00)
    				{
    					t++;
    					DBG_DATA_PRINTF("卡片内部认证返回:");
    					menu_print(receive_buf, receive_len[0]);
                        memcpy(CardLanCPU.cardsed,receive_buf,8);
                        
    				}
    				else
    				{
    					printf("卡片内部认证返回SW1=%02X   SW2=%02X\n",receive_buf[0],receive_buf[1]);
    					flag = 0;
    				}
    			}
    			else flag = 0;      
			
			break;
            
			case 2://PSAM卡准备生成内部认证密文			 
			memset(Send,0,sizeof(Send));
			memcpy(Send,"\x80\x1a\x27\x01\x08",5);			    //命令头
			memcpy(&Send[5],CardLanCPU.appserialnumber+2,8);	//用户卡号低8位
			len = 13;
			memset(Recv,0,sizeof(Recv));
            DBG_DATA_PRINTF("准备认证发送:");
		    menu_print(Send, len);
            status = PsamCos(Send,Recv,&len);
            
			DBG_DATA_PRINTF("准备认证返回:");
		    menu_print(Recv, len);
	
            if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
			{
				t++;
		//		printf("PSAM卡计算MAC1返回:");
				menu_print(Recv, len);				
			}
			else flag = 0;		
			break;        

            case 3://PSAM卡开始生成内部认证密文			 
			memset(Send,0,sizeof(Send));
			memcpy(Send,"\x80\xfa\x00\x00\x08",5);			//命令头
			memcpy(&Send[5],CardLanCPU.random,8);			//设备随机数
			len = 13;
			memset(Recv,0,sizeof(Recv));
            DBG_DATA_PRINTF("开始认证发送:");
		    menu_print(Send, len);
            status = PsamCos(Send,Recv,&len);
            
			DBG_DATA_PRINTF("准备认证返回:");
		    menu_print(Recv, len);
	
            if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
			{
				t++;
				printf("PSAM卡认证返回:");
				menu_print(Recv, len);	
                memcpy(CardLanCPU.samsed,Recv,4);
			}
			else flag = 0;		
			break; 

            case 4:
            if(memcmp(CardLanCPU.cardsed,CardLanCPU.samsed,4)==0)
            {
                t++;
                }
            else
            {
                t = 0xBB;
                flag=0;
                }
            break;

            case 5://PSAM卡计算MAC
            memset(Send,0,sizeof(Send));
			memcpy(Send,"\x80\x1a\x45\x06\x10",5);			    //命令头
           	memcpy(&Send[5],CardLanCPU.appserialnumber+2,8);	//用户卡号低8位
            memcpy(&Send[13],CardLanCPU.issuerlabel,8);	        //用户卡发卡机构版本
            len = 21;
			memset(Recv,0,sizeof(Recv));
            DBG_DATA_PRINTF("开始计算发送:");
		    menu_print(Send, len);
            status = PsamCos(Send,Recv,&len);
            
			DBG_DATA_PRINTF("开始计算返回:");
		    menu_print(Recv, len);
	
            if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
			{
				t++;
				
			}
			else flag = 0;	
            break;

            case 6:
            memset(Send,0,sizeof(Send));
            memset(Send1,0,sizeof(Send1));
            memcpy(Send1,"\x04\xDC\x02\xD4\x84",5);
			memcpy(&Send1[5],"\x27\x02\x7D\x01\x01",5);
			
			Send1[10] = CardLanCPU.applockflag;
			memcpy(&Send1[11],CardLanCPU.tradeserialnumber,8);
			
			Send1[19] = CardLanCPU.tradestate;
			memcpy(&Send1[20],CardLanCPU.getoncitycode,2);
			memcpy(&Send1[22],CardLanCPU.getonissuerlabel,8);
			memcpy(&Send1[30],CardLanCPU.getonoperatorcode,2);
			memcpy(&Send1[32],CardLanCPU.getonline,2);
			
			Send1[34] = CardLanCPU.getonstation;
			memcpy(&Send1[35],CardLanCPU.getonbus,8);
			memcpy(&Send1[43],CardLanCPU.getondevice,8);
			memcpy(&Send1[51],CardLanCPU.getontime,7);
			memcpy(&Send1[58],CardLanCPU.markamount,4);
			
			Send1[62] = CardLanCPU.directionflag;
			memcpy(&Send1[63],CardLanCPU.getoffcitycode,2);
			memcpy(&Send1[65],CardLanCPU.getoffissuerlabel,8);
			memcpy(&Send1[73],CardLanCPU.getoffoperatorcode,2);
			memcpy(&Send1[75],CardLanCPU.getoffline,2);
			
			Send1[77] = CardLanCPU.getoffstation;
			memcpy(&Send1[78],CardLanCPU.getoffbus,8);
			memcpy(&Send1[86],CardLanCPU.getoffdevice,8);
			memcpy(&Send1[94],CardLanCPU.getofftime,7);
			memcpy(&Send1[101],CardLanCPU.tradeamount,4);
			memcpy(&Send1[105],CardLanCPU.ridedistance,2);
            
			memcpy(Send,"\x80\xfa\x05\x00\x8d",5);			    //命令头
			memcpy(&Send[5],CardLanCPU.random,8);
            memcpy(&Send[13],Send1,133);

            len = 146;
			memset(Recv,0,sizeof(Recv));
            DBG_DATA_PRINTF("计算发送:");
		    menu_print(Send, len);
            status = PsamCos(Send,Recv,&len);
            
			DBG_DATA_PRINTF("计算返回:");
		    menu_print(Recv, len);
	
            if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
			{
				t++;
                memcpy(CardLanCPU.MAC1,Recv,4);
				
			}
			else flag = 0;	
            break; 
            
			case 7://用户卡更新复合应用记录文件          
    			memset(Send,0,sizeof(Send));
    			memcpy(Send,Send1,133);             //命令5字节+数据128字节
    			memcpy(&Send[133],CardLanCPU.MAC1,4);			    			
    			len = 137;
    			result = write(mf_fd,Send,len);
                DBG_DATA_PRINTF("更新复合记录发送:");
    		    menu_print(Send, len);
    			if(result == MI_OK)
    			{
    				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
    				result = read(mf_fd, receive_buf, receive_len[0]);
    				if(receive_buf[0]==0x90 && receive_buf[1]==0x00)
    				{
    					t++;
    					DBG_DATA_PRINTF("更新复合应用数据缓存返回:");
    					menu_print(receive_buf, receive_len[0]);
    				}
    				else
    				{
    					printf("更新复合应用数据缓存SW1=%02X    SW2=%02X\n",receive_buf[0],receive_buf[1]);
    					flag = 0;
    				}
    			}
    			else flag = 0;                
			break;          
		           
		default:
			flag=0;
			t=0;
			break;
		}
	}
	return t;
}


unsigned char TopUpCardInfor_CPU_jiaotong(int type)
{
	unsigned char status = 1;
	unsigned char keybuff[20];

//	printf("in topupcardinfor_cpu_jiaotong\n");
	if(type == 1) 
	{
		status  = Card_DebitChas_complex_jiaotong(HostValue.i,keybuff,16);
	}
	else if(type == 2)
	{
		status  = Card_DebitChas_jiaotong(HostValue.i,keybuff,16);
	}
	else 
	{
		status = Card_DebitChas_Quickpass(HostValue.i,keybuff,16);
	}
    DBG_PRINTF("交易返回值:%02x\n",status);
	return status;

}

unsigned char GET_MAC()
{
	int result;
	unsigned char Timebuf[8];
	unsigned char Send[256],Recv[256];
	unsigned char flag,t,status,len,i;    
	
	flag = 1;
	t = 1;
	while(flag)
	{
	    switch(t)
		{
			case 1://进入PSAM卡片目录
			/*
			memset(Send,0,sizeof(Send));
			memcpy(Send,"\x00\xa4\x00\x00\x02\x3f\x00",7); 		//命令头
			len = 7;
            DBG_DATA_PRINTF("进入PSAM卡片目录:");
		    menu_print(Send, len);
			 status = PsamCos(Send,Recv,&len);
			 if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
			{				
					t++;			
				
			}
			else flag = 0;*/
            memset(Send,0,sizeof(Send));
            memset(CardLanCPU.PSAMRandom,0,sizeof(CardLanCPU.PSAMRandom));
			memcpy(Send,"\x00\x84\x00\x00\x04",5); 		//命令头			
			len = 5;
			result = write(mf_fd,Send,len);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
					t=3;
					printf("取卡随机数返回:");
					menu_print(receive_buf, receive_len[0]);
					memcpy(CardLanCPU.PSAMRandom,&receive_buf[0],4);
					
				}
				else
				{
					printf("取卡随机数返回SW1=%02X   SW2=%02X\n",receive_buf[0],receive_buf[1]);
					flag = 0;
				}
			}			
			break;
            
			case 2://PSAM卡生随机数
			memset(Send,0,sizeof(Send));
			memcpy(Send,"\x00\x84\x00\x00\x08",5);			//命令头			
			len = 5;
			memset(Recv,0,sizeof(Recv));

            DBG_DATA_PRINTF("生成随机数发送:");
		    menu_print(Send, len);
            status = PsamCos(Send,Recv,&len);
            
			DBG_DATA_PRINTF("生成随机数返回:");
		    menu_print(Recv, len);
	
            if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
			{
				t++;				
				memcpy(CardLanCPU.PSAMRandom,Recv,8);
				
			}
			else flag = 0;
		
			break;          
			
			case 3://准备发送
            memset(Send,0,sizeof(Send));
			memcpy(Send,"\x80\x1A\x45\x02\x10",5);			//命令头	
            memcpy(Send+5,CardLanCPU.appserialnumber+2,8);
            memcpy(Send+13,CardLanCPU.issuerlabel,8);
            len = 21;
            
            DBG_DATA_PRINTF("准备发送:");
		    menu_print(Send, len);
            status = PsamCos(Send,Recv,&len);
            
			DBG_DATA_PRINTF("准备发送返回:");
		    menu_print(Recv, len);
	
            if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
			{
				t++;				
			}
			else flag = 0;
            
			break;

            case 4://初始化计算MAC
            memset(Send,0,sizeof(Send));
			memcpy(Send,"\x80\xFA\x05\x00\x10",5);
            memcpy(Send+5,CardLanCPU.PSAMRandom,8);
            memcpy(Send+13,"\x84\x1E\x00\x00\x04\x80\x00\x00",8);
            len = 21;
            DBG_DATA_PRINTF("初始化计算MAC发送");
		    menu_print(Send, len);
            status = PsamCos(Send,Recv,&len);
            
			DBG_DATA_PRINTF("初始化计算MAC返回");
		    menu_print(Recv, len);    
            if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
			{
				t++;
                memcpy(CardLanCPU.DESCRY,Recv,4);
			}
			else flag = 0;
            
            break;

			case 5://应用锁定 			
			memset(Send,0,sizeof(Send));
			memcpy(Send,"\x84\x1E\x00\x00\x04",5); 		//命令头
			memcpy(&Send[5],CardLanCPU.DESCRY,4);	//终端交易序号										
			len = 9;
            DBG_DATA_PRINTF("应用锁定发送:");
		    menu_print(Send, len);
			result = write(mf_fd,Send,len);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
					t++;
					DBG_DATA_PRINTF("应用锁定发送:");
					menu_print(receive_buf, receive_len[0]);					
				}
				else if(receive_len[0] == 2)
				{
					printf("应用锁定返回:SW1=%02X   SW2=%02X\n",receive_buf[0],receive_buf[1]);
					flag = 0;
				}
				
			}
			else flag = 0;
			
			break;            
            
		           
		default:
			flag=0;
			t=0;
			break;
		}
	}
	return t;
}

unsigned char GET_MAC_zhujian()
{
	int result;
	unsigned char Timebuf[8];
	unsigned char Send[256],Recv[256];
	unsigned char flag,t,status,len,i;  
    unsigned char dev[16],dat[8];
    unsigned char upblackflag[]={0x04,0xd6,0x85,0x00,0x05,0xa5};

    memset(dev,0,16);    
    memcpy(dev,CardLanCPU.zappserialnumber,8);
    dev[9] = 0x10;
    dev[11] = 0xff;
    printf("分散因子:");
	menu_print(dev, 16);
    memset(dat,0,8);
    memcpy(dat,upblackflag,6);
    dat[6] = 0x80;    
    
	flag = 1;
	t = 1;
	while(flag)
	{
	    switch(t)
		{
			case 1://取用户卡随机数         
            memset(Send,0,sizeof(Send));
            memset(CardLanCPU.PSAMRandom,0,sizeof(CardLanCPU.PSAMRandom));
			memcpy(Send,"\x00\x84\x00\x00\x04",5); 			
			len = 5;
			result = write(mf_fd,Send,len);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
					t=3;
					printf("取卡随机数返回:");
					menu_print(receive_buf, receive_len[0]);
					memcpy(CardLanCPU.cardrandom,&receive_buf[0],4);
					
				}
				else
				{
					printf("取卡随机数返回SW1=%02X   SW2=%02X\n",receive_buf[0],receive_buf[1]);
					flag = 0;
				}
			}			
			break;
            
			case 2://PSAM卡选择应用目录
            memset(Send,0,sizeof(Send));
			memcpy(Send,"\x00\xa4\x00\x00\x02\x10\x01",7); 		//命令头
			len = 7;
            DBG_DATA_PRINTF("进入PSAM卡片目录:");
		    menu_print(Send, len);
			 status = PsamCos(Send,Recv,&len);
			 if((status != MI_OK)&&(Recv[len-2] != 0x90)&&(Recv[len-1] != 0x00))
			{				
					flag=0;							
			}
             else
                t++;
			
		
			break;          
			
			case 3://准备发送
            memset(Send,0,sizeof(Send));
			memcpy(Send,"\x80\x1A\x45\x01\x10",5);			//命令头	
            memcpy(Send+5,dev,16);            
            len = 21;            
            DBG_DATA_PRINTF("准备发送:");
		    menu_print(Send, len);
            status = PsamCos(Send,Recv,&len);
            
			DBG_DATA_PRINTF("准备发送返回:");
		    menu_print(Recv, len);
	
            if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
			{
				t++;				
			}
			else flag = 0;
            
			break;

            case 4://初始化计算MAC
            memset(Send,0,sizeof(Send));
			memcpy(Send,"\x80\xFA\x05\x00\x10",5);
            memcpy(Send+5,CardLanCPU.cardrandom,4);
            memcpy(Send+13,dat,8);
            len = 21;
            DBG_DATA_PRINTF("初始化计算MAC发送");
		    menu_print(Send, len);
            status = PsamCos(Send,Recv,&len);
            
			DBG_DATA_PRINTF("初始化计算MAC返回");
		    menu_print(Recv, len);    
            if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
			{
				t++;
                memcpy(CardLanCPU.DESCRY,Recv,4);
			}
			else flag = 0;
            
            break;

			case 5://更新0005文件 			
			memset(Send,0,sizeof(Send));
			memcpy(Send,upblackflag,7); 		    //命令头
			memcpy(&Send[7],CardLanCPU.DESCRY,4);	//终端交易序号										
			len = 11;
            DBG_DATA_PRINTF("更新0005发送:");
		    menu_print(Send, len);
			result = write(mf_fd,Send,len);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
					t++;
					DBG_DATA_PRINTF("更新0005发送:");
					menu_print(receive_buf, receive_len[0]);					
				}
				else if(receive_len[0] == 2)
				{
					printf("更新0005返回:SW1=%02X   SW2=%02X\n",receive_buf[0],receive_buf[1]);
					flag = 0;
				}
				
			}
			else flag = 0;
			
			break;            
            
		           
		default:
			flag=0;
			t=0;
			break;
		}
	}
	return t;
}

//
unsigned char Upnomalfile(unsigned char *data,unsigned char len1)
{
	int result;
	unsigned char Timebuf[8];
	unsigned char Send[256],Recv[256];
	unsigned char flag,t,status,len,i,len2;  
    unsigned char dev[16],dat[24];
    

    memset(dev,0,16);    
    memcpy(dev,CardLanCPU.zappserialnumber,8);
    dev[9] = 0x10;
    dev[11] = 0xff;
    printf("分散因子:");
	menu_print(dev, 16);
    memset(dat,0,24);
    memcpy(dat,data,len1);
    if(len1%8!=0)
    {
        if(len1<=8)
            len2=8;
        else if(8<len1<=16)
            len2=16;
        else if(16<len1<=24)
            len2=24;
        }
    dat[len1+1] =0x80;
    
	flag = 1;
	t = 1;
	while(flag)
	{
	    switch(t)
		{
			case 1://取用户卡随机数         
            memset(Send,0,sizeof(Send));
            memset(CardLanCPU.PSAMRandom,0,sizeof(CardLanCPU.PSAMRandom));
			memcpy(Send,"\x00\x84\x00\x00\x04",5); 			
			len = 5;
			result = write(mf_fd,Send,len);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
					t=3;
					printf("取卡随机数返回:");
					menu_print(receive_buf, receive_len[0]);
					memcpy(CardLanCPU.cardrandom,&receive_buf[0],4);
					
				}
				else
				{
					printf("取卡随机数返回SW1=%02X   SW2=%02X\n",receive_buf[0],receive_buf[1]);
					flag = 0;
				}
			}			
			break;
            
			case 2://PSAM卡选择应用目录
            memset(Send,0,sizeof(Send));
			memcpy(Send,"\x00\xa4\x00\x00\x02\x10\x01",7); 		//命令头
			len = 7;
            DBG_DATA_PRINTF("进入PSAM卡片目录:");
		    menu_print(Send, len);
			 status = PsamCos(Send,Recv,&len);
			 if((status != MI_OK)&&(Recv[len-2] != 0x90)&&(Recv[len-1] != 0x00))
			{				
					flag=0;							
			}
             else
                t++;
			
		
			break;          
			
			case 3://准备发送
            memset(Send,0,sizeof(Send));
			memcpy(Send,"\x80\x1A\x45\x01\x10",5);			//命令头	
            memcpy(Send+5,dev,16);            
            len = 21;            
            DBG_DATA_PRINTF("准备发送:");
		    menu_print(Send, len);
            status = PsamCos(Send,Recv,&len);
            
			DBG_DATA_PRINTF("准备发送返回:");
		    menu_print(Recv, len);
	
            if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
			{
				t++;				
			}
			else flag = 0;
            
			break;

            case 4://初始化计算MAC
            memset(Send,0,sizeof(Send));
			memcpy(Send,"\x80\xFA\x05\x00\x10",5);
            memcpy(Send+5,CardLanCPU.cardrandom,4);
            memcpy(Send+13,dat,len2);
            len = 21;
            DBG_DATA_PRINTF("初始化计算MAC发送");
		    menu_print(Send, len);
            status = PsamCos(Send,Recv,&len);
            
			DBG_DATA_PRINTF("初始化计算MAC返回");
		    menu_print(Recv, len);    
            if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
			{
				t++;
                memcpy(CardLanCPU.DESCRY,Recv,4);
			}
			else flag = 0;
            
            break;

			case 5://更新0005文件 			
			memset(Send,0,sizeof(Send));
			memcpy(Send,data,len1); 		    //命令头
			memcpy(&Send[len1],CardLanCPU.DESCRY,4);	//终端交易序号										
			len = 11;
            DBG_DATA_PRINTF("更新文件发送:");
		    menu_print(Send, len);
			result = write(mf_fd,Send,len);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
					t++;
					DBG_DATA_PRINTF("更新文件发送:");
					menu_print(receive_buf, receive_len[0]);					
				}
				else if(receive_len[0] == 2)
				{
					printf("更新文件返回:SW1=%02X   SW2=%02X\n",receive_buf[0],receive_buf[1]);
					flag = 0;
				}
				
			}
			else flag = 0;
			
			break;            
            
		           
		default:
			flag=0;
			t=0;
			break;
		}
	}
	return t;
}

unsigned char Card_White_Cpu_jiaotong(void)
{
	unsigned char  status = 1;
	unsigned char  CJCbuf[12];
	WhiteItem src;
	int find = 0;

	memset(CJCbuf,0,sizeof(CJCbuf));
	memcpy(CJCbuf,CardLanCPU.issuerlabel,4);
	memcpy(src.dat, CJCbuf, sizeof(WhiteItem));
	
	half_search_white(src, &find);
    if(find)
	    status = 0;

//	printf("Card_JudgeCsn() find=%d\n", find);
//	printf("Card_JudgeCsn() status=%d\n", status);	

	return status;
}


unsigned char Card_JudgeCsn_Cpu_jiaotong(void)
{
	unsigned char  status;
	unsigned char  CJCbuf[12];
	BlackItem src;
	int find;

	memset(CJCbuf,0,sizeof(CJCbuf));
	memcpy(CJCbuf,CardLanCPU.appserialnumber,10);
	memcpy(src.dat, CJCbuf, sizeof(BlackItem));
	DebugPrintf("\n");
    pthread_mutex_lock(&m_Blacklist);
    DebugPrintf("\n");
	half_search(src, &find);
    pthread_mutex_unlock(&m_Blacklist);
	status = find;

//	printf("Card_JudgeCsn() find=%d\n", find);
//	printf("Card_JudgeCsn() status=%d\n", status);	

	return status;
}


unsigned char Card_JudgeCsn_zhujian(void)
{
	unsigned char  status;
	unsigned char  CJCbuf[12];
	unsigned char  cardbuf[5];
	unsigned int   Nuo;
    
	memset(CJCbuf,0,sizeof(CJCbuf));
	//memcpy(CJCbuf,CardLan.CityId,2);	
   memcpy(CJCbuf+2,CardLanCPU.zappserialnumber,8);
	BlackItem src;
	int find;
	memcpy(src.dat, CJCbuf, sizeof(BlackItem));
	
	half_search_zhujian(src, &find);

	status = find;

	DBG_PRINTF("Card_JudgeCsn() find=%d\n", find);
	DBG_PRINTF("Card_JudgeCsn() status=%d\n", status);
	


	return status;
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



unsigned char Check_CardDate(unsigned char *starttime,unsigned char *endtime)
{
    unsigned status = 0;
    unsigned int start_time,end_time;
    
    start_time = starttime[0]<<24|starttime[1]<<16|starttime[2]<<8|starttime[3];
	end_time = endtime[0]<<24|endtime[1]<<16|endtime[2]<<8|endtime[3];
    if((start_time==0xffffffff)||(end_time==0xffffffff)||(start_time==0x0)||(end_time==0x0))
    {
        status = 1;
        }

    return status;
    }

unsigned char CheckAppDF(void)
{
	int result;
	char Send[256];
	unsigned char len;

	memcpy(Send,ReadFile04,5);
    result = write(mf_fd,Send,5);
    if(result == MI_OK)
    {
		ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
		result = read(mf_fd, receive_buf, receive_len[0]);  	
	    if((receive_buf[receive_len[0]-2]==0x90)&&(receive_buf[receive_len[0]-1]==0x00))          //此卡是住建部CPU卡
           {     
             memcpy(PsamNum,PsamNum_bak2,sizeof(PsamNum_bak2));	
             DBG_PRINTF("使用本地cpu卡流程(住建部):line=%d\n",__LINE__); 
             memcpy(CardLanCPU.zappserialnumber,receive_buf,8);
             CardLanCPU.zcardtype = receive_buf[8];
             if(CardLanCPU.zcardtype!=0x02)                         //本地用户卡
             {
                CardLanCPU.zcardstatus = receive_buf[9];
                memcpy(CardLanCPU.zstarttime,receive_buf+24,4);             
                memcpy(CardLanCPU.zendtime,receive_buf+28,4);             
                return 2;
                }
             else
             {
                CardLanCPU.zcardstatus = receive_buf[9];
                memcpy(CardLanCPU.zstarttime,receive_buf+12,4);             
                memcpy(CardLanCPU.zendtime,receive_buf+16,4);             
                return 24;
                }
            }
        else if((receive_buf[receive_len[0]-2]==0x93)&&(receive_buf[receive_len[0]-1]==0x03))    //卡片已锁定
            {
                DBG_PRINTF("出错9303\n");
                 use_jiaotong_stander=0x0;
                 Err_display(47);
                 return 0xff;
                }
        else
        {                
             use_jiaotong_stander=0x0;                                    //此卡可能是交通部卡
             return 0;
                
            }

	}

	DBG_PRINTF("CheckAppDF() is called.\n");
    menu_print(receive_buf,receive_len[0]);
	
	return 0;

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


	

	DBG_PRINTF("SelectAppDF() is called.\n");
    menu_print(DFname,*namelen);
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
		if((receive_len[0] >= 2)&&(((receive_buf[receive_len[0]-2]==0x90)&&(receive_buf[receive_len[0]-1]==0x00))||
            (receive_buf[receive_len[0]-2]==0x61)))
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
	unsigned char Timebuf[8];
	char Send[256],Recv[256];
	char buff1[8],buff2[8],buff3[8];
	unsigned char flag,t,status,len,Qflag;
	unsigned char InitChas[]= {0x80,0x50,0x01,0x02,0x0b}; //,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x0f};
	// 消费初始化  KEY:0x00  金额:0.01元 终端机号:1号
	unsigned char DebitChas[]= {0x80,0x54,0x01,0x00,0x0f}; //,0x00,0x00,0x00,0x01,0x20,0x12,0x09,0x24,0x15,0x26,0x00}; //+ MAC1
	// 消费   交易序号: 1 交易时间:20120924152600 + MAC1
	LongUnon vbuf,lbuf,olddev;

    DBG_PRINTF("SecondDebitChas() is call\n");

	DBG_PRINTF("SecondDebitChas start  Money == %d \n",Money);


	LEDL(1);
	beepopen(10);
	PlayMusic(15, 0);

	SetColor(Mcolor);
	SetTextColor(Color_red);
        #ifdef NEW0409
		SetTextSize(48);
		TextOut(0 , 75, "温馨提示");
		TextOut(0 , 144, "刷卡错误");
		TextOut(0	, 210,"请重新刷卡");
        #else
		SetTextSize(32);
		TextOut(100 , 50, "温馨提示");
		TextOut(100 , 90, "刷卡错误");
		TextOut(85	, 130,"请重新刷卡");
        #endif

	flag = 1;
	t = 1;
	Qflag = 1;
	ic = 0;
	while(Qflag)
	{
		Rd_time (Timebuf+1);
		if(Time.sec != Timebuf[6])
		{
			ic++;
			if(ic>30)
			{
				Qflag =0;
				break;
			}

			if((ic%2))
			{
				beepopen(10);
			}
			else
			{
				beepopen(11);
			}

			Timebuf[0] = 0x20;
			Time.year = Timebuf[1];
			Time.month = Timebuf[2];
			Time.day = Timebuf[3];
			Time.hour = Timebuf[4];
			Time.min = Timebuf[5];
			Time.sec = Timebuf[6];
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
					flag = 0;
				}
				break;

			case 2:
				memcpy(Send,LanSec.ADFNUM,2);
				len = 2;
				status = SelectAppDF(Send,Recv,&len);
				if(status == MI_OK)t++;
				else flag = 0;
				break;

			case 3: // 读 00018 交易记录文件 最后一条
				memset(Send,0,sizeof(Send));
				Send[0] = 0x00;
				Send[1] = 0xb2; //
				Send[2] = 0x01; //
				Send[3] = 0xc4; // 0018
				Send[4] = 0x00;
				result = write(mf_fd,Send,5);
				if(result == MI_OK)
				{
					ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
					result = read(mf_fd, receive_buf, receive_len[0]);
					if(receive_len[0] > 2)
					{
						olddev.i = 0;
						memcpy(olddev.longbuf,receive_buf+12,4); //上次交易终端机号
						t++;
					}
					else
					{
						flag = 0;
					}

#if APP_PRINTF
					{
						unsigned char i;

						printf("Readcord 0018 data = %d\n",receive_len[0]);
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

			case 4:
				memset(buff1,0,sizeof(buff1));
				memset(Send,0,sizeof(Send));
				MoneyChange(Money,buff1);
				memcpy(Send,InitChas,sizeof(InitChas)); //命令头
				Send[5] = Key[0]; //密钥类型
				memcpy(Send+6,buff1,4); //消费金额
				memcpy(Send+10,PsamNum,6); //终端机号
				Send[16] = 0x0f; //
				len = 17;
				result = write(mf_fd,Send,len);
				if(result == MI_OK)
				{
					ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
					result = read(mf_fd, receive_buf, receive_len[0]);
					if(receive_len[0] > 2)
					{
						memcpy(Recv,receive_buf,receive_len[0]);
						vbuf.i = 0;
						lbuf.i = 0;
						memcpy(vbuf.longbuf,Recv,4);
						//memcpy(lbuf.longbuf,Recv+4,2);
						lbuf.longbuf[0] = Recv[5];
						lbuf.longbuf[1] = Recv[4];
						if((vbuf.i == OldCash.i)&&(lbuf.i == OldCi.i))
						{
							memset(CardLan.ViewMoney,0,sizeof(CardLan.ViewMoney));
							CardLan.ViewMoney[0] = Recv[5];
							CardLan.ViewMoney[1] = Recv[4];
							t++;
						}
						else if((lbuf.i == (OldCi.i +1))&&(olddev.i == DevNum.i))
						{
							t = 6;
						}
						else
						{
							t = 10;
						}
					}
					else
					{
						flag = 0;
					}

#if APP_PRINTF
					{
						unsigned char i;
						printf("InitChas get data = %d\n",receive_len[0]);
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

			case 5:
				memset(buff1,0,sizeof(buff1));
				memset(buff2,0,sizeof(buff2));
				memset(buff3,0,sizeof(buff3));
				memcpy(buff1,Recv+11,4); //伪随机数
				memcpy(buff1+4,Recv+4,2); //钱包交易序号
				MoneyChange(DevSID.i,buff3);
				memcpy(buff1+6,buff3+2,2); //终端交易序号

				status = RunDes(0,0,buff1,buff2,8,Key+1,Keylen);
				if(status == MI_OK)
				{
					memset(buff1,0,sizeof(buff1));
					memset(buff3,0,sizeof(buff3));
					memset(Send,0,sizeof(Send));

					MoneyChange(Money,buff1);
					memcpy(Send,buff1,4); //交易金额
					Send[4] =0x06; //电子钱包消费
					memcpy(Send+5,PsamNum,6); //终端机号
					memcpy(Send+11,Timebuf,7); // 交易时间日期

					memset(buff1,0,sizeof(buff1));
					memset(buff3,0,sizeof(buff3));
					status = MacAnyLength(buff1,Send,buff3,18,buff2,8); //
					if(status == MI_OK)
					{
						memset(Send,0,sizeof(Send));
						memset(buff1,0,sizeof(buff1));

						memcpy(Send,DebitChas,sizeof(DebitChas));
						MoneyChange(DevSID.i,buff1);
						memcpy(Send+5,buff1,4);//终端交易序号
						memcpy(Send+9,Timebuf,7); //时间
						memcpy(Send+16,buff3,4); //MAC1
						Send[20]= 0x08; 		 //
						result = write(mf_fd,Send,21);
						if(result == MI_OK)
						{
							ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
							result = read(mf_fd, receive_buf, receive_len[0]);
							if(receive_len[0] > 2)
							{
								memcpy(Recv,receive_buf,receive_len[0]);
								memcpy(CardLan.CardTac,Recv,4); //TAC
								t++;
							}
							else if((receive_buf[0] == 0x93)&&(receive_buf[1] == 0x02))
							{
								Err_display(23);
                                Err_save(CARD_SPEC_CPU_PBCO20,23);
								t = 10;
								flag = 0;
							}
							else
							{
								Err_display(6);
                                Err_save(CARD_SPEC_CPU_PBCO20,6);
								t = 10;
								flag = 0;
							}



#if APP_PRINTF
							{
								unsigned char i;

								printf("DebitChas get data = %d\n",receive_len[0]);
								for(i =0 ; i < receive_len[0]; i ++)
								{
									printf("%02X ",receive_buf[i]);
								}
								printf("\n");
							}
#endif

						}
						else flag = 0;
					}
					else flag = 0;
				}
				else flag = 0;

				break;

			case 6:
				flag = 0;
				t = 0;
				break;

			default:
				flag = 0;
				break;
			}
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
	return t;
}



unsigned char SecondDebitChas_complex(unsigned int Money,char *Key,unsigned char Keylen)
{
	int result;
	unsigned short ic;
	unsigned char Timebuf[8];
	char Send[128],Recv[128];
	char write_data[48];
	char buff1[8],buff2[8],buff3[8];
	unsigned char flag,t,status,len,Qflag;
	unsigned char InitChas[]= {0x80,0x50,0x03,0x02,0x0b};
	unsigned char DebitChas[]= {0x80,0x54,0x01,0x00,0x0f};
	unsigned char update_capp_cache[] = {0x80,0xDC,0x09,0xB8,0x30};
	unsigned char verify_pin[]= {0x00,0x20,0x00,0x00,0x02,0x12,0x34};

	LongUnon vbuf,lbuf,olddev;
	LongUnon buf_Long;


#if APP_PRINTF
	printf("SecondDebitChas start  Money == %d \n",Money);
#endif

	LEDL(1);
	beepopen(10);
	PlayMusic(15, 0);

	SetColor(Mcolor);
	SetTextColor(Color_red);
    #ifdef NEW0409
	SetTextSize(48);
	TextOut(0 , 75, "温馨提示");
	TextOut(0 , 144, "刷卡错误");
	TextOut(0	, 210,"请重新刷卡");
    #else
	SetTextSize(32);
	TextOut(100 , 50, "温馨提示");
	TextOut(100 , 90, "刷卡错误");
	TextOut(85	, 130,"请重新刷卡");
    #endif

	flag = 1;
	t = 1;
	Qflag = 1;
	ic = 0;
	while(Qflag)
	{
		Rd_time (Timebuf+1);
		if(Time.sec != Timebuf[6])
		{
			ic++;
			if(ic>30)
			{
				Qflag =0;
				break;
			}

			if((ic%2))
			{
				beepopen(10);
			}
			else
			{
				beepopen(11);
			}

			Timebuf[0] = 0x20;
			Time.year = Timebuf[1];
			Time.month = Timebuf[2];
			Time.day = Timebuf[3];
			Time.hour = Timebuf[4];
			Time.min = Timebuf[5];
			Time.sec = Timebuf[6];
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
					flag = 0;
				}
				break;

			case 2:
				memcpy(Send,LanSec.ADFNUM,2);
				len = 2;
				status = SelectAppDF(Send,Recv,&len);
				if(status == MI_OK) {
#ifdef VERIFY_PIN
					{
						result = write(mf_fd,verify_pin,sizeof(verify_pin));
						if(result == MI_OK)
						{
							ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
							result = read(mf_fd, receive_buf, receive_len[0]);
							if((receive_buf[0] == 0x90)&&(receive_buf[1] == 0x00)) {
								t++;
							}
							else {
								flag = 0;
							}

						} else{
							flag = 0;
						}
					}
#else

					t++;
#endif
				}
				else flag = 0;
				break;

			case 3: // 读 00018 交易记录文件 最后一条
				memset(Send,0,sizeof(Send));
				Send[0] = 0x00;
				Send[1] = 0xb2; //
				Send[2] = 0x01; //
				Send[3] = 0xc4; // 0018
				Send[4] = 0x00;
				result = write(mf_fd,Send,5);
				if(result == MI_OK)
				{
					ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
					result = read(mf_fd, receive_buf, receive_len[0]);
					if(receive_len[0] > 2)
					{
						olddev.i = 0;
						memcpy(olddev.longbuf,receive_buf+12,4); //上次交易终端机号
						t++;
					}
					else
					{
						flag = 0;
					}

#if APP_PRINTF
					{
						unsigned char i;

						printf("Readcord 0018 data = %d\n",receive_len[0]);
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

			case 4:
				memset(buff1,0,sizeof(buff1));
				memset(Send,0,sizeof(Send));
				MoneyChange(Money,buff1);
				memcpy(Send,InitChas,sizeof(InitChas)); //命令头
				Send[5] = Key[0]; //密钥类型
				memcpy(Send+6,buff1,4); //消费金额
				memcpy(Send+10,PsamNum,6); //终端机号
				Send[16] = 0x0f; //
				len = 17;
				result = write(mf_fd,Send,len);
				if(result == MI_OK)
				{
					ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
					result = read(mf_fd, receive_buf, receive_len[0]);
					if(receive_len[0] > 2)
					{
						memcpy(Recv,receive_buf,receive_len[0]);
						vbuf.i = 0;
						lbuf.i = 0;
						memcpy(vbuf.longbuf,Recv,4);
						//memcpy(lbuf.longbuf,Recv+4,2);
						lbuf.longbuf[0] = Recv[5];
						lbuf.longbuf[1] = Recv[4];
						if((vbuf.i == OldCash.i)&&(lbuf.i == OldCi.i)) //上次没写进去
						{
#if 0
							memset(CardLan.ViewMoney,0,sizeof(CardLan.ViewMoney));
							CardLan.ViewMoney[0] = Recv[5];
							CardLan.ViewMoney[1] = Recv[4];
#endif
							t++;
						}
						else if((lbuf.i == (OldCi.i +1))&&(olddev.i == DevNum.i)) //上次写进去了
						{
							t = 8;
						}
						else //别的卡
						{
							t = 11;
						}
					}
					else
					{
						flag = 0;
					}

#if APP_PRINTF
					{
						unsigned char i;
						printf("InitChas get data = %d\n",receive_len[0]);
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

			case 5:
				memset(buff1,0,sizeof(buff1));
				memset(buff2,0,sizeof(buff2));
				memset(buff3,0,sizeof(buff3));
				memcpy(buff1,Recv+11,4); //伪随机数
				memcpy(buff1+4,Recv+4,2); //钱包交易序号
				MoneyChange(DevSID.i,buff3);
				memcpy(buff1+6,buff3+2,2); //终端交易序号

				status = RunDes(0,0,buff1,buff2,8,Key+1,Keylen);
				if(status == MI_OK)
				{
					memset(buff1,0,sizeof(buff1));
					memset(buff3,0,sizeof(buff3));
					memset(Send,0,sizeof(Send));

					MoneyChange(Money,buff1);
					memcpy(Send,buff1,4); //交易金额
					Send[4] =0x09; //复合 消费交易类型
					memcpy(Send+5,PsamNum,6); //终端机号
					memcpy(Send+11,Timebuf,7); // 交易时间日期

					memset(buff1,0,sizeof(buff1));
					memset(buff3,0,sizeof(buff3));
					status = MacAnyLength(buff1,Send,buff3,18,buff2,8); //
					if(status == MI_OK)
					{
						t++;
					}
					else flag = 0;
				}
				else flag = 0;
				break;

			case 6:
				memset(Send,0,sizeof(Send));
				memset(write_data,0,sizeof(write_data));
				memcpy(Send,update_capp_cache,sizeof(update_capp_cache));

				write_data[0] = 9;  //flag
				write_data[1] = 46; //data lenth
				write_data[2] = 0;  //lock
				write_data[3] = 0;  //version
				write_data[4] = CardLan.EnterExitFlag;
				memcpy(write_data+5,PsamNum,6); //mach id

				MoneyChange(Money,buff1);
				memcpy(write_data+11,buff1,4); //交易金额

				memcpy(write_data+15,Timebuf,7); //time


				memcpy(write_data+22,CardLan.ViewsValue,3); //period limit
				memcpy(write_data+25,CardLan.DayValue,3);	//day limit
				memcpy(write_data+28,CardLan.MonthValue,4); //month limit
				write_data[32] = CardLan.Period;		//

				write_data[33] = CardLan.StationID;	//station id
				write_data[34] = CardLan.EnterCarCi; //how many get on times
				write_data[35] = Section.Updown; //up or down line

				MoneyChange(cpu_money.Subsidies.i,buf_Long.longbuf);
				memcpy(write_data+36,buf_Long.longbuf,4);

				// added by taeguk
				memcpy(write_data+44,CardLan.ViewMoney,2);

				memcpy(Send+5,write_data,sizeof(write_data));	
				
				len = 53;
#if GC_DBG
				{
					int i;
					DBG_PRINTF("update_capp_cache write date\n");
					for(i =0 ; i < len; i++)
					{
						DBG_PRINTF("%02X ",Send[i]);
					}
					DBG_PRINTF("\n");
				}
#endif

				result = write(mf_fd,Send,len);
				if(result == MI_OK)
				{
					ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
					result = read(mf_fd, receive_buf, receive_len[0]);
					if((receive_buf[0] == 0x90)&&(receive_buf[1] == 0x00))
					{
						t++;
					}
					else
					{
						if(receive_len[0]>=2)
						{
							printf("MRecv again f2No:%02X%02X\n",receive_buf[receive_len[0]-2],receive_buf[receive_len[0]-1]);
						}
						else
						{
							printf("MRecv again f2No:0000\n");
						}
						Err_display(23);
                        Err_save(CARD_SPEC_CPU_PBCO20,23);
						flag = 0;
					}

#if APP_PRINTF
					{
						unsigned char i;
						printf("update_capp_cache get data = %d\n",receive_len[0]);
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
					//Err_display(56);
					flag = 0;
				}
				break;


			case 7:
				memset(Send,0,sizeof(Send));
				memcpy(Send,DebitChas,sizeof(DebitChas));
				MoneyChange(DevSID.i,buff1);
				memcpy(Send+5,buff1,4);//终端交易序号
				memcpy(Send+9,Timebuf,7); //时间
				memcpy(Send+16,buff3,4); //MAC1
				Send[20]= 0x08;
				result = write(mf_fd,Send,21);
				if(result == MI_OK)
				{
					ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
					read(mf_fd, receive_buf, receive_len[0]);
					if(receive_len[0] > 2)
					{
						t++;
						memcpy(Recv,receive_buf,receive_len[0]);
						memcpy(CardLan.CardTac,Recv,4); //TAC
					}
					else if((receive_buf[0] == 0x93)&&(receive_buf[1] == 0x02))
					{
						printf("MRecv f3No:%02X%02X",receive_buf[receive_len[0]-2],receive_buf[receive_len[0]-1]);
						Err_display(23);
                        Err_save(CARD_SPEC_CPU_PBCO20,23);
						flag = 0;
					}
					else
					{
						if(receive_len[0]>=2)
						{
							printf("MRecv f3No:%02X%02X",receive_buf[receive_len[0]-2],receive_buf[receive_len[0]-1]);
						}
						else
						{
							printf("MRecv f3No:0000");
						}
						Err_display(6);
                        Err_save(CARD_SPEC_CPU_PBCO20,6);
						flag = 0;
					}


#if APP_PRINTF
					{
						unsigned char i;
						printf("DebitChas get data = %d\n",receive_len[0]);
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
					printf("MRecv f3Err\n");
					flag = 0;

				}
				break;

			case 8:
				flag = 0;
				t = 0;
				break;

			default:
				flag = 0;
				break;
			}
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
unsigned char Card_DebitChas(unsigned int Money,char *Key,unsigned char Keylen)
{
	int result,i;
	unsigned char Timebuf[8];
	char Send[256],Recv[256];
	char buff1[8],buff2[8],buff3[8];
	unsigned char flag,t,status,len;
    
    
     DBG_PRINTF("Card_DebitChas() is called.\n");
//char *UserPIN,unsigned char pinlen


//unsigned char selfileDF[]={0x00,0xa4,0x04,0x00,0x09,0xA0,0x00,0x00,0x00,0x03,0x86,0x98,0x07,0x01,0x00};
//选择应用 DF01  电信应用(DF01 -- DF06)


//   unsigned char VerPIN[]= {0x00,0x20,0x00,0x00}; //,0x04,0x54,0x43,0x50,0x53};
//校验PIN

	unsigned char InitChas[]= {0x80,0x50,0x01,0x02,0x0b}; //,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x0f};
// 消费初始化  KEY:0x00  金额:0.01元 终端机号:1号

	unsigned char DebitChas[]= {0x80,0x54,0x01,0x00,0x0f}; //,0x00,0x00,0x00,0x01,0x20,0x12,0x09,0x24,0x15,0x26,0x00}; //+ MAC1
// 消费   交易序号: 1 交易时间:20120924152600 + MAC1


//unsigned char InitLoad[]={0x80,0x50,0x00,0x02,0x0b};//,0x01,0x00,0x00,0x00,0xff,0x00,0x00,0x00,0x00,0x00,0x01,0x10};
// 圈存初始化 KEY:0x01  金额:2.25元 终端机号:1号

//unsigned char CrebitChas[]={0x80,0x52,0x00,0x00,0x0b};//,0x20,0x12,0x09,0x24,0x15,0x26,0x00}; //+ MAC2
// 圈存  交易时间:20120924152600 + MAC2
	unsigned char SamMac1[]= {0x80,0x70,0x00,0x00,0x24};
	unsigned char SamMac2[]= {0x80,0x72,0x00,0x00,0x04};
    unsigned char selectapp[]  = {0x00,0xa4,0x00,0x00,0x02,0x10,0x01};
    
	Timebuf[0] = 0x20;
	Timebuf[1] = Time.year;
	Timebuf[2] = Time.month;
	Timebuf[3] = Time.day;
	Timebuf[4] = Time.hour;
	Timebuf[5] = Time.min;
	Timebuf[6] = Time.sec;

	flag = 1;
	t = 0;
	while(flag)
	{
		switch(t)
		{
        case 0://卡片交易计数器+1

                t++;
            break;
            
		case 1:
			memset(buff1,0,sizeof(buff1));
			memset(Send,0,sizeof(Send));
			MoneyChange(Money,buff1);
			memcpy(Send,InitChas,sizeof(InitChas)); //命令头
			Send[5] = 0x00;                         //密钥类型
			memcpy(Send+6,buff1,4);                 //消费金额
			memcpy(Send+10,PsamNum,6);              //终端机号
			Send[16] = 0x0f; //
			len = 17;
			result = write(mf_fd,Send,len);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
                DBG_PRINTF("消费初始化返回:");
					menu_print(receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
					t++;
					memcpy(Recv,receive_buf,receive_len[0]);
					OldCash.i = 0;
					OldCi.i = 0;
					memcpy(OldCash.longbuf,Recv,4);                    
                    MoneyChange(OldCash.i,CardLan.QCash);
						
					//memcpy(OldCi.longbuf,Recv+4,2);
					OldCi.longbuf[0] = Recv[5];
					OldCi.longbuf[1] = Recv[4];
                    
					DBG_PRINTF("OldC:%d \n", OldCi.i);
                    memcpy(CardLanCPU.offlineSN,&receive_buf[4],2);
					memset(CardLan.ViewMoney,0,sizeof(CardLan.ViewMoney));
					CardLan.ViewMoney[0] = Recv[5];
					CardLan.ViewMoney[1] = Recv[4];
                    CardLan.CountType = Recv[10];
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
					unsigned char i;
					DBG_PRINTF("InitChas get data = %d\n",receive_len[0]);
					for(i =0 ; i < receive_len[0]; i ++)
					{
						DBG_PRINTF("%02X ",receive_buf[i]);
					}
					DBG_PRINTF("\n");
				}
#endif
			}
			else flag = 0;
			break;

		case 2:                            
                memset(Recv,0,sizeof(Recv));
    			len = sizeof(selectapp);
    			status = PsamCos(selectapp,Recv,&len);
    			if((status == MI_OK)&&(Recv[len-2]== 0x90)&&(Recv[len-1]== 0x00))
    			{
                    memset(Send,0,sizeof(Send));  
    				memset(Send,0,sizeof(Send));
    				memcpy(Send,SamMac1,5);
    				memcpy(Send+5,Recv+11,4); //伪随机数
    				memcpy(Send+9,Recv+4,2); //伪随机数
    				memcpy(Send+11,buff1,4); //伪随机数
    				Send[15] = 0x09;
    				memcpy(Send+16,Timebuf,7);
    				memcpy(Send+23,Recv+9,2);
    				memcpy(Send+25,CardLanCPU.zappserialnumber,8);
    				memcpy(Send+33,"\x10\x00\xff\xff\x00\x00\x00\x00",8);    				
    				len = 41;
    				memset(Recv,0,sizeof(Recv));

    				status = PsamCos(Send,Recv,&len);

    				{
    					unsigned char i;
    					DBG_PRINTF("SamMac1(%d)\n",__LINE__);
    					for(i =0 ; i < 35; i ++)
    					{
    						DBG_PRINTF("%02X ",Send[i]);
    					}
    					DBG_PRINTF("\n");
    				}

    				if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
    				{
    					t ++;
    					memset(buff2,0,sizeof(buff2));
    					memset(Buf1.longbuf,0,4);
    					memcpy(Buf1.longbuf,Recv,4);
                        memcpy(CardLanCPU.PSAMOfflineSN,Recv,4);
    					MoneyChange(Buf1.i,buff2);
    					memcpy(DevSIDCPU.longbuf,buff2,4);   //终端交易序号
    				}
    				else
    				{
    					Err_display(60);
    					flag = 0;
    				}
                 }
                else
                {
                    flag = 0;
                    }
				break;

			case 3:
				memset(Send,0,sizeof(Send));
				memcpy(Send,DebitChas,sizeof(DebitChas));
				memcpy(Send+5,Recv,4);//终端交易序号
				memcpy(Send+9,Timebuf,7); //时间
				memcpy(Send+16,Recv+4,4); //MAC1
				Send[20]= 0x08;           //
				len =21;


				{
					unsigned char i;
					DBG_PRINTF("DebitChas send data = %d\n",len);
					for(i =0 ; i < len; i ++)
					{
						DBG_PRINTF("%02X ",Send[i]);
					}
					DBG_PRINTF("\n");
				}


                
				result = write(mf_fd,Send,len);
				if(result == MI_OK)
				{
                    ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				    result = read(mf_fd, receive_buf, receive_len[0]);

					{
						unsigned char i;
						DBG_PRINTF("DebitChas get data = %d\n",receive_len[0]);
						for(i =0 ; i < receive_len[0]; i ++)
						{
							DBG_PRINTF("%02X",receive_buf[i]);
						}
						DBG_PRINTF("\n");
					}

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
							DBG_PRINTF("MRecv f3No:%02X%02X",receive_buf[receive_len[0]-2],receive_buf[receive_len[0]-1]);
							Err_display(6);
							flag = 0;						
						}
					}
					else
					{
						DBG_PRINTF("MRecv f3No:0000");
						status = SecondDebitChas(Money,Key,Keylen);
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
					status = SecondDebitChas(Money,Key,Keylen);
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

            case 4://PSAM卡验证MAC2
    			memset(Send,0,sizeof(Send));
    			memcpy(Send,"\x80\x72\x00\x00\x04",5);		//命令头
    			memcpy(&Send[5],Recv+4,4);		//MAC2
    			len = 9;
                int i;
    			for(i=0;i<2;i++)
    			{
    				status = PsamCos(Send,Recv,&len);
    				if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
    				{
    					DBG_PRINTF("PSAM卡验证MAC2  SW1=%02X  SW2=%02X\n",Recv[len-2],Recv[len-1]);
    					break;
    				}
    			}
    			t++;
    			break;
                
           case 5:// 查询余额
				//printf("查询余额\n");				
				
				result = write(mf_fd,"\x80\x5c\x00\x02\x04",5);
				if(result == MI_OK)
				{
					ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
					result = read(mf_fd, receive_buf, receive_len[0]);
					if(receive_len[0] > 2)
					{
						t++;                       
						memcpy(CardLanCPU.beforemoney,receive_buf,4);                      
						printf("\n交易后余额:%d分\n",receive_buf[0]<<24|receive_buf[1]<<16|receive_buf[2]<<8|receive_buf[3]);
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


		case 6:
			t = 0;
			flag =0 ;
			break;

		default :
			flag =0 ;
			break;

		}
	}


#if APP_PRINTF
	printf("Card_DebitChas   End == %d \n",t);
#endif
    if(t>=4)
       {
        t = 0;
        i = Valueq.i-HostValue.i;
        DBG_PRINTF("刷卡过快余额:%d\n",i);
        MoneyChange(i,CardLanCPU.beforemoney);
        }
	return t;
}

unsigned char Card_DebitChas_complex_zhujian(unsigned int Money,char *Key,unsigned char Keylen)
{
	int result;
	unsigned char Timebuf[8];
	unsigned char Send[256],Recv[256];
	unsigned char flag,t,status,len,i;
    unsigned int beforeMoney;
    char selectapp[] = {0x00,0xa4,0x00,0x00,0x02,0x10,0x01};
    unsigned char up0005[]={0x04,0xd6,0x85,0x01,0x11};    
    unsigned char up0014[]={0x04,0xd6,0x94,0x00,0x0e};
    
	Timebuf[0] = 0x20;
	Timebuf[1] = Time.year;
	Timebuf[2] = Time.month;
	Timebuf[3] = Time.day;
	Timebuf[4] = Time.hour;
	Timebuf[5] = Time.min;
	Timebuf[6] = Time.sec;
	
	flag = 1;
    if((bStolenDis!=0)||(CardLanCPU.ytradestatus == 0x30))
	    t = 1;
    else
        t= 3;
	while(flag)
	{
			switch(t)
		{
			case 1://复合应用消费初始化
			memset(Send,0,sizeof(Send));
			memcpy(Send,"\x80\x50\x00\x02\x0B",5); 		//命令头
			Send[5] = 0x00;          					//密钥索引
			Send[6] = (Money&0xFF000000)>>24;
			Send[7] = (Money&0xFF0000)>>16;
			Send[8] = (Money&0xFF00)>>8;
			Send[9] = Money&0xFF;						//交易金额
			memcpy(&Send[10],PsamNum,6); 				//终端机号
			Send[16] = 0x0f; 							//Le
			len = 17;
            DBG_DATA_PRINTF("应用消费初始化发送:");
		    menu_print(Send, len);
			result = write(mf_fd,Send,len);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
					t++;
					DBG_DATA_PRINTF("应用消费初始化返回:");
					menu_print(receive_buf, receive_len[0]);
					memcpy(CardLanCPU.offlineSN,&receive_buf[4],2);					
					CardLanCPU.keyVersion = receive_buf[6];
					CardLanCPU.arithmeticLabel = receive_buf[7];
					memcpy(CardLanCPU.PRandom,&receive_buf[8],4);
				}
				else
				{
					printf("复合应用消费初始化SW1=%02X   SW2=%02X\n",receive_buf[0],receive_buf[1]);
					flag = 0;
				}
			}
			else flag = 0;
			break;
            
			case 2://PSAM卡生成MAC1
                memset(Recv,0,sizeof(Recv));
                len = sizeof(selectapp);
                status = PsamCos(selectapp,Recv,&len);
                if((status == MI_OK)&&(Recv[len-2]== 0x90)&&(Recv[len-1]== 0x00))
                {
    			    DBG_DATA_PRINTF("初始化后的ATC1:%02x %02x\n",CardLanCPU.offlineSN[0],CardLanCPU.offlineSN[1]);
        			memset(Send,0,sizeof(Send));
        			memcpy(Send,"\x80\x70\x00\x00\x24",5);			//命令头
        			memcpy(&Send[5],CardLanCPU.PRandom,4);			//用户卡片随机数
        			memcpy(&Send[9],CardLanCPU.offlineSN,2);		//用户卡脱机交易序号
        			Send[11] = (Money&0xFF000000)>>24;
        			Send[12] = (Money&0xFF0000)>>16;
        			Send[13] = (Money&0xFF00)>>8;
        			Send[14] = Money&0xFF;							//交易金额
        			Send[15] = 0x09;								//交易类型
        			memcpy(&Send[16],Timebuf,7);					//交易日期时间
        			Send[23] = CardLanCPU.keyVersion;					//密钥版本号
        			Send[24] = CardLanCPU.arithmeticLabel;				//密钥算法标识
        			memcpy(&Send[25],&CardLanCPU.zappserialnumber,8);//用户卡号
        			memcpy(&Send[33],"\x10\x00\xff\xff\x00\x00\x00\x00",8);		//发卡机构编码
        			len = 41;
        			memset(Recv,0,sizeof(Recv));

                    DBG_DATA_PRINTF("生成MAC1发送:");
        		    menu_print(Send, len);
                    status = PsamCos(Send,Recv,&len);
                    
        			DBG_DATA_PRINTF("生成MAC1返回:");
        		    menu_print(Recv, len);
        	
                    if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
        			{
        				t++;
        		//		printf("PSAM卡计算MAC1返回:");
        				menu_print(Recv, len);
        				memcpy(CardLanCPU.PSAMOfflineSN,Recv,4);
        				memcpy(CardLanCPU.MAC1,&Recv[4],4);
        			}
        			else flag = 0;
                 }
                else
                    flag = 0;
			break;          
			
			case 3://用户卡更新0014文件
            if(CardLanCPU.ytradestatus == 0x20)                     //上车需要更新0014文件
            {
                memset(Send,0,sizeof(Send));
			    memcpy(Send,up0014,5); 		                        //命令头
			    memcpy(Send+5,&CardLanCPU.ygetontime[0],5);         //上车时间
                Send[10]=CardLanCPU.ygetonstation;                  //上车站点     
                Send[11]=CardLanCPU.ygetonstation;                  //上车方向 
                memcpy(Send+12,CardLanCPU.ymarkamount,2);           //标注金额
                memcpy(Send+14,CardLanCPU.ygetonlinenum,2);         //上车线路号
                memcpy(Send+16,CardLanCPU.ygetonvehiclnum,3);       //上车车辆号
                status = Upnomalfile(Send, 19);
                if(status==0)
                    t++;
                else
                {
                    flag = 0;
                    }
                
                }
            else
                t++;            
			break;

            case 4://用户卡更新0005文件                           
                memset(Send,0,sizeof(Send));
			    memcpy(Send,up0005,5); 		              //命令头
			    memcpy(Send+5,&CardLanCPU.ydata0005[1],17);
                status = Upnomalfile(Send, 22);
                if(status==0)
                    {
                        if((bStolenDis>0)||(CardLanCPU.ytradestatus==0x30))
                                t++;
                        else
                            t=7;
                    }
                else
                    flag = 0;
                
            break;

			case 5://执行扣款
			 DBG_DATA_PRINTF("初始化后的ATC2:%02x %02x\n",CardLanCPU.offlineSN[0],CardLanCPU.offlineSN[1]);
			memset(Send,0,sizeof(Send));
			memcpy(Send,"\x80\x54\x01\x00\x0F",5); 		    //命令头
			memcpy(&Send[5],CardLanCPU.PSAMOfflineSN,4);	//终端交易序号
			memcpy(&Send[9],Timebuf,7); 				    //终端交易日期时间
			memcpy(&Send[16],CardLanCPU.MAC1,4);		    //MAC1
			
			Send[20] = 0x08; 							    //Le
			len = 21;
            DBG_DATA_PRINTF("执行扣款发送:");
		    menu_print(Send, len);
			result = write(mf_fd,Send,len);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
					t++;
					DBG_DATA_PRINTF("执行扣款返回:");
					menu_print(receive_buf, receive_len[0]);
					memcpy(CardLanCPU.TAC,receive_buf,4);
					memcpy(CardLanCPU.MAC2,&receive_buf[4],4);
				}
				else if(receive_len[0] == 2)
				{
					printf("执行扣款SW1=%02X   SW2=%02X\n",receive_buf[0],receive_buf[1]);
					flag = 0;
				}
				else
				{
					//防插拔处理
					status = SecondDebitChas_jiaotong(Money,Key,Keylen);
					if(status == 0)
						t ++;
					else if(status == 0xFF)
						{
                            t = 1;
                            flag=0;
                            
                       }
					else
						{
                            t = 0xAA;
                            flag=0;
                       }
				}
			}
			else 
                //flag = 0;
			{
                    
					status = SecondDebitChas_jiaotong(Money,Key,Keylen);
                    DBG_DATA_PRINTF("重新刷卡返回:%02x\n",status);
					if(status == 0)
						t ++;
					else if(status == 0xFF)
						{
                            t = 1;
                            flag=0;
                            
                       }
					else
						{
                            t = 0xAA;
                            flag=0;
                       }                
                }
			
			break;
            
            case 6://PSAM卡验证MAC2
			memset(Send,0,sizeof(Send));
			memcpy(Send,"\x80\x72\x00\x00\x04",5);		//命令头
			memcpy(&Send[5],CardLanCPU.MAC2,4);		//MAC2
			len = 9;
			for(i=0;i<2;i++)
			{
				status = PsamCos(Send,Recv,&len);
				if((status == MI_OK)&&(Recv[len-2] == 0x90)&&(Recv[len-1] == 0x00))
				{
					printf("PSAM卡验证MAC2  SW1=%02X  SW2=%02X\n",Recv[len-2],Recv[len-1]);
					break;
				}
			}
			t++;
			break;
		           
		default:
			flag=0;
			t=0;
			break;
		}
	}
#if APP_PRINTF
	printf("Card_DebitChas_complex   End == %d \n",t);
#endif

	return t;
}


unsigned char TopUpCardInfor_CPU(int type)
{
	unsigned char status = 1;
	unsigned char keybuff[20];

	keybuff[0] = 0x01;
#if KEYTEST

	memcpy(keybuff+1,MmasterKey,16);

#else
	CardKey(keybuff+1);

#endif

//   printf("CardKey: \n");


	if(type == 0) {		
		//status  = Card_DebitChas_complex(HostValue.i,keybuff,16);
	}
	else if(type == 1) {
		status  = Card_DebitChas_complex_zhujian(HostValue.i,keybuff,16);
	}
	else if(type == 2)
		status  = Card_DebitChas(HostValue.i,keybuff,16);
	else {
		printf("impossible at TopUpCardInfor_CPU\n");
	}
	return status;
}

unsigned char SupportType_Cpu_jiaotong(unsigned char cardtype)
{
	
	unsigned char i = 0,status = 1;
	unsigned char card_type;
    unsigned char NandBuf[13];
    
    card_type=cardtype;
    DBG_PRINTF("card_type=%02x\n",card_type);
	for(i=0;i<13;i++)
    {
	    NandBuf[i]=CardLanBuf[1152+i*48+1];
        }
    DBG_PRINTF("支持的卡类:");
	menu_print(NandBuf, 15);
	for(i = 0; i< 13; i++)
	{
    		//  printf("%02X",NandBuf[i]);
    		if(NandBuf[i] == 0x0) 
		        break;
    		else
    		{
        		if(NandBuf[i] == card_type)
			status = 0;
    		}
    		if(status == MI_OK)
		break;
	}

    	return status;
}

unsigned char SupportType_Cpu_zhujian(unsigned char cardtype)
{
	
	unsigned char i = 0,status = 1;
	unsigned char card_type;
    unsigned char NandBuf[13];
    
    card_type=cardtype;
    DBG_PRINTF("card_type=%02x\n",card_type);

    for(i=0;i<13;i++)
    {
	    NandBuf[i]=CardLanBuf[512+i*48+1];
        }
    DBG_PRINTF("支持的卡类:");
	menu_print(NandBuf, 13);
	for(i = 0; i< 13; i++)
	{
    		//  printf("%02X",NandBuf[i]);
    		if(NandBuf[i] == 0) 
		        break;
    		else
    		{
        		if(NandBuf[i] == card_type)
			    status = 0;
    		}
    		if(status == MI_OK)
		break;
	}

    	return status;
}

unsigned char ReadCardInfor_CPU(void)
{
	int result;
	char Recv[256];
	char binname[20];  
	unsigned char flag,t,status,len;
    int ci;
    ShortUnon tmp,tmp1,startadd;
    char buff[13*48];
    char buff1[169*24]; 
    char buff2[500];
    char buff3[65535];
	LongUnon Value;   
    char tempbuf[8],tempbuf1[8];   
    char data[50],plen;
    unsigned char i; 
    WhiteItem item;
	DBG_PRINTF("ReadCardInfor_CPU() is called.\n");
    unsigned char verify_pin[]= {0x00,0x20,0x00,0x00,0x02,0x12,0x34};

	flag = 1;
    t = CheckAppDF();
    if(t==0xff)
      flag = 0;
	while(flag)
	{
        DBG_PRINTF("use_jiaotong_stander=%02x\n",use_jiaotong_stander);
		switch(t)
		{
        case 0:
                len = strlen(PPSE);
                status = SelectAppDF(PPSE,Recv,&len);                
                status = SelectAID(Recv, &len, binname, &plen);		
                DBG_PRINTF("status = %d:line=%d\n",status,__LINE__);     
                if(status == MI_OK)
                {                    
                        len = plen; 
    					memcpy(PsamNum,PsamNum_bak2,sizeof(PsamNum_bak2));					
    					DBG_PRINTF("使用交通部cpu卡流程00:line=%d\n",__LINE__);
                        use_jiaotong_stander=0x55;
                    }
                else
                {            
                    memcpy(binname,"\xA0\x00\x00\x06\x32\x01\x01\x05",8);
					len=8;
                    memcpy(PsamNum,PsamNum_bak2,sizeof(PsamNum_bak2));					
					DBG_PRINTF("使用交通部cpu卡流程00:line=%d\n",__LINE__);
                    use_jiaotong_stander=0x55;
                }
            t++;
            break;
            
    	case 1:		                  
                status=SelectAppDF(binname,Recv,&len);
                if(status!=MI_OK)
                {
                    Err_display(30);
    				flag=0;
    				}
                else 
                    {
                        if((receive_buf[len-2]==0x90)&&(receive_buf[len-1]==0x00))
                       {
                        memcpy(PsamNum,PsamNum_bak2,sizeof(PsamNum_bak2));					
                        DBG_PRINTF("使用交通部流程02:line=%d\n",__LINE__);
                        }
                    else if(((receive_buf[len-2]==0x62)&&(receive_buf[len-1]==0x84))||
                             ((receive_buf[len-2]==0x93)&&(receive_buf[len-1]==0x03)))
                        {
                            DBG_PRINTF("出错6284或9303\n");
                             use_jiaotong_stander=0x0;
                             Err_display(47);
                            flag=0; 
                            break;
                            }                        
                    else if((receive_buf[len-2]==0x62)&&(receive_buf[len-1]==0x83))
                        {
                            DBG_PRINTF("出错6283\n");
                             use_jiaotong_stander=0x0;
                             Err_display(10);
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
		    if((status==MI_OK)&&((mystrncmp(binname,"\xA0\x00\x00\x06\x32\x01\x01\x05",8)==0)))           
            {
				//交通部规范
				memcpy(PsamNum,PsamNum_bak2,sizeof(PsamNum_bak2));
				t=10;
				use_jiaotong_stander=0x55;
			}
            break;

        //本地卡用户卡处理过程
        case 2://判定启用标志
            if(CardLanCPU.zcardstatus!=0x02)
            {
                Err_display(71);
                flag = 0;
                use_jiaotong_stander=0x0;
                }
            else
                t++;
            break;

        case 3://判定起用日期和失效日期
            status = Card_JudgeDate_jiaotong(CardLanCPU.zstarttime,CardLanCPU.zendtime);
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

        case 4://判定黑名单标志
            memset(binname,0,sizeof(binname));
            memset(CardLanCPU.ydata0005,0,sizeof(CardLanCPU.ydata0005));
			memcpy(binname,ReadFiley05,5);
			result = write(mf_fd,binname,5);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{	
                    memcpy(CardLanCPU.ydata0005,receive_buf,18);
					CardLanCPU.yblackflag = receive_buf[0];
                    memcpy(CardLanCPU.ycardtransnum,receive_buf+1,4);
                    CardLanCPU.ytradestatus = receive_buf[9];
                    memcpy(CardLanCPU.ytradetime,receive_buf+10,5);
				}
				else
				{
					flag = 0;
				}
				{
					

					DBG_PRINTF("ReadFiley05 data = %d\n",receive_len[0]);
					for(i =0 ; i < receive_len[0]; i ++)
					{
						DBG_PRINTF("%02X ",receive_buf[i]);
					}
					DBG_PRINTF("\n");
				}

			}
			else flag = 0;            
            if(CardLanCPU.yblackflag==0xa5)
            {
                Err_display(10);                
				use_jiaotong_stander=0;
				flag = 0;
                }
            else
                t++;                
            break;

        case 5://判定可用卡类型参数
             status  = SupportType_Cpu_zhujian(CardLanCPU.zcardtype);    //待根据卡类型参数列表查询
			if(status == MI_OK)
				t++;
			else
			{
                if(flc0005.glocalnodefinecard==0)
				{
                    Err_display(11);
				    use_jiaotong_stander=0;
				    flag = 0;
                    }
                else
                    t++;
			}
            break;

        case 6:
            //判断卡片是否是黑名单卡
			//printf("判断黑名单\n");
			status  = Card_JudgeCsn_zhujian();
			if(status == MI_OK)t++;
			else
			{
                //是黑名单,则临时锁定应用
                status=GET_MAC_zhujian();
                if(status==0)
				{
                    Err_display(10);
                    /***锁卡后需要增加记录
                    
                    ***/
    				use_jiaotong_stander=0;
    				flag = 0;
                    }
                else
                    flag = 0;
			}
            break;
            

		case 7: // 读 0014 读公交过程文件
			memset(binname,0,sizeof(binname));
			memcpy(binname,ReadFile14,5);
			result = write(mf_fd,binname,5);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if(receive_len[0] > 2)
				{
					t++;
                    memcpy(CardLanCPU.ygetontime,receive_buf,5);
                    CardLanCPU.ygetonstation = receive_buf[5];
                    CardLanCPU.ydirectionflag = receive_buf[6];
                    memcpy(CardLanCPU.ymarkamount,receive_buf+7,2);
                    memcpy(CardLanCPU.ygetonlinenum,receive_buf+9,2);
                    memcpy(CardLanCPU.ygetonvehiclnum,receive_buf+11,3);					
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

        case 8:   //查询余额
                result = write(mf_fd,"\x80\x5c\x00\x02\x04",5);
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
            break;

		case 9:
			flag = 0;
		//	t  = 0;
			break;

        //交通部互通卡处理过程
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
			//交通部测试暂时关掉
            if(memcmp(CardLanCPU.issuerlabel,localissuerlabel,8)==0)		//如果是本地发行的卡，则不用判定白名单
                t++;
             else
            {
                status  = Card_White_Cpu_jiaotong();
    			if(status == MI_OK)t++;
    			else
    			{
    				Err_display(65);
    				use_jiaotong_stander=0;
    				flag = 0;
    			}
               }

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
            if(Check_CardDate(CardLanCPU.appstarttime,CardLanCPU.appendtime)!=0)
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
			status = Card_JudgeDate_jiaotong(CardLanCPU.appstarttime,CardLanCPU.appendtime);
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

		break;

		case 17:
			//判断卡片是否是黑名单卡
			//printf("判断黑名单\n");
			status  = Card_JudgeCsn_Cpu_jiaotong();
			if(status == MI_OK)t++;
			else
			{
                //是黑名单,则临时锁定应用
                    GET_MAC();
				Err_display(10);
                /***锁卡后需要增加记录

                ***/
				use_jiaotong_stander=0;
				flag = 0;
			}
			break;
              
		case 18:
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
            
		case 19:
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

            case 20:// 查询余额
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
            
            case 21:                
            //本地卡查询一卡通私有应用
            if(CardLanCPU.IsLocalCard==1)   
			{
                status = SelectAppDF(PrivateAID,Recv,&len);   
                if(status==0)
                {
                    memset(binname,0,sizeof(binname));
        			binname[0] = 0x00;
        			binname[1] = 0xb2; 
        			binname[2] = 0x01; 
        			binname[3] = 0x14; // 0002
        			binname[4] = 0x14;
        			result = write(mf_fd,binname,5);
        			if(result == MI_OK)
        			{
                        ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
        				result = read(mf_fd, receive_buf, receive_len[0]);
        				if(receive_len[0] > 2)
        				{
                            CardLanCPU.logiccardtype = receive_buf[2];
                            CardLanCPU.littlecardtype = receive_buf[3];
                            }
                     }
                    else
                    {
                        flag = 0;
                        use_jiaotong_stander=0;
                        }
                     status  = SupportType_Cpu_jiaotong(CardLanCPU.logiccardtype);    //待根据卡类型参数列表查询
        			if(status == MI_OK)
        				t++;
        			else
        			{
                        if(flc0005.glocalnodefinecard==0)
            				{
                                Err_display(11);
            				    use_jiaotong_stander=0;
            				    flag = 0;
                                }
                            else
                                t++;      
        				
        			}
                     
                }
            else
            {
                Err_display(52);
    			use_jiaotong_stander=0;
    			flag = 0;
                }
             }
            else
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

            case 22:
                //读取0002文件中的第6条记录
                memset(binname,0,sizeof(binname));
    			binname[0] = 0x00;
    			binname[1] = 0xb2; 
    			binname[2] = 0x06; 
    			binname[3] = 0x14; // 0002
    			binname[4] = 0x14;
    			result = write(mf_fd,binname,5);
    			if(result == MI_OK)
    			{
                    ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
    				result = read(mf_fd, receive_buf, receive_len[0]);
    				if(receive_len[0] > 2)
    				{
                        CardLanCPU.quickpassflag = receive_buf[2];
                        memcpy(CardLanCPU.qpstarttime,receive_buf+3,4);
                        memcpy(CardLanCPU.qpendtime,receive_buf+7,4);
                        }
                 }
                else
                {
                    flag = 0;
                    use_jiaotong_stander=0;
                    }                 

                if(CardLanCPU.quickpassflag!=0x01)
                    {
                        status = SelectAppDF(TunionAID,Recv,&len);  
                        if(status==0)
                            t++;
                        else
                           {
                            flag = 0;
                            use_jiaotong_stander=0;
                            } 
                    }

                break;
			
			case 23:
				flag = 0;
			//	t  = 0;
				break;  

            #if 0// VERIFY_PIN
                case 24:
				result = write(mf_fd,verify_pin,sizeof(verify_pin));
				if(result == MI_OK)
				{
					ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
					result = read(mf_fd, receive_buf, receive_len[0]);
				{
					unsigned char i;

					printf("Read verify_pin data = %d\n",receive_len[0]);
					for(i =0 ; i < receive_len[0]; i ++)
					{
						printf("%02X ",receive_buf[i]);
					}
					printf("\n");
				}                    
					if((receive_buf[0] == 0x90)&&(receive_buf[1] == 0x00))
					{
						printf("校验引脚成功\n");
						t++;
					}
					else{
						printf("引脚校验失败\n");
						flag = 0;
					}
					
				}
				else{
					printf("引脚校验失败\n");
					flag = 0;
				}

            break;
            #endif


            //本地卡功能卡处理过程(费率卡)            
            case 24:
            memset(binname,0,sizeof(binname));            
			memcpy(binname,"\x00\xb0\x85\x00\x22",5);
			result = write(mf_fd,binname,5);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if((receive_len[0] > 2)&&(receive_buf[receive_len[0]-2]==0x90)&&(receive_buf[receive_len[0]-1]==0x00))
				{	
                    
					CardLanCPU.yblackflag = receive_buf[0];
                    CardLanCPU.gcardtype = receive_buf[1];
					memcpy(CardLanCPU.glinenum,receive_buf+10,2);
					CardLanCPU.gupstationnum = receive_buf[11];
					CardLanCPU.gdownstationnum = receive_buf[12];
					memcpy(CardLanCPU.greleasever,receive_buf+13,2);
					CardLanCPU.glineattr = receive_buf[16];
					CardLanCPU.glocalnodefinecard = receive_buf[17];
					CardLanCPU.gremotnodefinecard = receive_buf[18];
					memcpy(CardLanCPU.gbasicpice,receive_buf+19,2);
					memcpy(CardLanCPU.goperatornum,receive_buf+21,2);
					CardLanCPU.gvehicleattr = receive_buf[23];
					CardLanCPU.gyidibupiaomo = receive_buf[24];
					memcpy(CardLanCPU.glinefilesize,receive_buf+25,2);
					memcpy(CardLanCPU.gbupiaolimittime,receive_buf+27,2);
					CardLanCPU.gruleofupanddowm = receive_buf[29];
					memcpy(CardLanCPU.gbubiaodiscountrateflag,receive_buf+30,2);					
					memcpy(CardLanCPU.gfuncflag,receive_buf+32,2);
                    memset(&flc0005,0,sizeof(FLC0005));
					memcpy(&flc0005,receive_buf,34);
				}
				else
				{
					flag = 0;
				}
				{
					

					DBG_PRINTF("ReadFiley05 data = %d\n",receive_len[0]);
					for(i =0 ; i < receive_len[0]; i ++)
					{
						DBG_PRINTF("%02X ",receive_buf[i]);
					}
					DBG_PRINTF("\n");
				}

			}
			else flag = 0;            
            if(CardLanCPU.yblackflag==0xa5)
            {
                Err_display(10);                
				use_jiaotong_stander=0;
				flag = 0;
                }
            else
                {   
                    t++;                
                    ReadandWriteBasicRateFile(0);
                }
                break;
                
        case 25://本地卡类型折扣率文件
            memset(binname,0,sizeof(binname));            
			memcpy(binname,"\x00\xb0\x86\x00\x20",5);                   //读取文件头
			result = write(mf_fd,binname,5);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if((receive_len[0] > 2)&&(receive_buf[receive_len[0]-2]==0x90)&&(receive_buf[receive_len[0]-1]==0x00))
				{
                    memcpy(&flc0006,receive_buf,32);
                    t++;
                    }
             }
            else
            {
                Err_display(74);
                use_jiaotong_stander=0;
				flag = 0;
                }
            break;

        case 26:
            memset(buff,0,sizeof(buff));
            tmp.i = 0;
            memcpy(tmp.intbuf,flc0006.totolnum,2);
            DBG_PRINTF("包记录总数=%d\n",tmp.i);
            startadd.i = 0;            
            memcpy(startadd.intbuf,flc0006.recordstartadd,2);
            DBG_PRINTF("记录起始地址=%d\n",startadd.i);
           
            ci = 0;
            ci = tmp.i/4;
            if(tmp.i%4)
                ci++;
            for(i=0;i<ci;i++)
            {
                memset(binname,0,sizeof(binname));
    			binname[0] = 0x00;
    			binname[1] = 0xb0; 
    			binname[2] = 0x86; 
    			binname[3] = startadd.i+i*ci*48;
    			binname[4] = 4*48;
    			result = write(mf_fd,binname,5);
    			if(result == MI_OK)
    			{
                    if((receive_len[0] > 2)&&(receive_buf[receive_len[0]-2]==0x90)&&(receive_buf[receive_len[0]-1]==0x00))
    				{
                        memcpy(buff+i*4*48,receive_buf,receive_buf[0]-2);
                        DBG_PRINTF("0005每次读取到的记录数据:");
                        menu_print(buff, receive_buf[0]-2);
                        }
                    else
                        {
                            Err_display(74);
                            use_jiaotong_stander=0;
				            flag = 0;
                        }
                 }
                
                }
            memcpy(CardLanBuf+512,buff,tmp.i*48);
            WriteCardRate_Para(0);
            break;

        case 27://本地卡互通城市白名单文件
            memset(binname,0,sizeof(binname));            
			memcpy(binname,"\x00\xb0\x87\x00\x20",5);                   //读取文件头
			result = write(mf_fd,binname,5);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if((receive_len[0] > 2)&&(receive_buf[receive_len[0]-2]==0x90)&&(receive_buf[receive_len[0]-1]==0x00))
				{
                    memcpy(&flc0007,receive_buf,32);
                    t++;
                    }
             }
            else
            {
                Err_display(75);
                use_jiaotong_stander=0;
				flag = 0;
                }
            break;

      case 28:
            memset(buff1,0,sizeof(buff1));
            tmp.i = 0;
            memcpy(tmp.intbuf,flc0007.totolnum,2);
            DBG_PRINTF("包记录总数=%d\n",tmp.i);
            startadd.i = 0;            
            memcpy(startadd.intbuf,flc0007.recordstartadd,2);
            DBG_PRINTF("记录起始地址=%d\n",startadd.i);           
            ci = 0;
            ci = tmp.i/8;
            if(tmp.i%8)
                ci++;
            for(i=0;i<ci;i++)
            {
                memset(binname,0,sizeof(binname));
    			binname[0] = 0x00;
    			binname[1] = 0xb0; 
    			binname[2] = 0x86; 
    			binname[3] = startadd.i+i*ci*24;
    			binname[4] = 8*24;
    			result = write(mf_fd,binname,5);
    			if(result == MI_OK)
    			{
                    if((receive_len[0] > 2)&&(receive_buf[receive_len[0]-2]==0x90)&&(receive_buf[receive_len[0]-1]==0x00))
    				{
                        memcpy(buff1+i*8*24,receive_buf,receive_buf[0]-2);
                        DBG_PRINTF("0006每次读取到的记录数据:");
                        menu_print(buff1, receive_buf[0]-2);
                        }
                    else
                        {
                            Err_display(75);
                            use_jiaotong_stander=0;
				            flag = 0;
                        }
                 }
                
                }
           for(i = 0; i < tmp.i; i++)
			{				
					memcpy(item.dat, buff1[0+i*12],sizeof(item));
					update_sortfile_white(item, 0);
				
			}
			SavetWhiteListBuff();
           
            break;  

       case 29://交通互通卡类型折扣率文件
            memset(binname,0,sizeof(binname));            
			memcpy(binname,"\x00\xb0\x88\x00\x20",5);                   //读取文件头
			result = write(mf_fd,binname,5);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if((receive_len[0] > 2)&&(receive_buf[receive_len[0]-2]==0x90)&&(receive_buf[receive_len[0]-1]==0x00))
				{
                    memcpy(&flc0008,receive_buf,32);
                    t++;
                    }
             }
            else
            {
                Err_display(76);
                use_jiaotong_stander=0;
				flag = 0;
                }
            break;

        case 30:
            memset(buff,0,sizeof(buff));
            tmp.i = 0;
            memcpy(tmp.intbuf,flc0008.totolnum,2);
            DBG_PRINTF("包记录总数=%d\n",tmp.i);
            startadd.i = 0;            
            memcpy(startadd.intbuf,flc0008.recordstartadd,2);
            DBG_PRINTF("记录起始地址=%d\n",startadd.i);
           
            ci = 0;
            ci = tmp.i/4;
            if(tmp.i%4)
                ci++;
            for(i=0;i<ci;i++)
            {
                memset(binname,0,sizeof(binname));
    			binname[0] = 0x00;
    			binname[1] = 0xb0; 
    			binname[2] = 0x86; 
    			binname[3] = startadd.i+i*ci*48;
    			binname[4] = 4*48;
    			result = write(mf_fd,binname,5);
    			if(result == MI_OK)
    			{
                    if((receive_len[0] > 2)&&(receive_buf[receive_len[0]-2]==0x90)&&(receive_buf[receive_len[0]-1]==0x00))
    				{
                        memcpy(buff+i*4*48,receive_buf,receive_len[0]-2);
                        DBG_PRINTF("0008每次读取到的记录数据:");
                        menu_print(buff, receive_buf[0]-2);
                        }
                    else
                        {
                            Err_display(76);
                            use_jiaotong_stander=0;
				            flag = 0;
                        }
                 }
                
                }
            memcpy(CardLanBuf+1152,buff,tmp.i*48);
            WriteCardRate_Para(1);
            break;

         case 31://公里数文件头
            memset(buff2,0,sizeof(buff2));
            memset(binname,0,sizeof(binname));            
			memcpy(binname,"\x00\xb0\x89\x00\x0e",5);                   //读取文件头
			result = write(mf_fd,binname,5);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if((receive_len[0] > 2)&&(receive_buf[receive_len[0]-2]==0x90)&&(receive_buf[receive_len[0]-1]==0x00))
				{
                    memcpy(&flc0009,receive_buf,14);
                    DBG_PRINTF("0009读取到的记录数据:");
                    menu_print(buff, receive_buf[0]-2);
                    memcpy(buff2,receive_buf+3,6);
                    
                    tmp.i = 0;
                    memcpy(tmp.intbuf,receive_buf+12,2);
                    //改写成写参数型式
                    //WriteSationDis_Para(0, buff2,12,0);
                    t++;
                    }
             }
            else
            {
                Err_display(77);
                use_jiaotong_stander=0;
				flag = 0;
                }
            break;

        case 32://上行公里数 
            memset(buff2,0,sizeof(buff2));
            memset(binname,0,sizeof(binname));            
			memcpy(binname,"\x00\xb0\x89\x0f",4);                   //读取文件头
			binname[4]=tmp.i+5;                                     //读取上的公里数列表+CRC+0xA2+A2的长度
			result = write(mf_fd,binname,5);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if((receive_len[0] > 2)&&(receive_buf[receive_len[0]-2]==0x90)&&(receive_buf[receive_len[0]-1]==0x00))
				{
                    tmp1.i = 0;
                    memcpy(tmp1.intbuf,receive_buf+binname[4]-2,2); //A2的数据长度               
                    memcpy(buff2,receive_buf,tmp.i); 
                    DBG_PRINTF("0009读取到的记录数据:");
                    menu_print(buff2, tmp.i);
                    WriteSationDis_Para(0, buff2,tmp.i);                    
                    t++;
                    }
             }
            else
            {
                Err_display(77);
                use_jiaotong_stander=0;
				flag = 0;
                }            
            break;
            
        case 33://下行公里数
            memset(buff2,0,sizeof(buff2));
            memset(binname,0,sizeof(binname));            
			memcpy(binname,"\x00\xb0\x89",3);                   //读取文件头
			binname[3] = 14+tmp.i+5;
			binname[4] = tmp1.i;                                //读取下行的公里数据
			result = write(mf_fd,binname,5);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if((receive_len[0] > 2)&&(receive_buf[receive_len[0]-2]==0x90)&&(receive_buf[receive_len[0]-1]==0x00))
				{                    
                    memcpy(buff2,receive_buf,tmp1.i); 
                    DBG_PRINTF("0009读取到的记录数据:");
                    menu_print(buff2, tmp1.i);
                    WriteSationDis_Para(1, buff2,tmp1.i);
                    t++;
                    }
             }
            else
            {
                Err_display(77);
                use_jiaotong_stander=0;
				flag = 0;
                } 

            break;

        case 34://上行票价
            memset(buff3,0,sizeof(buff3));
            memset(binname,0,sizeof(binname));            
			memcpy(binname,"\x00\xb0\x8a\x00\x00",5);                   //读取文件头
			result = write(mf_fd,binname,5);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if((receive_len[0] > 2)&&(receive_buf[receive_len[0]-2]==0x90)&&(receive_buf[receive_len[0]-1]==0x00))
				{
                    
                    
                    memcpy(buff3,receive_buf,receive_len[0]);
                    DBG_PRINTF("0010读取到的记录数据:");
                    menu_print(buff3, receive_buf[0]);       
                    tmp.i = 0;
                    memcpy(tmp.intbuf,receive_buf+1,2);
                    WriteSection_Para(0, buff3+3,tmp.i,0);
                    t++;
                    }
             }
            else
            {
                Err_display(78);
                use_jiaotong_stander=0;
				flag = 0;
                }
            break;

        case 35://下行票价
            memset(buff3,0,sizeof(buff3));
            memset(binname,0,sizeof(binname));            
			memcpy(binname,"\x00\xb0\x8b\x00\x00",5);                   //读取文件头
			result = write(mf_fd,binname,5);
			if(result == MI_OK)
			{
				ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
				result = read(mf_fd, receive_buf, receive_len[0]);
				if((receive_len[0] > 2)&&(receive_buf[receive_len[0]-2]==0x90)&&(receive_buf[receive_len[0]-1]==0x00))
				{
                    
                    
                    memcpy(buff3,receive_buf,receive_len[0]);
                    DBG_PRINTF("0011读取到的记录数据:");
                    menu_print(buff3, receive_buf[0]);       
                    tmp.i = 0;
                    memcpy(tmp.intbuf,receive_buf+1,2);
					tmp1.i = 0;
					memcpy(tmp1.intbuf,filem4.downrecordnum,2);
                    WriteSection_Para(1, buff3+3,tmp.i,tmp1.i);
                    t++;
                    }
             }
            else
            {
                Err_display(79);
                use_jiaotong_stander=0;
				flag = 0;
                }
            break;
            

		default:
			flag = 0;
			break;
		}
		printf("ReadCardInfor_CPU	 End == %d \n",t);
	}

#if APP_PRINTF
	printf("ReadCardInfor_CPU	 End == %d \n",t);
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
unsigned char YueConsume_cpu(void)
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
				Sector.FlagValue = 8; // 时间包月
				step++;
			}
			else
			{
				Err_display(17);
                Err_save(CARD_SPEC_M1_LINUX,17);
				Loop = 0;
			}
			break;

		case 2:
			IncSerId();
			step++;
			break;

		case 3:
			status = SaveCardData(CARD_SPEC_CPU_PBCO20, CONSUME_MODE_PRESET, GET_RECORD | SAVE_RECORD); //保存数据  定额
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

	DBG_PRINTF("ReadorRepairCard_CPU() is called.\n");

	while(Loop)
	{
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
            if(CardLan.MonthOrCountflag==1)
            {
                if(YueConsume_cpu() == MI_OK){
						step = 0;
						//ShowSwipeCardStatus(2);
					}else 
						Loop = 0;
                }
            else
            {
                step++;
                }
            break;

		case 5:
			HostValue.i = DecValue.i = 0;
			status = AnalysisSheet(1);				//查消费表
			if(status == MI_OK) step ++;
			else
			{
				Err_display(16);
                Err_save(CARD_SPEC_CPU_PBCO20,16);
				ioctl(mf_fd, RC531_HALT);
				Loop = 0;
			}
			break;

		case 6:								//扣现金
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
			status = SaveCardData(CARD_SPEC_CPU_PBCO20, CONSUME_MODE_PRESET, GET_RECORD | SAVE_RECORD); //保存数据  定额
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
                        case 0xBB:
                             Err_display(32);
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
//	printf("the step 0f readorrepaircard is :%d\n",step);
	return step;
}

unsigned char SecurityCertificate(void)
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
					Err_display(52);
                    Err_save(CARD_SPEC_CPU_PBCO20,52);
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
				// memset(ErrNum,0,sizeof(ErrNum));
				// sprintf(ErrNum,"Write Err");
				Err_display(52);
				flag = 0;
			}
			break;


		case 2:
			flag = 0;
			t = 0;
			break;

		default :
			flag = 0;
			break;
		}
	}

#if APP_PRINTF

	printf("SecurityCertificate	 End == %d \n",t);

#endif

	return t;

}


unsigned char ComplexConsumption()
{
	unsigned char Loop = 1;
	unsigned char step = 1;
	unsigned char status;
	unsigned int beforeMoney;
    unsigned int markamount;

	
	DBG_PRINTF("ComplexConsumption() is called.\n");
	
	while(Loop)
	{
		switch(step)
		{
		case 1:
			status = ReadCardInfor_CPU();
        
            if(status==9)                                                //执行本地CPU卡流程   
            {   
                step++;
                }
            else if((status==23)&&(use_jiaotong_stander==0x55))         //执行交通部互通卡流程
            {
                step=10;
                } 
            else if(status==27)                                         //执行功能卡流程
            {
                step=18;
                }
			else
			{
				Loop = 0;
			}
            break;

		case 2:
			status = IsGetOnOff_zhujian();				
			if(status == MI_OK) step ++;
			else
			{
				ioctl(mf_fd, RC531_HALT);
				Loop = 0;
			}
			break;

        case 3:
            beforeMoney=CardLanCPU.beforemoney[0]<<24|CardLanCPU.beforemoney[1]<<16|CardLanCPU.beforemoney[2]<<8|CardLanCPU.beforemoney[3];
            markamount = CardLanCPU.ymarkamount[0]<<8|CardLanCPU.ymarkamount[0];
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
        
		case 4:
			status = TopUpCardInfor_CPU(1);
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
                 //   case 0xBB:
                 //        Err_display(32);
                //        break;    
                    case 0x02:
                        Err_display(31);
                        break;
                    case 0x03:
                        Err_display(71);
                        break; 
                    case 0x04:
                        Err_display(72);
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

		case 5:	
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

		case 7:
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
            /*
			status=Permissions_jiaotong(0);
			if(status == MI_OK) step ++;
			else {
				Loop = 0;
				use_jiaotong_stander=0;
				}
				*/
				step++;
			break;
			
		case 13:	//判断上下车
			status = IsGetOnOff();
			if(status == MI_OK) 
                {
                    if(CardLanCPU.quickpassflag==0x01)
                        step = 15;
                    else
                        step++;
             }
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
            if(CardLanCPU.quickpassflag==0x01)
                {
                   status = Card_JudgeDate_jiaotong(CardLanCPU.qpstarttime,CardLanCPU.qpendtime);       //判定快捷支付卡的日间有效性
        			if(status == 2)
        				{
        					Err_display(1);
        					use_jiaotong_stander=0;
        					Loop = 0;                                
        				}
                    else if(status == 1)
                    {
                        Err_display(49);
        				use_jiaotong_stander=0;
        				Loop = 0;                
                        }
        			 
                status = TopUpCardInfor_CPU_jiaotong(3);
                }
            else
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
                    case 0xBB:
                         Err_display(32);
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
        //功能卡处理结果
        case 18:
            
            break;
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



int erase_MF(void)
{
	unsigned char binname[2],Recv[30],Send[30],challenge[8],des[8],len;
	unsigned char get_challenge[]= {0x00,0x84,0x00,0x00,0x08};
	unsigned char ex_auth[]= {0x00,0x82,0x00,0x00,0x08};
	unsigned char erase_cmd[]= {0x80,0x0E,0x00,0x00,0x00};
	unsigned char key[]= {0x57,0x41,0x54,0x43,0x48,0x44,0x41,0x54,0x41,0x54,0x69,0x6D,0x65,0x43,0x4F,0x53};
	int status,i,error=0;


	GC_ENTER();
	//select MF
	binname[0] = 0x3F;
	binname[1] = 0x00;
	len = 2;
	status = SelectAppDF(binname,Recv,&len);
	if(status != MI_OK) {
		printf("select MF error\n");
		goto error_line;
	}


	DBG_PRINTF("get challenge\n");
	//get challenge
	memcpy(Send,get_challenge,sizeof(get_challenge));
	status = write(mf_fd,Send,sizeof(get_challenge));
	if(status == MI_OK)
	{
		ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
		status = read(mf_fd, receive_buf, receive_len[0]);
		if(receive_len[0] > 2)
		{
			memcpy(challenge,receive_buf,8);
			//challenge[4]=0;
			//challenge[5]=0;
			//challenge[6]=0;
			//challenge[7]=0;
		}
		else {
			printf("get challenge read error\n");
			goto error_line;
		}

	}
	else {
		printf("get challenge write error\n");
		goto error_ret;
	}
#if APP_PRINTF
	{

		printf("get challenge data = %d\n",receive_len[0]);
		for(i =0 ; i < receive_len[0]; i ++)
		{
			printf("%02X ",receive_buf[i]);
		}
		printf("\n");
	}
#endif


	DBG_PRINTF("run des\n");
	//run des
//	for(i=0;i<8;i++){
//		key[i] = 0xFF;
//	}
	RunDes(0,0,challenge,des,8,key,16);
#if APP_PRINTF
	{

		printf("des data is ");
		for(i =0 ; i < 8; i++)
		{
			printf("%02X ",des[i]);
		}
		printf("\n");
	}
#endif


	DBG_PRINTF("EXTERNAL AUTHENTICATE\n");
	//EXTERNAL AUTHENTICATE
	memcpy(Send,ex_auth,5);
	memcpy(Send+5,des,8);
	status = write(mf_fd,Send,13);
	if(status == MI_OK)
	{
		ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
		status = read(mf_fd, receive_buf, receive_len[0]);
		if((receive_buf[0] == 0x90)&&(receive_buf[1] == 0x00))
		{
		}
		else {
			printf("EXTERNAL AUTHENTICATE read error\n");
			goto error_line;
		}

	}
	else {
		printf("EXTERNAL AUTHENTICATE write error\n");
		goto error_ret;
	}
#if APP_PRINTF
	{

		printf("EXTERNAL AUTHENTICATE data = %d\n",receive_len[0]);
		for(i =0 ; i < receive_len[0]; i ++)
		{
			printf("%02X ",receive_buf[i]);
		}
		printf("\n");
	}
#endif


	//erase MF
	DBG_PRINTF("erase MF\n");
	memcpy(Send,erase_cmd,5);
	status = write(mf_fd,Send,5);
	if(status == MI_OK)
	{
		ioctl(mf_fd, FIFO_RCV_LEN, receive_len);
		status = read(mf_fd, receive_buf, receive_len[0]);
		if((receive_buf[0] == 0x90)&&(receive_buf[1] == 0x00))
		{
		}
		else {
			printf("erase MF read error\n");
			goto error_line;
		}

	}
	else {
		printf("erase MF write error\n");
		goto error_ret;
	}
#if APP_PRINTF
	{

		printf("erase MF data = %d\n",receive_len[0]);
		for(i =0 ; i < receive_len[0]; i ++)
		{
			printf("%02X ",receive_buf[i]);
		}
		printf("\n");
	}
#endif

	ioctl(mf_fd, RC531_HALT);
	return 0;


error_line:

	printf("error recevie data = %d\n",receive_len[0]);
	for(i =0 ; i < receive_len[0]; i ++)
	{
		printf("%02X ",receive_buf[i]);
	}
	printf("\n");

error_ret:
	printf("ret = %d\n",status);

	ioctl(mf_fd, RC531_HALT);
	return -1;

}



void print_cpu_des(void)
{
	unsigned char key_data[20];
	int i;

	CardKey(key_data);

	for(i =0 ; i < 16; i++)
	{
		printf("%02X ",key_data[i]);
	}
	printf("\n");

	ioctl(mf_fd, RC531_HALT);

}

#endif

