#include "timer.h"

#include "sys.h"

u8 data EnableTimer;   //8个软定时器是否使能
u8 data OutTimeFlag;   //8个软定时器是否超时
u16 data TimerTime[8];  //8个软定时器定时时间
u8 data SysTick;

/*****************************************************************************
 函 数 名  : void T0_Init(void)
 功能描述  : 定时器0初始化	定时间隔1ms
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年9月2日
    作    者   :  陈美书
    修改内容   : 创建
*****************************************************************************/
//定时器时钟为系统时钟的12分频，最大能设定到3.8ms
//计算公式为(65536-n*FOSC/12),其中n的单位为秒
void Timer_init(void)
{
	  TMOD=0x11;          //16位定时器
    //T0
    TH0=0x00;
    TL0=0x00;
    TR0=0x00;
	
	  OutTimeFlag=0;
	  EnableTimer=0;
	
    TMOD|=0x01;
    TH0=T1MS>>8;        //1ms定时器
    TL0=T1MS;
    ET0=1;              //开启定时器0中断
//    EA=1;               //开总中断
    TR0=1;              //开启定时器
}

void Timer_handleInterruption(void) interrupt 1
{
	 u8 data i;
	
    EA=0;
    TH0=T1MS>>8;
    TL0=T1MS;
		SysTick++;
		for(i=0;i<8;i++)
		{
			if(EnableTimer&(0x01<<i))
			{
				TimerTime[i]--;
				if(TimerTime[i]==0)
				{
					OutTimeFlag |= 0x01<<i;
					EnableTimer &= ~(0x01<<i); 					 
				}
			}
		}
    EA=1;
}

void Timer_start(u8 ID, u16 nTime)
{
	  EA=0;
	  EnableTimer=EnableTimer|(1<<ID);
	  TimerTime[ID]=nTime;
	  OutTimeFlag&=~(1<<ID);
	  EA=1; 
}

u8 Timer_timeout(u8 ID)
{
  u8 flag;
	EA=0;
	flag=OutTimeFlag&(1<<ID);
	EA=1;
	return flag;
}
