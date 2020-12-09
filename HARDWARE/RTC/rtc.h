#ifndef __RTC_H
#define __RTC_H
#include "matrixos.h"	

extern RTC_HandleTypeDef RTC_Handler;  //RTC句柄
    
u8 RTC_Init(void);              //RTC初始化
HAL_StatusTypeDef RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm);      //RTC时间设置
HAL_StatusTypeDef RTC_Set_Date(u8 year,u8 month,u8 date,u8 week);	//RTC日期设置
void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec); //设置闹钟时间(按星期闹铃,24小时制)
void RTC_Set_WakeUp(u32 wksel,u16 cnt);             //周期性唤醒定时器设置
/**example:
	RTC_TimeTypeDef RTC_TimeStruct;
  RTC_DateTypeDef RTC_DateStruct;
	
	RTC_Init();
	//RTC_Set_WakeUp(RTC_WAKEUPCLOCK_CK_SPRE_16BITS,0); //配置WAKE UP中断,1秒钟中断一次  
	
	u8 tbuf[40];
	HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
	printf("Time:%02d:%02d:%02d\r\n",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds);

	HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
	printf("Date:20%02d-%02d-%02d\r\nWeek:%d\r\n",RTC_DateStruct.Year,RTC_DateStruct.Month,RTC_DateStruct.Date,RTC_DateStruct.WeekDay);
**/

#endif
