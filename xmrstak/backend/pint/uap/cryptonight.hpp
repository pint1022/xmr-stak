#pragma once

#include <stdint.h>
#include <string>

#include "../../cryptonight.hpp"
#include "pint_native.h"

#define CORE_NUM 2
#define HASH_STATE_LENGTH 224
#define HASH_LENGTH 32

#if defined _MSC_VER
#define ABI_ATTRIBUTE
#else
#define ABI_ATTRIBUTE __attribute__((ms_abi))
#endif


typedef struct
{
	uap_variable_t *var_list;
	int device_id;
	const char* device_name;
	char device_core[CORE_NUM];
	int syncMode;
	bool memMode;

	uint64_t d_input;        // the device memory address for the inputs of each cores. the offset of each core is calculated by core_id * workblob_size
	uint32_t inputlen;
	uint64_t  d_results_addr;     // the hash space on the device after one round of searching
	uint32_t outputlen;
	uint64_t d_long_state;   // the device memory address for the long state of each core: the offset of each core is calculated by core_id * size (CN_MEMORY)
	uint64_t d_ctx_state;    // the device memory address for  state of each core: the offset of each core is calculated by core_id * size (HASH_STATE_LENGTH?)
	uint64_t d_ctx_state2;   // not used
	uint64_t d_ctx_a;       // the device memory address for the context
//	uint64_t d_ctx_b;       // not used
	uint32_t d_varcount;    // not used


	xmrstak_algo cached_algo = {xmrstak_algo_id::invalid_algo};
	uint8_t d_results[HASH_LENGTH * CORE_NUM];     // the copy of all hashes from the device after one round of searching
} pint_ctx;


extern "C"
{

	/** get device count
 *
 * @param deviceCount[out] cuda device count
 * @return error code: 0 == error is occurred, 1 == no error
 */
//	int ppu_get_devicecount(int* deviceCount);
//	int ppu_get_deviceinfo(pint_ctx* ctx);
//	bool cryptonight_ppu_init(pint_ctx* ctx);
//	void cryptonight_ppu_set_data(pint_ctx* ctx, cryptonight_ctx* cpu_ctx, void* data, uint32_t len,  xmrstak_algo& miner_algo);
//	void cryptonight_ppu_hash(pint_ctx* ctx, xmrstak_algo& miner_algo);
//	bool cryptonight_ppu_exit(pint_ctx* ctx);
}

