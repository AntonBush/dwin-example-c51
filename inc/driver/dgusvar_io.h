#ifndef __DGUSVAR_IO_H__
#define __DGUSVAR_IO_H__

#include "lib/int.h"

u8 DgusVar_getU8(u32 variable_address);
u16 DgusVar_getU16(u32 variable_address);
u32 DgusVar_getU32(u32 variable_address);

s8 DgusVar_getS8(u32 variable_address);
s16 DgusVar_getS16(u32 variable_address);
s32 DgusVar_getS32(u32 variable_address);

float DgusVar_getFloat(u32 variable_address);

void DgusVar_updateU8(u32 variable_address, u8 value);
void DgusVar_updateU16(u32 variable_address, u16 value);
void DgusVar_updateU32(u32 variable_address, u32 value);

void DgusVar_updateS8(u32 variable_address, s8 value);
void DgusVar_updateS16(u32 variable_address, s16 value);
void DgusVar_updateS32(u32 variable_address, s32 value);

void DgusVar_updateFloat(u32 variable_address, float value);

#endif
