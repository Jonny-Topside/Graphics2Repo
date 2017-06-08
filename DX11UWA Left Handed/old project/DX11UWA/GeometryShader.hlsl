struct GSOutput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR;
};

[maxvertexcount(3)]
void main(	triangle float4 input[3] : SV_POSITION, inout TriangleStream< GSOutput > output)
{
	for (uint i = 0; i < 3; i++)
	{
	//GSOutput element = nullptr;
	//element.pos = input[i];
	//output.Append(element);
	}
}

