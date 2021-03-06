#define v0 2048
#define v1 2048
#define i_max 1

__kernel void dnn_0( __global const float* b0, __global const float* b1, __global const float* b2, __global float* b3)
{
	int globalId = get_global_id(0);

	typedef float4 floatX;
	floatX wt,temp;
	float dotProduct;

	for(int i=0; i<i_max; i++)
	{
		dotProduct=0.0;
		if(globalId < v1)
		{
		#pragma unroll
			for(int x=0; x<v0/4; x++)
			{
				temp= vload4(0,(__global const float *)b0+(4*x));
				wt= vload4(0,(__global const float *)b1+(globalId*v0+4*x));
				dotProduct += dot(wt,temp);
			}
			b3[globalId] = dotProduct+b2[globalId];
		}
	}

}