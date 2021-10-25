__kernel void pooling_1( __global const float* b4, __global float* b5)
{
	int lId = get_local_id(0) ;
	int gId = (lId/8)*8*4 + lId%8 ;

	int dp_0= get_group_id(0)*4*get_local_size(0) + gId + 8*0 ;
	int dp_1= get_group_id(0)*4*get_local_size(0) + gId + 8*1 ;
	int dp_2= get_group_id(0)*4*get_local_size(0) + gId + 8*2 ;
	int dp_3= get_group_id(0)*4*get_local_size(0) + gId + 8*3 ;


	if(dp_0< 32*16*16)
	{
		typedef float2 floatP;
		floatP data0_P,data1_P;

		int localId_P, outputRow_P, outputCol_P, image2dIdx_P, plane_P, n_P, pool_size, inputRow_P, inputCol_P, inputImageOffset, poolInputOffset;
		float maxValue;
		bool process;

		localId_P = dp_0 % (16*16);
		outputRow_P = localId_P / 16;
		outputCol_P = localId_P % 16;
		image2dIdx_P = dp_0 / (16*16);
		plane_P = image2dIdx_P % 32;
		n_P = image2dIdx_P / 32;
		if(n_P > 1) 
			return;
		
		pool_size = 2;
		inputRow_P = outputRow_P * pool_size;
		inputCol_P = outputCol_P * pool_size;
		inputImageOffset  = (n_P*32 + plane_P)*32*32;
		poolInputOffset = inputImageOffset + inputRow_P * 32 + inputCol_P;
		maxValue = b4[ poolInputOffset ];

		process = (inputRow_P + 1 < 32) && (inputCol_P + 1 < 32);

		if(process)
		{
			data0_P = vload2(0,(__global const float *)b4+poolInputOffset);
			data1_P = vload2(0,(__global const float *)b4+poolInputOffset+32);

			data0_P = fmax(data0_P,data1_P);
			maxValue = fmax(data0_P.s0,data0_P.s1);
		}

		if(outputRow_P<16 && outputCol_P<16)
			b5[dp_0] = maxValue;

		printf("");
	}
	if(dp_1< 32*16*16)
	{
		typedef float2 floatP;
		floatP data0_P,data1_P;

		int localId_P, outputRow_P, outputCol_P, image2dIdx_P, plane_P, n_P, pool_size, inputRow_P, inputCol_P, inputImageOffset, poolInputOffset;
		float maxValue;
		bool process;

		localId_P = dp_1 % (16*16);
		outputRow_P = localId_P / 16;
		outputCol_P = localId_P % 16;
		image2dIdx_P = dp_1 / (16*16);
		plane_P = image2dIdx_P % 32;
		n_P = image2dIdx_P / 32;
		if(n_P > 1) 
			return;
		
		pool_size = 2;
		inputRow_P = outputRow_P * pool_size;
		inputCol_P = outputCol_P * pool_size;
		inputImageOffset  = (n_P*32 + plane_P)*32*32;
		poolInputOffset = inputImageOffset + inputRow_P * 32 + inputCol_P;
		maxValue = b4[ poolInputOffset ];

		process = (inputRow_P + 1 < 32) && (inputCol_P + 1 < 32);

		if(process)
		{
			data0_P = vload2(0,(__global const float *)b4+poolInputOffset);
			data1_P = vload2(0,(__global const float *)b4+poolInputOffset+32);

			data0_P = fmax(data0_P,data1_P);
			maxValue = fmax(data0_P.s0,data0_P.s1);
		}

		if(outputRow_P<16 && outputCol_P<16)
			b5[dp_1] = maxValue;

		printf("");
	}
	if(dp_2< 32*16*16)
	{
		typedef float2 floatP;
		floatP data0_P,data1_P;

		int localId_P, outputRow_P, outputCol_P, image2dIdx_P, plane_P, n_P, pool_size, inputRow_P, inputCol_P, inputImageOffset, poolInputOffset;
		float maxValue;
		bool process;

		localId_P = dp_2 % (16*16);
		outputRow_P = localId_P / 16;
		outputCol_P = localId_P % 16;
		image2dIdx_P = dp_2 / (16*16);
		plane_P = image2dIdx_P % 32;
		n_P = image2dIdx_P / 32;
		if(n_P > 1) 
			return;
		
		pool_size = 2;
		inputRow_P = outputRow_P * pool_size;
		inputCol_P = outputCol_P * pool_size;
		inputImageOffset  = (n_P*32 + plane_P)*32*32;
		poolInputOffset = inputImageOffset + inputRow_P * 32 + inputCol_P;
		maxValue = b4[ poolInputOffset ];

		process = (inputRow_P + 1 < 32) && (inputCol_P + 1 < 32);

		if(process)
		{
			data0_P = vload2(0,(__global const float *)b4+poolInputOffset);
			data1_P = vload2(0,(__global const float *)b4+poolInputOffset+32);

			data0_P = fmax(data0_P,data1_P);
			maxValue = fmax(data0_P.s0,data0_P.s1);
		}

		if(outputRow_P<16 && outputCol_P<16)
			b5[dp_2] = maxValue;

		printf("");
	}
	if(dp_3< 32*16*16)
	{
		typedef float2 floatP;
		floatP data0_P,data1_P;

		int localId_P, outputRow_P, outputCol_P, image2dIdx_P, plane_P, n_P, pool_size, inputRow_P, inputCol_P, inputImageOffset, poolInputOffset;
		float maxValue;
		bool process;

		localId_P = dp_3 % (16*16);
		outputRow_P = localId_P / 16;
		outputCol_P = localId_P % 16;
		image2dIdx_P = dp_3 / (16*16);
		plane_P = image2dIdx_P % 32;
		n_P = image2dIdx_P / 32;
		if(n_P > 1) 
			return;
		
		pool_size = 2;
		inputRow_P = outputRow_P * pool_size;
		inputCol_P = outputCol_P * pool_size;
		inputImageOffset  = (n_P*32 + plane_P)*32*32;
		poolInputOffset = inputImageOffset + inputRow_P * 32 + inputCol_P;
		maxValue = b4[ poolInputOffset ];

		process = (inputRow_P + 1 < 32) && (inputCol_P + 1 < 32);

		if(process)
		{
			data0_P = vload2(0,(__global const float *)b4+poolInputOffset);
			data1_P = vload2(0,(__global const float *)b4+poolInputOffset+32);

			data0_P = fmax(data0_P,data1_P);
			maxValue = fmax(data0_P.s0,data0_P.s1);
		}

		if(outputRow_P<16 && outputCol_P<16)
			b5[dp_3] = maxValue;

		printf("");
	}
}