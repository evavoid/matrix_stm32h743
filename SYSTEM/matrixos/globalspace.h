#ifndef __GLOBALSPACE_H
#define __GLOBALSPACE_H
#include "sys.h"
/*******************************************************************************************************
describ:matrixos_data_struction
function:keep the state machine and many supported packets run
other:***
********************************************************************************************************/
/***状态块*****/
typedef enum{
	sleep_state0=1,sleep_state1=2,sleep_state2=3,sleep_state3=4,
	
	prepare_state=5,dead_state=6,error_state=17,wrong_state=18,
	
	run_state0=7,run_state1=8,run_state2=9,run_state3=10,
	run_state4=11,run_state5=12,run_state6=13,
	
	test_state0=14,test_state1=15,test_state2=16,
}state_space;
//sleep_state,开启读数，休眠多少个时间片后苏醒，或者等待外部信号唤醒，即休眠状态下
//只能由外部将状态修改为准备状态，或者读数读够之后自动将状态替换为准备状态
//prepare_state,准备状态，此状态下检测外部信号判断是否进入运行状态，是则去运行
//不是则直接返回，若是则需要进行运行所需相关资源的初始化配置，此外就是最开始的初始化部分
//检测模块应当丢在run_state0里
//run_state,判断是否进入下一个阶段或者回退到上一个状态，要注意的是，回退与进入都需要
//资源的重新分配，此外可以直接进入休眠模式，需指定时间或永久休眠
//dead_state,进入该模式后该线程被死锁，并且在死锁前应发出错误信号
/****内存管理块与主数据空间定义块*****/
typedef struct 
{
	u8 global_interpreter_flag;
	u32 global_test_time;
	
	state_space global_state;
	
	u32 data[15];
	float float_data[10];
	double double_data[10];
	char string[50];//格式化字符串，利用sprintf，再调用lcd的string显示函数
	
	void * global_data_address[5];//指向数据地址的指针//很危险，请在万不得已的情况下来使用//或者是一定要知道存放的数据的起始长度
	/*example//不可存储栈中地址，若是函数中的数据，需声明static关键字
	u8 test[3]={1,2,3};
	global_data_block.data_address[0]=&test;//数组的第一位存放了一个u8数组的首地址
	/////////////////////////////////////
	u8* address;
	address=(u8*)global_data_block.data_address[0];//将地址提取出来
	u8 num=0;
	num=*((u8*)global_data_block.data_address[0]);//取出第一个数
	num=*((u8*)global_data_block.data_address[0]+1);//第二个
	num=*((u8*)global_data_block.data_address[0]+2);//第三个，指针提取出来后转化为对应类型（u8*）的，再加上偏移，再取索引
	*/
}global_data_block;//动态调整或者设置指针数组，将某状态内静态局部变量的地址登记在表中来做数据发送
extern global_data_block global_data;	
					
//标志位最大的意义就是发送者在准备好待处理数据后，告知接收者让其进行某种状态来处理数据
//发生标志位者不显式地清除标志位，只有接收方接收后才可
//接收方响应速度应大于发送方
//减少丢包率，标志位只能由接收方显式清除
////////////////////////////////////////////////////////////////////////
typedef struct
{
	/*****状态控制变量*****************/
  state_space ready_state;//统一名称，每一个状态在末尾的时候修改其值
	state_space sleep_next_state;//睡眠结束后的指定跳转状态，该变量由开启睡眠的状态//负责写入，若未写入则触发死锁状态
	
	u32 sleep_time;
	/*****访问控制变量****************/
	void *local_data_address[3];
	/***操作控制变量******************/
	u32 data[10];
	float float_data[5];
	double double_data[5];
	/**************数据***************/
}local_state_space; //结构体名是LED_state_space
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

