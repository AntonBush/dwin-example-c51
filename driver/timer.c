#include "timer.h"

#include "sys.h"
#include "lib/bits.h"

#define TIMER__ENABLE_INTERRUPT_TIMER_0() ET0 = 1

#define TIMER__MODE_TIMER_0(flags) ((flags) << 0)
#define TIMER__MODE_TIMER_1(flags) ((flags) << 4)

#define TIMER__START_TIMER_0() TR0 = 1
#define TIMER__STOP_TIMER_0() TR0 = 0

#define TIMER__SET_TIMER_0(value) TH0 = (value) >> 8; TL0 = (value)

typedef enum Timer_Mode
{
  Timer_Mode_13bit
  , Timer_Mode_16Bit
  , Timer_Mode_8BitAutoReload
  , Timer_Mode_Double8BitTimer
  , Timer_Mode_Counter = Bits_Bit8_2
  , Timer_Mode_Gate = Bits_Bit8_3
} Timer_Mode_t;

data u8 Timer_Tick = 0;

data u8 Timer_EnableFlags;
data u8 Timer_TimeoutFlags;
data u16 Timer_Durations[8];

void Timer_init(void)
{
  TIMER__STOP_TIMER_0();

  Timer_TimeoutFlags = 0;
  Timer_EnableFlags = 0;

  TMOD = (
    TIMER__MODE_TIMER_0(Timer_Mode_16Bit)
    | TIMER__MODE_TIMER_1(Timer_Mode_16Bit)
  );

  TIMER__SET_TIMER_0(T1MS);

  TIMER__ENABLE_INTERRUPT_TIMER_0();

  TIMER__START_TIMER_0();
}

void Timer_handleInterruption(void) interrupt 1
{
   u8 data i;

    EA=0;
    TH0=T1MS>>8;
    TL0=T1MS;
    Timer_Tick++;
    for(i=0;i<8;i++)
    {
      if(Timer_EnableFlags&(0x01<<i))
      {
        Timer_Durations[i]--;
        if(Timer_Durations[i]==0)
        {
          Timer_TimeoutFlags |= 0x01<<i;
          Timer_EnableFlags &= ~(0x01<<i);
        }
      }
    }
    EA=1;
}

void Timer_start(u8 ID, u16 nTime)
{
    EA=0;
    Timer_EnableFlags=Timer_EnableFlags|(1<<ID);
    Timer_Durations[ID]=nTime;
    Timer_TimeoutFlags&=~(1<<ID);
    EA=1;
}

u8 Timer_timeout(u8 ID)
{
  u8 flag;
  EA=0;
  flag=Timer_TimeoutFlags&(1<<ID);
  EA=1;
  return flag;
}
