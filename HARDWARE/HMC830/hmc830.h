#ifndef __HMC830_H
#define __HMC830_H
#include "matrixos.h"

#define MODE_FRA 1
//�Ƿ���С��ģʽ��1����С����Ƶ


//IO����
//cen��ʱ���ã��ڿ����İ�����Ҫ�ã��ڲ��İ����ϲ���
#define CEN(n)  (n?HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_RESET)) //SCL
#define SEN(n)  (n?HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_RESET)) //SDA
#define SDI(n)  (n?HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_RESET)) //SCL
#define SCK(n)  (n?HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_RESET)) //SDA
#define SDO(n)  (n?HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2,GPIO_PIN_RESET)) //SCL
#define READ_SDO    HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_2)  //����SDA
//
//0	reg00h 95198h spi check
//1	reg01h 2h			CEN is controlled by host
//2	reg02h 1h			R devider = 1
////////////////3	reg03h Ah			N devider int part = 10
//4	reg05h 6510h	[2:0] = 000 HMC830LP6GE
//								[6:3] = 0010 VCO_Reg02h
//								[15:7] = 011001010 vco output devider = 10 , max gain
//5	reg05h 898h		single output RF_N
//6 reg05h 0h			enable all bands
//7 reg06h 307CAh	PFD:int ,mode B		0011 0000 0111 1100 1010
////7 reg06h F4Ah           1111 0100 1010
//8 reg07h CCDh 	lock detect window
//9 reg08h null		jump over this if rf_refer < 200m
//a	reg09h 376Eh	PFD gain
//b reg0Ah 2046h	
//c reg0Bh 7C061h
//d reg0Fh 81h		locked_sign output
//e reg03h 35			Ndevider=53


//��ʼ��
void HMC830_Init(void);//������ģʽ�����ݺ궨���������
//��������
void HMC830_Write(uint32_t dat);
uint32_t HMC830_Read(uint32_t dat);
void freq_set(u32* nint,u32* decimal,u32* rf_reg,double frew_out_hz);
//�û��ӿ�
u8 HMC830_freq_def(double freq_out_Hz);//������Ƶ����Ӧ������ʼ��
u8 HMC830_freq_def_fra(double freq_out_Hz);//С����Ƶ����ӦС����ʼ��


#endif
