#include "sys.h"

/*****************************************************************************
 �� �� ��  : void read_dgus_vp(u32 addr,u8* buf,u16 len)
 ��������  : ��dgus��ַ��ֵ
 �������  :	 addr��dgus��ֵַ  len�������ݳ���
 �������  : buf�����ݱ��滺����
 �޸���ʷ  :
  1.��    ��   : 2019��4��2��
    ��    ��   : chengjing
    �޸�����   : ����
*****************************************************************************/
#define INTVPACTION
void read_dgus_vp(u16 addr, u8 *buf, u16 len)
{
	u16 OS_addr = 0;
	u16 OS_addr_offset = 0;
	u16 OS_len = 0, OS_len_offset = 0;
	u32 LenLimit;
	
	if(0==len)
		return;
	LenLimit = 0xffffU - addr + 1;
	if(LenLimit < len)
	{
		len = LenLimit;
	}
	OS_addr = addr >> 1;
	OS_addr_offset = addr & 0x01;
#ifdef INTVPACTION	
	EA = 0;
#endif
	ADR_H = 0;
	ADR_M = (u8)(OS_addr >> 8);
	ADR_L = (u8)OS_addr;
	ADR_INC = 1;	
	RAMMODE = 0xAF; 
	while (!APP_ACK);			
	if (OS_addr_offset)
	{
		APP_EN = 1;
		while (APP_EN);
		*buf++ = DATA1;
		*buf++ = DATA0;
		len--;
	}
	OS_len = len >> 1;
	OS_len_offset = len & 0x01;
	while (OS_len--)
	{
		APP_EN = 1;
		while (APP_EN);
		*buf++ = DATA3;
		*buf++ = DATA2;
		*buf++ = DATA1;
		*buf++ = DATA0;
	}
	if (OS_len_offset)
	{
		APP_EN = 1;
		while (APP_EN);
		*buf++ = DATA3;
		*buf++ = DATA2;
	}
	RAMMODE = 0x00;
#ifdef INTVPACTION	
	EA = 1;
#endif
}




/*****************************************************************************
 �� �� ��  : void write_dgus_vp(u32 addr,u8* buf,u16 len)
 ��������  : дdgus��ַ����
�������  :	 addr��д��ֵַ	buf��д������ݱ��滺���� len���ֳ���
 �������  : 
 �޸���ʷ  :
  1.��    ��   : 2019��4��2��
    ��    ��   : chengjing
    �޸�����   : ����
*****************************************************************************/
void write_dgus_vp(u16 addr, u8 *buf, u16 len)
{
	u16 OS_addr = 0;
	u16 OS_addr_offset = 0;
	u16 OS_len = 0,OS_len_offset = 0;
	u16 LenLimit;
	
	if(0==len)
		return;
	LenLimit = 0xffffU - addr + 1;
	if(LenLimit < len)
	{
		len = LenLimit;
	}
	OS_addr = addr >> 1;
	OS_addr_offset = addr & 0x01;
#ifdef INTVPACTION	
	EA = 0;
#endif
	ADR_H = 0;
	ADR_M = (u8)(OS_addr >> 8);
	ADR_L = (u8)OS_addr;
	ADR_INC = 0x01; 
	RAMMODE = 0x83;
	while (!APP_ACK);
	if (OS_addr_offset)
	{
		DATA1 = *buf++;
		DATA0 = *buf++;
		APP_EN = 1;
		while (APP_EN);
		len--;
	}
	OS_len = len >> 1;
	OS_len_offset = len & 0x01;
	RAMMODE = 0x8F;
	while (OS_len--)
	{
		DATA3 = *buf++;
		DATA2 = *buf++;
		DATA1 = *buf++;
		DATA0 = *buf++;
		APP_EN = 1;
		while (APP_EN);
	}
	if (OS_len_offset)
	{
		RAMMODE = 0x8C;
		DATA3 = *buf++;
		DATA2 = *buf++;
		APP_EN = 1;
		while (APP_EN);
	}
	RAMMODE = 0x00;
#ifdef INTVPACTION	
	EA = 1;
#endif
}
