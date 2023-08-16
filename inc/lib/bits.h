#ifndef __BITS_H__
#define __BITS_H__

typedef enum Bits_Bit8
{
  BITS__BIT8_0 = 1 << 0
  , BITS__BIT8_1 = 1 << 1
  , BITS__BIT8_2 = 1 << 2
  , BITS__BIT8_3 = 1 << 3

  , BITS__BIT8_4 = 1 << 4
  , BITS__BIT8_5 = 1 << 5
  , BITS__BIT8_6 = 1 << 6
  , BITS__BIT8_7 = 1 << 7
} Bits_Bit8_t;

typedef enum Bits_State
{
  BITS__STATE_NOT_SET
  , BITS__STATE_SET
} Bits_State_t;

#endif
