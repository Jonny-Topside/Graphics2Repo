cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

struct GSOutput
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
};

[maxvertexcount(3)]
void main(point float4 input[1] : SV_POSITION, point float2 uvInput[1] : TEXCOORD0, inout TriangleStream< GSOutput > output)
{
	GSOutput pointOne;
	GSOutput pointTwo;
	GSOutput pointThree;

	pointOne.pos = float4(0, 0, 0, 0);
	pointOne.uv = float2(0, 0);
	pointOne.normal = float3(0, 0, 0);
	pointTwo = pointThree = pointOne;

	pointOne.pos = input[0];// -1;

	//pointOne.pos.x = input[0].x - 1;
	//pointOne.pos.y = input[0].y - 1;
	//pointOne.pos.z = input[0].z - 1;
	//pointOne.pos.w = input[0].w;

	//pointOne.pos.x -= 3;
	pointOne.uv = uvInput[0];
	if (pointOne.uv.x >= 0.2f && pointOne.uv.x < 0.35f && pointOne.uv.y >= 0.1 && pointOne.uv.y < 0.25f)
	{
		pointOne.uv.x = -1;
		pointOne.uv.y = -1;
	}
	pointTwo.pos.x = pointOne.pos.x + 10;
	pointTwo.pos.y = pointOne.pos.y - 10;
	pointThree.pos.z = pointTwo.pos.z = pointOne.pos.z + 10;
	pointThree.pos.x = pointOne.pos.x - 10;
	pointThree.pos.y = pointOne.pos.y - 10;
	pointTwo.uv = pointOne.uv;
	pointThree.uv = pointOne.uv;
	pointTwo.normal = pointOne.normal;
	pointThree.normal = pointOne.normal;

	//GSOutput element = nullptr;
	//element.pos = input[i];
	//output.Append(element);
	output.Append(pointOne);
	output.Append(pointTwo);
	output.Append(pointThree);

	output.RestartStrip();
	//
	//pointOne.pos = input[0];
	//pointOne.pos.x += 3;
	//pointOne.uv = uvInput[0];
	

	
	//pointTwo.pos.x = pointOne.pos.x + 100;
	//pointTwo.pos.y = pointOne.pos.y - 100;
	//pointThree.pos.z = pointTwo.pos.z = pointOne.pos.z + 100;
	//pointThree.pos.x = pointOne.pos.x - 100;
	//pointThree.pos.y = pointOne.pos.y - 100;
	//pointTwo.uv = pointOne.uv;
	//pointThree.uv = pointOne.uv;
	//pointTwo.normal = pointOne.normal;
	//pointThree.normal = pointOne.normal;
	//
	//output.Append(pointOne);
	//output.Append(pointTwo);
	//output.Append(pointThree);
}

