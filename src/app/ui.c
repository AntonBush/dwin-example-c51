#include "app/ui.h"

#include "app/vpmap.h"
#include "driver/dgusvar_io.h"

void Ui_init(Ui_MainPage_t *page)
{
  page->tank.can_error  = UI__ICON_STATE_PASSIVE;
  page->tank.valve_open = UI__ICON_STATE_PASSIVE;

  page->tank.balloon = UI__ICON_STATE_ACTIVE;
  page->tank.filling = 0;

  page->tank.temp_bounds.min = 0;
  page->tank.temp_bounds.max = 0;
  page->tank.prs_bounds.min = 0;
  page->tank.prs_bounds.max = 0;


  page->battery.hv.voltage = 0;
  page->battery.hv.current = 0;

  page->battery.discharge_power = 0;
  page->battery.charge_power = 0;

  page->battery.tms_state = UI__TMS_STATE_OFF;
  page->battery.battery = UI__ICON_STATE_ACTIVE;
  page->battery.soc = 0;

  page->battery.temp_bounds.min = 0;
  page->battery.temp_bounds.max = 0;
  page->battery.voltage_bounds.min = 0;
  page->battery.voltage_bounds.max = 0;


  page->fuel_cell.state = UI__ICON_STATE_PASSIVE;

  page->fuel_cell.output_power = 0;
  page->fuel_cell.temp = 0;

  page->hed_rate.hydrogen = 0;
  page->hed_rate.energy = 0;
}

void Ui_update(Ui_MainPage_t *page)
{
  DgusVar_updateU16(VPMAP__TANK_CAN_ERROR, page->tank.can_error);

  DgusVar_updateU16(VPMAP__TANK_VALVE_OPEN, page->tank.valve_open);

  DgusVar_updateU16(VPMAP__TANK_BALLOON, page->tank.balloon);

  DgusVar_updateU16(VPMAP__TANK_FILLING, page->tank.filling);

  DgusVar_updateU16(VPMAP__TANK_MIN_TEMP, page->tank.temp_bounds.min);

  DgusVar_updateU16(VPMAP__TANK_MAX_TEMP, page->tank.temp_bounds.max);

  DgusVar_updateU16(VPMAP__TANK_MIN_PRS, page->tank.prs_bounds.min);

  DgusVar_updateU16(VPMAP__TANK_MAX_PRS, page->tank.prs_bounds.max);

  DgusVar_updateU16(VPMAP__BATT_HV_VOLTAGE, page->battery.hv.voltage);

  DgusVar_updateS16(VPMAP__BATT_HV_CURRENT, page->battery.hv.current);

  DgusVar_updateU16(
    VPMAP__BATT_HV_DISCHARGE_POWER
    , page->battery.discharge_power
  );

  DgusVar_updateU16(
    VPMAP__BATT_HV_CHARGE_POWER
    , page->battery.charge_power
  );

  DgusVar_updateU16(VPMAP__BATT_TMS_STATE, page->battery.tms_state);

  DgusVar_updateU16(VPMAP__BATT_BATTERY, page->battery.battery);

  DgusVar_updateU16(VPMAP__BATT_SOC, page->battery.soc);

  DgusVar_updateS16(VPMAP__BATT_MIN_TEMP, page->battery.temp_bounds.min);

  DgusVar_updateS16(VPMAP__BATT_MAX_TEMP, page->battery.temp_bounds.max);

  DgusVar_updateU16(VPMAP__BATT_MIN_VOLTAGE, page->battery.voltage_bounds.min);

  DgusVar_updateU16(VPMAP__BATT_MAX_VOLTAGE, page->battery.voltage_bounds.max);

  DgusVar_updateU16(VPMAP__FC_STATE, page->fuel_cell.state);

  DgusVar_updateU16(VPMAP__FC_OUTPUT_POWER, page->fuel_cell.output_power);

  DgusVar_updateU16(VPMAP__FC_TEMP, page->fuel_cell.temp);

  DgusVar_updateU16(VPMAP__HED_HYDROGEN_RATE, page->hed_rate.hydrogen);

  DgusVar_updateU16(VPMAP__HED_ENERGY_RATE, page->hed_rate.energy);
}
