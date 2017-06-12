
float clamp(float x, float maxN, float minN)
{
	return min(maxN, max(x, minN));
}

struct light
{
	float rat; float4 dir;
	//float4 pos;	
	float4 color;
};

//struct Light
//{
//	float3 direction;
//	float4 ambient;
//	float4 diffuse;
//};



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
	light lighter;
	//input.normal = normalize(input.normal);
float4 colorToReturn = pengText.Sample(pengSamplerState, input.uv);
//this is all lighting things
lighter.color = float4(1, 1, 1, 0.4f);
lighter.dir = float4(0.33f, 0.33f, -0.33f, 1);
lighter.rat = clamp(dot(-lighter.dir, input.normal), 1, 0);
//ligher.color.r *= lighter.rat;
//lighter.color.g *= lighter.rat;
//lighter.color.b *= lighter.rat;
//lighter.color.a *= lighter.rat;
lighter.color = lighter.color * lighter.rat;
//return lighter.color;
return mul(lighter.color, colorToReturn);


//finalColor = diffuse * light.ambient;
//finalColor += saturate(mul(light.dir, input.normal) * light.diffuse * diffuse);
//return float4(finalColor, diffuse.a);
//if (input.uv.x != -1 && input.uv.y != -1)
//{
	//return colorToReturn;

	//}
	//else
	//return float4(input.normal, 1.0f);
	//return float4(0,1,0,1);

}
