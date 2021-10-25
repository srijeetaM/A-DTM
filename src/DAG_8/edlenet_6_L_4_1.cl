__kernel void linear_6( __global const float* b6, __global const float* b7, __global float* b8)
{
	int lId = get_local_id(0) ;
	int gId = (lId/8)*8*4 + lId%8 ;

	int dp_0= get_group_id(0)*4*get_local_size(0) + gId + 8*0 ;
	int dp_1= get_group_id(0)*4*get_local_size(0) + gId + 8*1 ;
	int dp_2= get_group_id(0)*4*get_local_size(0) + gId + 8*2 ;
	int dp_3= get_group_id(0)*4*get_local_size(0) + gId + 8*3 ;


	if(dp_0< 128)
	{
		typedef float4 floatX;
		floatX wt,temp;
		float dotProduct;

		dotProduct=0.0;
		for(int x=0; x<2048/4; x++)
		{
			temp= vload4(0,(__global const float *)b6+(4*x));
			wt.x= b7[128*(4*x)+dp_0];
			wt.y= b7[128*((4*x)+1)+dp_0];
			wt.z= b7[128*((4*x)+2)+dp_0];
			wt.w= b7[128*((4*x)+3)+dp_0];
			dotProduct += dot(wt,temp);
		}
b8[dp_0] = dotProduct;
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
			temp= vload4(0,(__global const float *)b6+(4*x));
			wt.x= b7[128*(4*x)+dp_1];
			wt.y= b7[128*((4*x)+1)+dp_1];
			wt.z= b7[128*((4*x)+2)+dp_1];
			wt.w= b7[128*((4*x)+3)+dp_1];
			dotProduct += dot(wt,temp);
		}
b8[dp_1] = dotProduct;
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
			temp= vload4(0,(__global const float *)b6+(4*x));
			wt.x= b7[128*(4*x)+dp_2];
			wt.y= b7[128*((4*x)+1)+dp_2];
			wt.z= b7[128*((4*x)+2)+dp_2];
			wt.w= b7[128*((4*x)+3)+dp_2];
			dotProduct += dot(wt,temp);
		}
b8[dp_2] = dotProduct;
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
			temp= vload4(0,(__global const float *)b6+(4*x));
			wt.x= b7[128*(4*x)+dp_3];
			wt.y= b7[128*((4*x)+1)+dp_3];
			wt.z= b7[128*((4*x)+2)+dp_3];
			wt.w= b7[128*((4*x)+3)+dp_3];
			dotProduct += dot(wt,temp);
		}
b8[dp_3] = dotProduct;
		printf("");
	}

}