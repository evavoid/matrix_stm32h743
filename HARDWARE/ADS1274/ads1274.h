#ifndef _ADS1274_H
#define _ADS1274_H
#include "matrixos.h"
#define SPI_4_CHANNEL 0
 
#define ADS1274_SCLK_L    HAL_GPIO_WritePin(GPIOH,GPIO_PIN_12, GPIO_PIN_RESET)
#define ADS1274_SCLK_H    HAL_GPIO_WritePin(GPIOH,GPIO_PIN_12, GPIO_PIN_SET)

#define ADS1274_SYNC_L    HAL_GPIO_WritePin(GPIOH,GPIO_PIN_13, GPIO_PIN_RESET)
#define ADS1274_SYNC_H    HAL_GPIO_WritePin(GPIOH,GPIO_PIN_13, GPIO_PIN_SET)

#define ADS1274_DRDY 			HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_14)
#define ADS1274_DOUT 			HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_15)

#define LSB_P 0.298023259
#define LSB_N -0.297372377


/*******初始化*********************************************************/
void ADS1274_Init(void);



/*******单通道单次转化*************************************************
example:
	ADS1274_Data2Vol(ADS1274_VolConv(ADS1274ReadData()));
	or:
	ADS1274_Data2Vol(ADS1274ReadData());//上一个可能暴毙了
**/

u32 ADS1274ReadData(void);//读取一个通道的24bit数据，也就是一个通道的一次转化的值
u32 ADS1274_VolConv(u32 data);//电压的24bit数据的换一种编码的表示
double ADS1274_Data2Vol(u32 data);//将新编码转化为double



/*******多通道单次转化***********************************************
example:
	一次转化四通道示例
	ADS1274ReadData_SPI(data_in);
	ADS1274_DATAconv(data_in,data_out);
	每次转化将从中取出4个24bit数据，它们将被保存在12个u8型数据中，
	这12个u8型数据先要拼接为4个u32数据，再被转化为4个double数据
**/
void ADS1274ReadData_SPI(u8 *data_in);//接收数据存在data_in里，共存12个
void ADS1274_DATAconv(u8 *data_in, double *data_out);//将上面函数的12个u8转化为4个double



/********高层次应用函数********************************************
example:
	//先初始化
	fpga_32_init();
	ADS1274_Init();

	double ad1274_data[32768];
	ADS1274_getlist_value(ad1274_data,8192);
	for(u32 i=0;i<32768/4;i++)
	{
		printf("%f,",ad1274_data[4*i+1]);
		delay_us(1);
	}
**/
//blocksize不可大于8192点，data_out大小应该为blocksize*4;
void ADS1274_getlist_value(double *data_out,u32 blocksize);

/**
---------------电源
*   *   *   *
5V  GND N/A N/A

I/0
|sync  |gnd
|d4    |gnd
|d3    |gnd
|d2    |gnd
|d1    |gnd
|sclk  |gnd
|drdy  |gnd
**/

#endif
