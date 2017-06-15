

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

TextureCube skyBox : register(t0);
SamplerState skyBoxSS : register(s0);

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 skyBoxBox = skyBox.Sample(skyBoxSS, input.color);
	return skyBoxBox;
//	return skyBox.Sample(skyBoxSS, input.uv)
}