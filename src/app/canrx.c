#include "app/canrx.h"

#include "ByteVector/ByteVector_HelpFunctions.h"

static void CanRx_recieveVcuMsg(CanRx_VcuMsg_t *rx_data, Can_Message_t *message);
static void CanRx_recieveGasTankData(CanRx_GasTankData_t *rx_data, Can_Message_t *message);
static void CanRx_recievePcuMsg1(CanRx_PcuMsg1_t *rx_data, Can_Message_t *message);
static void CanRx_recievePcuMsg2(CanRx_PcuMsg2_t *rx_data, Can_Message_t *message);
static void CanRx_recievePcuMsg4(CanRx_PcuMsg4_t *rx_data, Can_Message_t *message);

void CanRx_init(CanRx_Data_t *rx_data)
{
  rx_data->vcu.ignition = J1939__STATE_PASSIVE;
  rx_data->vcu.emergency_stop_req = J1939__STATE_PASSIVE;
  rx_data->vcu.power_req = 0;
  rx_data->vcu.traction_current = 0;

  rx_data->tank.temp_bounds.min = 0;
  rx_data->tank.temp_bounds.max = 0;
  rx_data->tank.prs_bounds.min = 0;
  rx_data->tank.prs_bounds.max = 0;
  rx_data->tank.filling = 0;
  rx_data->tank.valve_state = J1939__STATE_PASSIVE;
  rx_data->tank.life_counter = 0;

  rx_data->pcu1.hv.voltage = 0;
  rx_data->pcu1.hv.current = 0;
  rx_data->pcu1.hv.max_charge_current = 0;
  rx_data->pcu1.hv.max_discharge_current = 0;
  rx_data->pcu1.soc = 0;
  rx_data->pcu1.system_status = CANRX__SYSTEM_STATUS_OFF;
  rx_data->pcu1.life_counter = 0;

  rx_data->pcu2.cell_voltage_bounds.min = 0;
  rx_data->pcu2.cell_voltage_bounds.max = 0;
  rx_data->pcu2.battery_temp_bounds.min = 0;
  rx_data->pcu2.battery_temp_bounds.max = 0;
  rx_data->pcu2.tms_state = CANRX__TMS_STATE_OFF;
  rx_data->pcu2.power_req_fb = 0;

  rx_data->pcu4.operation_state = CANRX__OPERATION_STATE_OFF;
  rx_data->pcu4.battery_state = CANRX__BATTERY_STATE_OFF;
  rx_data->pcu4.term15_state = J1939__STATE_PASSIVE;
  rx_data->pcu4.term50_state = J1939__STATE_PASSIVE;
  rx_data->pcu4.fuel_cell.state = CANRX__FUEL_CELL_STATE_OFF;
  rx_data->pcu4.fuel_cell.output_power = 0;
  rx_data->pcu4.fuel_cell.temp = 0;
  rx_data->pcu4.rate.hydrogen = 0;
  rx_data->pcu4.rate.energy = 0;
}

void CanRx_recieve(CanRx_Data_t *rx_data, Can_Message_t *message)
{
  if (message->id == CANRX__VCU_MSG_ID)
  {
    CanRx_recieveVcuMsg(&(rx_data->vcu), message);
  }
  else if (message->id == CANRX__GAS_TANK_DATA_ID)
  {
    CanRx_recieveGasTankData(&(rx_data->tank), message);
  }
  else if (message->id == CANRX__PCU_MSG_1_ID)
  {
    CanRx_recievePcuMsg1(&(rx_data->pcu1), message);
  }
  else if (message->id == CANRX__PCU_MSG_2_ID)
  {
    CanRx_recievePcuMsg2(&(rx_data->pcu2), message);
  }
  else if (message->id == CANRX__PCU_MSG_4_ID)
  {
    CanRx_recievePcuMsg4(&(rx_data->pcu4), message);
  }
}

static u32 ByteVector_regularParameter(uint8_t *vectorData, uint32_t bitSize, uint32_t index)
{
  return ByteVector_GetRegularParam(vectorData, bitSize, index) << (32 - bitSize) >> (32 - bitSize);
}
static u32 ByteVector_parameter(uint8_t *vectorData, uint32_t bitSize, uint32_t bitPosition)
{
  return ByteVector_GetParam(vectorData, bitSize, bitPosition, BYTEVECTOR_INTEL) << (32 - bitSize) >> (32 - bitSize);
}

void CanRx_recieveVcuMsg(CanRx_VcuMsg_t *rx_data, Can_Message_t *message)
{
  rx_data->ignition = ByteVector_regularParameter(message->bytes, 2, 0);
  rx_data->emergency_stop_req = ByteVector_regularParameter(message->bytes, 2, 1);
  rx_data->power_req = ByteVector_parameter(message->bytes, 12, 8) - 1500;
  rx_data->traction_current = ByteVector_parameter(message->bytes, 12, 20) - 1500;
}

void CanRx_recieveGasTankData(CanRx_GasTankData_t *rx_data, Can_Message_t *message)
{
  rx_data->temp_bounds.max = ByteVector_regularParameter(message->bytes, 8, 0) - 40;
  rx_data->temp_bounds.min = ByteVector_regularParameter(message->bytes, 8, 1) - 40;
  rx_data->prs_bounds.max = ByteVector_regularParameter(message->bytes, 16, 1);
  rx_data->prs_bounds.min = ByteVector_regularParameter(message->bytes, 16, 2);

  rx_data->filling = 0.4F * ByteVector_regularParameter(message->bytes, 8, 6);
  rx_data->valve_state = ByteVector_regularParameter(message->bytes, 2, 28);
  rx_data->life_counter = ByteVector_regularParameter(message->bytes, 4, 15);
}

void CanRx_recievePcuMsg1(CanRx_PcuMsg1_t *rx_data, Can_Message_t *message)
{
  rx_data->hv.voltage = 0.5F * ByteVector_regularParameter(message->bytes, 12, 0);
  rx_data->hv.current = ByteVector_regularParameter(message->bytes, 12, 1) - 1500;
  rx_data->hv.max_charge_current = ByteVector_regularParameter(message->bytes, 12, 2);
  rx_data->hv.max_discharge_current = ByteVector_regularParameter(message->bytes, 12, 3);

  rx_data->soc = 0.4F * ByteVector_regularParameter(message->bytes, 8, 6);
  rx_data->system_status = ByteVector_regularParameter(message->bytes, 4, 14);
  rx_data->life_counter = ByteVector_regularParameter(message->bytes, 4, 15);
}

void CanRx_recievePcuMsg2(CanRx_PcuMsg2_t *rx_data, Can_Message_t *message)
{
  rx_data->cell_voltage_bounds.max = 0.001F * ByteVector_regularParameter(message->bytes, 16, 0);
  rx_data->cell_voltage_bounds.min = 0.001F * ByteVector_regularParameter(message->bytes, 16, 1);
  rx_data->battery_temp_bounds.max = ByteVector_regularParameter(message->bytes, 8, 4) - 40;
  rx_data->battery_temp_bounds.min = ByteVector_regularParameter(message->bytes, 8, 5) - 40;
  rx_data->tms_state = ByteVector_regularParameter(message->bytes, 2, 24);
  rx_data->power_req_fb = ByteVector_parameter(message->bytes, 12, 52);
}

void CanRx_recievePcuMsg4(CanRx_PcuMsg4_t *rx_data, Can_Message_t *message)
{
  rx_data->operation_state = ByteVector_regularParameter(message->bytes, 4, 0);
  rx_data->battery_state = ByteVector_regularParameter(message->bytes, 4, 1);
  rx_data->term15_state = ByteVector_regularParameter(message->bytes, 2, 4);
  rx_data->term50_state = ByteVector_regularParameter(message->bytes, 2, 5);
  rx_data->fuel_cell.state = ByteVector_regularParameter(message->bytes, 4, 5);

  rx_data->fuel_cell.output_power = 0.1F * ByteVector_regularParameter(message->bytes, 12, 2);
  rx_data->fuel_cell.temp = ByteVector_regularParameter(message->bytes, 8, 5);
  rx_data->rate.hydrogen = ByteVector_regularParameter(message->bytes, 8, 6);
  rx_data->rate.energy = ByteVector_regularParameter(message->bytes, 8, 7);
}
