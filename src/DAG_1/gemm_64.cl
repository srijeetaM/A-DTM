/**
 * gemm.cl: This file is part of the PolyBench/GPU 1.0 test suite.
 *
 *
 * Contact: Scott Grauer-Gray <sgrauerg@gmail.com>
 * Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://www.cse.ohio-state.edu/~pouchet/software/polybench/GPU
 */

#if defined(cl_khr_fp64)  // Khronos extension available?
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#elif defined(cl_amd_fp64)  // AMD extension available?
#pragma OPENCL EXTENSION cl_amd_fp64 : enable
#endif

typedef float DATA_TYPE;


#define i_max 300
#define ni 64
#define nj 64
#define nk 64

__kernel void gemm(__global DATA_TYPE *a, __global DATA_TYPE *b, __global DATA_TYPE *c)
{
  	int j = get_global_id(1);
	int i = get_global_id(0);

	if ((i < ni) && (j < nj))
	{
		c[i * nj + j] = 0.0f;

	    int k;
		for(k=0; k < nk; k++)
		{
      		c[i * nj + j] += a[i * nk + k] * b[k * nj +j];
		}
	}
 }

