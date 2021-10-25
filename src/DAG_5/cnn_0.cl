__kernel void convolution_0( __global const float* b20, __global const float* b21, __global const float* b22, __global float* b23)
{
	int lId = get_local_id(0) ;
	int dp_0= (get_group_id(0)*1 + 1*0) * get_local_size(0) + lId ;


	if(dp_0< 32*32*32)
	{
		typedef float4 floatC;
		floatC data0_C,data1_C;
		int localId_C, w2 , h2, outputRow_C, outputCol_C, image2dIdx_C, plane_C, n_C, stride, pad, inputRow_C, inputCol_C;
		float dots;

		w2 = 32;
		h2 = 32;

		localId_C = dp_0%(w2*h2);
		outputRow_C = localId_C / w2;
		outputCol_C = localId_C % w2;

		image2dIdx_C = dp_0 / (h2*w2);
		plane_C = image2dIdx_C % 32;
		n_C = image2dIdx_C / 32;
		dots = 0.0;
		stride = 1;
		pad=1;
		inputRow_C = outputRow_C * stride - pad;
		inputCol_C = outputCol_C * stride - pad;

		for(int j=0;j<1;j++)
		{
			for(int i=0;i<3;i++)
			{
				if(inputRow_C+i<0 || inputRow_C+i>=32)
					data0_C.xyz = (float3)(0.0f,0.0f,0.0f);
				else if(inputCol_C<0)
				{
					if(inputRow_C+i==0 && j==0)
					{
						data0_C.y = b20[0];
						data0_C.z = b20[1];
					}
					else
						data0_C.xyz = vload3(0,(__global const float *)b20+(j*32*32)+(inputRow_C+i)*32+inputCol_C);
					data0_C.x = 0;
				}
				else if(inputCol_C+3-1>=32)
				{
					if(inputRow_C+i==32 && j==2)
					{
						data0_C.x = b20[(j*32*32)+(inputRow_C+i)*32+2];
						data0_C.y = b20[(j*32*32)+(inputRow_C+i)*32+3];
					}
					else
						data0_C.xyz = vload3(0,(__global const float *)b20+(j*32*32)+(inputRow_C+i)*32+inputCol_C);
					data0_C.z = 0;
				}
				else
				{
					data0_C.xyz = vload3(0,(__global const float *)b20+(j*32*32)+(inputRow_C+i)*32+inputCol_C);
				}
				data0_C.w = 0.0;
				data1_C.xyz = vload3(0,(__global const float *)b21+(plane_C*3*3*1)+(j*3*3)+i*3);
				dots += dot(data0_C,data1_C);
			}
		}
		if(outputRow_C<32 && outputCol_C<32)
			b23[plane_C*32*32+outputRow_C*32+outputCol_C] = dots + b22[plane_C];
	}

}