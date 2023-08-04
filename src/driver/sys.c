#include "driver/sys.h"

#include "driver/pio.h"
#include "lib/bits.h"

void Sys_init(void)
{
// Look to STARTUP.A51
//  PAGESEL = Bits_Bit8_0;
//  D_PAGESEL = Bits_Bit8_1;
//  CKCON = 0;
//  DPC = 0;
//  T2CON = Bits_Bit8_6 | Bits_Bit8_5 | Bits_Bit8_4;
//  PIO__SET_PERIPHERALS(Pio_Peripheral_Uart2 | Pio_Peripheral_Uart3);
//  RAMMODE = 0;
//  PORTDRV = Bits_Bit8_0;
//  IEN0 = 0;
//  IEN1 = 0;
//  IEN2 = 0;
//  IP0 = 0;
//  IP1 = 0;
//  PSW &= ~(Bits_Bit8_4 | Bits_Bit8_3);
}
