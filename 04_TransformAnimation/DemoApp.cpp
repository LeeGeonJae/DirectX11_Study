#include "../Engine/pch.h"
#include "../Engine/TimeManager.h"

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

	createPS();

	createConstantBuffer();

	setTransform();
}

void DemoApp::Update()
{
	__super::Update();

	float t = GameTimer::m_Instance->TotalTime();

	// 1st Cube: Rotate around the origin
	XMMATRIX mSpin1 = XMMatrixRotationY(-t);
	XMMATRIX mTranslate1 = XMMatrixTranslation(m_imgui->GetFirstCubePosition().x, m_imgui->GetFirstCubePosition().y, m_imgui->GetFirstCubePosition().z);
	m_World1 = mSpin1 * mTranslate1;


	// 2nd Cube:  Rotate around origin
	XMMATRIX mSpin2 = XMMatrixRotationY(t * 5.0f);
	XMMATRIX mTranslate2 = XMMatrixTranslation(m_imgui->GetSecondCubePosition().x, m_imgui->GetSecondCubePosition().y, m_imgui->GetSecondCubePosition().z);
	XMMATRIX mScale2 = XMMatrixScaling(0.5f, 0.5f, 0.5f);

	XMMATRIX mSpin3 = XMMatrixRotationY(-t * 4.0f);
	XMMATRIX mScale3 = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	XMMATRIX mTranslate3 = XMMatrixTranslation(m_imgui->GetThirdCubePosition().x, m_imgui->GetThirdCubePosition().y, m_imgui->GetThirdCubePosition().z);

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


	XMVECTOR Eye = XMVectorSet(m_imgui->GetCameraPos()[0], m_imgui->GetCameraPos()[1], m_imgui->GetCameraPos()[2], m_imgui->GetCameraPos()[3]);
	XMVECTOR At = XMVectorSet (m_imgui->GetCameraPos()[0], m_imgui->GetCameraPos()[1] + 1.f, 100.f, 0.0f);
	XMVECTOR Up = XMVectorSet (0.0f, 1.0f, 0.0f, 0.0f);
	m_View = XMMatrixLookAtLH(Eye, At, Up);
	m_Projection = XMMatrixPerspectiveFovLH(m_imgui->GetCameraFov(), m_ClientWidth / (FLOAT)m_ClientHeight, m_imgui->GetCameraNear(), m_imgui->GetCameraFar());
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

		// RS

		// PS
		m_DeviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);

		// OM
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
		m_vertices.resize(8);

		m_vertices[0].position = Vector3(-1.0f, 1.0f, -1.0f);
		m_vertices[1].position = Vector3(1.0f, 1.0f, -1.0f);
		m_vertices[2].position = Vector3(1.0f, 1.0f, 1.0f);
		m_vertices[3].position = Vector3(-1.0f, 1.0f, 1.0f);
		m_vertices[4].position = Vector3(-1.0f, -1.0f, -1.0f);
		m_vertices[5].position = Vector3(1.0f, -1.0f, -1.0f);
		m_vertices[6].position = Vector3(1.0f, -1.0f, 1.0f);
		m_vertices[7].position = Vector3(-1.0f, -1.0f, 1.0f);
		m_vertices[0].color = Color(0.0f, 0.0f, 1.0f, 1.0f);
		m_vertices[1].color = Color(0.0f, 1.0f, 0.0f, 1.0f);
		m_vertices[2].color = Color(0.0f, 1.0f, 1.0f, 1.0f);
		m_vertices[3].color = Color(1.0f, 0.0f, 0.0f, 1.0f);
		m_vertices[4].color = Color(1.0f, 0.0f, 1.0f, 1.0f);
		m_vertices[5].color = Color(1.0f, 1.0f, 0.0f, 1.0f);
		m_vertices[6].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
		m_vertices[7].color = Color(0.0f, 0.0f, 0.0f, 1.0f);
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
			3,1,0,  2,1,3,
			0,5,4,  1,5,0,
			3,4,7,  0,4,3,
			1,6,5,  2,6,1,
			2,7,6,  3,7,2,
			6,4,5,  7,4,6,
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
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
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
	desc.ByteWidth = sizeof(TransformData);
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