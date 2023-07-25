#include "driver/interrupt.h"

#include "driver/sys.h"
#include "lib/null.h"

typedef enum Interrupt_State
{
  Interrupt_State_Disabled
  , Interrupt_State_Enabled
} Interrupt_State_t;

static void Interrupt_setInterruption(
  Interrupt_Interruption_t interruption
  , Interrupt_State_t state
);

Interrupt_Handler Interrupt_Timer0Handler = NULL;
Interrupt_Handler Interrupt_CanHandler = NULL;

u8 Interrupt_enabled(void)
{
  return EA;
}

void Interrupt_enable(void)
{
  EA = 1;
}

void Interrupt_disable(void)
{
  EA = 0;
}

void Interrupt_restore(u8 enabled)
{
  if (enabled) Interrupt_enable();
  else Interrupt_disable();
}

void Interrupt_enableInterruption(Interrupt_Interruption_t interruption)
{
  Interrupt_setInterruption(interruption, Interrupt_State_Enabled);
}

void Interrupt_disableInterruption(Interrupt_Interruption_t interruption)
{
  Interrupt_setInterruption(interruption, Interrupt_State_Disabled);
}

u8 Interrupt_canStatus(void)
{
  return CAN_IR;
}

void Interrupt_setCanStatus(u8 status)
{
  CAN_IR = status;
}

//

void Interrupt_setInterruption(
  Interrupt_Interruption_t interruption
  , Interrupt_State_t state
)
{
  switch (interruption)
  {
    case Interrupt_Interruption_External0:
      EX0 = state;
      break;

    case Interrupt_Interruption_Timer0:
      ET0 = state;
      break;

    case Interrupt_Interruption_External1:
      EX1 = state;
      break;

    case Interrupt_Interruption_Timer1:
      ET1 = state;
      break;

    case Interrupt_Interruption_Uart2:
      ES0 = state;
      break;

    case Interrupt_Interruption_Timer2:
      ET2 = state;
      break;

    case Interrupt_Interruption_Can:
      ECAN = state;
      break;
  }
}

static void Interrupt_Timer0Isr(void) interrupt 1
{
  if (Interrupt_Timer0Handler)
  {
    Interrupt_Timer0Handler();
  }
}

static void Interrupt_CanIsr(void) interrupt 9
{
  if (Interrupt_CanHandler)
  {
    Interrupt_CanHandler();
  }
}
