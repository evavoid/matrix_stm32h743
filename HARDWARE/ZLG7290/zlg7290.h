#ifndef __ZLG7290_H
#define __ZLG7290_H
#include "matrixos.h"

//////////////////////////////////////////////////////////////////////底层iic
#define SDA_IN()  {GPIOH->MODER&=0XFFCFFFFF;}//输入 无上下拉设置
#define SDA_OUT() {GPIOH->MODER&=0XFFCFFFFF;GPIOH->MODER|=1<<20;}

#define ZLG_IIC_SDA(n)		(n?HAL_GPIO_WritePin(GPIOH,GPIO_PIN_10,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOH,GPIO_PIN_10,GPIO_PIN_RESET))
#define ZLG_IIC_SCL(n)    (n?HAL_GPIO_WritePin(GPIOH,GPIO_PIN_11,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOH,GPIO_PIN_11,GPIO_PIN_RESET))
#define READ_SDA      HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_10)

void ZLG_IIC_Init(void);                //???IIC?IO?				 
void ZLG_IIC_Start(void);				//??IIC????
void ZLG_IIC_Stop(void);	  			//??IIC????
void ZLG_IIC_Send_Byte(u8 txd);			//IIC??????
u8 ZLG_IIC_Read_Byte(unsigned char ack);//IIC??????
u8 ZLG_IIC_Wait_Ack(void); 				//IIC??ACK??
void ZLG_IIC_Ack(void);					//IIC??ACK??
void ZLG_IIC_NAck(void);				//IIC???ACK??

void ZLG_IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 ZLG_IIC_Read_One_Byte(u8 daddr,u8 addr);	
void zlg7290_event(local_state_space *ZLG7290_state);
///////////////////////////////////////////////////////////////////////
u8 key_read(void);
void key_return(void);
#endif

