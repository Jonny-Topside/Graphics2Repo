
struct light
{
	
	float4 diffuse;
	float4 coneDir;
	float4 conePos;
	float4 ambient;
	float4 color;
	float range;
	float4 dir;

	float4 pos;
	float3 att;
};





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
texture2D pengText : register(t0);
texture2D pengText1 : register(t1);

SamplerState pengSamplerState : register(s0);


// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	//ambient is multiplying texture color by small scalar value, then add to final color
	//ask if this counts as a reflective surface
	//float4 colorToReturn = pengText.Sample(pengSamplerState, input.normal);
	float4 colorToReturn = pengText.Sample(pengSamplerState, input.uv);

	//return colorToReturn;
	 float4 directionReturnVal;
	 float4 pointReturnVal;
	 float4 spotLightResult;
	 light lighter;
	 //INITIALIZING LIGHT STRUCT
	 lighter.dir = float4(0.33f, 0.33f, -0.33f, 1);
	 lighter.coneDir = float4(0, 0, 1.0f, 1.0f);
	 lighter.ambient = colorToReturn * 0.05f;
	 //RATIOS
	 float surfaceRat = 0;
	 float lightRat = 0;
	 float coneRat = 0;
	// float4 gray = float4(1.0f, 0.5f, 0.5f, 0.5f);
	// colorToReturn.a *= gray;
	// colorToReturn.r *= gray;
	// colorToReturn.g *= gray;
	// colorToReturn.b *= gray;
	 
	 //colorToReturn = (colorToReturn.r *colorToReturn.g*colorToReturn.b);
	 
	 
	 	lighter.coneDir = float4(1.0f, 1.0f, 1.0f, 1);
	 	//DIRECTIONAL LIGHT : BLUE
	 	lighter.color = float4(0, 0, 1, 1.0f);
	 	 lightRat = clamp(dot(-lighter.dir.xyz, input.normal), 0, 1);
	 	 directionReturnVal = lightRat * lighter.color * colorToReturn;
	 	//POINT LIGHT : RED
	 	 lighter.color = float4(1, 0, 0.0f, 1.0f);
	 	 lighter.pos = float4(0.0F, 0.0f, 0.0f, 1);// *sin(180);
	 	 lighter.dir = normalize(lighter.pos - input.wPos);
	 	 lightRat = saturate(dot(lighter.dir.xyz, input.normal.xyz));
	 	 float attenuation = 1.0f - clamp(length(lighter.pos - input.wPos) / 15.1f, 0, 1);
	 	 pointReturnVal = lightRat * lighter.color * colorToReturn * attenuation;
	 	//SPOTLIGHT : YELLOW
	 	lighter.conePos = float4(0.0f, 0.7f, 0.0f, 1.0f);
	 	lighter.color = float4(1, 1, 0.0f, 1.0f);
	 	lighter.dir = normalize(lighter.conePos - input.wPos);
	 	surfaceRat = clamp(dot(-lighter.dir, lighter.coneDir),0,1);
	 	float spotFactor = (surfaceRat > coneRat) ? 1 : 0;
	 	lightRat = clamp(dot(lighter.dir, input.normal),0,1);
	 	spotLightResult = spotFactor * lightRat * lighter.color * colorToReturn;

		//if (input.pos.w == 1)
		//{
		//colorToReturn = float4(1, 1.0f / 10, 0, 0);
		//return saturate(spotLightResult + (directionReturnVal + pointReturnVal) + lighter.ambient + colorToReturn);
		//}
		//else

		return saturate(spotLightResult + (directionReturnVal + pointReturnVal) + lighter.ambient);
}
