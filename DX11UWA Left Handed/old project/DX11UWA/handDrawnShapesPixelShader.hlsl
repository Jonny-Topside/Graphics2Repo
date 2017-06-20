
// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 uv : UV;
	float3 normal : NORMAL;
	float4 wPos : WORLDPOSITION;
	//float4 color : COLOR;
	//ADDED A COLOR  
};
texture2D planeTex : register(t0);
texture2D planeTexNormal : register(t1);
SamplerState planeSS : register(s0);


// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 newNormal =  planeTexNormal.Sample(planeSS, input.normal);
	newNormal = (2.0f * newNormal) - 1.0f;
	input.normal = normalize(newNormal);
	input.normal = float4(normalize(input.normal.xyz), 1);
	//ambient is multiplying texture color by small scalar value, then add to final color
	float4 colorToReturn = planeTex.Sample(planeSS, input.uv);
	return newNormal;
}