#include "tjc_usartlcd.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//����STM32H743IIT6������ԭ�ӵײ���վ���K0��������������
//by �����								  
////////////////////////////////////////////////////////////////////////////////// 	
#if TJC_LCD
//PA2 TX, PA3 RX
UART_HandleTypeDef USART2_Handler;  //USART2���
u8 hmi_recieve[1];
u8 hmi_recieve_flag=0;//���ձ�־λ��0ΪĬ�ϣ�1ָ����Ч��2ָ��ɹ���3�����¼���4�������꣬5͸��������6͸����ɣ�7�ַ������أ�8��ֵ����
u8 hmi_finish_flag=3;//������־λ������3ʱһ�����ݴ������
u8 hmi_recieve_bit=0;//�ڲ���������Ϲ�����޸ĵ���
u8 hmi_ready_flag=0;//����׼����־��0Ϊ׼����1Ϊռ��
u8 hmi_write_waiting=0;
u8 hmi_recieve_data[50], hmi_write_buf[4];//�������ݴ���
u8 hmi_recieve[1];
u8 finish_bit[3]={0xff,0xff,0xff};

//��ʼ��IO ����2
//bound:������
void usart2_init(u32 bound)//Ĭ��921600��ԭװ9600
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();			//ʹ��GPIOAʱ��
	__HAL_RCC_USART2_CLK_ENABLE();			//ʹ��USART2ʱ��
	
	GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3; //PA2,3
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
	GPIO_Initure.Pull=GPIO_PULLUP;			//����
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;		//����
	GPIO_Initure.Alternate=GPIO_AF7_USART2;	//����ΪUSART2
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA2,3
	
	HAL_NVIC_EnableIRQ(USART2_IRQn);				//ʹ��USART1�ж�ͨ��
	HAL_NVIC_SetPriority(USART2_IRQn,0,0);			//��ռ���ȼ�3�������ȼ�3
    
    //USART ��ʼ������
	USART2_Handler.Instance=USART2;					    //USART2
	USART2_Handler.Init.BaudRate=bound;				    //������
	USART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;	//�ֳ�Ϊ8λ���ݸ�ʽ
	USART2_Handler.Init.StopBits=UART_STOPBITS_1;		//һ��ֹͣλ
	USART2_Handler.Init.Parity=UART_PARITY_NONE;		//����żУ��λ
	USART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;	//��Ӳ������
	USART2_Handler.Init.Mode=UART_MODE_TX_RX;			    //�շ�ģʽ
	HAL_UART_Init(&USART2_Handler);					    //HAL_UART_Init()��ʹ��USART2
  HAL_UART_Receive_IT(&USART2_Handler, &hmi_recieve[0], 1);
	
	HAL_UART_Transmit(&USART2_Handler, finish_bit, 3, 0xffff);
}

//��ʾ�����ַ� ���������ƣ�ʡ��T�����ַ�
u8 hmi_show_txt(u8 name, u8 number)
{
	u8 out[30];
	sprintf(out,"t%d.txt=\"%c\"",name, number);
	
	hmi_ready_flag=1;
	HAL_UART_Transmit(&USART2_Handler, out, strlen(out), 0xffff);
	HAL_UART_Transmit(&USART2_Handler, finish_bit, 3, 0xffff);
	hmi_ready_flag=0;
}

//��ʾ�ַ��� ���������ƣ�ʡ��T�����ַ�����ָ��
u8 hmi_show_string(u8 name, char* mystring)
{
	u8 out[30];
	sprintf(out,"t%d.txt=\"%s\"",name, mystring);
	
	hmi_ready_flag=1;
	HAL_UART_Transmit(&USART2_Handler, out, strlen(out), 0xffff);
	HAL_UART_Transmit(&USART2_Handler, finish_bit, 3, 0xffff);
	hmi_ready_flag=0;
}

//���ı����Ϳؼ�����ʾ���֣��˺���������bug�� ���������ƣ�ʡ��T�����ַ���ģʽ��0Ϊ������1Ϊ��������
u8 hmi_show_number(u8 name, double number, u8 mode)
{
	u8 out[30];
	u32 temp=0;
	switch(mode){
		case 0: temp=(u32)number; sprintf(out,"t%d.txt=\"%d\"",name, temp);break;
		case 1: sprintf(out,"t%d.txt=\"%5.2f\"",name, number);break;
	}
	
	hmi_ready_flag=1;
	HAL_UART_Transmit(&USART2_Handler, out, strlen(out), 0xffff);
	HAL_UART_Transmit(&USART2_Handler, finish_bit, 3, 0xffff);
	
}

//��ʾ���� ���������ƣ�ʡ��n��������
void hmi_val(u8 name, int32_t number)
{
	u8 out[30];
	
	sprintf(out,"n%d.val=%d",name, number);
	hmi_ready_flag=1;
	HAL_UART_Transmit(&USART2_Handler, out, strlen(out), 0xffff);
	HAL_UART_Transmit(&USART2_Handler, finish_bit, 3, 0xffff);
	hmi_ready_flag=0;
}

//��ʾ���⸡����  ���������ƣ�ʡ��x�������֣�С��λλ��
void hmi_xval(u8 name, float number, double point)
{
	u8 out[30];
	int32_t show_number = (int32_t)(number*pow(10, point)+0.5);
	sprintf(out,"x%d.val=%d",name, show_number);
	hmi_ready_flag=1;
	HAL_UART_Transmit(&USART2_Handler, out, strlen(out), 0xffff);
	HAL_UART_Transmit(&USART2_Handler, finish_bit, 3, 0xffff);
	hmi_ready_flag=0;
}

u8 get_hmi_recieve_flag(void)
{
	switch(hmi_recieve[0])
	{
		case 0x00: return 1;
		case 0x01: return 2;
		case 0x65: return 3;
		case 0x67: return 4;
		case 0xfe: return 5;
		case 0xfd: return 6;
		case 0x70: return 7;
		case 0x71: return 8;
		default: return 0;
	}
		
}

u8 hmi_read_data(void)
{
	switch(hmi_recieve_flag)
	{
		case 0: return 0xff;
		case 1: return 1;
		case 2: return 2;
		case 3: {
		  hmi_recieve_data[hmi_recieve_bit]=hmi_recieve[0];
			hmi_recieve_bit++;
			return 3;}
		case 4: {
			hmi_recieve_data[hmi_recieve_bit]=hmi_recieve[0];
			hmi_recieve_bit++;
			return 4;}
		case 5: return 5;
		case 6: return 6;
		case 7: {
			hmi_recieve_data[hmi_recieve_bit]=hmi_recieve[0];
			hmi_recieve_bit++;
			return 7;}
		case 8: {
			hmi_recieve_data[hmi_recieve_bit]=hmi_recieve[0];
			hmi_recieve_bit++;
			return 8;}
		default: return 0xff;
	}
			
}
//��Ƭ�����յ�һ���������ݰ�֮��Ĵ���������Ը������ݵĴ������ڴ˺�������ɡ����������޸��жϺ���
//���������Ѽ�¼�ڱ���hmi_recieve_flag��
u8 hmi_write_data(void)
{
//	u8 data=0, mode=0;
//	if(hmi_recieve_flag==3)
//	{
//		if(hmi_recieve_data[1]<24)
//			data = hmi_recieve_data[1]-14;
//		else
//			switch(hmi_recieve_data[1])
//			{
//				case 24: data = 0;  mode=0; printf("%d\n",data); break;
//				case 25: data = '<'; mode=1; printf("%c\n",data) ;break;
//				case 26: data = '>'; mode=1; printf("%c\n",data); break;
//				case 27: data = 'A'; mode=1; printf("%c\n",data); break;
//				case 28: data = 'B'; mode=1; printf("%c\n",data); break;
//				case 29: data = 'C'; mode=1; printf("%c\n",data); break;
//				case 30: data = 'D'; mode=1; printf("%c\n",data); break;
//			}
//			
//		if(hmi_ready_flag==0&&mode==1)
//			hmi_show_txt(HMI_ID_keyword,data);
//			else 
//				if(hmi_ready_flag==0&&mode==0)
//					hmi_show_number(HMI_ID_keyword,data,0);
//				else
//				{
//					hmi_write_waiting=1;
//					hmi_write_buf[0]=data;
//					hmi_write_buf[1]=mode;
//				}
//	}
}

//���Ӵ˺���
u8 hmi_write_supplement(void)
{
	if(hmi_write_waiting==1&&hmi_write_buf[1]==0)
		hmi_show_number(HMI_ID_keyword,hmi_write_buf[0],0);
	if(hmi_write_waiting==1&&hmi_write_buf[1]==1)
		hmi_show_txt(HMI_ID_keyword,hmi_write_buf[0]);
	hmi_write_waiting=0;
}
	
//����͸�� ���ɹ�����0��ʧ�ܷ���0xfe(ָ��ʧ��)0xff(����ʧ��)	����������Ϊ�ؼ�id��ͨ���š������������͵�����ָ��
u8 serial_passthrough(u8 id, u8 ch, int lenth, u8* obj)
{
	int i=0, nmd_timeout=12000000;
	u8 nmd_transmit[20]={0};
	sprintf(nmd_transmit, "addt %d,%d,%d",id,ch,lenth);
	hmi_ready_flag=1;
	HAL_UART_Transmit(&USART2_Handler, nmd_transmit, strlen(nmd_transmit), 0xffff);
	HAL_UART_Transmit(&USART2_Handler, finish_bit, 3, 0xffff);
	
	for(i=0; i<nmd_timeout; i++)
		if(hmi_recieve_flag==5&&hmi_finish_flag==3)
			break;
	if(i>=nmd_timeout)
		return 0xfe;
	else
		HAL_UART_Transmit(&USART2_Handler, obj, lenth, 0xffff);
	
	for(i=0; i<nmd_timeout; i++)
		if(hmi_recieve_flag==6&&hmi_finish_flag==3)
			break;
		hmi_ready_flag=0;
	if(i>=nmd_timeout)
		return 0xff;
	else
		return 0;

}

//�����߿ؼ��д�һ�����ݵ� ���������߿ؼ�ID��ͨ��������
u8 hmi_add_data(u8 id, u8 ch, int number)
{
	u8 out[20]={0};
	sprintf(out, "add %d,%d,%d",id,ch,number);
	hmi_ready_flag=1;
	HAL_UART_Transmit(&USART2_Handler, out, strlen(out), 0xffff);
	HAL_UART_Transmit(&USART2_Handler, finish_bit, 3, 0xffff);
	hmi_ready_flag=0;
}

//�жϴ�����
void USART2_IRQHandler(void)                	
{
	HAL_UART_IRQHandler(&USART2_Handler);	//����HAL���жϴ����ú���
	if(hmi_recieve[0]==0xff)
	{
		hmi_finish_flag++;
		if(hmi_finish_flag==3)
			hmi_write_data();
	}
	else 
	{
		if(hmi_recieve_flag!=0&&hmi_finish_flag!=3)
			hmi_finish_flag=0;
		if(hmi_finish_flag==3)
		{
			hmi_recieve_flag=get_hmi_recieve_flag();
			hmi_finish_flag=0;
			hmi_recieve_bit=0;
		}
		else
			hmi_read_data();
	}
		
//	printf("%x ",hmi_recieve[0]);
	//else
	//	printf("error\n");
	HAL_UART_Receive_IT(&USART2_Handler, &hmi_recieve[0], 1);
	
} 
#endif

