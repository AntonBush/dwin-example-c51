#include "driver/timer.h"

#include "driver/sys.h"
#include "lib/bits.h"

#define TIMER__ENABLE_INTERRUPT_TIMER_0() ET0 = 1

#define TIMER__MODE_TIMER_0(flags) ((flags) << 0)
#define TIMER__MODE_TIMER_1(flags) ((flags) << 4)

#define TIMER__START_TIMER_0() TR0 = 1
#define TIMER__STOP_TIMER_0() TR0 = 0

#define TIMER__SET_TIMER_0(value) TH0 = (value) >> 8; TL0 = (value)

typedef enum Timer_Mode
{
  TIMER__MODE_13BIT
  , TIMER__MODE_16BIT
  , TIMER__MODE_8BIT_AUTO_RELOAD
  , TIMER__MODE_DOUBLE_8BIT_TIMER
  , TIMER__MODE_COUNTER = BITS__BIT8_2
  , TIMER__MODE_GATE = BITS__BIT8_3
} Timer_Mode_t;

u8 data Timer_Tick = 0;

u8 data Timer_EnableFlags;
u8 data Timer_TimeoutFlags;
u16 data Timer_Durations[8];

void Timer_init(void)
{
  TIMER__STOP_TIMER_0();

  Timer_EnableFlags = 0;
  Timer_TimeoutFlags = 0;

  TMOD = (
    TIMER__MODE_TIMER_0(TIMER__MODE_16BIT)
    | TIMER__MODE_TIMER_1(TIMER__MODE_16BIT)
  );

  TIMER__SET_TIMER_0(T1MS);

  TIMER__ENABLE_INTERRUPT_TIMER_0();

  TIMER__START_TIMER_0();
}

void Timer_handleInterruption(void) small interrupt 1
{
  u8 i;

  SYS__DISABLE_INTERRUPT();

  Timer_Tick += 1;

  TIMER__SET_TIMER_0(T1MS);

  for(i = 0; i < 8; ++i)
  {
    u8 timer_id = 1 << i;
    if (!(Timer_EnableFlags & timer_id)) continue;

    Timer_Durations[i] -= 1;
    if(Timer_Durations[i]) continue;

    Timer_TimeoutFlags |= timer_id;
    Timer_EnableFlags &= ~timer_id;
  }

  SYS__ENABLE_INTERRUPT();
}

void Timer_start(u8 id, u16 time)
{
  u8 timer_id = 1 << id;

  Timer_EnableFlags = Timer_EnableFlags | timer_id;
  Timer_Durations[id] = time;
  Timer_TimeoutFlags &= ~timer_id;
}

u8 Timer_timeout(u8 id)
{
  u8 timer_id = 1 << id;
  u8 flag;

  flag = Timer_TimeoutFlags & timer_id;

  return flag;
}
