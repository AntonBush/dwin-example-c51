#ifndef __SYS_H__
#define __SYS_H__

#include "device/board.h"
#include "lib/int.h"

// Определение основной частоты системы и значения времени 1 мс
#define FOSC            206438400UL
#define T1MS            (65536-FOSC/12/1000)

// определение макросов
#define WDT_ON()        MUX_SEL|=0x02    // включить сторожевой таймер
#define WDT_OFF()       MUX_SEL&=0xFD    // отключить сторожевой таймер
#define WDT_RST()       MUX_SEL|=0x01    // сбросить сторожевой таймер
#define GET_VP(invar)   (u8*)&invar      // возвращает указатель на переменную

#define ENABLE_INTERRUPT() EA = 1
#define DISABLE_INTERRUPT() EA = 0

#define INTERRUPT_GUARD(expression) \
DISABLE_INTERRUPT(); \
expression; \
ENABLE_INTERRUPT();

// определение функций
void sys_init(void);

void write_dgus_vp(u16 addr,u8* buf,u16 len);
void read_dgus_vp(u16 addr,u8* buf,u16 len);

#endif
