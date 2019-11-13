// keccak.c
// Nov-1-2019  Steven Wang <panyi.ai>
// A 32bit Keccak (3rd round) implementation.

#include <pintdev.h>
#include <stdint.h>
#include "skein_port.h"

int main() {
	char tmp[]="abcdef1";
	tmp[3] = 1;
	tmp[4] = 0;
	tmp[5] = 0;
	tmp[6] = 0;
	uint32_t a[][2] = {{0xfe1d03f1,0x7fd60805},{0x2e58eef0, 0xdf7f2edc}};

	for(int i = 0; i < 6; i++)
		pint_printf("%x ", tmp[i]);
	uint32_t hi = a[0][1] + a[1][1];
	uint32_t lo = a[1][0] + a[1][0];

	pint_printf("\ncast to integer \n%d \n", ((int*) tmp)[0]);
	pint_printf("\n0 hi %x \n lo %x \n", a[0][1],a[0][0]);
	pint_printf("\n1 hi %x \n lo %x \n", a[1][1],a[1][0]);

	pint_printf("\nhi %x \n lo %x \n", hi, lo);
}
