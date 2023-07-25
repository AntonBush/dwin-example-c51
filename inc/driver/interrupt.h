#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include "lib/int.h"
#include "lib/bits.h"

typedef enum Interrupt_Type
{
  Interrupt_Type_0
  , Interrupt_Type_1
} Interrupt_Type_t;

typedef enum Interrupt_Interruption
{
  Interrupt_Interruption_External0 = (Interrupt_Type_0 << 8) | Bits_Bit8_0
  , Interrupt_Interruption_Timer0 = (Interrupt_Type_0 << 8) | Bits_Bit8_1
  , Interrupt_Interruption_External1 = (Interrupt_Type_0 << 8) | Bits_Bit8_2
  , Interrupt_Interruption_Timer1 = (Interrupt_Type_0 << 8) | Bits_Bit8_3
  , Interrupt_Interruption_Uart2 = (Interrupt_Type_0 << 8) | Bits_Bit8_4
  , Interrupt_Interruption_Timer2 = (Interrupt_Type_0 << 8) | Bits_Bit8_5
  , Interrupt_Interruption_Can = (Interrupt_Type_1 << 8) | Bits_Bit8_0
} Interrupt_Interruption_t;

typedef enum Interrupt_CanStatus
{
  Interrupt_CanStatus_SendArbitrationFailure = Bits_Bit8_2
  , Interrupt_CanStatus_Error = Bits_Bit8_3
  , Interrupt_CanStatus_Overflow = Bits_Bit8_4
  , Interrupt_CanStatus_Send = Bits_Bit8_5
  , Interrupt_CanStatus_Recieve = Bits_Bit8_6
  , Interrupt_CanStatus_RemoteFrame = Bits_Bit8_7
} Interrupt_CanStatus_t;

typedef void (*Interrupt_Handler)(void);

extern Interrupt_Handler Interrupt_Timer0Handler;
extern Interrupt_Handler Interrupt_CanHandler;

u8 Interrupt_enabled(void);

void Interrupt_enable(void);
void Interrupt_disable(void);
void Interrupt_restore(u8 enabled);

void Interrupt_enableInterruption(Interrupt_Interruption_t interruption);
void Interrupt_disableInterruption(Interrupt_Interruption_t interruption);

u8 Interrupt_canStatus(void);
void Interrupt_setCanStatus(u8 status);

#endif
