/* utils.c
 *
 * 32 bit fractional multiplication. Requires 64 bit integer support.
 */
#include "stdafx.h"
/* Fractional multiply. */
long mul(long x, long y)
{
  return (long)(((long long)x * (long long)y) >> 32);
}

long mul(int x, int y)
{
  return (long)(((long long)x * (long long)y) >> 32);
}
/* Left justified fractional multiply. */
long muls(long x, long y)
{
  return (long)(((long long)x * (long long)y) >> 31);
}

long muls(int x, int y)
{
  return (long)(((long long)x * (long long)y) >> 31);
}
/* Fractional multiply with rounding. */
long mulr(long x, long y)
{
  return (long)((((long long)x * (long long)y) + 0x80000000) >> 32);
}

long mulr(int x, int y)
{
  return (long)((((long long)x * (long long)y) + 0x80000000) >> 32);
}
/* Left justified fractional multiply with rounding. */
long mulsr(long x, long y)
{
  return (long)((((long long)x * (long long)y) + 0x40000000) >> 31);
}

