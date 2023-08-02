#ifndef __TIMER_H__
#define __TIMER_H__

#include "sys.h"
#include "lib/int.h"

extern data u8 Timer_Tick;

void Timer_init(void);
void Timer_start(u8 id, u16 time) compact;
u8 Timer_timeout(u8 id) compact;

#endif
