#include "driver/sys.h"

#include "driver/pio.h"
#include "lib/bits.h"

void Sys_init(void)
{
// Look to STARTUP.A51
//  PAGESEL = BITS__BIT8_0;
//  D_PAGESEL = BITS__BIT8_1;
//  CKCON = 0;
//  DPC = 0;
//  T2CON = BITS__BIT8_6 | BITS__BIT8_5 | BITS__BIT8_4;
//  PIO__SET_PERIPHERALS(PIO__PERIPHERAL_UART2 | PIO__PERIPHERAL_UART3);
//  RAMMODE = 0;
//  PORTDRV = BITS__BIT8_0;
//  IEN0 = 0;
//  IEN1 = 0;
//  IEN2 = 0;
//  IP0 = 0;
//  IP1 = 0;
//  PSW &= ~(BITS__BIT8_4 | BITS__BIT8_3);
}
