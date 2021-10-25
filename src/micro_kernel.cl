
__kernel void micro_kernel(__global float *A, __global float *B, __global float *C) 
{    
	int j = get_global_id(0);
	int i = get_global_id(1);

	int ni,nj,nk;
	ni=1024;
	nj=1024;
	nk=1024;
	
	if ((i < ni) && (j < nj))
	{ 
		int k;
		for (k = 0; k < nk; k++)
		{
			C[i * nj + j] += A[i * nk + k] * B[k * nj + j];
		}
	}
}

/*
#define var 8192
#define i_max 100
#define j_max 100

__kernel void micro_kernel(__global int* in_data_1, __global int* in_data_2, __global int* out_data)
{
	int tid = get_global_id(0);	
	int data_tmp = 0;
	int data_1,data_2;

	if(tid < var)
	{
		for(int i=0; i<i_max; i++)
		{
			//Step 1: read data (memory related)
	 		data_1 = in_data_1[tid];
			data_2 = in_data_2[tid];
		
		 	//Step 2: do calculation (memory unrelated)
			for(int j=0; j<j_max; j++)
			{
				data_tmp += j;
				data_tmp = data_tmp * data_tmp;
			}
		
			//Step 3:  write back (memory related)
			out_data[tid] = data_1 + data_2*data_tmp;
		
		}     
	}  
    
} 
*/