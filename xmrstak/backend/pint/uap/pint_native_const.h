/**
 * @defgroup pint-native
 * @ingroup pint-native-api
 * Implementation of native support API.
 */

/**
 * PINT native api
 *
 * @file    pint_native_const.c
 * @ingroup pint-native-api
 * @brief   it defines the common constants among host and device side codes
 * @author  steven wang
 * @email   yning@panyi.ai
 * @version 0.0.1
 *
 */

#ifndef _PINT_NATIVE_CONST_H__
#define _PINT_NATIVE_CONST_H__

#define VALUE_PARAM  0
#define POINTER_PARAM  1

#define VAVIRABLE_NAME_LENGTH  40
#define VAR_NAME 0
#define VAR_ADDR 1
#define VAR_LEN 2


#define OP_RELU			1
#define OP_MATMUL		2
#define OP_BLAKE		3
#define OP_GROESTL		4
#define OP_JH 			5
#define OP_KECCAK 		6
#define OP_KECCAKF 		7
#define OP_KECCAK_MD 	8
#define OP_SKEIN 		9

#endif
