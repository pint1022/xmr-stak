/*
 * Parts of this file are originally copyright (c) 2019, Panyi Inc
 */
#pragma once

typedef char pint_m128i[128];   // replace _m128i

uint32_t 	pint_mm_cvtsi128_si32(pint_m128i);
void pint_mm_shuffle_epi32(pint_m128i, int imm8, pint_m128i out);
void pint_mm_set_epi32(int, int, int, int, pint_mm_128i out);
void pint_mm_xor_si128(pint_mm_128i, pint_mm_128i, pint_mm_128i out);

__umul128(u64b_t multiplier, u64b_t multiplicand, u64b_t *product_hi, u64b_t *product_lo);

#define SELECT4(src, CTL)                           \
	uint32_t *tmp = (uint32_t *) src;				\
	int idx = (CTL & 0x3);    			 			\
	tmp[idx]

#define MUL64(MULER, MULCAND, OUT) (__umul128((MULER), (MULCAND, &(OUT)[1], &(OUT)[0]))
