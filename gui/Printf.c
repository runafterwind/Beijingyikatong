#include <apparel.h>
#include "RC500.h"
#include "InitSystem.h"
#include "des.h"
#include "OnlineRecharge.h"

#define  PrintDis  1


int speed_arr[] =
{
    B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300,
    B38400, B19200, B9600,  B4800, B2400, B1200,B600, B300,
};

int name_arr[] =
{
    115200, 38400, 19200, 9600, 4800, 2400, 1200, 300,
    38400, 19200, 9600, 4800, 2400,  1200,600, 300,
};

/*
*************************************************************************************************************
- 函数名称 : int OpenDev (char *Dev)
- 函数说明 : 设置串口属性
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void set_speed (int fd, int speed)
{
    int i;
    int status;
    struct termios Opt;
    tcgetattr (fd, &Opt);
    for (i = 0; i < sizeof (speed_arr) / sizeof(int); i++)
    {
        if(speed == name_arr[i])
        {
            tcflush (fd, TCIOFLUSH);
            if(cfsetispeed (&Opt, speed_arr[i])==0)
            {
#if PrintDis
                printf("set_speed sucess \n");
#endif
            }
            else
            {
            
#if PrintDis
                printf("set speed failed\n");
#endif
             return;
            }
            cfsetospeed (&Opt, speed_arr[i]);
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
- 函数名称 : int OpenDev (char *Dev)
- 函数说明 : 设置串口属性
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
int set_Parity (int fd, int databits, int stopbits, int parity)
{
    struct termios options;
    if (tcgetattr (fd, &options) != 0)
    {
        perror ("uart SetupSerial 1");
        return (-1);
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
        fprintf (stderr, "uart Unsupported data size\n");
        return (-1);
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
        fprintf (stderr, "uart Unsupported parity\n");
        return (-1);
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
        fprintf (stderr, "uart Unsupported stop bits\n");
        return (-1);
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
        perror ("uart SetupSerial 3");
        return (-1);
    }
    return (0);
}

/*
*************************************************************************************************************
- 函数名称 : int OpenDev (char *Dev)
- 函数说明 : 打开驱动
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
int OpenDev (char *Dev)
{
    int fd = open (Dev, O_RDWR );
    if (fd == -1)
    {
        perror("Uart_Readdata open()");
        return -1;
    }
    else
    {
        return fd;
    }
}

/*
*************************************************************************************************************
- 函数名称 : int initialize (const char *com, int speed)
- 函数说明 : 初始化串口驱动
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
int initialize (const char *com, int speed)
{
    char serial[16] = "/dev/";
    strcat(serial, com);

    int fd = OpenDev (serial);
    if (fd > 0)
    {
        set_speed (fd, speed);
        if (set_Parity (fd, 8,1,'n') == -1)
        {
            close(fd);
            printf ("uart set_parity error\n");
        }
        return fd;
    }
    return -1;
}
/*
*************************************************************************************************************
- 函数名称 : void uninitialize (int fd)
- 函数说明 : 关闭串口驱动
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void uninitialize (int fd)
{
    close (fd);
}

void InitUart(int *uartfd,char *uart,int speed)
{
int fd;

    fd = initialize(uart,speed);
    tcflush(fd,TCIFLUSH);
    Uart_Printf(fd,"Uart  test \n");
	*uartfd = fd;

#if PrintDis
    printf("Inint  Uart OK \n");
#endif

}

/*
*************************************************************************************************************
- 函数名称 : int ReadOrWriteFile (unsigned char RW_Type)
- 函数说明 : 写文件方式
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/

int Uart_SendBye(int fd,char *uartdata,unsigned short len)
{
    int ret;
#if PrintDis
    {
        int i;
        printf("W uart=%03d::",len);
        for(i = 0; i<len; i++)
        {
            printf("%02X",uartdata[i]);
        }
        printf("\n");
    }
#endif

    ret= write(fd,uartdata,len);

#if PrintDis
    printf("Uart_SendBye == %02d\n",ret);
#endif

    return ret;
}

/*
*************************************************************************************************************
- 函数名称 : int ReadOrWriteFile (unsigned char RW_Type)
- 函数说明 : 写文件方式
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/

int Uart_Printf(int fd,char *uartdata)
{
    int ret;
    ret= write(fd,uartdata,strlen(uartdata));

#if PrintDis
    printf("Uart_Printf == %02d\n",ret);
#endif

    return ret;
}


/*
*************************************************************************************************************
- 函数名称 : unsigned char Read_Uart(unsigned char *buffer)
- 函数说明 : 读SOCKET数据
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
int Uart_Readdata(int fd,unsigned char *buffer, int size)
{
	DebugPrintf("\n");
    fd_set rfds;
    struct timeval tv;
    int retval;
    int ret = 0;

    tv.tv_sec = 20; 				  // the rcv wait time
    tv.tv_usec = 0; 				  // 50ms

    while(1)
    {
        FD_ZERO(&rfds);
        FD_SET(fd,&rfds);
        retval = select(fd+1,&rfds,NULL,NULL,&tv);
        if(retval ==-1)
        {
            perror("Uart_Readdata select()");
            return(-1);
        }
        else if(retval)
        {
            ret= read(fd, buffer, size);
#if 0			
//#if    PrintDis
            {
                int i;
                printf("R uart=%03d::",ret);
                for(i = 0; i<ret; i++)
                {
                    printf("%02X",buffer[i]);
                }
                printf("\n");
            }
#endif
            break;
        }
        else
        {
            return(-1);
        }
    }
	
    return(ret);
}


/*

int readline(const char *filename,int line,char *date)
{
FILE*fp ;
int zch;
int xlin = -1;

if(access(filename, 0) == 0)//要是不存在就建立一个新test.db数据库文件，并创建record记录表
{
	fp = fopen(filename,"r");
	zch = fgetc(fp);
	while(zch != (-1))
	{
		if(zch == '\n')xlin++;
		if(xlin == line)break;
		zch = fgetc(fp);
	}
       if(zch < 0)
	{
		fseek(fp, -1, SEEK_CUR);
	}
	fgets(date,100,fp);//用fgets读第一行到数组s中
	zch = strlen(date);
	if(zch > 2)
	{
		date[zch - 2] = '\x0';
		date[zch - 1] = '\x0';
	}
	fclose(fp);
}
return xlin;
}

*/

/*
void Start_Name(const char *filename)
{
char daline[100];
int x;
	if(access(filename, 0) == 0)
	{
		memset(daline,0,sizeof(daline));
		x = readline(filename,0,daline);
		if(x == 0)
		{
			if(strlen(daline) > 4)
			{
				Big_PrintText(daline);
			}
		}
	}
}
*/


/*
*************************************************************************************************************
- 函数名称 : void PrintferInformation (void)
- 函数说明 : 三级菜单－进车选项
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
/*
void PrintMText(const char *filename,char big)
{
int a,b;
char daline[100];
char prirtbuf[35];
	//if(big)
	//{
	//	b = 1;
	//	Start_Name(filename);
	//}
	//else
	//{
		b = 0;
	//}
	if(access(filename, 0) == 0)
	{
	 for(;;)
	 {
		memset(daline,0,sizeof(daline));
		a = readline(filename,b,daline);
		if(a != b)break;
		if(strlen(daline) > 8)
		{
			a = strlen(daline);
			switch(a)
			{
			case 0 ... 32:
					Uart_Printf(uart4_fd,daline);
					break;
			case 33 ... 58:
					Uart_Printf(uart4_fd,daline);
					memset(prirtbuf,0,sizeof(prirtbuf));
					strcpy(prirtbuf,"      ");
					if((unsigned char)daline[31] <= 0x80)
					{
						memcpy(prirtbuf + 5,daline + 32,26);
					}
					else
					{
						memcpy(prirtbuf + 5,daline + 31,26);
					}

#if  PrintDis
					printf("%s\n",prirtbuf);
#endif
					Uart_Printf(uart4_fd,prirtbuf);
					break;
			}
		}
		b++;
	}
      }
}
*/
/*
*************************************************************************************************************
- 函数名称 : void PrintferInformation (void)
- 函数说明 : 三级菜单－进车选项
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
/*
void PrintferInformation (RecordFormat Save)
{
	unsigned char Content[32];
        LongUnon Klly;
	//LongUnon Buf;
        ShortUnon Glly;
	PrintMText(PRINT_START,1);
        //Uart_Printf(uart4_fd,"");
        //Uart_Printf(uart4_fd,"          消费成功");
	Uart_Printf(uart4_fd,".........................................\n");
        Klly.i = 0;
	memcpy(Klly.longbuf,Save.RFrove,4);
	sprintf(Content,"消费流水号:%08d",Klly.i);
	Uart_Printf(uart4_fd,Content);
	memcpy(Klly.longbuf,Save.RFderno,4);
	sprintf(Content,"卡号:%02X%02X%02X%02X  终端:%08d号",Save.RFcsn[0],Save.RFcsn[1],\
                Save.RFcsn[2],Save.RFcsn[3],Klly.i);
	Uart_Printf(uart4_fd,Content);

       if(Save.RFXFtype == 2)
	{

		memset(Content,0,sizeof(Content));
		strcpy(Content,"消费金额:");
		Klly.i = 0;
		memcpy(Klly.longbuf,Save.RFvaluej,3);//
		MoneyValue(Content+9,Klly.i);
		Uart_Printf(uart4_fd,Content);

		memset(Content,0,sizeof(Content));
		strcpy(Content,"消费后金额:");
		memcpy(Klly.longbuf,Save.RFvalueh,4);//
		MoneyValue(Content+11,Klly.i);
		Uart_Printf(uart4_fd,Content);
	}
        else  if(Save.RFXFtype == 8)
         {

           Uart_Printf(uart4_fd,"时间包月卡");
           memset(Content,0,sizeof(Content));
         //              if(MonthlyFlay == 1)
           //              {
       sprintf(Content,"包月开始时间:%02X年%02X月",CardLan.SMonth[0],CardLan.SMonth[1]);
    //                     }
     //                    else
     //                      {
      // sprintf(Content,"包月开始时间:%02X年%02X月",CardLan.SMonth2[0],CardLan.SMonth2[1]);
      //                     }

           Uart_Printf(uart4_fd,Content);
         //  memset(Content,0,sizeof(Content));

            //           if(MonthlyFlay == 1)
             //            {
       sprintf(Content,"包月结束时间:%02X年%02X月",CardLan.EMonth[0],CardLan.EMonth[1]);
          //                }
          //               else
          //                 {
      // sprintf(Content,"包月结束时间:%02X年%02X月",CardLan.EMonth2[0],CardLan.EMonth2[1]);
         //                  }

          // Uart_Printf(uart4_fd,Content);
         }

	memset(Content,0,sizeof(Content));
	sprintf(Content,"交易时间:20%02X-%02X-%02X %02X:%02X",Save.RFtime[0],Save.RFtime[1],Save.RFtime[2],\
		Save.RFtime[3],Save.RFtime[4]);
	Uart_Printf(uart4_fd,Content);
	Uart_Printf(uart4_fd,".......................................\n");
	memset(Content,0,sizeof(Content));
	Glly.i = 0;
	memcpy(Glly.intbuf,Save.RFoperator,2);//
	sprintf(Content,"操作员:%05d",Glly.i);
	Uart_Printf(uart4_fd,Content);
        Uart_Printf(uart4_fd,"");
	PrintMText(PRINT_END,0);
	Uart_Printf(uart4_fd,"");
	Uart_Printf(uart4_fd,"");
	Uart_Printf(uart4_fd,"");
	Uart_Printf(uart4_fd,"");
	Uart_Printf(uart4_fd,"");
	Uart_Printf(uart4_fd,"");
}
*/
