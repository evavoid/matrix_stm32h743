#ifndef __DSP_FILTER_H
#define __DSP_FILTER_H
#include "matrixos.h"

#define STATE_LENGTH 200

extern float firStatef32[];
extern float iirStatef32[];

void f32_fir_init(
	arm_fir_instance_f32* Spin,
	float*pCoeffs,u32 numTaps,u32 signal_size);

void f32_fir_check(
	arm_fir_instance_f32* Spin,
	float* datain,float* dataout,
	u32 data_length,u32 single_data_length);
	
void f32_iir_check(
	arm_biquad_casd_df1_inst_f32* Spin,
	float* datain,float* dataout,
	u32 data_length);
/*************************************************************************************************/
#define MY_ABS(a) ((a>0)?(a):(-a))
void limit_amp_filter(float* lis,float limit,u32 blocksize);
void quicksort(float * lis,u32 low,u32 high);
void mid_value_filter(float* lis,u32 step,u32 blocksize);
void cal_value_filter(double* lis,u32 step,u32 blocksize);

#endif

