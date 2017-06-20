
cbuffer skyBuffer {
	matrix model;
	matrix view;
	matrix projection;
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
	//PixelShaderInput output;
	//float4 pos = mul(float4(input.pos, 1.0f), wvp).xyzw;
	//
	//output.pos = pos; 
	

	PixelShaderInput output;
	float4 pos = float4(input.pos, 1.0f);

	input.pos = mul(input.pos, model);
	// Transform the vertex position into projected space.
	pos = mul(pos, model);
	//wwwoutput.wPos = pos;
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	output.uv = input.pos;

	return output;
	//PixelShaderInput output;
	//float4 pos = float4(input.pos, 1.0f);
	//pos = mul(pos, wvp);
	//output.pos = pos;
	////output.pos = float4(input.pos, 1.0f);
	////output.pos.w = 1;
	//output.uv = input.pos;
	//
	//return output;
}
