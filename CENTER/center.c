#include "center.h"

void global_parameter_init(void)
{
	
	
	
}
void global_functions_init(void)
{
	Cache_Enable();                 //��L1-Cache
	HAL_Init();				              //��ʼ��HAL��
	SDRAM_Init();										//��ʼ���ⲿsdram
	Stm32_Clock_Init(160,5,2,4);    //����ʱ��,400Mhz
	delay_init(400);	    	        //system����	
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
