
/******************************************************************************

                  ��Ȩ���� (C), 2019, �������ĿƼ����޹�˾

 ******************************************************************************
  �� �� ��   : sys.h
  �� �� ��   : V1.0
  ��    ��   : chenmeishu
  ��������   : 2019.9.2
  ��������   : 
  �޸���ʷ   :
  1.��    ��   : 
    ��    ��   : 
    �޸�����   : 
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
