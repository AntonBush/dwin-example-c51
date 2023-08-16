#include "driver/pio.h"

#include "driver/sys.h"

// internal implementation defines

static u8 Pio_portMode(Pio_Port_t port);

// interface implementation

void Pio_setPortMode(Pio_Port_t port, u8 mode)
{
  switch (port)
  {
    case PIO__PORT_0:
      P0MDOUT = mode;
      break;

    case PIO__PORT_1:
      P1MDOUT = mode;
      break;

    case PIO__PORT_2:
      P2MDOUT = mode;
      break;

    case PIO__PORT_3:
      P3MDOUT = mode;
      break;
  }
}

void Pio_setPinModes(Pio_Port_t port, u8 pins, Pio_PinMode_t mode)
{
  u8 old_mode = Pio_portMode(port);
  u8 new_mode
    = mode == PIO__PIN_MODE_OUT
    ? old_mode | pins
    : old_mode & (~pins);
  Pio_setPortMode(port, new_mode);
}

void Pio_setPeripheralModes(u8 peripherals, Bits_State_t mode)
{
  u8 old_mode = MUX_SEL;
  MUX_SEL
    = mode == BITS__STATE_SET
    ? old_mode | peripherals
    : old_mode & (~peripherals);
}

u8 Pio_readPort(Pio_Port_t port)
{
  switch (port)
  {
    case PIO__PORT_0:
      return P0;

    case PIO__PORT_1:
      return P1;

    case PIO__PORT_2:
      return P2;

    case PIO__PORT_3:
      return P3;
  }
  return 0;
}

u8 Pio_readPins(Pio_Port_t port, u8 pins)
{
  return Pio_readPort(port) & pins;
}

void Pio_writePort(Pio_Port_t port, u8 state)
{
  switch (port)
  {
    case PIO__PORT_0:
      P0 = state;
      break;

    case PIO__PORT_1:
      P1 = state;
      break;

    case PIO__PORT_2:
      P2 = state;
      break;

    case PIO__PORT_3:
      P3 = state;
      break;
  }
}

void Pio_writePins(Pio_Port_t port, u8 pins, Bits_State_t state)
{
  u8 old_state = Pio_readPort(port);
  u8 new_state
    = state == BITS__STATE_SET
    ? old_state | pins
    : old_state & (~pins);
  Pio_writePort(port, new_state);
}

// internal implementation

u8 Pio_portMode(Pio_Port_t port)
{
  switch (port)
  {
    case PIO__PORT_0:
      return P0MDOUT;

    case PIO__PORT_1:
      return P1MDOUT;

    case PIO__PORT_2:
      return P2MDOUT;

    case PIO__PORT_3:
      return P3MDOUT;
  }
  return 0;
}
