#ifndef  _CPUCARD_H_
#define  _CPUCARD_H_

#include "InitSystem.h"


/***卡片数据结构***/
typedef struct
{
	unsigned char IsLocalCard;		    //是否本地卡            1-本地卡 2-外地卡
	unsigned char CSN[4];			    //物理卡号
	unsigned char beforemoney[4];	    //交易前余额
	unsigned char thismoney[4];		    //本次交易金额
	unsigned char aftermoney[4];		//交易后余额
	unsigned char offlineSN[2];		    //脱机交易序号
	unsigned char overdraftAmount[3];	//透支限额
	unsigned char keyVersion;		    //密钥版本号
	unsigned char arithmeticLabel;	    //算法标识
	unsigned char PRandom[4];		    //伪随机数
	unsigned char TAC[4];			    //TAC
	unsigned char MAC2[4];			    //MAC2
	unsigned char DESCRY[4];
	//分段控制信息
	unsigned char enterexitflag;		//上下车标志
	//PSAM卡数据
    unsigned char PSAMOfflineSN[4];	    //终端脱机交易序号
	unsigned char MAC1[4];			    //MAC1
	unsigned char PSAMRandom[8];        //PSAM卡随机数
	//0015文件
	unsigned char issuerlabel[8];		//发卡机构标识
	unsigned char apptypelabel;		    //应用类型标识          (01-只有ED,02-只有EP,03-ED和EP都有)
	unsigned char issuerappversion;	    //发卡机构应用版本
	unsigned char appserialnumber[10];	//应用序列号
	unsigned char appstarttime[4];		//应用启用日期
	unsigned char appendtime[4];		//应用截止日期
	//0017文件
	unsigned char countrycode[4];		//国家代码
	unsigned char provincecode[2];	    //省级代码
	unsigned char citycode[2];		    //城市代码
	unsigned char unioncardtype[2];	    //互通卡种              0000-非互通卡，0001-互通卡
	unsigned char cardtype;			    //卡类型
	unsigned char settlenumber[4];	    //结算单元编号
	//0018文件
	unsigned char tradenumber[2];	    //ED/EP联机或脱机交易序号
	unsigned char overdraftlimit[3];	//透支限额
	unsigned char trademoney[4];		//交易金额
	unsigned char tradetype;			//交易类型
	unsigned char deviceNO[6];		    //终端机编号
	unsigned char tradedate[4];		    //交易日期
	unsigned char tradetime[3];		    //交易时间
	
	//001A记录文件
	unsigned char applockflag;			//应用锁定标志          0-应用没锁，1-应用已锁
	unsigned char tradeserialnumber[8];	//交易流水号
	unsigned char tradestate;			//交易状态              0-初始值，1-上车，2-下车
	unsigned char getoncitycode[2];		//上车城市代码
	unsigned char getonissuerlabel[8];	//上车机构标识
	unsigned char getonoperatorcode[2];	//上车运营商代码
	unsigned char getonline[2];			//上车线路号
	unsigned char getonstation;			//上车站点
	unsigned char getonbus[8];			//上车车辆号ASCII
	unsigned char getondevice[8];		//上车终端编号BCD
	unsigned char getontime[7];			//上车时间
	unsigned char markamount[4];		//标注金额,用于逃票追缴
	unsigned char directionflag;		//方向标识              AB-上行，BA-下行
	unsigned char getoffcitycode[2];	//下车城市代码
	unsigned char getoffissuerlabel[8];	//下车机构标识
	unsigned char getoffoperatorcode[2];//下车运营商代码
	unsigned char getoffline[2];		//下车线路号
	unsigned char getoffstation;		//下车站点
	unsigned char getoffbus[8];			//下车车辆号ASCII
	unsigned char getoffdevice[8];		//下车终端编号BCD
	unsigned char getofftime[7];		//下车时间
	unsigned char tradeamount[4];		//交易金额
	unsigned char ridedistance[2];		//乘车里程
	//001E记录文件
	unsigned char tradetype1E;			//交易类型
	unsigned char deviceNO1E[8];		//终端编号
	unsigned char industrycode1E;		//行业代码 01-公交
	unsigned char line1E[2];			//线路
	unsigned char station1E[2]; 		//站点
	unsigned char operatorcode[2];		//运营代码
	unsigned char trademoney1E[4];		//交易金额
	unsigned char tradeaftermoney1E[4]; //交易后余额
	unsigned char tradetime1E[7];		//交易日期时间
	unsigned char acceptorcitycode[2];	//受理方城市代码
	unsigned char acceptorissuerlabel[8];	//受理方机构标识
    //互通卡私有AID下0002文件01记录
    unsigned char logiccardtype;        //逻辑卡类型
    unsigned char littlecardtype;       //小卡类型
    //互通卡私有AID下0002文件06记录
    unsigned char quickpassflag;        //快捷支付标志          01为启用
    unsigned char qpstarttime[4];       //快捷支付启用日期
    unsigned char qpendtime[4];         //快捷支付失效日期
    //用于快捷支付的变量
    unsigned char cardsed[4];           //卡片内部认密钥
    unsigned char samsed[4];            //psam卡内部认证密钥
    unsigned char random[8];            //设备随机数
    unsigned char cardrandom[4];        //用户卡随机数
    //住建部本地卡0004文件
    unsigned char zappserialnumber[8];  //发卡时生成的序列号
    unsigned char zcardtype;            //卡片类型
    unsigned char zcardstatus;          //卡状态
    unsigned char zreleasever[2];       //卡片发行版本
    unsigned char zstarttime[4];        //卡片发行日期
    unsigned char zendtime[4];          //卡片失效日期
    //住建部本地卡0005文件(用户卡)
    unsigned char ydata0005[18];        //用户卡0005文件当前使用的数据，用于更新上下车过程
    unsigned char yblackflag;           //黑名单标志
    unsigned char ycardtransnum[4];     //用户卡交易次数
    unsigned char ytradestatus;         //用户卡上下车标志     上车:0x20  下车:0x30
    unsigned char ytradetime[5];        //用户卡本交交易时间   年月日时分
    unsigned char yoperatercode[3];     //本次交易运营商代码   取SAM卡高5位 XXXXX+0；
    //住建部本地卡0005文件(管理卡-费率卡)
    unsigned char gblackflag;           //黑名单标志
    unsigned char gcardtype;            //管理卡类型            费率卡:0x08
    unsigned char glinenum[2];          //线路编号
    unsigned char gupstationnum;        //上行总站数
    unsigned char gdownstationnum;      //下行总站数
    unsigned char greleasever[2];       //费率版本
    unsigned char glineattr;            //线路属性              非环一票制:0x00 非环分段:0x01 环型一票制:0x02 环型分段:0x03
    unsigned char glocalnodefinecard;   //本地卡未定义的卡类处理模式   不处理本费率表中未定义的卡类:0x00  按1类卡折扣执行:0x01 按1类卡无折扣执行:0x02  非法:其他值
    unsigned char gremotnodefinecard;   //异地卡未定义的卡类处理模式   不处理本费率表中未定义的卡类:0x00  按1类卡折扣执行:0x01 按1类卡无折扣执行:0x02  非法:其他值
    unsigned char gbasicpice[2];        //基础票价              本站上本站下的扣款金额
    unsigned char goperatornum[2];      //操作员ID              BCD格式
    unsigned char gvehicleattr;         //车辆属性              空调车:0x01  非空调车:0x02
    unsigned char gyidibupiaomo;        //异地逃票补模模式      自动补互通卡异地逃票:0x01  互通卡异地逃票时禁止使用:0x02  不判定异地逃票:0x03
    unsigned char glinefilesize[2];     //线路文件大小
    unsigned char gbupiaolimittime[2];  //补票界定时间
    unsigned char gruleofupanddowm;     //上下车方向不同的补票规则   补票:0x01  不补票:0x02,0x03
    unsigned char gbubiaodiscountrateflag[2];   //补票金额折扣标志   该线路按折前费率计算标注金额:0x0008  该线路按折后费率计长虹标注金额
    unsigned char gfuncflag[2];         //功能开关 位0 快捷支付开:1  快捷支付关:0 
    //住建部用户卡0014文件
    unsigned char ygetontime[5];        //上车时间              BCD码 年月日时分
    unsigned char ygetonstation;        //记录上车站的站号
    unsigned char ydirectionflag;       //上车时车辆运行方向    AB-上行，BA-下行
    unsigned char ymarkamount[2];       //标注金额              当前上车站到终点站的票价 
    unsigned char ygetonlinenum[2];     //上车线路码
    unsigned char ygetonvehiclnum[3];   //上车车辆号            BCD码
    }CardInformCPU;

//费率卡0005	
typedef struct{
	unsigned char gblackflag;           //黑名单标志
    unsigned char gcardtype;            //管理卡类型            费率卡:0x08
    unsigned char Operationunit[2];	    //运营单位标识
    unsigned char Operationcode[4];     //运营单位代码
    unsigned char glinenum[2];          //线路编号
    unsigned char gupstationnum;        //上行总站数
    unsigned char gdownstationnum;      //下行总站数
    unsigned char greleasever[2];       //费率版本
    unsigned char glineattr;            //线路属性              非环一票制:0x00 非环分段:0x01 环型一票制:0x02 环型分段:0x03
    unsigned char glocalnodefinecard;   //本地卡未定义的卡类处理模式   不处理本费率表中未定义的卡类:0x00  按1类卡折扣执行:0x01 按1类卡无折扣执行:0x02  非法:其他值
    unsigned char gremotnodefinecard;   //异地卡未定义的卡类处理模式   不处理本费率表中未定义的卡类:0x00  按1类卡折扣执行:0x01 按1类卡无折扣执行:0x02  非法:其他值
    unsigned char gbasicpice[2];        //基础票价              本站上本站下的扣款金额
    unsigned char goperatornum[2];      //操作员ID              BCD格式
    unsigned char gvehicleattr;         //车辆属性              空调车:0x01  非空调车:0x02
    unsigned char gyidibupiaomo;        //异地逃票补模模式      自动补互通卡异地逃票:0x01  互通卡异地逃票时禁止使用:0x02  不判定异地逃票:0x03
    unsigned char glinefilesize[2];     //线路文件大小
    unsigned char gbupiaolimittime[2];  //补票界定时间
    unsigned char gruleofupanddowm;     //上下车方向不同的补票规则   补票:0x01  不补票:0x02,0x03
    unsigned char gbubiaodiscountrateflag[2];   //补票金额折扣标志   该线路按折前费率计算标注金额:0x0008  该线路按折后费率计算标注金额
    unsigned char gfuncflag[2];         //功能开关 位0 快捷支付开:1  快捷支付关:0 	
	unsigned char none[16];			    //预留		
}FLC0005;

extern FLC0005 flc0005;

//费率卡0006文件
//本地卡记录体结构
typedef struct {
    unsigned char phycardtype;          //卡片物理类型
    unsigned char logiccardtype;        //卡片逻辑卡类型        13种卡
    unsigned char cardattr;             //卡片属性   0x01:普通储值卡    0x02:计次卡     0x03:定期卡   0x04:特殊储值卡
    unsigned char consumemode;          //收费模式   高4位使用模式  低4位表示提示模式
    ShortUnon 		minblancelimit;    //最小余额限制  卡内余额小于此金额不允许交易  单位分 2HEX
    ShortUnon 		overdraw;          //最大透支限额  单位分  2HEX
    LongUnon 		maxblancelimit;    //最大余额限制  卡内余额大于此金额不允许交易  单位分   4hex
    ShortUnon 		maxdebit;          //最大扣款额度  2hex
    unsigned char   indiscontrate;        //界内优惠率
    ShortUnon 		indiscontlimit;    //界内优惠额度 2hex
    unsigned char   outdiscontrate;       //界外优惠率
    ShortUnon	        outdiscontlimit;   //界外优惠额度 2hex
    unsigned char   none[28];             //预留
}CardRate_local;


extern CardRate_local LocalCardRate[];

typedef struct{
    unsigned char version;              //包版本  0x07
    unsigned char totolnum;             //包总数
    unsigned char packsn;               //包序号  该数据包在本类型包中的顺序号
    unsigned char recordsum[2];         //包中记录总数
    unsigned char precordlen[2];        //单条记录长度
    unsigned char recordstartadd[2];    //包中记录开始位置   相对文件开始位置的偏移
    unsigned char creatdate[4];         //生成日期 BCD格式
    unsigned char unitcode[4];          //接收方单位代码
    unsigned char parever[4];           //参数版本号
    unsigned char efficetime[7];        //生效时间
    unsigned char none[2];              //预留
    unsigned char crc16[2];             //CRC16的校验码   
}FLC0006;

extern FLC0006 flc0006;

//费率卡0007文件
//互通卡白名单结构
typedef struct{
    unsigned char version;              //包版本  0x07
    unsigned char totolnum;             //包总数
    unsigned char packsn;               //包序号  该数据包在本类型包中的顺序号
    unsigned char recordsum[2];         //包中记录总数
    unsigned char precordlen[2];        //单条记录长度
    unsigned char recordstartadd[2];    //包中记录开始位置   相对文件开始位置的偏移
    unsigned char creatdate[4];         //生成日期 BCD格式
    unsigned char unitcode[4];          //接收方单位代码
    unsigned char parever[4];           //参数版本号
    unsigned char efficetime[7];        //生效时间
    unsigned char none[2];              //预留
    unsigned char crc16[2];             //CRC16的校验码   
}FLC0007;

extern FLC0007 flc0007;


//费率卡0008文件
//互通卡记录体结构
typedef struct {
    unsigned char cardissuerlabel[8];   //发卡机构标识
    unsigned char logiccardtype;        //卡片逻辑卡类型        13种卡   
    unsigned char consumemode;          //收费模式   高4位使用模式  低4位表示提示模式    
    unsigned char overdraw[2];          //最大透支限额  单位分
    unsigned char maxblancelimit[4];    //最大余额限制  卡内余额大于此金额不允许交易  单位分
    unsigned char maxdebit[2];          //最大扣款额度
    unsigned char indiscontrate;        //界内优惠率
    unsigned char indiscontlimit[2];    //界内优惠额度
    unsigned char outdiscontrate;       //界外优惠率
    unsigned char outdiscontlimit[2];   //界外优惠额度
    unsigned char none[24];             //预留
}CardRate_remot;

extern CardRate_remot remotcard[13];

typedef struct{
    unsigned char version;              //包版本  0x07
    unsigned char totolnum;             //包总数
    unsigned char packsn;               //包序号  该数据包在本类型包中的顺序号
    unsigned char recordsum[2];         //包中记录总数
    unsigned char precordlen[2];        //单条记录长度
    unsigned char recordstartadd[2];    //包中记录开始位置   相对文件开始位置的偏移
    unsigned char creatdate[4];         //生成日期 BCD格式
    unsigned char unitcode[4];          //接收方单位代码
    unsigned char parever[4];           //参数版本号
    unsigned char efficetime[7];        //生效时间
    unsigned char none[2];              //预留
    unsigned char crc16[2];             //CRC16的校验码   
}FLC0008;

extern FLC0008 flc0008;


typedef struct{
    unsigned char Tag1;                  //市界面列表  0xA3
    unsigned char Len1;                  //列表长度
    unsigned char Value1[6];             //数据
    unsigned char crc16[2];              //CRC16的校验码   
    unsigned char Tag2;                  //市界面列表  0xA1
    unsigned char Len2;                  //列表长度  
}FLC0009;

extern FLC0009 flc0009;


typedef struct {
	unsigned char uprecord;				//上行记录长度
	unsigned char uprecordnun[2];		//上行记录个数
	unsigned char downrecord;			//下行记录长度
	unsigned char downrecordnum[2];     //下行记录个数
	unsigned char m4ver[2];             //文件版本
}FileM4;

extern FileM4 filem4;


typedef struct {
	unsigned char localrate;			//本地折率单个记录长度
	unsigned char localratenum[2];		//本地折率记录个数
	unsigned char remotrate;			//互通折率单个记录长度
	unsigned char remotratenum[2];     //互通记录个数
	unsigned char m5ver[2];             //文件版本
}FileM5;

extern FileM4 filem5;

//下载的MP费率卡二次发行信息文件
typedef struct{
	unsigned char linenum[2];			// 线路号	
	unsigned char linename[16];			//线路名称
	unsigned char corpflag[2];			//运营单位标示
	unsigned char corpenterprisename[40];  //客运企业名称
	unsigned char corpcode[4];			//所属分公司代码		
	unsigned char corpcompanyname[40];	//分公司名称
	unsigned char lineattr;				//线路属性
	ShortUnon uppricesitemnum;			//上行站点总数
	ShortUnon downpricesitemnum;		//下行站点总数
	unsigned char pricever[2];			//当前线路文件格式版本号
	ShortUnon tickettransfer;			//换乘时间
	ShortUnon defaultbaseprice;			//基本票价
	unsigned char upstationmid;			//上行市界起点
	unsigned char downstationmid;		//下行市界起点
	unsigned char modebj;				//本地未定义卡类型处理模式
	unsigned char modehl;				//异地未定义卡类型处理模式
	unsigned char vehicleattr;			//车辆属性
	unsigned char modehlbp;			//异地逃跑补票模式
	unsigned char onoffdir;				//上下车方向不同补票规则
	unsigned char pricediscount;		//补票金额是否打折标识
	unsigned char funcflag[2];			//功能开关
	unsigned char filever[2];			//本文件版本
	
}FileMP;

extern FileMP filemp;








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

