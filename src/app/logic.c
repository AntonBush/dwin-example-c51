#include "app/logic.h"

#include "driver/sys.h"
#include "lib/round.h"

static void Logic_updateMainPage(Logic_Data_t *logic);

void Logic_init(Logic_Data_t *logic)
{
  logic->rx_timeout = 0;
  CanRx_init(&(logic->rx));
  Ui_init(&(logic->ui));
}

void Logic_rx(Logic_Data_t *logic, Can_Message_t *message)
{
  if (message->id == CANRX__VCU_MSG_ID)
  {
    logic->rx_timeout = LOGIC__RX_TIMEOUT;
  }
  else if (logic->rx_timeout == 0)
  {
    return;
  }

  CanRx_recieve(&(logic->rx), message);
}

void Logic_update(Logic_Data_t *logic)
{
  if (logic->rx_timeout == 0)
  {
    Ui_Page_t p = logic->ui.current_page;
    Logic_init(logic);
    logic->ui.current_page = p;
  }
  else
  {
    logic->rx_timeout -= 1;
    logic->ui.current_page = logic->rx.pcu4.operation_state == CANRX__OPERATION_STATE_OFF
      ? UI__PAGE_BACKGROUND
      : UI__PAGE_MAIN;
  }

  switch (logic->ui.current_page)
  {
  case UI__PAGE_BACKGROUND:
    break;
  case UI__PAGE_MAIN:
    Logic_updateMainPage(logic);
    break;
  }
  SYS__INTERRUPT_GUARD(Ui_update(&(logic->ui)));
}

void Logic_updateMainPage(Logic_Data_t *logic)
{
  CanRx_Data_t * const rx = &(logic->rx);
  UiMain_Data_t * const ui = &(logic->ui.main_page);
  float hv_discharge_power;

  ui->tank.can_error
    = logic->rx_timeout == 0
    ? UI__ICON_STATE_ACTIVE
    : UI__ICON_STATE_PASSIVE;
  ui->tank.valve_open
    = rx->tank.valve_state == J1939__STATE_ACTIVE
    ? UI__ICON_STATE_ACTIVE
    : UI__ICON_STATE_PASSIVE;
  ui->tank.filling = round(rx->tank.filling);
  ui->tank.temp_bounds.min = rx->tank.temp_bounds.min;
  ui->tank.temp_bounds.max = rx->tank.temp_bounds.max;
  ui->tank.prs_bounds = rx->tank.prs_bounds;

  ui->battery.hv.voltage = round(logic->rx.pcu1.hv.voltage);
  ui->battery.hv.current = logic->rx.pcu1.hv.current;
  hv_discharge_power = round(rx->pcu1.hv.voltage * rx->pcu1.hv.current / 1000);
  if (hv_discharge_power < 0)
  {
    ui->battery.discharge_power = 0;
    ui->battery.charge_power = -hv_discharge_power;
  }
  else
  {
    ui->battery.discharge_power = hv_discharge_power;
    ui->battery.charge_power = 0;
  }
  switch (rx->pcu2.tms_state)
  {
    case CANRX__TMS_STATE_OFF:
      ui->battery.tms_state = UIMAIN__TMS_STATE_OFF;
      break;

    case CANRX__TMS_STATE_COOLING:
      ui->battery.tms_state = UIMAIN__TMS_STATE_COOLING;
      break;

    case CANRX__TMS_STATE_HEATING:
      ui->battery.tms_state = UIMAIN__TMS_STATE_HEATING;
      break;

    case CANRX__TMS_STATE_CIRCULATION:
      ui->battery.tms_state = UIMAIN__TMS_STATE_CIRCULATION;
      break;
  }
  ui->battery.soc = round(rx->pcu1.soc);
  ui->battery.temp_bounds.min
    = rx->pcu2.battery_temp_bounds.min;
  ui->battery.temp_bounds.max
    = rx->pcu2.battery_temp_bounds.max;
  ui->battery.voltage_bounds.min
    = round(rx->pcu2.cell_voltage_bounds.min);
  ui->battery.voltage_bounds.max
    = round(rx->pcu2.cell_voltage_bounds.max);

  ui->fuel_cell.state
    = rx->pcu4.fuel_cell.state == CANRX__FUEL_CELL_STATE_POWER_OUTPUT
      || rx->pcu4.fuel_cell.state == CANRX__FUEL_CELL_STATE_TURNING_OFF
    ? UI__ICON_STATE_ACTIVE
    : UI__ICON_STATE_PASSIVE;
  ui->fuel_cell.output_power = round(rx->pcu4.fuel_cell.output_power);
  ui->fuel_cell.temp = rx->pcu4.fuel_cell.temp;

  ui->hed_rate.hydrogen = rx->pcu4.rate.hydrogen;
  ui->hed_rate.energy = rx->pcu4.rate.energy;
}
