#include "client.h"
#include "queue.h"

#define IP "211.145.51.150"
#define PORT 40009

//#define IP "172.0.9.106"
//#define PORT 5800


#define CLIENT_DEBUG 1

#ifdef CLIENT_DEBUG
	#define DebugPrintf  printf
#else
	#define DebugPrintf(format, ...)	
#endif



static char ipbuf[24]={IP};
static unsigned short  port=PORT;
static int  netdevicestatus=0;		//网络设备可用状态: 0 不可用，1 可用
static int  connetstatus=0;			//连接服务器状态:  0 断开 ，1 连接
static int  sockfd=-1;		
pthread_mutex_t m_socketwrite = PTHREAD_MUTEX_INITIALIZER; //


static heart_info * heatinfo=NULL;
static device_info * devinfo=NULL;

enum NET_ERROCODE{
	
	ERAD_TIMEOUT = -11,
	READ_ERR 	= -10,
	WRITE_TIMOUT = -9,
	WRITE_ERR	= -8,
	SELECT_ERR	=-7,
	NULL_POINT_ERR = -6,
	UNKNOWN_ERR   = -5,
	SELECT_TIMOUT = -4,
	CONNET_ERR  = -3,
	F_SETFL_ERR = -2,
	F_GETFL_ERR = -1,
	SUCCESS		= 0
};





void display_neterr(enum NET_ERROCODE err)		//写入日志
{

	switch(err)
	{
		case WRITE_ERR:
				printf("write socket err\n");
				break;
		case WRITE_TIMOUT:
				printf("write socket timeout\n");
				break;
		case SELECT_ERR:
				printf("select socket err\n");
				break;
		case UNKNOWN_ERR:
				printf("unknown err\n");
				break;
		case SELECT_TIMOUT:
				printf("select socket timout\n");
				break;
		case CONNET_ERR:
				printf("connect err\n");
				break;
		default:
			break;
	}	
	
}




device_info * init_device_info()
{

	 
	device_info * devinfo=(device_info *)malloc(sizeof(device_info));
	if(devinfo==NULL)
	{
	    fprintf(stderr, "Failed to malloc memory, errno:%u, reason:%s\n",errno, strerror(errno));
	    printf(" malloc err for new space \n");
    	return NULL;
     }

	memset(devinfo,0,sizeof(device_info));
	
#if 1
	memcpy(devinfo->posid,"\x21\x00\x17\x1F",4);
#endif
	
	return devinfo;
}


heart_info * get_heart_info(device_info * devinfo)
{

	if(devinfo==NULL){
		printf(" in func %s ,the devinfo is NULL \n",__func__);
		return NULL;
	}

	heart_info * heartinfo=(heart_info *)malloc(sizeof(heart_info));
	if(heartinfo==NULL)
	{
	    fprintf(stderr, "Failed to malloc memory, errno:%u, reason:%s\n",errno, strerror(errno));
	    printf(" malloc err for new space \n");
    	return NULL;
     }

#if 0
	heartinfo->PosStatus=0;
	memcpy(heartinfo->FirVer,devinfo->FirVer,2);
	heartinfo->TerParVer=devinfo->TerParVer;
	heartinfo->Line=devinfo->Line;
	heartinfo->BlkParVer=devinfo->BlkParVer;
	heartinfo->CatpParVer=devinfo->CatpParVer;
	heartinfo->MchCode=devinfo->MchCode;
	
	memcpy(heartinfo->SamID,devinfo->SamID,6);	
	memcpy(heartinfo->LinVer,devinfo->LinVer,2);
	heartinfo->InCarBlkParVer=devinfo->InCarBlkParVer;
	heartinfo->QRBlkVer=devinfo->QRBlkVer;
	heartinfo->InQRBlkParVer=devinfo->InQRBlkParVer;
	heartinfo->HTBlkParVer=devinfo->HTBlkParVer;
	heartinfo->HTInBlkParVer=devinfo->HTInBlkParVer;

	memcpy(heartinfo->PriParVer,devinfo->PriParVer,2);	
	memcpy(heartinfo->MilParVer,devinfo->MilParVer,2);	
	memcpy(heartinfo->HTtwoRelParVer,devinfo->HTtwoRelParVer,2);	
	memcpy(heartinfo->HTWriPar,devinfo->HTWriPar,2);	
#else	//模拟数据
	heartinfo->PosStatus=0;
	heartinfo->FirVer[0]=0x00;heartinfo->FirVer[1]=0x46;
	memcpy(heartinfo->TerParVer,"\x00\x00",2);
	memcpy(heartinfo->Line,"\x01\x00",2);
	memcpy(heartinfo->BlkParVer,"\x07\x00\x00\x00",4);
	memcpy(heartinfo->CatpParVer,"\x00\x00\x00\x00",4);
	memcpy(heartinfo->MchCode,"\x00\x00",2);
	memcpy(heartinfo->SamID,"\x99\x99\x99\x00\x05\x06",6);
	memcpy(heartinfo->LinVer,"\x00\x11",2);
	memcpy(heartinfo->InCarBlkParVer,"\x02\x00\x00\x00",4);
	memcpy(heartinfo->QRBlkVer,"\x00\x00\x00\x00",4);
	memcpy(heartinfo->InQRBlkParVer,"\x00\x00\x00\x00",4);
	memcpy(heartinfo->HTBlkParVer,"\x03\x00\x00\x00",4);
	memcpy(heartinfo->HTInBlkParVer,"\x02\x00\x00\x00",4);
	memcpy(heartinfo->PriParVer,"\x00\x11",2);
	memcpy(heartinfo->MilParVer,"\x00\x25",2);
	memcpy(heartinfo->HTtwoRelParVer,"\x00\x11",2);
	memcpy(heartinfo->HTWriPar,"\x00\x05",2);
#endif


	return heartinfo;
}


int write_datas_tty(int fd, unsigned char *buffer, int buf_len)
{

	struct timeval tv;
    fd_set w_set;
    int bytes_to_write_total = buf_len;
    int bytes_have_written_total = 0;
    int bytes_write = 0;
    int result = -1;
    unsigned char *ptemp = buffer;
	int i;
	enum NET_ERROCODE err;

	if ((fd<0) ||( NULL == buffer )|| (buf_len <=0))
    {
    	printf("in func %s send buffer is NULL \n",__func__);
        return NULL_POINT_ERR;
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
            return SELECT_ERR;
        }
		else if (0 == result)    //this means timeout, it is not an error, so we return 0.
        {
		
            return SELECT_TIMOUT;
        }
		else
		{
			if (FD_ISSET(fd, &w_set))
            {
				
                DebugPrintf("W socket=%03d::%s \n",bytes_to_write_total,ptemp);
				for(i=0;i<bytes_to_write_total;i++)
					DebugPrintf("%02x ",ptemp[i]);
				DebugPrintf("\n");
		
				bytes_write = send(fd, ptemp, bytes_to_write_total, 0);
				DebugPrintf("bytes_write = 0x%02X\n",bytes_write);

				  if (bytes_write < 0)
                  {
                    if (EAGAIN == errno || EINTR == errno)
                    {
                        continue;
                    }
                    return WRITE_ERR;
                 } 
				 else if (0 == bytes_write)
                 {
        
                    return WRITE_TIMOUT;
                 } 
				 else{
					bytes_to_write_total -= bytes_write;
                    bytes_have_written_total += bytes_write;
                    ptemp += bytes_have_written_total;
				 }
			}
		}
		
	 }
	
	 return SUCCESS;
}


int write_datas_gprs(int fd, unsigned char *buffer, int buf_len)
{
    int status;
	printf("---in func %s\n",__func__);
    pthread_mutex_lock(&m_socketwrite); //线程自锁
    status = write_datas_tty(fd,buffer,buf_len);
    pthread_mutex_unlock(&m_socketwrite);//线程解锁
    return status;
}




int create_sorket(const char * ip,unsigned short port,int * fd)
{

	int sockfd;
	int ret;

	if(ip==NULL || fd==NULL){

		return NULL_POINT_ERR;
	}

	printf("in func %s ,ip=%s ,port=%d \n",__func__,ip,port);

	struct sockaddr_in tcpaddr;		//服务器地址
	enum NET_ERROCODE err;
	 
	sockfd =socket(AF_INET,SOCK_STREAM,0);
	
	int setvalue=1;
	int setflag;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(const char*)&setvalue,sizeof(int));
	 if((setflag= fcntl(sockfd, F_GETFL)) < 0 )
    {
        perror("fcntl F_SETFL");
        close(sockfd);
        err = F_GETFL_ERR;
		goto SCKERR;
    }
    setflag|= O_NONBLOCK;
    if(fcntl(sockfd, F_SETFL,setflag) < 0)
    {
        perror("fcntl");
        close(sockfd);
        err = F_SETFL_ERR;
		goto SCKERR;
    }

	  
	  tcpaddr.sin_family=AF_INET; 
	  tcpaddr.sin_port=htons(port);
	  tcpaddr.sin_addr.s_addr=inet_addr(ip);
	  ret = connect(sockfd,(struct sockaddr *)&tcpaddr,sizeof(struct sockaddr));
	 /* 
	  if(ret){
		err = CONNET_ERR;
		goto SCKERR;
	  }
	*/
	
	fd_set fdset;	
	struct timeval vl;
	
	FD_ZERO(&fdset);
    FD_SET(sockfd,&fdset);
	vl.tv_sec  = 5;
    vl.tv_usec = 0;
    ret=select(sockfd+1,0,&fdset,0,&vl);
	if(ret==0)
	{
		perror("connect socket select() timeout");
        close(sockfd);
        err=SELECT_TIMOUT;
	}
	else if (ret>0)
	{
		setflag = fcntl(sockfd, F_GETFL,0);
        setflag &= ~ O_NONBLOCK;
        fcntl(sockfd, F_SETFL,setflag);
        FD_CLR(sockfd,&fdset);
        *fd=sockfd;
        return SUCCESS;

	}
	else{
		err=UNKNOWN_ERR;
		goto SCKERR;
	}

SCKERR:
	return err;

}




int update_connect_server_status(int status)
{

	if(status!=0 && status !=1)
		{
			printf("invalid peramiter \n");
			return -1;
		}

	connetstatus=status;
	return 0;


}




int update_net_device_satus(int status)
{
	if(status!=0 && status !=1)
		{
			printf("invalid peramiter \n");
			return -1;
		}

	netdevicestatus=status;
	return 0;
}


int set_server_ip(char * svip,int iplen,int svport)
{
	/* 先检查ip的格式是否正确*/


	memset(ipbuf,0,sizeof(ipbuf));		
	memcpy(ipbuf,svip,iplen);
	port=svport;
	return 0;
}



/* 以下是构建各类任务的实例 */

/*该函数只针对数字的转换，其他字符无效 */

static unsigned char char_to_bin(unsigned char bchar)
{
	if ((bchar >= '0')&&(bchar <= '9'))
		return(bchar-'0');
	else
	{
		if ((bchar >= 'A')&&(bchar <= 'F'))
			return(bchar-'A'+10);
		if ((bchar >= 'a')&&(bchar <= 'f'))
			return(bchar-'a'+10);
		else
            return(0);        
	}
}


static void asc_to_bcd(unsigned char *bcd, unsigned char *asc, unsigned int asc_len)
{
	unsigned int i,j;
	unsigned char bOddFlag, bchar, bchar1, bchar2;

	if (asc_len%2)
		bOddFlag = 1;
	else
		bOddFlag = 0;

	for (i=0,j=0; j<asc_len; i++)
	{
		if ((i==0)&&(bOddFlag))
		{
			bchar1 = asc[j++];
			bcd[i] = char_to_bin(bchar1);
		}
		else
		{
			bchar1 = asc[j++];
			bchar = char_to_bin(bchar1);
			bchar *= 16;
			bchar1 = asc[j++];
			bchar2 = char_to_bin(bchar1);
			bcd[i] = bchar + bchar2;
		}
	}
}


static unsigned int bcd_to_bin(unsigned char *bcd, int len)
{
	unsigned int lbin=0;
	int i,k;
	

	for(i=0; i<len; i++)
	{
		k=(bcd[i]>>4)*10+(bcd[i]&0x0f);
		lbin=lbin*100+k;
		printf("--- i=[%d]  lbin=%u \n",i,lbin);
	}
    
	return lbin;
}




/*crc-16*/

 static int CRC_POLY_DEFAULT = 0xC0C1; //生成多项式:X^15 + X^14 + X^7 + X^6 + 1
 /*
	初始值默认为0
 */
 int CRC_16(unsigned char * bytes,int length) { 
		int i,index;
		int crc = 0x00;          // initial value  
          
       	for (index = 0 ; index< length; index++) {  
       		unsigned char b = bytes[index];  
       		for ( i = 0; i < 8; i++) {  
       			
       			int bit = ((b   >> (7-i) & 1) == 1);  
       			int c15 = ((crc >> 15    & 1) == 1);  
       			
       			crc <<= 1;  
       			
       			if (c15 ^ bit) crc ^= CRC_POLY_DEFAULT;  
       		}  
       	}  
       	
       	crc &= 0xffff;  
       	return crc; 
}

 /*
	初始值需要外部传入
 */
int CRC_16_no_init(unsigned char * bytes,int length,int begain)
{
	int i,index;
	int crc = begain;          // initial value  
          
       	for (index = 0 ; index< length; index++) {  
       		unsigned char b = bytes[index];  
       		for ( i = 0; i < 8; i++) {  
       			
       			int bit = ((b   >> (7-i) & 1) == 1);  
       			int c15 = ((crc >> 15    & 1) == 1);  
       			
       			crc <<= 1;  
       			
       			if (c15 ^ bit) crc ^= CRC_POLY_DEFAULT;  
       		}  
       	}  
       	
       	crc &= 0xffff;  
       	return crc; 


}




unsigned short crc_16(unsigned char * data,int len)
{
const unsigned short crc16_table[256] ={
0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
	};

unsigned short crc=0;
unsigned char da;
unsigned char *ptr;
	ptr=data;
	while(len--!=0)
	{
		da=(unsigned char)(crc/256);
		crc=crc<<8&0xFFFF;
		crc^=crc16_table[da^(*ptr)];
		ptr++;
	}
	
	return crc;

}


int Calc_crc(unsigned char *in, unsigned int len, unsigned short * out,unsigned short begin)
{
	int  i, j, stat;
	//unsigned int crc = 0x0000;//ttt;
	unsigned short crc;
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
	if (crc)
		stat=1;
	
	return (stat);	
}


/* 计算mesg 的crc16值*/
/*
	datalen: boy部分data 的长度
*/
int get_msg_crc16(const message * mesg,int datalen,char *crc)
{

	if(mesg==NULL){
		printf("in func %s paramiter is NULL\n",__func__);
		return -1;
	}
	/*CRC16 计算*/
	unsigned char *crc_buf=NULL;
	int crc_buflen=MSG_TYPE_LEN+VERSION_LEN+POS_ID_LEN+datalen;
	crc_buf=(unsigned char *)malloc(crc_buflen);
	if(crc_buf==NULL)
	{
			printf(" malloc err \n");
			return -1;
	}
	
	int pos=0;
	memcpy(crc_buf+pos,mesg->type,MSG_TYPE_LEN);
	pos+=MSG_TYPE_LEN;
	crc_buf[pos]=mesg->ver;
	pos+=VERSION_LEN;
	memcpy(crc_buf+pos,mesg->posid,POS_ID_LEN);
	pos+=POS_ID_LEN;
	memcpy(crc_buf+pos,mesg->data,datalen);
	//pos+=datalen;

#if CLIENT_DEBUG
		int i;
		printf("in func %s ,crcbuff len=%d\n",__func__,crc_buflen);
		printf("crc buff = ");
		for(i=0;i<crc_buflen;i++)
			printf("%02x ",crc_buf[i]);
		printf("\n");
#endif

	unsigned short crc16in=0x0,crc16out;
	unsigned char asc_buff[16];
	//Calc_crc(crc_buf,crc_buflen,&crc16out,crc16in);
	//crc16out=crc_16(crc_buf, crc_buflen);
	crc16out=CRC_16(crc_buf, crc_buflen);
	free(crc_buf);

	printf("in func %s, crc_out = %02x\n",__func__,crc16out);
	//sprintf(asc_buff,"%04d",crc16out);
	//asc_to_bcd(crc,asc_buff,4);
	ShortUnon sd;
	sd.i=crc16out;
	crc[0]=sd.intbuf[1];
	crc[1]=sd.intbuf[0];
	
	return 0;
	
}

#define  HEART_PART_PRO

/*************************************************心跳部分****************************************/

/*
	构建心跳任务信息体

*/
int create_send_heart_mission(heart_info *heartinfo,mission_info * out)
{
	
	int mesglen,datalen,pos;
	char asc_buff[64];
	message mesg={0};
	ShortUnon shortdata;

	
	if(heartinfo==NULL)
		return -1;
	
	/*
		
	*/
	if(out==NULL)
		return -1;

	printf("-------- in func %s---------\n",__func__);

	datalen=sizeof(heart_info);
	mesglen=datalen+SYN_INFO_LEN+COMPRESS_LEN+MSG_TYPE_LEN+VERSION_LEN+POS_ID_LEN+CRC_LEN;
#if 0	
	memset(asc_buff,0,sizeof(asc_buff));		
	sprintf(asc_buff,"%04d",mesglen);
	asc_to_bcd(mesg.len,asc_buff,2*sizeof(mesg.len));
#else
	shortdata.i=mesglen;
	//memcpy(mesg.len,shortdata.intbuf,2);
	mesg.len[0]=shortdata.intbuf[1];
	mesg.len[1]=shortdata.intbuf[0];
#endif

	/*同步信息*/
	mesg.syncinfo[0]=(SEND_HEART_SYN>>8 )& 0x00ff;
	mesg.syncinfo[1]=(SEND_HEART_SYN & 0x00ff);

	mesg.compress=0x0;

	mesg.type[0]=0x30;
	mesg.type[1]=0x01;
	
	mesg.ver=0x01;
	
	memcpy(mesg.posid,devinfo->posid,sizeof(mesg.posid));
	memcpy(mesg.data,heartinfo,datalen);

	/*CRC16 计算*/
	int ret;
	char crc[2];
	ret=get_msg_crc16(&mesg,datalen,crc);
	if(ret==0)
		{
		mesg.crc[0]=crc[0];
		mesg.crc[1]=crc[1];
		}
	else
		return -1;

	/*将消息内容填入任务包*/
	out->type=0x3001;
	out->datalen=mesglen+2;		//+2 是包含了包长字段
	out->status=0;
	out->priority=0;
	
	pos=0;
	memcpy(out->data+pos,mesg.len,2);
	pos+=2;
	memcpy(out->data+pos,mesg.syncinfo,SYN_INFO_LEN);
	pos+=SYN_INFO_LEN;
	out->data[pos]=mesg.compress;
	pos+=COMPRESS_LEN;
	memcpy(out->data+pos,mesg.type,MSG_TYPE_LEN);
	pos+=MSG_TYPE_LEN;
	out->data[pos]=mesg.ver;
	pos+=VERSION_LEN;
	memcpy(out->data+pos,mesg.posid,POS_ID_LEN);
	pos+=POS_ID_LEN;
	memcpy(out->data+pos,mesg.data,datalen);
	pos+=datalen;
	memcpy(out->data+pos,mesg.crc,CRC_LEN);

	return 0;
	
}


int send_mesg(mission_list * mission)
{
	enum NET_ERROCODE err;

	if(mission==NULL)
		{
			printf("can not send NULL heart mission data \n");
			return -1;
		}

	printf("send_mesage :: ready to send === \n");	
	err=write_datas_gprs(sockfd,mission->missinfo.data,mission->missinfo.datalen);
	if(err!=SUCCESS)
	{
		display_neterr( err);
		return -1;
	}
	else
		return 0;
}


/*bit 是从0开始计数的 */
unsigned char get_BitMap(unsigned char *pbitmap, unsigned char bit) //first from one
{
	unsigned char nbyte;
	unsigned char nbit;
	if (pbitmap == NULL)  return 0;
	bit -= 1;
	nbyte = bit/8;
	nbit = bit%8;

	return (pbitmap[nbyte] & (0x01<<(7-nbit)));
}

int analyse_bitmap(int bit)
{

	int status;
	mission_info missioninfo={0};
	printf("-- in func %s ,bit = %d \n",__func__,bit);

	switch(bit)
	{
		case 1:						//添加更新二维码黑名单任务
			
			break;
		case 2:						//添加更新固件任务
		
			/*添加请求下载固件描述信息任务*/	
			status=create_request_update_firm_info_mission(&missioninfo);
			break;
		case 3:						//添加更新线路站点信息文件任务 ，M3文件
			status=create_request_file_desinfo(4,&missioninfo);
			break;
		case 4:						//添加更新终端运营参数任务，就是商户信息
			status=create_request_mchntid_mission(&missioninfo);
			break;
		case 5:						//添加更新卡类型参数任务M2
			break;
		case 6:						//更新实体卡黑名单G1
			status=create_request_file_desinfo(1, &missioninfo);
			break;
		case 7:						//更新本地实体卡增量黑名单G4
			status=create_request_file_desinfo(2, &missioninfo);
			break;
		case 8:						//更新二维码增量黑名单 G10
			status=create_request_file_desinfo(5,&missioninfo);
			break;
		case 9:						//更新互通卡黑名单	G7
			status=create_request_file_desinfo(7,&missioninfo);
			break;
		case 10:						//更新互通卡增量黑名单 G6
			status=create_request_file_desinfo(6,&missioninfo);
			break;
		case 11:					//M4 文件，更新票价表文件
			status=create_request_file_desinfo(8,&missioninfo);
			break;
		case 12:					//M5 文件，更新折扣率文件
			status=create_request_file_desinfo(9,&missioninfo);
			break;
		case 13:					//Mp 文件，更新费率卡二次发行信息
			status=create_request_file_desinfo(10,&missioninfo);
			break;
		case 14:				//更新互通城市白名单文件W1
			status=create_request_file_desinfo(11,&missioninfo);
			break;
		case 15:
			break;
		case 16:
			break;
		default:
			break;
	}

	mission_list * tempmission;
	
	if(status<0)
	{
		printf("in func %s ,create send  mission faild \n",__func__);	
		return -1;
	}
	else
	{

	tempmission=new_mission(&missioninfo,NULL);
	if(tempmission!=NULL)
			if(tempmission->missinfo.priority)
				add_mission( tempmission);
			else
				add_mission_tail(tempmission);
	}	

	return 0;
}


/*
	mission:为发送时的任务消息
	framdata:对应该任务消息的返回报文
	framesz:报文大小
*/
int heart_back_process(mission_list * mission,char * framdata,int framesz)
{

	message msg;
	heart_bak_info heartbakinfo;
	

	printf("--------- in func %s ----\n",__func__);

	if(framdata==NULL || mission==NULL)
		{
			printf("in func %s paramiter is NULL \n",__func__);
			return -1;
		}
	
	int pos=0;
	memcpy(msg.len,framdata+pos,2);
	pos+=2;
	memcpy(msg.syncinfo,framdata+pos,SYN_INFO_LEN);	
	pos+=SYN_INFO_LEN;
	msg.compress=framdata[pos];
	pos+=COMPRESS_LEN;
	memcpy(msg.type,framdata+pos,MSG_TYPE_LEN);
	pos+=MSG_TYPE_LEN;
	msg.ver=framdata[pos];
	pos+=VERSION_LEN;
	memcpy(msg.posid,framdata+pos,POS_ID_LEN);
	pos+=POS_ID_LEN;
	
	int datalen=framesz-2-SYN_INFO_LEN-COMPRESS_LEN-MSG_TYPE_LEN-VERSION_LEN-POS_ID_LEN-CRC_LEN;
	memcpy(msg.data,framdata+pos,datalen);
	pos+=datalen;
	memcpy(msg.crc,framdata+pos,CRC_LEN);
	

	/*计算该包的crc是否正确*/
	int ret;
	unsigned char crc[2];
	ret=get_msg_crc16(&msg,datalen,crc);
	if(crc[0]!=msg.crc[0]||crc[1]!=msg.crc[1])
		{
			printf("in func %s crc err,my crc=%02x%02x  msg.crc=%02x%02x\n",__func__,crc[0],crc[1],msg.crc[0],msg.crc[1]);
			return -2;
		}

	/*类型匹配和同步匹配*/
	if(mission->missinfo.type!=MISS_SEND_HEART)
		{
			printf("in func %s ,mission not match the back data\n",__func__);
			return -3;
		}
	
	if(msg.type[0]!=0x30 || msg.type[1]!=0x81)
		{
			printf("in func %s ,back data not match the mission\n",__func__);
			return -3;
		}

	if(msg.syncinfo[0]!=mission->missinfo.data[2] || msg.syncinfo[1]!=mission->missinfo.data[3])
		{
			printf("in func %s ,SYNC INFO NOT MATHC \n",__func__);
			return -4;
		}

	
	memcpy(&heartbakinfo,msg.data,sizeof(heartbakinfo));

	DebugPrintf(" DATAtime: %02x%02x%02x%02x%02x%02x%02x \n",heartbakinfo.dattime[0],\
		heartbakinfo.dattime[1],heartbakinfo.dattime[2],heartbakinfo.dattime[3],\
		heartbakinfo.dattime[4],heartbakinfo.dattime[5],heartbakinfo.dattime[6]);
	DebugPrintf(" FLAG: %02x%02x \n",heartbakinfo.flag[0],heartbakinfo.flag[1]);
	DebugPrintf(" BUSCODE: %02x%02x%02x%02x \n",heartbakinfo.buscode[0],heartbakinfo.buscode[1],heartbakinfo.buscode[2],heartbakinfo.buscode[3]);

	int i;
#if 0
	for(i=0;i<16;i++)
	{
			if(get_BitMap(heartbakinfo.flag,i+1))
				{
					analyse_bitmap(i);		//处理完一个任务再检查下一个更新
					break;
				}
	}
#else
		//analyse_bitmap(13);		//Mp	
		//analyse_bitmap(12);		//M5
		//analyse_bitmap(11);		//M4
		analyse_bitmap(3);		//M3
		//analyse_bitmap(2);		//固件，没通过

		//analyse_bitmap(6);		//G1文件
		//analyse_bitmap(7);		//G4 文件
		//analyse_bitmap(8);		//G10
		//analyse_bitmap(10);		//G6
		//analyse_bitmap(9);		//G7
		//analyse_bitmap(14);		//w1
#endif

	mission->missinfo.status++;
	return 0;
	
}



#define RECORD_AUTO_UP_PART

/*************************************** A2,A5,A6上传部分 ****************************************/

/*
	构建上传记录信息体 0x3002命令的上传内容
*/
int create_send_A2_mission(record_info * recinfo,mission_info * out)
{
	int mesglen,datalen,pos;
	char asc_buff[64];
	message mesg={0};


	if(recinfo==NULL)
			return -1;
	if(out==NULL)
			return -1;

	printf(" -- in func %s --  \n",__func__);
	datalen=sizeof(record_info);
	mesglen=datalen+SYN_INFO_LEN+COMPRESS_LEN+MSG_TYPE_LEN+VERSION_LEN+POS_ID_LEN+CRC_LEN;	//不包括包长字段的长度

#if 0
	memset(asc_buff,0,sizeof(asc_buff));	
	sprintf(asc_buff,"%04d",mesglen);
	asc_to_bcd(mesg.len,asc_buff,2*sizeof(mesg.len));	//注意 包长可能不一定是bcd码
#else
	ShortUnon shortdata;
	shortdata.i=mesglen;
	//memcpy(mesg.len,shortdata.intbuf,2);
	mesg.len[0]=shortdata.intbuf[1];
	mesg.len[1]=shortdata.intbuf[0];
	
#endif
	

	/*同步信息*/
	mesg.syncinfo[0]=(SEND_A2_SYN>>8) & 0x00ff;
	mesg.syncinfo[1]=(SEND_A2_SYN & 0x00ff);

	mesg.compress=0x0;

	mesg.type[0]=0x30;
	mesg.type[1]=0x02;
	
	mesg.ver=0x1;
	
	memcpy(mesg.posid,devinfo->posid,sizeof(mesg.posid));
	memcpy(mesg.data,recinfo,datalen);

	/*CRC16 计算*/
	int ret;
	char crc[2];
	ret=get_msg_crc16(&mesg,datalen,crc);
	if(ret==0)
		{
		mesg.crc[0]=crc[0];
		mesg.crc[1]=crc[1];
		}
	else
		return -1;

	/*将消息内容填入任务包*/
	out->type=MISS_SEND_A2;
	out->datalen=mesglen+2;		//+2 是包含了包长字段
	out->status=0;
	out->priority=0;
	
	pos=0;
	memcpy(out->data+pos,mesg.len,2);
	pos+=2;
	memcpy(out->data+pos,mesg.syncinfo,SYN_INFO_LEN);
	pos+=SYN_INFO_LEN;
	out->data[pos]=mesg.compress;
	pos+=COMPRESS_LEN;
	memcpy(out->data+pos,mesg.type,MSG_TYPE_LEN);
	pos+=MSG_TYPE_LEN;
	out->data[pos]=mesg.ver;
	pos+=VERSION_LEN;
	memcpy(out->data+pos,mesg.posid,POS_ID_LEN);
	pos+=POS_ID_LEN;
	memcpy(out->data+pos,mesg.data,datalen);
	pos+=datalen;
	memcpy(out->data+pos,mesg.crc,CRC_LEN);

	return 0;
}


/*
	错误码请参照文档
	成功返回0

*/
int  analyse_A2_respondcode(char * respond,int reslen)
{

	
	return 0;
}

/*
	mission:为发送时的任务消息
	framdata:对应该任务消息的返回报文
	framesz:报文大小
*/
int A2_back_process(mission_list * mission,char * framdata,int framesz,record_bak_info * respond)
{
	message msg;
	record_bak_info backinfo;
	

	DebugPrintf("-----  in func %s  ----- \n",__func__);
	if(framdata==NULL || mission==NULL || respond==NULL)
		{
			printf("in func %s paramiter is NULL \n",__func__);
			return -1;
		}
	
	int pos=0;
	memcpy(msg.len,framdata+pos,2);
	pos+=2;
	memcpy(msg.syncinfo,framdata+pos,SYN_INFO_LEN);	
	pos+=SYN_INFO_LEN;
	msg.compress=framdata[pos];
	pos+=COMPRESS_LEN;
	memcpy(msg.type,framdata+pos,MSG_TYPE_LEN);
	pos+=MSG_TYPE_LEN;
	msg.ver=framdata[pos];
	pos+=VERSION_LEN;
	memcpy(msg.posid,framdata+pos,POS_ID_LEN);
	pos+=POS_ID_LEN;
	
	int datalen=framesz-2-SYN_INFO_LEN-COMPRESS_LEN-MSG_TYPE_LEN-VERSION_LEN-POS_ID_LEN-CRC_LEN;
	memcpy(msg.data,framdata+pos,datalen);
	pos+=datalen;
	memcpy(msg.crc,framdata+pos,CRC_LEN);
	

	/*计算该包的crc是否正确*/
	int ret;
	unsigned char crc[2];
	ret=get_msg_crc16(&msg,datalen,crc);
	if(crc[0]!=msg.crc[0]||crc[1]!=msg.crc[1])
	{
		printf("in func %s crc err\n",__func__);
		return -2;
	}

	/*类型匹配和同步匹配*/
	if(mission->missinfo.type!=MISS_SEND_A2)
		{
			printf("in func %s ,mission not match the back data\n",__func__);
			return -3;
		}
	
	if(msg.type[0]!=0x30 || msg.type[1]!=0x82)
		{
			printf("in func %s ,back data not match the mission\n",__func__);
			return -3;
		}

	if(msg.syncinfo[0]!=mission->missinfo.data[2] || msg.syncinfo[1]!=mission->missinfo.data[3])
		{
			printf("in func %s ,SYNC INFO NOT MATHC \n",__func__);
			return -4;
		}
	memcpy(&backinfo,msg.data,sizeof(backinfo));

	DebugPrintf("A2 back: respond=%02x%02x%02x \n",backinfo.respond[0],backinfo.respond[1],backinfo.respond[2]);

	memcpy(respond->respond,backinfo.respond,sizeof(backinfo.respond));

	DebugPrintf("respond code = %02x %02x  %02x\n",backinfo.respond[0],backinfo.respond[1],backinfo.respond[2]);
	
	return 0;
	
}


#define FIX_DESCRIPE_PART

/*************************************************固件描述部分 ****************************************/

int create_request_update_firm_info_mission(mission_info * out)
{

	int mesglen,datalen,pos;
	char asc_buff[64];
	message mesg={0};


	printf("--- in func %s ---\n",__func__);
	if(out==NULL)
		return -1;
		
	datalen=0;
	mesglen=datalen+SYN_INFO_LEN+COMPRESS_LEN+MSG_TYPE_LEN+VERSION_LEN+POS_ID_LEN+CRC_LEN;	//不包括包长字段的长度
#if 0
	memset(asc_buff,0,sizeof(asc_buff));	
	sprintf(asc_buff,"%04d",mesglen);
	asc_to_bcd(mesg.len,asc_buff,2*sizeof(mesg.len));	//注意 包长可能不一定是bcd码
#else
	ShortUnon shortdata;
	shortdata.i=mesglen;
	//memcpy(mesg.len,shortdata.intbuf,2);
	mesg.len[0]=shortdata.intbuf[1];
	mesg.len[1]=shortdata.intbuf[0];
#endif
	
	/*同步信息*/
	mesg.syncinfo[0]=(SEND_REQUEST_FIX_SYN>>8) & 0x00ff;
	mesg.syncinfo[1]=(SEND_REQUEST_FIX_SYN & 0x00ff);

	mesg.compress=0x0;

	mesg.type[0]=0x30;
	mesg.type[1]=0x04;
	
	mesg.ver=0x0;
	
	memcpy(mesg.posid,devinfo->posid,sizeof(mesg.posid));
	memset(mesg.data,0,sizeof(mesg.data));
	


	/*CRC16 计算*/
	int ret;
	char crc[2];
	ret=get_msg_crc16(&mesg,datalen,crc);
	if(ret==0)
		{
		mesg.crc[0]=crc[0];
		mesg.crc[1]=crc[1];
		}
	else
		return -1;

	
	/*将消息内容填入任务包*/
	out->type=MISS_SEND_A2;
	out->datalen=mesglen+2; 	//+2 是包含了包长字段
	out->status=0;
	out->priority=0;			//优先级一般都是普通
	
	pos=0;
	memcpy(out->data+pos,mesg.len,2);
	pos+=2;
	memcpy(out->data+pos,mesg.syncinfo,SYN_INFO_LEN);
	pos+=SYN_INFO_LEN;
	out->data[pos]=mesg.compress;
	pos+=COMPRESS_LEN;
	memcpy(out->data+pos,mesg.type,MSG_TYPE_LEN);
	pos+=MSG_TYPE_LEN;
	out->data[pos]=mesg.ver;
	pos+=VERSION_LEN;
	memcpy(out->data+pos,mesg.posid,POS_ID_LEN);
	pos+=POS_ID_LEN;
	memcpy(out->data+pos,mesg.data,datalen);
	pos+=datalen;
	memcpy(out->data+pos,mesg.crc,CRC_LEN);

	return 0;
}




int update_firm_info_back_process(mission_list * mission,char * framdata,int framesz,updatefirm_back_info * out)
{
	message msg;
	updatefirm_back_info backinfo;

	printf("-- in func %s -- \n",__func__);

	if(framdata==NULL || mission==NULL || out==NULL)
		{
			printf("in func %s paramiter is NULL \n",__func__);
			return -1;
		}
	
	int pos=0;
	memcpy(msg.len,framdata+pos,2);
	pos+=2;
	memcpy(msg.syncinfo,framdata+pos,SYN_INFO_LEN);	
	pos+=SYN_INFO_LEN;
	msg.compress=framdata[pos];
	pos+=COMPRESS_LEN;
	memcpy(msg.type,framdata+pos,MSG_TYPE_LEN);
	pos+=MSG_TYPE_LEN;
	msg.ver=framdata[pos];
	pos+=VERSION_LEN;
	memcpy(msg.posid,framdata+pos,POS_ID_LEN);
	pos+=POS_ID_LEN;
	
	int datalen=framesz-2-SYN_INFO_LEN-COMPRESS_LEN-MSG_TYPE_LEN-VERSION_LEN-POS_ID_LEN-CRC_LEN;
	memcpy(msg.data,framdata+pos,datalen);
	pos+=datalen;
	memcpy(msg.crc,framdata+pos,CRC_LEN);

	/*计算该包的crc是否正确*/
	int ret;
	unsigned char crc[2];
	ret=get_msg_crc16(&msg,datalen,crc);
	if(crc[0]!=msg.crc[0]||crc[1]!=msg.crc[1])
	{
		printf("in func %s crc err\n",__func__);
		return -2;
	}

	/*类型匹配和同步匹配*/
	if(mission->missinfo.type!=MISS_REQUEST_FIX)
		{
			printf("in func %s ,mission not match the back data\n",__func__);
			return -3;
		}
	
	if(msg.type[0]!=0x30 || msg.type[1]!=0x84)
		{
			printf("in func %s ,back data not match the mission\n",__func__);
			return -3;
		}

	if(msg.syncinfo[0]!=mission->missinfo.data[2] || msg.syncinfo[1]!=mission->missinfo.data[3])
		{
			printf("in func %s ,SYNC INFO NOT MATHC \n",__func__);
			return -4;
		}
	memcpy(&backinfo,msg.data,sizeof(backinfo));	
	memcpy(out,&backinfo,sizeof(backinfo));
	
	DebugPrintf("fix version=%02x%02x\n",backinfo.version[0],backinfo.version[1]);
	DebugPrintf("fix size =0x%02x%02x%02x%02x\n",backinfo.filesz[0],backinfo.filesz[1],backinfo.filesz[2],backinfo.filesz[3]);
	LongUnon ldata;
	memcpy(ldata.longbuf,backinfo.filesz,4);
	DebugPrintf("fix size = %d \n",ldata.i);
	return 0;
	
}



#define DOWNLOAD_FIX_PART
/********************************** 下载固件部分 ********************************/

download_process_info * dwnPrcInfo=NULL;		
int init_download_fix(updatefirm_back_info info)
{

	DebugPrintf("----in func %s ---\n",__func__);
		if(dwnPrcInfo!=NULL)
			free(dwnPrcInfo);

		dwnPrcInfo=(download_process_info *)malloc(sizeof(download_process_info));
		if(dwnPrcInfo==NULL)
		{
			fprintf(stderr, "Failed to malloc memory, errno:%u, reason:%s\n",errno, strerror(errno));
	    	printf(" malloc err for new space \n");
			return -1;	
		}

		dwnPrcInfo->version[0]=info.version[0];
		dwnPrcInfo->version[1]=info.version[1];

		LongUnon fsz;
		int fd,ret;
		firmhead filehead;
		char ascbuff[16]={0};
		
		memcpy(fsz.longbuf,info.filesz,4);
		dwnPrcInfo->origisz=fsz.i;
		
		
		sprintf(dwnPrcInfo->tmppath,"%s",TMEP_FIX_FILE_PREFIX);
		sprintf(ascbuff,".%02x",dwnPrcInfo->version[0]); //0x1002 表示V1.0.02
		ascbuff[4]='\0';
		ascbuff[3]=ascbuff[2];
		ascbuff[2]='.';
		sprintf(dwnPrcInfo->destpath,"%s%s.%02x",FIX_FILE_PREFIX,ascbuff,dwnPrcInfo->version[1]);
		
		if(access(dwnPrcInfo->tmppath,0)==0)
		{
			fd=open(dwnPrcInfo->tmppath,O_RDWR);	
			if(fd<0)
			{
				printf("in func %s , can not creat %s \n",__func__,dwnPrcInfo->tmppath);
				return -1;
			}

			memset(&filehead,0,sizeof(filehead));
			ret=read(fd,&filehead,sizeof(filehead));
			if(ret<0)
				{
					printf("in func %s , can not read %s \n",__func__,dwnPrcInfo->tmppath);
					close(fd);
					return -1;
				}
			close(fd);
			
			if(filehead.ver[0]==dwnPrcInfo->version[0] && filehead.ver[1]==dwnPrcInfo->version[1] && \
				filehead.currsz==filehead.offset && filehead.origisz==dwnPrcInfo->origisz && filehead.headsz==sizeof(firmhead))
				{
					;
				}
			else{
					fd=open(dwnPrcInfo->tmppath,O_RDWR|O_TRUNC);	
					if(fd<0)
						{
							printf("in func %s , can not creat %s \n",__func__,dwnPrcInfo->tmppath);
							return -1;
						}
					memcpy(filehead.ver,dwnPrcInfo->version,2);
					filehead.origisz=dwnPrcInfo->origisz;
					filehead.offset=0;
					filehead.currsz=0;
					filehead.headsz=sizeof(firmhead);
					ret=write(fd,&filehead,sizeof(firmhead));
					if(ret<0)
						{
							printf("in func %s , can not write %s \n",__func__,dwnPrcInfo->tmppath);
							close(fd);
							return -1;
						}
					close(fd);
				}
				
		}
		else{
			fd=open(dwnPrcInfo->tmppath,O_RDWR|O_CREAT);	
			if(fd<0)
				{
					printf("in func %s , can not creat %s \n",__func__,dwnPrcInfo->tmppath);
					return -1;
				}
			memcpy(filehead.ver,dwnPrcInfo->version,2);
			filehead.origisz=dwnPrcInfo->origisz;
			filehead.offset=0;
			filehead.currsz=0;
			filehead.headsz=sizeof(firmhead);
			ret=write(fd,&filehead,sizeof(firmhead));
			if(ret<0)
				{
					printf("in func %s , can not write %s \n",__func__,dwnPrcInfo->tmppath);
					close(fd);
					return -1;
				}
			close(fd);
		}
		
		dwnPrcInfo->currsz=filehead.currsz;
		dwnPrcInfo->offset=filehead.offset;

		DebugPrintf("dwnfixpro->tmppath=%s ,destpath=%s \n",dwnPrcInfo->tmppath,dwnPrcInfo->destpath);
		DebugPrintf("dwnfixpro->version:%02x%02x\n",dwnPrcInfo->version[0],dwnPrcInfo->version[1]);
		DebugPrintf("dwnfixpro->destlen=%d currsize=%d offset=%d",dwnPrcInfo->origisz,dwnPrcInfo->currsz,dwnPrcInfo->offset);
		return 0;
}


int create_download_firm_mission(download_process_info * processinfo,mission_info * out)
{
	
	int mesglen,datalen,pos;
	char asc_buff[64];
	message mesg={0};
		
	if(processinfo==NULL)
		return -1;
		
	/*
		
	*/
	DebugPrintf("---- in func %s ------ \n",__func__);
	if(out==NULL)
		return -1;
	
	down_fir_info dowinfo;
	LongUnon longfig;
	ShortUnon shortfig;
		
	longfig.i=processinfo->offset;
	shortfig.i=processinfo->origisz-processinfo->currsz;
	memcpy(dowinfo.length,shortfig.intbuf,2);			//	这里格式要注意一下
	memcpy(dowinfo.offset,longfig.longbuf,4);			//

	/*构建消息包*/
	
	datalen=sizeof(down_fir_info);
	mesglen=datalen+SYN_INFO_LEN+COMPRESS_LEN+MSG_TYPE_LEN+VERSION_LEN+POS_ID_LEN+CRC_LEN;	//不包括包长字段的长度
#if 0
	memset(asc_buff,0,sizeof(asc_buff));	
	sprintf(asc_buff,"%04d",mesglen);
	asc_to_bcd(mesg.len,asc_buff,2*sizeof(mesg.len));	//注意 包长可能不一定是bcd码
#else
	ShortUnon shortdata;
	shortdata.i=mesglen;
	//memcpy(mesg.len,shortdata.intbuf,2);
	mesg.len[0]=shortdata.intbuf[1];
	mesg.len[1]=shortdata.intbuf[0];
#endif
	
	/*同步信息*/
	mesg.syncinfo[0]=(SEND_DOWNLOAD_FIX_SYN >>8) & 0x00ff;
	mesg.syncinfo[1]=(SEND_DOWNLOAD_FIX_SYN & 0x00ff);

	mesg.compress=0x0;

	mesg.type[0]=0x30;
	mesg.type[1]=0x05;
	
	mesg.ver=0x0;
	
	memcpy(mesg.posid,devinfo->posid,sizeof(mesg.posid));
	memcpy(mesg.data,&dowinfo,datalen);

	/*CRC16 计算*/
	int ret;
	char crc[2];
	ret=get_msg_crc16(&mesg,datalen,crc);
	if(ret==0)
	{
		mesg.crc[0]=crc[0];
		mesg.crc[1]=crc[1];
	}
	else
		return -1;
	
		/*将消息内容填入任务包*/
	out->type=MISS_DOWNLOAD_FIX;
	out->datalen=mesglen+2; 	//+2 是包含了包长字段
	out->status=0;
	out->priority=0;			//优先级一般都是普通
	
	pos=0;
	memcpy(out->data+pos,mesg.len,2);
	pos+=2;
	memcpy(out->data+pos,mesg.syncinfo,SYN_INFO_LEN);
	pos+=SYN_INFO_LEN;
	out->data[pos]=mesg.compress;
	pos+=COMPRESS_LEN;
	memcpy(out->data+pos,mesg.type,MSG_TYPE_LEN);
	pos+=MSG_TYPE_LEN;
	out->data[pos]=mesg.ver;
	pos+=VERSION_LEN;
	memcpy(out->data+pos,mesg.posid,POS_ID_LEN);
	pos+=POS_ID_LEN;
	memcpy(out->data+pos,mesg.data,datalen);
	pos+=datalen;
	memcpy(out->data+pos,mesg.crc,CRC_LEN);

	return 0;
}



/*

	0:下载成功；
	-1:网络数据异常或过程信息错误
	-2:临时文件异常

*/

int down_firm_back_process(mission_list * mission,char * framdata,int framesz)
{
	message msg;
	down_fir_back_info backinfo;
	

	if(framdata==NULL || mission==NULL )
		{
			printf("in func %s paramiter is NULL \n",__func__);
			return -1;
		}

	
	int pos=0;
	memcpy(msg.len,framdata+pos,2);
	pos+=2;
	memcpy(msg.syncinfo,framdata+pos,SYN_INFO_LEN);	
	pos+=SYN_INFO_LEN;
	msg.compress=framdata[pos];
	pos+=COMPRESS_LEN;
	memcpy(msg.type,framdata+pos,MSG_TYPE_LEN);
	pos+=MSG_TYPE_LEN;
	msg.ver=framdata[pos];
	pos+=VERSION_LEN;
	memcpy(msg.posid,framdata+pos,POS_ID_LEN);
	pos+=POS_ID_LEN;
	
	int datalen=framesz-2-SYN_INFO_LEN-COMPRESS_LEN-MSG_TYPE_LEN-VERSION_LEN-POS_ID_LEN-CRC_LEN;
	memcpy(msg.data,framdata+pos,datalen);
	pos+=datalen;
	memcpy(msg.crc,framdata+pos,CRC_LEN);
	

	/*计算该包的crc是否正确*/
	int ret;
	unsigned char crc[2];
	ret=get_msg_crc16(&msg,datalen,crc);
	if(crc[0]!=msg.crc[0]||crc[1]!=msg.crc[1])
	{
		printf("in func %s crc err\n",__func__);
		return -1;
	}


	/*类型匹配和同步匹配*/
	if(mission->missinfo.type!=MISS_DOWNLOAD_FIX)
		{
			printf("in func %s ,mission not match the back data\n",__func__);
			return -1;
		}
	
	if(msg.type[0]!=0x30 || msg.type[1]!=0x85)
		{
			printf("in func %s ,back data not match the mission\n",__func__);
			return -1;
		}

	if(msg.syncinfo[0]!=mission->missinfo.data[2] || msg.syncinfo[1]!=mission->missinfo.data[3])
		{
			printf("in func %s ,SYNC INFO NOT MATHC \n",__func__);
			return -1;
		}

	/* 将数据内容更新到文件中 */
	memcpy(backinfo.offset,msg.data+0,4);
	memcpy(backinfo.length,msg.data+4,2);
	backinfo.fdata=&msg.data[6];
	
	LongUnon offset;
	ShortUnon len;
	int fd;
	firmhead filehead;
	char cmd[128];
	
	memcpy(offset.longbuf,backinfo.offset,4);
	memcpy(len.intbuf,backinfo.length,2);

	if(dwnPrcInfo!=NULL && offset.i==dwnPrcInfo->offset && dwnPrcInfo->offset < dwnPrcInfo->origisz)
	{
		fd=open(dwnPrcInfo->tmppath,O_RDWR);
		if(fd<0){
			printf("in func %s can not open %s \n",__func__,dwnPrcInfo->tmppath);	
			return -2;
		}
		ret=read(fd,&filehead,sizeof(filehead));
		if(ret<0){
			printf("in func %s can not read %s \n",__func__,dwnPrcInfo->tmppath);	
			return -2;
		}
		
		/*检查文件头*/
		if(filehead.currsz!=filehead.offset || filehead.offset!=dwnPrcInfo->offset)
			{
				printf(" in func %s ,%s is bad\n",__func__,dwnPrcInfo->tmppath);
				close(fd);	
				return -2;		//要删除文件
			}
		else
		{
				lseek(fd,offset.i+filehead.headsz,SEEK_SET);
				ret=write(fd,msg.data+6,len.i);
				if(ret==len.i)
				{
					dwnPrcInfo->offset+=len.i;
					dwnPrcInfo->currsz=dwnPrcInfo->offset;
					filehead.offset+=len.i;
					filehead.currsz+=len.i;
					lseek(fd,0,SEEK_SET);
					ret=write(fd,&filehead,sizeof(filehead));
					close(fd);
					return 0;
					
				}
				else{
					printf(" in func %s , write %s err\n",__func__,dwnPrcInfo->tmppath);
					close(fd);	
					return -2;
				}
		}
			
	}
	else{
		printf(" in func %s dwon load process info err\n",__func__);
		return -1;
	}
	
	return 0;
}


#define DOWNLOAD_MCHNT_PART
/******************************下载商户ID部***************************************/

int create_request_mchntid_mission(mission_info * out)
{

	int mesglen,datalen,pos;
	char asc_buff[64];
	message mesg={0};


	if(out==NULL)
		return -1;
		
	datalen=0;
	mesglen=datalen+SYN_INFO_LEN+COMPRESS_LEN+MSG_TYPE_LEN+VERSION_LEN+POS_ID_LEN+CRC_LEN;	//不包括包长字段的长度
#if 0
	memset(asc_buff,0,sizeof(asc_buff));	
	sprintf(asc_buff,"%04d",mesglen);
	asc_to_bcd(mesg.len,asc_buff,2*sizeof(mesg.len));	//注意 包长可能不一定是bcd码
#else
	ShortUnon shortdata;
	shortdata.i=mesglen;
	//memcpy(mesg.len,shortdata.intbuf,2);
	mesg.len[0]=shortdata.intbuf[1];
	mesg.len[1]=shortdata.intbuf[0];
#endif
	
	/*同步信息*/
	mesg.syncinfo[0]=(SEND_REQUEST_MCH_SYN>>8) & 0x00ff;
	mesg.syncinfo[1]=(SEND_REQUEST_MCH_SYN & 0x00ff);

	mesg.compress=0x0;

	mesg.type[0]=0x30;
	mesg.type[1]=0x07;
	
	mesg.ver=0x0;
	
	memcpy(mesg.posid,devinfo->posid,sizeof(mesg.posid));
	memset(mesg.data,0,sizeof(mesg.data));
	


	/*CRC16 计算*/
	int ret;
	char crc[2];
	ret=get_msg_crc16(&mesg,datalen,crc);
	if(ret==0)
		{
		mesg.crc[0]=crc[0];
		mesg.crc[1]=crc[1];
		}
	else
		return -1;

	
	/*将消息内容填入任务包*/
	out->type=MISS_SEND_A2;
	out->datalen=mesglen+2; 	//+2 是包含了包长字段
	out->status=0;
	out->priority=0;			//优先级一般都是普通
	
	pos=0;
	memcpy(out->data+pos,mesg.len,2);
	pos+=2;
	memcpy(out->data+pos,mesg.syncinfo,SYN_INFO_LEN);
	pos+=SYN_INFO_LEN;
	out->data[pos]=mesg.compress;
	pos+=COMPRESS_LEN;
	memcpy(out->data+pos,mesg.type,MSG_TYPE_LEN);
	pos+=MSG_TYPE_LEN;
	out->data[pos]=mesg.ver;
	pos+=VERSION_LEN;
	memcpy(out->data+pos,mesg.posid,POS_ID_LEN);
	pos+=POS_ID_LEN;
	memcpy(out->data+pos,mesg.data,datalen);
	pos+=datalen;
	memcpy(out->data+pos,mesg.crc,CRC_LEN);

	return 0;
}




int request_Mchn_back_process(mission_list * mission,char * framdata,int framesz)
{

	message msg;
	request_Merchant_back_info backinfo;

	if(framdata==NULL || mission==NULL)
		{
			printf("in func %s paramiter is NULL \n",__func__);
			return -1;
		}
	
	int pos=0;
	memcpy(msg.len,framdata+pos,2);
	pos+=2;
	memcpy(msg.syncinfo,framdata+pos,SYN_INFO_LEN); 
	pos+=SYN_INFO_LEN;
	msg.compress=framdata[pos];
	pos+=COMPRESS_LEN;
	memcpy(msg.type,framdata+pos,MSG_TYPE_LEN);
	pos+=MSG_TYPE_LEN;
	msg.ver=framdata[pos];
	pos+=VERSION_LEN;
	memcpy(msg.posid,framdata+pos,POS_ID_LEN);
	pos+=POS_ID_LEN;
	
	int datalen=framesz-2-SYN_INFO_LEN-COMPRESS_LEN-MSG_TYPE_LEN-VERSION_LEN-POS_ID_LEN-CRC_LEN;
	memcpy(msg.data,framdata+pos,datalen);
	pos+=datalen;
	memcpy(msg.crc,framdata+pos,CRC_LEN);
	
	/*计算该包的crc是否正确*/
	int ret;
	unsigned char crc[2];
	ret=get_msg_crc16(&msg,datalen,crc);
	if(crc[0]!=msg.crc[0]||crc[1]!=msg.crc[1])
	{
		printf("in func %s crc err\n",__func__);
		return -2;
	}

	/*类型匹配和同步匹配*/
	if(mission->missinfo.type!=MISS_REQUEST_MCH)
		{
			printf("in func %s ,mission not match the back data\n",__func__);
			return -3;
		}
	
	if(msg.type[0]!=0x30 || msg.type[1]!=0x87)
		{
			printf("in func %s ,back data not match the mission\n",__func__);
			return -3;
		}

	if(msg.syncinfo[0]!=mission->missinfo.data[2] || msg.syncinfo[1]!=mission->missinfo.data[3])
		{
			printf("in func %s ,SYNC INFO NOT MATHC \n",__func__);
			return -4;
		}
	memcpy(&backinfo,msg.data,sizeof(backinfo));	

	
	/*显示内容*/

#if CLIENT_DEBUG
	DebugPrintf("---0x3087 back info:\n");
	DebugPrintf("param Ver:%02x %02x \n",backinfo.paraVer[0],backinfo.paraVer[1]);
    DebugPrintf("agentId  :%02x %02x %02x %02x \n",backinfo.agentId[0],backinfo.agentId[1],backinfo.agentId[2],backinfo.agentId[3]);
	DebugPrintf("agentSpId:%02x \n",backinfo.agentSpId);
	DebugPrintf("Mchntid-1:%02x %02x %02x %02x %02x %02x",backinfo.mchntId1[0],backinfo.mchntId1[1],backinfo.mchntId1[2],\
		backinfo.mchntId1[3],backinfo.mchntId1[4],backinfo.mchntId1[5]);
	DebugPrintf("MchntSpid-1:%02x %02x",backinfo.mchntSpId1[0],backinfo.mchntSpId1[1]);
	DebugPrintf("Mchntid-2:%02x %02x %02x %02x %02x %02x",backinfo.mchntId2[0],backinfo.mchntId2[1],backinfo.mchntId2[2],\
			backinfo.mchntId2[3],backinfo.mchntId2[4],backinfo.mchntId2[5]);
	DebugPrintf("MchntSpid-2:%02x %02x",backinfo.mchntSpId2[0],backinfo.mchntSpId2[1]);
	DebugPrintf("mchntname:%s \n",backinfo.mchniName);
	DebugPrintf("storeId:%02x %02x %02x %02x %02x %02x",backinfo.storeId[0],backinfo.storeId[1],backinfo.storeId[2],\
			backinfo.storeId[3],backinfo.storeId[4],backinfo.storeId[5]);
	DebugPrintf("storepid:%02x %02x \n",backinfo.storeSpId[0],backinfo.storeSpId[1]);
	DebugPrintf("store name:%s \n",backinfo.storeNmae);
	DebugPrintf("off season month:%s \n",backinfo.offSeasonMon);
	DebugPrintf("off season price:%02x%02x%02x%02x \n",backinfo.offSeasonPri[0],backinfo.offSeasonPri[1],backinfo.offSeasonPri[2],backinfo.offSeasonPri[3]);

	
	DebugPrintf("on season month:%s \n",backinfo.onSeasonMon);
	DebugPrintf("on season price:%02x%02x%02x%02x \n",backinfo.onSeasonPri[0],backinfo.onSeasonPri[1],backinfo.onSeasonPri[2],backinfo.onSeasonPri[3]);
#endif
	
	return 0;

	
}



#define REQUEST_FILE_DESCRIPE_PART

/**************************请求文件描述信息部分*******************************/

#define FILE_SUPPORT_MAX 	11

downfileprocess dwnFilPro[FILE_SUPPORT_MAX]={
		{.keyword=FIL1_BLACK},
		{.keyword=FIL2_INCR_BLACK},
		{.keyword=FIL3_CARD_TYPE},
		{.keyword=FIL4_LINE_PARA},
		{.keyword=FIL5_QR_INCR_BLACK},
		{.keyword=FIL6_HT_INCR_BLACK},
		{.keyword=FIL7_HT_BLACK},
		{.keyword=FIL8_PRICE},
		{.keyword=FIL9_MILS_PARA},
		{.keyword=FIL10_TWO_RELE_PARA},
		{.keyword=FIL11_HT_WHITE},
};


int init_dwonfie_process(file_desinfo_back * backinfo)
{
	LongUnon longdata;
	int fd;	
	if(backinfo==NULL)
		return -1;

	int i;
	unsigned char cmd[128];
	
	DebugPrintf("--- in func %s -----\n",__func__);
	for(i=0;i<FILE_SUPPORT_MAX;i++)
		{
			DebugPrintf("bcakinfo->filename=%s  , dwnfilpro[%d].keywor=%s \n",backinfo->filename,\
				i,dwnFilPro[i].keyword);
		
			if(strncmp(backinfo->filename,dwnFilPro[i].keyword,strlen(backinfo->filename))==0)
				{
					//sprintf(dwnFilPro[i].tmpname,"/mnt/record/%s.tmp",dwnFilPro[i].keyword+3);
					//sprintf(dwnFilPro[i].destname,"/mnt/record/%s.%d",dwnFilPro[i].keyword+3,dwnFilPro[i].ver);
					
					sprintf(dwnFilPro[i].tmpname,"./record/%s.tmp",dwnFilPro[i].keyword+3);
					memcpy(dwnFilPro[i].ver,backinfo->version,4);	
					//sprintf(dwnFilPro[i].destname,"./record/%s.%02x%02x",dwnFilPro[i].keyword+3,dwnFilPro[i].ver[0],dwnFilPro[i].ver[1]);
					sprintf(dwnFilPro[i].destname,"./record/%s",dwnFilPro[i].keyword+3);

					
					
					fd=open(dwnFilPro[i].tmpname,O_RDWR|O_TRUNC|O_CREAT,0666);
					
					if(fd<0){
						printf(" in func %s ,can not create %s \n",__func__,dwnFilPro[i].tmpname);	
						return -1;
					}
					else{
						printf("--- in func %s , open %s suncces \n",__func__,dwnFilPro[i].tmpname);
						system(" ls -l ./record");
					}
					close(fd);
					//sprintf(cmd,"chmod 777 %s ",dwnFilPro[i].tmpname);
					//system(cmd);
					/*这里无需初始化文件头，文件头内容也是又服务端传送，只需要下载完毕后再检查文件头即可*/	
					
					memcpy(longdata.longbuf,backinfo->filesz,4);
					//longdata.longbuf[0]=backinfo->filesz[3];
					//longdata.longbuf[1]=backinfo->filesz[2];
					//longdata.longbuf[2]=backinfo->filesz[1];
					//longdata.longbuf[3]=backinfo->filesz[0];
					
					dwnFilPro[i].destlen=longdata.i;
					memcpy(dwnFilPro[i].filecrc,backinfo->fileCRC,2);
					dwnFilPro[i].offset=0;			
					dwnFilPro[i].status=1;
					DebugPrintf("dwnfilpro.keywor=%s\n",dwnFilPro[i].keyword);
					DebugPrintf("dwnfilpro.tmpname=%s \n",dwnFilPro[i].tmpname);
					DebugPrintf("dwnfilpro.destlen=%d \n",dwnFilPro[i].destlen);
					DebugPrintf("dwnfilpro.offset=%d \n",dwnFilPro[i].offset);
					DebugPrintf("dwnfilpro.fileCRC=%02x%02x\n",dwnFilPro[i].filecrc[0],dwnFilPro[i].filecrc[1]);
					
				}
			else
				dwnFilPro[i].status=0;

		}

		return 0;
}



int create_request_file_desinfo(int fileindex,mission_info * out)
{
	int mesglen,datalen,pos;
	char asc_buff[64];
	message mesg={0};
	unsigned char filename[40]={0};


	if(out==NULL)
		return -1;

	DebugPrintf("---- in func %s , fileindex = %d ----\n",__func__,fileindex);

	switch(fileindex)
	{
		case 1:
			memcpy(filename,FIL1_BLACK,strlen(FIL1_BLACK));
			break;
		case 2:
			memcpy(filename,FIL2_INCR_BLACK,strlen(FIL2_INCR_BLACK));
			break;
		case 3:
			memcpy(filename,FIL3_CARD_TYPE,strlen(FIL3_CARD_TYPE));
			break;
		case 4:
			memcpy(filename,FIL4_LINE_PARA,strlen(FIL4_LINE_PARA));
			break;
		case 5:
			memcpy(filename,FIL5_QR_INCR_BLACK,strlen(FIL5_QR_INCR_BLACK));
			break;
		case 6:
			memcpy(filename,FIL6_HT_INCR_BLACK,strlen(FIL6_HT_INCR_BLACK));
			break;
		case 7:
			memcpy(filename,FIL7_HT_BLACK,strlen(FIL7_HT_BLACK));
			break;
		case 8:
			memcpy(filename,FIL8_PRICE,strlen(FIL8_PRICE));
			break;
		case 9:
			memcpy(filename,FIL9_MILS_PARA,strlen(FIL9_MILS_PARA));
			break;
		case 10:
			memcpy(filename,FIL10_TWO_RELE_PARA,strlen(FIL10_TWO_RELE_PARA));
			break;	
		case 11:
			memcpy(filename,FIL11_HT_WHITE,strlen(FIL11_HT_WHITE));
			break;
		default:
			return -1;
	}


	datalen=sizeof(filename);
	mesglen=datalen+SYN_INFO_LEN+COMPRESS_LEN+MSG_TYPE_LEN+VERSION_LEN+POS_ID_LEN+CRC_LEN;	//不包括包长字段的长度
#if 0
	memset(asc_buff,0,sizeof(asc_buff));	
	sprintf(asc_buff,"%04d",mesglen);
	asc_to_bcd(mesg.len,asc_buff,2*sizeof(mesg.len));	//注意 包长可能不一定是bcd码
#else
	ShortUnon shortdata;
	shortdata.i=mesglen;
	//memcpy(mesg.len,shortdata.intbuf,2);
	mesg.len[0]=shortdata.intbuf[1];
	mesg.len[1]=shortdata.intbuf[0];
#endif
	

	/*同步信息*/
	mesg.syncinfo[0]=(SEND_REQUEST_FILE_DES_SYN>>8) & 0x00ff;
	mesg.syncinfo[1]=(SEND_REQUEST_FILE_DES_SYN& 0x00ff);

	mesg.compress=0x0;

	mesg.type[0]=0x30;
	mesg.type[1]=0x0A;
	
	mesg.ver=0x0;
	
	memcpy(mesg.posid,devinfo->posid,sizeof(mesg.posid));
	memcpy(mesg.data,filename,datalen);

	/*CRC16 计算*/
	int ret;
	char crc[2];
	ret=get_msg_crc16(&mesg,datalen,crc);
	if(ret==0)
		{
		mesg.crc[0]=crc[0];
		mesg.crc[1]=crc[1];
		}
	else
		return -1;
	

	/*将消息内容填入任务包*/
	out->type=MISS_REQUEST_FILE_DESINFO;
	out->datalen=mesglen+2;		//+2 是包含了包长字段
	out->status=0;
	out->priority=0;
	
	pos=0;
	memcpy(out->data+pos,mesg.len,2);
	pos+=2;
	memcpy(out->data+pos,mesg.syncinfo,SYN_INFO_LEN);
	pos+=SYN_INFO_LEN;
	out->data[pos]=mesg.compress;
	pos+=COMPRESS_LEN;
	memcpy(out->data+pos,mesg.type,MSG_TYPE_LEN);
	pos+=MSG_TYPE_LEN;
	out->data[pos]=mesg.ver;
	pos+=VERSION_LEN;
	memcpy(out->data+pos,mesg.posid,POS_ID_LEN);
	pos+=POS_ID_LEN;
	memcpy(out->data+pos,mesg.data,datalen);
	pos+=datalen;
	memcpy(out->data+pos,mesg.crc,CRC_LEN);

	return 0;
}

/*请求返回处理*/
int request_file_desinfo_process(mission_list * mission,char * framdata,int framesz,file_desinfo_back * out)
{

	message msg;
	file_desinfo_back backinfo;

	DebugPrintf(" ----in fun %s ,------\n",__func__);
	if(framdata==NULL || mission==NULL || out==NULL)
		{
			printf("in func %s paramiter is NULL \n",__func__);
			return -1;
		}

	
	int pos=0;
	memcpy(msg.len,framdata+pos,2);
	pos+=2;
	memcpy(msg.syncinfo,framdata+pos,SYN_INFO_LEN); 
	pos+=SYN_INFO_LEN;
	msg.compress=framdata[pos];
	pos+=COMPRESS_LEN;
	memcpy(msg.type,framdata+pos,MSG_TYPE_LEN);
	pos+=MSG_TYPE_LEN;
	msg.ver=framdata[pos];
	pos+=VERSION_LEN;
	memcpy(msg.posid,framdata+pos,POS_ID_LEN);
	pos+=POS_ID_LEN;
	
	int datalen=framesz-2-SYN_INFO_LEN-COMPRESS_LEN-MSG_TYPE_LEN-VERSION_LEN-POS_ID_LEN-CRC_LEN;
	memcpy(msg.data,framdata+pos,datalen);
	pos+=datalen;
	memcpy(msg.crc,framdata+pos,CRC_LEN);
	
	/*计算该包的crc是否正确*/
	int ret;
	unsigned char crc[2];
	ret=get_msg_crc16(&msg,datalen,crc);
	if(crc[0]!=msg.crc[0]||crc[1]!=msg.crc[1])
	{
		printf("in func %s crc err\n",__func__);
		return -2;
	}


	/*类型匹配和同步匹配*/
	if(mission->missinfo.type!=MISS_REQUEST_FILE_DESINFO)
		{
			printf("in func %s ,mission not match the back data\n",__func__);
			return -3;
		}
	
	if(msg.type[0]!=0x30 || msg.type[1]!=0x8A)
		{
			printf("in func %s ,back data not match the mission\n",__func__);
			return -3;
		}

	if(msg.syncinfo[0]!=mission->missinfo.data[2] || msg.syncinfo[1]!=mission->missinfo.data[3])
		{
			printf("in func %s ,SYNC INFO NOT MATHC \n",__func__);
			return -4;
		}
	memcpy(&backinfo,msg.data,sizeof(backinfo));	


	#if CLIENT_DEBUG
		DebugPrintf("--- 0x308A back info:\n");
		DebugPrintf("file name:%s \n",backinfo.filename);
		DebugPrintf("version:%02x%02x%02x%02x\n",backinfo.version[0],backinfo.version[1],backinfo.version[2],backinfo.version[3]);
		DebugPrintf("file CRC:%02x%02x\n",backinfo.fileCRC[0],backinfo.fileCRC[1]);
		DebugPrintf("file size:%02x%02x%02x%02x\n",backinfo.filesz[0],backinfo.filesz[1],backinfo.filesz[2],backinfo.filesz[3]);

		LongUnon ldata;
		memcpy(ldata.longbuf,backinfo.filesz,4);
		//ldata.longbuf[0]=backinfo.filesz[3];
		//ldata.longbuf[1]=backinfo.filesz[2];
		//ldata.longbuf[2]=backinfo.filesz[1];
		//ldata.longbuf[3]=backinfo.filesz[0];
		DebugPrintf("file size:%d\n",ldata.i);
	#endif
	memcpy(out,&backinfo,sizeof(backinfo));

	return 0;
}


#define DOWNLOAD_FILE_PART
/**********************************************下载文件部分********************************/

int create_dwonload_file_mission(mission_info * out)
{

	int mesglen,datalen,pos;
	char asc_buff[64];
	message mesg={0};
	down_file_info  dwninfo;
	int i;	
	LongUnon lgdata;
	ShortUnon shdata;
	
	if(out==NULL)
		return -1;

	/* find download which file */

	DebugPrintf("--- in func %s --- \n",__func__);
	for(i=0;i<FILE_SUPPORT_MAX;i++)
		{
			if(dwnFilPro[i].status==1)
				{
					memcpy(dwninfo.filename,dwnFilPro[i].keyword,40);
					shdata.i=dwnFilPro[i].destlen;
					memcpy(dwninfo.length,shdata.intbuf,2);
					lgdata.i=dwnFilPro[i].offset;
					memcpy(dwninfo.offset,lgdata.longbuf,4);
					DebugPrintf("dwninfo.length=%02x%02x",dwninfo.offset[0],dwninfo.offset[1]);
					DebugPrintf("dwninfo.offset=%02x%02x%02x%02x\n",dwninfo.offset[0],dwninfo.offset[1],dwninfo.offset[2],dwninfo.offset[3]);
					break;
				}
		}


	
		datalen=sizeof(down_file_info);
		mesglen=datalen+SYN_INFO_LEN+COMPRESS_LEN+MSG_TYPE_LEN+VERSION_LEN+POS_ID_LEN+CRC_LEN;	//不包括包长字段的长度
#if 0
		memset(asc_buff,0,sizeof(asc_buff));	
		sprintf(asc_buff,"%04d",mesglen);
		asc_to_bcd(mesg.len,asc_buff,2*sizeof(mesg.len));	//注意 包长可能不一定是bcd码
#else
		ShortUnon shortdata;
		shortdata.i=mesglen;
	//memcpy(mesg.len,shortdata.intbuf,2);
		mesg.len[0]=shortdata.intbuf[1];
		mesg.len[1]=shortdata.intbuf[0];

#endif
		
	
		/*同步信息*/
		mesg.syncinfo[0]=(SEND_DOWNLOAD_FILE_SYN>>8) & 0x00ff;
		mesg.syncinfo[1]=(SEND_DOWNLOAD_FILE_SYN& 0x00ff);
	
		mesg.compress=0x0;
	
		mesg.type[0]=0x30;
		mesg.type[1]=0x0B;
		
		mesg.ver=0x0;
		
		memcpy(mesg.posid,devinfo->posid,sizeof(mesg.posid));
		memcpy(mesg.data,&dwninfo,datalen);


		/*CRC16 计算*/
		int ret;
		char crc[2];
		ret=get_msg_crc16(&mesg,datalen,crc);
		if(ret==0)
			{
			mesg.crc[0]=crc[0];
			mesg.crc[1]=crc[1];
			}
		else
			return -1;

		/*将消息内容填入任务包*/
		out->type=MISS_DOWNLOAD_FILE;
		out->datalen=mesglen+2;		//+2 是包含了包长字段
		out->status=0;
		out->priority=0;
	
		
		pos=0;
		memcpy(out->data+pos,mesg.len,2);
		pos+=2;
		memcpy(out->data+pos,mesg.syncinfo,SYN_INFO_LEN);
		pos+=SYN_INFO_LEN;
		out->data[pos]=mesg.compress;
		pos+=COMPRESS_LEN;
		memcpy(out->data+pos,mesg.type,MSG_TYPE_LEN);
		pos+=MSG_TYPE_LEN;
		out->data[pos]=mesg.ver;
		pos+=VERSION_LEN;
		memcpy(out->data+pos,mesg.posid,POS_ID_LEN);
		pos+=POS_ID_LEN;
		memcpy(out->data+pos,mesg.data,datalen);
		pos+=datalen;
		memcpy(out->data+pos,mesg.crc,CRC_LEN);
		
		return 0;

}



int down_file_back_process(mission_list * mission,char * framdata,int framesz,int *index)
{
	
	message msg;
	down_file_back_info backinfo;
	
	if(framdata==NULL || mission==NULL|| index==NULL)
		{
			printf("in func %s paramiter is NULL \n",__func__);
			return -1;
		}

	printf("--- in func %s --- \n",__func__);
	int pos=0;
	memcpy(msg.len,framdata+pos,2);
	pos+=2;
	memcpy(msg.syncinfo,framdata+pos,SYN_INFO_LEN); 
	pos+=SYN_INFO_LEN;
	msg.compress=framdata[pos];
	pos+=COMPRESS_LEN;
	memcpy(msg.type,framdata+pos,MSG_TYPE_LEN);
	pos+=MSG_TYPE_LEN;
	msg.ver=framdata[pos];
	pos+=VERSION_LEN;
	memcpy(msg.posid,framdata+pos,POS_ID_LEN);
	pos+=POS_ID_LEN;

	int datalen=framesz-2-SYN_INFO_LEN-COMPRESS_LEN-MSG_TYPE_LEN-VERSION_LEN-POS_ID_LEN-CRC_LEN;
	memcpy(msg.data,framdata+pos,datalen);
	pos+=datalen;
	memcpy(msg.crc,framdata+pos,CRC_LEN);
	

	/*计算该包的crc是否正确*/
	int ret;
	unsigned char crc[2];
	ret=get_msg_crc16(&msg,datalen,crc);
	if(crc[0]!=msg.crc[0]||crc[1]!=msg.crc[1])
	{
		printf("in func %s crc err\n",__func__);
		return -1;
	}

	

	
	/*类型匹配和同步匹配*/
	if(mission->missinfo.type!=MISS_DOWNLOAD_FILE)
		{
			printf("in func %s ,mission not match the back data\n",__func__);
			return -1;
		}
	
	if(msg.type[0]!=0x30 || msg.type[1]!=0x8B)
		{
			printf("in func %s ,back data not match the mission\n",__func__);
			return -1;
		}

	if(msg.syncinfo[0]!=mission->missinfo.data[2] || msg.syncinfo[1]!=mission->missinfo.data[3])
		{
			printf("in func %s ,SYNC INFO NOT MATHC \n",__func__);
			return -1;
		}

	memcpy(backinfo.name,msg.data,40);
	memcpy(backinfo.offset,msg.data+40,4);
	memcpy(backinfo.length,msg.data+44,2);
	backinfo.filedata=&msg.data[46];

	LongUnon offset;
	ShortUnon len;
	memcpy(offset.longbuf,backinfo.offset,4);
	memcpy(len.intbuf,backinfo.length,2);
	
#if CLIENT_DEBUG

	DebugPrintf("----0x308B back info:\n");
	DebugPrintf("download file:%s\n",backinfo.name);
	DebugPrintf("offset:%d\n",offset.i);
	DebugPrintf("download lenght:%d \n",len.i);
#endif

	
	/*将数据更新到临时文件*/

	int i,fd;
	for(i=0;i<FILE_SUPPORT_MAX;i++)
	{
		if(memcmp(backinfo.name,dwnFilPro[i].keyword,strlen(backinfo.name))==0 && dwnFilPro[i].status==1)
			break;
	}

	if(i>=FILE_SUPPORT_MAX){
		printf(" in func %s dwnfilpro info err, redwnload \n",__func__);
		return -1;
	}

	if(offset.i!=dwnFilPro[i].offset)
	{
		printf("in func %s, offset not match, download offset=%d ,process offset=%d\n ",__func__,offset.i,dwnFilPro[i].offset);
		return -1;	
	}

	fd=open(dwnFilPro[i].tmpname,O_RDWR);
	if(fd<0){
		printf(" in func %s cannot open %s \n",__func__,dwnFilPro[i].tmpname);	
		return -1;
	}

	lseek(fd,dwnFilPro[i].offset,SEEK_SET);
	ret=write(fd,backinfo.filedata,len.i);
	close(fd);
	if(ret!=len.i)
		{
			printf("in func %s ,write %s err \n",__func__,dwnFilPro[i].tmpname);
			return -1;
		}
	
	dwnFilPro[i].offset+=len.i;			//更新下一次写入文件的位置
	*index=i;
	return 0;
	
}



int verify_download_file(int index , char *out)
{
	int fd;
	
	struct stat filinfo;

	DebugPrintf("-------------- in func %s --- -----------\n",__func__);

	if(access(dwnFilPro[index].tmpname,0)!=0)
		{
			printf("in func %s,%s is not exist\n",__func__,dwnFilPro[index].tmpname);
			return -1;
		}

	stat(dwnFilPro[index].tmpname,&filinfo);
	DebugPrintf("in func %s,file size = %d , dwnfilpro.destlen = %d\n",__func__,filinfo.st_size,dwnFilPro[index].destlen);
	
	if(filinfo.st_size!=dwnFilPro[index].destlen)
		{
			printf(" file size is not match \n");
			return -1;
		}
	

	
	fd=open(dwnFilPro[index].tmpname,O_RDWR);
	
	if(fd<0){
		printf("in func %s can not open %s \n",__func__,dwnFilPro[index].tmpname);
		return -1;
	}

	unsigned char crc[2],asc_buff[16];
	unsigned short crcin=0,crcout;
	int count,ret;
	unsigned char crcbuf[1024];		//一次读取256最多
	
	count=0;
	while((ret=read(fd,crcbuf,1024))>0)
	{
		//Calc_crc(crcbuf,ret,&crcout,crcin);
		//crcin=crcout;
		crcin=CRC_16_no_init(crcbuf, ret, crcin);
		count+=ret;
	}
	close(fd);

	#if 0
		sprintf(asc_buff,"%04d",crcout);
		asc_to_bcd(crc,asc_buff,4);
	#else
		ShortUnon sd;
		sd.i=crcin;
		crc[0]=sd.intbuf[1];
		crc[1]=sd.intbuf[0];
	#endif
	
	DebugPrintf("in func %s, file crc=%02x%02x  dwnfilpro.crc=%02x%02x \n",__func__,crc[0],crc[1],dwnFilPro[index].filecrc[0],dwnFilPro[index].filecrc[1]);
	DebugPrintf("in func %s, file size=%d  dwnfilpro.destsz=%d \n",__func__,count,dwnFilPro[index].destlen);

	if(count!=dwnFilPro[index].destlen)
		{
			printf(" file size is not match \n");
			return -1;
		}

	if(crc[0]!=dwnFilPro[index].filecrc[0] || crc[1]!=dwnFilPro[index].filecrc[1])
		{
			printf(" in func %s ,crc not match \n",__func__);
			return -1;
		}

	out[0]=crc[0];
	out[1]=crc[1];

	/*追加版本号到文件末尾*/	
	fd=open(dwnFilPro[index].tmpname,O_RDWR);
	lseek(fd,0,SEEK_END);
	write(fd,dwnFilPro[index].ver,2);
	close(fd);
	DebugPrintf("-- in func %s ,verify file success \n",__func__);
	return 0;
}


#define UP_FILE_INFO_PART

/*****************************************上传文件信息部分******************************/
int create_send_file_info_mission(int index,unsigned char * incrc,mission_info * out)
{
	
	int mesglen,datalen,pos;
	char asc_buff[64];
	message mesg={0};
	LongUnon lgdata;

	send_file_info sendinfo;

	memcpy(sendinfo.name,dwnFilPro[index].keyword,40);
	lgdata.i=dwnFilPro[index].destlen;
	memcpy(sendinfo.filesz,lgdata.longbuf,4);
	lgdata.i=dwnFilPro[index].ver;
	memcpy(sendinfo.ver,lgdata.longbuf,4);
	memcpy(sendinfo.fileCRC,incrc,2);
	
	DebugPrintf("---- in func %s ----\n",__func__);
	
	if(out==NULL)
			return -1;
	
	datalen=sizeof(sendinfo);
	mesglen=datalen+SYN_INFO_LEN+COMPRESS_LEN+MSG_TYPE_LEN+VERSION_LEN+POS_ID_LEN+CRC_LEN;	//不包括包长字段的长度
#if 0
	memset(asc_buff,0,sizeof(asc_buff));	
	sprintf(asc_buff,"%04d",mesglen);
	asc_to_bcd(mesg.len,asc_buff,2*sizeof(mesg.len));	//注意 包长可能不一定是bcd码
#else
	ShortUnon shortdata;
	shortdata.i=mesglen;
	//memcpy(mesg.len,shortdata.intbuf,2);
	mesg.len[0]=shortdata.intbuf[1];
	mesg.len[1]=shortdata.intbuf[0];

#endif
	

	/*同步信息*/
	mesg.syncinfo[0]=(SEND_FILE_INFO_SYN>>8) & 0x00ff;
	mesg.syncinfo[1]=(SEND_FILE_INFO_SYN& 0x00ff);

	mesg.compress=0x0;

	mesg.type[0]=0x30;
	mesg.type[1]=0x0C;
	
	mesg.ver=0x0;
	
	memcpy(mesg.posid,devinfo->posid,sizeof(mesg.posid));
	memcpy(mesg.data,&sendinfo,datalen);

	/*CRC16 计算*/
	int ret;
	char crc[2];
	ret=get_msg_crc16(&mesg,datalen,crc);
	if(ret==0)
		{
		mesg.crc[0]=crc[0];
		mesg.crc[1]=crc[1];
		}
	else
		return -1;

	/*将消息内容填入任务包*/
	out->type=MISS_SEND_FILE_INFO;
	out->datalen=mesglen+2;		//+2 是包含了包长字段
	out->status=0;
	out->priority=0;
	
	pos=0;
	memcpy(out->data+pos,mesg.len,2);
	pos+=2;
	memcpy(out->data+pos,mesg.syncinfo,SYN_INFO_LEN);
	pos+=SYN_INFO_LEN;
	out->data[pos]=mesg.compress;
	pos+=COMPRESS_LEN;
	memcpy(out->data+pos,mesg.type,MSG_TYPE_LEN);
	pos+=MSG_TYPE_LEN;
	out->data[pos]=mesg.ver;
	pos+=VERSION_LEN;
	memcpy(out->data+pos,mesg.posid,POS_ID_LEN);
	pos+=POS_ID_LEN;
	memcpy(out->data+pos,mesg.data,datalen);
	pos+=datalen;
	memcpy(out->data+pos,mesg.crc,CRC_LEN);


	return 0;



}


int send_file_back_process(mission_list * mission,char * framdata,int framesz)
{

	message msg;
	send_file_back backinfo;


	DebugPrintf("----- in func %s ------\n",__func__);
	if(framdata==NULL || mission==NULL )
			{
				printf("in func %s paramiter is NULL \n",__func__);
				return -1;
			}
		
		int pos=0;
		memcpy(msg.len,framdata+pos,2);
		pos+=2;
		memcpy(msg.syncinfo,framdata+pos,SYN_INFO_LEN); 
		pos+=SYN_INFO_LEN;
		msg.compress=framdata[pos];
		pos+=COMPRESS_LEN;
		memcpy(msg.type,framdata+pos,MSG_TYPE_LEN);
		pos+=MSG_TYPE_LEN;
		msg.ver=framdata[pos];
		pos+=VERSION_LEN;
		memcpy(msg.posid,framdata+pos,POS_ID_LEN);
		pos+=POS_ID_LEN;
		
		int datalen=framesz-2-SYN_INFO_LEN-COMPRESS_LEN-MSG_TYPE_LEN-VERSION_LEN-POS_ID_LEN-CRC_LEN;
		memcpy(msg.data,framdata+pos,datalen);
		pos+=datalen;
		memcpy(msg.crc,framdata+pos,CRC_LEN);
		
	
		/*计算该包的crc是否正确*/
		int ret;
		unsigned char crc[2];
		ret=get_msg_crc16(&msg,datalen,crc);
		if(crc[0]!=msg.crc[0]||crc[1]!=msg.crc[1])
		{
			printf("in func %s crc err\n",__func__);
			return -2;
		}

			/*类型匹配和同步匹配*/
	if(mission->missinfo.type!=MISS_DOWNLOAD_FILE)
		{
			printf("in func %s ,mission not match the back data\n",__func__);
			return -1;
		}
	
	if(msg.type[0]!=0x30 || msg.type[1]!=0x8B)
		{
			printf("in func %s ,back data not match the mission\n",__func__);
			return -1;
		}

	if(msg.syncinfo[0]!=mission->missinfo.data[2] || msg.syncinfo[1]!=mission->missinfo.data[3])
		{
			printf("in func %s ,SYNC INFO NOT MATHC \n",__func__);
			return -1;
		}

	memcpy(&backinfo,msg.data,sizeof(backinfo));

	DebugPrintf("-- in func %s: filename=%s , state=%d\n",__func__,backinfo.name,backinfo.state);

	int i;
	char cmd[128];
	if(backinfo.state==1)
	{
			for(i=0;i<FILE_SUPPORT_MAX;i++)
				if(memcmp(backinfo.name,dwnFilPro[i].keyword,strlen(backinfo.name))==0)
					{
						sprintf(cmd,"%s mv %s",dwnPrcInfo[i].tmppath,dwnPrcInfo[i].destpath);
						system(cmd);
						system("syc");
						printf("---------downfile success \n");
						return 0;
					}
			if(i>=FILE_SUPPORT_MAX)
			{
				printf("in func %s , dwnprinfo is err\n",__func__);
				return -1;
			}
	}
	else{
		printf("in func %s, download file err\n",__func__);
		return -1;
		}

}




int read_datas_tty(int fd,unsigned char *buffer,int * revlen)
{
	
	int retval;
	fd_set rfds;
	struct timeval tv;
	int ret = 0;
	int rilen = 0;
	
	if ((fd < 0) ||( NULL == buffer))
		{
			return NULL_POINT_ERR;
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
            return SELECT_ERR;
        }
        else if(retval)
        {
           // ret= read(fd,buffer + rilen,512);
            ret= read(fd,buffer + rilen,2048);
            rilen += ret;
            if(ret < 0)
            {
                return READ_ERR;
            }
            
			
#if CLIENT_DEBUG
            printf("read socket ret:%d data:%s \n",ret,buffer);
			int i;
			for(i=0;i<ret;i++)
				printf("%02x ",buffer[i]);
			printf("\n");
#endif
            break;
        }
        else
        {
            return SELECT_TIMOUT;
        }
    }

	*revlen=rilen;
    return SUCCESS;
}


/*
	返回值: 0 获取到数据帧
			1 获取不完整数据帧
			-1 读取失败
*/
int recive_data(char * out,int * outlen)
{
	
	static 	int readfailcounts=0;
	static int rcvlen=0;		
	int ret;
	int len=0;
		
	unsigned char singe_cache[MAX_TRANSFER];		//单次接收缓存
	unsigned char total_cache[MAX_TRANSFER];		//总缓存

	printf("--- --------in func %s ----------\n",__func__);
	ret = read_datas_tty(sockfd,singe_cache,&len);	
	
	if(ret==SELECT_TIMOUT)
	{
		printf("-----in func %s  reive form server tiomout \n",__func__);
		readfailcounts++;
		return -1;
	}
	else if(ret==SUCCESS)
	{	
		rcvlen+=len;
		if(rcvlen>(2+SYN_INFO_LEN+COMPRESS_LEN+MSG_TYPE_LEN+VERSION_LEN+POS_ID_LEN))
			{
				memcpy(total_cache,singe_cache,rcvlen);
				readfailcounts=0;
			}
		else{
			printf("recive incomplety data ,tatol length is %d \n",rcvlen);
			rcvlen= 0;
			memset (total_cache,0,sizeof(total_cache));
			memset (singe_cache,0,sizeof(singe_cache));
			
			return 1;
			}
	}
	else{
		readfailcounts++;
		if(readfailcounts>=3){
				readfailcounts=0;
				/*
					这里可能是与服务器断开了，需要重新连接服务器
				*/
				//update_connect_server_status(0);
				return -1;
		}
		return -1;
	}


	//检验是否获取到一帧数据

	int pos=0;
	unsigned short mesglen;		//数据包大小

	DebugPrintf("in func %s recive[0][1]=%02x%02x\n",__func__,total_cache[0],total_cache[1]);
	//mesglen=bcd_to_bin(total_cache+pos,2);				//长度这里可能不是用bcd码
	mesglen=((total_cache[0+pos]<<8) & 0xff00) | total_cache[1+pos];
	DebugPrintf("in func %s mesglen=%d \n",__func__,mesglen);
		
	if((mesglen+2)==rcvlen)		/*获取到一帧数据*/
	{

		DebugPrintf("------------get one frame data------------\n");
		*outlen=rcvlen;
		memcpy(out,total_cache,rcvlen);
		rcvlen= 0;
		memset (total_cache,0,sizeof(total_cache));
		memset (singe_cache,0,sizeof(singe_cache));

	}
	else
	{
		printf("recive wrong data,clear\n");
		rcvlen= 0;
		memset (total_cache,0,sizeof(total_cache));
		memset (singe_cache,0,sizeof(singe_cache));
		return -1;
	}

	return 0;
	
}


/*接收函数，根据接收到的不同命令，来构建不同的任务 */


/*	
	返回值说明
	< 0 : 出现异常
	  0 : 正常
	  1 : 重新执行该任务

*/
int client_react_server()
{
	static int sendfailtims=0;		//发送失败次数，连续的
	static int rcivfalitims=0;		//接收返回失败次数

	static int downfilefailecount=0;		//下载文件失败次数
	static int downfixfailecount=0;			//下载固件失败次数
	
	static time_t now=0, old=0;
	mission_info missinfo;
	mission_list *tempmission;

	/*接收返回需要的一些临时变量*/
	record_bak_info recordback;
	updatefirm_back_info tmpUpFirBakInf;
	file_desinfo_back tmpFilDesBak;
	char cmd[128];
	char crcout[2];
	
	int ret;
	int status;
	int pos;
	
		/*检查任务链表是否为空*/


		
		if(list_empty(&listhead->list_node))
		{
			//printf("no mission  \n");
			time(&now);
			if((now-60*2)>old)
				{
					old=now;
					/*增加心跳任务*/
					printf("---over 2 minute, new send heart mission----\n");
					memset(&missinfo,0,sizeof(missinfo));
					ret=create_send_heart_mission(heatinfo,&missinfo);
					if(ret<0)
						{
							printf("in func %s ,create send haert mission faild \n",__func__);	
							return -1;
						}
					else{
					
						tempmission=new_mission(&missinfo,NULL);
						if(tempmission!=NULL)
								if(tempmission->missinfo.priority)
									add_mission( tempmission);
								else
									add_mission_tail(tempmission);
					}
					
				}
			else
				return 0;
		}

		DebugPrintf("---- --in func %s -----\n",__func__);

		/*
			建立短连接
		*/
		printf(" create short connect !!!\n");
		close(sockfd);
		ret=create_sorket(ipbuf, port,&sockfd);
		if(ret!=SUCCESS)
		{
			printf("create sockect falied \n");
			display_neterr(ret);
			sleep(3);
			return -1;
		}


		/*获取第一个任务*/
		tempmission=NULL;
		tempmission=get_first_node_of_list(&listhead->list_node);
		if(tempmission!=NULL)
		{
			printf("get node success \n");
			printf("mission type=%02x status=%d priority=%d \n",tempmission->missinfo.type,tempmission->missinfo.status,tempmission->missinfo.priority);
		}
		else{

			printf("in func %s get mission failed \n",__func__);
			return -1;
		}

		/*发送请求*/
		switch(tempmission->missinfo.type)
		{
			case MISS_SEND_HEART:
					status=send_mesg(tempmission);
					break;
			case MISS_SEND_A2:
					status=send_mesg(tempmission);
					break;
			case MISS_REQUEST_FIX:
					status=send_mesg(tempmission);	
					break;
			case MISS_DOWNLOAD_FIX:
					status=send_mesg(tempmission);
					break;
			case MISS_REQUEST_MCH:
					status=send_mesg(tempmission);
					break;
			case MISS_REQUEST_FILE_DESINFO:
					status=send_mesg(tempmission);
					break;
			case MISS_DOWNLOAD_FILE:
					status=send_mesg(tempmission);
					break;
			case MISS_SEND_FILE_INFO:
					status=send_mesg(tempmission);
					break;
			default:
					printf("in valid mission \n");
					/*删除该任务*/
					del_mission(&listhead->list_node);
					return 0;
			
		}

		if(status<0)
		{
			/*删除该任务*/
			del_mission(&listhead->list_node);
			sendfailtims++;
			
			if(sendfailtims > 5){
				sendfailtims=0;
				//更新服务器连接标记，重连服务器
				update_connect_server_status(0);
			}
			return -1;
		}
		else if(status==0)
		{
			tempmission->missinfo.status++;		//应当避免第一个节点在处理时在其头部插入新节点
			
		}
		

		/*等待接收返回数据*/

		char framdata[MAX_TRANSFER];
		int  msglen;
		status=recive_data(framdata,&msglen);	

		if(status!=0)
		{
			rcivfalitims++;
			if(rcivfalitims>=3)
			{
				rcivfalitims=0;
				/*删除该任务*/
				printf("delet mission missiontype=%02x \n",tempmission->missinfo.type);
				del_mission(&listhead->list_node);
				return 0;
			}
			
			return -1;
		}

		/* 返回数据处理 */
		switch(tempmission->missinfo.type)
		{
			case MISS_SEND_HEART:
					status=heart_back_process(tempmission,framdata,msglen);
					if(status==0)
						{
							tempmission->missinfo.status++;
							del_mission(&listhead->list_node);
							printf("-- heart bleeding--\n");
						}
					else
						del_mission(&listhead->list_node);
					break;
			case MISS_SEND_A2:
					status=A2_back_process(tempmission,framdata,msglen,&recordback);
					if(status==0)
						{
							tempmission->missinfo.status++;
							del_mission(&listhead->list_node);
							
							ret=analyse_A2_respondcode(recordback.respond,3);
							if(ret=0)
								{
									/*通知自动上传线程该条记录上传成功，准备上传下一条*/
									DebugPrintf(" send record success \n");
								}
							else{
									/*通知自动上传线程该条记录上传失败，并做相应处理*/
								
								}
						}
					else
						del_mission(&listhead->list_node);
					break;
			case MISS_REQUEST_FIX:
					status=update_firm_info_back_process(tempmission,framdata,msglen,&tmpUpFirBakInf);
					if(status==0)
						{
						
							tempmission->missinfo.status++;
							del_mission(&listhead->list_node);
							
							/*初始化固件文件及相关过程信息*/
							ret=init_download_fix(tmpUpFirBakInf);

						#if TEST
							break;
						#endif
							/* 构建下载固件任务 */	
							if(ret==0)
							{
								memset(&missinfo,0,sizeof(missinfo));
								ret=create_download_firm_mission(dwnPrcInfo,&missinfo);
								if(ret==0)
								{
									tempmission=new_mission(&missinfo,NULL);
									if(tempmission!=NULL)
										if(tempmission->missinfo.priority)
											add_mission( tempmission);
										else
											add_mission_tail(tempmission);
								}
							}
							
							
						}
					else
						del_mission(&listhead->list_node);
					break;
	
			case MISS_DOWNLOAD_FIX:
					status = down_firm_back_process(tempmission,framdata,msglen);

					if(status==0)
					{
						tempmission->missinfo.status++;
						if(dwnPrcInfo!=NULL&&dwnPrcInfo->currsz==dwnPrcInfo->origisz)	 
						{
							sprintf(cmd,"%s mv %s",dwnPrcInfo->tmppath,dwnPrcInfo->destpath);
							system(cmd);
							system("syc");
						}
						else
						{
							ret=create_download_firm_mission(dwnPrcInfo,&missinfo);
							if(ret==0)
							{
								tempmission=new_mission(&missinfo,NULL);
								if(tempmission!=NULL)
									if(tempmission->missinfo.priority)
										add_mission( tempmission);
									else
										add_mission_tail(tempmission);
							}
						}
					}
					else if(status==-2)
					{
						sprintf(cmd,"rm -rf %s",dwnPrcInfo->tmppath);
						system(cmd);
						system("sync");
					}
			
					del_mission(&listhead->list_node);
					break;

			case MISS_REQUEST_MCH:
					status=request_Mchn_back_process(tempmission,framdata,msglen);
					del_mission(&listhead->list_node);
					break;
			case MISS_REQUEST_FILE_DESINFO:
					status=request_file_desinfo_process(tempmission,framdata,msglen,&tmpFilDesBak);
					if(status==0)
					{

						tempmission->missinfo.status++;
						del_mission(&listhead->list_node);

						/*初始化文件下载过程信息*/
						ret =init_dwonfie_process(&tmpFilDesBak);
					#if TEST
						//break;
					#endif	
						/* 构建下载文件任务 */
						memset(&missinfo,0,sizeof(missinfo));
						ret=create_dwonload_file_mission(&missinfo);
						if(ret==0)
						{
							tempmission=new_mission(&missinfo,NULL);
							if(tempmission!=NULL)
								if(tempmission->missinfo.priority)
									add_mission( tempmission);
								else
									add_mission_tail(tempmission);
						}
							
					}
					else
						del_mission(&listhead->list_node);
					break;
					
			case MISS_DOWNLOAD_FILE:
					status=down_file_back_process(tempmission,framdata,msglen,&pos);
					if(status==0)
						{
							tempmission->missinfo.status++;
							if(dwnFilPro[pos].destlen==dwnFilPro[pos].offset)
								{
									
									/*校验文件crc*/
									ret=verify_download_file(pos,crcout);
									
								#if 0
										printf("------------ dwon load file success---\n");
										del_mission(&listhead->list_node);
										break;
								#endif
								
									if(ret==0)
									{
										
									#if 	0	//该命令无效
										/*添加下载完成任务，上送下载结果*/
										ret=create_send_file_info_mission(pos,crcout,&missinfo);
										if(ret==0)
										{
											tempmission=new_mission(&missinfo,NULL);
											if(tempmission!=NULL)
												if(tempmission->missinfo.priority)
													add_mission( tempmission);
												else
													add_mission_tail(tempmission);
										}
									#else
										printf("tmpath=%s  destpath=%s \n",dwnFilPro[pos].tmpname,dwnFilPro[pos].destname);
										sprintf(cmd," mv %s  %s",dwnFilPro[pos].tmpname,dwnFilPro[pos].destname);
										system(cmd);
										system("sync");
										printf("------------ dwon load file success---\n");
									#endif
									}
									
									
								}
							else
							{
									
								ret=create_dwonload_file_mission(&missinfo);
								if(ret==0)
								{
									tempmission=new_mission(&missinfo,NULL);
									if(tempmission!=NULL)
										if(tempmission->missinfo.priority)
											add_mission( tempmission);
										else
											add_mission_tail(tempmission);
								}
							}
							del_mission(&listhead->list_node);
						}
					else{
							downfilefailecount++;
							if(downfilefailecount>=3)
								{
									downfilefailecount=0;
									del_mission(&listhead->list_node);
								}
						}
					break;
					
				case MISS_SEND_FILE_INFO:
					status=send_file_back_process(tempmission,framdata,msglen);
					del_mission(&listhead->list_node);
					break;	
			default :
					break;
				
		}
		
		
		return 0;
}


void * main_client_thread(void * args)
{

	int ret;
	int failecounts=0;
	mission_info missinfo;
	mission_list *tempmission;


	printf("------- in func %s -\n",__func__);

	signal(SIGPIPE,SIG_IGN);  //关闭SIGPIPE信号，防死机
	
	//初始化设备信息
	devinfo=init_device_info();
	if(devinfo==NULL)
	{
		printf(" in func %s init device info failed \n",__func__);
		return ;
	}

	heatinfo=get_heart_info(devinfo);
	if(heatinfo==NULL)
	{
		printf(" in func %s init device info failed \n",__func__);
		return ;
	}

	//初始化任务队列
	ret=init_queue();
	if(ret<0)
	{
		printf(" in func %s init queue failed \n",__func__);
		return ;
	}

	while(1)
	{

		if(netdevicestatus)
		{
			if(connetstatus)
			{
				ret=client_react_server();
				sleep(1);
			}
			else
			{
				printf("connecting server !!!!!!!!\n");
				if(sockfd>0)
				close(sockfd);
				sleep(3);

				ret=create_sorket(ipbuf, port,&sockfd);
				if(ret!=SUCCESS)
				{
					printf("create sockect falied \n");
					display_neterr(ret);
					sleep(3);
					failecounts++;
				}
				else{

					//第一次连接上时发送一次心跳,构建心跳任务,前置签到
					#if	0
					memset(&missinfo,0,sizeof(missinfo));
					ret=create_send_heart_mission(heatinfo,&missinfo);
					if(ret<0)
						{
							printf("in func %s ,create send haert mission faild \n",__func__);	
							failecounts++;
						}
					else{
						//增加任务
						tempmission=new_mission(&missinfo,NULL);
						if(tempmission!=NULL)
								if(tempmission->missinfo.priority)
									add_mission( tempmission);
								else
									add_mission_tail(tempmission);
						
						//更新服务器连接状态
						failecounts=0;
						update_connect_server_status(1);
						printf("connet server success \n");
						sleep(3);
						continue;
					}
					#else
						#if TEST
								/*增加一个上传记录任务*/
							record_info recorinfo;
									
							recorinfo.record.RType=0x27;
							memcpy(recorinfo.record.RSannum,"\x99\x99\x99\x00\x05\x06",6);
							memcpy(recorinfo.record.RValuej,"\x00\x00\x00\x00",4);
							memcpy(recorinfo.record.RTradenum,"\x53\x07\x00\x00",4);
							memcpy(recorinfo.record.RValuey,"\x00\x00\x00\x00",4);
							memcpy(recorinfo.record.RTime,"\x20\x17\x10\x18\x09\x08\x12",7);
							memcpy(recorinfo.record.RCsn,"\x00\x00\x00\x00\x00\x00",6);
							memcpy(recorinfo.record.RSeq,"\xF6\xD8\xF9\x17",4);
							memcpy(recorinfo.record.RCardcount,"\x00\x00",2);
							recorinfo.record.RCardtype=0x01;
							recorinfo.record.RCardattr = 0x19;
							memcpy(recorinfo.record.RAuthcode,"\x00\x00\x00\x00",4);
							memcpy(recorinfo.record.RCardid0,"\x03\x10",2);
							memcpy(recorinfo.record.RCityid,"\x51\x70",2);
							memcpy(recorinfo.record.RUnionid,"\x08\x00",2);
							memcpy(recorinfo.record.RCardissueid,"\x00\x00\x02\x28",4);
							memcpy(recorinfo.record.RPrebanlance,"\x00\x00\x00\x00",4);
							memcpy(recorinfo.record.RSerialnum,"\x00\x00\x00\x00",4);
							recorinfo.record.RTradestatus = 0x00;
							recorinfo.record.RTacflag=0x00;
							recorinfo.record.RTradetypeflag=0x00;
							memcpy(recorinfo.record.RCpucounter,"\x00\x00",2);
							memcpy(recorinfo.record.RTradeserpsam,"\x00\x00\x00\x00",4);
							recorinfo.record.RKeyver=0x00;
							recorinfo.record.RKeyindex=0x08;
							memcpy(recorinfo.record.RIssuerid,"\x00\x00\x00\x00\x00\x00\x00\x00",8);
							recorinfo.record.RTimecardtype=0x00;
							memcpy(recorinfo.record.RChargeideal,"\x00\x00\x00\x00",4);
							memcpy(recorinfo.record.RMarktime,"\x20\x17\x10\x18\x09\x08\x12",7);
							memcpy(recorinfo.record.RLastcityid,"\x10\x00",2);
							memcpy(recorinfo.record.RUpjigouid,"\x01\x01\x10\x00\xFF\xFF\xFF\xFF",8);
							memcpy(recorinfo.record.RUpcorpid,"\x33\x00",2);
							memcpy(recorinfo.record.RMarklineid,"\x01\x00",2);
							recorinfo.record.RMarkstation=0x02;
							memcpy(recorinfo.record.RMarkvehicleid,"\x87\x65\x43\x21",4);
							memcpy(recorinfo.record.RDowncorpid,"\x00\x00",2);
							memcpy(recorinfo.record.RTradelineid,"\x00\x00",2);
							recorinfo.record.RTradestation=0x00;
							memcpy(recorinfo.record.RTradevehiclid,"\x00\x00\x00\x00",4);
							memcpy(recorinfo.record.RLicheng,"\x00\x00",2);
							recorinfo.record.RDirflag=0xab;
							memcpy(recorinfo.record.RDrivercode,"\x00\x00\x00\x00",4);
							memcpy(recorinfo.record.RSellorcode,"\x00\x00\x00\x00",4);
							memcpy(recorinfo.record.RSellorcode1,"\x00\x00\x00\x00",4);
							recorinfo.record.RRecordpricetype,0x00;;
							recorinfo.record.RRecordcardtype=0xb0;
							memcpy(recorinfo.record.RCheckcode,"\xc7\x45",4);
							ret=create_send_A2_mission(&recorinfo, &missinfo);
							tempmission=new_mission(&missinfo,NULL);
							if(tempmission!=NULL)
								if(tempmission->missinfo.priority)
									add_mission( tempmission);
								else
									add_mission_tail(tempmission);
							
						#endif
					#endif
					printf("creete socket success \n");	
					update_connect_server_status(1);
					sleep(1);
					
				}

				printf("---- creat socket fialted times =%d \n",failecounts);


				if(failecounts>=30)
					{
						//可以发送信号给拨号线程重新拨号
						//sprintf(logbuff,"无法连接卡联服务器,休眠5分钟  isconnectserver=%d !!!!!",IsConnectServer);
						//WRITELOGFILE(LOG_ERROR,logbuff);
						failecounts=0;
						sleep(300);
					}
				else if (failecounts>=10)
					{
						//sprintf(logbuff,"无法连接卡联服务器,休眠2分钟  isconnectserver=%d !!!!!",IsConnectServer);
						//WRITELOGFILE(LOG_ERROR,logbuff);
						sleep(120);
					}
				else if(failecounts>=5)
					{
						
						//sprintf(logbuff,"无法连接卡联服务器,休眠1分钟  isconnectserver=%d !!!!!",IsConnectServer);
						//WRITELOGFILE(LOG_ERROR,logbuff);
						sleep(60);
					}
				
			}
			
		}
		else
		{
			//更新服务器连接状态
			update_connect_server_status(0);
		}
	}
	

}




/*
void * send_thread(void * args)
{

	while(1){

			if(connetstatus && netdevicestatus){

				//执行任务队列里的内容
				

				//尝试从任务队列里获取一个任务
			}
			else{
				sleep(10);
			}

	}

}
*/


/*test program*/

int main()
{


		pthread_t client_pt;
		pthread_attr_t attr;
		int ret;

		
		printf("------welcom to this test client ------\n");	
		
		update_net_device_satus(1);
		
		//et=pthread_create(&client_pt,&attr,main_client_thread,"AVBC");	
		
		main_client_thread(NULL);
		if(ret!=0)
			{
				printf("create pthread failed \n");
				fprintf(stderr, "Failed to malloc memory, errno:%u, reason:%s\n",errno, strerror(errno));
				return 0;
			}
		while(1){
			sleep(60);
		}
		pthread_join (client_pt, NULL);
		return 0;
		
}
