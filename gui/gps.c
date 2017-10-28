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

#define FALSE 0
#define TRUE  1
extern unsigned char ConnectFlag; //������־
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
 - �������� : void set_speedgps (int fd, int speed)
 - ����˵�� : ��������GPS�Ĵ��ڲ�����
 - ������� : fd :�����豸�����speed�������ʣ�һ������Ϊ9600
 - ������� : ��
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
 - �������� : int set_Paritygps (int fd, int databits, int stopbits, int parity)
 - ����˵�� : ��������GPS�Ĵ�������
 - ������� : fd :�����豸�����databits������λ��һ��Ϊ8;stopbits��ֹͣλ��һ������Ϊ'N'��parity��У��λ��һ������Ϊ1
 - ������� : ��
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
 - �������� : int OpenDevgps (char *Dev)
 - ����˵�� : ��GPSʹ�õĴ���
 - ������� : Dev :�豸ָ��
 - ������� : ��
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
 - �������� : void CloseDevgps (int fd)
 - ����˵�� : �ر�GPSʹ�õĴ���
 - ������� : fd :�����豸���
 - ������� : ��
 *************************************************************************************************************
 */
void CloseDevgps (int fd)
{
	close (fd);
}

/*
 *************************************************************************************************************
 - �������� : int read_datas_ttygps(int fd,char *rcv_buf,int *len)
 - ����˵�� : ����������
 - ������� : fd :�����豸�����rcv_buf:�������ݻ�����ָ�룻len���������ݳ���ָ��
 - ������� : ��
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
		else      //��ʱ
		{
				return 1;
		}
	}
	return 0;
}
/*
 *************************************************************************************************************
 - �������� : int initializegps (const char *com, int speed)
 - ����˵�� : ��ʼ������
 - ������� : com:��������ָ�룻speed:�����ʣ�һ��Ϊ9600
 - ������� : ��
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
 - �������� : void uninitializegps (int fd)
 - ����˵�� : ж�س�ʼ������
 - ������� : fd:�����豸ָ��
 - ������� : ��
 *************************************************************************************************************
 */
void uninitializegps (int fd)
{
	CloseDevgps(fd);
}

#if 1
/*
 *************************************************************************************************************
 - �������� : int gps_date_GGA(char *string,char *okdate)
 - ����˵�� : ��ѯGGA�ź��ַ���
 - ������� : *string:���յ����ַ�����*okdate:��Ҫ��ѯ���ַ���
 - ������� : 0��-1
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
 - �������� : int gps_date_GGA(char *string,char *okdate)
 - ����˵�� : ��ѯRMC�ź��ַ���
 - ������� : *string:���յ����ַ�����*okdate:��Ҫ��ѯ���ַ���
 - ������� : 0��-1
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
 - �������� : int search_char(char *str,char ch)
 - ����˵�� : ���ַ����в�ѯĳһ���ַ�
 - ������� : *str:�ַ���ָ�룻ch����Ҫ���ҵ��ַ�
 - ������� : ��
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




static u8 NMEA_CMD=NMEA_NULL;         //NMEA ���
static u8 NMEA_CMD_Buff[]="$GPxxx,"; //NMEA ������ͻ���
static u8 NMEA_CMD_Index=0;         //��ȡ CMD�ַ��ĸ���
        //CMD���ͽ������
static u8 NMEA_DAT_Block=0;         //NMEA �����ֶκ� ��0��ʼ
static u8 NMEA_DAT_BlockIndex=0;     //NMEA ����ÿ���ֶ����ַ����� ��0��ʼ
            //���ݽ������. ���һ�� GPRMC ��䷢�������1,

static u8 ucTempA=0;                 //�洢������λ�����õĵ�ʮλ��ʱ����
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
 - �������� : void ShowGPSTime(void) 
 - ����˵�� :  gpsʱ��utc ת����ʱ��
 - ������� : ��
 - ������� : ��
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
 - �������� : void ParserGPGGA(void)   
 - ����˵�� :  GPS gga��λ��Ϣ
 - ������� : ��
 - ������� : ��
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
	signal(SIGPIPE,SIG_IGN);  //�ر�SIGPIPE�źţ�������
	read_datas_ttygps(gps_fd,rx_string,&iLen);
	if(iLen>0)
	{
		bzero(lcdtest,sizeof(lcdtest));
	    if(gps_date_GGA(rx_string,lcdtest) == 0)
		{
			printf("%s\n",lcdtest);
			comma[0] = strstr(lcdtest, ",");  //UTC ʱ�䣬hhmmss(ʱ����)��ʽ
			if(NULL!=comma[0])
			{
				for(i=1;i<6;i++)
				{
					comma[i] = strstr(comma[i-1]+1, ",");  
					if(NULL==comma[i])
					{
						printf("GPGGA��Ϣ������\n");
						return;
					}
				}
			}
			else
			{
				printf("GPGGA��Ϣ������\n");
				return;
			}
#if 0			
			comma[0] = strstr(lcdtest, ",");        //UTC ʱ�䣬hhmmss(ʱ����)��ʽ
			comma[1] = strstr(comma[0]+1, ",");		//γ�ȣ���ʽΪddmm.mmmm(��һλ����Ҳ������)��
			comma[2] = strstr(comma[1]+1, ",");     //γ�Ȱ���N(������)��S(�ϰ���)
			comma[3] = strstr(comma[2]+1, ",");     //����dddmm.mmmm(�ȷ�)��ʽ(ǰ���0Ҳ��������)
			comma[4] = strstr(comma[3]+1, ",");     //���Ȱ���E(����)��W(����)
			comma[5] = strstr(comma[4]+1, ",");     //��λ����ָʾ��0=��λ��Ч��1=��λ��Ч��
			comma[6] = strstr(comma[5]+1, ",");     //ʹ��������������00��12(��һ����Ҳ������)
			comma[7] = strstr(comma[6]+1, ",");     //ˮƽ��ȷ�ȣ�0.5��99.9
			comma[8] = strstr(comma[7]+1, ",");     //�����뺣ƽ��ĸ߶ȣ�-9999.9��9999.9��Mָ��λ��
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

							//ʱ������
			time=atoi(GPSBF_GGA_DATA.UTCTime);
				
			GPS_GGA_Data.UTCTime[0]=(u8)((time/10000)%100);
			GPS_GGA_Data.UTCTime[1]=(u8)((time/100)%100);
			GPS_GGA_Data.UTCTime[2]=(u8)(time%100);

			GPS_GGA_Data.UTCTime[0]+=8;
			if (GPS_GGA_Data.UTCTime[0]>23)
			{
				GPS_GGA_Data.UTCTime[0]-=24;
			}

							//γ��
			fn=atof(GPSBF_GGA_DATA.Latitude);
			GPS_GGA_Data.Latitude[0]=(u8)(fn/100);
			GPS_GGA_Data.Latitude[1]=(u8)(((int)fn)%100);
			GPS_GGA_Data.Latitude[2]=(u8)((fn-(float)((int)fn))/0.01);
			GPS_GGA_Data.Latitude[3]=(u8)((int)((fn-(float)((int)fn))/0.0001)%100);
			//����
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
			if(1==status)           //��Ч��λ
			{			
				printf(" GPGGA GPS��Ч��λ��Ϣ:\n");
			}
			else
			{
				printf("GPGGA GPS��Ч��λ��Ϣ:\n");
			}
#endif
			printf("GGA GPS��λ��Ϣ:\n");
printf("γ��: %d%d.%d%d    %c\n",GPS_GGA_Data.Latitude[0],GPS_GGA_Data.Latitude[1],GPS_GGA_Data.Latitude[2],GPS_GGA_Data.Latitude[3],GPS_GGA_Data.NS[0]);
printf("����: %d%d%d.%d%d   %c\n",GPS_GGA_Data.Longitude[0],GPS_GGA_Data.Longitude[1],GPS_GGA_Data.Longitude[2],GPS_GGA_Data.Longitude[3],GPS_GGA_Data.Longitude[4],GPS_GGA_Data.EW[0]);
			printf("������: %d\n",5);
			printf("ʱ��: %d:%d:%d\n",GPS_GGA_Data.UTCTime[0],GPS_GGA_Data.UTCTime[1],GPS_GGA_Data.UTCTime[2]);						
		}	
	}
		
return;
}



 
 /*
 *************************************************************************************************************
 - �������� : int write_datas_gprs(int fd, unsigned char *buffer, int buf_len)
 - ����˵�� : �򴮿ڷ�������
 - ������� : fd:�����豸�����buffer:��Ҫ���͵����ݻ�����ָ�룻buf_len�����ݳ���
 - ������� : ��
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
 - �������� : int write_datas_gprs(int sockfd, unsigned char *buffer, int buf_len)
 - ����˵�� : д SOCKET ����
 - ������� : socketfd:��������buffer:��Ҫ���͵����ݻ�����ָ�룻buf_len�����ݳ���
 - ������� : ��
 *************************************************************************************************************
 */
 static int write_datas_gps(int fd, unsigned char *buffer, int buf_len)
 {
	 int status;
 
 //  printf("write_datas_gprs buf_len :%d\n",buf_len);
	 
	 pthread_mutex_lock(&m_socketwrite); //�߳�����
	 status = write_datas_tty(fd,buffer,buf_len);
	 pthread_mutex_unlock(&m_socketwrite);//�߳̽���
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
 - �������� :void ParserGPRMC(void)
 - ����˵�� : ����RMC���ݸ�ʽ
 - ������� : ��
 - ������� : ��
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
	signal(SIGPIPE,SIG_IGN);  //�ر�SIGPIPE�źţ�������
	read_datas_ttygps(gps_fd,rx_string,&iLen);
/*
	printf("GPS��ȡ�������ݸ���:%d\n",iLen);
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
			comma[0] = strstr(lcdtest, ",");  //UTC ʱ�䣬hhmmss(ʱ����)��ʽ
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
			//	printf("GPRMC��Ϣ������2\n");
				return 0;
			}
			memcpy(GPSBF_RMC_DATA.UTCTime,comma[0]+1,(comma[1]-comma[0]-1));	//utc time
			memcpy(GPSBF_RMC_DATA.Status,comma[1]+1,(comma[2]-comma[1])-1);		//��λ״̬
			memcpy(GPSBF_RMC_DATA.Latitude,comma[2]+1,(comma[3]-comma[2]-1));	//γ��
			memcpy(GPSBF_RMC_DATA.NS,comma[3]+1,(comma[4]-comma[3]-1));			//����
			memcpy(GPSBF_RMC_DATA.Longitude,comma[4]+1,(comma[5]-comma[4]-1));	//����
			memcpy(GPSBF_RMC_DATA.EW,comma[5]+1,(comma[6]-comma[5]-1));			//����
			memcpy(GPSBF_RMC_DATA.Speed,comma[6]+1,(comma[7]-comma[6]-1));
			memcpy(GPSBF_RMC_DATA.Course,comma[7]+1,(comma[8]-comma[7]-1));
			memcpy(GPSBF_RMC_DATA.UTCDate,comma[8]+1,(comma[9]-comma[8]-1));	// utc date

			memcpy(GPS_RMC_Data.Status,GPSBF_RMC_DATA.Status,sizeof(GPSBF_RMC_DATA.Status));


			memcpy(GPS_RMC_Data.EW,GPSBF_RMC_DATA.EW,sizeof(GPSBF_RMC_DATA.EW));
			memcpy(GPS_RMC_Data.NS,GPSBF_RMC_DATA.NS,sizeof(GPSBF_RMC_DATA.NS));
				
			//ʱ������
			time=atoi(GPSBF_RMC_DATA.UTCTime);
			date=atoi(GPSBF_RMC_DATA.UTCDate);
			
			GPS_RMC_Data.UTCDateTime[3]=(u8)((time/10000)%100);
			GPS_RMC_Data.UTCDateTime[4]=(u8)((time/100)%100);
			GPS_RMC_Data.UTCDateTime[5]=(u8)(time%100);
			GPS_RMC_Data.UTCDateTime[2]=(u8)((date/10000)%100);
			GPS_RMC_Data.UTCDateTime[1]=(u8)((date/100)%100);
			GPS_RMC_Data.UTCDateTime[0]=(u8)(date%100);
			//γ��
			fn=atof(GPSBF_RMC_DATA.Latitude);
			GPS_RMC_Data.Latitude[0]=(u8)(fn/100);
			GPS_RMC_Data.Latitude[1]=(u8)(((int)fn)%100);
			GPS_RMC_Data.Latitude[2]=(u8)((fn-(float)((int)fn))/0.01);
			GPS_RMC_Data.Latitude[3]=(u8)((int)((fn-(float)((int)fn))/0.0001)%100);
			//����
			fe=atof(GPSBF_RMC_DATA.Longitude);
			GPS_RMC_Data.Longitude[0]=(u8)(fe/10000);
			GPS_RMC_Data.Longitude[1]=(u8)(((int)(fe/100))%100);
			GPS_RMC_Data.Longitude[2]=(u8)(((int)fe)%100);
			GPS_RMC_Data.Longitude[3]=(u8)((fe-(float)((int)fe))/0.01);
			GPS_RMC_Data.Longitude[4]=(u8)((int)((fe-(float)((int)fe))/0.0001)%100);

			//����
			sp=atof(GPSBF_RMC_DATA.Speed);
			GPS_RMC_Data.Speed[0]=(u8)(sp/100);
			GPS_RMC_Data.Speed[1]=(u8)(((int)sp)%100);
			GPS_RMC_Data.Speed[2]=(u8)((sp-(float)((int)sp))/0.01);

			//����
			co=atof(GPSBF_RMC_DATA.Course);
			GPS_RMC_Data.Course[0]=(u8)(co/100);
			GPS_RMC_Data.Course[1]=(u8)(((int)co)%100);
			GPS_RMC_Data.Course[2]=(u8)((co-(float)((int)co))/0.01);
			ShowGPSTime();
			
			if(0 == strcmp(GPS_RMC_Data.Status,"A"))           //��Ч��λ
			{			
				//printf("GPRMC GPS��Ч��λ��Ϣ:\n");
			}
			else
			{
				//printf("GPRMC GPS��Ч��λ��Ϣ:\n");
                memset(GPS_RMC_Data.UTCDateTime,0,sizeof(stru_GPSRMC));
                return 0;
			}
			TempPosition = (GPS_RMC_Data.Latitude[1] + GPS_RMC_Data.Latitude[2]*0.01 + GPS_RMC_Data.Latitude[3]*0.0001)/60;
			DUSUGAO_GPS_latitude = GPS_RMC_Data.Latitude[0] + TempPosition;
			
			TempPosition = (GPS_RMC_Data.Longitude[2] + GPS_RMC_Data.Longitude[3]*0.01 + GPS_RMC_Data.Longitude[4]*0.0001)/60;
			DUSUGAO_GPS_longitude = GPS_RMC_Data.Longitude[0]*100 + GPS_RMC_Data.Longitude[1] + TempPosition;
#if 0			
			printf("ԭʼRMC GPS��λ��Ϣ:\n");
			GPS_latitude = GPS_RMC_Data.Latitude[0] + GPS_RMC_Data.Latitude[1]*0.01 + GPS_RMC_Data.Latitude[2]*0.0001 + GPS_RMC_Data.Latitude[3]*0.000001;
			GPS_longitude = GPS_RMC_Data.Longitude[0]*100 + GPS_RMC_Data.Longitude[1] + GPS_RMC_Data.Longitude[2]*0.01 + GPS_RMC_Data.Longitude[3]*0.0001 + GPS_RMC_Data.Longitude[4]*0.000001;
			printf("γ��: %f %c\n", GPS_latitude, GPS_RMC_Data.NS[0]);
			printf("����: %f %c\n", GPS_longitude, GPS_RMC_Data.EW[0]);
			
			//DebugPrintf("γ��: %d%d.%d%d    %c\n",GPS_RMC_Data.Latitude[0],GPS_RMC_Data.Latitude[1],GPS_RMC_Data.Latitude[2],GPS_RMC_Data.Latitude[3],GPS_RMC_Data.NS[0]);
			//DebugPrintf("����: %d%d%d.%d%d   %c\n",GPS_RMC_Data.Longitude[0],GPS_RMC_Data.Longitude[1],GPS_RMC_Data.Longitude[2],GPS_RMC_Data.Longitude[3],GPS_RMC_Data.Longitude[4],GPS_RMC_Data.EW[0]);
			printf("����: %d%d.%d\n",GPS_RMC_Data.Speed[0],GPS_RMC_Data.Speed[1],GPS_RMC_Data.Speed[2]);
			//DebugPrintf("����: %d%d.%d\n",GPS_RMC_Data.Course[0],GPS_RMC_Data.Course[1],GPS_RMC_Data.Course[2]);
			//DebugPrintf("ʱ��: 20%d-%d-%d %d:%d:%d\n",GPS_RMC_Data.UTCDateTime[0],GPS_RMC_Data.UTCDateTime[1],GPS_RMC_Data.UTCDateTime[2],GPS_RMC_Data.UTCDateTime[3],GPS_RMC_Data.UTCDateTime[4],GPS_RMC_Data.UTCDateTime[5]);
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

/* ====GPS�ź���ʾ==== */

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
- �������� : char * Rd_time (char* buff)
- ����˵�� : ��ʱ��
- ������� : ��
- ������� : ��
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
 - �������� : void * Gps_Pthread (void * args)
 - ����˵�� : GPS�����̣߳����������м�����̼߳���
 - ������� : ��
 - ������� : ��
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
    		//printf("��ǰ��ַ%s\n",buff1);
    		if(buff1[1] != 0 && buff1[2] != 0 && buff1[3] != 0)      //�����ղ�Ϊ0
    		{
    			if((buff1[7]==78||buff1[7]==83)&&(buff1[14]==69||buff1[14]==87))    //γ�Ⱦ��ȱ���ΪN S E W
    			{
    				if((buff1[8]!=0||buff1[9]!=0||buff1[10]!=0||buff1[11]!=0)&&(buff1[15]!=0||buff1[16]!=0||buff1[17]!=0||buff1[18]!=0))//��γ�Ȳ�Ϊ0
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

unsigned char G_STARTWGPS[8];  //ȫ�ֱ���
/*
*************************************************************************************************************
- �������� : char WaveCOmPc(unsigned char *Date)
- ����˵�� : �����Զ��ϴ�GPS ����
- ������� : ��
- ������� : ��
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
 - �������� : void * sendGps_Pthread (void * args)
 - ����˵�� : GPS���������̣߳����������м�����̼߳���
 - ������� : ��
 - ������� : ��
 *************************************************************************************************************
 */
void * sendGps_Pthread (void * args)
{
	int ret;
	while(1)
	{
		if((ConnectFlag == 0)&&(GPSSIG==0))
		{
            DebugPrintf("func=%s:line=%d\n",__func__,__LINE__);
			ret=gpsWaveCOmPc(buffer0);
			if(ret<0)
			{
				printf("send gps data filed!\n");
			}
			else 
			{
				sleep(3); //����ȷ����ÿ5�뷢��һ��
			}
		}
		else
		{
			sleep(1);
		}
	}
}




