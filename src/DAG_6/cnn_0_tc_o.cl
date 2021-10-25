#define CF 2
#define SL 8
__kernel void cnn_0( __global const float* b0, __global const float* b1, __global const float* b2, __global float* b3)
{

	int lId = get_local_id(0);
	int tid_0=(lId/SL)*SL*CF+lId%SL;
	int dp_0= get_group_id(0)*CF*get_local_size(0) + tid_0;
	int dp_1= get_group_id(0)*CF*get_local_size(0) + tid_0+SL;


	typedef float3 floatC;
	floatC data0_C_0,data1_C_0,data0_C_1,data1_C_1;
	int w2,h2,stride,pad;
	float dots;
	w2 = 32;
	h2 = 32;
	stride = 1;
	pad=1;

	if(dp_0 < 16*32*32)
	{
		dots = 0.0;
		
		int localId_C_0, outputRow_C_0, outputCol_C_0, image2dIdx_C_0, plane_C_0, n_C_0, inputRow_C_0, inputCol_C_0;
	
		localId_C_0 = dp_0%(w2*h2);
		outputRow_C_0 = localId_C_0 / w2;
		outputCol_C_0 = localId_C_0 % w2;

		image2dIdx_C_0 = dp_0 / (h2*w2);
		plane_C_0 = image2dIdx_C_0 % 16;
		n_C_0 = image2dIdx_C_0 / 16;
		
		inputRow_C_0 = outputRow_C_0 * stride - pad;
		inputCol_C_0 = outputCol_C_0 * stride - pad;

		if(localId_C_0 <1024)
		{
			for(int j=0;j<3;j++)
			{
				for(int i=0;i<3;i++)
				{
					// data0_C.xyz = (float3)(115.0f,212.0f,76.0f);
					if(inputRow_C_0+i<0 || inputRow_C_0+i>=32)
						data0_C_0.xyz = (float3)(0.0f,0.0f,0.0f);
					else if(inputCol_C_0<0)
					{
						data0_C_0 = vload3(0,(__global const float *)b0+(j*32*32)+(inputRow_C_0+i)*32+inputCol_C_0);
						data0_C_0.x = 0;
					}
					else if(inputCol_C_0+3-1>=32)
					{
						data0_C_0 = vload3(0,(__global const float *)b0+(j*32*32)+(inputRow_C_0+i)*32+inputCol_C_0);
						data0_C_0.z = 0;
					}
					else
					{
						data0_C_0 = vload3(0,(__global const float *)b0+(j*32*32)+(inputRow_C_0+i)*32+inputCol_C_0);
					}
					data1_C_0 = vload3(0,(__global const float *)b1+(plane_C_0*3*3*3)+(j*3*3)+i*3);
					dots += dot(data0_C_0,data1_C_0);
				}
			}
			if(outputRow_C_0<32 && outputCol_C_0<32)
				b3[plane_C_0*32*32+outputRow_C_0*32+outputCol_C_0] = dots + b2[plane_C_0];
		}
		printf("");
	}

	if(dp_1 < 16*32*32)
	{
		dots = 0.0;
		
		int localId_C_1, outputRow_C_1, outputCol_C_1, image2dIdx_C_1, plane_C_1, n_C_1, inputRow_C_1, inputCol_C_1;
	
		localId_C_1 = dp_1%(w2*h2);
		outputRow_C_1 = localId_C_1 / w2;
		outputCol_C_1 = localId_C_1 % w2;

		image2dIdx_C_1 = dp_1 / (h2*w2);
		plane_C_1 = image2dIdx_C_1 % 16;
		n_C_1 = image2dIdx_C_1 / 16;
	
		inputRow_C_1 = outputRow_C_1 * stride - pad;
		inputCol_C_1 = outputCol_C_1 * stride - pad;

		if(localId_C_1 <1024)
		{
			for(int j=0;j<3;j++)
			{
				for(int i=0;i<3;i++)
				{
					if(inputRow_C_1+i<0 || inputRow_C_1+i>=32)
						data0_C_1.xyz = (float3)(0.0f,0.0f,0.0f);
					else if(inputCol_C_1<0)
					{
						data0_C_1 = vload3(0,(__global const float *)b0+(j*32*32)+(inputRow_C_1+i)*32+inputCol_C_1);
						data0_C_1.x = 0;
					}
					else if(inputCol_C_1+3-1>=32)
					{
						data0_C_1 = vload3(0,(__global const float *)b0+(j*32*32)+(inputRow_C_1+i)*32+inputCol_C_1);
						data0_C_1.z = 0;
					}
					else
					{
						data0_C_1 = vload3(0,(__global const float *)b0+(j*32*32)+(inputRow_C_1+i)*32+inputCol_C_1);
					}
					data1_C_1 = vload3(0,(__global const float *)b1+(plane_C_1*3*3*3)+(j*3*3)+i*3);
					dots += dot(data0_C_1,data1_C_1);

				}
			}
			if(outputRow_C_1<32 && outputCol_C_1<32)
				b3[plane_C_1*32*32+outputRow_C_1*32+outputCol_C_1] = dots + b2[plane_C_1];
		}
		printf("");
	}
}
