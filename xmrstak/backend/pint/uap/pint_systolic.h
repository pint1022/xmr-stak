#pragma once

#ifndef _PINT_SYSTOLIC_H__
#define _PINT_SYSTOLIC_H__

#define new_dev_lib 1

//=================================================================================================
// systolic array operation

inline void 
  pint_set_systolic(
    int A_x_stride,int A_y_stride,int ld_A,
    int B_x_stride,int B_y_stride,int ld_B,
    int x_cnt,int y_cnt)
{
  mat_param_t para;
  para.A_x_stride = A_x_stride;
  para.B_x_stride = B_x_stride;
  para.A_y_stride = A_y_stride*ld_A - A_x_stride*(x_cnt-1); // + 1;
  para.B_y_stride = B_y_stride*ld_B - B_x_stride*(x_cnt-1); // + 1;
  para.y_cnt = y_cnt;
  pint_mat_param_set(para);

  //asm volatile("csrw\t0x0c,%0"::"r"(A_x_stride));
	//asm volatile("csrw\t0x0d,%0"::"r"(B_x_stride));
	//asm volatile("csrw\t0x10,%0"::"r"(y_cnt));
	//asm volatile("csrw\t0x0e,%0"::"r"(A_y_stride*ld_A - A_x_stride*(x_cnt-1)));
	//asm volatile("csrw\t0x0f,%0"::"r"(B_y_stride*ld_B - B_x_stride*(x_cnt-1)));

  //pintMatopSetParam(
  //  A_x_stride, 
  //  B_x_stride, 
  //  y_cnt, 
  //  A_y_stride*ld_A - A_x_stride*x_cnt + 1,
  //  B_y_stride*ld_B - B_x_stride*x_cnt + 1);
}

inline void 
  pint_start_systolic(const void* A,const void* B,int cmd_x_cnt)
{
  //cmd_x_cnt_t vv = *((cmd_x_cnt_t*)(&cmd_x_cnt));
#if new_dev_lib==0
  pint_mat_start(
    (unsigned int)A,
    (unsigned int)B,
    *((cmd_x_cnt_t*)(&cmd_x_cnt)));
#else
  pint_mat_start(
    A,
    B,
    *((cmd_x_cnt_t*)(&cmd_x_cnt)));
#endif
}

inline void 
  pint_wait_systolic(int cmd)
{
  //unsigned int status;
	//unsigned int reg = 0x40000000;
	//do
	//{
	//	asm volatile("csrr\t%0,0x1d":"=r"(status));	
	//} while (0 == (status & 0x01));
  //asm volatile("csrw\t0x21,%0"::"r"(reg));
  
#if new_dev_lib==0
  pint_mat_wait_mode1();
#else
  pint_mat_wait( *((cmd_x_cnt_t*)(&cmd)) );
#endif

}

float 
  pint_read_systolic_fp32(int cmd)
{
  /*
  the second parameter is use for 'int8' only.
  so for 'fp32' the second param set any number will be ok.
  */
  //result_t re = pint_mat_result_read_update( *((cmd_x_cnt_t*)(&cmd)) , 0 );
  //return re.result_type.result_fp;
  
  //return pint_mat_result_read_update_mode1();
  
  float result;
  asm volatile("csrr\t%0,0x20\t\n \
    csrwi\t0x21,0":"=r"(result));
  return result;

  //float ret;
  //asm volatile("csrr\t%0,0x20":"=r"(ret));	
  //asm volatile("csrwi\t0x21,0");
  //return ret;
}

inline void 
  pint_set_systolic_int8(
    int A_x_stride,int A_y_stride,int ld_A,
    int B_x_stride,int B_y_stride,int ld_B,
    int x_cnt,int y_cnt)
{
  mat_param_t para;
  para.A_x_stride = A_x_stride;
  para.B_x_stride = B_x_stride;
  para.A_y_stride = A_y_stride*ld_A - A_x_stride*(x_cnt-1); // + 1;
  para.B_y_stride = B_y_stride*ld_B - B_x_stride*(x_cnt-1); // + 1;
  para.y_cnt = y_cnt;
  pint_mat_param_set(para);
}

inline void 
  pint_start_systolic_int8(
    const void* A,
    const void* B,
    int cmd_x_cnt)
{
#if new_dev_lib==0
  pint_mat_start(
    (unsigned int)(A),
    (unsigned int)(B),
    *((cmd_x_cnt_t*)(&cmd_x_cnt)) );
#else
  pint_mat_start(
    A,
    B,
    *((cmd_x_cnt_t*)(&cmd_x_cnt)) );
#endif
}

// mode1
inline void 
  pint_wait_systolic_int8(cmd_x_cnt_t cmd)
{
  // [bug]
#if new_dev_lib==0
  pint_mat_wait_int8_mode1();
#else
  pint_mat_wait(cmd);
#endif
}

inline int 
  pint_read_systolic_int8()
{
  short result[4];

	int resultL,resultH;
	asm volatile("csrr\t%0,0x20\t\n \
				  csrwi\t0x21,0":"=r"(resultL));	
	asm volatile("csrr\t%0,0x29\t\n \
				  csrwi\t0x2a,0":"=r"(resultH));
	result[0] = resultL & 0xffff;
	result[1] = (resultL >> 16 ) & 0xffff;
	result[2] = resultH & 0xffff;
	result[3] = (resultH >> 16 ) & 0xffff;
	
  return result[0]+result[1]+result[2]+result[3];

  //pint_mat_result_read_update_int8_mode1(result);
}

inline int 
  pint_start_share_cache_skip(
    const void* s_addr,
    const void* e_addr)
{
	unsigned int skip_mode=0x1c;
	int _s = ((int )s_addr)/64;
	int _e = ((int )e_addr)/64;
	
	asm volatile("csrw\t0x37,%0"::"r"(_s));
	asm volatile("csrw\t0x38,%0"::"r"(_e));
	asm volatile("csrw\t0x39,%0"::"r"(skip_mode));
	
	unsigned int ready;
	
	asm volatile("csrr\t%0,0x39":"=r"(ready));
	return (ready & 0x10);
}

inline int
  pint_stop_share_cache_skip()
{
	unsigned int skip_mode=0x18;
	asm volatile("csrw\t0x39,%0"::"r"(skip_mode));
	
	unsigned int ready;
	
	asm volatile("csrr\t%0,0x39":"=r"(ready));
	return (ready & 0x10);
}

inline void 
  pint_start_bcache_prefetch_2D(
    const void* pf_addr,int ld,
    int x_stride,
    int y_stride,
    unsigned int x_cnt,
	  unsigned int y_cnt,
    int pf_mode /* manual_mode=0; auto_mode=1; */)
{
  mat_pf_param_t pf_param;
  pf_param.x_stride = x_stride;
  //pf_param.y_stride = y_stride*ld - x_stride*(x_cnt-1);
  pf_param.y_stride = y_stride*ld;
  pf_param.z_stride = 0;
  pf_param.x_cnt    = x_cnt;
  pf_param.y_cnt    = y_cnt;
  pf_param.z_cnt    = 1;
#if new_dev_lib==0
  pint_mat_pf_set(reinterpret_cast<float*>(const_cast<void*>(pf_addr)), pf_param, mode(pf_mode));
#else
  // [bug]
  //pint_mat_pf_set(pf_addr, pf_param, mode(pf_mode));
#endif
}

inline void 
  pint_wait_bcache_prefetch(void)
{
  pint_mat_pf_wait();
}

inline void 
  pint_start_dcache_prefetch_2D(
    const void* pf_addr,int ld,
    int x_stride,
    int y_stride,
    unsigned int x_cnt,
	  unsigned int y_cnt)
{  
  dcache_pf_param_t pf_param;
  pf_param.x_stride = x_stride;
	//pf_param.y_stride = y_stride*ld - x_stride*(x_cnt-1);
	pf_param.y_stride = y_stride*ld;
	pf_param.x_cnt    = x_cnt;
	pf_param.y_cnt    = y_cnt;
#if new_dev_lib==0
  pint_dcache_pf_set(reinterpret_cast<float*>(const_cast<void*>(pf_addr)),pf_param);
#else
  pint_dcache_pf_set(pf_addr,pf_param);
#endif
}

inline void 
  pint_wait_dcache_prefetch(void)
{  
  pint_dcache_pf_wait();
}

inline void 
  pint_set_bcache(unsigned int valid_in_lower_bit)
{
  //pint_assert( (valid_in_lower_bit&0xFFFFFFFC) == 0);
  unsigned int old_cfg;
  asm volatile("csrr\t%0,0x09":"=r"(old_cfg));

  unsigned int new_cfg = (valid_in_lower_bit<<16) | (old_cfg&0xFFFCFFFF);
  asm volatile("csrw\t0x09,%0"::"r"(new_cfg));
}

//=================================================================================================
//// performance counter & settings.
//// name format: pint_[function]_[operator] ;

// enable and clear miss counter.
inline void 
  pint_bcache_miss_counter_start()
{
  unsigned int val;
  asm volatile("csrr\t%0,0x1b":"=r"(val));
  val = val | 0x06; // set [1:2] bits to `1` for enable and clear miss counter.
  asm volatile("csrw\t0x1b,%0"::"r"(val));
}

// retrieve miss counter. And disable miss counter.
inline unsigned int 
  pint_bcache_miss_counter_stop()
{
  unsigned int miss_cnt;
  asm volatile("csrr\t%0,0x1b":"=r"(miss_cnt));

//   *status =miss_cnt >>31;
  // disable.
  unsigned int val = miss_cnt & 0xFFFFF800;
  asm volatile("csrw\t0x1b,%0"::"r"(val));

  return (miss_cnt & 0x7FF);
}

// mask for {dcache_range,stack_range,hit}.  csr<12h3a>[15:17]
#define DCACHE_COUNTER_MASK_BIT0         0x20000 
#define DCACHE_COUNTER_MASK_BIT1         0x10000
#define DCACHE_COUNTER_MASK_BIT2         0x8000

// reference for {dcache_range,stack_range,hit}. csr<12h3a>[14:12]
#define DCACHE_COUNTER_RO                0x4000 
#define DCACHE_COUNTER_STACK             0x2000
#define DCACHE_COUNTER_HIT               0x1000

// clear dcache counter. csr<12h3a>[11]
#define DCACHE_COUNTER_CLEAR             0x800

// enable dcache counter. csr<12h3a>[10]
#define DCACHE_COUNTER_ENABLE            0x400

// mask for opcode. csr<12h3a>[9:5]
#define DCACHE_COUNTER_OPCODE_MASK_BIT0  0x20
#define DCACHE_COUNTER_OPCODE_MASK_BIT1  0x40
#define DCACHE_COUNTER_OPCODE_MASK_BIT2  0x80
#define DCACHE_COUNTER_OPCODE_MASK_BIT3  0x100
#define DCACHE_COUNTER_OPCODE_MASK_BIT4  0x200

// mask for opcode. csr<12h3a>[4:0]
#define DCACHE_COUNTER_OPCODE_LS_SB      0x01
#define DCACHE_COUNTER_OPCODE_LS_SH      0x05
#define DCACHE_COUNTER_OPCODE_LS_SW      0x09
#define DCACHE_COUNTER_OPCODE_LS_WLINE   0x19
#define DCACHE_COUNTER_OPCODE_LS_RW      0x02
#define DCACHE_COUNTER_OPCODE_LS_RLINE   0x06
#define DCACHE_COUNTER_OPCODE_LS_RONCE   0xE
#define DCACHE_COUNTER_OPCODE_LS_FAA     0x03

// record dcache miss address.csr<12h3c>[14]
#define DCACHE_COUNTER_RECORD_LAST_MISS  0x4000 

// Dcache preformance counter setting. 
inline void 
  pint_dcache_counter_start(unsigned int cfg)
{
  unsigned int old;
  asm volatile("csrr\t%0,0x3a":"=r"(old));
  // set enable and clear bit
  old = (old & 0xFFFC0000) | (cfg | DCACHE_COUNTER_CLEAR | DCACHE_COUNTER_ENABLE);
  asm volatile("csrw\t0x3a,%0"::"r"(old));
}

// retrieve counter.
inline unsigned int 
  pint_dcache_counter_stop()
{
  unsigned int cnt;
  asm volatile("csrr\t%0,0x3b":"=r"(cnt));
  return (cnt & 0x00FFFFFF);
}

// Dcache preformance counter setting.
inline void 
  pint_dcache2shared_counter_start(unsigned int cfg)
{
  unsigned int old;
  asm volatile("csrr\t%0,0x3c":"=r"(old));
  // set enable and clear bit
  old = (old & 0xFFFFF000) | (cfg | DCACHE_COUNTER_CLEAR | DCACHE_COUNTER_ENABLE);
  asm volatile("csrw\t0x3c,%0"::"r"(old));
}

// retrieve counter.
inline unsigned int 
  pint_dcache2shared_counter_stop()
{
  unsigned int cnt;
  asm volatile("csrr\t%0,0x3d":"=r"(cnt));
  return (cnt & 0x00FFFFFF);
}

#define PINT_DCACHE_COUNTER_START() \
  pint_dcache_counter_start(\
    DCACHE_COUNTER_MASK_BIT0 | \
    DCACHE_COUNTER_MASK_BIT1 |\
    DCACHE_COUNTER_MASK_BIT2 |\
    DCACHE_COUNTER_RO |\
    /*DCACHE_COUNTER_HIT |*/\
    DCACHE_COUNTER_OPCODE_MASK_BIT0 |\
    DCACHE_COUNTER_OPCODE_MASK_BIT1 |\
    DCACHE_COUNTER_OPCODE_MASK_BIT2 |\
    DCACHE_COUNTER_OPCODE_MASK_BIT3 |\
    DCACHE_COUNTER_OPCODE_MASK_BIT4 |\
    DCACHE_COUNTER_OPCODE_LS_RW\
    /*DCACHE_COUNTER_OPCODE_LS_RLINE*/\
  );

#define PINT_DCACHE2SHARED_COUNTER_START() \
  pint_dcache2shared_counter_start(\
    DCACHE_COUNTER_OPCODE_MASK_BIT0 |\
    DCACHE_COUNTER_OPCODE_MASK_BIT1 |\
    DCACHE_COUNTER_OPCODE_MASK_BIT2 |\
    DCACHE_COUNTER_OPCODE_MASK_BIT3 |\
    DCACHE_COUNTER_OPCODE_MASK_BIT4 |\
    /*DCACHE_COUNTER_OPCODE_LS_RW*/\
    DCACHE_COUNTER_OPCODE_LS_RLINE\
  );

#endif

