#include "../Engine/pch.h"
#include "../Engine/TimeManager.h"
#include "../Engine/Helper.h"

#include "DemoApp.h"
#include <DirectXMath.h>
#include <Directxtk/DDSTextureLoader.h>


#define USE_FLIPMODE 0

DemoApp::DemoApp(HINSTANCE hInstance)
	:GameApp::GameApp(hInstance)
{
}

DemoApp::~DemoApp()
{
}

void DemoApp::Initialize(UINT Width, UINT Height)
{
	__super::Initialize(Width, Height);

	initD3D();
	initScene();

	m_imgui = new ImGuiMenu(this);
	m_imgui->Init(m_hWnd, m_Device.Get(), m_DeviceContext.Get());
}

void DemoApp::initD3D()
{
	createDeviceAndSwapChain();
	createRenderTargetView();
	setViewport();
	createDeathStencilView();
}

void DemoApp::initScene()
{
	createGeometry();
	createVS();
	createInputLayout();

	createRasterizeState();
	createSamplerState();
	createBlendState();
	createSRV();

	createPS();

	createConstantBuffer();

	setTransform();
}

void DemoApp::Update()
{
	__super::Update();

	float t = GameTimer::m_Instance->TotalTime();

	// Cube
	{
		// 1st Cube
		XMMATRIX mSpin1 = XMMatrixRotationX(ImGuiMenu::CubeRotation.x * 3.14f);
		XMMATRIX mSpin2 = XMMatrixRotationY(ImGuiMenu::CubeRotation.y * 3.14f);
		XMMATRIX mScale = XMMatrixScaling(ImGuiMenu::CubeScale.x, ImGuiMenu::CubeScale.y, ImGuiMenu::CubeScale.z);
		XMMATRIX mTranslate1 = XMMatrixTranslation(ImGuiMenu::CubePosition.x, ImGuiMenu::CubePosition.y, ImGuiMenu::CubePosition.z);

		// Cube World Setting
		m_World = mScale * mSpin1 * mSpin2 * mTranslate1;

		m_CBChangesEveryFrame.World = XMMatrixTranspose(m_World);
		m_CBChangesEveryFrame.View = XMMatrixTranspose(m_View);
		m_CBChangesEveryFrame.Projection = XMMatrixTranspose(m_Projection);
	}

	// Camera
	{
		XMVECTOR Eye = XMVectorSet(ImGuiMenu::CameraPos.x, ImGuiMenu::CameraPos.y, ImGuiMenu::CameraPos.z, ImGuiMenu::CameraPos.w);
		XMVECTOR At = XMVectorSet(ImGuiMenu::CameraPos.x, ImGuiMenu::CameraPos.y + 1.f, 100.f, 0.0f);
		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		m_View = XMMatrixLookAtLH(Eye, At, Up);
		m_Projection = XMMatrixPerspectiveFovLH(ImGuiMenu::CameraFov, m_ClientWidth / (FLOAT)m_ClientHeight, ImGuiMenu::CameraNearFar[0], ImGuiMenu::CameraNearFar[1]);
		m_CBCamera.CameraPosition = Vector4{ ImGuiMenu::CameraPos.x, ImGuiMenu::CameraPos.y, ImGuiMenu::CameraPos.z, 0.f };
	}

	// Lighting
	{
		XMFLOAT4 LightDirection = ImGuiMenu::DirectionLightDir;
		Color LightColor = ImGuiMenu::DirectionLightColor;
		Vector3 AmbientColor = ImGuiMenu::AmbientColor;
		float SpeclarPower = ImGuiMenu::SpecularPower;

		m_CBLightData.LightDir = LightDirection;
		m_CBLightData.LightColor = LightColor;
		m_CBLightData.AmbientColor = AmbientColor;
		m_CBLightData.SpecularPower = SpeclarPower;
	}

	// NormalMap
	{
		m_CBNormalMap.UseNormalMap = ImGuiMenu::bIsNormalMap;
		m_CBNormalMap.UseSpecularMap = ImGuiMenu::bIsSpecularMap;
		m_CBNormalMap.UseGammaCorrection = ImGuiMenu::bIsGammaCorrection;
	}
}

void DemoApp::Render()
{
	renderBegin();

	{
		uint32 stride = sizeof(ShaderVertex);
		uint32 offset = 0;

		// IA
		m_DeviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
		m_DeviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		m_DeviceContext->IASetInputLayout(m_inputLayout.Get());
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// VS
		m_DeviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
		m_DeviceContext->VSSetConstantBuffers(0, 1, m_pCBChangesEveryFrame.GetAddressOf());
		m_DeviceContext->VSSetConstantBuffers(1, 1, m_pCBLight.GetAddressOf());
		m_DeviceContext->VSSetConstantBuffers(2, 1, m_pCBCamera.GetAddressOf());
		m_DeviceContext->VSSetConstantBuffers(3, 1, m_pCBNormalMap.GetAddressOf());

		// RAS
		m_DeviceContext->RSSetState(m_rasterizerState.Get());

		// RS

		// PS
		m_DeviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
		m_DeviceContext->PSSetShaderResources(0, 1, m_shaderResourceView1.GetAddressOf());
		m_DeviceContext->PSSetShaderResources(1, 1, m_shaderResourceView2.GetAddressOf());
		m_DeviceContext->PSSetShaderResources(2, 1, m_shaderResourceView3.GetAddressOf());
		m_DeviceContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
		m_DeviceContext->PSSetConstantBuffers(0, 1, m_pCBChangesEveryFrame.GetAddressOf());
		m_DeviceContext->PSSetConstantBuffers(1, 1, m_pCBLight.GetAddressOf());
		m_DeviceContext->PSSetConstantBuffers(2, 1, m_pCBCamera.GetAddressOf());
		m_DeviceContext->PSSetConstantBuffers(3, 1, m_pCBNormalMap.GetAddressOf());

		// OM

		//
		m_DeviceContext->UpdateSubresource(m_pCBChangesEveryFrame.Get(), 0, nullptr, &m_CBChangesEveryFrame, 0, 0);
		m_DeviceContext->UpdateSubresource(m_pCBLight.Get(), 0, nullptr, &m_CBLightData, 0, 0);
		m_DeviceContext->UpdateSubresource(m_pCBCamera.Get(), 0, nullptr, &m_CBCamera, 0, 0);
		m_DeviceContext->UpdateSubresource(m_pCBNormalMap.Get(), 0, nullptr, &m_CBNormalMap, 0, 0);
		m_DeviceContext->DrawIndexed(m_indices.size(), 0, 0);
	}

	// ImGUI
	{
		m_imgui->Render();
	}

	renderEnd();
}


void DemoApp::renderBegin()
{
	float clearColor[4] = { 0.0f, 0.5f, 0.5f, 1.f };

	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), clearColor);
	m_DeviceContext->ClearDepthStencilView(m_depthStancilView.Get(), D3D11_CLEAR_DEPTH, 1.f, 0);	// �������� 1.0f�� �ʱ�ȭ
	m_DeviceContext->RSSetViewports(1, &m_Viewport);
	m_DeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_depthStancilView.Get());
}

void DemoApp::renderEnd()
{
	HRESULT hr = m_SwapChain->Present(1, 0);
	assert(SUCCEEDED(hr));
}

void DemoApp::createDeviceAndSwapChain()
{
	HRESULT hr = 0;

	// ����ü�� �Ӽ� ���� ����ü�� �ʱ�ȭ�մϴ�.
	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(swapDesc));
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// �� ������ �ȼ� ������ �����մϴ�.
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferDesc.Width = m_ClientWidth;					// �� ������ �ʺ� �����մϴ�.
	swapDesc.BufferDesc.Height = m_ClientHeight;				// �� ������ ���̸� �����մϴ�.
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;				// ȭ�� �ֻ����� ���ڸ� �����մϴ�.
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;			// ȭ�� �ֻ����� �и� �����մϴ�.
	swapDesc.SampleDesc.Count = 1;								// ��Ƽ ���ø� ���� �����մϴ�.
	swapDesc.SampleDesc.Quality = 0;							// ��Ƽ ���ø� ǰ���� �����մϴ�.
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.BufferCount = 1;
	swapDesc.OutputWindow = m_hWnd;
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;


	// 1. ����̽� ����. 2. ����ü�� ����. 3. ����̽� ���ؽ�Ʈ ����.
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapDesc,
		m_SwapChain.GetAddressOf(),
		m_Device.GetAddressOf(),
		nullptr,
		m_DeviceContext.GetAddressOf());

	assert(SUCCEEDED(hr));
}

void DemoApp::createRenderTargetView()
{
	HRESULT hr = 0;

	// 4. ����Ÿ�ٺ� ���� (����۸� �̿��ϴ� ����Ÿ�ٺ�)
	ID3D11Texture2D* pBackBufferTexture = nullptr;
	hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture);
	assert(SUCCEEDED(hr));

	hr = m_Device->CreateRenderTargetView(pBackBufferTexture, NULL, m_RenderTargetView.GetAddressOf()); // �� ���� �ؽ�ó�� ���� Ÿ�� ��� �����մϴ�.
	assert(SUCCEEDED(hr));
}

void DemoApp::setViewport()
{
	m_Viewport.TopLeftX = 0.f;
	m_Viewport.TopLeftY = 0.f;
	m_Viewport.Width = static_cast<float>(m_ClientWidth);
	m_Viewport.Height = static_cast<float>(m_ClientHeight);
	m_Viewport.MinDepth = 0.f;
	m_Viewport.MaxDepth = 1.f;
}

void DemoApp::createDeathStencilView()
{
	HRESULT hr;

	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(D3D11_TEXTURE2D_DESC));
	descDepth.Width = m_ClientWidth;
	descDepth.Height = m_ClientHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	ID3D11Texture2D* textureDepthStancil = nullptr;
	hr = m_Device->CreateTexture2D(&descDepth, nullptr, &textureDepthStancil);
	assert(SUCCEEDED(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	hr = m_Device->CreateDepthStencilView(textureDepthStancil, &descDSV, m_depthStancilView.GetAddressOf());
	assert(SUCCEEDED(hr));

	assert(textureDepthStancil != nullptr);
	textureDepthStancil->Release();
}

void DemoApp::createGeometry()
{
	HRESULT hr;

	// vertex
	{
		m_vertices.resize(24);

		m_vertices[0].Position = Vector3(-1.0f, 1.0f, -1.0f);	m_vertices[0].Normal = Vector3(0.0f, 1.0f, 0.0f);  m_vertices[0].UV = Vector2(1.0f, 0.0f);
		m_vertices[1].Position = Vector3(1.0f, 1.0f, -1.0f);	m_vertices[1].Normal = Vector3(0.0f, 1.0f, 0.0f);	m_vertices[1].UV = Vector2(0.0f, 0.0f);
		m_vertices[2].Position = Vector3(1.0f, 1.0f, 1.0f);		m_vertices[2].Normal = Vector3(0.0f, 1.0f, 0.0f);	m_vertices[2].UV = Vector2(0.0f, 1.0f);
		m_vertices[3].Position = Vector3(-1.0f, 1.0f, 1.0f);	m_vertices[3].Normal = Vector3(0.0f, 1.0f, 0.0f);	m_vertices[3].UV = Vector2(1.0f, 1.0f);

		m_vertices[4].Position = Vector3(-1.0f, -1.0f, -1.0f);	m_vertices[4].Normal = Vector3(0.0f, -1.0f, 0.0f);	m_vertices[4].UV = Vector2(0.0f, 0.0f);
		m_vertices[5].Position = Vector3(1.0f, -1.0f, -1.0f);	m_vertices[5].Normal = Vector3(0.0f, -1.0f, 0.0f);	m_vertices[5].UV = Vector2(1.0f, 0.0f);
		m_vertices[6].Position = Vector3(1.0f, -1.0f, 1.0f);	m_vertices[6].Normal = Vector3(0.0f, -1.0f, 0.0f);	m_vertices[6].UV = Vector2(1.0f, 1.0f);
		m_vertices[7].Position = Vector3(-1.0f, -1.0f, 1.0f);	m_vertices[7].Normal = Vector3(0.0f, -1.0f, 0.0f);	m_vertices[7].UV = Vector2(0.0f, 1.0f);

		m_vertices[8].Position = Vector3(-1.0f, -1.0f, 1.0f);	m_vertices[8].Normal = Vector3(-1.0f, 0.0f, 0.0f);	m_vertices[8].UV = Vector2(0.0f, 1.0f);
		m_vertices[9].Position = Vector3(-1.0f, -1.0f, -1.0f);	m_vertices[9].Normal = Vector3(-1.0f, 0.0f, 0.0f);	m_vertices[9].UV = Vector2(1.0f, 1.0f);
		m_vertices[10].Position = Vector3(-1.0f, 1.0f, -1.0f);	m_vertices[10].Normal = Vector3(-1.0f, 0.0f, 0.0f); m_vertices[10].UV = Vector2(1.0f, 0.0f);
		m_vertices[11].Position = Vector3(-1.0f, 1.0f, 1.0f);	m_vertices[11].Normal = Vector3(-1.0f, 0.0f, 0.0f); m_vertices[11].UV = Vector2(0.0f, 0.0f);

		m_vertices[12].Position = Vector3(1.0f, -1.0f, 1.0f);	m_vertices[12].Normal = Vector3(1.0f, 0.0f, 0.0f);	m_vertices[12].UV = Vector2(1.0f, 1.0f);
		m_vertices[13].Position = Vector3(1.0f, -1.0f, -1.0f);	m_vertices[13].Normal = Vector3(1.0f, 0.0f, 0.0f);	m_vertices[13].UV = Vector2(0.0f, 1.0f);
		m_vertices[14].Position = Vector3(1.0f, 1.0f, -1.0f);	m_vertices[14].Normal = Vector3(1.0f, 0.0f, 0.0f);	m_vertices[14].UV = Vector2(0.0f, 0.0f);
		m_vertices[15].Position = Vector3(1.0f, 1.0f, 1.0f);	m_vertices[15].Normal = Vector3(1.0f, 0.0f, 0.0f);	m_vertices[15].UV = Vector2(1.0f, 0.0f);

		m_vertices[16].Position = Vector3(-1.0f, -1.0f, -1.0f); m_vertices[16].Normal = Vector3(0.0f, 0.0f, -1.0f); m_vertices[16].UV = Vector2(0.0f, 1.0f);
		m_vertices[17].Position = Vector3(1.0f, -1.0f, -1.0f);	m_vertices[17].Normal = Vector3(0.0f, 0.0f, -1.0f); m_vertices[17].UV = Vector2(1.0f, 1.0f);
		m_vertices[18].Position = Vector3(1.0f, 1.0f, -1.0f);	m_vertices[18].Normal = Vector3(0.0f, 0.0f, -1.0f); m_vertices[18].UV = Vector2(1.0f, 0.0f);
		m_vertices[19].Position = Vector3(-1.0f, 1.0f, -1.0f);	m_vertices[19].Normal = Vector3(0.0f, 0.0f, -1.0f); m_vertices[19].UV = Vector2(0.0f, 0.0f);

		m_vertices[16].Tangent = Vector3(1.f, 0.f, 0.f);
		m_vertices[17].Tangent = Vector3(1.f, 0.f, 0.f);
		m_vertices[18].Tangent = Vector3(1.f, 0.f, 0.f);
		m_vertices[19].Tangent = Vector3(1.f, 0.f, 0.f);

		m_vertices[20].Position = Vector3(-1.0f, -1.0f, 1.0f);	m_vertices[20].Normal = Vector3(0.0f, 0.0f, 1.0f);	m_vertices[20].UV = Vector2(1.0f, 1.0f);
		m_vertices[21].Position = Vector3(1.0f, -1.0f, 1.0f);	m_vertices[21].Normal = Vector3(0.0f, 0.0f, 1.0f);	m_vertices[21].UV = Vector2(0.0f, 1.0f);
		m_vertices[22].Position = Vector3(1.0f, 1.0f, 1.0f);	m_vertices[22].Normal = Vector3(0.0f, 0.0f, 1.0f);	m_vertices[22].UV = Vector2(0.0f, 0.0f);
		m_vertices[23].Position = Vector3(-1.0f, 1.0f, 1.0f);	m_vertices[23].Normal = Vector3(0.0f, 0.0f, 1.0f);	m_vertices[23].UV = Vector2(1.0f, 0.0f);
	}

	// VertexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.ByteWidth = (unsigned int)(sizeof(ShaderVertex) * m_vertices.size());

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = m_vertices.data();

		hr = m_Device->CreateBuffer(&desc, &data, m_vertexBuffer.GetAddressOf());
		assert(SUCCEEDED(hr));
	}

	// index
	{
		m_indices =
		{
		3,1,0, 2,1,3,
		6,4,5, 7,4,6,
		11,9,8, 10,9,11,
		14,12,13, 15,12,14,
		19,17,16, 18,17,19,
		22,20,21, 23,20,22
		};
	}

	// indexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.ByteWidth = (unsigned int)(sizeof(uint32) * m_indices.size());

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = m_indices.data();

		hr = m_Device->CreateBuffer(&desc, &data, m_indexBuffer.GetAddressOf());
		assert(SUCCEEDED(hr));
	}
}

void DemoApp::createInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	const int count = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	m_Device->CreateInputLayout(layout, count, m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(), m_inputLayout.GetAddressOf());
}

void DemoApp::createConstantBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC desc;

	// ��ǥ(World, View, Projection) ������ Constant Buffer�� �Ѱ��ֱ�
	desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(CBCoordinateData);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = m_Device->CreateBuffer(&desc, nullptr, m_pCBChangesEveryFrame.GetAddressOf());
	assert(SUCCEEDED(hr));

	// Light ������ Constant Buffer�� �Ѱ��ֱ�
	desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(CBLightData);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = m_Device->CreateBuffer(&desc, nullptr, m_pCBLight.GetAddressOf());
	assert(SUCCEEDED(hr));

	// Camera ������ Constant Buffer�� �Ѱ��ֱ�
	desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(CBCameraData);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = m_Device->CreateBuffer(&desc, nullptr, m_pCBCamera.GetAddressOf());
	assert(SUCCEEDED(hr));

	// Camera ������ Constant Buffer�� �Ѱ��ֱ�
	desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(CBNormalMap);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = m_Device->CreateBuffer(&desc, nullptr, m_pCBNormalMap.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void DemoApp::createVS()
{
	LoadShaderFromFile(L"Defalut.hlsl", "VS", "vs_5_0", m_vsBlob);
	HRESULT hr = m_Device->CreateVertexShader(m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void DemoApp::createPS()
{
	LoadShaderFromFile(L"Defalut.hlsl", "PS", "ps_5_0", m_psBlob);
	HRESULT hr = m_Device->CreatePixelShader(m_psBlob->GetBufferPointer(), m_psBlob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void DemoApp::createRasterizeState()
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = false;		// ������ �ð�������� ���鼭 �ﰢ���� �׸��� ������ �պκ����� ����. true�� ������ �ݴ������ ���Ѵ�.

	HRESULT hr = m_Device->CreateRasterizerState(&desc, m_rasterizerState.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void DemoApp::createSamplerState()
{
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//desc.BorderColor[0] = 1;
	//desc.BorderColor[1] = 0;
	//desc.BorderColor[2] = 0;
	//desc.BorderColor[3] = 1;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	//desc.MaxAnisotropy = 16;
	desc.MaxLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	//desc.MipLODBias = 0.0f;

	HRESULT hr = m_Device->CreateSamplerState(&desc, m_samplerState.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void DemoApp::createBlendState()
{
	D3D11_BLEND_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;

	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HRESULT hr = m_Device->CreateBlendState(&desc, m_blendState.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void DemoApp::createSRV()
{
	HRESULT hr;
	DirectX::TexMetadata md;
	DirectX::ScratchImage img;
	hr = ::LoadFromWICFile(L"Wall.jpg", WIC_FLAGS_NONE, &md, img);
	assert(SUCCEEDED(hr));

	hr = ::CreateShaderResourceView(m_Device.Get(), img.GetImages(), img.GetImageCount(), md, m_shaderResourceView1.GetAddressOf());
	assert(SUCCEEDED(hr));

	hr = ::LoadFromWICFile(L"Wall_Normal.jpg", WIC_FLAGS_NONE, &md, img);
	assert(SUCCEEDED(hr));

	hr = ::CreateShaderResourceView(m_Device.Get(), img.GetImages(), img.GetImageCount(), md, m_shaderResourceView2.GetAddressOf());
	assert(SUCCEEDED(hr));

	hr = ::LoadFromWICFile(L"Wall_Specular.png", WIC_FLAGS_NONE, &md, img);
	assert(SUCCEEDED(hr));

	hr = ::CreateShaderResourceView(m_Device.Get(), img.GetImages(), img.GetImageCount(), md, m_shaderResourceView3.GetAddressOf());
	assert(SUCCEEDED(hr));

	//hr = ::CreateDDSTextureFromFile(m_Device.Get(), L"seafloor.dds", nullptr, m_shaderResourceView.GetAddressOf());
	//assert(SUCCEEDED(hr));
}


void DemoApp::LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob)
{
	const uint32 compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	HRESULT hr = ::D3DCompileFromFile(
		path.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		name.c_str(),
		version.c_str(),
		compileFlag,
		0,
		blob.GetAddressOf(),
		nullptr);

	assert(SUCCEEDED(hr));
}

void DemoApp::setTransform()
{
	m_World = XMMatrixIdentity();
	m_DirectionLight = Vector4{ 1.f, 0.f, 0.f, 0.f };
}

void DemoApp::Release()
{
	if (m_imgui)
	{
		delete m_imgui;
		m_imgui = nullptr;
	}

	m_RenderTargetView.ReleaseAndGetAddressOf();
	m_SwapChain.ReleaseAndGetAddressOf();
	m_DeviceContext.ReleaseAndGetAddressOf();
	m_Device.ReleaseAndGetAddressOf();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK DemoApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	return __super::WndProc(hWnd, message, wParam, lParam);
}