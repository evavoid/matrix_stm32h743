#ifndef __MATRIXCORE_H
#define __MATRIXCORE_H
#include "sys.h"
#include "delay.h"
#include "globalspace.h"
#include "value_map_table.h"
/********************************��ʱ��******************************************/
//��ͷ�ļ���Ҫ�������ڸ���event�ļ����Լ�main�ļ��ͬʱ���ļ�ҲҪ����"sys.h" 
//��������������ͬ����Ҫ����.c����ʹ�õ��Ŀ�,�������.c���漴��,hֻ�����ر���
#define SYSTEM_TIME_MS 5
#define USE_FULL_ASSERT 1

extern TIM_HandleTypeDef TIM5_Handler;
extern TIM_HandleTypeDef TIM3_Handler;      //???3PWM?? 
extern TIM_HandleTypeDef TIM4_Handler;

void SYSTEM_timer_Init(u16 timerange_ms);//���ֻ���䵽15λ��������������
void TEST_timer_Init(void);
void HIGH_precision_timer_Init(u16 timerange_us);

void test_end(void);
void test_start(void);
/**********************************************************************************/
u32 BITS_1(char* string,u8 size);
u32 BITS_0(char* string,u8 size);
/**********************************************************************************/
void _Error_Handler(uint8_t* file, uint32_t line,char * string,uint8_t loopen);
void _unsigned_assert(uint8_t* file, uint32_t line,u32 value,char *name);
void _float_assert(uint8_t* file, uint32_t line,float value,char *name);
void _double_assert(uint8_t* file, uint32_t line,double value,char *name);

#define ERROR_HANDLER(__STRING__,__LOOPEN__) _Error_Handler(__FILE__,__LINE__,__STRING__,__LOOPEN__)

#ifdef  USE_FULL_ASSERT
	#define U32_ASSERT(__VALUE__,__NAME__) _unsigned_assert(__FILE__,__LINE__,__VALUE__,__NAME__)
	#define F32_ASSERT(__VALUE__,__NAME__) _float_assert(__FILE__,__LINE__,__VALUE__,__NAME__)
	#define F64_ASSERT(__VALUE__,__NAME__) _double_assert(__FILE__,__LINE__,__VALUE__,__NAME__)
#else 
	#define U32_ASSERT(_,_) 
	#define F32_ASSERT(_,_)
	#define F64_ASSERT(_,_)
#endif 

/* ����ȫ���жϵĺ� */
#define ENABLE_INT()	__set_PRIMASK(0)	/* ʹ��ȫ���ж� */
#define DISABLE_INT()	__set_PRIMASK(1)	/* ��ֹȫ���ж� */
/**********************************************************************************/
void printf_list(double * list,u32 block_size);
#endif

