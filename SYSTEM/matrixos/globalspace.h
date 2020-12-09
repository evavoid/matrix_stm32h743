#ifndef __GLOBALSPACE_H
#define __GLOBALSPACE_H
#include "sys.h"
/*******************************************************************************************************
describ:matrixos_data_struction
function:keep the state machine and many supported packets run
other:***
********************************************************************************************************/
/***״̬��*****/
typedef enum{
	sleep_state0=1,sleep_state1=2,sleep_state2=3,sleep_state3=4,
	
	prepare_state=5,dead_state=6,error_state=17,wrong_state=18,
	
	run_state0=7,run_state1=8,run_state2=9,run_state3=10,
	run_state4=11,run_state5=12,run_state6=13,
	
	test_state0=14,test_state1=15,test_state2=16,
}state_space;
//sleep_state,�������������߶��ٸ�ʱ��Ƭ�����ѣ����ߵȴ��ⲿ�źŻ��ѣ�������״̬��
//ֻ�����ⲿ��״̬�޸�Ϊ׼��״̬�����߶�������֮���Զ���״̬�滻Ϊ׼��״̬
//prepare_state,׼��״̬����״̬�¼���ⲿ�ź��ж��Ƿ��������״̬������ȥ����
//������ֱ�ӷ��أ���������Ҫ�����������������Դ�ĳ�ʼ�����ã���������ʼ�ĳ�ʼ������
//���ģ��Ӧ������run_state0��
//run_state,�ж��Ƿ������һ���׶λ��߻��˵���һ��״̬��Ҫע����ǣ���������붼��Ҫ
//��Դ�����·��䣬�������ֱ�ӽ�������ģʽ����ָ��ʱ�����������
//dead_state,�����ģʽ����̱߳�����������������ǰӦ���������ź�
/****�ڴ������������ݿռ䶨���*****/
typedef struct 
{
	u8 global_interpreter_flag;
	u32 global_test_time;
	
	state_space global_state;
	
	u32 data[15];
	float float_data[10];
	double double_data[10];
	char string[50];//��ʽ���ַ���������sprintf���ٵ���lcd��string��ʾ����
	
	void * global_data_address[5];//ָ�����ݵ�ַ��ָ��//��Σ�գ������򲻵��ѵ��������ʹ��//������һ��Ҫ֪����ŵ����ݵ���ʼ����
	/*example//���ɴ洢ջ�е�ַ�����Ǻ����е����ݣ�������static�ؼ���
	u8 test[3]={1,2,3};
	global_data_block.data_address[0]=&test;//����ĵ�һλ�����һ��u8������׵�ַ
	/////////////////////////////////////
	u8* address;
	address=(u8*)global_data_block.data_address[0];//����ַ��ȡ����
	u8 num=0;
	num=*((u8*)global_data_block.data_address[0]);//ȡ����һ����
	num=*((u8*)global_data_block.data_address[0]+1);//�ڶ���
	num=*((u8*)global_data_block.data_address[0]+2);//��������ָ����ȡ������ת��Ϊ��Ӧ���ͣ�u8*���ģ��ټ���ƫ�ƣ���ȡ����
	*/
}global_data_block;//��̬������������ָ�����飬��ĳ״̬�ھ�̬�ֲ������ĵ�ַ�Ǽ��ڱ����������ݷ���
extern global_data_block global_data;	
					
//��־λ����������Ƿ�������׼���ô��������ݺ󣬸�֪�������������ĳ��״̬����������
//������־λ�߲���ʽ�������־λ��ֻ�н��շ����պ�ſ�
//���շ���Ӧ�ٶ�Ӧ���ڷ��ͷ�
//���ٶ����ʣ���־λֻ���ɽ��շ���ʽ���
////////////////////////////////////////////////////////////////////////
typedef struct
{
	/*****״̬���Ʊ���*****************/
  state_space ready_state;//ͳһ���ƣ�ÿһ��״̬��ĩβ��ʱ���޸���ֵ
	state_space sleep_next_state;//˯�߽������ָ����ת״̬���ñ����ɿ���˯�ߵ�״̬//����д�룬��δд���򴥷�����״̬
	
	u32 sleep_time;
	/*****���ʿ��Ʊ���****************/
	void *local_data_address[3];
	/***�������Ʊ���******************/
	u32 data[10];
	float float_data[5];
	double double_data[5];
	/**************����***************/
}local_state_space; //�ṹ������LED_state_space
/*
void xxx_event(local_state_space *XXX_state)
{
	if(XXX_state->ready_state==prepare_state)
	{
		XXX_Init();
		XXX_state->ready_state=run_state0;
		return;
	}
	else if(XXX_state->ready_state==run_state0)
	{
		XXX_state->sleep_next_state=run_state0;
		XXX_state->sleep_time=10;
		XXX_state->ready_state=sleep_state0;
		return;
	}
	else if(XXX_state->ready_state==sleep_state0)
	{
		XXX_state->sleep_time--;
		if(XXX_state->sleep_time==0)
			XXX_state->ready_state=XXX_state->sleep_next_state;
	}
}
void xxx_event(local_state_space *XXX_state);
*/

/*******************************************************************************************************
describ:cycle_fifo
function:just a cycle_fifo
other:***
********************************************************************************************************/
/*
typedef struct
{
	void* list;
	int front;
	int rear;
	int length;
}matrix_queue;
*/







#endif

