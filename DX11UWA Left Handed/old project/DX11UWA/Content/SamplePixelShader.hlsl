// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 uv : UV;
	float3 normal : NORMAL;
	//float4 color : COLOR;
	//ADDED A COLOR  
};
texture2D pengText : register(t0);
SamplerState pengSamplerState : register(s0);

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
float4 pengReturnColor = pengText.Sample(pengSamplerState, input.uv);
if (input.uv.x != -1 && input.uv.y != -1)
{
	return pengReturnColor;
}
else
return float4(0,1,0,1);
//float4(input.normal, 1.0f);

}
