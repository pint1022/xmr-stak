/**
 * @defgroup pint-native
 * @ingroup pint-native-common-device
 * Implementation of native support API.
 */

/**
 * PINT native common functions on the device
 *
 * @file    pint_native_common.h
 * @ingroup pint-native-common-device
 * @brief   the header file for the common functions on device side codes
 * @author  steven wang
 * @email   yning@panyi.ai
 * @version 0.0.1
 *
 */
#include "skein_port.h"

#ifndef _PINT_NATIVE_COMMON_H__
#define _PINT_NATIVE_COMMON_H__

#define NUM_4G 0x100000000

//void memcpy(void* dst, void *src, int len);
//void memset(void* dst, int val, int len);
void rot64(uint32_t hi, uint32_t lo, uint32_t n, uint32_t *hi_out, uint32_t *lo_out);
void add64 (uint32_t hi_0, uint32_t lo_0, uint32_t hi_1, uint32_t lo_1, uint32_t *hi_out, uint32_t *lo_out);
void shift64_l(uint32_t hi, uint32_t lo, uint32_t n, uint32_t *hi_out, uint32_t *lo_out);
void shift64_r(uint32_t hi, uint32_t lo, uint32_t n, uint32_t *hi_out, uint32_t *lo_out);
uint32_t* shift64_l_VAL(uint32_t hi, uint32_t lo, uint32_t n);
uint32_t* shift64_r_VAL(uint32_t hi, uint32_t lo, uint32_t n);

#define ROTL64(x, y, z) (rot64((x)[1], (x)[0], (y), &((z)[1]), &((z)[0])))

#define SHIFTL64(x,y,z) (shift64_l((x)[1], (x)[0], (y), &((z)[1]), &((z)[0])))
//#define SHIFTL64_1(x,y) (shift64_l_VAL((x)[0], (x)[1], (y))))
#define SHIFTR64(x,y,z) (shift64_r((x)[1], (x)[0], (y), &((z)[1]), &((z)[0])))
//#define SHIFTR64_1(x,y) (shift64_r_VAL((x)[1], (x)[0], (y))))

#define ADD64(x, y, z) (add64((x)[1], (x)[0], (y)[1],(y)[0], &((z)[1]), &((z)[0])))

#define HI(x) ((x)[1])
#define LO(x) ((x)[0])
#define NOT64(x,y) ((y)[0] = ~(x)[0], (y)[1] = ~(x)[1])
#define NOT64_1(x) ({~(x)[0],  ~(x)[1]})
#define AND64(x,y, z) (z[0] = (x)[0] & (y)[0], z[1] = (x)[1] & (y)[1])
#define AND64_1(x,y) (({(x)[0] & (y)[0], (x)[1] & (y)[1]}))
#define OR64(x,y,z) ((z[0]= ((x)[0] | (y)[0])), (z[1] = ((x)[1] | (y)[1])))

#define L64(x) (HI(x)<<32 | LO(x))
#define XOR64(x, y, z) ((z)[0] = (x)[0] ^ (y)[0], (z)[1] = (x)[1] ^ (y)[1])
#define XOR64_1(x, y) ({(x)[0] ^ (y)[0], (x)[1] ^ (y)[1]})
#define ASSIGN64(x, y) (((x)[0] = (y)[0]), ((x)[1] = (y)[1]))

#define LARGE64(x, y) {  								\
	if ((x)[1]>(y)[1])			 						\
		1;												\
    else if (((x)[1] == (y)[1]) && ((x)[0] > (y)[0]))	\
	   1;												\
	else												\
       0;												\
}
#endif
