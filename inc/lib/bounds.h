#ifndef __BOUNDS_H__
#define __BOUNDS_H__

#include "lib/int.h"

typedef struct Bounds_U8Bounds
{
  u8 min;
  u8 max;
} Bounds_U8Bounds_t;

typedef struct Bounds_U16Bounds
{
  u16 min;
  u16 max;
} Bounds_U16Bounds_t;

typedef struct Bounds_S8Bounds
{
  s8 min;
  s8 max;
} Bounds_S8Bounds_t;

typedef struct Bounds_S16Bounds
{
  s16 min;
  s16 max;
} Bounds_S16Bounds_t;

typedef struct Bounds_FloatBounds
{
  float min;
  float max;
} Bounds_FloatBounds_t;

#endif
