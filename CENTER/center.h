#ifndef __CENTER_H
#define __CENTER_H
#include "matrixos.h"//系统文件包
#include "includes.h"//外设文件包

/****工程管理块*******/
typedef struct
{
	u32 data;
}task_data_block;
extern task_data_block task_data;	

void global_parameter_init(void);
void global_functions_init(void);
void test_bench_single(local_state_space *TEST_state);//循环外只运行一次
void test_bench_multip(local_state_space *TEST_state);//循环内


#endif




