/**
 * block-chain implementation test:
 * compile: gcc -I/usr/include/pint test_functions.c pint_native.c
 * 
 */

#include "pintapi.h"
#include "pint_native.h"
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TEST_LEN        0x100000
#define TEST_CORE_NUM   64

int32_t test_len = 0x8000/TEST_CORE_NUM;
int* data_base = (int*)0x10000000; //256M

unsigned long get_timestamp(void)
{
    struct timespec host_spec;
    unsigned long time;
    clock_gettime(CLOCK_REALTIME, &host_spec);
    time = host_spec.tv_sec * 1e+9 + host_spec.tv_nsec;
    return time;
}



static int verify(int n, const volatile int* test, const int* verify)
{
    int i;
    // Unrolled for faster verification
    for (i = 0; i < n / 2 * 2; i += 2) {
        int t0 = test[i], t1 = test[i + 1];
        int v0 = verify[i], v1 = verify[i + 1];
        if (t0 != v0) {
            printf("error: %d \n", i + 1);
            return i + 1;
        }

        if (t1 != v1) {
            printf("error: %d\n", i + 2);
            return i + 2;
        }
            
    }
    if (n % 2 != 0 && test[n - 1] != verify[n - 1]) {
        printf("error: %d\n", n);
        return n;
    }
        
    return 0;
}

#define CHECK_COUNT 10
bool globalmap_check(uap_variable_t *data) {
    uap_variable_t data_[CHECK_COUNT];

    strcpy(data_[0].vname , "matB_y_stride");
    data_[0].address = 191324;
    data_[0].length = 4;
    strcpy(data_[1].vname , "N");
    data_[1].address = 191268;
    data_[1].length = 4;
    strcpy(data_[2].vname , "op");
    data_[2].address = 191276;
    data_[2].length = 4;
    strcpy(data_[3].vname , "nWidth");
    data_[3].address = 191284;
    data_[3].length = 4;
    strcpy(data_[4].vname , "offB");
    data_[4].address = 191240;
    data_[4].length = 4;
    strcpy(data_[5].vname , "K");
    data_[5].address = 191264;
    data_[5].length = 4;
    strcpy(data_[6].vname , "pA_SrcData");
    data_[6].address = 191308;
    data_[6].length = 4;
    strcpy(data_[7].vname , "offA");
    data_[7].address = 191244;
    data_[7].length = 4;
    strcpy(data_[8].vname , "ldb");
    data_[8].address = 191228;
    data_[8].length = 4;
    strcpy(data_[9].vname , "mode_lstm");
    data_[9].address = 191312;
    data_[9].length = 4;
    for(int i=0; i<10; i++) {
    	if ((data[i].address != data_[i].address)
			|| (data[i].length != data_[i].length)
			|| (strcmp(data[i].vname, data_[i].vname) != 0)) {
//    	if (data[i].address != data_[i].address)
    		printf("%d address is not the same %d, %d\n", i, data[i].address, data_[i].address);
//    	if (data[i].length != data_[i].length)
    		printf("%d length is not the same, %d, %d\n", i, data[i].length, data_[i].length);
//    	if (strcmp(data[i].vname, data_[i].vname) != 0) {
    		printf("%d name is not the same, %s, %s\n", i, data[i].vname, data_[i].vname);
    		return false;
		}
//		printf("Error: %d: ", i);
//		printf("name %s, address %d, length %d\n", data[i].vname , data[i].address , data[i].length);
    }
    return true;
}

static void insertion_sort(size_t n, int arr[])
{
    int *i, *j;
    int value;
    for (i = arr + 1; i < arr + n; i++) {
        value = *i;
        j = i;
        while (value < *(j - 1)) {
            *j = *(j - 1);
            if (--j == arr)
                break;
        }
        *j = value;
    }
}
void add_1(size_t n, int arr[])
{
    for(int i = 0; i < n; i++) {
        arr[i] = arr[i] + 1;
    }
}

void test_global_map_read() {
	char test_name[] = "global_map read";
    char map_path[] = "recomm_GlobalMap.txt";

	int var_count = get_variable_counts(map_path);
	if (var_count > 0) {
	    uap_variable_t * varlist;

//	    printf("size of %d\n", var_count * sizeof(uap_variable_t));
	    varlist = (uap_variable_t *) malloc(var_count * sizeof(uap_variable_t));
		read_map_file(map_path, varlist);

		if (globalmap_check(varlist))
			printf("============== \n%s succeeds!\n==============\n\n", test_name);
		else
			printf("============== \n%s fails!\n==============\n\n", test_name);

		free(varlist);
	} else
		printf("The 'globalMap.txt is empty\n");
}

#define HASH_LENGTH 32
bool hash_check(char * res) {
    char* b[] = {24,51,169 ,250, 124, 244,   8, 107, 213, 253, 167,  61, 163,  46,  90,  29, 117, 180, 195, 248, 157,  92,  67,  99, 105, 249, 215, 139, 178, 218,  92,  40};
//	pint_memcpy(res, res_addr, length, pint_copy_device2host);

    for (int i = 0; i < HASH_LENGTH; i++) {
      int a = res[i] & 0xff;
      if (b[i] != a) {
    	  printf("i %d, val %x, res %x, a %d\n", i, b[i], res[i], a);
    	  return false;
      }
    }

    return true;
}


void setup_hash_arg(uap_variable_t *varlist, int var_count, uint64_t* res_ptr, int* res_length) {
	char str[]="abc";
	int str_len = strlen(str);
	int  index[]={str_len};
	bool ret;

	uint64_t data_ptr = pint_malloc(str_len, PINT_ARG_GLOBAL_PTR);
	ret = pint_memcpy(str, data_ptr, str_len, pint_copy_host2device);

//	printf("data addr %d\n", data_ptr);

	int i = set_parameter(varlist, var_count, "p_SrcData", PINT_ARG_VALUE, data_ptr, 3*4);
//	ret = pint_memcpy(&data_ptr, varlist[i].address, 4, pint_copy_host2device);
//	if (ret != 0)
//		printf("set up p_SrcData fails\n");

	uint64_t index_ptr = pint_malloc(4, PINT_ARG_GLOBAL_PTR);
	pint_memcpy(index, index_ptr, 4, pint_copy_host2device);
	i = set_parameter(varlist, var_count, "p_Index", PINT_ARG_VALUE, index_ptr, 3*4);
//	ret = pint_memcpy(&index_ptr, varlist[i].address, 4, pint_copy_host2device);
//	if (ret != 0)
//		printf("set up index_ptr fails\n");

//	printf("index addr %d\n", index_ptr);

	*res_ptr = pint_malloc(HASH_LENGTH, PINT_ARG_GLOBAL_PTR);
	*res_length = HASH_LENGTH;
	set_parameter(varlist, var_count, "p_DstData", PINT_ARG_VALUE, *res_ptr, *res_length);
//	printf("results addr %d\n", res_ptr);

	set_parameter(varlist, var_count, "nBatch", PINT_ARG_VALUE, 1, 4);
	set_parameter(varlist, var_count, "op", PINT_ARG_VALUE, 3, 4);
}


void test_blake256() {
	char test_name[] = "blake256";
    char map_path[] = "recomm_GlobalMap.txt";
    char exe_file_b[] = "recomm.b";
    int code_addr = 0;

	int var_count = get_variable_counts(map_path);
	if (var_count > 0) {
	    uap_variable_t * varlist;
//	    printf("size of %d\n", var_count * sizeof(uap_variable_t));
	    varlist = (uap_variable_t *) malloc(var_count * sizeof(uap_variable_t));
		read_map_file(map_path, varlist);

		uint64_t res_ptr;
		int res_length = HASH_LENGTH+1;
		char res_host[HASH_LENGTH+1];

		//
		// set arguments for the app
		//
		pint_app_init();
	    if (pint_app_load(exe_file_b, code_addr)) {
			setup_hash_arg(varlist, var_count, &res_ptr, &res_length);
			//
			// execute the app
			//
	    	pint_app_exe(res_host, res_ptr, res_length, OP_KECCAK);
	    } else
	    	printf("Error: loading executables fails....\n");

//		pint_app_close();

		//
		// check the results
		//
		if (hash_check(res_host))
			printf("============== \n%s succeeds!\n==============\n\n", test_name);
		else
			printf("============== \n%s fails!\n==============\n\n", test_name);

		free(varlist);
	}
}

void blake256_test()
{
  char test_name[] = "blake256_sanity";
  int i, v;
  uint8_t in[72], out[32];
  uint8_t test1[] =
  {
    0x0c, 0xe8, 0xd4, 0xef, 0x4d, 0xd7, 0xcd, 0x8d,
    0x62, 0xdf, 0xde, 0xd9, 0xd4, 0xed, 0xb0, 0xa7,
    0x74, 0xae, 0x6a, 0x41, 0x92, 0x9a, 0x74, 0xda,
    0x23, 0x10, 0x9e, 0x8f, 0x11, 0x13, 0x9c, 0x87
  };
  uint8_t test2[] =
  {
    0xd4, 0x19, 0xba, 0xd3, 0x2d, 0x50, 0x4f, 0xb7,
    0xd4, 0x4d, 0x46, 0x0c, 0x42, 0xc5, 0x59, 0x3f,
    0xe5, 0x44, 0xfa, 0x4c, 0x13, 0x5d, 0xec, 0x31,
    0xe2, 0x1b, 0xd9, 0xab, 0xdc, 0xc2, 0x2d, 0x41
  };

  memset( in, 0, 72 );
  pint_hash256( out, in, 1 );
  v = 0;

  for( i = 0; i < 32; ++i )
  {
    if ( out[i] != test1[i] ) v = 1;
//    printf("out %02x, exp %02x\n", out[i], test1[i]);
  }

  if ( v ) {
	  printf("============== \n%s (test1) fails.\n==============\n", test_name);
  }
//  printf("\n   ===== input after test 1 ====\n");

//  for (i = 0; i < 72; ++i)
//	  printf( "%02x", in[i] );

  pint_hash256( out, in, 72);
  v = 0;

  for( i = 0; i < 32; ++i )
  {
    if ( out[i] != test2[i] ) v = 1;
  }

  if ( v )
//	  printf( "\ntest 2 error\n" );
	  printf("============== \n%s (test2) fails.\n==============\n", test_name);
  else
	  printf("============== \n%s succeeds.\n==============\n", test_name);
}

void blake_abc() {
	  char test_name[] = "blake_abc";
	  int i, v;
	  uint8_t in[] = "abc";
	  uint8_t out[32];
	  uint8_t test[] =
	  {
		0x18, 0x33, 0xa9, 0xfa, 0x7c, 0xf4, 0x08, 0x6b,
		0xd5, 0xfd, 0xa7, 0x3d, 0xa3, 0x2e, 0x5a, 0x1d,
		0x75, 0xb4, 0xc3, 0xf8, 0x9d, 0x5c, 0x43, 0x63,
		0x69, 0xf9, 0xd7, 0x8b, 0xb2, 0xda, 0x5c, 0x28
//		  24,51,169 ,250, 124, 244,   8, 107,
//		  213, 253, 167,  61, 163,  46,  90,
//		  29, 117, 180, 195, 248, 157,  92,
//		  67,  99, 105, 249, 215, 139, 178, 218,  92,  40
	  };

//	  uint8_t test1[] =
//	  {
//	    0x0c, 0xe8, 0xd4, 0xef, 0x4d, 0xd7, 0xcd, 0x8d,
//	    0x62, 0xdf, 0xde, 0xd9, 0xd4, 0xed, 0xb0, 0xa7,
//	    0x74, 0xae, 0x6a, 0x41, 0x92, 0x9a, 0x74, 0xda,
//	    0x23, 0x10, 0x9e, 0x8f, 0x11, 0x13, 0x9c, 0x87
//	  };

	  pint_hash256(out, in, 3);
	  v = 0;

	  for( i = 0; i < 32; ++i )
	  {
	    if ( out[i] != test[i] ) v = 1;
	  }
	if (!v)
		printf("============== \n%s succeeds!\n==============\n\n", test_name);
	else
		printf("============== \n%s fails!\n==============\n\n", test_name);
}

void groestl_abc() {
	  char test_name[] = "groestl_abc";
	  int i, v;
	  uint8_t in[] = "abc";
	  uint8_t out[32];
	  uint8_t test[] =
	  {
		0x46, 0x26, 0xa4, 0x59, 0x74, 0x93, 0xfe, 0x60,
		0x24, 0x2b, 0xda, 0xc7, 0x3b, 0xc7, 0x22, 0x2c,
		0x51, 0xfb, 0xb9, 0xfb, 0xf4, 0x65, 0x3f, 0x16,
		0x29, 0x31, 0x60, 0xb3, 0x83, 0xc7, 0xcf, 0x01
	  };

	  pint_groestl( in, 3, out);
	  v = 0;

	  for( i = 0; i < 32; ++i )
	  {
	    if ( out[i] != test[i] ) v = 1;
//	    printf("out: %02x, exp: %02x\n", out[i], test[i]);
	  }
	if (!v)
		printf("============== \n%s succeeds!\n==============\n\n", test_name);
	else
		printf("============== \n%s fails!\n==============\n\n", test_name);
}


void jh_abc() {
	  char test_name[] = "jh_abc";
	  int i, v;
	  uint8_t in[] = "abc";
	  uint8_t out[32];
	  uint8_t test[] =
	  {
		0xf1, 0xa4, 0x82, 0x99, 0xd7, 0x6f, 0xf8, 0xaa,
		0x19, 0x32, 0x4d, 0x6a, 0xfc, 0xcd, 0x2e, 0x54,
		0x8d, 0xb2, 0xa2, 0x5b, 0x9c, 0xab, 0xb1, 0x08,
		0x36, 0x71, 0xbb, 0x92, 0x06, 0xd5, 0x34, 0x1e
	  };

	  int out_len = 32;
	  pint_hash( in, 3, out, OP_JH, out_len);
	  v = 0;

	  for( i = 0; i < 32; ++i )
	  {
	    if ( out[i] != test[i] ) v = 1;
	    if (v)
	    	printf("out: %02x, exp: %02x\n", out[i], test[i]);
	  }
	if (!v)
		printf("============== \n%s succeeds!\n==============\n\n", test_name);
	else
		printf("============== \n%s fails!\n==============\n\n", test_name);
}


void keccak_abc() {
	  char test_name[] = "keccak_abc";
	  int i, v;
//	  uint8_t in[] = "abc";
	  uint8_t in[] = "This is a test This is a test This is a test";
	  uint8_t out[32];
	  uint8_t test[] =
	  {
	    0xaf, 0x6f, 0xe9, 0x6f, 0x8c, 0xb4, 0x09, 0xbd, 0xd2,
		0xa6, 0x1f, 0xb8, 0x37, 0xe3, 0x46, 0xf1, 0xa2, 0x80,
		0x07, 0xb0, 0xf0, 0x78, 0xa8, 0xd6, 0x8b, 0xc1, 0x22,
		0x4b, 0x6f, 0xcf, 0xcc, 0x3c
//		0x4e, 0x03, 0x65, 0x7a, 0xea, 0x45, 0xa9, 0x4f, 0xc7,
//		0xd4, 0x7b, 0xa8, 0x26, 0xc8, 0xd6, 0x67, 0xc0, 0xd1,
//		0xe6, 0xe3, 0x3a, 0x64, 0xa0, 0x36, 0xec, 0x44, 0xf5,
//		0x8f, 0xa1, 0x2d, 0x6c, 0x45
	  };

	  int out_len = 32;
	  pint_hash( in, 44, out, OP_KECCAK, out_len);
	  v = 0;

	  for( i = 0; i < 32; ++i )
	  {
	    if ( out[i] != test[i] ) v = 1;
	    if (v)
	    	printf("out: %02x, exp: %02x\n", out[i], test[i]);
	  }
	if (!v)
		printf("============== \n%s succeeds!\n==============\n\n", test_name);
	else
		printf("============== \n%s fails!\n==============\n\n", test_name);
}

void kecca_md_abc() {
	  char test_name[] = "kecca_md_abc";
	  int i, v;
//	  uint8_t in[] = "abc";
	  uint8_t in[] = "This is a test This is a test This is a test";
	  uint8_t out[200];
	  uint8_t test[] =
	  {
			  0xaf, 0x6f, 0xe9, 0x6f, 0x8c, 0xb4, 0x09, 0xbd,
			  0xd2, 0xa6, 0x1f, 0xb8, 0x37, 0xe3, 0x46, 0xf1,
			  0xa2, 0x80, 0x07, 0xb0, 0xf0, 0x78, 0xa8, 0xd6,
			  0x8b, 0xc1, 0x22, 0x4b, 0x6f, 0xcf, 0xcc, 0x3c,
			  0x39, 0xf1, 0x24, 0x4d, 0xb8, 0xc0, 0xaf, 0x06,
			  0xe9, 0x41, 0x73, 0xdb, 0x4a, 0x54, 0x03, 0x8a,
			  0x2f, 0x7a, 0x6a, 0x9c, 0x72, 0x99, 0x28, 0xb5,
			  0xec, 0x79, 0x66, 0x8a, 0x30, 0xcb, 0xf5, 0xf2,
			  0x66, 0x11, 0x06, 0x65, 0xe2, 0x3e, 0x89, 0x1e,
			  0xa4, 0xee, 0x23, 0x37, 0xfb, 0x30, 0x4b, 0x35,
			  0xbf, 0x8d, 0x9c, 0x2e, 0x4c, 0x35, 0x24, 0xe5,
			  0x2e, 0x62, 0xdb, 0x67, 0xb0, 0xb1, 0x70, 0x48,
			  0x7a, 0x68, 0xa3, 0x4f, 0x80, 0x26, 0xa8, 0x1b,
			  0x35, 0xdc, 0x83, 0x5c, 0x60, 0xb3, 0x56, 0xd2,
			  0xc4, 0x11, 0xad, 0x22, 0x7b, 0x6c, 0x67, 0xe3,
			  0x0e, 0x9b, 0x57, 0xba, 0x34, 0xb3, 0xcf, 0x27,
			  0xfc, 0xce, 0xca, 0xe9, 0x72, 0x85, 0x0c, 0xf3,
			  0x88, 0x9b, 0xb3, 0xff, 0x83, 0x47, 0xb5, 0x5a,
			  0x57, 0x10, 0xd5, 0x80, 0x86, 0x97, 0x3d, 0x12,
			  0xd7, 0x5a, 0x33, 0x40, 0xa3, 0x94, 0x30, 0xb6,
			  0x5e, 0xe2, 0xf4, 0xbe, 0x27, 0xc2, 0x1e, 0x7b,
			  0x39, 0xf4, 0x73, 0x41, 0xdd, 0x03, 0x6f, 0xe1,
			  0x3b, 0xf4, 0x3b, 0xb2, 0xc5, 0x5b, 0xce, 0x49,
			  0x8a, 0x3a, 0xdc, 0xbf, 0x07, 0x39, 0x7e, 0xa6,
			  0x60, 0x62, 0xb6, 0x6d, 0x56, 0xcd, 0x81, 0x36
	  };

	  int out_len = 200;
	  pint_hash( in, 44, out, OP_KECCAK, out_len);
	  v = 0;

	  for( i = 0; i < out_len; ++i )
	  {
	    if ( out[i] != test[i] ) v = 1;
	  }
	if (!v)
		printf("============== \n%s succeeds!\n==============\n\n", test_name);
	else {
		printf("============== \n%s fails!\n==============\n\n", test_name);
		  for (i = 0; i < out_len; i++) {
			  if (i % 8 == 0)
				  printf("\n");
			  printf("0x%02x, ", out[i]);
		  }
	}
}
void test_pointer() {
	char test_name[] = "test_pointer";
    char map_path[] = "test_uap_pointer_GlobalMap.txt";
    char exe_file_b[] = "test_uap_pointer.b";
    int code_addr = 0;
	uint64_t res_ptr;
	int res_length = HASH_LENGTH+1;
	char res_host[HASH_LENGTH+1];
	if (pint_app_load(exe_file_b, code_addr)) {
	    printf("\ntest pointer\n");
		pint_app_exe(res_host, res_ptr, res_length, OP_KECCAKF);
	}
}

void skein_abc() {
	  char test_name[] = "skein_abc";
	  int i, v;
	  uint8_t in[] = "abc";
	  uint8_t out[32];
	  uint8_t test[] =
	  {
		0x08, 0xdf, 0xa9, 0x30, 0x93, 0xdf, 0x2b, 0x5e,
		0xd8, 0xee, 0xa7, 0xa3, 0x2d, 0x27, 0x99, 0x60,
		0x5b, 0x39, 0xb6, 0x37, 0x0b, 0xb2, 0x4c, 0x9c,
		0xd8, 0x64, 0x7f, 0x89, 0xc2, 0xc9, 0x7a, 0xa3
	  };

	  int out_len = 32;
	  pint_hash( in, 3, out, OP_SKEIN, out_len);
	  v = 0;
	  v = 0;

	  for( i = 0; i < 32; ++i )
	  {
	    if ( out[i] != test[i] ) v = 1;
//	    printf("out: %02x, exp: %02x\n", out[i], test[i]);
	  }
	if (!v)
		printf("============== \n%s succeeds!\n==============\n\n", test_name);
	else
		printf("============== \n%s fails!\n==============\n\n", test_name);
}

void test_keccakf()
{
	uint8_t ini[] = {
		0x15, 0x37, 0x41, 0x93, 0x78, 0xd4, 0xb1, 0x47,
		0x87, 0xb0, 0x55, 0xe6, 0xf8, 0x77, 0x2c, 0x98,
		0x16, 0x4c, 0x2f, 0x50, 0x18, 0x6b, 0x44, 0x28,
		0xc4, 0xd0, 0x0e, 0x9b, 0x99, 0x69, 0x04, 0xe5,
		0x73, 0x74, 0x20, 0x62, 0x65, 0x20, 0x61, 0x6e,
		0x20, 0x69, 0x6e, 0x74, 0x65, 0x67, 0x65, 0x72,
		0x20, 0x6c, 0x61, 0x72, 0x67, 0x65, 0x72, 0x20,
		0x74, 0x68, 0x61, 0x6e, 0x20, 0x30, 0x2e, 0x0a,
		0x89, 0x1f, 0x4c, 0x33, 0x5f, 0xb9, 0x12, 0x74,
		0x25, 0x37, 0x41, 0x18, 0x06, 0x14, 0x9e, 0xfc,
		0x0d, 0xce, 0x15, 0x3d, 0xf8, 0x70, 0x9c, 0xd6,
		0xd4, 0x22, 0x37, 0x66, 0xb1, 0xc1, 0xc6, 0xfb,
		0x67, 0xa9, 0x24, 0x89, 0xce, 0x5e, 0x8f, 0xc5,
		0xa9, 0x89, 0x36, 0x3f, 0xb3, 0x57, 0x03, 0x83,
		0xd8, 0x3e, 0xd5, 0x36, 0xd6, 0x98, 0x88, 0x1a,
		0x13, 0xce, 0xf9, 0xca, 0x3d, 0x87, 0xc1, 0x03,
		0x35, 0x90, 0xa9, 0x9f, 0xc5, 0x18, 0xb5, 0xf7,
		0x7f, 0x88, 0xae, 0xcc, 0xcd, 0xde, 0xd9, 0x5f,
		0x80, 0x0b, 0x1a, 0xb9, 0xd2, 0x4c, 0xb7, 0x3f,
		0xa6, 0x1e, 0x8a, 0x7f, 0xd4, 0xe3, 0x48, 0x05,
		0xdc, 0x79, 0x5e, 0xaa, 0x49, 0xf9, 0x25, 0x40,
		0x9e, 0xf4, 0x29, 0x2b, 0xf8, 0x9b, 0x13, 0x9b,
		0xab, 0xac, 0x9b, 0xcb, 0x47, 0x87, 0x52, 0xd3,
		0x21, 0x79, 0xc2, 0x2e, 0xea, 0x0d, 0x35, 0x99,
		0x7e, 0xf6, 0x6f, 0xd3, 0x7e, 0x85, 0xb8, 0xb9
	};
	  uint8_t out[] =
	  {
			  0x05, 0x54, 0xe2, 0xa6, 0xd0, 0xff, 0xe2, 0xe3,
			  0xbf, 0x73, 0x75, 0xfc, 0xde, 0x93, 0x6d, 0x06,
			  0xd4, 0x1c, 0xe4, 0x59, 0x9d, 0x3b, 0x66, 0x7a,
			  0xdf, 0x57, 0xf2, 0x31, 0x51, 0xe4, 0x9c, 0xa8,
			  0x45, 0x9e, 0x89, 0xfa, 0x14, 0xce, 0xda, 0xfc,
			  0xe4, 0x71, 0x6a, 0x7c, 0x6d, 0xb1, 0xef, 0xf5,
			  0xb3, 0x70, 0x62, 0xd9, 0xbd, 0x91, 0xe4, 0xd4,
			  0xab, 0x38, 0xfd, 0xcb, 0xb7, 0x40, 0xe9, 0x4a,
			  0x79, 0xdb, 0xc4, 0x1c, 0xb2, 0xd7, 0x48, 0x09,
			  0xc9, 0xdb, 0x96, 0x22, 0x5d, 0x0d, 0x6c, 0x9f,
			  0x78, 0xd5, 0xa0, 0x23, 0x4f, 0xd1, 0x7e, 0xba,
			  0xeb, 0x22, 0xf9, 0x55, 0x0c, 0x3e, 0x43, 0xea,
			  0x99, 0x50, 0x78, 0x45, 0x15, 0x1e, 0x1a, 0x67,
			  0x9f, 0x7a, 0xbe, 0x42, 0x12, 0xc3, 0x21, 0x70,
			  0x6f, 0xf9, 0x44, 0xc6, 0xca, 0x77, 0xf8, 0x92,
			  0x58, 0xfc, 0x8b, 0x7b, 0x60, 0xc0, 0xe0, 0xf6,
			  0xf2, 0x6e, 0x79, 0xbd, 0x71, 0x5f, 0x3b, 0x5f,
			  0x03, 0x52, 0xac, 0x01, 0x79, 0xdf, 0xc6, 0xc3,
			  0x14, 0xab, 0x7d, 0xd4, 0xc7, 0xec, 0x71, 0x78,
			  0x3a, 0x80, 0x40, 0x89, 0x4d, 0x70, 0x45, 0x6d,
			  0xfc, 0xca, 0xe0, 0xb3, 0xe6, 0x6a, 0x63, 0x84,
			  0x9b, 0x2d, 0xdd, 0x0e, 0xed, 0x8b, 0x0d, 0xcb,
			  0x79, 0x14, 0xfd, 0x1c, 0x39, 0xc9, 0x61, 0x52,
			  0xdd, 0x7e, 0xea, 0xe8, 0x05, 0x96, 0xdf, 0xc5,
			  0x4f, 0x44, 0xd3, 0xdf, 0xd5, 0xfe, 0xb5, 0xd5
	  };
	  int i, v;
	  printf("\n================================\ninputs\n");
	  int str_l = 200;
//	  for( i = 0; i < str_l; ++i )
//	  {
//			if ((i % 8) == 0)
//				printf("\n");
////		  printf( "0x%02x, ", ini[i] );
//	  }
//	  printf("\n================================\noutputs\n");

	  pint_keccakf(ini, str_l, ini, 24);
	  v = 0;

	  for( i = 0; i < str_l; ++i )
	  {
		if ( ini[i] != out[i] ) {
			v = 1;
			printf( "0x%02x, ", ini[i] );
		}
	  }
	  printf("\n");
	  if ( v )
		  printf( "keccakf fails\n" );
	  else
		  printf("\n>>>>> keccakf passes <<<<<<\n");
}


int main(int argc, char** argv)
{
    int filesize = 0;

    char data_path[512] = "data.b";
    int fd_in, i, ret_num, binary_sz;
    char* binary = NULL;
    int test_result;
    int ret = 0;
//    int* test_buff = malloc(TEST_LEN* 4);
//    int* test_ret = malloc(TEST_LEN*4);
    int file_fd;
    int value = 0;
    int core_num = 0;
    
//    ctl_read_dword(0x08, &value);
//    core_num = ((value&0xff00)>>8) * ((value&0xff0000)>>16);
//    printf("v:%x core: %d  %d  %d\n", value, core_num, (value&0xff00)>>8, (value&0xff0000)>>16);
    srand((int)(get_timestamp()));
	test_global_map_read();
	blake256_test();
//	test_blake256();
	blake_abc();
	groestl_abc();
    jh_abc();
    keccak_abc();

    test_pointer();
    skein_abc();

    printf("--- keccakf test --- \n");
    test_keccakf();

    printf("--- kecca_md test --- \n");
    kecca_md_abc();

}

