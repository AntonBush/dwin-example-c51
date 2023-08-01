
/******************************************************************************

                  版权所有 (C), 2019, 北京迪文科技有限公司

 ******************************************************************************
  文 件 名   : sys.h
  版 本 号   : V1.0
  作    者   : chenmeishu
  生成日期   : 2019.9.2
  功能描述   : 
  修改历史   :
  1.日    期   : 
    作    者   : 
    修改内容   : 
******************************************************************************/
#ifndef __SYS_H__
#define __SYS_H__

#include "T5LOS8051.h"
#include "lib/int.h"

#define FOSC     206438400UL
#define T1MS    (65536-FOSC/12/1000)

#define ENABLE_INTERRUPT() EA = 1
#define DISABLE_INTERRUPT() EA = 0

void write_dgus_vp(u16 addr,u8* buf,u16 len);
void read_dgus_vp(u16 addr,u8* buf,u16 len);

void SetPinOut(u8 Port,u8 Pin);
void SetPinIn(u8 Port,u8 Pin);
void  PinOutput(u8 Port,u8 Pin,u8 value);

void delay_us(unsigned int t);
void delay_ms(u16 t);

#endif
