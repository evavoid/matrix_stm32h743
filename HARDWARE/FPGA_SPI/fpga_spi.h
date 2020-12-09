#ifndef __FPGASPI_H
#define __FPGASPI_H
#include "matrixos.h"

extern SPI_HandleTypeDef SPI2_Handler_FPGA;  //SPI���
extern DMA_HandleTypeDef  SPI2RxDMA_Handler;      //DMA���
extern DMA_HandleTypeDef  SPI2TxDMA_Handler;      //DMA���
/*******************************************base init******************************************************/
void fpga_32_init(void);
void DMA_ConfigRX(DMA_Stream_TypeDef *DMA_Streamx);
void DMA_ConfigTX(DMA_Stream_TypeDef *DMA_Streamx);
/********************************************user interface************************************************/
//�ܵĳ�ʼ��������������DMA_ConfigTX��DMA_ConfigRX��fpga_32_init��������dma��ֻ��ʼ�����һ������ 
void fpga_spi_stm32_init(void);
void FPGA_ReadWriteByte(u8* TxData,u8 *Rxdata,u16 num);//��dma��ʹ��
void FPGA_ReadWriteByte_DMA(u8* TxData,u8 *Rxdata,u16 num);//dmaģʽ��ʹ��
/*****************************************example**********************************************************/	
	/*
		fpga_spi_stm32_init();
		u8 outdata[16]={123,29,75,55,97,01,75,55,123,29,75,55,97,29,75,55};
		u8 indata[16]={0};
		//Cache_Enable();                 //��L1-Cache
		//���������ж�
		FPGA_ReadWriteByte_DMA(outdata,indata,4);
	*/
	/*
		fpga_32_init();
		u8 outdata[3]={123,29,75};
		u8 indata[3]={0};
		FPGA_ReadWriteByte(outdata,indata,3);
	*/
#endif

