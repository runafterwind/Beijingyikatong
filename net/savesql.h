#ifndef SAVESQL_H
#define SAVESQL_H

#if 0
//公用体
typedef union
{
	unsigned char intbuf[2];
	unsigned short i;
} ShortUnon;

typedef union
{
	unsigned char longbuf[4];
	unsigned int  i;
} LongUnon;
#endif

//记录文件格式
//本记录格式支持A2、A5、A6(实体卡与二维码电子卡)刷卡数据
typedef struct
{
    unsigned char RType;            //0       交易类型
    unsigned char RSannum[6];       // -6     SAM卡号
    unsigned char RValuej[4];       //7-10    交易金额
    unsigned char RTradenum[4];     //11-14   交易流水号
    unsigned char RValuey[4];       //15-18   卡内余额
    unsigned char RTime[7];         //19-25   交易日期
    unsigned char RCsn[6];          //26-31   卡片CSN(实体卡号)
    unsigned char RSeq[4];          //32-35   卡片序列号
    unsigned char RCardcount[2];    //36-37   卡累计交易次数
    unsigned char RCardtype;        //38      卡类型
    unsigned char RCardattr;        //39      卡物理类型
    unsigned char RAuthcode[4];     //40-43   交易认证码
    unsigned char RCardid0[2];      //44-45   BCD格式
    unsigned char RCityid[2];       //46-47   城市号
    unsigned char RUnionid[2];      //48-49   行业号
    unsigned char RCardissueid[4];  //50-53   卡发行号
    unsigned char RPrebanlance[4];  //54-57   卡交易前余额
    unsigned char RSerialnum[4];    //58-61   记录流水号
    unsigned char RTradestatus;     //62      补票参考交易状态
    unsigned char RTacflag;         //63      TAC算法标记
    unsigned char RTradetypeflag;   //64      交易类型标记
    unsigned char RCpucounter[2];   //65-66   电子钱包脱机交易序号
    unsigned char RTradeserpsam[4]; //67-70   PSAM交易流水号
    unsigned char RKeyver;          //71      密钥版本
    unsigned char RKeyindex;        //72      密钥索引
    unsigned char RIssuerid[8];     //73-80   发卡方机构代码
    unsigned char RTimecardtype;    //81      时间票类型
    unsigned char RChargeideal[4];  //82-85   应收金额
    unsigned char RMarktime[7];     //86-92   上车时间
    unsigned char RLastcityid[2];   //93-94   上车城市代码
    unsigned char RUpjigouid[8];    //95-102  上车受理机构代码
    unsigned char RUpcorpid[2];     //103-104 上车运营商代码
    unsigned char RMarklineid[2];   //105-106 上车线路号
    unsigned char RMarkstation;     //107     上车站点号
    unsigned char RMarkvehicleid[4];//108-111 上车车辆号
    unsigned char RDowncorpid[2];   //112-113 下车运营商代码
    unsigned char RTradelineid[2];  //114-115 下车线路号
    unsigned char RTradestation;    //116     下车交易站号
    unsigned char RTradevehiclid[4];//117-120 下车车辆号
    unsigned char RLicheng[2];      //121-122 乘车里程
    unsigned char RDirflag;         //123     上下行标志
    unsigned char RDrivercode[4];   //124-127 司机卡ID
    unsigned char RSellorcode[4];   //128-131 监票员ID
    unsigned char RSellorcode1[4];  //132-135 监票员ID1
    unsigned char RRecordpricetype; //136     交易记录计费/次类型
    unsigned char RRecordcardtype;  //137     交易记录卡类型
    unsigned char RCheckcode[2];    //138-139 结构校验码
    
    }RecordFormat_new;

//数据补充说明
/*偏移位置       定义说明                               增写备注(实体卡为1，二维码为0)***   
    0         
*/
#endif

