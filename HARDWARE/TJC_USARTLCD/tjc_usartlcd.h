#ifndef __TJCUSATRLCD_H
#define __TJCUSATRLCD_H
#include "matrixos.h"
#include "string.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//基于STM32H743IIT6、正点原子底层的陶晶驰K0串口屏驱动函数
//by 王天可						  
////////////////////////////////////////////////////////////////////////////////// 	
#define TJC_LCD 0
#if TJC_LCD
#define HMI_ID_keyword 7

void usart2_init(u32 bound);
u8 get_hmi_recieve_flag(void);
u8 hmi_read_data(void);
u8 hmi_write_data(void);
u8 get_button(void);
u8 serial_passthrough(u8 id, u8 ch, int lenth, u8* obj);
u8 hmi_add_data(u8 id, u8 ch, int number);
u8 hmi_show_txt(u8 name, u8 number);
u8 hmi_show_string(u8 name, char* mystring);
u8 hmi_write_supplement(void);
u8 hmi_show_number(u8 name, double number, u8 mode);
void hmi_val(u8 name, int32_t number);
void hmi_xval(u8 name, float number, double point);
#endif

#endif

