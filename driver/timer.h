#ifndef __TIMER_H__
#define __TIMER_H__

#include "lib/int.h"

extern data u8 Timer_Tick;

void Timer_init(void);
void Timer_start(u8 id, u16 time);
u8 Timer_timeout(u8 id);

#endif
