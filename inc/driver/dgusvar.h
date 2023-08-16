#ifndef __DGUSVAR_H__
#define __DGUSVAR_H__

#include "lib/int.h"
#include "lib/bits.h"

#define DGUSVAR__CAN_ADDRESS 0xFF0060UL
#define DGUSVAR__BUFFER_SIZE 512

#define DGUSVAR__WRITE_U8_TO_BUFFER(ptr, val) \
*(ptr)++ = (val)
#define DGUSVAR__WRITE_U16_TO_BUFFER(ptr, val) \
*((u16 xdata *)(ptr))++ = (val)
#define DGUSVAR__WRITE_U32_TO_BUFFER(ptr, val) \
*((u32 xdata *)(ptr))++ = (val)

#define DGUSVAR__READ_U8_FROM_BUFFER(ptr) \
(*(ptr)++)
#define DGUSVAR__READ_U16_FROM_BUFFER(ptr) \
(*((u16 xdata *)(ptr))++)
#define DGUSVAR__READ_U32_FROM_BUFFER(ptr) \
(*((u32 xdata *)(ptr))++)

#define DGUSVAR__FILLED_BUFFER_SIZE(ptr, buffer) \
(( \
  ((ptr) - (buffer)) + (((ptr) - (buffer)) & BITS__BIT8_0) \
) >> 1)

typedef u8 xdata * DgusVar_BufferPointer_t;

extern u8 xdata DgusVar_RxBuffer[DGUSVAR__BUFFER_SIZE];
extern u8 xdata DgusVar_TxBuffer[DGUSVAR__BUFFER_SIZE];

void DgusVar_read(u32 variable_address, u16 n_variables) small;
void DgusVar_write(u32 variable_address, u16 n_variables) small;

#endif
