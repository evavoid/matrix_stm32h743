#ifndef _LED_H
#define _LED_H
#include "matrixos.h"

//LED端口定义
#define LED0(n)		(n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET))
#define LED0_Toggle (HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1)) //LED0输出电平翻转
#define LED1(n)		(n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET))
#define LED1_Toggle (HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0)) //LED1输出电平翻转

void led_event(local_state_space *LED_state);//大写的LED指代的是函数内的形式参数
//即形参，小写的led_state统一为函数外部定义的led_state
void LED_Init(void); //LED初始化函数

#endif
