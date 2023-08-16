#ifndef __PIO_H__
#define __PIO_H__

#include "driver/sys.h"

#include "lib/int.h"
#include "lib/bits.h"

#define PIO__SET_PERIPHERALS(peripherals) \
MUX_SEL = peripherals

typedef enum Pio_Port
{
  PIO__PORT_0
  , PIO__PORT_1
  , PIO__PORT_2
  , PIO__PORT_3

  , PIO__PORT_CAN_TX = PIO__PORT_0
  , PIO__PORT_CAN_RX = PIO__PORT_0
} Pio_Port_t;

typedef enum Pio_Pin
{
  PIO__PIN_0 = BITS__BIT8_0
  , PIO__PIN_1 = BITS__BIT8_1
  , PIO__PIN_2 = BITS__BIT8_2
  , PIO__PIN_3 = BITS__BIT8_3

  , PIO__PIN_4 = BITS__BIT8_4
  , PIO__PIN_5 = BITS__BIT8_5
  , PIO__PIN_6 = BITS__BIT8_6
  , PIO__PIN_7 = BITS__BIT8_7

  , PIO__PIN_CAN_TX = PIO__PIN_2
  , PIO__PIN_CAN_RX = PIO__PIN_3
} Pio_Pin_t;

typedef enum Pio_PinMode
{
  PIO__PIN_MODE_IN
  , PIO__PIN_MODE_OUT
} Pio_PinMode_t;

typedef enum Pio_Peripheral
{
  PIO__PERIPHERAL_WDT_FEED_DOG = BITS__BIT8_0
  , PIO__PERIPHERAL_WDT_CONTROL = BITS__BIT8_1
  , PIO__PERIPHERAL_UART3 = BITS__BIT8_5
  , PIO__PERIPHERAL_UART2 = BITS__BIT8_6
  , PIO__PERIPHERAL_CAN = BITS__BIT8_7
} Pio_Peripheral_t;

void Pio_setPortMode(Pio_Port_t port, u8 mode);
void Pio_setPinModes(Pio_Port_t port, u8 pins, Pio_PinMode_t mode);
void Pio_setPeripheralModes(u8 peripherals, Bits_State_t mode);

u8 Pio_readPort(Pio_Port_t port);
u8 Pio_readPins(Pio_Port_t port, u8 pins);
void Pio_writePort(Pio_Port_t port, u8 state);
void Pio_writePins(Pio_Port_t port, u8 pins, Bits_State_t state);

#endif


