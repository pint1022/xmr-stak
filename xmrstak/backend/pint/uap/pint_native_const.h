/**
 * @defgroup pint-native
 * @ingroup pint-native-api
 * Implementation of native support API.
 */

/**
 * PINT native api
 *
 * @file    pint_native_const.c
 * @ingroup pint-native-api
 * @brief   it defines the common constants among host and device side codes
 * @author  steven wang
 * @email   yning@panyi.ai
 * @version 0.0.1
 *
 */

#ifndef _PINT_NATIVE_CONST_H__
#define _PINT_NATIVE_CONST_H__

#define VALUE_PARAM  0
#define POINTER_PARAM  1

#define VAVIRABLE_NAME_LENGTH  40
#define VAR_NAME 0
#define VAR_ADDR 1
#define VAR_LEN 2


#define OP_RELU			1
#define OP_MATMUL		2
#define OP_BLAKE		3
#define OP_GROESTL		4
#define OP_JH 			5
#define OP_KECCAK 		6
#define OP_KECCAKF 		7
#define OP_KECCAK_MD 	8
#define OP_SKEIN 		9

//
// MM operations
//
#define OP_MM_MUL					1001 //MUL64(MULER, MULCAND, OUT)
#define OP_MM_LOAD					1002 //pint_mm_load_si128(pint_m128i src, pint_m128i dst);
#define OP_MM_ADD					1003 //pint_mm_add_epi64(pint_m128i in_a, pint_m128i in_b, pint_m128i out);
#define OP_MM_SHUFFILE_EPI32		1004 //pint_mm_shuffle_epi32(pint_m128i, int imm8, pint_m128i out);
#define OP_MM_SET_EPI32				1005 //pint_mm_set_epi32(int, int, int, int, pint_m128i out);
#define OP_MM_XOR					1006 //pint_mm_xor_si128(pint_m128i, pint_m128i, pint_m128i out);
#define OP_MM_END					1100 // end of section for MM OP

//void pint_mm_slli_si128(pint_m128i src, int imm8, pint_m128i dst);
//void pint_mm_store_si128(pint_m128i dst, pint_m128i src);
//void pint_mm_cvtsi64_si128(u64b_t in_a, pint_m128i out);
//void pint_mm_set_epi64x(u64b_t e1, u64b_t e0, pint_m128i out);
//void pint_mm_cvtsi128_si64(pint_m128i in_a, u64b_t out);
//uint32_t 	pint_mm_cvtsi128_si32(pint_m128i);

#endif //PINT_NATIVE_CONST
