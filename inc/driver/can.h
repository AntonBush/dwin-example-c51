#ifndef __CAN_H__
#define __CAN_H__

#include "lib/int.h"
#include "lib/bits.h"

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
  Bits_State_t tx_flag;
} Can_Bus_t;

typedef struct Can_InitConfig
{
  u8 baud_rate_frequency_divider;
  u8 BTR0;
  u8 BTR1;
} Can_InitConfig_t;

typedef struct Can_Filter
{
  union Can_Filter_AcceptanceCode
  {
    u32 value;
    u8 bytes[4];
  } acceptance_code;
  // '0' - match, '1' - ignore
  union Can_Filter_AcceptanceMask
  {
    u32 value;
    u8 bytes[4];
  } acceptance_mask;
} Can_Filter_t;

void Can_init(
  const Can_InitConfig_t *config
  , const Can_Filter_t *filter
);
void Can_resetError(void) small;
u8 Can_rx(Can_Message_t *message);
void Can_tx(u8 status, u32 id, const u8 *bytes, u16 length);

#endif
