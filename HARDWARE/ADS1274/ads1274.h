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


/*******��ʼ��*********************************************************/
void ADS1274_Init(void);



/*******��ͨ������ת��*************************************************
example:
	ADS1274_Data2Vol(ADS1274_VolConv(ADS1274ReadData()));
	or:
	ADS1274_Data2Vol(ADS1274ReadData());//��һ�����ܱ�����
**/

u32 ADS1274ReadData(void);//��ȡһ��ͨ����24bit���ݣ�Ҳ����һ��ͨ����һ��ת����ֵ
u32 ADS1274_VolConv(u32 data);//��ѹ��24bit���ݵĻ�һ�ֱ���ı�ʾ
double ADS1274_Data2Vol(u32 data);//���±���ת��Ϊdouble



/*******��ͨ������ת��***********************************************
example:
	һ��ת����ͨ��ʾ��
	ADS1274ReadData_SPI(data_in);
	ADS1274_DATAconv(data_in,data_out);
	ÿ��ת��������ȡ��4��24bit���ݣ����ǽ���������12��u8�������У�
	��12��u8��������Ҫƴ��Ϊ4��u32���ݣ��ٱ�ת��Ϊ4��double����
**/
void ADS1274ReadData_SPI(u8 *data_in);//�������ݴ���data_in�����12��
void ADS1274_DATAconv(u8 *data_in, double *data_out);//�����溯����12��u8ת��Ϊ4��double



/********�߲��Ӧ�ú���********************************************
example:
	//�ȳ�ʼ��
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
//blocksize���ɴ���8192�㣬data_out��СӦ��Ϊblocksize*4;
void ADS1274_getlist_value(double *data_out,u32 blocksize);

/**
---------------��Դ
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
