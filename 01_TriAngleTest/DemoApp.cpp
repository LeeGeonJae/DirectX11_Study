#include "../Engine/pch.h"

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

	InitD3D();
}

void DemoApp::Update()
{
}

void DemoApp::Render()
{
	renderBegin();

	{
		uint32 stride = sizeof(Vertex);
		uint32 offset = 0;

		// IA
		m_pDeviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
		m_pDeviceContext->IASetInputLayout(m_inputLayout.Get());
		m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// VS
		m_pDeviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);

		// RS

		// PS
		m_pDeviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);

		// OM
		m_pDeviceContext->Draw(m_vertices.size(), 0);
	}

	renderEnd();
}

void DemoApp::renderBegin()
{
	float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.f };

	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, clearColor);
	m_pDeviceContext->RSSetViewports(1, &m_Viewport);
}

void DemoApp::renderEnd()
{
	HRESULT hr = m_pSwapChain->Present(1, 0);
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
	createPS();
}

void DemoApp::createDeviceAndSwapChain()
{
	HRESULT hr = 0;

	// 스왑체인 속성 설정 구조체를 초기화합니다.
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
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
		&m_pSwapChain,
		&m_pDevice,
		nullptr,
		&m_pDeviceContext);

	assert(SUCCEEDED(hr));
}

void DemoApp::createRenderTargetView()
{
	HRESULT hr = 0;

	// 4. 렌더타겟뷰 생성 (백버퍼를 이용하는 렌더타겟뷰)
	ID3D11Texture2D* pBackBufferTexture = nullptr;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture);
	assert(SUCCEEDED(hr));

	hr = m_pDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &m_pRenderTargetView); // 백 버퍼 텍스처를 렌더 타겟 뷰로 생성합니다.
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
	{
		m_vertices.resize(3);

		m_vertices[0].position = Vector3(-0.5f, -0.5f, 0.f);
		m_vertices[0].color = Color(1.f, 0.f, 0.f, 1.f);
		m_vertices[1].position = Vector3(0.0f, 0.5f, 0.f);
		m_vertices[1].color = Color(0.f, 1.f, 0.f, 1.f);
		m_vertices[2].position = Vector3(0.5f, -0.5f, 0.f);
		m_vertices[2].color = Color(0.f, 0.f, 1.f, 1.f);
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

		m_pDevice->CreateBuffer(&desc, &data, m_vertexBuffer.GetAddressOf());
	}
}

void DemoApp::createInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	const int count = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	m_pDevice->CreateInputLayout(layout, count, m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(), m_inputLayout.GetAddressOf());
}

void DemoApp::createVS()
{
	LoadShaderFromFile(L"Defalut.hlsl", "VS", "vs_5_0", m_vsBlob);
	HRESULT hr = m_pDevice->CreateVertexShader(m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void DemoApp::createPS()
{
	LoadShaderFromFile(L"Defalut.hlsl", "PS", "ps_5_0", m_psBlob);
	HRESULT hr = m_pDevice->CreatePixelShader(m_psBlob->GetBufferPointer(), m_psBlob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf());
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
