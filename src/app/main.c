#include "driver/sys.h"
#include "driver/timer.h"
#include "driver/can.h"

#include "app/logic.h"

#define LOGIC_TIMER_ID 0
#define LOGIC_TIMER_DURATION 20

#define CAN_TX_TIMER_ID 1
#define CAN_TX_TIMER_DURATION 500

#define START_LOGIC_TIMER() \
SYS__INTERRUPT_GUARD(Timer_start(LOGIC_TIMER_ID, LOGIC_TIMER_DURATION))

#define START_CAN_TX_TIMER() \
SYS__INTERRUPT_GUARD(Timer_start(CAN_TX_TIMER_ID, CAN_TX_TIMER_DURATION))

Logic_Data_t xdata logic;

void init(void);
void canTxControl(void);
void logicControl(void);

void main(void)
{
  init();

  START_LOGIC_TIMER();
  START_CAN_TX_TIMER();

  while(1)
  {
    if (Timer_timeout(LOGIC_TIMER_ID))
    {
      logicControl();
      START_LOGIC_TIMER();
    }
    if (Timer_timeout(CAN_TX_TIMER_ID))
    {
      canTxControl();
      START_CAN_TX_TIMER();
    }
    Can_resetError();
  }
}

void init(void)
{
  // Baud rate:
  //   125K{ 0x3F, 0x40, 0x72 }
  //   , 250K{ 0x1F, 0x40, 0x72 }
  //   , 500K{ 0x0F, 0x40, 0x72 }
  //   , 1M{ 0x07, 0x40, 0x72 }
  Can_InitConfig_t can_config = { 0x1F, 0x40, 0x72 };
  Can_Filter_t can_filter;

  SYS__DISABLE_INTERRUPT();

  can_filter.acceptance_code.value = 0;
  can_filter.acceptance_mask.value = ~(u32)0;

  Sys_init(); // инициализация основных регистров
  Timer_init();
  Can_init(&can_config, &can_filter);
  Logic_init(&logic);

  SYS__ENABLE_INTERRUPT();
}

void logicControl(void)
{
  Can_Message_t message;

  //logic.dashboardParams.iterationsCount++;

  while (Can_rx(&message))
  {
    Logic_rx(&logic, &message);
  }

  Logic_update(&logic);
}

void canTxControl(void)
{
  static u8 data life_counter = 0;
  static u8 data buffer[2];
  buffer[0] = life_counter++;
  //buffer[1] = logic.game.game.on & BITS__BIT8_0;
  //buffer[1] |= (logic.dashboardParams.pageIndex & 0x0F) << 4;

  Can_tx(0, 0x111, buffer, 2);
}
