#ifndef __DGUSVAR_H__
#define __DGUSVAR_H__

#include "lib/int.h"

#define DGUSVAR__CAN_BASE_ADDRESS_H 0xFF
#define DGUSVAR__CAN_BASE_ADDRESS_M 0x00
#define DGUSVAR__CAN_BASE_ADDRESS_L 0x60

typedef union DgusVar_Address
{
  u8 raw[3];
  struct DgusVar_Address_Bytes
  {
    u8 high;
    u8 middle;
    u8 low;
  } bytes;
} DgusVar_Address_t;

typedef union DgusVar_Data
{
  u8 bytes[4];
  u16 variables[2];
} DgusVar_Data_t;

typedef struct DgusVar_Message
{
  DgusVar_Address_t address;
  u8 length;
  DgusVar_Data_t content;
} DgusVar_Message_t;

DgusVar_Address_t DgusVar_newAddress(u32 address);
DgusVar_Address_t DgusVar_newVpAddress(u16 address);

DgusVar_Data_t DgusVar_newData(u32 content);
DgusVar_Data_t DgusVar_newRawData(u8 *bytes, u8 length);
DgusVar_Data_t DgusVar_newVariableData(u16 *variables, u8 count);

DgusVar_Message_t DgusVar_newMessage(
  DgusVar_Address_t address
  , u8 length
  , DgusVar_Data_t content
);

void DgusVar_beginReadWrite(void);
void DgusVar_endReadWrite(void);

void DgusVar_setAddressIncrement(u8 increment);

DgusVar_Data_t DgusVar_read(DgusVar_Address_t address, u8 length);
void DgusVar_write(DgusVar_Message_t message);

DgusVar_Data_t DgusVar_continueRead(u8 length);
void DgusVar_continueWrite(u8 length, DgusVar_Data_t content);

#endif
