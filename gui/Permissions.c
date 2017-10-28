#include <apparel.h>
#include "RC500.h"
#include "InitSystem.h"
#include "des.h"
#include "cpucard.h"


#define PRINT_NAND 0
#define PERMISS    0


extern SysTime Time;

extern LongUnon HostValue,DecValue;
extern unsigned char *CardLanBuf;
extern unsigned char *SectionParBuf,*SectionParUpBuf;
extern CardInform CardLan;
extern JackRegal Sector;
extern JackValue  MoneyNo;
extern unsigned char CardTypeIC;
extern unsigned short SectionNum;
extern LongUnon DevNum;
extern SectionFarPar Section,Sectionup;


static LongUnon JackArm,Test,Buf;
//extern unsigned char CardModeFalg;
unsigned char NandBuf[512];
unsigned char LastCardNum[4];	//�ϴ�ˢ������
RouteJack   TypeTime;
RouteSelection JackCmd;
FixTimerBuf FixTime;
Interval  retdata;	//ʱ����
NumValue CIshu;
ShortUnon Infor;
unsigned char g_FgWriteNoDiscount = 0;
unsigned char g_FgNoDiscount=0;


#if Transport_Stander
extern  CardInformCPU CardLanCPU;
extern unsigned char use_jiaotong_stander;
extern char PsamNum;
extern char PsamKeyIndex;
#endif

unsigned char CheckSwipeCardAfter(void)
{
	    unsigned int min,hour,sec, i;
   	 LongUnon Buf,Test,JackArm;
	   unsigned int total_sec1, total_sec2;
	 if((CardLan.FirstTimeDiscount[0] == Time.year)&&(CardLan.FirstTimeDiscount[1] == Time.month)&&(CardLan.FirstTimeDiscount[2] == Time.day))
	    {
	        Buf.i = 0;
	        Buf.longbuf[1]	= Time.sec;
	        Buf.longbuf[2]	= Time.min;
	        Buf.longbuf[3]	= Time.hour;
	        Test.i = 0;
	        Test.longbuf[1] = CardLan.FirstTimeDiscount[5];
	        Test.longbuf[2] = CardLan.FirstTimeDiscount[4];
	        Test.longbuf[3] = CardLan.FirstTimeDiscount[3];
	        if(Buf.i >= Test.i)
	        {
	            for(i = 0 ; i < 4; i++)Buf.longbuf[i]  = BCD2HEX(Buf.longbuf[i]);
	            for(i = 0 ; i < 4; i++)Test.longbuf[i] = BCD2HEX(Test.longbuf[i]);
		  #if 0
	            if(Buf.longbuf[1] < Test.longbuf[1])
	            {
	                Buf.longbuf[1] += 60;
	                Buf.longbuf[2]--;
	            }
	            sec = Buf.longbuf[1] - Test.longbuf[1];
	            if(Buf.longbuf[2] < Test.longbuf[2])
	            {
	                Buf.longbuf[2] += 60;
	                Buf.longbuf[3]--;
	            }
	            min = Buf.longbuf[2] - Test.longbuf[2];
	            if(Buf.longbuf[3] >= Test.longbuf[3])
	            {
	                hour = Buf.longbuf[3] - Test.longbuf[3];
	            }
	            else
	            {
	            	printf(" 1CheckSwipeCardAfter return 1\n");
	                return 1;
	            }
		   JackArm.i = hour*3600 + min*60 + sec;
		  #else
		    total_sec1 =  Buf.longbuf[1] +  Buf.longbuf[2]*60 +  Buf.longbuf[3]*3600;
		    total_sec2 =  Test.longbuf[1] +  Test.longbuf[2]*60 +  Test.longbuf[3]*3600;
		  if (total_sec1 >= total_sec2)
		  {
			JackArm.i = total_sec1 - total_sec2;
		  }
		  else
		  {
		  	//printf(" 1CheckSwipeCardAfter return 1\n");
			return 1;
		  }
		  #endif
	          
		 // printf("===CardAfter second = %d\n", JackArm.i);
	            if(JackArm.i <= 120)   //two mins
	            {
	            	 return 0;
	            }
	        	}
	    }
	//printf(" 2CheckSwipeCardAfter return 1\n");
	 return 1;
}

#ifdef SHENGKE_TIANGUAN
/*
�����������趨��ת���Ż�ʱ���ڣ�
���˵�һ��ˢ�����Żݣ������Ǯ
*/
unsigned char g_FgDiscntRegTime = 0;
unsigned char CheckStatusCardSwipe(void)
{
	unsigned char status = 1;
	if(memcmp(DevNum.longbuf, CardLan.OldTermNo, 4) == 0)   //the same of device
	{
		status = 0;// normal exchange
	}
	else
	{
		if(g_FgDiscntRegTime == 1)
			status = 1;
		else
			status = 0;
	}
	printf("CheckStatusCardSwipe = %d\n", status);
	return status;
}

#endif


/*
*************************************************************************************************************
- �������� : unsigned char Discounts(unsigned char Type)
- ����˵�� : �ÿ�����Ϣ�ʹ򿨼��ʱ��������ʺ��Ż�Ʊ��//����ת���Żݱ�
- ������� : Type    //����
- ������� : 1.��ʾHostValue��Ч
             0.��ʾHostValue��Ч
             Ӱ��HostValue��ֵ��Ϊ��� 15940503044
*************************************************************************************************************
*/
unsigned char  ValueDiscounts(unsigned char Type,unsigned int sec)
{
    unsigned char i,status = 1;
    unsigned char CsnBuf[9];
    ShortUnon Tim,INbff;
    for(i = 0; i < 64; i++)
    {
        memcpy(CsnBuf,NandBuf+(i * 8),8);
        if((CsnBuf[0] == 0x55)&&(CsnBuf[1] == 0xAA)) break;
        Tim.i  = 0;
        INbff.i  = 0;
        memcpy(Tim.intbuf,CsnBuf+1,2);
        memcpy(INbff.intbuf,CsnBuf+3,2);
	//printf(" ValueDiscounts time para=%d, %d, %d\n", Tim.i, INbff.i, g_FgDiscntRegTime);
	#if 0//def SHENGKE_TIANGUAN
	 if((CsnBuf[0] == Type)&&(Tim.i > sec)&&(sec >= 0))
	 {
		return 0;
	 }
	 else
	#endif
        if((CsnBuf[0] == Type)&&(Tim.i <= sec)&&(sec < INbff.i))
        {
		    #ifdef SHENGKE_TIANGUAN
		    status = CheckStatusCardSwipe();
		    #elif HANGJIA_BUS
            status  = 1;
            #else
           	status = CheckSwipeCardAfter(); //20170323 status = 1;
           	#endif
		    if (status)  //��һ��ת�˴���
		    {
		            INbff.i = 0;
		            memcpy(INbff.intbuf, CsnBuf+6, 2);
		            HostValue.i = ((HostValue.i*CsnBuf[5])/100);
		            if(HostValue.i >= INbff.i)HostValue.i  = HostValue.i - INbff.i;
		            status = 0xaa;
		            i = 200;
		    }
		    else
  	     	   {
  	     		  status = 0;  //���������ڲ����״�ˢ����������
  	     	    }
	            break;
        }
    }
	//printf(" 1ValueDiscounts return =%d\n", status);
    return status;
}

/***************************�����ϴδ�ʱ����������ε�ʱ����******************************************************/
unsigned char Discounts(unsigned char Type)
{
    unsigned char buf,i,status = 1;
    unsigned int min,hour,sec;
    LongUnon Buf,Test,JackArm;
   unsigned int total_sec1, total_sec2;

#if defined(CONFIG_BZLINUXBUS)

    if(CardTypeIC == 0x08)
    {
        buf = Sector.SFivZero[14]&0x0f;	//��ȡ�������ͣ�1��ֵ��2������3���ѣ�4����
    }
    else
    {
//        if(CardLan.OldTransType == 0x06)
//        {
            buf = 0x03;
  //      }
//        else
//        {
//            buf = 0;
//        }
    }

#else
    buf = 0x03;
#endif
#if 0
  printf("===ye===NoDiscountTime:");
   for(i=0;i<6;i++)
  	printf("%02x ", CardLan.NoDiscountTime[i]);
   printf("\n");
   
  printf("===ye===Time NOW:");
   for(i=0;i<6;i++)
  	printf("%02x ", ((unsigned char*)&Time)[i]);
   printf("\n");
 #endif
 
     // if((CardLan.OldTime[0] == Time.year)&&(CardLan.OldTime[1] == Time.month)&&(CardLan.OldTime[2] == Time.day)&&(buf==0x03))
#ifdef HANGJIA_BUS
    memcpy(&CardLan.NoDiscountTime[0], &CardLan.OldTime[0],6);
#endif
   if((CardLan.NoDiscountTime[0] == Time.year)&&(CardLan.NoDiscountTime[1] == Time.month)&&(CardLan.NoDiscountTime[2] == Time.day)&&(buf==0x03))
   {

        Buf.i = 0;
        Buf.longbuf[1]  = Time.sec;
        Buf.longbuf[2]  = Time.min;
        Buf.longbuf[3]  = Time.hour;
        Test.i = 0;
        Test.longbuf[1] = CardLan.NoDiscountTime[5]; //OldTime[5]
        Test.longbuf[2] = CardLan.NoDiscountTime[4]; //OldTime[4]
        Test.longbuf[3] = CardLan.NoDiscountTime[3];//OldTime[3]
        if(Buf.i >= Test.i)
        {
            for(i = 0 ; i < 4; i++) Buf.longbuf[i]  = BCD2HEX(Buf.longbuf[i]);
            for(i = 0 ; i < 4; i++) Test.longbuf[i] = BCD2HEX(Test.longbuf[i]);
	  #if 0
            if(Buf.longbuf[1] < Test.longbuf[1])
            {
                   Buf.longbuf[1] += 60;	
                  	 Buf.longbuf[2]--;
            }
            sec = Buf.longbuf[1] - Test.longbuf[1];
            if(Buf.longbuf[2] < Test.longbuf[2])
            {
                Buf.longbuf[2] += 60;
                Buf.longbuf[3]--;
            }
            min = Buf.longbuf[2] - Test.longbuf[2];
            if(Buf.longbuf[3] >= Test.longbuf[3])
            {
                hour = Buf.longbuf[3] - Test.longbuf[3];
            }
            else
            {
            	printf(" 1Discounts return 1\n");
                return 1;
            }
	JackArm.i = hour*3600 + min*60 + sec;
	  #else
	  total_sec1 =  Buf.longbuf[1] +  Buf.longbuf[2]*60 +  Buf.longbuf[3]*3600;
	  total_sec2 =  Test.longbuf[1] +  Test.longbuf[2]*60 +  Test.longbuf[3]*3600;
	  if (total_sec1 >= total_sec2)
	  {
		JackArm.i = total_sec1 - total_sec2;
	  }
	  else
	  {
	  	//printf(" 1Discounts return 1\n");
		return 1;				//����ʱ��С�ڿ�Ƭˢ��ʱ��
	  }
	  #endif
	  
            
	printf("===Discounts second = %d\n\n", JackArm.i);
            if(JackArm.i <= 86400)
            {
                status =  ValueDiscounts(Type,JackArm.i);
            }
        }
    }
  // printf(" 1Discounts return %d\n", status);
    return status;
}





/*
*************************************************************************************************************
- �������� : unsigned char CardDiscountsValue(unsigned char Type)
- ����˵�� : �жϿ�Ƭʱ����
- ������� : ��
- ������� : �������
*************************************************************************************************************
*/
unsigned char CardDiscountsValue(unsigned char Type,unsigned int sec)
{
    unsigned char i,status = 1;
    LongUnon Buf;

    memcpy(NandBuf,CardLanBuf,512);
    for(i = 0; i < 32; i++)
    {
        if((NandBuf[i*5] == 0x55)&&(NandBuf[i*5+1] == 0xAA))break;
        if(NandBuf[i*5] == Type)
        {
            memcpy(Buf.longbuf,NandBuf+i*5+1,4);
            if(sec < Buf.i)
            {
                status = 0;
            }
            break;
        }
    }
    return status;
}

/*
*************************************************************************************************************
- �������� : unsigned char CardDiscounts(unsigned char Type)
- ����˵�� : �жϿ�Ƭʱ����
- ������� : ��
- ������� : �������
*************************************************************************************************************
*/
unsigned char CardDiscounts(unsigned char Type,unsigned char Mode)
{
    unsigned char buf,i,status = 1;
    unsigned int min,hour,sec;
    LongUnon JackArm,Buf,Test;
  unsigned int total_sec1, total_sec2;
#if defined(CONFIG_BZLINUXBUS)

    if(CardTypeIC == 0x08)
    {
        buf = Sector.SFivZero[14]&0x0f;
    }
    else
    {
//        if(CardLan.OldTransType == 0x06)
//        {
            buf = 0x03;
//        }
//        else
//        {
//            buf = 0;
//        }
    }

#else
    buf = 0x03;
#endif

#if 0//def SUPPORT_QR_CODE
	if (Type == QR_CODE_TYPE)
	{
	        buf = 0x03;
	        memcpy(CardLan.OldTime, QRCode_OldTime, 6);  //���ж�ʱ�������ػ��ٿ�����Ч
	}
#endif

	DebugPrintf("DevNum=%02x%02x%02x%02x\n",DevNum.longbuf[0],DevNum.longbuf[1],DevNum.longbuf[2],DevNum.longbuf[3]);
	DebugPrintf("CardLan.OldTermNo=%02x%02x%02x%02x\n",CardLan.OldTermNo[0],CardLan.OldTermNo[1],CardLan.OldTermNo[2],CardLan.OldTermNo[3]);
    if((memcmp(DevNum.longbuf,CardLan.OldTermNo,4)==0)&&(CardLan.OldTime[0] == Time.year)&&(CardLan.OldTime[1] == Time.month)&&(CardLan.OldTime[2] == Time.day)&&((buf == 0x03)||(buf == 0x04)))
    {
        Buf.i = 0;
        Buf.longbuf[1]	= Time.sec;
        Buf.longbuf[2]	= Time.min;
        Buf.longbuf[3]	= Time.hour;
        Test.i = 0;
        Test.longbuf[1] = CardLan.OldTime[5];
        Test.longbuf[2] = CardLan.OldTime[4];
        Test.longbuf[3] = CardLan.OldTime[3];
        if(Buf.i >= Test.i)
        {
            for(i = 0 ; i < 4; i++)Buf.longbuf[i]  = BCD2HEX(Buf.longbuf[i]);
            for(i = 0 ; i < 4; i++)Test.longbuf[i] = BCD2HEX(Test.longbuf[i]);
	#if 0
            if(Buf.longbuf[1] < Test.longbuf[1])
            {
                Buf.longbuf[1] += 60;
                Buf.longbuf[2]--;
            }
            sec = Buf.longbuf[1] - Test.longbuf[1];
            if(Buf.longbuf[2] < Test.longbuf[2])
            {
                Buf.longbuf[2] += 60;
                Buf.longbuf[3]--;
            }
            min = Buf.longbuf[2] - Test.longbuf[2];
            if(Buf.longbuf[3] >= Test.longbuf[3])
            {
                hour = Buf.longbuf[3] - Test.longbuf[3];
            }
            else
            {
                return 1;
            }

            JackArm.i = hour*3600 + min*60 + sec;
	#else
	  total_sec1 =  Buf.longbuf[1] +  Buf.longbuf[2]*60 +  Buf.longbuf[3]*3600;
	  total_sec2 =  Test.longbuf[1] +  Test.longbuf[2]*60 +  Test.longbuf[3]*3600;
	  if (total_sec1 >= total_sec2)
	  {
		JackArm.i = total_sec1 - total_sec2;
	  }
	  else
	  {
		return 1;
	  }
	  #endif


#if  PRINT_NAND
            printf("CardDiscounts = == %d \n",JackArm.i);
#endif

            if(JackArm.i <= 86400)
            {
                status = CardDiscountsValue(Type,JackArm.i);
            }
		  
        }
    }
    return status;
}

#if Transport_Stander
unsigned char CardDiscounts_Cpu_jiaotong(unsigned char Type,unsigned char Mode)
{
	unsigned char buf,i,status = 1;
    unsigned int min,hour,sec;
    LongUnon JackArm,Buf,Test;
	unsigned char *p,*q;
	unsigned int total_sec1, total_sec2;	
	//printf("in carddiscounts_cpu_jiaotong\n");

	p=&PsamNum;
	q=&CardLanCPU.deviceNO;
	for(i=0;i<6;i++){
		if((*(p+i))!= (*(q+i)))
			break;
	}
	p=NULL;
	q=NULL;
	
	if((CardLanCPU.tradetype==0x09||CardLanCPU.tradetype==0x06)&&(i==6)&&(CardLanCPU.tradedate[1]== Time.year)&&(CardLanCPU.tradedate[2] == Time.month)&&(CardLanCPU.tradedate[3] == Time.day))
    {
    	
		Buf.i = 0;
		Buf.longbuf[1]	= Time.sec;
		Buf.longbuf[2]	= Time.min;
		Buf.longbuf[3]	= Time.hour;
		Test.i = 0;
		Test.longbuf[1] = CardLanCPU.tradetime[2];
		Test.longbuf[2] = CardLanCPU.tradetime[1];
		Test.longbuf[3] = CardLanCPU.tradetime[0];

		
		if(Buf.i >= Test.i)
		{
			for(i = 0 ; i < 4; i++)Buf.longbuf[i]  = BCD2HEX(Buf.longbuf[i]);
			for(i = 0 ; i < 4; i++)Test.longbuf[i] = BCD2HEX(Test.longbuf[i]);
			
			total_sec1 =  Buf.longbuf[1] +  Buf.longbuf[2]*60 +  Buf.longbuf[3]*3600;
	 		total_sec2 =  Test.longbuf[1] +  Test.longbuf[2]*60 +  Test.longbuf[3]*3600;

			if(total_sec1>=total_sec2){
				JackArm.i=total_sec1-total_sec2;
			}
			else{
				return 1;
			}
/*			
			if(Buf.longbuf[1] < Test.longbuf[1])
			{
					Buf.longbuf[1] += 60;
					Buf.longbuf[2]--;
			}
			sec = Buf.longbuf[1] - Test.longbuf[1];
			if(Buf.longbuf[2] < Test.longbuf[2])
			{
					Buf.longbuf[2] += 60;
					Buf.longbuf[3]--;
			}
			min = Buf.longbuf[2] - Test.longbuf[2];
			if(Buf.longbuf[3] >= Test.longbuf[3])
			{
					hour = Buf.longbuf[3] - Test.longbuf[3];
			}
			else
			{
					return 1;
			}
		
			JackArm.i = hour*3600 + min*60 + sec;
#if  PRINT_NAND
            printf("CardDiscounts = == %d \n",JackArm.i);
#endif

*/
    		if(JackArm.i <= 86400)
    		{
        			status = CardDiscountsValue(Type,JackArm.i);
    		}
		  
        }
    }

	return status;
		
}

#endif

/***********************************************************
		���п�ʵ��ʱ����
************************************************************/

unsigned char QpbocCardDiscounts(unsigned char Type)
{
    unsigned char buf,i,status = 1;
    unsigned int min,hour,sec;
    LongUnon JackArm,Buf,Test;
    unsigned int total_sec1, total_sec2;
	
	DBG_PRINTF("QpbocCardDiscounts() is called.\n");
	memcpy(&min, CardLan.OldTime, 4);	
	if((min == 0)||(min == 0xFFFFFFFF))
		return 1;

    if((CardLan.OldTime[0] == Time.year)&&(CardLan.OldTime[1] == Time.month)&&(CardLan.OldTime[2] == Time.day))
    {
        Buf.i = 0;
        Buf.longbuf[1]	= Time.sec;
        Buf.longbuf[2]	= Time.min;
        Buf.longbuf[3]	= Time.hour;
        Test.i = 0;
        Test.longbuf[1] = CardLan.OldTime[5];
        Test.longbuf[2] = CardLan.OldTime[4];
        Test.longbuf[3] = CardLan.OldTime[3];
        if(Buf.i >= Test.i)
        {
            for(i = 0 ; i < 4; i++)Buf.longbuf[i]  = BCD2HEX(Buf.longbuf[i]);
            for(i = 0 ; i < 4; i++)Test.longbuf[i] = BCD2HEX(Test.longbuf[i]);

	#if 0
            if(Buf.longbuf[1] < Test.longbuf[1])
            {
                Buf.longbuf[1] += 60;
                Buf.longbuf[2]--;
            }
            sec = Buf.longbuf[1] - Test.longbuf[1];
            if(Buf.longbuf[2] < Test.longbuf[2])
            {
                Buf.longbuf[2] += 60;
                Buf.longbuf[3]--;
            }
            min = Buf.longbuf[2] - Test.longbuf[2];
            if(Buf.longbuf[3] >= Test.longbuf[3])
            {
                hour = Buf.longbuf[3] - Test.longbuf[3];
            }
            else
            {
            	//printf("sec:%d,min:%d,hour is negtive \n", sec,min);
                return 1;
            }

            JackArm.i = hour*3600 + min*60 + sec;
	#else
	  total_sec1 =  Buf.longbuf[1] +  Buf.longbuf[2]*60 +  Buf.longbuf[3]*3600;
	  total_sec2 =  Test.longbuf[1] +  Test.longbuf[2]*60 +  Test.longbuf[3]*3600;
	  if (total_sec1 >= total_sec2)
	  {
		JackArm.i = total_sec1 - total_sec2;
	  }
	  else
	  {
		return 1;
	  }
	#endif

#if  PRINT_NAND
            printf("CardDiscounts = == %d \n",JackArm.i);
#endif


        //  if((Mode == 2)&&(CardLan.EnterExitFlag == 0x55))
        //  {       
        //      return 1; 
		//  }

            if(JackArm.i <= 86400)
            {
                status = CardDiscountsValue(Type,JackArm.i);
            }
		  
        }
    }
    return status;
}




/*
*************************************************************************************************************
- �������� : unsigned char TimeRange(void)
- ����˵�� : �жϿ�Ƭ���ڵĺϷ�����
- ������� : ��
- ������� : �������
			0����ʾ�Ϸ�����
			1����Ƭû�е���������
			2����Ƭ����
*************************************************************************************************************
*/
unsigned char TimeRange(void)
{
    unsigned char i,status = 0x00;
    unsigned char buff[8];
    unsigned char TRBuf[8];
    unsigned char TimeBuf[512];
    unsigned char h;
    unsigned char stp = 1;
    LongUnon Timbuf,JackArm,Test,Buf;

	DBG_PRINTF("TimeRange() is called.\n");

    for(h = 0; h < 5; h++)
    {
        Rd_time (buff);
        Time.year = buff[0];
        Time.month = buff[1];
        Time.day = buff[2];
        Time.hour = buff[3];
        Time.min = buff[4];
        Time.sec = buff[5];

        Buf.i = 0;
        Buf.longbuf[0] = Time.sec;
        Buf.longbuf[1] = Time.min;
        Buf.longbuf[2] = Time.hour;
        memcpy(TimeBuf,CardLanBuf+7*512,512);
        for(i = 0; i < 70; i++)
        {
            memset(TRBuf,0,sizeof(TRBuf));
            memcpy(TRBuf,TimeBuf + (i*7), 7 );
            if((TimeBuf[i*7] == 0x55)&&(TimeBuf[i*7+1] == 0xAA)) break;

            Test.i = 0;
            Test.longbuf[0] = TRBuf[3];
            Test.longbuf[1] = TRBuf[2];
            Test.longbuf[2] = TRBuf[1];
            JackArm.i = 0;
            JackArm.longbuf[0] = TRBuf[6];
            JackArm.longbuf[1] = TRBuf[5];
            JackArm.longbuf[2] = TRBuf[4];


            Timbuf.i = 0;
            Timbuf.longbuf[0] = 0;
            Timbuf.longbuf[1] = 0;
            Timbuf.longbuf[2] = 0x24;

#if PRINT_NAND
            //   printf("Buf.i = %d\n",Buf.i);
            //   printf("Test.i = %d\n",Test.i);
            //   printf("JackArm.i = %d\n",JackArm.i);
#endif
            if(Test.i < JackArm.i)
            {
                if((Buf.i > Test.i)&&(Buf.i < JackArm.i))
                {
                    status = TRBuf[0];
                    TypeTime.TimeNum = TRBuf[0];
                    break;
                }
            }
            else
            {
                if(((Buf.i > Test.i)&&(Buf.i < Timbuf.i))||((Buf.i > 0)&&(Buf.i < JackArm.i)))
                {
                    status = TRBuf[0];
                    TypeTime.TimeNum = TRBuf[0];
                    break;
                }
            }
        }
        if(status != 0x00)break;
        else
        {
            if(stp == 1)
            {
                Read_Parameter();

#if PRINT_NAND
                printf("\n\n ----- Read_Parameter  Tw0 \n\n");
#endif

            }
            stp = 0;
        }
    }

#if PRINT_NAND
    printf("TypeTime = %d\n",status);
#endif

    return status;
}


/*
*************************************************************************************************************
- �������� : unsigned char SupportType(void)
- ����˵�� : �жϿ����Ƿ�֧��
- ������� : ��
- ������� : �������
			0����ʾ�Ϸ�����
			1�����಻֧��
*************************************************************************************************************
*/
unsigned char SupportType(void)
{
    unsigned char i = 0,status = 1;
	unsigned char cardtype_0xAA = 0;
	DBG_PRINTF("SupportType() is called.\n");

    // printf("SupportType::== %02X\n",CardLan.CardType);
    memcpy(NandBuf,CardLanBuf+1*512,512);
#ifdef SUPPORT_QR_CODE
  for(i = 0; i<0xff; i++)
#else
    for(i = 0; i< 32; i++)
#endif
    {
        if((NandBuf[i] == 0x55)&&(NandBuf[i+1] == 0xAA)) break;
        else
        {
        		if (CardLan.CardType == 0xAA)
        		{
			if (NandBuf[i] == CardLan.CardType) //��Ϊ��βҲ��0XAA�������ҵ�����AA����
			{
				cardtype_0xAA++;
				if (cardtype_0xAA == 2)
				{
					return 0;
				}
			}
		}
		else
		{
            		if(NandBuf[i] == CardLan.CardType)
				status = 0;
		}
        }
        if(status == MI_OK)break;
    }
    // printf("\n");

    return status;
}


/*
*************************************************************************************************************
- �������� : unsigned char ParseRMB(RouteJack Rj)
- ����˵�� : //�����۸��
- ������� : Rj
	   Rj.Type    //����
	   Rj.TimeNum //ʱ�α��
	        ����ȥ����
- ������� : 1.��ʾHostValue��Ч
           0.��ʾHostValue��Ч
             	        Ӱ��HostValue��ֵ��Ϊ���
*************************************************************************************************************
*/
unsigned char ParseRMB(RouteJack Rj,unsigned char FlagV)
{
    unsigned char  status = 1;
    unsigned int i;

	DBG_PRINTF("ParseRMB() is called.\n");

 #if(defined QINGDAO_TONGRUAN)//#ifdef QINGDAO_TONGRUAN
	printf("====times use = %d\n ", Sector.STwoZero[0]);
	if (g_FindTheDiscount)
	{
		g_FindTheDiscount = 0;
		  if ( Sector.STwoZero[0] <= MAX_DISCOUNT_TIMES)
		 {
		 	 memcpy(HostValue.longbuf, g_DiscountOfMoney.longbuf, 4);
               		 memcpy(DecValue.longbuf, g_DiscountOfMoney.longbuf, 4);
			 HostValue.i = (HostValue.i*g_SepcialDiscount)/100;
			 return 0;
		  }
	}
#endif

    for(i = 0 ; i < 72; i++)
    {
    	
        if((NandBuf[i*7] == 0x55)&&(NandBuf[i*7+1] == 0xAA))
        {
            DecValue.i = 0;
            HostValue.i = 0;
            status = 2;
            break;
        }

        if((NandBuf[i*7] == Rj.Type)&&(NandBuf[(i*7) + 1] == Rj.TimeNum))
        {
            if(FlagV)
            {   

                #ifdef GUNZHA
                HostValue.longbuf[0] =  NandBuf[i*7 + 6];     //CS���
	  			HostValue.longbuf[1] =  NandBuf[i*7 + 5]; 
	  			HostValue.longbuf[2] =  NandBuf[i*7 + 4]; 
	  			HostValue.longbuf[3] =  NandBuf[i*7 + 3];
	  			DecValue.longbuf[0]  =  NandBuf[i*7 + 6]; 
	  			DecValue.longbuf[1]  =  NandBuf[i*7 + 5]; 
	  			DecValue.longbuf[2]  =  NandBuf[i*7 + 4]; 
	  			DecValue.longbuf[3]  =  NandBuf[i*7 + 3];

                #else
                memcpy(HostValue.longbuf,NandBuf+i*7 + 3,4);
                memcpy(DecValue.longbuf,NandBuf+i*7 + 3,4);             
                #endif
            }
            else
            {
                DecValue.i = HostValue.i;
            }
#ifdef Transport_Stander
         //   printf("CardLanCPU.IsLocalCard=%02x:line=%d\n",CardLanCPU.IsLocalCard,__LINE__);
 
            if(CardLanCPU.IsLocalCard!=2)                           //��ͨ����׼�����ؿ����ۣ���ؿ�������
                HostValue.i = (HostValue.i*NandBuf[i*7 + 2])/100;            
                
#else
			HostValue.i = (HostValue.i*NandBuf[i*7 + 2])/100;
#endif

            status = 0;
            break;
        }
    }
    //printf("ParseRMBstatus = %d\n",status);
    return status;
}
/*
*************************************************************************************************************
- �������� : unsigned char DiscountRate(void)
- ����˵�� : ���ʱ����
- ������� : num :������Ϣ���
- ������� : ��
*************************************************************************************************************
*/
unsigned char DiscountRate(unsigned char Type)
{
    unsigned char status = 1,i;
    unsigned char CsnBuf[11];
    for(i = 0; i < 51 ; i++)
    {
        memcpy(CsnBuf,NandBuf+(i * 10),10);
#if	DBG_RC500
		unsigned int j;
		printf("\n rate : ");
		for(j = 0; j < 10; j++)
		{
			printf(" 0x%x ", CsnBuf[j]);
		}	
		printf("\n");
#endif		
        if((CsnBuf[0] == 0x55)&&(CsnBuf[1] == 0xAA))break;


	

        // Test.longbuf[0] = CsnBuf[4];
        //  Test.longbuf[1] = CsnBuf[3];
        //   Test.longbuf[2] = CsnBuf[2];
        //  Test.longbuf[3] = CsnBuf[1];
        memcpy(Test.longbuf,CsnBuf+1,4);
        memcpy(Buf.longbuf,CsnBuf+5,4);
        //  Buf.longbuf[0] = CsnBuf[8];
        //  Buf.longbuf[1] = CsnBuf[7];
        //  Buf.longbuf[2] = CsnBuf[6];
        // Buf.longbuf[3] = CsnBuf[5];
        if((Type == CsnBuf[0])&&(Test.i <= HostValue.i)&&(Buf.i > HostValue.i))
        {
            HostValue.i = (HostValue.i*CsnBuf[9])/100;
            status = 0;
            i = 200;			
        }		
    }
    return status;
}
/*
*************************************************************************************************************
- �������� : unsigned char IntegralSheet(void)
- ����˵�� : �жϿ�Ƭ����ĺϷ�����
- ������� : ��
- ������� : �������
			0����ʾ��Ƭ���Ǻ�����
			1����ʾ��Ƭ�Ѿ��Ǻ�����
*************************************************************************************************************
*/
unsigned char IntegralSheet(unsigned char Type)
{
    unsigned char i,status = 1;
    unsigned char CsnBuf[12];
    ShortUnon PaGeF;
//LongUnon JIfen;
    for(i = 0; i < 51; i++)
    {
        memcpy(CsnBuf,NandBuf+(i * 10),10);
        if((CsnBuf[0] == 0x55)&&(CsnBuf[1] == 0xAA)) break;
        if(CsnBuf[0] == Type)
        {
            Buf.i = 0;     //���ַ�ΧСֵ
            Test.i = 0;    //���ַ�Χ��ֵ
            memcpy(Buf.longbuf,CsnBuf+1,3);
            memcpy(Test.longbuf,CsnBuf+4,3);
            memcpy(PaGeF.intbuf,CsnBuf+7,3);
            // for(j = 0;j < 3 ;j++)Buf.longbuf[j] = CsnBuf[3 - j];
            // for(j = 0;j < 3 ;j++)Test.longbuf[j] = CsnBuf[6 - j];
            // PaGeF.intbuf[0] = CsnBuf[8];
            // PaGeF.intbuf[1] = CsnBuf[7];
            // JackArm.i  = 0;

            if((Buf.i < HostValue.i)&&(Test.i >=  HostValue.i))
            {
                switch(CsnBuf[9])
                {
                case 0:
                    //JackArm.i = PaGe.i;//����
                    //memcpy(JIfen.longbuf,CardLan.MoneyJack,4);
                    //JIfen.i += PaGeF.i;
                    //memcpy(LuRegal.FiveOne + 8,JIfen.longbuf,4);
                    break;
                case 1:	  //����
                    //HostValue.i = (HostValue.i*PaGeF.i)/100;
                    break;
                default:
                    //JackArm.i = 0;
                    break;
                }
                status = 0;
            }
            //Uart_Printf(0,"fenMin = %d,fenMax = %d,zhelv = %d,HostValue = %d,Fen = %d\n", Buf.i, Test.i,Page.i,HostValue.i,JackArm.i);
        }

    }
    return status;
}
/*
*************************************************************************************************************
- �������� : unsigned char Date_MinLooK(void)
- ����˵�� : ������ʱ�䡣
- ������� : ��
- ������� : ����������
*************************************************************************************************************
*/

unsigned char Date_MinLooK(unsigned char *Start,unsigned char *End)
{
    unsigned char  month[]= {00,31,28,31,30,31,30,31,31,30,31,30,31};
    LongUnon          New,Old;
    unsigned char     ch,st,en,i;
    unsigned int      yue,year;
    unsigned char     StartBuf[6];
    unsigned char     EndBuf[6];
    unsigned char     status = 1;

    sprintf(StartBuf,"20%02x",Start[0]);
    New.i = myatoi(StartBuf);
    if(((New.i%4 == 0)&&(New.i%100!=0))||(New.i%400 == 0))
    {
        month[2]+=1;
    }
    sprintf(EndBuf,"20%02x",End[0]);
    New.i = myatoi(EndBuf);
    year = (unsigned short)(New.i);

    StartBuf[0] = 0x14;
    EndBuf[0]   = 0x14;
    for(ch = 0; ch < 5; ch ++)
    {
        StartBuf[ch+1] = BCD2HEX(Start[ch]);//��ʼ
        EndBuf[ch+1]   = BCD2HEX(End[ch]);    //��
    }
    retdata.date = 0;
    retdata.min = 0;
    New.i = 0;
    Old.i = 0;

    for(i = 0; i< 3; i++)
    {
        New.longbuf[i] = StartBuf[2 - i];
        Old.longbuf[i] = EndBuf[2 - i];
    }

    if(New.i <= Old.i)    //������ʱ�䣨��ǰʱ��϶��ȿ�ʼʱ���
    {
        New.i = 0;
        Old.i = 0;
        Old.longbuf[0] = EndBuf[1];
        Old.longbuf[1] = EndBuf[0];
        New.longbuf[0] = StartBuf[1];
        New.longbuf[1] = StartBuf[0];
        ch = (unsigned char)(Old.i - New.i);
        if(ch < 2)
        {
            if(ch == 0)//��ͬ����
            {
                en = EndBuf[2];  //��
                st = StartBuf[2];//��
                yue = 0;
                for(i = 0; i < st; i++)
                {
                    yue = yue + month[i];
                }
                New.i = (unsigned int)((yue + StartBuf[3]) * 1440  + StartBuf[4] * 60 + StartBuf[5] + 1);
                yue = 0;
                for(i = 0; i < en; i++)
                {
                    yue = yue + month[i];
                }
                Old.i = (unsigned int)((yue  + EndBuf[3]) * 1440  + EndBuf[4] * 60 + EndBuf[5] + 1);
                retdata.date = (unsigned short)((Old.i - New.i) / 1440);
                retdata.min  = (unsigned short)((Old.i - New.i) % 1440);
            }
            else//����һ��
            {
                st = StartBuf[2];//��
                yue = 0;
                for(i = 0; i < st; i++)
                {
                    yue = yue + month[i];
                }
                New.i = (unsigned int)((yue + StartBuf[3]) * 1440  + StartBuf[4] * 60 + StartBuf[5] + 1);

                st = 12;
                yue = 0;
                for(i = 0; i < st; i++)
                {
                    yue = yue + month[i];
                }
                Old.i = (unsigned int)((yue + 31) * 1440  + 23 * 60 + 60);
                retdata.date = (unsigned short)((Old.i - New.i) / 1440);
                retdata.min  = (unsigned short)((Old.i - New.i) % 1440);
                if(((New.i%4 == 0)&&(New.i%100!=0))||(New.i%400 == 0))
                {
                    month[2] = 29;
                }
                else
                {
                    month[2] = 28;
                }
                en = EndBuf[2];
                yue = 0;
                for(i = 0; i < en; i++)
                {
                    yue = yue + month[i];
                }
                Old.i = (unsigned int)((yue  + EndBuf[3]) * 1440  + EndBuf[4] * 60 + EndBuf[5] + 1);
                retdata.date = (unsigned short)(retdata.date  + (Old.i / 1440));
                retdata.min  = (unsigned short)(retdata.min   + (Old.i % 1440));
                retdata.date += retdata.min / 1440;
                retdata.min   = retdata.min % 1440;
                if(retdata.date >= 1)retdata.date--;
            }
            status = 0;
        }
    }
    return status;
}


#ifdef QINGDAO_TONGRUAN
/*
����һЩ���۵Ŀ���   ����+XX XX XX XX XX ֧�����10�����ʣ��������10�����࣬����ŵ����ű�����
*/
unsigned char GetDiscountFromPara(unsigned char type)
{
	int i;
	unsigned char tmp;
	unsigned char AddBuf[512];
	unsigned char TRBuf[60];
	//TypeTime.Type = type
	g_SepcialDiscount = 100; //����Ϊ100 ��ʼ��
	memset(g_DiscountOfMoney.longbuf, 0, 4);
	if (Sector.STwoZero[0] >  MAX_DISCOUNT_TIMES)  return 1;  //max is 5 
	
	memcpy(AddBuf,CardLanBuf+5*512,512);
	tmp = Sector.STwoZero[0]-1;
	for(i = 0; i < 10; i++)
	{
		 memset(TRBuf,0,sizeof(TRBuf));
	       	memcpy(TRBuf,AddBuf + (i*51), 51 );
	       	if((AddBuf[i*51] != 0x55)&&(AddBuf[i*51+1] != 0xAA))
	       	{
			if (type == TRBuf[0]) //card type
			{
				g_SepcialDiscount = TRBuf[tmp*5+1];
				memcpy(g_DiscountOfMoney.longbuf, &TRBuf[tmp*5+2], 4);
				//printf("money %d, %d, %d, %d\n", TRBuf[tmp*5+2], TRBuf[tmp*5+3], TRBuf[tmp*5+4], TRBuf[tmp*5+5]);
				break;
			}
		}
	}

	if(i == 10) return 2;
	
	//printf("GetDiscountFromParae = 0x%x, add = %d, money %d\n", type,  g_SepcialDiscount, g_DiscountOfMoney.i);
	return 0;
}
#endif

/*
*************************************************************************************************************
- �������� : char ReadTimeMoney(void)
- ����˵�� : �жϿ�Ƭ����ĺϷ�����
- ������� : ��
- ������� : �������
*************************************************************************************************************
*/
/*
char ReadTimeMoney(void)
{
unsigned char i,status = 1;
unsigned char Loop =1,step = 1;
//unsigned int j;
while(Loop)
{
    switch(step)
    {
	    case 1:
                printf("sflsjadlfjasd\n");
                //printf("\n");
		memcpy(NandBuf,CardLanBuf+12*512,512);
                //for(j = 0; j < 512; j++)
                //{
                //     printf("%02X ",NandBuf[j]);
		//     if((j+1)%16 == 0) printf("\n");
                //}
                //printf("\n");
                if((LastCardNum[0] == CardLan.CardCsnB[0])&&(LastCardNum[1] == CardLan.CardCsnB[1])&&
                   (LastCardNum[2] == CardLan.CardCsnB[2])&&(LastCardNum[3] == CardLan.CardCsnB[3]))
                {
	        	if(CardLan.EnterExitFlag == 0x55)step ++;			        //0x55��ʾ�� 0xAA��ʾ��
	        	else step = 3;

                }
                else step = 3;                							//���Ų�ͬ
	        break;
	    case 2:										//�˴�Ϊ��״̬
		for(i = 0; i < 46;i++)
		{
                        if((NandBuf[11*i] == 0x55)&&(NandBuf[11*i+1] == 0xaa)) {Loop = 0;break;}
		        if((CardLan.CardType == NandBuf[11*i])&&(CardLan.Period == NandBuf[11*i+1]))
		        {
				memcpy(FixTime.TimeBuf,NandBuf+11*i+2,2);
                                memcpy(Infor.intbuf,FixTime.TimeBuf,2);
				status = Date_MinLooK((unsigned char *)&CardLan.OldTime,(unsigned char *)&Time);
				if(status != MI_OK)
                                {
                                     CardLan.EnterExitFlag = 0xAA;
                                     step ++;
                                     break;
				}
				Buf.i = retdata.date * 1440 + retdata.min;
                                if(Infor.i >= Buf.i)
                                {
                                     Infor.i = Infor.i - Buf.i;
				     Buf.longbuf[0] = NandBuf[11*i+4];
				     Buf.longbuf[1] = NandBuf[11*i+5];
				     Buf.longbuf[2] = NandBuf[11*i+6];
				     Buf.longbuf[3] = 0;
                                     Buf.i =  Buf.i * Infor.i;
		                     DecValue.i = Buf.i;
		                     HostValue.i = Buf.i;
				     memcpy(FixTime.Money,HostValue.longbuf,3);			//���۸�
	                             FixTime.IncDecFlag = 0xaa;					//0xaa��Ǯ
#if PERMISS
				     printf("FixTime.Inc= %02X\n",FixTime.IncDecFlag);
				     printf("FixTime.Money = %d\n",HostValue.i);
                                     printf("Infor.i = %d\n",Infor.i);
#endif
		                     status = 0;
                                     step ++;
                                     step ++;
		                     break;
                                }
                                else
                                {
                                     CardLan.EnterExitFlag = 0xaa;
                                     step ++;
                                     break;
                                }
		        }
	        }
		if(i == 46) Loop = 0;
	        break;
	    case 3:
		for(i = 0; i < 46;i++)								//�˴�Ϊ��״̬
		{
			if((NandBuf[11*i] == 0x55)&&(NandBuf[11*i+1] == 0xaa)) {Loop = 0;break;}
                        //printf("CardLan.CardType = %d\n",CardLan.CardType);
                        //printf("TypeTime.TimeNum = %d\n",TypeTime.TimeNum);
                        //printf("NandBuf[11*i] = %d\n",NandBuf[11*i]);
                        //printf("NandBuf[11*i+1]) = %d\n",NandBuf[11*i+1]);
		        if((CardLan.CardType == NandBuf[11*i])&&(TypeTime.TimeNum == NandBuf[11*i+1]))
		        {
				Buf.longbuf[0] = NandBuf[11*i+4];
				Buf.longbuf[1] = NandBuf[11*i+5];
				Buf.longbuf[2] = NandBuf[11*i+6];
				Buf.longbuf[3] = 0;
	                        memcpy(FixTime.Money,Buf.longbuf,3);					//Ԥ�ۼ۸�
				memcpy(FixTime.TimeBuf,NandBuf+11*i+2,2);				//Ԥ��ʱ��
				memcpy(Infor.intbuf,FixTime.TimeBuf,2);
	                        Buf.i =  Buf.i * Infor.i;
		                DecValue.i = Buf.i;
		                HostValue.i = Buf.i;
	                        FixTime.IncDecFlag = 0x55;
		                status = 0;
#if PERMISS
				printf("FixTime.Dec= %02X\n",FixTime.IncDecFlag);
				printf("FixTime.Money = %d\n",HostValue.i);
                                printf("Infor.i = %d\n",Infor.i);
#endif
                                step ++;
		                break;
		        }
	        }
		if(i == 46) Loop = 0;
	        break;
	    default:
                Loop = 0;
	        break;
    }
}
return(status);
}
*/
/*
*************************************************************************************************************
- �������� : char ReadMoney(void)
- ����˵�� : �жϿ�Ƭ����ĺϷ�����
- ������� : ��
- ������� : �������
*************************************************************************************************************
*/
/*
char ReadMoney(void)
{
	unsigned char i,status = 1;
	for(i = 0; i < 72;i++)
        {
              if((TypeTime.TimeNum == CardLanBuf[7*i])&&(CardModeFalg == CardLanBuf[7*i+1]))
              {
		    Buf.longbuf[0] = CardLanBuf[7*i+5];
		    Buf.longbuf[1] = CardLanBuf[7*i+4];
		    Buf.longbuf[2] = CardLanBuf[7*i+3];
		    Buf.longbuf[3] = CardLanBuf[7*i+2];
                    DecValue.i = Buf.i;
                    Buf.i = Buf.i * CardLanBuf[7*i+6]/100;
                    HostValue.i = Buf.i;
#if PRINT_NAND
        printf("\n�����۸��HostValue = %d",HostValue.i);
#endif
                    status = 0;
                    break;
              }
        }
        return(status);
}
*/
/*
*************************************************************************************************************
- �������� : unsigned char AnalysisSheet(unsigned char TypeVae,)
- ����˵�� : �жϿ�Ƭ����ĺϷ�����
- ������� : ��
- ������� : �������
*************************************************************************************************************
*/
/*
unsigned char AnalysisSheet(unsigned char TypeVae)
{
unsigned char i,status;
	TypeTime.Type = CardLan.CardType;
        DecValue.i = HostValue.i = 0;
        if((CardModeFalg == 1)||(CardModeFalg == 2))
        {
        	status = ReadMoney();
        	if(status != MI_OK) return(4);
        }
        else
        {
        	status = ReadTimeMoney();
        	if(status != MI_OK) return(4);
        }
	for(i = 0; i < 6;i++)
	{
	    if((JackCmd.Parameter[i] > 7)&&(JackCmd.Parameter[i] < 24))
	    {
                 memcpy(NandBuf,CardLanBuf+JackCmd.Parameter[i]*512,512);
		 switch(JackCmd.Parameter[i])
		 {
		 case 8:   //�����۸����
		 case 9:
		 case 10:
		 case 11:
			break;
		 case 12:  //����ת���Żݱ���
		 case 13:
		 case 14:
		 case 15:
			break;
		 case 16:  //�������ʱ���
		 case 17:
		 case 18:
		 case 19:
#if PRINT_NAND
		 	printf("\n�������ʱ� PaGe = %02d,TypeTime.Type = %d",JackCmd.Parameter[i],TypeTime.Type);
#endif
		 	status = DiscountRate(TypeTime.Type);
#if PRINT_NAND
		 	if(status == MI_OK)
				printf("   �۸� = %d",HostValue.i);
			else
#endif
			if(status == 2)i = 200;
#if PRINT_NAND
			printf("\n");
#endif
			break;
		 case 20:   //�������ֱ���
		 case 21:
		 case 22:
		 case 23:
#if PRINT_NAND
		 	printf("\n�������ֱ� PaGe = %02d",JackCmd.Parameter[i]);
#endif
		        status = IntegralSheet(TypeTime.Type);
#if PRINT_NAND
		        if(status == MI_OK)
				printf("   �۸� = %d",HostValue.i);
			else
#endif
			if(status == 2)i = 200;
#if PRINT_NAND
			printf("\n");
#endif
			break;
		 case 24: //���޴�������
		 case 25:
		 case 26:
		 case 27:
			break;
		 case 28: //���޼۸����
		 case 29:
		 case 30:
		 case 31:
			break;
	         default:
			break;
		}
	   }
	}
	if(i == 6)
	{
		status = 0;
		//memcpy(FixTime.Money,HostValue.longbuf,3);
	}
	else if(i == 201)status = 2;
	else if(i == 206)status = 3;
	else status = 1;
	return status;
}
*/
/*
*************************************************************************************************************
- �������� : unsigned char AnalysisSheet(unsigned char TypeVae,)
- ����˵�� : �жϿ�Ƭ����ĺϷ�����
- ������� : ��
- ������� : �������
*************************************************************************************************************
*/
unsigned char AnalysisSheet(unsigned char TypeVae)
{
    unsigned char i;
    unsigned char status=1,b=0;

	DBG_PRINTF("AnalysisSheet() is called.\n");

    TypeTime.Type = CardLan.CardType;
    memcpy(JackCmd.Parameter,CardLanBuf+2*512,16);
    for(i = 0; i < 6; i++)
    {
        if((JackCmd.Parameter[i] > 7)&&(JackCmd.Parameter[i] < 24))
        {
            memcpy(NandBuf,CardLanBuf+JackCmd.Parameter[i]*512,512);
            switch(JackCmd.Parameter[i])
            {
            case 8://�����۸����
            case 9:
            case 10:
            case 11:
				
#if PRINT_NAND
                printf("\n�����۸�� PaGe = %02d ��ʼ�۸�:%d \n",JackCmd.Parameter[i],HostValue.i);
#endif
                if(TypeVae)
                {
                    status = ParseRMB(TypeTime,TypeVae);//����ĺϷ���
                    if(status == 2)i = 200;
                    else if(status == 1)i = 205;
                }
                else
                {
                    status = ParseRMB(TypeTime,0);//����ĺϷ���
                    if(status == 2)i = 200;
                    else if(status == 1)i = 205;
                }

	#ifdef QINGDAO_TONGRUAN
		g_DisplayMoney.i = DecValue.i; //0����ʾXXԪ
	#endif


#if PRINT_NAND
                printf("status == %d �۸� = %d  \n",status,HostValue.i);
#endif
                break;
            case 12:  //����ת���Żݱ���
            case 13:
            case 14:
            case 15:
#if PRINT_NAND
                printf("\nת���Żݱ� PaGe = %02d ת���Żݱ�׼=%d\n",JackCmd.Parameter[i], g_FgNoDiscount);
#endif
                if(g_FgNoDiscount)
                    break;
                status = Discounts(TypeTime.Type);
	#ifdef CONFIG_BZLINUXBUS  
                g_FgWriteNoDiscount = 0;
	#ifdef SHENGKE_TIANGUAN
		if (status == 0xaa)
	       	{	
	       		g_FgDiscntRegTime = 0;
			g_FgWriteNoDiscount = 2;
		 }
		else if (status != 0)
		{
			CardLan.NoDiscountTime[0] = Time.year;
			CardLan.NoDiscountTime[1] = Time.month;
			CardLan.NoDiscountTime[2] = Time.day;
			CardLan.NoDiscountTime[3] = Time.hour;
			CardLan.NoDiscountTime[4] = Time.min;
			CardLan.NoDiscountTime[5] = Time.sec;
			g_FgDiscntRegTime = 1;
			g_FgWriteNoDiscount = 1;
		}
	#else
                   if (status == 0xaa)
                	{
			//printf("========FirstTimeDisc \n");
			CardLan.FirstTimeDiscount[0] = Time.year;
			CardLan.FirstTimeDiscount[1] = Time.month;
			CardLan.FirstTimeDiscount[2] = Time.day;
			CardLan.FirstTimeDiscount[3] = Time.hour;
			CardLan.FirstTimeDiscount[4] = Time.min;
			CardLan.FirstTimeDiscount[5] = Time.sec;
			g_FgWriteNoDiscount = 2;
		}
	         else if (status != 0)
	       	{	
	       		//printf("========No discount\n");
			CardLan.NoDiscountTime[0] = Time.year;
			CardLan.NoDiscountTime[1] = Time.month;
			CardLan.NoDiscountTime[2] = Time.day;
			CardLan.NoDiscountTime[3] = Time.hour;
			CardLan.NoDiscountTime[4] = Time.min;
			CardLan.NoDiscountTime[5] = Time.sec;
			g_FgWriteNoDiscount = 1;
		 }
	  #endif
           #endif
                if(status == 2)i = 200;

#if PRINT_NAND
                printf("status == %d �۸� = %d  \n",status,HostValue.i);
#endif
                break;
            case 16:  //�������ʱ���
            case 17:
            case 18:
            case 19:
#if PRINT_NAND
                printf("\n�������ʱ� PaGe = %02d,TypeTime.Type = %d \n",JackCmd.Parameter[i],TypeTime.Type);
#endif
                	status = DiscountRate(TypeTime.Type);
                if(status == 2)i = 200;

#if PRINT_NAND
                printf("status == %d �۸� = %d \n",status,HostValue.i);
#endif
                break;
            case 20: //�������ֱ���
            case 21:
            case 22:
            case 23:
#if PRINT_NAND
                printf("\n�������ֱ� PaGe = %02d  \n",JackCmd.Parameter[i]);
#endif
                status = IntegralSheet(TypeTime.Type);
                if(status == 2) i = 200;

#if PRINT_NAND
                printf("status == %d �۸� = %d \n",status,HostValue.i);
#endif

                break;

            case 24: //���޴�������
            case 25:
            case 26:
            case 27:
                break;
            case 28: //���޼۸����
            case 29:
            case 30:
            case 31:
                break;
            default:
                break;
            }
        }
        else
        {
            b++;
        }
    }
    if((i == 6)&&(b!=6))status = 0;
    else if(i == 201)status = 2;
    else if(i == 206)status = 3;
    else status = 1;

    return status;
}

/*
*************************************************************************************************************
- �������� : unsigned char Analysischeck(unsigned char TypeVae,)
- ����˵�� : ���ҹ̶�����Ʊ��
- ������� : ��
- ������� : 
*************************************************************************************************************
*/
unsigned char AnalysisSheetcheck()
{
    unsigned char i;
    unsigned char status=1,b=0;

	DBG_PRINTF("AnalysisSheet() is called.\n");

    TypeTime.Type = 8;
    TypeTime.TimeNum = 1;
    HostValue.i = 0;
    memcpy(JackCmd.Parameter,CardLanBuf+2*512,16);
    for(i = 0; i < 1; i++)
    {
        if((JackCmd.Parameter[i] > 7)&&(JackCmd.Parameter[i] < 24))
        {
            memcpy(NandBuf,CardLanBuf+JackCmd.Parameter[i]*512,512);
            switch(JackCmd.Parameter[i])
            {
            case 8://�����۸����
            case 9:
            case 10:
            case 11:
				
#if 0
                printf("\n�����۸�� PaGe = %02d ��ʼ�۸�:%d \n",JackCmd.Parameter[i],HostValue.i);
#endif
               
                    status = ParseRMB(TypeTime,1);//����ĺϷ���
                    if(status == 2)i = 200;
                    else if(status == 1)i = 205;
                
#if 0
                printf("status == %d �۸� = %d  \n",status,HostValue.i);
#endif
                break;
          
            default:
                break;
            }
        }
        else
        {
            b++;
        }
    }
    if((i == 0)&&(b!=1))status = 0;
    else if(i == 201)status = 2;
    else if(i == 206)status = 3;
    else status = 1;

    return status;
}

#if Transport_Stander
unsigned char AnalysisSheet_Cpu_jiaotong(unsigned char TypeVae)	
{
	unsigned char i;
    unsigned char status=1,b=0;

	TypeTime.Type = GetCardType();	
	
	memcpy(JackCmd.Parameter,CardLanBuf+2*512,16);
	for(i = 0; i < 6; i++)
	{
        	if((JackCmd.Parameter[i] > 7)&&(JackCmd.Parameter[i] < 24))
        	{
            	memcpy(NandBuf,CardLanBuf+JackCmd.Parameter[i]*512,512);
            	switch(JackCmd.Parameter[i])
            	{
            		case 8://�����۸����
            		case 9:
            		case 10:
            		case 11:
                			if(TypeVae)
                			{
                    				status = ParseRMB(TypeTime,TypeVae);//����ĺϷ���
                    				if(status == 2)i = 200;
                    				else if(status == 1)i = 205;
                			}
                			else
                			{
                    				status = ParseRMB(TypeTime,0);//����ĺϷ���
                    				if(status == 2)i = 200;
                    				else if(status == 1)i = 205;
                			}
                			break;
                    case 16:  //�������ʱ���
                    case 17:
                    case 18:
                    case 19:
#if PRINT_NAND
                        printf("\n�������ʱ� PaGe = %02d,TypeTime.Type = %d \n",JackCmd.Parameter[i],TypeTime.Type);
#endif
                        	status = DiscountRate(TypeTime.Type);
                        if(status == 2)i = 200;

#if PRINT_NAND
                        printf("status == %d �۸� = %d \n",status,HostValue.i);
#endif                            
            	}
        	}
        	else
        	{
            	b++;
        	}
	}
	
	if((i == 6)&&(b!=6))
		status = 0;
    else if(i == 201)
		status = 2;
    else if(i == 206)
		status = 3;
    else 
		status = 1;

    return status;

}

#endif

/*
*************************************************************************************************************
- �������� : INT8U OverTimeEnter (void)
- ����˵�� : ����Ƿ����û��
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char OverTimeEnter(void)
{
    unsigned char buff[20];
    unsigned char buff1[20];
    unsigned char buff2[20];
    unsigned char statusFlag = 1;
    unsigned char status;


    memset(Test.longbuf,0,4);
    memset(Buf.longbuf,0,4);
    memset(buff,0,sizeof(buff));
    memset(buff1,0,sizeof(buff1));
    memcpy(Buf.longbuf, CardLan.OldTermNo, 4);//yao
    myitoa(buff,Buf.i);
    myitoa(buff1,DevNum.i);

    //printf();
    if((strlen(buff) == 8)&&(strlen(buff1) == 8))
    {
        memset(buff2,0,sizeof(buff2));
        memcpy(buff2,buff+1,7);
        Buf.i = atoi(buff2);

        memset(buff2,0,sizeof(buff2));
        memcpy(buff2,buff1+1,7);
        Test.i = atoi(buff2);
    }
    else
    {
        memcpy(Test.longbuf,DevNum.longbuf,4);//yao
    }

	DebugPrintf("Buf.i = %u Test.i = %u\n", Buf.i, Test.i);
    if(Buf.i  !=  Test.i)  //�жϻ��ŵ�3λ����һλ�����ֶ����ӻ�
    {
        statusFlag = 0;
    }
    else
    {
        status = Date_MinLooK((unsigned char *)&CardLan.OldTime,(unsigned char *)&Time);
	
		DebugPrintf("status = 0x%02X\n",status);

		if(status == MI_OK)
        {
            memcpy(Infor.intbuf,Section.DeductTime,2);
			DebugPrintf("interval : %u\n", ((retdata.date*1440) + retdata.min));
			DebugPrintf("Infor.i : %u\n", Infor.i);
            if(((retdata.date*1440) + retdata.min) > Infor.i)		//����ʱ����
            {
                statusFlag = 0;
            }
        }
    }
	DebugPrintf("statusFlag = 0x%02X\n", statusFlag);
    return(statusFlag);
}


/*
*************************************************************************************************************
- �������� : unsigned char DiscountRate(void)
- ����˵�� : ���ʱ����
- ������� : num :������Ϣ���
- ������� : ��
*************************************************************************************************************
*/
unsigned int SectionDiscountRate(unsigned int val,unsigned char Type)
{
    unsigned int bval = val;
    unsigned char i;
    unsigned char CsnBuf[11];

    memcpy(JackCmd.Parameter,CardLanBuf+2*512,16);
    for(i = 0; i < 16; i++)
    {
        if((JackCmd.Parameter[i] > 15)&&(JackCmd.Parameter[i] < 20))break;
    }

    if(i!=16)
    {
        memcpy(NandBuf,CardLanBuf+JackCmd.Parameter[i]*512,512);
        for(i = 0; i < 50 ; i++)
        {
            memcpy(CsnBuf,NandBuf+(i * 10),10);
            if((CsnBuf[0] == 0x55)&&(CsnBuf[1] == 0xAA)) break;
            memcpy(Test.longbuf,CsnBuf+1,4);
            memcpy(Buf.longbuf,CsnBuf+5,4);
            if((Type == CsnBuf[0])&&(Test.i <= bval)&&(Buf.i > bval))
            {
                bval = (bval*CsnBuf[9])/100;
                i = 200;
                break;
            }
        }
    }
    return bval;
}

#define qixia_bus 0
unsigned char FindUpdoorType(void)
{
    unsigned char i,status = 1;
    LongUnon Buf;
	FILE *upfile;
	char whichdoor;

    Section.Updoor = 0x00;
    memcpy(NandBuf,CardLanBuf+3*512,512);
    for(i = 0; i < 100; i++)
    {
        if((NandBuf[i*5] == 0x55)&&(NandBuf[i*5+1] == 0xAA)) break;
        memcpy(Buf.longbuf,NandBuf+i*5,4);
        if(Buf.i == DevNum.i)
        {
            Section.Updoor = NandBuf[i*5+4];
            status = 0;
            break;
        }

    }
	printf("****************************Section.Updoor = %02X ******************\n",Section.Updoor);


    return status;
}

unsigned char FindCardValue(unsigned int value)
{
    unsigned char i,status = 1;
    LongUnon Buf;


    memcpy(NandBuf,CardLanBuf+4*512,512);
    for(i = 0; i < 100; i++)
    {
        if((NandBuf[i*4] == 0x55)&&(NandBuf[i*4+1] == 0xAA))
        {
            if(value < 1000)
            {
                status = 0;
            }
            break;
        }

        if(CardLan.CardType == NandBuf[i*4])
        {
            Buf.i = 0;
            memcpy(Buf.longbuf,NandBuf+i*4+1,3);
            if(value < Buf.i)
            {
                status = 0;
            }
            break;
        }
    }

    if((i==100)&&(value < 1000))
    {
        status = 0;
    }

#if  PRINT_NAND
    printf("FindCardValue  status = %d   value= %d \n",status,value);
#endif

    return status;
}




#if 0
unsigned char SectionSheet(void)
{
    unsigned char status =1;
    unsigned short Sin,Sout,Sall;
    unsigned short addr;
// memcpy(Infor.intbuf,Section.SationNum,2)
    Sall = SectionNum;
    if(((CardLan.StationID <= Sall)&&(Section.SationNow<= Sall))||((Section.SationNow == 1)&&(CardLan.StationID!=0)))
    {

        if(Section.Enableup != 0x55)
        {

            if(CardLan.StationOn == Section.Updown)
            {
                if(CardLan.StationID <= Section.SationNow)
                {
                    Sin = CardLan.StationID -1;
                    Sout = Section.SationNow -1;
                }
                else
                {
                    Sin = Section.SationNow - 1;
                    Sout = CardLan.StationID - 1;
                }
            }
            else
            {

                // if(CardLan.StationOn == 0)
                // {

                if(CardLan.StationID <= (Sall - Section.SationNow))
                {

                    Sin = CardLan.StationID -1;
                    Sout = (Sall - Section.SationNow);
                }
                else
                {
                    Sin = (Sall - Section.SationNow);
                    Sout = CardLan.StationID - 1;
                }
                // }
                //else
                // {


                //  if(CardLan.StationID <= (Sall - Section.SationNow))
                //    {
                //      Sin = (Sall - Section.SationNow);
                //      Sout = CardLan.StationID -1;

                //    }
                //    else
                //   {
                //        Sin = CardLan.StationID - 1;
                //        Sout = (Sall - Section.SationNow);

                //    }
                // }
            }

            addr = Sout*Sall*4 + Sin*4;
            if(addr <= 16380)
            {
                memcpy(HostValue.longbuf,SectionParBuf+addr,4);
                status = 0;
            }

        }
        else
        {
            if((CardLan.StationID <= Section.SationNow)&&(CardLan.StationOn == Section.Updown))
            {
                Sin = CardLan.StationID -1;
                Sout = Section.SationNow -1;
                addr = Sout*Sall*4 + Sin*4;
                if(addr <= 16380)
                {
                    if(Section.Updown)
                    {
                        memcpy(HostValue.longbuf,SectionParUpBuf+ addr,4); //����
                    }
                    else
                    {
                        memcpy(HostValue.longbuf,SectionParBuf+ addr,4); //����
                    }
#if  PRINT_NAND
                    printf("SectionSheet 01 Card Sta %d   Sec Stanow %d   Card updown %d ",CardLan.StationID,Section.SationNow,CardLan.StationOn);
#endif


                    status =0;
                }


            }
            else
            {

                if((Section.SationNow == 1)&&(CardLan.StationOn != Section.Updown))
                {
                    if(CardLan.StationOn == 0)
                    {

                        if(CardLan.StationID <= Section.SationNum[0])
                        {
                            Sin = CardLan.StationID -1;
                            Sout = Section.SationNum[0]-1;
                            addr = Sout*Section.SationNum[0]*4 + Sin*4;
                            if(addr <= 16380)
                            {
                                memcpy(HostValue.longbuf,SectionParBuf+ addr,4); //����
                                status =0;
#if   PRINT_NAND
                                printf("SectionSheet 02 Card Sta %d   Sec Stanow %d   Card updown %d ",CardLan.StationID,Section.SationNow,CardLan.StationOn);
#endif


                            }

                        }
                        else return -1;
                    }
                    else
                    {
                        if(CardLan.StationID <= Sectionup.SationNum[0])
                        {
                            Sin = CardLan.StationID -1;
                            Sout = Sectionup.SationNum[0]-1;
                            addr = Sout*Sectionup.SationNum[0]*4 + Sin*4;
                            if(addr <= 16380)
                            {
                                memcpy(HostValue.longbuf,SectionParUpBuf+ addr,4); //����
                                status =0;

#if   PRINT_NAND
                                printf("SectionSheet 03 Card Sta %d   Sec Stanow %d   Card updown %d ",CardLan.StationID,Section.SationNow,CardLan.StationOn);
#endif

                            }

                        }
                        else return -1;

                    }
                }
                else return -1;

                // Sin = Section.SationNow - 1;
                // Sout = CardLan.StationID - 1;
            }
        }

        if(status == 0)
        {
            status = 1;
            DecValue.i = HostValue.i;
            if((HostValue.i != 0)&&(HostValue.i < 100000))
            {
                status = 0;
            }
        }
    }

#if   PRINT_NAND
    printf("SectionSheet %d \n",status);
#endif
    return status;
}

#endif
unsigned char SectionSheet(void)
{
    unsigned char status =1;
    unsigned short Sin,Sout,Sall;
    unsigned short addr;
	// added by taeguk
	// struct timeval tim;
	// memcpy(Infor.intbuf,Section.SationNum,2)
    Sall = SectionNum;
	// added by taeguk 


	
#ifdef DALI_BUS
	Section.SationNow = Section.Sationdis+1;
#elif RUSHAN_BUS
		;
#else
	Section.SationNow = Section.Sationdis + 1;
#endif


 	
	
    if(((CardLan.StationID <= Sall)&&(Section.SationNow<= Sall))||((Section.SationNow == 1)&&(CardLan.StationID!=0)))
    {
        if(Section.Enableup != 0x55)
        {
            if(CardLan.StationOn == Section.Updown)
            {
                if(CardLan.StationID <= Section.SationNow)
                {
                    Sin = CardLan.StationID -1;
                    Sout = Section.SationNow -1;
                }
                else
                {
                    //Sin = Section.SationNow - 1;
                    //Sout = CardLan.StationID - 1;
                    //modified by taeguk                    
					goto   _STATION_ERROR;                  
                    Sin = CardLan.StationID - 1;
                    Sout = CardLan.StationID;
					Section.SationNow = CardLan.StationID - 1;
                }
            }
            else
            {
				goto  _STATION_ERROR;
				
                if(CardLan.StationID <= (Sall - Section.SationNow))
                {

                    Sin = CardLan.StationID -1;
                    Sout = (Sall - Section.SationNow);
                }
                else
                {
                    Sin = (Sall - Section.SationNow);
                    Sout = CardLan.StationID - 1;                  
                }
            }
            addr = Sout*Sall*4 + Sin*4;
            if(addr <= 16380)
            {
                memcpy(HostValue.longbuf,SectionParBuf+addr,4);
                status = 0;
            }
        }
        else
        {
            if((CardLan.StationID <= Section.SationNow)&&(CardLan.StationOn == Section.Updown))
            {
                Sin = CardLan.StationID -1;
                Sout = Section.SationNow -1;
                addr = Sout*Sall*4 + Sin*4;
                if(addr <= 16380)
                {
                    if(Section.Updown)
                    {
                        memcpy(HostValue.longbuf,SectionParUpBuf+ addr,4); //����
                    }
                    else
                    {
                        memcpy(HostValue.longbuf,SectionParBuf+ addr,4); //����
                    }
#if  PRINT_NAND
                    printf("SectionSheet 01 Card Sta %d   Sec Stanow %d   Card updown %d ",CardLan.StationID,Section.SationNow,CardLan.StationOn);
#endif
                    status =0;
                }
            }			
            else
            {

                if((Section.SationNow == 1)&&(CardLan.StationOn != Section.Updown))
                {
                    if(CardLan.StationOn == 0)
                    {
                        if(CardLan.StationID <= Section.SationNum[0])
                        {
                            Sin = CardLan.StationID -1;
                            Sout = Section.SationNum[0]-1;
                            addr = Sout*Section.SationNum[0]*4 + Sin*4;
                            if(addr <= 16380)
                            {
                                memcpy(HostValue.longbuf,SectionParBuf+ addr,4); //����
                                status =0;
#if   PRINT_NAND
                                printf("SectionSheet 02 Card Sta %d   Sec Stanow %d   Card updown %d ",CardLan.StationID,Section.SationNow,CardLan.StationOn);
#endif
                            }
                        }
                        else return -1;
                    }
                    else
                    {
                        if(CardLan.StationID <= Sectionup.SationNum[0])
                        {
                            Sin = CardLan.StationID -1;
                            Sout = Sectionup.SationNum[0]-1;
                            addr = Sout*Sectionup.SationNum[0]*4 + Sin*4;
                            if(addr <= 16380)
                            {
                                memcpy(HostValue.longbuf,SectionParUpBuf+ addr,4); //����
                                status =0;

#if   PRINT_NAND
                                printf("SectionSheet 03 Card Sta %d   Sec Stanow %d   Card updown %d ",CardLan.StationID,Section.SationNow,CardLan.StationOn);
#endif

                            }

                        }
                        else return -1;

                    }
                }
/*                else return -1;   modified by taeguk , 
�ڳ�վIDС�ڽ�վID��ʱ���Կ��ϵ�IDΪ׼����һ��վ�ķ���*/
				else
				{
//					if(CardLan.StationOn != Section.Updown)
//					{
//						if(!gettimeofday(&tim,0))
//						{
//							if((tim.tv_sec-GapOfRevStationId) > 8)
//								Section.Updown = CardLan.StationOn;
//							else
//								CardLan.StationOn = Section.Updown;
//						}
//					}
/*
	CardLan.StationOn : ����¼������
	Section.Updown     : ����������
	CardLan.StationID :  �ϳ�վ��
	Section.SationNow : �³�վ��			
*/
_STATION_ERROR:
					//SAVE_CARLAN_LOG(RS485_ERROR);
					DebugPrintf("----Section.SationNow:%d,CardLan.StationID:%d,SectionNum:%d\n",Section.SationNow,CardLan.StationID,SectionNum);
					DebugPrintf("Section.Updown = 0x%02X\n", Section.Updown);
					DebugPrintf("CardLan.StationOn = 0x%02X\n", CardLan.StationOn);
					
					//if(CardLan.StationOn != Section.Updown)
						//Section.Updown = CardLan.StationOn;
					
					if((Section.Enableup == 0x55)&&(Section.Updown==1))
						SectionNum = Sectionup.SationNum[0];
					else
						SectionNum = Section.SationNum[0];
					if(CardLan.StationID >= SectionNum){
						//CardLan.StationID = SectionNum - 1;
						//Section.SationNow = CardLan.StationID - 1;
						Sin = CardLan.StationID - 1-1;
						Sout = CardLan.StationID-1;
					}
					else{						
						//Section.SationNow = CardLan.StationID + 1;
						Sin = CardLan.StationID-1;
						Sout = CardLan.StationID + 1-1;						
					}
					//if(CardLan.StationID > Section.SationNow)
					{
						//Sin = 1;
						//Sout = 2;
//						printf("----Section.SationNow:%d,CardLan.StationID:%d,SectionNum:%d\n",Section.SationNow,CardLan.StationID,SectionNum);
                        addr = Sout*SectionNum*4 + Sin*4;

//						printf("Sectionup.SationNum[0]:%d,Sout:%d,Sin:%d\n",Sectionup.SationNum[0],Sout,Sin);
                        if(addr <= 16380)
                        {
							if(Section.Updown && (Section.Enableup == 0x55))
							{
//								printf("SectionParUpBuf:0x%x,addr:%d\n",SectionParUpBuf,addr);
								memcpy(HostValue.longbuf,SectionParUpBuf+ addr,4); //����
							}
							else
							{
//								printf("SectionParUpBuf:0x%x,addr:%d\n",SectionParBuf,addr);
								memcpy(HostValue.longbuf,SectionParBuf+ addr,4); //����
							}
                            status =0;
                        }	
						printf("--test:\n");
					}
				}
                // Sin = Section.SationNow - 1;
                // Sout = CardLan.StationID - 1;
            }
        }

        if(status == 0)
        {
            status = 1;
            DecValue.i = HostValue.i;
			printf("-----HostValue.i : %d ,addr:%d\n",HostValue.i,addr);
            if((HostValue.i != 0)&&(HostValue.i < 100000))
            {
                status = 0;
            }
        }
    }

#if   PRINT_NAND
    printf("SectionSheet %d \n",status);
#endif
    return status;
}	


#if Transport_Stander
unsigned char SectionSheet_CPU(void)
{
	
	unsigned char status = 1;
	unsigned short Sin,Sout,Sall;
	unsigned short addr;
	unsigned char directionFlag_Card;//��Ƭ��¼��������

	
	if((CardLanCPU.directionflag == 0xAB)||(CardLanCPU.ydirectionflag == 0xAB))             //����
			{
                directionFlag_Card = 0x00;  
                Sall = CardLanCPU.gupstationnum;
       }
		else
			{
                directionFlag_Card = 0x01;
                Sall = CardLanCPU.gdownstationnum;
          }
	
			
		Section.SationNow = Section.Sationdis + 1;
	
		printf("Section.Enableup=%02x,directionFlag_Card=%02x,Section.Updown=%02x\n",Section.Enableup,directionFlag_Card,Section.Updown);
		printf("�����ֶμ۸�:\n");
		printf("1.CardLanCPU.getonstation=%02x,Sall=%02x,Section.SationNow=%02x\n",CardLanCPU.getonstation,Sall,Section.SationNow);

		if(((CardLanCPU.getonstation <= Sall)&&(Section.SationNow<= Sall))||((Section.SationNow == 1)&&(CardLanCPU.getonstation!=0)))
    	{
			if(Section.Enableup != 0x55)	//��ʹ������
        		{
            		if(directionFlag_Card == Section.Updown)	//����Ƭ�ͳ�����ͬһ��
            		{
                			if(CardLanCPU.getonstation <= Section.SationNow)
                			{
                    				Sin = CardLanCPU.getonstation -1;	
                    				Sout = Section.SationNow -1;
                			}
                			else
                			{
								goto   _STATION_ERROR_jiaotong;                  
                			}
            		}
            		else
            		{
						goto   _STATION_ERROR_jiaotong;
            		}
            		addr = Sout*Sall*4 + Sin*4;	//������λ��
            		if(addr <= 16380)
            		{
                			memcpy(HostValue.longbuf,SectionParBuf+addr,4);
                			status = 0;
            		}
        		}
			 else	//	ʹ������
        		{
            		if((CardLanCPU.getonstation <= Section.SationNow)&&(directionFlag_Card == Section.Updown))
            		{
                			Sin = CardLanCPU.getonstation -1;
                			Sout = Section.SationNow -1;
                			addr = Sout*Sall*4 + Sin*4;
                			if(addr <= 16380)
                			{
                    				if(Section.Updown)		// 1Ϊ����
                    				{
                        				memcpy(HostValue.longbuf,SectionParUpBuf+ addr,4); //����
                    				}
                    				else
                    				{
                        				memcpy(HostValue.longbuf,SectionParBuf+ addr,4); //����
                    				}
                    				status =0;
                			}
            		}
					else
						{
							if((Section.SationNow == 1)&&(directionFlag_Card != Section.Updown))
                			{
                    				if(directionFlag_Card == 0)
                    				{
                        				if(CardLanCPU.getonstation <= Section.SationNum[0])
                        				{
                            					Sin = CardLanCPU.getonstation -1;
                            					Sout = Section.SationNum[0]-1;
                            					addr = Sout*Section.SationNum[0]*4 + Sin*4;
                            					if(addr <= 16380)
                            					{
                                					memcpy(HostValue.longbuf,SectionParBuf+ addr,4); //����
                                					status =0;
                            					}
                        				}
                        				else return -1;
                    				}
                    				else
                    				{
                        				if(CardLanCPU.getonstation <= Sectionup.SationNum[0])
                        				{
                            					Sin = CardLanCPU.getonstation -1;
                            					Sout = Sectionup.SationNum[0]-1;
                            					addr = Sout*Sectionup.SationNum[0]*4 + Sin*4;
                            					if(addr <= 16380)
                            					{
                                					memcpy(HostValue.longbuf,SectionParUpBuf+ addr,4); //����
                                					status =0;
                            					}
                        				}
                        				else return -1;
                    				}
                			}
							else
							{
								/*
	CardLan.StationOn : ����¼������
	Section.Updown    : ����������
	CardLan.StationID :  �ϳ�վ��
	Section.SationNow : �³�վ��			
*/
_STATION_ERROR_jiaotong:
								if((Section.Enableup == 0x55)&&(Section.Updown==1))
									SectionNum = Sectionup.SationNum[0];
								else
									SectionNum = Section.SationNum[0];
								if(CardLanCPU.getonstation >= SectionNum)
								{
									Sin = CardLanCPU.getonstation - 1-1;
									Sout = CardLanCPU.getonstation-1;
								}
								else
								{						
									Sin = CardLanCPU.getonstation -1;
									Sout = CardLanCPU.getonstation + 1-1;						
								}
		            			addr = Sout*SectionNum*4 + Sin*4;
		            			if(addr <= 16380)
		            			{
									if(Section.Updown && (Section.Enableup == 0x55))
									{
										memcpy(HostValue.longbuf,SectionParUpBuf+ addr,4); //����
									}
									else
									{
										memcpy(HostValue.longbuf,SectionParBuf+ addr,4); //����
									}
		                				status =0;
	        					}		
							}
						
						}
			 	}

			if(status == 0)
        		{
            		status = 1;
            		DecValue.i = HostValue.i;
					printf("-----HostValue.i : %d ,addr:%d\n",HostValue.i,addr);
            		if((HostValue.i != 0)&&(HostValue.i < 100000))
            		{
                			status = 0;
            		}
        		}
		}
		
		return status;
		
}
#endif


#if defined(CONFIG_BZLINUXBUS)   //LINUX ��׼����

/*
*************************************************************************************************************
- �������� : unsigned char Limited_Number_Times(unsigned char Type,unsigned char *DatdCi)
- ����˵�� : ��ʾ������Ϣ�ӳ���
- ������� : num :������Ϣ���
- ������� : ��
*************************************************************************************************************
*/
unsigned char Limited_Number_Times(unsigned char Type)
{
    unsigned char i,status = 1;
    unsigned char CsnBuf[6];
    ShortUnon TCtime;

    memcpy(JackCmd.Parameter,CardLanBuf+2*512,16);
	
    for(i = 0; i < 16; i++)
    {
        if((JackCmd.Parameter[i] > 23)&&(JackCmd.Parameter[i] < 28))break;
    }
    if(i != 16)
    {
        //NFReadPage(JackCmd.Parameter[i],NandBuf);
        memcpy(NandBuf,CardLanBuf+JackCmd.Parameter[i]*512,512);
        for( i = 0; i < 102; i++)
        {
            memcpy(CsnBuf,NandBuf+(i * 5),5);
            if(CsnBuf[0] == Type)
            {
                CIshu.ci = CsnBuf[1];
                CIshu.re = CsnBuf[2];
                memcpy(TCtime.intbuf,CsnBuf+3,2);
                //TCtime.intbuf[0] = CsnBuf[4];
                //TCtime.intbuf[1] = CsnBuf[3];
                CIshu.yue = TCtime.i;
                status = 0;
            }
        }
    }
	//printf("Limited_Number_Times i %d\n", i);
    return  status;
}
/*
*************************************************************************************************************
- �������� : unsigned char YesOrNoCiShu(void)
- ����˵�� : ��������
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
//CIshu
unsigned char YesOrNoCiShu(void)
{
    unsigned char status,chet;
    ShortUnon  ShCishu;

    status = Limited_Number_Times(CardLan.CardType);
	DebugPrintf("status = 0x%02X\n", status);
	//printf("limitnumbertimes status = 0x%02X\n", status);
	//for(chet=0; chet<16;chet++)
	//	printf("0x%02x ", Sector.STwoZero[chet]);
	//printf("\n");
    if(status == MI_OK)							//��Ϊ0  ��ʾ������֧�ִ�������
    {
        CardLan.CiMoneyFlag = 0x01;
        if((Time.year == CardLan.OldTime[0])&&(Time.month == CardLan.OldTime[1]))
        {
            memcpy(ShCishu.intbuf,Sector.STwoZero+2,2);
            ShCishu.i +=1;

			//printf("ShCishu.i = %02X, CIshu.yue = %02X, CIshu.re = %02X\n", ShCishu.i, CIshu.yue, CIshu.re);

			if(ShCishu.i <= CIshu.yue)
            {
                if(Time.day == CardLan.OldTime[2])
                {
                    chet = 	Sector.STwoZero[1];				//��
                    chet +=1;
					//printf("#####chet = %02X\n", chet);
                    if(chet <= CIshu.re)
                    {
                        if(TypeTime.TimeNum  == CardLan.Period)
                        {
                            chet = 	Sector.STwoZero[0];		//��
                            chet +=1;
							//printf("#####chet = %02X CIshu.ci = %02X\n",chet, CIshu.ci);
                            if(chet <= CIshu.ci)
                            {
                                Sector.STwoZero[0]++;
                                Sector.STwoZero[1]++;
                                memcpy(ShCishu.intbuf,Sector.STwoZero+2,2);
                                ShCishu.i++;
                                memcpy(Sector.STwoZero+2,ShCishu.intbuf,2);
                                Sector.FlagValue = 0;
                                Sector.STwoZero[14] = TypeTime.TimeNum;
                                status = 0;

                            }
                            else
                            {
                                status = 1;
                            }
                        }
                        else
                        {

                            memset(Sector.STwoZero+4,0,3); //��ʱ���޶�
                            Sector.STwoZero[0] = 1;
                            Sector.STwoZero[1]++;
                            memcpy(ShCishu.intbuf,Sector.STwoZero+2,2);
                            ShCishu.i++;
                            memcpy(Sector.STwoZero+2,ShCishu.intbuf,2);
                            Sector.FlagValue = 0;
                            Sector.STwoZero[14] = TypeTime.TimeNum;
                            status = 0;
                        }
                    }
                    else
                    {
                        status = 1;
                    }
                }
                else
                {

                    memset(Sector.STwoZero+4,0,6); //�����޶�

                    memcpy(ShCishu.intbuf,Sector.STwoZero+2,2);
                    ShCishu.i++;
                    memcpy(Sector.STwoZero+2,ShCishu.intbuf,2);
                    Sector.STwoZero[14] = TypeTime.TimeNum;
                    Sector.FlagValue = 0;
                    Sector.STwoZero[0] = 1;
                    Sector.STwoZero[1] = 1;
                    status = 0;
                }
            }
            else
            {
                status = 1;
            }
        }
        else
        {
            memset(Sector.STwoZero+4,0,10); //�����޶�

            Sector.FlagValue = 0;
            Sector.STwoZero[0] = 1;
            Sector.STwoZero[1] = 1;
            ShCishu.i = 1;
            memcpy(Sector.STwoZero+2,ShCishu.intbuf,2);
            Sector.STwoZero[14] = TypeTime.TimeNum;
            status = 0;
        }
    }
    else
    {
    		#if(defined QINGDAO_TONGRUAN)//#ifdef QINGDAO_TONGRUAN //�����ƵĿ�ҲҪ�ƴ�
		CardLan.CiMoneyFlag = 0x01;  //special function
		if((Time.year == CardLan.OldTime[0])&&(Time.month == CardLan.OldTime[1]) && (Time.day == CardLan.OldTime[2]) && (TypeTime.TimeNum  == CardLan.Period))
		{
			 Sector.STwoZero[0]++;
			 Sector.STwoZero[1]++;
		}
		else
		{
			Sector.STwoZero[0] = 1;
			Sector.STwoZero[1] = 1;
		}	
		#endif

        status = 2;//û�����ƴ˿���
     // printf("2====ye===  status = 0x%02X\n", status);
    }
    return status;
}
/*
*************************************************************************************************************
- �������� : unsigned char ControlConsumption(unsigned char Type)
- ����˵�� : ��ʾ������Ϣ�ӳ���
- ������� : num :������Ϣ���
- ������� : ��
*************************************************************************************************************
*/
unsigned char ControlConsumption(unsigned char Type)
{
    unsigned char i,status = 1;
    unsigned char CsnBuf[12];

    memcpy(JackCmd.Parameter,CardLanBuf+2*512,16);
    for(i = 0; i < 16; i++)
    {
        if((JackCmd.Parameter[i] > 27)&&(JackCmd.Parameter[i] < 32))break;
    }
    if(i != 16)
    {
        memcpy(NandBuf,CardLanBuf+JackCmd.Parameter[i]*512,512);
        for( i = 0; i < 46; i++)
        {
            memcpy(CsnBuf,NandBuf+(i * 11),11);
            if(CsnBuf[0] == Type)
            {
                Buf.i  = 0;
                // Buf.longbuf[0] = CsnBuf[3];
                // Buf.longbuf[1] = CsnBuf[2];
                // Buf.longbuf[2] = CsnBuf[1];
                memcpy(Buf.longbuf,CsnBuf +1,3);


                Test.i  = 0;
                // Test.longbuf[0] = CsnBuf[6];
                // Test.longbuf[1] = CsnBuf[5];
                // Test.longbuf[2] = CsnBuf[4];
                memcpy(Test.longbuf,CsnBuf +4,3);
                JackArm.i = 0;
                //JackArm.longbuf[0] = CsnBuf[10];
                //JackArm.longbuf[1] = CsnBuf[9];
                // JackArm.longbuf[2] = CsnBuf[8];
                // JackArm.longbuf[3] = CsnBuf[7];
                memcpy(JackArm.longbuf,CsnBuf+7,4);

                MoneyNo.Ci  = Buf.i;
                MoneyNo.Day = Test.i;
                MoneyNo.Mon = JackArm.i;
                status = 0;
            }
        }
    }
    return  status;
}
/*
*************************************************************************************************************
- �������� : unsigned char  YesOrNoMoney(void)
- ����˵�� : ����
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char  YesOrNoMoney(void)
{
    unsigned char status,errstatus = 4;
    LongUnon Interim;
    status = ControlConsumption(CardLan.CardType);
    if(status == MI_OK)
    {
        CardLan.CiMoneyFlag = 0x01;
        if((Time.year == CardLan.OldTime[0])&&(Time.month == CardLan.OldTime[1]))
        {
            memcpy(Interim.longbuf,CardLan.MonthValue,4);//�������ѵĽ��
        }
        else
        {
            Interim.i = 0;
            memset(CardLan.DayValue,0,sizeof(CardLan.DayValue));
            memset(CardLan.ViewsValue,0,sizeof(CardLan.ViewsValue));
            memset(Sector.STwoZero,0,4); //�����޴�
        }
        Interim.i += HostValue.i;
        if(Interim.i <= MoneyNo.Mon)
        {
            memcpy(Sector.STwoZero+10,Interim.longbuf,4);
            if(Time.day == CardLan.OldTime[2])
            {
                Interim.i = 0;
                memcpy(Interim.longbuf,CardLan.DayValue,3);//�������ѵĽ��
            }
            else
            {
                Interim.i = 0;
                memset(CardLan.ViewsValue,0,sizeof(CardLan.ViewsValue));
                memset(Sector.STwoZero,0,2); //�����޴�
            }
            Interim.i += HostValue.i;
            if(Interim.i <= MoneyNo.Day)
            {
                memcpy(Sector.STwoZero+7,Interim.longbuf,3);
                if(TypeTime.TimeNum  == CardLan.Period)
                {
                    Interim.i = 0;
                    memcpy(Interim.longbuf,CardLan.ViewsValue,3);//�������ѵĽ��
                }
                else
                {
                    Interim.i = 0;
                    memset(Sector.STwoZero,0,1); //��ʱ���޴�
                }
                Interim.i += HostValue.i;
                if(Interim.i <= MoneyNo.Ci)
                {
                    errstatus = 0;
                    memcpy(Sector.STwoZero+4,Interim.longbuf,3);
                    Sector.STwoZero[14] = TypeTime.TimeNum;
                }
                else errstatus = 1;
            }
            else errstatus = 2;
        }
        else errstatus = 3;
    }
    else
    {
        memset(Sector.STwoZero+4,0,10);
        Sector.STwoZero[14] = TypeTime.TimeNum;
        errstatus = 0;
    }
    return errstatus;
}
unsigned char  YesOrNoMoney_cpu(void)
{
    unsigned char status,errstatus = 4;
    LongUnon Interim;
    status = ControlConsumption(CardLan.CardType);
    if(status == MI_OK)
    {
        CardLan.CiMoneyFlag = 0x01;
        if((Time.year == CardLan.OldTime[0])&&(Time.month == CardLan.OldTime[1]))
        {
            memcpy(Interim.longbuf,CardLan.MonthValue,4);//�������ѵĽ��
        }
        else
        {
            Interim.i = 0;
            memset(CardLan.DayValue,0,sizeof(CardLan.DayValue));
            memset(CardLan.ViewsValue,0,sizeof(CardLan.ViewsValue));
			memset(CardLan.MonthValue,0,sizeof(CardLan.MonthValue));
            //���¶�
        }
		
        Interim.i += HostValue.i;
        if(Interim.i <= MoneyNo.Mon)
        {
            memcpy(CardLan.MonthValue,Interim.longbuf,4);
            if(Time.day == CardLan.OldTime[2])
            {
                Interim.i = 0;
                memcpy(Interim.longbuf,CardLan.DayValue,3);//�������ѵĽ��
            }
            else
            {
                Interim.i = 0;
                memset(CardLan.ViewsValue,0,sizeof(CardLan.ViewsValue));
				memset(CardLan.DayValue,0,sizeof(CardLan.DayValue));
            }
			
            Interim.i += HostValue.i;
            if(Interim.i <= MoneyNo.Day)
            {
                memcpy(CardLan.DayValue,Interim.longbuf,3);
                if(TypeTime.TimeNum  == CardLan.Period)
                {
                    Interim.i = 0;
                    memcpy(Interim.longbuf,CardLan.ViewsValue,3);//�������ѵĽ��
                }
                else
                {
                    Interim.i = 0;
					memset(CardLan.ViewsValue,0,sizeof(CardLan.ViewsValue));
					//��ʱ���޶�
                }
				
                Interim.i += HostValue.i;
                if(Interim.i <= MoneyNo.Ci)
                {
                    errstatus = 0;
                    memcpy(CardLan.ViewsValue,Interim.longbuf,3);
                    CardLan.Period = TypeTime.TimeNum;
                }
                else errstatus = 1;
            }
            else errstatus = 2;
        }
        else errstatus = 3;
    }
    else
    {
        //memset(Sector.STwoZero+4,0,10);
        //Sector.STwoZero[14] = TypeTime.TimeNum;
#if 0
        memcpy(Interim.longbuf,CardLan.MonthValue,4)
        Interim.i += HostValue.i;
        memcpy(CardLan.MonthValue,Interim.longbuf,4);
#endif
        errstatus = 0;
    }
    return errstatus;
}

#endif



#if   defined(CONFIG_LINUXBUS8BIT)||defined(Stander_Compatible_8Bit) 
/*
*************************************************************************************************************
- �������� : unsigned char Limited_Number_Times(unsigned char Type,unsigned char *DatdCi)
- ����˵�� : ��ʾ������Ϣ�ӳ���
- ������� : num :������Ϣ���
- ������� : ��
*************************************************************************************************************
*/
unsigned char Limited_Number_Times_8bit(unsigned char Type)
{
    unsigned char i,status = 1;
    unsigned char CsnBuf[6];
    ShortUnon TCtime;

    memcpy(JackCmd.Parameter,CardLanBuf+2*512,16);
    for(i = 0; i < 16; i++)
    {
        if((JackCmd.Parameter[i] > 23)&&(JackCmd.Parameter[i] < 28))break;
    }
    if(i != 16)
    {
        memcpy(NandBuf,CardLanBuf+JackCmd.Parameter[i]*512,512);
        for( i = 0; i < 100; i++)
        {
            memcpy(CsnBuf,NandBuf+(i * 5),5);
            if(CsnBuf[0] == Type)
            {
                CIshu.ci = CsnBuf[1];
                CIshu.re = CsnBuf[2];
                memcpy(TCtime.intbuf,CsnBuf+3,2);
                CIshu.yue = TCtime.i;
                status = 0;
            }
        }
    }
    return  status;
}


/*
*************************************************************************************************************
- �������� : unsigned char YesOrNoCiShu(void)
- ����˵�� : ��������
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char YesOrNoCiShu_8bit(void)
{
    unsigned char status = 1 ;

    status = Limited_Number_Times_8bit(CardLan.CardType);
    if(status == MI_OK)
    {
        if((Time.year == CardLan.OldTime[0])&&(Time.month == CardLan.OldTime[1])&&(Time.day== CardLan.OldTime[2]))
        {
            if(Sector.SThrOne[7] > 0)
            {
                Sector.SThrOne[7]--;
                status = 0;
            }
            else
            {
                status = 1;
            }
        }
        else
        {
            Sector.SThrOne[7] = CIshu.re - 1;
            status = 0;
        }
    }
    else
    {
        Sector.SThrOne[7] = 0;
        status = 0;
    }
    return status;
}


/*
*************************************************************************************************************
- �������� : unsigned char ControlConsumption(unsigned char Type)
- ����˵�� : ��ʾ������Ϣ�ӳ���
- ������� : num :������Ϣ���
- ������� : ��
*************************************************************************************************************
*/
unsigned char ControlConsumption_8bit(unsigned char Type)
{
    unsigned char i,status = 1;
    unsigned char CsnBuf[12];

    memcpy(JackCmd.Parameter,CardLanBuf+2*512,16);
    for(i = 0; i < 16; i++)
    {
        if((JackCmd.Parameter[i] > 27)&&(JackCmd.Parameter[i] < 32))break;
    }
    if(i != 16)
    {
        memcpy(NandBuf,CardLanBuf+JackCmd.Parameter[i]*512,512);
        for( i = 0; i < 46; i++)
        {
            memcpy(CsnBuf,NandBuf+(i * 11),11);
            if(CsnBuf[0] == Type)
            {
                Buf.i  = 0;
                // Buf.longbuf[0] = CsnBuf[3];
                // Buf.longbuf[1] = CsnBuf[2];
                // Buf.longbuf[2] = CsnBuf[1];
                memcpy(Buf.longbuf,CsnBuf +1,3);


                Test.i  = 0;
                // Test.longbuf[0] = CsnBuf[6];
                // Test.longbuf[1] = CsnBuf[5];
                // Test.longbuf[2] = CsnBuf[4];
                memcpy(Test.longbuf,CsnBuf +4,3);
                JackArm.i = 0;
                //JackArm.longbuf[0] = CsnBuf[10];
                //JackArm.longbuf[1] = CsnBuf[9];
                // JackArm.longbuf[2] = CsnBuf[8];
                // JackArm.longbuf[3] = CsnBuf[7];
                memcpy(JackArm.longbuf,CsnBuf+7,4);

                MoneyNo.Ci  = Buf.i;
                MoneyNo.Day = Test.i;
                MoneyNo.Mon = JackArm.i;
                status = 0;
            }
        }
    }
    return  status;
}


#if 0
unsigned char  YesOrNoMoney_cpu(void)
{
    unsigned char status,errstatus = 4;
    LongUnon Interim;
    status = ControlConsumption_8bit(CardLan.CardType);
    if(status == MI_OK)
    {
        CardLan.CiMoneyFlag = 0x01;
        if((Time.year == CardLan.OldTime[0])&&(Time.month == CardLan.OldTime[1]))
        {
            memcpy(Interim.longbuf,CardLan.MonthValue,4);//�������ѵĽ��
        }
        else
        {
            Interim.i = 0;
            memset(CardLan.DayValue,0,sizeof(CardLan.DayValue));
            memset(CardLan.ViewsValue,0,sizeof(CardLan.ViewsValue));
			memset(CardLan.MonthValue,0,sizeof(CardLan.MonthValue));
            //���¶�
        }
		
        Interim.i += HostValue.i;
        if(Interim.i <= MoneyNo.Mon)
        {
            memcpy(CardLan.MonthValue,Interim.longbuf,4);
            if(Time.day == CardLan.OldTime[2])
            {
                Interim.i = 0;
                memcpy(Interim.longbuf,CardLan.DayValue,3);//�������ѵĽ��
            }
            else
            {
                Interim.i = 0;
                memset(CardLan.ViewsValue,0,sizeof(CardLan.ViewsValue));
				memset(CardLan.DayValue,0,sizeof(CardLan.DayValue));
            }
			
            Interim.i += HostValue.i;
            if(Interim.i <= MoneyNo.Day)
            {
                memcpy(CardLan.DayValue,Interim.longbuf,3);
                if(TypeTime.TimeNum  == CardLan.Period)
                {
                    Interim.i = 0;
                    memcpy(Interim.longbuf,CardLan.ViewsValue,3);//�������ѵĽ��
                }
                else
                {
                    Interim.i = 0;
					memset(CardLan.ViewsValue,0,sizeof(CardLan.ViewsValue));
					//��ʱ���޶�
                }
				
                Interim.i += HostValue.i;
                if(Interim.i <= MoneyNo.Ci)
                {
                    errstatus = 0;
                    memcpy(CardLan.ViewsValue,Interim.longbuf,3);
                    CardLan.Period = TypeTime.TimeNum;
                }
                else errstatus = 1;
            }
            else errstatus = 2;
        }
        else errstatus = 3;
    }
    else
    {
        //memset(Sector.STwoZero+4,0,10);
        //Sector.STwoZero[14] = TypeTime.TimeNum;
#if 0
        memcpy(Interim.longbuf,CardLan.MonthValue,4)
        Interim.i += HostValue.i;
        memcpy(CardLan.MonthValue,Interim.longbuf,4);
#endif
        errstatus = 0;
    }
    return errstatus;
}
#endif
#endif

#if defined(CONFIG_LINUXBUS32BIT)

/*
*************************************************************************************************************
- �������� : unsigned char Limited_Number_Times(unsigned char Type,unsigned char *DatdCi)
- ����˵�� : ��ʾ������Ϣ�ӳ���
- ������� : num :������Ϣ���
- ������� : ��
*************************************************************************************************************
*/
unsigned char Limited_Number_Times(unsigned char Type)
{
    unsigned char i,status = 1;
    unsigned char CsnBuf[6];
    ShortUnon TCtime;

#if  PRINT_NAND
    printf("Limited_Number_Times Type == %d \n",Type);
#endif

    memcpy(JackCmd.Parameter,CardLanBuf+2*512,16);
    for(i = 0; i < 16; i++)
    {
        if((JackCmd.Parameter[i] > 23)&&(JackCmd.Parameter[i] < 28))break;
    }

    if(i != 16)
    {
        memcpy(NandBuf,CardLanBuf+JackCmd.Parameter[i]*512,512);
        for( i = 0; i < 100; i++)
        {
            memcpy(CsnBuf,NandBuf+(i * 5),5);

            if((CsnBuf[0] == 0x55)&&(CsnBuf[1] == 0xAA)) break;
            if(CsnBuf[0] == Type)
            {
                CIshu.ci = CsnBuf[1];
                CIshu.re = CsnBuf[2];
                memcpy(TCtime.intbuf,CsnBuf+3,2);
                CIshu.yue = TCtime.i;
                status = 0;
                break;
            }
        }
    }

#if  PRINT_NAND
    printf("Limited_Number_Times status == %d  c%d r%d y%d\n",status,CIshu.ci,CIshu.re,CIshu.yue);
#endif

    return  status;
}
/*
*************************************************************************************************************
- �������� : unsigned char YesOrNoCiShu(void)
- ����˵�� : ��������
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
//CIshu
unsigned char YesOrNoCiShu(void)
{
    unsigned char status,chet;
    ShortUnon  ShCishu;
    status = Limited_Number_Times(CardLan.CardType);
    if(status == MI_OK)							//��Ϊ0  ��ʾ������֧�ִ�������
    {
        if((Time.year == CardLan.OldTime[0])&&(Time.month == CardLan.OldTime[1]))
        {
            memcpy(ShCishu.intbuf,Sector.SFivOne+9,2);
            ShCishu.i +=1;
            if(ShCishu.i <= CIshu.yue)
            {
                if(Time.day == CardLan.OldTime[2])
                {
                    chet = 	Sector.SFivOne[8];				//��
                    chet +=1;
                    if(chet <= CIshu.re)
                    {
                        if(TypeTime.TimeNum  == CardLan.Period)
                        {
                            chet = 	Sector.SFivOne[7];		//��
                            chet +=1;
                            if(chet <= CIshu.ci)
                            {
                                Sector.SFivOne[7]++;
                                Sector.SFivOne[8]++;
                                memcpy(ShCishu.intbuf,Sector.SFivOne+9,2);
                                ShCishu.i++;
                                memcpy(Sector.SFivOne+9,ShCishu.intbuf,2);
                                Sector.FlagValue = 0;
                                status = 0;
                            }
                            else
                            {
                                status = 1;
                            }
                        }
                        else
                        {

                            memset(Sector.SFivTwo,0,4); //��ʱ���޶�
                            Sector.SFivOne[7] = 1;
                            Sector.SFivOne[8]++;
                            memcpy(ShCishu.intbuf,Sector.SFivOne+9,2);
                            ShCishu.i++;
                            memcpy(Sector.SFivOne+9,ShCishu.intbuf,2);
                            Sector.FlagValue = 0;
                            status = 0;
                        }
                    }
                    else
                    {
                        status = 1;
                    }
                }
                else
                {

                    memset(Sector.SFivTwo,0,8); //�����޶�

                    memcpy(ShCishu.intbuf,Sector.SFivOne+9,2);
                    ShCishu.i++;
                    memcpy(Sector.SFivOne+9,ShCishu.intbuf,2);
                    Sector.FlagValue = 0;
                    Sector.SFivOne[7] = 1;
                    Sector.SFivOne[8] = 1;
                    status = 0;
                }
            }
            else
            {
                status = 1;
            }
        }
        else
        {

            memset(Sector.SFivTwo,0,12); //�����޶�
            Sector.FlagValue = 0;
            Sector.SFivOne[7] = 1;
            Sector.SFivOne[8] = 1;
            ShCishu.i = 1;
            memcpy(Sector.SFivOne+9,ShCishu.intbuf,2);
            status = 0;
        }
    }
    else
    {
        status = 1;//û�����ƴ˿���
    }

#if  PRINT_NAND
    printf("YesOrNoCiShu status == %d \n",status);
#endif

    return status;
}
/*
*************************************************************************************************************
- �������� : unsigned char ControlConsumption(unsigned char Type)
- ����˵�� : ��ʾ������Ϣ�ӳ���
- ������� : num :������Ϣ���
- ������� : ��
*************************************************************************************************************
*/
unsigned char ControlConsumption(unsigned char Type)
{
    unsigned char i,status = 1;
    unsigned char CsnBuf[12];

    memcpy(JackCmd.Parameter,CardLanBuf+2*512,16);
    for(i = 0; i < 16; i++)
    {
        if((JackCmd.Parameter[i] > 27)&&(JackCmd.Parameter[i] < 32))break;
    }
    if(i != 16)
    {
        memcpy(NandBuf,CardLanBuf+JackCmd.Parameter[i]*512,512);
        for( i = 0; i < 46; i++)
        {
            memcpy(CsnBuf,NandBuf+(i * 11),11);
            if(CsnBuf[0] == Type)
            {
                Buf.i  = 0;
                // Buf.longbuf[0] = CsnBuf[3];
                // Buf.longbuf[1] = CsnBuf[2];
                // Buf.longbuf[2] = CsnBuf[1];
                memcpy(Buf.longbuf,CsnBuf +1,3);


                Test.i  = 0;
                // Test.longbuf[0] = CsnBuf[6];
                // Test.longbuf[1] = CsnBuf[5];
                // Test.longbuf[2] = CsnBuf[4];
                memcpy(Test.longbuf,CsnBuf +4,3);
                JackArm.i = 0;
                //JackArm.longbuf[0] = CsnBuf[10];
                //JackArm.longbuf[1] = CsnBuf[9];
                // JackArm.longbuf[2] = CsnBuf[8];
                // JackArm.longbuf[3] = CsnBuf[7];
                memcpy(JackArm.longbuf,CsnBuf+7,4);

                MoneyNo.Ci  = Buf.i;
                MoneyNo.Day = Test.i;
                MoneyNo.Mon = JackArm.i;
                status = 0;
            }
        }
    }
    return  status;
}
/*
*************************************************************************************************************
- �������� : unsigned char  YesOrNoMoney(void)
- ����˵�� : ����
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char  YesOrNoMoney(void)
{
    unsigned char status,errstatus = 4;
    LongUnon Interim;
    status = ControlConsumption(CardLan.CardType);
    if(status == MI_OK)
    {
        if((Time.year == CardLan.OldTime[0])&&(Time.month == CardLan.OldTime[1]))
        {
            memcpy(Interim.longbuf,CardLan.MonthValue,4);//�������ѵĽ��
        }
        else
        {
            Interim.i = 0;
            memset(CardLan.DayValue,0,sizeof(CardLan.DayValue));
            memset(CardLan.ViewsValue,0,sizeof(CardLan.ViewsValue));
            memset(Sector.SFivOne+7,0,4); //�����޴�
        }
        Interim.i += HostValue.i;
        if(Interim.i <= MoneyNo.Mon)
        {
            memcpy(Sector.SFivTwo+8,Interim.longbuf,4);
            if(Time.day == CardLan.OldTime[2])
            {
                Interim.i = 0;
                memcpy(Interim.longbuf,CardLan.DayValue,3);//�������ѵĽ��
            }
            else
            {
                Interim.i = 0;
                memset(CardLan.ViewsValue,0,sizeof(CardLan.ViewsValue));
                memset(Sector.SFivOne+7,0,2); //�����޴�
            }
            Interim.i += HostValue.i;
            if(Interim.i <= MoneyNo.Day)
            {
                memcpy(Sector.SFivTwo+4,Interim.longbuf,4);
                if(TypeTime.TimeNum  == CardLan.Period)
                {
                    Interim.i = 0;
                    memcpy(Interim.longbuf,CardLan.ViewsValue,3);//�������ѵĽ��
                }
                else
                {
                    Interim.i = 0;
                    memset(Sector.SFivOne+7,0,1); //��ʱ���޴�
                }
                Interim.i += HostValue.i;
                if(Interim.i <= MoneyNo.Ci)
                {
                    errstatus = 0;
                    memcpy(Sector.SFivTwo,Interim.longbuf,4);
                }
                else errstatus = 1;
            }
            else errstatus = 2;
        }
        else errstatus = 3;
    }
    else
    {
        memset(Sector.SFivTwo,0,12);
        errstatus = 0;
    }
    return errstatus;
}

unsigned char  YesOrNoMoney_cpu(void)
{
    unsigned char status,errstatus = 4;
    LongUnon Interim;
    status = ControlConsumption(CardLan.CardType);
    if(status == MI_OK)
    {
        CardLan.CiMoneyFlag = 0x01;
        if((Time.year == CardLan.OldTime[0])&&(Time.month == CardLan.OldTime[1]))
        {
            memcpy(Interim.longbuf,CardLan.MonthValue,4);//�������ѵĽ��
        }
        else
        {
            Interim.i = 0;
            memset(CardLan.DayValue,0,sizeof(CardLan.DayValue));
            memset(CardLan.ViewsValue,0,sizeof(CardLan.ViewsValue));
			memset(CardLan.MonthValue,0,sizeof(CardLan.MonthValue));
            //���¶�
        }
		
        Interim.i += HostValue.i;
        if(Interim.i <= MoneyNo.Mon)
        {
            memcpy(CardLan.MonthValue,Interim.longbuf,4);
            if(Time.day == CardLan.OldTime[2])
            {
                Interim.i = 0;
                memcpy(Interim.longbuf,CardLan.DayValue,3);//�������ѵĽ��
            }
            else
            {
                Interim.i = 0;
                memset(CardLan.ViewsValue,0,sizeof(CardLan.ViewsValue));
				memset(CardLan.DayValue,0,sizeof(CardLan.DayValue));
            }
			
            Interim.i += HostValue.i;
            if(Interim.i <= MoneyNo.Day)
            {
                memcpy(CardLan.DayValue,Interim.longbuf,3);
                if(TypeTime.TimeNum  == CardLan.Period)
                {
                    Interim.i = 0;
                    memcpy(Interim.longbuf,CardLan.ViewsValue,3);//�������ѵĽ��
                }
                else
                {
                    Interim.i = 0;
					memset(CardLan.ViewsValue,0,sizeof(CardLan.ViewsValue));
					//��ʱ���޶�
                }
				
                Interim.i += HostValue.i;
                if(Interim.i <= MoneyNo.Ci)
                {
                    errstatus = 0;
                    memcpy(CardLan.ViewsValue,Interim.longbuf,3);
                    CardLan.Period = TypeTime.TimeNum;
                }
                else errstatus = 1;
            }
            else errstatus = 2;
        }
        else errstatus = 3;
    }
    else
    {
        //memset(Sector.STwoZero+4,0,10);
        //Sector.STwoZero[14] = TypeTime.TimeNum;
#if 0
        memcpy(Interim.longbuf,CardLan.MonthValue,4)
        Interim.i += HostValue.i;
        memcpy(CardLan.MonthValue,Interim.longbuf,4);
#endif
        errstatus = 0;
    }
    return errstatus;
}
#endif


#if defined(CONFIG_LINUXBUS2SEC)


/*
*************************************************************************************************************
- �������� : unsigned char ControlConsumption(unsigned char Type)
- ����˵�� : ��ʾ������Ϣ�ӳ���
- ������� : num :������Ϣ���
- ������� : ��
*************************************************************************************************************
*/
unsigned char ControlConsumption(unsigned char Type)
{
    unsigned char i,status = 1;
    unsigned char CsnBuf[12];

    memcpy(JackCmd.Parameter,CardLanBuf+2*512,16);
    for(i = 0; i < 16; i++)
    {
        if((JackCmd.Parameter[i] > 27)&&(JackCmd.Parameter[i] < 32))break;
    }
    if(i != 16)
    {
        memcpy(NandBuf,CardLanBuf+JackCmd.Parameter[i]*512,512);
        for( i = 0; i < 46; i++)
        {
            memcpy(CsnBuf,NandBuf+(i * 11),11);
            if(CsnBuf[0] == Type)
            {
                Buf.i  = 0;
                // Buf.longbuf[0] = CsnBuf[3];
                // Buf.longbuf[1] = CsnBuf[2];
                // Buf.longbuf[2] = CsnBuf[1];
                memcpy(Buf.longbuf,CsnBuf +1,3);


                Test.i  = 0;
                // Test.longbuf[0] = CsnBuf[6];
                // Test.longbuf[1] = CsnBuf[5];
                // Test.longbuf[2] = CsnBuf[4];
                memcpy(Test.longbuf,CsnBuf +4,3);
                JackArm.i = 0;
                //JackArm.longbuf[0] = CsnBuf[10];
                //JackArm.longbuf[1] = CsnBuf[9];
                // JackArm.longbuf[2] = CsnBuf[8];
                // JackArm.longbuf[3] = CsnBuf[7];
                memcpy(JackArm.longbuf,CsnBuf+7,4);

                MoneyNo.Ci  = Buf.i;
                MoneyNo.Day = Test.i;
                MoneyNo.Mon = JackArm.i;
                status = 0;
            }
        }
    }
    return  status;
}
/*
*************************************************************************************************************
- �������� : unsigned char  YesOrNoMoney(void)
- ����˵�� : ����
- ������� : ��
- ������� : ��
*************************************************************************************************************
*/
unsigned char  YesOrNoMoney(void)
{
    unsigned char status,errstatus = 4;
    LongUnon Interim;
    status = ControlConsumption(CardLan.CardType);
    if(status == MI_OK)
    {
        if((Time.year == CardLan.OldTime[0])&&(Time.month == CardLan.OldTime[1]))
        {
            memcpy(Interim.longbuf,CardLan.MonthValue,4);//�������ѵĽ��
        }
        else
        {
            Interim.i = 0;
            memset(CardLan.DayValue,0,sizeof(CardLan.DayValue));
            memset(CardLan.ViewsValue,0,sizeof(CardLan.ViewsValue));
            memset(Sector.SOneTwo+10,0,4); //�����޴�
        }
        Interim.i += HostValue.i;
        if(Interim.i <= MoneyNo.Mon)
        {
            memcpy(Sector.SOneTwo+10,Interim.longbuf,4);
            if(Time.day == CardLan.OldTime[2])
            {
                Interim.i = 0;
                memcpy(Interim.longbuf,CardLan.DayValue,3);//�������ѵĽ��
            }
            else
            {
                Interim.i = 0;
                memset(CardLan.ViewsValue,0,sizeof(CardLan.ViewsValue));
                memset(Sector.SOneTwo+7,0,3); //�����޴�
            }
            Interim.i += HostValue.i;
            if(Interim.i <= MoneyNo.Day)
            {
                memcpy(Sector.SOneTwo+7,Interim.longbuf,4);
              //  if(TypeTime.TimeNum  == CardLan.Period)
                if(1)
                {
                    Interim.i = 0;
                    memcpy(Interim.longbuf,CardLan.ViewsValue,3);//�������ѵĽ��
                }
                else
                {
                    Interim.i = 0;
                    memset(Sector.SOneTwo+4,0,3); //��ʱ���޴�
                }
                Interim.i += HostValue.i;
                if(Interim.i <= MoneyNo.Ci)
                {
                    errstatus = 0;
                    memcpy(Sector.SOneTwo+4,Interim.longbuf,3);
                }
                else errstatus = 1;
            }
            else errstatus = 2;
        }
        else errstatus = 3;
    }
    else
    {
        memset(Sector.SFivTwo,0,12);
        errstatus = 0;
    }
    return errstatus;
}

unsigned char  YesOrNoMoney_cpu(void)
{
    unsigned char status,errstatus = 4;
    LongUnon Interim;
    status = ControlConsumption(CardLan.CardType);
    if(status == MI_OK)
    {
        CardLan.CiMoneyFlag = 0x01;
        if((Time.year == CardLan.OldTime[0])&&(Time.month == CardLan.OldTime[1]))
        {
            memcpy(Interim.longbuf,CardLan.MonthValue,4);//�������ѵĽ��
        }
        else
        {
            Interim.i = 0;
            memset(CardLan.DayValue,0,sizeof(CardLan.DayValue));
            memset(CardLan.ViewsValue,0,sizeof(CardLan.ViewsValue));
			memset(CardLan.MonthValue,0,sizeof(CardLan.MonthValue));
            //���¶�
        }
		
        Interim.i += HostValue.i;
        if(Interim.i <= MoneyNo.Mon)
        {
            memcpy(CardLan.MonthValue,Interim.longbuf,4);
            if(Time.day == CardLan.OldTime[2])
            {
                Interim.i = 0;
                memcpy(Interim.longbuf,CardLan.DayValue,3);//�������ѵĽ��
            }
            else
            {
                Interim.i = 0;
                memset(CardLan.ViewsValue,0,sizeof(CardLan.ViewsValue));
				memset(CardLan.DayValue,0,sizeof(CardLan.DayValue));
            }
			
            Interim.i += HostValue.i;
            if(Interim.i <= MoneyNo.Day)
            {
                memcpy(CardLan.DayValue,Interim.longbuf,3);
                if(TypeTime.TimeNum  == CardLan.Period)
                {
                    Interim.i = 0;
                    memcpy(Interim.longbuf,CardLan.ViewsValue,3);//�������ѵĽ��
                }
                else
                {
                    Interim.i = 0;
					memset(CardLan.ViewsValue,0,sizeof(CardLan.ViewsValue));
					//��ʱ���޶�
                }
				
                Interim.i += HostValue.i;
                if(Interim.i <= MoneyNo.Ci)
                {
                    errstatus = 0;
                    memcpy(CardLan.ViewsValue,Interim.longbuf,3);
                    CardLan.Period = TypeTime.TimeNum;
                }
                else errstatus = 1;
            }
            else errstatus = 2;
        }
        else errstatus = 3;
    }
    else
    {
        //memset(Sector.STwoZero+4,0,10);
        //Sector.STwoZero[14] = TypeTime.TimeNum;
#if 0
        memcpy(Interim.longbuf,CardLan.MonthValue,4)
        Interim.i += HostValue.i;
        memcpy(CardLan.MonthValue,Interim.longbuf,4);
#endif
        errstatus = 0;
    }
    return errstatus;
}
#endif









