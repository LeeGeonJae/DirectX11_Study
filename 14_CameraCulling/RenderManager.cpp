#include "RenderManager.h"

void RenderManager::Init(HWND _hwnd, UINT _clientHeight, UINT _clientWidth)
{
	m_hWnd = _hwnd;
	m_ClientHeight = _clientHeight;
	m_ClientWidth = _clientWidth;

	createDeviceAndSwapChain();
	createRenderTargetView();
	setViewport();
}

void RenderManager::createDeviceAndSwapChain()
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

void RenderManager::createRenderTargetView()
{
	HRESULT hr = 0;

	// 4. 렌더타겟뷰 생성 (백버퍼를 이용하는 렌더타겟뷰)
	ID3D11Texture2D* pBackBufferTexture = nullptr;
	hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture);
	assert(SUCCEEDED(hr));

	hr = m_Device->CreateRenderTargetView(pBackBufferTexture, NULL, m_RenderTargetView.GetAddressOf()); // 백 버퍼 텍스처를 렌더 타겟 뷰로 생성합니다.
	assert(SUCCEEDED(hr));
}

void RenderManager::setViewport()
{
	m_Viewport.TopLeftX = 0.f;
	m_Viewport.TopLeftY = 0.f;
	m_Viewport.Width = static_cast<float>(m_ClientWidth);
	m_Viewport.Height = static_cast<float>(m_ClientHeight);
	m_Viewport.MinDepth = 0.f;
	m_Viewport.MaxDepth = 1.f;
}

void RenderManager::createDeathStencilView()
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

void DemoApp::createRasterizeState()
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_NONE;
	desc.FrontCounterClockwise = false;		// 정점이 시계방향으로 돌면서 삼각형을 그리면 정면이 앞부분으로 간다. true면 정면이 반대방향을 향한다.

	HRESULT hr = m_Device->CreateRasterizerState(&desc, m_rasterizerState.GetAddressOf());
	assert(SUCCEEDED(hr));
}