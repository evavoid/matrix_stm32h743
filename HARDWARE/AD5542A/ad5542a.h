#ifndef __AD5542A_H
#define __AD5542A_H
#include "matrixos.h"

//AD5542A驱动by王天可
/*******************************************************************************************************
Single-channel Serial Control 16-bit High Precision DAC (INL=+ +1LSB), Typical Voltage Output Range +2.5V.
Interface pins have been written in H file. If you need to change them, just modify macro definition and 
clock enabler function.

Usage method:
Initialize by calling AD5542A_init (void), then write the voltage directly by calling AD5542A_write_data 
(double vol) function.Default parameter range +2.5, unit V.The AD5542A_CLRdata (void) function can reset 
the AD5542A and set the voltage in the middle of the swing, that is, 0V.
*******************************************************************************************************/


#define AD5542A_PORT GPIOE
#define AD5542A_CSpin GPIO_PIN_11
#define AD5542A_CLKpin GPIO_PIN_12
#define AD5542A_DATApin GPIO_PIN_13
#define AD5542A_LDACpin GPIO_PIN_14
#define AD5542A_CLRpin GPIO_PIN_15

#define AD5542A_CS(n)			(n?HAL_GPIO_WritePin(AD5542A_PORT,AD5542A_CSpin,GPIO_PIN_SET):HAL_GPIO_WritePin(AD5542A_PORT,AD5542A_CSpin,GPIO_PIN_RESET))
#define AD5542A_CLK(n)		(n?HAL_GPIO_WritePin(AD5542A_PORT,AD5542A_CLKpin,GPIO_PIN_SET):HAL_GPIO_WritePin(AD5542A_PORT,AD5542A_CLKpin,GPIO_PIN_RESET))
#define AD5542A_DATA(n)		(n?HAL_GPIO_WritePin(AD5542A_PORT,AD5542A_DATApin,GPIO_PIN_SET):HAL_GPIO_WritePin(AD5542A_PORT,AD5542A_DATApin,GPIO_PIN_RESET))
#define AD5542A_LDAC(n)		(n?HAL_GPIO_WritePin(AD5542A_PORT,AD5542A_LDACpin,GPIO_PIN_SET):HAL_GPIO_WritePin(AD5542A_PORT,AD5542A_LDACpin,GPIO_PIN_RESET))
#define AD5542A_CLR(n)		(n?HAL_GPIO_WritePin(AD5542A_PORT,AD5542A_CLRpin,GPIO_PIN_SET):HAL_GPIO_WritePin(AD5542A_PORT,AD5542A_CLRpin,GPIO_PIN_RESET))

void AD5542A_init(void);
void AD5542A_CLRdata(void);
void AD5542A_write_data(double vol);





#endif
