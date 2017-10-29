#ifndef  _CPUCARD_H_
#define  _CPUCARD_H_

#include "InitSystem.h"


/***��Ƭ���ݽṹ***/
typedef struct
{
	unsigned char IsLocalCard;		    //�Ƿ񱾵ؿ�            1-���ؿ� 2-��ؿ�
	unsigned char CSN[4];			    //������
	unsigned char beforemoney[4];	    //����ǰ���
	unsigned char thismoney[4];		    //���ν��׽��
	unsigned char aftermoney[4];		//���׺����
	unsigned char offlineSN[2];		    //�ѻ��������
	unsigned char overdraftAmount[3];	//͸֧�޶�
	unsigned char keyVersion;		    //��Կ�汾��
	unsigned char arithmeticLabel;	    //�㷨��ʶ
	unsigned char PRandom[4];		    //α�����
	unsigned char TAC[4];			    //TAC
	unsigned char MAC2[4];			    //MAC2
	unsigned char DESCRY[4];
	//�ֶο�����Ϣ
	unsigned char enterexitflag;		//���³���־
	//PSAM������
    unsigned char PSAMOfflineSN[4];	    //�ն��ѻ��������
	unsigned char MAC1[4];			    //MAC1
	unsigned char PSAMRandom[8];        //PSAM�������
	//0015�ļ�
	unsigned char issuerlabel[8];		//����������ʶ
	unsigned char apptypelabel;		    //Ӧ�����ͱ�ʶ          (01-ֻ��ED,02-ֻ��EP,03-ED��EP����)
	unsigned char issuerappversion;	    //��������Ӧ�ð汾
	unsigned char appserialnumber[10];	//Ӧ�����к�
	unsigned char appstarttime[4];		//Ӧ����������
	unsigned char appendtime[4];		//Ӧ�ý�ֹ����
	//0017�ļ�
	unsigned char countrycode[4];		//���Ҵ���
	unsigned char provincecode[2];	    //ʡ������
	unsigned char citycode[2];		    //���д���
	unsigned char unioncardtype[2];	    //��ͨ����              0000-�ǻ�ͨ����0001-��ͨ��
	unsigned char cardtype;			    //������
	unsigned char settlenumber[4];	    //���㵥Ԫ���
	//0018�ļ�
	unsigned char tradenumber[2];	    //ED/EP�������ѻ��������
	unsigned char overdraftlimit[3];	//͸֧�޶�
	unsigned char trademoney[4];		//���׽��
	unsigned char tradetype;			//��������
	unsigned char deviceNO[6];		    //�ն˻����
	unsigned char tradedate[4];		    //��������
	unsigned char tradetime[3];		    //����ʱ��
	
	//001A��¼�ļ�
	unsigned char applockflag;			//Ӧ��������־          0-Ӧ��û����1-Ӧ������
	unsigned char tradeserialnumber[8];	//������ˮ��
	unsigned char tradestate;			//����״̬              0-��ʼֵ��1-�ϳ���2-�³�
	unsigned char getoncitycode[2];		//�ϳ����д���
	unsigned char getonissuerlabel[8];	//�ϳ�������ʶ
	unsigned char getonoperatorcode[2];	//�ϳ���Ӫ�̴���
	unsigned char getonline[2];			//�ϳ���·��
	unsigned char getonstation;			//�ϳ�վ��
	unsigned char getonbus[8];			//�ϳ�������ASCII
	unsigned char getondevice[8];		//�ϳ��ն˱��BCD
	unsigned char getontime[7];			//�ϳ�ʱ��
	unsigned char markamount[4];		//��ע���,������Ʊ׷��
	unsigned char directionflag;		//�����ʶ              AB-���У�BA-����
	unsigned char getoffcitycode[2];	//�³����д���
	unsigned char getoffissuerlabel[8];	//�³�������ʶ
	unsigned char getoffoperatorcode[2];//�³���Ӫ�̴���
	unsigned char getoffline[2];		//�³���·��
	unsigned char getoffstation;		//�³�վ��
	unsigned char getoffbus[8];			//�³�������ASCII
	unsigned char getoffdevice[8];		//�³��ն˱��BCD
	unsigned char getofftime[7];		//�³�ʱ��
	unsigned char tradeamount[4];		//���׽��
	unsigned char ridedistance[2];		//�˳����
	//001E��¼�ļ�
	unsigned char tradetype1E;			//��������
	unsigned char deviceNO1E[8];		//�ն˱��
	unsigned char industrycode1E;		//��ҵ���� 01-����
	unsigned char line1E[2];			//��·
	unsigned char station1E[2]; 		//վ��
	unsigned char operatorcode[2];		//��Ӫ����
	unsigned char trademoney1E[4];		//���׽��
	unsigned char tradeaftermoney1E[4]; //���׺����
	unsigned char tradetime1E[7];		//��������ʱ��
	unsigned char acceptorcitycode[2];	//�������д���
	unsigned char acceptorissuerlabel[8];	//����������ʶ
    //��ͨ��˽��AID��0002�ļ�01��¼
    unsigned char logiccardtype;        //�߼�������
    unsigned char littlecardtype;       //С������
    //��ͨ��˽��AID��0002�ļ�06��¼
    unsigned char quickpassflag;        //���֧����־          01Ϊ����
    unsigned char qpstarttime[4];       //���֧����������
    unsigned char qpendtime[4];         //���֧��ʧЧ����
    //���ڿ��֧���ı���
    unsigned char cardsed[4];           //��Ƭ�ڲ�����Կ
    unsigned char samsed[4];            //psam���ڲ���֤��Կ
    unsigned char random[8];            //�豸�����
    unsigned char cardrandom[4];        //�û��������
    //ס�������ؿ�0004�ļ�
    unsigned char zappserialnumber[8];  //����ʱ���ɵ����к�
    unsigned char zcardtype;            //��Ƭ����
    unsigned char zcardstatus;          //��״̬
    unsigned char zreleasever[2];       //��Ƭ���а汾
    unsigned char zstarttime[4];        //��Ƭ��������
    unsigned char zendtime[4];          //��ƬʧЧ����
    //ס�������ؿ�0005�ļ�(�û���)
    unsigned char ydata0005[18];        //�û���0005�ļ���ǰʹ�õ����ݣ����ڸ������³�����
    unsigned char yblackflag;           //��������־
    unsigned char ycardtransnum[4];     //�û������״���
    unsigned char ytradestatus;         //�û������³���־     �ϳ�:0x20  �³�:0x30
    unsigned char ytradetime[5];        //�û�����������ʱ��   ������ʱ��
    unsigned char yoperatercode[3];     //���ν�����Ӫ�̴���   ȡSAM����5λ XXXXX+0��
    //ס�������ؿ�0005�ļ�(����-���ʿ�)
    unsigned char gblackflag;           //��������־
    unsigned char gcardtype;            //��������            ���ʿ�:0x08
    unsigned char glinenum[2];          //��·���
    unsigned char gupstationnum;        //������վ��
    unsigned char gdownstationnum;      //������վ��
    unsigned char greleasever[2];       //���ʰ汾
    unsigned char glineattr;            //��·����              �ǻ�һƱ��:0x00 �ǻ��ֶ�:0x01 ����һƱ��:0x02 ���ͷֶ�:0x03
    unsigned char glocalnodefinecard;   //���ؿ�δ����Ŀ��ദ��ģʽ   ���������ʱ���δ����Ŀ���:0x00  ��1�࿨�ۿ�ִ��:0x01 ��1�࿨���ۿ�ִ��:0x02  �Ƿ�:����ֵ
    unsigned char gremotnodefinecard;   //��ؿ�δ����Ŀ��ദ��ģʽ   ���������ʱ���δ����Ŀ���:0x00  ��1�࿨�ۿ�ִ��:0x01 ��1�࿨���ۿ�ִ��:0x02  �Ƿ�:����ֵ
    unsigned char gbasicpice[2];        //����Ʊ��              ��վ�ϱ�վ�µĿۿ���
    unsigned char goperatornum[2];      //����ԱID              BCD��ʽ
    unsigned char gvehicleattr;         //��������              �յ���:0x01  �ǿյ���:0x02
    unsigned char gyidibupiaomo;        //�����Ʊ��ģģʽ      �Զ�����ͨ�������Ʊ:0x01  ��ͨ�������Ʊʱ��ֹʹ��:0x02  ���ж������Ʊ:0x03
    unsigned char glinefilesize[2];     //��·�ļ���С
    unsigned char gbupiaolimittime[2];  //��Ʊ�綨ʱ��
    unsigned char gruleofupanddowm;     //���³�����ͬ�Ĳ�Ʊ����   ��Ʊ:0x01  ����Ʊ:0x02,0x03
    unsigned char gbubiaodiscountrateflag[2];   //��Ʊ����ۿ۱�־   ����·����ǰ���ʼ����ע���:0x0008  ����·���ۺ���ʼƳ����ע���
    unsigned char gfuncflag[2];         //���ܿ��� λ0 ���֧����:1  ���֧����:0 
    //ס�����û���0014�ļ�
    unsigned char ygetontime[5];        //�ϳ�ʱ��              BCD�� ������ʱ��
    unsigned char ygetonstation;        //��¼�ϳ�վ��վ��
    unsigned char ydirectionflag;       //�ϳ�ʱ�������з���    AB-���У�BA-����
    unsigned char ymarkamount[2];       //��ע���              ��ǰ�ϳ�վ���յ�վ��Ʊ�� 
    unsigned char ygetonlinenum[2];     //�ϳ���·��
    unsigned char ygetonvehiclnum[3];   //�ϳ�������            BCD��
    }CardInformCPU;

//���ʿ�0005	
typedef struct{
	unsigned char gblackflag;           //��������־
    unsigned char gcardtype;            //��������            ���ʿ�:0x08
    unsigned char Operationunit[2];	    //��Ӫ��λ��ʶ
    unsigned char Operationcode[4];     //��Ӫ��λ����
    unsigned char glinenum[2];          //��·���
    unsigned char gupstationnum;        //������վ��
    unsigned char gdownstationnum;      //������վ��
    unsigned char greleasever[2];       //���ʰ汾
    unsigned char glineattr;            //��·����              �ǻ�һƱ��:0x00 �ǻ��ֶ�:0x01 ����һƱ��:0x02 ���ͷֶ�:0x03
    unsigned char glocalnodefinecard;   //���ؿ�δ����Ŀ��ദ��ģʽ   ���������ʱ���δ����Ŀ���:0x00  ��1�࿨�ۿ�ִ��:0x01 ��1�࿨���ۿ�ִ��:0x02  �Ƿ�:����ֵ
    unsigned char gremotnodefinecard;   //��ؿ�δ����Ŀ��ദ��ģʽ   ���������ʱ���δ����Ŀ���:0x00  ��1�࿨�ۿ�ִ��:0x01 ��1�࿨���ۿ�ִ��:0x02  �Ƿ�:����ֵ
    unsigned char gbasicpice[2];        //����Ʊ��              ��վ�ϱ�վ�µĿۿ���
    unsigned char goperatornum[2];      //����ԱID              BCD��ʽ
    unsigned char gvehicleattr;         //��������              �յ���:0x01  �ǿյ���:0x02
    unsigned char gyidibupiaomo;        //�����Ʊ��ģģʽ      �Զ�����ͨ�������Ʊ:0x01  ��ͨ�������Ʊʱ��ֹʹ��:0x02  ���ж������Ʊ:0x03
    unsigned char glinefilesize[2];     //��·�ļ���С
    unsigned char gbupiaolimittime[2];  //��Ʊ�綨ʱ��
    unsigned char gruleofupanddowm;     //���³�����ͬ�Ĳ�Ʊ����   ��Ʊ:0x01  ����Ʊ:0x02,0x03
    unsigned char gbubiaodiscountrateflag[2];   //��Ʊ����ۿ۱�־   ����·����ǰ���ʼ����ע���:0x0008  ����·���ۺ���ʼ����ע���
    unsigned char gfuncflag[2];         //���ܿ��� λ0 ���֧����:1  ���֧����:0 	
	unsigned char none[16];			    //Ԥ��		
}FLC0005;

extern FLC0005 flc0005;

//���ʿ�0006�ļ�
//���ؿ���¼��ṹ
typedef struct {
    unsigned char phycardtype;          //��Ƭ��������
    unsigned char logiccardtype;        //��Ƭ�߼�������        13�ֿ�
    unsigned char cardattr;             //��Ƭ����   0x01:��ͨ��ֵ��    0x02:�ƴο�     0x03:���ڿ�   0x04:���ⴢֵ��
    unsigned char consumemode;          //�շ�ģʽ   ��4λʹ��ģʽ  ��4λ��ʾ��ʾģʽ
    ShortUnon 		minblancelimit;    //��С�������  �������С�ڴ˽�������  ��λ�� 2HEX
    ShortUnon 		overdraw;          //���͸֧�޶�  ��λ��  2HEX
    LongUnon 		maxblancelimit;    //����������  ���������ڴ˽�������  ��λ��   4hex
    ShortUnon 		maxdebit;          //���ۿ���  2hex
    unsigned char   indiscontrate;        //�����Ż���
    ShortUnon 		indiscontlimit;    //�����Żݶ�� 2hex
    unsigned char   outdiscontrate;       //�����Ż���
    ShortUnon	        outdiscontlimit;   //�����Żݶ�� 2hex
    unsigned char   none[28];             //Ԥ��
}CardRate_local;


extern CardRate_local LocalCardRate[];

typedef struct{
    unsigned char version;              //���汾  0x07
    unsigned char totolnum;             //������
    unsigned char packsn;               //�����  �����ݰ��ڱ����Ͱ��е�˳���
    unsigned char recordsum[2];         //���м�¼����
    unsigned char precordlen[2];        //������¼����
    unsigned char recordstartadd[2];    //���м�¼��ʼλ��   ����ļ���ʼλ�õ�ƫ��
    unsigned char creatdate[4];         //�������� BCD��ʽ
    unsigned char unitcode[4];          //���շ���λ����
    unsigned char parever[4];           //�����汾��
    unsigned char efficetime[7];        //��Чʱ��
    unsigned char none[2];              //Ԥ��
    unsigned char crc16[2];             //CRC16��У����   
}FLC0006;

extern FLC0006 flc0006;

//���ʿ�0007�ļ�
//��ͨ���������ṹ
typedef struct{
    unsigned char version;              //���汾  0x07
    unsigned char totolnum;             //������
    unsigned char packsn;               //�����  �����ݰ��ڱ����Ͱ��е�˳���
    unsigned char recordsum[2];         //���м�¼����
    unsigned char precordlen[2];        //������¼����
    unsigned char recordstartadd[2];    //���м�¼��ʼλ��   ����ļ���ʼλ�õ�ƫ��
    unsigned char creatdate[4];         //�������� BCD��ʽ
    unsigned char unitcode[4];          //���շ���λ����
    unsigned char parever[4];           //�����汾��
    unsigned char efficetime[7];        //��Чʱ��
    unsigned char none[2];              //Ԥ��
    unsigned char crc16[2];             //CRC16��У����   
}FLC0007;

extern FLC0007 flc0007;


//���ʿ�0008�ļ�
//��ͨ����¼��ṹ
typedef struct {
    unsigned char cardissuerlabel[8];   //����������ʶ
    unsigned char logiccardtype;        //��Ƭ�߼�������        13�ֿ�   
    unsigned char consumemode;          //�շ�ģʽ   ��4λʹ��ģʽ  ��4λ��ʾ��ʾģʽ    
    unsigned char overdraw[2];          //���͸֧�޶�  ��λ��
    unsigned char maxblancelimit[4];    //����������  ���������ڴ˽�������  ��λ��
    unsigned char maxdebit[2];          //���ۿ���
    unsigned char indiscontrate;        //�����Ż���
    unsigned char indiscontlimit[2];    //�����Żݶ��
    unsigned char outdiscontrate;       //�����Ż���
    unsigned char outdiscontlimit[2];   //�����Żݶ��
    unsigned char none[24];             //Ԥ��
}CardRate_remot;

extern CardRate_remot remotcard[13];

typedef struct{
    unsigned char version;              //���汾  0x07
    unsigned char totolnum;             //������
    unsigned char packsn;               //�����  �����ݰ��ڱ����Ͱ��е�˳���
    unsigned char recordsum[2];         //���м�¼����
    unsigned char precordlen[2];        //������¼����
    unsigned char recordstartadd[2];    //���м�¼��ʼλ��   ����ļ���ʼλ�õ�ƫ��
    unsigned char creatdate[4];         //�������� BCD��ʽ
    unsigned char unitcode[4];          //���շ���λ����
    unsigned char parever[4];           //�����汾��
    unsigned char efficetime[7];        //��Чʱ��
    unsigned char none[2];              //Ԥ��
    unsigned char crc16[2];             //CRC16��У����   
}FLC0008;

extern FLC0008 flc0008;


typedef struct{
    unsigned char Tag1;                  //�н����б�  0xA3
    unsigned char Len1;                  //�б���
    unsigned char Value1[6];             //����
    unsigned char crc16[2];              //CRC16��У����   
    unsigned char Tag2;                  //�н����б�  0xA1
    unsigned char Len2;                  //�б���  
}FLC0009;

extern FLC0009 flc0009;


typedef struct {
	unsigned char uprecord;				//���м�¼����
	unsigned char uprecordnun[2];		//���м�¼����
	unsigned char downrecord;			//���м�¼����
	unsigned char downrecordnum[2];     //���м�¼����
	unsigned char m4ver[2];             //�ļ��汾
}FileM4;

extern FileM4 filem4;


typedef struct {
	unsigned char localrate;			//�������ʵ�����¼����
	unsigned char localratenum[2];		//�������ʼ�¼����
	unsigned char remotrate;			//��ͨ���ʵ�����¼����
	unsigned char remotratenum[2];     //��ͨ��¼����
	unsigned char m5ver[2];             //�ļ��汾
}FileM5;

extern FileM4 filem5;

//���ص�MP���ʿ����η�����Ϣ�ļ�
typedef struct{
	unsigned char linenum[2];			// ��·��	
	unsigned char linename[16];			//��·����
	unsigned char corpflag[2];			//��Ӫ��λ��ʾ
	unsigned char corpenterprisename[40];  //������ҵ����
	unsigned char corpcode[4];			//�����ֹ�˾����		
	unsigned char corpcompanyname[40];	//�ֹ�˾����
	unsigned char lineattr;				//��·����
	ShortUnon uppricesitemnum;			//����վ������
	ShortUnon downpricesitemnum;		//����վ������
	unsigned char pricever[2];			//��ǰ��·�ļ���ʽ�汾��
	ShortUnon tickettransfer;			//����ʱ��
	ShortUnon defaultbaseprice;			//����Ʊ��
	unsigned char upstationmid;			//�����н����
	unsigned char downstationmid;		//�����н����
	unsigned char modebj;				//����δ���忨���ʹ���ģʽ
	unsigned char modehl;				//���δ���忨���ʹ���ģʽ
	unsigned char vehicleattr;			//��������
	unsigned char modehlbp;			//������ܲ�Ʊģʽ
	unsigned char onoffdir;				//���³�����ͬ��Ʊ����
	unsigned char pricediscount;		//��Ʊ����Ƿ���۱�ʶ
	unsigned char funcflag[2];			//���ܿ���
	unsigned char filever[2];			//���ļ��汾
	
}FileMP;

extern FileMP filemp;


/*
m5 �ļ�
*/
typedef struct {
    unsigned char phycardtype;          //��Ƭ��������
    unsigned char logiccardtype;        //��Ƭ�߼�������        13�ֿ�
    unsigned char cardattr;             //��Ƭ����   0x01:��ͨ��ֵ��    0x02:�ƴο�     0x03:���ڿ�   0x04:���ⴢֵ��
    unsigned char consumemode;          //�շ�ģʽ   ��4λʹ��ģʽ  ��4λ��ʾ��ʾģʽ
    ShortUnon 		minblancelimit;    //��С�������  �������С�ڴ˽�������  ��λ�� 2HEX
    ShortUnon 		overdraw;          //���͸֧�޶�  ��λ��  2HEX
    LongUnon 		maxblancelimit;    //����������  ���������ڴ˽�������  ��λ��   4hex
    ShortUnon 		maxdebit;          //���ۿ���  2hex
    unsigned char   indiscontrate;        //�����Ż���
    ShortUnon 		indiscontlimit;    //�����Żݶ�� 2hex
    unsigned char   outdiscontrate;       //�����Ż���
    ShortUnon	        outdiscontlimit;   //�����Żݶ�� 2hex       //Ԥ��
}FileM5_T;
extern FileM5_T CardConParam[CARD_NUMBER];





typedef  struct{
	LongUnon QCash;
	LongUnon Subsidies;
	LongUnon overdraw;
}CpuMoney;


typedef struct {
	unsigned char Type; //1:DDF,2:ADF
	unsigned char DFNameExist;//0-non exist;1-exist.
	unsigned char DFNameLen;
	unsigned char DFName[16]; //5-16,ADF or DDF name according to Type.
	unsigned char AppLabelExist;
	unsigned char AppLabelLen;
	unsigned char AppLabel[16];
	unsigned char PreferNameExist;
	unsigned char PreferNameLen;
	unsigned char PreferName[16];
	unsigned char PriorityExist;
	unsigned char Priority; //tag'87'
	unsigned char DirDiscretExist;
	unsigned char DirDiscretLen;
	unsigned char DirDiscret[222];
}RECORD_PPSE;

extern unsigned char ReadCardInfor_CPU(void);
extern unsigned char ReadorRepairCard_CPU(void);
extern unsigned char FreeReadorRepairCard_CPU(void);


#endif

