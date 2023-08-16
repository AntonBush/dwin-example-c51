#include "driver/dgusvar_io.h"

#include "driver/dgusvar.h"

u8 DgusVar_getU8(u32 variable_address)
{
  return DgusVar_getU16(variable_address) >> 8;
}

u16 DgusVar_getU16(u32 variable_address)
{
  DgusVar_BufferPointer_t rx_buffer = DgusVar_RxBuffer;

  DgusVar_read(variable_address, 1);

  return DGUSVAR__READ_U16_FROM_BUFFER(rx_buffer);
}

u32 DgusVar_getU32(u32 variable_address)
{
  DgusVar_BufferPointer_t rx_buffer = DgusVar_RxBuffer;

  DgusVar_read(variable_address, 2);

  return DGUSVAR__READ_U32_FROM_BUFFER(rx_buffer);
}

s8 DgusVar_getS8(u32 variable_address)
{
  u8 uvalue = DgusVar_getU8(variable_address);
  return *((s8 *)(&uvalue));
}

s16 DgusVar_getS16(u32 variable_address)
{
  u16 uvalue = DgusVar_getU16(variable_address);
  return *((s16 *)(&uvalue));
}

s32 DgusVar_getS32(u32 variable_address)
{
  u32 uvalue = DgusVar_getU32(variable_address);
  return *((s32 *)(&uvalue));
}

float DgusVar_getFloat(u32 variable_address)
{
  u32 uvalue = DgusVar_getU32(variable_address);
  return *((float *)(&uvalue));
}

void DgusVar_updateU8(u32 variable_address, u8 value)
{
  DgusVar_updateU16(variable_address, value << 8);
}

void DgusVar_updateU16(u32 variable_address, u16 value)
{
  DgusVar_BufferPointer_t tx_buffer = DgusVar_TxBuffer;

  DGUSVAR__WRITE_U16_TO_BUFFER(tx_buffer, value);

  DgusVar_write(variable_address, 1);
}

void DgusVar_updateU32(u32 variable_address, u32 value)
{
  DgusVar_BufferPointer_t tx_buffer = DgusVar_TxBuffer;

  DGUSVAR__WRITE_U32_TO_BUFFER(tx_buffer, value);

  DgusVar_write(variable_address, 2);
}

void DgusVar_updateS8(u32 variable_address, s8 value)
{
  u8 uvalue = *((u8 *)(&value));
  DgusVar_updateU8(variable_address, uvalue);
}

void DgusVar_updateS16(u32 variable_address, s16 value)
{
  u16 uvalue = *((u16 *)(&value));
  DgusVar_updateU16(variable_address, uvalue);
}

void DgusVar_updateS32(u32 variable_address, s32 value)
{
  u32 uvalue = *((u32 *)(&value));
  DgusVar_updateU32(variable_address, uvalue);
}

void DgusVar_updateFloat(u32 variable_address, float value)
{
  u32 uvalue = *((u32 *)(&value));
  DgusVar_updateU32(variable_address, uvalue);
}
