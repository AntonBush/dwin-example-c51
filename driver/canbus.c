#include "canbus.h"

#include "timer.h"
#include "sys.h"

static xdata Can_Bus_t Can_Bus;

//125K{0x20,0x20,0xF6,0x00},150K{0x3F,0x40,0x72,0x00},
//125K{0x3F,0x40,0x72,0x00},250K{0x1F,0x40,0x72,0x00},500K{0x0F,0x40,0x72,0x00},1M{0x07,0x40,0x72,0x00}
void Can_init(
  const Can_InitConfig_t *config
  , const Can_AcceptanceFilter_t *filter
)
{
  SetPinOut(0,2);
  SetPinIn(0,3);
  PinOutput(0,2,1);
  MUX_SEL |= Bits_Bit8_7;
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
  DATA3 = config->baud_rate_frequency_divider;
  DATA2 = config->BTR0;
  DATA1 = config->BTR1;
  DATA0 = 0;
  #endif
  APP_EN = 1;
  while(APP_EN);
//  DATA3 = 0;
//  DATA2 = 0;
//  DATA1 = 0x42;
//  DATA0 = 0xc7;
  // 0x 00 00 00 00
  DATA3 = filter->acceptance_code[0];
  DATA2 = filter->acceptance_code[1];
  DATA1 = filter->acceptance_code[2];
  DATA0 = filter->acceptance_code[3];
  APP_EN = 1;
  while(APP_EN);
//  DATA3 = 0x77;
//  DATA2 = 0xff;
//  DATA1 = 0xbd;
//  DATA0 = 0x00;
  // 0x FF FF FF FF
  DATA3 = filter->acceptance_mask[0];
  DATA2 = filter->acceptance_mask[1];
  DATA1 = filter->acceptance_mask[2];
  DATA0 = filter->acceptance_mask[3];
  APP_EN = 1;
  while(APP_EN);
  RAMMODE = 0;
  CAN_CR = Bits_Bit8_7 | Bits_Bit8_5;
  while(CAN_CR & Bits_Bit8_5);
  ECAN = 1;
}

void Can_resetError(void) small
{
  if (!(CAN_ET & Bits_Bit8_5)) return;

  CAN_ET &= ~Bits_Bit8_5;

  CAN_CR |= Bits_Bit8_6;
  Timer_start(6, 1);
  while(!Timer_timeout(6));
  CAN_CR &= ~Bits_Bit8_6;

  Can_Bus.tx_flag = Bits_State_NotSet;
}

void LoadOneFrame(void) small
{
  Can_Message_t *tx_message = Can_Bus.tx + Can_Bus.tx.tail;
  ADR_H = 0xFF;
  ADR_M = 0x00;
  ADR_L = 0x64;
  ADR_INC = 1;
  RAMMODE = 0x8F;
  while(!APP_ACK);
  DATA3 = tx_message->status;
  DATA2 = 0;
  DATA1 = 0;
  DATA0 = 0;
  APP_EN = 1;
  while(APP_EN);
  DATA3 = tx_message->id >> 24;
  DATA2 = tx_message->id >> 16;
  DATA1 = tx_message->id >>  8;
  DATA0 = tx_message->id >>  0;
  APP_EN = 1;
  while(APP_EN);
  DATA3 = tx_message->bytes[0];
  DATA2 = tx_message->bytes[1];
  DATA1 = tx_message->bytes[2];
  DATA0 = tx_message->bytes[3];
  APP_EN = 1;
  while(APP_EN);
  DATA3 = tx_message->bytes[4];
  DATA2 = tx_message->bytes[5];
  DATA1 = tx_message->bytes[6];
  DATA0 = tx_message->bytes[7];
  APP_EN = 1;
  while(APP_EN);
  Can_Bus.tx.tail += 1;
  RAMMODE = 0;
}

void Can_tx(const Can_Message_t *message, u16 length) compact
{
  u8 i,j,k,framnum,framoffset;
  u32 idtmp,statustmp;

  if(length > 2048)
    return;
  if(status & Bits_Bit8_7)
  {
    idtmp = ID << 3;
  }
  else
  {
    idtmp = ID << 21;
  }
  if(Can_Bus.tx[Can_Bus.tx.head].status & Bits_Bit8_6)
  {
    Can_Bus.tx[Can_Bus.tx.head].ID = idtmp;
    Can_Bus.tx[Can_Bus.tx.head].status = status & (Bits_Bit8_7 | Bits_Bit8_6);
    Can_Bus.tx.head += 1;
  }
  else
  {
    framnum = len >> 3;
    framoffset = len % 8;
    k = 0;
    statustmp = status & (Bits_Bit8_7 | Bits_Bit8_6);
    for(i = 0; i < framnum; ++i)
    {
      Can_Bus.tx[Can_Bus.tx.head].ID = idtmp;
      Can_Bus.tx[Can_Bus.tx.head].status = statustmp | Bits_Bit8_3;
      for(j = 0; j < 8; ++j)
      {
        Can_Bus.tx[Can_Bus.tx.head].candata[j] = pData[k];
        k++;
      }
      Can_Bus.tx.head += 1;
    }
    if(framoffset)
    {
      Can_Bus.tx[Can_Bus.tx.head].ID = idtmp;
      Can_Bus.tx[Can_Bus.tx.head].status = statustmp | framoffset;
      for(j=0;j<framoffset;j++)
      {
        Can_Bus.tx[Can_Bus.tx.head].candata[j] = pData[k];
        k++;
      }
      for(;j<8;j++)
        Can_Bus.tx[Can_Bus.tx.head].candata[j] = 0;
      Can_Bus.tx.head++;
    }
  }
  if(0==Can_Bus.tx_flag)
  {
    EA = 0;
    LoadOneFrame();
    EA = 1;
    Can_Bus.tx_flag = 1;
    Timer_start(7,3000);
    CAN_CR |= 0x04;
  }
   if(Can_Bus.tx_flag!=0)
   {
     if(Timer_timeout(7))
     {
       Can_Bus.tx_flag = 0;
     }
   }
}

void Can_rx(Can_Message_t *message) compact
{
  u8 i;
  Can_Message_t *rx_message = Can_Bus.rx.messages + Can_Bus.rx.tail;

  if (Can_Bus.rx.head == Can_Bus.rx.tail)
  {
    message->status = 0;
  }

  (*message) = rx_message;
  message->status = 1;

  Can_Bus.rx.tail += 1;
  Can_Bus.rx.tail %= CAN__BUFFER_SIZE;
}

void Can_handleInterruption(void) small interrupt 9
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
    Can_Bus.BusRXbuf[Can_Bus.rx.head].status = status;
    APP_EN = 1;
    while(APP_EN);
    Can_Bus.BusRXbuf[Can_Bus.rx.head].ID <<= 8;
    Can_Bus.BusRXbuf[Can_Bus.rx.head].ID |= DATA3;
    Can_Bus.BusRXbuf[Can_Bus.rx.head].ID <<= 8;
    Can_Bus.BusRXbuf[Can_Bus.rx.head].ID |= DATA2;
    Can_Bus.BusRXbuf[Can_Bus.rx.head].ID <<= 8;
    Can_Bus.BusRXbuf[Can_Bus.rx.head].ID |= DATA1;
    Can_Bus.BusRXbuf[Can_Bus.rx.head].ID <<= 8;
    Can_Bus.BusRXbuf[Can_Bus.rx.head].ID |= DATA0;
    Can_Bus.BusRXbuf[Can_Bus.rx.head].ID=Can_Bus.BusRXbuf[Can_Bus.rx.head].ID>>3;
    if(0==(status&0x80))
    {
      Can_Bus.BusRXbuf[Can_Bus.rx.head].ID >>= 18;
    }
    if(0==(status&0x40))
    {
      APP_EN = 1;
      while(APP_EN);
      Can_Bus.BusRXbuf[Can_Bus.rx.head].candata[0] = DATA3;
      Can_Bus.BusRXbuf[Can_Bus.rx.head].candata[1] = DATA2;
      Can_Bus.BusRXbuf[Can_Bus.rx.head].candata[2] = DATA1;
      Can_Bus.BusRXbuf[Can_Bus.rx.head].candata[3] = DATA0;
      APP_EN = 1;
      while(APP_EN);
      Can_Bus.BusRXbuf[Can_Bus.rx.head].candata[4] = DATA3;
      Can_Bus.BusRXbuf[Can_Bus.rx.head].candata[5] = DATA2;
      Can_Bus.BusRXbuf[Can_Bus.rx.head].candata[6] = DATA1;
      Can_Bus.BusRXbuf[Can_Bus.rx.head].candata[7] = DATA0;
    }
    RAMMODE = 0;
    Can_Bus.rx.head = (Can_Bus.rx.head + 1) % CANBUFFSIZE;
  }
  if((CAN_IR&0x20) == 0x20)
  {
    CAN_IR &= ~(0x20);
    if(Can_Bus.tx.tail != Can_Bus.tx.head)
    {
      LoadOneFrame();
      CAN_CR |= 0x04;
      Timer_start(7,3000);
    }
    else
    {
      Can_Bus.tx_flag = 0;
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


