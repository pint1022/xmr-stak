/*!
 * Copyright 2019 by Panyi Inc
 * \file pint_hash.cpp
 * \brief This is an implementation of 'blake256'.
 *  Author: Steven Wang, swang@panyi.ai.
 *
 *  compile: uapcc -d pint_hash256.cpp relu.cpp c_blake256.cpp sgemm1.cpp
 *
 */
#include <pintdev.h>
#include <stdint.h>
#include "pint_systolic.h"
#include "pint_native_dev.h"
#include "c_blake256.h"
#include "c_groestl.h"
#include "c_jh.h"
#include "c_keccak.h"
#include "c_skein.h"
#include "pint_native_const.h"

float *pA_SrcData; // input data
float *pB_SrcData; // input data
float *pC_SrcData; // input data
float *pDstData;                // output data

int nBatch;                     // batch size
int nHeight;                    // height of input data
int nWidth;                     // width of input data
int nChannel;                    // channel of iniput data
int op;                          // opcode
                                   // 1: relu
                                   // 2: matmul
//
// matmul parameter
//
int M;
int N;
int K; 

int transA;
int transB; 
float alpha;
float beta;
int offA;
int offB;
int offC;
int lda;
int ldb;
int ldc;

//
// hash data
//
unsigned char *p_SrcData; // input data
unsigned char *p_DstData; // input data
unsigned int *p_Index; // input data and output data length: 0: input data length; 1: output data length

void relu() 
{
    int thdid = pint_global_ID_get().idPTC;
    int tNum = pint_core_number_get().nPTC;
    int total = nBatch * nWidth;

    if (!pA_SrcData  || !pDstData)
    {
        pint_printf("relu: Please check buffer pointer. \n");
        return;
    }

    pint_ro_seg_set(pA_SrcData, pA_SrcData + total);

    unsigned v;
    asm volatile("csrr\t%0,0x09\t\n" : "=r"(v));
    v |= 0x08;
    asm volatile("csrw\t0x09,%0\t\n" ::"r"(v));


    layer_pint_relu(pA_SrcData, pDstData, total, tNum, thdid);;
    
}	

GemmParam gemmpara;

void matmul()
{
    int thdid = pint_global_ID_get().idPTC;
    float *D = pC_SrcData;

    int Ax = K;
    int Ay = M;
    int Bx = N;
    if (!pA_SrcData  || !pB_SrcData || !pC_SrcData)
    {
        pint_printf("matmul: Please check buffer pointer. \n");
        return;
    }
    asm volatile("uap.sync");
    if (thdid==0){
        gemmpara.transA = transA;
        gemmpara.transB = transB;
    }
    asm volatile("uap.sync");

    matrix_mul(pA_SrcData,pB_SrcData, 0, D, Ax, Ay, Bx, thdid);
}

void hash256() {
    int thdid = pint_global_ID_get().idPTC;
	if (thdid < nBatch) {
	   char * d_tmp = p_DstData + thdid;
	   int width = p_Index[thdid];
	   int * src_tmp = (int*)p_SrcData + width*thdid;
	   char *tmp= (char*)src_tmp;
//	   pint_printf("====== core %d: src %d,int addr %d, dst %d, width %d\n", thdid, tmp, src_tmp, d_tmp, width);
//	   for (int i = 0; i < width; i++) {
//		   tmp[i]= src_tmp[i];
//	   }
	   blake256_hash(d_tmp, tmp, width);
//	   for (int i=0; i<32; i++)
//	   	  pint_printf("i %d, out %x\n", i, d_tmp[i]);
	}
}

void groestl_hash() {
    int thdid = pint_global_ID_get().idPTC;
	if (thdid < nBatch) {
	   char * d_tmp = p_DstData + thdid;
	   int width = p_Index[thdid];
	   int * src_tmp = (int*)p_SrcData + width*thdid;
	   BitSequence *tmp= (BitSequence*)src_tmp;
//	   pint_printf("====== core %d: src %d,int addr %d, dst %d, width %d\n", thdid, tmp, src_tmp, d_tmp, width);
//	   for (int i = 0; i < width; i++) {
//		   tmp[i]= src_tmp[i];
//	   }
	   groestl(tmp, width, (BitSequence*)d_tmp);
//	   for (int i=0; i<32; i++)
//	   	  pint_printf("i %d, out %x\n", i, d_tmp[i]);
	}
}

void jh() {
    int thdid = pint_global_ID_get().idPTC;
	if (thdid < nBatch) {
	   char * d_tmp = p_DstData + thdid;
	   int width = p_Index[thdid];
	   int * src_tmp = (int*)p_SrcData + width*thdid;
	   BitSequence *tmp= (BitSequence*)src_tmp;
//	   pint_printf("====== core %d: src %d,int addr %d, dst %d, width %d\n", thdid, tmp, src_tmp, d_tmp, width);
//	   for (int i = 0; i < width; i++) {
//		   tmp[i]= src_tmp[i];
//	   }
	   jh_hash(256, tmp, width, (BitSequence*)d_tmp);
//	   for (int i=0; i<32; i++)
//	   	  pint_printf("i %d, out %x\n", i, d_tmp[i]);
	}
}

void keccak_hash() {
    int thdid = pint_global_ID_get().idPTC;
	if (thdid < nBatch) {
	   char * d_tmp = p_DstData + thdid;
	   int width = p_Index[thdid];
	   char * tmp = p_SrcData + width*thdid;
//	   pint_printf("====== core %d: src %d, dst %d, width %d\n", thdid, tmp, d_tmp, width);
//	   for (int i = 0; i < width; i++) {
//		   tmp[i]= src_tmp[i];
//	   }
	   keccak1600(tmp, width, (char*)d_tmp);
//	   for (int i=0; i<32; i++)
//	   	  pint_printf("i %d, out %x\n", i, d_tmp[i]);
	}
}

void keccak_md_hash() {
    int thdid = pint_global_ID_get().idPTC;
	if (thdid < nBatch) {
	   char * d_tmp = p_DstData + thdid;
	   int * indx_ptr = p_Index + thdid * 2;
	   int width = *indx_ptr;
	   int md = *(indx_ptr + 1);
	   char * tmp = p_SrcData + width*thdid;
//	   pint_printf("====== core %d: src %d, dst %d, idx_ptr %d width %d, md %d\n", thdid, tmp, d_tmp, p_Index, width, md);
//	   for (int i = 0; i < width; i++) {
//		   tmp[i]= src_tmp[i];
//	   }
	   keccak(tmp, width, (char*)d_tmp, md);
//	   for (int i=0; i<32; i++)
//	   	  pint_printf("i %d, out %x\n", i, d_tmp[i]);
	}
}

void keccakf_hash() {
    int thdid = pint_global_ID_get().idPTC;
	if (thdid < nBatch) {
	   char * d_tmp = p_DstData + thdid;
	   int rounds = p_Index[thdid];
	   uint64b_t * tmp = (uint64b_t*)(p_SrcData + 25*64*thdid);
//	   pint_printf("====== core %d: src %d, dst %d, rounds %d\n", thdid, tmp, d_tmp, rounds);
//	   for (int i = 0; i < width; i++) {
//		   tmp[i]= src_tmp[i];
//	   }
	   keccakf((uint64b_t*)tmp, rounds);
//	   for (int i=0; i<32; i++)
//	   	  pint_printf("i %d, out %x\n", i, d_tmp[i]);
	}
}


void skein_call() {
    int thdid = pint_global_ID_get().idPTC;
	if (thdid < nBatch) {
	   char * d_tmp = p_DstData + thdid;
	   int width = p_Index[thdid];
	   char * tmp = p_SrcData + width*thdid;
//	   pint_printf("===== core %d: src %d, dst %d, width %d\n", thdid, tmp, d_tmp, width);
//	   for (int i = 0; i < width; i++) {
//		   tmp[i]= src_tmp[i];
//	   }
//	   kein_hash(int hashbitlen, SkeinBitSequence* data, SkeinDataLength databitlen, SkeinBitSequence* hashval);
	   skein_hash(256, tmp, width, (BitSequence*)d_tmp);
//	   for (int i=0; i<32; i++)
//	   	  pint_printf("i %d, out %x\n", i, d_tmp[i]);
	}
}

int main() 
{ 
	switch(op) {
	case OP_RELU:
//        pint_printf("\n\n ++++++++++++ : Relu(%d), len = %d, batch size = %d\n", op, nWidth, nBatch);
		pint_parallel_start(relu);
		break;
	case OP_MATMUL:
//        pint_printf("\n\n ++++++++++++ : Matmul(%d), M = %d, N = %d, K = %d\n", op, M, N, K);
		pint_parallel_start(matmul);
		break;
	case OP_BLAKE:
		p_SrcData[p_Index[0]] = '\0';
//        pint_printf("\n\n ++++++++++++ : Hash256(%d),  batch = %d\n", op, nBatch);
		pint_parallel_start(hash256);
		break;
	case OP_GROESTL:
		p_SrcData[p_Index[0]] = '\0';
//        pint_printf("\n\n ++++++++++++ : groestl(%d),  batch = %d\n",op, nBatch);
		pint_parallel_start(groestl_hash);
		break;
	case OP_JH:
		p_SrcData[p_Index[0]] = '\0';
//        pint_printf("\n\n ++++++++++++ : jh(%d),  batch = %d\n", op,  nBatch);
		pint_parallel_start(jh);
		break;
	case OP_KECCAK:
		p_SrcData[p_Index[0]] = '\0';
//        pint_printf("\n\n ++++++++++++ : keccak_hash(%d), batch = %d\n", op, nBatch);
		pint_parallel_start(keccak_hash);
		break;
	case OP_KECCAKF:
		p_SrcData[25*64] = '\0';
//        pint_printf("\n\n ++++++++++++ : keccakf_hash(%d), batch = %d\n", op, nBatch);
		pint_parallel_start(keccakf_hash);
		break;
	case OP_KECCAK_MD:
		p_SrcData[25*64] = '\0';
//        pint_printf("\n\n ++++++++++++ : keccak_md_hash(%d), batch = %d\n", op, nBatch);
		pint_parallel_start(keccak_md_hash);
		break;
	case OP_SKEIN:
		p_SrcData[p_Index[0]] = '\0';
//        pint_printf("\n\n ++++++++++++ : skein_hash(%d), batch = %d\n", op, nBatch);
		pint_parallel_start(skein_call);
		break;
	default:
        pint_printf("\n\n PPU Error: ++++++++++++ Unknown operation (%d): %d\n ", &op, op);
	}
    return 0;  		
} 
