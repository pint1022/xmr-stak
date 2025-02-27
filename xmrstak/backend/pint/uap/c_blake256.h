#ifndef _BLAKE256_H_
#define _BLAKE256_H_

typedef unsigned char uint8_t;
//typedef unsigned int[2] uint64_t;
typedef unsigned int uint32_t;

typedef struct
{
	unsigned int h[8], s[4], t[2];
	int buflen, nullt;
	unsigned char buf[64];
} state;

typedef struct
{
	state inner;
	state outer;
} hmac_state;


void blake256_init(state*);
void blake224_init(state*);

void blake256_update(state*, uint8_t*, uint32_t);
void blake224_update(state*, uint8_t*, uint32_t);

void blake256_final(state*, uint8_t*);
void blake224_final(state*, uint8_t*);

void blake256_hash(uint8_t*, uint8_t*, uint32_t);
void blake224_hash(uint8_t*, uint8_t*, uint32_t);

/* HMAC functions: */

//void hmac_blake256_init(hmac_state*, const uint8_t*, uint64_t);
//void hmac_blake224_init(hmac_state*, const uint8_t*, uint64_t);
//
//void hmac_blake256_update(hmac_state*, const uint8_t*, uint32_t);
//void hmac_blake224_update(hmac_state*, const uint8_t*, uint32_t);
//
//void hmac_blake256_final(hmac_state*, uint8_t*);
//void hmac_blake224_final(hmac_state*, uint8_t*);
//
//void hmac_blake256_hash(uint8_t*, const uint8_t*, uint64_t, const uint8_t*, uint32_t);
//void hmac_blake224_hash(uint8_t*, const uint8_t*, uint64_t, const uint8_t*, uint32_t);

#endif /* _BLAKE256_H_ */
