/*
** $Id: listview.c,v 1.25 2007-08-30 01:20:10 xwyan Exp $
**
** listview.c: Sample program for MiniGUI Programming Guide
**      Usage of LISTVIEW control.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** License: GPL
*/
	/*
#include <apparel.h>
#include "querysql.h"
#include "InitSystem.h"
#include "RC500.h"
#include "des.h"
#include <printer.h>

#define SqlOPEN 0
#define  DISQUER  0

extern FILE *Datafile;

LongUnon AllMoney;
LongUnon TypeMoney;
//char timebuf[10];
volatile unsigned char SQLQUERY;
LongUnon Vaul;

extern LongUnon Buf;
extern LongUnon SaveNumBs;
extern LongUnon CodeNum;


extern unsigned char SQL_Falg ;
extern SysTime Time;
//extern unsigned char CardModeFalg;
extern RouteJack  TypeTime;
extern CardInform CardLan;
//extern  unsigned char FileOpenFlag;    		//文件自锁
//extern volatile unsigned char savedataflag;
unsigned char readflag;

LongUnon QTime,DTime;



static char * captiona [] =
{
    "卡号", "交易", "余额(次)"
};

#define   SCOL_NA       TABLESIZE(captiona)


static CTRLDATA CtrlScoreList[] =
{
    {
        "listview",
        WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | LVS_TREEVIEW,
         5, 5, 310, 188,
        IDC_LISTVIEWSTOCK,
        "score table",
        0
    },
     {
	CTRL_STATIC,
	WS_VISIBLE | SS_SIMPLE,
	0, 198, 320, 20,
	IDC_STATIC_ALL,
	" 总额:0.00元    总次:0次",
	 0
    },
};




static DLGTEMPLATE ListCtrlScore =
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NONE,
    0, 0, 320, 240,
    "查询统计列表",
    0, 0,
    TABLESIZE(CtrlScoreList),
    CtrlScoreList,
    0
};
*/


////////////////////////////////  按卡号查询(统计)消费记录
///////////////////////////////////////////////////////////
/*

#define IDC_MainCard                800
#define IDC_MainCard_STATIC1         (IDC_MainCard + 1)
#define IDC_MainCard_STATIC2         (IDC_MainCard + 2)
#define IDC_MainCard_STATIC3         (IDC_MainCard + 3)
#define IDC_MainCard_EDIT1           (IDC_MainCard + 4)
#define IDC_MainCard_BUTTOK          (IDC_MainCard + 5)
#define IDC_MainCard_BUTTNO          (IDC_MainCard + 6)

#define DATE_MainCardTime            810

static CTRLDATA CtrlMQMainCard[] =
{
        {
        CTRL_STATIC,
	WS_VISIBLE | WS_BORDER  | ES_BASELINE | ES_AUTOWRAP,
         0, 0, 320, 25,
        IDC_MainCard,
        "按条件查询(统计)消费记录",
        0
        },
	{
		CTRL_STATIC,
		WS_VISIBLE | SS_CENTER,
		30,40,210,25,
		IDC_MainCard_STATIC1,
		"请输入查询条件",
		 0
	},
	{
		CTRL_STATIC,
		WS_VISIBLE | SS_CENTER,
		30,75,210,25,
		IDC_MainCard_STATIC2,
		"例:20110808",
		 0
	},
	{
                CTRL_EDIT,
                WS_VISIBLE | WS_TABSTOP | WS_BORDER,
                70, 95, 150, 25,
                IDC_MainCard_EDIT1,
                NULL,
                0
        },
        {
                CTRL_BUTTON,
                WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
                50, 140, 60, 25,
                IDC_MainCard_BUTTOK,
                "确定",
                0
        },
	{
                CTRL_BUTTON,
                WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
                160,140, 60, 25,
                IDC_MainCard_BUTTNO,
                "取消",
                0
        },
        {
	CTRL_STATIC,
	WS_VISIBLE | WS_BORDER  | ES_BASELINE | ES_AUTOWRAP,
	0, 215, 320,25,
	IDC_MainCard_STATIC3,
	"",
	0
        }
};
static DLGTEMPLATE DlgMQMainCard =
{
       WS_BORDER ,
       WS_EX_NONE,
	0, 0, 320,240,
	"",
	0,
	0,
	TABLESIZE(CtrlMQMainCard),
    	CtrlMQMainCard,
	0
};
*/
/*
*************************************************************************************************************
- 函数名称 : char TotalAllDat (void)
- 函数说明 :
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
/*
char TotalAllDat (void)
{
sqlite3* dba;
int index;
int result;
int nRow,nColumn;
LongUnon Savebuf;
char **dbResult;
char* errmsga=NULL;
char bufsql[100];
unsigned char ReadBuf[20],purse;
unsigned int AllDat,AllMonkey,icount;
AllMonkey = 0;
while(SQL_Falg != 0); 					//数据库不在使用为0
SQL_Falg = 1;
if (!(access("cardsave.db", 0)))			//判断数据库文件是否存在
{
    	result = sqlite3_open("cardsave.db",&dba);
    	if(result != SQLITE_OK)
    	{
		#if   SqlOPEN
		       	printf("Open sql Rrr \n");
		#endif
		       	sqlite3_close(dba);
		        return(1);
    	}
	sprintf(bufsql,"select * from Maindata");
	result = sqlite3_get_table(dba,bufsql,&dbResult,&nRow,&nColumn,&errmsga);
	if(SQLITE_OK==result)
    	{
                AllDat = nRow;
                index = nColumn;

		#if   SqlOPEN
		       	printf("Open sql ok1  %d\n",AllDat);
		#endif

                for(icount = 0; icount <= AllDat;icount++)
                {

		        sprintf(ReadBuf,"%s",dbResult[icount*index+17]);
		        Savebuf.i = atoi(ReadBuf);
		        purse = Savebuf.longbuf[0];
			if((purse == 1)||(purse == 2))
                        {
			        sprintf(ReadBuf,"%s",dbResult[icount*index+11]);//交易金额
			        Savebuf.i = atoi(ReadBuf);
			  #if   SqlOPEN
			       	printf("Savebuf.i = %d\n",Savebuf.i);
			  #endif
	                        AllMonkey = AllMonkey + Savebuf.i;
                        }
                }
                AllMoney.i = AllMonkey;
  		sqlite3_free_table(dbResult);
   		sqlite3_close(dba);
   		SQL_Falg = 0;
		#if   SqlOPEN
		       	printf("Open sql ok2 \n");
		#endif
 	}
}
else
{
	return(3);
}
SQL_Falg = 0;
return (0);
}
*/

/*
*************************************************************************************************************
- 函数名称 : void MoneyValue(INT8U *OUT_Data,unsigned int Money)
- 函数说明 : 转换成金钱格式
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
/*

static int ScoreProcList (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
HWND hListView;
LVCOLUMN lvcol;
LVITEM item;
LVSUBITEM subdata;
char zrbuf[150];
char moneybuf[100];
char readdata[73];
//char bufmon[100];
//sqlite3 *db=NULL;
//sqlite3_stmt * stmt;
//const char *tail;

int ncols;
unsigned int i,n,Saddrs,add;
unsigned int rc,val;
//static int iStartPos = 0;
static int space,nspace;
static unsigned char reep;
char status;

    hListView = GetDlgItem (hDlg, IDC_LISTVIEWSTOCK);
          #if DISQUER
     printf("message:%d \n",message);
         #endif
    switch (message)
    {

   case MSG_CREATE:
        Vaul.i = 0;
        space = 0;
        status = -1;
        reep = 0;
        SetTimer(hDlg,DATE_QUERY,20);
	break;

    case MSG_INITDIALOG:
    {
	for (i = 0; i < SCOL_NA; i++)
	{
            lvcol.nCols = i;
	    lvcol.pszHeadText = captiona[i];
            lvcol.width = 100;
            lvcol.pfnCompare = NULL;
            lvcol.colFlags = LVCF_CENTERALIGN;//LVCF_CENTERALIGN;//LVCF_LEFTALIGN;//LVCF_RIGHTALIGN;
            SendMessage (hListView, LVM_ADDCOLUMN, 0, (LPARAM) &lvcol);
        }

#if DISQUER
     printf("timeb:%02X%02X%02X%02X \n",QTime.longbuf[0],QTime.longbuf[1],QTime.longbuf[2],QTime.longbuf[3]);
#endif

#if 1
*/
/*
      memset(zrbuf,0,sizeof(zrbuf));

sprintf(zrbuf,"select RFcsn,RFvaluej,RFvalueh,RFpurse from Maindata where Savetime between '%-4.4s-%-2.2s-%-2.2s 00:00:00' and '%-4.4s-%-2.2s-%-2.2s 23:59:59'",\
	timebuf,timebuf+4,timebuf+6,timebuf,timebuf+4,timebuf+6);

//sprintf(zrbuf,"select RFcsn,RFvaluej,RFvalueh,RFpurse from Maindata where Savetime between '2011-10-08 00:00:00' and '2011-10-08 23:59:59'");

	item.nItemHeight = 25;
        SendMessage (hListView, MSG_FREEZECTRL, TRUE, 0);
	rc = sqlite3_open("cardsave.db",&db);
	rc = sqlite3_prepare(db, zrbuf, (int)strlen(zrbuf), &stmt, &tail);
	if(rc != SQLITE_OK)
	{
		printf("sqlite3_prepare Error\n");
	}
	rc = sqlite3_step(stmt);
	ncols = sqlite3_column_count(stmt);
*/
/*
   item.nItemHeight = 25;
   SendMessage (hListView, MSG_FREEZECTRL, TRUE, 0);

     while (FileOpenFlag == 0);
	FileOpenFlag = 0;
     ReadOrWriteFile(RSDATA);
       FileOpenFlag = 1;


     while(savedataflag);
         savedataflag = 1;
       Datafile = fopen("cardsave.bin","rb+");
        ncols = 3;
        add = 0;
	for(n=0;n< SaveNum.i;n++)
	{

                Saddrs = n * 72;
                rc = fseek(Datafile,Saddrs, SEEK_SET);
                rc = fread(readdata,sizeof(unsigned char),64,Datafile);
               //sprintf(cond,"%02X%02X%02X%02X",readdata[26],readdata[27],readdata[28],readdata[29]);

                 memset(DTime.longbuf,0,sizeof(LongUnon));
                 memcpy(DTime.longbuf,readdata+27,3);
               if(DTime.i == QTime.i)
               {
		item.nItem = add;
		SendMessage (hListView, LVM_ADDITEM, 0, (LPARAM) & item);
		for(i = 0;i < ncols;i++)
		{
			subdata.nItem = add;
			subdata.subItem = i;
			subdata.nTextColor = 0;
			//"编号", "数量", "总价","流水号","日期时间"
			switch(i)
			{
			case 0:
				//sprintf(zrbuf,"%s",sqlite3_column_text(stmt,0));//卡号
                              //  val = atoi(zrbuf);
                                memset(moneybuf,0,sizeof(moneybuf));
                                sprintf(moneybuf,"%02X%02X%02X%02X",readdata[8],readdata[9],\
                                     readdata[10],readdata[11]);
				subdata.pszText = moneybuf;
#if DISQUER
printf("sql0 %s\n",moneybuf);
#endif

				break;
			case 1:
                                memset(moneybuf,0,sizeof(moneybuf));
                                Buf.i = 0;
                                memcpy(Buf.longbuf,readdata+23,3);//交易金额
                                val = Buf.i;
                                if(readdata[44] == 0)  // 交易类型
                                {
                                 sprintf(moneybuf,"%d次",val);
                                  space +=val;
                                }
                                else
                                {
                                MoneyValue(moneybuf,val);
                                 Vaul.i +=val;
                                }
				subdata.pszText = moneybuf;

#if DISQUER
printf("sql1 %s\n",moneybuf);
#endif
				break;
			case 2:
				memset(moneybuf,0,sizeof(moneybuf));
                                Buf.i = 0;
                                memcpy(Buf.longbuf,readdata+45,4);  //卡片余额
                                val = Buf.i;
                                if(readdata[44] == 0)// 交易类型
                                {
                                 memset(moneybuf,0,sizeof(moneybuf));
                                 sprintf(moneybuf,"%d次",val);
                                }
                                else
                                {
                                memset(moneybuf,0,sizeof(moneybuf));
                                MoneyValue(moneybuf,val);
                                }
				subdata.pszText = moneybuf;

#if DISQUER
printf("sql2 %s\n",moneybuf);
#endif

				break;
*/
/*
			case 3:
				sprintf(zrbuf,"%s",sqlite3_column_text(stmt,3));//交易时间
                               memset(moneybuf,0,sizeof(moneybuf));
                               memcpy(moneybuf,zrbuf+5,16);
                                subdata.pszText = moneybuf;
                                 // subdata.pszText = zrbuf;
				break;

			case 4:
				sprintf(zrbuf,"%s",sqlite3_column_text(stmt,8));//时间
				subdata.pszText = zrbuf;
				break;
			case 5:
				sprintf(zrbuf,"%s",sqlite3_column_text(stmt,4));//品名
				subdata.pszText = zrbuf;
				break;
*/
/*


			default:
				break;
			}
			SendMessage (hListView, LVM_SETSUBITEM,0, (LPARAM)&subdata);
		}
               add++;
	      }
		//rc=sqlite3_step(stmt);
	}
   fclose(Datafile);
   savedataflag = 0;
     nspace = add;

#if DISQUER
printf("sql %d\n",add);
#endif


	SendMessage (hListView, MSG_FREEZECTRL, FALSE, 0);
	//释放statement
	//sqlite3_finalize(stmt);
	//关闭数据库
	//sqlite3_close(db);
	SendMessage (hListView, IVM_SETCURSEL,(add-1), 1);
	SendMessage(hDlg, MSG_COMMAND, 0, 0);
#endif
	break;
    }
    return 1;

    //处理水平滚动条

*/
/*
    case MSG_HSCROLL:
    if (wParam == SB_LINERIGHT)
         {
            if (iStartPos < 5)
               {
               iStartPos ++;
               // 向右滚，每次滚一个系统字符宽度
               ScrollWindow (hDlg, -GetSysCharWidth (), 0, NULL, NULL);
               }
         }
    else if (wParam == SB_LINELEFT)
         {
            if (iStartPos > 0)
               {
                iStartPos --;
               // 向左滚，每次滚一个系统字符宽度
               ScrollWindow (hDlg, GetSysCharWidth (), 0, NULL, NULL);
               }
          }
    break;
*/
/*
    case MSG_QUERREAD:
       {
        int sel,count;
              status = ReadCardInfor();
              if(status == 0)
               {
                  reep = 0;
               if(CardLan.CardType ==0xce)	 //总额统计
         	 {
                   SQLQUERY = 1;
                   SendMessage(hDlg, MSG_CLOSE, 0, 0);
                 }
             else if(CardLan.CardType ==0xcf)
                {
	              sel = SendMessage (hListView, IVM_GETCURSEL, 0, 0);
	              count = SendMessage (hListView, IVM_GETITEMCOUNT, 0, 0);
		      if(sel <= 0)sel = count;
		      sel--;
		      SendMessage (hListView, IVM_SETCURSEL, sel, 1);
                }
               else status = 1;
	     }
             else
               {
                 reep ++;
                if(reep >= 15)
                  {
                   SQLQUERY = 0;
                   SendMessage(hDlg, MSG_CLOSE, 0, 0);
                  }
               }
            break;
        }

    case MSG_COMMAND:
       {
	//int countview = SendMessage (hListView, LVM_GETITEMCOUNT, 0, 0);
	if(nspace > 0)
	{
*/
/*
		n = 0;
		for(i = 0;i < countview ; i++)
		{
			subdata.nItem = i;
			subdata.subItem = 1;
			subdata.flags = 0;
			subdata.pszText = moneybuf;
			subdata.nTextColor = 0;
			subdata.nTextMax = 20;
			SendMessage (hListView, LVM_GETSUBITEMTEXT, 0, (LPARAM) &subdata);
			//status = strlen(moneybuf);
                       // memcpy(bufmon,moneybuf,(status-2));
                        //rc = atoi(bufmon);
			//n = n + rc;
		}
*/
/*
		memset(zrbuf,0,sizeof(zrbuf));
		sprintf(zrbuf," 共%02d条  总额:",nspace);
		//memset(bufmon,0,sizeof(bufmon));
		//myitoa(bufmon,n);
		memset(moneybuf,0,sizeof(moneybuf));
		//strmoney(bufmon,moneybuf);
		//strcat(zrbuf,moneybuf);
		//strcat(zrbuf,"元");
              // n=(unsigned int)(n*100);
                MoneyValue(moneybuf,Vaul.i);
                strcat(zrbuf,moneybuf);
                //strcat(zrbuf,"元");
                memset(moneybuf,0,sizeof(moneybuf));
                sprintf(moneybuf," 总次:%d次",space);
                strcat(zrbuf,moneybuf);
		SetDlgItemText(hDlg, IDC_STATIC_ALL,zrbuf);
	}
	else
	{
		SetDlgItemText(hDlg, IDC_STATIC_ALL," 总额:0.00元    总次:0次");
	}
      }
     break;

   case MSG_TIMER:
	    {
		  if(wParam == DATE_QUERY)
		    {
			SendMessage(hDlg, MSG_QUERREAD,0, 0);
		    }

	    }
	   break;

    case MSG_CLOSE:
        EndDialog (hDlg,wParam);
        DestroyAllControls (hDlg);
        return 0;

    default :
         break;
    }
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}
*/
/*
unsigned char Disbuf[60];
unsigned int space,add;
unsigned int readcardcode(int massgadd,unsigned char type)
{
unsigned int rc,n,Saddrs,val;
unsigned char i,ncols;
unsigned char readdata[73];
unsigned char moneybuf[20];

     while (FileOpenFlag == 0);
	FileOpenFlag = 0;
     ReadOrWriteFile(RSDATA);
       FileOpenFlag = 1;
     while(savedataflag);
         savedataflag = 1;
       Datafile = fopen("cardsave.bin","rb+");
        ncols = 3;
        space = 0;
        add = 0;
        val = 0;
        Vaul.i = 0;
	for(n = massgadd;n< SaveNumBs.i;n++)
	{
                Saddrs = n * 72;
                rc = fseek(Datafile,Saddrs, SEEK_SET);
                rc = fread(readdata,sizeof(unsigned char),64,Datafile);
                memset(DTime.longbuf,0,sizeof(LongUnon));
                memcpy(DTime.longbuf,readdata+27,3);
               if(DTime.i == QTime.i)
               {
                add++;
              if(type == 0x55)
               {
                memset(Disbuf,0,sizeof(Disbuf));
		for(i = 0;i < ncols;i++)
		{
			switch(i)
			{
			case 0:
                                memset(moneybuf,0,sizeof(moneybuf));
                                sprintf(moneybuf,"%02X%02X%02X%02X",readdata[8],readdata[9],\
                                     readdata[10],readdata[11]);
                               memcpy(Disbuf,moneybuf,20);

				#if DISQUER
				printf("sql0 %s\n",moneybuf);
				#endif
				break;

			case 1:
                                memset(moneybuf,0,sizeof(moneybuf));
                                Buf.i = 0;
                                memcpy(Buf.longbuf,readdata+23,3);//交易金额
                                val = Buf.i;
                                if(readdata[44] == 0)  // 交易类型
                                {
                                 sprintf(moneybuf,"%d次",val);
                                }
                                else
                                {
                                MoneyValue(moneybuf,val);
                                }
                               memcpy(Disbuf+20,moneybuf,20);

			#if DISQUER
			printf("sql1 %s\n",moneybuf);
			#endif
		            break;

			case 2:
				memset(moneybuf,0,sizeof(moneybuf));
                                Buf.i = 0;
                                memcpy(Buf.longbuf,readdata+45,4);  //卡片余额
                                val = Buf.i;
                                if(readdata[44] == 0)// 交易类型
                                {
                                 memset(moneybuf,0,sizeof(moneybuf));
                                 sprintf(moneybuf,"%d次",val);
                                }
                                else
                                {
                                memset(moneybuf,0,sizeof(moneybuf));
                                MoneyValue(moneybuf,val);
                                }
                               memcpy(Disbuf+40,moneybuf,20);

		#if DISQUER
		printf("sql2 %s\n",moneybuf);
		#endif
						break;
			default:
				break;
			}
		  }
		#if 1
		printf("\n\nQuerry  OK\n"); //add
		printf("Number: %d\n",add); //add
		memset(moneybuf,0,sizeof(moneybuf));
		memcpy(moneybuf,Disbuf,20);
		printf("Cardnum: %s\n",moneybuf);
		memset(moneybuf,0,sizeof(moneybuf));
		memcpy(moneybuf,Disbuf+20,20);
		printf("value  : %s\n",moneybuf);
		memset(moneybuf,0,sizeof(moneybuf));
		memcpy(moneybuf,Disbuf+40,20);
		printf("Svalue : %s\n",moneybuf);
		usleep(500000);
		#endif
                break;
               }
               else
                  {
                   Buf.i = 0;
                   memcpy(Buf.longbuf,readdata+23,3);//交易金额
                   val = Buf.i;
                   if(readdata[44] == 0)  // 交易类型
                     {
                       space +=val;
                     }
                    else
                     {
                       Vaul.i +=val;
                     }
                  }
	      }

	}
       fclose(Datafile);
       savedataflag = 0;
	#if 1
	printf("Querry  OK\n"); //add
	printf("Querry space == %d\n",add);
	printf("Querry ci  == %d\n",space);
	memset(moneybuf,0,sizeof(moneybuf));
	memset(readdata,0,sizeof(readdata));
	sprintf(readdata,"Querry value :");
	MoneyValue(moneybuf,Vaul.i);
	strcat(readdata,moneybuf);
	printf(readdata);
	#endif

  return  n;
}

*/
/*
*************************************************************************************************************
- 函数名称 : void MoneyValue(INT8U *OUT_Data,unsigned int Money)
- 函数说明 : 转换成金钱格式
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
/*
//yao
static int QuerrydataPoc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
static int messagedd;
static int codeaddr;
char Dbuffer[60];
char buff[30];
unsigned char status;
static unsigned char reep;

    switch (message)
    {

       case MSG_CREATE:
	        Vaul.i = 0;
                status = -1;
                add = 0;
	        space = 0;
                codeaddr = 0;
                memset(Disbuf,0,sizeof(Disbuf));
	        SetTimer(hDlg,DATE_QUERY,20);
	     break;

       case MSG_INITDIALOG:
              {
   	       SetWindowAdditionalData (hDlg, lParam);
	       messagedd  = GetWindowAdditionalData(hDlg);
               SetWindowBkColor (hDlg, PIXEL_blue);
              }
             break;
	case MSG_PAINT:
		{

			HDC hdc = BeginPaint (hDlg);
			SetBkMode (hdc, BM_TRANSPARENT);//输入提示文字
                        SetTextColor(hdc,PIXEL_black);
                        TextOut(hdc, 60 , 30,"正在按条件查询数据,请稍候!");
                       codeaddr = readcardcode(codeaddr,messagedd);
                       if(add != 0)
                       {
                        if(messagedd == 0x55)
                           {
                             TextOut(hdc, 40 , 55,"查询数据成功");

                             memset(buff,0,sizeof(buff));
                             memcpy(buff,Disbuf,20);
                             TextOut(hdc, 40 , 80, Disbuf);

                             memset(buff,0,sizeof(buff));
                             memcpy(buff,Disbuf+20,20);
                             TextOut(hdc, 40 , 105, Disbuf);

                             memset(buff,0,sizeof(buff));
                             memcpy(buff,Disbuf+40,20);
                             TextOut(hdc, 40 , 130, Disbuf);

                           }
                           else
                             {

                               TextOut(hdc, 40 , 55," 统计成功    ");

                               memset(Dbuffer,0,sizeof(Dbuffer));
                               sprintf(Dbuffer,"共计： %d条",add);
                               TextOut(hdc, 40 , 80,Dbuffer);

                               memset(Dbuffer,0,sizeof(Dbuffer));
                               sprintf(Dbuffer,"次数消费总次: %d次",space);
                               TextOut(hdc, 40 , 105,Dbuffer);

                               memset(Dbuffer,0,sizeof(Dbuffer));
                               memset(buff,0,sizeof(buff));
	                       sprintf(Dbuffer,"现金消费总额:");
	                       MoneyValue(buff,Vaul.i);
	                       strcat(Dbuffer,buff);
                               TextOut(hdc, 40 , 130, Dbuffer);
                             }
                           }
                          else
                             {
                                TextOut(hdc, 40 , 55,"      温馨提示");
                                TextOut(hdc, 40 , 80,"此时间段没有消费数据");
                               TextOut(hdc, 40 , 105,"      谢谢使用");
                                sleep(1);
                               //usleep(500000);
                             }
                   	 EndPaint(hDlg,hdc);

                }
                break;

     case MSG_QUERREAD:
       {
              status = ReadCardInfor();
              if(status == 0)
               {
                  reep = 0;
               if(CardLan.CardType ==0xce)	 //总额统计
         	 {
                   SQLQUERY = 1;
                   SendMessage(hDlg, MSG_CLOSE, 0, 0);
                 }
             else if(CardLan.CardType ==0xcf)
                {
                  SendMessage(hDlg,MSG_PAINT, 0, 0);
                }
               else status = 1;
	     }
             else
               {
                 reep ++;
                if(reep >= 25)
                  {
                   SQLQUERY = 0;
                   SendMessage(hDlg, MSG_CLOSE, 0, 0);
                  }
               }
            break;
        }

   // case MSG_COMMAND:
   //    {
   //
   //    }
   //   break;

   case MSG_TIMER:
	    {
	      if(wParam == DATE_QUERY)
	        {
		SendMessage(hDlg, MSG_QUERREAD,0, 0);
	        }
	    }
	   break;

    case MSG_CLOSE:
         KillTimer (hDlg,DATE_QUERY);
         EndDialog (hDlg,wParam);
         DestroyAllControls (hDlg);
         return 0;

    default :
         break;
    }
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}
*/
/*
*************************************************************************************************************
- 函数名称 : static int Modify_Time(HWND hWnd)
- 函数说明 : 三级主菜单界面程序 - 修改时间界面
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
/*
//yao
int Admin_Query(HWND hWnd,int messaged)
{
     char buff[8];

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "listview" , 0 , 0);
#endif
    if (!InitMiniGUIExt())
    {
        return 2;
    }
   Rd_time (buff);
   SQLQUERY = 1;
   while(SQLQUERY)
    {
    // sprintf(timebuf,"20%02X%02X%02X",buff[0],buff[1],buff[2]);
      memset(QTime.longbuf,0,sizeof(LongUnon));
      memcpy(QTime.longbuf,buff,3);
     if(buff[2] !=0)
      {
        DlgMessboxcard.controls = CtrlPopupcard;
        DialogBoxIndirectParam (&DlgMessboxcard,hWnd,QuerrydataPoc,messaged);
        buff[2] --;
        if((buff[2]&0x0f) == 0x0f)
	  {
	  buff[2] = (buff[2]&0xf0)+9;
          }
      }
      else
        {
           SQLQUERY = 0;
           break;
        }
    }
      return 0;
}

*/


/*
*************************************************************************************************************
- 函数名称 : void MoneyValue(INT8U *OUT_Data,unsigned int Money)
- 函数说明 : 转换成金钱格式
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
/*
static int ScoreProcList (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
HWND hListView;
LVCOLUMN lvcol;
LVITEM item;
LVSUBITEM subdata;
char zrbuf[150];
char moneybuf[100];
//char bufmon[100];
sqlite3 *db=NULL;
sqlite3_stmt * stmt;
const char *tail;
int ncols;
unsigned int i,n;
unsigned int rc,val;
static int iStartPos = 0;


    hListView = GetDlgItem (hDlg, IDC_LISTVIEWSTOCK);
          #if DISQUER
     printf("message:%d \n",message);
         #endif
    switch (message)
    {
    case MSG_INITDIALOG:
    {
     //  hListView = GetDlgItem (hDlg, IDC_LISTVIEWSTOCK);
   #if DISQUER
     printf("timebuf 02 \n");
    #endif

	for (i = 0; i < SCOL_NA; i++)
	{
            lvcol.nCols = i;
	    lvcol.pszHeadText = captiona[i];
            if(i == 3) lvcol.width = 120;
            else lvcol.width = 80;
            lvcol.pfnCompare = NULL;
            lvcol.colFlags = LVCF_CENTERALIGN;//LVCF_CENTERALIGN;//LVCF_LEFTALIGN;//LVCF_RIGHTALIGN;
            SendMessage (hListView, LVM_ADDCOLUMN, 0, (LPARAM) &lvcol);
        }

#if DISQUER
     printf("timebuf:%s \n",timebuf);
#endif

#if 1
	i = strlen(timebuf);
	switch(i)
	{
	case 4:
	case 5:
	sprintf(zrbuf,"select RFcsn,RFvaluej,RFvalueh,Savetime from Maindata where Savetime between '%-4.4s-01-01 00:00:00' and '%-4.4s-12-31 23:59:59'",\
			timebuf,timebuf);
		break;
	case 6:
	case 7:
sprintf(zrbuf,"select RFcsn,RFvaluej,RFvalueh,Savetime from Maindata where Savetime between '%-4.4s-%-2.2s-01 00:00:00' and '%-4.4s-%-2.2s-31 23:59:59'",\
	timebuf,timebuf+4,timebuf,timebuf+4);
		break;
	case 8:
sprintf(zrbuf,"select RFcsn,RFvaluej,RFvalueh,Savetime from Maindata where Savetime between '%-4.4s-%-2.2s-%-2.2s 00:00:00' and '%-4.4s-%-2.2s-%-2.2s 23:59:59'",\
	timebuf,timebuf+4,timebuf+6,timebuf,timebuf+4,timebuf+6);
		break;
	}

	item.nItemHeight = 25;
        SendMessage (hListView, MSG_FREEZECTRL, TRUE, 0);
	rc = sqlite3_open("cardsave.db",&db);
	rc = sqlite3_prepare(db, zrbuf, (int)strlen(zrbuf), &stmt, &tail);
	if(rc != SQLITE_OK)
	{
		printf("sqlite3_prepare Error\n");
	}
	rc = sqlite3_step(stmt);
	ncols = sqlite3_column_count(stmt);
	n = 0;
	while(rc == SQLITE_ROW)
	{
		item.nItem = n;
		SendMessage (hListView, LVM_ADDITEM, 0, (LPARAM) & item);
		for(i = 0;i < ncols;i++)
		{
			subdata.nItem = n;
			subdata.subItem = i;
			subdata.nTextColor = 0;
			//"编号", "数量", "总价","流水号","日期时间"
			switch(i)
			{
			case 0:
				sprintf(zrbuf,"%s",sqlite3_column_text(stmt,0));//卡号
                                val = atoi(zrbuf);
                                memset(moneybuf,0,sizeof(moneybuf));
                                sprintf(moneybuf,"%08d",val);
				subdata.pszText = moneybuf;
				break;
			case 1:
				sprintf(zrbuf,"%s",sqlite3_column_text(stmt,1));//交易金额
				val = atoi(zrbuf);
                                memset(moneybuf,0,sizeof(moneybuf));
                                MoneyValue(moneybuf,val);
				subdata.pszText = moneybuf;
				break;
			case 2:
				memset(moneybuf,0,sizeof(moneybuf));
				sprintf(zrbuf,"%s",sqlite3_column_text(stmt,2));//卡片余额
				val = atoi(zrbuf);
                                memset(moneybuf,0,sizeof(moneybuf));
                                MoneyValue(moneybuf,val);
				subdata.pszText = moneybuf;
				break;
			case 3:
				sprintf(zrbuf,"%s",sqlite3_column_text(stmt,3));//交易时间
                               memset(moneybuf,0,sizeof(moneybuf));
                               memcpy(moneybuf,zrbuf+5,16);
                                subdata.pszText = moneybuf;
                                 // subdata.pszText = zrbuf;
				break;
*/
/*
			case 4:
				sprintf(zrbuf,"%s",sqlite3_column_text(stmt,8));//时间
				subdata.pszText = zrbuf;
				break;
			case 5:
				sprintf(zrbuf,"%s",sqlite3_column_text(stmt,4));//品名
				subdata.pszText = zrbuf;
				break;
*/
/*
			default:
				break;
			}
			SendMessage (hListView, LVM_SETSUBITEM,0, (LPARAM) & subdata);
		}
		n++;
		rc=sqlite3_step(stmt);
	}
	SendMessage (hListView, MSG_FREEZECTRL, FALSE, 0);
	//释放statement
	sqlite3_finalize(stmt);
	//关闭数据库
	sqlite3_close(db);
	SendMessage (hListView, IVM_SETCURSEL,( n - 1), 1);
	SendMessage(hDlg, MSG_COMMAND, 0, 0);
#endif
	break;
    }
    return 1;
   //处理水平滚动条
    case MSG_HSCROLL:
    if (wParam == SB_LINERIGHT)
         {
            if (iStartPos < 5)
               {
               iStartPos ++;
               //向右滚，每次滚一个系统字符宽度
               ScrollWindow (hDlg, -GetSysCharWidth (), 0, NULL, NULL);
               }
         }
    else if (wParam == SB_LINELEFT)
         {
            if (iStartPos > 0)
               {
                iStartPos --;
               // 向左滚，每次滚一个系统字符宽度
               ScrollWindow (hDlg, GetSysCharWidth (), 0, NULL, NULL);
               }
          }
    break;
    case MSG_KEYDOWN:
      //  Beepkey();
      {
	int sel = SendMessage (hListView, IVM_GETCURSEL, 0, 0);
	int count = SendMessage (hListView, IVM_GETITEMCOUNT, 0, 0);
	SendMessage(hQueryWnd, MSG_USER, 2, 0);
	switch(wParam)
	{
		case SCANCODE_CURSORBLOCKDOWN:
		{
			sel++;
			if(sel >= count)sel = 0;
			SendMessage (hListView, IVM_SETCURSEL, sel, 1);
		}
		break;
		case SCANCODE_CURSORBLOCKUP:
		{
			if(sel <= 0)sel = count;
			sel--;
			SendMessage (hListView, IVM_SETCURSEL, sel, 1);
		}
		break;
		case SCANCODE_CURSORBLOCKLEFT:
		{
			SendMessage(hListView, MSG_HSCROLL, SB_LINELEFT, 0);
		}
		break;
		case SCANCODE_CURSORBLOCKRIGHT:
		{
			SendMessage(hListView, MSG_HSCROLL, SB_LINERIGHT, 0);
		}
		break;
		case SCANCODE_BACKSPACE:
		{
			SendMessage(hDlg, MSG_CLOSE, 0, 0);
		}
		break;
	}
    break;
     }
    case MSG_COMMAND:
       {
	int countview = SendMessage (hListView, LVM_GETITEMCOUNT, 0, 0);
	if(countview > 0)
	{
		n = 0;
		for(i = 0;i < countview ; i++)
		{
			subdata.nItem = i;
			subdata.subItem = 1;
			subdata.flags = 0;
			subdata.pszText = moneybuf;
			subdata.nTextColor = 0;
			subdata.nTextMax = 20;
			SendMessage (hListView, LVM_GETSUBITEMTEXT, 0, (LPARAM) &subdata);
			rc = atoi(moneybuf);
			n = n + rc;
		}
		memset(zrbuf,0,sizeof(zrbuf));
		sprintf(zrbuf," 统计:共%02d条   交易总额:",countview);
		//memset(bufmon,0,sizeof(bufmon));
		//myitoa(bufmon,n);
		memset(moneybuf,0,sizeof(moneybuf));
		//strmoney(bufmon,moneybuf);
		//strcat(zrbuf,moneybuf);
		//strcat(zrbuf,"元");
                n=(unsigned int)(n*100);
                MoneyValue(moneybuf,n);
                strcat(zrbuf,moneybuf);
                strcat(zrbuf,"元");
		SetDlgItemText(hDlg, IDC_STATIC_ALL,zrbuf);
	}
	else
	{
		SetDlgItemText(hDlg, IDC_STATIC_ALL,"统计总额:0.00元");
	}
      }
     break;

    case MSG_CLOSE:
        EndDialog (hDlg,wParam);
        DestroyAllControls (hDlg);
        return 0;

    default :
        #if DISQUER
     printf("message d:%d \n",message);
         #endif
         break;

    }
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}
*/
/*
*************************************************************************************************************
- 函数名称 : static int ModifyTimeProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
- 函数说明 :
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
/*
static int ModifyCardProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
   unsigned char buff[10];
   unsigned char time_buf[50];
   unsigned char tbuf[100];
   switch(message)
   {
        case MSG_INITDIALOG:
                SendDlgItemMessage (hDlg,IDC_MainCard_EDIT1, EM_LIMITTEXT, 8, 0);
                SetTimer(hDlg,DATE_MainCardTime,50);
            return(1);
        case MSG_COMMAND:
             break;
        case MSG_TIMER:
            if(wParam == DATE_MainCardTime)
            {
               sprintf(tbuf,"         %s",mk_time(time_buf));
               SetDlgItemText(hDlg,IDC_MainCard_STATIC3,tbuf);
            }
            break;
       case MSG_CLOSE:
            EndDialog (hDlg,wParam);
            DestroyAllControls (hDlg);
            return 0;
        case MSG_KEYDOWN:
	   {
              //  Beepkey();
		 switch(wParam)
		 {
		       case SCANCODE_BACKSPACE:
                        GetDlgItemText (hDlg,IDC_MainCard_EDIT1,buff, 8);
                         if((strlen(buff)==0))
                            {
                            SendMessage(hDlg, MSG_CLOSE,0, 0);
                            }
                            break;
                      case SCANCODE_F5:
                           SendMessage(hDlg, MSG_CLOSE,0, 0);
                           break;
                      case SCANCODE_ENTER:
			   memset(buff,0,sizeof(buff));
	                   GetDlgItemText (hDlg,IDC_MainCard_EDIT1,buff, 8);
                           if((strlen(buff) < 4))
                             {
                           	MessageBox(hDlg,"输入长度有误","输入错误",MB_OKCANCEL|MB_ICONHAND);
                                memset(buff,0,sizeof(buff));
                             }
                           else
                            {
                               memset(timebuf,0,sizeof(timebuf));
                               memcpy(timebuf,buff,10);
                               SQLQUERY = 1;
                               SendMessage(hDlg, MSG_CLOSE,0, 0);
                            }
                          break;
                      default:
                              break;
               }
           }
         break;

        default:
            break;
   }
   return DefaultDialogProc (hDlg, message, wParam, lParam);
}
*/
/*
*************************************************************************************************************
- 函数名称 : static int Modify_Time(HWND hWnd)
- 函数说明 : 三级主菜单界面程序 - 修改时间界面
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
/*

int Admin_Query(HWND hWnd)
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "listview" , 0 , 0);
#endif
    if (!InitMiniGUIExt())
    {
        return 2;
    }
        SQLQUERY = 0;
	DlgMQMainCard.controls = CtrlMQMainCard;
	DialogBoxIndirectParam(&DlgMQMainCard, hWnd, ModifyCardProc,0L);
        if(SQLQUERY ==1)
         {
      //memset(timebuf,0,sizeof(timebuf));
      //sprintf(timebuf,"2011");
#if DISQUER
          printf("timebuf: %s  SQL_Falg=%d \n",timebuf,SQL_Falg);
#endif
          while(SQL_Falg); //数据库不在使用为0
           SQL_Falg = 1;
          ListCtrlScore.controls = CtrlScoreList;
          DialogBoxIndirectParam (&ListCtrlScore, hWnd, ScoreProcList, 0L);
           SQL_Falg = 0;
           SQLQUERY = 0;
         }
       // MiniGUIExtCleanUp ();
	return 0;
}
*/






