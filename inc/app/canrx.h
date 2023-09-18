#ifndef __CANRX_H__
#define __CANRX_H__

#define CANRX__VCU_MSG_ID 0x18A70003
#define CANRX__GAS_TANK_DATA_ID 0x18F0AC9D
#define CANRX__PCU_MSG_1_ID 0x18FFC69D
#define CANRX__PCU_MSG_2_ID 0x18FFC79D
#define CANRX__PCU_MSG_3_ID 0x18FFC89D
#define CANRX__PCU_MSG_4_ID 0x18FED930

#include "lib/j1939.h"
#include "driver/can.h"
#include "lib/bounds.h"

typedef enum CanRx_SystemStatus
{
  CANRX__SYSTEM_STATUS_OFF = 0
, CANRX__SYSTEM_STATUS_PRECHARGE = 1
//, CANRX__SYSTEM_STATUS_OFF = 2
, CANRX__SYSTEM_STATUS_SHUTDOWN = 3
, CANRX__SYSTEM_STATUS_POWER_LIMIT = 4

, CANRX__SYSTEM_STATUS_CHARGING = 5
, CANRX__SYSTEM_STATUS_NOTAVAIL = 15
} CanRx_SystemStatus_t;

typedef enum CanRx_TmsState
{
  CANRX__TMS_STATE_OFF
, CANRX__TMS_STATE_COOLING
, CANRX__TMS_STATE_HEATING
, CANRX__TMS_STATE_CIRCULATION
} CanRx_TmsState_t;

typedef enum CanRx_OperationState
{
  CANRX__OPERATION_STATE_OFF
, CANRX__OPERATION_STATE_IGNITION_ON
, CANRX__OPERATION_STATE_TURNING_ON
, CANRX__OPERATION_STATE_LOAD_TURNING_ON
, CANRX__OPERATION_STATE_BATTERY_TURNING_OFF

, CANRX__OPERATION_STATE_ON
, CANRX__OPERATION_STATE_CHARGING
} CanRx_OperationState_t;

typedef enum CanRx_BatteryState
{
  CANRX__BATTERY_STATE_OFF
, CANRX__BATTERY_STATE_TURNING_ON
, CANRX__BATTERY_STATE_ON
, CANRX__BATTERY_STATE_TURNING_OFF
} CanRx_BatteryState_t;

typedef enum CanRx_FuelCellState
{
  CANRX__FUEL_CELL_STATE_OFF
, CANRX__FUEL_CELL_STATE_POWER_OUTPUT
, CANRX__FUEL_CELL_STATE_TURNING_OFF
, CANRX__FUEL_CELL_STATE_WAITING_BATTERY
} CanRx_FuelCellState_t;

typedef struct CanRx_VcuMsg
{
  J1939_State_t ignition;
  J1939_State_t emergency_stop_req;
  s16 power_req;
  s16 traction_current;
} CanRx_VcuMsg_t;

typedef struct CanRx_GasTankData
{
  Bounds_S16Bounds_t temp_bounds;
  Bounds_S16Bounds_t prs_bounds;
  float filling;
  J1939_State_t valve_state;
  u8 life_counter;
} CanRx_GasTankData_t;

typedef struct CanRx_PcuMsg1
{
  struct CanRx_PcuMsg1_HighVoltage
  {
    float voltage;
    s16 current;
    u16 max_charge_current;
    u16 max_discharge_current;
  } hv;
  float soc;
  CanRx_SystemStatus_t system_status;
  u8 life_counter;
} CanRx_PcuMsg1_t;

typedef struct CanRx_PcuMsg2
{
  Bounds_FloatBounds_t cell_voltage_bounds;
  Bounds_S16Bounds_t battery_temp_bounds;
  CanRx_TmsState_t tms_state;
  s16 power_req_fb;
} CanRx_PcuMsg2_t;

typedef struct CanRx_PcuMsg4
{
  CanRx_OperationState_t operation_state;
  CanRx_BatteryState_t battery_state;
  J1939_State_t term15_state;
  J1939_State_t term50_state;
  struct CanRx_PcuMsg4_FuelCell
  {
    CanRx_FuelCellState_t state;
    float output_power;
    u8 temp;
  } fuel_cell;
  struct CanRx_PcuMsg4_Rate
  {
    u8 hydrogen;
    u8 energy;
  } rate;
} CanRx_PcuMsg4_t;

typedef struct CanRx_Data
{
  CanRx_VcuMsg_t vcu;
  CanRx_GasTankData_t tank;
  CanRx_PcuMsg1_t pcu1;
  CanRx_PcuMsg2_t pcu2;
  CanRx_PcuMsg4_t pcu4;
} CanRx_Data_t;

void CanRx_init(CanRx_Data_t *rx_data);
void CanRx_recieve(CanRx_Data_t *rx_data, Can_Message_t *message);

#endif
