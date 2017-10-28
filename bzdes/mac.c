#include <stdio.h>#include <string.h>#include <stdlib.h>#include <time.h>#include "stades.h"#include "mac.h"char PBOC(char* In,unsigned int datalen,char* Out,const char* Key,const unsigned char Keylen){   	int result;	char KC[16]={0};	char KCL[8]={0};	char KCR[8]={0};	//填充八位卡商代码，不足八位补0x20	char Out_Temp[8];	//八位卡商代码，Not 后取值	char NotCardCodeOut[8]={0};	unsigned int TempLen;	if(datalen==0)		return false;	if(Keylen!=16)		return false;	memset(Out_Temp,0,8);	if(datalen>8)		return false;	if(datalen<8)	{		 result=RunPad(3,In,datalen,Out_Temp,&TempLen);	}	else	{		TempLen=datalen;		memcpy(Out_Temp,In,datalen);	}	if(8!=TempLen)		return false;	//3DES PBOC ECB加密生成KCL	RunDes(0,0,Out_Temp,KCL,TempLen,Key,Keylen);	Not(NotCardCodeOut,Out_Temp,TempLen);	//3DES PBOC ECB加密生成KCR	RunDes(0,0,NotCardCodeOut,KCR,TempLen,Key,Keylen);	//合并密钥输出KC	memcpy(KC,KCL,8);	memcpy(KC+8,KCR,8);	memset(Out,0,16);	memcpy(Out,KC,16);	return true;}/*void RandEX(int MAX,int Num,char *RAND){	int k=0;	int j=0;	time_t t;	//设置rand函数所用的启始种子值，以期每次产生的随机数序列均不相同。	char TRand[8]={0};	srand((unsigned) time(&t));	for (k=1;k<=Num;k++)//定制随机数数量。	{		TRand[k]=rand()%MAX;//定制随机数在0至最大值之间。		do		{			for (j=1;j<k;j++)				if (TRand[j]==TRand[k]) //一次随机数序列中有相同随机数则再					//产生一个，直至一次随机数序列中随机数全不相同。				{					TRand[k]=rand()%MAX;					break;				}		}while(j<k);	}	vector<char> vi;	for (int i = 0; i <Num; i++)		vi.push_back(TRand[i]);	random_shuffle(vi.begin(), vi.end());	for (i = 0; i <Num; i++)		RAND[i]=vi[i];}*/char Char2HEX(char *In,unsigned int datalen,char *Out){	int InLen;	int i,j;	if(datalen>16)		return false;	else		InLen=16-datalen;	memset(In+InLen,0x20,InLen);	for(i=0,j=0;i<8;i++,j++)	{		char Temp1=In[2*i];		char Temp2=In[2*i+1];		Out[j]=(Temp1-0x30)*16+(Temp2-0x30);	}	return true;}char EncryptAnyLength(char* In,int datalen,char *Out,unsigned int *OutLen,const char* Key,const unsigned char keylen){	if(datalen==0)		return false;	if(keylen!=16)		return false;	if(datalen == 8)	{		RunDes(0,0,In,Out,datalen,Key,keylen);		return true;	}	else if(datalen < 8)	{		unsigned int TempLen=0;		char Out_Temp[8]={0};		memcpy(Out_Temp,In,datalen);		Out_Temp[datalen]=(char)0x80;		RunDes(0,0,Out_Temp,Out,TempLen,Key,keylen);		return true;	}	else if(datalen > 8)	{		int iParts = datalen>>3;		int iResidue = datalen % 8;		int i;		char TempIn[8]={0};		char TempOut[8]={0};		for( i=0;i<iParts;i++)		{			memset(TempIn,0,8);			memset(TempOut,0,8);			memcpy(TempIn,In + (i<<3),8);			RunDes(0,0,TempIn,TempOut,8,Key,keylen);			memcpy(Out + (i<<3),TempOut,8);		}		if(iResidue!=0)		{			memset(TempIn,0,8);			memset(TempOut,0,8);			memcpy(TempIn,In + (iParts<<3),iResidue);			TempIn[iResidue]=(char)0x80;			RunDes(0,0,TempIn,TempOut,8,Key,keylen);			memcpy(Out + (iParts<<3),TempOut,8);		}		Out[((iParts+1)<<3)] = '\0';		*OutLen=(iParts+1)<<3;	}	return true;}char DecryptAnyLength(char* In,unsigned int datalen,char *Out,unsigned int *OutLen,const char* Key,const unsigned char keylen){	if(datalen==0)		return false;	if(keylen!=16)		return false;	if(datalen == 8)	{		RunDes(1,0,In,Out,datalen,Key,keylen);		return true;	}	else if(datalen < 8)	{		/*		unsigned TempLen=0;		char Out_Temp[8]={0};		memcpy(Out_Temp,In,datalen);		Out_Temp[datalen]=(char)0x80;//这个思路有问题,但这种情况不会出现		RunDes(1,0,Out_Temp,Out,TempLen,Key,keylen);		*/		return false;	}	else if(datalen > 8)	{		int iParts = datalen>>3;		int iResidue = datalen % 8;		int i;		if(iResidue!=0)		{			return false;		}		char TempIn[8]={0};		char TempOut[8]={0};		for(i=0;i<iParts;i++)		{			memset(TempIn,0,8);			memset(TempOut,0,8);			memcpy(TempIn,In + (i<<3),8);			RunDes(1,0,TempIn,TempOut,8,Key,keylen);			memcpy(Out + (i<<3),TempOut,8);		}		/*		if(iResidue!=0)		{			memset(TempIn,0,8);			memset(TempOut,0,8);			memcpy(TempIn,In + (iParts<<3),8);			TempIn[iResidue]=(char)0x80;//这个思路有问题,但这种情况不会出现			RunDes(1,0,TempIn,TempOut,8,Key,keylen);			memcpy(Out + (iParts<<3),TempOut,iResidue);		}		*/		*OutLen=Out[0]*256+Out[1]+2;		Out[*OutLen]='\0';	}  return true;}char MacAnyLength(char* Init,char* In,char *Out,unsigned int datalen,const char* Key,const unsigned char keylen){	char KMA[8]={0};	char KMB[8]={0};	//char OutTemp[8]={0};	char InitTemp[8]={0};//初始数据8个十六进制'0'	if(datalen==0)		return false; if(keylen == 8) 	{	memcpy(KMA,Key,8);    }   else   	{     memcpy(KMA,Key,8);	 memcpy(KMB,Key+8,8);    }    memcpy(InitTemp,Init,8);	if(datalen == 8)	{	    char Temp[8]={0x80,0,0,0,0,0,0,0};		XOR(InitTemp,In,datalen);		RunDes(0,0,InitTemp,InitTemp,datalen,KMA,8);		XOR(InitTemp,Temp,8);		//RunDes(0,0,OutTemp,Out,datalen,KMA,8);		//RunDes(1,0,Out,Out,datalen,KMB,8);		//RunDes(0,0,Out,Out,datalen,KMA,8);		//KMA,KMB,KMA集合在一起		RunDes(0,0,InitTemp,InitTemp,datalen,Key,keylen);		//return true;	}	else if(datalen < 8)	{		char Out_Temp[8]={0};		memcpy(Out_Temp,In,datalen);		Out_Temp[datalen]=(char)0x80;        XOR(InitTemp,Out_Temp,8);		//KMA,KMB,KMA集合在一起		RunDes(0,0,InitTemp,InitTemp,8,Key,keylen);		//return true;	}	else if(datalen > 8)	{		int iParts = datalen>>3;		int iResidue = datalen % 8;		int i;		char TempIn[8]={0};		char TempOut[8]={0};		for(i=0;i<iParts;i++)		{			memcpy(TempIn,In + (i<<3),8);			XOR(InitTemp,TempIn,8);			RunDes(0,0,InitTemp,InitTemp,8,KMA,8);		}		if(iResidue==0)		{			char TempResidue[8]={0x80,0,0,0,0,0,0,0};			XOR(InitTemp,TempResidue,datalen);			RunDes(0,0,InitTemp,InitTemp,8,KMA,8);		}		else		{			memset(TempIn,0,8);			memset(TempOut,0,8);			memcpy(TempIn,In + (iParts<<3),iResidue);			TempIn[iResidue]=(char)0x80;			XOR(InitTemp,TempIn,8);			RunDes(0,0,InitTemp,InitTemp,8,KMA,8);		}		 if(keylen == 16)		   {	  	  RunDes(1,0,InitTemp,InitTemp,8,KMB,8);		  RunDes(0,0,InitTemp,InitTemp,8,KMA,8);		   }	  }	memcpy(Out,InitTemp,4);	return true;}