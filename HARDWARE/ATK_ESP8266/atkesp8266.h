#if ESPUSART3


#ifndef _ESP8266_H__
#define _ESP8266_H__	 
#include "matrixos.h"
#include "stdio.h"	
#include "stdarg.h"	 	 
#include "stdlib.h"
#include "string.h"	
//PB10发送接ch340的rx//PB11接收接ch340的tx
//////////////////////////////////////////////////////////////////////////////////////////////////////usart
/***********handle********************/ 
extern UART_HandleTypeDef UART3_Handler; //UART句柄
extern TIM_HandleTypeDef TIM7_Handler;      //定时器句柄 
/*****data size defination************/ 
//usart3
#define USART3_MAX_RECV_LEN		800					//最大接收缓存字节数
#define USART3_MAX_SEND_LEN		800					//最大发送缓存字节数
#define USART3_RX_EN 			1					//0,不接收;1,接收.
/**********data space*****************/
//usart3
extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//接收缓冲,最大USART3_MAX_RECV_LEN字节
extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//发送缓冲,最大USART3_MAX_SEND_LEN字节
extern u16 USART3_RX_STA;   						//接收数据状态
/**********function******************/
void usart3_init(u32 bound);				//串口3初始化 
void u3_printf(char* fmt, ...);
void TIM7_Int_Init(u16 arr,u16 psc);

/////////////////////////////////////////////////////////////////////////////////////////////////////esp8266 
/*****辅助函数******/
void atk_8266_at_response(u8 mode);
u8* atk_8266_check_cmd(u8 *str);
/*****基础函数******/
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime);
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
/*****顶层功能函数******/
u8 atk_8266_quit_trans(void);//退出透传
u8 atk_8266_apsta_check(void);//查询连接函数
u8 atk_8266_consta_check(void);//查询连接函数
void atk_8266_get_wanip(u8* ipbuf);//查询ip函数
void atk_8266_get_ip(u8 x,u8 y);//查询ip函数
/****最顶层应用函数****/
/**ap**/
void atk8266_ap_init(u8 mode,u8 ipbuf[16]);//UDP0 TCPS1 TCPC2
void atk8266_ap_data_transform(u8 mode,u8 stop,u32 byte_size,u8* data);//模式，透传结束标志，数据信息
void atk8266_ap_check_connect(void);
void atk8266_sta_init(u8 mode,u8 ipbuf[16]);//UDP0 TCPS1 TCPC2
void atk8266_sta_data_transform(u8 mode,u8 stop,u32 byte_size,u8* data);//模式，透传结束标志，数据信息
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
u8 atk_8266_apsta_test(void);	//WIFI AP+STA模式测试
u8 atk_8266_wifista_test(void);	//WIFI STA测试
u8 atk_8266_wifiap_test(void);	//WIFI AP测试
*/
//用户配置参数
extern const u8* portnum;			//连接端口
 
extern const u8* wifista_ssid;		//WIFI STA SSID
extern const u8* wifista_encryption;//WIFI STA 加密方式
extern const u8* wifista_password; 	//WIFI STA 密码

extern const u8* wifiap_ssid;		//WIFI AP SSID
extern const u8* wifiap_encryption;	//WIFI AP 加密方式
extern const u8* wifiap_password; 	//WIFI AP 密码

extern const u8* ATK_ESP8266_CWMODE_TBL[3];
extern const u8* ATK_ESP8266_WORKMODE_TBL[3];
extern const u8* ATK_ESP8266_ECN_TBL[5];
#endif

#endif




