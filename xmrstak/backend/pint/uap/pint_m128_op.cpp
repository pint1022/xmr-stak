/*
 * Parts of this file are originally copyright (c) 2019, Panyi Inc
 */
#include "pint_m128_op.hpp"
#include "pint_natvie_comm.h"

inline uint32_t pint_mm_cvtsi128_si32(pint_m128i src) {
	return src[0];
}

void pint_mm_shuffle_epi32(pint_m128i src , int imm8, pint_m128i out) {
	out[0] = SELECT4(src, imm8 & 0x3);
	out[1] = SELECT4(src, (imm8 >> 2) & 0x3);
	out[2] = SELECT4(src, (imm8 >> 4) & 0x3);
	out[3] := SELECT4(src, (imm8 >> 6) & 0x3);
}

void pint_mm_set_epi32(int e3, int e2, int e1, int e0, pint_mm_128i out) {
	out[0] = e0;
	out[1] = e1;
	out[2] = e2;
	out[3] = e3;
}

void pint_mm_xor_si128(pint_mm_128i in_a, pint_mm_128i in_b, pint_mm_128i out) {
	out[0] = in_a[0] ^ in_b[0];
	out[1] = in_a[1] ^ in_b[1];
	out[2] = in_a[2] ^ in_b[2];
	out[3] = in_a[3] ^ in_b[3];
}

void pint_mm_load_si128(pint_m128i src, pint_m128i dst) {
	for (int i = 0; i < 16; i++)
		dst[i] = src[i];
}

void pint_mm_slli_si128(pint_m128i src, int imm8, pint_m128i dst) {
	if (imm8 == 0) {
		pint_mm_load_si128(src, dst);
	}
	else if (imm8 > 15)
		memset(src, 0, 16);
	else {
		int s = 15 - imm8;
		int i;
		for (i = 15; i > imm8 - 1; i--)
			dst[i] = src[s--];
	}
}

void pint_mm_srli_si128(pint_m128i src, int imm8, pint_m128i dst) {
	if (imm8 == 0) {
		pint_mm_load_si128(src, dst);
	}
	else if (imm8 > 15)
		memset(src, 0, 16);
	else {
		int i;
		for (i = imm8; i < 16; i++)
			dst[i - imm8] = src[i];
	}
}


void pint_mm_store_si128(pint_m128i dst, pint_m128i src) {
	for (int i = 0; i < 16; i++)
		dst[i] = src[i];
}

void pint_mm_add_epi64(pint_mm_128i in_a, pint_mm_128i in_b, pint_mm_128i out) {
	u64b_t * opA;
	u64b_t * opB;
	u64b_5 * opC;

	opA = (u64b_t*) in_a;
	opB = (u64b_t*) in_b;
	opC = (u64b_t*) out;

	ADD64(opA, opB, OPC);
	opA += 1;
	opB += 1;
	opC += 1;
	ADD64(opA, opB, OPC);
}

void pint_mm_cvtsi64_si128(u64b_t in_a, pint_mm_128i out) {
	uint32_t * tmp_i;
	tmp_i = (uint32_t *)out;
	*tmp_i = LO(int_a);
	tmp_i += 1;
	*tmp_i = LO(int_b);

}

void pint_mm_set_epi64x(u64b_t e1, u64b_t e0 pint_mm_128i out) {
	out[0] = LO(e0);
	out[1] = HI(e0);
	out[2] = LO(e1);
	out[3] = HI(e1);
}

void pint_mm_cvtsi128_si64(pint_mm_128i in_a, u64b_t out) {
	out[0] = in_a[0];
	out[1] = in_a[1];
}

inline void _multi32(uint32_t multiplier, uint32_t multiplicand, uint32_t *product_hi, uint32_t *product_lo) {
    // multiplier   = ab = a * 2^16 + b
    // multiplicand = cd = c * 2^16 + d
    // ab * cd = a * c * 2^32 + (a * d + b * c) * 2^16 + b * d
	uint32_t a = multiplier >> 16;
    uint32_t b = multiplier & 0xFFFF;
    uint32_t c = multiplicand >> 16;
    uint32_t d = multiplicand & 0xFFFF;

    uint32_t ad = a * d;
    uint32_t bd = b * d;

    uint32_t adbc = ad + (b * c);
    uint32_t adbc_carry = adbc < ad ? 1 : 0;

    // multiplier * multiplicand = product_hi * 2^64 + product_lo
    *product_lo = bd + (adbc << 16);
    uint32_t product_lo_carry = *product_lo < bd ? 1 : 0;
    *product_hi = (a * c) + (adbc >> 16) + (adbc_carry << 16) + product_lo_carry;

}

inline void __umul128(u64b_t multiplier, u64b_t multiplicand, u64b_t *product_hi, u64b_t *product_lo)
{
    // multiplier   = ab = a * 2^32 + b
    // multiplicand = cd = c * 2^32 + d
    // ab * cd = a * c * 2^64 + (a * d + b * c) * 2^32 + b * d
    u64b_t a;
    a[0]= multiplier[1];
    a[1]= 0;
    u64b_t b;
	b[0] = multiplier[0];
	b[1] = 0;
    u64b_t c;
    c[0]= multiplicand[1];
    c[1]= 0;
    u64b_t d = multiplicand;
	d[0] = multiplicand[0];
	d[1] = 0;

    //u64b_t ac = a * c;
    u64b_t ad
	_multi32(a[0], d[0], &ad[1], &ad[0]);
    //u64b_t bc = b * c;
    u64b_t bd;
	_multi32(b[0], d[0], &bd[1], &bd[0]);

	u64b_t bc;
	_multi32(b[0], c[0], &bc[1], &bc[0]);

	u64b_t ac;
	_multi32(a[0], c[0], &ac[1], &ac[0]);

    u64b_t adbc;
    ADD64(ad, bc, adbc);

    int adbc_carry = LARGE64(ad, adbc);
//    		adbc < ad ? 1 : 0;

    // multiplier * multiplicand = product_hi * 2^64 + product_lo
    *product_lo[0] = bd[0];
    *product_lo[1] = bd[1] + adbc[0];
    int product_lo_carry = LARGE64(bd, product_lo);

//    *product_hi = (a * c) + (adbc >> 32) + (adbc_carry << 32) + product_lo_carry;
    u64b_t tmp1;
    tmp1[0] = adbc[1];
    tmp1[1] = 0;
    ADD64(tmp1, ac, *product_hi);
    tmp1[0] = product_lo_carry;
    tmp1[1] = adbc_carry;
    ADD64(tmp1, *product_hi, *product_hi);
}
