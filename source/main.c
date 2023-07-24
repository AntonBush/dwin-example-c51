#include "driver/timer.h"
#include "driver/canbus.h"
#include "driver/sys.h"
#include "source/const.h"
#include "logic/inc/driver/interrupt.h"
//#include "ui.h"

#include "logic/inc/app/logic.h"

sbit CAN_RLSEL = P0^4;

xdata Logic_Data_t logic;
xdata uint8_t lifeCounter = 0;

void main()
{
	uint32_t msgId = 0;
	uint8_t msgData[8];
	
	u8 canReg[4];
	Timer_init();			//定时器0初始化
	//Baud rate:125K{0x3F,0x40,0x72,0x00},250K{0x1F,0x40,0x72,0x00},500K{0x0F,0x40,0x72,0x00},1M{0x07,0x40,0x72,0x00}
	canReg[0] = 0x1f;
	canReg[1] = 0x40;
	canReg[2] = 0x72;
	canReg[3] = 0x00;
	CanBusInit(canReg);
	
	Interrupt_enable();
	Timer_start(0,20);
	Timer_start(1,500);

	Logic_init(&(logic));
	
	while(1)
	{
		if(Timer_timeout(1))
		{
			
			chabuf[0] = (lifeCounter) & 0xFF;
//			chabuf[1] = (logic.game.game.on) & 0x01;
//			chabuf[1] |= ((logic.dashboardParams.pageIndex) & 0x0F) << 4;
			
			CanTx(0x111, 0, 2, chabuf);
			Timer_start(1,500);
			
			lifeCounter++;
		}
		if(Timer_timeout(0))
		{
//			logic.dashboardParams.iterationsCount++;
			
			while (canRxTreat(&(msgId), msgData))
			{
        u8 k;
        Can_Message_t message;
        message.id = msgId;
        for (k = 0; k < 8; ++k) message.bytes[k] = msgData[k];
				Logic_rx(&(logic), message);				
			}

			Logic_update(&(logic));
			
			Timer_start(0, 20);
		}
		CanErrorReset();
	}
}


