#include "logic/inc/app/bytevector.h"

u32 ByteVector_regularParameter(u8 *vector, u32 bit_size, u32 index)
{
  return ByteVector_parameter(vector, bit_size, bit_size * index);
}

u32 ByteVector_parameter(u8 *vector, u32 bit_size, u32 bit_position)
{
  u32 result = 0;
  u32 bit_shift = bit_position % 8;
  u32 i;

  result |= vector[bit_position / 8] >> bit_shift;

  for (i = 8 - bit_shift; i < bit_size; i += 8)
  {
      result |= vector[(bit_position + i) / 8] << i;
  }

  result <<= 32 - bit_size;
  result >>= 32 - bit_size;
  return result;
}
