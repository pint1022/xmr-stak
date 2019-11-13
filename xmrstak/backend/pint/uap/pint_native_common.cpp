/**
 * @defgroup pint-native
 * @ingroup pint-native-common-device
 * Implementation of native support API.
 */

/**
 * PINT native common functions on the device
 *
 * @file    pint_native_const.c
 * @ingroup pint-native-common-device
 * @brief   it defines the common functions on device side codes
 * @author  steven wang
 * @email   yning@panyi.ai
 * @version 0.0.1
 *
 */
#include <stdint.h>
#include "skein_port.h"
#include "pintdev.h"

void
add64 (uint32_t hi_0, uint32_t lo_0, uint32_t hi_1, uint32_t lo_1, uint32_t *hi_out, uint32_t *lo_out)
{
	uint32_t o_lo;
    /* Rotations go modulo 64 */

    o_lo = lo_0 + lo_1;

    *hi_out = hi_0 + hi_1;
//    pint_printf("o_lo \n%x ", o_lo);
    if (o_lo < lo_0) {
//    	o_lo = NUM_4G + o_lo;
//    	pint_printf("negative hi_out \n%x ", *hi_out);
    	*hi_out += 1;
//    	pint_printf("hi_out \n%x ", *hi_out);
   }
    *lo_out = o_lo;
}

void
rot64 (uint32_t hi, uint32_t lo, uint32_t n, uint32_t *hi_out, uint32_t *lo_out)
{
	uint32_t o_hi;
    /* Rotations go modulo 64 */
    n &= 0x3f;

    /* Swap values if 32 <= n < 64 */
    if (n & 0x20) {
        lo ^= hi;
        hi ^= lo;
        lo ^= hi;
    }

    /* Shift 0-31 steps */
    uint8_t shift = n & 0x1f;

    if (!shift) {
        *hi_out = hi;


        *lo_out = lo;
        return;
    }

    o_hi = hi;
    *hi_out = (o_hi << shift) | (lo >> (32 - shift));
    *lo_out = (lo << shift) | (o_hi >> (32 - shift));
}

void
shift64_l (uint32_t hi, uint32_t lo, uint32_t n, uint32_t *hi_out, uint32_t *lo_out)
{
    /* Rotations go modulo 64 */
    n &= 0x3f;

    /* Swap values if 32 <= n < 64 */
    if (n & 0x20) {
        hi = lo;
        lo = 0;
    }

    /* Shift 0-31 steps */
    uint8_t shift = n & 0x1f;

    if (!shift) {
        *hi_out = hi;


        *lo_out = lo;
        return;
    }

    *hi_out = (hi << shift) | (lo >> (32 - shift));
    *lo_out = (lo << shift);
}

void
shift64_r (uint32_t hi, uint32_t lo, uint32_t n, uint32_t *hi_out, uint32_t *lo_out)
{
    /* Rotations go modulo 64 */
    n &= 0x3f;

    /* Swap values if 32 <= n < 64 */
    if (n & 0x20) {
        lo = hi;
        hi = 0;
    }

    /* Shift 0-31 steps */
    uint8_t shift = n & 0x1f;

    if (!shift) {
        *hi_out = hi;


        *lo_out = lo;
        return;
    }

    *lo_out = (lo >> shift) | (hi << (32 - shift));
    *hi_out = (hi >> shift);
}

uint32_t*
shift64_l_VAL (uint32_t hi, uint32_t lo, uint32_t n)
{
	u64b_t _out;
    /* Rotations go modulo 64 */
    n &= 0x3f;

    /* Swap values if 32 <= n < 64 */
    if (n & 0x20) {
        hi = lo;
        lo = 0;
    }

    /* Shift 0-31 steps */
    uint8_t shift = n & 0x1f;

    if (!shift) {
    	_out[1] = hi;
    	_out[0] = lo;
    }

    _out[1] = (hi << shift) | (lo >> (32 - shift));
    _out[0] = (lo << shift);
    return (_out);
}

uint32_t*
shift64_r_VAL (uint32_t hi, uint32_t lo, uint32_t n)
{
	u64b_t _out;
    /* Rotations go modulo 64 */
    n &= 0x3f;

    /* Swap values if 32 <= n < 64 */
    if (n & 0x20) {
        lo = hi;
        hi = 0;
    }

    /* Shift 0-31 steps */
    uint8_t shift = n & 0x1f;

    if (!shift) {
    	_out[1] = hi;
    	_out[0] = lo;
    } else {
    	_out[0] = (lo >> shift) | (hi << (32 - shift));
    	_out[1] = (hi >> shift);
    }

    return (_out);
}

void memcpy(void* dst, void *src, int len)
{
   // Typecast src and dest addresses to (char *)
   char *csrc = (char *)src;
   char *cdest = (char *)dst;

   // Copy contents of src[] to dest[]
   for (int i = 0; i < len; i++)
       cdest[i] = csrc[i];
}

void memset(void* dst, int val, int len) {
   char *cdest = (char *)dst;

   // Copy contents of src[] to dest[]
   for (int i = 0; i < len; i++)
	   cdest[i] = val;
}
