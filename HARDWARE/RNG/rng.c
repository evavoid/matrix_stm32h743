#include "rng.h"

RNG_HandleTypeDef RNG_Handler;  //RNG句柄

//初始化RNG
u8 RNG_Init(void)
{
    u16 retry=0;
	
    RNG_Handler.Instance=RNG;   
    HAL_RNG_DeInit(&RNG_Handler);
    HAL_RNG_Init(&RNG_Handler);//初始化RNG
    while(__HAL_RNG_GET_FLAG(&RNG_Handler,RNG_FLAG_DRDY)==RESET&&retry<10000)//等待RNG准备就绪
    {
        retry++;
        delay_us(10);
    }
    if(retry>=10000) return 1;//随机数产生器工作不正常
    return 0;
}
void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng)
{
	RCC_PeriphCLKInitTypeDef RNGClkInitStruct;

	//设置RNG时钟源，选择PLL，时钟为400MHz
	RNGClkInitStruct.PeriphClockSelection=RCC_PERIPHCLK_RNG;  	//设置RNG时钟源
	RNGClkInitStruct.RngClockSelection=RCC_RNGCLKSOURCE_PLL;	//RNG时钟源选择PLL
	HAL_RCCEx_PeriphCLKConfig(&RNGClkInitStruct);
	
	__HAL_RCC_RNG_CLK_ENABLE();//使能RNG时钟
}

//得到随机数
//返回值:获取到的随机数
u32 RNG_Get_RandomNum(void)
{
	u32 randomnum;
	
    HAL_RNG_GenerateRandomNumber(&RNG_Handler,&randomnum);
	return randomnum;
}

//生成[min,max]范围的随机数
int RNG_Get_RandomRange(int min,int max)
{ 
	u32 randomnum;
	
	HAL_RNG_GenerateRandomNumber(&RNG_Handler,&randomnum);
	return randomnum%(max-min+1)+min;
}

void random_in_place(float*list,u32 blocksize)
{
	for(u32 i=1;i<blocksize;i++)
	{
		int rand=RNG_Get_RandomRange(i+1,blocksize-1);
		float temp=list[i];
		list[i]=list[rand];
		list[rand]=temp;
	}
}
