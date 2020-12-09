#ifndef _LED_H
#define _LED_H
#include "matrixos.h"

//LED�˿ڶ���
#define LED0(n)		(n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET))
#define LED0_Toggle (HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1)) //LED0�����ƽ��ת
#define LED1(n)		(n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET))
#define LED1_Toggle (HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0)) //LED1�����ƽ��ת

void led_event(local_state_space *LED_state);//��д��LEDָ�����Ǻ����ڵ���ʽ����
//���βΣ�Сд��led_stateͳһΪ�����ⲿ�����led_state
void LED_Init(void); //LED��ʼ������

#endif