#include "timer.h"

#include "sys.h"

u8 data EnableTimer;   //8����ʱ���Ƿ�ʹ��
u8 data OutTimeFlag;   //8����ʱ���Ƿ�ʱ
u16 data TimerTime[8];  //8����ʱ����ʱʱ��
u8 data SysTick;

/*****************************************************************************
 �� �� ��  : void T0_Init(void)
 ��������  : ��ʱ��0��ʼ��	��ʱ���1ms
 �������  :	 
 �������  : 
 �޸���ʷ  :
  1.��    ��   : 2019��9��2��
    ��    ��   :  ������
    �޸�����   : ����
*****************************************************************************/
//��ʱ��ʱ��Ϊϵͳʱ�ӵ�12��Ƶ��������趨��3.8ms
//���㹫ʽΪ(65536-n*FOSC/12),����n�ĵ�λΪ��
void Timer_init(void)
{
	  TMOD=0x11;          //16λ��ʱ��
    //T0
    TH0=0x00;
    TL0=0x00;
    TR0=0x00;
	
	  OutTimeFlag=0;
	  EnableTimer=0;
	
    TMOD|=0x01;
    TH0=T1MS>>8;        //1ms��ʱ��
    TL0=T1MS;
    ET0=1;              //������ʱ��0�ж�
//    EA=1;               //�����ж�
    TR0=1;              //������ʱ��
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
