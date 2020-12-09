#include "zlg7290.h"

void EXTI9_5_IRQHandler(void)
{
  //global_data.data[0]=1;
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_9);
}

void ZLG_IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);

  /*Configure GPIO pin : PH9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins : PH10 PH11 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 2);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	//////////////////
	ZLG_IIC_SCL(1);
	ZLG_IIC_SDA(1);

	ZLG_IIC_Start();
	ZLG_IIC_Send_Byte(0x70);ZLG_IIC_Wait_Ack();
	ZLG_IIC_Send_Byte(0x00);ZLG_IIC_Wait_Ack();
	ZLG_IIC_Send_Byte(0xf0);ZLG_IIC_Wait_Ack();
	ZLG_IIC_Stop();
	
	ZLG_IIC_Start();
	ZLG_IIC_Send_Byte(0x70);ZLG_IIC_Wait_Ack();
	ZLG_IIC_Send_Byte(0x01);ZLG_IIC_Wait_Ack();
	ZLG_IIC_Send_Byte(0x00);ZLG_IIC_Wait_Ack();
	ZLG_IIC_Stop();
	
	delay_us(30);
}
void ZLG_IIC_Start(void)
{
	SDA_OUT();
	ZLG_IIC_SDA(1);delay_us(20);	
	ZLG_IIC_SCL(1);delay_us(20);
 	ZLG_IIC_SDA(0);delay_us(20);
	ZLG_IIC_SCL(0);delay_us(20);
}	  
void ZLG_IIC_Stop(void)
{
	SDA_OUT();//sda???
	ZLG_IIC_SCL(0);delay_us(20);
	ZLG_IIC_SDA(0);delay_us(20);
	ZLG_IIC_SCL(1);delay_us(20);
	ZLG_IIC_SDA(1);delay_us(20);							   	
}
u8 ZLG_IIC_Wait_Ack(void)
{
	u16 ucErrTime=0;
	SDA_IN();
	ZLG_IIC_SDA(1);delay_us(20);	   
	ZLG_IIC_SCL(1);delay_us(20);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>1000)
		{
			ZLG_IIC_Stop();
			return 1;
		}
	}
	ZLG_IIC_SCL(0);delay_us(20); 
	return 0;  
} 
//??ACK??
void ZLG_IIC_Ack(void)
{
	ZLG_IIC_SCL(0);delay_us(20);
	SDA_OUT();
	ZLG_IIC_SDA(0);delay_us(20);
	ZLG_IIC_SCL(1);delay_us(20);
	ZLG_IIC_SCL(0);delay_us(20);
}
//???ACK??		    
void ZLG_IIC_NAck(void)
{
	ZLG_IIC_SCL(0);delay_us(20);
	SDA_OUT();
	ZLG_IIC_SDA(1);delay_us(20);
	ZLG_IIC_SCL(1);delay_us(20);
	ZLG_IIC_SCL(0);delay_us(20);
}					 				     

void ZLG_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
		SDA_OUT(); 	    
    ZLG_IIC_SCL(0);delay_us(20);
    for(t=0;t<8;t++)
    {              
        ZLG_IIC_SDA((txd&0x80)>>7);delay_us(20);
        txd<<=1;delay_us(30);
				ZLG_IIC_SCL(1);delay_us(30); 
				ZLG_IIC_SCL(0);delay_us(30);
    }	 
} 	    
u8 ZLG_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA?????
  for(i=0;i<8;i++ )
	{
    ZLG_IIC_SCL(0); delay_us(4);
		ZLG_IIC_SCL(1);delay_us(20);
    receive<<=1;
    if(READ_SDA)receive++;  
		ZLG_IIC_SCL(0); delay_us(20); 
  }					 
  if (!ack)ZLG_IIC_NAck();//??nACK
	else ZLG_IIC_Ack(); //??ACK   
  return receive;
}
u8 key_read(void)
{
	u8 num=10;
	ZLG_IIC_Start();
	ZLG_IIC_Send_Byte(0x70);ZLG_IIC_Wait_Ack();
	ZLG_IIC_Send_Byte(0x01);ZLG_IIC_Wait_Ack();	
	
	ZLG_IIC_Start();
	ZLG_IIC_Send_Byte(0x71);ZLG_IIC_Wait_Ack();
	num=ZLG_IIC_Read_Byte(0);
	ZLG_IIC_Stop();
	
	return num;
}

void key_return()
{
	printf("%d",key_read());
}

void zlg7290_event(local_state_space *ZLG7290_state)
{
	if(ZLG7290_state->ready_state==prepare_state)
	{
		ZLG_IIC_Init();
		ZLG7290_state->ready_state=run_state0;
	}
	else if(ZLG7290_state->ready_state==run_state0)
	{
		/*
		if(global_data.flags[0]==1)
		{
			key_return();
			global_data.flags[0]=0;
		}
		*/
	}
}

