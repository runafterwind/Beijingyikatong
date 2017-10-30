#include <apparel.h>

#include "InitSystem.h"
#include "des.h"
#include "RC500.h"

#include <arpa/inet.h>

#include "../display/fbtools.h"
#include "../update/Resumedownload.h"


#define  HEART         1   					//1 ʱ���������з���     0 ʱ���������޷���
#define  TerminalType  1


int M26_fd;
int sockfd = -1; 						//Socket
int  RI = 0; 						//����������
extern FILE *canshu; 					//�ļ�
extern FILE *BlackFile;					//�ļ�
extern FILE *ParaFile;
extern FILE *ParaFileBuf;
extern FILE *Rechargefile;
FILE *PRINTFile;
extern unsigned char TcpIpBuf[35];     			//IP ��ַ
extern CardInform CardLan;             			//��Ƭ��ʽ������
extern RecordFormat SendDataK;         			//��������
extern LongUnon DevNum;                			//����
extern LongUnon IDaddr;                 	//���ݿ� ID ��
extern unsigned char *CardLanBuf;
LongUnon WeiBuf,Jackarm;
extern int bp_fd;
LongUnon Bufaddr;

char HeartData[10]= {"ZZXXCCPK"}; 		//������
unsigned char G_START[8];  				// Э����
unsigned char G_STARTW[8]; 				// Э����
/********************** Flag  **********************/
extern volatile unsigned char SendDataTimes;
extern unsigned char COMNET;  // 1: ����TCP/IP   2: ����WIFI     3: ����CDMA   4: ����GPRS
extern pthread_mutex_t m_socketwrite; //
extern pthread_mutex_t m_Blacklist;
extern SectionFarPar Section,Sectionup;
extern unsigned char BankCardSwitch;


//unsigned char ConnectFlag; 			//���Ϸ��������˱�־Ϊ  1
unsigned char DLink;   			//�Զ��ϴ���¼ʧ����
unsigned short Heart;
unsigned char heartlen;
unsigned char AutoUpFlag;

extern LongUnon CodeNum;       	//�����ϴ�
extern LongUnon SaveNumBs;          //����
extern LongUnon TransactionNum;
extern CardLanSector LanSec;
extern unsigned char SendeghitDat[512];
extern unsigned char SelfAddress[32];
static ShortUnon Infor;
extern unsigned char DevVersion[30];
extern char ServerIP[32];
extern unsigned char ReadDushugaoCfgError;
extern unsigned char WEISHENG_ShowFlag;

// added by taeguk for update
FILE *pFd = NULL;
unsigned int UpdateSize;
unsigned short UpCRC16;
unsigned short LocalCRC16 = 0;
static unsigned int ErrLogSize = 0;

unsigned char bDisNoSimCard = 0;

extern st_BlackFile BlackListFile;


 int write_datas_gprs(int fd, unsigned char *buffer, int buf_len);


extern unsigned char g_FgFileOccurError;
extern unsigned char g_FgSendErrorAgain;
static char SendAgainCnt = 0;
extern unsigned short Mcolor;
extern unsigned short Textcolor;
extern unsigned short TextSize;


//#if defined(ZHAOTONG_BUS)
extern unsigned char g_FgCardHandleOrNot; 
//#endif

extern int Rechargeoffset;
extern unsigned char updataflag;
#ifdef SUPPORT_QR_CODE
extern struct QRCode G_QRCodeInfo;
#endif

unsigned char g_EnableCallPPPD = 0;
static unsigned char FgM26Module = 0;




#if 0
/*
*************************************************************************************************************
- �������� : unsigned char Judge_Rcv(unsigned char *Packet,unsigned char Data_len)
- ����˵�� : ��������У��
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char Judge_Rcv(unsigned char *Packet,unsigned char Data_len)
{
    unsigned char i,ch;
    unsigned char status = 2;
    if(Packet[0] == ST_RX)
    {
        if(Packet[1] == Data_len)
        {
            if(Packet[Data_len + 3] == ST_END)
            {
                ch = Packet[3];
                for(i = 4; i < Data_len + 3; i++)
                {
                    ch^= Packet[i];
                }
                if(ch == Packet[2]) status = MI_OK;
            }
        }
    }
    return status;
}

/*
*************************************************************************************************************
- �������� : unsigned char LenDataCmd(unsigned char *DataLen)
- ����˵�� : ģ�鷵�����ݷ���
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char LenDataCmd(unsigned char *DataLen)
{
    unsigned char Hbyte,Lbyte;
    if((DataLen[0] >= '0')&&(DataLen[0] <= '9'))
    {
        Hbyte = DataLen[0] - '0';
    }
    else if((DataLen[0] >= 'A')&&(DataLen[0] <= 'F'))
    {
        Hbyte = DataLen[0] - '7';
    }
    else if((DataLen[0] >= 'a')&&(DataLen[0] <= 'f'))
    {
        Hbyte = DataLen[0] - 0x57;
    }
    else
    {
        Hbyte = 0x00;
    }
    if((DataLen[1] >= '0')&&(DataLen[1] <= '9'))
    {
        Lbyte = DataLen[1] - '0';
    }
    else if((DataLen[1] >= 'A')&&(DataLen[1] <= 'F'))
    {
        Lbyte = DataLen[1] - '7';
    }
    else if((DataLen[1] >= 'a')&&(DataLen[1] <= 'f'))
    {
        Lbyte = DataLen[1] - 0x57;
    }
    else
    {
        Lbyte = 0x00;
    }
    Hbyte = Hbyte<<4|Lbyte;
    return Hbyte;

}

/*
*************************************************************************************************************
- �������� : unsigned char SendHandData(unsigned char *send55AA)
- ����˵�� : ��������  ������Э��
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char SendHandData(unsigned char *send55AA)
{
    unsigned char status = 1;

#if GPRSPR
    printf("SendHandData = %s\n",send55AA);
#endif
    status = write_datas_gprs(sockfd,send55AA,strlen(send55AA));
    return(status);
}

/*
*************************************************************************************************************
- �������� : unsigned char CheakIsConnectToInternel(void)
- ����˵�� : ��������  ������Э��
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char CheakIsConnectToInternel(void)
{


}

/*
*************************************************************************************************************
- �������� : char GprsCmd(unsigned char *SndDa)
- ����˵�� : ��������      ����Э��
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
char GprsCmd(unsigned char *SndDa)
{
    char status;
    unsigned char sendOut[1024];
    unsigned char CmdDate[512];

    memset(CmdDate,0,sizeof(CmdDate));
    hex_2_ascii(SndDa,CmdDate,SndDa[1] + 4);
    memset(sendOut,0,sizeof(sendOut));
    memcpy(sendOut,CMDSTART,5);
    memcpy(sendOut+5,G_START,8);
    strcat(sendOut,CmdDate);

#if GPRSPR
    printf("GprsCmd = %s\n",sendOut);
#endif

 status = write_datas_gprs(sockfd,sendOut,strlen(sendOut));
    return status;
}
/*
*************************************************************************************************************
- �������� : void SendCmdPc(INT8U *Date,INT8U Len)
- ����˵�� :  ��������      ����Э��
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
char  SendCmdPc(unsigned char *Date,unsigned char Len)
{
    char status;
    unsigned char Scheme[135*2];
    unsigned char i,ch;

    memset(Scheme,0,sizeof(Scheme));
    Scheme[0] = ST_RX;
    Scheme[1] = Len;
    ch =  Date[0];
    for(i = 1; i < Len; i++)
    {
        ch ^= 	Date[i];
    }
    Scheme[2] = ch;
    memcpy(Scheme + 3,Date,Len);
    Scheme[i + 3] = ST_END;
    status = GprsCmd(Scheme);

    return status;
}
unsigned char EightGps[4096];
/*
*************************************************************************************************************
- �������� : void  GPRSEight(void)
- ����˵�� : ������ȫ�ɼ�����
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
char GprsCmdEight(void)
{
    unsigned char status;
    status = write_datas_gprs(sockfd,EightGps,strlen(EightGps));
    return status;
}
/*
*************************************************************************************************************
- �������� : void SendCmdPcEight(unsigned char *Date,unsigned char Len,unsigned char icount)
- ����˵�� : ��ȫ�ɼ�
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
void SendCmdPcEight(unsigned char *Date,unsigned char Len,unsigned char icount)
{
    unsigned char Scheme[135];
    unsigned char i,ch;
    unsigned char sendOut[512];
    unsigned char CmdDate[300];

    /*printf("\n");
        for(pcount = 0;pcount < 64;pcount++)
    {
    	printf("%02X ",Date[pcount]);
                if(((pcount%16) == 0)&&(pcount!=0)) printf("\n");
    	if(((pcount%64) == 0)&&(pcount!=0)) printf("\n");
        }
        printf("\n");*/

    memset(Scheme,0,sizeof(Scheme));
    Scheme[0] = ST_RX;
    Scheme[1] = Len;
    ch =  Date[0];
    for(i = 1; i < Len; i++)
    {
        ch ^= 	Date[i];
    }
    Scheme[2] = ch;
    memcpy(Scheme + 3,Date,Len);
    Scheme[i + 3] = ST_END;

    memset(CmdDate,0,sizeof(CmdDate));
    hex_2_ascii(Scheme,CmdDate,Scheme[1] + 4);
    memset(sendOut,0,sizeof(sendOut));
    memcpy(sendOut,G_START,8);
    strcat(sendOut,CmdDate);
    //printf("sendOut = %s\n",sendOut);
    strcat(EightGps,sendOut);
}
/*
*************************************************************************************************************
- �������� : char WaveCOmPc(unsigned char *Date)
- ����˵�� : �����Զ��ϴ�����
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
char WaveCOmPc(unsigned char *Date)
{
    char status;
    unsigned char Scheme[300];
    unsigned char i,ch;
    unsigned char sendOut[300];
    unsigned char AusendOut[300];
    unsigned char  Len = 66;

    memset(Scheme,0,sizeof(Scheme));
    Scheme[0] = 0x37;
    Scheme[1] = 0x00;
    memcpy(Scheme+2,Date,64);

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


// CRC calculate
/*
int Calc_crc(unsigned char *in,unsigned int len, unsigned char *out)
{
	int  i, j, stat;
	unsigned int crc = 0x0000;//ttt;
	
	stat=0;
	//CRC16		
	for (j=0;j<len; j++)
	{
		crc=crc^((unsigned int)in[j]<<8);
		for (i=0;i<8;i++)  {if (crc&0x8000) crc=(crc<<1)^0x1021; else crc=crc<<1; }
	}
	out[0]=(unsigned char)(crc>>8);	out[1]=(unsigned char)crc;
	if (crc) stat=1;
	return (stat);
}
*/

int Calc_crc(unsigned char *in, unsigned int len, unsigned short * out,unsigned int begin)
{
	int  i, j, stat;
	//unsigned int crc = 0x0000;//ttt;
	unsigned int crc;
	crc = begin;
	
	stat=0;
	//CRC16		
	for (j=0;j<len; j++)
	{
		crc=crc^((unsigned int)in[j]<<8);
		for (i=0;i<8;i++)  {if (crc&0x8000) crc=(crc<<1)^0x1021; else crc=crc<<1; }
	}
	//out[0]=(unsigned char)(crc>>8);	out[1]=(unsigned char)crc;
	*out = (unsigned short)crc;
	if (crc) stat=1;
	return (stat);	
}


void Calc_UpdateCrc(void)
{
	int ret;
	FILE * fd;
	unsigned char buf[1024];
	LocalCRC16 = 0;
	fd = fopen("/opt/update.bin","rb");
	if(NULL != fd)
	{
		do{
			ret = fread(buf,sizeof(char),1024,fd);
			if(ret)
			{
				Calc_crc(buf,ret,&LocalCRC16,LocalCRC16);
			}
		}while(ret);
		fclose(fd);
	}
}
// �������վ��������������
void StopCursor(void)
{
	system("echo -e -n '\033[?25l' > /dev/tty0");
	system("echo -e -n '\033[9]' > /dev/tty0");
}

/*
*************************************************************************************************************
- �������� : void All_Cmd_Snd(unsigned char *Data)
- ����˵�� : ����������ݻظ�
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
//extern void Set_bStartUpLoad(void);
//extern void Clear_bStartUpLoad(void);
void All_Cmd_Snd(unsigned char *Data)
{
    unsigned char  status,i;
    unsigned char  RcvPc[132];
    unsigned char  buff[20];
    unsigned char  CsnAsc[10];
    unsigned char  SelfIpa[16];
    unsigned char  SelfIpb[40];
    unsigned int  SnPc;
    int            result;
//	ShortUnon Saddrs;
    BlackItem item;
    int find;
	struct stat stbuff;
	union
	{
		unsigned char buff[4];
		unsigned int word;
	}BuffWord;
	unsigned char *pTmp;

    memset(RcvPc,0,sizeof(RcvPc));
    status = Judge_Rcv(Data,Data[1]);
    if(status == MI_OK)
    {
        RcvPc[0] = Data[3];
        printf("��������:%02x\n",RcvPc[0]);
        switch(Data[3])
        {
        case 0x20:							//��ʽ���ڴ�����
#if GPRSPR
            printf("rm all record ....\n");
#endif
            WeiBuf.i = 0;
            status = RMAllRecord(WeiBuf.i);			
            if(status==0)
            {
            	system("sync;");
                RcvPc[1] = 0x00;
                SendCmdPc(RcvPc,2);
            }
            else
            {
            	system("sync;");
                RcvPc[1] = 0xff;
                SendCmdPc(RcvPc,2);
            }
            break;

        case 0x21:							//��ʽ������������
            #ifdef SAVE_CONSUM_DATA_DIRECT
            system("rm -rf /mnt/record/Blacklist.sys");
            BlackFile = fopen("/mnt/record/Blacklist.sys","a+");
            #else
            system("rm -rf /var/run/Blacklist.sys");
            BlackFile = fopen("/var/run/Blacklist.sys","a+");
            #endif
            if(BlackFile)
            {            
                fclose(BlackFile);
				system("sync;");
            }
            RcvPc[1] = 0x00;
            SendCmdPc(RcvPc,2);
            break;

        case 0x22:							//���غ���������
            if(((Data[1] - 1)%4) == 0)
            {
                for(i = 0; i < (Data[1] - 1)/4; i++)
                {
                    memset(CsnAsc,0,sizeof(CsnAsc));
                    Bcd_To_Asc(CsnAsc,(Data + (4* i) + 4),8);
                    Number(myatoi(CsnAsc),0);
                }
				system("sync;");
                RcvPc[1] = 0x00;
                SendCmdPc(RcvPc,2);
            }
            else
            {
                RcvPc[1] = 0xFF;
                SendCmdPc(RcvPc,2);
            }
            break;

        case 0x23:							//��Һ���������
            if(((Data[1] - 1)%4) == 0)
            {
                for(i = 0; i < (Data[1] - 1)/4; i++)
                {
                    memset(CsnAsc,0,sizeof(CsnAsc));
                    Bcd_To_Asc(CsnAsc,(Data + (4* i) + 4),8);
                    Number(myatoi(CsnAsc),1);
                }
				system("sync;");
                RcvPc[1] = 0x00;
                SendCmdPc(RcvPc,2);
            }
            else
            {
                RcvPc[1] = 0xFF;
                SendCmdPc(RcvPc,2);
            }
            break;

        case 0x24:							//��ѯ������
            memset(CsnAsc,0,sizeof(CsnAsc));
            Bcd_To_Asc(CsnAsc,Data + 4,8);
            RcvPc[1] = 0;
            RcvPc[2] = Number(myatoi(CsnAsc),2);
            SendCmdPc(RcvPc,3);
            break;
        case 0x25:							//����¼����
#if GPRSPR
            printf("Read record ....\n");
#endif

            SqlCheckNewDat(0x55);
            RcvPc[1] = 0x00;
            Bufaddr.i = SaveNumBs.i -1;
            memcpy(RcvPc+2,Bufaddr.longbuf,4);
            Bufaddr.i = CodeNum.i -1;
            memcpy(RcvPc+6,Bufaddr.longbuf,4);
            //Bufaddr.i = TransactionNum.i -1;
            //memcpy(RcvPc+10,Bufaddr.longbuf,4);
            //SendCmdPc(RcvPc,14);
            SendCmdPc(RcvPc,10);
            break;
  
       case 0x28:							//��ȡ�ն�ʱ��
#if GPRSPR
            printf("Read write time ....\n");
#endif
            Rd_time (buff);
            RcvPc[1] = 0x00;
            memcpy(RcvPc+2,buff,7);
            SendCmdPc(RcvPc,9);
            break;

        case 0x29:							//д�ն�ʱ��
#if GPRSPR
            printf("write time ....\n");
#endif
            sprintf(buff,"20%02x-%02x-%02x %02x:%02x:%02x",Data[4],\
                    Data[5],Data[6],Data[7],Data[8],Data[9]);
            status  = Wr_time(buff);
            if(status == 0)
            {
                system ("hwclock -w");
                system ("hwclock -s");

#if MULTI_FILE_STORAGE
				printf("����ʱ��ɹ���׼����ʼ����¼�ļ�\n");
				if(init_record()<0){
					printf("��¼��ʼ��ʧ��\n");
				}
				else{
				    printf("��¼��ʼ���ɹ�\n");
					show_index_item();
					show_all_record_head();		//test
				}
#endif
            }
            RcvPc[1] = status;
#if GPRSPR
            printf("write time ok....\n");
#endif
            SendCmdPc(RcvPc,2);
            break;

        case 0x30:							//���ն˻���
#if GPRSPR
            printf("read DerNum ....\n");
#endif
            memcpy(RcvPc+2,DevNum.longbuf,4);
            RcvPc[1] = 0x00;
            SendCmdPc(RcvPc,6);
            break;

        case 0x31:							//д�ն˻���
#if GPRSPR
            printf("write DerNum ....\n");
#endif
            memcpy(DevNum.longbuf,Data+4,4);
            // while (FileOpenFlag == 0);
            //	FileOpenFlag = 0;
            ReadOrWriteFile (MTEMNO);
            //    FileOpenFlag = 1;
            system("sync;");
            RcvPc[1] = 0;
            SendCmdPc(RcvPc,2);
            ConnectFlag = 1;
            break;
        case 0x32:							//��������������
		printf("remove  /mnt/record/cardlan.bin.\n");
            #ifdef SAVE_CONSUM_DATA_DIRECT
            system("rm -rf /mnt/record/cardlan.bin");
            ParaFileBuf = fopen("/mnt/record/cardlan.bin","a+");
            #else
            system("rm -rf /var/run/cardlan.bin");
            ParaFileBuf = fopen("/var/run/cardlan.bin","a+");
            #endif
            if(ParaFileBuf)
            {
                fclose(ParaFileBuf);
				system("sync;");
            }
            RcvPc[1] = 0;
            SendCmdPc(RcvPc,2);
            break;
        case 0x33:							//д����������
		printf("write  /mnt/record/cardlan.bin.\n");
            memcpy(Bufaddr.longbuf,Data+4,4);
            WeiBuf.i = 0;
            memcpy(WeiBuf.longbuf,Data+8,2);
            Bufaddr.i = Bufaddr.i*512 + WeiBuf.i;
            i = Data[10];						//����
            status = Para_cardlan(Data+11,Bufaddr.i,i,0);
            if(status == MI_OK)
            {
            	system("sync;");
                RcvPc[1] = 0x00;
                status = Para_cardlan(RcvPc+2,Bufaddr.i,i,1);
                SendCmdPc(RcvPc,i + 2);
            }
            else
            {
            	system("sync;");
                RcvPc[1] = 0xFD;
                SendCmdPc(RcvPc,2);
            }
            break;
        case 0x34:		//������������ת�Ƶ����ÿ�
		    printf("copy  /mnt/record/cardlan.bin.\n");
            system("rm -rf "PARM_FILE_PATH);
            #ifdef SAVE_CONSUM_DATA_DIRECT
            system("mv /mnt/record/cardlan.bin /mnt/record/cardlan.sys");
            #else
            system("mv /var/run/cardlan.bin /var/run/cardlan.sys");    
            #endif
        
            if(access(PARM_FILE_PATH,F_OK)==0)
            {    
                Read_Parameter();
    			system("sync;");
                RcvPc[1] = 0;
                SendCmdPc(RcvPc,2);
                }
            else
            {
            	system("sync;");
                RcvPc[1] = 0xFD;
                SendCmdPc(RcvPc,2);
            }
            break;
        case 0x35:							//��������
#if GPRSPR
            printf("beep open ....\n");
#endif
            ioctl(bp_fd,0);
            RcvPc[1] = 0;
            SendCmdPc(RcvPc,2);
            break;
        case 0x36:							//��������
#if GPRSPR
            printf("beep close ....\n");
#endif
            ioctl(bp_fd,1);
            RcvPc[1] = 0;
            SendCmdPc(RcvPc,2);
            break;
        case 0x37:							//��ȫ�ɼ�
            #ifdef CANGNAN_BUS
            status = ReadEightDat_Zhujian(Data+4);
            #else
            status = ReadEightDat(Data+4);
            #endif

            if(status == 0)
            {
                SnPc = 0x00;
                memset(EightGps,0,sizeof(EightGps));
                #ifdef CANGNAN_BUS
                  for(i = 0; i < 8; i++)
                {
                    memcpy(RcvPc + 2,SendeghitDat + SnPc,72);
                    SnPc += 72;
                    SendCmdPcEight(RcvPc,74,i);
                }                
                #else
                for(i = 0; i < 8; i++)
                {
                    memcpy(RcvPc + 2,SendeghitDat + SnPc,64);
                    SnPc += 64;
                    SendCmdPcEight(RcvPc,66,i);
                }                
                #endif
                GprsCmdEight();
            }
            else
            {
                RcvPc[1] = status;
                SendCmdPc(RcvPc,2);
            }
			
        break;

        case 0x38:							//�����û�������
#if GPRSPR
            printf("set IP ....\n");
#endif
            memcpy(TcpIpBuf,Data+4,34);
            //    while (FileOpenFlag == 0);
            //   FileOpenFlag = 0;
            ReadOrWriteFile (MSEVERIP);
            //   FileOpenFlag = 1;
            system("sync;");
            RcvPc[1] = 0x00;
            SendCmdPc(RcvPc,2);
            break;
        case 0x39:							//�����û�����
            memcpy(&LanSec,Data+4,8); // 1--16ΪM1������
            memcpy(LanSec.ADFNUM,Data+20,2); // 17-- ΪCPU��ADF
            /// while (FileOpenFlag == 0);
            //    FileOpenFlag = 0;
            ReadOrWriteFile (MUSERSETOR);
            //   FileOpenFlag = 1;
            system("sync;");
            RcvPc[1] = 0x00;
            SendCmdPc(RcvPc,2);
            break;
        case 0x40:							//ɾ����ӡͷ
            system("rm start.bin");
            PRINTFile = fopen("start.bin","a+");
            if(PRINTFile)
            {
                fclose(PRINTFile);
				system("sync;");
            }
            RcvPc[1] = 0x00;
            SendCmdPc(RcvPc,2);
            break;
			
        case 0x41:	//g						//���ô�ӡͷ
            PRINTFile = fopen("start.bin","rb+");
            memcpy(Bufaddr.longbuf,Data+4,4);
            i = Data[10];						//����
            result = fseek(PRINTFile, Bufaddr.i, SEEK_SET);
            result = fwrite(Data+11,sizeof(unsigned char),i,PRINTFile);
            RcvPc[1] = 0x00;
            result = fseek(PRINTFile, Bufaddr.i, SEEK_SET);
            result = fread(RcvPc+2,sizeof(unsigned char),i,PRINTFile);
            SendCmdPc(RcvPc,i + 2);
            fclose(PRINTFile);
			system("sync;");
            break;

        case 0x42:							//ɾ����ӡͷ
            system("rm end.bin");
            PRINTFile = fopen("end.bin","a+");
            if(PRINTFile)
            {
            	system("sync;");
                fclose(PRINTFile);
            }
            RcvPc[1] = 0x00;
            SendCmdPc(RcvPc,2);
            break;
			
        case 0x43:		//g					//���ô�ӡͷ
            PRINTFile = fopen("end.bin","rb+");
            memcpy(Bufaddr.longbuf,Data+4,4);
            i = Data[10];						//����
            result = fseek(PRINTFile, Bufaddr.i, SEEK_SET);
            result = fwrite(Data+11,sizeof(unsigned char),i,PRINTFile);
            RcvPc[1] = 0x00;
            result = fseek(PRINTFile, Bufaddr.i, SEEK_SET);
            result = fread(RcvPc+2,sizeof(unsigned char),i,PRINTFile);
            SendCmdPc(RcvPc,i + 2);
            fclose(PRINTFile);
			system("sync;");
            break;

        case 0x45:
            CodeNum.i = 1;
            //    while (FileOpenFlag == 0);
            //  FileOpenFlag = 0;
            ReadOrWriteFile (CODEFILE);
            //FileOpenFlag = 1;
            system("sync;");
            RcvPc[1] = 0x00;
            SendCmdPc(RcvPc,2);
            break;


        case 0x46:
            memcpy(Bufaddr.longbuf,Data+4,4);
            WeiBuf.i = 0;
            memcpy(WeiBuf.longbuf,Data+8,2);
            Bufaddr.i = Bufaddr.i*512 + WeiBuf.i;
            i = Data[10];						//����
            status = Para_cardlan(Data+11,Bufaddr.i,i,2);
            if(status == MI_OK)
            {
            	system("sync;");
                RcvPc[1] = 0x00;
                status = Para_cardlan(RcvPc+2,Bufaddr.i,i,3);
                SendCmdPc(RcvPc,i + 2);
            }
            else
            {
            	system("sync;");
                RcvPc[1] = 0xFD;
                SendCmdPc(RcvPc,2);
            }
            break;

        case 0x47:
            memcpy(&Section.SationNum,Data+4,10);
            ReadOrWriteFile (SETSECTION);
			if(Section.Enableup != 0x55)
			  {
            memcpy(&Sectionup.SationNum,Data+4,6);
            ReadOrWriteFile (SETSECTIONUP);
			  }
            #ifdef SAVE_CONSUM_DATA_DIRECT
            system("rm /mnt/record/section.sys");
            usleep(10000);
            system("mv /mnt/record/section.bin  /mnt/record/section.sys");
            if(access("/mnt/record/section.sys",F_OK)==0)
            #else
            system("rm /var/run/section.sys");
            usleep(10000);
            system("mv /var/run/section.bin  /var/run/section.sys");
            if(access("/var/run/section.sys",F_OK)==0)
            #endif
            {
                CardLanFile(SectionPar); //���Ѳ�����ȡ
                system("sync;");
                RcvPc[1] = 0;
                SendCmdPc(RcvPc,2);
                }
            else
            {
                system("sync;");
                RcvPc[1] = 0xFD;
                SendCmdPc(RcvPc,2);
                }
           break;

        case 0x48:
            memcpy(&Sectionup.SationNum,Data+4,6);
            ReadOrWriteFile (SETSECTIONUP);
            #ifdef SAVE_CONSUM_DATA_DIRECT
            system("rm /mnt/record/sectionup.sys");
            usleep(10000);
            system("mv /mnt/record/section.bin  /mnt/record/sectionup.sys");
            if(access("/mnt/record/sectionup.sys",F_OK)==0)
            #else
            system("rm /var/run/sectionup.sys");
            usleep(10000);
            system("mv //var/run/section.bin  /var/run/sectionup.sys");
            if(access("/var/run/sectionup.sys",F_OK)==0)
            #endif
             {
                CardLanFile(SectionParup); //���Ѳ�����ȡ
                system("sync;");
                RcvPc[1] = 0;
                SendCmdPc(RcvPc,2);
                }
            else
                {
                system("sync;");
                RcvPc[1] = 0xFD;
                SendCmdPc(RcvPc,2);
                }
            break;



        case 0x50:					//������������ת�Ƶ����ÿ�
            system("rm -rf start.txt");
            system("mv start.bin start.txt");
			system("sync;");
            RcvPc[1] = 0;
            SendCmdPc(RcvPc,2);
            break;
        case 0x51:							//������������ת�Ƶ����ÿ�
            system("rm -rf end.txt");
            system("mv end.bin end.txt");
			system("sync;");
            RcvPc[1] = 0;
            SendCmdPc(RcvPc,2);
            break;
        case 0x52:							//�����û�������
#if GPRSPR
            printf("set selfIP ....\n");
#endif

            RcvPc[1] = 0x00;
            SendCmdPc(RcvPc,2);

            memset(SelfIpa,0,sizeof(SelfIpa));
            memset(SelfIpb,0,sizeof(SelfIpb));
            memcpy(SelfIpa,SelfAddress,16);
            sprintf(SelfIpb,"ifconfig eth0 ");
            strcat(SelfIpb,SelfIpa);
            system(SelfIpb);

            memset(SelfIpa,0,sizeof(SelfIpa));
            memset(SelfIpb,0,sizeof(SelfIpb));
            memcpy(SelfIpa,SelfAddress+16,16);
            sprintf(SelfIpb,"route del default gw ");
            strcat(SelfIpb,SelfIpa);
            system(SelfIpb);

            memcpy(SelfAddress,Data+4,32);
            // while (FileOpenFlag == 0);
            //  FileOpenFlag = 0;
            ReadOrWriteFile (SELFIP);
            //  FileOpenFlag = 1;
            // close(sockfd);
            //  sockfd = -1;
            ConnectFlag = 1;
            break; 
		//add by wxy
		case 0x53:				//ɾ��log.bmp
			system("rm -rf /var/run/logo.bmp");
			system("sync");
			RcvPc[1] = 0x00;
            SendCmdPc(RcvPc,2);
			break;
		case 0x54:				//ɾ��back.bmp�����׵�����
			system("rm -rf /var/run/back.bm");
			system("sync");
			RcvPc[1] = 0x00;
            SendCmdPc(RcvPc,2);
			break;
		
		case 0x55:				//ɾ���ϵ���������ʱ�ļ�
			system("rm -rf /var/run/tempupfile.bin");
			system("sync");
			RcvPc[1] = 0x00;
            SendCmdPc(RcvPc,2);
			
		case 0x76:
			if(pFd != NULL){
				fclose(pFd);
				pFd = NULL;
			}
			system("sync");
			pFd = fopen("/tmp/update.bin", "ab+");			 
			if(NULL == pFd)
			{
				RcvPc[1] = 0x01; // not enought space
				SendCmdPc(RcvPc,2);
				LocalCRC16 = 0;
			}
			else
			{
				//fclose(pFd);
				stat("/tmp/update.bin",&stbuff);
				//BuffWord.word = stbuff.st_size;
				BuffWord.buff[0] = (unsigned char)(stbuff.st_size>>24);
				BuffWord.buff[1] = (unsigned char)(stbuff.st_size>>16);
				BuffWord.buff[2] = (unsigned char)(stbuff.st_size>>8);
				BuffWord.buff[3] = (unsigned char)stbuff.st_size;				
				RcvPc[1] = 0x00;
				memcpy(RcvPc+2,BuffWord.buff,4);
				if(stbuff.st_size == 0)
					LocalCRC16 = 0;	
				//memcpy(RcvPc+6,&LocalCRC16,2);
				RcvPc[6] = (unsigned char)(LocalCRC16>>8);
				RcvPc[7] = (unsigned char)LocalCRC16;
				SendCmdPc(RcvPc,8);
				UpdateSize = stbuff.st_size;
				UpCRC16 = (unsigned short)((Data[4]<<8)|Data[5]);		
				fseek(pFd, 0L, SEEK_END);
			}						
			break;
		case 0x77:
			if(pFd != NULL)
			{
				//stat("/opt/update.bin",&stbuff);
				//memcpy(BuffWord.buff, Data+4, 4);
				BuffWord.buff[3] = Data[4];
				BuffWord.buff[2] = Data[5];
				BuffWord.buff[1] = Data[6];
				BuffWord.buff[0] = Data[7];				
				//if(BuffWord.word != stbuff.st_size)
				if(BuffWord.word != UpdateSize)
				{
					RcvPc[1] = 0x01;
					//BuffWord.word = UpdateSize; 
					BuffWord.buff[0] = (unsigned char)(UpdateSize>>24);
					BuffWord.buff[1] = (unsigned char)(UpdateSize>>16);
					BuffWord.buff[2] = (unsigned char)(UpdateSize>>8);
					BuffWord.buff[3] = (unsigned char)UpdateSize;
					memcpy(RcvPc+2, BuffWord.buff, 4);
					SendCmdPc(RcvPc,6);
				}
				else
				{
					BuffWord.word = 0;
					//memcpy(BuffWord.buff,Data+8,2);
					BuffWord.buff[1] = Data[8];
					BuffWord.buff[0] = Data[9];
					//printf("The Data len: %d \n",BuffWord.word);
					result = fwrite(Data+10,sizeof(unsigned char),BuffWord.word,pFd);
					//printf("the result : %d \n",result);					
					RcvPc[1] = 0x00;
					SendCmdPc(RcvPc,2);
					Calc_crc(Data+10,BuffWord.word,&LocalCRC16,(unsigned int)LocalCRC16);
					UpdateSize += BuffWord.word;
				}
			}
			else
			{
				RcvPc[1] = 0x02;
				SendCmdPc(RcvPc,2);
				LocalCRC16 = 0;
			}
			break;
		case 0x78:
			if(LocalCRC16 == UpCRC16)
			{
				RcvPc[1] = 0x00;
				SendCmdPc(RcvPc,2);		
				LocalCRC16 = 0;
				fclose(pFd);
				pFd = NULL;
				system("sync");
				BuffWord.word = 0;
				//memcpy(BuffWord.buff,Data+4,2);
				BuffWord.buff[1] = Data[4];
				BuffWord.buff[0] = Data[5];
				// test
				//BuffWord.word = 10;
				
				printf("path len : %d \n",BuffWord.word);
				pTmp = (unsigned char*)malloc(300);		
				memset(pTmp,0,300);
				sprintf(pTmp, "mv /tmp/update.bin  /tmp/update.tar.gz");
				//strcat(pTmp, Data+6);			
				//memcpy(pTmp + strlen(pTmp), Data+6, BuffWord.word);
				if(!(Data[6]==0x42 && Data[7]==0x6C))
				{
					printf("\n...here...\n");
					system(pTmp);	
				}
				
				memset(pTmp,0,300);	
				sprintf(pTmp, "tar zxvf /tmp/update.tar.gz -C ");
				if(BuffWord.word > 1000) BuffWord.word = 1000;
				*(Data+6+BuffWord.word) = 0;
				strcpy(pTmp + strlen(pTmp), dirname(Data+6));				
				#if 0
				sprintf(pTmp, "tar zxvf /opt/update.tar.gz -C /opt/");			
				system(pTmp);

				memset(pTmp,0,300);
				sprintf(pTmp, "mv /opt/update ");
				memcpy(pTmp + strlen(pTmp), Data+6, BuffWord.word);
				#endif
				
				if(!(Data[6]==0x42 && Data[7]==0x6C))
				{
					printf("\n...here...\n");
					system(pTmp);	
					if(Data[6]==0x42 && Data[7]==0x61)
						system("mv ../../../BankNoPrefix.bin /mnt/record/BankNoPrefix.bin");
				}
				else
				{
					printf("\n...there...\n");
					memset(pTmp,0,300);	
					sprintf(pTmp, "mv /tmp/update.bin /mnt/record/BankBlacklist.sys");
					system(pTmp);					
				}
				
				printf("last cmd : %s \n",pTmp);
				free(pTmp);
				system("rm -f /tmp/update.tar.gz");
				system("sync;");
				sleep(5);
				if(Data[6]==0x42 && Data[7]==0x6C)//Bl������
				{
					ReloadBlackListBuff();
				}
				else if(Data[6]==0x42 && Data[7]==0x61)//Ba����
				{
				//	InitCardBin(0);
				}
				else
				{
					system("reboot");
				}
			}
			else
			{
				RcvPc[1] = 0x01;
				SendCmdPc(RcvPc,2);	
				LocalCRC16 = 0;
				fclose(pFd);	
				pFd = NULL;	
				system("rm -f /tmp/update.bin");
			}
			break;
		case 0x79:	

			LocalCRC16 = 0;
			UpdateSize = 0;
			if(pFd != NULL){
				fclose(pFd);
				pFd = NULL;
			}				
			system("rm -f /tmp/update.bin");
			system("sync");
			RcvPc[1] = 0x00;
			SendCmdPc(RcvPc,2);				
			break;
		/* ȡ��־�ļ�*/
		//case 0x8B:  //0x80:
		  case 0x80:
			//Set_bStartUpLoad();
			pFd = NULL;
			system("sync");
			if (!(access("/var/run/CARDLAN_LOG.dat", 0)) && (pFd = fopen("/var/run/CARDLAN_LOG.dat", "rb"))!=NULL){
				stat("/var/run/CARDLAN_LOG.dat",&stbuff);				
				RcvPc[1] = 0x00;
				BuffWord.buff[0] = (unsigned char)(DevNum.i>>24);
				BuffWord.buff[1] = (unsigned char)(DevNum.i>>16);
				BuffWord.buff[2] = (unsigned char)(DevNum.i>>8);
				BuffWord.buff[3] = (unsigned char)DevNum.i;				
				memcpy(RcvPc+2, BuffWord.buff, 4);
				BuffWord.buff[0] = (unsigned char)(stbuff.st_size>>24);
				BuffWord.buff[1] = (unsigned char)(stbuff.st_size>>16);
				BuffWord.buff[2] = (unsigned char)(stbuff.st_size>>8);
				BuffWord.buff[3] = (unsigned char)stbuff.st_size;
				ErrLogSize = stbuff.st_size;
				memcpy(RcvPc+6, BuffWord.buff, 4);
				SendCmdPc(RcvPc,10);								
			}
			else{
				if(pFd != NULL)
					fclose(pFd);
				pFd = NULL;					
				//Clear_bStartUpLoad();
				RcvPc[1] = 0x01;
				SendCmdPc(RcvPc,2);				
			}
			break;
		//case 0x8C:   //0x81:	
		  case 0x81:
			BuffWord.buff[3] = Data[4];
			BuffWord.buff[2] = Data[5];
			BuffWord.buff[1] = Data[6];
			BuffWord.buff[0] = Data[7];				
			if((pFd != NULL) && (BuffWord.word-1)*168 < ErrLogSize)
			{			
				fseek(pFd,(BuffWord.word-1)*168, SEEK_SET);
				RcvPc[1] = 0x00;
				//printf("--BuffWord.word:%d\n",BuffWord.word);
				fread(RcvPc+2,sizeof(unsigned char),168,pFd);
				//printf("---Read over\n");
				SendCmdPc(RcvPc,170);					
			}
			else{
				RcvPc[1] = 0x01;
				SendCmdPc(RcvPc,2);		
				fclose(pFd);
				pFd = NULL;	
				
//				Clear_bStartUpLoad();
			}
			break;
		//case 0x8D:     //0x82:
		 case 0x82:
			RcvPc[1] = 0x00;
			SendCmdPc(RcvPc,2);	
			fclose(pFd);
			pFd = NULL;

			system("rm -f /var/run/CARDLAN_LOG.dat");
			system("sync");
//			Clear_bStartUpLoad();
			break;			
		//case  0x8E:     //0x83:
		 case 0x83:
			RcvPc[1] = 0x00;
			BuffWord.word = strlen(DevVersion);
			RcvPc[3] = BuffWord.buff[0];
			RcvPc[2] = BuffWord.buff[1];
			//printf("version = %s\n", DevVersion);
			memcpy(RcvPc+4, DevVersion, BuffWord.word);
			SendCmdPc(RcvPc, BuffWord.word+4);
			break;	

		case 0x84:
			RcvPc[1] = 0x00;
            SendCmdPc(RcvPc,2);
			BankCardSwitch = Data[4];
			ReadOrWriteFile (SETBANKCARDSWITCH);
			break;
		 
		case 0xb5: //���ݳ������޸�����input/output error
			if (g_FgFileOccurError)
			{
				RcvPc[1] = 0x00;
				 SendCmdPc(RcvPc,2);
				SetColor(Mcolor);
				SetTextSize(32);
				SetTextColor(Color_white);
				TextOut(100,50, "��ܰ��ʾ");
				TextOut(45,100,"�����޸�����");
				TextOut(110,150,"......");
				system("chmod u+x /var/run/repair.sh");
				system("sh /var/run/repair.sh");
				sleep(4);

			}
			else
			{
				RcvPc[1] = 0x01;
				 SendCmdPc(RcvPc,2);
			}
			break;
			


            

			case 0xC9://���ػ�����ͨ����������
				printf("���ػ�����ͨ���������� ....\n");
				if(((Data[1] - 1)%10) == 0)
				{
                    #ifdef Transport_Stander
					for(i = 0; i < (Data[1] - 1)/10; i++)
					{
						memset(CsnAsc,0,sizeof(CsnAsc));
						memcpy(CsnAsc,(Data + (10* i) + 4),10);
						{
							memcpy(item.dat, CsnAsc, sizeof(item));
							update_sortfile(item, 0);
						}
					}
					SavetBlackListBuff();
                    #elif CANGNAN_BUS
                    for(i = 0; i < (Data[1] - 1)/10; i++)
					{
						memset(CsnAsc,0,sizeof(CsnAsc));
						memcpy(CsnAsc,(Data + (10* i) + 4),10);
						{
							memcpy(item.dat, CsnAsc, sizeof(item));
							update_sortfile_zhujian(item, 0);
						}
					}
					SavetBlackListBuff_zhujian();
                    #endif
                    
					//system("mv /mnt/record/Blacklistbak.sys /mnt/record/Blacklist.sys");
					RcvPc[1] = 0x00;
					SendCmdPc(RcvPc,2);
				}
				else
				{
					RcvPc[1] = 0xFF;
					SendCmdPc(RcvPc,2);
				}			
				break;

           case 0xCA://���ػ�����ͨ����������
				printf("���ػ�����ͨ���������� ....\n");
				if(((Data[1] - 1)%4) == 0)
				{
					for(i = 0; i < (Data[1] - 1)/4; i++)
					{
						memset(CsnAsc,0,sizeof(CsnAsc));
						memcpy(CsnAsc,(Data + (4* i) + 4),4);
						{
							memcpy(item.dat, CsnAsc, sizeof(item));
							update_sortfile_white(item, 0);
						}
					}
					SavetWhiteListBuff();
					//system("mv /mnt/record/Blacklistbak.sys /mnt/record/Blacklist.sys");
					RcvPc[1] = 0x00;
					SendCmdPc(RcvPc,2);
				}
				else
				{
					RcvPc[1] = 0xFF;
					SendCmdPc(RcvPc,2);
				}			
				break;

             case 0xCB://���������ͨ����������    
                pthread_mutex_lock(&m_Blacklist);
                memset(BlackListFile.buf, 0x00, (MAX_BLACK_CONNT * sizeof(BlackListFile.buf[0])));
                BlackListFile.count = 0;
                system("rm /mnt/record/Blacklistbak.sys");
                system("sync");
                RcvPc[1] = 0x00;
				SendCmdPc(RcvPc,2);
                pthread_mutex_unlock(&m_Blacklist);
                break;
			
#ifdef SUPPORT_QR_CODE
	case QR_CODE_CMD_ACK:
		//memcpy((unsigned char *)&G_QRCodeInfo.status, Data+4, 38); /*1byte status+1byte type +4bytes trans No+32bytes name = 37*/
		G_QRCodeInfo.status = Data[4];
		G_QRCodeInfo.type = Data[5];
		memcpy(G_QRCodeInfo.tranNo.longbuf, Data+6, 4);
		G_QRCodeInfo.name_len = Data[10];
		//printf("return qr code len = %d", G_QRCodeInfo.name_len);
		memcpy(G_QRCodeInfo.name, Data+11, G_QRCodeInfo.name_len);
		g_FgQRCodeRcvAck = 1;//rcv ack from server
		RcvPc[1] = 0x00;
		SendCmdPc(RcvPc,2);
		break;
#endif
			
        default:
            RcvPc[0] = Data[3];
            RcvPc[1] = 0xFE;						//δ֪����
            SendCmdPc(RcvPc,2);
            break;
        }
    }
}



/*
*************************************************************************************************************
- �������� : int createProxy(int *sock)
- ����˵�� : ���� SOCKET ����
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
int createProxy(int *sock)
{
    struct hostent *hp;
    struct timeval vl;
    struct sockaddr_in tcpaddr;
    fd_set connectionAble;
    int setValue=1;
    int flags,ret;
    int selectRet,iResult=0;
    int trueSocket;  //=socket(AF_INET,SOCK_STREAM,0);
    unsigned char IPaddrbuf[6];
    char *hostname;
    char *IPBuf1,*IPBuf2,*IPBuf3,*IPBuf4;


    trueSocket =socket(AF_INET,SOCK_STREAM,0);
    vl.tv_sec  = 5;
    vl.tv_usec = 0;
    tcpaddr.sin_family=AF_INET;
    memcpy(Infor.intbuf,TcpIpBuf+32,2);
    sprintf(IPaddrbuf,"%05d",Infor.i);
    if((TcpIpBuf[0]>='0')&&(TcpIpBuf[0]<='9'))				//IP
    {
        sscanf(TcpIpBuf, "%d.%d.%d.%d",(int *)&IPBuf1,(int *)&IPBuf2,(int *)&IPBuf3,(int *)&IPBuf4);
        sprintf(TcpIpBuf,"%d.%d.%d.%d",(int )IPBuf1,(int )IPBuf2,(int )IPBuf3,(int )IPBuf4);
#if GPRSPR
        printf("client1 IP %s,Port %s\n",TcpIpBuf,IPaddrbuf);
#endif

#if 0
        tcpaddr.sin_port=htons(atoi("3000"));
        tcpaddr.sin_addr.s_addr=inet_addr("192.168.18.120");
#else
        tcpaddr.sin_port=htons(atoi(IPaddrbuf));
        tcpaddr.sin_addr.s_addr=inet_addr(TcpIpBuf);
#endif

    }
    else									//
    {
        if(COMNET > 2)
        {
            system("cp /etc/ppp/resolv.conf  /etc/");
            usleep(200000);
        }

        hostname = TcpIpBuf;
#if GPRSPR
        printf("hostname client...%s\n",hostname);
#endif
        if((hp = gethostbyname(hostname))==NULL)
        {
            printf("hostname is bad\n");
            close(trueSocket);
            return(1);
        };
#if GPRSPR
        printf("client2 IP %s,Port %s\n",inet_ntoa(*((struct in_addr*)(hp->h_addr))),IPaddrbuf);
#endif
        tcpaddr.sin_port=htons(atoi(IPaddrbuf));
        tcpaddr.sin_addr.s_addr=inet_addr(inet_ntoa(*((struct in_addr*)(hp->h_addr))));
    }
    bzero(&(tcpaddr.sin_zero),8);
    setsockopt(trueSocket,SOL_SOCKET,SO_REUSEADDR,(const char*)&setValue,sizeof(int));

    if((flags = fcntl(trueSocket, F_GETFL)) < 0 )
    {
#if GPRSPR
        perror("fcntl F_SETFL");
#endif
        close(trueSocket);
        return -1;
    }
    flags |= O_NONBLOCK;
    if(fcntl(trueSocket, F_SETFL,flags) < 0)
    {
#if GPRSPR
        perror("fcntl");
#endif
        close(trueSocket);
        return -2;
    }

    ret = connect(trueSocket,(struct sockaddr *)&tcpaddr,sizeof(struct sockaddr));
  //    if(ret != 0)
   //  {
    //      close(trueSocket);
    //     return -1;
   // printf("connection operation errno = %d\n", errno);
//	perror("connect");
  //    }

    FD_ZERO(&connectionAble);
    FD_SET(trueSocket,&connectionAble);
    selectRet=select(trueSocket+1,0,&connectionAble,0,&vl);
    if(selectRet == 0)
    {
#if GPRSPR
       // printf("connection timeout! errno = %d\n", errno);
	perror("connect socket select()");
#endif
        close(trueSocket);
        iResult=-1;
    }
    else if(selectRet > 0)
    {
        flags = fcntl(trueSocket, F_GETFL,0);
        flags &= ~ O_NONBLOCK;
        fcntl(trueSocket, F_SETFL,flags);
        FD_CLR(trueSocket,&connectionAble);
        *sock=trueSocket;
        iResult = 0;
#if GPRSPR
        printf("selectRet = %d\n",selectRet);
        //printf("connection sucess!\n");
#endif
    }
    return iResult;
}

/*
*************************************************************************************************************
- �������� : int write_datas_gprs(int sockfd, unsigned char *buffer, int buf_len)
- ����˵�� : д SOCKET ����
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
 int write_datas_tty(int fd, unsigned char *buffer, int buf_len)
{
    struct timeval tv;
    fd_set w_set;
    int bytes_to_write_total = buf_len;
    int bytes_have_written_total = 0;
    int bytes_write = 0;
    int result = -1;
    unsigned char *ptemp = buffer;
	//struct RecordHeader Head;

    if ((fd<0) ||( NULL == buffer )|| (buf_len <=0))
    {
#if GPRSPR
        printf("Send Buffer is Nc\n");
#endif
        ConnectFlag = 1;	//�������߱�־
        return -1;
    }

    while (bytes_to_write_total > 0)
    {
        FD_ZERO(&w_set);
        FD_SET(fd,&w_set);
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        result = select(fd+1, NULL, &w_set, NULL, &tv);
        if (result < 0)
        {
            if (EINTR == errno)
            {
                continue;
            }
            perror("Write socket select()");
            ConnectFlag = 1; //�������߱�־
            return -1;
        }
        else if (0 == result)    //this means timeout, it is not an error, so we return 0.
        {

#if GPRSPR
            printf("Send Data Timeout --->3\n");
#endif

            return 0;
        }
        else
        {
            if (FD_ISSET(fd, &w_set))
            {

#if GPRSPR
                printf("W socket=%03d::%s \n",bytes_to_write_total,ptemp);
#endif

                bytes_write = send(fd, ptemp, bytes_to_write_total, 0);

#if GPRSPR
                printf("bytes_write = %02X\n",bytes_write);
#endif

                if (bytes_write < 0)
                {
                    if (EAGAIN == errno || EINTR == errno)
                    {
                        continue;
                    }
#if GPRSPR
                    printf("open Send Macine is error2\n");
#endif
                    ConnectFlag = 1;	//�������߱�־
                    return -1;
                }
                else if (0 == bytes_write)
                {
#if GPRSPR
                    printf("Send Data Timeout --->2\n");
#endif
                    ConnectFlag = 1;	//�������߱�־
                    return -1;
                }
                else
                {
                    bytes_to_write_total -= bytes_write;
                    bytes_have_written_total += bytes_write;
                    ptemp += bytes_have_written_total;
#if GPRSPR
                    printf("Write GPRS data\n");
#endif
                }
            }
#ifdef GUANGZHOU_WEISHENG
			WEISHENG_UploadRecodeToServer();
#endif
        }
		
    }

#if GPRSPR
    printf("----------------->Send Data OK\n");
#endif
    return 0;
}


/*
*************************************************************************************************************
- �������� : int write_datas_gprs(int sockfd, unsigned char *buffer, int buf_len)
- ����˵�� : д SOCKET ����
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
//static int write_datas_gprs(int fd, unsigned char *buffer, int buf_len)
int write_datas_gprs(int fd, unsigned char *buffer, int buf_len)
{
    int status;
    pthread_mutex_lock(&m_socketwrite); //�߳�����
    status = write_datas_tty(fd,buffer,buf_len);
    pthread_mutex_unlock(&m_socketwrite);//�߳̽���
    return status;
}


/*
*************************************************************************************************************
- �������� : int write_datas_tty(int sockxx, unsigned char *buffer, int buf_len)
- ����˵�� : д SOCKET ���� ����ʱ����
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
/*
int write_datas_tty(int sockxx, unsigned char *buffer, int buf_len)
{
 	struct timeval tv;
	fd_set w_set;
	int bytes_to_write_total = buf_len;
	int bytes_have_written_total = 0;
	int bytes_write = 0;
	int result = -1;
	unsigned char *ptemp = buffer;

	if (-1 == sockxx || NULL == buffer || buf_len <=0)
	{
                //printf("���ͻ�����������\n");
		return -1;
	}

	while (bytes_to_write_total > 0)
	{
		FD_ZERO(&w_set);
                FD_SET(sockxx,&w_set);
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		result = select(sockxx+1, NULL, &w_set, NULL, &tv);
		if (result < 0)
		{
			if (EINTR == errno)
			{
				continue;
			}
			close(sockxx);
                        //printf("�򿪷�������ʧ��\n");
			return -1;
		}
		else if (0 == result)    //this means timeout, it is not an error, so we return 0.
		{
			//printf("�������ݳ�ʱ\n");
			return 0;
		}
		else
		{
			if (FD_ISSET(sockxx, &w_set))
			{
				bytes_write = send(sockxx, ptemp, bytes_to_write_total, 0);
				if (bytes_write < 0)
				{
					if (EAGAIN == errno || EINTR == errno)
					{
						continue;
					}
                                        //printf("�򿪷�������ʧ��2\n");
					close(sockxx);
					return -1;
				}
				else if (0 == bytes_write)
				{
					//this means that the server has close the connection gracefully.
					close(sockxx);
                                        //printf("�������ݳ�ʱ2\n");
					return -1;

				}
				else
				{
					bytes_to_write_total -= bytes_write;
					bytes_have_written_total += bytes_write;
					ptemp += bytes_have_written_total;
				}
			}
		}
	}
        //printf("��������OK\n");
        Heart = 0;
        heartlen = 0;
	return 0;
}

*/



/*
*************************************************************************************************************
- �������� : unsigned char read_datas_tty(int fd,unsigned char *buffer)
- ����˵�� : ��SOCKET����
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
int read_datas_tty(int fd,unsigned char *buffer)
{
    int retval;
    fd_set rfds;
    struct timeval tv;
    int ret = 0;
    int rilen = 0;


    if ((fd < 0) ||( NULL == buffer))
    {
#if GPRSPR
        printf("Read Buffer is Nc\n");
#endif
        ConnectFlag = 1; //�������߱�־
        return -1;
    }


    tv.tv_sec = 10;                   // the rcv wait time
    tv.tv_usec = 0;                   // 50ms

    while(1)
    {
        FD_ZERO(&rfds);
        FD_SET(fd,&rfds);
        retval = select(fd+1,&rfds,NULL,NULL,&tv);
        if(retval ==-1)
        {
            perror("Read socket select()");
            ConnectFlag = 1; //�������߱�־
            return -1;
        }
        else if(retval)
        {
           // ret= read(fd,buffer + rilen,512);
            ret= read(fd,buffer + rilen,2048);
            rilen += ret;
            if(ret > 0)
            {
                RI+=rilen;
            }
            else
            {
                ConnectFlag = 1; //�������߱�־
                return -1;
            }
#if GPRSPR
            printf("read socket ret:%d data:%s \n",ret,buffer);
#endif
            break;
        }
        else
        {
            return 1;
        }
    }
    return 0;
}

void Killpppd(void)
{
    switch(COMNET)
    {
    case 4:
        //system("/bin/sh /etc/ppp/ppp-off");
        system("/etc/ppp/peers/quectel-ppp-kill.sh");
        sleep(1);
        system("killall -9 pppd > /dev/null");
        system("killall -9 gprs > /dev/null");
        system("killall -9 pppd > /dev/null");
        sleep(2);
        break;
    case 3:
        //system("ppp-off");
        system("/etc/ppp/peers/quectel-ppp-kill.sh");
        sleep(1);
	system("killall pppd");
	sleep(1);
        system("killall chat");
        sleep(2);
        break;
    default :
        //system("ppp-off");
        system("/etc/ppp/peers/quectel-ppp-kill.sh");
        sleep(1);
        system("pkill -9 pppd > /dev/null");
        system("pkill -9 gprs > /dev/null");
        system("pkill -9 pppd > /dev/null");
        sleep(2);
        break;
    }
}

extern int mg_fd;
static int InitializePower(void)
{
	return mg_fd;
}


/****************************************************************************
* ���ƣ�ConnetWIFI()
* ���ܣ�����WIFI���нű�
* ��ڲ�������
* ���ڲ�������
****************************************************************************/
void ConnetWIFI()
{
    system("./network.sh ");   
	sleep(35);
}


static void sendDevFileErrorStatus(void)
{
	unsigned char buff[3];
	
	if (g_FgFileOccurError)
	{
		buff[0] = 0xb7;
		buff[1] = 0;
		buff[2] = 0xff;
		SendCmdPc(buff, 3);
		g_FgSendErrorAgain = 1;
	}
}

/****************************************************************************
* ���ƣ�int InquireSIMIMSI(char *dev)
* ���ܣ���ʼ��M26
* ��ڲ�������
* ���ڲ�������
****************************************************************************/
int M26_init(char *dev) 
{ 
	M26_fd = open(dev,O_RDWR); 
	if(M26_fd < 0)
	{
		//printf("��ͨ��ģ��ʧ��\n");
		return -1;
	} 
	
    if(M26_fd > 0)
    {
        set_speed (M26_fd, 115200);
        if (set_Parity (M26_fd, 8,1,'n') < 0)
        {
            close(M26_fd);
            //printf ("uart set_parity error\n");
        }
        return M26_fd;
    }

}

int read_ttyS0_data(int fd, char *rcv_buf, int *len)
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
			ret= read(fd, rcv_buf+pos, 64);
			if(ret>0)
			{
				pos += ret;
				*len = pos;
				if (pos >= 7)   //20
					return 0;
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
int InquireSIMIMSI(void)
{
	char * TX_buff = "AT+CIMI\r\n";
	char * TX_buff1 = "AT+COPS=1,2,\"46000\"\r\n";
	char * TX_buff2 = "AT+COPS=1,2,\"46001\"\r\n"; 

	char *cmd_cgreg = "AT+CGREG?\r\n";
	char * cmd_fcun1 = "AT+CFUN=1\r\n";
	char * cmd_fcun0 = "AT+CFUN=0\r\n";
	
	char RX_buff[64] = {"\0"};
	int ret, rlen;   
	unsigned char cnt = 0;

	//g_EnableCallPPPD = 1;    //enable first
	 cnt = 0;
 	rlen = 0;
	ret = M26_init("/dev/ttyS0"); 
	if(ret > 0)
	{  
		 cnt = 0;
		 do {
		 	 rlen = 0;
			 memset(RX_buff,0,sizeof(RX_buff));  
		 	 write(M26_fd,cmd_cgreg,strlen(cmd_cgreg)); 
			ret = read_ttyS0_data(M26_fd, RX_buff, &rlen);
			if (rlen)
			{
				if((strchr(RX_buff, '1') !=  NULL) || (strchr(RX_buff, '5') !=  NULL)) 
					break;
			}
			sleep(1);
			cnt++;
		   }while(cnt < 30); //30s check network status

		if (cnt >= 30)
		{
			if((strchr(RX_buff, '0') !=  NULL) && (strchr(RX_buff, '2') !=  NULL)) 
			{
				printf("error 0û�в忨������������\n");
			 	 write(M26_fd,cmd_fcun0,strlen(cmd_fcun0)); 
				 sleep(2);
				  write(M26_fd,cmd_fcun1,strlen(cmd_fcun1)); 
				  sleep(2);
				  close(M26_fd);
				return 1; 
			}
			else if ((strchr(RX_buff, '0') !=  NULL) && (strchr(RX_buff, '3') !=  NULL))    //��Ч�绰��
			{
				printf("error 00��Ч�绰��\n");
				return 1;
			}
		
			memset(RX_buff,0,sizeof(RX_buff));  
			write(M26_fd,TX_buff,strlen(TX_buff));   
			 cnt = 0;
 			rlen = 0;
			 do {
				ret = read_ttyS0_data(M26_fd, RX_buff, &rlen);
				if(ret == 1)
					cnt++;
				//printf("M26_fd read rev = %d, %s\n", cnt, RX_buff);
			 }while((ret != -1) && (!rlen) && (cnt < 2));      
			if(rlen > 0)   
			{ 
				if(strstr(RX_buff,"46004")!= NULL) 
				{
					 ret = write(M26_fd,TX_buff1,strlen(TX_buff1));   
					 cnt = 0;
					 rlen = 0;
					 memset(RX_buff,0,sizeof(RX_buff));  
					 do {
						ret = read_ttyS0_data(M26_fd, RX_buff, &rlen);
						if(ret == 1)
							cnt++;
						//printf("11111M26_fd read rev = %d, %s\n", rlen, RX_buff);
					 }while((ret != -1) && (!rlen) && (cnt < 80));

					   
					 cnt = 0;
					 do {
					 	 rlen = 0;
						 memset(RX_buff,0,sizeof(RX_buff));  
					 	 write(M26_fd,cmd_cgreg,strlen(cmd_cgreg)); 
						ret = read_ttyS0_data(M26_fd, RX_buff, &rlen);
						if (rlen)
						{
							//printf("111111CGREG = %d, %d, %s\n", ret, cnt, RX_buff);
							if((strchr(RX_buff, '1') !=  NULL) || (strchr(RX_buff, '5') !=  NULL)) 
								break;
						}
						sleep(1);
						cnt++;
					 }while(cnt < 30);

					 if (cnt >= 30)
					 {
					 	printf("error 1û�в忨�����ź�\n");
					 	 write(M26_fd,cmd_fcun0,strlen(cmd_fcun0)); 
						 sleep(2);
						  write(M26_fd,cmd_fcun1,strlen(cmd_fcun1)); 
						  sleep(2);
						  close(M26_fd);
						return 1; 
					 }
				}
				else if(strstr(RX_buff,"46006")!= NULL) 
				{
				 	 ret =  write(M26_fd,TX_buff2,strlen(TX_buff2)); 
				 	cnt = 0;
					 rlen = 0;
					 memset(RX_buff,0,sizeof(RX_buff));  
					 do {
						ret = read_ttyS0_data(M26_fd, RX_buff, &rlen);
						if(ret == 1)
							cnt++;
						//if(strstr(RX_buff, "OK")!= NULL) break;
						//printf("222222M26_fd read rev = %d, %s\n", cnt, RX_buff);
					 }while((ret != -1) && (!rlen) && (cnt < 80));

					 cnt = 0;
					 do {
					 	 rlen = 0;
						 memset(RX_buff,0,sizeof(RX_buff));  
					 	write(M26_fd,cmd_cgreg,strlen(cmd_cgreg)); 
						ret = read_ttyS0_data(M26_fd, RX_buff, &rlen);
						if (rlen)
						{
							if((strchr(RX_buff, '1') !=  NULL) || (strchr(RX_buff, '5') !=  NULL)) 
								break;
						}
						sleep(1);
						cnt++;
					 }while(cnt < 30);
					 
					 if (cnt >= 30)
					 {
					 	printf("error 2û�в忨�����ź�\n");
						 write(M26_fd,cmd_fcun0,strlen(cmd_fcun0)); 
						 sleep(2);
						  write(M26_fd,cmd_fcun1,strlen(cmd_fcun1)); 
						  sleep(2);
						  close(M26_fd);
						return 1; 
					 }
				}
				else {
					 cnt = 0;
					 do {
					 	 rlen = 0;
						 memset(RX_buff,0,sizeof(RX_buff));  
					 	 write(M26_fd,cmd_cgreg,strlen(cmd_cgreg)); 
						ret = read_ttyS0_data(M26_fd, RX_buff, &rlen);
						if (rlen)
						{
							if((strchr(RX_buff, '1') !=  NULL) || (strchr(RX_buff, '5') !=  NULL)) 
								break;
						}
						sleep(1);
						cnt++;
					 }while(cnt < 30);

					 if (cnt >= 30)
					 {
					 	printf("error 3û�в忨�����ź�\n");
						 write(M26_fd,cmd_fcun0,strlen(cmd_fcun0)); 
						 sleep(2);
						  write(M26_fd,cmd_fcun1,strlen(cmd_fcun1)); 
						  sleep(2);
						  close(M26_fd);
						return 1; 
					 }
				}
			} 
			else
			{
				printf("error 4û�в忨�����ź�\n");
				 write(M26_fd,cmd_fcun0,strlen(cmd_fcun0)); 
				 sleep(2);
				  write(M26_fd,cmd_fcun1,strlen(cmd_fcun1)); 
				  sleep(2);
				  close(M26_fd);
				  return 1; 
			}
		}
		close(M26_fd);
		sleep(2);
		return 0; 	  
	}
	printf("error 5�򿪴���ʧ�ܻ���Ҫȥ����\n");
	return 0;
}

/*
*************************************************************************************************************
- �������� : void * ReadGprs_Pthread (void * args)
- ����˵�� : SOCKET ���� �� ������  ����
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
#define		GPRS_REV_MAX_LEN	1024
#define		GPRS_REV_TMP_LEN	2048

void * ReadGprs_Pthread(void * args)
{
	int ret;
	struct stat file_info;
	unsigned int i;
	unsigned int  lend = 0,AHLen=0;
	unsigned int  Restartlen;
	unsigned char status;
	unsigned char fileBuffer[16];
	unsigned char filebuf[80];
	unsigned char send55AA[40];
	unsigned char  bufdata[8];
	unsigned char RcvCmd[GPRS_REV_TMP_LEN];
	unsigned char RCmdata[GPRS_REV_TMP_LEN];
	unsigned char rebuf[GPRS_REV_TMP_LEN];
	unsigned char Gprs_xf[GPRS_REV_TMP_LEN];
	unsigned char *dpp=NULL;
	unsigned char *dppd=NULL;
	unsigned char CallGprsCount = 0;
	char lcount;
	#ifdef USE_ISO8583_FORMAT
	unsigned short callLen = 0;
	#endif
	i = 0;
	Heart = 0;		//������������
	heartlen = 0;
	Restartlen = 0;
	ConnectFlag = 1;	//�������߱�־
	
	
	memset(G_STARTW,0x00,sizeof(G_STARTW));
	signal(SIGPIPE,SIG_IGN);  //�ر�SIGPIPE�źţ�������

	
	#if WCDMA  /*4Gģ����Ƶ�Դ*/
	w55fa93_setio(GPIO_GROUP_A, 1, 1);	
	sleep(1);
	#endif
	//FgM26Module = 0;   //init
	switch(COMNET)
	{
		case 3:
		Killpppd();

		cdma_rest(3);
		sleep(40);
		Display_signal(10);
		system("pppd call cdma  > /dev/null&");
		//system("pppd call cdma&");
		sleep(10);
		break;
		case 4:
#if WCDMA       
		cdma_rest(4);
	    Display_signal(70);
		sleep(30); //13
		Display_signal(10);
        		system("pppd call wcdma  > /dev/null&");
		g_EnableCallPPPD = 1;
#else	
		//FgM26Module = 1;
		cdma_rest(5);
	    Display_signal(70);
        sleep(20); 
		if (InquireSIMIMSI() == 0)   //enable  call pppd
		{
			Display_signal(10);
			g_EnableCallPPPD = 1;  
		 	system("pppd call gprs &");//gprs
		}
		else
		{
			g_EnableCallPPPD = 0;
			Display_signal(60);
		}
    		//system("pppd call gprs  > /dev/null&");//gprs
#endif	    
		sleep(10);
		break;
        case 2: 
        printf("\n close WIFI \n");
       /* system("killall -9 udhcpc");
        system("udhcpc -i ra0 -T 1");
        */
        system("killall -9 udhcpc");
        system("killall sh");
        system("sh /mnt/nand1-1/wifi/network.sh &");
        Display_signal(10);
        sleep(30);
        break;
    case 1:       
	default :
#if GPRSPR
		printf("\n close TCP/IP \n");
#endif  
        system("insmod /mnt/nand1-1/drivers/asix.ko");
        sleep(2);
        system("ifconfig eth0 down");
        sleep(2);
        system("ifconfig eth0 up");
        sleep(2);
        if((TcpIpBuf[0]>='0')&&(TcpIpBuf[0]<='9')&&(SelfAddress[0]>='0')&&(SelfAddress[0]<='9'))
    		{
    			memset(fileBuffer,'\0',sizeof(fileBuffer));
    			memset(filebuf,'\0',sizeof(filebuf));
    			memcpy(fileBuffer,SelfAddress,16);
    			sprintf(filebuf,"ifconfig eth0 ");
    			strcat(filebuf,fileBuffer);
    			system(filebuf);
    			usleep(200000);
#if GPRSPR
    			printf("\n	 ip eth0 %s \n",fileBuffer);
#endif

    			memset(fileBuffer,'\0',sizeof(fileBuffer));
    			memcpy(fileBuffer,SelfAddress+16,16);
    			sprintf(filebuf,"route add default gw ");
    			strcat(filebuf,fileBuffer);
    			system(filebuf);
    			usleep(200000);
    			system("route&");
        	}
    		else
    		{
    			printf("\n killall %d \n", system("killall udhcpc"));
    			system("udhcpc &");
    			usleep(200000);
    		}            
    		break;
	}

LOOP1:
	while(1)
	{
        sleep(5);
		system("ifconfig > /var/run/ppp0.txt");
		stat("/var/run/ppp0.txt" , &file_info);
		if(file_info.st_size!=0)
		{
			printf("\n�����ѳɹ�!!!!!!!!!\n");
			Display_signal(20);
			system("rm /var/run/ppp0.txt");
			system("sync");
			i = 0;
			break;
		}
		else
		{
			sleep(1);
			i++;
			if  (i >= 50)   ///10mins  100
			{
				printf("GPRSģ������!!!!!!!\n");
				i = 0;
				switch(COMNET)
				{
					case 3:
					Killpppd();		
					cdma_rest(3);
					sleep(40);
					Display_signal(10);
					system("pppd call cdma > /dev/null&");
					sleep(5);
					goto LOOP1; //may be goto LOOP1 better
					break;
		    		case 4:
		        	if (g_EnableCallPPPD)
		        	{
		        		//system("ppp-off ");
			        	system("/etc/ppp/peers/quectel-ppp-kill.sh");
	                    system("kill -9 $(ps | grep \"pppd call gprs\" |grep -v 'grep' | awk '{print $1}')");
	        			system("kill -9 $(ps | grep \"/etc/ppp/gprs\" |grep -v 'grep' | awk '{print $1}')");
	       				system("killall -9 pppd");
						system("killall chat");
		        	}
        			sleep(1);
        			cdma_rest(4);
                    Display_signal(70);
#if WCDMA            
                     sleep(30);  //13
                     Display_signal(10);
                    system("pppd call wcdma  > /dev/null &");
		            g_EnableCallPPPD = 1;
#else
                    sleep(20); 
            	  	 if (InquireSIMIMSI() == 0)   //enable  call pppd
            		{
            			Display_signal(10);
            			g_EnableCallPPPD = 1;  
            		 	system("pppd call gprs &");//gprs
            		}
            		else
            		{
            			g_EnableCallPPPD = 0;
            			Display_signal(60);
            		}
                   // system("pppd call gprs  > /dev/null &");
#endif 
        			sleep(10);
                    goto LOOP1; //may be goto LOOP1 better
		        	break;  
                    case 2:
                     system("killall -9 udhcpc");
                     system("killall sh");
                     system("sh /mnt/nand1-1/wifi/network.sh &");
                     Display_signal(10);
                    sleep(30);                        
                    break;
                    case 1:
		    		default :
#if GPRSPR
        				printf("\n close TCP/IP \n");
#endif                      
                    system("insmod /mnt/nand1-1/drivers/asix.ko");
                    sleep(2);
                    system("ifconfig eth0 down");
                    sleep(2);
                    system("ifconfig eth0 up");
                    sleep(2);
                    if((TcpIpBuf[0]>='0')&&(TcpIpBuf[0]<='9')&&(SelfAddress[0]>='0')&&(SelfAddress[0]<='9'))
    					{
							memset(fileBuffer,'\0',sizeof(fileBuffer));
							memset(filebuf,'\0',sizeof(filebuf));
							memcpy(fileBuffer,SelfAddress,16);
							sprintf(filebuf,"ifconfig eth0 ");
							strcat(filebuf,fileBuffer);
							system(filebuf);
							usleep(200000);
#if GPRSPR
							printf("\n	 ip eth0 %s \n",fileBuffer);
#endif

							memset(fileBuffer,'\0',sizeof(fileBuffer));
							memcpy(fileBuffer,SelfAddress+16,16);
							sprintf(filebuf,"route add default gw ");
							strcat(filebuf,fileBuffer);
							system(filebuf);
							usleep(200000);
							system("route&");
                    	}
    					else
    					{
							printf("\n killall %d \n", system("killall udhcpc"));
							system("udhcpc &");
							usleep(200000);
    					}                        
		        		break;
				}
			}
		}
	}

	for(;;)
	{
    	if(ConnectFlag == 0)  //���յ�����
    	{
        	ret = read_datas_tty(sockfd,rebuf);				//����������
    		if(ret  == 0)
    		{
    				Heart = 0;
    				heartlen = 0;
#ifdef USE_ISO8583_FORMAT
			if (RI >= 8) //heart packet length is 8
			{
				memcpy(Gprs_xf, rebuf, RI);   //test 
				//for(i=0; i<RI; i++)
				//	printf("%02x ", Gprs_xf[i]);
				//printf("\n");
			}
#else
			if((strlen(Gprs_xf) + strlen(rebuf)) < 1000)
			{
				strcat (Gprs_xf,rebuf);
				memset (rebuf,0,sizeof(rebuf));

	#if GPRSPR
				printf("Gprs_xf1 = %s\n",Gprs_xf);
	#endif
    		}
    		else
    		{
				memset (Gprs_xf,0,sizeof(Gprs_xf));
				strcat (Gprs_xf,rebuf);
				memset (rebuf,0,sizeof(rebuf));
	#if GPRSPR
				printf("Gprs_xf2 = %s\n",Gprs_xf);
	#endif
        		}
#endif
    		}
    		else if(ret == 1)
    		{
				AutoUpFlag = 0x55;		//��ʱ
				Heart ++;
				heartlen ++;
				if(heartlen >= 3)
				{
            	    status = SendHandData(HeartData);
        		}

        		if(Heart >= 12)
        		{
            		Heart = 0;
            		ConnectFlag = 1;
        		}
    		}

#ifdef USE_ISO8583_FORMAT
		callLen = 0;
Line_parse:
            		if(RI >= 8)
            		{
				//printf("=====rcvlen = %d\n", callLen);
				unsigned short rcvlen;
				//if (RI == 8)
				{
					//dppd = strstr(Gprs_xf+callLen, HeartData);
					//if(dppd != NULL)
					if (!strncmp(Gprs_xf+callLen, HeartData, strlen(HeartData)))
						{
							Heart = 0;
							RI -= 8;
							callLen += 8;
							dppd = NULL;
							if (RI > 0)
							{
								goto Line_parse;
							}
							RI = 0;
							memset (Gprs_xf,0,sizeof(Gprs_xf));
							memset (rebuf,0,sizeof(rebuf));
						}
					}

					if (RI > 0)
					{
						rcvlen = ((Gprs_xf[0+callLen]<<8) & 0xff00) | Gprs_xf[1+callLen];
						//printf("=====rcvlen = %d\n", rcvlen);
						if (RI >= (rcvlen+2))
						{
							//printf("rcv data length correct\n");
							if (!memcmp(Gprs_xf+2+callLen, HEADER_BUFF, HEADER_LENGTH))
							{
								Heart = 0;
								ParserRcvCmdData(Gprs_xf+callLen);
								RI = RI - (rcvlen+2);
								callLen += (rcvlen+2);
								if (RI > 0)
								{
									goto Line_parse;
								}
								RI = 0;
								memset (Gprs_xf,0,sizeof(Gprs_xf));
								memset (rebuf,0,sizeof(rebuf));
							}
							else
							{
								RI = 0;
								memset (Gprs_xf,0,sizeof(Gprs_xf));
								memset (rebuf,0,sizeof(rebuf));
							}
						}
						else
						{
							RI = 0;
							memset (Gprs_xf,0,sizeof(Gprs_xf));
							memset (rebuf,0,sizeof(rebuf));
						}
					}
            	}

#else
			
    		if(RI > 0)
    		{
	#if GPRSPR
        		printf("----have data OK \n");
        		printf("have data\n");
	#endif
				sprintf(G_STARTW,"TT%02X%02X%02X",IDaddr.longbuf[0],IDaddr.longbuf[1],IDaddr.longbuf[2]);
				dppd = strstr(Gprs_xf,G_STARTW);
				if(dppd != 0)
				{
					ascii_2_hex(dppd +2,bufdata,6);
					WeiBuf.i = 0;
					memcpy(WeiBuf.longbuf,bufdata,3);
	#if GPRSPR
					printf("----Auto Send data:%d\n",WeiBuf.i);
	#endif
					status = UPdateRecord(WeiBuf.i,0xaa);
					if(status == 0)
					{
	#if GPRSPR
						printf("Auto is sucess\n");
	#endif
						status = 0;
						memset(dppd,0xff,sizeof(G_STARTW));// ������صı��
						memset(G_STARTW,0,sizeof(G_STARTW));
						AutoUpFlag = 0xBB;
						memset (Gprs_xf,0,sizeof(Gprs_xf));
						memset (rebuf,0,sizeof(rebuf));
						RI = 0;
        			}
        		}
	#if  HEART
			dppd = strstr(Gprs_xf,HeartData);
			if(dppd != NULL)
			{
				Heart = 0;
				memset(dppd,0xff,sizeof(HeartData));// ������صı��
				Netstatus = 1;                      //���յ�����������������־����ʾ�ڿ���״̬�²Ž����ļ����� 
			}
	#endif

			dpp = strstr(Gprs_xf,CMDSTART);
			if(dpp != NULL)
			{
				Heart = 0;
				heartlen = 0;
				memset(RcvCmd,0,sizeof(RcvCmd));
				lend = LenDataCmd(dpp+ 15);
				if((lend > 0)&&(lend <= 200))
				{
					memcpy(RcvCmd,dpp+5,(lend+8)*2);
					if(((lend+8)*2 == strlen(RcvCmd)))  //�յ�������������
					{
	#if GPRSPR
						printf("------------>");
						printf(RcvCmd);		//�յ�������������
						printf("\n");		//�յ�����������
	#endif
						memcpy(G_START,dpp+5,8);	//Э����
						memset(dpp,'f',5);

						AHLen = ascii_2_hex(RcvCmd+8,RCmdata,(lend+4)*2);
						if(AHLen == (lend + 4))
						{
							All_Cmd_Snd(RCmdata);
							RI = 0;
							memset (Gprs_xf,0,sizeof(Gprs_xf));
						}
            			}
            			else
            			{
            				RI = 0;
            				memset(dpp,0xff,5);
            			}
        			}
        			else
        			{
            			RI = 0;
            			memset(dpp,0xff,5);
        			}
        		}
        /*		else
        		{
        			RI = 0;
        			memset (Gprs_xf,0,sizeof(Gprs_xf));
        		}
        		*/

                dpp = strstr(Gprs_xf,FILEHEAD);        //�����ļ�����  
                if(dpp != NULL)
                {
                    Heart = 0;
					heartlen = 0;
                    memset(RcvCmd,0,sizeof(RcvCmd));
                    lend = LenDataCmd1(dpp+ 14);      
                    //printf("���յ������ݳ���:%d\n",lend);  
					
                    if(lend < 2048)  
                    { 
                        memcpy(RcvCmd,dpp+4,(lend+9)*2);   
						//printf("strlen(RcvCmd)= %d\n",strlen(RcvCmd));  
                        if(((lend+9)*2 == strlen(RcvCmd)))  //�յ�������������
                        {
	#if GPRSPR
                            printf("FIEL------------>"); 
                            printf(RcvCmd);		//�յ�������������
                            printf("\n");		//�յ�����������
	#endif
                            memcpy(G_START,dpp+4,8);	//Э����
                            memset(dpp,'f',5);
                            memset(RCmdata,0,sizeof(RCmdata));
                            AHLen = ascii_2_hex(RcvCmd+8,RCmdata,(lend+5)*2);  //�����ݴ�ASC2ת����HEX
                            //printf("ת�������ݳ���:%d\n",AHLen);
	#if UPDEG 
                            printf("CHANGCODEFIEL------------>");
                            for(i=0;i<(lend+5);i++)
                            {
                                printf("0x%02x ",RCmdata[i]);       
                                }
                            printf("\n");		//�յ�����������
	#endif                             
                            if(AHLen == (lend + 5))
                            {
                               All_Cmd_Snd1(RCmdata);    
								
                                RI = 0;
                                memset (Gprs_xf,0,sizeof(Gprs_xf));
                            }
                        }
                        else
                        {
                            RI = 0;
                            memset(dpp,0xff,5);
                        }
                    }
                    else
                    {
                        RI = 0;
                        memset(dpp,0xff,5);
                    }                    
                    }
                else
                {
                    RI = 0;
                    memset (Gprs_xf,0,sizeof(Gprs_xf));
                }        
    		}
         if(g_FgSendErrorAgain)
	     {  
	     	    SendAgainCnt++;
		   if (SendAgainCnt > 2)
		   {
		   	    SendAgainCnt = 0;
                sendDevFileErrorStatus();
		   }
	     }
#endif
    	}
    	else
    	{
		ConnectFlag = 1; //�������߱�־
		if(sockfd>0)
		close(sockfd);
		sleep(3);
		printf("�������ӷ�����!!!!!!!!\n");
		ret = createProxy(&sockfd);
		if(ret != 0)
		{
#if  GPRSPR
    		printf("----------->>>1 createProxy  Restartlen == %d \n",Restartlen);
#endif
			Display_signal(50);
			CallGprsCount++;
			
			if(CallGprsCount > 3)
				WEISHENG_ShowFlag = 1;
			Restartlen++;
			//printf("1111Restartlen creat = %d\n", Restartlen);
			if(Restartlen >= 39)   //600 s == 10mins  real is 78
			//if(Restartlen >= 4)
            {
    			Restartlen = 0;
    			switch(COMNET)
    			{
    				case 4:
#if GPRSPR
        				printf("\n createProxy close GPRS \n");
#endif
				if (g_EnableCallPPPD)
				{
	        				//system("ppp-off ");
	        				system("/etc/ppp/peers/quectel-ppp-kill.sh");
	        				system("kill -9 $(ps | grep \"pppd call gprs\" |grep -v 'grep' | awk '{print $1}')");
	        				system("kill -9 $(ps | grep \"/etc/ppp/gprs\" |grep -v 'grep' | awk '{print $1}')");
	        				system("killall -9 pppd ");
				}
        				sleep(1);
        				cdma_rest(4);
                        Display_signal(70);
#if WCDMA            
                        sleep(30);
		                Display_signal(10);
                        system("pppd call wcdma > /dev/null &");
			        g_EnableCallPPPD = 1;
#else
                        sleep(20); 
                        //system("pppd call gprs > /dev/null &");
	             if (InquireSIMIMSI() == 0)   //enable  call pppd
                    {
                    	Display_signal(10);
                    	g_EnableCallPPPD = 1;  
                     	system("pppd call gprs &");//gprs
                    }
                    else
                    {
                    	g_EnableCallPPPD = 0;
                    	Display_signal(60);
                    }
#endif 
				sleep(10);
        				goto LOOP1;
        				break;
    				case 3:
        				Killpppd();		
        				cdma_rest(3);
        				sleep(40);
        				Display_signal(10);
        				system("pppd call cdma > /dev/null &");
        				sleep(5);
        				goto LOOP1;
        				break;
    				case 2:
#if GPRSPR
        				printf("\n close WIFI \n");
#endif
                        system("killall -9 udhcpc");
                        system("killall sh");
                        system("sh /mnt/nand1-1/wifi/network.sh &");
                        Display_signal(10);
                        sleep(30);
                        goto LOOP1;
        				break;
    				case 1:
    				default:
#if GPRSPR
        				printf("\n close TCP/IP \n");
#endif
                        system("insmod /mnt/nand1-1/drivers/asix.ko");
                        sleep(2);
                        system("ifconfig eth0 down");
                        sleep(2);
                        system("ifconfig eth0 up");
                        sleep(2);
        				if((TcpIpBuf[0]>='0')&&(TcpIpBuf[0]<='9')&&(SelfAddress[0]>='0')&&(SelfAddress[0]<='9'))                   
                        {
							memset(fileBuffer,'\0',sizeof(fileBuffer));
							memset(filebuf,'\0',sizeof(filebuf));
							memcpy(fileBuffer,SelfAddress,16);
							sprintf(filebuf,"ifconfig eth0 ");
							strcat(filebuf,fileBuffer);
							system(filebuf);
							usleep(200000);
#if GPRSPR
							printf("\n	 ip eth0 %s \n",fileBuffer);
#endif

							memset(fileBuffer,'\0',sizeof(fileBuffer));
							memcpy(fileBuffer,SelfAddress+16,16);
							sprintf(filebuf,"route add default gw ");
							strcat(filebuf,fileBuffer);
							system(filebuf);
							usleep(200000);
							system("route &");
        				}
        				else
        				{
						system("killall -9 udhcpc");
						system("udhcpc &");
						usleep(200000);
        				}
                        Display_signal(10);
                        sleep(20);
                        goto LOOP1;                        
        				break;
    				}
        		}
    		}
    		else  						  		//��������
    		{
        		sleep(1);
#ifndef USE_ISO8583_FORMAT
				
#ifdef ADD_MONEY_SUPPORT
                sprintf(send55AA,"TAB:VoucherCenter");                
#else

    #if defined(CONFIG_BZLINUXBUS)

        #if (defined  Transport_Stander)
                    sprintf(send55AA,"TAB:LINUXBAOYINGBUS");
        #elif  (defined CANGNAN_BUS)                
                    sprintf(send55AA,"TAB:LINUXCANGNANBUS");         
        #else
            		sprintf(send55AA,"TAB:LINUXBZCPUCARDBUS");
        #endif

    #elif defined(CONFIG_LINUXBUS8BIT)

        #ifdef Transport_Stander
                    sprintf(send55AA,"TAB:LINUXBAOYINGBUS");            
        #else
            		sprintf(send55AA,"TAB:LINUX8BITBZBUS");
        #endif

    #elif defined(CONFIG_LINUXBUS32BIT)
        #ifdef Transport_Stander
                    sprintf(send55AA,"TAB:LINUXBAOYINGBUS");            
        #else
            		sprintf(send55AA,"TAB:LINUX8BITBZBUS");
        #endif
    #elif defined CONFIG_LINUXBUS2SEC
                    sprintf(send55AA,"TAB:LINUXBZCPUCARDBUS");
    #endif

#endif

				Display_signal(40);
#ifdef GUANGZHOU_WEISHENG
				WEISHENG_ShowFlag = 0;
				WEISHENG_UploadRecodeToServer();
#endif

#endif  //USE_ISO8583_FORMAT

        		lcount = 3;
        		while(lcount--)  //�����ն˻��ź��ն�����
        		{
        	#ifdef USE_ISO8583_FORMAT
				status = FirstConnectNetWork();
			#else
        			status = SendHandData(send55AA);
			#endif
        			if(status != 0)
        			{
#if  GPRSPR
            			printf("----------->>>1  Restartlen == %d \n",Restartlen);
#endif
            			Restartlen ++;
            			//if(Restartlen >= 12)
            			if(Restartlen >= 67)   ///10mins  135
            			{
            				Restartlen = 0;
            				switch(COMNET)
            				{
            					case 4:
#if GPRSPR
            					printf("\n close GPRS \n");
#endif
        						if (g_EnableCallPPPD)
        						{
                					//system("ppp-off ");
                					system("/etc/ppp/peers/quectel-ppp-kill.sh");
                					system("kill -9 $(ps | grep \"pppd call gprs\" |grep -v 'grep' | awk '{print $1}')");
                					system("kill -9 $(ps | grep \"/etc/ppp/gprs\" |grep -v 'grep' | awk '{print $1}')");
        							system("killall -9 pppd > /dev/null");
        						}
                                    cdma_rest(4);
                                    Display_signal(70);
#if WCDMA            
                                     sleep(30);
            				        Display_signal(10);
                                    system("pppd call wcdma  > /dev/null&");
            				        g_EnableCallPPPD = 1;
#else
                                    sleep(20); 
                                                //system("pppd call gprs  > /dev/null&");
            			         if (InquireSIMIMSI() == 0)   //enable  call pppd
                        			{
                        				Display_signal(10);
                        				g_EnableCallPPPD = 1;  
                        			 	system("pppd call gprs &");//gprs
                        			}
                        			else
                        			{
                        				g_EnableCallPPPD = 0;
                        				Display_signal(60);
                        			}
#endif  						
                				    sleep(10);
                				    goto LOOP1;
                    				break;
                        			case 3:
                					Killpppd();		
                					cdma_rest(3);
                					sleep(40);
                					Display_signal(10);			
                					/*
                					wavecom_power_off();
                					sleep(5);
                					wavecom_power_no();
                					sleep(16);
                					*/
                					system("pppd call cdma  > /dev/null &");
                					sleep(5);
                					goto LOOP1;
                					break;
                        			case 2:
#if GPRSPR
                            					printf("\n close WIFI \n");
#endif
                                                
                                        system("killall -9 udhcpc");
                                        system("killall sh");
                                        system("sh /mnt/nand1-1/wifi/network.sh &");
                                        sleep(30);
                                        Display_signal(10);
                                        goto LOOP1;
                                        

                    					break;
                					    case 1:
                					    default:
#if GPRSPR
                    					printf("\n close TCP/IP \n");
#endif
                                        system("insmod /mnt/nand1-1/drivers/asix.ko");
                                        sleep(2);
                                        system("ifconfig eth0 down");
                                        sleep(2);
                                        system("ifconfig eth0 up");
                                        sleep(2);
                    					if((TcpIpBuf[0]>='0')&&(TcpIpBuf[0]<='9')&&(SelfAddress[0]>='0')&&(SelfAddress[0]<='9'))
                    					{
            								memset(fileBuffer,'\0',sizeof(fileBuffer));
            								memset(filebuf,'\0',sizeof(filebuf));
            								memcpy(fileBuffer,SelfAddress,16);
            								sprintf(filebuf,"ifconfig eth0 ");
            								strcat(filebuf,fileBuffer);
            								system(filebuf);
            								usleep(200000);
#if GPRSPR
            								printf("\n	 ip eth0 %s \n",fileBuffer);
#endif

            								memset(fileBuffer,'\0',sizeof(fileBuffer));
            								memcpy(fileBuffer,SelfAddress+16,16);
            								sprintf(filebuf,"route add default gw ");
            								strcat(filebuf,fileBuffer);
            								system(filebuf);
            								usleep(200000);
            								system("route&");
                                    	}
                    					else
                    					{
            								printf("\n killall %d \n", system("killall udhcpc"));
            								system("udhcpc &");
            								usleep(200000);
                    					}
                                        Display_signal(10);
                                        sleep(20);
                                        goto LOOP1;                                    
                    					break;
                    				}
            			}
            			break;
        			}
        			usleep(500000);
        		}
				
		DebugPrintf("status = %u\n",status);

		
                if(status == 0)
        		{
        			#ifndef USE_ISO8583_FORMAT
        			sprintf(send55AA,"55AA%02X%02X%02X%02X\n",\
                	DevNum.longbuf[0],DevNum.longbuf[1],DevNum.longbuf[2],DevNum.longbuf[3]);
        			lcount = 3;
        			while(lcount--)
        			{
            			status = SendHandData(send55AA);
            			if(status != 0) 
						{
							ConnectFlag = 1;	//������
        							close(sockfd);
							sockfd = -1;
							break;
						}
            			usleep(500000);
        			}
			DebugPrintf("status = %u\n",status);
			#endif
        			if(status == 0)
        			{
        				#ifndef USE_ISO8583_FORMAT
                    		sendDevFileErrorStatus(); 
				#endif
            			AutoUpFlag = 0x55;
            			ConnectFlag = 0;
            			Heart = 0;
#ifndef GUANGZHOU_WEISHENG
			            Display_signal(30);
#endif
			            //DebugPrintf("ConnectFlag = 0x&02X\n", ConnectFlag);
            			//Display_signal(ConnectFlag);ȥ�����з�ֹ��������
        			}
        		}
    		}
    	}
	}
	ConnectFlag = 1;//������
	close(sockfd);
}





#endif
