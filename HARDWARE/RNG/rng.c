#include "rng.h"

RNG_HandleTypeDef RNG_Handler;  //RNG���

//��ʼ��RNG
u8 RNG_Init(void)
{
    u16 retry=0;
	
    RNG_Handler.Instance=RNG;   
    HAL_RNG_DeInit(&RNG_Handler);
    HAL_RNG_Init(&RNG_Handler);//��ʼ��RNG
    while(__HAL_RNG_GET_FLAG(&RNG_Handler,RNG_FLAG_DRDY)==RESET&&retry<10000)//�ȴ�RNG׼������
    {
        retry++;
        delay_us(10);
    }
    if(retry>=10000) return 1;//���������������������
    return 0;
}
void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng)
{
	RCC_PeriphCLKInitTypeDef RNGClkInitStruct;

	//����RNGʱ��Դ��ѡ��PLL��ʱ��Ϊ400MHz
	RNGClkInitStruct.PeriphClockSelection=RCC_PERIPHCLK_RNG;  	//����RNGʱ��Դ
	RNGClkInitStruct.RngClockSelection=RCC_RNGCLKSOURCE_PLL;	//RNGʱ��Դѡ��PLL
	HAL_RCCEx_PeriphCLKConfig(&RNGClkInitStruct);
	
	__HAL_RCC_RNG_CLK_ENABLE();//ʹ��RNGʱ��
}

//�õ������
//����ֵ:��ȡ���������
u32 RNG_Get_RandomNum(void)
{
	u32 randomnum;
	
    HAL_RNG_GenerateRandomNumber(&RNG_Handler,&randomnum);
	return randomnum;
}

//����[min,max]��Χ�������
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