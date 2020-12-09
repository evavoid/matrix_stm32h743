#include "center.h"

void global_parameter_init(void)
{
	
	
	
}
void global_functions_init(void)
{
	Cache_Enable();                 //打开L1-Cache
	HAL_Init();				              //初始化HAL库
	SDRAM_Init();										//初始化外部sdram
	Stm32_Clock_Init(160,5,2,4);    //设置时钟,400Mhz
	delay_init(400);	    	        //system函数	
	uart_init(115200);
	usmart_dev.init(200);	
	//mem_global_init();
	//SYSTEM_timer_Init(SYSTEM_TIME_MS);
	//HIGH_precision_timer_Init(2000);
	//TEST_timer_Init();
	//usartlcd_init(115200);
}
void test_bench_multip(local_state_space *TEST_state)
{
}
void test_bench_single(local_state_space *TEST_state)
{ 
}
