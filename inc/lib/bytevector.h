#ifndef __BYTEVECTOR_H__
#define __BYTEVECTOR_H__

#include "lib/int.h"

u32 ByteVector_regularParameter(u8 *vector, u32 bit_size, u32 index);
u32 ByteVector_parameter(u8 *vector, u32 bit_size, u32 bit_position);

#endif
