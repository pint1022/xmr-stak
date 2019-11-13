#ifndef _PINT_NATIVE_H__
#define _PINT_NATIVE_H__

void layer_pint_relu(
    const float *pA_SrcData, // input data 1
    float *pDstData,        // output data
    int len,                // len of iniput data
    int tNum,               // total core num
    int thdid               // the numer of core
);

void matrix_mul(float *A,  //defaul B is col major, A is row major.
                float *B,
                float *C,
                float *D,
                int Ax,
                int Ay, 
                int Bx,
                int thdid
                );
/////param for matrix multi
typedef struct{
    int order;
    int transA;
    int transB;
    float alpha;
    int iOffA;
    int iOffB;
    int iOffC;
    int iLda;
    int iLdb;
    int iLdc;
}GemmParam;                

#endif