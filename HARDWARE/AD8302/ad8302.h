#ifndef __AD8302_H
#define __AD8302_H
#include "matrixos.h"//系统文件包

void ad8302_event(local_state_space *TEST_state);

void AD8302_adc_init(void);
float get_ADC_D(u8 type);
float get_ADC_C(u8 type);

#endif




