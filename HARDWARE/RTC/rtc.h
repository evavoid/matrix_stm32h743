#ifndef __RTC_H
#define __RTC_H
#include "matrixos.h"	

extern RTC_HandleTypeDef RTC_Handler;  //RTC���
    
u8 RTC_Init(void);              //RTC��ʼ��
HAL_StatusTypeDef RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm);      //RTCʱ������
HAL_StatusTypeDef RTC_Set_Date(u8 year,u8 month,u8 date,u8 week);	//RTC��������
void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec); //��������ʱ��(����������,24Сʱ��)
void RTC_Set_WakeUp(u32 wksel,u16 cnt);             //�����Ի��Ѷ�ʱ������
/**example:
	RTC_TimeTypeDef RTC_TimeStruct;
  RTC_DateTypeDef RTC_DateStruct;
	
	RTC_Init();
	//RTC_Set_WakeUp(RTC_WAKEUPCLOCK_CK_SPRE_16BITS,0); //����WAKE UP�ж�,1�����ж�һ��  
	
	u8 tbuf[40];
	HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
	printf("Time:%02d:%02d:%02d\r\n",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds);

	HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
	printf("Date:20%02d-%02d-%02d\r\nWeek:%d\r\n",RTC_DateStruct.Year,RTC_DateStruct.Month,RTC_DateStruct.Date,RTC_DateStruct.WeekDay);
**/

#endif
