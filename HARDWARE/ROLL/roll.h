#ifndef __ROLL_H
#define __ROLL_H
#include "matrixos.h" 	

#define MAX_VOLTAGE (3.3)
#define MIN_VOLTAGE (0.0)

#define ADVANCE_SEL 5
//5档的选择

u32 roll_judge(float adc);
void roll_Init(void); 				//ADC通道初始化
float get_x_channel(void);
float get_y_channel(void);
void roll_event(local_state_space *	ROLL_state);
#endif 
