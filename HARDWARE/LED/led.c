#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//��ʼ��PB0,PB1Ϊ���.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOB_CLK_ENABLE();					//����GPIOBʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1;			//PB0��1
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		//�������
    GPIO_Initure.Pull=GPIO_PULLUP;         			//����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  	//����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);     		//��ʼ��GPIOB.0��GPIOB.1
	
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);	//PB0��0
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);	//PB1��1 
		
}

void led_event(local_state_space *LED_state)//ÿһ���ײ��ж��ﶼ��һ��return,�Ƚϱ���
{
	if(LED_state->ready_state==prepare_state)
	{
		LED_Init();
		LED_state->sleep_next_state=run_state0;//˯�߱ر����
		LED_state->sleep_time=5;//˯�߱ر����
		LED_state->ready_state=sleep_state0;//˯�߱ر����
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
		LED_state->sleep_time--;//��֤ʱ���׼ȷ�ԣ���Ȼֻ��ӡ����sleeping������������else if�ﱻ�ķѹ�ȥ�ˣ������ܹ���������������
		if(LED_state->sleep_time==0)//������ʱ��Ϊ0������תΪ����״̬
			LED_state->ready_state=LED_state->sleep_next_state;//�����ټ�һ����䣬��next_stateָ��
		//dead_state,�����ĳһ��״̬����������ģʽȴû������������ת����
		//����������״̬����������
	}
}


//1.�ڸ��ļ��Ķ�Ӧ��ͷ�ļ��е���"systimer.h"���������������涨��ר���ṹ�壬����
//"systimer.h"�е�ö������
//2.���¼�����д�꣬�亯������Ϊһ���ṹ��ָ��,LED_state_space *LED_state��������
//������������ӵ�ͷ�ļ��
//3.���������У�����һ��ȫ�ֱ���LED_state_space led_state��������ֵ����ʹ�øñ���
//main�ļ��е����Ĵ����ͷ�ļ��Լ�led.h

