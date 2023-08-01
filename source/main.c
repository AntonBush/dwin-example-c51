#include "timer.h"
#include "can.h"
#include "sys.h"
#include "ui.h"

#include "app/logic.h"

sbit CAN_RLSEL = P0^4;

void main()
{
  xdata Logic_Data_t logic;
  xdata u8 lifeCounter = 0;

	//Baud rate:125K{0x3F,0x40,0x72,0x00},250K{0x1F,0x40,0x72,0x00},500K{0x0F,0x40,0x72,0x00},1M{0x07,0x40,0x72,0x00}
  Can_InitConfig_t can_config = { 0x1f , 0x40 , 0x72 };
  Can_AcceptanceFilter_t can_filter = {
    { 0, 0, 0, 0 }
    , { 0xFF, 0xFF, 0xFF, 0xFF }
  };

	Timer_init();			//定时器0初始化
	Can_init(&can_config, &can_filter);

	ENABLE_INTERRUPT();

	TIMER__START_SAFE(0,20);
	TIMER__START_SAFE(1,500);

	Logic_init(&(logic));

	while(1)
	{
//		if(Timer_timeout(1))
//		{
//			u8 chabuf[2] = { 0, 0 };
//      chabuf[0] = (lifeCounter) & 0xFF;
////			chabuf[1] = (logic.game.game.on) & 0x01;
////			chabuf[1] |= ((logic.dashboardParams.pageIndex) & 0x0F) << 4;

//			Can_tx(0, 0x111, chabuf, 2);
//			TIMER__START_SAFE(1,500);

//			lifeCounter++;
//		}
		if(Timer_timeout(0))
		{
      Can_Message_t message;
			//logic.dashboardParams.iterationsCount++;

			while (Can_rx(&message))
			{
				//Logic_Rx_Message(&(logic.dashboardParams), &msgId, msgData);
        Logic_rx(&logic, &message);
			}

			Logic_update(&(logic));

			TIMER__START_SAFE(0, 20);
		}
		Can_resetError();
	}
}


