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
/****使用到的变量*******/
unsigned char Netstatus;
struct Upfile FileInfo;                   //文件信息变量
FILE *Upfilecfg;                          //配置文件的文件指针
FILE *Upfiletmp;                          //临时文件的文件指针
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


/****引用其他文件的变量****/
extern  BMP_INFO bmppicture1;
//extern unsigned char ConnectFlag;			
extern int sockfd;
extern pthread_mutex_t m_socketwrite; 
#if (L9==0)
    extern volatile unsigned char GPRSSendFlag; 
    extern unsigned char ProgramUpdateFlag;//升级文件标识
    extern unsigned int upfilecount_flag ;//升级文件计数值
#else
    extern PIXEL _pix[76800];
#endif



/****************************************
- 函数名称 : int Convert_IntTOBcd(int num,unsigned char *OutBuf)
- 函数说明 : 数据格式转换
- 函数功能 : 将十进制数转换成BCD码
- 输入参数 : 
            num:待转换的10进制数
            *OutBuf:转换后的BCD码
- 输出参数 : 无
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
- 函数名称 : void InitEnv()
- 函数说明 : 初始化断点续传函数
- 函数功能 : 读取已存在于机器中的下载，如果文件不存在则创建一个
- 输入参数 : 无
- 输出参数 : 无
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
                printf("打开下载配置文件成功!\n");   
                fseek(Upfilecfg,0,SEEK_SET);
                fread(&FileInfo.filenamelen,sizeof(unsigned char),sizeof(struct Upfile),Upfilecfg);
                fclose(Upfilecfg);                
            }
            else
            {
                printf("打开配置文件失败\n");               
            }        
        }
    else
        {
            Upfilecfg = fopen(PROCESSFILE,"a+");
         if(Upfilecfg)
            {
                printf("创建下载配置文件成功!\n");                
                fclose(Upfilecfg);                
            }
            else
            {
                printf("创建配置文件失败\n");               
            } 
#if L9
            system("sync");
#endif
        }
    }


/****************************************
- 函数名称 : int DownloadProcess(unsigned char *data);
- 函数说明 : 服务器下载文件信息
- 函数功能 : 接收服务器下载过来的，关于需要下载文件的信息(文件名，大小，校验值)
- 函数返回 : 操作结果
             0:保存成功
             -1:操作失败
- 输入参数 : 
            *data--文件相关信息
            len:文件名长度
- 输出参数 : 无
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
- 函数名称 : int StopProcess()
- 函数说明 : 停止下载
- 函数功能 : 服务器下发停止文件请求命令
- 函数返回 : 操作结果
             0:保存成功
             -1:操作失败
- 输入参数 : 无
- 输出参数 : 无
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
- 函数名称 : int ReqProcess(unsigned char *reson,unsigned char *filename,unsigned char filenamelen,unsigned short *downloadper)
- 函数说明 : 过程查询
- 函数功能 : 服务器下发查询当前已下载文件的信息
- 函数返回 : 查询到的状态
             0:文件下载完成
             1:文件下载失败
             2:文件下载中
             
- 输入参数 : 
             *filename:查询下载的文件名
             filenamelen:查询文件名的长度
- 输出参数 : 
             *reson:下载失败原因
             *downloadper:已下载的百分比
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
        	//printf("%d,文件百分比\n",__LINE__); 
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
- 函数名称 : int ExeProcess()
- 函数说明 : 文件执行
- 函数功能 : 服务器下发执行文件更新命
- 函数返回 : 操作结果
             0:保存成功
             -1:操作失败
- 输入参数 : 无
- 输出参数 : 无
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
			//获取配置文件中的下载保存路径
			memcpy(dir_path,FileInfo.upfilename,FileInfo.filenamelen);
			//strcpy(pTmp + strlen(pTmp), dirname(FileInfo.upfilename));
			strcpy(pTmp + strlen(pTmp), dirname(dir_path));
			printf("\n last dir : %s \n",pTmp);
            strs = FileInfo.upfilename;  
		    printf("FileInfo.filename=%s\n",strs); 
	        sprintf(save_path,"%s",FileInfo.upfilename);				
			//printf("last cmd : %s \n",pTmp);//pTmp :/opt/minigui/app
			//文件转存，将临时文件转存为需要下载的文件
			memset(pTmp_,0,300);
			sprintf(pTmp_,"mv "TEMPFILE" %s",FileInfo.upfilename);//pTmp_ : mv /opt/minigui/app/update.bin /opt/minigui/app/desktop.tar.gz 
			printf("\n last name : %s \n",pTmp_);
			system(pTmp_);

	        //保存原来的程序文件
	        if(access("/opt/minigui/app/desktop",0)==0)
	           system("cp desktop desktop_bak");
	        if(access("/var/run/armok",0)==0)  
	           system("cp armok armok_bak");
	        
	        //文件解压
			memset(pTmp_,0, 300);
			sprintf(pTmp_, "tar -zxvf %s -C %s", save_path, pTmp);
            printf("\n last name : %s \n",pTmp_);
			system(pTmp_);
			
			//转存图片文件到指定目录
		#if 0	
			memset(pTmp_,0,300);
			sprintf(pTmp_,"mv %s/*.png /opt/res",pTmp);	//pTmp :/opt/minigui/app				
			system(pTmp_);
		#endif
#if L9
            if(access("back.bmp",0)==0)
            {
                //load_BMP("/var/run/back.bmp", &_pix);
                printf("--------------------发现bmp图片文件，准备升级---------------------\n");
			#if 0	// 立即生效
                free(bmppicture1.data);
				bmppicture1.data=NULL;
                if(Load_Bmp_Img("/var/run/back.bmp",&bmppicture1))
                	{
                		printf("升级图片失败------------\n");
							Eflag = 0;
                	}
				else{
						system("cp back.bmp ./res");
							Eflag = 1;
					}		
			#else	//重开机后生效
					system("cp back.bmp ./res");
			
			#endif
				
               }
			else{
				  printf("--------------------未发现bmp图片文件---------------------\n");
				}


			//任何图片只要在执行路径下都可以下载正确
			
            system("mv *.wav /var/run/sound/");
#endif            

			//完成执行之后将配置文件清零作为请求线程的判断条件
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
- 函数名称 : int RecoveryProcess()
- 函数说明 : 恢复原文件
- 函数功能 : 服务器下发恢复上一个版本程序命令
- 函数返回 : 操作结果
             0:保存成功
             -1:操作失败
- 输入参数 : 无
- 输出参数 : 无
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
- 函数名称 : int write_datas_gprs_upfile(int fd, unsigned char *buffer, int buf_len)
- 函数说明 : 发送请教数据
- 函数功能 : 通过SOKET套接字发送请求数据
- 函数返回 : 操作结果
             0:保存成功
             -1:操作失败
- 输入参数 : 
            fd:发送使用的套接字
            *buffer:待发送的数据
            buf_len:发送数据的长度
- 输出参数 : 无
******************************************/
 int write_datas_gprs_upfile(int fd, unsigned char *buffer, int buf_len)
{
    int status;
    pthread_mutex_lock(&m_socketwrite); //线程自锁
    status = write_datas_tty(fd,buffer,buf_len);
    pthread_mutex_unlock(&m_socketwrite);//线程解锁
    return status;
}



/****************************************
- 函数名称 : static char GprsCmd_Upfile(unsigned char *SndDa)()
- 函数说明 : 发送数据
- 函数功能 : 通过SOCET，向服务端发送数据
- 函数返回 : 操作结果
             0:保存成功
             -1:操作失败
- 输入参数 : 
            *SndDa:要发送的数据
- 输出参数 : 无
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
- 函数名称 : static char  SendCmdPc_Upfile(unsigned char *Date,unsigned short LLen) 
- 函数说明 : 发送数据组包
- 函数功能 : 将要发送的数据按照相关格式进行组包
- 函数返回 : 操作结果
             0:保存成功
             -1:操作失败
- 输入参数 : 
            *Date:要发送的数据
            LLEN:发送数据的长度
- 输出参数 : 无
******************************************/
static char  SendCmdPc_Upfile(unsigned char *Date,unsigned char LLen) 
{
    char status;
    unsigned char Scheme[135];
    unsigned char i,ch; 
	//printf("上送的数据长度LLen = %x",LLen);         	
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
- 函数名称 : unsigned int LenDataCmd1(unsigned char *DataLen) 
- 函数说明 : 计算发送数据长度
- 函数功能 : 通过公式计算出接收到数据的长度
- 函数返回 : 接收到的数据长度
- 输入参数 : 
            *DateLen:接收到数据的长度
            
- 输出参数 : 无
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
- 函数名称 : unsigned char Judge_Rcv1(unsigned char *Packet,unsigned int Data_len)
- 函数说明 : 校验数据
- 函数功能 : 校验接收到数据包的正确性
- 函数返回 : 
            0:成功
            2:失败
- 输入参数 :
            *Packet:接收到的数据
            Data_len:接收到数据的长度            
- 输出参数 : 
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
        printf("\n");		//收到完整的贞数
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
- 函数名称 : void All_Cmd_Snd1(unsigned char *Data) 
- 函数说明 : 命令解析
- 函数功能 : 将接收到的数据进行解析
- 函数返回 : 无
- 输入参数 : 
            *DateLen:接收到数据的长度            
- 输出参数 : 
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
            if(result == 0)//成功
            {
                RcvPc[1] = 0; 
                RcvPc[2] = Data[5];
                memcpy(&RcvPc[3],&Data[6],Data[5]);
                SendCmdPc_Upfile(RcvPc,Data[5]+3);   
                }
             if(result == 1)//失败
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
                    //printf("下载起始地址不正确 重新请求 line=%d------------>",__LINE__);	
					Netstatus = 1; 
					fclose(Upfiletmp);
				}
				else
				{
	                //printf("正常下载 line=%d------------>",__LINE__);   
					len.i =0;
					len.intbuf[1] = Data[10];
					len.intbuf[0] = Data[9];  
					//printf("正常下载 line=%d:数据=%d:9=%02x:10=%02x------------>\n",__LINE__,len.i,Data[9],Data[10]);
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
            RcvPc[1] = 0xFE;						//未知命令
            SendCmdPc_Upfile(RcvPc,2);
            break;
        }
        
    }
    
#if L9    
    system("sync;");
#endif
}



/****************************************
- 函数名称 : void close_pthread();
- 函数说明 : 关闭线程
- 函数功能 : 关闭文件升级线程
- 函数返回 : 无
- 输入参数 : 无
- 输出参数 : 无
******************************************/
void close_pthread()
{
    pthread_join(upfile,NULL);  
    }


/****************************************
- 函数名称 : *DownloadFile_pthread(void *args);
- 函数说明 : 文件下载线程函数
- 函数功能 : 设备开机后，开启线程准备文件下载
- 函数返回 : 无
- 输入参数 : 无
- 输出参数 : 无
******************************************/   
 void *DownloadFile_pthread(void *args)
{
    unsigned char sendbuf[50];
    unsigned char len;
    struct stat file_info_pthread;
    int file_size_pthread;

#if UPDEG 
        printf("升级线程启动  line=%d\n",__LINE__);
#endif   
    while(1)    
    {
    	if((Netstatus)&&(ConnectFlag==0))   
	    {
	    	if(access(TEMPFILE ,0)==0)			//下载过一次    
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
			else		//未下载过
			{
			  if(FileInfo.filenamelen>0)// 1. 下载过程中停止(异常中断)  
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
			  else		//2.没有给下载命令
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
         

