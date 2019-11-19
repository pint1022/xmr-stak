/**
 * @defgroup pint-native
 * @ingroup pint-native-api
 * Implementation of native support API.
 */

/**
 * PINT native api
 *
 * @file    pint_native.cpp
 * @ingroup pint-native-api
 * @brief   api to develop applications on uap call directly
 * @author  steven wang
 * @email   yning@panyi.ai
 * @version 0.0.1
 *
 */

#include <pintapi.h>
//#include <pintdev.h>
#include "pint_native.h"
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void print_out(char* title, char* out, int count) {
    printf("\n==== %s =====\n", title);
    for( int j = 0; j < count; ++j )
      printf( "%02x ", out[j] & 0xFF);
    printf("\n=============\n");
}

/**
 * @ingroup pint-native-api
 * @function
 *    trim
 *
 * @brief
 *  - This function remove leading and trailing spaces in the string.
 *
 * @parameters
 *  - char* str: the input string
 * @return
 */
void trim(char * str)
{
    int index, i;

    /*
     * Trim leading white spaces
     */
    index = 0;
    while(str[index] == ' ' || str[index] == '\t' || str[index] == '\n')
    {
        index++;
    }

    /* Shift all trailing characters to its left */
    i = 0;
    while(str[i + index] != '\0')
    {
        str[i] = str[i + index];
        i++;
    }
    str[i] = '\0'; // Terminate string with NULL


    /*
     * Trim trailing white spaces
     */
    i = 0;
    index = -1;
    while(str[i] != '\0')
    {
        if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
        {
            index = i;
        }

        i++;
    }

    /* Mark the next character to last non white space character as NULL */
    str[index + 1] = '\0';
}


/**
 * @ingroup pint-native-api
 * @function
 *    set_parameter
 *
 * @brief
 *  - This function sets up one parameter of the function/kernel on the device before the function runs on the device.
 *    If the parameters are pointer-type, it is assumed to allocate the memory in this call. If the parameter is a pointer,
 *    however the memories have been allocated for this parameter before calling this function, this function is used to
 *    pass the memory address to the parameter, then param_type is set to 0 to avoid allocating memory again in the call.
 *
 * @parameters
 *  - uap_variable_t *varlist: list of all parameters from GlobalMap.txt
 *  - int var_count: the number of parameters in the varlist
 *  - char * param_name: the parameter name to be processed
 *  - int param_type: the type of parameter.(PINT_ARG_VALUE : non-pointer; PINT_ARG_GLOBAL_PTR: pointer, need allocate space)
 *  - int param_value: the value to put into the parameter. if it is a pointer-type, the address is passed into the parameter.
 *  - int length: the length of spaces required if it is a pointer parameter.
 * @return
 *  - >=0: the index of the parameter to be set.
 *  - -1: can not find the parameter in the varlist.
 */
int set_parameter(uap_variable_t *varlist, int var_count, char * param_name, int param_type, uint64_t param_value, int length )
{
    int i;
    void * mem;
    int ret = -1;

    for (i = 0; i < var_count; i++){
//    	printf("param %s\n", varlist[i].vname);

    	if (strcmp(varlist[i].vname, param_name) == 0) {
			if (PINT_ARG_GLOBAL_PTR == param_type){
				mem = (void*)pint_malloc(length, param_type);
				ret = pint_memcpy(mem, varlist[i].address, length, pint_copy_host2device);
				printf("%s, %d: %s addr %d, value %l, length %d\n", __FILE__, __LINE__, param_name, varlist[i].address,  mem, length);
//				*(unsigned int *)varlist[i].address = mem;
			}else {
//				*(unsigned int *)varlist[i].address = param_value;
				ret = pint_memcpy(&param_value, varlist[i].address, 4, pint_copy_host2device);
//				printf("Global Var: %s, addr %d, value %d\n",  __FILE__, __LINE__,param_name, varlist[i].address, param_value);
			}
			ret = i;
			break;
    	}
    }

    return ret;
}

/**
 * @ingroup pint-native-api
 * @function
 *    get_field
 *
 * @brief
 *  - This function reads one column of csv file, and return the string.
 *
 * @parameters
 *  - char* row: which line of csv is read from
 *  - char* col: which colomn of the line of csv is read from
 * @return
 *  - char*: the string at the location of defined by row x col
 */
const char* get_field(char* line, int num)
{

}

/**
 * @ingroup pint-native-api
 * @function
 *    get_variable_counts
 *
 * @brief
 *  - This function counts variable number in globalmap.txt.
 *
 * @parameters
 *  - char* mapfile: globalmap.txt path
 * @return
 *  - int: the variable count in the globalmap.txt
 */
int get_variable_counts(char* mapfile)
{
	FILE *fp = fopen(mapfile,"r");
	int ch=0;
	int count=0;

	if (fp == NULL)
		return 0;
	for (char c = getc(fp); c != EOF; c = getc(fp))
		if (c == '\n') // Increment count if this character is newline
			count = count + 1;
	fclose(fp);
//	printf("file name %s, %d\n", mapfile, count);
	return count;
}

/**
 * @ingroup pint-native-api
 * @function
 *    read_map_file
 *
 * @brief
 *  - This function reads globalMap.txt; identify variable name, address, and length.
 *
 * @parameters
 *  - char* mapfilename: the path to globalMap.txt
 *  - char* v_name: the variable name
 *  - char* v_name: the variable name
 * @return
 *  - char*: the string at the location of defined by row x col
 */
#define LINE_MAX 256
void read_map_file(char *mapfile, uap_variable_t * var_list) {
	FILE* stream = fopen(mapfile, "r");

	char line[LINE_MAX];
	int idx = 0;

	while (fgets(line, LINE_MAX, stream))
	{
		char* tmp = strdup(line);

		// need to check the return values; right now, assume the globalmap.txt
		// has no empty line.
		// the lime has 3 items (var, address, length)
		char* token = strtok(line, ",");

		strcpy(var_list[idx].vname, token);
		trim(var_list[idx].vname);
//		printf("variable name %s\n", var_list[idx].vname);
		token = strtok(NULL, ",");
		var_list[idx].address = atoi(token);
		token = strtok(NULL, ",");
		var_list[idx].length = atoi(token);
//			printf("   address %d, length %d\n", var_list[idx].address , var_list[idx].length);
		// NOTE strtok clobbers tmp
		free(tmp);
		idx++;
	}
	fclose(stream);
}

/**
 * @ingroup pint-native-api
 * @function
 *    pint_app_init
 *
 * @brief
 *  - This function load executable b file and data b file to the device.
 *  - It executes the application, and read the results back to the buffer.
 *
 * @parameters
 * @return
 *  - true: the device is retrieved correctly
 *  - false: the device is not available for over 5-min
 */
bool pint_app_init() {
//	if (pint_device_is_alive()) {
//		pintInitDevice();
//	}
//	else
//		return false;
}

/**
 * @ingroup pint-native-api
 * @function
 *    pint_app_close
 *
 * @brief
 *  - This function closes the device, release it for other users.
 *
 * @parameters
 * @return
 */
void pint_app_close() {
//    pintCloseDevice();
}


/**
 * @ingroup pint-native-api
 * @function
 *    pint_app_exe_with_data
 *
 * @brief
 *  - This function load  data b file to the device.
 *  - It executes the application, and read the results back to the buffer.
 *
 * @parameters
 *  - data_file_b
 *       type: char*
 *       description: the data b file location
 *  - data_addr
 *       type: int
 *       description: the starting address to load the data into.
 *  - result_host
 *       type: char*
 *       description: the memory address of the host to save the results from the device
 *  - result_dev
 *       type: int
 *       description: the starting address of the result on the executable.
 *  - result_length
 *       type: int
 *       description: the  result length in bytes.
 * @return
 *  - True: if execution is completed.
 *  - False: if there is an error in the execution
 */
bool pint_app_exe_with_data(char* data_file_b,  int data_addr, char* result_host, uint64_t result_dev, int result_length) {
    int fd_in, i, ret_num, binary_sz;
    char* binary = NULL;
    char exe_path[512] = "recomm.b";
    char data_path[512] = "data.b";
//    FILE * file_fd;
    int file_fd;

//    if (!pint_app_init())
//    	return false;

    //
    // data file extension is '.b'
	if((file_fd = open(data_file_b, O_RDONLY)) <0){
		printf("The data file %s was not opened\n","test_data.data");
		return -1;
	}

    binary_sz = lseek(file_fd, 0, SEEK_END);
    lseek(file_fd, 0, SEEK_SET);
    binary = (char*)malloc(binary_sz);
    read(file_fd, binary, binary_sz);
    close(file_fd);

    pint_memcpy(binary, data_addr, binary_sz, pint_copy_host2device);

    int ret = pint_execute(0);
    printf("device test completed\n");
    pint_memcpy(result_host, result_dev, result_length, pint_copy_device2host);
//    pintCloseDevice();
    return true;
}


/**
 * @ingroup pint-native-api
 * @function
 *    pint_app_exe
 *
 * @brief
 *  - This function executes the application, and read the results back to the buffer.
 *
 * @parameters
 *  - result_host
 *       type: char*
 *       description: the memory address of the host to save the results from the device
 *  - result_dev
 *       type: int
 *       description: the starting address of the result on the executable.
 *  - result_length
 *       type: int
 *       description: the  result length in bytes.
 * @return
 *  - True: if execution is completed.
 *  - False: if there is an error in the execution
 */
bool pint_app_exe(char* result_host, uint64_t result_dev, int result_length, int op) {
    int ret = pint_execute(0);
//    if (op != OP_KECCAKF)
    pint_memcpy(result_host, result_dev, result_length, pint_copy_device2host);
    return true;
}

/**
 * @ingroup pint-native-api
 * @function
 *    pint_app_exe
 *
 * @brief
 *  - This function load executable b file and data b file to the device.
 *  - It executes the application, and read the results back to the buffer.
 *
 * @parameters
 *  - exe_file_b
 *       type: char*
 *       description: the executable b file location
 *  - code_addr
 *       type: int
 *       description: the starting address of the code to load the executable.
 * @return
 *  - True: if app code is loaded successfully.
 *  - False: if there is an error in loading
 */
bool pint_app_load(char* exe_file_b, int code_addr) {
    int fd_in, i, ret_num, binary_sz;
    char* binary = NULL;
//    char exe_path[512] = "recomm.b";
//    char data_path[512] = "data.b";
    FILE * file_fd;

    //
    // code file extension is '.b'
    if ((fd_in = open(exe_file_b, O_RDONLY)) < 0) {
        printf("The binary file %s was not opened\n", exe_file_b);
        return false;
    }
    binary_sz = lseek(fd_in, 0, SEEK_END);
    lseek(fd_in, 0, SEEK_SET);
    binary = (char*)malloc(binary_sz);
    read(fd_in, binary, binary_sz);
    close(fd_in);

    pint_memcpy(binary, code_addr, binary_sz, pint_copy_host2device);
    free(binary);

    return true;
}

#define HASH_LENGTH 32
//bool hash_check(char * res) {
//    char* b[] = {24,51,169 ,250, 124, 244,   8, 107, 213, 253, 167,  61, 163,  46,  90,  29, 117, 180, 195, 248, 157,  92,  67,  99, 105, 249, 215, 139, 178, 218,  92,  40};
////	pint_memcpy(res, res_addr, length, pint_copy_device2host);
//
//    for (int i = 0; i < HASH_LENGTH; i++) {
//      int a = res[i] & 0xff;
//      if (b[i] != a) {
//    	  printf("i %d, val %x, res %x, a %d\n", i, b[i], res[i], a);
//    	  return false;
//      }
//    }
//
//    return true;
//}


void set_hash_arg(uap_variable_t *varlist, int var_count, char *input, int str_len,  uint64_t* data_ptr, uint64_t* indx_ptr, uint64_t* res_ptr, int* res_length) {
	int  index[2];
//	char str[]="123456789012345678901234567890123456789012345678901234567890123456789012";
	bool ret;

//	if (str_len<4)
//		str_len=4;
	index[0] = str_len;


	*data_ptr = pint_malloc(str_len, PINT_ARG_GLOBAL_PTR);
//	printf("\ncopy %d to device addr %d\n", str_len, data_ptr);
	ret = pint_memcpy(input, *data_ptr, str_len, pint_copy_host2device);


	int i = set_parameter(varlist, var_count, "p_SrcData", PINT_ARG_VALUE, *data_ptr, 4);
//	ret = pint_memcpy(&data_ptr, varlist[i].address, str_len, pint_copy_host2device);
//	if (ret != 0)
//		printf("set up p_SrcData fails\n");

	*indx_ptr = pint_malloc(4, PINT_ARG_GLOBAL_PTR);
	pint_memcpy(index, *indx_ptr, 4, pint_copy_host2device);
	i = set_parameter(varlist, var_count, "p_Index", PINT_ARG_VALUE, *indx_ptr, 4);
//	ret = pint_memcpy(&index_ptr, varlist[i].address, 4, pint_copy_host2device);
//	if (ret != 0)
//		printf("set up index_ptr fails\n");

	*res_ptr = pint_malloc(HASH_LENGTH, PINT_ARG_GLOBAL_PTR);
	*res_length = HASH_LENGTH;
	set_parameter(varlist, var_count, "p_DstData", PINT_ARG_VALUE, *res_ptr, *res_length);
//	printf("results addr %d\n", res_ptr);

	set_parameter(varlist, var_count, "nBatch", PINT_ARG_VALUE, 1, 4);
	set_parameter(varlist, var_count, "op", PINT_ARG_VALUE, 3, 4);
}


void pint_hash256(char *outp, char *inp, int length) {
    char map_path[] = "pint_hash256_GlobalMap.txt";
    char exe_file_b[] = "pint_hash256.b";
    int code_addr = 0;
    int op = OP_BLAKE;

	int var_count = get_variable_counts(map_path);
	if (var_count > 0) {
	    uap_variable_t * varlist;
//	    printf("\nsize of %d, len %d\n", var_count * sizeof(uap_variable_t), length);
	    varlist = (uap_variable_t *) malloc(var_count * sizeof(uap_variable_t));
		read_map_file(map_path, varlist);

		uint64_t data_ptr, indx_ptr, res_ptr;
		int res_length = HASH_LENGTH+1;
//		char res_host[HASH_LENGTH+1];
		//
		// set arguments for the app
		//
		pint_app_init();
	    if (pint_app_load(exe_file_b, code_addr)) {
//	    	printf("%s, %d: length %d\n", __FILE__, __LINE__, length);
			set_hash_arg(varlist, var_count, inp, length,&data_ptr, &indx_ptr, &res_ptr, &res_length);
			//
			// execute the app
			//
	    	pint_app_exe(outp, res_ptr, res_length, op);
// free allocated memory;
	    	//FIXME
	    	pint_free(res_ptr);
	    	pint_free(data_ptr);
	    	pint_free(indx_ptr);
	    } else
	    	printf("Error: loading executables fails....\n");
//		pint_app_close();

		free(varlist);
	}
}

void set_groestl_arg(uap_variable_t *varlist, int var_count, char *input, int str_len,  uint64_t* res_ptr, int* res_length) {
	int  index[1];
//	char str[]="123456789012345678901234567890123456789012345678901234567890123456789012";
	bool ret;

//	if (str_len<4)
//		str_len=4;
	index[0] = str_len;


	uint64_t data_ptr = pint_malloc(str_len, PINT_ARG_GLOBAL_PTR);
//	printf("\ncopy %d to device addr %d\n", str_len, data_ptr);
	ret = pint_memcpy(input, data_ptr, str_len, pint_copy_host2device);


	int i = set_parameter(varlist, var_count, "p_SrcData", PINT_ARG_VALUE, data_ptr, 4);
//	ret = pint_memcpy(&data_ptr, varlist[i].address, str_len, pint_copy_host2device);
//	if (ret != 0)
//		printf("set up p_SrcData fails\n");

	uint64_t index_ptr = pint_malloc(4, PINT_ARG_GLOBAL_PTR);
	pint_memcpy(index, index_ptr, 4, pint_copy_host2device);
	i = set_parameter(varlist, var_count, "p_Index", PINT_ARG_VALUE, index_ptr, 4);
//	ret = pint_memcpy(&index_ptr, varlist[i].address, 4, pint_copy_host2device);
//	if (ret != 0)
//		printf("set up index_ptr fails\n");

	*res_ptr = pint_malloc(HASH_LENGTH, PINT_ARG_GLOBAL_PTR);
	*res_length = HASH_LENGTH;
	set_parameter(varlist, var_count, "p_DstData", PINT_ARG_VALUE, *res_ptr, *res_length);
//	printf("results addr %d\n", res_ptr);

	set_parameter(varlist, var_count, "nBatch", PINT_ARG_VALUE, 1, 4);
	set_parameter(varlist, var_count, "op", PINT_ARG_VALUE, 4, 4);
}

void pint_groestl(char *inp, int length, char *outp) {
    char map_path[] = "pint_hash256_GlobalMap.txt";
    char exe_file_b[] = "pint_hash256.b";
    int code_addr = 0;
    int op = OP_GROESTL;

	int var_count = get_variable_counts(map_path);
	if (var_count > 0) {
	    uap_variable_t * varlist;
//	    printf("\nsize of %d, len %d\n", var_count * sizeof(uap_variable_t), length);
	    varlist = (uap_variable_t *) malloc(var_count * sizeof(uap_variable_t));
		read_map_file(map_path, varlist);

		uint64_t res_ptr;
		int res_length = HASH_LENGTH+1;
//		char res_host[HASH_LENGTH+1];
		//
		// set arguments for the app
		//
		pint_app_init();
	    if (pint_app_load(exe_file_b, code_addr)) {
//	    	printf("%s, %d: length %d\n", __FILE__, __LINE__, length);
			set_groestl_arg(varlist, var_count, inp, length, &res_ptr, &res_length);
			//
			// execute the app
			//
	    	pint_app_exe(outp, res_ptr, res_length, op);
	    } else
	    	printf("Error: loading executables fails....\n");
//		pint_app_close();

		free(varlist);
	}
}

//
// Jh hash: JH is a cryptographic hash function submitted to the NIST hash function competition by Hongjun Wu.
//
void hash_arg(uap_variable_t *varlist, int var_count, char *input, int str_len, uint64_t *data_ptr, uint64_t *index_ptr, uint64_t* res_ptr, int* res_length, int op) {
	int  index[2];
//	char str[]="123456789012345678901234567890123456789012345678901234567890123456789012";
	bool ret;

	if (op == OP_KECCAKF){
		*res_length = 200;
		*data_ptr = pint_malloc(*res_length, PINT_ARG_GLOBAL_PTR);
		ret = pint_memcpy(input, *data_ptr, *res_length, pint_copy_host2device);
		*res_ptr = *data_ptr;
	}
	else {
		*data_ptr = pint_malloc(str_len, PINT_ARG_GLOBAL_PTR);
		ret = pint_memcpy(input, *data_ptr, str_len, pint_copy_host2device);
		*res_ptr = pint_malloc(HASH_LENGTH, PINT_ARG_GLOBAL_PTR);
//		*res_length = HASH_LENGTH;
	}
	int indx_count = 2;
	index[0] = str_len;
	index[1] = *res_length;

	int i = set_parameter(varlist, var_count, "p_SrcData", PINT_ARG_VALUE, *data_ptr, 4);
//	ret = pint_memcpy(&data_ptr, varlist[i].address, str_len, pint_copy_host2device);
//	if (ret != 0)
//		printf("set up p_SrcData fails\n");

	*index_ptr = pint_malloc(4, PINT_ARG_GLOBAL_PTR);
	pint_memcpy(index, *index_ptr, 4 * indx_count , pint_copy_host2device);
	i = set_parameter(varlist, var_count, "p_Index", PINT_ARG_VALUE, *index_ptr, 4);
//	ret = pint_memcpy(&index_ptr, varlist[i].address, 4, pint_copy_host2device);
//	if (ret != 0)
//		printf("set up index_ptr fails\n");

	set_parameter(varlist, var_count, "p_DstData", PINT_ARG_VALUE, *res_ptr, *res_length);
//	printf("results addr %d\n", res_ptr);

	set_parameter(varlist, var_count, "nBatch", PINT_ARG_VALUE, 1, 4);
	set_parameter(varlist, var_count, "op", PINT_ARG_VALUE, op, 4);
}


void pint_hash(char *inp, int length, char *outp, int op, int out_len) {
    char map_path[] = "pint_hash256_GlobalMap.txt";
    char exe_file_b[] = "pint_hash256.b";
    int code_addr = 0;

	int var_count = get_variable_counts(map_path);

	if (var_count > 0) {
	    uap_variable_t * varlist;
//	    printf("\nsize of %d, len %d\n", var_count * sizeof(uap_variable_t), length);
	    varlist = (uap_variable_t *) malloc(var_count * sizeof(uap_variable_t));
		read_map_file(map_path, varlist);

		uint64_t res_ptr, data_ptr, indx_ptr;
		int res_length = out_len;
//		char res_host[HASH_LENGTH+1];
		//
		// set arguments for the app
		//
		pint_app_init();
//		printf("%s %d: results\n.............\n\n", __FILE__, __LINE__);
//		show_pint("hash_fn", inp, length);

	    if (pint_app_load(exe_file_b, code_addr)) {
//	    	printf("%s, %d: length %d\n", __FILE__, __LINE__, length);
			hash_arg(varlist, var_count, inp, length,&data_ptr, &indx_ptr, &res_ptr, &res_length, op);
			//
			// execute the app
			//
	    	pint_app_exe(outp, res_ptr, res_length, op);
//	    	show_pint("push_hash", outp, length);
//	    	printf("%s %d: result_length %d\n", res_length);
	    	pint_free(data_ptr);
	    	pint_free(indx_ptr);
	    	pint_free(res_ptr);
	    } else
	    	printf("Error: loading executables fails....\n");
//		pint_app_close();

		free(varlist);
	}
}

void pint_keccakf(uint64_t *inp, int str_len, char *outp, int rounds) {
    char map_path[] = "pint_hash256_GlobalMap.txt";
    char exe_file_b[] = "pint_hash256.b";
    int code_addr = 0;

	int var_count = get_variable_counts(map_path);

	if (var_count > 0) {
	    uap_variable_t * varlist;
	    varlist = (uap_variable_t *) malloc(var_count * sizeof(uap_variable_t));
		read_map_file(map_path, varlist);

		uint64_t res_ptr, data_ptr, indx_ptr;
		int res_length = str_len; //25 states, uint64_t type
		char res_host[str_len+1];
		//
		// set arguments for the app
		//
	    if (pint_app_load(exe_file_b, code_addr)) {
			hash_arg(varlist, var_count, (char*)inp, rounds,&data_ptr, &indx_ptr, &res_ptr, &res_length, OP_KECCAKF);
			//
			// execute the app
			//
	    	pint_app_exe(outp, res_ptr, res_length, OP_KECCAKF);
	    	pint_free(data_ptr);
	    	pint_free(indx_ptr);
	    	pint_free(res_ptr);
//			memcpy(inp, (char*)res_host, str_len);
//			for( int j = 0; j < str_len; ++j )
//				inp[j] = res_host[j];
//	    	print_out("keccakf", inp, 32);
	    } else
	    	printf("Error: loading executables fails....\n");
//		pint_app_close();

		free(varlist);
	}
}
