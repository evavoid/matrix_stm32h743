#ifndef __UARTLCD_H
#define __UARTLCD_H
#include "matrixos.h"
extern UART_HandleTypeDef UART2_Handler;    //UART??
typedef void(*callbake)(void);//指令调用函数的指针
#define USART2_DATA_LEN  	(520) //200 bytes 	

typedef struct{
	u8 USART_BUF[USART2_DATA_LEN];//指令缓冲
	u8 order_successful;//指令阻塞与否
	//0:when there(USART_RX/TX_BUF) is no instruction that is not processed.
	//1:when there(USART_RX/TX_BUF) is a  instruction that is not processed.
	u8 state;//状态机
	u32 cnt_pin;//指针计数
}data_management;

extern data_management tx_manage;//发送结构体
extern data_management rx_manage;//接收结构体

void usartlcd_init(u32 bound);//初始化
callbake instructions(u32 code_group,u32 code_id);
//指令集散中心，用来管理所有的指令，顶层模块和该集散中心有接口，顶层模块也和用户以及串口屏有接口
/****************************************************/
void Instruction_rx(void);//顶层模块之接收
//首先接收数据，然后根据数据特征得到特征参数，把特征参数传入指令集散中心后，即可触发相应事件
/*****************************************************/
void Instruction_tx(u32 code_group,u32 code_id);//顶层模块之发送
//首先根据用户给的特征参数，把特征参数传入指令集散中心后，得到具体的指令包，然后发送指令

void Parameter_key_rx(void);
void Parameter_wave_tx(void);
void Parameter_datasheet_data_tx(void);
void Parameter_datasheet_clear_tx(void);
void Parameter_labal_tx(void);

void Parameter_error(void);

u32 data_size_u32(u32 data);

#endif

//example
//Instruction_tx(1,0);Instruction_tx(1,1); 
//接收指令增减，首先包装好一个指令函数Parameter_xxx_rx，然后在指令集散中心里登记，然后修改Instruction_rx，修改特征参数的提取过程
//发送指令增减，首先包装好一个指令函数Parameter_xxx_tx，然后在指令集散中心里登记，然后由特征参数来使用


