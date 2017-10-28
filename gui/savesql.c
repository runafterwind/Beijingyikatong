#include <apparel.h>
#include "InitSystem.h"
#include "des.h"
#include "RC500.h"
#include "cpucard.h"
#include "apparel.h"
#include "savesql.h"

#define SqlOPEN  0



typedef union
{
	unsigned char buff[8];
	unsigned long long  i;
}BigUnion;

BigUnion BigData;
//extern unsigned long long big_data;
extern CardInformCPU CardLanCPU;
extern unsigned char PsamNum[6];





extern FILE *Datafile;
extern int sockfd;	//
extern unsigned char SnBack[4];
extern volatile unsigned char DLink;  // 自动上传记录失败数
extern unsigned char AutoUpFlag;
extern unsigned char ConnectFlag; //上网标志
extern unsigned char SavedataErr;
extern LongUnon DevSID;
extern LongUnon CodeNum;
extern LongUnon SaveNum;
extern LongUnon  TransactionNum;
extern LongUnon SaveNumBc;
extern LongUnon SaveNumBs;
extern LongUnon DecValue;
extern LongUnon HostValue;
extern LongUnon DevNum;
extern ShortUnon Infor;
extern ShortUnon Driver;
extern JackRegal Sector;
extern SysTime Time;
extern RouteJack TypeTime;
extern unsigned char CardTypeIC;
extern SectionFarPar Section,Sectionup;
extern time_t utc_time;

extern char StuToOrd;//0-不将学生卡转普通卡消费,1-将学生卡当普通卡消费

//extern  unsigned char savedataflag;    		//文件自锁

unsigned char SendDataTimes;
unsigned char SendeghitDat[576];
unsigned char Senddata[SAVE_DATA_LEN];
extern CardInform CardLan;
RecordFormat SaveData;
LongUnon IDaddr;    					// 数据库 ID 号
LongUnon LBuf;
extern int myflag;
extern unsigned int addrflag;
extern unsigned int Dci;
extern unsigned int firstcon;
extern unsigned char bStolenDis;
extern LongUnon HostValuej;
extern LongUnon HostValuet;

RecordFormat_new SaveData_new;

unsigned long long big_data;
extern unsigned char  VerifySaveDataIsRight(RecordFormat SaveDataInfo);

#ifdef SUPPORT_QR_CODE
extern struct QRCode G_QRCodeInfo;
#endif

long long bcd_to_bin(unsigned char *bcd, unsigned char len)
{
	unsigned long long lbin;
	int i,k;
    big_data = 0;
	lbin=0;
	for(i=0; i<len; i++)
	{
		k=(bcd[i]>>4)*10+(bcd[i]&0x0f);
		lbin=lbin*100+k;
	}
    big_data=lbin;
	return lbin;
}


void Chang4to4(unsigned char *IN,unsigned char *OUT)
{
    unsigned char buff[4];
    unsigned char i;

    memcpy(buff,IN,4);
    for(i=0; i<4; i++)
    {
        OUT[i] = buff[3-i];
    }
}


/*
**************************************************************
- 函数名称 : char UPdateRecord(unsigned int Drdevnum,int SValue)
- 函数说明 : 自动上传数据   判断返回数据
- 入口参数：
- 输出参数 :
**************************************************************
*/
char UPdateRecord(unsigned int Drdevnum,int SValue)
{

#if SqlOPEN

    printf(" UPdateRecord  Drdevnum == %d  \n",Drdevnum);
    printf(" UPdateRecord  IDaddr == %d  \n",IDaddr.i);

#endif

    IDaddr.longbuf[3] = 0;
    if( Drdevnum == IDaddr.i)
    {
        IDaddr.i = 0;
        return 0;
    }
    else
    {
        return 1;
    }
}


/*

char UPdateRecord(unsigned int Drdevnum,int SValue)
{
 unsigned char sql[200];
 char  status;
 int result;

 while(SQL_Falg); 				//数据库不在使用为0
     	SQL_Falg = 1;
 status = 1;
 if(access("cardsave.db", 0)) //判断数据库文件是否存在
 {
      	status = 1;
 }
 else
 {
  	memset(sql,'\0',sizeof(sql));
  	result = sqlite3_open ("cardsave.db", &dba);
  	if(result==SQLITE_OK)
    	{
      		sprintf(sql, "update Maindata set SaveStatus=%d  where ID=%d;",SValue,Drdevnum);
	    	result = sqlite3_exec( dba , sql , NULL , NULL , &zErrMsg );
            	if(result != SQLITE_OK)
            	{
#if   SqlOPEN
	         	printf("create zErrMsg = %s \n", zErrMsg);
#endif
                 	status = 1;
                 	sqlite3_close(dba);
            	}
            	else  status = 0;
    	}
   	sqlite3_close(dba);
 }
 SQL_Falg = 0;
 return  (status);
}
*/

/*
**************************************************************
- 函数名称 : unsigned char ReadEightDat (unsigned char *StartDat)
- 函数说明 :完全采集   每次发送8条数据
- 入口参数： 数据
- 输出参数 :
**************************************************************
*/
unsigned char ReadEightDat (unsigned char *StartDat)
{
    int result;
    unsigned int i,Saddr,nu;

    memset(SendeghitDat,0xff,sizeof(SendeghitDat));
    memcpy(LBuf.longbuf,StartDat,4);

    if(SaveNumBs.i >= CodeNum.i)
    {
        //while(savedataflag);
        //    savedataflag =1;
        pthread_mutex_lock(&m_datafile);
#ifdef SAVE_CONSUM_DATA_DIRECT
		Datafile = open(OFF_LINE_CONSUM_FILE,O_SYNC|O_RDWR);
#else
        Datafile = fopen(OFF_LINE_CONSUM_FILE,"rb+");
#endif
        Saddr = (LBuf.i-1)*72;
        if(SaveNumBs.i >= (LBuf.i + 8))
        {
            for(i = 0; i<8; i++)
            {
#ifdef SAVE_CONSUM_DATA_DIRECT
				result = lseek(Datafile,Saddr+i*72, SEEK_SET);
				result = read(Datafile,SendeghitDat+i*64,64);

#else
                result = fseek(Datafile,Saddr+i*72, SEEK_SET);
                result = fread(SendeghitDat+i*64,sizeof(unsigned char),64,Datafile);
#endif				
            }
            // status = 0;
#ifdef SAVE_CONSUM_DATA_DIRECT
			close(Datafile);
#else
            fclose_nosync(Datafile);
#endif
            pthread_mutex_unlock(&m_datafile);
            //  savedataflag = 0;
            return 0;

        }
        else
        {
            nu = SaveNumBs.i - LBuf.i;
            for(i = 0; i< nu; i++)
            {
#ifdef SAVE_CONSUM_DATA_DIRECT
				result = lseek(Datafile,Saddr+i*72, SEEK_SET);
				result = read(Datafile,SendeghitDat+i*64,64);

#else
                result = fseek(Datafile,Saddr+i*72, SEEK_SET);
                result = fread(SendeghitDat+i*64,sizeof(unsigned char),64,Datafile);
#endif			
            }
            // status = 0;
#ifdef SAVE_CONSUM_DATA_DIRECT
			close(Datafile);
#else            
            fclose_nosync(Datafile);
#endif
            // savedataflag = 0;
            pthread_mutex_unlock(&m_datafile);
            return 0;
        }
    }
    else
    {
        return -1;
    }
}


/*
**************************************************************
- 函数名称 : char RMAllRecord(unsigned int recordnum)
- 函数说明 : 册除所有数据
- 入口参数：
- 输出参数 :
**************************************************************
*/
char RMAllRecord(unsigned int recordnum)
{

    if((SaveNum.i == CodeNum.i)||(recordnum == 0))
    {
        pthread_mutex_lock(&m_datafile);
        system("rm  "OFF_LINE_CONSUM_FILE);
        pthread_mutex_unlock(&m_datafile);

        SaveNum.i = 1;
        TransactionNum.i = 1;
        CodeNum.i = 1;

        ReadOrWriteFile(WSDATA);
        ReadOrWriteFile(CODEFILE);

        if(access(OFF_LINE_CONSUM_FILE, 0)) //判断数据库文件是否存在
        {
#ifdef	 SAVE_CONSUM_DATA_DIRECT
			Datafile = open(OFF_LINE_CONSUM_FILE,O_SYNC|O_RDWR|O_CREAT);
#else
            Datafile = fopen(OFF_LINE_CONSUM_FILE,"a+");
#endif
            if(Datafile)
            {
#if SqlOPEN
                printf("open  cardsave.bin ok!\n");
#endif
#ifdef	 SAVE_CONSUM_DATA_DIRECT
				close(Datafile);
#else
                fclose_nosync(Datafile);
#endif
                system("sync;");

		system("chmod 755 "OFF_LINE_CONSUM_FILE);
                return  0;
            }
            else
            {

#if SqlOPEN
                printf("Can't open cardsave.bin \n");
#endif
                return -5;
            }

        }
        else
        {
            return -3;
        }
    }
    else
    {
        return -1;
    }
    system("sync;");
}



/*
char RMAllRecord(unsigned int recordnum)
{
 unsigned char sql[200];
 char  status;
 int result;
 int Rstatus;

  while(SQL_Falg); 				//数据库不在使用为0
     SQL_Falg = 1;

  status = 1;
 if(access("cardsave.db", 0)) //判断数据库文件是否存在
   {
      status = 1;
   }
  else
 {
  memset(sql,'\0',sizeof(sql));
  result = sqlite3_open ("cardsave.db", &dba);
  if(result==SQLITE_OK)
    {
            Rstatus = 0x55;
            sprintf(sql, "DELETE FROM Maindata where SaveStatus!=%d;",Rstatus); //格式化
	    result = sqlite3_exec( dba , sql , NULL , NULL , &zErrMsg );
            if(result != SQLITE_OK)
            {
#if   SqlOPEN
	         printf("create zErrMsg = %s \n", zErrMsg);
#endif
                 status = 1;
                 sqlite3_close(dba);
            }
            else  status = 0;
    }
   sqlite3_close(dba);
 }

  SQL_Falg = 0;
 return  (status);
}
*/
/*
**************************************************************
- 函数名称 : void Check_db(void)
- 函数说明 :
- 入口参数：
- 输出参数 :
- 错误返回 :
**************************************************************
*/
/*
void Check_db(void)
{
unsigned char sql[1024];
int rc;
    if (access("cardsave.db", 0))			//判断数据库文件是否存在
    {
	  rc = sqlite3_open ("cardsave.db", &dba);
	  if(rc)
 	  {
  		printf("Can't open database: %s\n", sqlite3_errmsg(dba));
  		sqlite3_close(dba);
 	  }
	  memset(sql,'\0',sizeof(sql));
          strcpy(sql,"CREATE TABLE Maindata(ID INTEGER PRIMARY KEY,SaveStatus INTEGER,RFIccsn VARCHAR(8),RFcity INTEGER,RFapp INTEGER,RFcsn INTEGER,RFrove INTEGER,RFtype INTEGER,RFXFtype INTEGER,RFStationID INTEGER,RFvalueq INTEGER,RFvaluej INTEGER,RFtime VARCHAR(14),RFtran INTEGER,RFMoneyJF INTEGER,RFMoneyJFbuf INTEGER,RFtac INTEGER,RFpurse INTEGER,RFvalueh INTEGER,RFvaluey INTEGER,RFtimeno INTEGER,RFderno INTEGER,RFEnterOrExit INTEGER,RFcarp INTEGER,RFoperator INTEGER,RFflag INTEGER,RFspare INTEGER,RFXor INTEGER,Savetime datetime);");
          printf("create db ...\n");
          rc = sqlite3_exec( dba , sql , NULL , NULL , &zErrMsg );
	  if(rc)
 	  {
	         printf("create zErrMsg = %s \n", zErrMsg);
                 sqlite3_close(dba);
	  }
     }
     else
     {
	  rc = sqlite3_open ("cardsave.db", &dba);
	  if( rc )
 	  {
  		printf("Can't open: %s\n", sqlite3_errmsg(dba));
  		sqlite3_close(dba);
 	  }
     }
     sqlite3_close(dba); 					//关闭数据库
}
*/
/*
*************************************************************************************************************
- 函数名称 : char Savedatasql(RecordFormat Save)
- 函数说明 : 保存数据
- 入口参数：
- 输出参数 :
*************************************************************************************************************
*/
char Savedatasql(RecordFormat Save,int flag,int tao)  //flag 0:消费数据 1:银行卡EMV数据  tao:0 正常数据 1:逃票数据
{
	int result;
	unsigned int Saddrs;
	unsigned char loop;
	unsigned char DXOR,t,status = 1;
	unsigned char dbuff[8];
	unsigned char dtatabuf[SAVE_DATA_LEN] = {0};
	unsigned char *saddr;


	if(SaveNum.i > 100000)
	{
    	FindSavedata();
	}

	printf("Enter Savedatasql !SavedataErr=%d\n",SavedataErr);

	if((SavedataErr == 0)||(SavedataErr == 2))
	{
    	pthread_mutex_lock(&m_datafile);
#ifdef	SAVE_CONSUM_DATA_DIRECT
		Datafile = open(OFF_LINE_CONSUM_FILE,O_RDWR|O_SYNC);
#else
    	Datafile = fopen(OFF_LINE_CONSUM_FILE,"rb+");
#endif
    	if(Datafile < 0)
    	{
        		printf("open cardsave.bin Err\n");
		//Datafile = open(OFF_LINE_CONSUM_FILE,O_RDWR|O_SYNC);
        		return 1;
    	}
		printf("open cardsave.bin\n");
    	loop = 1;
		
    	while(loop)
    	{
    		memset(dtatabuf,0,sizeof(dtatabuf));
    		Saddrs = (unsigned int)((SaveNum.i -1) * SAVE_DATA_LEN);
#ifdef	SAVE_CONSUM_DATA_DIRECT
			result = lseek(Datafile,Saddrs, SEEK_SET);
			result = read(Datafile, dtatabuf, sizeof(dtatabuf));
#else
    		result = fseek(Datafile,Saddrs, SEEK_SET);
    		result = fread(dtatabuf,sizeof(unsigned char),sizeof(dtatabuf),Datafile);
#endif			
    		for(t=0; t<8; t++)
    		{
        		memset(dbuff,0,sizeof(dbuff));
        		memcpy(dbuff,dtatabuf+t*8,8);
        		if(mystrncmp(dbuff,"\x00\x00\x00\x00\x00\x00\x00\x00",8) != 0)
        		{
        			t= 0;
        			break;
        		}
    		}
    		if(t >= 8)
    		{
        		loop = 0;
        		break;
    		}
    		else
    		{
#if SqlOPEN
        		printf(" SaveNum  tt  == %d \n",t);
#endif
        		SaveNum.i += 1;
        		if(SaveNum.i > 200000)
        		{
        			printf("SavedataErr error\n");
					SavedataErr = 1;
					status = 1;
					loop = 0;
					break;
        		}
    		}

#if   SqlOPEN
    		printf("Savedatasql 1 == %d \n",SaveNum.i);
#endif
    	}

    	loop = 0;
	
    	if(SavedataErr == 0)
    	{
    		for(loop = 5; loop>0; )
    		{
			memset(dtatabuf,0,sizeof(dtatabuf));
			saddr = (unsigned char *)&Save;
			memcpy(dtatabuf,saddr,SAVE_DATA_LEN-SAVE_DATA_RESERVE);			
			dtatabuf[SAVE_DATA_LEN-1] = 0x55;



#ifdef	SAVE_CONSUM_DATA_DIRECT				
    		result = lseek(Datafile,Saddrs, SEEK_SET);
    		result = write(Datafile,dtatabuf,sizeof(dtatabuf));				
#else
    		result = fseek(Datafile,Saddrs, SEEK_SET);
    		result = fwrite(dtatabuf,sizeof(unsigned char),sizeof(dtatabuf),Datafile);
#endif
		
			memset(dtatabuf,0,sizeof(dtatabuf));
#ifdef	SAVE_CONSUM_DATA_DIRECT	
			result = lseek(Datafile,Saddrs, SEEK_SET);
			result = read(Datafile,dtatabuf,sizeof(dtatabuf));
#else
			result = fseek(Datafile,Saddrs, SEEK_SET);
			result = fread(dtatabuf,sizeof(unsigned char),sizeof(dtatabuf),Datafile);
#endif				

			DXOR  = Save_Data_Xor(dtatabuf);
			 if(DXOR == dtatabuf[SAVE_DATA_LEN-SAVE_DATA_RESERVE-1])
			{
				break;
			}
			else
			{
				loop --;
#if   SqlOPEN
				printf("\n\n save data  loop=%d \n\n",loop);
#endif
    		}
		}
	}
#ifdef	SAVE_CONSUM_DATA_DIRECT	
	close(Datafile);
#else
    	fclose_nosync(Datafile);
#endif
    	pthread_mutex_unlock(&m_datafile);

    	if(loop == 0)
    	{
			DebugPrintf("SavedataErr [%02X] status [%02X]\n",SavedataErr,  status);
			SavedataErr = 1;
			status = 1;
    	}
    	else
    	{
    		SaveNum.i += 1;
    		ReadOrWriteFile(WSDATA);
        	status = 0;
    	}
	}
	else
	{
    	status = 1;
	}

	return status;
}

char Savedatasql_new(RecordFormat_new Save, int flag, int tao)  //flag 0:消费数据 1:银行卡EMV数据  tao:0 正常数据 1:逃票数据
{
	int result;
	unsigned int Saddrs;
	unsigned char loop;
	unsigned char DXOR,t,status = 1;
	unsigned char dbuff[8];
	unsigned char dtatabuf[SAVE_DATA_LEN] = {0};
	unsigned char *saddr;


	if(SaveNum.i > 100000)
	{
    	FindSavedata();
	}

	printf("Enter Savedatasql !SavedataErr=%d\n",SavedataErr);

	if((SavedataErr == 0)||(SavedataErr == 2))
	{
    	pthread_mutex_lock(&m_datafile);
#ifdef	SAVE_CONSUM_DATA_DIRECT
		Datafile = open(OFF_LINE_CONSUM_FILE,O_RDWR|O_SYNC);
#else
    	Datafile = fopen(OFF_LINE_CONSUM_FILE,"rb+");
#endif
    	if(Datafile < 0)
    	{
        		printf("open cardsave.bin Err\n");
		//Datafile = open(OFF_LINE_CONSUM_FILE,O_RDWR|O_SYNC);
        		return 1;
    	}
		printf("open cardsave.bin\n");
    	loop = 1;
		
    	while(loop)
    	{
    		memset(dtatabuf,0,sizeof(dtatabuf));
    		Saddrs = (unsigned int)((SaveNum.i -1) * SAVE_DATA_LEN);
#ifdef	SAVE_CONSUM_DATA_DIRECT
			result = lseek(Datafile,Saddrs, SEEK_SET);
			result = read(Datafile, dtatabuf, sizeof(dtatabuf));
#else
    		result = fseek(Datafile,Saddrs, SEEK_SET);
    		result = fread(dtatabuf,sizeof(unsigned char),sizeof(dtatabuf),Datafile);
#endif			
    		for(t=0; t<8; t++)
    		{
        		memset(dbuff,0,sizeof(dbuff));
        		memcpy(dbuff,dtatabuf+t*8,8);
        		if(mystrncmp(dbuff,"\x00\x00\x00\x00\x00\x00\x00\x00",8) != 0)
        		{
        			t= 0;
        			break;
        		}
    		}
    		if(t >= 8)
    		{
        		loop = 0;
        		break;
    		}
    		else
    		{
#if SqlOPEN
        		printf(" SaveNum  tt  == %d \n",t);
#endif
        		SaveNum.i += 1;
        		if(SaveNum.i > 200000)
        		{
        			printf("SavedataErr error\n");
					SavedataErr = 1;
					status = 1;
					loop = 0;
					break;
        		}
    		}

#if   SqlOPEN
    		printf("Savedatasql 1 == %d \n",SaveNum.i);
#endif
    	}

    	loop = 0;
	
    	if(SavedataErr == 0)
    	{
    		for(loop = 5; loop>0; )
    		{
			memset(dtatabuf,0,sizeof(dtatabuf));
			saddr = (unsigned char *)&Save;
			memcpy(dtatabuf,saddr,SAVE_DATA_LEN-SAVE_DATA_RESERVE);			
			dtatabuf[SAVE_DATA_LEN-1] = 0x55;

#ifdef	SAVE_CONSUM_DATA_DIRECT				
    		result = lseek(Datafile,Saddrs, SEEK_SET);
    		result = write(Datafile,dtatabuf,sizeof(dtatabuf));				
#else
    		result = fseek(Datafile,Saddrs, SEEK_SET);
    		result = fwrite(dtatabuf,sizeof(unsigned char),sizeof(dtatabuf),Datafile);
#endif
		
			memset(dtatabuf,0,sizeof(dtatabuf));
#ifdef	SAVE_CONSUM_DATA_DIRECT	
			result = lseek(Datafile,Saddrs, SEEK_SET);
			result = read(Datafile,dtatabuf,sizeof(dtatabuf));
#else
			result = fseek(Datafile,Saddrs, SEEK_SET);
			result = fread(dtatabuf,sizeof(unsigned char),sizeof(dtatabuf),Datafile);
#endif				

			DXOR  = Save_Data_Xor(dtatabuf);
			 if(DXOR == dtatabuf[SAVE_DATA_LEN-SAVE_DATA_RESERVE-1])
			{
				break;
			}
			else
			{
				loop --;
#if   SqlOPEN
				printf("\n\n save data  loop=%d \n\n",loop);
#endif
    		}
		}
	}
#ifdef	SAVE_CONSUM_DATA_DIRECT	
	close(Datafile);
#else
    	fclose_nosync(Datafile);
#endif
    	pthread_mutex_unlock(&m_datafile);

    	if(loop == 0)
    	{
			DebugPrintf("SavedataErr [%02X] status [%02X]\n",SavedataErr,  status);
			SavedataErr = 1;
			status = 1;
    	}
    	else
    	{
    		SaveNum.i += 1;
    		ReadOrWriteFile(WSDATA);
        	status = 0;
    	}
	}
	else
	{
    	status = 1;
	}

	return status;
}
char SaveDataToFile(unsigned char* Save) 
{
    	int result,i;
    	unsigned int Saddrs;
    	unsigned char loop;
    	unsigned char t,status = 1;
    	unsigned char dbuff[8];
    	unsigned char dtatabuf[72];


    	if(SaveNum.i > 100000)
    	{
        	FindSavedata();
    	}

	printf("Enter SaveDataToFile !SavedataErr=%d\n",SavedataErr);

    	if((SavedataErr == 0)||(SavedataErr == 2))
    	{
		pthread_mutex_lock(&m_datafile);
		
        	Datafile = fopen(OFF_LINE_CONSUM_FILE,"rb+");
        	if(Datafile < 0)
        	{
            		printf("open cardsave.bin Err\n");
            		return 1;
        	}
			
        	loop = 1;
        	while(loop)
        	{
            		memset(dtatabuf,0,sizeof(dtatabuf));
            		Saddrs = (unsigned int)((SaveNum.i -1) * 72);
            		result = fseek(Datafile,Saddrs, SEEK_SET);
            		result = fread(dtatabuf,sizeof(unsigned char),sizeof(dtatabuf),Datafile);
			//printf("\n72 data:\n");
			//for(i=0;i<72;i++)
			//	printf("%02x ",dtatabuf[i]);
			//printf("\n");
            		for(t=0; t<8; t++)
            		{
                		memset(dbuff,0,sizeof(dbuff));
                		memcpy(dbuff,dtatabuf+t*8,8);
                		if(mystrncmp(dbuff,"\x00\x00\x00\x00\x00\x00\x00\x00",8) != 0)
                		{
                    			t= 0;
                    			break;
                		}
            		}
					
            		if(t >= 8)
            		{
                		loop = 0;
                		break;
            		}
            		else
            		{
                		printf(" tt  == %d \n",t);

                		SaveNum.i += 1;
                		if(SaveNum.i > 200000)
                		{
                    			SavedataErr = 1;
                    			status = 1;
                    			loop = 0;
                    			break;
                		}
            		}
            		printf("SaveNum.i == %d \n",SaveNum.i);
        	}

        	loop = 0;
        	if(SavedataErr == 0)
        	{
            		for(loop = 5; loop>0;)
            		{
                		result = fseek(Datafile,Saddrs, SEEK_SET);
                		result = fwrite(Save,1,288,Datafile);
				printf("\n............fwrite result=%d...........\n",result);
				if(result != 288)
					loop --;
				else
					break;	
            		}
        	}
        	fclose_nosync(Datafile);
		pthread_mutex_unlock(&m_datafile);

        	if(loop == 0)
        	{
            		SavedataErr = 1;
            		status = 1;
        	}
        	else
        	{
            		SaveNum.i += 4;
            		ReadOrWriteFile(WSDATA);
            		status = 0;
        	}
    	}
    	else
    	{
        	status = 1;
    	}

    	return status;
}



/*
*************************************************************************************************************
- 函数名称 : uint8_t get_time_stamp(uint8_t *pt, uint8_t len, SysTime time)     
- 函数说明 : 获得时间秒
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
uint8_t get_time_stamp(uint8_t *pt, uint8_t len, SysTime time)
{
    FILE *fp = NULL;
    uint8_t buff[50] = 
    {
    0
}
, mon[5] = 
{
    0
}
;
    
    if (pt == NULL) return -1;
    
    sprintf(buff, "date +%%s -d '20%02X-%02X-%02X %02X:%02X:%02X'", time.year, time.month, time.day, time.hour, time.min, time.sec);
    //   printf("\r\n\e[1;32m func=%s : command=%s \e[0m", __func__, buff);
    fp = popen(buff , "r");
    if (fp == NULL) 
    {
        return -1;
    }
    memset(pt, 0, len);
    fread(pt, len, 1, fp);
    pclose(fp);
    
    //   printf("\r\n\e[1;32m func=%s : output time stamp : %s \e[0m", __func__, pt);
    return 0;
}



/*
	Calculate driver's income
	return 0 successful  1 failed
*/
int GetTotalConsumption(unsigned int *ptotal)
{
	int result;
	unsigned int Saddrs,i;
	RecordFormat OneRec;
    char tempbuf[10],timebuf[7];
    SysTime timebuf1;
    LongUnon timestart,timeend,timerecord;
	union{
		unsigned char buf[4];
		unsigned int val;
	}Tmp;
	unsigned char LocalTime[8];
	unsigned char bGetDriverID = 0;
	unsigned int ActiveTotal;
	unsigned int preval,subval;

	
	if(SaveNum.i == 0){
		printf("SaveNum.i : %d \n",SaveNum.i);
		return 1;
	}
    
    Rd_time(timebuf);    
    Time.year = timebuf[0];
    Time.month = timebuf[1];
    Time.day = timebuf[2];
    Time.hour = timebuf[3];
    Time.min = timebuf[4];
    Time.sec = timebuf[5];
    timebuf1.year = timebuf[0];
    timebuf1.month = timebuf[1];
    timebuf1.day = timebuf[2];
    timebuf1.hour = 0;
    timebuf1.min = 0;
    timebuf1.sec = 0;
        
    timestart.i=0; 
    get_time_stamp(tempbuf,10,timebuf1);		//创建管道读十个字节装入tempbuf
    timestart.i = atoi(tempbuf);   
    timeend.i=0;
    get_time_stamp(tempbuf,10,Time);
    timeend.i = atoi(tempbuf);
//    printf("timestart.i=%d:timeend.i=%d\n",timestart.i,timeend.i);
	pthread_mutex_lock(&m_datafile);
#ifdef SAVE_CONSUM_DATA_DIRECT
	Datafile = open(OFF_LINE_CONSUM_FILE,O_RDWR|O_SYNC);
#else
	Datafile = fopen(OFF_LINE_CONSUM_FILE,"rb+");
#endif	
    if(Datafile < 0)
    {
        printf("open cardsave.bin Err\n");
        return 1;
    }	
	i = SaveNum.i;
	*ptotal = 0;
	preval = 0;
	subval = 0;
	Rd_time(LocalTime);
	ActiveTotal = 0;
	while(i){
   
#if ((defined Transport_Stander)||(defined CANGNAN_BUS))
        if(i==1)
            break;
#endif 
		Saddrs = (i-2)*72;	
		//Saddrs = addrflag;	
#ifdef SAVE_CONSUM_DATA_DIRECT
		result = lseek(Datafile,Saddrs, SEEK_SET);
		result = read(Datafile,&OneRec,sizeof(OneRec));
		
#else
		result = fseek(Datafile,Saddrs, SEEK_SET);
		result = fread(&OneRec,sizeof(unsigned char),sizeof(OneRec),Datafile);	
#endif		
#if 0
        timebuf1.year=OneRec.RFtime[1];
        timebuf1.month=OneRec.RFtime[2];
        timebuf1.day=OneRec.RFtime[3];
        timebuf1.hour=OneRec.RFtime[4];
        timebuf1.min=OneRec.RFtime[5];
        timebuf1.sec=OneRec.RFtime[6];
        timerecord.i =0;
        get_time_stamp(tempbuf,10,timebuf1);
        timerecord.i = atoi(tempbuf);
#endif
 //       printf("timerecord.i=%d\n",timerecord.i);
#if ((defined Transport_Stander)||(defined CANGNAN_BUS))
        Tmp.val = 0;
        memcpy(Tmp.buf,OneRec.RFtime,4);
  //      printf("recordtime=%d\n",Tmp.val);
         if((Tmp.val<timestart.i)||(Tmp.val>timeend.i))
         {
            i--;
            continue;
            }
        if((OneRec.RFflag==3)||(OneRec.RFflag==0x10)){
			if(i==1)
				break;
			i--;
			continue;
		}	 
#else
		if(OneRec.RFtime[0]!=0x20){
			if(i==1)
				break;
			i--;
			continue;
		}		
	   
	  	if((Time.year!=OneRec.RFtime[1])||(Time.month!=OneRec.RFtime[2])||(Time.day!=OneRec.RFtime[3]))
	    {
            i--;
            continue;
            }
#endif
        
        else
        {
		if(OneRec.RFtype != 0xcc)
		{
			Tmp.val = 0;
			memcpy(Tmp.buf,OneRec.RFvaluej,3);	
        //    printf("记录金额:%d\n",Tmp.val);
			if(Section.Enable != 0x55)
			{
				*ptotal += Tmp.val;
             //   printf("非分段记录汇总=%d\n",*ptotal);
			}
			else
			{
				if((Section.Updoor!=1)&&(Section.Updoor!=2))
				{
					if(OneRec.RFEnterOrExit != 0x55)
						*ptotal -= Tmp.val;
					else
						*ptotal += Tmp.val;
                 //   printf("分段记录上下车汇总=%d\n",*ptotal);
				}
				else
				{
					*ptotal += Tmp.val;
                  //  printf("分段记录汇总=%d\n",*ptotal);
				}
			}
		}
       
		i--;
		Dci++;
     //   printf("交易次数:%d\n",Dci);
        }
	}
#ifdef SAVE_CONSUM_DATA_DIRECT
	close(Datafile);
#else
	fclose(Datafile);
#endif	
	pthread_mutex_unlock(&m_datafile);

		return 0;
}


extern struct DRIVER_SEARCH SearchNextPage;
extern void MoneyValue(unsigned char *OUT_Data,unsigned int Money);
extern unsigned int nowye;
int GetUserRec(struct USER_RESERCH *pUserVal)
{
	unsigned int count,Saddrs,i;
	int result;	
	struct{
		RecordFormat OneRec;
		unsigned char space[8];
	}OneFrame;
	union{
		unsigned char buf[4];
		unsigned int val;
	}Tmp;	
	if((pUserVal == NULL)||(SaveNum.i ==0))
		return 0;
	/*if(SaveNum.i > 5)
		count = 5;
	else
		count = SaveNum.i;
    */
    if(((TransactionNum.i-1)-(nowye-1)*5)>5)
        count = 5;
    else
        count = (TransactionNum.i-1)-(nowye-1)*5;
	printf("444444444444count=%d:nowye=%d\n",count,nowye);
	pthread_mutex_lock(&m_datafile);
//	printf("44444444444422count=%d\n",count);
#ifdef SAVE_CONSUM_DATA_DIRECT
	Datafile = open(OFF_LINE_CONSUM_FILE,O_RDWR|O_SYNC);
#else
	Datafile = fopen(OFF_LINE_CONSUM_FILE,"rb+");
#endif	
    if(Datafile <0)
    {
        printf("open cardsave.bin Err\n");
        return 1;
    }	
	if(myflag==1)
		Saddrs = SearchNextPage.addr;
	else
		Saddrs = (SaveNum.i-1)*72;	
	i = 0;
    printf("444444444455Saddrs=%d\n",Saddrs);
    if(Saddrs==0)
    {
        Saddrs = count*72;
        }
	addrflag=Saddrs;
	while((count-i) > 0)
	{		
	//	printf("444444444433count=%d\n",count);
#ifdef SAVE_CONSUM_DATA_DIRECT
		result = lseek(Datafile,Saddrs, SEEK_SET);
		result = read(Datafile,&OneFrame,sizeof(OneFrame));
		
#else
		result = fseek(Datafile,Saddrs, SEEK_SET);
		result = fread(&OneFrame,sizeof(unsigned char),sizeof(OneFrame),Datafile);	
#endif	
		if(OneFrame.OneRec.RFtime[0]!=0x20)
		{
			if(Saddrs==0)
			{
			//	printf("444444444488count=%d\n",count);
				SearchNextPage.addr = addrflag;
				break;	
			}
			Saddrs -= 72;
			continue;
		}		
		if(OneFrame.OneRec.RFtype==0xcc)
		{
			if(Saddrs==0)
			{
			//	printf("444444444499count=%d\n",count);
				SearchNextPage.addr = addrflag;
				break;		
			}
			Saddrs -= 72;
			continue;
		}
		Tmp.val = 0;
		memcpy(Tmp.buf,OneFrame.OneRec.RFcsn,4);
		//sprintf((pUserVal+i)->id,"%d",Tmp.val);		
        sprintf((pUserVal+i)->id,"%02X%02X%02X%02X",OneFrame.OneRec.RFcsn[0],\
        OneFrame.OneRec.RFcsn[1],OneFrame.OneRec.RFcsn[2],OneFrame.OneRec.RFcsn[3]);
		#if 0
		sprintf((pUserVal+i)->tim, /*"20%d年-%d月-%d日-*/"%d:%d:%d",/*OneFrame.OneRec.RFtime[1],\
				OneFrame.OneRec.RFtime[2],OneFrame.OneRec.RFtime[3],*/OneFrame.OneRec.RFtime[4],\
				OneFrame.OneRec.RFtime[5],OneFrame.OneRec.RFtime[6]);		
		#endif
		sprintf((pUserVal+i)->tim, /*"20%d年-*/"%02X-%02X-%02X:%02X:%02X",/*OneFrame.OneRec.RFtime[1],*/\
				OneFrame.OneRec.RFtime[2],OneFrame.OneRec.RFtime[3],OneFrame.OneRec.RFtime[4],\
				OneFrame.OneRec.RFtime[5],OneFrame.OneRec.RFtime[6]);		
		
		Tmp.val = 0;
		memcpy(Tmp.buf,OneFrame.OneRec.RFvaluej,3);
		if(Section.Enable == 0x55)
		{			
			if(OneFrame.OneRec.RFEnterOrExit != 0x55)
				Tmp.val = 0-Tmp.val;			
		}
		MoneyValue((pUserVal+i)->val,Tmp.val);
		//sprintf((pUserVal+i)->val,"%d元",Tmp.val);
		i++;
		if(Saddrs==0)
		{
			SearchNextPage.addr = addrflag;
			break;		
		}
		Saddrs -= 72;
		
	}
	SearchNextPage.addr = Saddrs;
	myflag=1;
#ifdef SAVE_CONSUM_DATA_DIRECT
	close(Datafile);
#else
	fclose(Datafile);
#endif
//	printf("444444444455count=%d\n",count);

	pthread_mutex_unlock(&m_datafile);
//	printf("444444444466count=%d\n",count);
	return i;
}


int GetUserRec_(struct USER_RESERCH *pUserVal)
{
	unsigned int count,Saddrs,i;
	int result;	
	struct{
		RecordFormat OneRec;
		unsigned char space[8];
	}OneFrame;
	union{
		unsigned char buf[4];
		unsigned int val;
	}Tmp;	

    typedef union
	{
		uint64_t i;
		unsigned char longbuf[8];
	}LongLongUnon;
	LongLongUnon cardnum;

    time_t t;
	struct tm *p;
    
	if((pUserVal == NULL)||(SaveNum.i ==0))
		return 0;
	/*if(SaveNum.i > 5)
		count = 5;
	else
		count = SaveNum.i;
    */
/*    if(((SaveNum.i-1)-(nowye-1)*5)>=5)
        count = 5;
    else
        count = (SaveNum.i -1)-(nowye-1)*5;
	*/
	if(((TransactionNum.i-1)-(nowye-1)*5)>5)
        count = 5;
    else
        count = (TransactionNum.i-1)-(nowye-1)*5;
	pthread_mutex_lock(&m_datafile);
//	printf("44444444444422count=%d\n",count);
#ifdef SAVE_CONSUM_DATA_DIRECT
	Datafile = open(OFF_LINE_CONSUM_FILE,O_RDWR|O_SYNC);
#else
	Datafile = fopen(OFF_LINE_CONSUM_FILE,"rb+");
#endif	
    if(Datafile <0)
    {
        printf("open cardsave.bin Err\n");
        return 1;
    }	
	Saddrs = (((SaveNum.i-1)-(nowye-1)*5)-1)*72;	
 //   printf("444444444433count=%d:nowye=%d\n",Saddrs,nowye);
	i = 0;
    
   // if(Saddrs==0)
   // {
   //     Saddrs = count*72;
   //     }	
	while((count-i) > 0)
	{		
	//	printf("444444444433count=%d\n",count);
#ifdef SAVE_CONSUM_DATA_DIRECT
		result = lseek(Datafile,Saddrs, SEEK_SET);
		result = read(Datafile,&OneFrame,sizeof(OneFrame));
		
#else
		result = fseek(Datafile,Saddrs, SEEK_SET);
		result = fread(&OneFrame,sizeof(unsigned char),sizeof(OneFrame),Datafile);	
#endif	
#if ( (defined Transport_Stander)||(defined CANGNAN_BUS))
				
		if((OneFrame.OneRec.RFtype==0xcc)||(OneFrame.OneRec.RFflag==3)||(OneFrame.OneRec.RFflag==0x10))
		{
			if(Saddrs==0)
			{
				break;		
			}
			Saddrs -= 72;
			continue;
		}
        cardnum.i = 0;
		memcpy(cardnum.longbuf,OneFrame.OneRec.RFcsn,8);    
        sprintf((pUserVal+i)->id,"%08d",cardnum.i%100000000);        
	
			
       // sprintf((pUserVal+i)->id,"%02X%02X%02X%02X",OneFrame.OneRec.RFcsn[0],\
       // OneFrame.OneRec.RFcsn[1],OneFrame.OneRec.RFcsn[2],OneFrame.OneRec.RFcsn[3]);
		#if 0
		sprintf((pUserVal+i)->tim, /*"20%d年-%d月-%d日-*/"%d:%d:%d",/*OneFrame.OneRec.RFtime[1],\
				OneFrame.OneRec.RFtime[2],OneFrame.OneRec.RFtime[3],*/OneFrame.OneRec.RFtime[4],\
				OneFrame.OneRec.RFtime[5],OneFrame.OneRec.RFtime[6]);		
		#endif

        Tmp.val = 0;
        memcpy(Tmp.buf,OneFrame.OneRec.RFtime,4);
        t = Tmp.val;       
        p = localtime(&t);        
		sprintf((pUserVal+i)->tim, /*"20%d年-*/"%d-%d-%d:%d:%d",(p->tm_mon+1),
		p->tm_mday,p->tm_hour, p->tm_min, p->tm_sec);

#else
		if(OneFrame.OneRec.RFtime[0]!=0x20)
		{
			if(Saddrs==0)
			{
				break;	
			}
			Saddrs -= 72;
			continue;
		}		
		if(OneFrame.OneRec.RFtype==0xcc)
		{
			if(Saddrs==0)
			{
				break;		
			}
			Saddrs -= 72;
			continue;
		}
		Tmp.val = 0;
		memcpy(Tmp.buf,OneFrame.OneRec.RFcsn,4);
		//sprintf((pUserVal+i)->id,"%d",Tmp.val);		
        sprintf((pUserVal+i)->id,"%02X%02X%02X%02X",OneFrame.OneRec.RFcsn[0],\
        OneFrame.OneRec.RFcsn[1],OneFrame.OneRec.RFcsn[2],OneFrame.OneRec.RFcsn[3]);
		#if 0
		sprintf((pUserVal+i)->tim, /*"20%d年-%d月-%d日-*/"%d:%d:%d",/*OneFrame.OneRec.RFtime[1],\
				OneFrame.OneRec.RFtime[2],OneFrame.OneRec.RFtime[3],*/OneFrame.OneRec.RFtime[4],\
				OneFrame.OneRec.RFtime[5],OneFrame.OneRec.RFtime[6]);		
		#endif
		sprintf((pUserVal+i)->tim, /*"20%d年-*/"%02X-%02X-%02X:%02X:%02X",/*OneFrame.OneRec.RFtime[1],*/\
				OneFrame.OneRec.RFtime[2],OneFrame.OneRec.RFtime[3],OneFrame.OneRec.RFtime[4],\
				OneFrame.OneRec.RFtime[5],OneFrame.OneRec.RFtime[6]);		
#endif		
		Tmp.val = 0;
		memcpy(Tmp.buf,OneFrame.OneRec.RFvaluej,3);
    //    printf("%dOneFrame.OneRec.RFvaluej:%d\n",i,Tmp.val);
		if(Section.Enable == 0x55)
		{			
			if(OneFrame.OneRec.RFEnterOrExit != 0x55)
				Tmp.val = 0-Tmp.val;			
		}
		if ((OneFrame.OneRec.RFpurse == 1) ||(OneFrame.OneRec.RFpurse == 2)||(OneFrame.OneRec.RFpurse >= 0xc8))
			MoneyValue((pUserVal+i)->val,Tmp.val);
		else
			sprintf((pUserVal+i)->val,"%d次",Tmp.val);  //包月。。。次数等
		i++;
		if(Saddrs==0)
        {
			
			break;		
		}
		Saddrs -= 72;
		
	}
	
#ifdef SAVE_CONSUM_DATA_DIRECT
	close(Datafile);
#else
	fclose(Datafile);
#endif
//	printf("444444444455count=%d\n",count);

	pthread_mutex_unlock(&m_datafile);
//	printf("444444444466count=%d\n",count);
	return i;
}
/*
*************************************************************************************************************
- 函数名称 : unsigned char Save_Data_Xor(unsigned char *YDate)
- 函数说明 : 保存数据校验
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
unsigned char Save_Data_Xor(unsigned char *YDate)
{
    unsigned char i,Xor;
    Xor = YDate[0];

    printf("%02X\n ",Xor);
     for(i = 1; i < (SAVE_DATA_LEN-SAVE_DATA_RESERVE-1); i++)
    {
        Xor ^= YDate[i];
        printf("%02X ",YDate[i]);
    }
    printf("%02X\n ",Xor);
	
    return Xor;
}


/*
*************************************************************************************************************
- 函数名称 : char SaveCardData(void)
- 函数说明 : 保存记录
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
char SaveCardData(int sepc, int Mode, int OperationFlag)
{
	LongUnon LBufVal;
	unsigned char status = 0;
	unsigned char *strcharr;
	unsigned char buffin[8];
	unsigned char buffino[8];
	unsigned char buffout[6];
	unsigned char Timebuf[8];

	
	if( OperationFlag & GET_RECORD )
	{
		    memset(&SaveData_new, 0, sizeof(RecordFormat_new));

		   SaveData_new.RType = UserCard.TsRecord.TransType; // 交易类型
		   memcpy(SaveData_new.RSannum, PsamNum, 6);
		 //  memcpy(SaveData_new.RValuej, HostValue.longbuf, 4)  // 交易金额

		memcpy(SaveData_new.RTradenum, DevSID.longbuf, 4);

	        	memcpy(SaveData_new.RSeq, CardLan.UserIcNo,4);


		 memcpy(SaveData_new.RPrebanlance, UserCard.TsRecord.OldMoney, 3);
		 LBufVal.i = 0;
		 memcpy(LBufVal.longbuf, UserCard.TsRecord.OldMoney, 3);
		memcpy(SaveData_new.RValuej, HostValue.longbuf, 4);
		LBufVal.i = LBufVal.i - HostValue.i;
		memcpy(SaveData_new.RValuey,LBufVal.longbuf, 4);


		SaveData_new.RTime[0] = 0x20;
		memcpy(SaveData_new.RTime+1, &Time.year, 6);
		memcpy(SaveData_new.RCsn, UserCard.ScZero.VendorSpace, 6);
		
	        memcpy(SaveData_new.RCardcount, UserCard.ScFive.TransCnt1, 2);
	        SaveData_new.RCardtype = UserCard.ScOne.CardType;
			
		//SaveData_new.RCardattr   // 物理卡类?
		memcpy(SaveData_new.RAuthcode, UserCard.ScOne.AuthCode, 4);

		memcpy(SaveData_new.RCityid, UserCard.ScOne.CityNum, 2);
		memcpy(SaveData_new.RUnionid, UserCard.ScOne.TradeNum, 2);
		memcpy(SaveData_new.RCardissueid, UserCard.ScOne.IssueNum, 4);
		memcpy(SaveData_new.RSerialnum, DevSID.longbuf, 4);
		SaveData_new.RTradestatus = flc0005.gyidibupiaomo;
		SaveData_new.RTradetypeflag = UserCard.TsRecord.TransType;
		//SaveData_new.RKeyver = 
		//SaveData_new.RIssuerid

		SaveData_new.RMarktime[0]= 0x20;
		memcpy(SaveData_new.RMarktime+1, UserCard.ScSix.GetOnTime, 5);
		//memcpy(SaveData_new.RLastcityid, UserCard.ScOne.CityNum, 2);

		
		//设置des加密密码
		memcpy(buffin,SaveData.RFrove,4);
		memcpy(buffin+4,SaveData.RFderno,4);
		memcpy(buffino,CardLan.UserIcNo,4);
		memcpy(buffino+4,SnBack,4);
		DES_CARD(buffin,buffino,buffout);
		memcpy(SaveData.RFDtac,buffout+1,4);

		strcharr = (unsigned char *)(&SaveData_new);
		SaveData.RFXor = Save_Data_Xor(strcharr);
	}	

	if( OperationFlag & SAVE_RECORD )
	{
		status = Savedatasql_new(SaveData_new, 0, 0);
		if (!status)
			IncTransactionNum();
		
		DebugPrintf("####SAVE_RECORD status = 0x%02X\n", status);
	}

	return  status;
}

#if Transport_Stander
extern unsigned char use_jiaotong_stander;

char SaveCardData_jiaotong(int sepc, int Mode, int OperationFlag)
{
    DebugPrintf("\n");
	int i;
	LongUnon LBufVal;
	char status;
	unsigned char *strcharr;
	unsigned char buffin[8];
	unsigned char buffino[8];
	unsigned char buffout[6];

	#ifdef ShenMuBUS
	if(StuToOrd == 1)
	{
		CardLan.CardType = 0x02;
		StuToOrd = 0;
	}
	#endif

 //   printf("use_jiaotong_stander=%02x\n",use_jiaotong_stander);
	if( OperationFlag & GET_RECORD )
	{
		//---start
   		memset(&SaveData,0,sizeof(RecordFormat));
		//---0
		if((CardTypeIC == 0x08)||(use_jiaotong_stander == 0x00))
	        memcpy(SaveData.RFIccsn,CardLan.UserIcNo,4);
		else
			memcpy(SaveData.RFIccsn,CardLanCPU.CSN,4);
		//---2
		if((CardTypeIC == 0x08)||(use_jiaotong_stander == 0x00))
		{
            
			bcd_to_bin(CardLan.CardCsnB, 4);
			BigData.i = big_data;
			memcpy(SaveData.RFcsn,BigData.buff,8);
		}
		else
		{
           
			bcd_to_bin(CardLanCPU.appserialnumber, 10);
            BigData.i = 0;
			BigData.i = big_data;           
			memcpy(SaveData.RFcsn,BigData.buff,8);
		}
    	 #ifdef SUPPORT_QR_CODE	
    	 if (Sector.FlagValue >= QR_CODE_WECHAT_TYPE && Sector.FlagValue <= QR_CODE_MAX_TYPE)
    	 {
    		memcpy(SaveData.RFcsn, G_QRCodeInfo.tranNo.longbuf, 4);
    	 }
         #endif   
		//---3
		if(CardTypeIC == 0x08)
	        memcpy(SaveData.RFrove,DevSID.longbuf,4);
		else
			{
                if(use_jiaotong_stander==0)
                    Chang4to4(DevSID.longbuf, SaveData.RFrove);
                else
                    memcpy(SaveData.RFrove,CardLanCPU.PSAMOfflineSN,4);
          }
        
    		memcpy(LBufVal.longbuf, DevSID.longbuf, 4);
		//---4
		if((CardTypeIC == 0x08)||(use_jiaotong_stander == 0x00))  	
    			SaveData.RFtype = CardLan.CardType;
		else
			//SaveData.RFtype = CardLanCPU.cardtype;
			SaveData.RFtype = CardLan.CardType;
		//---5
        		SaveData.RFXFtype = 0x00;

	//	printf("CardLan.StationDEC=%02x\n",CardLan.StationDEC);
        		if(CardLan.StationDEC != 0xaa)
        		{
            		if(Sector.FlagValue == 0)
            		{
        				memcpy(SaveData.RFvalueq,CardLan.Views,3);
        				memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
        				memcpy(LBufVal.longbuf,CardLan.Views,4);
        				LBufVal.i = LBufVal.i - HostValue.i;
        				memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
        				memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
            		}
        			else if(Sector.FlagValue == 1)
        			{
        				memcpy(SaveData.RFvalueq,CardLan.Subsidies,3);
        				memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
        				memcpy(LBufVal.longbuf,CardLan.Subsidies,4);
        				LBufVal.i = LBufVal.i - HostValue.i;
        				memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
        				memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
        			}
            		else if(Sector.FlagValue == 2)
            		{
						if((CardTypeIC == 0x08)||(use_jiaotong_stander == 0x00))
		        		    memcpy(SaveData.RFvalueq,CardLan.QCash,3);
						else
						{
							SaveData.RFvalueq[0] = CardLanCPU.beforemoney[3];
							SaveData.RFvalueq[1] = CardLanCPU.beforemoney[2];
							SaveData.RFvalueq[2] = CardLanCPU.beforemoney[1];
						}
        				memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
						if((CardTypeIC == 0x08)||(use_jiaotong_stander == 0x00))
						{
		        					memcpy(LBufVal.longbuf,CardLan.QCash,4);
		        					LBufVal.i = LBufVal.i - HostValue.i;
		        					memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
						}
						else
						{
							SaveData.RFvalueh[0] = CardLanCPU.aftermoney[3];
							SaveData.RFvalueh[1] = CardLanCPU.aftermoney[2];
							SaveData.RFvalueh[2] = CardLanCPU.aftermoney[1];
						}
        				memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
            		}
            		else if(Sector.FlagValue == 8)
            		{
        				LBufVal.i = 0;
        				memcpy(SaveData.RFvalueq,LBufVal.longbuf,3);
        				memcpy(SaveData.RFvaluej,LBufVal.longbuf,3);
        				memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
        				memcpy(SaveData.RFvaluey,LBufVal.longbuf,3);
            		}
            		else if((Sector.FlagValue == 9)||(Sector.FlagValue == 10))
            		{
        				memcpy(SaveData.RFvalueq,CardLan.Views,3);
        				memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
        				memcpy(LBufVal.longbuf,CardLan.Views,4);
        				LBufVal.i = LBufVal.i - HostValue.i;
        				memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
        				memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
            		}

            		#if SUPPORT_QR_CODE
            		else if (Sector.FlagValue >= QR_CODE_WECHAT_TYPE && Sector.FlagValue <= QR_CODE_MAX_TYPE)
            		{
            				memset(SaveData.RFvalueq,0,4);
            				memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
            				memset(SaveData.RFvalueh,0,4);
            				memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
            		}
            		#endif                    
        		}
        		else
        		{
            		if(Sector.FlagValue == 1)
            		{
        				memcpy(SaveData.RFvalueq,CardLan.Subsidies,3);
        				memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
        				memcpy(LBufVal.longbuf,CardLan.Subsidies,4);
        				LBufVal.i = LBufVal.i + HostValue.i;
        				memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
        				memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
            		}
            		else if(Sector.FlagValue == 2)
            		{
        				memcpy(SaveData.RFvalueq,CardLan.QCash,3);
        				memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
        				memcpy(LBufVal.longbuf,CardLan.QCash,4);
        				LBufVal.i = LBufVal.i + HostValue.i;
        				memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
        				memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
            		}
        		}

	//	printf("utc_time=%d\n",utc_time);
	
		SaveData.RFtime[0] = (utc_time&0xFF)>>0;
		SaveData.RFtime[1] = (utc_time&0xFF00)>>8;
		SaveData.RFtime[2] = (utc_time&0xFF0000)>>16;
		SaveData.RFtime[3] = (utc_time&0xFF000000)>>24;

#ifdef	PLAY_GROUND_USE
		SaveData.RFtran = 0x0A;// 游乐场
#else
        		SaveData.RFtran = 0x01;//公交
#endif        
		if((CardTypeIC == 0x08)||(use_jiaotong_stander == 0x00))
			memset(SaveData.RFMoneyJF,0,sizeof(SaveData.RFMoneyJF));
		else
			memcpy(SaveData.RFMoneyJF,CardLanCPU.issuerlabel,4);
		if((CardTypeIC == 0x08)||(use_jiaotong_stander == 0x00))
			memset(SaveData.RFMoneyJFbuf,0,sizeof(SaveData.RFMoneyJFbuf));
		else
			memcpy(SaveData.RFMoneyJFbuf,CardLanCPU.citycode,2);
		if((CardTypeIC == 0x08)||(use_jiaotong_stander == 0x00))
			memcpy(SaveData.RFtac, CardLan.ViewMoney,3);
		else
		{
			SaveData.RFtac[0] = (((CardLanCPU.offlineSN[0]<<8|CardLanCPU.offlineSN[1])+1)&0xFF)>>0;
			SaveData.RFtac[1] = (((CardLanCPU.offlineSN[0]<<8|CardLanCPU.offlineSN[1])+1)&0xFF00)>>8;
			SaveData.RFtac[2] = 0x00;
		}
		memcpy(LBufVal.longbuf, CardLan.ViewMoney, 4);

        		if( CardLan.StationDEC == 0x55 )
        		{
            		SaveData.RFpurse = Sector.FlagValue;
        		}
        		else if(CardLan.StationDEC == 0xaa)
        		{
            		SaveData.RFpurse = Sector.FlagValue + 3;
        		}
        		else
        		{
            		SaveData.RFpurse = Sector.FlagValue;
        		}

    		SaveData.RFtimeno = TypeTime.TimeNum;
			
		if((CardTypeIC == 0x08)||(use_jiaotong_stander == 0x00))
    			memcpy(SaveData.RFderno,DevNum.longbuf,4);
		else
			memcpy(SaveData.RFderno,&PsamNum[2],4);
		if((CardTypeIC == 0x08)||(use_jiaotong_stander == 0x00))
    			SaveData.RFEnterOrExit = CardLan.EnterExitFlag;
		else
			SaveData.RFEnterOrExit = CardLanCPU.enterexitflag;

        		if(CardLan.EnterExitFlag == 0x55||CardLanCPU.enterexitflag == 0x55)
        		{
					SaveData.RFcarp = Section.Updown;
					SaveData.RFStationID = Section.SationNow - 1;
        		}
        		else
        		{
					if((CardTypeIC == 0x08)||(use_jiaotong_stander == 0x00))
					{
						SaveData.RFcarp = CardLan.StationOn;
						if((Section.SationNow == 1)&&(CardLan.StationOn == 0)&&(CardLan.StationOn != Section.Updown))
						{
			                			SaveData.RFStationID = Section.SationNum[0] - 1;
			            		}
			            		else if((Section.SationNow == 1)&&(CardLan.StationOn == 1)&&(CardLan.StationOn != Section.Updown))
			            		{
			                			SaveData.RFStationID = Sectionup.SationNum[0] -1;
			            		}
			            		else
			            		{
			                			SaveData.RFStationID = Section.SationNow - 1;
			            		}
					}
					else
					{
						if(CardLanCPU.directionflag==0xAB)
							SaveData.RFcarp = 0x00;
						else
							SaveData.RFcarp = 0x01;
						if((Section.SationNow == 1)&&(SaveData.RFcarp == 0)&&(SaveData.RFcarp != Section.Updown))
						{
			                			SaveData.RFStationID = Section.SationNum[0] - 1;
			            		}
			            		else if((Section.SationNow == 1)&&(SaveData.RFcarp == 1)&&(SaveData.RFcarp != Section.Updown))
			            		{
			                			SaveData.RFStationID = Sectionup.SationNum[0] -1;
			            		}
			            		else
			            		{
			                			SaveData.RFStationID = Section.SationNow - 1;
			            		}
					}
        		}
				
		memcpy(SaveData.RFoperator,PsamNum,2);
      //  printf("Driver.intbuf=%02x%02x\n",Driver.intbuf[0],Driver.intbuf[1]);
		memcpy(&SaveData.RFoperator[2],Driver.intbuf,2);
      //  printf("SaveData.RFoperator=%02x%02x%02x%02x\n",SaveData.RFoperator[0],SaveData.RFoperator[1],SaveData.RFoperator[2],SaveData.RFoperator[3]);
		SaveData.RFflag = sepc;

		SaveData.RFspare = (unsigned char)Mode;	// (定额:0 自由:1 分段:2 计时:3)
		
		if(SaveData.RFflag == 1)					//wsl
		{
            if(use_jiaotong_stander == 0x00)
                memcpy(SaveData.RFDtac,CardLan.CardTac,4);
            else
			    memcpy(SaveData.RFDtac,CardLanCPU.TAC,4);
		}
		else
		{
			//设置des加密密码
			memcpy(buffin,SaveData.RFrove,4);
			memcpy(buffin+4,SaveData.RFderno,4);
			
			memcpy(buffino,CardLan.UserIcNo,4);
			memcpy(buffino+4,SnBack,4);
			DES_CARD(buffin,buffino,buffout);
			memcpy(SaveData.RFDtac,buffout+1,4);
		}
	//	printf("SaveData.RFpurse=%d\n",SaveData.RFpurse);
		strcharr = (unsigned char *)(&SaveData);
		SaveData.RFXor = Save_Data_Xor(strcharr);

		DBG_PRINTF("保存的数据:\n");
		for(i=0;i<64;i++)
			DBG_PRINTF("%02X ",strcharr[i]);
		DBG_PRINTF("\n");

	}	

	if( OperationFlag & SAVE_RECORD )
	{
#if MULTI_FILE_STORAGE
		DBG_PRINTF("使用分页存储\n");
		status=save_record(1,0,0);
#else
		status = Savedatasql(SaveData,0,0);
		IncTransactionNum();
#endif
	//	printf("####SAVE_RECORD status = 0x%02X\n", status);
	}

    	return  status;
}

char SaveCardData_Tao_jiaotong(int sepc, int Mode)
{
    DebugPrintf("\n");
    LongUnon LBufVal;
    char status;
    unsigned char *strcharr;
    unsigned char buffin[8];
    unsigned char buffino[8];
    unsigned char buffout[6];
	
    memset(&SaveData,0,sizeof(RecordFormat));
    memcpy(SaveData.RFIccsn,CardLan.UserIcNo,4);

    
    bcd_to_bin(CardLan.CardCsnB, 4);
	BigData.i = big_data;
	memcpy(SaveData.RFcsn,BigData.buff,8);
    
    memcpy(SaveData.RFrove,DevSID.longbuf,4);
    SaveData.RFtype = CardLan.CardType;
	
	if(bStolenDis)
		SaveData.RFXFtype = 0x06;
	else
    	SaveData.RFXFtype = 0x00;
    //SaveData.RFStationID = CardLan.StationID;


    if(CardLan.StationDEC != 0xaa)
    {
        if(Sector.FlagValue == 0)
        {
            memcpy(SaveData.RFvalueq,CardLan.Views,3);
            memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
            memcpy(LBufVal.longbuf,CardLan.Views,4);
            LBufVal.i = LBufVal.i - HostValue.i;
            memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
            memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
        }
        else if(Sector.FlagValue == 1)
        {
            memcpy(SaveData.RFvalueq,CardLan.Subsidies,3);
            memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
            memcpy(LBufVal.longbuf,CardLan.Subsidies,4);
            LBufVal.i = LBufVal.i - HostValue.i;
            memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
            memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
        }
        else if(Sector.FlagValue == 2)
        {
            memcpy(SaveData.RFvalueq,CardLan.QCash,3);
            memcpy(SaveData.RFvaluej,HostValuet.longbuf,3);   //tao
            memcpy(LBufVal.longbuf,CardLan.QCash,4);
            LBufVal.i = LBufVal.i - HostValuet.i;
            memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
            memcpy(SaveData.RFvaluey,HostValuet.longbuf,3);
        }
        else if(Sector.FlagValue == 8)
        {
            LBufVal.i = 0;
            memcpy(SaveData.RFvalueq,LBufVal.longbuf,3);
            memcpy(SaveData.RFvaluej,LBufVal.longbuf,3);
            memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
            memcpy(SaveData.RFvaluey,LBufVal.longbuf,3);
        }
        else if((Sector.FlagValue == 9)||(Sector.FlagValue == 10))
        {
            memcpy(SaveData.RFvalueq,CardLan.Views,3);
            memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
            memcpy(LBufVal.longbuf,CardLan.Views,4);
            LBufVal.i = LBufVal.i - HostValue.i;
            memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
            memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
        }
    }
    else
    {
        if(Sector.FlagValue == 1)
        {
            memcpy(SaveData.RFvalueq,CardLan.Subsidies,3);
            memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
            memcpy(LBufVal.longbuf,CardLan.Subsidies,4);
            LBufVal.i = LBufVal.i + HostValue.i;
            memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
            memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
        }
        else if(Sector.FlagValue == 2)
        {
            memcpy(SaveData.RFvalueq,CardLan.QCash,3);
            memcpy(SaveData.RFvaluej,HostValuet.longbuf,3); 
            memcpy(LBufVal.longbuf,CardLan.QCash,4);
            LBufVal.i = LBufVal.i + HostValuet.i;
            memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
            memcpy(SaveData.RFvaluey,HostValuet.longbuf,3);
        }
    }

    SaveData.RFtime[0] = (utc_time&0xFF)>>0;
    SaveData.RFtime[1] = (utc_time&0xFF00)>>8;
	SaveData.RFtime[2] = (utc_time&0xFF0000)>>16;
	SaveData.RFtime[3] = (utc_time&0xFF000000)>>24;
    SaveData.RFtran = 0x01;//公交
    memset(SaveData.RFMoneyJF,0,sizeof(SaveData.RFMoneyJF));
    memset(SaveData.RFMoneyJFbuf,0,sizeof(SaveData.RFMoneyJFbuf));
    memcpy(SaveData.RFtac,CardLan.ViewMoney,3);

    if( CardLan.StationDEC == 0x55 )
    {
        SaveData.RFpurse = Sector.FlagValue;
    }
    else if(CardLan.StationDEC == 0xaa)
    {
        SaveData.RFpurse = Sector.FlagValue + 3;
    }
    else
    {
        SaveData.RFpurse = Sector.FlagValue;
    }


    SaveData.RFtimeno = TypeTime.TimeNum;

	memcpy(SaveData.RFderno,CardLan.OldTermNo,4);   //逃票时保存原来终端机号

    SaveData.RFEnterOrExit = CardLan.EnterExitFlag;

    if(CardLan.EnterExitFlag == 0x55)
    {
        SaveData.RFcarp = Section.Updown;
        SaveData.RFStationID = Section.SationNow - 1;
    }
    else
    {
        SaveData.RFcarp = CardLan.StationOn;
        if((Section.SationNow == 1)&&(CardLan.StationOn == 0)&&(CardLan.StationOn != Section.Updown))
        {
            SaveData.RFStationID = Section.SationNum[0] - 1;
        }
        else if((Section.SationNow == 1)&&(CardLan.StationOn == 1)&&(CardLan.StationOn != Section.Updown))
        {
            SaveData.RFStationID = Sectionup.SationNum[0] -1;
        }
        else
        {
            SaveData.RFStationID = Section.SationNow - 1;
        }

    }

    //Infor.i = 0;
    memcpy(SaveData.RFoperator,Driver.intbuf,2);
	SaveData.RFflag = sepc;

    SaveData.RFspare = (unsigned char)Mode;	// (定额:0 自由:1 分段:2 计时:3)

    //设置des加密密码
    memcpy(buffin,SaveData.RFrove,4);
    memcpy(buffin+4,SaveData.RFderno,4);
    memcpy(buffino,CardLan.UserIcNo,4);
    memcpy(buffino+4,SnBack,4);
    DES_CARD(buffin,buffino,buffout);
    memcpy(SaveData.RFDtac,buffout+1,4);

	//	TransactionNum.i++;



    strcharr = (unsigned char *)(&SaveData);
    SaveData.RFXor = Save_Data_Xor(strcharr);
    status =  Savedatasql(SaveData,0,0);
    	int i;
		DBG_PRINTF("保存的数据:\n");
		for(i=0;i<64;i++)
			DBG_PRINTF("%02X ",strcharr[i]);
		DBG_PRINTF("\n");

	//IncTransactionNum();

    return  status;
}

char SaveCardData_shij_jiaotong(int sepc, int Mode)
{
    DebugPrintf("\n");
    LongUnon LBufVal,shij;
    char status;
    unsigned char *strcharr;
        unsigned char buffin[8];
        unsigned char buffino[8];
        unsigned char buffout[6];
		
    memset(&SaveData,0,sizeof(RecordFormat));



        memcpy(SaveData.RFIccsn,CardLan.UserIcNo,4);
        // memcpy(SaveData.RFcity,CardLan.CityId,2);
        // memcpy(SaveData.RFapp,CardLan.AppId,2);
        bcd_to_bin(CardLan.CardCsnB, 4);
		BigData.i = big_data;
		memcpy(SaveData.RFcsn,BigData.buff,8);
        
        memcpy(SaveData.RFrove,DevSID.longbuf,4);
        SaveData.RFtype = CardLan.CardType;
		if(bStolenDis)
			SaveData.RFXFtype = 0x06;
		else
        	SaveData.RFXFtype = 0x00;
        //SaveData.RFStationID = CardLan.StationID;


        if(CardLan.StationDEC != 0xaa)
        {
            if(Sector.FlagValue == 0)
            {
                memcpy(SaveData.RFvalueq,CardLan.Views,3);
                memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
                memcpy(LBufVal.longbuf,CardLan.Views,4);
                LBufVal.i = LBufVal.i - HostValue.i;
                memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
                memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
            }
            else if(Sector.FlagValue == 1)
            {
                memcpy(SaveData.RFvalueq,CardLan.Subsidies,3);
                memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
                memcpy(LBufVal.longbuf,CardLan.Subsidies,4);
                LBufVal.i = LBufVal.i - HostValue.i;
                memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
                memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
            }
            else if(Sector.FlagValue == 2)
            {
            	memcpy(shij.longbuf,CardLan.QCash,4);
                shij.i = shij.i - HostValuet.i;
                memcpy(SaveData.RFvalueq,shij.longbuf,3);
                memcpy(SaveData.RFvaluej,HostValuej.longbuf,3);
                memcpy(LBufVal.longbuf,shij.longbuf,4);
                LBufVal.i = LBufVal.i - HostValuej.i;
                memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
                memcpy(SaveData.RFvaluey,HostValuej.longbuf,3);
            }
            else if(Sector.FlagValue == 8)
            {
                LBufVal.i = 0;
                memcpy(SaveData.RFvalueq,LBufVal.longbuf,3);
                memcpy(SaveData.RFvaluej,LBufVal.longbuf,3);
                memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
                memcpy(SaveData.RFvaluey,LBufVal.longbuf,3);
            }
            else if((Sector.FlagValue == 9)||(Sector.FlagValue == 10))
            {
                memcpy(SaveData.RFvalueq,CardLan.Views,3);
                memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
                memcpy(LBufVal.longbuf,CardLan.Views,4);
                LBufVal.i = LBufVal.i - HostValue.i;
                memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
                memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
            }

        }
        else
        {
            if(Sector.FlagValue == 1)
            {
                memcpy(SaveData.RFvalueq,CardLan.Subsidies,3);
                memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
                memcpy(LBufVal.longbuf,CardLan.Subsidies,4);
                LBufVal.i = LBufVal.i + HostValue.i;
                memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
                memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
            }
            else if(Sector.FlagValue == 2)
            {
                memcpy(SaveData.RFvalueq,CardLan.QCash,3);
                memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
                memcpy(LBufVal.longbuf,CardLan.QCash,4);
                LBufVal.i = LBufVal.i + HostValue.i;
                memcpy(SaveData.RFvalueh,LBufVal.longbuf,3);
                memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
            }
        }

 		SaveData.RFtime[0] = (utc_time&0xFF)>>0;
		SaveData.RFtime[1] = (utc_time&0xFF00)>>8;
		SaveData.RFtime[2] = (utc_time&0xFF0000)>>16;
		SaveData.RFtime[3] = (utc_time&0xFF000000)>>24;
        SaveData.RFtran = 0x01;//公交
        memset(SaveData.RFMoneyJF,0,sizeof(SaveData.RFMoneyJF));
        memset(SaveData.RFMoneyJFbuf,0,sizeof(SaveData.RFMoneyJFbuf));
        memcpy(SaveData.RFtac,CardLan.ViewMoney,2);

        if( CardLan.StationDEC == 0x55 )
        {
            SaveData.RFpurse = Sector.FlagValue;
        }
        else if(CardLan.StationDEC == 0xaa)
        {
            SaveData.RFpurse = Sector.FlagValue + 3;
        }
        else
        {
            SaveData.RFpurse = Sector.FlagValue;
        }


        SaveData.RFtimeno = TypeTime.TimeNum;
        memcpy(SaveData.RFderno,DevNum.longbuf,4);

        SaveData.RFEnterOrExit = CardLan.EnterExitFlag;

        if(CardLan.EnterExitFlag == 0x55)
        {
            SaveData.RFcarp = Section.Updown;
            SaveData.RFStationID = Section.SationNow - 1;
        }
        else
        {
            SaveData.RFcarp = CardLan.StationOn;
            if((Section.SationNow == 1)&&(CardLan.StationOn == 0)&&(CardLan.StationOn != Section.Updown))
            {
                SaveData.RFStationID = Section.SationNum[0] - 1;
            }
            else if((Section.SationNow == 1)&&(CardLan.StationOn == 1)&&(CardLan.StationOn != Section.Updown))
            {
                SaveData.RFStationID = Sectionup.SationNum[0] -1;
            }
            else
            {
                SaveData.RFStationID = Section.SationNow - 1;
            }

        }

        //Infor.i = 0;
        memcpy(SaveData.RFoperator,Driver.intbuf,2);
		SaveData.RFflag = sepc;

        SaveData.RFspare = (unsigned char)Mode;	// (定额:0 自由:1 分段:2 计时:3)

        //设置des加密密码
        memcpy(buffin,SaveData.RFrove,4);
        memcpy(buffin+4,SaveData.RFderno,4);
        memcpy(buffino,CardLan.UserIcNo,4);
        memcpy(buffino+4,SnBack,4);
        DES_CARD(buffin,buffino,buffout);
        memcpy(SaveData.RFDtac,buffout+1,4);

		//TransactionNum.i++;
		

    strcharr = (unsigned char *)(&SaveData);
    SaveData.RFXor = Save_Data_Xor(strcharr);
    status =  Savedatasql(SaveData,0,0);
	if (!status)
	{
		TransactionNum.i++;
		ReadOrWriteFile(WSDATA);
	}
	int i;
		DBG_PRINTF("保存的数据:\n");
		for(i=0;i<64;i++)
			DBG_PRINTF("%02X ",strcharr[i]);
		DBG_PRINTF("\n");
	//IncTransactionNum();

    return  status;
}




#endif

char SaveCardData_Tao(int sepc, int Mode)
{
    LongUnon LBufVal;
    char status;
    unsigned char *strcharr;
    unsigned char buffin[8];
    unsigned char buffino[8];
    unsigned char buffout[6];
	
    memset(&SaveData,0,sizeof(RecordFormat));
    memcpy(SaveData.RFIccsn,CardLan.UserIcNo,4);

    memcpy(SaveData.RFcsn,CardLan.CardCsnB,4);
    memcpy(SaveData.RFrove,DevSID.longbuf,4);
    SaveData.RFtype = CardLan.CardType;
	
	if(bStolenDis)
		SaveData.RFXFtype = 0x06;
	else
    	SaveData.RFXFtype = 0x00;
    //SaveData.RFStationID = CardLan.StationID;


    if(CardLan.StationDEC != 0xaa)
    {
        if(Sector.FlagValue == 0)
        {
            memcpy(SaveData.RFvalueq,CardLan.Views,4);
            memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
            memcpy(LBufVal.longbuf,CardLan.Views,4);
            LBufVal.i = LBufVal.i - HostValue.i;
            memcpy(SaveData.RFvalueh,LBufVal.longbuf,4);
            memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
        }
        else if(Sector.FlagValue == 1)
        {
            memcpy(SaveData.RFvalueq,CardLan.Subsidies,4);
            memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
            memcpy(LBufVal.longbuf,CardLan.Subsidies,4);
            LBufVal.i = LBufVal.i - HostValue.i;
            memcpy(SaveData.RFvalueh,LBufVal.longbuf,4);
            memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
        }
        else if(Sector.FlagValue == 2)
        {
            memcpy(SaveData.RFvalueq,CardLan.QCash,4);
            memcpy(SaveData.RFvaluej,HostValuet.longbuf,3);   //tao
            memcpy(LBufVal.longbuf,CardLan.QCash,4);
            LBufVal.i = LBufVal.i - HostValuet.i;
            memcpy(SaveData.RFvalueh,LBufVal.longbuf,4);
            memcpy(SaveData.RFvaluey,HostValuet.longbuf,3);
        }
        else if(Sector.FlagValue == 8)
        {
            LBufVal.i = 0;
            memcpy(SaveData.RFvalueq,LBufVal.longbuf,4);
            memcpy(SaveData.RFvaluej,LBufVal.longbuf,3);
            memcpy(SaveData.RFvalueh,LBufVal.longbuf,4);
            memcpy(SaveData.RFvaluey,LBufVal.longbuf,3);
        }
        else if((Sector.FlagValue == 9)||(Sector.FlagValue == 10))
        {
            memcpy(SaveData.RFvalueq,CardLan.Views,4);
            memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
            memcpy(LBufVal.longbuf,CardLan.Views,4);
            LBufVal.i = LBufVal.i - HostValue.i;
            memcpy(SaveData.RFvalueh,LBufVal.longbuf,4);
            memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
        }
    }
    else
    {
        if(Sector.FlagValue == 1)
        {
            memcpy(SaveData.RFvalueq,CardLan.Subsidies,4);
            memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
            memcpy(LBufVal.longbuf,CardLan.Subsidies,4);
            LBufVal.i = LBufVal.i + HostValue.i;
            memcpy(SaveData.RFvalueh,LBufVal.longbuf,4);
            memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
        }
        else if(Sector.FlagValue == 2)
        {
            memcpy(SaveData.RFvalueq,CardLan.QCash,4);
            memcpy(SaveData.RFvaluej,HostValuet.longbuf,3); 
            memcpy(LBufVal.longbuf,CardLan.QCash,4);
            LBufVal.i = LBufVal.i + HostValuet.i;
            memcpy(SaveData.RFvalueh,LBufVal.longbuf,4);
            memcpy(SaveData.RFvaluey,HostValuet.longbuf,3);
        }
    }

    SaveData.RFtime[0] = 0x20;
    SaveData.RFtime[1] = Time.year;
    SaveData.RFtime[2] = Time.month;
    SaveData.RFtime[3] = Time.day;
    SaveData.RFtime[4] = Time.hour;
    SaveData.RFtime[5] = Time.min;
    SaveData.RFtime[6] = Time.sec;
    SaveData.RFtran = 0x01;//公交
    memset(SaveData.RFMoneyJF,0,sizeof(SaveData.RFMoneyJF));
    memset(SaveData.RFMoneyJFbuf,0,sizeof(SaveData.RFMoneyJFbuf));
    memcpy(SaveData.RFtac,CardLan.ViewMoney,2);

    if( CardLan.StationDEC == 0x55 )
    {
        SaveData.RFpurse = Sector.FlagValue;
    }
    else if(CardLan.StationDEC == 0xaa)
    {
        SaveData.RFpurse = Sector.FlagValue + 3;
    }
    else
    {
        SaveData.RFpurse = Sector.FlagValue;
    }


    SaveData.RFtimeno = TypeTime.TimeNum;

	memcpy(SaveData.RFderno,CardLan.OldTermNo,4);   //逃票时保存原来终端机号

    SaveData.RFEnterOrExit = CardLan.EnterExitFlag;

    if(CardLan.EnterExitFlag == 0x55)
    {
        SaveData.RFcarp = Section.Updown;
        SaveData.RFStationID = Section.SationNow - 1;
    }
    else
    {
        SaveData.RFcarp = CardLan.StationOn;
        if((Section.SationNow == 1)&&(CardLan.StationOn == 0)&&(CardLan.StationOn != Section.Updown))
        {
            SaveData.RFStationID = Section.SationNum[0] - 1;
        }
        else if((Section.SationNow == 1)&&(CardLan.StationOn == 1)&&(CardLan.StationOn != Section.Updown))
        {
            SaveData.RFStationID = Sectionup.SationNum[0] -1;
        }
        else
        {
            SaveData.RFStationID = Section.SationNow - 1;
        }

    }

    //Infor.i = 0;
    memcpy(SaveData.RFoperator,Driver.intbuf,2);
	SaveData.RFflag = sepc;

    SaveData.RFspare = (unsigned char)Mode;	// (定额:0 自由:1 分段:2 计时:3)

    //设置des加密密码
    memcpy(buffin,SaveData.RFrove,4);
    memcpy(buffin+4,SaveData.RFderno,4);
    memcpy(buffino,CardLan.UserIcNo,4);
    memcpy(buffino+4,SnBack,4);
    DES_CARD(buffin,buffino,buffout);
    memcpy(SaveData.RFDtac,buffout+1,4);

	//	TransactionNum.i++;

    strcharr = (unsigned char *)(&SaveData);
    SaveData.RFXor = Save_Data_Xor(strcharr);
    status =  Savedatasql(SaveData,0,0);

	//IncTransactionNum();

    return  status;
}

char SaveCardData_shij(int sepc, int Mode)
{
    LongUnon LBufVal,shij;
    char status;
    unsigned char *strcharr;
        unsigned char buffin[8];
        unsigned char buffino[8];
        unsigned char buffout[6];
		
    memset(&SaveData,0,sizeof(RecordFormat));



        memcpy(SaveData.RFIccsn,CardLan.UserIcNo,4);
        // memcpy(SaveData.RFcity,CardLan.CityId,2);
        // memcpy(SaveData.RFapp,CardLan.AppId,2);
        memcpy(SaveData.RFcsn,CardLan.CardCsnB,4);
        memcpy(SaveData.RFrove,DevSID.longbuf,4);
        SaveData.RFtype = CardLan.CardType;
		if(bStolenDis)
			SaveData.RFXFtype = 0x06;
		else
        	SaveData.RFXFtype = 0x00;
        //SaveData.RFStationID = CardLan.StationID;


        if(CardLan.StationDEC != 0xaa)
        {
            if(Sector.FlagValue == 0)
            {
                memcpy(SaveData.RFvalueq,CardLan.Views,4);
                memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
                memcpy(LBufVal.longbuf,CardLan.Views,4);
                LBufVal.i = LBufVal.i - HostValue.i;
                memcpy(SaveData.RFvalueh,LBufVal.longbuf,4);
                memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
            }
            else if(Sector.FlagValue == 1)
            {
                memcpy(SaveData.RFvalueq,CardLan.Subsidies,4);
                memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
                memcpy(LBufVal.longbuf,CardLan.Subsidies,4);
                LBufVal.i = LBufVal.i - HostValue.i;
                memcpy(SaveData.RFvalueh,LBufVal.longbuf,4);
                memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
            }
            else if(Sector.FlagValue == 2)
            {
            	memcpy(shij.longbuf,CardLan.QCash,4);
                shij.i = shij.i - HostValuet.i;
                memcpy(SaveData.RFvalueq,shij.longbuf,4);
                memcpy(SaveData.RFvaluej,HostValuej.longbuf,3);
                memcpy(LBufVal.longbuf,shij.longbuf,4);
                LBufVal.i = LBufVal.i - HostValuej.i;
                memcpy(SaveData.RFvalueh,LBufVal.longbuf,4);
                memcpy(SaveData.RFvaluey,HostValuej.longbuf,3);
            }
            else if(Sector.FlagValue == 8)
            {
                LBufVal.i = 0;
                memcpy(SaveData.RFvalueq,LBufVal.longbuf,4);
                memcpy(SaveData.RFvaluej,LBufVal.longbuf,3);
                memcpy(SaveData.RFvalueh,LBufVal.longbuf,4);
                memcpy(SaveData.RFvaluey,LBufVal.longbuf,3);
            }
            else if((Sector.FlagValue == 9)||(Sector.FlagValue == 10))
            {
                memcpy(SaveData.RFvalueq,CardLan.Views,4);
                memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
                memcpy(LBufVal.longbuf,CardLan.Views,4);
                LBufVal.i = LBufVal.i - HostValue.i;
                memcpy(SaveData.RFvalueh,LBufVal.longbuf,4);
                memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
            }

        }
        else
        {
            if(Sector.FlagValue == 1)
            {
                memcpy(SaveData.RFvalueq,CardLan.Subsidies,4);
                memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
                memcpy(LBufVal.longbuf,CardLan.Subsidies,4);
                LBufVal.i = LBufVal.i + HostValue.i;
                memcpy(SaveData.RFvalueh,LBufVal.longbuf,4);
                memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
            }
            else if(Sector.FlagValue == 2)
            {
                memcpy(SaveData.RFvalueq,CardLan.QCash,4);
                memcpy(SaveData.RFvaluej,HostValue.longbuf,3);
                memcpy(LBufVal.longbuf,CardLan.QCash,4);
                LBufVal.i = LBufVal.i + HostValue.i;
                memcpy(SaveData.RFvalueh,LBufVal.longbuf,4);
                memcpy(SaveData.RFvaluey,DecValue.longbuf,3);
            }
        }

        SaveData.RFtime[0] = 0x20;
        SaveData.RFtime[1] = Time.year;
        SaveData.RFtime[2] = Time.month;
        SaveData.RFtime[3] = Time.day;
        SaveData.RFtime[4] = Time.hour;
        SaveData.RFtime[5] = Time.min;
        SaveData.RFtime[6] = Time.sec;
        SaveData.RFtran = 0x01;//公交
        memset(SaveData.RFMoneyJF,0,sizeof(SaveData.RFMoneyJF));
        memset(SaveData.RFMoneyJFbuf,0,sizeof(SaveData.RFMoneyJFbuf));
        memcpy(SaveData.RFtac,CardLan.ViewMoney,2);

        if( CardLan.StationDEC == 0x55 )
        {
            SaveData.RFpurse = Sector.FlagValue;
        }
        else if(CardLan.StationDEC == 0xaa)
        {
            SaveData.RFpurse = Sector.FlagValue + 3;
        }
        else
        {
            SaveData.RFpurse = Sector.FlagValue;
        }


        SaveData.RFtimeno = TypeTime.TimeNum;
        memcpy(SaveData.RFderno,DevNum.longbuf,4);

        SaveData.RFEnterOrExit = CardLan.EnterExitFlag;

        if(CardLan.EnterExitFlag == 0x55)
        {
            SaveData.RFcarp = Section.Updown;
            SaveData.RFStationID = Section.SationNow - 1;
        }
        else
        {
            SaveData.RFcarp = CardLan.StationOn;
            if((Section.SationNow == 1)&&(CardLan.StationOn == 0)&&(CardLan.StationOn != Section.Updown))
            {
                SaveData.RFStationID = Section.SationNum[0] - 1;
            }
            else if((Section.SationNow == 1)&&(CardLan.StationOn == 1)&&(CardLan.StationOn != Section.Updown))
            {
                SaveData.RFStationID = Sectionup.SationNum[0] -1;
            }
            else
            {
                SaveData.RFStationID = Section.SationNow - 1;
            }

        }

        //Infor.i = 0;
        memcpy(SaveData.RFoperator,Driver.intbuf,2);
		SaveData.RFflag = sepc;

        SaveData.RFspare = (unsigned char)Mode;	// (定额:0 自由:1 分段:2 计时:3)

        //设置des加密密码
        memcpy(buffin,SaveData.RFrove,4);
        memcpy(buffin+4,SaveData.RFderno,4);
        memcpy(buffino,CardLan.UserIcNo,4);
        memcpy(buffino+4,SnBack,4);
        DES_CARD(buffin,buffino,buffout);
        memcpy(SaveData.RFDtac,buffout+1,4);

		//TransactionNum.i++;

    strcharr = (unsigned char *)(&SaveData);
    SaveData.RFXor = Save_Data_Xor(strcharr);
    status =  Savedatasql(SaveData,0,0);
	if (!status)
	{
		TransactionNum.i++;
		ReadOrWriteFile(WSDATA);
	}

	//IncTransactionNum();

    return  status;
}


unsigned char SaveDirverData(void)
{
    Sector.FlagValue = 0;
    HostValue.i = 0;
    DecValue.i = 0;
#ifdef Transport_Stander
    SaveCardData_jiaotong(CARD_SPEC_M1_LINUX, CONSUME_MODE_PRESET, GET_RECORD | SAVE_RECORD);
#else
    SaveCardData(CARD_SPEC_M1_LINUX, CONSUME_MODE_PRESET, GET_RECORD | SAVE_RECORD);
#endif
    return 0;
}



/*
char Savedatasql(RecordFormat Save)
{
LongUnon Savebuf;
unsigned char status;
unsigned char sql[1024];
int datastatus;
int rc;
unsigned int csn;
unsigned char buf[20];
unsigned char dbuf[80];

     while(SQL_Falg); 					//数据库不在使用为0
       SQL_Falg = 1;
       status = 1;

    if (access("cardsave.db", 0))			//判断数据库文件是否存在
     {
	  rc = sqlite3_open ("cardsave.db", &dba);
	  if(rc)
 	  {
  		printf("Can't open database: %s\n", sqlite3_errmsg(dba));
  		sqlite3_close(dba);
 	  }
	  memset(sql,'\0',sizeof(sql));
          strcpy(sql,"CREATE TABLE Maindata(ID INTEGER PRIMARY KEY,SaveStatus INTEGER,RFIccsn VARCHAR(8),RFcity INTEGER,RFapp INTEGER,RFcsn INTEGER,RFrove INTEGER,RFtype INTEGER,RFXFtype INTEGER,RFStationID INTEGER,RFvalueq INTEGER,RFvaluej INTEGER,RFtime VARCHAR(14),RFtran INTEGER,RFMoneyJF INTEGER,RFMoneyJFbuf INTEGER,RFtac INTEGER,RFpurse INTEGER,RFvalueh INTEGER,RFvaluey INTEGER,RFtimeno INTEGER,RFderno INTEGER,RFEnterOrExit INTEGER,RFcarp INTEGER,RFoperator INTEGER,RFflag INTEGER,RFspare INTEGER,RFXor INTEGER,Savetime datetime);");
          printf("create db ...\n");
          rc = sqlite3_exec( dba , sql , NULL , NULL , &zErrMsg );
	  if(rc)
 	  {
	         printf("create zErrMsg = %s \n", zErrMsg);
                 sqlite3_close(dba);
	  }
     }
     else
     {

	  rc = sqlite3_open ("cardsave.db", &dba);
	  if( rc )
 	  {
  		printf("Can't open: %s\n", sqlite3_errmsg(dba));
  		sqlite3_close(dba);
                 SQL_Falg = 0;
                return status;
 	  }
     }
     datastatus = 0x55;
     memset(sql,'\0',sizeof(sql));
     sprintf(sql,"INSERT INTO Maindata VALUES( NULL ,%d,'%02X%02X%02X%02X'",datastatus,Save.RFIccsn[0],\
     Save.RFIccsn[1],Save.RFIccsn[2],Save.RFIccsn[3]);		// 芯片号

     memset(buf,'\0',sizeof(buf)); 				// 城市代码
     memcpy(Infor.intbuf,Save.RFcity,2);
     sprintf(buf,",%d",Infor.i);
     strcat(sql,buf);

     memset(buf,'\0',sizeof(buf)); 				// 应用代码
     memcpy(Infor.intbuf,Save.RFapp,2);
     sprintf(buf,",%d",Infor.i);
     strcat(sql,buf);

     csn = BCDToDec(Save.RFcsn,4);   				// 卡号
     memset(buf,'\0',sizeof(buf));
     sprintf(buf,",%d",csn);
     strcat(sql,buf);

     Buf.i = 0;
     memset(buf,'\0',sizeof(buf)); 				// 硬件流水号
     memcpy(Buf.longbuf,Save.RFrove,4);
     sprintf(buf,",%d",Buf.i);
     strcat(sql,buf);

     memset(buf,'\0',sizeof(buf)); 				// 卡类
     sprintf(buf,",%d",Save.RFtype);
     strcat(sql,buf);

     memset(buf,'\0',sizeof(buf)); 				// 消费类型
     sprintf(buf,",%d",Save.RFXFtype);
     strcat(sql,buf);

     memset(buf,'\0',sizeof(buf)); 				// 站台编号
     sprintf(buf,",%d",Save.RFStationID);
     strcat(sql,buf);


     memcpy(Savebuf.longbuf,Save.RFvalueq,4);			// 交易前卡片金额
     memset(buf,'\0',sizeof(buf));
     sprintf(buf,",%d",Savebuf.i);
     strcat(sql,buf);

     Savebuf.i = 0;
     memcpy(Savebuf.longbuf,Save.RFvaluej,3);			// 实交金额
     memset(buf,'\0',sizeof(buf));
     sprintf(buf,",%d",Savebuf.i);
     strcat(sql,buf);


     memset(buf,'\0',sizeof(buf)); 				// 交易时间
     sprintf(buf,",'%02X%02X%02X%02X%02X%02X%02X'",Save.RFtime[0],Save.RFtime[1],Save.RFtime[2],\
     Save.RFtime[3],Save.RFtime[4],Save.RFtime[5],Save.RFtime[6]);
     strcat(sql,buf);

     memset(buf,'\0',sizeof(buf)); 				// 交易类型
     sprintf(buf,",%d",Save.RFtran);
     strcat(sql,buf);

     memcpy(Savebuf.longbuf,Save.RFMoneyJF,4);			// 积分总数
     memset(buf,'\0',sizeof(buf));
     sprintf(buf,",%d",Savebuf.i);
     strcat(sql,buf);

     Savebuf.i = 0;
     memcpy(Savebuf.longbuf,Save.RFMoneyJFbuf,2);		// 积分
     memset(buf,'\0',sizeof(buf));
     sprintf(buf,",%d",Savebuf.i);
     strcat(sql,buf);

     Savebuf.i = 0;
     memcpy(Savebuf.longbuf,Save.RFtac,4);			// 卡片操作次数
     memset(buf,'\0',sizeof(buf));
     sprintf(buf,",%d",Savebuf.i);
     strcat(sql,buf);

     memset(buf,'\0',sizeof(buf)); 				// 交易钱包类型
     sprintf(buf,",%d",Save.RFpurse);
     strcat(sql,buf);


     memcpy(Savebuf.longbuf,Save.RFvalueh,4);			// 交易后卡片金额
     memset(buf,'\0',sizeof(buf));
     sprintf(buf,",%d",Savebuf.i);
     strcat(sql,buf);

     Savebuf.i = 0;
     memcpy(Savebuf.longbuf,Save.RFvaluey,3);			// 应交金额
     memset(buf,'\0',sizeof(buf));
     sprintf(buf,",%d",Savebuf.i);
     strcat(sql,buf);

     memset(buf,'\0',sizeof(buf)); 				// 时间编号
     sprintf(buf,",%d",Save.RFtimeno);
     strcat(sql,buf);


     memset(buf,'\0',sizeof(buf)); 				// 终端机号
     memcpy(Savebuf.longbuf,Save.RFderno,4);
     sprintf(buf,",%d",Savebuf.i);
     strcat(sql,buf);

     memset(buf,'\0',sizeof(buf)); 				// 进出标志
     sprintf(buf,",%d",Save.RFEnterOrExit);
     strcat(sql,buf);

     memset(buf,'\0',sizeof(buf)); 				// 司机编号
     sprintf(buf,",%d",Save.RFcarp);
     strcat(sql,buf);

     memset(buf,'\0',sizeof(buf)); 				// 操作员编号
     memcpy(Infor.intbuf,Save.RFoperator,2);
     sprintf(buf,",%d",Infor.i);
     strcat(sql,buf);

     memset(buf,'\0',sizeof(buf)); 				// 卡标志
     sprintf(buf,",%d",Save.RFflag);
     strcat(sql,buf);

     memset(buf,'\0',sizeof(buf)); 				// 备用
     sprintf(buf,",%d",Save.RFspare);
     strcat(sql,buf);

     memset(buf,'\0',sizeof(buf)); 				// 效验位
     sprintf(buf,",%d",Save.RFXor);
     strcat(sql,buf);

     memset(dbuf,'\0',sizeof(buf)); 				// time
     sprintf(dbuf,",datetime('now','localtime'));");
     strcat(sql,dbuf);

     rc = sqlite3_exec( dba , sql , 0 , 0 , &zErrMsg );
     if( rc != SQLITE_OK )
     {
	   printf("SQL error: %s\n", zErrMsg);
	   sqlite3_free(zErrMsg);
	   status = 1;
     }
     sqlite3_close(dba); 					//关闭数据库

     SQL_Falg = 0;
     return status;
}
*/



/*
*************************************************************************************************************
- 函数名称 :int SqlCheckNewDat (unsigned int Dtype)
- 函数说明 : 查询是否有数据要上传
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
int SqlCheckNewDat (unsigned int Dtype)
{
    int result;
    int Saddr;
    int status;
    char tempbuf[4];
	DebugPrintf("SaveNumBc = %u CodeNum = %u\n", SaveNumBc.i, CodeNum.i);
    if(SaveNumBc.i > CodeNum.i)
    {
        // while(savedataflag);
        //     savedataflag = 1;
        pthread_mutex_lock(&m_datafile);

        Saddr = (int)((CodeNum.i-1) * SAVE_DATA_LEN);
        memset(Senddata, 0, sizeof(Senddata));
#ifdef	 SAVE_CONSUM_DATA_DIRECT
		Datafile = open(OFF_LINE_CONSUM_FILE, O_SYNC|O_RDWR);
		result = lseek(Datafile, Saddr, SEEK_SET);
		result = read(Datafile, Senddata, SAVE_DATA_LEN-SAVE_DATA_RESERVE);
#ifdef Transport_Stander
       if(Senddata[61]==0)
            memcpy(IDaddr.longbuf, Senddata+16, 4);
       else
       {
            memcpy(tempbuf, Senddata+16, 4);
            Chang4to4(tempbuf, IDaddr.longbuf);
            
        }
#else

		memcpy(IDaddr.longbuf, Senddata+12, 4);

#endif
		status = 0;
		close(Datafile);

		
#else  //SAVE_CONSUM_DATA_DIRECT


        Datafile = fopen(OFF_LINE_CONSUM_FILE,"rb+");
        result = fseek(Datafile,Saddr, SEEK_SET);
        result = fread(Senddata,sizeof(unsigned char),64,Datafile);
#ifdef Transport_Stander
       if(Senddata[61]==0)
            memcpy(IDaddr.longbuf, Senddata+16, 4);
       else
       {
            memcpy(tempbuf, Senddata+16, 4);
            ChangChcek(tempbuf, IDaddr.longbuf);
            
        }
#else
		memcpy(IDaddr.longbuf, Senddata+12, 4);
#endif
        status = 0;
        fclose_nosync(Datafile);


#endif //SAVE_CONSUM_DATA_DIRECT

        pthread_mutex_unlock(&m_datafile);

        // savedataflag = 0;

    }
    else
    {
        status = 2;
    }
    return  status;
}



/*
*************************************************************************************************************
- 函数名称 : int SqlCheckNewDat (void)
- 函数说明 : 建立新线程
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
int SqlGetNewDat (unsigned char *Data,unsigned int Dtype,unsigned char num)
{
    int result;
    int status;
    unsigned int Saddr;
    unsigned char i;

    if(SaveNumBc.i > Dtype)
    {
        pthread_mutex_lock(&m_datafile);
        Saddr = (Dtype-1) * 72;
#ifdef SAVE_CONSUM_DATA_DIRECT
		Datafile = open(OFF_LINE_CONSUM_FILE,O_SYNC|O_RDWR);
		for(i = 0; i<num/64; i++)
		{
			result = lseek(Datafile,Saddr+i*72, SEEK_SET);
			result = read(Datafile,Data+i*64,64);
		}

		close(Datafile);

#else
        Datafile = fopen(OFF_LINE_CONSUM_FILE,"rb+");
        for(i = 0; i<num/64; i++)
        {
            result = fseek(Datafile,Saddr+i*72, SEEK_SET);
            result = fread(Data+i*64,sizeof(unsigned char),64,Datafile);
        }

        fclose_nosync(Datafile);
#endif		
        pthread_mutex_unlock(&m_datafile);
        status = 0;
    }
    else
    {

#if   SqlOPEN
        printf("check data Err\n");
#endif
        status = 2;
    }

    return  status;
}


/*
int SqlCheckNewDat (unsigned int Dtype)
{
int index;
int Stu;
int result;
int nRow,nColumn;
char **dbResult;
char* errmsga=NULL;
char bufsql[100];
unsigned char ReadBuf[20];
LongUnon Savebuf;   					// 数据存储
   while(SQL_Falg); 					//数据库不在使用为0
   SQL_Falg = 1;
   Stu = 1;

if (!(access("cardsave.db", 0)))			//判断数据库文件是否存在
{
    result = sqlite3_open("cardsave.db",&dba);
    if(result != SQLITE_OK)
    {
#if   SqlOPEN
        printf("Open sql Rrr \n");
#endif
        sqlite3_close(dba);
        return(-1);
    }
sprintf(bufsql,"select * from Maindata");					//整表查询
result = sqlite3_get_table(dba,bufsql,&dbResult,&nRow,&nColumn,&errmsga);
if(SQLITE_OK==result)
    {
        CodeNumAll.i = nRow;
#if   SqlOPEN
		printf("---CodeNum:%d ----\n",CodeNum.i);
		printf("---CodeNumAll :%d------\n",CodeNumAll.i);
#endif
	if(CodeNum.i < 1) CodeNum.i = 1;
        if((nRow > 0)&&(CodeNum.i <= CodeNumAll.i))
        {
		index = nColumn;


		memset(ReadBuf,0,simemcpy(IDaddr.longbuf,Senddata+12,4);zeof(ReadBuf));				//芯片号
		ascii_2_hex(dbResult[CodeNum.i*index+2],ReadBuf,8);
        	memcpy(SendDataK.RFIccsn,ReadBuf,4);

                memset(ReadBuf,0,sizeof(ReadBuf));				//城市编号
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+3]);
                Savebuf.i = atoi(ReadBuf);
                memcpy(SendDataK.RFcity,Savebuf.longbuf,2);

                memset(ReadBuf,0,sizeof(ReadBuf));				//应用代码
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+4]);
                Savebuf.i = atoi(ReadBuf);
                memcpy(SendDataK.RFapp,Savebuf.longbuf,2);

                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+5]);		//用户卡号
                Savebuf.i = atoi(ReadBuf);
                HEX8TOBCD( Savebuf.i,ReadBuf);
                memcpy(SendDataK.RFcsn,ReadBuf,4);

                memset(ReadBuf,0,sizeof(ReadBuf));				//终端机流水号
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+6]);
                Savebuf.i = atoi(ReadBuf);
                memcpy(SendDataK.RFrove,Savebuf.longbuf,4);

                memset(ReadBuf,0,sizeof(ReadBuf));				//卡类
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+7]);
                Savebuf.i = atoi(ReadBuf);
                SendDataK.RFtype = Savebuf.longbuf[0];

                memset(ReadBuf,0,sizeof(ReadBuf));				//消费类别
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+8]);
                Savebuf.i = atoi(ReadBuf);
                SendDataK.RFXFtype = Savebuf.longbuf[0];

                memset(ReadBuf,0,sizeof(ReadBuf));				//站台编号
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+9]);
                Savebuf.i = atoi(ReadBuf);
                SendDataK.RFStationID = Savebuf.longbuf[0];

                memset(ReadBuf,0,sizeof(ReadBuf));				//交易前卡片金额
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+10]);
                Savebuf.i = atoi(ReadBuf);
                memcpy(SendDataK.RFvalueq,Savebuf.longbuf,4);

                memset(ReadBuf,0,sizeof(ReadBuf));				//实际交易金额
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+11]);
                Savebuf.i = atoi(ReadBuf);
                memcpy(SendDataK.RFvaluej,Savebuf.longbuf,3);

		memset(ReadBuf,0,sizeof(ReadBuf));				//交易时间
		ascii_2_hex(dbResult[CodeNum.i*index+12],ReadBuf,14);
                memcpy(SendDataK.RFtime,ReadBuf,7);

                memset(ReadBuf,0,sizeof(ReadBuf));				//交易类型
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+13]);
                Savebuf.i = atoi(ReadBuf);
                SendDataK.RFtran = Savebuf.longbuf[0];

                memset(ReadBuf,0,sizeof(ReadBuf));				//总积分
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+14]);
                Savebuf.i = atoi(ReadBuf);
                memcpy(SendDataK.RFMoneyJF,Savebuf.longbuf,4);

		memset(ReadBuf,0,sizeof(ReadBuf));				//积分数
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+15]);
                Savebuf.i = atoi(ReadBuf);
                memcpy(SendDataK.RFMoneyJFbuf,Savebuf.longbuf,2);

		memset(ReadBuf,0,sizeof(ReadBuf));				//卡片操作次数
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+16]);
                Savebuf.i = atoi(ReadBuf);
                memcpy(SendDataK.RFtac,Savebuf.longbuf,4);

                memset(ReadBuf,0,sizeof(ReadBuf));				//交易钱包类型
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+17]);
                Savebuf.i = atoi(ReadBuf);
                SendDataK.RFpurse = Savebuf.longbuf[0];

                memset(ReadBuf,0,sizeof(ReadBuf));				//交易后余额
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+18]);
                Savebuf.i = atoi(ReadBuf);
                memcpy(SendDataK.RFvalueh,Savebuf.longbuf,4);

                memset(ReadBuf,0,sizeof(ReadBuf));				//原交易金额
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+19]);
                Savebuf.i = atoi(ReadBuf);
                memcpy(SendDataK.RFvaluey,Savebuf.longbuf,3);

                memset(ReadBuf,0,sizeof(ReadBuf));				//时间编号
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+20]);
                Savebuf.i = atoi(ReadBuf);
                SendDataK.RFtimeno = Savebuf.longbuf[0];

                memset(ReadBuf,0,sizeof(ReadBuf));				//终端机号
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+21]);
                Savebuf.i = atoi(ReadBuf);
                memcpy(SendDataK.RFderno,Savebuf.longbuf,4);

                memset(ReadBuf,0,sizeof(ReadBuf));				//进出标志
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+22]);
                Savebuf.i = atoi(ReadBuf);
                SendDataK.RFEnterOrExit = Savebuf.longbuf[0];

                memset(ReadBuf,0,sizeof(ReadBuf));				//司机编号
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+23]);
                Savebuf.i = atoi(ReadBuf);
                SendDataK.RFcarp = Savebuf.longbuf[0];

                memset(ReadBuf,0,sizeof(ReadBuf));				//操作员
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+24]);
                Savebuf.i = atoi(ReadBuf);
                memcpy(SendDataK.RFoperator,Savebuf.longbuf,2);

                memset(ReadBuf,0,sizeof(ReadBuf));				//卡标志
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+25]);
                Savebuf.i = atoi(ReadBuf);
                SendDataK.RFflag = Savebuf.longbuf[0];

                memset(ReadBuf,0,sizeof(ReadBuf));				//备用
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+26]);
                Savebuf.i = atoi(ReadBuf);
                SendDataK.RFspare = Savebuf.longbuf[0];

                memset(ReadBuf,0,sizeof(ReadBuf));				//效验位
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index+27]);
                Savebuf.i = atoi(ReadBuf);
                SendDataK.RFXor = Savebuf.longbuf[0];

                memset(IDaddr.longbuf,0,sizeof(IDaddr.longbuf));
                memset(ReadBuf,0,sizeof(ReadBuf));				//数据库ID
                sprintf(ReadBuf,"%s",dbResult[CodeNum.i*index]);
                IDaddr.i = atoi(ReadBuf);

                Stu = 0;
        }
     }
   sqlite3_free_table(dbResult);
   sqlite3_close(dba);
   SQL_Falg = 0;
 }

SQL_Falg = 0;
return (Stu);
}
*/
/*
*************************************************************************************************************
- 函数名称 : void * ReadGprs_Pthread (void * args)
- 函数说明 : 自动上传数据
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void * Readsql_Pthread (void * args)
{
    int status = 0xff;
    AutoUpFlag = 0x55;
    while(1)
    {
        if(ConnectFlag == 0)  //自动上传数据
        {
            if((AutoUpFlag == 0x55)||(AutoUpFlag == 0xBB))
            {
                if(AutoUpFlag == 0xBB)
                {
#if 1	//SqlOPEN
                    printf("--------------------------------------->\n");
#endif
                    if(CodeNum.i >= 1)
                    {
#ifdef XIAN_MIMA
						CodeNum.i += 5;
#else
	                    CodeNum.i ++;
#endif
      
                        ReadOrWriteFile (CODEFILE);  //写上传记录数
                    }
                }
                AutoUpFlag = 0xaa;
                #ifdef CANGNAN_BUS
                status = SqlCheckNewDat_Zhujian(0x55);   //判断是否有数据要上传
                if(status == 0)
                {
                    WaveCOmPc_Zhujian(Senddata); //自动上传数据
                    // AutoUpFlag = 0xAA;
                    usleep(200000);
                }
                #else
                status = SqlCheckNewDat(0x55);   //判断是否有数据要上传
                if(status == 0)
                {
                    WaveCOmPc(Senddata); //自动上传数据
                    // AutoUpFlag = 0xAA;
                    usleep(200000);
                }
                #endif
                else
                {
                    sleep(5);
                }
            }
            else
            {
                usleep(200000);
            }
        }
        else
        {
            sleep(5);
        }
    }
    
}

