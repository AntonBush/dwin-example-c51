
/******************************************************************************

                  版权所有 (C), 2019, 北京迪文科技有限公司

 ******************************************************************************
  文 件 名   : sys.c
  版 本 号   : V1.0
  作    者   : chenmeishu
  生成日期   : 2019.9.2
  功能描述   : 
  修改历史   :
  1.日    期   : 
    作    者   : 
    修改内容   : 
******************************************************************************/
#include "sys.h"
#include "const.h"
//MNORFLASH Nor_Flash;
//MSPIFLASH Spi_Flash;
//MMUSIC MusicBuf;
//s16 PosXChangeSpeed,PosYChangeSpeed,RealPosX,RealPosY;
//s16 xdistance,ydistance,PressPosX,PressPosY,LastPosX,LastPosY;
//u16 PressPageID;
//u8 PressPosChange,Touchstate;

/*****************************************************************************
 函 数 名  : void read_dgus_vp(u32 addr,u8* buf,u16 len)
 功能描述  : 读dgus地址的值
 输入参数  :	 addr：dgus地址值  len：读数据长度
 输出参数  : buf：数据保存缓存区
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
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
 函 数 名  : void write_dgus_vp(u32 addr,u8* buf,u16 len)
 功能描述  : 写dgus地址数据
输入参数  :	 addr：写地址值	buf：写入的数据保存缓存区 len：字长度
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
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

/*****************************************************************************
 函 数 名  : void INIT_CPU(void)
 功能描述  : CPU初始化函数
			根据实际使用外设修改或单独配置
 输入参数  :	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月1日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/ 
//void INIT_CPU(void)
//{
//    EA=0;
//    RS0=0;
//    RS1=0;

//    CKCON=0x00;
//    T2CON=0x70;
//    DPC=0x00;
//    PAGESEL=0x01;
//    D_PAGESEL=0x02;   //DATA RAM  0x8000-0xFFFF
//    MUX_SEL=0x60;   //UART2,UART2开启，WDT关闭
//    RAMMODE=0x00;
//    PORTDRV=0x01;   //驱动强度+/-8mA
//    IEN0=0x00;      //关闭所有中断
//    IEN1=0x00;
//    IEN2=0x00;
//    IP0=0x00;      //中断优先级默认
//    IP1=0x00;
///*****************
//    WDT_OFF();      //关闭开门狗
//************************************/
//	 ClearRAM();									//初始化RAM为0
//}

void ClearRAM(void)
{
	u8 xdata*ptr;
	u16 data i;
	
	ptr = 0x8000;
	
	for(i=0;i<0x8000;i++)
		*ptr++ = 0;
}

//void ResetT5L(void)
//{
//	u32 tmp;
//	
//	tmp = 0x55aa5aa5;
//	write_dgus_vp(0x04,(u8*)&tmp,2);
//}

void delay_us(unsigned int t)
{ 
 u8 i;

 while(t)
 {
  for(i=0;i<50;i++)
   {i=i;}
  t--;
 }
}

void TouchSwitch(u16 PageID, u8 TouchType, u8 TouchID, u8 Status)
{
	u8 k_data[8];
	
	*(u16*)k_data = 0x5aa5;
	*(u16*)&k_data[2] = PageID;
	k_data[4] = TouchID;
	k_data[5] = TouchType;
	if(Status)
		*(u16*)&k_data[6] = 1;
	else
		*(u16*)&k_data[6] = 0;
	write_dgus_vp(0xb0,k_data,4);
	do
	{
		delay_us(500);
		read_dgus_vp(0xb0,k_data,1);
	}while(k_data[0]!=0);
}

//void MusicPlay(u8 MusicId)
//{
//	u8 k_data[4];
//	
//	read_dgus_vp(0xa0,k_data,2);
//	k_data[0] = MusicId;
//	k_data[1] = 0x01;
//	k_data[3] = 0;
//	write_dgus_vp(0xa0,k_data,2);
//}

//void NorFlash_Action(void)
//{
//	u8 buf[8];	
//	
//	buf[0] = Nor_Flash.Mode;
//	buf[1] = (u8)(Nor_Flash.FLAddr>>16);
//	buf[2] = (u8)(Nor_Flash.FLAddr>>8);
//	buf[3] = (u8)Nor_Flash.FLAddr;
//	buf[4] = (u8)(Nor_Flash.VPAddr>>8);
//	buf[5] = (u8)Nor_Flash.VPAddr;
//	buf[6] = (u8)(Nor_Flash.Len>>8);
//	buf[7] = (u8)Nor_Flash.Len;
//	if(Nor_Flash.Mode == 0xa5)//写数据
//	{
//		if(Nor_Flash.Buf != NULL)
//			write_dgus_vp(Nor_Flash.VPAddr,Nor_Flash.Buf,Nor_Flash.Len);
//	}
//	write_dgus_vp(0x08,buf,4);
//	do
//	{
//		delay_us(500);
//		read_dgus_vp(0x08,buf,1);
//	}while(buf[0]!=0);
//	if(Nor_Flash.Mode == 0x5a)//读数据
//	{
//		if(Nor_Flash.Buf != NULL)
//			read_dgus_vp(Nor_Flash.VPAddr,Nor_Flash.Buf,Nor_Flash.Len);
//	}
//	delay_ms(FLASH_ACCESS_CYCLE);
//}

//void SPIFlash_Action(void)//对于写操作需要提前准备好32K的VP数据
//{
//	u8 buf[12];
//	
//	buf[0] = 0x5a;
//	buf[1] = Spi_Flash.Mode;
//	if(Spi_Flash.Mode == 1)
//	{
//		buf[2] = Spi_Flash.ID;
//		buf[3] = (u8)(Spi_Flash.FLAddr>>16);
//		buf[4] = (u8)(Spi_Flash.FLAddr>>8);
//		buf[5] = (u8)Spi_Flash.FLAddr;
//		buf[6] = (u8)(Spi_Flash.VPAddr>>8);
//		buf[7] = (u8)Spi_Flash.VPAddr;
//		buf[8] = (u8)(Spi_Flash.Len>>8);
//		buf[9] = (u8)Spi_Flash.Len;
//		buf[10] = 0;
//		buf[11] = 0;
//	}
//	else if(Spi_Flash.Mode == 2)
//	{
//		buf[2] = (u8)(Spi_Flash.ID>>8);
//		buf[3] = (u8)Spi_Flash.ID;
//		buf[4] = (u8)(Spi_Flash.VPAddr>>8);
//		buf[5] = (u8)Spi_Flash.VPAddr;
//		buf[6] = (u8)(Spi_Flash.Delay>>8);
//		buf[7] = (u8)Spi_Flash.Delay;
//		buf[8] = 0;
//		buf[9] = 0;
//		buf[10] = 0;
//		buf[11] = 0;
//	}	
//	write_dgus_vp(0xaa,buf,6);
//	do
//	{
//		delay_us(500);
//		read_dgus_vp(0xaa,buf,1);
//	}while(buf[0]!=0);
//	if(Spi_Flash.Mode == 1)//读数据
//	{
//		if(Spi_Flash.Buf != NULL)
//			read_dgus_vp(Spi_Flash.VPAddr,Spi_Flash.Buf,Spi_Flash.Len);
//	}
//	delay_ms(FLASH_ACCESS_CYCLE);
//}

////获得当前页面
//u16 GetPageID()
//{
//	  u16  nPage;
//	
//    read_dgus_vp(PIC_NOW,(u8*)(&nPage),1);	
//	  return nPage;
//}

void Page_Change(u16 PageID)
{
	u8 buf[4];
 
	buf[0] = 0x5a;
	buf[1] = 0x01;
	buf[2] = (u8)(PageID >> 8);
	buf[3] = (u8)PageID;
 write_dgus_vp(0x84,buf,2);
//	do
//	{
//		delay_us(500);
//		read_dgus_vp(0x14,buf,2);
//	}while(*(u16*)buf!=PageID);
//	buf[0] = 0;
//	buf[1] = 0;
//	buf[2] = 0;
//	buf[3] = 0;
//	write_dgus_vp(0x16,buf,2);
}

void delay_ms(u16 t)
{
	u16 i,j;
	  for(i=0;i<t;i++)
	  {
			for(j=0;j<300;j++)
	     delay_us(1);
		}
}

void SetPinOut(u8 Port,u8 Pin)
{
	  switch(Port)
		{
			  case 0: P0MDOUT|=(1<<Pin);
					      break;
				case 1: P1MDOUT|=(1<<Pin);
					      break;
				case 2: P2MDOUT|=(1<<Pin);
					      break;
				case 3: P3MDOUT|=(1<<Pin);
					      break;	
        default:break;				
		}
		
}

void SetPinIn(u8 Port,u8 Pin)
{
	  switch(Port)
		{
			  case 0: P0MDOUT&=~(1<<Pin);
					      break;
				case 1: P1MDOUT&=~(1<<Pin);
					      break;
				case 2: P2MDOUT&=~(1<<Pin);
					      break;
				case 3: P3MDOUT&=~(1<<Pin);
					      break;	
        default:break;				
		}
		
}



void  PinOutput(u8 Port,u8 Pin,u8 value)
{
	  switch(Port)
		{
			  case 0: if(value) P0|=(1<<Pin);
					      else      P0&=~(1<<Pin);
					      break;
				case 1: if(value) P1|=(1<<Pin);
					      else      P1&=~(1<<Pin);
					      break;
				case 2: if(value) P2|=(1<<Pin);
					      else      P2&=~(1<<Pin);
					      break;
				case 3: if(value) P3|=(1<<Pin);
					      else      P3&=~(1<<Pin);
					      break;	
        default:break;				
		}
}

// u8 GetPinIn(u8 Port,u8 Pin)
// {	 
//   	u8 value;
// 	  switch(Port)
// 		{
// 			  case 0: value=P0&(1<<Pin);
// 					      break;
// 				case 1: value=P1&(1<<Pin);
// 					      break;
// 				case 2: value=P2&(1<<Pin);
// 					      break;
// 				case 3: value=P3&(1<<Pin);
// 					      break;	
//         default:
// 					      value=0;
// 					    break;				
// 		}
// 		return value;
// }

// void num_init(void)
// {
// 	u16 i;
// 	write_dgus_vp(0xA000,(u8*)&num[0],1);
// 	delay_ms(20);
// 	write_dgus_vp(0xA010,(u8*)&num[0],1);
// 	delay_ms(20);
// 	write_dgus_vp(0x5000,(u8*)&gaojnum2,1);
// 	delay_ms(20);
// 	write_dgus_vp(0x3503,(u8*)&num[20],1);
// 	write_dgus_vp(0x3020,(u8*)&canid1,2);
// 	TouchSwitch(0,5,0,0);
// 	for(i=2;i<5;i++)
// 	{TouchSwitch(i,5,0,0);}
// 	for(i=34;i<40;i++)
// 	{TouchSwitch(i,5,0,0);}
// }
// void io_init(void)
// {
// 	SetPinOut(0,4);
// 	PORTDRV=0x01;   //驱动强度+/-8mA
// }