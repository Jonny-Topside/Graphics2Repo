cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
	matrix rotation;
};

struct VertexShaderInput
{
	float3 pos : POSITION;
	float2 uv : UV;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 uv : UV;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 biTangent : BITANGENT;
	float4 wPos : WORLDPOSITION;

};

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	output.pos = float4(1, 1, 1, 1);
	 

	//float3 vertEdge0 = vert1 - vert0;
	//float3 vertEdge1 = vert2 - vert0;
	//
	//float2 texEdge0 = tex1 - tex0;
	//float2 texEdge1 = tex2 - tex0;
	//
	//
	//
	//
	//		
	//		normal = mul(input.normal, model);
	//		tangent = mul(float4(tangent.xyz * tangent.w, 0.0f), model);
	//		biTangent = mul(float4(cross(input.normal.xyz, tangent.xyz), 0.0f), model);
	//
	//
	//		output.wPos = pos;
	//		pos = mul(pos, view);
	//		pos = mul(pos, projection);
	//		output.pos = pos;
	return output;











}