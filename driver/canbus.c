#include "canbus.h"
#include "timer.h"
#include "const.h"

CANBUSUNIT   CanData;
//125K{0x20,0x20,0xF6,0x00},150K{0x3F,0x40,0x72,0x00},
//125K{0x3F,0x40,0x72,0x00},250K{0x1F,0x40,0x72,0x00},500K{0x0F,0x40,0x72,0x00},1M{0x07,0x40,0x72,0x00}
void CanBusInit(u8* RegCfg)
{
  SetPinOut(0,2);
  SetPinIn(0,3);
  PinOutput(0,2,1);
  MUX_SEL |= 0x80;
  ADR_H = 0xFF;
  ADR_M = 0x00;
  ADR_L = 0x60;
  ADR_INC = 1;
  RAMMODE = 0x8F;
  while(!APP_ACK);
  #if 0
  DATA3 = 0x3f;
  DATA2 = 0x40;
  DATA1 = 0x72;
  DATA0 = 0x00;
  #else
  DATA3 = RegCfg[0];
  DATA2 = RegCfg[1];
  DATA1 = RegCfg[2];
  DATA0 = RegCfg[3];
  #endif
  APP_EN = 1;
  while(APP_EN);
//  DATA3 = 0;
//  DATA2 = 0;
//  DATA1 = 0x42;
//  DATA0 = 0xc7;
  DATA3 = 0;
  DATA2 = 0;
  DATA1 = 0;
  DATA0 = 0;
  APP_EN = 1;
  while(APP_EN);
//  DATA3 = 0x77;
//  DATA2 = 0xff;
//  DATA1 = 0xbd;
//  DATA0 = 0x00;
  DATA3 = 0xFF;
  DATA2 = 0xFF;
  DATA1 = 0xFF;
  DATA0 = 0xFF;
  APP_EN = 1;
  while(APP_EN);
  RAMMODE = 0;
  CAN_CR = 0xA0;
  while(CAN_CR&0x20);
  ECAN = 1;
}


void CanErrorReset(void)
{
  // EA=0;
  if(CAN_ET&0X20)
  {
    CAN_ET &= 0XDF;
    CAN_CR |= 0X40;
    Timer_start(6, 1);
    while(!Timer_timeout(6));
    CAN_CR &= 0XBF;
    CanData.CanTxFlag = 0;
  }
  // EA=1;
}


void LoadOneFrame(void)
{
  ADR_H = 0xFF;
  ADR_M = 0x00;
  ADR_L = 0x64;
  ADR_INC = 1;
  RAMMODE = 0x8F;
  while(!APP_ACK);
  DATA3 = CanData.BusTXbuf[CanData.CanTxTail].status;
  DATA2 = 0;
  DATA1 = 0;
  DATA0 = 0;
  APP_EN = 1;
  while(APP_EN);
  DATA3 = (u8)(CanData.BusTXbuf[CanData.CanTxTail].ID>>24);
  DATA2 = (u8)(CanData.BusTXbuf[CanData.CanTxTail].ID>>16);
  DATA1 = (u8)(CanData.BusTXbuf[CanData.CanTxTail].ID>>8);
  DATA0 = (u8)(CanData.BusTXbuf[CanData.CanTxTail].ID);
  APP_EN = 1;
  while(APP_EN);
  DATA3 = CanData.BusTXbuf[CanData.CanTxTail].candata[0];
  DATA2 = CanData.BusTXbuf[CanData.CanTxTail].candata[1];
  DATA1 = CanData.BusTXbuf[CanData.CanTxTail].candata[2];
  DATA0 = CanData.BusTXbuf[CanData.CanTxTail].candata[3];
  APP_EN = 1;
  while(APP_EN);
  DATA3 = CanData.BusTXbuf[CanData.CanTxTail].candata[4];
  DATA2 = CanData.BusTXbuf[CanData.CanTxTail].candata[5];
  DATA1 = CanData.BusTXbuf[CanData.CanTxTail].candata[6];
  DATA0 = CanData.BusTXbuf[CanData.CanTxTail].candata[7];
  APP_EN = 1;
  while(APP_EN);
  CanData.CanTxTail++;
  RAMMODE = 0;
}

void CanTx(u32 ID, u8 status, u16 len, const u8 *pData)
{
  u8 i,j,k,framnum,framoffset;
  u32 idtmp,statustmp;

  if(len>2048)
    return;
  if(status&0x80)
  {
    idtmp = ID << 3;
  }
  else
  {
    idtmp = ID << 21;
  }
  if(CanData.BusTXbuf[CanData.CanTxHead].status&0x40)
  {
    CanData.BusTXbuf[CanData.CanTxHead].ID = idtmp;
    CanData.BusTXbuf[CanData.CanTxHead].status = status&0xC0;
    CanData.CanTxHead++;
  }
  else
  {
    framnum = len >> 3;
    framoffset = len % 8;
    k=0;
    statustmp = status&0xC0;
    for(i=0;i<framnum;i++)
    {
      CanData.BusTXbuf[CanData.CanTxHead].ID = idtmp;
      CanData.BusTXbuf[CanData.CanTxHead].status = statustmp | 0x08;
      for(j=0;j<8;j++)
      {
        CanData.BusTXbuf[CanData.CanTxHead].candata[j] = pData[k];
        k++;
      }
      CanData.CanTxHead++;
    }
    if(framoffset)
    {
      CanData.BusTXbuf[CanData.CanTxHead].ID = idtmp;
      CanData.BusTXbuf[CanData.CanTxHead].status = statustmp | framoffset;
      for(j=0;j<framoffset;j++)
      {
        CanData.BusTXbuf[CanData.CanTxHead].candata[j] = pData[k];
        k++;
      }
      for(;j<8;j++)
        CanData.BusTXbuf[CanData.CanTxHead].candata[j] = 0;
      CanData.CanTxHead++;
    }
  }
  if(0==CanData.CanTxFlag)
  {
    EA = 0;
    LoadOneFrame();
    EA = 1;
    CanData.CanTxFlag = 1;
    Timer_start(7,3000);
    CAN_CR |= 0x04;
  }
   if(CanData.CanTxFlag!=0)
   {
     if(Timer_timeout(7))
     {
       CanData.CanTxFlag = 0;
     }
   }
}


u8 canRxTreat(u32 *msgId, u8 *msgData)
{
  u8 sendbuf[30];
  u16 test;
  u8 i;

  if (CanData.CanRxHead != CanData.CanRxTail)
  {
    *msgId = (u32)CanData.BusRXbuf[CanData.CanRxTail].ID;

    for (i = 0; i < 8; i++)
    {
      msgData[i] = CanData.BusRXbuf[CanData.CanRxTail].candata[i];
    }

    CanData.CanRxTail = (CanData.CanRxTail + 1) % CANBUFFSIZE;

    return 0x01;
  }
  else
  {
    return 0x00;
  }
}

void Can_Isr() interrupt 9
{
  u8 status;

  EA = 0;
  if((CAN_IR&0x80) == 0x80)
  {
    CAN_IR &= 0x3F;
  }
  if((CAN_IR&0x40) == 0x40)
  {
    CAN_IR &= 0xBF;
    ADR_H = 0xFF;
    ADR_M = 0x00;
    ADR_L = 0x68;
    ADR_INC = 1;
    RAMMODE = 0xAF;
    while(!APP_ACK);
    APP_EN = 1;
    while(APP_EN);
    status = DATA3;
    CanData.BusRXbuf[CanData.CanRxHead].status = status;
    APP_EN = 1;
    while(APP_EN);
    CanData.BusRXbuf[CanData.CanRxHead].ID <<= 8;
    CanData.BusRXbuf[CanData.CanRxHead].ID |= DATA3;
    CanData.BusRXbuf[CanData.CanRxHead].ID <<= 8;
    CanData.BusRXbuf[CanData.CanRxHead].ID |= DATA2;
    CanData.BusRXbuf[CanData.CanRxHead].ID <<= 8;
    CanData.BusRXbuf[CanData.CanRxHead].ID |= DATA1;
    CanData.BusRXbuf[CanData.CanRxHead].ID <<= 8;
    CanData.BusRXbuf[CanData.CanRxHead].ID |= DATA0;
    CanData.BusRXbuf[CanData.CanRxHead].ID=CanData.BusRXbuf[CanData.CanRxHead].ID>>3;
    if(0==(status&0x80))
    {
      CanData.BusRXbuf[CanData.CanRxHead].ID >>= 18;
    }
    if(0==(status&0x40))
    {
      APP_EN = 1;
      while(APP_EN);
      CanData.BusRXbuf[CanData.CanRxHead].candata[0] = DATA3;
      CanData.BusRXbuf[CanData.CanRxHead].candata[1] = DATA2;
      CanData.BusRXbuf[CanData.CanRxHead].candata[2] = DATA1;
      CanData.BusRXbuf[CanData.CanRxHead].candata[3] = DATA0;
      APP_EN = 1;
      while(APP_EN);
      CanData.BusRXbuf[CanData.CanRxHead].candata[4] = DATA3;
      CanData.BusRXbuf[CanData.CanRxHead].candata[5] = DATA2;
      CanData.BusRXbuf[CanData.CanRxHead].candata[6] = DATA1;
      CanData.BusRXbuf[CanData.CanRxHead].candata[7] = DATA0;
    }
    RAMMODE = 0;
    CanData.CanRxHead = (CanData.CanRxHead + 1) % CANBUFFSIZE;
  }
  if((CAN_IR&0x20) == 0x20)
  {
    CAN_IR &= ~(0x20);
    if(CanData.CanTxTail != CanData.CanTxHead)
    {
      LoadOneFrame();
      CAN_CR |= 0x04;
      Timer_start(7,3000);
    }
    else
    {
      CanData.CanTxFlag = 0;
    }
  }
  if((CAN_IR&0x10) == 0x10)
  {
    CAN_IR &= 0xEF;
  }
  if((CAN_IR&0x08) == 0x08)
  {
    CAN_IR &= 0xF7;
  }
  if((CAN_IR&0x04) == 0x04)
  {
    CAN_IR &= 0xFB;
    CAN_CR |= 0x04;
  }
  CAN_ET=0;
  EA = 1;
}


