/*
 * Parts of this file are originally copyright (c) 2019, Panyi Inc
 */
#pragma once

typedef char pint_m128i[128];   // replace _m128i


inline uint32_t SELECT4(char *src, int CTL) {
	uint32_t * tmp = (uint32_t *) src;
	int idx = (CTL & 0x3);
	return (tmp[idx]);
}


void pint_mm_load_si128(pint_m128i src, pint_m128i dst);
void pint_mm_slli_si128(pint_m128i src, int imm8, pint_m128i dst);
void pint_mm_srli_si128(pint_m128i src, int imm8, pint_m128i dst);
void pint_mm_store_si128(pint_m128i dst, pint_m128i src);
void pint_mm_add_epi64(pint_m128i in_a, pint_m128i in_b, pint_m128i out);
void pint_mm_cvtsi64_si128(u64b_t in_a, pint_m128i out);
void pint_mm_set_epi64x(u64b_t e1, u64b_t e0, pint_m128i out);
void pint_mm_cvtsi128_si64(pint_m128i in_a, u64b_t out);
uint32_t 	pint_mm_cvtsi128_si32(pint_m128i);
void pint_mm_shuffle_epi32(pint_m128i, int imm8, pint_m128i out);
void pint_mm_set_epi32(int, int, int, int, pint_m128i out);
void pint_mm_xor_si128(pint_m128i, pint_m128i, pint_m128i out);
void __umul128(u64b_t multiplier, u64b_t multiplicand, u64b_t *product_hi, u64b_t *product_lo);

//#define MUL64(MULER, MULCAND, OUT) (__umul128((MULER), (MULCAND), ((uint8_t *)(OUT) + 8), OUT))
#define MUL64(MULER, MULCAND, OUT) (__umul128((MULER), (MULCAND), &((OUT)[1]), &((OUT)[0])))
