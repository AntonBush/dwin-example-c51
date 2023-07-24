#ifndef __CAN_H__
#define __CAN_H__

#include "driver/int.h"

typedef struct Can_Message
{
  u8 status;
  u32 id;
  u8 bytes[8];
} Can_Message_t;

#define CAN__BUFFER_SIZE 256

typedef struct Can_BusBuffer
{
  Can_Message_t messages[CAN__BUFFER_SIZE];
  u8 head;
  u8 tail;
} Can_BusBuffer_t;

typedef struct Can_Bus
{
  Can_BusBuffer_t rx;
  Can_BusBuffer_t tx;
  u8 tx_flag;
} Can_Bus_t;

// RX only
typedef struct Can_Bus8283
{
  u8 buffer[CAN__BUFFER_SIZE];
  u16 head;
  u16 tail;
} Can_Bus8283_t;

typedef struct Can_InitConfig
{
  u8 baud_rate_frequency_divider;
  u8 BTR0;
  u8 BTR1;
} Can_InitConfig_t;

typedef struct Can_AcceptanceFilter
{
  u32 acceptance_code;
  // '0' - match, '1' - ignore
  u32 acceptance_mask;
} Can_AcceptanceFilter_t;

void Can_init(Can_InitConfig_t config, Can_AcceptanceFilter_t filter);
void Can_resetError(void);
Can_Message_t Can_rx(void);
void Can_tx(u32 id, u8 status, u16 n_bytes, const u8 *bytes);

#endif
