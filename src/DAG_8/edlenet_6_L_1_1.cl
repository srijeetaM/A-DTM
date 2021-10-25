__kernel void linear_6( __global const float* b15, __global const float* b16, __global float* b17)
{
	int lId = get_local_id(0) ;
	int dp_0= (get_group_id(0)*1 + 1*0) * get_local_size(0) + lId ;


	if(dp_0< 128)
	{
		typedef float4 floatX;
		floatX wt,temp;
		float dotProduct;

		dotProduct=0.0;
		for(int x=0; x<2048/4; x++)
		{
			temp= vload4(0,(__global const float *)b15+(4*x));
			wt.x= b16[128*(4*x)+dp_0];
			wt.y= b16[128*((4*x)+1)+dp_0];
			wt.z= b16[128*((4*x)+2)+dp_0];
			wt.w= b16[128*((4*x)+3)+dp_0];
			dotProduct += dot(wt,temp);
		}
b17[dp_0] = dotProduct;
		printf("");
	}

}