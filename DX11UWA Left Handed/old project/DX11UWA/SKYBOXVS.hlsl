
cbuffer skyBuffer {
	float4x4 wvp;
};

struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 uv : UV;
	float3 normal:NORMAL;
};
 struct PixelShaderInput
 {
 	float4 pos : SV_POSITION;
 	float3 uv : UV; 
 };
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = mul(float4(input.pos, 1.0f), wvp).xyzw;

	output.pos = pos;
	//output.pos = float4(input.pos, 1.0f);
	//output.pos.w = 1;
	output.uv = input.pos;

	return output;
}
