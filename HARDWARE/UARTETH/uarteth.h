#define ETHUSART3 1
#if ETHUSART3


#ifndef __UARTETH_H
#define __UARTETH_H
#include "matrixos.h"

/*write setting code*/
#define CODE_START          0x0e
#define CODE_UPDATE         0x0d
#define CODE_EXTICOFG       0x5e
#define CODE_RESET          0x02
#define CODE_SETLOCALIP     0x11
#define CODE_SETMASK        0x12
#define CODE_SETGATEWAY     0x13
#define CODE_SETMODE        0x10
#define CODE_SETLOCALPORT   0x14
#define CODE_SETREMOTEIP    0x15
#define CODE_SETREMOTEPORT  0x16
#define CODE_SETBOUND       0x21
#define CODE_SETUSRTSTOP    0x22
/*read setting code*/
#define CODE_GETMODE        0x60
#define CODE_GETLOCALIP     0x61
#define CODE_GETMASK        0x62
#define CODE_GETGATEWAY     0x63
#define CODE_GETLOCALPORT   0x64
#define CODE_GETREMOTEIP    0x65
#define CODE_GETREMOTEPORT  0x66
#define CODE_GETRETRYNUM    0x67
#define CODE_GETBOUND       0x71
#define CODE_GETUSRTSTOP    0x72
#define CODE_GETUSRTTIMEOUT 0x73
#define CODE_GETMAC         0x81
#define CODE_TCPLINKSTATE   0x03
/*data defination*/
#define USART3_ETHDATA_LEN  (520) //200 bytes 	
typedef struct{
	u8 USART_BUF[USART3_ETHDATA_LEN];//指令缓冲
	u32 cnt_pin;//指针计数
}ethdata_management;

/*extern handle*/
extern UART_HandleTypeDef UART3_Handler;    //UART??
extern ethdata_management tx_ethmanage;//发送结构体
extern ethdata_management rx_ethmanage;//接收结构体

/*basic function*/
void usarteth_init(u32 bound);//初始化
void ch9121_write_data(u8 *data,u32 blocksize);
void ch9121_read_setting(u8 code,u8 *data);
void ch9121_write_setting(u8 code,u8 *data);

/*example:
	io_cfg down;
	ch9121_write_setting(...,...);
	ch9121_write_setting(...,...);
	ch9121_write_setting(...,...);
	ch9121_write_setting(CODE_UPDATE,"");
	ch9121_write_setting(CODE_START,"");
	//ch9121_write_setting(CODE_EXTICOFG,"");
	io_cfg up;
*/



#endif

#endif


