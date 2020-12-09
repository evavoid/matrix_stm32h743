#include "key.h"

//按键初始化函数
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

		__HAL_RCC_GPIOA_CLK_ENABLE();           //开启GPIOA时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();           //开启GPIOC时钟
    __HAL_RCC_GPIOH_CLK_ENABLE();           //开启GPIOH时钟

    GPIO_Initure.Pin=GPIO_PIN_0;            //PA0
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //下拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;     //高速
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_13;           //PC13
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;     //高速
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3; //PH2,3
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
}

void key_lcd_event(local_state_space *KEY_state)
{
	if(KEY_state->ready_state==prepare_state)
	{
		KEY_Init();
		KEY_state->data[0]=0;
		KEY_state->ready_state=run_state0;
	}
	else if(KEY_state->ready_state==run_state0)
	{
		KEY_state->data[0]=(!KEY0)*4+(!KEY1)*2+(WK_UP)*1;
		
		if(KEY_state->data[0])
			KEY_state->ready_state=run_state1;
	}
	else if(KEY_state->ready_state==run_state1)
	{
		if( KEY_state->data[0]==( (!KEY0)*4+(!KEY1)*2+(WK_UP)*1 ) )
		{
			switch(KEY_state->data[0])
			{
				case 0:{break;}
				case 1:{break;}
				case 2:{break;}
				case 3:{break;}
				case 4:{break;}
				case 5:{break;}
				case 6:{break;}
				case 7:{break;}
			}
		}
		KEY_state->ready_state=run_state0;
	}
}
