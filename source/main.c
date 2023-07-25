#include "timer.h"
#include "canbus.h"
#include "sys.h"
#include "const.h"
#include "ui.h"

#include "app/logic.h"

sbit CAN_RLSEL = P0^4;

xdata Logic_Data_t logic;
xdata uint8_t lifeCounter = 0;

void main()
{
	uint32_t msgId = 0;
	uint8_t msgData[8];

	u8 canReg[4];
	T0_Init();			//定时器0初始化
	//Baud rate:125K{0x3F,0x40,0x72,0x00},250K{0x1F,0x40,0x72,0x00},500K{0x0F,0x40,0x72,0x00},1M{0x07,0x40,0x72,0x00}
	canReg[0] = 0x1f;
	canReg[1] = 0x40;
	canReg[2] = 0x72;
	canReg[3] = 0x00;
	CanBusInit(canReg);

	EA = 1;
	StartTimer(0,20);
	StartTimer(1,500);

	Logic_init(&(logic));

	while(1)
	{
		if(GetTimeOutFlag(1))
		{

			chabuf[0] = (lifeCounter) & 0xFF;
//			chabuf[1] = (logic.game.game.on) & 0x01;
//			chabuf[1] |= ((logic.dashboardParams.pageIndex) & 0x0F) << 4;

			CanTx(0x111, 0, 2, chabuf);
			StartTimer(1,500);

			lifeCounter++;
		}
		if(GetTimeOutFlag(0))
		{
			//logic.dashboardParams.iterationsCount++;

			while (canRxTreat(&(msgId), msgData))
			{
        Can_Message_t message;
        u8 i;
        message.id = msgId;
        for (i = 0; i < 8; ++i) message.bytes[i] = msgData[i];
				//Logic_Rx_Message(&(logic.dashboardParams), &msgId, msgData);
        Logic_rx(&logic, message);
			}

			Logic_update(&(logic));

			StartTimer(0, 20);
		}
		CanErrorReset();
	}
}


