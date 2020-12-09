#define ETHUSART3 1
#if ETHUSART3


#include "uarteth.h"
//���ڵ�Ƭ��PA2 TX PA3 RX
//PA2��ת�Ӱ���TXD,PA3��ת�Ӱ���RXD,9600
UART_HandleTypeDef UART3_Handler; 
/***********************************��������***********************************/
ethdata_management tx_ethmanage={.cnt_pin=0};
ethdata_management rx_ethmanage={.cnt_pin=0};
/**************************************init**************************************/
void usarteth_init(u32 bound)
{	
	/**msp**/
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOB_CLK_ENABLE();			//ʹ��GPIOBʱ��
	__HAL_RCC_USART3_CLK_ENABLE();			//ʹ��USART3ʱ��
	GPIO_Initure.Pin=GPIO_PIN_10|GPIO_PIN_11;			//PB10
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
	GPIO_Initure.Pull=GPIO_PULLUP;			//����
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;		//����
	GPIO_Initure.Alternate=GPIO_AF7_USART3;	//����ΪUSART3
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//��ʼ��PB10
	//UART ��ʼ������
	UART3_Handler.Instance=USART3;					    //USART3
	UART3_Handler.Init.BaudRate=bound;				    //������
	UART3_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	UART3_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	UART3_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	UART3_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	UART3_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&UART3_Handler);					    //HAL_UART_Init()��ʹ��UART1
	/******NVIC*********/
	HAL_NVIC_EnableIRQ(USART3_IRQn);			
	HAL_NVIC_SetPriority(USART3_IRQn,2,3);		
	/******IQR**********/
	__HAL_UART_ENABLE_IT(&UART3_Handler,UART_IT_RXNE);	
}
/**********************************management**************************************/
/*********���ɼ�st-link�жϺ���**********/
void USART3_IRQHandler(void)                	
{ 
	if((__HAL_UART_GET_FLAG(&UART3_Handler,UART_FLAG_RXNE)!=RESET))//���յ�һ��byte
	{
		u8 Res;HAL_UART_Receive(&UART3_Handler,&Res,1,1000); 
		rx_ethmanage.USART_BUF[rx_ethmanage.cnt_pin++]=Res;			
	}
	HAL_UART_IRQHandler(&UART3_Handler);	
}
/*********���ɼ�st-link�жϺ���************/
void ch9121_write_setting(u8 code,u8 *data)
{
	tx_ethmanage.USART_BUF[0]=0x57;
	tx_ethmanage.USART_BUF[1]=0xab;
	tx_ethmanage.USART_BUF[2]=code;
	tx_ethmanage.cnt_pin=3;
	
	if((code==CODE_SETLOCALIP)||(code==CODE_SETGATEWAY)||(code==CODE_SETMASK)||(code==CODE_SETREMOTEIP))
	{	
		/**/
		u32 i=0,w=1;int point[5];point[0]=-1;//////
		while(data[i]!='\0')
		{
			if(data[i]=='.')point[w++]=i;
			i++;
		}
		point[4]=i;
		/**/
		for(u32 k=1;k<5;k++)/////
		{
			u8 temp=0;
			for(u32 i=point[k-1]+1;i<point[k];i++)///////
			{
				temp*=10;
				temp+=(data[i]-48);
			}
			tx_ethmanage.USART_BUF[tx_ethmanage.cnt_pin++]=temp;
		}
	}
	else if((code==CODE_SETREMOTEPORT)||(code==CODE_SETLOCALPORT))
	{
		u32 temp=0;u32 i=0;////
		while(data[i]!='\0')
		{
			temp*=10;
			temp+=(data[i]-48);
			i++;
		}
		tx_ethmanage.USART_BUF[tx_ethmanage.cnt_pin++]=(u8)temp;
		tx_ethmanage.USART_BUF[tx_ethmanage.cnt_pin++]=(u8)((temp-(u8)temp)>>8);
	}
	else if(code==CODE_SETBOUND)
	{
		u32 i=0;u32 temp=0;///////
		while(data[i]!='\0')///////
		{
			temp*=10;////
			temp+=(data[i]-48);////
			i++;////
		}
		tx_ethmanage.USART_BUF[tx_ethmanage.cnt_pin++]=(u8)temp;temp-=(u8)temp;temp>>=8;//////
		tx_ethmanage.USART_BUF[tx_ethmanage.cnt_pin++]=(u8)temp;temp-=(u8)temp;temp>>=8;/////
		tx_ethmanage.USART_BUF[tx_ethmanage.cnt_pin++]=(u8)temp;temp-=(u8)temp;temp>>=8;/////
		tx_ethmanage.USART_BUF[tx_ethmanage.cnt_pin++]=(u8)temp;temp-=(u8)temp;temp>>=8;/////
	}
	else if(code==CODE_SETMODE)tx_ethmanage.USART_BUF[tx_ethmanage.cnt_pin++]=data[0]-48;
	else{}
	HAL_UART_Transmit(&UART3_Handler,tx_ethmanage.USART_BUF,tx_ethmanage.cnt_pin,0xffff);
	while(1)
	{
		if(rx_ethmanage.cnt_pin==1&&rx_ethmanage.USART_BUF[0]==0xaa)
		{
			rx_ethmanage.cnt_pin=0;
			return;
		}
	}
}
void ch9121_read_setting(u8 code,u8 *data)
{
	tx_ethmanage.USART_BUF[0]=0x57;
	tx_ethmanage.USART_BUF[1]=0xab;
	tx_ethmanage.USART_BUF[2]=code;
	tx_ethmanage.cnt_pin=3;
	HAL_UART_Transmit(&UART3_Handler,tx_ethmanage.USART_BUF,tx_ethmanage.cnt_pin,0xffff);
	
	if((code==CODE_GETLOCALIP)||(code==CODE_GETMASK)||(code==CODE_GETGATEWAY)||(code==CODE_GETREMOTEIP))
	{
		while(1)
		{
			if(rx_ethmanage.cnt_pin==4)
			{
				sprintf((char*)data,"%u.%u.%u.%u",rx_ethmanage.USART_BUF[0],rx_ethmanage.USART_BUF[1],rx_ethmanage.USART_BUF[2],rx_ethmanage.USART_BUF[3]);
				rx_ethmanage.cnt_pin=0;
				break;
			}
		}
	}
	else if((code==CODE_GETMODE)||(code==CODE_GETRETRYNUM)||(code==CODE_GETUSRTTIMEOUT)||(code==CODE_TCPLINKSTATE))
	{
		while(1)
		{
			if(rx_ethmanage.cnt_pin==1)
			{
				sprintf((char*)data,"%u",rx_ethmanage.USART_BUF[0]);
				rx_ethmanage.cnt_pin=0;
				break;
			}
		}
	}
	else if((code==CODE_GETREMOTEPORT)||(code==CODE_GETLOCALPORT))
	{
		while(1)
		{
			if(rx_ethmanage.cnt_pin==2)
			{
				sprintf((char*)data,"%u",((u32)rx_ethmanage.USART_BUF[1]*256)+(u32)rx_ethmanage.USART_BUF[0]);
				rx_ethmanage.cnt_pin=0;
				break;
			}
		}
	}
	else if(code==CODE_GETBOUND)
	{
		while(1)
		{
			if(rx_ethmanage.cnt_pin==4)
			{
				u32 temp=((u32)rx_ethmanage.USART_BUF[3]<<24)+((u32)rx_ethmanage.USART_BUF[2]<<16)+((u32)rx_ethmanage.USART_BUF[1]<<8)+((u32)rx_ethmanage.USART_BUF[0]);
				sprintf((char*)data,"%u",temp);
				rx_ethmanage.cnt_pin=0;
				break;
			}
		}
	}
	else if(code==CODE_GETMAC)
	{
		while(1)
		{
			if(rx_ethmanage.cnt_pin==6)
			{
				sprintf((char*)data,"%x:%x:%x:%x:%x:%x",rx_ethmanage.USART_BUF[0],rx_ethmanage.USART_BUF[1],rx_ethmanage.USART_BUF[2],rx_ethmanage.USART_BUF[3],rx_ethmanage.USART_BUF[4],rx_ethmanage.USART_BUF[5]);
				rx_ethmanage.cnt_pin=0;
				break;
			}
		}	
	}
}
void ch9121_write_data(u8 *data,u32 blocksize)
{
	HAL_UART_Transmit(&UART3_Handler,data,blocksize,0xffff);
}
/**********************************parameter**************************************/



#endif


