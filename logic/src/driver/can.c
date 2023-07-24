#include "driver/can.h"

#include "driver/sys.h"
#include "driver/pio.h"
#include "driver/interrupt.h"
#include "driver/timer.h"
#include "driver/dgusvar.h"

#define CAN__TIMER_ID 7
#define CAN__TIMER_DURATION 3000

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

static const DgusVar_Address_t Can_BaseAddress = {
  DGUSVAR__CAN_BASE_ADDRESS_H
  , DGUSVAR__CAN_BASE_ADDRESS_M
  , DGUSVAR__CAN_BASE_ADDRESS_L
};

static Can_Bus_t Can_CanBus;

// internal implementation defines

static void Can_resetControl(void);
static void Can_configure(void);
static void Can_enable(void);
static void Can_handleInterruption(void);
static u8 Can_error(void);
static void Can_setError(u8 error);
static void Can_softwareReset(void);
static void Can_loadTxMessage(void);
static void Can_sendMessage(void);
static void Can_moveBufferHead(Can_BusBuffer_t *buffer);
static void Can_moveBufferTail(Can_BusBuffer_t *buffer);
static void Can_startTimer(void);

// interface implementation

void Can_init(Can_InitConfig_t config, Can_AcceptanceFilter_t filter)
{
  DgusVar_Message_t message;

  Can_resetControl();

  Pio_setPinModes(Pio_Port_CanTx, Pio_Pin_CanTx, Pio_PinMode_Out);
  Pio_setPinModes(Pio_Port_CanRx, Pio_Pin_CanRx, Pio_PinMode_In);
  Pio_writePins(Pio_Port_CanTx, Pio_Pin_CanTx, Pio_PinState_Active);
  Pio_setPeripheralModes(Pio_Peripheral_Can, Pio_PeripheralMode_Active);

  DgusVar_beginReadWrite();
  DgusVar_setAddressIncrement(1);

  message.address = Can_BaseAddress;
  message.length = 4;

//  message.content.bytes[0] = 0x3f;
//  message.content.bytes[1] = 0x40;
//  message.content.bytes[2] = 0x72;
//  message.content.bytes[3] = 0x00;
  message.content.bytes[0] = config.baud_rate_frequency_divider;
  message.content.bytes[1] = config.BTR0;
  message.content.bytes[2] = config.BTR1;
  message.content.bytes[3] = 0;
  DgusVar_write(message);

//  DATA3 = 0;
//  DATA2 = 0;
//  DATA1 = 0x42;
//  DATA0 = 0xc7;       //Ťփѩʕ¼Ĵ憷ACR
  message.content.variables[0] = (filter.acceptance_code >> 16) & 0xFFFF;
  message.content.variables[1] = (filter.acceptance_code >>  0) & 0xFFFF;
  DgusVar_continueWrite(message.length, message.content);

//  DATA3 = 0x77;
//  DATA2 = 0xff;
//  DATA1 = 0xbd;
//  DATA0 = 0x00;     //ŤփAMR
  message.content.variables[0] = (filter.acceptance_mask >> 16) & 0xFFFF;
  message.content.variables[1] = (filter.acceptance_mask >>  0) & 0xFFFF;
  DgusVar_continueWrite(message.length, message.content);

  DgusVar_endReadWrite();

  Can_configure();
  Can_enable();

  Interrupt_CanHandler = Can_handleInterruption;

  Interrupt_enableInterruption(Interrupt_Interruption_Can);//´򿪃AN֐¶ω
}

void Can_resetError(void)
{
  u8 error = Can_error();
  if (!(error & Can_Error_Passive)) return;

  Can_setError(error & ~Can_Error_Passive);
  Can_softwareReset();
  Can_CanBus.tx_flag = 0;
}

Can_Message_t Can_rx(void)
{
  Can_Message_t message;

  if (Can_CanBus.rx.head == Can_CanBus.rx.tail)
  {
    message.status = 0;
    return message;
  }

  message = Can_CanBus.rx.messages[Can_CanBus.rx.tail];
  message.status = 1;

  Can_moveBufferTail(&Can_CanBus.rx);

  return message;
}

void Can_tx(u32 id, u8 status, u16 n_bytes, const u8 *bytes)
{
  u8 i, j, k, frame_no, frame_offset;
  u32 id_temp, status_temp;
  Can_Message_t *tx_message;

  if (2048 < n_bytes) return;

  if (status & Bits_Bit8_7)
  {
    id_temp = id << 3;
  }
  else
  {
    id_temp = id << 21;
  }

  tx_message = Can_CanBus.tx.messages + Can_CanBus.tx.head;
  if (tx_message->status & Bits_Bit8_6)
  {
    tx_message->status = status & (Bits_Bit8_7 | Bits_Bit8_6);
    tx_message->id = id_temp;
    Can_moveBufferHead(&Can_CanBus.tx);
  }
  else
  {
    frame_no = n_bytes >> 3;
    frame_offset = n_bytes % 8;
    k = 0;
    status_temp = status & (Bits_Bit8_7 | Bits_Bit8_6);
    for (i = 0; i < frame_no; ++i)
    {
      tx_message = Can_CanBus.tx.messages + Can_CanBus.tx.head;
      tx_message->status = status_temp | Bits_Bit8_3;
      tx_message->id = id_temp;
      for (j = 0; j < 8; ++j)
      {
        tx_message->bytes[j] = bytes[k];
        ++k;
      }
      Can_moveBufferHead(&Can_CanBus.tx);
    }
    if (frame_offset)
    {
      tx_message = Can_CanBus.tx.messages + Can_CanBus.tx.head;
      tx_message->status = status_temp | frame_offset;
      tx_message->id = id_temp;
      for (j = 0; j < frame_offset; ++j)
      {
        tx_message->bytes[j] = bytes[k];
        ++k;
      }
      for (; j < 8; ++j)
      {
        tx_message->bytes[j] = 0;
      }
      Can_moveBufferHead(&Can_CanBus.tx);
    }
  }
  if (!Can_CanBus.tx_flag)
  {
    Can_loadTxMessage();
    Can_CanBus.tx_flag = 1;
    Can_startTimer();
    Can_sendMessage();
  }
  if (Can_CanBus.tx_flag && Timer_timeout(CAN__TIMER_ID))
  {
    Can_CanBus.tx_flag = 0;
  }
}


// internal implementation

// CAN_CR.3:
// 0 = single (filter size = 32 bit)
// 1 = double (filter size = 16 bit)
void Can_resetControl(void)
{
  CAN_CR = 0;
}

void Can_configure(void)
{
  CAN_CR |= Bits_Bit8_5;
  while(CAN_CR & Bits_Bit8_5);  //ִАŤփFF0060-FF0062¶¯׷
}

void Can_enable(void)
{
  CAN_CR |= Bits_Bit8_7;
}

void Can_handleInterruption(void)
{
  u8 interrupt_flag = Interrupt_enabled();
  Interrupt_disable();

  if (Interrupt_canStatus() & Interrupt_CanStatus_RemoteFrame)
  {
    // @attention У китайцев сбрасывается два бита, но это не похоже на обработку остальных сатусов
    Interrupt_setCanStatus(
      Interrupt_canStatus() & ~(Interrupt_CanStatus_RemoteFrame | Interrupt_CanStatus_Recieve)
    );
  }
  if (
    Interrupt_canStatus() & Interrupt_CanStatus_Recieve
  )
  {
    DgusVar_Message_t message;
    Can_Message_t *rx_message = Can_CanBus.rx.messages + Can_CanBus.rx.head;
    u8 i;

    Interrupt_setCanStatus(
      Interrupt_canStatus() & ~Interrupt_CanStatus_Recieve
    );

    DgusVar_beginReadWrite();
    DgusVar_setAddressIncrement(1);

    message.address = Can_BaseAddress;
    message.address.bytes.low += 8;
    message.length = 1;

    message.content = DgusVar_read(message.address, message.length);
    rx_message->status = message.content.bytes[0];

    message.length = 4;
    message.content = DgusVar_continueRead(message.length);

    for (i = 0; i < 4; ++i)
    {
      rx_message->id <<= 8;
      rx_message->id |= message.content.bytes[i];
    }
    rx_message->id >>= 3;

    if (!(rx_message->status & Bits_Bit8_7))
    {
      rx_message->id >>= 18;
    }
    if (!(rx_message->status & Bits_Bit8_6))
    {
      message.length = 4;
      message.content = DgusVar_continueRead(message.length);
      for (i = 0; i < 4; ++i)
      {
        rx_message->bytes[i] = message.content.bytes[i];
      }

      message.length = 4;
      message.content = DgusVar_continueRead(message.length);
      for (i = 4; i < 8; ++i)
      {
        rx_message->bytes[i] = message.content.bytes[i - 4];
      }
    }

    DgusVar_endReadWrite();

    Can_moveBufferHead(&Can_CanBus.rx);
  }
  if (
    Interrupt_canStatus() & Interrupt_CanStatus_Send
  )
  {
    Interrupt_setCanStatus(
      Interrupt_canStatus() & ~Interrupt_CanStatus_Send
    );
    if(Can_CanBus.tx.tail != Can_CanBus.tx.head)
    {
      Can_loadTxMessage();
      Can_sendMessage();
      Can_startTimer();
    }
    else
    {
      Can_CanBus.tx_flag = 0;
    }
  }
  if (
    Interrupt_canStatus() & Interrupt_CanStatus_Overflow
  )
  {
    Interrupt_setCanStatus(
      Interrupt_canStatus() & ~Interrupt_CanStatus_Overflow
    );
  }
  if (
    Interrupt_canStatus() & Interrupt_CanStatus_Error
  )
  {
    Interrupt_setCanStatus(
      Interrupt_canStatus() & ~Interrupt_CanStatus_Error
    );
  }
  if (
    Interrupt_canStatus() & Interrupt_CanStatus_SendArbitrationFailure
  )
  {
    Interrupt_setCanStatus(
      Interrupt_canStatus() & ~Interrupt_CanStatus_SendArbitrationFailure
    );
    Can_sendMessage();
  }
  Can_setError(0);

  Interrupt_restore(interrupt_flag);
}

u8 Can_error(void)
{
  return CAN_ET;
}

void Can_setError(u8 error)
{
  CAN_ET = error;
}

void Can_softwareReset(void)
{
  CAN_CR |= 1 << 6;
  sys_delay_ms(1);
  CAN_CR &= ~(1 << 6);
}

void Can_loadTxMessage(void)
{
  DgusVar_Message_t message;
  Can_Message_t *can_message = Can_CanBus.tx.messages + Can_CanBus.tx.tail;
  u8 i;

  DgusVar_beginReadWrite();
  DgusVar_setAddressIncrement(1);

  message.address = Can_BaseAddress;
  message.address.bytes.low += 4;
  message.length = 4;

  message.content.bytes[0] = can_message->status;
  message.content.bytes[1] = 0;
  message.content.bytes[2] = 0;
  message.content.bytes[3] = 0;
  DgusVar_write(message);

  message.content.bytes[0] = (u8) (Can_CanBus.tx.messages[Can_CanBus.tx.tail].id >> 24);
  message.content.bytes[1] = (u8) (Can_CanBus.tx.messages[Can_CanBus.tx.tail].id >> 16);
  message.content.bytes[2] = (u8) (Can_CanBus.tx.messages[Can_CanBus.tx.tail].id >>  8);
  message.content.bytes[3] = (u8) (Can_CanBus.tx.messages[Can_CanBus.tx.tail].id >>  0);
  DgusVar_continueWrite(message.length, message.content);

  for (i = 0; i < 4; ++i) message.content.bytes[i] = can_message->bytes[i];
  DgusVar_continueWrite(message.length, message.content);

  for (i = 4; i < 8; ++i) message.content.bytes[i - 4] = can_message->bytes[i];
  DgusVar_continueWrite(message.length, message.content);

  DgusVar_endReadWrite();

  Can_moveBufferTail(&Can_CanBus.tx);
}

void Can_sendMessage(void)
{
  CAN_CR |= Bits_Bit8_2;
}

void Can_moveBufferHead(Can_BusBuffer_t *buffer)
{
  buffer->head += 1;
  buffer->head %= CAN__BUFFER_SIZE;
}

void Can_moveBufferTail(Can_BusBuffer_t *buffer)
{
  buffer->tail += 1;
  buffer->tail %= CAN__BUFFER_SIZE;
}

void Can_startTimer(void)
{
  Timer_start(CAN__TIMER_ID, CAN__TIMER_DURATION);
}
