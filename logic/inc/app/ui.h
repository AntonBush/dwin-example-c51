#ifndef __UI_H__
#define __UI_H__

#include "driver/int.h"
#include "logic/inc/app/bounds.h"

typedef enum Ui_IconState
{
  Ui_IconState_Passive
  , Ui_IconState_Active
} Ui_IconState_t;

typedef enum Ui_TmsState
{
  Ui_TmsState_Off = 1
  , Ui_TmsState_Circulation = 2
  , Ui_TmsState_Cooling = 3
  , Ui_TmsState_Heating = 4
} Ui_TmsState_t;

typedef struct Ui_Tank
{
  Ui_IconState_t valve_open;

  Ui_IconState_t balloon;
  u16 filling;

  Bounds_U16Bounds_t temp_bounds;
  Bounds_U16Bounds_t prs_bounds;
} Ui_Tank_t;

typedef struct Ui_Battery
{
  struct Ui_Battery_HighVoltage
  {
    u16 voltage;
    s16 current;
  } hv;

  u16 discharge_power;
  u16 charge_power;

  Ui_TmsState_t tms_state;
  Ui_IconState_t battery;
  u16 soc;

  Bounds_S16Bounds_t temp_bounds;
  Bounds_U16Bounds_t voltage_bounds;
} Ui_Battery_t;

typedef struct Ui_FuelCell
{
  Ui_IconState_t state;

  u16 output_power;
  u16 temp;
} Ui_FuelCell_t;

typedef struct Ui_HedRate
{
  u16 hydrogen;
  u16 energy;
} Ui_HedRate_t;

typedef struct Ui_MainPage_t
{
  Ui_Tank_t tank;
  Ui_Battery_t battery;
  Ui_FuelCell_t fuel_cell;
  Ui_HedRate_t hed_rate;
} Ui_MainPage_t;

void Ui_init(Ui_MainPage_t *page);
void Ui_update(Ui_MainPage_t *page);

#endif

