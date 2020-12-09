/***********************Global Environment loading**********************************/
#include "matrixos.h"//系统文件包
#include "includes.h"//外设文件包
#include "center.h"  //工程文件包
global_data_block global_data={.global_interpreter_flag=0};task_data_block task_data;
u32 sdram_data[125000] __attribute__((at(0XC0000000)));//外部sdram数组可配置为16位,32位，8位
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

