#ifndef __UIMAIN_H__
#define __UIMAIN_H__

#include "app/uidef.h"

#include "lib/int.h"
#include "lib/bounds.h"

typedef enum UiMain_TmsState
{
  UIMAIN__TMS_STATE_OFF = 1
  , UIMAIN__TMS_STATE_CIRCULATION = 2
  , UIMAIN__TMS_STATE_COOLING = 3
  , UIMAIN__TMS_STATE_HEATING = 4
} UiMain_TmsState_t;

typedef struct UiMain_Tank
{
  Ui_IconState_t can_error;
  Ui_IconState_t valve_open;

  Ui_IconState_t balloon;
  u16 filling;

  Bounds_S16Bounds_t temp_bounds;
  Bounds_S16Bounds_t prs_bounds;
} UiMain_Tank_t;

typedef struct UiMain_Battery
{
  struct UiMain_Battery_HighVoltage
  {
    u16 voltage;
    s16 current;
  } hv;

  u16 discharge_power;
  u16 charge_power;

  UiMain_TmsState_t tms_state;
  Ui_IconState_t battery;
  u16 soc;

  Bounds_S16Bounds_t temp_bounds;
  Bounds_U16Bounds_t voltage_bounds;
} UiMain_Battery_t;

typedef struct UiMain_FuelCell
{
  Ui_IconState_t state;

  u16 output_power;
  u16 temp;
} UiMain_FuelCell_t;

typedef struct UiMain_HedRate
{
  u16 hydrogen;
  u16 energy;
} UiMain_HedRate_t;

typedef struct UiMain_Data
{
  UiMain_Tank_t tank;
  UiMain_Battery_t battery;
  UiMain_FuelCell_t fuel_cell;
  UiMain_HedRate_t hed_rate;
} UiMain_Data_t;

void UiMain_init(UiMain_Data_t *page);
void UiMain_update(UiMain_Data_t *page);

#endif

