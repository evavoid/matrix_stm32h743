/***********************Global Environment loading**********************************/
#include "matrixos.h"//ϵͳ�ļ���
#include "includes.h"//�����ļ���
#include "center.h"  //�����ļ���
global_data_block global_data={.global_interpreter_flag=0};task_data_block task_data;
u32 sdram_data[125000] __attribute__((at(0XC0000000)));//�ⲿsdram���������Ϊ16λ,32λ��8λ
int main(void)
{
	/**********************Local space loading & Init********************************/
	local_state_space test_bench_state={.ready_state=prepare_state};
	global_functions_init();global_parameter_init();
	/********************************User code***************************************/
	test_bench_single(&test_bench_state);
	while(1)
	{
		test_bench_multip(&test_bench_state);
		if(global_data.global_interpreter_flag)
		{
			
			
			
			
			
	/********************************************************************************/
			global_data.global_interpreter_flag=0;
		}
	}
}

