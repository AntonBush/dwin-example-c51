#include "driver/sys.h"
#include "driver/timer.h"
#include "driver/interrupt.h"
#include "driver/can.h"

#include "app/logic.h"

#define LOGIC_TIMER_ID 0
#define LOGIC_TIMER_DURATION 20

#define CAN_TX_TIMER_ID 1
#define CAN_TX_TIMER_DURATION 500

data u8 LifeCounter = 0;
xdata Logic_Data_t logic;

void init(void);

void startCanTxTimer(void);
void canTxControl(void);

void startLogicTimer(void);
void logicControl(void);

void main(void)
{
  init();

  Interrupt_enable();

  startLogicTimer();
  startCanTxTimer();

  while(1)
  {
    if (Timer_timeout(LOGIC_TIMER_ID))
    {
      logicControl();
      startLogicTimer();
    }
    if (Timer_timeout(CAN_TX_TIMER_ID))
    {
      canTxControl();
      startCanTxTimer();
   }
    Can_resetError();
  }
}

void init(void)
{
  Can_InitConfig_t can_config;
  Can_AcceptanceFilter_t can_filter;

  sys_init(); // инициализация основных регистров
  Timer_init();

  // Baud rate:
  //   125K{0x3F, 0x40, 0x72, 0x00}
  //   , 250K{0x1F, 0x40, 0x72, 0x00}
  //   , 500K{0x0F, 0x40, 0x72, 0x00}
  //   , 1M{0x07, 0x40, 0x72, 0x00}
  can_config.baud_rate_frequency_divider = 0x1F;
  can_config.BTR0 = 0x40;
  can_config.BTR1 = 0x72;

  can_filter.acceptance_code = 0;
  can_filter.acceptance_mask = 0xFFFFFFFF;

  Can_init(can_config, can_filter);
}

void startLogicTimer(void)
{
  Timer_start(LOGIC_TIMER_ID, LOGIC_TIMER_DURATION);
}

void logicControl(void)
{
  Can_Message_t message;

  //logic.dashboardParams.iterationsCount++;

  message = Can_rx();
  while (message.status)
  {
    Logic_rx(&logic, message);
    message = Can_rx();
  }

  Logic_update(&logic);
}

void startCanTxTimer(void)
{
  Timer_start(CAN_TX_TIMER_ID, CAN_TX_TIMER_DURATION);
}

void canTxControl(void)
{
  u8 buffer[2];
  buffer[0] = LifeCounter & 0xFF;
  //buffer[1] = logic.game.game.on & Bits_Bit8_0;
  //buffer[1] |= (logic.dashboardParams.pageIndex & 0x0F) << 4;

  Can_tx(0x111, 0, 2, buffer);

  ++LifeCounter;
}
