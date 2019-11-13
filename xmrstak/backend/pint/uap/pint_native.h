#ifndef _PINT_NATIVE_H__
#define _PINT_NATIVE_H__

#include "pint_native_const.h"
//int pintInitDevice(void);
//bool pint_device_is_alive(void);
//int pintCloseDevice(void);

typedef unsigned long  uint64_t;

typedef struct {
	char vname[VAVIRABLE_NAME_LENGTH];
	uint64_t address;
	int length;
} uap_variable_t;

bool pint_app_exe(char* result_host, uint64_t result_dev, int result_length, int op);
bool pint_app_load(char* exe_file_b, int code_addr);
bool pint_app_exe_with_data(char* data_file_b,  int data_addr, char* result_host, uint64_t result_dev, int result_length);
//bool pint_app_exe(char* exe_file_b, int code_addr, char* result_host, uint32_t result_dev, int result_length);
void pint_app_close();
bool pint_app_init();
void read_map_file(char *mapfile, uap_variable_t * var_list);
int set_parameter(uap_variable_t *varlist, int var_count, char * param_name, int param_type, uint64_t param_value, int length );
void pint_hash256(char *outp, char *inp, int len);
void pint_groestl(char *inp, int length, char *outp);
void pint_hash(char *inp, int length, char *outp, int op, int out_len);
void pint_keccakf(uint64_t *inp, int length, char *outp, int rounds);
void pint_skein(char *inp, int length, char *outp, int io);
//void show_pint(char* title, char* out, int count);
#endif
