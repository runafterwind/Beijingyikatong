#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <errno.h>
#include <netdb.h>
#include <sys/wait.h>
#include <dirent.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <popt.h>
#include <ctype.h>
#include <sys/socket.h>

#include "gps.h"
#include "../gui/InitSystem.h"
#include "../net/client.h"

#define FALSE 0
#define TRUE  1
//extern unsigned char ConnectFlag; //上网标志
extern pthread_mutex_t m_socketwrite;
extern int sockfd;	//

extern int write_datas_gprs(int fd, unsigned char *buffer, int buf_len);	



int gps_fd;
int gpssockfd=-1;
unsigned char GPSSIG;

volatile float GPS_longitude = 0;
volatile float GPS_latitude = 0;

volatile float DUSUGAO_GPS_longitude = 0;
volatile float DUSUGAO_GPS_latitude = 0;
int speed_arrgps[] =
{
	B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300,
	B38400, B19200, B9600,  B4800, B2400, B1200,B600, B300,
};

int name_arrgps[] =
{
	115200, 38400, 19200, 9600, 4800, 2400, 1200, 300,
	38400, 19200, 9600, 4800, 2400,  1200,600, 300,
};
char NGPS[15];
char EGPS[15];
 /*
 *************************************************************************************************************
 - 函数名称 : void set_speedgps (int fd, int speed)
 - 函数说明 : 设置连接GPS的串口波特率
 - 输入参数 : fd :串口设备句柄，speed：波特率：一般设置为9600
 - 输出参数 : 无
 *************************************************************************************************************
 */
void set_speedgps (int fd, int speed)
{
	int i;
	int status;
	struct termios Opt;
	tcgetattr (fd, &Opt);
	for (i = 0; i < sizeof (speed_arrgps) / sizeof (int); i++)
	{
		if(speed == name_arrgps[i])
		{
			tcflush (fd, TCIOFLUSH);
			if(cfsetispeed (&Opt, speed_arrgps[i])==0) printf("sucess\n");
			else printf("set speed failed");
			cfsetospeed (&Opt, speed_arrgps[i]);
			status = tcsetattr (fd, TCSANOW, &Opt);
			if (status != 0)
			{
				perror ("tcsetattr fd1");
				return;
			}
			tcflush (fd, TCIOFLUSH);
		}
	}
}

 /*
 *************************************************************************************************************
 - 函数名称 : int set_Paritygps (int fd, int databits, int stopbits, int parity)
 - 函数说明 : 设置连接GPS的串口属性
 - 输入参数 : fd :串口设备句柄，databits：数据位，一般为8;stopbits，停止位，一般设置为'N'；parity：校验位，一般设置为1
 - 输出参数 : 无
 *************************************************************************************************************
 */
int set_Paritygps (int fd, int databits, int stopbits, int parity)
{
	struct termios options;
	if (tcgetattr (fd, &options) != 0)
	{
		perror ("SetupSerial 1");
		return (FALSE);
	}
	options.c_cflag &= ~CSIZE;
	switch (databits)
	{
	case 7:
		options.c_cflag |= CS7;
		break;
	case 8:
		options.c_cflag |= CS8;
		break;
	default:
		fprintf (stderr, "Unsupported data size\n");
		return (FALSE);
	}
	switch (parity)
	{
	case 'n':
	case 'N':
		options.c_cflag &= ~PARENB;		/* Clear parity enable */
		options.c_iflag &= ~INPCK;		/* Enable parity checking */
		break;
	case 'o':
	case 'O':
		options.c_cflag |= (PARODD | PARENB);
		options.c_iflag |= INPCK;		/* Disnable parity checking */
		break;
	case 'e':
	case 'E':
		options.c_cflag |= PARENB;		/* Enable parity */
		options.c_cflag &= ~PARODD;
		options.c_iflag |= INPCK;		/* Disnable parity checking */
		break;
	case 'S':
	case 's':					/*as no parity */
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		break;
	default:
		fprintf (stderr, "Unsupported parity\n");
		return (FALSE);
	}
	switch (stopbits)
	{
	case 1:
		options.c_cflag &= ~CSTOPB;
		break;

	case 2:
		options.c_cflag |= CSTOPB;
		break;

	default:
		fprintf (stderr, "Unsupported stop bits\n");
		return (FALSE);
	}
	if (parity != 'n')
		options.c_iflag |= INPCK;
	tcflush (fd, TCIFLUSH);
	options.c_cc[VTIME] = 50;
	options.c_cc[VMIN] = 0;				/* Update the options and do it NOW */

	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_iflag &= ~( ICRNL | IXON);
	options.c_oflag &= ~( ICRNL | IXON);
	options.c_oflag &= ~OPOST;
	if (tcsetattr (fd, TCSANOW, &options) != 0)
	{
		perror ("SetupSerial 3");
		return (FALSE);
	}
	return (TRUE);
}

 /*
 *************************************************************************************************************
 - 函数名称 : int OpenDevgps (char *Dev)
 - 函数说明 : 打开GPS使用的串口
 - 输入参数 : Dev :设备指针
 - 输出参数 : 无
 *************************************************************************************************************
 */
int OpenDevgps (char *Dev)
{
	int fd = open (Dev, O_RDWR );
	if (fd == -1)
	{
		printf ("can not open uart!");
		return -1;
	}
	else
	{
		printf ("open ok!\n");
		return fd;
	}
}
 /*
 *************************************************************************************************************
 - 函数名称 : void CloseDevgps (int fd)
 - 函数说明 : 关闭GPS使用的串口
 - 输入参数 : fd :串口设备句柄
 - 输出参数 : 无
 *************************************************************************************************************
 */
void CloseDevgps (int fd)
{
	close (fd);
}

/*
 *************************************************************************************************************
 - 函数名称 : int read_datas_ttygps(int fd,char *rcv_buf,int *len)
 - 函数说明 : 读串口数据
 - 输入参数 : fd :串口设备句柄；rcv_buf:接收数据缓冲区指针；len：接收数据长度指针
 - 输出参数 : 无
 *************************************************************************************************************
 */
int read_datas_ttygps(int fd,char *rcv_buf,int *len)
{
	int retval;
	fd_set rfds;
	struct timeval tv;
	int ret=0,pos=0;
	tv.tv_sec = 0;//set the rcv wait time
	tv.tv_usec = 100000;//200000us = 0.3s
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
			ret= read(fd,rcv_buf+pos,100);
			if(ret>0)
			{
				pos += ret;
				*len = pos;
				
				if(pos >= (strlen(rcv_buf)-16))
						return 0;
			}
			else
				return -1;
		}
		else      //超时
		{
				return 1;
		}
	}
	return 0;
}
/*
 *************************************************************************************************************
 - 函数名称 : int initializegps (const char *com, int speed)
 - 函数说明 : 初始化串口
 - 输入参数 : com:串口名称指针；speed:波特率，一般为9600
 - 输出参数 : 无
 *************************************************************************************************************
 */
int initializegps (const char *com, int speed)
{
	char serial[16] = "/dev/";
	strcat(serial, com);
	int fd = OpenDevgps (serial);
	if (fd > 0)
	{
		set_speedgps (fd, speed);
		if (set_Paritygps (fd, 8,1,'n') == FALSE)
		{
			printf ("set_parity error\n");
		}
		return fd;
	}
	return -1;
}
/*
 *************************************************************************************************************
 - 函数名称 : void uninitializegps (int fd)
 - 函数说明 : 卸载初始化串口
 - 输入参数 : fd:串口设备指针
 - 输出参数 : 无
 *************************************************************************************************************
 */
void uninitializegps (int fd)
{
	CloseDevgps(fd);
}

#if 1
/*
 *************************************************************************************************************
 - 函数名称 : int gps_date_GGA(char *string,char *okdate)
 - 函数说明 : 查询GGA信号字符串
 - 输入参数 : *string:接收到的字符串；*okdate:需要查询的字符串
 - 输出参数 : 0或-1
 *************************************************************************************************************
 */
int gps_date_GGA(char *string,char *okdate)
{
	char *pointer = NULL;
	char *end = NULL;
	int  ch;
	char tmp[1024];
	bzero(tmp,sizeof(tmp));
	pointer = strstr(string, "$GPGGA");
	if (pointer != NULL)
	{
		strncpy(tmp, pointer, 100);
		strncpy(okdate, tmp, 100);			
		return 0;
		}
	return -1;
}

#endif

/*
 *************************************************************************************************************
 - 函数名称 : int gps_date_GGA(char *string,char *okdate)
 - 函数说明 : 查询RMC信号字符串
 - 输入参数 : *string:接收到的字符串；*okdate:需要查询的字符串
 - 输出参数 : 0或-1
 *************************************************************************************************************
 */
int gps_date_RMC(char *string,char *okdate)
{
	char *pointer = NULL;
	char *end = NULL;
	int  ch;
	char tmp[1024];
	bzero(tmp,sizeof(tmp));
	pointer = strstr(string, "RMC");
	if (pointer != NULL)
	{
		strncpy(tmp, pointer, 100);
		strncpy(okdate, tmp, 100);
		return 0;
	}
	return -1;
}

 /*
 *************************************************************************************************************
 - 函数名称 : int search_char(char *str,char ch)
 - 函数说明 : 在字符串中查询某一个字符
 - 输入参数 : *str:字符串指针；ch：需要查找的字符
 - 输出参数 : 无
 *************************************************************************************************************
 */
int search_char(char *str,char ch)
{
	int n;
	for(n=0; str[n]; n++) {
		if(str[n] == ch)
			return n;
	}
	return -1;
}


/*fill des string 6 bytes with str. if str not enough 6 bytes ,use '0' filled*/
int cent_sixbit(char *des, char *str)
{
	int n,str_len,point_index;

	point_index = search_char(str,'.');
	point_index++;

	for(n=0; n<6; n++) {
		if(str[point_index]) {			//use str char
			des[n] = str[point_index];
			point_index++;
		}
		else {					//use '0' char
			des[n] = '0';
		}

	}
	
	//des[6] = 0;	 //may be beyone
}


int conver_degree(char *des,char *str)
{
	float temp_float,float_d1,float_d2;
	int   temp_int,des_index=0,str_index=0;
	char temp_char[5],cent_char[15];


	temp_int = search_char(str,'.');
	if(temp_int==4) {
		des[des_index++] = str[str_index++];
		des[des_index++] = str[str_index++];
		des[des_index++] = '.';
	}
	else if(temp_int==5) {
		des[des_index++] = str[str_index++];
		des[des_index++] = str[str_index++];
		des[des_index++] = str[str_index++];
		des[des_index++] = '.';
	}
	else {
		printf("wrong format! strstr return %d",temp_int);
		return -1;
	}

	// deal cent
	memcpy(temp_char,&str[str_index],2);
	str_index +=2;
	temp_char[2]=0;
	temp_int = atoi(temp_char);
	temp_float = temp_int/60.0;
	float_d1 = temp_float;

	//skip '.' in str
	str_index++;

	//deal float cent
	memcpy(temp_char,&str[str_index],4);
	temp_char[4]=0;
	temp_int = atoi(temp_char);
	temp_float = temp_int/10000.0;
	temp_float = temp_float/60.0;
	float_d2 = temp_float;

	temp_float = float_d1+float_d2;

	sprintf(cent_char,"%.6f",temp_float);
	cent_sixbit(&des[des_index],cent_char);

}




static u8 NMEA_CMD=NMEA_NULL;         //NMEA 语句
static u8 NMEA_CMD_Buff[]="$GPxxx,"; //NMEA 语句类型缓存
static u8 NMEA_CMD_Index=0;         //读取 CMD字符的个数
        //CMD类型解析完毕
static u8 NMEA_DAT_Block=0;         //NMEA 数据字段号 从0开始
static u8 NMEA_DAT_BlockIndex=0;     //NMEA 数据每个字段内字符索引 从0开始
            //数据接收完成. 最后一条 GPRMC 语句发送完毕置1,

static u8 ucTempA=0;                 //存储解析两位数字用的的十位临时变量
static u8 SateInfoIndex=0;            //
//static u8 ucTemp[5];

stru_GPSRMC  GPS_RMC_Data;
stru_GPSGGA  GPS_GGA_Data;
stru_GPSGSA  GPS_GSA_Data;
stru_GPSGSV  GPS_GSV_Data;
strubf_GPSRMC GPSBF_RMC_DATA;
strubf_GPSGGA GPSBF_GGA_DATA;
unsigned char  buffer0[200];

 /*
 *************************************************************************************************************
 - 函数名称 : void ShowGPSTime(void) 
 - 函数说明 :  gps时间utc 转北京时间
 - 输入参数 : 无
 - 输出参数 : 无
 *************************************************************************************************************
 */
void ShowGPSTime(void)    
{
    GPS_RMC_Data.UTCDateTime[3]+=8;
    if (GPS_RMC_Data.UTCDateTime[3]>23)
    {
        GPS_RMC_Data.UTCDateTime[3]-=24; //Hour
        GPS_RMC_Data.UTCDateTime[2]++; //Day
        if (((GPS_RMC_Data.UTCDateTime[1]==1)||\
             (GPS_RMC_Data.UTCDateTime[1]==3)||\
             (GPS_RMC_Data.UTCDateTime[1]==5)||\
             (GPS_RMC_Data.UTCDateTime[1]==7)||\
             (GPS_RMC_Data.UTCDateTime[1]==8)||\
             (GPS_RMC_Data.UTCDateTime[1]==10)||\
             (GPS_RMC_Data.UTCDateTime[1]==12))&&\
             (GPS_RMC_Data.UTCDateTime[2]>31))
        {
            GPS_RMC_Data.UTCDateTime[2]=1;//Day
            GPS_RMC_Data.UTCDateTime[1]++;//Month
        }
        if (((GPS_RMC_Data.UTCDateTime[1]==4)||\
             (GPS_RMC_Data.UTCDateTime[1]==6)||\
             (GPS_RMC_Data.UTCDateTime[1]==9)||\
             (GPS_RMC_Data.UTCDateTime[1]==11))&&\
             (GPS_RMC_Data.UTCDateTime[2]>30))
        {
            GPS_RMC_Data.UTCDateTime[2]=1;
            GPS_RMC_Data.UTCDateTime[1]++;
        }
        if ((GPS_RMC_Data.UTCDateTime[1]==2)&&(GPS_RMC_Data.UTCDateTime[2]>28))
        {
            GPS_RMC_Data.UTCDateTime[2]=1;
            GPS_RMC_Data.UTCDateTime[1]++;
        }

        if(GPS_RMC_Data.UTCDateTime[1]>12)
        {
            GPS_RMC_Data.UTCDateTime[1]=1;
            GPS_RMC_Data.UTCDateTime[0]++;
        }
    }
return;
}
 /*
 *************************************************************************************************************
 - 函数名称 : void ParserGPGGA(void)   
 - 函数说明 :  GPS gga定位信息
 - 输入参数 : 无
 - 输出参数 : 无
 *************************************************************************************************************
 */
 void ParserGPGGA(void)       
{
	int i,iLen,time,status,used,ttyr;
	char rx_string[1024] = {0};
	char lcdtest[200];
	char *comma[20];
	float fn,fe;
	
	bzero(rx_string,1024);
	signal(SIGPIPE,SIG_IGN);  //关闭SIGPIPE信号，防死机
	read_datas_ttygps(gps_fd,rx_string,&iLen);
	if(iLen>0)
	{
		bzero(lcdtest,sizeof(lcdtest));
	    if(gps_date_GGA(rx_string,lcdtest) == 0)
		{
			printf("%s\n",lcdtest);
			comma[0] = strstr(lcdtest, ",");  //UTC 时间，hhmmss(时分秒)格式
			if(NULL!=comma[0])
			{
				for(i=1;i<6;i++)
				{
					comma[i] = strstr(comma[i-1]+1, ",");  
					if(NULL==comma[i])
					{
						printf("GPGGA信息不完整\n");
						return;
					}
				}
			}
			else
			{
				printf("GPGGA信息不完整\n");
				return;
			}
#if 0			
			comma[0] = strstr(lcdtest, ",");        //UTC 时间，hhmmss(时分秒)格式
			comma[1] = strstr(comma[0]+1, ",");		//纬度，格式为ddmm.mmmm(第一位是零也将传送)；
			comma[2] = strstr(comma[1]+1, ",");     //纬度半球N(北半球)或S(南半球)
			comma[3] = strstr(comma[2]+1, ",");     //经度dddmm.mmmm(度分)格式(前面的0也将被传输)
			comma[4] = strstr(comma[3]+1, ",");     //经度半球E(东经)或W(西经)
			comma[5] = strstr(comma[4]+1, ",");     //定位质量指示，0=定位无效，1=定位有效；
			comma[6] = strstr(comma[5]+1, ",");     //使用卫星数量，从00到12(第一个零也将传送)
			comma[7] = strstr(comma[6]+1, ",");     //水平精确度，0.5到99.9
			comma[8] = strstr(comma[7]+1, ",");     //天线离海平面的高度，-9999.9到9999.9米M指单位米
			comma[9] = strstr(comma[8]+1, ",");		
#endif
			memcpy(GPSBF_GGA_DATA.UTCTime,comma[0]+1,(comma[1]-comma[0]-1));
			memcpy(GPSBF_GGA_DATA.Latitude,comma[1]+1,(comma[2]-comma[1])-1);
			memcpy(GPSBF_GGA_DATA.NS,comma[2]+1,(comma[3]-comma[2]-1));
			memcpy(GPSBF_GGA_DATA.Longitude,comma[3]+1,(comma[4]-comma[3]-1));
			memcpy(GPSBF_GGA_DATA.EW,comma[4]+1,(comma[5]-comma[4]-1));
		//	memcpy(GPSBF_GGA_DATA.PositionFix,comma[5]+1,(comma[6]-comma[5]-1));
		//	memcpy(GPSBF_GGA_DATA.SatUsed,comma[6]+1,(comma[7]-comma[6]-1));
		//	memcpy(GPSBF_RMC_DATA.Course,comma[7]+1,(comma[8]-comma[7]-1));
		//	memcpy(GPSBF_RMC_DATA.UTCDate,comma[8]+1,(comma[9]-comma[8]-1));

			memcpy(GPS_GGA_Data.EW,GPSBF_GGA_DATA.EW,sizeof(GPSBF_GGA_DATA.EW));
		    memcpy(GPS_GGA_Data.NS,GPSBF_GGA_DATA.NS,sizeof(GPSBF_GGA_DATA.NS));

							//时间日期
			time=atoi(GPSBF_GGA_DATA.UTCTime);
				
			GPS_GGA_Data.UTCTime[0]=(u8)((time/10000)%100);
			GPS_GGA_Data.UTCTime[1]=(u8)((time/100)%100);
			GPS_GGA_Data.UTCTime[2]=(u8)(time%100);

			GPS_GGA_Data.UTCTime[0]+=8;
			if (GPS_GGA_Data.UTCTime[0]>23)
			{
				GPS_GGA_Data.UTCTime[0]-=24;
			}

							//纬度
			fn=atof(GPSBF_GGA_DATA.Latitude);
			GPS_GGA_Data.Latitude[0]=(u8)(fn/100);
			GPS_GGA_Data.Latitude[1]=(u8)(((int)fn)%100);
			GPS_GGA_Data.Latitude[2]=(u8)((fn-(float)((int)fn))/0.01);
			GPS_GGA_Data.Latitude[3]=(u8)((int)((fn-(float)((int)fn))/0.0001)%100);
			//经度
			fe=atof(GPSBF_GGA_DATA.Longitude);
			GPS_GGA_Data.Longitude[0]=(u8)(fe/10000);
			GPS_GGA_Data.Longitude[1]=(u8)(((int)(fe/100))%100);
			GPS_GGA_Data.Longitude[2]=(u8)(((int)fe)%100);
			GPS_GGA_Data.Longitude[3]=(u8)((fe-(float)((int)fe))/0.01);
			GPS_GGA_Data.Longitude[4]=(u8)((int)((fe-(float)((int)fe))/0.0001)%100);
#if 0
			status=atoi(GPSBF_GGA_DATA.PositionFix);
			used=atoi(GPSBF_GGA_DATA.SatUsed);
			GPS_GGA_Data.PositionFix=status;
			GPS_GGA_Data.SatUsed=used;
			printf("status=%d\n",status);
			if(1==status)           //有效定位
			{			
				printf(" GPGGA GPS有效定位信息:\n");
			}
			else
			{
				printf("GPGGA GPS无效定位信息:\n");
			}
#endif
			printf("GGA GPS定位信息:\n");
printf("纬度: %d%d.%d%d    %c\n",GPS_GGA_Data.Latitude[0],GPS_GGA_Data.Latitude[1],GPS_GGA_Data.Latitude[2],GPS_GGA_Data.Latitude[3],GPS_GGA_Data.NS[0]);
printf("经度: %d%d%d.%d%d   %c\n",GPS_GGA_Data.Longitude[0],GPS_GGA_Data.Longitude[1],GPS_GGA_Data.Longitude[2],GPS_GGA_Data.Longitude[3],GPS_GGA_Data.Longitude[4],GPS_GGA_Data.EW[0]);
			printf("卫星数: %d\n",5);
			printf("时间: %d:%d:%d\n",GPS_GGA_Data.UTCTime[0],GPS_GGA_Data.UTCTime[1],GPS_GGA_Data.UTCTime[2]);						
		}	
	}
		
return;
}



 
 /*
 *************************************************************************************************************
 - 函数名称 : int write_datas_gprs(int fd, unsigned char *buffer, int buf_len)
 - 函数说明 : 向串口发送数据
 - 输入参数 : fd:串口设备句柄；buffer:需要发送的数据缓冲区指针；buf_len：数据长度
 - 输出参数 : 无
 *************************************************************************************************************
 */
 /*
 static int write_datas_tty(int fd, unsigned char *buffer, int buf_len)
 {
	 struct timeval tv;
	 fd_set w_set;
	 int bytes_to_write_total = buf_len;
	 int bytes_have_written_total = 0;
	 int bytes_write = 0;
	 int result = -1;
	 unsigned char *ptemp = buffer;
 
	 if ((fd<0) ||( NULL == buffer )|| (buf_len <=0))
	 {
		 printf("Send Buffer is Nc\n");
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
			 return -1;
		 }
		 else if (0 == result)	  //this means timeout, it is not an error, so we return 0.
		 {
			 printf("Send Data Timeout --->3\n");
			 return 0;
		 }
		 else
		 {
			 if (FD_ISSET(fd, &w_set))
			 {
 
				 printf("W socket=%03d::%s \n",bytes_to_write_total,ptemp);
 
				 bytes_write = send(fd, ptemp, bytes_to_write_total, 0);
 
				 printf("bytes_write = %02X\n",bytes_write);
 
				 if (bytes_write < 0)
				 {
					 if (EAGAIN == errno || EINTR == errno)
					 {
						 continue;
					 }
					 printf("open Send Macine is error2\n");
					 return -1;
				 }
				 else if (0 == bytes_write)
				 {
					 printf("Send Data Timeout --->2\n");
					 return -1; 
				 }
				 else
				 {
					 bytes_to_write_total -= bytes_write;
					 bytes_have_written_total += bytes_write;
					 ptemp += bytes_have_written_total;
					 printf("Write GPS data\n");
				 }
			 }
		 }
	 }

	 printf("----------------->Send Data OK\n");
	 return 0;
 }
*/
 /*
 *************************************************************************************************************
 - 函数名称 : int write_datas_gprs(int sockfd, unsigned char *buffer, int buf_len)
 - 函数说明 : 写 SOCKET 数据
 - 输入参数 : socketfd:网络句柄；buffer:需要发送的数据缓冲区指针；buf_len：数据长度
 - 输出参数 : 无
 *************************************************************************************************************
 */
 static int write_datas_gps(int fd, unsigned char *buffer, int buf_len)
 {
	 int status;
 
 //  printf("write_datas_gprs buf_len :%d\n",buf_len);
	 
	 pthread_mutex_lock(&m_socketwrite); //线程自锁
	 status = write_datas_tty(fd,buffer,buf_len);
	 pthread_mutex_unlock(&m_socketwrite);//线程解锁
	 return status;
 }


 unsigned char SendHandDatagps(unsigned char *send55AA)
 {
	 unsigned char status = 1;
 
#if GPRSPR
	 printf("SendHandData = %s\n",send55AA);
#endif
	 status = write_datas_gps(gpssockfd,send55AA,strlen(send55AA));
	 return(status);
 }
//#endif


int myPrintfChar(char * name ,char *buf, int len)
{	
	int i;
	printf("%s : ", name);
	for(i=0; i<len; i++)
		printf("%c ", buf[i]);
	printf("\n");
}

 /*
 *************************************************************************************************************
 - 函数名称 :void ParserGPRMC(void)
 - 函数说明 : 解析RMC数据格式
 - 输入参数 : 无
 - 输出参数 : 无
 *************************************************************************************************************
 */
 unsigned char ParserGPRMC(void)
{
	int i,iLen=0,time,date;
	char lcdtest[300];
	char rx_string[2048] = {0};
	char *comma[20];
	char *UTCTime;
	float fn,fe,sp,co, TempPosition;
	

	bzero(rx_string,2048);
	signal(SIGPIPE,SIG_IGN);  //关闭SIGPIPE信号，防死机
	read_datas_ttygps(gps_fd,rx_string,&iLen);
/*
	printf("GPS读取到的数据个数:%d\n",iLen);
    for(i=0;i<iLen;i++)
    {
        printf("%c",rx_string[i]);
        }
    printf("\n");
    */
//	printf("GPS  = : %s",rx_string);
	if(iLen > 0)
	{
		bzero(lcdtest,sizeof(lcdtest));
		if(gps_date_RMC(rx_string,lcdtest) == 0)
		{
		//	printf("%s\n",lcdtest);
			comma[0] = strstr(lcdtest, ",");  //UTC 时间，hhmmss(时分秒)格式
			if(NULL!=comma[0])
			{
				for(i=1;i<10;i++)
				{
					comma[i] = strstr(comma[i-1]+1, ",");
                   
					if(NULL==comma[i])
					{
						return 0;
					}
				}
			}
			else
			{
			//	printf("GPRMC信息不完整2\n");
				return 0;
			}
			memcpy(GPSBF_RMC_DATA.UTCTime,comma[0]+1,(comma[1]-comma[0]-1));	//utc time
			memcpy(GPSBF_RMC_DATA.Status,comma[1]+1,(comma[2]-comma[1])-1);		//定位状态
			memcpy(GPSBF_RMC_DATA.Latitude,comma[2]+1,(comma[3]-comma[2]-1));	//纬度
			memcpy(GPSBF_RMC_DATA.NS,comma[3]+1,(comma[4]-comma[3]-1));			//半球
			memcpy(GPSBF_RMC_DATA.Longitude,comma[4]+1,(comma[5]-comma[4]-1));	//经度
			memcpy(GPSBF_RMC_DATA.EW,comma[5]+1,(comma[6]-comma[5]-1));			//半球
			memcpy(GPSBF_RMC_DATA.Speed,comma[6]+1,(comma[7]-comma[6]-1));
			memcpy(GPSBF_RMC_DATA.Course,comma[7]+1,(comma[8]-comma[7]-1));
			memcpy(GPSBF_RMC_DATA.UTCDate,comma[8]+1,(comma[9]-comma[8]-1));	// utc date

			memcpy(GPS_RMC_Data.Status,GPSBF_RMC_DATA.Status,sizeof(GPSBF_RMC_DATA.Status));


			memcpy(GPS_RMC_Data.EW,GPSBF_RMC_DATA.EW,sizeof(GPSBF_RMC_DATA.EW));
			memcpy(GPS_RMC_Data.NS,GPSBF_RMC_DATA.NS,sizeof(GPSBF_RMC_DATA.NS));
				
			//时间日期
			time=atoi(GPSBF_RMC_DATA.UTCTime);
			date=atoi(GPSBF_RMC_DATA.UTCDate);
			
			GPS_RMC_Data.UTCDateTime[3]=(u8)((time/10000)%100);
			GPS_RMC_Data.UTCDateTime[4]=(u8)((time/100)%100);
			GPS_RMC_Data.UTCDateTime[5]=(u8)(time%100);
			GPS_RMC_Data.UTCDateTime[2]=(u8)((date/10000)%100);
			GPS_RMC_Data.UTCDateTime[1]=(u8)((date/100)%100);
			GPS_RMC_Data.UTCDateTime[0]=(u8)(date%100);
			//纬度
			fn=atof(GPSBF_RMC_DATA.Latitude);
			GPS_RMC_Data.Latitude[0]=(u8)(fn/100);
			GPS_RMC_Data.Latitude[1]=(u8)(((int)fn)%100);
			GPS_RMC_Data.Latitude[2]=(u8)((fn-(float)((int)fn))/0.01);
			GPS_RMC_Data.Latitude[3]=(u8)((int)((fn-(float)((int)fn))/0.0001)%100);
			//经度
			fe=atof(GPSBF_RMC_DATA.Longitude);
			GPS_RMC_Data.Longitude[0]=(u8)(fe/10000);
			GPS_RMC_Data.Longitude[1]=(u8)(((int)(fe/100))%100);
			GPS_RMC_Data.Longitude[2]=(u8)(((int)fe)%100);
			GPS_RMC_Data.Longitude[3]=(u8)((fe-(float)((int)fe))/0.01);
			GPS_RMC_Data.Longitude[4]=(u8)((int)((fe-(float)((int)fe))/0.0001)%100);

			//航速
			sp=atof(GPSBF_RMC_DATA.Speed);
			GPS_RMC_Data.Speed[0]=(u8)(sp/100);
			GPS_RMC_Data.Speed[1]=(u8)(((int)sp)%100);
			GPS_RMC_Data.Speed[2]=(u8)((sp-(float)((int)sp))/0.01);

			//航向
			co=atof(GPSBF_RMC_DATA.Course);
			GPS_RMC_Data.Course[0]=(u8)(co/100);
			GPS_RMC_Data.Course[1]=(u8)(((int)co)%100);
			GPS_RMC_Data.Course[2]=(u8)((co-(float)((int)co))/0.01);
			ShowGPSTime();
			
			if(0 == strcmp(GPS_RMC_Data.Status,"A"))           //有效定位
			{			
				//printf("GPRMC GPS有效定位信息:\n");
			}
			else
			{
				//printf("GPRMC GPS无效定位信息:\n");
                memset(GPS_RMC_Data.UTCDateTime,0,sizeof(stru_GPSRMC));
                return 0;
			}
			TempPosition = (GPS_RMC_Data.Latitude[1] + GPS_RMC_Data.Latitude[2]*0.01 + GPS_RMC_Data.Latitude[3]*0.0001)/60;
			DUSUGAO_GPS_latitude = GPS_RMC_Data.Latitude[0] + TempPosition;
			
			TempPosition = (GPS_RMC_Data.Longitude[2] + GPS_RMC_Data.Longitude[3]*0.01 + GPS_RMC_Data.Longitude[4]*0.0001)/60;
			DUSUGAO_GPS_longitude = GPS_RMC_Data.Longitude[0]*100 + GPS_RMC_Data.Longitude[1] + TempPosition;
#if 0			
			printf("原始RMC GPS定位信息:\n");
			GPS_latitude = GPS_RMC_Data.Latitude[0] + GPS_RMC_Data.Latitude[1]*0.01 + GPS_RMC_Data.Latitude[2]*0.0001 + GPS_RMC_Data.Latitude[3]*0.000001;
			GPS_longitude = GPS_RMC_Data.Longitude[0]*100 + GPS_RMC_Data.Longitude[1] + GPS_RMC_Data.Longitude[2]*0.01 + GPS_RMC_Data.Longitude[3]*0.0001 + GPS_RMC_Data.Longitude[4]*0.000001;
			printf("纬度: %f %c\n", GPS_latitude, GPS_RMC_Data.NS[0]);
			printf("经度: %f %c\n", GPS_longitude, GPS_RMC_Data.EW[0]);
			
			//DebugPrintf("纬度: %d%d.%d%d    %c\n",GPS_RMC_Data.Latitude[0],GPS_RMC_Data.Latitude[1],GPS_RMC_Data.Latitude[2],GPS_RMC_Data.Latitude[3],GPS_RMC_Data.NS[0]);
			//DebugPrintf("经度: %d%d%d.%d%d   %c\n",GPS_RMC_Data.Longitude[0],GPS_RMC_Data.Longitude[1],GPS_RMC_Data.Longitude[2],GPS_RMC_Data.Longitude[3],GPS_RMC_Data.Longitude[4],GPS_RMC_Data.EW[0]);
			printf("航速: %d%d.%d\n",GPS_RMC_Data.Speed[0],GPS_RMC_Data.Speed[1],GPS_RMC_Data.Speed[2]);
			//DebugPrintf("航向: %d%d.%d\n",GPS_RMC_Data.Course[0],GPS_RMC_Data.Course[1],GPS_RMC_Data.Course[2]);
			//DebugPrintf("时间: 20%d-%d-%d %d:%d:%d\n",GPS_RMC_Data.UTCDateTime[0],GPS_RMC_Data.UTCDateTime[1],GPS_RMC_Data.UTCDateTime[2],GPS_RMC_Data.UTCDateTime[3],GPS_RMC_Data.UTCDateTime[4],GPS_RMC_Data.UTCDateTime[5]);
#else
			DebugPrintf("UTCTime = %s\n",GPSBF_RMC_DATA.UTCTime);
			DebugPrintf("Status = %s\n",GPSBF_RMC_DATA.Status);
			DebugPrintf("Latitude = %s\n",GPSBF_RMC_DATA.Latitude);
			DebugPrintf("NS = %s\n",GPSBF_RMC_DATA.NS);
			DebugPrintf("Longitude = %s\n",GPSBF_RMC_DATA.Longitude);
			DebugPrintf("EW = %s\n",GPSBF_RMC_DATA.EW);
			DebugPrintf("Speed = %s\n",GPSBF_RMC_DATA.Speed);
			DebugPrintf("Course = %s\n",GPSBF_RMC_DATA.Course);
			DebugPrintf("UTCDate = %s\n",GPSBF_RMC_DATA.UTCDate);	
#endif		
            return 1;
		}
		return 0;
	}
    return 0;
}

/* ====GPS信号显示==== */

void gpsDisplay_signal(unsigned char type)
{
	if(type == 0)
	{
		Show_BMP(100,10,"sigbmp.bmp");
	}
	else
	{
	//	Show_BMP(100,10,"signalno.bmp");
	    SetTextSize(16);		
		TextOut(100,10,"     ");
	}
}

int d2hex(u8 data)
{
	int a=0;
	a=(data/10)*16+((int)data%10);
	return a;
}

/*
*************************************************************************************************************
- 函数名称 : char * Rd_time (char* buff)
- 函数说明 : 读时间
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
char * gpsRd_time (unsigned char* buff)
{
		time_t t;
	struct tm * tm;
	int la=0,lo=0,speed=0,Course=0;
	time (&t);
	tm = localtime (&t);
	char buf[30];
	buff[0] = d2hex((tm->tm_year+1900)/100);//HEX2BCD((unsigned char)20);		
    buff[1] = d2hex((tm->tm_year+1900)%100);//HEX2BCD((unsigned char)GPS_RMC_Data.UTCDateTime[0]);
    buff[2] = d2hex(tm->tm_mon+1);//HEX2BCD((unsigned char)GPS_RMC_Data.UTCDateTime[1]);
    buff[3] = d2hex(tm->tm_mday);//HEX2BCD((unsigned char)GPS_RMC_Data.UTCDateTime[2]);
    buff[4] = d2hex(GPS_RMC_Data.UTCDateTime[3]);//HEX2BCD((unsigned char)GPS_RMC_Data.UTCDateTime[3]);
    buff[5] = d2hex(GPS_RMC_Data.UTCDateTime[4]);//HEX2BCD((unsigned char)GPS_RMC_Data.UTCDateTime[4]);
	buff[6] = d2hex(GPS_RMC_Data.UTCDateTime[5]);//HEX2BCD((unsigned char)GPS_RMC_Data.UTCDateTime[5]);
	buff[7] = GPS_RMC_Data.NS[0];
	la=(GPS_RMC_Data.Latitude[0]*1000000)+(GPS_RMC_Data.Latitude[1]*10000)+(GPS_RMC_Data.Latitude[2]*100)+GPS_RMC_Data.Latitude[3];	
	if(la<0)
	{
		la=0;
	}
	buff[8] = (la>>24)&0xff;  //GPS_GGA_Data.Latitude[0];
	buff[9] = (la>>16)&0xff;  //GPS_GGA_Data.Latitude[1];
	buff[10] = (la>>8)&0xff;  //GPS_GGA_Data.Latitude[2];
	buff[11] = la&0xff;  //GPS_GGA_Data.Latitude[3];
	buff[14] = GPS_RMC_Data.EW[0];
	lo=(GPS_RMC_Data.Longitude[0]*100000000)+(GPS_RMC_Data.Longitude[1]*1000000)+(GPS_RMC_Data.Longitude[2]*10000)+(GPS_RMC_Data.Longitude[3]*100)+GPS_RMC_Data.Longitude[4];
	if(lo<0)
	{
		lo=0;
	}
	buff[15] = (lo>>24)&0xff; //GPS_GGA_Data.Longitude[0];
	buff[16] = (lo>>16)&0xff;  //GPS_GGA_Data.Longitude[1];
	buff[17] = (lo>>8)&0xff;  //GPS_GGA_Data.Longitude[2];
	buff[18] = lo&0xff;  //GPS_GGA_Data.Longitude[3];
	speed=(GPS_RMC_Data.Speed[0]*10000)+(GPS_RMC_Data.Speed[1]*100)+GPS_RMC_Data.Speed[2];
	buff[21] = 0x00;
	buff[22] = (speed>>16)&0xff;
	buff[23] = (speed>>8)&0xff;
	buff[24] = speed&0xff;
	Course=(GPS_RMC_Data.Course[0]*10000)+(GPS_RMC_Data.Course[1]*100)+GPS_RMC_Data.Course[2];
	buff[25] = (Course>>8)&0xff;
	buff[26] = Course&0xff;
    return buff;
}


 /*
 *************************************************************************************************************
 - 函数名称 : void * Gps_Pthread (void * args)
 - 函数说明 : GPS调用线程，在主函数中加入该线程即可
 - 输入参数 : 无
 - 输出参数 : 无
 *************************************************************************************************************
 */
void * Gps_Pthread (void * args)
{
    printf("GPS pthread run!\n");
    unsigned char status;
    int num;
	unsigned char  buff1[200];	
	gps_fd = initializegps("ttyC2", 9600);
	
	tcflush (gps_fd, TCIFLUSH);
	printf("Begin to Read:\n");
    num=0;
	for(;;)
	{
		//ParserGPGGA();
		status = ParserGPRMC();
        if(status)
		{
            memset(buff1,0xFF,sizeof(buff1));
    		gpsRd_time(buff1);
    		//buff1[7] = 78;
    		//buff1[14]= 69;
    		//printf("当前地址%s\n",buff1);
    		if(buff1[1] != 0 && buff1[2] != 0 && buff1[3] != 0)      //年月日不为0
    		{
    			if((buff1[7]==78||buff1[7]==83)&&(buff1[14]==69||buff1[14]==87))    //纬度经度必须为N S E W
    			{
    				if((buff1[8]!=0||buff1[9]!=0||buff1[10]!=0||buff1[11]!=0)&&(buff1[15]!=0||buff1[16]!=0||buff1[17]!=0||buff1[18]!=0))//经纬度不为0
    				{
                        GPSSIG = 0;
                        num=0;
    				//	gpsDisplay_signal(0);
    					memset(buffer0,0,sizeof(buffer0));
    					memcpy(buffer0,buff1,sizeof(buff1));
    				}
    				else
    				{
                        num++;
                        if(num>500)
                        {
                            num=0;            
                            GPSSIG = 1;
                        }
    				//	gpsDisplay_signal(1);
    				}
    			}
    			else
    			{
                    num++;
                    if(num>500)
                    {
                        num=0;            
                        GPSSIG = 1;
                        };
    			//	gpsDisplay_signal(1);
    			}
    		}
    		else
    		{
                num++;
                if(num>500)
                {
                    num=0;            
                    GPSSIG = 1;
                }
    		//	gpsDisplay_signal(1);
    		}
    		//sleep(1);
        }
        else
        {
            num++;
            if(num>500)
            {
                num=0;            
                GPSSIG = 1;
            }
          //  gpsDisplay_signal(1);
        }
    }
	
	if( gps_fd > 0 )
	{
		uninitializegps(gps_fd);
	}
return 0;
}

unsigned char G_STARTWGPS[8];  //全局变量
/*
*************************************************************************************************************
- 函数名称 : char WaveCOmPc(unsigned char *Date)
- 函数说明 : 发送自动上传GPS 数据
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
int gpsWaveCOmPc(unsigned char *Date)
{
    int status;
    unsigned char AusendOut[300];
	unsigned char Scheme[300];
	unsigned char i,ch;
    unsigned char sendOut[300];
	unsigned char  Len = 28;

	int result;
	FILE *gpsfile;
	char buf[100];
	char buf1[100];
	
    memset(Scheme,0,sizeof(Scheme));
    Scheme[0] = 0xA0;
 //   Scheme[1] = 0x00;
    memcpy(Scheme+1,Date,28);

    memset(sendOut,0,sizeof(sendOut));
//	 sendOut1[0] = 0x80;
    sendOut[0] = ST_RX;
    sendOut[1] = Len;
    ch =  Scheme[0];
    for(i = 1; i < Len; i++)
    {
        ch ^= Scheme[i];
    }
    sendOut[2] = ch;
    memcpy(sendOut + 3,Scheme,Len);
    sendOut[i + 3] = ST_END;

	memset(Scheme,0,sizeof(Scheme));
    hex_2_ascii_gps(sendOut,Scheme,sendOut[1] + 4);
    memset(AusendOut,0,sizeof(AusendOut));

  //  memcpy(G_STARTW,"DATA:AABBCCDD",13);
 //   strcat(AusendOut,G_STARTW);
 
   
#ifndef BS
    memcpy(G_STARTWGPS,"DATA:AABBCCDD",13);
    strcat(AusendOut,G_STARTWGPS);
    strcat(AusendOut,Scheme);
#else
     memcpy(G_STARTWGPS,"AABBCCDD",8);
    strcat(AusendOut,"SYS:");
    strcat(AusendOut,G_STARTWGPS);
    strcat(AusendOut,Scheme);
    strcat(AusendOut,":0D");
#endif
//	printf("eeeAusendOut=%s\n",AusendOut);
#if 0
	sprintf(buf1,"%s ",AusendOut);
	mk_time1(buf);
    strcat(buf,buf1);
	gpsfile = fopen("gpssave.txt","at+");
	if(gpsfile)
	{
		printf("open  gpssave.txt ok!\n");
	}
	else
	{
		printf("Can't open gpssave.txt \n");
		return -1;
	}
	result = fwrite(buf,sizeof(char),sizeof(buf),gpsfile);						
	if(result<=0)
	{
		printf("save gps send data file!!\n");	
	}
	system("sync");
	fclose(gpsfile);
#endif
#if GPRSPR
		printf("AUTO Send = %s\n",AusendOut);
#endif
    status = write_datas_gprs(sockfd,AusendOut,strlen(AusendOut));
    return  status;

}


 /*
 *************************************************************************************************************
 - 函数名称 : void * sendGps_Pthread (void * args)
 - 函数说明 : GPS发送数据线程，在主函数中加入该线程即可
 - 输入参数 : 无
 - 输出参数 : 无
 *************************************************************************************************************
 */
void * sendGps_Pthread (void * args)
{
	int ret;
	while(1)
	{
		//if((ConnectFlag == 0)&&(GPSSIG==0))
		if((is_net_connect()== 1)&&(GPSSIG==0)&&(is_server_connect()==1))
		{
            		DebugPrintf("func=%s:line=%d\n",__func__,__LINE__);
			ret=gpsWaveCOmPc(buffer0);
			if(ret<0)
			{
				printf("send gps data filed!\n");
			}
			else 
			{
				sleep(3); //有正确数据每5秒发送一次
			}
		}
		else
		{
			sleep(1);
		}
	}
}




