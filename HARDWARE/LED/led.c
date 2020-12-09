#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//³õÊ¼»¯PB0,PB1ÎªÊä³ö.²¢Ê¹ÄÜÕâÁ½¸ö¿ÚµÄÊ±ÖÓ		    
//LED IO³õÊ¼»¯
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOB_CLK_ENABLE();					//¿ªÆôGPIOBÊ±ÖÓ
	
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1;			//PB0£¬1
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		//ÍÆÍìÊä³ö
    GPIO_Initure.Pull=GPIO_PULLUP;         			//ÉÏÀ­
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  	//¸ßËÙ
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);     		//³õÊ¼»¯GPIOB.0ºÍGPIOB.1
	
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);	//PB0ÖÃ0
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);	//PB1ÖÃ1 
		
}

void led_event(local_state_space *LED_state)//Ã¿Ò»¸öµ×²ãÅĞ¶ÏÀï¶¼¸úÒ»¸öreturn,±È½Ï±£ÏÕ
{
	if(LED_state->ready_state==prepare_state)
	{
		LED_Init();
		LED_state->sleep_next_state=run_state0;//Ë¯Ãß±Ø±¸Óï¾ä
		LED_state->sleep_time=5;//Ë¯Ãß±Ø±¸Óï¾ä
		LED_state->ready_state=sleep_state0;//Ë¯Ãß±Ø±¸Óï¾ä
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
		LED_state->sleep_time--;//±£Ö¤Ê±¼äµÄ×¼È·ĞÔ£¬ËäÈ»Ö»´òÓ¡Á½´Îsleeping£¬µ«µÚÈı´ÎÔÚelse ifÀï±»ºÄ·Ñ¹ıÈ¥ÁË£¬ËùÒÔ×Ü¹²ĞİÃßÁËÈı¸öÖÜÆÚ
		if(LED_state->sleep_time==0)//ÈôĞİÃßÊ±¼äÎª0£¬ÔòÌø×ªÎªÕı³£×´Ì¬
			LED_state->ready_state=LED_state->sleep_next_state;//¿ÉÒÔÔÙ¼ÓÒ»ÌõÓï¾ä£¬½«next_stateÖ¸Ïò
		//dead_state,Èç¹ûÓĞÄ³Ò»¸ö×´Ì¬¿ªÆôÁËĞİÃßÄ£Ê½È´Ã»ÓĞÉèÖÃĞİÃßÌø×ª·½Ïò£¬
		//Ôò»á½øÈëËÀËø×´Ì¬²¢·¢³ö¾¯±¨
	}
}


//1.ÔÚ¸ÃÎÄ¼şµÄ¶ÔÓ¦µÄÍ·ÎÄ¼şÖĞµ¼Èë"systimer.h"£¬²¢ÔÚÆäÖĞ×îÉÏÃæ¶¨Òå×¨Êô½á¹¹Ìå£¬½èÖú
//"systimer.h"ÖĞµÄÃ¶¾ÙÀàĞÍ
//2.½«ÊÂ¼şº¯ÊıĞ´Íê£¬Æäº¯Êı²ÎÊıÎªÒ»¸ö½á¹¹ÌåÖ¸Õë,LED_state_space *LED_state£¬²¢½«¸Ã
//º¯ÊıµÄÉùÃ÷Ìí¼Óµ½Í·ÎÄ¼şÖ
//3.ÔÚÖ÷º¯ÊıÖĞ£¬¶¨ÒåÒ»¸öÈ«¾Ö±äÁ¿LED_state_space led_state£¬²¢¸³³õÖµ£¬À´Ê¹ÓÃ¸Ã±äÁ¿
//mainÎÄ¼şÖĞµ¼ÈëËÄ´ó»ù±¾Í·ÎÄ¼şÒÔ¼°led.h

