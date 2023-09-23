#include "../Engine/pch.h"
#include "../Engine/TimeManager.h"
#include "../Engine/Helper.h"

#include "DemoApp.h"
#include <DirectXMath.h>


#define USE_FLIPMODE 0			//경고 메세지를 없애려면 Flip 모드를 사용한다.

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
		XMMATRIX mSpin1 = XMMatrixRotationY(-t);
		XMMATRIX mTranslate1 = XMMatrixTranslation(ImGuiMenu::FirstCubePosition.x, ImGuiMenu::FirstCubePosition.y, ImGuiMenu::FirstCubePosition.z);

		// 2nd Cube
		XMMATRIX mSpin2 = XMMatrixRotationY(t * 5.0f);
		XMMATRIX mTranslate2 = XMMatrixTranslation(ImGuiMenu::SecondCubePosition.x, ImGuiMenu::SecondCubePosition.y, ImGuiMenu::SecondCubePosition.z);
		XMMATRIX mScale2 = XMMatrixScaling(0.5f, 0.5f, 0.5f);

		// 3rd Cube
		XMMATRIX mSpin3 = XMMatrixRotationY(-t * 2.0f);
		XMMATRIX mScale3 = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		XMMATRIX mTranslate3 = XMMatrixTranslation(ImGuiMenu::ThirdCubePosition.x, ImGuiMenu::ThirdCubePosition.y, ImGuiMenu::ThirdCubePosition.z);

		// Cube World Setting
		m_World1 = mSpin1 * mTranslate1;
		m_World2 = mScale2 * mSpin2 * mTranslate2 * static_cast<XMMATRIX>(m_World1);
		m_World3 = mScale3 * mSpin3 * mTranslate3 * static_cast<XMMATRIX>(m_World2);

		m_transformData1.World = XMMatrixTranspose(m_World1);
		m_transformData1.View = XMMatrixTranspose(m_View);
		m_transformData1.Projection = XMMatrixTranspose(m_Projection);

		m_transformData2.World = XMMatrixTranspose(m_World2);
		m_transformData2.View = XMMatrixTranspose(m_View);
		m_transformData2.Projection = XMMatrixTranspose(m_Projection);

		m_transformData3.World = XMMatrixTranspose(m_World3);
		m_transformData3.View = XMMatrixTranspose(m_View);
		m_transformData3.Projection = XMMatrixTranspose(m_Projection);
	}

	// Camera
	{
		XMVECTOR Eye = XMVectorSet(ImGuiMenu::CameraPos.x, ImGuiMenu::CameraPos.y, ImGuiMenu::CameraPos.z, ImGuiMenu::CameraPos.w);
		XMVECTOR At = XMVectorSet(ImGuiMenu::CameraPos.x, ImGuiMenu::CameraPos.y + 1.f, 100.f, 0.0f);
		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		m_View = XMMatrixLookAtLH(Eye, At, Up);
		m_Projection = XMMatrixPerspectiveFovLH(ImGuiMenu::CameraFov, m_ClientWidth / (FLOAT)m_ClientHeight, ImGuiMenu::CameraNearFar[0], ImGuiMenu::CameraNearFar[1]);
	}

	// Lighting
	{
		XMFLOAT4 LightNormal1 = ImGuiMenu::DirectionLightDir1;
		Color LightColor1 = ImGuiMenu::DirectionLightColor1;
		XMFLOAT4 LightNormal2 = ImGuiMenu::DirectionLightDir2;
		Color LightColor2 = ImGuiMenu::DirectionLightColor2;

		m_transformData1.LightDir[0] = LightNormal1;
		m_transformData2.LightDir[0] = LightNormal1;
		m_transformData3.LightDir[0] = LightNormal1;
		m_transformData1.LightColor[0] = LightColor1;
		m_transformData2.LightColor[0] = LightColor1;
		m_transformData3.LightColor[0] = LightColor1;
		m_transformData1.LightDir[1] = LightNormal2;
		m_transformData2.LightDir[1] = LightNormal2;
		m_transformData3.LightDir[1] = LightNormal2;
		m_transformData1.LightColor[1] = LightColor2;
		m_transformData2.LightColor[1] = LightColor2;
		m_transformData3.LightColor[1] = LightColor2;
	}
}

void DemoApp::Render()
{
	renderBegin();

	{
		uint32 stride = sizeof(Vertex);
		uint32 offset = 0;

		// IA
		m_DeviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
		m_DeviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		m_DeviceContext->IASetInputLayout(m_inputLayout.Get());
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// VS
		m_DeviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
		m_DeviceContext->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

		// RAS
		m_DeviceContext->RSSetState(m_rasterizerState.Get());

		// RS

		// PS
		m_DeviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
		m_DeviceContext->PSSetShaderResources(0, 1, m_shaderResourceView.GetAddressOf());
		m_DeviceContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
		m_DeviceContext->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

		// OM

		//
		m_DeviceContext->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &m_transformData1, 0, 0);
		m_DeviceContext->DrawIndexed(m_indices.size(), 0, 0);

		m_DeviceContext->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &m_transformData2, 0, 0);
		m_DeviceContext->DrawIndexed(m_indices.size(), 0, 0);

		m_DeviceContext->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &m_transformData3, 0, 0);
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
	m_DeviceContext->ClearDepthStencilView(m_depthStancilView.Get(), D3D11_CLEAR_DEPTH, 1.f, 0);	// 뎁스버퍼 1.0f로 초기화
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

	// 스왑체인 속성 설정 구조체를 초기화합니다.
	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(swapDesc));
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 백 버퍼의 픽셀 형식을 설정합니다.
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferDesc.Width = m_ClientWidth;					// 백 버퍼의 너비를 설정합니다.
	swapDesc.BufferDesc.Height = m_ClientHeight;				// 백 버퍼의 높이를 설정합니다.
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;				// 화면 주사율의 분자를 설정합니다.
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;			// 화면 주사율의 분모를 설정합니다.
	swapDesc.SampleDesc.Count = 1;								// 멀티 샘플링 수를 설정합니다.
	swapDesc.SampleDesc.Quality = 0;							// 멀티 샘플링 품질을 설정합니다.
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.BufferCount = 1;
	swapDesc.OutputWindow = m_hWnd;
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;


	// 1. 디바이스 생성. 2. 스왑체인 생성. 3. 디바이스 컨텍스트 생성.
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

	// 4. 렌더타겟뷰 생성 (백버퍼를 이용하는 렌더타겟뷰)
	ID3D11Texture2D* pBackBufferTexture = nullptr;
	hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture);
	assert(SUCCEEDED(hr));

	hr = m_Device->CreateRenderTargetView(pBackBufferTexture, NULL, m_RenderTargetView.GetAddressOf()); // 백 버퍼 텍스처를 렌더 타겟 뷰로 생성합니다.
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

		m_vertices[0].position = Vector3(-1.0f, 1.0f, -1.0f);	m_vertices[0].normal = Vector3(0.0f, 1.0f, 0.0f);  m_vertices[0].uv = Vector2(1.0f, 0.0f);
		m_vertices[1].position = Vector3(1.0f, 1.0f, -1.0f);	m_vertices[1].normal = Vector3(0.0f, 1.0f, 0.0f);	m_vertices[1].uv = Vector2(0.0f, 0.0f);
		m_vertices[2].position = Vector3(1.0f, 1.0f, 1.0f);		m_vertices[2].normal = Vector3(0.0f, 1.0f, 0.0f);	m_vertices[2].uv = Vector2(0.0f, 1.0f);
		m_vertices[3].position = Vector3(-1.0f, 1.0f, 1.0f);	m_vertices[3].normal = Vector3(0.0f, 1.0f, 0.0f);	m_vertices[3].uv = Vector2(1.0f, 1.0f);
		
		m_vertices[4].position = Vector3(-1.0f, -1.0f, -1.0f);	m_vertices[4].normal = Vector3(0.0f, -1.0f, 0.0f);	m_vertices[4].uv = Vector2(0.0f, 0.0f);
		m_vertices[5].position = Vector3(1.0f, -1.0f, -1.0f);	m_vertices[5].normal = Vector3(0.0f, -1.0f, 0.0f);	m_vertices[5].uv = Vector2(1.0f, 0.0f);
		m_vertices[6].position = Vector3(1.0f, -1.0f, 1.0f);	m_vertices[6].normal = Vector3(0.0f, -1.0f, 0.0f);	m_vertices[6].uv = Vector2(1.0f, 1.0f);
		m_vertices[7].position = Vector3(-1.0f, -1.0f, 1.0f);	m_vertices[7].normal = Vector3(0.0f, -1.0f, 0.0f);	m_vertices[7].uv = Vector2(0.0f, 1.0f);
		
		m_vertices[8].position = Vector3(-1.0f, -1.0f, 1.0f);	m_vertices[8].normal = Vector3(-1.0f, 0.0f, 0.0f);	m_vertices[8].uv = Vector2(0.0f, 1.0f);
		m_vertices[9].position = Vector3(-1.0f, -1.0f, -1.0f);	m_vertices[9].normal = Vector3(-1.0f, 0.0f, 0.0f);	m_vertices[9].uv = Vector2(1.0f, 1.0f);
		m_vertices[10].position = Vector3(-1.0f, 1.0f, -1.0f);	m_vertices[10].normal = Vector3(-1.0f, 0.0f, 0.0f); m_vertices[10].uv = Vector2(1.0f, 0.0f);
		m_vertices[11].position = Vector3(-1.0f, 1.0f, 1.0f);	m_vertices[11].normal = Vector3(-1.0f, 0.0f, 0.0f); m_vertices[11].uv = Vector2(0.0f, 0.0f);
		
		m_vertices[12].position = Vector3(1.0f, -1.0f, 1.0f);	m_vertices[12].normal = Vector3(1.0f, 0.0f, 0.0f);	m_vertices[12].uv = Vector2(1.0f, 1.0f);
		m_vertices[13].position = Vector3(1.0f, -1.0f, -1.0f);	m_vertices[13].normal = Vector3(1.0f, 0.0f, 0.0f);	m_vertices[13].uv = Vector2(0.0f, 1.0f);
		m_vertices[14].position = Vector3(1.0f, 1.0f, -1.0f);	m_vertices[14].normal = Vector3(1.0f, 0.0f, 0.0f);	m_vertices[14].uv = Vector2(0.0f, 0.0f);
		m_vertices[15].position = Vector3(1.0f, 1.0f, 1.0f);	m_vertices[15].normal = Vector3(1.0f, 0.0f, 0.0f);	m_vertices[15].uv = Vector2(1.0f, 0.0f);
		
		m_vertices[16].position = Vector3(-1.0f, -1.0f, -1.0f); m_vertices[16].normal = Vector3(0.0f, 0.0f, -1.0f); m_vertices[16].uv = Vector2(0.0f, 1.0f);
		m_vertices[17].position = Vector3(1.0f, -1.0f, -1.0f);	m_vertices[17].normal = Vector3(0.0f, 0.0f, -1.0f); m_vertices[17].uv = Vector2(1.0f, 1.0f);
		m_vertices[18].position = Vector3(1.0f, 1.0f, -1.0f);	m_vertices[18].normal = Vector3(0.0f, 0.0f, -1.0f); m_vertices[18].uv = Vector2(1.0f, 0.0f);
		m_vertices[19].position = Vector3(-1.0f, 1.0f, -1.0f);	m_vertices[19].normal = Vector3(0.0f, 0.0f, -1.0f); m_vertices[19].uv = Vector2(0.0f, 0.0f);

		m_vertices[20].position = Vector3(-1.0f, -1.0f, 1.0f);	m_vertices[20].normal = Vector3(0.0f, 0.0f, 1.0f);	m_vertices[20].uv = Vector2(1.0f, 1.0f);
		m_vertices[21].position = Vector3(1.0f, -1.0f, 1.0f);	m_vertices[21].normal = Vector3(0.0f, 0.0f, 1.0f);	m_vertices[21].uv = Vector2(0.0f, 1.0f);
		m_vertices[22].position = Vector3(1.0f, 1.0f, 1.0f);	m_vertices[22].normal = Vector3(0.0f, 0.0f, 1.0f);	m_vertices[22].uv = Vector2(0.0f, 0.0f);
		m_vertices[23].position = Vector3(-1.0f, 1.0f, 1.0f);	m_vertices[23].normal = Vector3(0.0f, 0.0f, 1.0f);	m_vertices[23].uv = Vector2(1.0f, 0.0f);
	}

	// VertexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.ByteWidth = (unsigned int)(sizeof(Vertex) * m_vertices.size());

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
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0}
	}; 

	const int count = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	m_Device->CreateInputLayout(layout, count, m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(), m_inputLayout.GetAddressOf());
}

void DemoApp::createConstantBuffer()
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT; // CPU_Write + GPU_Read
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = sizeof(ConstantData);
	desc.CPUAccessFlags = 0;

	HRESULT hr = m_Device->CreateBuffer(&desc, nullptr, m_constantBuffer.GetAddressOf());
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
	desc.FrontCounterClockwise = false;		// 정점이 시계방향으로 돌면서 삼각형을 그리면 정면이 앞부분으로 간다. true면 정면이 반대방향을 향한다.

	HRESULT hr = m_Device->CreateRasterizerState(&desc, m_rasterizerState.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void DemoApp::createSamplerState()
{
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.BorderColor[0] = 1;
	desc.BorderColor[1] = 0;
	desc.BorderColor[2] = 0;
	desc.BorderColor[3] = 1;
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	desc.MaxAnisotropy = 16;
	desc.MaxLOD = FLT_MAX;
	desc.MaxLOD = FLT_MIN;
	desc.MipLODBias = 0.0f;

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
	DirectX::TexMetadata md;
	DirectX::ScratchImage img;
	HRESULT hr = ::LoadFromWICFile(L"ground.jpg", WIC_FLAGS_NONE, &md, img);
	assert(SUCCEEDED(hr));

	hr = ::CreateShaderResourceView(m_Device.Get(), img.GetImages(), img.GetImageCount(), md, m_shaderResourceView.GetAddressOf());
	assert(SUCCEEDED(hr));
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
	m_World1 = XMMatrixIdentity();
	m_World2 = XMMatrixIdentity();
	m_DirectionLight = Vector4{1.f, 0.f, 0.f, 0.f};
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