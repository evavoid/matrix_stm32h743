#ifndef __CENTER_H
#define __CENTER_H
#include "matrixos.h"//ϵͳ�ļ���
#include "includes.h"//�����ļ���

/****���̹����*******/
typedef struct
{
	u32 data;
}task_data_block;
extern task_data_block task_data;	

void global_parameter_init(void);
void global_functions_init(void);
void test_bench_single(local_state_space *TEST_state);//ѭ����ֻ����һ��
void test_bench_multip(local_state_space *TEST_state);//ѭ����


#endif




