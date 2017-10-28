#ifndef __ELETRIC__H__
#define __ELETRIC__H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>

#include <math.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>

#include "stades.h"

#define ELETRON_HEAD_LEN	8
#define ELETRON_MESG_LEN	40


typedef enum ELETRON_ERROCODE_{

		INVALID_TIME_ERR=-9,			//��Ч���ѹ�
		VERIFY_ERR= -8,					//������֤�����
		OVERUN_ERR = -7,				//���ѳ���
		CARD_TYPE_ERR = -6,				//�������
		CARD_NO_ERR	=-5,				//���Ŵ���
		DATA_LEN_ERR=-4,				//���ݳ��ȴ���
		KEY_ID_ERR  =-3,				//��Կidc����
		CODE_VER_ERR=-2,				//��汾����
		MESG_LEN_ERR=-1,				//���ӿ���Ϣ���ȴ���
		MESG_OK     =0
}ELETRON_ERROCODE;




typedef struct eletro_head_{
	unsigned char ver;
	unsigned char keyid;
	unsigned char lenth[2];
	unsigned char res[4];
}eletro_head;


typedef struct eletro_body_{
	unsigned char cardno[10];
	unsigned char verify[2];
	unsigned char cardtype;
	unsigned char invalidtime[6];
	unsigned char offlinelimit[2];
	unsigned char Authentication[4];
	unsigned char ID[4];
	unsigned char res[3];
}eletro_body;

typedef struct eletron_info_{
	/*head part*/
	eletro_head  head;
	/*data body*/
	eletro_body body;
}eletron_info;



extern unsigned char PsamCos(char *Intdata, char *Outdata,unsigned char *len);






#endif
