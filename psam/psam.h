#ifndef __PSAM_H
#define __PSAM_H


#define   RESPONSE		0x61
#define	  POWER_DOWN	0x09
#define	  WARM_RESET	0x10  
#define	  COLD_RESET	0x11  //冷复位的命令
#define   RECEIVE_LEN	0x12  //读取接收数据长度的命令
#define   BAUD_CMD		0x13  //更改波特率的命令
#define   PPS_CMD		0x14  //PPS
#define   SEL_CARD		0x15
#define	  CHECK_PRESENCE	0x16


#define   BAUD_2400       0x30
#define   BAUD_3200       0x24
#define   BAUD_4800       0x18
#define   BAUD_6400       0x12
#define   BAUD_9600       0x0C
#define   BAUD_12800      0x09
#define   BAUD_19200      0x06
#define   BAUD_28800      0x04
#define   BAUD_38400      0x03
#define   BAUD_57600      0x02
#define   BAUD_115200     0x01


#define USE_STDINT_TYPES 1

#if USE_STDINT_TYPES
#include "stdint.h"
#else
// Data types
#ifndef uint8_t
#define uint8_t  	unsigned char
#define uint16_t 	unsigned short
#define uint32_t 	unsigned int
#endif

#ifndef int16_t
typedef signed short int16_t;
typedef signed int int32_t;
#endif
#endif



#define PPS_MAX_F 	14
#define PPS_MAX_D 	10

typedef struct
{
	char psc;
	char pdr;
} tda_pps_config;




char InitPsam(void);
void ClosePsam(void);
unsigned char PsamCos(char *Intdata, char *Outdata,unsigned char *len);
unsigned char GetPsamID(void);
unsigned char PsamApply(void);
unsigned char PsamTacFst(void);


int16_t menu_tda8007_open(void);
int16_t menu_tda8007_receiveResponse(uint8_t *recvdata);
int16_t menu_tda8007_coldreset(void);
int16_t menu_tda8007_warmreset(void);
int16_t menu_tda8007_powerdown(void);
int16_t menu_tda8007_selectcard(uint8_t slot);
int16_t menu_tda8007_setbaudrate(uint8_t val);
int16_t menu_tda8007_send(uint8_t *buffer,int16_t length);
int16_t menu_tda8007_checkpresence(uint8_t slot);




#endif

