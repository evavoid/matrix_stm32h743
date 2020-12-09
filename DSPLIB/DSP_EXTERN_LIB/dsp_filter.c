#include "dsp_filter.h"

/*******************************�˲�������������******************************************/
float firStatef32[STATE_LENGTH];//����״̬����
float iirStatef32[STATE_LENGTH];
/*****************************************************************************************/

//static fir_init fir_init_group[10];//��ʼ��������
//static arm_fir_instance_f32 fir_filter_group[10];//�˲���������
/*
first  �����˲����������飬���Ϸ�
second ��ʼ���˲���������..._init����
third  ����..._check
*/
//���ܴ���һ������
void f32_fir_init(
	arm_fir_instance_f32* Spin,
	float*pCoeffs,u32 numTaps,u32 signal_size)
{
	arm_fir_init_f32(Spin,numTaps,(float *)pCoeffs,firStatef32,signal_size);
}

void f32_fir_check(
	arm_fir_instance_f32* Spin,
	float* datain,float* dataout,
	u32 data_length,u32 single_data_length)
{
	u32 i;
	u32 times=data_length/single_data_length;
	for(i=0;i<times;i++)
	{
		arm_fir_f32(Spin,datain+(i * single_data_length), dataout + (i * single_data_length), single_data_length);
	}
}

void f32_iir_init(
	arm_biquad_casd_df1_inst_f32* Spin,
	float*pCoeffs,u32 numStages)
{
	arm_biquad_cascade_df1_init_f32(Spin,numStages,pCoeffs,iirStatef32);
}

void f32_iir_check(
	arm_biquad_casd_df1_inst_f32* Spin,
	float* datain,float* dataout,
	u32 data_length)
{
	arm_biquad_cascade_df1_f32(Spin, datain, dataout, data_length);
}
/********************************************************************************************************/
//�޷��˲�:�˷�żȻ���壬��ƽ���Ȳ���޷����������Ը���
void limit_amp_filter(float* lis,float limit,u32 blocksize)
{
	if(blocksize<3)
	{
		lis[0]=(lis[0]+lis[1]+lis[2])/3;
		lis[1]=lis[0];
		lis[2]=lis[0];
		return;
	}
	for(u32 i=1;i<blocksize-1;i++)//0 1->...->blocksize-2 blocksize-1
	{
		if((fabsf(lis[i]-lis[i-1])>limit)&&(fabsf(lis[i]-lis[i+1])>limit))
			lis[i]=(lis[i+1]+lis[i-1])/2;
	}
}
//��������
void quicksort(float * lis,u32 low,u32 high)
{
    if(low >= high)return;
	
    u32 first = low;
    u32 last = high;
    u32 key = lis[first];
    while(first < last)
		{
        if(first < last && lis[last] >= key) last --;
        lis[first] = lis[last];
        if(first < last && lis[first] <= key) first ++;
        lis[last] = lis[first];
    }
    lis[first] = key;
    quicksort(lis, low, first - 1);
    quicksort(lis, first + 1, high);
}
//��ֵ�˲������Կ˷�żȻ������ɵĲ������Ի����仯���и��õ�Ч�������ʺϿ��ٱ仯��
void mid_value_filter(float* lis,u32 step,u32 blocksize)
{
	if(step>blocksize)return;
	u32 times=blocksize/step;
	float mid=0;
	/////
	for(u32 i=0;i<times;i++)
	{
		quicksort(lis,i*step,(i+1)*step-1);
		mid=lis[i*step+step/2];
		for(u32 w=i*step;w<(i+1)*step;w++)lis[w]=mid;
	}
	/////
	quicksort(lis,times*step,blocksize-1);
	mid=lis[times*step+(blocksize-1-times*step)/2];
	for(u32 w=step*times;w<blocksize;w++)lis[w]=mid;
}
//ƽ��ֵ�˲���step��ʱ��ƽ���ȸߣ��������ȵ�
//It is suitable for filtering general signals with random interference.
//The characteristic of this kind of signal is that it has an average value, and the signal fluctuates up 
//and down near a certain range of values.
//It is not suitable for real-time control with slower measurement speed or faster data calculation speed.
void cal_value_filter(double* lis,u32 step,u32 blocksize)
{
	if(step>blocksize)return;
	u32 times=blocksize/step;
	double value=0;
	/////
	for(u32 i=0;i<times;i++)
	{
		value=0;
		for(u32 w=i*step;w<(i+1)*step;w++)value+=lis[w];
		value/=(double)step;
		
		for(u32 w=i*step;w<(i+1)*step;w++)lis[w]=value;
	}
	/////
	value=0;
	for(u32 i=step*times;i<blocksize;i++)value+=lis[i];
	value/=(double)(blocksize-step*times);
	for(u32 i=step*times;i<blocksize;i++)lis[i]=value;
}
//Median Average Filtering Method
/*
Method:
Take a group of queues to remove the maximum and minimum values and then take the average value.
It is equivalent to "median filtering method"+ "arithmetic average filtering method".
Sampling N data continuously, removing one maximum and one minimum.
Then the arithmetic mean of N-2 data is calculated.
Selection of N value: 3-14.
*/
/*
Advantage:
The merits of the two filtering methods, median filtering method 
and arithmetic average filtering method, are combined.
For the occasional impulsive interference, the sampling 
deviation caused by it can be eliminated.
It has good inhibition effect on periodic interference.
High smoothness, suitable for high frequency oscillation system.
*/
/*
Disadvantages:
The computational speed is slow, which is the same as the arithmetic 
average filtering method.Waste RAM.
*/
void mid_aver_filter(float *lis,u32 step,u32 blocksize)
{
		if(step>blocksize)return;
		u32 times=blocksize/step;
		float value=0;
		
		for(u32 i=0;i<times;i++)
		{
			u32 index0;u32 index1;float temp;
			arm_max_f32(&lis[i*step],step,&temp,&index0);
			arm_min_f32(&lis[i*step],step,&temp,&index1);
			
			for(u32 w=i*step;w<(i+1)*step;w++)
			{
				if(((index0+i*step)==w)||((index1+i*step)==w))continue;
				value+=lis[w];
			}
			value/=(float)(step-2);
			for(u32 w=i*step;w<(i+1)*step;w++)lis[w]=value;
		}
}
//Limited Average Filtering Method
/*
Method:
It is equivalent to "limiting filtering method"+ "recursive average filtering method".
The new data sampled at each time are processed by limiting the amplitude.
Then it is sent to the queue for recursive average filtering.
*/
/*
Advantages:
The advantages of the two filtering methods are combined.
For occasional impulsive interference, the sampling deviation caused by impulsive interference can be eliminated.
*/
/*
Disadvantages:Waste RAM.
*/
void limit_Aver_filtering(float* lis,float limit,u32 blocksize)
{
	
	limit_amp_filter(lis,limit,blocksize);
	//���޷��ٵ����˲�


}



