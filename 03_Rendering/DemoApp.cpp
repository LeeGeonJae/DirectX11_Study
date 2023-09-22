#include "../Engine/pch.h"

#include "DemoApp.h"
#include <DirectXMath.h>

#define USE_FLIPMODE 0			//��� �޼����� ���ַ��� Flip ��带 ����Ѵ�.

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

	InitD3D();
}

void DemoApp::Update()
{
	// ��ȯ ������ ������Ʈ: x�� y ��ǥ�� ���� 0.01f�� ������ŵ�ϴ�.
	//m_transformData.offset.x += 0.01f;
	//m_transformData.offset.y += 0.01f;

	// ��� ���� ���� �� ������ ����
	D3D11_MAPPED_SUBRESOURCE subResource;
	ZeroMemory(&subResource, sizeof(subResource));

	// ��� ���۸� �����Ͽ� CPU���� GPU�� �����͸� ������ �� �ִ� ���·� ����ϴ�.
	// - m_constantBuffer.Get(): ������ ��� ���� ��ü
	// - 0: ���긮�ҽ� ��ȣ (���� ��� �����̹Ƿ� 0�� ���)
	// - D3D11_MAP_WRITE_DISCARD: ���۸� ���� ���� �����ϰ�, ���� �����ʹ� �����մϴ�.
	// - 0: CPU�� GPU�� ����ȭ �ɼ� (0�� ����Ͽ� �⺻ ����ȭ ����)
	// - &subResource: ���ε� �����Ϳ� ���� ������ ���� ����ü
	m_DeviceContext->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);

	// ��ȯ ������(m_transformData)�� ��� ���ۿ� �����մϴ�.
	// - subResource.pData: ���ε� �������� �����͸� ���ͼ� �����͸� �����մϴ�.
	// - &m_transformData: ������ ������
	// - sizeof(m_transformData): ������ �������� ũ��
	memcpy(subResource.pData, &m_transformData, sizeof(m_transformData));

	// ��� ������ ������ �����մϴ�.
	m_DeviceContext->Unmap(m_constantBuffer.Get(), 0);
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
		m_DeviceContext->PSSetShaderResources(0, 1, m_shaderResoureceView.GetAddressOf());
		m_DeviceContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

		// OM
		m_DeviceContext->OMSetBlendState(m_blendState.Get(), nullptr, 0xFFFFFFFF);
		
		m_DeviceContext->DrawIndexed(m_indices.size(), 0, 0);
	}

	renderEnd();
}

void DemoApp::renderBegin()
{
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.f };

	m_DeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), nullptr);
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), clearColor);
	m_DeviceContext->RSSetViewports(1, &m_Viewport);
}

void DemoApp::renderEnd()
{
	HRESULT hr = m_SwapChain->Present(1, 0);
	assert(SUCCEEDED(hr));
}

void DemoApp::InitD3D()
{
	createDeviceAndSwapChain();
	createRenderTargetView();
	setViewport();

	createGeometry();
	createVS();
	createInputLayout();

	createRasterizerState();

	createPS();

	createSRV();
	createConstantBuffer();
	createSamplerState();
	createBlendState();
}

void DemoApp::createDeviceAndSwapChain()
{
	HRESULT hr = 0;

	// ����ü�� �Ӽ� ���� ����ü�� �ʱ�ȭ�մϴ�.
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
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

void DemoApp::createGeometry()
{
	HRESULT hr;

	// vertex
	{
		m_vertices.resize(4);

		m_vertices[0].position = Vector3(-0.5f, -0.5f, 0.f);
		m_vertices[0].uv = Vector2(0.f, 1.f);
		m_vertices[0].color = Color(1.f, 0.f, 0.f, 1.f);
		m_vertices[1].position = Vector3(-0.5f, 0.5f, 0.f);
		m_vertices[1].uv = Vector2(0.f, 0.f);
		m_vertices[1].color = Color(0.f, 1.f, 0.f, 1.f);
		m_vertices[2].position = Vector3(0.5f, -0.5f, 0.f);
		m_vertices[2].uv = Vector2(1.f, 1.f);
		m_vertices[2].color = Color(0.f, 0.f, 1.f, 1.f);
		m_vertices[3].position = Vector3(0.5f, 0.5f, 0.f);
		m_vertices[3].uv = Vector2(1.f, 0.f);
		m_vertices[3].color = Color(0.f, 0.f, 1.f, 0.f);
	}

	// VertexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.ByteWidth = (unsigned int)(sizeof(Vertex) * m_vertices.size());

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = m_vertices.data();

		hr = m_Device->CreateBuffer(&desc, &data, m_vertexBuffer.GetAddressOf());
		assert(SUCCEEDED(hr));
	}

	// index
	{
		m_indices = { 0, 1, 2, 2, 1, 3 };
	}

	// indexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
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

void DemoApp::createConstantBuffer()
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DYNAMIC; // CPU_Write + GPU_Read
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = sizeof(TransformData);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	 
	HRESULT hr = m_Device->CreateBuffer(&desc, nullptr, m_constantBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void DemoApp::createRasterizerState()
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = false;		// ������ �ð�������� ���鼭 �ﰢ���� �׸��� ������ �պκ����� ����. true�� ������ �ݴ������ ���Ѵ�.
	
	HRESULT hr = m_Device->CreateRasterizerState(&desc, m_rasterizerState.GetAddressOf());
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

	m_Device->CreateSamplerState(&desc, m_samplerState.GetAddressOf());
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

	m_Device->CreateBlendState(&desc, m_blendState.GetAddressOf());
}

void DemoApp::createSRV()
{
	DirectX::TexMetadata md;
	DirectX::ScratchImage img;
	HRESULT hr = ::LoadFromWICFile(L"Monster.png", WIC_FLAGS_NONE, &md, img);
	assert(SUCCEEDED(hr));

	hr = ::CreateShaderResourceView(m_Device.Get(), img.GetImages(), img.GetImageCount(), md, m_shaderResoureceView.GetAddressOf());
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
