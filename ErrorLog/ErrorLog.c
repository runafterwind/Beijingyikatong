#include  <apparel.h>
#include "../gui/RC500.h"
#include "../gui/InitSystem.h"
#include "../ErrorLog/ErrorLog.h"
#include <libgen.h>

#define		LOG_FILE	"/var/run/CARDLAN_LOG.dat"
#define		LOG_FILE_SIZE		(1024*10)
#define		ONE_LOG_SIZE	28

unsigned int SaveRet;

static unsigned char bStartUpLoad = 0;
static unsigned char bLogLock= 0;
unsigned char DevVersion[30];


extern CardInform CardLan;
extern SectionFarPar Section,Sectionup;
extern LongUnon DevNum;
extern unsigned short SectionNum;

typedef struct{
	unsigned short type;
	unsigned char tim[5];
	unsigned char space[11];
	unsigned char card_id[4];
	unsigned char file[4];
	unsigned short line;
}NORMAL_ERR_TYPE;

void Set_bStartUpLoad(void)
{
	while(bLogLock)
		usleep(1000);
	bStartUpLoad = 1;
}
void Clear_bStartUpLoad(void)
{
	bStartUpLoad = 0;
}

static void GetSemphon(void)
{
	while(bLogLock)
		usleep(1000);
	bLogLock = 1;
}


void SaveLog(unsigned int type,void *pfile,unsigned int line)
{
	FILE *pFd = NULL;
	struct stat stbuff;
	/*int ret;*/
	unsigned char buf[ONE_LOG_SIZE];
	unsigned char tmpbuf[6];
	unsigned int len;
	unsigned char *pbsname;
	struct STATION_ERROR_ *pStaErr;
	struct SWIPING_TIME_  *pSwipErr;
	struct HAVE_NOT_ABOARD_ *pAboardErr;
	struct SHEET_RATION_ERR_ *pSheetErr;
	NORMAL_ERR_TYPE *pNormalErr;
	struct USER_ACTION_RECORD_ *pUserAct;
    struct UART_ANNOUNCER_ERR_  *pUart;
	if(bStartUpLoad)
		return;
	GetSemphon();
	pFd = fopen(LOG_FILE, "ab+");
	stat(LOG_FILE,&stbuff);
	if(stbuff.st_size > LOG_FILE_SIZE)
		fseek(pFd,0, SEEK_SET);
	else
		fseek(pFd,0, SEEK_END);	
   
//	pbsname = basename(pfile);
    pbsname = "uart";
   
	len = strlen(pbsname);
	if(len > 4) len = 4;
	memset(buf,0,sizeof(buf));
	switch(type)
	{
		case STATION_ERROR: // Õ¾µã´íÎó
			pStaErr = (struct STATION_ERROR_ *)buf;
			pStaErr->error_type = type;
			Rd_time(tmpbuf);
			memcpy(pStaErr->time,tmpbuf+1,5);
			pStaErr->card_stationid = CardLan.StationID;
			pStaErr->card_stationOn = CardLan.StationOn;
			pStaErr->section_stationNow = Section.SationNow;
			pStaErr->section_Updown = Section.Updown;
			pStaErr->section_stationNum = Section.SationNum[0];
			pStaErr->sectionup_stationNum = Sectionup.SationNum[0];
			pStaErr->sectionNum = SectionNum;

			memcpy(pStaErr->card_id, CardLan.CardCsnB, 4);
			memcpy(pStaErr->file, pbsname, len);
			pStaErr->line = (unsigned short)line;
			break;
		case EXCEED_TIME_GAP:
			pSwipErr = (struct SWIPING_TIME_ *)buf;
			pSwipErr->error_type = type;
			Rd_time(tmpbuf);
			memcpy(pSwipErr->time,tmpbuf+1,5);
			pSwipErr->DevNum_i = DevNum.i;
			//pSwipErr->CardLan_OldTermNo = CardLan.OldTermNo;
			memcpy(&(pSwipErr->CardLan_OldTermNo),CardLan.OldTermNo,4);
			memcpy(&(pSwipErr->section_DeductTime), Section.DeductTime, 2);
			//pSwipErr->section_DeductTime = Section.DeductTime

			memcpy(pSwipErr->card_id, CardLan.CardCsnB, 4);
			memcpy(pSwipErr->file, pbsname, len);
			pSwipErr->line = (unsigned short)line;			
			break;
		case HAVE_NO_ABOARD_FLAG:
			pAboardErr = (struct HAVE_NOT_ABOARD_ *)buf;
			pAboardErr->error_type = type;
			Rd_time(tmpbuf);
			memcpy(pAboardErr->time,tmpbuf+1,5);
			pAboardErr->card_stationid = CardLan.StationID;
			pAboardErr->card_stationOn = CardLan.StationOn;
			pAboardErr->card_EnterExitFlag = CardLan.EnterExitFlag;

			memcpy(pAboardErr->card_id, CardLan.CardCsnB, 4);
			memcpy(pAboardErr->file, pbsname, len);
			pAboardErr->line = (unsigned short)line;			
			break;
		case SHEET_RATION_ERR:
			pSheetErr = (struct SHEET_RATION_ERR_ *)buf;
			pSheetErr->error_type = type;
			Rd_time(tmpbuf);
			memcpy(pSheetErr->time,tmpbuf+1,5);
			pSheetErr->status = SaveRet;

			memcpy(pSheetErr->card_id, CardLan.CardCsnB, 4);
			memcpy(pSheetErr->file, pbsname, len);
			pSheetErr->line = (unsigned short)line;			
			break;
		case GPRS_DOWNLOAD_PARAM_OK:
		case GPRS_DOWNLOAD_PARAM_FAIL:
		case U_DISK_DOWN_PARAM_OK:
		case U_DISK_DOWN_PARAM_FAIL:
		case GPRS_UPDATE_OK:
		case GPRS_UPDATE_FAIL:
		case U_DISK_UPDATE_OK:
		case U_DISK_UPDATE_FAIL:
		case SIM_CARD_NOT_IN_POS:
		case USB_FORMAT_DATA:
		case GPRS_FORMAT_DATA:
		case USB_RM_LIMI_USER:
		case GPRS_RM_LIMI_USER:
		case USB_SET_SECTOR:
		case GPRS_SET_SECTOR:
			pUserAct = (struct USER_ACTION_RECORD_ *)buf;
			pUserAct->error_type = type;
			Rd_time(tmpbuf);
			memcpy(pUserAct->time,tmpbuf+1,5);
			pUserAct->term_id = DevNum.i;

			//memcpy(pUserAct->card_id, CardLan.CardCsnB, 4);
			memcpy(pUserAct->file, pbsname, len);
			pUserAct->line = (unsigned short)line;				
			break;
		case USB_RESET_TERM_NUM:
		case GPRS_RESET_TERM_NUM:
			pUserAct = (struct USER_ACTION_RECORD_ *)buf;
			pUserAct->error_type = type;
			Rd_time(tmpbuf);
			memcpy(pUserAct->time,tmpbuf+1,5);
			pUserAct->term_id = DevNum.i;

			//memcpy(pUserAct->card_id, CardLan.CardCsnB, 4);
			pUserAct->tmp_4byte.old_term_id = SaveRet;
			memcpy(pUserAct->file, pbsname, len);
			pUserAct->line = (unsigned short)line;				
			break;
        case RS485_ERROR:
         //   printf("aaaaaaaaaaa33\n");
            pUart  = (struct UART_ANNOUNCER_ERR_ *)buf;
            pUart->error_type = type;
            Rd_time(tmpbuf);
            memcpy(pUart->time,tmpbuf,6);
   			pUart->section_stationNow = Section.SationNow;
			pUart->section_Updown = Section.Updown;
			pUart->section_stationNum = Section.SationNum[0];
			pUart->sectionup_stationNum = Sectionup.SationNum[0];
			pUart->sectionNum = SectionNum;
            memcpy(pUart->file, pbsname, len);
            pUart->line = (unsigned short)line;	
         //   printf("aaaaaaaaaaa44\n");
            break;
		default:
			pNormalErr = (NORMAL_ERR_TYPE*)buf;
			pNormalErr->type = type;
			Rd_time(tmpbuf);			
			memcpy(pNormalErr->tim,tmpbuf+1,5);
			pNormalErr->space[0] = SaveRet;

			memcpy(pNormalErr->card_id, CardLan.CardCsnB, 4);
			memcpy(pNormalErr->file, pbsname, len);
			pNormalErr->line = (unsigned short)line;				
			break;
	}	
	fwrite(buf,sizeof(unsigned char),ONE_LOG_SIZE,pFd);
	fclose(pFd);
	bLogLock = 0;
    system("sync;");
}


