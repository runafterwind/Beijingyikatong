#ifndef _Resumedownload_H
#define _Resumedownload_H

#if 0
#include "apparel.h"
/*******打印调试开关*******/
#define UPDEG       0                   

/*******使用设备定义*******/

#define L9  1              //配置用于L9产品，用于L11产品是为0
#ifdef SAVE_CONSUM_DATA_DIRECT
#define GONGJIAO    1      //代码用于公交程序时为1，其他为0
#else
#define GONGJIAO    0      //代码用于公交程序时为1，其他为0
#endif

/*******文件定义*******/
#if L9
#if GONGJIAO
//临时下载文件
//#define TEMPFILE         "/mnt/record/tempupfile.bin"
#define TEMPFILE         "/var/run/tempupfile.bin"

//文件下载过程配置文件
#define PROCESSFILE      "/mnt/record/upprocess.sys"
//执行文件
#define EXFILE           "/mnt/record/upfile.tar.gz"
#else
//临时下载文件
#define TEMPFILE         "/var/run/tempupfile.bin"
//文件下载过程配置文件
#define PROCESSFILE      "/var/run/upprocess.sys"
//执行文件
#define EXFILE           "/var/run/upfile.tar.gz"
#endif
#else
//临时下载文件
#define TEMPFILE         "/opt/minigui/app/tempupfile.bin"
//文件下载过程配置文件
#define PROCESSFILE      "/opt/minigui/app/upprocess.sys"
//执行文件
#define EXFILE           "/opt/minigui/app/upfile.tar.gz"
#endif

/*******变量定义*******/

extern unsigned char Netstatus;           //当前网络状态

//文件信息结构体
struct Upfile
{
    unsigned char  filenamelen;
    unsigned char  upfilename[30];        //文件名
    unsigned int   upfilelen;             //文件大小
    unsigned short crc;                   //文件校验值
    unsigned int offset;                //下载偏移量  
};


/*******命令定义*******/
#ifdef BS
#define  FILEHEAD           "BPD:"
#else
#define  FILEHEAD           "SYS:"
#endif
#define  COMMAND_DOWN       0xd2          //下载
#define  COMMAND_STOP       0xd3          //终止
#define  COMMAND_REQ        0xd4          //请求
#define  COMMAND_CHECK      0xd5          //查询
#define  COMMAND_EXECUTE    0xd6          //执行
#define  COMMAND_RECOVER    0xd7          //恢复


/*******函数定义*******/
/****************************************
- 函数名称 : void InitEnv()
- 函数说明 : 初始化断点续传函数
- 函数功能 : 读取已存在于机器中的下载，如果文件不存在则创建一个
- 输入参数 : 无
- 输出参数 : 无
****************************************/
extern void InitEnv();

/****************************************
- 函数名称 : int DownloadProcess(unsigned char *data,unsigned char len);
- 函数说明 : 服务器下载文件信息
- 函数功能 : 接收服务器下载过来的，关于需要下载文件的信息(文件名，大小，校验值)
- 函数返回 : 操作结果
             0:保存成功
             -1:操作失败
- 输入参数 : 
            *data--文件相关信息
            len -- 文件相关信息长度
- 输出参数 : 无
****************************************/
extern int DownloadProcess(unsigned char *data,unsigned char len);

/****************************************
- 函数名称 : int StopProcess()
- 函数说明 : 停止下载
- 函数功能 : 服务器下发停止文件请求命令
- 函数返回 : 操作结果
             0:保存成功
             -1:操作失败
- 输入参数 : 无
- 输出参数 : 无
****************************************/
extern int  StopProcess();

/****************************************
- 函数名称 : int ReqProcess()
- 函数说明 : 过程查询
- 函数功能 : 服务器下发查询当前已下载文件的信息
- 函数返回 : 查询到的状态
             0:文件下载完成
             1:文件下载失败
             2:文件下载中
- 输入参数 : 
             *filename:查询下载的文件名
             filenamelen:查询文件名的长度
- 输出参数 : 
             *reson:下载失败原因
             *downloadper:已下载的百分比
******************************************/
extern int ReqProcess(unsigned char *reson,unsigned char *filename,unsigned char filenamelen,unsigned short *downloadper);

/****************************************
- 函数名称 : int ExeProcess()
- 函数说明 : 文件执行
- 函数功能 : 服务器下发执行文件更新命
- 函数返回 : 操作结果
             0:保存成功
             -1:操作失败
- 输入参数 : 无
- 输出参数 : 无
******************************************/
extern int ExeProcess();


/****************************************
- 函数名称 : int RecoveryProcess()
- 函数说明 : 恢复原文件
- 函数功能 : 服务器下发恢复上一个版本程序命令
- 函数返回 : 操作结果
             0:保存成功
             -1:操作失败
- 输入参数 : 无
- 输出参数 : 无
******************************************/
extern int RecoverProcess();



/****************************************
- 函数名称 : unsigned int LenDataCmd1(unsigned char *DataLen) 
- 函数说明 : 计算发送数据长度
- 函数功能 : 通过公式计算出接收到数据的长度
- 函数返回 : 接收到的数据长度
- 输入参数 : 
            *DateLen:接收到数据的长度
            
- 输出参数 : 无
******************************************/

extern unsigned int LenDataCmd1(unsigned char *DataLen);



/****************************************
- 函数名称 : *DownloadFile_pthread(void *args);
- 函数说明 : 文件下载线程函数
- 函数功能 : 设备开机后，开启线程准备文件下载
- 函数返回 : 无
- 输入参数 : 无
- 输出参数 : 无
******************************************/
extern void *DownloadFile_pthread(void *args);

/****************************************
- 函数名称 : void close_pthread();
- 函数说明 : 关闭线程
- 函数功能 : 关闭文件升级线程
- 函数返回 : 无
- 输入参数 : 无
- 输出参数 : 无
******************************************/
extern void close_pthread();

#endif

#endif


