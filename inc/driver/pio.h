#ifndef __PIO_H__
#define __PIO_H__

#include "driver/sys.h"

#include "lib/int.h"
#include "lib/bits.h"

#define PIO__SET_PERIPHERALS(peripherals) \
MUX_SEL = peripherals

typedef enum Pio_Port
{
  Pio_Port_0
  , Pio_Port_1
  , Pio_Port_2
  , Pio_Port_3

  , Pio_Port_CanTx = Pio_Port_0
  , Pio_Port_CanRx = Pio_Port_0
} Pio_Port_t;

typedef enum Pio_Pin
{
  Pio_Pin_0 = Bits_Bit8_0
  , Pio_Pin_1 = Bits_Bit8_1
  , Pio_Pin_2 = Bits_Bit8_2
  , Pio_Pin_3 = Bits_Bit8_3

  , Pio_Pin_4 = Bits_Bit8_4
  , Pio_Pin_5 = Bits_Bit8_5
  , Pio_Pin_6 = Bits_Bit8_6
  , Pio_Pin_7 = Bits_Bit8_7

  , Pio_Pin_CanTx = Pio_Pin_2
  , Pio_Pin_CanRx = Pio_Pin_3
} Pio_Pin_t;

typedef enum Pio_PinMode
{
  Pio_PinMode_In
  , Pio_PinMode_Out
} Pio_PinMode_t;

typedef enum Pio_Peripheral
{
  Pio_Peripheral_WdtFeedDog = Bits_Bit8_0
  , Pio_Peripheral_WdtControl = Bits_Bit8_1
  , Pio_Peripheral_Uart3 = Bits_Bit8_5
  , Pio_Peripheral_Uart2 = Bits_Bit8_6
  , Pio_Peripheral_Can = Bits_Bit8_7
} Pio_Peripheral_t;

void Pio_setPortMode(Pio_Port_t port, u8 mode);
void Pio_setPinModes(Pio_Port_t port, u8 pins, Pio_PinMode_t mode);
void Pio_setPeripheralModes(u8 peripherals, Bits_State_t mode);

u8 Pio_readPort(Pio_Port_t port);
u8 Pio_readPins(Pio_Port_t port, u8 pins);
void Pio_writePort(Pio_Port_t port, u8 state);
void Pio_writePins(Pio_Port_t port, u8 pins, Bits_State_t state);

#endif


