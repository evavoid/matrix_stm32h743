#ifndef __BASIC_CA_H
#define __BASIC_CA_H
#include "matrixos.h"


void matlab_conv(float *lis1,float *lis2,u32 blocksize1,u32 blocksize2,float *output);
void decimation(float*input_data,float *output_data,u32 de_step,u32 indata_len);
void DZKX_fixed_to_double(uint8_t *data_fixed, double *data_float, uint32_t data_size, uint8_t point_position);
void DZKX_double_to_fixed(double *data_float, uint8_t *data_fixed, uint32_t data_size, uint8_t point_position);
void change_value(float *a,float *b);
void new_quick_sort(float*list,u32 blocksize);
void max_choose(float*list,u32 blocksize,u32 max_th,u32* max_i);
void min_choose(float*list,u32 blocksize,u32 min_th,u32* min_i);





#endif





