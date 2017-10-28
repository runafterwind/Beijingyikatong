#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "psam.h"
#include "../include/apparel.h"

#define  PSAMDIS  0
#define  MI_OK   0


int psamfd;

const char psam_path[4][36]={
	"/dev/psam1",
	"/dev/psam2",
	"/dev/psam3",
	"/dev/psam4",	
};

int  Psamreceive_len[1] = {0};
 char Psamreceive_buf[128]= {0};

extern unsigned char PsamNum[6];
extern unsigned char PsamKeyIndex;

int read_t(int fd, void *buf, int nbytes, unsigned int timout)
{
	fd_set			readfds;
	int				val;
	struct timeval	tv;
	tv.tv_sec = timout;
	tv.tv_usec = 0;
	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);
	val = select(fd+1, &readfds, NULL, NULL, &tv);
	if (val <= 0) {
		return -1;
	}
	return (read(fd, buf, nbytes));
}

char checkpsam(int index)
{
    char i,j;
    int fd;
    int ret=0,val;
    int result = 1;
    char bound[2]={BAUD_38400,BAUD_9600};

    psamfd =-1;
    psamfd=open(psam_path[index],O_RDWR);    	
    printf("psam_path=%s:line=%d\n",psam_path[index],__LINE__);
	if(psamfd<0)
	{
    		printf("Can't open /dev/psam \n");
    		return -1;
	}
    
    for(i=0;i<2;i++)
    {
        ret = 0;
        ret+=ioctl(psamfd, BAUD_CMD,bound[i]);	
   //     printf("bound[%d]=%d\n",i,bound[i]);
   //     printf("ret1 = %d:line= %d\n",ret,__LINE__);
    	ret+=ioctl(psamfd, SEL_CARD,1);
   //     printf("ret2 = %d:line = %d\n",ret,__LINE__);
    	ret += ioctl(psamfd, COLD_RESET);    //复位PSAM卡，RATS
  //      printf("ret3 = %d:line = %d\n",ret,__LINE__);
    	if(ret == 0)
     	{     
            ret = 1;
    		ioctl(psamfd, RECEIVE_LEN, Psamreceive_len);         //取复位数据长度
    //		printf("ret = %d:Psamreceive_len = %d\n",ret,Psamreceive_len[0]);
         	if(Psamreceive_len[0]>0)
    		{
                
                 if((val = read_t(psamfd, Psamreceive_buf, *Psamreceive_len, 8)) < 0) 
            		{
                    //    printf("val=%d\n",val);
            			return -1;	
            		}
                 else
            		{
#if PSAMDIS                                           

                        for(j = 0; j < Psamreceive_len[0]; j++)
                        {
                        printf("%02X",Psamreceive_buf[i]);
                        }
                        printf("\n");
                    
#endif
            			ret = GetPsamID();
            		}
                
                }
     	}
    //    printf("ret = %d:line = %d\n",ret,__LINE__);
        if(ret == 0)
           {
            result = 0;
            break;
            }
                 
       }
    if(result==1)
        close(psamfd);
    return result;
    }


//psam卡初始化
char InitPsam(void)
{
    	int ret=0,val;
		printf("开始读psam卡初始化\n");
/*        
    	psamfd=open("/dev/psam1",O_RDWR);
    	//psamfd=open("/dev/psam3",O_RDWR);
    	if(psamfd<0)
    	{
        		printf("Can't open /dev/psam \n");
        		return -1;
    	}

    	//ioctl(psamfd, BAUD_CMD,3);        //设置波特率  38400   BAUD_9600

#if (defined (YAN_AN_BUS) || defined (NINGXIA_GUYUAN))
    	ret+=ioctl(psamfd, BAUD_CMD,BAUD_9600); //设置波特率  9600  Test
#else
        ret+=ioctl(psamfd, BAUD_CMD,BAUD_38400); //设置波特率  38400  
    //  ret+=ioctl(psamfd, BAUD_CMD,BAUD_9600);
#endif   
	//printf("1...ret=%d\n",ret);
    	ret+=ioctl(psamfd, SEL_CARD,1);         //选择PSAM卡槽
	//printf("2...ret=%d\n",ret);
    	ret += ioctl(psamfd, COLD_RESET);    //复位PSAM卡，RATS
	//printf("3...ret=%d\n",ret);
    
    	if(ret == 0)
     	{     
        		ret = ioctl(psamfd, RECEIVE_LEN, Psamreceive_len);         //取复位数据长度
        		printf("ret = %d:Psamreceive_len = %d\n",ret,Psamreceive_len[0]);
		if((val = read_t(psamfd, Psamreceive_buf, *Psamreceive_len, 8)) < 0) 
		{
			return -1;	
		}
		else
		{
			ret = GetPsamID();
		}
     	}
    
	printf("InitPSAM ret = %d\n",ret);
*/
        int i;
        for(i=0;i<4;i++)
        {
            ret = checkpsam(i);
            if(ret==0)
                break;
            }


    	return ret;
}


void ClosePsam(void)
{
    close(psamfd);
}


unsigned char PsamCos(char *Intdata, char *Outdata,unsigned char *len)
{
    int ret;
    unsigned char lens;

    lens = *len;

#if PSAMDIS
    {
        unsigned char i;
        printf("\n PsamCos  in:%02d\n",lens);
        for(i = 0; i<lens; i++)
        {
            printf("%02X",Intdata[i]);
        }
        printf("\n");
    }
#endif
    memset(Psamreceive_buf,0,sizeof(Psamreceive_buf));
    ret = write(psamfd,Intdata,lens);//TCOS命令
    if(ret >= MI_OK)
    {
        ioctl(psamfd, RECEIVE_LEN, Psamreceive_len);
        read(psamfd, Psamreceive_buf, Psamreceive_len[0]);

       // if(Psamreceive_len[0] == 2) {
            memcpy(Outdata,Psamreceive_buf,Psamreceive_len[0]);
            *len = Psamreceive_len[0];
      //  }
       /* else {
            memcpy(Outdata,Psamreceive_buf+1,(Psamreceive_len[0]-1));
            *len = (Psamreceive_len[0]-1);
        }*/
        ret = MI_OK;
    }


#if PSAMDIS
    {
        unsigned char i;
        printf(" PsamCos  out:%02d:ret=%d:Psamreceive_len[0]=%d\n",*len,ret,Psamreceive_len[0]);

        for(i = 0; i < Psamreceive_len[0]; i++)
        {
            printf("%02X",Psamreceive_buf[i]);
        }
        printf("\n");
    }
#endif

    return ret;
}


unsigned char GetPsamID(void)
{
    	unsigned char status = 1;
    	unsigned char Loop = 1;
    	unsigned char step = 1;
    	unsigned char len;
    	char buff[64];
    	char getpsamnum[] = {0x00,0xb0,0x96,0x00,0x06};

    	char selectapp[]  = {0x00,0xa4,0x00,0x00,0x02,0x80,0x11};

        char selectindex[] = {0x00,0xb0,0x97,0x00,0x01};

    	while(Loop)
    	{
        		switch(step)
        		{
        			case 1:
            			memset(buff,0,sizeof(buff));
				len = sizeof(getpsamnum);
				status = PsamCos(getpsamnum,buff,&len);
				if((status == MI_OK)&&(buff[len-2]== 0x90)&&(buff[len-1]== 0x00))
				{
                				memcpy(PsamNum,buff,6);
                				step ++;
            			}
            			else
            			{
                				Loop  = 0;
            			}
            			break;

        			case 2:
            			memset(buff,0,sizeof(buff));
            			len = sizeof(selectapp);
            			status = PsamCos(selectapp,buff,&len);
            			if((status == MI_OK)&&(buff[len-2]== 0x90)&&(buff[len-1]== 0x00))
            			{
                				step ++;
            			}
            			else
            			{
                				Loop  = 0;
            			}
            			break;

        			case 3:                        
			//	PsamKeyIndex = 0x01;
			//	step ++;
			            memset(buff,0,sizeof(buff));
            			len = sizeof(selectindex);
            			status = PsamCos(selectindex,buff,&len);
            			if((status == MI_OK)&&(buff[len-2]== 0x90)&&(buff[len-1]== 0x00))
            			{
                                PsamKeyIndex= buff[0];
                				step ++;
            			}
            			else
            			{
                				Loop  = 0;
            			}
            			break;
            			

        			case 4:
            			step = 0;
            			Loop = 0;
            			break;

        			default:
            			Loop = 0;
            			break;
        		}
    	}

    	printf(" GetPsamID step== %d  \n",step);

    	return step;
}



unsigned char PsamApply(void)
{
    unsigned char status = 1,len;
    char buff[64];
    char HangTwo[] = {0x00,0xa4,0x00,0x00,0x02,0x10,0x03};

    len = sizeof(HangTwo);
    status = PsamCos(HangTwo,buff,&len);
    if((status == 0)&&(buff[len-2]== 0x90)&&(buff[len-1]== 0x00))
    {
        status = 0;
    }
    else
    {
        status = 1;
    }

#if PSAMDIS
    printf(" PsamApply == %d::------------->\n",status);
#endif

    return status;
}


unsigned char PsamTacFst(void)
{
    unsigned char status = 1,len;

    char buff[50];
    char Tacbin[]= {0x80,0x1a,0x28,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

    len = sizeof(Tacbin);
    status = PsamCos(Tacbin,buff,&len);
    if((status == MI_OK)&&(buff[len-2]== 0x90)&&(buff[len-1]== 0x00))
    {
        status = 0;
    }
    else
    {
        status = 1;
    }

#if PSAMDIS
    printf(" PsamTacFst == %d::------------->\n",status);
#endif

    return status;
}

int16_t menu_tda8007_open(void)
{

  psamfd=open("/dev/psam3",O_RDWR);
  if(psamfd<0)
  {
	  printf("Can't open /dev/psam3 \n");
	  return -1;
  }
  
return 0;
}




int16_t menu_tda8007_receiveResponse(uint8_t *recvdata)
{
	int16_t ret;

	ret = ioctl(psamfd, RECEIVE_LEN, Psamreceive_len);
	if (ret != 0) return -1;
	
	ret = read(psamfd, Psamreceive_buf, Psamreceive_len[0]);
	if (ret != 0) return -1;
	
    memcpy(recvdata,Psamreceive_buf,Psamreceive_len[0]);
	
    return 0;
}


/**
 * \brief   Power up the currently selected smartcard.
 * \param   mode Powerup mode to follow (e.g. POWERUP_ISO or POWERUP_EMV)
 * \param   voltage Powerup voltage to use (POWERUP_5V, POWERUP3V or POWERUP_1p8V)
 * \return  0 for success or <0 for failure
 */
int16_t menu_tda8007_coldreset(void)
{
  return ioctl(psamfd, COLD_RESET);
}

/**
 * \brief   Warm reset the currently selected smartcard.
 * \param   mode Powerup mode to follow (e.g. POWERUP_ISO or POWERUP_EMV)
 * \return  0 for success or <0 for failure
 */
int16_t menu_tda8007_warmreset(void)
{
	return ioctl(psamfd, WARM_RESET);
}

/**
 * \brief   Power down the currently selected smartcard.
 * \return  0 for success or <0 for failure
 */
int16_t menu_tda8007_powerdown(void)
{
	return ioctl(psamfd, POWER_DOWN);
}

/**
 * \brief   Select a smartcard slot to use.
 * \param   slot Slot to use.  1, 2, or 3.
 * \return  0 for success or <0 if slot not in range
 */
int16_t menu_tda8007_selectcard(uint8_t slot)
{
	return ioctl(psamfd, SEL_CARD, slot);
}

// set card baudrate
/*
0x01: 115200
0x02: 57600
0x03: 38400
0x06: 19200
0x0C: 9600
0x18: 4800
*/
int16_t menu_tda8007_setbaudrate(uint8_t val)
{
	return ioctl(psamfd, BAUD_CMD, val);
}

/**
 * \brief   Send an APDU to currently selected smartcard.
 *
 * Will choose to call either menu_tda8007_sendAPDUT0 or T1 based on TD
 * protocol selection of ATR.
 * 
 * \param   buffer Holds the Command APDU to send
 * \param   length Length of Command APDU
 * \param   rbuffer Location to put Response APDU
 * \return  Length of APDU buffer returned or <0 for failure
 */
int16_t menu_tda8007_send(uint8_t *buffer,int16_t length)
{
	return write(psamfd, buffer, length);
}

int16_t menu_tda8007_checkpresence(uint8_t slot)
{
	return ioctl(psamfd, CHECK_PRESENCE, slot);
}








