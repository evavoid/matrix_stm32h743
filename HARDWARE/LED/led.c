#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//初始化PB0,PB1为输出.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOB_CLK_ENABLE();					//开启GPIOB时钟
	
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1;			//PB0，1
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		//推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;         			//上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  	//高速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);     		//初始化GPIOB.0和GPIOB.1
	
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);	//PB0置0
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);	//PB1置1 
		
}

void led_event(local_state_space *LED_state)//每一个底层判断里都跟一个return,比较保险
{
	if(LED_state->ready_state==prepare_state)
	{
		LED_Init();
		LED_state->sleep_next_state=run_state0;//睡眠必备语句
		LED_state->sleep_time=5;//睡眠必备语句
		LED_state->ready_state=sleep_state0;//睡眠必备语句
		return;
	}
	else if(LED_state->ready_state==run_state0)
	{
		LED0_Toggle;
		LED_state->sleep_next_state=run_state0;
		LED_state->sleep_time=1;
		LED_state->ready_state=sleep_state0;
		return;
	}
	else if(LED_state->ready_state==sleep_state0)
	{
		LED_state->sleep_time--;//保证时间的准确性，虽然只打印两次sleeping，但第三次在else if里被耗费过去了，所以总共休眠了三个周期
		if(LED_state->sleep_time==0)//若休眠时间为0，则跳转为正常状态
			LED_state->ready_state=LED_state->sleep_next_state;//可以再加一条语句，将next_state指向
		//dead_state,如果有某一个状态开启了休眠模式却没有设置休眠跳转方向，
		//则会进入死锁状态并发出警报
	}
}


//1.在该文件的对应的头文件中导入"systimer.h"，并在其中最上面定义专属结构体，借助
//"systimer.h"中的枚举类型
//2.将事件函数写完，其函数参数为一个结构体指针,LED_state_space *LED_state，并将该
//函数的声明添加到头文件�
//3.在主函数中，定义一个全局变量LED_state_space led_state，并赋初值，来使用该变量
//main文件中导入四大基本头文件以及led.h

