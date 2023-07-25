#include "app/ui.h"

#include "app/vpmap.h"
#include "driver/dgusvar.h"

void Ui_init(Ui_MainPage_t *page)
{
  page->tank.valve_open = Ui_IconState_Passive;

  page->tank.balloon = Ui_IconState_Active;
  page->tank.filling = 0;

  page->tank.temp_bounds.min = 0;
  page->tank.temp_bounds.max = 0;
  page->tank.prs_bounds.min = 0;
  page->tank.prs_bounds.max = 0;


  page->battery.hv.voltage = 0;
  page->battery.hv.current = 0;

  page->battery.discharge_power = 0;
  page->battery.charge_power = 0;

  page->battery.tms_state = Ui_TmsState_Off;
  page->battery.battery = Ui_IconState_Active;
  page->battery.soc = 0;

  page->battery.temp_bounds.min = 0;
  page->battery.temp_bounds.max = 0;
  page->battery.voltage_bounds.min = 0;
  page->battery.voltage_bounds.max = 0;


  page->fuel_cell.state = Ui_IconState_Passive;

  page->fuel_cell.output_power = 0;
  page->fuel_cell.temp = 0;

  page->hed_rate.hydrogen = 0;
  page->hed_rate.energy = 0;
}

void Ui_update(Ui_MainPage_t *page)
{
  DgusVar_Message_t message;
  message.length = 2;

  DgusVar_beginReadWrite();
  DgusVar_setAddressIncrement(1);

  message.address = DgusVar_newVpAddress(VPMAP__TANK_VALVE_OPEN);
  message.content.variables[0] = page->tank.valve_open;
  DgusVar_write(message);

  message.address = DgusVar_newVpAddress(VPMAP__TANK_BALLOON);
  message.content.variables[0] = page->tank.balloon;
  DgusVar_write(message);

  message.content.variables[0] = page->tank.filling;
  DgusVar_continueWrite(message.length, message.content);

  message.content.variables[0] = page->tank.temp_bounds.min;
  DgusVar_continueWrite(message.length, message.content);

  message.content.variables[0] = page->tank.temp_bounds.max;
  DgusVar_continueWrite(message.length, message.content);

  message.content.variables[0] = page->tank.prs_bounds.min;
  DgusVar_continueWrite(message.length, message.content);

  message.content.variables[0] = page->tank.prs_bounds.max;
  DgusVar_continueWrite(message.length, message.content);

  message.content.variables[0] = page->battery.hv.voltage;
  DgusVar_continueWrite(message.length, message.content);

  message.content.variables[0] = page->battery.hv.current;
  DgusVar_continueWrite(message.length, message.content);

  message.content.variables[0] = page->battery.discharge_power;
  DgusVar_continueWrite(message.length, message.content);

  message.content.variables[0] = page->battery.charge_power;
  DgusVar_continueWrite(message.length, message.content);

  message.content.variables[0] = page->battery.tms_state;
  DgusVar_continueWrite(message.length, message.content);

  message.content.variables[0] = page->battery.battery;
  DgusVar_continueWrite(message.length, message.content);

  message.content.variables[0] = page->battery.soc;
  DgusVar_continueWrite(message.length, message.content);

  message.content.variables[0] = page->battery.temp_bounds.min;
  DgusVar_continueWrite(message.length, message.content);

  message.content.variables[0] = page->battery.temp_bounds.max;
  DgusVar_continueWrite(message.length, message.content);

  message.content.variables[0] = page->battery.voltage_bounds.min;
  DgusVar_continueWrite(message.length, message.content);

  message.content.variables[0] = page->battery.voltage_bounds.max;
  DgusVar_continueWrite(message.length, message.content);

  message.address = DgusVar_newVpAddress(VPMAP__FC_STATE);
  message.content.variables[0] = page->fuel_cell.state;
  DgusVar_write(message);

  message.address = DgusVar_newVpAddress(VPMAP__FC_OUTPUT_POWER);
  message.content.variables[0] = page->fuel_cell.output_power;
  DgusVar_write(message);

  message.content.variables[0] = page->fuel_cell.temp;
  DgusVar_continueWrite(message.length, message.content);

  message.address = DgusVar_newVpAddress(VPMAP__HED_HYDROGEN_RATE);
  message.content.variables[0] = page->hed_rate.hydrogen;
  DgusVar_write(message);

  message.content.variables[0] = page->hed_rate.energy;
  DgusVar_continueWrite(message.length, message.content);

  DgusVar_endReadWrite();
}
