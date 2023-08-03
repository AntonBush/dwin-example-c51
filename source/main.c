#include "timer.h"
#include "can.h"
#include "sys.h"
#include "ui.h"

#include "app/logic.h"

void main()
{
  Logic_Data_t xdata logic;
  u8 data life_counter = 0;

	//Baud rate:125K{0x3F,0x40,0x72,0x00},250K{0x1F,0x40,0x72,0x00},500K{0x0F,0x40,0x72,0x00},1M{0x07,0x40,0x72,0x00}
  Can_InitConfig_t can_config = { 0x1f , 0x40 , 0x72 };
  Can_AcceptanceFilter_t can_filter = {
    { 0, 0, 0, 0 }
    , { 0xFF, 0xFF, 0xFF, 0xFF }
  };

	Timer_init();			//定时器0初始化
	Can_init(&can_config, &can_filter);

	ENABLE_INTERRUPT();

	INTERRUPT_GUARD(Timer_start(0, 20));
  INTERRUPT_GUARD(Timer_start(1, 500));

	Logic_init(&(logic));

	while(1)
	{
		if(Timer_timeout(1))
		{
			u8 chabuf[2] = { 0, 0 };
      chabuf[0] = life_counter;
//			chabuf[1] = (logic.game.game.on) & 0x01;
//			chabuf[1] |= ((logic.dashboardParams.pageIndex) & 0x0F) << 4;

			Can_tx(0, 0x111, chabuf, 2);
      INTERRUPT_GUARD(Timer_start(1, 500));

			++life_counter;
		}
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

      INTERRUPT_GUARD(Timer_start(0,20));
		}
		Can_resetError();
	}
}


