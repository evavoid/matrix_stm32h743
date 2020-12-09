#include "basic_calculation.h"
void matlab_conv(float *lis1,float *lis2,u32 blocksize1,u32 blocksize2,float *output)
//output-->size==blocksize1+blocksize2-1
{
  u32 k,w=0;
  u32 output_size=blocksize1+blocksize2;
  if(blocksize1==0||blocksize2==0){}else output_size--;
  
  if ((blocksize1 > 0) && (blocksize2 > 0)) 
  {
    if (blocksize1 > blocksize2) 
		{
      for (w = 0; w + 1 <= blocksize1; w++) 
			{
        for (k = 0; k < blocksize2; k++)output[w + k] += lis1[w]*lis2[k];
      }
    } 
		else
		{
			for (w = 0; w + 1 <= blocksize2; w++) 
			{
				for (k = 0; k < blocksize1; k++)output[w + k] += lis2[w] * lis1[k];
			}
		}
	}
}
void decimation(float*input_data,float *output_data,u32 de_step,u32 indata_len)
//output-->size==int(indata_len/de_step)
{
	u32 out_data=0;
	for(out_data=0;out_data<indata_len/de_step;out_data++)
	{
		output_data[out_data]=input_data[de_step*out_data];
	}
}
//接收数据的定浮转换(表面格式u8,实质格式int32,转化为double,第一个字为0,直接截掉)
void DZKX_fixed_to_double(uint8_t *data_fixed, double *data_float, uint32_t data_size, uint8_t point_position)
{
	for(uint32_t i=0; i<data_size; i++)
		data_float[i] = ((int32_t)(((uint32_t)data_fixed[4*(i+1)]<<24)+((uint32_t)data_fixed[4*(i+1)+1]<<16)+\
										((uint32_t)data_fixed[4*(i+1)+2]<<8)+(uint32_t)data_fixed[4*(i+1)+3]))/pow(2,point_position);
}

//发送数据的浮定转换(原格式double,转化为u8,实质格式u32)
void DZKX_double_to_fixed(double *data_float, uint8_t *data_fixed, uint32_t data_size, uint8_t point_position)
{
	uint32_t int_temp;
	uint32_t dec_temp, trans_temp;
	
	for(uint32_t i=0; i<data_size; i++)
	{
		int_temp = (uint32_t)(int32_t)data_float[i];
		dec_temp = (uint32_t)(int32_t)((data_float[i]-int_temp)*pow(2,point_position));
		trans_temp = (0x80000000&int_temp)|(0x7fffffff&(0xffffffff<<point_position)&(int_temp<<point_position))|\
								 (!(0xffffffff<<point_position)&(dec_temp>>(32-point_position)));
		data_fixed[4*i] = (uint8_t)(trans_temp>>24);
		data_fixed[4*i+1] = (uint8_t)(0x00ff&(trans_temp>>16));
		data_fixed[4*i+2] = (uint8_t)(0x0000ff&(trans_temp>>8));
		data_fixed[4*i+3] = (uint8_t)(0x000000ff&trans_temp);
	}
}

void new_quick_sort(float*list,u32 blocksize)
{
	long stack[50];//栈
	long top=0;//栈指针
	stack[top++]=0;stack[top++]=blocksize-1;

	while(top!=0)
	{
		long right=stack[top-1];top--;
		long left=stack[top-1];top--;
		long standard=list[right];//基准数
		long point=left-1;//预备返还值
		for(long i=left;i<right;i++)
		{
			if(list[i]<=standard)
			{
				point++;
				change_value(&list[point],&list[i]);
			}
		}
		change_value(&list[right],&list[point+1]);
		if(left<=point){stack[top++]=left;stack[top++]=point;}
		if(point+2<=right){stack[top++]=point+2;stack[top++]=right;}
	}
}
void change_value(float *a,float *b)
{
	float temp=*a;
	*a=*b;
	*b=temp;
}
void max_min_f32(float *list,u32 blocksize,u32 *min_index,u32 *max_index)
{
	if(blocksize<2){*min_index=0;*max_index=0;return;}
	if(blocksize==2)
	{
		*min_index=(list[0]>list[1])?1:0;
		*max_index=(list[0]>list[1])?0:1;return;
	}
	
	float max,min;u32 max_i,min_i;
	if(list[0]>list[1]){max=list[0];min=list[1];max_i=0;min_i=1;}
	else{max=list[1];min=list[0];max_i=1;min_i=0;}
	
	for(u32 i=1;i<blocksize/2;i++)
	{
		if(list[2*i]>list[2*i+1])
		{
			if(list[2*i]>max){max=list[2*i];max_i=2*i;}
			if(list[2*i+1]<min){min=list[2*i+1];min_i=2*i+1;}
		}
		else
		{
			if(list[2*i+1]>max){max=list[2*i+1];max_i=2*i+1;}
			if(list[2*i]<min){min=list[2*i];min_i=2*i;}
		}
	}
	
	if(list[blocksize-1]>max)max_i=blocksize-1;
	else if(list[blocksize-1]<min)min_i=blocksize-1;
	*min_index=min_i;
	*max_index=max_i;
}
//??????,?????,???????max_i???????? 
void max_choose(float*list,u32 blocksize,u32 max_th,u32* max_i)
{
	long stack[50];
	long top=0;
	stack[top++]=0;stack[top++]=blocksize-1;
	//
	float standard,temp;
	long i,point,right,left;
	//
	while(top!=0)
	{
		right=stack[top-1];top--;
		left=stack[top-1];top--;
		standard=list[right];
		point=left-1;
		for(i=left;i<right;i++)
		{
			if(list[i]<=standard)
			{
				point++;
				temp=list[point];
				list[point]=list[i];
				list[i]=temp;	
			}
		}
		temp=list[right];
		list[right]=list[point+1];
		list[point+1]=temp;		
		
		if(max_th==blocksize-point-1){*max_i=point+1;return;}
		else if(max_th>blocksize-point-1)
		{
			if(left<=point)
			{
				stack[top++]=left;
				stack[top++]=point;
			}
		}
		else
		{
			if(point+2<=right)
			{
				stack[top++]=point+2;
				stack[top++]=right;
			}	
		}
	}
}
//从一个序列里面选出第min_th小的元素，并返回下标min_i，list会被打乱，返回的下标是针对打乱后的数组的
void min_choose(float*list,u32 blocksize,u32 min_th,u32* min_i)
{
	long stack[50];
	long top=0;
	stack[top++]=0;stack[top++]=blocksize-1;
	//
	float standard,temp;
	long i,point,right,left;
	//
	while(top!=0)
	{
		right=stack[top-1];top--;
		left=stack[top-1];top--;
		standard=list[right];
		point=left-1;
		for(i=left;i<right;i++)
		{
			if(list[i]<=standard)
			{
				point++;
				temp=list[point];
				list[point]=list[i];
				list[i]=temp;	
			}
		}
		temp=list[right];
		list[right]=list[point+1];
		list[point+1]=temp;		
		
		if(min_th==point+2){*min_i=point+1;return;}
		else if(min_th>point+2)
		{
			if(point+2<=right)
			{
				stack[top++]=point+2;
				stack[top++]=right;
			}	
		}
		else
		{
			if(left<=point)
			{
				stack[top++]=left;
				stack[top++]=point;
			}
		}
	}
}
