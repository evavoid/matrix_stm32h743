#if ESPUSART3


#include "atkesp8266.h"

u8 ap_string[64];
void atk8266_ap_init(u8 mode,u8 ipbuf[16])//UDP0 TCPS1 TCPC2
//192.168.1.2
//�����UDP��TCP�ͻ��ˣ���inpuf������ip���룬�����TCP������򲻹ܼ���
{
	/*****������******/
	usart3_init(115200);delay_us(10);//��ʼ������3������Ϊ115200
	while(atk_8266_send_cmd("AT","OK",20))//���ģ���Ƿ����
	{
		atk_8266_quit_trans();//�ر�͸��
		atk_8266_send_cmd("AT+CIPMODE=0","OK",200); //�ر�͸��ģʽ
		printf("δ��⵽ģ��!\r\n");
		delay_ms(800);
		printf("��������ģ��...\r\n");
	}printf("ģ�����ӳɹ�...\r\n");
	while(atk_8266_send_cmd("ATE0","OK",20));printf("�ɹ��رջ���\r\n");//�����رջ���ֱ����OK����
	/*****AP��������******/
	printf("��ʼ����AP\r\n");
	atk_8266_send_cmd("AT+CWMODE=2","OK",100);//����ΪAPģʽ
	atk_8266_send_cmd("AT+RST","OK",100);//����
	delay_ms(1000);	delay_ms(1000);	delay_ms(1000); delay_ms(1000);//�����ȴ�
	sprintf((char*)ap_string,"AT+CWSAP=\"%s\",\"%s\",1,4",wifiap_ssid,wifiap_password);
	atk_8266_send_cmd(ap_string,"OK",1000);//��������AP����
	/*****APϸ������******/
	if(mode==0)//UDP
	{
		printf("Ŀ���ip:%s,port:%s\r\n",ipbuf,(u8*)portnum);
		printf("��������UDP...\r\n");
		atk_8266_send_cmd("AT+CIPMUX=0","OK",100);  //������ģʽ
		sprintf((char*)ap_string,"AT+CIPSTART=\"UDP\",\"%s\",%s",ipbuf,(u8*)portnum);    //����Ŀ��UDP������
		while(atk_8266_send_cmd(ap_string,"OK",500));//��������Ŀ���
		printf("UDP���óɹ���������Ŀ���\r\n");
	}
	else if(mode==1)//TCP Client
	{
		printf("Ŀ���ip:%s,port:%s\r\n",ipbuf,(u8*)portnum);
		printf("��������TCP_Client\r\n");
		atk_8266_send_cmd("AT+CIPMUX=0","OK",100);  //������ģʽ
		sprintf((char*)ap_string,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //����Ŀ��UDP������
		while(atk_8266_send_cmd(ap_string,"OK",200));//��������Ŀ���	
		atk_8266_send_cmd("AT+CIPMODE=1","OK",200);//����ģʽΪ��͸��		
		printf("TCP_Client���óɹ���������Ŀ���\r\n");
	}
	else if(mode==2)//TCP Server
	{
		printf("��������TCP_Server\r\n");
		atk_8266_send_cmd("AT+CIPMUX=1","OK",20);//1��������
		sprintf((char*)ap_string,"AT+CIPSERVER=1,%s",(u8*)portnum);
		atk_8266_send_cmd(ap_string,"OK",20);//����TCP����
		printf("TCP_Server���óɹ�\r\n");
		atk_8266_get_wanip(ipbuf);//������ģʽ,��ȡIP
		sprintf((char*)ap_string,"IP��ַ:%s �˿�:%s",ipbuf,(u8*)portnum);printf("%s\r\n",ap_string);
		/****�ȴ�����******/
		printf("�����豸����WIFI�ȵ�:%s,%s,%s",(u8*)wifiap_ssid,(u8*)wifiap_encryption,(u8*)wifiap_password);
	}
}

void atk8266_ap_data_transform(u8 mode,u8 stop,u32 byte_size,u8* data)
{
	u8 tem_string[32];
	if(stop)
	{
		atk_8266_quit_trans();	//�˳�͸��
		atk_8266_send_cmd("AT+CIPMODE=0","OK",20);   //�ر�͸��ģʽ
		return;
	}
	if(mode==0)//UDP
	{
		sprintf((char*)tem_string,"AT+CIPSEND=%d",byte_size);
		atk_8266_send_cmd(tem_string,"OK",200);  //����ָ�����ȵ����ݵ�Ԥ��ָ��
		delay_ms(200);
		atk_8266_send_data(data,"OK",100);  //����ָ�����ȵ�����
	}
	else if(mode==1)//TCP Client
	{
		atk_8266_quit_trans();
		atk_8266_send_cmd("AT+CIPSEND","OK",20);       //��ʼ͸��
		u3_printf("%s",data);
	}
	else if(mode==2)//TCP Server
	{
		sprintf((char*)tem_string,"AT+CIPSEND=0,%d",byte_size);
		atk_8266_send_cmd(tem_string,"OK",200);  //����ָ�����ȵ����ݵ�Ԥ��ָ��
		delay_ms(200);
		atk_8266_send_data(data,"OK",100);  //����ָ�����ȵ�����
	}
}
void atk8266_ap_check_connect(void)
{
	if(atk_8266_consta_check()=='+')printf("���ӳɹ�");  //����״̬
	else printf("����ʧ��"); 	  	
}
//atk_8266_at_response��Ϊһ����׼�Ľ���ģ��
/*
if(USART3_RX_STA&0X8000)		//���յ�һ��������
{ 
	rlen=USART3_RX_STA&0X7FFF;	//�õ����ν��յ������ݳ���
	USART3_RX_BUF[rlen]=0;		//��ӽ����� 
	printf("%s",USART3_RX_BUF);	//���͵�����   
		
	USART3_RX_STA=0;
}  
*/

#endif








