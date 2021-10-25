__kernel void linear_6( __global const float* b21, __global const float* b22, __global float* b23)
{
	int lId = get_local_id(0) ;
	int dp_0= (get_group_id(0)*4 + 1*0) * get_local_size(0) + lId ;
	int dp_1= (get_group_id(0)*4 + 1*1) * get_local_size(0) + lId ;
	int dp_2= (get_group_id(0)*4 + 1*2) * get_local_size(0) + lId ;
	int dp_3= (get_group_id(0)*4 + 1*3) * get_local_size(0) + lId ;


	if(dp_0< 128)
	{
		typedef float4 floatX;
		floatX wt,temp;
		float dotProduct;

		dotProduct=0.0;
		for(int x=0; x<2048/4; x++)
		{
			temp= vload4(0,(__global const float *)b21+(4*x));
			wt.x= b22[128*(4*x)+dp_0];
			wt.y= b22[128*((4*x)+1)+dp_0];
			wt.z= b22[128*((4*x)+2)+dp_0];
			wt.w= b22[128*((4*x)+3)+dp_0];
			dotProduct += dot(wt,temp);
		}
b23[dp_0] = dotProduct;
		printf("");
	}

	if(dp_1< 128)
	{
		typedef float4 floatX;
		floatX wt,temp;
		float dotProduct;

		dotProduct=0.0;
		for(int x=0; x<2048/4; x++)
		{
			temp= vload4(0,(__global const float *)b21+(4*x));
			wt.x= b22[128*(4*x)+dp_1];
			wt.y= b22[128*((4*x)+1)+dp_1];
			wt.z= b22[128*((4*x)+2)+dp_1];
			wt.w= b22[128*((4*x)+3)+dp_1];
			dotProduct += dot(wt,temp);
		}
b23[dp_1] = dotProduct;
		printf("");
	}

	if(dp_2< 128)
	{
		typedef float4 floatX;
		floatX wt,temp;
		float dotProduct;

		dotProduct=0.0;
		for(int x=0; x<2048/4; x++)
		{
			temp= vload4(0,(__global const float *)b21+(4*x));
			wt.x= b22[128*(4*x)+dp_2];
			wt.y= b22[128*((4*x)+1)+dp_2];
			wt.z= b22[128*((4*x)+2)+dp_2];
			wt.w= b22[128*((4*x)+3)+dp_2];
			dotProduct += dot(wt,temp);
		}
b23[dp_2] = dotProduct;
		printf("");
	}

	if(dp_3< 128)
	{
		typedef float4 floatX;
		floatX wt,temp;
		float dotProduct;

		dotProduct=0.0;
		for(int x=0; x<2048/4; x++)
		{
			temp= vload4(0,(__global const float *)b21+(4*x));
			wt.x= b22[128*(4*x)+dp_3];
			wt.y= b22[128*((4*x)+1)+dp_3];
			wt.z= b22[128*((4*x)+2)+dp_3];
			wt.w= b22[128*((4*x)+3)+dp_3];
			dotProduct += dot(wt,temp);
		}
b23[dp_3] = dotProduct;
		printf("");
	}

}