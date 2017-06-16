#pragma once

namespace DX11UWA
{
	struct forLightsOnly
	{
		//put all light data: lightDir, color, 
		DirectX::XMFLOAT3 lightDir;// = float3(0, 0, 0);
		DirectX::XMFLOAT3 surfaceNormal;// = float3(0, 0, 0);
		//float result = 0;
		DirectX::XMFLOAT4 lightColor;// = float4(0, 0, 0, 0);
		DirectX::XMFLOAT4 surfaceColor;// = float4(0, 0, 0, 0);
	};
	// Constant buffer used to send MVP matrices to the vertex shader.
	struct ModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT4X4 rotation;

	};

	// Used to send per-vertex data to the vertex shader.
	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
	};

	struct VertexPositionUVNormal
	{
		//ADD COLOR FOR THE LIGHTS
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 uv;
		DirectX::XMFLOAT3 normal;
	};
}