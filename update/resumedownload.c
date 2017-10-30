#if 0
#include "apparel.h"
#include "Resumedownload.h"
#if (L9==0)
#include "../gui/SendDataNet.h"
#endif
#include "../gui/InitSystem.h"
#if L9
#include "../Display/fbtools.h"
#endif
/****ʹ�õ��ı���*******/
unsigned char Netstatus;
struct Upfile FileInfo;                   //�ļ���Ϣ����
FILE *Upfilecfg;                          //�����ļ����ļ�ָ��
FILE *Upfiletmp;                          //��ʱ�ļ����ļ�ָ��
pthread_t upfile;


extern 	char Eflag;

typedef union
{
              unsigned char longbuf[4];
              unsigned int  i;
} Lfileunion;

 Lfileunion Update;
 Lfileunion Update2;
 
 Lfileunion Update_pthread; 
typedef union
{
              unsigned char intbuf[2];
              unsigned short i;
} Sfileunion;

Sfileunion Update1;
Sfileunion Update3;  


/****���������ļ��ı���****/
extern  BMP_INFO bmppicture1;
//extern unsigned char ConnectFlag;			
extern int sockfd;
extern pthread_mutex_t m_socketwrite; 
#if (L9==0)
    extern volatile unsigned char GPRSSendFlag; 
    extern unsigned char ProgramUpdateFlag;//�����ļ���ʶ
    extern unsigned int upfilecount_flag ;//�����ļ�����ֵ
#else
    extern PIXEL _pix[76800];
#endif



/****************************************
- �������� : int Convert_IntTOBcd(int num,unsigned char *OutBuf)
- ����˵�� : ���ݸ�ʽת��
- �������� : ��ʮ������ת����BCD��
- ������� : 
            num:��ת����10������
            *OutBuf:ת�����BCD��
- ������� : ��
****************************************/

int Convert_IntTOBcd(int num,unsigned char *OutBuf)
{
	int i = 0, j = 0;
	unsigned char NumBuf[8];

	memset(NumBuf, 0, sizeof(NumBuf));
	sprintf(NumBuf, "%04d", num);

	for(i=0; i<4; i+=2)
	{
		OutBuf[j] = (NumBuf[i] - '0') << 4 | (NumBuf[i+1] - '0');
		j++;
	}

	return 0;
}


/****************************************
- �������� : void InitEnv()
- ����˵�� : ��ʼ���ϵ���������
- �������� : ��ȡ�Ѵ����ڻ����е����أ�����ļ��������򴴽�һ��
- ������� : ��
- ������� : ��
****************************************/
void InitEnv()
{
    memset(&FileInfo,0,sizeof(struct Upfile));
    Netstatus = 0;
    pthread_create(&upfile, NULL, DownloadFile_pthread,"AVBC"); 
    if(access(PROCESSFILE,0)==0)
    {
        Upfilecfg = fopen(PROCESSFILE,"rb+");
         if(Upfilecfg)
            {
                printf("�����������ļ��ɹ�!\n");   
                fseek(Upfilecfg,0,SEEK_SET);
                fread(&FileInfo.filenamelen,sizeof(unsigned char),sizeof(struct Upfile),Upfilecfg);
                fclose(Upfilecfg);                
            }
            else
            {
                printf("�������ļ�ʧ��\n");               
            }        
        }
    else
        {
            Upfilecfg = fopen(PROCESSFILE,"a+");
         if(Upfilecfg)
            {
                printf("�������������ļ��ɹ�!\n");                
                fclose(Upfilecfg);                
            }
            else
            {
                printf("���������ļ�ʧ��\n");               
            } 
#if L9
            system("sync");
#endif
        }
    }


/****************************************
- �������� : int DownloadProcess(unsigned char *data);
- ����˵�� : �����������ļ���Ϣ
- �������� : ���շ��������ع����ģ�������Ҫ�����ļ�����Ϣ(�ļ�������С��У��ֵ)
- �������� : �������
             0:����ɹ�
             -1:����ʧ��
- ������� : 
            *data--�ļ������Ϣ
            len:�ļ�������
- ������� : ��
****************************************/
 int DownloadProcess(unsigned char *data,unsigned char len)
{

    struct stat file_info;
    int file_size;
	char offset;   
	char *strs= NULL;        

    if(access(TEMPFILE,0)==0)  
    {
        stat(TEMPFILE,&file_info);
        Update.i = 0;
        memcpy(Update.longbuf,&FileInfo.upfilelen,4); 
        if(file_info.st_size == Update.i)  
            return 0;
        else
            return -1;
        }
    else
     {        
        memset(&FileInfo,0,sizeof(struct Upfile));        
        FileInfo.filenamelen = *data;
		offset = *data;  
		memcpy(&FileInfo.upfilename[0],data+1,offset);   
		memcpy(&FileInfo.upfilelen,&data[1+offset],len-1-offset );        
        Upfilecfg = fopen(PROCESSFILE,"rb+");           
        if(Upfilecfg<0)   
        {
            return -1;   
            }
        else
        {
            
           fseek(Upfilecfg,0,SEEK_SET);       
           fwrite(&FileInfo.filenamelen,sizeof(unsigned char),sizeof(struct Upfile),Upfilecfg);
           fclose(Upfilecfg);
#if UPDEG
           printf("FileInfo.len=%02x\n",FileInfo.filenamelen);
		   strs = FileInfo.upfilename;  
		   printf("FileInfo.filename=%s\n",strs);     
           Update.i = 0;
           memcpy(Update.longbuf,&FileInfo.upfilelen,4);
           printf("FileInfo.upfilelen=%d\n",Update.i);       
           Update1.i = 0;
           memcpy(Update1.intbuf,&FileInfo.crc,2);
           printf("FileInfo.crc=%d\n",Update1.i);
           Update1.i = 0;
           memcpy(Update.longbuf,&FileInfo.offset,2);
           printf("FileInfo.offset=%d\n",Update.i);       
         

#endif
#if L9
        system("sync");
#endif
            return 1;
            }
            
           }
        return -1;
    }

/****************************************
- �������� : int StopProcess()
- ����˵�� : ֹͣ����
- �������� : �������·�ֹͣ�ļ���������
- �������� : �������
             0:����ɹ�
             -1:����ʧ��
- ������� : ��
- ������� : ��
****************************************/
 int  StopProcess()
{
    memset(&FileInfo.filenamelen,0,sizeof(struct Upfile));
    if(access(PROCESSFILE,0)==0)
   {
        Upfilecfg = fopen(PROCESSFILE,"rb+");
        if(Upfilecfg==NULL)
        {
        printf("123\n");
            return -1;
            }
        else
        {
           fseek(Upfilecfg,0,SEEK_SET);   
           fwrite(&FileInfo.filenamelen,sizeof(unsigned char),sizeof(struct Upfile),Upfilecfg);
           fclose(Upfilecfg);
            } 
		//system("rm /opt/minigui/app/tempupfile.bin");  
        system("rm "TEMPFILE);  
#if L9
        system("sync");
#endif
        }
    else
        return -1;
    return 0;
    }


/****************************************
- �������� : int ReqProcess(unsigned char *reson,unsigned char *filename,unsigned char filenamelen,unsigned short *downloadper)
- ����˵�� : ���̲�ѯ
- �������� : �������·���ѯ��ǰ�������ļ�����Ϣ
- �������� : ��ѯ����״̬
             0:�ļ��������
             1:�ļ�����ʧ��
             2:�ļ�������
             
- ������� : 
             *filename:��ѯ���ص��ļ���
             filenamelen:��ѯ�ļ����ĳ���
- ������� : 
             *reson:����ʧ��ԭ��
             *downloadper:�����صİٷֱ�
******************************************/
int ReqProcess(unsigned char *reson,unsigned char *filename,unsigned char filenamelen,unsigned short *downloadper)
{
    struct stat file_info;
    int file_size;
    int ret = 0;
    unsigned short percent;
    int status;
    unsigned short crc16;
    char *filep=NULL;
    if((memcmp(filename,FileInfo.upfilename,filenamelen)==0)&&(access(TEMPFILE,0)==0))
    {
        ret=stat(TEMPFILE,&file_info);
        if(ret == -1)
    	{
    		
            *reson = 1;             
    		return 1;
    	}
        Update.i = 0;
        memcpy(Update.longbuf,&FileInfo.upfilelen,4);
        percent = file_info.st_size*100/Update.i; 
        //percent += (file_info.st_size%Update.i)*100;  
        if(percent == 100)  
        {
        	//printf("%d,�ļ��ٷֱ�\n",__LINE__); 
            file_size = file_info.st_size;
            filep = (char *)malloc(file_size);
            memset(filep,0,file_size);
            Upfiletmp = fopen(TEMPFILE,"a+");
            fread(filep,sizeof(unsigned char),file_size,Upfiletmp);
            fclose(Upfiletmp);
            Calc_crc(filep,file_size,&crc16,0);
            free(filep);
            Update1.i = 0;
            memcpy(Update1.intbuf,&FileInfo.crc,2);
			Update3.intbuf[0] = Update1.intbuf[1];
			Update3.intbuf[1] = Update1.intbuf[0];      
			
			//printf("crc16 =%02x,Update3.i =%02x\n",crc16,Update3.i);     
            if(crc16 == Update3.i)                       
                return 0;
            else
            {
                *reson = 2;
                return 1;
                }
            }
        else
        { 
            *downloadper = percent;
            return 2;
            }
    
       }
    else
    {
        *reson = 1;
        return 1;
        }
    }


/****************************************
- �������� : int ExeProcess()
- ����˵�� : �ļ�ִ��
- �������� : �������·�ִ���ļ�������
- �������� : �������
             0:����ɹ�
             -1:����ʧ��
- ������� : ��
- ������� : ��
******************************************/

int ExeProcess()
{
    unsigned char *pTmp=NULL,*pTmp_=NULL;
    struct stat file_info;
    int file_size;
    int ret;
    char save_path[50];
    char dir_path[50];
    char *strs;   
	
    ret=stat(TEMPFILE,&file_info);
        if(ret == -1)
    	{                   
    		return -1;
    	}
        file_size = file_info.st_size;
        Update.i = FileInfo.upfilelen;        
    if((access(TEMPFILE,0)==0)&&(file_size == Update.i))     
	{
			pTmp = (unsigned char*)malloc(50);	
			pTmp_ = (unsigned char*)malloc(300);   

			memset(pTmp,0,50);	
			memset(pTmp_,0,300);			
	        memset(save_path,0,sizeof(save_path));	
			FileInfo.upfilename[FileInfo.filenamelen+1]='\0'; 
            memset(dir_path,0,50);
			//��ȡ�����ļ��е����ر���·��
			memcpy(dir_path,FileInfo.upfilename,FileInfo.filenamelen);
			//strcpy(pTmp + strlen(pTmp), dirname(FileInfo.upfilename));
			strcpy(pTmp + strlen(pTmp), dirname(dir_path));
			printf("\n last dir : %s \n",pTmp);
            strs = FileInfo.upfilename;  
		    printf("FileInfo.filename=%s\n",strs); 
	        sprintf(save_path,"%s",FileInfo.upfilename);				
			//printf("last cmd : %s \n",pTmp);//pTmp :/opt/minigui/app
			//�ļ�ת�棬����ʱ�ļ�ת��Ϊ��Ҫ���ص��ļ�
			memset(pTmp_,0,300);
			sprintf(pTmp_,"mv "TEMPFILE" %s",FileInfo.upfilename);//pTmp_ : mv /opt/minigui/app/update.bin /opt/minigui/app/desktop.tar.gz 
			printf("\n last name : %s \n",pTmp_);
			system(pTmp_);

	        //����ԭ���ĳ����ļ�
	        if(access("/opt/minigui/app/desktop",0)==0)
	           system("cp desktop desktop_bak");
	        if(access("/var/run/armok",0)==0)  
	           system("cp armok armok_bak");
	        
	        //�ļ���ѹ
			memset(pTmp_,0, 300);
			sprintf(pTmp_, "tar -zxvf %s -C %s", save_path, pTmp);
            printf("\n last name : %s \n",pTmp_);
			system(pTmp_);
			
			//ת��ͼƬ�ļ���ָ��Ŀ¼
		#if 0	
			memset(pTmp_,0,300);
			sprintf(pTmp_,"mv %s/*.png /opt/res",pTmp);	//pTmp :/opt/minigui/app				
			system(pTmp_);
		#endif
#if L9
            if(access("back.bmp",0)==0)
            {
                //load_BMP("/var/run/back.bmp", &_pix);
                printf("--------------------����bmpͼƬ�ļ���׼������---------------------\n");
			#if 0	// ������Ч
                free(bmppicture1.data);
				bmppicture1.data=NULL;
                if(Load_Bmp_Img("/var/run/back.bmp",&bmppicture1))
                	{
                		printf("����ͼƬʧ��------------\n");
							Eflag = 0;
                	}
				else{
						system("cp back.bmp ./res");
							Eflag = 1;
					}		
			#else	//�ؿ�������Ч
					system("cp back.bmp ./res");
			
			#endif
				
               }
			else{
				  printf("--------------------δ����bmpͼƬ�ļ�---------------------\n");
				}


			//�κ�ͼƬֻҪ��ִ��·���¶�����������ȷ
			
            system("mv *.wav /var/run/sound/");
#endif            

			//���ִ��֮�������ļ�������Ϊ�����̵߳��ж�����
			memset(&FileInfo.filenamelen,0,sizeof(struct Upfile));	 		
			if(access(PROCESSFILE,0)==0)
			  {
				   Upfilecfg = fopen(PROCESSFILE,"rb+");
				   if(Upfilecfg==NULL)
				   {
					   return -1;
					   }
				   else
				   {
					  fseek(Upfilecfg,0,SEEK_SET);	 
					  fwrite(&FileInfo.filenamelen,sizeof(unsigned char),sizeof(struct Upfile),Upfilecfg);
					  fclose(Upfilecfg); 
					   }
			   }
			 
			memset(pTmp_,0,300);
			sprintf(pTmp_,"rm %s",save_path);					
			system(pTmp_);  

			
			free(pTmp);
			free(pTmp_);    

			
#if L9
	        system("sync");  
#endif
	}
    else
    {
        return -1;
        }
    return 0;
    }

/****************************************
- �������� : int RecoveryProcess()
- ����˵�� : �ָ�ԭ�ļ�
- �������� : �������·��ָ���һ���汾��������
- �������� : �������
             0:����ɹ�
             -1:����ʧ��
- ������� : ��
- ������� : ��
******************************************/
int RecoverProcess()
{    
    if(access("/opt/minigui/app/desktop_bak",0)==0)
       system("cp desktop_bak desktop");
    if(access("/var/run/armok_bak",0)==0)
       system("mv armok_bak armok.new");
    return 0;
 }


/****************************************
- �������� : int write_datas_gprs_upfile(int fd, unsigned char *buffer, int buf_len)
- ����˵�� : �����������
- �������� : ͨ��SOKET�׽��ַ�����������
- �������� : �������
             0:����ɹ�
             -1:����ʧ��
- ������� : 
            fd:����ʹ�õ��׽���
            *buffer:�����͵�����
            buf_len:�������ݵĳ���
- ������� : ��
******************************************/
 int write_datas_gprs_upfile(int fd, unsigned char *buffer, int buf_len)
{
    int status;
    pthread_mutex_lock(&m_socketwrite); //�߳�����
    status = write_datas_tty(fd,buffer,buf_len);
    pthread_mutex_unlock(&m_socketwrite);//�߳̽���
    return status;
}



/****************************************
- �������� : static char GprsCmd_Upfile(unsigned char *SndDa)()
- ����˵�� : ��������
- �������� : ͨ��SOCET�������˷�������
- �������� : �������
             0:����ɹ�
             -1:����ʧ��
- ������� : 
            *SndDa:Ҫ���͵�����
- ������� : ��
******************************************/
static char GprsCmd_Upfile(unsigned char *SndDa)
{
    char status;
    unsigned char sendOut[512];
    unsigned char CmdDate[300];

    memset(CmdDate,0,sizeof(CmdDate)); 
    hex_2_ascii(SndDa,CmdDate,SndDa[2] + 4);  
    memset(sendOut,0,sizeof(sendOut)); 
    memcpy(sendOut,FILEHEAD,4);
    memcpy(sendOut+4,"AABBCCDD",8);
    strcat(CmdDate,":0D"); 
    strcat(sendOut,CmdDate); 

#if GPRSPR
    printf("GprsCmd  Send :%s\n",sendOut);
#endif

#if L9
    status = write_datas_gprs_upfile(sockfd,sendOut,strlen(sendOut));    
#else
    while(GPRSSendFlag);
    GPRSSendFlag = 1;
	status = write_datas_gprs_upfile(sockfd,sendOut,strlen(sendOut));

    GPRSSendFlag = 0;
#endif
    if(status != 0)
    {
        //ConnectFlag  = 1;
        
    }

#if GPRSPR
    printf("GprsCmd status = %d\n",status);
#endif
    return status;
}


/****************************************
- �������� : static char  SendCmdPc_Upfile(unsigned char *Date,unsigned short LLen) 
- ����˵�� : �����������
- �������� : ��Ҫ���͵����ݰ�����ظ�ʽ�������
- �������� : �������
             0:����ɹ�
             -1:����ʧ��
- ������� : 
            *Date:Ҫ���͵�����
            LLEN:�������ݵĳ���
- ������� : ��
******************************************/
static char  SendCmdPc_Upfile(unsigned char *Date,unsigned char LLen) 
{
    char status;
    unsigned char Scheme[135];
    unsigned char i,ch; 
	//printf("���͵����ݳ���LLen = %x",LLen);         	
    memset(Scheme,0,sizeof(Scheme));  
    Scheme[0] = 0x80;
    Scheme[1] = 0;
    Scheme[2] = LLen;
    ch =  Date[0];
    for(i = 1; i < LLen; i++)
    {
        ch ^= 	Date[i];
    }
    Scheme[3] = ch;
    memcpy(Scheme + 4,Date,LLen); 
   // Scheme[i + 4] = 0x0d; 
    status = GprsCmd_Upfile(Scheme);   

#if GPRSPR
    printf("SendCmdPc status = %d\n",status);
#endif
    return status;
}

/****************************************
- �������� : unsigned int LenDataCmd1(unsigned char *DataLen) 
- ����˵�� : ���㷢�����ݳ���
- �������� : ͨ����ʽ��������յ����ݵĳ���
- �������� : ���յ������ݳ���
- ������� : 
            *DateLen:���յ����ݵĳ���
            
- ������� : ��
******************************************/

unsigned int LenDataCmd1(unsigned char *DataLen)
{
    unsigned char Hbyte,Lbyte;
    Sfileunion filedata;
    filedata.i = 0;
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
    filedata.intbuf[1]= Hbyte<<4|Lbyte;
    
    if((DataLen[2] >= '0')&&(DataLen[2] <= '9'))
    {
        Hbyte = DataLen[2] - '0';
    }
    else if((DataLen[2] >= 'A')&&(DataLen[2] <= 'F'))
    {
        Hbyte = DataLen[2] - '7';
    }
    else if((DataLen[2] >= 'a')&&(DataLen[2] <= 'f'))
    {
        Hbyte = DataLen[2] - 0x57;
    }
    else
    {
        Hbyte = 0x00;
    }
    if((DataLen[3] >= '0')&&(DataLen[3] <= '9'))
    {
        Lbyte = DataLen[3] - '0';
    }
    else if((DataLen[3] >= 'A')&&(DataLen[3] <= 'F'))
    {
        Lbyte = DataLen[3] - '7';
    }
    else if((DataLen[3] >= 'a')&&(DataLen[3] <= 'f'))
    {
        Lbyte = DataLen[3] - 0x57;
    }
    else
    {
        Lbyte = 0x00;
    }
    filedata.intbuf[0]= Hbyte<<4|Lbyte;
    
    return filedata.i;

}

/****************************************
- �������� : unsigned char Judge_Rcv1(unsigned char *Packet,unsigned int Data_len)
- ����˵�� : У������
- �������� : У����յ����ݰ�����ȷ��
- �������� : 
            0:�ɹ�
            2:ʧ��
- ������� :
            *Packet:���յ�������
            Data_len:���յ����ݵĳ���            
- ������� : 
******************************************/
unsigned char Judge_Rcv1(unsigned char *Packet,unsigned int Data_len)
{
    unsigned char ch;
    unsigned int i;
    unsigned char status = 2;
    Sfileunion len;
    len.i = 0;
    len.intbuf[1] = Packet[1];
    len.intbuf[0] = Packet[2];
    
#if UPDEG
        printf("len.i = %d:Data_len=%d\n",len.i,Data_len);
        printf("CHANGCODEFIEL------------>");
        for(i=0;i<len.i+5;i++)
        {
            printf("0x%02x ",Packet[i]);
            }
        printf("\n");		//�յ�����������
#endif 
    if(Packet[0] == ST_RX)
    {
        if(len.i == Data_len)
        {
            if(Packet[Data_len + 4] == ST_END)
            {
                ch = Packet[4];
                for(i = 5; i < Data_len + 4; i++)
                {
                    ch^= Packet[i];
                }
                //printf("ch=%02x:Packet[3]=%02x\n",ch,Packet[3]);   
                if(ch == Packet[3]) status = MI_OK;
            }
        }
    }
    return status;
}



/****************************************
- �������� : void All_Cmd_Snd1(unsigned char *Data) 
- ����˵�� : �������
- �������� : �����յ������ݽ��н���
- �������� : ��
- ������� : 
            *DateLen:���յ����ݵĳ���            
- ������� : 
******************************************/

void All_Cmd_Snd1(unsigned char *Data)  
{
    unsigned char  status,i;
    unsigned char  RcvPc[132*2];
    int result;
    unsigned char reson;
	char *strs = NULL;

	Lfileunion BuffWord;
    Sfileunion len; 
    Sfileunion per;
    
    len.i = 0;
    len.intbuf[1] = Data[1];
    len.intbuf[0] = Data[2];
    memset(RcvPc,0,sizeof(RcvPc));
    status = Judge_Rcv1(Data,len.i);   
#if UPDEG
    printf("len.i = %d:status=%d\n",len.i,status);  
#endif   
    if(status == MI_OK)
    {
		RcvPc[0] = Data[4]; 
		//printf("%d,=====All_Cmd_Snd1 COMD=%02x\n",__LINE__,Data[4]);  
        switch(Data[4])    
        {  
#if UPDEG
		printf("%d,=====All_Cmd_Snd1 COMD=%02x\n",__LINE__,Data[4]);    
#endif
		case COMMAND_DOWN:  
            result= DownloadProcess(&Data[5], len.i-1);
            if(result>=0)
        	{
	            RcvPc[1] = result;
        	}
            else
                RcvPc[1] = 0xfe;              
            SendCmdPc_Upfile(RcvPc,2);   
			break;

        case COMMAND_STOP:
            result = StopProcess();
            if(result>=0)
                RcvPc[1] = result;
            else
                RcvPc[1] = 0xfe;
            SendCmdPc_Upfile(RcvPc,2);
            break;

        case COMMAND_CHECK: 
            per.i = 0;  
            result = ReqProcess(&reson, &Data[6], Data[5],&per.i);
            if(result == 0)//�ɹ�
            {
                RcvPc[1] = 0; 
                RcvPc[2] = Data[5];
                memcpy(&RcvPc[3],&Data[6],Data[5]);
                SendCmdPc_Upfile(RcvPc,Data[5]+3);   
                }
             if(result == 1)//ʧ��
            {
                RcvPc[1] = 1;
                RcvPc[2] = reson;                
                SendCmdPc_Upfile(RcvPc,3);  
                }
              if(result == 2)
            {
                RcvPc[1] = 2;
                len.i = 0;
                Convert_IntTOBcd(per.i, len.intbuf);      
                RcvPc[2] = len.intbuf[1];      				
                SendCmdPc_Upfile(RcvPc,3);  
                }
                    
            break;
            
        case COMMAND_EXECUTE:
            result  = ExeProcess();   
            if(result>=0)
        	{  
#if (L9==0)                
				ProgramUpdateFlag = 1; 
				upfilecount_flag = 0; 	   
#endif                
	            RcvPc[1] = result;
        	}
			else
                RcvPc[1] = 0xfe;
            SendCmdPc_Upfile(RcvPc,2);
            break;

        case COMMAND_RECOVER:
            result = RecoverProcess();
            if(result>=0)
                RcvPc[1] = result;
            else
                RcvPc[1] = 0xfe;
            SendCmdPc_Upfile(RcvPc,2);
            break;
           
		case COMMAND_REQ: 
			Upfiletmp = fopen(TEMPFILE, "ab+");  	    	
			if(Upfiletmp != NULL)
			{
				BuffWord.i = 0;
				BuffWord.longbuf[3] = Data[8];
				BuffWord.longbuf[2] = Data[7];
				BuffWord.longbuf[1] = Data[6];
				BuffWord.longbuf[0] = Data[5];				
				//if(BuffWord.word != stbuff.st_size)
				//printf("BuffWord.i = %d:UpdateSize=%d\n",BuffWord.i,FileInfo.offset);
                
                if(BuffWord.i != FileInfo.offset)
				{
                    //printf("������ʼ��ַ����ȷ �������� line=%d------------>",__LINE__);	
					Netstatus = 1; 
					fclose(Upfiletmp);
				}
				else
				{
	                //printf("�������� line=%d------------>",__LINE__);   
					len.i =0;
					len.intbuf[1] = Data[10];
					len.intbuf[0] = Data[9];  
					//printf("�������� line=%d:����=%d:9=%02x:10=%02x------------>\n",__LINE__,len.i,Data[9],Data[10]);
					result = fwrite(Data+11,sizeof(unsigned char),len.i,Upfiletmp);  	  		
                    fclose(Upfiletmp);
                    FileInfo.offset+= len.i; 
                    Upfilecfg = fopen(PROCESSFILE,"rb+"); 
                    fseek(Upfilecfg,0,SEEK_SET);                     
                    fwrite(&FileInfo.filenamelen,sizeof(unsigned char),sizeof(struct Upfile),Upfilecfg);
                    fclose(Upfilecfg); 
#if UPDEG

           printf("FileInfo.len=%02x\n",FileInfo.filenamelen);
		   strs = FileInfo.upfilename;  
		   printf("FileInfo.filename=%s\n",strs);     
           Update.i = 0;
           memcpy(Update.longbuf,&FileInfo.upfilelen,4);
           printf("FileInfo.upfilelen=%d\n",Update.i);       
           Update1.i = 0;
           memcpy(Update1.intbuf,&FileInfo.crc,2);
           printf("FileInfo.crc=%d\n",Update1.i);
           Update1.i = 0;
           memcpy(Update.longbuf,&FileInfo.offset,4); 
           printf("FileInfo.offset=%d\n",Update.i);        

         

#endif
                    Netstatus = 1;
				}
			}
			
			break; 
         
		
        default:
            RcvPc[0] = Data[4];
            RcvPc[1] = 0xFE;						//δ֪����
            SendCmdPc_Upfile(RcvPc,2);
            break;
        }
        
    }
    
#if L9    
    system("sync;");
#endif
}



/****************************************
- �������� : void close_pthread();
- ����˵�� : �ر��߳�
- �������� : �ر��ļ������߳�
- �������� : ��
- ������� : ��
- ������� : ��
******************************************/
void close_pthread()
{
    pthread_join(upfile,NULL);  
    }


/****************************************
- �������� : *DownloadFile_pthread(void *args);
- ����˵�� : �ļ������̺߳���
- �������� : �豸�����󣬿����߳�׼���ļ�����
- �������� : ��
- ������� : ��
- ������� : ��
******************************************/   
 void *DownloadFile_pthread(void *args)
{
    unsigned char sendbuf[50];
    unsigned char len;
    struct stat file_info_pthread;
    int file_size_pthread;

#if UPDEG 
        printf("�����߳�����  line=%d\n",__LINE__);
#endif   
    while(1)    
    {
    	if((Netstatus)&&(ConnectFlag==0))   
	    {
	    	if(access(TEMPFILE ,0)==0)			//���ع�һ��    
    		{
				stat(TEMPFILE,&file_info_pthread);   
				file_size_pthread = file_info_pthread.st_size;
				Update_pthread.i = 0;  
				memcpy(Update_pthread.longbuf,&FileInfo.upfilelen,4); 
				if((file_size_pthread == Update_pthread.i)||(FileInfo.filenamelen==0)/*(COMMAND_Flag != 1)*/)          
				{
					sleep(1);   
				}
				else
				{
			        memset(sendbuf,0,50);
			        sendbuf[0] = COMMAND_REQ;  
			        sendbuf[1] = FileInfo.filenamelen;
			        len = FileInfo.filenamelen;
			        memcpy(&sendbuf[2],FileInfo.upfilename,FileInfo.filenamelen);
			        Update2.i = FileInfo.offset;
			        memcpy(&sendbuf[2+len],Update2.longbuf,4);   
			        SendCmdPc_Upfile(sendbuf,len+6);    
			        Netstatus = 0;         
				}
			}
			else		//δ���ع�
			{
			  if(FileInfo.filenamelen>0)// 1. ���ع�����ֹͣ(�쳣�ж�)  
			  	{
			  		memset(sendbuf,0,50);
			        sendbuf[0] = COMMAND_REQ;  
			        sendbuf[1] = FileInfo.filenamelen;
			        len = FileInfo.filenamelen;
			        memcpy(&sendbuf[2],FileInfo.upfilename,FileInfo.filenamelen);
			        Update2.i = FileInfo.offset;
			        memcpy(&sendbuf[2+len],Update2.longbuf,4);   
			        SendCmdPc_Upfile(sendbuf,len+6);      
			        Netstatus = 0;  
			  	}
			  else		//2.û�и���������
			  	{
			  	sleep(1);
			  	}			
			}
	    
           }
        else
        {
            sleep(1);
            }        
        }
}
#endif   
         

