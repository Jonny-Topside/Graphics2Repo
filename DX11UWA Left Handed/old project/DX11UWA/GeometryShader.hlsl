cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

struct GSOutput
{
	float4 pos : SV_POSITION;
	float2 uv : UV;
	float3 normal : NORMAL;
};

[maxvertexcount(3)]
void main(	point float4 input[1] : SV_POSITION, inout TriangleStream< GSOutput > output)
{
	GSOutput pointOne;
	GSOutput pointTwo;
	GSOutput pointThree;

	pointOne.pos = float4( 0,0,0,0 );
	pointOne.uv = float2( 0,0 );
	pointOne.normal = float3( 0,0,0 );
	pointTwo = pointThree = pointOne;

	pointOne.pos = input[0];
	pointTwo.pos.x = pointOne.pos.x + 1;
	pointTwo.pos.y = pointOne.pos.y - 1;
	pointThree.pos.z = pointTwo.pos.z = pointOne.pos.z;
	pointThree.pos.x = pointOne.pos.x - 1;
	pointThree.pos.y = pointOne.pos.y - 1;
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
	
}

