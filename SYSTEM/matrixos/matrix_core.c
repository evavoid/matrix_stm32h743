#include "matrix_core.h"//系统文件包
//此处的引用毫无疑问是为了使用全局数据结构

//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
TIM_HandleTypeDef TIM5_Handler;      //高精度us定时器
TIM_HandleTypeDef TIM3_Handler;      //时间片定时器
TIM_HandleTypeDef TIM2_Handler;      //测试定时器

extern global_data_block global_data;
/************************time slice&test************************************************/
void HIGH_precision_timer_Init(u16 timerange_us)
{  
    TIM5_Handler.Instance=TIM5;                          //?????3
    TIM5_Handler.Init.Prescaler=9;                     //??
    TIM5_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //?????
    TIM5_Handler.Init.Period=timerange_us*20-1;                        //?????
    TIM5_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//??????
    HAL_TIM_Base_Init(&TIM5_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM5_Handler); //?????3????3????:TIM_IT_UPDATE    
}

void SYSTEM_timer_Init(u16 timerange_ms)
{  
    TIM3_Handler.Instance=TIM3;                          //?????3
    TIM3_Handler.Init.Prescaler=20000-1;                     //??
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //?????
    TIM3_Handler.Init.Period=timerange_ms*10-1;                        //?????
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//??????
    HAL_TIM_Base_Init(&TIM3_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM3_Handler); //?????3????3????:TIM_IT_UPDATE    
}
void TEST_timer_Init(void)
{  
	  TIM2_Handler.Instance=TIM2;                          //通用定时器3
    TIM2_Handler.Init.Prescaler=200-1;                     //分频
    TIM2_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM2_Handler.Init.Period=65535;                        //自动装载值
    TIM2_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&TIM2_Handler);
    HAL_TIM_Base_Start_IT(&TIM2_Handler); //使能定时器3和定时器3中断  
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
  if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();            //??TIM3??
		HAL_NVIC_SetPriority(TIM3_IRQn,1,3);    //???????,?????1,????3
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //??ITM3??   
	}
	else if(htim->Instance==TIM2)
	{
		__HAL_RCC_TIM2_CLK_ENABLE();            //??TIM2??
		HAL_NVIC_SetPriority(TIM2_IRQn,1,3);    //???????,?????1,????3
		HAL_NVIC_EnableIRQ(TIM2_IRQn);          //??ITM3??   
	}
	else if(htim->Instance==TIM5)
	{
		__HAL_RCC_TIM5_CLK_ENABLE();            //??TIM2??
		HAL_NVIC_SetPriority(TIM5_IRQn,1,3);    //???????,?????1,????3
		HAL_NVIC_EnableIRQ(TIM5_IRQn);          //??ITM3??   
	}
}
//回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim==(&TIM3_Handler)) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		global_data.global_interpreter_flag=1;//打开全局标志
	}
	else if(htim==(&TIM2_Handler)) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		global_data.global_test_time++;
	}
	else if(htim==(&TIM5_Handler)) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		
	}
}
void TIM3_IRQHandler(void)
{
   HAL_TIM_IRQHandler(&TIM3_Handler);
}
void TIM2_IRQHandler(void)
{
   HAL_TIM_IRQHandler(&TIM2_Handler);
}
void TIM5_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM5_Handler);
}
/***********************************************************************************/
void test_start(void)
{
	__HAL_TIM_SET_COUNTER(&TIM2_Handler,0);//重设TIM3定时器的计数器值
	global_data.global_test_time=0;
}
void test_end(void)
{
	double time;
	time=__HAL_TIM_GET_COUNTER(&TIM2_Handler)+(u32)global_data.global_test_time*65536;
	printf("%0.3fms\r\n",time/1000);	
}
/****************************************************************************/
u32 BITS_1(char* string,u8 size)
{
	u8 i=0;
	u32 output=0;
	for(i=0;i<size;i++)
	{
		output+=(1<<((string[2*i]-48)*10+(string[2*i+1]-48)));
	}
	return output;
}
u32 BITS_0(char* string,u8 size)
{
	u32 output;
	output=0xffffffff-BITS_1(string,size);
	return output;
}
/*******************************debug********************************************/
void _Error_Handler(uint8_t* file, uint32_t line,char * string,uint8_t loopen)
{ 
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
	if (loopen)
	{
		while(1)
		{
			delay_ms(500);
			printf("Error on file %s on line %d\r\n---%s\r\n",file,line,string);
		}
	}
	else return;
	/*
		while(NRF24L01_Check())///////////////////////////		  
		{	
			ERROR_HANDLER("nrf24l01_tx init error",0);
		}
	*/
  /* USER CODE END Error_Handler_Debug */
}

void _unsigned_assert(uint8_t* file, uint32_t line,u32 value,char *name)
{ 
	printf("file %s on line %d|%s:%d\r\n",file,line,name,value);
}
void _float_assert(uint8_t* file, uint32_t line,float value,char *name)
{ 
	printf("file %s on line %d|%s:%f\r\n",file,line,name,value);
}
void _double_assert(uint8_t* file, uint32_t line,double value_pin,char *name)
{ 
	printf("file %s on line %d|%s:%lf\r\n",file,line,name,value_pin);
}
/*浮点数一律用指针访问，不直接把值传入函数*///主要问题是这几个函数没声明，/捂脸，所以出现了乱码
/*******************************debug********************************************/
void printf_list(double * list,u32 block_size)
{
	printf("lis=[");
	for(u32 i=0;i<block_size-1;i++)printf("%f,",list[i]);
	printf("%f];",list[block_size-1]);
}

