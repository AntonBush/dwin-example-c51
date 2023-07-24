#include "driver/timer.h"

#include "driver/sys.h"
#include "driver/interrupt.h"

#define TIMER__MODE_TIMER_0(flags) ((flags) << 0)
#define TIMER__MODE_TIMER_1(flags) ((flags) << 4)

typedef enum Timer_Mode
{
  Timer_Mode_13bit
  , Timer_Mode_16Bit
  , Timer_Mode_8BitAutoReload
  , Timer_Mode_Double8BitTimer
  , Timer_Mode_Counter = Bits_Bit8_2
  , Timer_Mode_Gate = Bits_Bit8_3
} Timer_Mode_t;

u32 data Timer_Tick = 0;

//עӢú޻ֹ˽ؔ؃ϊ֢ЩҤìֻŜԃޓࠚگ˽؃ϊ
u8 data Timer_EnableFlags;   //8ٶɭ֨ʱǷˇرʹŜ
u8 data Timer_TimeoutFlags;   //8ٶɭ֨ʱǷˇرӬʱ
u16 data Timer_Durations[8];  //8ٶɭ֨ʱǷ֨ʱʱݤ

static void Timer_setTimer0(u16 value);
static void Timer_startTimer0(void);
static void Timer_stopTimer0(void);
static void Timer_handleInterruption(void);

void Timer_init(void)
{
  TMOD = (
    TIMER__MODE_TIMER_0(Timer_Mode_16Bit)
    | TIMER__MODE_TIMER_1(Timer_Mode_16Bit)
  );

  Timer_setTimer0(0);
  Timer_stopTimer0();

  Timer_TimeoutFlags = 0;
  Timer_EnableFlags = 0;

  TMOD |= TIMER__MODE_TIMER_0(Timer_Mode_16Bit);
  Timer_setTimer0(T1MS);

  Interrupt_Timer0Handler = Timer_handleInterruption;
  
  Interrupt_enableInterruption(Interrupt_Interruption_Timer0);

  Timer_startTimer0();
}

void Timer_start(u8 id, u16 time)
{
  u8 interrupt_flag = Interrupt_enabled();
  u8 timer_id;
  Interrupt_disable();
  
  if (7 < id) id = 7;
  if (time == 0) time = 1;
  timer_id = 1 << id;

  Timer_EnableFlags |= timer_id;
  Timer_Durations[id] = time;
  Timer_TimeoutFlags &= ~timer_id;

  Interrupt_restore(interrupt_flag);
}

u8 Timer_timeout(u8 id)
{
  u8 interrupt_flag = Interrupt_enabled();
  u8 flag;
  Interrupt_disable();

  flag = Timer_TimeoutFlags & (1 << id);

  Interrupt_restore(interrupt_flag);
  return flag;
}

void Timer_handleInterruption(void)
{
  u8 interrupt_flag = Interrupt_enabled();
  u8 data i;
  Interrupt_disable();

  Timer_setTimer0(T1MS);
  ++Timer_Tick;

  for(i = 1; i < 8; ++i)
  {
    u8 timer_id = 1 << i;
    if (!(Timer_EnableFlags & timer_id)) continue;

    --Timer_Durations[i];
    if(Timer_Durations[i] == 0)
    {
      Timer_TimeoutFlags |= timer_id;
      Timer_EnableFlags &= ~timer_id;
    }
  }

  Interrupt_restore(interrupt_flag);
}

void Timer_setTimer0(u16 value)
{
  TH0 = (value >> 8) & 0xFF;
  TL0 = (value >> 0) & 0xFF;
}

void Timer_startTimer0(void)
{
  TR0 = 1;
}

void Timer_stopTimer0(void)
{
  TR0 = 0;
}
