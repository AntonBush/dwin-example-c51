#include "driver/can.h"

#include "timer.h"
#include "sys.h"

#define CAN__LOOP_INDEX(index) ((index) + 1) % CAN__BUFFER_SIZE

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
  TIMER__START_SAFE(6, 1);
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
  Can_Bus.tx.tail = CAN__LOOP_INDEX(Can_Bus.tx.tail);
  RAMMODE = 0;
}

void Can_tx(u8 status, u32 id, const u8 *bytes, u16 length) compact
{
  u8 i,j,k,framnum,framoffset;
  u32 idtmp,statustmp;
  Can_Message_t *tx_message;

  if(length > 2048)
    return;

  if(status & Bits_Bit8_7)
  {
    idtmp = id << 3;
  }
  else
  {
    idtmp = id << 21;
  }

  tx_message = Can_Bus.tx.messages + Can_Bus.tx.head;
  if(tx_message->status & Bits_Bit8_6)
  {
    tx_message->id = idtmp;
    tx_message->status = status & (Bits_Bit8_7 | Bits_Bit8_6);
    Can_Bus.tx.head = CAN__LOOP_INDEX(Can_Bus.tx.head);
    tx_message = Can_Bus.tx.messages + Can_Bus.tx.head;
  }
  else
  {
    framnum = length >> 3;
    framoffset = length % 8;
    k = 0;
    statustmp = status & (Bits_Bit8_7 | Bits_Bit8_6);
    for(i = 0; i < framnum; ++i)
    {
      tx_message->id = idtmp;
      tx_message->status = statustmp | Bits_Bit8_3;
      for(j = 0; j < 8; ++j)
      {
        tx_message->bytes[j] = bytes[k];
        k += 1;
      }
      Can_Bus.tx.head = CAN__LOOP_INDEX(Can_Bus.tx.head);
      tx_message = Can_Bus.tx.messages + Can_Bus.tx.head;
    }
    if(framoffset)
    {
      tx_message->id = idtmp;
      tx_message->status = statustmp | framoffset;
      for(j = 0; j < framoffset; ++j)
      {
        tx_message->bytes[j] = bytes[k];
        k += 1;
      }
      for(; j < 8; ++j)
      {
        tx_message->bytes[j] = 0;
      }
      Can_Bus.tx.head = CAN__LOOP_INDEX(Can_Bus.tx.head);
      tx_message = Can_Bus.tx.messages + Can_Bus.tx.head;
    }
  }
  if(!Can_Bus.tx_flag)
  {
    DISABLE_INTERRUPT();
    LoadOneFrame();
    ENABLE_INTERRUPT();
    Can_Bus.tx_flag = Bits_State_Set;
    TIMER__START_SAFE(7,3000);
    CAN_CR |= Bits_Bit8_2;
  }
   if(Can_Bus.tx_flag)
   {
     if(Timer_timeout(7))
     {
       Can_Bus.tx_flag = Bits_State_NotSet;
     }
   }
}

u8 Can_rx(Can_Message_t *message) compact
{
  Can_Message_t *rx_message = Can_Bus.rx.messages + Can_Bus.rx.tail;

  if (Can_Bus.rx.head == Can_Bus.rx.tail)
  {
    return 0;
  }

  (*message) = (*rx_message);
  Can_Bus.rx.tail = CAN__LOOP_INDEX(Can_Bus.rx.tail);

  return 1;
}

void Can_handleInterruption(void) small interrupt 9
{
  u8 status;

  DISABLE_INTERRUPT();
  if(CAN_IR & Bits_Bit8_7)
  {
    CAN_IR &= ~(Bits_Bit8_7 | Bits_Bit8_6);
  }
  if(CAN_IR & Bits_Bit8_6)
  {
    Can_Message_t *rx_message = Can_Bus.rx.messages + Can_Bus.rx.head;

    CAN_IR &= ~(Bits_Bit8_6);
    ADR_H = 0xFF;
    ADR_M = 0x00;
    ADR_L = 0x68;
    ADR_INC = 1;
    RAMMODE = 0xAF;
    while(!APP_ACK);
    APP_EN = 1;
    while(APP_EN);
    status = DATA3;
    rx_message->status = status;
    APP_EN = 1;
    while(APP_EN);

    rx_message->id <<= 8;
    rx_message->id |= DATA3;
    rx_message->id <<= 8;
    rx_message->id |= DATA2;
    rx_message->id <<= 8;
    rx_message->id |= DATA1;
    rx_message->id <<= 8;
    rx_message->id |= DATA0;
    rx_message->id >>= 3;

    if(!(status & Bits_Bit8_7))
    {
      rx_message->id >>= 18;
    }
    if(!(status & Bits_Bit8_6))
    {
      APP_EN = 1;
      while(APP_EN);
      rx_message->bytes[0] = DATA3;
      rx_message->bytes[1] = DATA2;
      rx_message->bytes[2] = DATA1;
      rx_message->bytes[3] = DATA0;
      APP_EN = 1;
      while(APP_EN);
      rx_message->bytes[4] = DATA3;
      rx_message->bytes[5] = DATA2;
      rx_message->bytes[6] = DATA1;
      rx_message->bytes[7] = DATA0;
    }
    RAMMODE = 0;
    Can_Bus.rx.head = CAN__LOOP_INDEX(Can_Bus.rx.head);
  }
  if(CAN_IR & Bits_Bit8_5)
  {
    CAN_IR &= ~Bits_Bit8_5;
    if(Can_Bus.tx.tail != Can_Bus.tx.head)
    {
      LoadOneFrame();
      CAN_CR |= Bits_Bit8_2;
      Timer_start(7, 3000);
    }
    else
    {
      Can_Bus.tx_flag = Bits_State_NotSet;
    }
  }
  if(CAN_IR & Bits_Bit8_4)
  {
    CAN_IR &= ~Bits_Bit8_4;
  }
  if(CAN_IR & Bits_Bit8_3)
  {
    CAN_IR &= ~Bits_Bit8_3;
  }
  if(CAN_IR & Bits_Bit8_2)
  {
    CAN_IR &= ~Bits_Bit8_2;
    CAN_CR |= Bits_Bit8_2;
  }
  CAN_ET = 0;
  ENABLE_INTERRUPT();
}


