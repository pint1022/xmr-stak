// keccak.h
// Nov-1-2019  Steven Wang <swang@panyi.ai>

#ifndef KECCAK_H
#define KECCAK_H

#include <stdint.h>
#include <string.h>
#include "pint_native_common.h"

#ifndef KECCAK_ROUNDS
#define KECCAK_ROUNDS 24
#endif

//void rot64(uint32_t hi, uint32_t lo, uint32_t n, uint32_t *hi_out, uint32_t *lo_out);
//#define ROTL64(x, y, z) (rot64((x)[0], (x)[1], (y), &((z)[0]), &((z)[1])))
//#define HI(x) ((x)[1])
//#define LO(x) ((x)[0])
//#define NOT64(x,y) (y[0] = ~(x)[0], y[1] = ~(x)[1])
//#define NOT64_1(x) ({~(x)[0],  ~(x)[1]})
//#define AND64(x,y, z) (z[0] = (x)[0] & (y)[0], z[1] = (x)[1] & (y)[1])
//#define AND64_1(x,y) ({(x)[0] & (y)[0], (x)[1] & (y)[1]})
//#define L64(x) (HI(x)<<32 | LO(x))
//#define XOR64(x, y, z) (z[0] = ((x)[0] ^ (y)[0]), z[1] = ((x)[1] ^ (y)[1]))
//#define XOR64_1(x, y) ({x[0] ^ y[0], x[1] ^ y[1]})
//#define ASSIGN64(x, y) (((x)[0] = (y)[0]), ((x)[1] = (y)[1]))

//#define ROTL64(x, y, z) (rot64(x[0], x[1], y, &(z[0]), &(z[1])))
//#define ASSIGN64(x, y) {(x[0] = y[0]); (x[1] = y[1])}
//#define XOR64(x, y) {(x[0] ^ y[0]), (x[1] ^ y[1])}
//#define HI(x) (x[0])
//#define LO(x) (x[1])
//#define NOT64(x) ({~x[0], ~x[1]})
//#define AND64(x,y) ({x[0] & y[0], x[1] & y[1]})
//#define L64(x) (((uint64_t)HI(x))<<32 | LO(x))

typedef uint32_t uint64b_t[2];
// compute a keccak hash (md) of given byte length from "in"
void keccak(uint8_t* in, int inlen, uint8_t* md, int mdlen);

// update the state
void keccakf(uint64b_t st[25], int norounds);

void keccak1600(uint8_t* in, int inlen, uint8_t* md);

#endif
