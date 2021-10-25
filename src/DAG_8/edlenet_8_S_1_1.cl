__kernel void softmax_8( __global const float* b10, __global float* b11)
{
	int lId = get_local_id(0) ;
	int dp_0= (get_group_id(0)*1 + 1*0) * get_local_size(0) + lId ;


	if(dp_0 == 0)
	{
		int class;
		float sum, max;

		class=16;
		sum = 0.0;
		max = b10[0];

		for(int i=0;i<class;i++)
			max = (max > b10[i]) ? max : b10[i];

		for(int i=0;i<class;i++)
			b11[i] = exp((b10[i] - max));

		for(int i=0;i<class;i++)
			sum+=b11[i];

		for(int i=0;i<class;i++)
			b11[i] = b11[i]/sum;

		printf("");
	}

}