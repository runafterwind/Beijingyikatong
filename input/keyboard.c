/*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <asm/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <linux/input.h>

#include "../gui/InitSystem.h"

#define Diskeyboard   1

static unsigned char KeBoardPath[100];
static unsigned char KeBoardPath_2[100];

int GetInputInfo(void)
{
	int fd=NULL,res;
	char buf[100];
	char *pfile;
	char VoidInputDevStr[4];
	int VoidInputDev;
	
	VoidInputDev = 0;
	system("cat /proc/bus/input/devices | awk '/Vendor=04d9 Product=1203/{print NR}' > /tmp/info.bin;");
		
	fd = fopen("/tmp/info.bin", "rb");
	if(fd == NULL)
	{		
		printf("/tmp/info can not opened \n");
		return -1;
	}

	memset(VoidInputDevStr , 0 , sizeof(VoidInputDevStr));
	res = fread(&VoidInputDevStr,1,4,fd);	
	fclose_nosync(fd);	
	VoidInputDev = atoi(VoidInputDevStr);
	//VoidInputDev -= 0x30;
	//printf("--------VoidInputDev: %d,res:%d \n", VoidInputDev,res);		
	memset(buf,0,sizeof(buf));
	//printf("here \n");
	
	sprintf(buf, "cat /proc/bus/input/devices |awk '{if(NR==%d)print}' > /tmp/kbd;",VoidInputDev+4);

	//printf("string : %s \n", buf);

	system(buf);

	memset(buf,0,sizeof(buf));
	fd = fopen("/tmp/kbd", "r");
	if(fd == NULL)
	{		
		printf("/tmp/kbd can not opened \n");
		return -1;
	}
	res = fread(buf,100,1,fd);	
	fclose_nosync(fd);
	//printf("res : %d \n",res);
	if(res >= sizeof(buf) || strlen(buf) == 0)
	{
		printf("read kbd info error ! \n");
		return -1;
	}
	while(buf[strlen(buf)-1] < 0x30 || buf[strlen(buf)-1] > 0x39)
	{
		buf[strlen(buf)-1] = 0;
	}
	
	pfile = buf+strlen(buf)-1;
	while(pfile != buf)
	{
		if((*pfile)==0x3D)
		{			
			break;
		}
		pfile--;
	}
	if(pfile == buf)
		return -1;
	
	if(*(pfile+1) != 0)
		pfile += 1;
	else
		return -1;
	
	memset(KeBoardPath, 0, sizeof(KeBoardPath));
	sprintf(KeBoardPath, "/dev/input/%s", pfile);
	pfile += 5;
	if(*pfile == 0)
		return -1;
	//printf("******* pfile : %s \n",pfile);
	res = 0;
	while((*pfile) != 0)
	{
		res *= 10;
		res += (*pfile - 0x30);
		//printf("*pfile :%x\n", *pfile);
		pfile++;
	}
	
	printf("GetInputInfo----string is %s ,res:%d\n", KeBoardPath,res);
	if(res > 2)
	{
		memset(buf,0,sizeof(buf));
		sprintf(buf, "mknode %s c 13 %d;", KeBoardPath, res+64);
		system(buf);
	}
	
	return res;	
}

int GetInputInfo_2(void)
{
	int fd=NULL,res;
	char buf[100];
	char *pfile;
	char VoidInputDevStr[4];
	int VoidInputDev;
	
	VoidInputDev = 0;
	system("cat /proc/bus/input/devices | awk '/Vendor=0005 Product=0001/{print NR}' > /tmp/info_2.bin;");
		
	fd = fopen("/tmp/info_2.bin", "rb");
	if(fd == NULL)
	{		
		printf("/tmp/info_2 can not opened \n");
		return -1;
	}

	memset(VoidInputDevStr , 0 , sizeof(VoidInputDevStr));
	res = fread(VoidInputDevStr,1,4,fd);	
	fclose_nosync(fd);
	VoidInputDev = atoi(VoidInputDevStr);
	//VoidInputDev -= 0x30;
	//printf("--------VoidInputDev: %d,res:%d \n", VoidInputDev,res);		
	memset(buf,0,sizeof(buf));
	//printf("here \n");
	
	sprintf(buf, "cat /proc/bus/input/devices |awk '{if(NR==%d)print}' > /tmp/kbd_2;",VoidInputDev+4);

	//printf("string : %s \n", buf);

	system(buf);

	memset(buf,0,sizeof(buf));
	fd = fopen("/tmp/kbd_2", "r");
	if(fd == NULL)
	{		
		printf("/tmp/kbd_2 can not opened \n");
		return -1;
	}
	res = fread(buf,100,1,fd);	
	fclose_nosync(fd);
	//printf("res : %d \n",res);
	if(res >= sizeof(buf) || strlen(buf) == 0)
	{
		printf("read kbd_2 info error ! \n");
		return -1;
	}
	while(buf[strlen(buf)-1] < 0x30 || buf[strlen(buf)-1] > 0x39)
	{
		buf[strlen(buf)-1] = 0;
	}
	
	pfile = buf+strlen(buf)-1;
	while(pfile != buf)
	{
		if((*pfile)==0x3D)
		{			
			break;
		}
		pfile--;
	}
	if(pfile == buf)
		return -1;
	
	if(*(pfile+1) != 0)
		pfile += 1;
	else
		return -1;
	
	memset(KeBoardPath_2, 0, sizeof(KeBoardPath_2));
	sprintf(KeBoardPath_2, "/dev/input/%s", pfile);
	pfile += 5;
	if(*pfile == 0)
		return -1;
	//printf("******* pfile : %s \n",pfile);
	res = 0;
	while((*pfile) != 0)
	{
		res *= 10;
		res += (*pfile - 0x30);
		//printf("*pfile :%x\n", *pfile);
		pfile++;
	}
	
	printf("GetInputInfo_2----string is %s ,res:%d\n", KeBoardPath_2,res);
	if(res > 2)
	{
		memset(buf,0,sizeof(buf));
		sprintf(buf, "mknode %s c 13 %d;", KeBoardPath_2, res+64);
		system(buf);
	}
	
	return res;	
}

unsigned char  DateTran(const char *dat)
{
	const unsigned char *Date_Str[12] = \
	{"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	const unsigned char  Date_Hex[13] = \
	{1,2,3,4,5,6,7,8,9,10,11,12,13};
	int j;
	for(j = 0; j < 12; j++)
	{
		if(strncmp(dat,Date_Str[j],3) == 0)break;
	}
	return Date_Hex[j];
}


/*
int main (int argc, char **argv)
{
	int fd = -1;    // the file descriptor for the device
	int yalv;   // loop counter
	size_t read_bytes;   //how many bytes were read

	struct input_event ev[64];   // the events (up to 64 at once)
	//read() requires a file descriptor, so we check for one, and then open it
        // /dev/event0

	if (argc != 2)
		{
			fprintf(stderr, "usage: %s event-device - probably /dev/input/evdev0\n", argv[0]);
			exit(1);
		}

	if ((fd = open(argv[1], O_RDONLY)) < 0)
		{
			perror("evdev open");
			exit(1);
		}

	while (1)
	{
		read_bytes = read(fd, ev, sizeof(struct input_event) * 64);

		if (read_bytes < (int) sizeof(struct input_event))
			{
				perror("evtest: short read");
				exit (1);
			}

		for (yalv = 0; yalv < (int) (read_bytes / sizeof(struct input_event)); yalv++)
			{
				printf("Event: time %ld.%06ld, type %d, code %d, value %d\n",
				ev[yalv].time.tv_sec, ev[yalv].time.tv_usec, ev[yalv].type,
				ev[yalv].code, ev[yalv].value);
			}
	}

	close(fd);
	exit(0);
}
*/

//#define addrkey0  "/dev/event0"
//#define addrkey1  "/dev/event1"


#define key_index_MAX	12

//static int keyfd[key_index_MAX]={0}; /* the file descriptor for the device */
static int kusb_step = 0;
static int kusb_step_2 = 0;
//Sstatic char addrkey[30];
//static char initkeyv = 0 ,stepredkey = 1;
/*
*************************************************************************************************************
- 函数名称 : void closekey(void)
- 函数说明 : 关闭键盘
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void closekey(int *p_fd)
{
	if(*p_fd >0) {
		close(*p_fd);
	}
	*p_fd = NULL;
}
#if 0
unsigned char readdrives(unsigned char *strs,unsigned char ntype,unsigned char type)
{
	unsigned char loop;
	unsigned char stepa = ntype;

	loop = 1;
	while(loop)
	{
		switch(stepa)
		{
		case 1:
			sprintf(strs,"/dev/input/event0");
			break;

		case 2:
			sprintf(strs,"/dev/input/event1");
			break;

		case 3:
			sprintf(strs,"/dev/input/event2");
			break;
		case 4: 
			sprintf(strs,"/dev/input/event3");
			break;
/*
		case 4:
			sprintf(strs,"/dev/input/event3");
			break;		
		case 5:
			sprintf(strs,"/dev/input/event4");
			break;
		case 6:
			sprintf(strs,"/dev/input/event5");
			break;

		case 7:
			sprintf(strs,"/dev/input/event6");
			break;

		case 8:
			sprintf(strs,"/dev/input/event7");
			break;

		case 9:
			sprintf(strs,"/dev/input/event8");
			break;

		case 10:
			sprintf(strs,"/dev/input/event9");
			break;

		case 11:
			sprintf(strs,"/dev/input/event10");
			break;

		case 12:
			sprintf(strs,"/dev/input/event11");
			break;
*/			
		default:
			loop=0;
			break;

		}
		/*
		if(access(strs, 0) != 0)
		{
			if(type)
			{
				initkeyv = 0;
			}
			if(stepa >= 4)
			{
				return -1;
			}
			stepa++;
		}
		else
		{
			loop=0;
		}
		*/
		loop = 0;
	}

	return stepa;
}
#endif
unsigned char read_keyboard()
{
    int retval;
    int yalv; /* loop counter */
    fd_set rfds;
    struct timeval tv;
    size_t read_bytes; /* how many bytes were read */

    struct input_event ev[64]; /* the events (up to 64 at once) */
    unsigned char keyvalue,status;
    unsigned char addrkeybuf[30]= {0};

    keyvalue = 0xff;

    tv.tv_sec = 0;                   // the rcv wait time
    tv.tv_usec = 50000;                   // 100ms
    while(1)
    {
        FD_ZERO(&rfds);
        FD_SET(kusb_step,&rfds);
		//printf("select \n");
        retval = select(kusb_step+1,&rfds,NULL,NULL,&tv);
        if(retval == -1)
        {
            	printf("select keyboard ()  ");
		closekey(&kusb_step);	
		kusb_step = 0;
            	return -1;
        }
        else if(retval)
        {
            read_bytes= read(kusb_step,ev,sizeof(struct input_event) * 64);
            if((read_bytes < (int)sizeof(struct input_event))|| (read_bytes == -1))
            {
				printf("evtest: short read");
				closekey(&kusb_step);	
				kusb_step = 0;
				return -1;
            }
			//printf("read_bytes : %d,\n",read_bytes);
            for(yalv = 0; yalv < (int) (read_bytes / sizeof(struct input_event)); yalv++)
            {
                if((ev[yalv].type == 1)&&(ev[yalv+1].type == 0)&&(ev[yalv].value == 1))
                {
                    keyvalue = ev[yalv].code;
#if  Diskeyboard
                    printf("input value 1 %d \n",keyvalue);
#endif
                    break;
                }
            }
            //  printf("input value 2 %d \n",keyvalue);
            break;
        }
        else
        {
            return -1;
        }
    }
    return(keyvalue);
}

unsigned char read_keyboard_2()
{
    int retval;
    int yalv; /* loop counter */
    fd_set rfds;
    struct timeval tv;
    size_t read_bytes; /* how many bytes were read */

    struct input_event ev[64]; /* the events (up to 64 at once) */
    unsigned char keyvalue,status;
    unsigned char addrkeybuf[30]= {0};

    keyvalue = 0xff;

    tv.tv_sec = 0;                   // the rcv wait time
    tv.tv_usec = 50000;                   // 100ms
    while(1)
    {
        FD_ZERO(&rfds);
        FD_SET(kusb_step_2,&rfds);
		//printf("select \n");
        retval = select(kusb_step_2+1,&rfds,NULL,NULL,&tv);
        if(retval == -1)
        {
            	printf("select keyboard ()  ");
		closekey(&kusb_step_2);	
		kusb_step_2 = 0;
            	return -1;
        }
        else if(retval)
        {
            read_bytes= read(kusb_step_2,ev,sizeof(struct input_event) * 64);
            if((read_bytes < (int)sizeof(struct input_event))|| (read_bytes == -1))
            {
                	printf("evtest: short read");
			closekey(&kusb_step_2);	
			kusb_step_2 = 0;
                	return -1;
            }
			//printf("read_bytes : %d,\n",read_bytes);
            for(yalv = 0; yalv < (int) (read_bytes / sizeof(struct input_event)); yalv++)
            {
                if((ev[yalv].type == 1)&&(ev[yalv+1].type == 0)&&(ev[yalv].value == 1))
                {
                    keyvalue = ev[yalv].code;
#if  Diskeyboard
                    printf("input value 1 %d \n",keyvalue);
#endif
                    break;
                }
            }
            //  printf("input value 2 %d \n",keyvalue);
            break;
        }
        else
        {
            return -1;
        }
    }
    return(keyvalue);
}


/*
*************************************************************************************************************
- 函数名称 : unsigned char updatekey(void)
- 函数说明 : 打开键盘和读键
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
//extern int thread_timer;
extern int GetInputInfo(void);
unsigned char updatekey_test(void){
	printf("updatekey \n");
	return 0xff;
}

#include <sys/stat.h>
static int fd2 = -1;
static char addrkey2[30];
static char initkeyv2 = 0 ,stepredkey2 = 1;


unsigned char updatekey_2(void)
{
	unsigned char  keyvalue = 0xff;
	unsigned int i;
	struct stat buf={0};
	static unsigned char bflg = 0;
	if(!kusb_step_2)
	{
		system("cat  /proc/bus/input/devices |grep \"Vendor=0005 Product=0001\" > /tmp/id_2");	
		stat("/tmp/id_2", &buf);
		if(!buf.st_size){
			bflg = 0;
			return -1;
		}
		else
		{
			if(!bflg){
				bflg = 1;	
				if(GetInputInfo_2() != -1)
				{
					if((kusb_step_2 = open(KeBoardPath_2, O_RDONLY)) < 0)
					{
						printf("Can not open %s \n", KeBoardPath_2);
						kusb_step_2 = 0;
						return -1;
					}
				}
			}
			else
			{
				//if(thread_timer)
				//	return -1;
			}		
		}
	}
	else
	{	
		keyvalue = read_keyboard_2();
		if(keyvalue != 0xff)
		{
			beepopen(1);				
		}		
	}

	return  (keyvalue);
}


unsigned char updatekey(void)
{
	unsigned char  keyvalue = 0xff;
	unsigned int i;
	struct stat buf={0};
	static unsigned char bflg = 0;
	if(!kusb_step)
	{
		system("cat /proc/bus/input/devices |grep \"Vendor=04d9 Product=1203\" > /tmp/id");	
		stat("/tmp/id", &buf);
		if(!buf.st_size){
			bflg = 0;
			//printf("clear over ! \n");
			return -1;
		}
		else
		{
			if(!bflg){
				//thread_timer = 10;
				bflg = 1;
				//return -1;
				//CreateNode(VoidInputDev^1);	
				if(GetInputInfo() != -1)
				{
					if((kusb_step = open(KeBoardPath, O_RDONLY)) < 0)	//打开设备节点
					{
						printf("Can not open %s \n", KeBoardPath);
						kusb_step = 0;
						return -1;
					}
					//printf("kusb_step : %d \n",kusb_step);
				}
			}
			else
			{
				//if(thread_timer)
				//	return -1;
			}		
		}
	}
	else
	{		//stepredkey = kusb_step;
			//printf("---start read \n");
			//keyvalue = read_once();
			keyvalue = read_keyboard();
			//printf("***keyvalue:%d---\n",keyvalue);
			if(keyvalue != 0xff)
			{
				beepopen(1);				
			}		
	}
#if 0
	if(kusb_step) {			//finded usb key index
		printf("------ kusb_step : %d ------\n",kusb_step);
		stepredkey = readdrives(addrkey,kusb_step,1);

		if(keyfd[stepredkey-1] == 0) {
			if((keyfd[stepredkey-1] = open(addrkey, O_RDONLY|O_NONBLOCK)) < 0)
			{
//				printf("%s open1 fail\n",addrkey);
				keyfd[stepredkey-1] =0;
				return -1;
			}
		}
		keyvalue = read_keyboard();
		if(keyvalue != 0xff)
		{
			beepopen(1);
		}

	}
	else {
		printf("------ stepredkey : %d ------\n",stepredkey);
		stepredkey = readdrives(addrkey,stepredkey,1);

		if(stepredkey != 0xff)
		{

			if(keyfd[stepredkey-1] == 0) {
				if((keyfd[stepredkey-1] = open(addrkey, O_RDONLY|O_NONBLOCK)) < 0)
				{
//					printf("%s open fail\n",addrkey);
					keyfd[stepredkey-1] =0;
					return -1;
				}
			}
			/*
					if(initkeyv == 0)
					{
						//closekey();
						if((keyfd = open(addrkey, O_RDONLY)) < 0)
						{
							perror("evdev open Err");
							initkeyv =0;
							// closekey();
							return -1;
						}
						initkeyv ++;
					}
			*/
			keyvalue = read_keyboard();
			if(keyvalue != 0xff)
			{
				beepopen(1);
			}

		}
		else
		{
			stepredkey = 1;
		}
	}
#endif	
	return  (keyvalue);
}

