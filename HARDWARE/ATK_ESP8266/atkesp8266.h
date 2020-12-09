#if ESPUSART3


#ifndef _ESP8266_H__
#define _ESP8266_H__	 
#include "matrixos.h"
#include "stdio.h"	
#include "stdarg.h"	 	 
#include "stdlib.h"
#include "string.h"	
//PB10���ͽ�ch340��rx//PB11���ս�ch340��tx
//////////////////////////////////////////////////////////////////////////////////////////////////////usart
/***********handle********************/ 
extern UART_HandleTypeDef UART3_Handler; //UART���
extern TIM_HandleTypeDef TIM7_Handler;      //��ʱ����� 
/*****data size defination************/ 
//usart3
#define USART3_MAX_RECV_LEN		800					//�����ջ����ֽ���
#define USART3_MAX_SEND_LEN		800					//����ͻ����ֽ���
#define USART3_RX_EN 			1					//0,������;1,����.
/**********data space*****************/
//usart3
extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//���ջ���,���USART3_MAX_RECV_LEN�ֽ�
extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
extern u16 USART3_RX_STA;   						//��������״̬
/**********function******************/
void usart3_init(u32 bound);				//����3��ʼ�� 
void u3_printf(char* fmt, ...);
void TIM7_Int_Init(u16 arr,u16 psc);

/////////////////////////////////////////////////////////////////////////////////////////////////////esp8266 
/*****��������******/
void atk_8266_at_response(u8 mode);
u8* atk_8266_check_cmd(u8 *str);
/*****��������******/
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime);
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
/*****���㹦�ܺ���******/
u8 atk_8266_quit_trans(void);//�˳�͸��
u8 atk_8266_apsta_check(void);//��ѯ���Ӻ���
u8 atk_8266_consta_check(void);//��ѯ���Ӻ���
void atk_8266_get_wanip(u8* ipbuf);//��ѯip����
void atk_8266_get_ip(u8 x,u8 y);//��ѯip����
/****���Ӧ�ú���****/
/**ap**/
void atk8266_ap_init(u8 mode,u8 ipbuf[16]);//UDP0 TCPS1 TCPC2
void atk8266_ap_data_transform(u8 mode,u8 stop,u32 byte_size,u8* data);//ģʽ��͸��������־��������Ϣ
void atk8266_ap_check_connect(void);
void atk8266_sta_init(u8 mode,u8 ipbuf[16]);//UDP0 TCPS1 TCPC2
void atk8266_sta_data_transform(u8 mode,u8 stop,u32 byte_size,u8* data);//ģʽ��͸��������־��������Ϣ
void atk8266_sta_check_connect(void);
void atk8266_apsta_init(u8 apmode,u8 stamode,u8 ipbuf[16]);//UDP0 TCPS1 TCPC2

/*
void atk_8266_msg_show(u16 x,u16 y,u8 wanip);
void atk_8266_wificonf_show(u16 x,u16 y,u8* rmd,u8* ssid,u8* encryption,u8* password);

u8 atk_8266_mode_cofig(u8 netpro);
u8 atk_8266_netpro_sel(u16 x,u16 y,u8* name);
u8 atk_8266_ip_set(u8* title,u8* mode,u8* port,u8* ip);
*/
/*****example******//*
void atk_8266_mtest_ui(u16 x,u16 y);
void atk_8266_test(void);
u8 atk_8266_apsta_test(void);	//WIFI AP+STAģʽ����
u8 atk_8266_wifista_test(void);	//WIFI STA����
u8 atk_8266_wifiap_test(void);	//WIFI AP����
*/
//�û����ò���
extern const u8* portnum;			//���Ӷ˿�
 
extern const u8* wifista_ssid;		//WIFI STA SSID
extern const u8* wifista_encryption;//WIFI STA ���ܷ�ʽ
extern const u8* wifista_password; 	//WIFI STA ����

extern const u8* wifiap_ssid;		//WIFI AP SSID
extern const u8* wifiap_encryption;	//WIFI AP ���ܷ�ʽ
extern const u8* wifiap_password; 	//WIFI AP ����

extern const u8* ATK_ESP8266_CWMODE_TBL[3];
extern const u8* ATK_ESP8266_WORKMODE_TBL[3];
extern const u8* ATK_ESP8266_ECN_TBL[5];
#endif

#endif




