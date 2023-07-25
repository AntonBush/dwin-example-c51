#ifndef __LOGIC_H__
#define __LOGIC_H__

#include "app/canrx.h"
#include "app/ui.h"

#define LOGIC__RX_TIMEOUT 200

typedef struct Logic_Data
{
  u32 rx_timeout;
  CanRx_Data_t rx;
  Ui_MainPage_t ui;
} Logic_Data_t;

void Logic_init(Logic_Data_t *logic);
void Logic_rx(Logic_Data_t *logic, Can_Message_t message);
void Logic_update(Logic_Data_t *logic);

#endif
