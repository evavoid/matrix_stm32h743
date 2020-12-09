#ifndef __UARTLCD_H
#define __UARTLCD_H
#include "matrixos.h"
extern UART_HandleTypeDef UART2_Handler;    //UART??
typedef void(*callbake)(void);//ָ����ú�����ָ��
#define USART2_DATA_LEN  	(520) //200 bytes 	

typedef struct{
	u8 USART_BUF[USART2_DATA_LEN];//ָ���
	u8 order_successful;//ָ���������
	//0:when there(USART_RX/TX_BUF) is no instruction that is not processed.
	//1:when there(USART_RX/TX_BUF) is a  instruction that is not processed.
	u8 state;//״̬��
	u32 cnt_pin;//ָ�����
}data_management;

extern data_management tx_manage;//���ͽṹ��
extern data_management rx_manage;//���սṹ��

void usartlcd_init(u32 bound);//��ʼ��
callbake instructions(u32 code_group,u32 code_id);
//ָ�ɢ���ģ������������е�ָ�����ģ��͸ü�ɢ�����нӿڣ�����ģ��Ҳ���û��Լ��������нӿ�
/****************************************************/
void Instruction_rx(void);//����ģ��֮����
//���Ƚ������ݣ�Ȼ��������������õ�������������������������ָ�ɢ���ĺ󣬼��ɴ�����Ӧ�¼�
/*****************************************************/
void Instruction_tx(u32 code_group,u32 code_id);//����ģ��֮����
//���ȸ����û�����������������������������ָ�ɢ���ĺ󣬵õ������ָ�����Ȼ����ָ��

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
//����ָ�����������Ȱ�װ��һ��ָ���Parameter_xxx_rx��Ȼ����ָ�ɢ������Ǽǣ�Ȼ���޸�Instruction_rx���޸�������������ȡ����
//����ָ�����������Ȱ�װ��һ��ָ���Parameter_xxx_tx��Ȼ����ָ�ɢ������Ǽǣ�Ȼ��������������ʹ��


