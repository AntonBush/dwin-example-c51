#include "logic/inc/driver/dgusvar.h"

#include "driver/sys.h"
#include "logic/inc/driver/interrupt.h"

typedef enum DgusVar_Mode
{
  DgusVar_Mode_Read = 1
  , DgusVar_Mode_Write = 0
} DgusVar_Mode_t;

static void DgusVar_setAddress(DgusVar_Address_t address);
static void DgusVar_selectBytes(u8 count);
static void DgusVar_performOperation(DgusVar_Mode_t mode);

DgusVar_Address_t DgusVar_newAddress(u32 address)
{
  DgusVar_Address_t new_address;
  new_address.bytes.high = address >> 16;
  new_address.bytes.middle = address >> 8;
  new_address.bytes.low = address >> 0;
  return new_address;
}

DgusVar_Address_t DgusVar_newVpAddress(u16 address)
{
  DgusVar_Address_t new_address;
  new_address.bytes.high = 0;
  new_address.bytes.middle = address >> 9;
  new_address.bytes.low = address >> 1;
  return new_address;
}

DgusVar_Data_t DgusVar_newData(u32 content)
{
  DgusVar_Data_t new_data;
  new_data.variables[1] = content >> 16;
  new_data.variables[0] = content >> 0;
  return new_data;
}

DgusVar_Data_t DgusVar_newRawData(u8 *bytes, u8 length)
{
  DgusVar_Data_t new_data;
  u8 i;
  for (i = 0; i < 4; ++i)
  {
    new_data.bytes[i] = i < length ? bytes[i] : 0;
  }
  return new_data;
}

DgusVar_Data_t DgusVar_newVariableData(u16 *variables, u8 count)
{
  DgusVar_Data_t new_data;
  u8 i;
  for (i = 0; i < 2; ++i)
  {
    new_data.variables[i] = i < count ? variables[i] : 0;
  }
  return new_data;
}

DgusVar_Message_t DgusVar_newMessage(
  DgusVar_Address_t address
  , u8 length
  , DgusVar_Data_t content
)
{
  DgusVar_Message_t new_message;
  new_message.address = address;
  new_message.length = length;
  new_message.content = content;
  return new_message;
}

void DgusVar_beginReadWrite(void)
{
  APP_REQ = 1;
  while (!APP_ACK);
}

void DgusVar_endReadWrite(void)
{
  RAMMODE = 0;
}

void DgusVar_setAddressIncrement(u8 increment)
{
  ADR_INC = increment;
}

DgusVar_Data_t DgusVar_read(DgusVar_Address_t address, u8 length)
{
  DgusVar_setAddress(address);
  return DgusVar_continueRead(length);
}

void DgusVar_write(DgusVar_Message_t message)
{
  DgusVar_setAddress(message.address);
  DgusVar_continueWrite(message.length, message.content);
}

DgusVar_Data_t DgusVar_continueRead(u8 length)
{
  DgusVar_Data_t read_data;

  DgusVar_selectBytes(length);
  DgusVar_performOperation(DgusVar_Mode_Read);

  read_data.bytes[0] = DATA3;
  read_data.bytes[1] = DATA2;
  read_data.bytes[2] = DATA1;
  read_data.bytes[3] = DATA0;

  return read_data;
}

void DgusVar_continueWrite(u8 length, DgusVar_Data_t content)
{
  DATA3 = content.bytes[0];
  DATA2 = content.bytes[1];
  DATA1 = content.bytes[2];
  DATA0 = content.bytes[3];

  DgusVar_selectBytes(length);
  DgusVar_performOperation(DgusVar_Mode_Write);
}

void DgusVar_setAddress(DgusVar_Address_t address)
{
  ADR_H = address.bytes.high;
  ADR_M = address.bytes.middle;
  ADR_L = address.bytes.low;
}

void DgusVar_selectBytes(u8 count)
{
  u8 byte_mask = 0;

  if (4 < count) count = 4;

  for (; count != 0; --count)
  {
    byte_mask >>= 1;
    byte_mask |= Bits_Bit8_3;
  }

  RAMMODE = (RAMMODE & 0xF0) | byte_mask;
}

void DgusVar_performOperation(DgusVar_Mode_t mode)
{
  u8 interrupt_flag = EA;//Interrupt_enabled();
  EA = 0;//Interrupt_disable();

  APP_RW = mode;
  APP_EN = 1;
  while (APP_EN);

  if (interrupt_flag) EA = 1;//Interrupt_restore(interrupt_flag);
}
