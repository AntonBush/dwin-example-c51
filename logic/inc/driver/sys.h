#ifndef __SYS_H__
#define __SYS_H__

#include "device/board.h"
#include "driver/int.h"

// определение макросов
#define WDT_ON()        MUX_SEL|=0x02    // включить сторожевой таймер
#define WDT_OFF()       MUX_SEL&=0xFD    // отключить сторожевой таймер
#define WDT_RST()       MUX_SEL|=0x01    // сбросить сторожевой таймер
#define GET_VP(invar)   (u8*)&invar      // возвращает указатель на переменную


// Определение основной частоты системы и значения времени 1 мс
#define FOSC            206438400UL
#define T1MS            (65536-FOSC/12/1000)

// определение функций
void sys_init(void);
void sys_delay_about_ms(u16 ms);
void sys_delay_about_us(u8 us);
void sys_delay_ms(u16 ms);
void sys_read_vp(u16 addr,u8* buf,u16 len);
void sys_write_vp(u16 addr,u8* buf,u16 len);

#endif


