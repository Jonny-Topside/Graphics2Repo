#include "pch.h"
#include "Sample3DSceneRenderer.h"
#include "DDSTextureLoader.h"
#include "..\Common\DirectXHelper.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
using namespace DX11UWA;

using namespace DirectX;
using namespace Windows::Foundation;

bool Sample3DSceneRenderer::loadOBJ(const char * path, std::vector<VertexPositionUVNormal> &vpuvn, std::vector<unsigned int> &outIndices)
{
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<XMFLOAT3> temp_vertices;
	std::vector<XMFLOAT2> temp_uvs;
	std::vector<XMFLOAT3> temp_normals;
	std::vector<XMFLOAT3> temp_tangents;
	FILE * file = fopen(path, "r");
	if (file == NULL)
	{
		printf("Cannot open file\n");
		return false;
	}

	while (1)
	{
		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;

		if (strcmp(lineHeader, "v") == 0)
		{
			XMFLOAT3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);

		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			XMFLOAT2 uv;
			fscanf_s(file, "%f %f \n", &uv.x, &uv.y);
			uv.y = 1 - uv.y;
			temp_uvs.push_back(uv);
		}

		else if (strcmp(lineHeader, "vn") == 0) {
			XMFLOAT3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0],
				&normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2],
				&normalIndex[2]);
			if (matches != 9)
			{
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);

			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);

			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);


		}

	}
	float vertEdge0		;
		float vertEdge1	;

		float texEdge0;
		float texEdge1;
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		VertexPositionUVNormal tempvpuvn;

		XMFLOAT3 vertex = temp_vertices[vertexIndices[i] - 1];
		tempvpuvn.pos = temp_vertices[vertexIndices[i] - 1];
		tempvpuvn.uv = temp_uvs[uvIndices[i] - 1];
		tempvpuvn.normal = temp_normals[normalIndices[i] - 1];


		vertEdge0 = tempvpuvn.pos.y - tempvpuvn.pos.x;
		vertEdge1 = tempvpuvn.pos.z - tempvpuvn.pos.x;

		float texEdge0 = tempvpuvn.uv.y - tempvpuvn.uv.x;
		//float texEdge1 = tempvpuvn.uv.z - tempvpuvn.uv.x;
		//tempvpuvn.tangent

		float ratio = 1.0f / (tempvpuvn.uv.x * tempvpuvn.uv.y - tempvpuvn.uv.x * tempvpuvn.uv.y);

		//XMVECTOR uDirection = XMFLOAT3(texEdge1.y * vertEdge0.x - texEdge0.y * vertEdge1.x) * ratio,
		//	(texEdge1.y * vertEdge0.y - texEdge0.y * vertEdge1.y) * ratio,
		//	(texEdge1.y * vertEdge0.z - texEdge0.y * vertEdge1.z) * ratio);
		//
		//	XMFLOAT3 vDirection = XMFLOAT3(texEdge0.x * vertEdge1.x - texEdge1.x * vertEdge0.x) * ratio,
		//		(texEdge0.y * vertEdge1.y - texEdge1.x * vertEdge0.y) * ratio,
		//		(texEdge0.x * vertEdge1.z - texEdge1.x * vertEdge0.z) * ratio);
		//
		//
		//	//	uDirection = normalize(uDirection);
		//		uDirection = XMVector3Normalize(uDirection);
		//		float dotResult = XMVector3Dot(tempvpuvn.normal, uDirection);
		//		float4 tangent = uDirection - input.normal * dotResult;
		//		tangent = normalize(tangent);
		//
		//		vDirection = normalize(vDirection);
		//		float crossResult = XMVector3Cross(input.normal, uDirection);
		//		float3 = vDirection;
		//		float dotResult = XMVector3Dot(cross, handedNess);
		//		tangent.w = (dotResult < 0.0f) ? -1.0f : 1.0f;


		vpuvn.push_back(tempvpuvn);
		outIndices.push_back(i);
	}
}

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_indexCount(0),
	m_tracking(false),
	m_deviceResources(deviceResources)
{
	memset(m_kbuttons, 0, sizeof(m_kbuttons));
	m_currMousePos = nullptr;
	m_prevMousePos = nullptr;
	memset(&m_camera, 0, sizeof(XMFLOAT4X4));
	memset(&m_camera1, 0, sizeof(XMFLOAT4X4));

	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources(void)
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = (outputSize.Width * 0.5f) / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	//if (aspectRatio < 1.0f)
	//{
	//	fovAngleY *= 2.0f;
	//}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, 0.01f, 100.0f);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();
	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);
	XMStoreFloat4x4(&m_constantBufferData.projection, XMMatrixTranspose(perspectiveMatrix * orientationMatrix));
	
	XMStoreFloat4x4(&pyramidConstantBufferData.projection, XMMatrixTranspose(perspectiveMatrix * orientationMatrix));
	XMStoreFloat4x4(&planeConstantBufferData.projection, XMMatrixTranspose(perspectiveMatrix * orientationMatrix));
	XMStoreFloat4x4(&skyboxConstantBufferData.projection, XMMatrixTranspose(perspectiveMatrix * orientationMatrix));
	XMStoreFloat4x4(&metalCubeConstantBufferData.projection, XMMatrixTranspose(perspectiveMatrix * orientationMatrix));

	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
	  XMVECTORF32 eye = { 0.0f, 0.7f, -1.5f, 0.0f };
	  XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
	  XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };
	 
	  XMVECTORF32 eye1 = { 0.0f, 3.0f, -2.5f, 0.0f };
	  XMVECTORF32 at1 = { 1.0f, 2.0f, 3.0f, 0.0f };
	  XMVECTORF32 up1 = { 0.0f, 1.0f, 0.0f, 0.0f };
	//good
	XMStoreFloat4x4(&m_camera, XMMatrixInverse(nullptr, XMMatrixLookAtLH(eye, at, up)));
	XMStoreFloat4x4(&m_camera1, XMMatrixInverse(nullptr, XMMatrixLookAtLH(eye1, at1, up1)));
	//good
	//
	//XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up)));
	//XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtLH(eye1, at1, up1)));
	//


}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(187);
	XMVECTOR camTarget;
	XMVECTOR xvec = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);



	if (!m_tracking)
	{
		// Convert degrees to radians, then convert seconds to rotation angle
		float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
		double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

		Rotate(radians);
	}

	// Update or move camera here
	UpdateCamera(timer, 3.0f, 0.75f);

}

// Rotate the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::Rotate(float radians)
{
	// Prepare to pass the updated model matrix to the shader
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
	XMStoreFloat4x4(&pyramidConstantBufferData.model, XMMatrixTranspose(XMMatrixTranslation(4, 0, -2)));
	XMStoreFloat4x4(&planeConstantBufferData.model, XMMatrixTranspose(XMMatrixTranslation(0, -1, 0)));
	//XMStoreFloat4x4(&skyboxConstantBufferData.model, XMMatrixTranslation(m_camera._41, m_camera._42, m_camera._43));
	XMStoreFloat4x4(&skyboxConstantBufferData.model, XMMatrixTranspose(XMMatrixMultiply(XMMatrixTranslation(0, 0, 0), XMMatrixScaling(100, 100, 100))));

	XMStoreFloat4x4(&metalCubeConstantBufferData.model, XMMatrixTranspose(XMMatrixTranslation(1, 0, 1)));

	//XMStoreFloat4x4(&m_constantBufferData.rotation, XMMatrixTranslation(m_camera._41, m_camera._42, m_camera._43));

	//ME TRYING TO ROTATE MY OWN THING
}

void Sample3DSceneRenderer::UpdateCamera(DX::StepTimer const& timer, float const moveSpd = 5.0f, float const rotSpd = 1.0f)
{
	const float delta_time = (float)timer.GetElapsedSeconds();

	if (m_kbuttons['W'])
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, moveSpd * delta_time);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	if (m_kbuttons['S'])
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, -moveSpd * delta_time);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	if (m_kbuttons['A'])
	{
		XMMATRIX translation = XMMatrixTranslation(-moveSpd * delta_time, 0.0f, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	if (m_kbuttons['D'])
	{
		XMMATRIX translation = XMMatrixTranslation(moveSpd * delta_time, 0.0f, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	if (m_kbuttons['X'])
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, -moveSpd * delta_time, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	if (m_kbuttons[VK_SPACE])
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, moveSpd * delta_time, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}

	if (m_currMousePos)
	{
		if (m_currMousePos->Properties->IsRightButtonPressed && m_prevMousePos)
		{
			float dx = m_currMousePos->Position.X - m_prevMousePos->Position.X;
			float dy = m_currMousePos->Position.Y - m_prevMousePos->Position.Y;

			XMFLOAT4 pos = XMFLOAT4(m_camera._41, m_camera._42, m_camera._43, m_camera._44);

			m_camera._41 = 0;
			m_camera._42 = 0;
			m_camera._43 = 0;

			XMMATRIX rotX = XMMatrixRotationX(dy * rotSpd * delta_time);
			XMMATRIX rotY = XMMatrixRotationY(dx * rotSpd * delta_time);

			XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
			temp_camera = XMMatrixMultiply(rotX, temp_camera);
			temp_camera = XMMatrixMultiply(temp_camera, rotY);

			XMStoreFloat4x4(&m_camera, temp_camera);

			m_camera._41 = pos.x;
			m_camera._42 = pos.y;
			m_camera._43 = pos.z;
		}
		m_prevMousePos = m_currMousePos;
	}


}

void Sample3DSceneRenderer::SetKeyboardButtons(const char* list)
{
	memcpy_s(m_kbuttons, sizeof(m_kbuttons), list, sizeof(m_kbuttons));
}
//
void Sample3DSceneRenderer::SetMousePosition(const Windows::UI::Input::PointerPoint^ pos)
{
	m_currMousePos = const_cast<Windows::UI::Input::PointerPoint^>(pos);
}
//
void Sample3DSceneRenderer::SetInputDeviceData(const char* kb, const Windows::UI::Input::PointerPoint^ pos)
{
	SetKeyboardButtons(kb);
	SetMousePosition(pos);
}
//
void DX11UWA::Sample3DSceneRenderer::StartTracking(void)
{
	m_tracking = true;
}
//
// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void Sample3DSceneRenderer::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
		Rotate(radians);
	}
}
//
void Sample3DSceneRenderer::StopTracking(void)
{
	m_tracking = false;
}

// Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render(void)
{
	auto context = m_deviceResources->GetD3DDeviceContext();
	HRESULT r;
	//PENG
		// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(VertexPositionUVNormal);
	UINT offset = 0;

	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}
	 
	//VP
	{

	viewport1.Width = m_deviceResources->GetOutputSize().Width / 2;
	viewport1.Height = m_deviceResources->GetOutputSize().Height;
	viewport1.MinDepth = 0.0f;
	viewport1.MaxDepth = 1.0f;
	viewport1.TopLeftX = 0;
	viewport1.TopLeftY = 0;



	m_deviceResources->GetD3DDeviceContext()->RSSetViewports(1, &viewport1);
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera))));
	XMStoreFloat4x4(&pyramidConstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera))));
	XMStoreFloat4x4(&planeConstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera))));
	XMStoreFloat4x4(&skyboxConstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera))));
	XMStoreFloat4x4(&metalCubeConstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera))));
	XMStoreFloat4x4(&skyboxConstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera))));
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera))));

	}
 
	skyboxConstantBufferData.view._14 = 0;
	skyboxConstantBufferData.view._24 = 0;
	skyboxConstantBufferData.view._34 = 0; 

	//PENG
	r = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/peng.dds", (ID3D11Resource**)pengTexture.Get(), &pengSRV);
	ID3D11ShaderResourceView* pengSRVpointer[] = { pengSRV.Get() };
	D3D11_SAMPLER_DESC pengSamplerDesc;
	ZeroMemory(&pengSamplerDesc, sizeof(pengSamplerDesc));
	pengSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	pengSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	pengSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	pengSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateSamplerState(&pengSamplerDesc, pengSS.GetAddressOf())); //&

	context->PSSetShaderResources(0, 1, pengSRVpointer);
	context->PSSetSamplers(0, 1, pengSS.GetAddressOf());
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	// Each index is one 16-bit unsigned integer (short).
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_inputLayout.Get());
	// Attach our vertex shader.
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);
	// Attach our pixel shader.
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	// Draw the objects.
	context->DrawIndexed(m_indexCount, 0, 0);

	////GEO SHADER
	{

	context->GSSetShader(GeometryShader.Get(), nullptr, 0);
	//	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->DrawIndexed(m_indexCount, 0, 0);
	//context->DrawIndexedInstanced(m_indexCount, 3624, 0, 0, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	}

	//PLANE

	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/MidBoss_Floor_Diffuse.dds", nullptr, &planeSRV);
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/MidBoss_Floor_Normal.dds",  nullptr, &planeSRV2);

	
	ID3D11ShaderResourceView* planeSRVpointer[] = { planeSRV.Get(), planeSRV2.Get()};
	//ID3D11ShaderResourceView* planeSRVpointer[] = {  planeSRV2.Get() };

	context->PSSetShaderResources(0, 2, planeSRVpointer);
	context->UpdateSubresource1(planeConstantBuffer.Get(), 0, NULL, &planeConstantBufferData, 0, 0, 0);
	D3D11_SAMPLER_DESC planeSamplerDesc;
	ZeroMemory(&planeSamplerDesc, sizeof(planeSamplerDesc));
	planeSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	planeSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	planeSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	planeSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateSamplerState(&planeSamplerDesc, planeSS.GetAddressOf())); //& 


	context->PSSetSamplers(0, 1, planeSS.GetAddressOf());
	context->PSSetSamplers(1, 1, planeSS.GetAddressOf());


	
	context->IASetVertexBuffers(0, 1, planeVertexBuffer.GetAddressOf(), &stride, &offset);
	// Each index is one 16-bit unsigned integer (short).
	context->IASetIndexBuffer(planeIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(planeInputLayout.Get());
	// Attach our vertex shader.
	context->VSSetShader(planeVS.Get(), nullptr, 0);
	context->VSSetConstantBuffers1(0, 1, planeConstantBuffer.GetAddressOf(), nullptr, nullptr);
	context->PSSetShader(planePS.Get(), nullptr, 0);

	//context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->DrawIndexed(planeIndexCount, 0, 0);


	// //PYRAMID
	{

		stride = sizeof(VertexPositionColor);

		context->IASetVertexBuffers(0, 1, pyramidVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(pyramidIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
		context->UpdateSubresource1(pyramidConstantBuffer.Get(), 0, NULL, &pyramidConstantBufferData, 0, 0, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(pyramidInputLayout.Get());
		//context->VSSetShader(pyramidVertexShader.Get(), nullptr, 0);
		context->VSSetConstantBuffers1(0, 1, pyramidConstantBuffer.GetAddressOf(), nullptr, nullptr);
		//context->PSSetShader(pyramidPixelShader.Get(), nullptr, 0);
		context->DrawIndexed(pyramidIndexCount, 0, 0);
	}

	//METAL CUBE
	

	HRESULT h = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/MidBoss_Floor_Normal.dds", (ID3D11Resource**)metalCubeTexture.Get(), &metalCubeSRV);
	ID3D11ShaderResourceView* metalCubeSRVpointer[] = { metalCubeSRV.Get() };
	context->PSSetShaderResources(0, 1, metalCubeSRVpointer);
	{
		context->PSSetSamplers(0, 1, metalCubeSS.GetAddressOf());
	}

	stride = sizeof(VertexPositionUVNormal);
	D3D11_SAMPLER_DESC metalCubeSamplerDesc;
	ZeroMemory(&metalCubeSamplerDesc, sizeof(metalCubeSamplerDesc));
	metalCubeSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	metalCubeSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	metalCubeSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	metalCubeSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateSamplerState(&metalCubeSamplerDesc, metalCubeSS.GetAddressOf())); //& 


	context->IASetVertexBuffers(0, 1, metalCubeVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(metalCubeIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->UpdateSubresource1(metalCubeConstantBuffer.Get(), 0, NULL, &metalCubeConstantBufferData, 0, 0, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(metalCubeInputLayout.Get());
	context->VSSetConstantBuffers1(0, 1, metalCubeConstantBuffer.GetAddressOf(), nullptr, nullptr);
	//context->DrawIndexed(skyboxIndexCount, 0, 0);
	  context->DrawIndexedInstanced(skyboxIndexCount, 4, 0, 0, 0);
	

	// //SKYBOX
	

	ID3D11ShaderResourceView* skyboxSRVpointer[] = { skyboxSRV.Get() };
	//stride = sizeof(VertexPositionUVNormal);

	D3D11_SAMPLER_DESC skyboxSamplerDesc;
	ZeroMemory(&skyboxSamplerDesc, sizeof(skyboxSamplerDesc));
	skyboxSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	skyboxSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	skyboxSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	skyboxSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateSamplerState(&skyboxSamplerDesc, skyboxSS.GetAddressOf())); //& 

	{
	context->UpdateSubresource1(skyboxConstantBuffer.Get(), 0, NULL, &skyboxConstantBufferData, 0, 0, 0);

	context->IASetIndexBuffer(skyboxIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetVertexBuffers(0, 1, skyboxVertexBuffer.GetAddressOf(), &stride, &offset);

	context->VSSetConstantBuffers(0, 1, skyboxConstantBuffer.GetAddressOf());
	context->PSSetShaderResources(0, 1, skyboxSRVpointer);
	context->PSSetSamplers(0, 1, skyboxSS.GetAddressOf());
	context->VSSetShader(skyboxVertexShader.Get(), nullptr, 0);
	context->PSSetShader(skyboxPixelShader.Get(), nullptr, 0);


	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(skyboxInputLayout.Get());
	context->DrawIndexed(skyboxIndexCount, 0, 0);
	}
 

	//VIEWPORT 2
	

	m_deviceResources->GetD3DDeviceContext()->RSSetViewports(1, &viewport2);

	viewport2.Width = m_deviceResources->GetOutputSize().Width / 2;
	viewport2.Height = m_deviceResources->GetOutputSize().Height;

	viewport2.MinDepth = 0.0f;
	viewport2.MaxDepth = 1.0f;
	viewport2.TopLeftX = m_deviceResources->GetOutputSize().Width / 2;
	viewport2.TopLeftY = 0;

	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera1))));
	XMStoreFloat4x4(&pyramidConstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera1))));
	XMStoreFloat4x4(&planeConstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera1))));
	XMStoreFloat4x4(&skyboxConstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera1))));
	XMStoreFloat4x4(&metalCubeConstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera1))));
	XMStoreFloat4x4(&skyboxConstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera1))));
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera1))));

	

	//XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera))));
	//XMStoreFloat4x4(&pyramidConstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera))));
	//XMStoreFloat4x4(&planeConstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera))));
	//XMStoreFloat4x4(&skyboxConstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera))));

	//XMStoreFloat4x4(&skyboxConstantBufferData.view,XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera))));
	//skyboxConstantBufferData.view._14 = 0;
	//skyboxConstantBufferData.view._24 = 0;
	//skyboxConstantBufferData.view._34 = 0;

	//XMStoreFloat4x4(&metalCubeConstantBufferData.view, XMLoadFloat4x4(&m_camera));
	//XMStoreFloat4x4(&metalCubeConstantBufferData.view, XMLoadFloat4x4(&m_camera1));

	//PENG
	{

		r = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/peng.dds", (ID3D11Resource**)pengTexture.Get(), &pengSRV);

		ZeroMemory(&pengSamplerDesc, sizeof(pengSamplerDesc));
		pengSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		pengSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		pengSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		pengSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateSamplerState(&pengSamplerDesc, pengSS.GetAddressOf())); //&

		context->PSSetShaderResources(0, 1, pengSRVpointer);
		context->PSSetSamplers(0, 1, pengSS.GetAddressOf());
		context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
		// Each index is one 16-bit unsigned integer (short).
		context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(m_inputLayout.Get());
		// Attach our vertex shader.
		context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
		// Send the constant buffer to the graphics device.
		context->VSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);
		// Attach our pixel shader.
		context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
		// Draw the objects.
		context->DrawIndexed(m_indexCount, 0, 0);
	}

	////GEO SHADER
	{

		context->GSSetShader(GeometryShader.Get(), nullptr, 0);
		//	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->DrawIndexed(m_indexCount, 0, 0);
		//context->DrawIndexedInstanced(m_indexCount, 3, 0, 0, 0);
		context->GSSetShader(nullptr, nullptr, 0);
	}

	//PLANE
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/MidBoss_Floor_Diffuse.dds", (ID3D11Resource**)planeTexture.Get(), &planeSRV);
	context->PSSetShaderResources(0, 1, planeSRVpointer);
	{
		context->PSSetSamplers(0, 1, planeSS.GetAddressOf());
	}
	context->PSSetShader(planePS.Get(), nullptr, 0);
	context->VSSetShader(planeVS.Get(), nullptr, 0);

	context->IASetVertexBuffers(0, 1, planeVertexBuffer.GetAddressOf(), &stride, &offset);
	// Each index is one 16-bit unsigned integer (short).
	context->IASetIndexBuffer(planeIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->UpdateSubresource1(planeConstantBuffer.Get(), 0, NULL, &planeConstantBufferData, 0, 0, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(planeInputLayout.Get());
	// Attach our vertex shader.
	//context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->VSSetConstantBuffers1(0, 1, planeConstantBuffer.GetAddressOf(), nullptr, nullptr);
	context->DrawIndexed(planeIndexCount, 0, 0);


	//PYRAMID
	stride = sizeof(VertexPositionColor);
	{
	context->IASetVertexBuffers(0, 1, pyramidVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(pyramidIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	context->UpdateSubresource1(pyramidConstantBuffer.Get(), 0, NULL, &pyramidConstantBufferData, 0, 0, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(pyramidInputLayout.Get());
	//context->VSSetShader(pyramidVertexShader.Get(), nullptr, 0);
	context->VSSetConstantBuffers1(0, 1, pyramidConstantBuffer.GetAddressOf(), nullptr, nullptr);
	//context->PSSetShader(pyramidPixelShader.Get(), nullptr, 0);
	context->DrawIndexed(pyramidIndexCount, 0, 0);
	}

	//METAL CUBE
	{

		h = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/MidBoss_Floor_Normal.dds", (ID3D11Resource**)metalCubeTexture.Get(), &metalCubeSRV);
		context->PSSetShaderResources(0, 1, metalCubeSRVpointer);
		context->PSSetSamplers(0, 1, metalCubeSS.GetAddressOf());

		stride = sizeof(VertexPositionUVNormal);
		metalCubeSamplerDesc;
		ZeroMemory(&metalCubeSamplerDesc, sizeof(metalCubeSamplerDesc));
		metalCubeSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		metalCubeSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		metalCubeSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		metalCubeSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateSamplerState(&metalCubeSamplerDesc, metalCubeSS.GetAddressOf())); //& 


		context->IASetVertexBuffers(0, 1, metalCubeVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(metalCubeIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->UpdateSubresource1(metalCubeConstantBuffer.Get(), 0, NULL, &metalCubeConstantBufferData, 0, 0, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(metalCubeInputLayout.Get());
		context->VSSetConstantBuffers1(0, 1, metalCubeConstantBuffer.GetAddressOf(), nullptr, nullptr);
		//context->DrawIndexed(skyboxIndexCount, 0, 0);
		context->DrawIndexedInstanced(skyboxIndexCount, 4, 0, 0, 0);
	}

	// //SKYBOX
	{
		 
		ZeroMemory(&skyboxSamplerDesc, sizeof(skyboxSamplerDesc));
		skyboxSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		skyboxSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		skyboxSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		skyboxSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateSamplerState(&skyboxSamplerDesc, skyboxSS.GetAddressOf())); //& 


		context->UpdateSubresource1(skyboxConstantBuffer.Get(), 0, NULL, &skyboxConstantBufferData, 0, 0, 0);

		context->IASetIndexBuffer(skyboxIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetVertexBuffers(0, 1, skyboxVertexBuffer.GetAddressOf(), &stride, &offset);

		context->VSSetConstantBuffers(0, 1, skyboxConstantBuffer.GetAddressOf());
		context->PSSetShaderResources(0, 1, skyboxSRVpointer);
		context->PSSetSamplers(0, 1, skyboxSS.GetAddressOf());
		context->VSSetShader(skyboxVertexShader.Get(), nullptr, 0);
		context->PSSetShader(skyboxPixelShader.Get(), nullptr, 0);
  		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(skyboxInputLayout.Get());
		context->DrawIndexed(skyboxIndexCount, 0, 0); 
	}
}

void Sample3DSceneRenderer::CreateDeviceDependentResources(void)
{


	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");
	auto loadGSTask = DX::ReadDataAsync(L"GeometryShader.cso");
	auto loadPyramidVSTask = DX::ReadDataAsync(L"handDrawnShapesVertexShader.cso");
	auto loadPyramidPSTask = DX::ReadDataAsync(L"handDrawnShapesPixelShader.cso");
	auto loadSkyboxVSTask = DX::ReadDataAsync(L"SKYBOXVS.cso");
	auto loadSkyboxPSTask = DX::ReadDataAsync(L"SKYBOXPS.cso");

	//CREATING GEOMETRY SHADER
	auto createGSTask = loadGSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateGeometryShader(&fileData[0], fileData.size(), nullptr, &GeometryShader));

	});

	//CREATING SKYBOX
	auto createSBVSTask = loadSkyboxVSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateVertexShader(&fileData[0], fileData.size(), nullptr, &skyboxVertexShader));

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },

		};
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), &fileData[0], fileData.size(), &skyboxInputLayout));
		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &skyboxConstantBuffer));

	});
	auto createSBPSTask = loadSkyboxPSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreatePixelShader(&fileData[0], fileData.size(), nullptr, &skyboxPixelShader));
	});
	auto createSBTask = (createSBPSTask && createSBVSTask).then([this]()
	{
		std::vector<VertexPositionUVNormal> vertUvNormal;
		std::vector<unsigned int> out_indices;

		CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/SunsetSkybox.dds", (ID3D11Resource**)skyboxTexture.Get(), &skyboxSRV);
		bool res = loadOBJ("Assets/skyboxCube.obj", vertUvNormal, out_indices);

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		vertexBufferData.pSysMem = vertUvNormal.data();
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;

		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionUVNormal) * vertUvNormal.size(), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &skyboxVertexBuffer));

		skyboxIndexCount = out_indices.size();

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = out_indices.data();
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned int) * out_indices.size(), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &skyboxIndexBuffer));

	});

	// THE PLANE
	auto createPlaneVSTask = loadVSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateVertexShader(&fileData[0], fileData.size(), nullptr, &planeVS));

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },

		};
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), &fileData[0], fileData.size(), &planeInputLayout));
	});
	auto createPlanePSTask = loadPSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreatePixelShader(&fileData[0], fileData.size(), nullptr, &planePS));

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &planeConstantBuffer));

	});
	auto createPlaneTask = (createPlanePSTask && createPlaneVSTask).then([this]()
	{
		std::vector<VertexPositionUVNormal> vertUvNormal;
		std::vector<unsigned int> out_indices;

		bool res = loadOBJ("Assets/PLANE.obj", vertUvNormal, out_indices);

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		vertexBufferData.pSysMem = vertUvNormal.data();
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;

		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionUVNormal) * vertUvNormal.size(), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &planeVertexBuffer));

		planeIndexCount = out_indices.size();

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = out_indices.data();
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned int) * out_indices.size(), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &planeIndexBuffer));

	});

	//METAL CUBE
	auto createMetalVSTask = loadPyramidVSTask.then([this](const std::vector<byte>& fileData)
	{
		//DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateVertexShader(&fileData[0], fileData.size(), nullptr, &planeVS));

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },

		};
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), &fileData[0], fileData.size(), &metalCubeInputLayout));
	});
	auto createMetalPSTask = loadPyramidPSTask.then([this](const std::vector<byte>& fileData)
	{
		//DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreatePixelShader(&fileData[0], fileData.size(), nullptr, &planePS));

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &metalCubeConstantBuffer));
	});
	auto createMetalTask = (createMetalPSTask && createMetalVSTask).then([this]()
	{
		std::vector<VertexPositionUVNormal> vertUvNormal;
		std::vector<unsigned int> out_indices;

		bool res = loadOBJ("Assets/normalCubeForTexturingTo.obj", vertUvNormal, out_indices);

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		vertexBufferData.pSysMem = vertUvNormal.data();
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;

		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionUVNormal) * vertUvNormal.size(), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &metalCubeVertexBuffer));

		planeIndexCount = out_indices.size();

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = out_indices.data();
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned int) * out_indices.size(), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &metalCubeIndexBuffer));

	});

	//PENGUIN
		// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateVertexShader(&fileData[0], fileData.size(), nullptr, &m_vertexShader));

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },

		};
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), &fileData[0], fileData.size(), &m_inputLayout));
	});
	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreatePixelShader(&fileData[0], fileData.size(), nullptr, &m_pixelShader));

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &m_constantBuffer));
	});
	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask).then([this]()
	{
		std::vector<VertexPositionUVNormal> vertUvNormal;
		std::vector<unsigned int> out_indices;

		bool res = loadOBJ("Assets/peng.obj", vertUvNormal, out_indices);

		//Load mesh vertices. Each vertex has a position and a color.
		//static const VertexPositionColor cubeVertices[] =
		//{
		//	{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
		//	{XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f)},
		//	{XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
		//	{XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f)},
		//	{XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
		//	{XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f)},
		//	{XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f)},
		//	{XMFLOAT3( 0.5f,  0.5f,  0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f)},
		//};

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		vertexBufferData.pSysMem = vertUvNormal.data();
		//vertexBufferData.pSysMem = cubeVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		//CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);

		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionUVNormal) * vertUvNormal.size(), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer));

		// Load mesh indices. Each trio of indices represents
		// a triangle to be rendered on the screen.
		// For example: 0,2,1 means that the vertices with indexes
		// 0, 2 and 1 from the vertex buffer compose the 
		// first triangle of this mesh.
	//	static const unsigned short cubeIndices[] =
	//	{
	//		0,1,2, // -x
	//		1,3,2,
	//
	//		4,6,5, // +x
	//		5,6,7,
	//
	//		0,5,1, // -y
	//		0,4,5,
	//
	//		2,7,6, // +y
	//		2,3,7,
	//
	//		0,6,4, // -z
	//		0,2,6,
	//
	//		1,7,3, // +z
	//		1,5,7,
	//	};
		//m_indexCount = ARRAYSIZE(cubeIndices);

		m_indexCount = out_indices.size();

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		//indexBufferData.pSysMem = cubeIndices;
		indexBufferData.pSysMem = out_indices.data();
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		//CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned int) * out_indices.size(), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_indexBuffer));

	});

	//PYRAMID
	auto createPyramidVSTask = loadPyramidVSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateVertexShader(&fileData[0], fileData.size(), nullptr, &pyramidVertexShader));

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },

		};
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), &fileData[0], fileData.size(), &pyramidInputLayout));

	});
	auto createPyramidPSTask = loadPyramidPSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreatePixelShader(&fileData[0], fileData.size(), nullptr, &pyramidPixelShader));

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &pyramidConstantBuffer));
	});
	auto createPyramidTask = (createPyramidPSTask && createPyramidVSTask).then([this]()
	{
		//Load mesh vertices. Each vertex has a position and a color.
		static const VertexPositionColor triVertices[] =
		{
			{XMFLOAT3(0.0f, 0.5f, 0.0f), //top  0
			XMFLOAT3(-0.5f, 0.0f, -0.5f)}, //front left 1
			{XMFLOAT3(-0.5f, 0.0f, -0.5f), //back left 2
			XMFLOAT3(0.5f, 0.0f, -0.5f)}, //back right 3
			{XMFLOAT3(0.5f,  0.0f, -0.5f)} //front right 4
		};

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = triVertices;
		//vertexBufferData.pSysMem = cubeVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(triVertices), D3D11_BIND_VERTEX_BUFFER);

		//CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionUVNormal) * vertUvNormal.size(), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &pyramidVertexBuffer));


		static const unsigned short triIndices[] =
		{
			0,1,4,
			0,2,1,

			2,0,3,
			3,0,4,

			4,3,1,
			2,3,1


		};
		pyramidIndexCount = ARRAYSIZE(triIndices);

		//m_indexCount = out_indices.size();

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		//indexBufferData.pSysMem = cubeIndices;
		indexBufferData.pSysMem = triIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(triIndices), D3D11_BIND_INDEX_BUFFER);
		//CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned int) * out_indices.size(), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &pyramidIndexBuffer));

	});

	//// Once the cube is loaded, the object is ready to be rendered.
	createPlaneTask.then([this]()
	{
		m_loadingComplete = true;
	});
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources(void)
{
	m_loadingComplete = false;
       m_vertexShader.Reset();
    	m_inputLayout.Reset();
        m_pixelShader.Reset();
     m_constantBuffer.Reset();
	   m_vertexBuffer.Reset();
        m_indexBuffer.Reset();
	   GeometryShader.Reset();
pyramidConstantBuffer.Reset();
   pyramidIndexBuffer.Reset();
   pyramidInputLayout.Reset();
   pyramidPixelShader.Reset();
  pyramidVertexShader.Reset();
  pyramidVertexBuffer.Reset();
 
}