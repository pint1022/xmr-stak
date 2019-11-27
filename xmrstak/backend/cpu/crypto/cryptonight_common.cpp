/*
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
  *
  * Additional permission under GNU GPL version 3 section 7
  *
  * If you modify this Program, or any covered work, by linking or combining
  * it with OpenSSL (or a modified version of that library), containing parts
  * covered by the terms of OpenSSL License and SSLeay License, the licensors
  * of this Program grant you additional permission to convey the resulting work.
  *
  */

extern "C"
{
#include "c_blake256.h"
#include "c_groestl.h"
#include "c_jh.h"
#include "c_skein.h"
}
#include "cryptonight.h"
#include "cryptonight_aesni.h"
#include "xmrstak/backend/cryptonight.hpp"
#include "xmrstak/jconf.hpp"
#include "xmrstak/misc/console.hpp"
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>

#ifdef __GNUC__
#include <mm_malloc.h>
#else
#include <malloc.h>
#endif // __GNUC__

#if defined(__APPLE__)
#include <mach/vm_statistics.h>
#endif

#ifdef _WIN32
#include <windows.h>
// this comment avoid that clang format reorders the includes
#include <ntsecapi.h>
#else
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#endif // _WIN32

int s_ptr = 0, num_ptr = 0;
bool print_m128i( char * nm, __m128i var) {
	bool ret = true;
	uint16_t *val = (uint16_t*) &var;
	if (val[0] == 15249)
		ret = true;
	printf("%s: %i %i %i %i %i %i %i %i \n", nm,
		   val[0], val[1], val[2], val[3], val[4], val[5],
		   val[6], val[7]);
	return false;
}

void cpu_p_init(uint64_t monero_const, __m128i ax0, uint64_t idx0, __m128i bx0, __m128i bx1, char * l0, uint32_t * cn_r_data, __m128i division_result_xmm) {

	printf("%s: \n all parameters:\n", __FILE__);
	printf("mon_const %lx, idx0 %lx\n", monero_const, idx0 );
	print_m128i("ax0", ax0);
	print_m128i("bx0", bx0);
	print_m128i("bx1", bx1);
	printf("\n\n");
	for (int i = 0; i<9 ; i++) {
		printf("%d, ", cn_r_data[i]);
	}
	printf("\n");
	for (int i = 0; i<10 ; i++) {
		printf("%lx, ", l0[i]);
	}
	printf("\n");
}
void show_out(char* title, char* out, int count) {
    printf("\n==== %s =====\n", title);
    for( int j = 0; j < count; ++j ) {
    	if (j % 8 == 0)
    		printf("\n");
        printf( "0x%02x, ", out[j] & 0xFF);
    }
    printf("\n=============\n");
}

void do_blake_hash(const void* input, uint32_t len, char* output)
{
//	printf("%s, %d\n", __FILE__, __LINE__);
	blake256_hash((uint8_t*)output, (const uint8_t*)input, len);
}

void do_groestl_hash(const void* input, uint32_t len, char* output)
{
//	printf("%s, %d\n", __FILE__, __LINE__);
	groestl((const uint8_t*)input, len * 8, (uint8_t*)output);
}

void do_jh_hash(const void* input, uint32_t len, char* output)
{
//	printf("%s, %d\n", __FILE__, __LINE__);
	jh_hash(32 * 8, (const uint8_t*)input, 8 * len, (uint8_t*)output);
}

void do_skein_hash(const void* input, uint32_t len, char* output)
{
//	printf("%s, %d\n", __FILE__, __LINE__);
	skein_hash(8 * 32, (const uint8_t*)input, 8 * len, (uint8_t*)output);
}

void (*const extra_hashes[4])(const void*, uint32_t, char*) = {do_blake_hash, do_groestl_hash, do_jh_hash, do_skein_hash};

//inline void print_m128i( char * nm, __m128i var) {
//	uint16_t *val = (uint16_t*) &var;
//	printf("%s: %i %i %i %i %i %i %i %i \n", nm,
//		   val[0], val[1], val[2], val[3], val[4], val[5],
//		   val[6], val[7]);
//}

void ppu_p_init(uint64_t monero_const, __m128i ax0, uint64_t idx0, __m128i bx0, __m128i bx1, __m128 conc_var, char * l0, uint32_t * cn_r_data, __m128i division_result_xmm) {

	printf("%s: \n all parameters:\n", __FILE__);
	printf("mon_const %lx, idx0 %lx\n", monero_const, idx0 );
	print_m128i("ax0", ax0);
	print_m128i("bx0", bx0);
	print_m128i("bx1", bx1);
	printf("\n\n");
	for (int i = 0; i<9 ; i++) {
		printf("%d, ", cn_r_data[i]);
	}
	printf("\n");
	for (int i = 0; i<10 ; i++) {
		printf("%lx, ", l0[i]);
	}
	printf("\n");
}
void show_line(char * title, uint8_t * out, int s, int e) {
	int k = 71;
//	if (out[k] == 0x1e) return;
	printf("\n-----------%s------------\n", title);
    for( int j = s; j < e; ++j ) {
    	if ((j % 8) == 0)
    		printf("\n");
      printf( "0x%02x, ", out[j] & 0xFF);
    }

}

//void show_pint(char* title, char* out, int count) {
//    printf("\n====  %s =====\n", title);
//    for( int j = 0; j < count; ++j ) {
//    	if ((j % 8) == 0)
//    		printf("\n");
//      printf( "0x%02x, ", out[j] & 0xFF);
//    }
//    printf("\n=============\n");
//}

void pint_blake_hash(void* input, uint32_t len, char* output)
{
//	printf("%s %d: \n", __FILE__, __LINE__);
	pint_hash( ( char*)input, len, (char*)output, OP_BLAKE, 32);
}

void pint_groestl_hash(void* input, uint32_t len, char* output)
{
//	printf("%s %d: \n", __FILE__, __LINE__);
//	groestl(( uint8_t*)input, len * 8, (uint8_t*)output);
	pint_hash( ( char*)input, len * 8, (char*)output, OP_GROESTL, 32);
}

void pint_jh_hash(void* input, uint32_t len, char* output)
{
//	jh_hash(32 * 8, ( uint8_t*)input, 8 * len, (uint8_t*)output);
//	printf("%s %d: \n", __FILE__, __LINE__);

	pint_hash( (char*)input, len * 8, (char*)output, OP_JH, 32);

}

void pint_skein_hash(void* input, uint32_t len, char* output)
{
//	skein_hash(8 * 32, ( uint8_t*)input, 8 * len, (uint8_t*)output);
//	printf("%s %d: \n", __FILE__, __LINE__);
	pint_hash( (char*)input, len * 8, (char*)output, OP_SKEIN, 32);

}

void (*const pint_extra_hashes[4])(void*, uint32_t, char*) = {pint_blake_hash, pint_groestl_hash, pint_jh_hash, pint_skein_hash};


#ifdef _WIN32
#include "xmrstak/misc/uac.hpp"

BOOL bRebootDesirable = FALSE; //If VirtualAlloc fails, suggest a reboot

BOOL AddPrivilege(TCHAR* pszPrivilege)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	BOOL status;

	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return FALSE;

	if(!LookupPrivilegeValue(NULL, pszPrivilege, &tp.Privileges[0].Luid))
		return FALSE;

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	status = AdjustTokenPrivileges(hToken, FALSE, &tp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	if(!status || (GetLastError() != ERROR_SUCCESS))
		return FALSE;

	CloseHandle(hToken);
	return TRUE;
}

BOOL AddLargePageRights()
{
	HANDLE hToken;
	PTOKEN_USER user = NULL;

	if(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken) == TRUE)
	{
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		BOOL bIsElevated = FALSE;

		if(GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize))
			bIsElevated = Elevation.TokenIsElevated;

		DWORD size = 0;
		GetTokenInformation(hToken, TokenUser, NULL, 0, &size);

		if(size > 0 && bIsElevated)
		{
			user = (PTOKEN_USER)LocalAlloc(LPTR, size);
			GetTokenInformation(hToken, TokenUser, user, size, &size);
		}

		CloseHandle(hToken);
	}

	if(!user)
		return FALSE;

	LSA_HANDLE handle;
	LSA_OBJECT_ATTRIBUTES attributes;
	ZeroMemory(&attributes, sizeof(attributes));

	BOOL result = FALSE;
	if(LsaOpenPolicy(NULL, &attributes, POLICY_ALL_ACCESS, &handle) == 0)
	{
		LSA_UNICODE_STRING lockmem;
		lockmem.Buffer = L"SeLockMemoryPrivilege";
		lockmem.Length = 42;
		lockmem.MaximumLength = 44;

		PLSA_UNICODE_STRING rights = NULL;
		ULONG cnt = 0;
		BOOL bHasRights = FALSE;
		if(LsaEnumerateAccountRights(handle, user->User.Sid, &rights, &cnt) == 0)
		{
			for(size_t i = 0; i < cnt; i++)
			{
				if(rights[i].Length == lockmem.Length &&
					memcmp(rights[i].Buffer, lockmem.Buffer, 42) == 0)
				{
					bHasRights = TRUE;
					break;
				}
			}

			LsaFreeMemory(rights);
		}

		if(!bHasRights)
			result = LsaAddAccountRights(handle, user->User.Sid, &lockmem, 1) == 0;

		LsaClose(handle);
	}

	LocalFree(user);
	return result;
}
#endif

size_t cryptonight_init(size_t use_fast_mem, size_t use_mlock, alloc_msg* msg)
{
#ifdef _WIN32
	if(use_fast_mem == 0)
		return 1;

	if(AddPrivilege(TEXT("SeLockMemoryPrivilege")) == 0)
	{
		printer::inst()->print_msg(L0, "Elevating because we need to set up fast memory privileges.");
		RequestElevation();

		if(AddLargePageRights())
		{
			msg->warning = "Added SeLockMemoryPrivilege to the current account. You need to reboot for it to work";
			bRebootDesirable = TRUE;
		}
		else
			msg->warning = "Obtaining SeLockMemoryPrivilege failed.";

		return 0;
	}

	bRebootDesirable = TRUE;
	return 1;
#else
	return 1;
#endif // _WIN32
}

cryptonight_ctx* cryptonight_alloc_ctx(size_t use_fast_mem, size_t use_mlock, alloc_msg* msg)
{
	auto neededAlgorithms = ::jconf::inst()->GetCurrentCoinSelection().GetAllAlgorithms();

	size_t hashMemSize = 0;
	for(const auto algo : neededAlgorithms)
	{
		hashMemSize = std::max(hashMemSize, algo.Mem());
	}

	cryptonight_ctx* ptr = (cryptonight_ctx*)_mm_malloc(sizeof(cryptonight_ctx), 4096);

	if(use_fast_mem == 0)
	{
		// use 2MiB aligned memory
		ptr->long_state = (uint8_t*)_mm_malloc(hashMemSize, hashMemSize);
		ptr->ctx_info[0] = 0;
		ptr->ctx_info[1] = 0;
		if(ptr->long_state == NULL)
			printer::inst()->print_msg(L0, "MEMORY ALLOC FAILED: _mm_malloc was not able to allocate %s byte", std::to_string(hashMemSize).c_str());
		return ptr;
	}

#ifdef _WIN32
	SIZE_T iLargePageMin = GetLargePageMinimum();

	if(hashMemSize > iLargePageMin)
		iLargePageMin *= 2;

	ptr->long_state = (uint8_t*)VirtualAlloc(NULL, iLargePageMin,
		MEM_COMMIT | MEM_RESERVE | MEM_LARGE_PAGES, PAGE_READWRITE);

	if(ptr->long_state == NULL)
	{
		_mm_free(ptr);
		if(bRebootDesirable)
			msg->warning = "VirtualAlloc failed. Reboot might help.";
		else
			msg->warning = "VirtualAlloc failed.";
		return NULL;
	}
	else
	{
		ptr->ctx_info[0] = 1;
		return ptr;
	}
#else
//http://man7.org/linux/man-pages/man2/mmap.2.html
#if defined(__APPLE__)
	ptr->long_state = (uint8_t*)mmap(NULL, hashMemSize, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANON, VM_FLAGS_SUPERPAGE_SIZE_2MB, 0);
#elif defined(__FreeBSD__)
	ptr->long_state = (uint8_t*)mmap(NULL, hashMemSize, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS | MAP_ALIGNED_SUPER | MAP_PREFAULT_READ, -1, 0);
#elif defined(__OpenBSD__)
	ptr->long_state = (uint8_t*)mmap(NULL, hashMemSize, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANON, -1, 0);
#else
	ptr->long_state = (uint8_t*)mmap(NULL, hashMemSize, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB | MAP_POPULATE, -1, 0);
	if(ptr->long_state == MAP_FAILED)
	{
		// try without MAP_HUGETLB for crappy kernels
		msg->warning = "mmap with HUGETLB failed, attempting without it (you should fix your kernel)";
		ptr->long_state = (uint8_t*)mmap(NULL, hashMemSize, PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE, -1, 0);
	}
#endif

	if(ptr->long_state == MAP_FAILED)
	{
		_mm_free(ptr);
		msg->warning = "mmap failed, check attribute 'use_slow_memory' in 'config.txt'";
		return NULL;
	}

	ptr->ctx_info[0] = 1;

	if(madvise(ptr->long_state, hashMemSize, MADV_RANDOM | MADV_WILLNEED) != 0)
		msg->warning = "madvise failed";

	ptr->ctx_info[1] = 0;
	if(use_mlock != 0 && mlock(ptr->long_state, hashMemSize) != 0)
		msg->warning = "mlock failed";
	else
		ptr->ctx_info[1] = 1;

//	printf("\n >>>>>>>>>>> %s %d <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n", __FILE__, __LINE__);
	memset(ptr->hash_state, 0, 224);

	return ptr;
#endif // _WIN32
}

void cryptonight_free_ctx(cryptonight_ctx* ctx)
{
	auto neededAlgorithms = ::jconf::inst()->GetCurrentCoinSelection().GetAllAlgorithms();

	size_t hashMemSize = 0;
	for(const auto algo : neededAlgorithms)
	{
		hashMemSize = std::max(hashMemSize, algo.Mem());
	}

	if(ctx->ctx_info[0] != 0)
	{
#ifdef _WIN32
		VirtualFree(ctx->long_state, 0, MEM_RELEASE);
#else
		if(ctx->ctx_info[1] != 0)
			munlock(ctx->long_state, hashMemSize);
		munmap(ctx->long_state, hashMemSize);
#endif // _WIN32
	}
	else
		_mm_free(ctx->long_state);

	_mm_free(ctx);
}
/**
 * @ingroup pint-native-api
 * @function
 *    cryptonight_ppu_init
 *
 * @brief
 *  - This function initialize the PPU device for the cryptonight hashing. set up the following data:
 *      ctx space
 *      state space
 *      long state space
 *      results space
 *
 * @parameters
 *  - pint_ctx * : the context of the operation
* @return
 */

bool cryptonight_ppu_init(pint_ctx* ctx) {
	bool ret;
    char map_path[] = "/home/swang/src/xmr_pint/pint_hash256_GlobalMap.txt";
    char exe_file_b[] = "/home/swang/src/xmr_pint/pint_hash256.b";
    int code_addr = 0;
//
//    int var_length = 16;

	int var_count = get_variable_counts(map_path);

	if (var_count > 0) {
	    uap_variable_t * varlist;
	    varlist = (uap_variable_t *) malloc(var_count * sizeof(uap_variable_t));
		read_map_file(map_path, varlist);

//		uint64_t res_ptr, data_ptr, indx_ptr;
		int res_length; //25 states, uint64_t type

		//
		// set arguments for the app
		//
	    if (pint_app_load(exe_file_b, code_addr)) {
//	    	int data_len  = sizeof(pint_ctx) + (CN_MEMORY + HASH_STATE_LENGTH + HASH_LENGTH + sizeof(cryptonight_ctx)) * CORE_NUM;  //inputs, state, long state
	    	int data_len  = sizeof(pint_ctx) + (CN_MEMORY + HASH_LENGTH + sizeof(cryptonight_ctx)) * CORE_NUM;  //inputs, state, long state
	    	ctx->d_ctx_a = pint_malloc(data_len , PINT_ARG_GLOBAL_PTR);
	    	ctx->d_ctx_b = ctx->d_ctx_a + sizeof(pint_ctx);

//	    	ctx->d_ctx_state = ctx->d_ctx_b + sizeof(cryptonight_ctx) * CORE_NUM;
//	    	ctx->d_long_state = ctx->d_ctx_state + HASH_STATE_LENGTH  * CORE_NUM;
	    	ctx->d_long_state = ctx->d_ctx_b + sizeof(cryptonight_ctx) * CORE_NUM;
	    	ctx->outputlen = HASH_LENGTH  * CORE_NUM;
	    	ctx->d_results_addr = ctx->d_long_state + CN_MEMORY  * CORE_NUM;  //results?

	    	set_parameter(varlist, var_count, "p_CtxData", PINT_ARG_VALUE, ctx->d_ctx_a, 4);
	    	set_parameter(varlist, var_count, "p_SrcData", PINT_ARG_VALUE, ctx->d_ctx_state, 4);
	    	set_parameter(varlist, var_count, "p_DstData", PINT_ARG_VALUE, ctx->d_result_nonce, 4);
	    //	printf("results addr %d\n", res_ptr);

	    	set_parameter(varlist, var_count, "nBatch", PINT_ARG_VALUE, CORE_NUM, 4);
	    	set_parameter(varlist, var_count, "op", PINT_ARG_VALUE, OP_CRYPTONIGHT, 4);

	    	ctx->var_list = varlist;
	    } else
	    {
	    	printf("Error: loading executables fails....\n");
	    	free(varlist);
	    	return false;
	    }
	}
	return true;
}

/**
 * @ingroup pint-native-api
 * @function
 *    cryptonight_ppu_set_data
 *
 * @brief
 *  - This function transfers the cryptonight workblob data to the device for the next round of hashing.
 *
 * @parameters
 *  - pint_ctx * : the context of the operation
 *  - uint8_t * : the blob of data.
 *  - int : the size of blob.
* @return
 */
void cryptonight_ppu_set_data(pint_ctx* ctx, cryptonight_ctx* cpu_ctx, void* data, uint32_t len,  xmrstak_algo& miner_algo) {
	if (ctx->var_list == nullptr)
		return;
	int i, ret, curr_addr;
	ctx->d_input = pint_malloc(len, PINT_ARG_GLOBAL_PTR);
	ctx->inputlen = len;
	ctx->cached_algo = miner_algo;

	curr_addr = ctx->d_input;

	for (i = 0; i < CORE_NUM; i++) {
		ret = pint_memcpy(curr_addr, data, ctx->inputlen, pint_copy_host2device);
		if (ret != ctx->inputlen)
			return;
		curr_addr += ctx->inputlen;
	}
	ret = pint_memcpy(ctx->d_ctx_a, ctx, sizeof(pint_ctx), pint_copy_host2device);

	curr_addr = ctx->d_ctx_b;

	int cpu_ctx_size = sizeof(cryptonight_ctx);
	for (i = 0; i < CORE_NUM; i++) {
		ret = pint_memcpy(curr_addr, cpu_ctx, cpu_ctx_size, pint_copy_host2device);
		curr_addr += cpu_ctx_size;
	}
}

void cryptonight_ppu_hash(pint_ctx* ctx, xmrstak_algo& miner_algo)
{

//	typedef void (*ppu_hash_fn)(nvid_ctx * ctx);

	if(miner_algo == invalid_algo)
		return;

//	static const ppu_hash_fn func_table[] = {
//		cryptonight_core_gpu_hash<cryptonight, 0>,
//		cryptonight_core_gpu_hash<cryptonight, 1>,
//
//		cryptonight_core_gpu_hash<cryptonight_lite, 0>,
//		cryptonight_core_gpu_hash<cryptonight_lite, 1>,
//
//		cryptonight_core_gpu_hash<cryptonight_monero, 0>,
//		cryptonight_core_gpu_hash<cryptonight_monero, 1>,
//
//		cryptonight_core_gpu_hash<cryptonight_heavy, 0>,
//		cryptonight_core_gpu_hash<cryptonight_heavy, 1>,
//
//		cryptonight_core_gpu_hash<cryptonight_aeon, 0>,
//		cryptonight_core_gpu_hash<cryptonight_aeon, 1>,
//
//		cryptonight_core_gpu_hash<cryptonight_ipbc, 0>,
//		cryptonight_core_gpu_hash<cryptonight_ipbc, 1>,
//
//		cryptonight_core_gpu_hash<cryptonight_stellite, 0>,
//		cryptonight_core_gpu_hash<cryptonight_stellite, 1>,
//
//		cryptonight_core_gpu_hash<cryptonight_masari, 0>,
//		cryptonight_core_gpu_hash<cryptonight_masari, 1>,
//
//		cryptonight_core_gpu_hash<cryptonight_haven, 0>,
//		cryptonight_core_gpu_hash<cryptonight_haven, 1>,
//
//		cryptonight_core_gpu_hash<cryptonight_bittube2, 0>,
//		cryptonight_core_gpu_hash<cryptonight_bittube2, 1>,
//
//		cryptonight_core_gpu_hash<cryptonight_monero_v8, 0>,
//		cryptonight_core_gpu_hash<cryptonight_monero_v8, 1>,
//
//		cryptonight_core_gpu_hash<cryptonight_superfast, 0>,
//		cryptonight_core_gpu_hash<cryptonight_superfast, 1>,
//
//		cryptonight_core_gpu_hash_gpu<cryptonight_gpu, 0>,
//		cryptonight_core_gpu_hash_gpu<cryptonight_gpu, 1>,
//
//		cryptonight_core_gpu_hash<cryptonight_conceal, 0>,
//		cryptonight_core_gpu_hash<cryptonight_conceal, 1>,
//
//		cryptonight_core_gpu_hash<cryptonight_r_wow, 0>,
//		cryptonight_core_gpu_hash<cryptonight_r_wow, 1>,
//
//		cryptonight_core_gpu_hash<cryptonight_r, 0>,
//		cryptonight_core_gpu_hash<cryptonight_r, 1>,
//
//		cryptonight_core_gpu_hash<cryptonight_v8_reversewaltz, 0>,
//		cryptonight_core_gpu_hash<cryptonight_v8_reversewaltz, 1>};
//
//	std::bitset<1> digit;
//	digit.set(0, ctx->memMode == 1);

	int op = OP_CRYPTONIGHT;
   	pint_app_exe(ctx->d_results, ctx->d_results_addr, ctx->outputlen, op);

}

/**
 * @ingroup pint-native-api
 * @function
 *    cryptonight_ppu_init
 *
 * @brief
 *  - This function frees all memories used by the PPU device and cpu for the cryptonight hashing
 *      ctx space
 *
 * @parameters
 *  - pint_ctx * : the context of the operation
* @return
 */

bool cryptonight_ppu_exit(pint_ctx* ctx) {
	bool ret;
	pint_free(ctx->d_results_addr);
	free(ctx->var_list);
	return true;
}

