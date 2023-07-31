#ifndef  __CANBUS__H___
#define  __CANBUS__H___
//如果使用CAN，那么sys.h  必须增加如下宏定义  #define INTVPACTION
#include "sys.h"

typedef struct _candataunit
{
	u8 status;   //bit7   IDE  0表示标准帧  1表示扩展帧     bit6 RTR  0表示数据帧，1表示远程帧   bit3_0 发送数据长度，0-8.
	u32  ID;	  
	u8   candata[8];
}CANUNIT;

#define CANBUFFSIZE		256

typedef struct _candataunitbuf
{
	CANUNIT BusRXbuf[CANBUFFSIZE];
	CANUNIT BusTXbuf[CANBUFFSIZE];
	u8 CanRxHead,CanRxTail;
	u8 CanTxHead,CanTxTail,CanTxFlag;
}CANBUSUNIT;

typedef struct _can8283
{
	u8 Busbuf[256];
	u16 Can8283RxHead,Can8283RxTail;
}CANBUS8283;

extern CANBUSUNIT CanData;



void CanTx(u32 ID, u8 status, u16 len, const u8 *pData);
void CanBusInit(u8* RegCfg);
void CanErrorReset(void);
u8 canRxTreat(u32 *msgId, u8 *msgData);
#endif










