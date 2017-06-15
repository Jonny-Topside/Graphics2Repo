

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 uv : UV;
};
//THIS IS OK, DO NOT DELET
TextureCube skyBox : register(t0);
SamplerState skyBoxSS : register(s0);

float4 SBPS_Main(PixelShaderInput input) : SV_TARGET
{
	return  skyBox.Sample(skyBoxSS, input.uv);
	
//	return skyBox.Sample(skyBoxSS, input.uv)
}