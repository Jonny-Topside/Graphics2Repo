//cbuffer constBuffer {
//	float4x4 WorldViewProj;
//};
//
 struct PixelShaderInput
 {
 	float4 pos : SV_POSITION;
 	float2 uv : UV; 
 };
PixelShaderInput SKYBOX_VS(float3 pos : POSITION, float2 uv : UV, float3 normal : NORMAL)
{
//	PixelShaderInput output = (PixelShaderInput)0;
//	output.pos = mul(float4(pos, 1.0f), WVP)
//
}
