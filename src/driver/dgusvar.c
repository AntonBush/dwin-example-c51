#include "driver/dgusvar.h"

#include "driver/sys.h"
#include "lib/bits.h"

#include <math.h>

#define DGUSVAR__BEGIN_READ_WRITE() \
APP_REQ = 1; \
do {} while (!APP_ACK)

#define DGUSVAR__END_READ_WRITE() \
RAMMODE = 0;

#define DGUSVAR__SET_ADDRESS(address) \
ADR_H = (address) >> 16; \
ADR_M = (address) >>  8; \
ADR_L = (address) >>  0

#define DGUSVAR__SET_ADDRESS_INCREMENT(adr_inc) \
ADR_INC = (adr_inc)

#define DGUSVAR__SELECT_FIRST_BYTES(count) \
RAMMODE = RAMMODE & 0xF0 | (16 - (1 << (4 - (count))))

#define DGUSVAR__SELECT_LAST_BYTES(count) \
RAMMODE = RAMMODE & 0xF0 | ((1 << (count)) - 1)

#define DGUSVAR__PERFORM_OPERATION(mode) \
APP_RW = (mode); \
APP_EN = 1; \
do {} while (APP_EN)

typedef enum DgusVar_Mode
{
  DgusVar_Mode_Read = 1
  , DgusVar_Mode_Write = 0
} DgusVar_Mode_t;

u8 xdata DgusVar_RxBuffer[DGUSVAR__BUFFER_SIZE];
u8 xdata DgusVar_TxBuffer[DGUSVAR__BUFFER_SIZE];

void DgusVar_read(u32 variable_address, u16 n_variables) small
{
  DgusVar_BufferPointer_t rx_buffer = DgusVar_RxBuffer;

  if(n_variables == 0) return;

  if (DGUSVAR__BUFFER_SIZE < n_variables)
  {
    n_variables = DGUSVAR__BUFFER_SIZE;
  }

  DGUSVAR__SET_ADDRESS(variable_address >> 1);
  DGUSVAR__SET_ADDRESS_INCREMENT(1);

  DGUSVAR__BEGIN_READ_WRITE();

  if (variable_address & Bits_Bit8_0)
  {
    DGUSVAR__SELECT_LAST_BYTES(2);
    DGUSVAR__PERFORM_OPERATION(DgusVar_Mode_Read);

    DGUSVAR__WRITE_U8_TO_BUFFER(rx_buffer, DATA1);
    DGUSVAR__WRITE_U8_TO_BUFFER(rx_buffer, DATA0);
    --n_variables;
  }

  DGUSVAR__SELECT_FIRST_BYTES(4);
  for (; 1 < n_variables; n_variables -= 2)
  {
    DGUSVAR__PERFORM_OPERATION(DgusVar_Mode_Read);

    DGUSVAR__WRITE_U8_TO_BUFFER(rx_buffer, DATA3);
    DGUSVAR__WRITE_U8_TO_BUFFER(rx_buffer, DATA2);
    DGUSVAR__WRITE_U8_TO_BUFFER(rx_buffer, DATA1);
    DGUSVAR__WRITE_U8_TO_BUFFER(rx_buffer, DATA0);
  }

  if (n_variables != 0)
  {
    DGUSVAR__SELECT_FIRST_BYTES(2);
    DGUSVAR__PERFORM_OPERATION(DgusVar_Mode_Read);

    DGUSVAR__WRITE_U8_TO_BUFFER(rx_buffer, DATA3);
    DGUSVAR__WRITE_U8_TO_BUFFER(rx_buffer, DATA2);
  }

  DGUSVAR__END_READ_WRITE();
}

void DgusVar_write(u32 variable_address, u16 n_variables) small
{
  DgusVar_BufferPointer_t tx_buffer = DgusVar_TxBuffer;

  if(n_variables == 0) return;

  if (DGUSVAR__BUFFER_SIZE < n_variables)
  {
    n_variables = DGUSVAR__BUFFER_SIZE;
  }

  DGUSVAR__SET_ADDRESS(variable_address >> 1);
  DGUSVAR__SET_ADDRESS_INCREMENT(1);

  DGUSVAR__BEGIN_READ_WRITE();

  if (variable_address & Bits_Bit8_0)
  {
    DATA1 = DGUSVAR__READ_U8_FROM_BUFFER(tx_buffer);
    DATA0 = DGUSVAR__READ_U8_FROM_BUFFER(tx_buffer);

    DGUSVAR__SELECT_LAST_BYTES(2);
    DGUSVAR__PERFORM_OPERATION(DgusVar_Mode_Write);

    --n_variables;
  }

  DGUSVAR__SELECT_FIRST_BYTES(4);
  for (; 1 < n_variables; n_variables -= 2)
  {
    DATA3 = DGUSVAR__READ_U8_FROM_BUFFER(tx_buffer);
    DATA2 = DGUSVAR__READ_U8_FROM_BUFFER(tx_buffer);
    DATA1 = DGUSVAR__READ_U8_FROM_BUFFER(tx_buffer);
    DATA0 = DGUSVAR__READ_U8_FROM_BUFFER(tx_buffer);

    DGUSVAR__PERFORM_OPERATION(DgusVar_Mode_Write);
  }

  if (n_variables != 0)
  {
    DATA3 = DGUSVAR__READ_U8_FROM_BUFFER(tx_buffer);
    DATA2 = DGUSVAR__READ_U8_FROM_BUFFER(tx_buffer);

    DGUSVAR__SELECT_FIRST_BYTES(2);
    DGUSVAR__PERFORM_OPERATION(DgusVar_Mode_Write);
  }

  DGUSVAR__END_READ_WRITE();
}
