#ifndef _SKEIN_PORT_H_
#define _SKEIN_PORT_H_

#include <limits.h>
//#include <stddef.h>
#include <stdint.h>
#include "pint_native_common.h"

#ifndef RETURN_VALUES
#define RETURN_VALUES
#if defined(DLL_EXPORT)
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#define VOID_RETURN __declspec(dllexport) void __stdcall
#define INT_RETURN __declspec(dllexport) int __stdcall
#elif defined(__GNUC__)
#define VOID_RETURN __declspec(__dllexport__) void
#define INT_RETURN __declspec(__dllexport__) int
#else
#error Use of the DLL is only available on the Microsoft, Intel and GCC compilers
#endif
#elif defined(DLL_IMPORT)
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#define VOID_RETURN __declspec(dllimport) void __stdcall
#define INT_RETURN __declspec(dllimport) int __stdcall
#elif defined(__GNUC__)
#define VOID_RETURN __declspec(__dllimport__) void
#define INT_RETURN __declspec(__dllimport__) int
#else
#error Use of the DLL is only available on the Microsoft, Intel and GCC compilers
#endif
#elif defined(__WATCOMC__)
#define VOID_RETURN void __cdecl
#define INT_RETURN int __cdecl
#else
#define VOID_RETURN void
#define INT_RETURN int
#endif
#endif

/*  These defines are used to declare buffers in a way that allows
	faster operations on longer variables to be used.  In all these
	defines 'size' must be a power of 2 and >= 8

	dec_unit_type(size,x)       declares a variable 'x' of length
								'size' bits

	dec_bufr_type(size,bsize,x) declares a buffer 'x' of length 'bsize'
								bytes defined as an array of variables
								each of 'size' bits (bsize must be a
								multiple of size / 8)

	ptr_cast(x,size)            casts a pointer to a pointer to a
								variable of length 'size' bits
*/

#define ui_type(size) uint##size##_t
#define dec_unit_type(size, x) typedef ui_type(size) x
#define dec_bufr_type(size, bsize, x) typedef ui_type(size) x[bsize / (size >> 3)]
#define ptr_cast(x, size) ((ui_type(size)*)(x))

typedef unsigned int uint_t; /* native unsigned integer */
typedef uint8_t u08b_t;		 /*  8-bit unsigned integer */
typedef uint32_t u64b_t[2];	 /* 64-bit unsigned integer */

#ifndef RotL_64
//#define RotL_64(x, N) (((x) << (N)) | ((x) >> (64 - (N))))
#define RotL_64(x, N) (ROT64(x, N, x))
#endif

/*
 * Skein is "natively" little-endian (unlike SHA-xxx), for optimal
 * performance on x86 CPUs.  The Skein code requires the following
 * definitions for dealing with endianness:
 *
 *    SKEIN_NEED_SWAP:  0 for little-endian, 1 for big-endian
 *    Skein_Put64_LSB_First
 *    Skein_Get64_LSB_First
 *    Skein_Swap64
 *
 * If SKEIN_NEED_SWAP is defined at compile time, it is used here
 * along with the portable versions of Put64/Get64/Swap64, which
 * are slow in general.
 *
 * Otherwise, an "auto-detect" of endianness is attempted below.
 * If the default handling doesn't work well, the user may insert
 * platform-specific code instead (e.g., for big-endian CPUs).
 *
 */
#ifndef SKEIN_NEED_SWAP /* compile-time "override" for endianness? */

#define PLATFORM_BYTE_ORDER IS_LITTLE_ENDIAN

/* special handler for IA64, which may be either endianness (?)  */
/* here we assume little-endian, but this may need to be changed */
#if defined(__ia64) || defined(__ia64__) || defined(_M_IA64)
#define PLATFORM_MUST_ALIGN (1)
#ifndef PLATFORM_BYTE_ORDER
#define PLATFORM_BYTE_ORDER IS_LITTLE_ENDIAN
#endif
#endif

#ifndef PLATFORM_MUST_ALIGN
#define PLATFORM_MUST_ALIGN (0)
#endif

#if PLATFORM_BYTE_ORDER == IS_BIG_ENDIAN
/* here for big-endian CPUs */
#define SKEIN_NEED_SWAP (1)
#elif PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN
/* here for x86 and x86-64 CPUs (and other detected little-endian CPUs) */
#define SKEIN_NEED_SWAP (0)
#if PLATFORM_MUST_ALIGN == 0 /* ok to use "fast" versions? */
#define Skein_Put64_LSB_First(dst08, src64, bCnt) memcpy(dst08, src64, bCnt)
#define Skein_Get64_LSB_First(dst64, src08, wCnt) memcpy(dst64, src08, 8 * (wCnt))
#endif
#else
#error "Skein needs endianness setting!"
#endif

#endif /* ifndef SKEIN_NEED_SWAP */

/*
 ******************************************************************
 *      Provide any definitions still needed.
 ******************************************************************
 */
#ifndef Skein_Swap64 /* swap for big-endian, nop for little-endian */
//#if SKEIN_NEED_SWAP
//#define Skein_Swap64(w64)                          \
//	(((((u64b_t)(w64)) & 0xFF) << 56) |            \
//		(((((u64b_t)(w64)) >> 8) & 0xFF) << 48) |  \
//		(((((u64b_t)(w64)) >> 16) & 0xFF) << 40) | \
//		(((((u64b_t)(w64)) >> 24) & 0xFF) << 32) | \
//		(((((u64b_t)(w64)) >> 32) & 0xFF) << 24) | \
//		(((((u64b_t)(w64)) >> 40) & 0xFF) << 16) | \
//		(((((u64b_t)(w64)) >> 48) & 0xFF) << 8) |  \
//		(((((u64b_t)(w64)) >> 56) & 0xFF)))
//#else
#define Skein_Swap64(w64) (w64)
//#endif
#endif /* ifndef Skein_Swap64 */

#ifndef Skein_Put64_LSB_First
void Skein_Put64_LSB_First(u08b_t* dst, u64b_t* src, uint32_t bCnt)
#ifdef SKEIN_PORT_CODE /* instantiate the function code here? */
{					   /* this version is fully portable (big-endian or little-endian), but slow */
	uint32_t n;
	int i, bit_c;

	for(n = 0, i = 0; n < bCnt; n += 8, i++) {
		*((uint32_t *)(dst + n)) = src[i][0];
		*((uint32_t *)(dst + n + 4)) = src[i][1];
//		 i = bCnt / 32; //index to the 32bit array: 0 low 32bit, 1 hi 32bit.
//		 if (i == 1)
//			 bit_c = n - 32;
//		dst[n] = (u08b_t)(src[i][bit_c >> 3] >> (8 * (bit_c & 7)));
	}
}
#else
	; /* output only the function prototype */
#endif
#endif /* ifndef Skein_Put64_LSB_First */

#ifndef Skein_Get64_LSB_First
void Skein_Get64_LSB_First(u64b_t* dst, u08b_t* src, uint32_t wCnt)
#ifdef SKEIN_PORT_CODE /* instantiate the function code here? */
{					   /* this version is fully portable (big-endian or little-endian), but slow */
	uint32_t n;

	for(n = 0; n < 8 * wCnt; n += 8) {
		dst[n/8][0] = (uint32_t) *(src + n);
		dst[n/8][1] = (uint32_t) *(src + n + 4);
	}
}
#else
	; /* output only the function prototype */
#endif
#endif /* ifndef Skein_Get64_LSB_First */

#endif /* ifndef _SKEIN_PORT_H_ */
