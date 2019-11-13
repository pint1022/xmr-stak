// keccak.c
// Nov-1-2019  Steven Wang <panyi.ai>
// A 32bit Keccak (3rd round) implementation.

#include <pintdev.h>
#include "c_keccak.h"
#include <stdint.h>
#include "pint_native_common.h"

#define HASH_DATA_AREA 136
#define KECCAK_ROUNDS 24

//#define ROTL64(x, y, z) (rot64(x[0], x[1], y, &(z[0]), &(z[1])))
//#define XOR64(x, y, z) (z[0] = ((x)[0] ^ (y)[0]), z[1] = ((x)[1] ^ (y)[1]))

uint64b_t keccakf_rndc[] =
{
		// {lo, hi}
		{0x00000001, 0x00000000},
		{0x00008082, 0x00000000},
		{0x0000808a, 0x80000000},
		{0x80008000, 0x80000000},
		{0x0000808b, 0x00000000},
		{0x80000001, 0x00000000},
		{0x80008081, 0x80000000},
		{0x00008009, 0x80000000},
		{0x0000008a, 0x00000000},
		{0x00000088, 0x00000000},
		{0x80008009, 0x00000000},
		{0x8000000a, 0x00000000},
		{0x8000808b, 0x00000000},
		{0x0000008b, 0x80000000},
		{0x00008089, 0x80000000},
		{0x00008003, 0x80000000},
		{0x00008002, 0x80000000},
		{0x00000080, 0x80000000},
		{0x0000800a, 0x00000000},
		{0x8000000a, 0x80000000},
		{0x80008081, 0x80000000},
		{0x00008080, 0x80000000},
		{0x80000001, 0x00000000},
		{0x80008008, 0x80000000}
	// {hi, lo}
//	{0x00000000, 0x00000001},
//	{0x00000000, 0x00008082},
//	{0x80000000, 0x0000808a},
//	{0x80000000, 0x80008000},
//	{0x00000000, 0x0000808b},
//	{0x00000000, 0x80000001},
//	{0x80000000, 0x80008081},
//	{0x80000000, 0x00008009},
//	{0x00000000, 0x0000008a},
//	{0x00000000, 0x00000088},
//	{0x00000000, 0x80008009},
//	{0x00000000, 0x8000000a},
//	{0x00000000, 0x8000808b},
//	{0x80000000, 0x0000008b},
//	{0x80000000, 0x00008089},
//	{0x80000000, 0x00008003},
//	{0x80000000, 0x00008002},
//	{0x80000000, 0x00000080},
//	{0x00000000, 0x0000800a},
//	{0x80000000, 0x8000000a},
//	{0x80000000, 0x80008081},
//	{0x80000000, 0x00008080},
//	{0x00000000, 0x80000001},
//	{0x80000000, 0x80008008}
};
// update the state with given number of rounds

void print_keccak(char* nm, uint64b_t *S, int count) {
	pint_printf("\n%s\n", nm);
	for (int i = 0; i < count; i++)
		pint_printf("%d: %x%x\n ",i, HI(S[i]), LO(S[i]));
	pint_printf("\n");
}

//void
//rot64 (uint32_t hi, uint32_t lo, uint32_t n, uint32_t *hi_out, uint32_t *lo_out)
//{
//    /* Rotations go modulo 64 */
//    n &= 0x3f;
//
//    /* Swap values if 32 <= n < 64 */
//    if (n & 0x20) {
//        lo ^= hi;
//        hi ^= lo;
//        lo ^= hi;
//    }
//
//    /* Shift 0-31 steps */
//    uint8_t shift = n & 0x1f;
//
//    if (!shift) {
//        *hi_out = hi;
//
//
//        *lo_out = lo;
//        return;
//    }
//
//    *hi_out = (hi << shift) | (lo >> (32 - shift));
//    *lo_out = (lo << shift) | (hi >> (32 - shift));
//}

void keccakf(uint64b_t st[25], int rounds)
{
	int i, j, round;
	uint64b_t t, bc[5];
	uint64b_t tmp_C;

	int log_r = 0;
	for(round = 0; round < rounds; ++round)
	{
//		if (round < log_r)
//			print_keccak("st", st, 25);

//
//		// Theta
//		ASSIGN64(bc[0], XOR64_1(XOR64_1(XOR64_1(XOR64_1(st[0],st[5]),st[10]),st[15]),st[20]));
//		bc[0] = st[0] ^ st[5] ^ st[10] ^ st[15] ^ st[20];
//		bc[0][0] = st[0][0] ^ st[5][0];
//		pint_printf("bc[0][0] %d: %x\n, %x %x \n", round, bc[0][0], st[0][0] ,st[5][0]);
//		bc[0][0] ^= st[10][0];
//		pint_printf("bc[0][0] %d: %x\n ", round, bc[0][0]);
//		bc[0][0] ^= st[15][0];
//		pint_printf("bc[0][0] %d: %x\n ", round, bc[0][0]);
//		bc[0][0] ^= st[20][0];
//		pint_printf("bc[0][0] %d: %x\n ", round, bc[0][0]);
		bc[0][0] = st[0][0] ^ st[5][0] ^ st[10][0] ^ st[15][0] ^ st[20][0];
		bc[0][1] = st[0][1] ^ st[5][1] ^ st[10][1] ^ st[15][1] ^ st[20][1];
//		ASSIGN64(bc[1], XOR64(XOR64(XOR64(XOR64(st[1],st[6]),st[11]),st[16]),st[21]));
		bc[1][0] = st[1][0] ^ st[6][0] ^ st[11][0] ^ st[16][0] ^ st[21][0];
		bc[1][1] = st[1][1] ^ st[6][1] ^ st[11][1] ^ st[16][1] ^ st[21][1];
//		ASSIGN64(bc[2], XOR64(XOR64(XOR64(XOR64(st[2],st[7]),st[12]),st[17]),st[22]));
		bc[2][0] = st[2][0] ^ st[7][0] ^ st[12][0] ^ st[17][0] ^ st[22][0];
		bc[2][1] = st[2][1] ^ st[7][1] ^ st[12][1] ^ st[17][1] ^ st[22][1];
//		ASSIGN64(bc[3], XOR64(XOR64(XOR64(XOR64(st[3],st[8]),st[13]),st[18]),st[23]));
		bc[3][0] = st[3][0] ^ st[8][0] ^ st[13][0] ^ st[18][0] ^ st[23][0];
		bc[3][1] = st[3][1] ^ st[8][1] ^ st[13][1] ^ st[18][1] ^ st[23][1];
//		ASSIGN64(bc[4], XOR64(XOR64(XOR64(XOR64(st[4],st[9]),st[14]),st[19]),st[24]));
		bc[4][0] = st[4][0] ^ st[9][0] ^ st[14][0] ^ st[19][0] ^ st[24][0];
		bc[4][1] = st[4][1] ^ st[9][1] ^ st[14][1] ^ st[19][1] ^ st[24][1];

//		if (round < log_r)
//			print_keccak("bc", bc, 5);

		for(i = 0; i < 5; ++i)
		{
			ROTL64(bc[(i + 1) % 5], 1, tmp_C);
//			t[0] = bc[(i + 4) % 5][0] ^ tmp_C[0];
//			t[1] = bc[(i + 4) % 5][1] ^ tmp_C[1];
			XOR64(bc[(i + 4) % 5], tmp_C, t);
//			st[i][0] ^= t[0];
//			st[i][1] ^= t[1];
			XOR64(st[i], t, st[i]);
//			st[i + 5][0] ^= t[0];
//			st[i + 5][1] ^= t[1];
//			XOR64(st[i + 5], t, st[i + 5])
			XOR64(st[i + 5], t, st[i + 5]);
//			st[i + 10][0] ^= t[0];
//			st[i + 10][1] ^= t[1];
//			XOR64(st[i + 10], t, st[i + 10])
			XOR64(st[i + 10], t, st[i + 10]);
//			st[i + 15][0] ^= t[0];
//			st[i + 15][1] ^= t[1];
			XOR64(st[i + 15], t, st[i + 15]);
//			XOR64(st[i + 15], t, st[i + 15])
//			st[i + 20][0] ^= t[0];
//			st[i + 20][1] ^= t[1];
			XOR64(st[i + 20], t, st[i + 20]);
//			XOR64(st[i + 20], t, st[i + 20])
		}
		if (round < log_r)
			print_keccak(" --- states -", st, 25);

		// Rho Pi
//		t[0] = st[1][0];
//		t[1] = st[1][1];
		ASSIGN64(t, st[1]);
		ROTL64(st[6], 44, st[1]);
//		rot64(st[6][0], st[6][1], 44, &(st[1][0]), &(st[1][1]));

		ROTL64(st[9], 20, st[6]);
		ROTL64(st[22], 61, st[9]);
		ROTL64(st[14], 39, st[22]); //x
		ROTL64(st[20], 18, st[14]);
		ROTL64(st[2], 62, st[20]); //x
		ROTL64(st[12], 43, st[2]);
		ROTL64(st[13], 25, st[12]);
		ROTL64(st[19], 8, st[13]);
		ROTL64(st[23], 56, st[19]);
		ROTL64(st[15], 41, st[23]);//x
		ROTL64(st[4], 27, st[15]);
		ROTL64(st[24], 14, st[4]);
		ROTL64(st[21], 2, st[24]);
		ROTL64(st[8], 55, st[21]);//x
		ROTL64(st[16], 45, st[8]);
		ROTL64(st[5], 36, st[16]);
		ROTL64(st[3], 28, st[5]);
		ROTL64(st[18], 21, st[3]);
		ROTL64(st[17], 15, st[18]);
		ROTL64(st[11], 10, st[17]);
		ROTL64(st[7], 6, st[11]);
		ROTL64(st[10], 3, st[7]);
		ROTL64(t, 1, st[10]);
//		if (round < log_r)
//			print_keccak(" --- states --", st, 25);
		//  Chi
		// unrolled loop, where only last iteration is different
		j = 0;
		ASSIGN64(bc[0], st[j]);
		ASSIGN64(bc[1], st[j + 1]);


		NOT64(st[j + 1], tmp_C);
		AND64(tmp_C, st[j + 2], tmp_C);
		XOR64(tmp_C, st[j], st[j]);

		NOT64(st[j + 2], tmp_C);
		AND64(tmp_C, st[j + 3], tmp_C);
		XOR64(tmp_C, st[j + 1], st[j + 1]);

		NOT64(st[j + 3], tmp_C);
		AND64(tmp_C, st[j + 4], tmp_C);
		XOR64(tmp_C, st[j + 2], st[j + 2]);

		NOT64(st[j + 4], tmp_C);
		AND64(tmp_C, bc[0], tmp_C);
		XOR64(tmp_C, st[j + 3], st[j + 3]);

		NOT64(bc[0], tmp_C);
		AND64(tmp_C, bc[1], tmp_C);
		XOR64(tmp_C, st[j + 4], st[j + 4]);

		j = 5;
		ASSIGN64(bc[0], st[j]);
		ASSIGN64(bc[1], st[j + 1]);

		NOT64(st[j + 1], tmp_C);
		AND64(tmp_C, st[j + 2], tmp_C);
		XOR64(tmp_C, st[j], st[j]);

		NOT64(st[j + 2], tmp_C);
		AND64(tmp_C, st[j + 3], tmp_C);
		XOR64(tmp_C, st[j + 1], st[j + 1]);

		NOT64(st[j + 3], tmp_C);
		AND64(tmp_C, st[j + 4], tmp_C);
		XOR64(tmp_C, st[j + 2], st[j + 2]);

		NOT64(st[j + 4], tmp_C);
		AND64(tmp_C, bc[0], tmp_C);
		XOR64(tmp_C, st[j + 3], st[j + 3]);

		NOT64(bc[0], tmp_C);
		AND64(tmp_C, bc[1], tmp_C);
		XOR64(tmp_C, st[j + 4], st[j + 4]);

		j = 10;
		ASSIGN64(bc[0], st[j]);
		ASSIGN64(bc[1], st[j + 1]);

		NOT64(st[j + 1], tmp_C);
		AND64(tmp_C, st[j + 2], tmp_C);
		XOR64(tmp_C, st[j], st[j]);

		NOT64(st[j + 2], tmp_C);
		AND64(tmp_C, st[j + 3], tmp_C);
		XOR64(tmp_C, st[j + 1], st[j + 1]);

		NOT64(st[j + 3], tmp_C);
		AND64(tmp_C, st[j + 4], tmp_C);
		XOR64(tmp_C, st[j + 2], st[j + 2]);

		NOT64(st[j + 4], tmp_C);
		AND64(tmp_C, bc[0], tmp_C);
		XOR64(tmp_C, st[j + 3], st[j + 3]);

		NOT64(bc[0], tmp_C);
		AND64(tmp_C, bc[1], tmp_C);
		XOR64(tmp_C, st[j + 4], st[j + 4]);

		j = 15;
		ASSIGN64(bc[0], st[j]);
		ASSIGN64(bc[1], st[j + 1]);

		NOT64(st[j + 1], tmp_C);
		AND64(tmp_C, st[j + 2], tmp_C);
		XOR64(tmp_C, st[j], st[j]);

		NOT64(st[j + 2], tmp_C);
		AND64(tmp_C, st[j + 3], tmp_C);
		XOR64(tmp_C, st[j + 1], st[j + 1]);

		NOT64(st[j + 3], tmp_C);
		AND64(tmp_C, st[j + 4], tmp_C);
		XOR64(tmp_C, st[j + 2], st[j + 2]);

		NOT64(st[j + 4], tmp_C);
		AND64(tmp_C, bc[0], tmp_C);
		XOR64(tmp_C, st[j + 3], st[j + 3]);

		NOT64(bc[0], tmp_C);
		AND64(tmp_C, bc[1], tmp_C);
		XOR64(tmp_C, st[j + 4], st[j + 4]);

		j = 20;
		ASSIGN64(bc[0], st[j]);
		ASSIGN64(bc[1], st[j + 1]);
		ASSIGN64(bc[2], st[j + 2]);
		ASSIGN64(bc[3], st[j + 3]);
		ASSIGN64(bc[4], st[j + 4]);
//		if (round < log_r)
//			print_keccak("  bc -", bc, 5);

//		st[j] ^= (~bc[1]) & bc[2];
		NOT64(bc[1], tmp_C);
		AND64(tmp_C, bc[2], tmp_C);
		XOR64(tmp_C, st[j], st[j]);
//		st[j + 1] ^= (~bc[2]) & bc[3];
		NOT64(bc[2], tmp_C);
		AND64(tmp_C, bc[3], tmp_C);
		XOR64(tmp_C, st[j + 1], st[j + 1]);
//		st[j + 2] ^= (~bc[3]) & bc[4];
		NOT64(bc[3], tmp_C);
		AND64(tmp_C, bc[4], tmp_C);
		XOR64(tmp_C, st[j + 2], st[j + 2]);
//		st[j + 3] ^= (~bc[4]) & bc[0];
		NOT64(bc[4], tmp_C);
		AND64(tmp_C, bc[0], tmp_C);
		XOR64(tmp_C, st[j + 3], st[j + 3]);
//		st[j + 4] ^= (~bc[0]) & bc[1];
		NOT64(bc[0], tmp_C);
		AND64(tmp_C, bc[1], tmp_C);
		XOR64(tmp_C, st[j + 4], st[j + 4]);

//		if (round < log_r)
//			print_keccak(" --- states ---", st, 25);

		//  Iota
//		st[0] ^= keccakf_rndc[round];
		XOR64(st[0], keccakf_rndc[round], st[0]);

	}
}

// compute a keccak hash (md) of given byte length from "in"
typedef uint64b_t state_t[25];

void keccak(uint8_t* in, int inlen, uint8_t* md, int mdlen)
{
	state_t st;
	uint8_t temp[144];
	int i, rsiz, rsizw;

	rsiz = sizeof(state_t) == mdlen ? HASH_DATA_AREA : 200 - 2 * mdlen;
	rsizw = rsiz / 8;
//	pint_printf("rsiz %d, rsizw %d, mdlen %d, size state_t %d\n", rsiz, rsizw, mdlen, sizeof(state_t));

	memset(st, 0, (int)sizeof(st));

//	pint_printf("after memset \n");
//	print_keccak("states", st, 24);
	for(; inlen >= rsiz; inlen -= rsiz, in += rsiz)
	{
		for(i = 0; i < rsizw; i++) {
//			ASSIGN64(st[i], XOR64(((uint64b_t*)in)[i], st[i]));
//			pint_printf("\nxor input i=%d\n", i);
			XOR64(((uint64b_t*)in)[i], st[i], st[i]);
//			print_keccak("states", st, 5);
		}
//			st[i] ^= ((uintb64_t*)in)[i];
		keccakf(st, KECCAK_ROUNDS);
	}

	// last block and padding
	memcpy(temp, in, inlen);
	temp[inlen++] = 1;
	memset(temp + inlen, 0, rsiz - inlen);
	temp[rsiz - 1] |= 0x80;
//	for(i = 0; i < rsiz; i++)
//		pint_printf("%x ", temp[i]);
//	pint_printf("\n");
//	print_keccak("temp",(uint64b_t*) temp, rsizw);

	for(i = 0; i < rsizw; i++) {
//		st[i] ^= ((uint64_t*)temp)[i];
		XOR64(((uint64b_t*)temp)[i], st[i], st[i]);
//		pint_printf("\nxor input i=%d\n", i);
//		print_keccak("states", st, 5);
	}
//	if (round < log_r)
//		print_keccak("st", st, 25);
	keccakf(st, KECCAK_ROUNDS);
//	print_keccak("final states", st, 25);

	memcpy(md, (char*)st, mdlen);
}

void keccak1600(uint8_t* in, int inlen, uint8_t* md)
{
	keccak(in, inlen, md, sizeof(state_t));
}
