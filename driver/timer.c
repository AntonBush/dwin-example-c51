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
  data u8 i;

  DISABLE_INTERRUPT();

  Timer_Tick += 1;

  TIMER__SET_TIMER_0(T1MS);

  for(i = 0; i < 8; ++i)
  {
    data u8 timer_id = 1 << i;
    if (!(Timer_EnableFlags & timer_id)) continue;

    Timer_Durations[i] -= 1;
    if(Timer_Durations[i]) continue;

    Timer_TimeoutFlags |= timer_id;
    Timer_EnableFlags &= ~timer_id;
  }

  ENABLE_INTERRUPT();
}

void Timer_start(u8 id, u16 time)
{
  data u8 timer_id = 1 << id;

  DISABLE_INTERRUPT();

  Timer_EnableFlags = Timer_EnableFlags | timer_id;
  Timer_Durations[id] = time;
  Timer_TimeoutFlags &= ~timer_id;

  ENABLE_INTERRUPT();
}

u8 Timer_timeout(u8 id)
{
  data u8 timer_id = 1 << id;
  data u8 flag;

  DISABLE_INTERRUPT();

  flag = Timer_TimeoutFlags & timer_id;

  ENABLE_INTERRUPT();

  return flag;
}
