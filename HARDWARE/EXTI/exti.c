#include "exti.h"

extern TIM_HandleTypeDef TIM2_Handler;

void EXTI1_IRQHandler(void)
{
	global_data.data[1]+=1;
  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
}
void EXTI3_IRQHandler(void)
{
  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_3);
}

void EXTI2_IRQHandler(void)
{
	/*
	HAL_NVIC_DisableIRQ(TIM3_IRQn);
	HAL_NVIC_DisableIRQ(TIM4_IRQn);
	HAL_NVIC_DisableIRQ(EXTI1_IRQn);
	HAL_NVIC_DisableIRQ(EXTI2_IRQn);
	HAL_NVIC_DisableIRQ(EXTI3_IRQn);
	*/
  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_3);
}

void EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();               //开启GPIOA时钟K0,PH3
		//__HAL_RCC_GPIOH_CLK_ENABLE();
    
    GPIO_Initure.Pin=GPIO_PIN_1;                //PA1
    GPIO_Initure.Mode=GPIO_MODE_IT_RISING;      //上升沿触发
    GPIO_Initure.Pull=GPIO_PULLDOWN;			//下拉
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
		/*
		GPIO_Initure.Pin=GPIO_PIN_3|GPIO_PIN_2;
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     
    GPIO_Initure.Pull=GPIO_PULLUP;			
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
		*/
    //中断线0
    HAL_NVIC_SetPriority(EXTI1_IRQn,2,0);       //抢占优先级为2，子优先级为0
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);             //使能中断线0
		/*
		HAL_NVIC_SetPriority(EXTI3_IRQn,2,0);       //抢占优先级为2，子优先级为0
    HAL_NVIC_EnableIRQ(EXTI3_IRQn); 
		
		HAL_NVIC_SetPriority(EXTI2_IRQn,3,0);       //抢占优先级为2，子优先级为0
    HAL_NVIC_EnableIRQ(EXTI2_IRQn); 
		*/
}
