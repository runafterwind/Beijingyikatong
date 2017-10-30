#ifndef _Resumedownload_H
#define _Resumedownload_H

#if 0
#include "apparel.h"
/*******��ӡ���Կ���*******/
#define UPDEG       0                   

/*******ʹ���豸����*******/

#define L9  1              //��������L9��Ʒ������L11��Ʒ��Ϊ0
#ifdef SAVE_CONSUM_DATA_DIRECT
#define GONGJIAO    1      //�������ڹ�������ʱΪ1������Ϊ0
#else
#define GONGJIAO    0      //�������ڹ�������ʱΪ1������Ϊ0
#endif

/*******�ļ�����*******/
#if L9
#if GONGJIAO
//��ʱ�����ļ�
//#define TEMPFILE         "/mnt/record/tempupfile.bin"
#define TEMPFILE         "/var/run/tempupfile.bin"

//�ļ����ع��������ļ�
#define PROCESSFILE      "/mnt/record/upprocess.sys"
//ִ���ļ�
#define EXFILE           "/mnt/record/upfile.tar.gz"
#else
//��ʱ�����ļ�
#define TEMPFILE         "/var/run/tempupfile.bin"
//�ļ����ع��������ļ�
#define PROCESSFILE      "/var/run/upprocess.sys"
//ִ���ļ�
#define EXFILE           "/var/run/upfile.tar.gz"
#endif
#else
//��ʱ�����ļ�
#define TEMPFILE         "/opt/minigui/app/tempupfile.bin"
//�ļ����ع��������ļ�
#define PROCESSFILE      "/opt/minigui/app/upprocess.sys"
//ִ���ļ�
#define EXFILE           "/opt/minigui/app/upfile.tar.gz"
#endif

/*******��������*******/

extern unsigned char Netstatus;           //��ǰ����״̬

//�ļ���Ϣ�ṹ��
struct Upfile
{
    unsigned char  filenamelen;
    unsigned char  upfilename[30];        //�ļ���
    unsigned int   upfilelen;             //�ļ���С
    unsigned short crc;                   //�ļ�У��ֵ
    unsigned int offset;                //����ƫ����  
};


/*******�����*******/
#ifdef BS
#define  FILEHEAD           "BPD:"
#else
#define  FILEHEAD           "SYS:"
#endif
#define  COMMAND_DOWN       0xd2          //����
#define  COMMAND_STOP       0xd3          //��ֹ
#define  COMMAND_REQ        0xd4          //����
#define  COMMAND_CHECK      0xd5          //��ѯ
#define  COMMAND_EXECUTE    0xd6          //ִ��
#define  COMMAND_RECOVER    0xd7          //�ָ�


/*******��������*******/
/****************************************
- �������� : void InitEnv()
- ����˵�� : ��ʼ���ϵ���������
- �������� : ��ȡ�Ѵ����ڻ����е����أ�����ļ��������򴴽�һ��
- ������� : ��
- ������� : ��
****************************************/
extern void InitEnv();

/****************************************
- �������� : int DownloadProcess(unsigned char *data,unsigned char len);
- ����˵�� : �����������ļ���Ϣ
- �������� : ���շ��������ع����ģ�������Ҫ�����ļ�����Ϣ(�ļ�������С��У��ֵ)
- �������� : �������
             0:����ɹ�
             -1:����ʧ��
- ������� : 
            *data--�ļ������Ϣ
            len -- �ļ������Ϣ����
- ������� : ��
****************************************/
extern int DownloadProcess(unsigned char *data,unsigned char len);

/****************************************
- �������� : int StopProcess()
- ����˵�� : ֹͣ����
- �������� : �������·�ֹͣ�ļ���������
- �������� : �������
             0:����ɹ�
             -1:����ʧ��
- ������� : ��
- ������� : ��
****************************************/
extern int  StopProcess();

/****************************************
- �������� : int ReqProcess()
- ����˵�� : ���̲�ѯ
- �������� : �������·���ѯ��ǰ�������ļ�����Ϣ
- �������� : ��ѯ����״̬
             0:�ļ��������
             1:�ļ�����ʧ��
             2:�ļ�������
- ������� : 
             *filename:��ѯ���ص��ļ���
             filenamelen:��ѯ�ļ����ĳ���
- ������� : 
             *reson:����ʧ��ԭ��
             *downloadper:�����صİٷֱ�
******************************************/
extern int ReqProcess(unsigned char *reson,unsigned char *filename,unsigned char filenamelen,unsigned short *downloadper);

/****************************************
- �������� : int ExeProcess()
- ����˵�� : �ļ�ִ��
- �������� : �������·�ִ���ļ�������
- �������� : �������
             0:����ɹ�
             -1:����ʧ��
- ������� : ��
- ������� : ��
******************************************/
extern int ExeProcess();


/****************************************
- �������� : int RecoveryProcess()
- ����˵�� : �ָ�ԭ�ļ�
- �������� : �������·��ָ���һ���汾��������
- �������� : �������
             0:����ɹ�
             -1:����ʧ��
- ������� : ��
- ������� : ��
******************************************/
extern int RecoverProcess();



/****************************************
- �������� : unsigned int LenDataCmd1(unsigned char *DataLen) 
- ����˵�� : ���㷢�����ݳ���
- �������� : ͨ����ʽ��������յ����ݵĳ���
- �������� : ���յ������ݳ���
- ������� : 
            *DateLen:���յ����ݵĳ���
            
- ������� : ��
******************************************/

extern unsigned int LenDataCmd1(unsigned char *DataLen);



/****************************************
- �������� : *DownloadFile_pthread(void *args);
- ����˵�� : �ļ������̺߳���
- �������� : �豸�����󣬿����߳�׼���ļ�����
- �������� : ��
- ������� : ��
- ������� : ��
******************************************/
extern void *DownloadFile_pthread(void *args);

/****************************************
- �������� : void close_pthread();
- ����˵�� : �ر��߳�
- �������� : �ر��ļ������߳�
- �������� : ��
- ������� : ��
- ������� : ��
******************************************/
extern void close_pthread();

#endif

#endif


