/*
** $Id: progressbar.c,v 1.10 2007-08-30 01:20:10 xwyan Exp $
**
** Listing 25.1
**
** progressbar.c: Sample program for MiniGUI Programming Guide
**      Usage of PORGRESSBAR control.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** License: GPL
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <apparel.h>
#include "../gui/RC500.h"
#include "../gui/OnlineRecharge.h"
#include "../gui/InitSystem.h" //OnlineRecharge.h
#include "../sound/sound.h"
#include "../gui/des.h"

#include "../sound/sound.h"
#include "../Display/fbtools.h"
#include "../input/keyboard.h"


#define USBPRINT 1




Operat USBFile;
FILE *USBParaFile;

//FILE *BlackFilebuf;
//extern FILE *BlackFile;
extern unsigned char ReadCardFirst;
extern LongUnon DevNum;
extern SysTime Time;
extern unsigned char *CardLanBuf;
extern CardLanSector LanSec;		//�û�����
extern SectionFarPar Section,Sectionup;
unsigned char *blackbuf = NULL;
extern unsigned char g_FgFileOccurError;

/*
*************************************************************************************************************
- �������� : unsigned char UsbCheckCardlanBin(void)
- ����˵�� : USB��ȡ��������
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char UsbCheckCardlanBin(void)
{
    int result;
    unsigned char Nandbuf[512];
    unsigned char Stu = 1;

    if (!(access("/mnt/usb/cardlan.bin", 0)))
    {
        USBParaFile = fopen("/mnt/usb/cardlan.bin","rb+");
        memset (Nandbuf,0,sizeof(Nandbuf));
        result = fseek(USBParaFile, 0, SEEK_SET);
        result = fread(Nandbuf,sizeof(unsigned char),512,USBParaFile);
        fclose(USBParaFile);
        if((Nandbuf[510] == 0x55)&&(Nandbuf[511] == 0xaa))
        {
            if(Nandbuf[0] == 0x55)
            {
                USBFile.Parameters = 1; //��������
                Stu = 0;
            }
            if(Nandbuf[80] == 0x55)
            {
                USBFile.Collection = 1; //�ɼ�����
                Stu = 0;
            }
            if((Nandbuf[176] == 0x55)||(Nandbuf[176] == 0x44)||(Nandbuf[176] == 0x33))
            {
                USBFile.Blacklist = 1;  //���º�����
                Stu = 0;
            }
            if(Nandbuf[336] == 0x55)
            {
                USBFile.PrintfStart = 1; //��ӡͷ
                Stu = 0;
            }
            if(Nandbuf[352] == 0x55)
            {
                USBFile.PrintfEnd = 1;  //��ӡβ
                Stu = 0;
            }
            if(Nandbuf[400] == 0x55)
            {
                USBFile.Sound = 1;     // ��������
                Stu = 0;
            }

            if(Nandbuf[96] == 0x55)//��ʽ������
            {
                USBFile.RmAll = 1; //��ʽ������
                Stu = 0;
            }

            if(Nandbuf[112] == 0x55)//�����û�����
            {
                USBFile.SetSector= 1;
                Stu = 0;
            }

            if(Nandbuf[144] == 0x55)//�����ն˻���
            {
                USBFile.SetDev= 1;
                Stu = 0;
            }

            if(Nandbuf[304] == 0x55)//���÷ֶ��շѣ����зֶ���Ч
            {
                USBFile.Section = 1;
                Stu = 0;
            }

            if(Nandbuf[320] == 0x55)//���÷ֶ��շѣ����зֶ���Ч
            {
                USBFile.Sectionup = 1;
                Stu = 0;
            }

            if(Nandbuf[496] == 0x55)
            {
                USBFile.Upgrade = 1; //���³���
                Stu = 0;
            }

        }
    }
    return(Stu);
}
/*
*************************************************************************************************************
- �������� : unsigned char UsbDownloadPara(void)
- ����˵�� : USB���ز���
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char UsbDownloadPara(void)
{
    unsigned char Stu = 1;
    unsigned char Nandbuf[512];
    int result;

    if (!(access("/mnt/usb/cardlan.bin", 0)))
    {
        USBParaFile = fopen("/mnt/usb/cardlan.bin","rb+");
        result = fseek(USBParaFile,0, SEEK_SET);
        result = fread(Nandbuf,sizeof(unsigned char),512,USBParaFile);
        ReadCardFirst  = Nandbuf[1];
        ReadOrWriteFile(SETCARDFIRST);

        memset (CardLanBuf,0,sizeof(CardLanBuf));
        result = fseek(USBParaFile, 512, SEEK_SET);
        result = fread(CardLanBuf,sizeof(unsigned char),16384,USBParaFile);
        fclose(USBParaFile);
		USBParaFile = NULL;

        #ifdef SAVE_CONSUM_DATA_DIRECT
        USBParaFile = fopen("/mnt/record/cardlan.sys","rb+");
        #else
        USBParaFile = fopen("/var/run/cardlan.sys","rb+");
        #endif
        result = fseek(USBParaFile, 0, SEEK_SET);
        result = fwrite(CardLanBuf,sizeof(unsigned char),16384,USBParaFile);
        fclose(USBParaFile);
        Stu = 0;

		Section.Enable = 0;
		ReadOrWriteFile(SETSECTION);
		
        usleep(500000);
    }

//	system("sync");
    return(Stu);
}
/*
*************************************************************************************************************
- �������� : unsigned char UsbDownloadPrintfStart(void)
- ����˵�� : USB���ô�ӡͷ
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char UsbDownloadPrintfStart(void)
{
    unsigned char Stu = 1;
    if (!(access("/mnt/usb/Sprint.txt", 0)))
    {
        system ("cp /mnt/usb/Sprint.txt  /var/run/start.txt");
        usleep(500000);
        Stu = 0;
    }
    return(Stu);
}

/*
*************************************************************************************************************
- �������� : unsigned char UsbDownloadPrintfEnd(void)
- ����˵�� : USB���ô�ӡβ
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char UsbDownloadPrintfEnd(void)
{
    unsigned char Stu = 1;
    if (!(access("/mnt/usb/Eprint.txt", 0)))
    {
        system ("cp /mnt/usb/Eprint.txt /var/run/end.txt");
        usleep(500000);
        Stu = 0;
    }
    return(Stu);
}
/*
*************************************************************************************************************
- �������� : unsigned char UsbUploadSdData (void)
- ����˵�� : USB�ϴ�����
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char UsbUploadSdData (void)
{
    unsigned char Buff[100];
    unsigned char Stu = 1;
    unsigned char buff[7];

    memset(buff,0,sizeof(buff));
    Rd_time (buff);
    Time.month = buff[1];
    Time.day = buff[2];
    if (!(access(OFF_LINE_CONSUM_FILE, 0)))
    {
        memset(Buff,0,sizeof(Buff));		
        sprintf(Buff,"cp "OFF_LINE_CONSUM_FILE" /mnt/usb/%08d.BIN",DevNum.i);
        system (Buff);
        usleep(500000);
		system("sync");
        Stu = 0;
    }
    return(Stu);
}
/*
*************************************************************************************************************
- �������� : unsigned char UsbUpdateSdProgram (void)
- ����˵�� : USB��������
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char UsbUpdateSdProgram (void)
{
    unsigned char Stu = 1;
    if (!(access("/mnt/usb/armok", 0)))
    {
        system ("cp /mnt/usb/armok /var/run/armok.new");
        //system ("cp /mnt/usb/armok /var/run");
        usleep(500000);
		system("sync");
        Stu = 0;
		
    }
    return(Stu);
}


/*
*************************************************************************************************************
- �������� : unsigned char UsbUpdataSound(void)
- ����˵�� : USB��������
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char UsbUpdataSound(void)
{
    unsigned char status = 1;
    unsigned char i,name[30];
    unsigned char buff[60];

    for(i = 1; i<MAX_FLOOR; i++)
    {
        memset(name,0,sizeof(name));
        sprintf(name,"/mnt/usb/music%02d.wav",i); //music01.wav
        if(access(name, 0) == 0)
        {
#if USBPRINT
            printf("UpdataSound 11 %s \n",name);
#endif
            memset(buff,0,sizeof(buff));
            sprintf(buff,"cp %s /var/run/sound/ ",name);
#if USBPRINT
            printf("UpdataSound 22 %s \n",buff);
#endif
            system(buff);
            usleep(200000);
			system("sync");
            status = 0;
        }
    }

//    readsound();

    return status;
}




/*
*************************************************************************************************************
- �������� : static unsigned char Blacklist (void)
- ����˵�� : SD�����º�����
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
static unsigned char UsbBlacklist (void)
{
    FILE *BlackFilebuf;
    int result;
    unsigned int CardByte,CardBit,i,file_pc;
    unsigned char Stu = 0xff;
    unsigned char Nandbuf[512];
    unsigned char CsnBuf[12];
    unsigned char Cardnum[6];
    unsigned char FileBuf[9];
    unsigned char NumBuf,t;
    LongUnon Buf,Test;

    if (!(access("/mnt/usb/cardlan.bin", 0)))
    {
        USBParaFile = fopen("/mnt/usb/cardlan.bin","rb+");
        memset (Nandbuf,0,sizeof(Nandbuf));
        result = fseek(USBParaFile, 0, SEEK_SET);
        result = fread(Nandbuf,sizeof(unsigned char),512,USBParaFile);
        if((Nandbuf[176] == 0x33)&&(Nandbuf[192] == 0x55)&&(Nandbuf[193] == 0xaa))
        {
            #ifdef SAVE_CONSUM_DATA_DIRECT
            system("rm -rf /mnt/record/Blacklist.sys");
            BlackFilebuf = fopen("/mnt/record/Blacklist.sys","a+");
            #else
            system("rm -rf /var/run/Blacklist.sys");
            BlackFilebuf = fopen("/var/run/Blacklist.sys","a+");
            #endif
            if(BlackFilebuf)
            {
                fclose(BlackFilebuf);
            }
            Stu = 0;
        }
        else
        {
            memcpy(Buf.longbuf,Nandbuf+192,4);			//�ļ���ʼλ��
            memcpy(Test.longbuf,Nandbuf+196,4);			//�������ĸ���
            if((Buf.i <= 0x4800)&&(Test.i <= 1000000))
            {
                #ifdef SAVE_CONSUM_DATA_DIRECT
                if(Nandbuf[176] == 0x44)				//��ʽ��������
                {
                    system("rm -rf /mnt/record/Blacklist.sys");
                    BlackFilebuf = fopen("/mnt/record/Blacklist.sys","a+");
                    fclose(BlackFilebuf);
                }

                BlackFilebuf = fopen("/mnt/record/Blacklist.sys","rb+");
                #else
                if(Nandbuf[176] == 0x44)				//��ʽ��������
                {
                    system("rm -rf /var/run/Blacklist.sys");
                    BlackFilebuf = fopen("/var/run/Blacklist.sys","a+");
                    fclose(BlackFilebuf);
                }

                BlackFilebuf = fopen("/var/run/Blacklist.sys","rb+");

                #endif
                if(BlackFilebuf)
                {
                    file_pc = Buf.i;

                    // printf("UsbBlacklist %02X  ==  %d\n",file_pc,Test.i);

                    for(i = 0; i < Test.i; i++)
                    {
                        memset(Cardnum,0,sizeof(Cardnum));
                        memset(CsnBuf,0,sizeof(CsnBuf));
                        result = fseek(USBParaFile, file_pc, SEEK_SET);
                        result = fread(Cardnum,sizeof(unsigned char),5,USBParaFile);
                        hex_2_ascii(Cardnum+1,CsnBuf,4);
                        Buf.i = myatoi(CsnBuf);
                        CardByte = Buf.i/8;
                        CardBit = Buf.i%8;

                        //    printf("nu %06d,,%d,,%d\n",Buf.i,CardByte,CardBit);

                        memset(FileBuf,0,sizeof(FileBuf));
                        result = fseek(BlackFilebuf,CardByte, SEEK_SET);
                        result = fread(FileBuf,sizeof(unsigned char),1,BlackFilebuf);

                        // printf("File1 %02X\n",FileBuf[0]);

                        if(Cardnum[0] == 0x35)
                        {
                            NumBuf = 0x01;
                            NumBuf = NumBuf << CardBit;
                            FileBuf[0] = FileBuf[0]|NumBuf;
                            //printf("File2 %02X\n",FileBuf[0]);
                            result = fseek(BlackFilebuf,CardByte, SEEK_SET);
                            result = fwrite(FileBuf,sizeof(unsigned char),1,BlackFilebuf);
                            /*
                                                            memset(FileBuf,0,sizeof(FileBuf));
                            								result = fseek(BlackFilebuf,CardByte, SEEK_SET);
                            								result = fread(FileBuf,sizeof(unsigned char),1,BlackFilebuf);
                            								printf("File3 %02X\n",FileBuf[0]);
                            */
                        }
                        else
                        {

                            NumBuf = 0x01;
                            NumBuf = NumBuf << CardBit;
                            if((NumBuf & FileBuf[0]))
                            {
                                NumBuf = 0xFE;
                                for(t = 0; t < CardBit; t++) NumBuf = (NumBuf << 1)|0x01;
                                FileBuf[0] = FileBuf[0] & NumBuf;
                                result = fseek(BlackFilebuf,CardByte, SEEK_SET);
                                result = fwrite(FileBuf,sizeof(unsigned char),1,BlackFilebuf);
                                /*
									 result = fseek(BlackFilebuf,CardByte, SEEK_SET);
									 result = fread(FileBuf,sizeof(unsigned char),1,BlackFilebuf);
									 printf("File3 %2X\n",FileBuf[0]);
                                */
                            }				  //���
                        }
                        file_pc += 5;

                    }
                    Stu = 0;
                }
                fclose(BlackFilebuf);
            }
        }
        fclose(USBParaFile);
    }

    return(Stu);

}



/*
*************************************************************************************************************
- �������� : unsigned char UsbBlacklist (void)
- ����˵�� : USB���º�����
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
/*
 unsigned char UsbBlacklist (void)
{
int result,file_pc;
unsigned char Stu = 1;
unsigned char j,i;
unsigned char Nandbuf[512];
unsigned char CsnBuf[11];
LongUnon Buf,Test;

	if (!(access("/mnt/usb/cardlan.bin", 0)))
        {
		USBParaFile = fopen("/mnt/usb/cardlan.bin","rb+");
		memset (Nandbuf,0,sizeof(Nandbuf));
		result = fseek(USBParaFile, 0, SEEK_SET);
		result = fread(Nandbuf,sizeof(unsigned char),512,USBParaFile);
        	fclose(USBParaFile);
              if((Nandbuf[176] == 0x33)&&(Nandbuf[192] == 0x55)&&(Nandbuf[193] == 0xaa))
               {
                      system("rm -rf Blacklist.sys");
		      BlackFilebuf = fopen("Blacklist.sys","a+");
		      if(BlackFilebuf)
		      {
		                fclose(BlackFilebuf);
		      }
               }
              else
              {

		if(Nandbuf[176] == 0x44)				//��ʽ��������
                {
		      system("rm -rf Blacklist.sys");
		      BlackFilebuf = fopen("Blacklist.sys","a+");
		      if(BlackFilebuf)
		      {
		                fclose(BlackFilebuf);
		      }
                }


		memcpy(Buf.longbuf,Nandbuf+192,4);			//�ļ���ʼλ��
		memcpy(Test.longbuf,Nandbuf+196,4);			//�������ĸ���
                if(Buf.i > 0xa00000) return 1;
                if(Test.i > 0x30d40) return 1;
	 	file_pc = Buf.i;
                USBParaFile = fopen("/mnt/usb/cardlan.bin","rb+");
		for(i = 0; i < Test.i;i++)
		{

                        result = fseek(USBParaFile, file_pc, SEEK_SET);
                        result = fread(Nandbuf,sizeof(unsigned char),5,USBParaFile);
		  	memset(CsnBuf,0,sizeof(CsnBuf));
	          	hex_2_ascii(Nandbuf+1,CsnBuf,4);
	          	Buf.i = myatoi(CsnBuf);
	          	if(Nandbuf[0] == 0x35)j = 0;
	          	else j = 1;
	         	Number(Buf.i,j);
                        file_pc += 5;
		}
                fclose(USBParaFile);
		usleep(500000);
              }
	      Stu = 0;
	}
        return(Stu);

}

*/
/*
*************************************************************************************************************
- �������� : static unsigned char  UsbSetSector(void)
- ����˵�� : USBȡ��������
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
static unsigned char  UsbSetSector(void)
{
    int result;
    unsigned char status = 1;
    unsigned char Nandbuf[512];

    if (!(access("/mnt/usb/cardlan.bin", 0)))
    {
        USBParaFile = fopen("/mnt/usb/cardlan.bin","rb+");
        memset (Nandbuf,0,sizeof(Nandbuf));
        result = fseek(USBParaFile, 0, SEEK_SET);
        result = fread(Nandbuf,sizeof(unsigned char),512,USBParaFile);
        fclose(USBParaFile);
        if((Nandbuf[112] == 0x55)&&(Nandbuf[128] != 0)&&(Nandbuf[129] != 0)&&(Nandbuf[130] != 0))
        {
            memcpy(LanSec.ADFNUM,Nandbuf+114,2);
            memcpy(&LanSec,Nandbuf+128,8);
            ReadOrWriteFile(MUSERSETOR);
            status = 0;
        }
    }

    return  status;
}

/*
*************************************************************************************************************
- �������� : static unsigned char  UsbSetDev(void)
- ����˵�� : USBȡ��������
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
static unsigned char  UsbSetDev(void)
{
    int result;
    unsigned char status = 1;
    unsigned char Nandbuf[512];
    LongUnon Buf;

    if (!(access("/mnt/usb/cardlan.bin", 0)))
    {
        USBParaFile = fopen("/mnt/usb/cardlan.bin","rb+");
        memset (Nandbuf,0,sizeof(Nandbuf));
        result = fseek(USBParaFile, 0, SEEK_SET);
        result = fread(Nandbuf,sizeof(unsigned char),512,USBParaFile);
        fclose(USBParaFile);
        if(Nandbuf[144] == 0x55)
        {
            memcpy(Buf.longbuf,Nandbuf+160,sizeof(LongUnon));
            //if((Buf.i != 0)/*&&(Buf.i < 100000000)*/) // BS ���ն˺�Ϊ10 λ����
            {
                memcpy(DevNum.longbuf,Nandbuf+160,sizeof(LongUnon));
                ReadOrWriteFile(MTEMNO);
                status = 0;
            }
        }
    }
    return  status;
}

/*
*************************************************************************************************************
- �������� : static unsigned char  UsbSetDev(void)
- ����˵�� : USBȡ��������
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
static unsigned char  UsbSection(void)
{
    FILE *Filebuf;
    int result;
    unsigned char status = 1,i;
    unsigned char Nandbuf[512];
    LongUnon Buf;

    if (!(access("/mnt/usb/cardlan.bin", 0)))
    {
        USBParaFile = fopen("/mnt/usb/cardlan.bin","rb+");
        memset (Nandbuf,0,sizeof(Nandbuf));
        result = fseek(USBParaFile, 0, SEEK_SET);
        result = fread(Nandbuf,sizeof(unsigned char),512,USBParaFile);

        if(Nandbuf[304] == 0x55)
        {
            memcpy(Buf.longbuf,Nandbuf+305,sizeof(LongUnon));
            if((Buf.i != 0)&&(Buf.i <= 0x10000))
            {
                memcpy(&Section.SationNum,Nandbuf+309,2);
                ReadOrWriteFile(SETSECTION);
                if(Section.Enableup != 0x55)
                {
                    memcpy(&Sectionup.SationNum,Nandbuf+309,2);
                    ReadOrWriteFile (SETSECTIONUP);
                }
                #ifdef SAVE_CONSUM_DATA_DIRECT
                Filebuf = fopen("/mnt/record/section.sys","rb+");
                #else
                Filebuf = fopen("/var/run/section.sys","rb+");
                #endif
                for(i=0; i<128; i++)
                {
                    memset (Nandbuf,0,sizeof(Nandbuf));
                    result = fseek(USBParaFile,Buf.i+i*512, SEEK_SET);
                    result = fread(Nandbuf,sizeof(unsigned char),512,USBParaFile);

                    result = fseek(Filebuf,i*512, SEEK_SET);
                    result = fwrite(Nandbuf,sizeof(unsigned char),512,Filebuf);
                }
                fclose(Filebuf);
                CardLanFile(SectionPar);
                status = 0;
            }

        }
        fclose(USBParaFile);
    }
    return  status;
}

/*
*************************************************************************************************************
- �������� : static unsigned char  UsbSetDev(void)
- ����˵�� : USBȡ��������
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
static unsigned char  UsbSectionUp(void)
{
    FILE *Filebuf;
    int result;
    unsigned char status = 1,i;
    unsigned char Nandbuf[512];
    LongUnon Buf;

    if (!(access("/mnt/usb/cardlan.bin", 0)))
    {
        USBParaFile = fopen("/mnt/usb/cardlan.bin","rb+");
        memset (Nandbuf,0,sizeof(Nandbuf));
        result = fseek(USBParaFile, 0, SEEK_SET);
        result = fread(Nandbuf,sizeof(unsigned char),512,USBParaFile);

        if(Nandbuf[320] == 0x55)
        {
            memcpy(Buf.longbuf,Nandbuf+321,sizeof(LongUnon));
            if((Buf.i != 0)&&(Buf.i <= 0x10000))
            {
                memcpy(&Sectionup.SationNum,Nandbuf+325,6);
                ReadOrWriteFile(SETSECTIONUP);
                #ifdef SAVE_CONSUM_DATA_DIRECT
                Filebuf = fopen("/mnt/record/sectionup.sys","rb+");
                #else
                 Filebuf = fopen("/var/run/sectionup.sys","rb+");
                #endif
                for(i=0; i<128; i++)
                {
                    memset (Nandbuf,0,sizeof(Nandbuf));
                    result = fseek(USBParaFile,Buf.i+i*512, SEEK_SET);
                    result = fread(Nandbuf,sizeof(unsigned char),512,USBParaFile);

                    result = fseek(Filebuf,i*512, SEEK_SET);
                    result = fwrite(Nandbuf,sizeof(unsigned char),512,Filebuf);
                }
                fclose(Filebuf);
                CardLanFile(SectionParup);
                status = 0;
            }

        }
        fclose(USBParaFile);
    }
    return  status;
}


int WEISHENG_UpdateGBK()
{
	system("cp /mnt/usb/GBK32c_ASC.DZK  /var/run/font/");
}

#if Yantai_Qixia
void deletline(void)
{
	FILE* temfile;
	struct stat file_info;
	char buffer[32],sysbuff[32];
	int len,num,i;		

	printf("����ɾ����·�ļ�\n");
	system("ls /mnt/record/ | grep \"cardlan[0-9]\" > line.txt");
	stat("line.txt" , &file_info);
	system("cat line.txt");
	if(file_info.st_size!=0){
		TextOut(30,100,"����ɾ����·�ļ�");
		len=sizeof("cardlan000.bin");
		num=file_info.st_size/len;
		temfile=fopen("line.txt","r");
		for(i=0;i<num;i++)
		{
			memset(buffer,0,32);
			memset(sysbuff,0,32);
			fseek(temfile,i*len,SEEK_SET);
			fread(buffer,len,1,temfile);
			strcat(sysbuff,"rm /mnt/record/");
			strcat(sysbuff,buffer);
			system(sysbuff);
			system("sync");
			strcat(buffer,"ɾ���ɹ�");
			TextOut(30,100,buffer);
			sleep(1);
		}
		fclose(temfile);	
	}
	system("rm line.txt");
	system("sync");
}

#endif
/*
*************************************************************************************************************
- �������� : int USB_Updata(void)
- ����˵�� : USBȡ��������
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/

int check_usb_update(int status,char *out1,char *out2)
{
#ifdef NEW0409
    DebugPrintf("status=%d\n",status);
	if(status==-1){
				SetColor(Mcolor);
				SetTextSize(48);
				SetTextColor(Color_white);
				TextOut(0,100,out2);
			}
	else{
		if(WIFEXITED(status)){
			if(0 == WEXITSTATUS(status)){
				SetColor(Mcolor);
				SetTextSize(48);
				SetTextColor(Color_white);
                TextOut(0,100,out1);
           	}
			else{
					SetColor(Mcolor);
					SetTextSize(48);
					SetTextColor(Color_white);
                  	TextOut(0,100,out2);
           		}
		}
		else{
			SetColor(Mcolor);
			SetTextSize(48);
			SetTextColor(Color_white);
			TextOut(0,100,out2);
			}
	}
#else
	DebugPrintf("status=%d\n",status);
	if(status==-1){
				SetColor(Mcolor);
				SetTextSize(32);
				SetTextColor(Color_white);
				TextOut(45,100,out2);
			}
	else{
		if(WIFEXITED(status)){
			if(0 == WEXITSTATUS(status)){
				SetColor(Mcolor);
				SetTextSize(32);
				SetTextColor(Color_white);
                TextOut(45,100,out1);
           	}
			else{
					SetColor(Mcolor);
					SetTextSize(32);
					SetTextColor(Color_white);
                  	TextOut(45,100,out2);
           		}
		}
		else{
			SetColor(Mcolor);
			SetTextSize(32);
			SetTextColor(Color_white);
			TextOut(45,100,out2);
			}
	}
#endif    
    beepopen(1);
	sleep(2);
	return 0;
}

int USB_Updata(void)
{
    unsigned char *pr;
    unsigned char status;
	int ret;
    unsigned char Loop =1,Task = 1;


    if(access("/mnt/usb",0)== 0)
    {
#if USBPRINT
        printf("usb start  \n");
#endif
        //system("mkdir /mnt/usb ");
        //system("mount /dev/uba1  /mnt/usb ");
        //  system("cp /mnt/usb/*.ko  /driver/");
        // system("cp /mnt/usb/*.in  /driver/");

		printf("mount /dev/sdb1  /mnt/usb\n");
		system("mount /dev/sdb1  /mnt/usb");
		//sleep(1);
		

        ret=system("cp /mnt/usb/*.sh  /var/run/");
		check_usb_update(ret,"���½ű��ɹ�","���½ű�ʧ��");
        if(access("/var/run/appstart.sh",0)== 0)
        {
			//TextOut(45,100,"���������ű��ɹ�\n");
            status=system("chmod 775 appstart.sh");	
        }

        ret=system("cp /mnt/usb/*.ko  /mnt/nand1-1/drivers/");
		check_usb_update(ret,"���������ɹ�","��������ʧ��");
		
        ret=system("cp /mnt/usb/*.bmp  /var/run/");
		check_usb_update(ret,"����ͼƬ�ɹ�","����ͼƬʧ��");
		// added by taeguk
		/* desktop display */
		system("cp /mnt/usb/dis.txt  /var/run/");
		ret=system("cp /mnt/usb/libcrypto.so.1.0.0 /var/run/");
		check_usb_update(ret,"���¿��ļ��ɹ�","���¿��ļ�ʧ��");
		ret=system("cp /mnt/usb/network.sh  /mnt/nand1-1/wifi/");
		check_usb_update(ret,"��������ű��ɹ�","��������ű�ʧ��");
		ret=system("cp /mnt/usb/appstart.sh /var/run/");
		check_usb_update(ret,"���������ű��ɹ�","���������ű�ʧ��");
		ret=system("cp /mnt/usb/cardlan*.bin /mnt/record/");
	
        ret=system("cp /mnt/usb/JWhitelist.sys /mnt/record/");
		check_usb_update(ret,"���°������ɹ�","���°�����ʧ��");
        ret=system("cp /mnt/usb/Blacklist.sys /mnt/record/");
		check_usb_update(ret,"����IC���������ɹ�","����IC��������ʧ��");


        
        if(access("/mnt/usb/repair.sh",0) == 0)
        {
        	if (g_FgFileOccurError)
        	{
	        	SetColor(Mcolor);
			SetTextSize(32);
			SetTextColor(Color_white);
			TextOut(100,50, "��ܰ��ʾ");
			TextOut(45,100,"�����޸�����");
			TextOut(110,150,"......");
	        system("cp /mnt/usb/repair.sh  /var/run/");
			system("chmod u+x /var/run/repair.sh");
			system("sync");
			sleep(2);
			system("sh /var/run/repair.sh");
			sleep(4);
        	}
	 }
	
    if(access("/var/run/CARDLAN_LOG.dat",0) == 0)
    {
        system("cp /var/run/CARDLAN_LOG.dat /mnt/usb/");
        system("sync;");
        }


		system("cp /mnt/usb/music*.wav  /var/run/sound/");


        memset(&USBFile,0,sizeof(Operat));

	if(access("/mnt/usb/mercode.bin",0)== 0)
		   {
				   system("cp /mnt/usb/mercode.bin /mnt/record/");
				   //check_usb_update(ret,"�����̻��ųɹ�","�����̻���ʧ��");
				   system("sync");
                   ret = 0;
				   check_usb_update(ret,"�����̻��ųɹ�","�����̻���ʧ��");
				   sleep(2);   
				   ReadMERCHANTNO();//��ȡ�̻����ļ� 
		   }
        

#if Yantai_Qixia

		FILE* temfile;
		struct stat file_info;
		char buffer[32];
		int len,num,i;
		int ret;
		system("ls /mnt/usb/ | grep \"cardlan[0-9]\" > txt.txt");
		stat("txt.txt" , &file_info);
		system("cat txt.txt");

		if(file_info.st_size!=0){
					TextOut(30,100,"����������·�ļ�");
					system("cp /mnt/usb/cardlan[0-9]*.bin /mnt/record/");
					system("sync");
					
					printf("�ļ���С%d\n",file_info.st_size);
					len=sizeof("cardlan000.bin");	
					printf("len = %d\n",len);
					num=file_info.st_size/len;
					printf("num = %d\n",num);
					temfile=fopen("txt.txt","r");
					for(i=0;i<num;i++)
					{
						memset(buffer,0,32);
						fseek(temfile,i*len,SEEK_SET);
						fread(buffer,len,1,temfile);
						printf("buffer =%s ",buffer);
						if((ret=mystrncmp(buffer,"cardlan999.bin",len-1))==0){				//ɾ����·�ļ�����
							deletline();
							break;
						}
						printf("ret =%d\n",ret);	
						strcat(buffer,"���سɹ�");
						TextOut(30,100,buffer);
						sleep(1);
					}
					fclose(temfile);
				}
				else
					TextOut(30,100,"����������·�ļ�ʧ��");
				
				system("rm txt.txt");
				system("sync");
#endif

        Task = 2;
        while(Loop)
        {
            switch(Task)
            {
            case 1:
                //    sleep(1);
                Task ++;
                //    break;
            case 2:							//��ȡcardlan.bin
#if USBPRINT
                DebugPrintf("CheckCardlanBin start\n");

#endif
                status = UsbCheckCardlanBin();
                if(status == 0)
                {
#if USBPRINT
                    printf("USBFile.Parameters = %d\r\n",USBFile.Parameters);  	//��������
                    printf("USBFile.Blacklist = %d\r\n",USBFile.Blacklist);  	//���º�����
                    printf("USBFile.PrintfStart = %d\r\n",USBFile.PrintfStart); //��ӡͷ
                    printf("USBFile.PrintfEnd = %d\r\n",USBFile.PrintfEnd);		//��ӡβ
                    printf("USBFile.Collection = %d\r\n",USBFile.Collection);	//�ɼ�����
                    printf("USBFile.Sound = %d\r\n",USBFile.Sound);				// ��������
                    printf("USBFile.Upgrade = %d\r\n",USBFile.Upgrade);			//���³���
                    printf("USBFile.SetSector = %d\r\n",USBFile.SetSector);		// ��������
                    printf("USBFile.SetDev = %d\r\n",USBFile.SetDev);			//���û���
                    printf("usbfile.section=%d\r\n",USBFile.Section);
					printf("usbfile.section=%d\r\n",USBFile.Sectionup);
#endif
					SetColor(Mcolor);
                #ifdef NEW0409
                    SetTextSize(48);
					SetTextColor(Color_white);
					TextOut(0,50, "��ܰ��ʾ");
					TextOut(0,144,"�������������ļ�");
					TextOut(0,210,"���Ժ�");
                #else
					SetTextSize(32);
					SetTextColor(Color_white);
					TextOut(100,50, "��ܰ��ʾ");
					TextOut(30,100,"�������������ļ�");
					TextOut(110,150,"���Ժ�");
                #endif
                    pr = (unsigned char *)(&USBFile);
                    while(Task < TASKTATOL)
                    {
                        Task ++;
                        if(*pr == 0)
                        {
                            pr ++;
                        }
                        else break;
                    }
                }
                else
                {
                    Loop = 0;
                }
                break;


            case 3:	//�Ƿ��в�������
#if USBPRINT
                printf("----------------------------->��������\n");
#endif
                if(UsbDownloadPara() == 0)
                {
                    Read_Parameter();
                    *pr = 0xaa;
                }
                pr++;
                while(Task < TASKTATOL)
                {
                    Task ++;
                    if(*pr != 1)
                    {
                        pr ++;
                    }
                    else break;
                }
                break;


            case 4:	//�Ƿ������
#if USBPRINT
                printf("----------------------------->������\n");
#endif
                status = UsbBlacklist();
                if(status == 0)
                {
                    *pr = 0xaa;
                }
                pr++;
                while(Task < TASKTATOL)
                {
                    Task ++;
                    if(*pr != 1)
                    {
                        pr ++;
                    }
                    else break;
                }
                break;



            case 5:	//�Ƿ��ӡͷ����
#if USBPRINT
                printf("----------------------------->��ӡͷ����\n");
#endif
                status = UsbDownloadPrintfStart();
                if(status == 0)
                {
                    *pr = 0xaa;

                }
                pr++;
                while(Task < TASKTATOL)
                {
                    Task ++;
                    if(*pr != 1)
                    {
                        pr ++;
                    }
                    else break;
                }
                break;
            case 6:							//�Ƿ��ӡβ����
#if USBPRINT
                printf("----------------------------->��ӡβ����\n");
#endif
                status = UsbDownloadPrintfEnd();
                if(status == 0)
                {
                    *pr = 0xaa;

                }
                pr++;
                while(Task < TASKTATOL)
                {
                    Task ++;
                    if(*pr != 1)
                    {
                        pr ++;
                    }
                    else break;
                }
                break;



            case 7:							//�Ƿ�ɼ�����
#if USBPRINT
                printf("----------------------------->�ɼ�����\n");
#endif

#if MULTI_FILE_STORAGE
				status = usb_collect_all(0);
#else
                status = UsbUploadSdData();
#endif

                if(status == 0)
                {
                    *pr = 0xaa;

                }
                pr++;
                while(Task < TASKTATOL)
                {
                    Task ++;
                    if(*pr != 1)
                    {
                        pr ++;
                    }
                    else break;
                }
                break;



            case 8:
#if USBPRINT
                printf("----------------------------->��������\n");
#endif
                status = UsbUpdataSound();
                if(status == 0)
                {
                    *pr = 0xaa;

                }
                pr++;
                while(Task < TASKTATOL)
                {
                    Task ++;
                    if(*pr != 1)
                    {
                        pr ++;
                    }
                    else break;
                }
                break;

            case 9:	        //��ʽ������
#if USBPRINT
                printf("��ʽ������    \n");

#endif
                status = RMAllRecord(0);
                if(status == 0)
                {
                    *pr = 0xaa;

                }
                pr++;
                while(Task < TASKTATOL)
                {
                    Task ++;
                    if(*pr != 1)
                    {
                        pr ++;
                    }
                    else break;
                }
                break;


            case 10:			//�Ƿ������ļ�
#if USBPRINT
                printf("�����ļ� start\n");

#endif
                status = UsbUpdateSdProgram();
                if(status == 0)
                {
                    *pr = 0xaa;

                }
                pr++;
                while(Task < TASKTATOL)
                {
                    Task ++;
                    if(*pr != 1)
                    {
                        pr ++;
                    }
                    else break;
                }
                break;

            case 11:			//�Ƿ������ļ�
#if USBPRINT
                printf("��������\n");

#endif
                status = UsbSetSector();
                if(status == 0)
                {
                    *pr = 0xaa;

                }
                pr++;
                while(Task < TASKTATOL)
                {
                    Task ++;
                    if(*pr != 1)
                    {
                        pr ++;
                    }
                    else break;
                }
                break;

            case 12:			//�Ƿ������ļ�
#if USBPRINT
                printf("���û���\n");

#endif
                status = UsbSetDev();
                if(status == 0)
                {
                    *pr = 0xaa;

                }
                pr++;
                while(Task < TASKTATOL)
                {
                    Task ++;
                    if(*pr != 1)
                    {
                        pr ++;
                    }
                    else break;
                }
                break;

            case 13:			 //�Ƿ������ļ�
#if USBPRINT
                printf("�ֶ��շ�\n");

#endif
                status = UsbSection();		//����
                if(status == 0)
                {
                	printf("�������гɹ�\r\n");
                    *pr = 0xaa;

                }
                pr++;
                while(Task < TASKTATOL)
                {
                    Task ++;
                    if(*pr != 1)
                    {
                        pr ++;
                    }
                    else break;
                }
                break;


            case 14:			 //�Ƿ������ļ�
#if USBPRINT
                printf("�ֶ��շ�\n");

#endif
                status = UsbSectionUp();	//����
                if(status == 0)
                {
                	printf("�������гɹ�\r\n");
                    *pr = 0xaa;

                }
                pr++;
                while(Task < TASKTATOL)
                {
                    Task ++;
                    if(*pr != 1)
                    {
                        pr ++;
                    }
                    else break;
                }
                break;

            default:
                Task = 0;
                Loop = 0;
                break;
            }
        }
//		system("sync");
        //system("umount /mnt/usb ");
    }


	system("sync");




#if USBPRINT
    printf("USB  status = %d  \n",status);
#endif

    return Task;
}





