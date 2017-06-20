
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 uv : UV;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 bitangent : BITANGENT;
	float4 wPos : WORLDPOSITION;
	//float4 color : COLOR;
	//ADDED A COLOR  
};



float4 main(PixelShaderInput input) : SV_TARGET
{
	//float3 newNormal = (newNormal * 2.0f) - 1.0f;
	float3x3 TBN;

	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);
	input.bitangent = normalize(input.bitangent);

	TBN[0] = input.tangent.xyz;
	TBN[1] = input.bitangent.xyz;
	TBN[2] = input.normal.xyz;


	//newNormal = mul(newNormal, TBN);

	return 1;
	//return float4(newNormal, 1.0f);

}