#include "driver/can.h"

#include "driver/timer.h"
#include "driver/pio.h"
#include "driver/sys.h"

#define CAN__RESET_CONTROL() CAN_CR = 0

#define CAN__ENABLE() CAN_CR |= Can_Control_Enable
#define CAN__DISABLE() CAN_CR &= ~Can_Control_Enable

#define CAN__ENABLE_INTERRUPT() ECAN = 1

#define CAN__RESET() \
CAN_CR |= Can_Control_Reset; \
INTERRUPT_GUARD(Timer_start(6, 1)) \
while(!Timer_timeout(6)); \
CAN_CR &= ~Can_Control_Reset

#define CAN__CONFIGURE() \
CAN_CR |= Can_Control_Configure; \
do {} while(CAN_CR & Can_Control_Configure)

#define CAN__SPEED_1_SAMPLING() CAN_CR |= Can_Control_SpeedMode
#define CAN__SPEED_3_SAMPLING() CAN_CR &= ~Can_Control_SpeedMode
#define CAN__SEND() CAN_CR |= Can_Control_Send
#define CAN__WAIT_SENDING() do {} while(CAN_CR & Can_Control_Send)

#define CAN__LOOP_INDEX(index) (((index) + 1) % CAN__BUFFER_SIZE)

typedef enum Can_Control
{
  Can_Control_Send = Bits_Bit8_2
  // 1 = double; 0 = single
  , Can_Control_FilterMode = Bits_Bit8_3

  // 1 = 1 sampling; 0 = 3 sampling
  , Can_Control_SpeedMode = Bits_Bit8_4
  , Can_Control_Configure = Bits_Bit8_5
  , Can_Control_Reset = Bits_Bit8_6
  , Can_Control_Enable = Bits_Bit8_7
} Can_Control_t;

typedef enum Can_Error
{
  Can_Error_Bit = Bits_Bit8_0
  , Can_Error_Bit_Filling = Bits_Bit8_1
  , Can_Error_Format = Bits_Bit8_2
  , Can_Error_Response = Bits_Bit8_3

  , Can_Error_CrcCheck = Bits_Bit8_4
  , Can_Error_Passive = Bits_Bit8_5
  , Can_Error_Active = Bits_Bit8_6
  , Can_Error_NodeSuspension = Bits_Bit8_7
} Can_Error_t;

typedef enum Can_Status
{
  Can_Status_Rtr = Bits_Bit8_6
  , Can_Status_Ide = Bits_Bit8_7
} Can_Status_t;

typedef enum Can_Length
{
  Can_Length_0
  , Can_Length_1
  , Can_Length_2
  , Can_Length_3

  , Can_Length_4
  , Can_Length_5
  , Can_Length_6
  , Can_Length_7
  , Can_Length_8
};

static Can_Bus_t xdata Can_Bus;

//125K{0x20,0x20,0xF6,0x00},150K{0x3F,0x40,0x72,0x00},
//125K{0x3F,0x40,0x72,0x00},250K{0x1F,0x40,0x72,0x00},500K{0x0F,0x40,0x72,0x00},1M{0x07,0x40,0x72,0x00}
void Can_init(
  const Can_InitConfig_t *config
  , const Can_Filter_t *filter
)
{
  Can_Bus.rx.head = 0;
  Can_Bus.rx.tail = 0;
  Can_Bus.tx.head = 0;
  Can_Bus.tx.tail = 0;
  Can_Bus.tx_flag = Bits_State_NotSet;

  CAN__RESET_CONTROL();

  Pio_setPinModes(Pio_Port_CanTx, Pio_Pin_CanTx, Pio_PinMode_Out);
  Pio_setPinModes(Pio_Port_CanRx, Pio_Pin_CanRx, Pio_PinMode_In);
  Pio_writePins(Pio_Port_CanTx, Pio_Pin_CanTx, Bits_State_Set);
  Pio_setPeripheralModes(Pio_Peripheral_Can, Bits_State_Set);
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
  DATA3 = filter->acceptance_code.bytes[0];
  DATA2 = filter->acceptance_code.bytes[1];
  DATA1 = filter->acceptance_code.bytes[2];
  DATA0 = filter->acceptance_code.bytes[3];
  APP_EN = 1;
  while(APP_EN);
//  DATA3 = 0x77;
//  DATA2 = 0xff;
//  DATA1 = 0xbd;
//  DATA0 = 0x00;
  // 0x FF FF FF FF
  DATA3 = filter->acceptance_mask.bytes[0];
  DATA2 = filter->acceptance_mask.bytes[1];
  DATA1 = filter->acceptance_mask.bytes[2];
  DATA0 = filter->acceptance_mask.bytes[3];
  APP_EN = 1;
  while(APP_EN);
  RAMMODE = 0;

  CAN__ENABLE();
  CAN__CONFIGURE();

  CAN__ENABLE_INTERRUPT();
}

void Can_resetError(void) small
{
  if (!(CAN_ET & Can_Error_Passive)) return;

  CAN_ET &= ~Can_Error_Passive;
  CAN__RESET();
  Can_Bus.tx_flag = Bits_State_NotSet;
}

void LoadOneFrame(void) small
{
  Can_Message_t *tx_message = Can_Bus.tx.messages + Can_Bus.tx.tail;
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

void Can_tx(u8 status, u32 id, const u8 *bytes, u16 length)
{
  Can_Message_t *tx_message;

  if (2048 < length)
    return;

  status &= (Can_Status_Ide | Can_Status_Rtr);

  if(status & Can_Status_Ide)
  {
    id <<= 3;
  }
  else
  {
    id <<= 21;
  }

  tx_message = Can_Bus.tx.messages + Can_Bus.tx.head;
  if(status & Can_Status_Rtr)
  {
    tx_message->id = id;
    tx_message->status = status;
    Can_Bus.tx.head = CAN__LOOP_INDEX(Can_Bus.tx.head);
  }
  else
  {
    u8 frame, byte;
    u8 n_frames = length >> 3;
    u8 frame_offset = length % 8;
    for (frame = 0; frame < n_frames; ++frame)
    {
      tx_message->id = id;
      tx_message->status = status | Can_Length_8;
      for(byte = 0; byte < 8; ++byte)
      {
        tx_message->bytes[byte] = *bytes++;
      }
      Can_Bus.tx.head = CAN__LOOP_INDEX(Can_Bus.tx.head);
      tx_message = Can_Bus.tx.messages + Can_Bus.tx.head;
    }
    if(frame_offset)
    {
      tx_message->id = id;
      tx_message->status = status | frame_offset;
      for(byte = 0; byte < 8; ++byte)
      {
        tx_message->bytes[byte]
          = byte < frame_offset
          ? *bytes++
          : 0;
      }
      Can_Bus.tx.head = CAN__LOOP_INDEX(Can_Bus.tx.head);
    }
  }

  if(!Can_Bus.tx_flag)
  {
    CAN__WAIT_SENDING();

    INTERRUPT_GUARD(LoadOneFrame());
    Can_Bus.tx_flag = Bits_State_Set;
    INTERRUPT_GUARD(Timer_start(7, 3000));

    CAN__SEND();
  }

  if(Can_Bus.tx_flag && Timer_timeout(7))
  {
    Can_Bus.tx_flag = Bits_State_NotSet;
  }
}

u8 Can_rx(Can_Message_t *message)
{
  if (Can_Bus.rx.head == Can_Bus.rx.tail)
  {
    return 0;
  }

  (*message) = Can_Bus.rx.messages[Can_Bus.rx.tail];
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
      CAN__WAIT_SENDING();

      LoadOneFrame();
      Timer_start(7, 3000);

      CAN__SEND();
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
    CAN__SEND();
  }
  CAN_ET = 0;
  ENABLE_INTERRUPT();
}


