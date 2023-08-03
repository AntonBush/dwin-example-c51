#include "driver/can.h"

#include "driver/timer.h"
#include "driver/pio.h"
#include "driver/dgusvar.h"
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
  DgusVar_BufferPointer_t tx_buffer = DgusVar_TxBuffer;

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

  DGUSVAR__WRITE_U8_TO_BUFFER(
    tx_buffer
    , config->baud_rate_frequency_divider
  );
  DGUSVAR__WRITE_U8_TO_BUFFER(tx_buffer, config->BTR0);
  DGUSVAR__WRITE_U8_TO_BUFFER(tx_buffer, config->BTR1);
  DGUSVAR__WRITE_U8_TO_BUFFER(tx_buffer, 0);

//  DATA3 = 0;
//  DATA2 = 0;
//  DATA1 = 0x42;
//  DATA0 = 0xc7;
  // 0x 00 00 00 00
  DGUSVAR__WRITE_U32_TO_BUFFER(
    tx_buffer
    , filter->acceptance_code.value
  );

//  DATA3 = 0x77;
//  DATA2 = 0xff;
//  DATA1 = 0xbd;
//  DATA0 = 0x00;
  // 0x FF FF FF FF
  DGUSVAR__WRITE_U32_TO_BUFFER(
    tx_buffer
    , filter->acceptance_mask.value
  );

  DgusVar_write(
    DGUSVAR__CAN_ADDRESS << 1
    , DGUSVAR__FILLED_BUFFER_SIZE(tx_buffer, DgusVar_TxBuffer)
  );

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
  DgusVar_BufferPointer_t tx_buffer = DgusVar_TxBuffer;
  Can_Message_t *tx_message = Can_Bus.tx.messages + Can_Bus.tx.tail;

  DGUSVAR__WRITE_U8_TO_BUFFER(tx_buffer, tx_message->status);
  DGUSVAR__WRITE_U8_TO_BUFFER(tx_buffer, 0);
  DGUSVAR__WRITE_U16_TO_BUFFER(tx_buffer, 0);

  DGUSVAR__WRITE_U32_TO_BUFFER(tx_buffer, tx_message->id);

  DGUSVAR__WRITE_U8_TO_BUFFER(tx_buffer, tx_message->bytes[0]);
  DGUSVAR__WRITE_U8_TO_BUFFER(tx_buffer, tx_message->bytes[1]);
  DGUSVAR__WRITE_U8_TO_BUFFER(tx_buffer, tx_message->bytes[2]);
  DGUSVAR__WRITE_U8_TO_BUFFER(tx_buffer, tx_message->bytes[3]);

  DGUSVAR__WRITE_U8_TO_BUFFER(tx_buffer, tx_message->bytes[4]);
  DGUSVAR__WRITE_U8_TO_BUFFER(tx_buffer, tx_message->bytes[5]);
  DGUSVAR__WRITE_U8_TO_BUFFER(tx_buffer, tx_message->bytes[6]);
  DGUSVAR__WRITE_U8_TO_BUFFER(tx_buffer, tx_message->bytes[7]);

  DgusVar_write(
    (DGUSVAR__CAN_ADDRESS + 4) << 1
    , DGUSVAR__FILLED_BUFFER_SIZE(tx_buffer, DgusVar_TxBuffer)
  );

  Can_Bus.tx.tail = CAN__LOOP_INDEX(Can_Bus.tx.tail);
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
  u16 ignore;

  DISABLE_INTERRUPT();
  if(CAN_IR & Bits_Bit8_7)
  {
    CAN_IR &= ~(Bits_Bit8_7 | Bits_Bit8_6);
  }
  if(CAN_IR & Bits_Bit8_6)
  {
    DgusVar_BufferPointer_t rx_buffer = DgusVar_RxBuffer;
    Can_Message_t *rx_message = Can_Bus.rx.messages + Can_Bus.rx.head;

    CAN_IR &= ~(Bits_Bit8_6);

    DgusVar_read((DGUSVAR__CAN_ADDRESS + 8) << 1, 8);

    rx_message->status = DGUSVAR__READ_U8_FROM_BUFFER(rx_buffer);
    ignore = DGUSVAR__READ_U8_FROM_BUFFER(rx_buffer);
    ignore = DGUSVAR__READ_U16_FROM_BUFFER(rx_buffer);

    rx_message->id = DGUSVAR__READ_U32_FROM_BUFFER(rx_buffer);
    rx_message->id >>= 3;

    if(!(rx_message->status & Bits_Bit8_7))
    {
      rx_message->id >>= 18;
    }

    if(!(rx_message->status & Bits_Bit8_6))
    {
      rx_message->bytes[0] = DGUSVAR__READ_U8_FROM_BUFFER(rx_buffer);
      rx_message->bytes[1] = DGUSVAR__READ_U8_FROM_BUFFER(rx_buffer);
      rx_message->bytes[2] = DGUSVAR__READ_U8_FROM_BUFFER(rx_buffer);
      rx_message->bytes[3] = DGUSVAR__READ_U8_FROM_BUFFER(rx_buffer);
      rx_message->bytes[4] = DGUSVAR__READ_U8_FROM_BUFFER(rx_buffer);
      rx_message->bytes[5] = DGUSVAR__READ_U8_FROM_BUFFER(rx_buffer);
      rx_message->bytes[6] = DGUSVAR__READ_U8_FROM_BUFFER(rx_buffer);
      rx_message->bytes[7] = DGUSVAR__READ_U8_FROM_BUFFER(rx_buffer);
    }

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


