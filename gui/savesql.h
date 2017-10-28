#ifndef SAVESQL_H
#define SAVESQL_H

//��¼�ļ���ʽ
//����¼��ʽ֧��A2��A5��A6(ʵ�忨���ά����ӿ�)ˢ������
typedef struct
{
    unsigned char RType;            //0       ��������
    unsigned char RSannum[6];       //1-6     SAM����
    unsigned char RValuej[4];       //7-10    ���׽��
    unsigned char RTradenum[4];     //11-14   ������ˮ��
    unsigned char RValuey[4];       //15-18   �������
    unsigned char RTime[7];         //19-25   ��������
    unsigned char RCsn[6];          //26-31   ��ƬCSN(ʵ�忨��)
    unsigned char RSeq[4];          //32-35   ��Ƭ���к�
    unsigned char RCardcount[2];    //36-37   ���ۼƽ��״���
    unsigned char RCardtype;        //38      ������
    unsigned char RCardattr;        //39      ����������
    unsigned char RAuthcode[4];     //40-43   ������֤��
    unsigned char RCardid0[2];      //44-45   BCD��ʽ
    unsigned char RCityid[2];       //46-47   ���к�
    unsigned char RUnionid[2];      //48-49   ��ҵ��
    unsigned char RCardissueid[4];  //50-53   �����к�
    unsigned char RPrebanlance[4];  //54-57   ������ǰ���
    unsigned char RSerialnum[4];    //58-61   ��¼��ˮ��
    unsigned char RTradestatus;     //62      ��Ʊ�ο�����״̬
    unsigned char RTacflag;         //63      TAC�㷨���
    unsigned char RTradetypeflag;   //64      �������ͱ��
    unsigned char RCpucounter[2];   //65-66   ����Ǯ���ѻ��������
    unsigned char RTradeserpsam[4]; //67-70   PSAM������ˮ��
    unsigned char RKeyver;          //71      ��Կ�汾
    unsigned char RKeyindex;        //72      ��Կ����
    unsigned char RIssuerid[8];     //73-80   ��������������
    unsigned char RTimecardtype;    //81      ʱ��Ʊ����
    unsigned char RChargeideal[4];  //82-85   Ӧ�ս��
    unsigned char RMarktime[7];     //86-92   �ϳ�ʱ��
    unsigned char RLastcityid[2];   //93-94   �ϳ����д���
    unsigned char RUpjigouid[8];    //95-102  �ϳ������������
    unsigned char RUpcorpid[2];     //103-104 �ϳ���Ӫ�̴���
    unsigned char RMarklineid[2];   //105-106 �ϳ���·��
    unsigned char RMarkstation;     //107     �ϳ�վ���
    unsigned char RMarkvehicleid[4];//108-111 �ϳ�������
    unsigned char RDowncorpid[2];   //112-113 �³���Ӫ�̴���
    unsigned char RTradelineid[2];  //114-115 �³���·��
    unsigned char RTradestation;    //116     �³�����վ��
    unsigned char RTradevehiclid[4];//117-120 �³�������
    unsigned char RLicheng[2];      //121-122 �˳����
    unsigned char RDirflag;         //123     �����б�־
    unsigned char RDrivercode[4];   //124-127 ˾����ID
    unsigned char RSellorcode[4];   //128-131 ��ƱԱID
    unsigned char RSellorcode1[4];  //132-135 ��ƱԱID1
    unsigned char RRecordpricetype; //136     ���׼�¼�Ʒ�/������
    unsigned char RRecordcardtype;  //137     ���׼�¼������
    unsigned char RCheckcode[2];    //138-139 �ṹУ����
    
    }RecordFormat_new;

extern RecordFormat_new SaveData_new;

//һ�����ݰ��ĳ���(����+���ÿռ�)
#define	SAVE_DATA_LEN	140
//���ÿռ�
#define	SAVE_DATA_RESERVE	8








//���ݲ���˵��
/*ƫ��λ��       ����˵��                               ��д��ע(ʵ�忨Ϊ1����ά��Ϊ0)***   
    0         
*/
#endif