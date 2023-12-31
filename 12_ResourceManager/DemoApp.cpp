#include "../Engine/pch.h"
#include "../Engine/TimeManager.h"
#include "../Engine/Helper.h"

#include "DemoApp.h"
#include <DirectXMath.h>
#include <Directxtk/DDSTextureLoader.h>

#include "ImGuiMenu.h"
#include "ModelLoadManager.h"
#include "ResourceManager.h"
#include "Model.h"


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

	m_imgui = make_shared<ImGuiMenu>(this);
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
	ResourceManager::GetInstance()->Init(m_Device, m_DeviceContext);
	ModelLoadManager::GetInstance()->Init(m_Device, m_DeviceContext);

	createRasterizeState();
	createSamplerState();
	createBlendState();

	createConstantBuffer();

	setTransform();
}

void DemoApp::Update()
{
	__super::Update();

	if (ImGuiMenu::SkinningTestFBX)
	{
		shared_ptr<Model> model = ResourceManager::GetInstance()->CreateModel("../Resource/Texture/SkinningTest.fbx");
		model->Init(m_Device.Get(), m_ModelCBBuffer);
		myModels.push_back(model);
	}
	else if (ImGuiMenu::DyingAnimationFBX)
	{
		shared_ptr<Model> model = ResourceManager::GetInstance()->CreateModel("../Resource/Texture/Dying.fbx");
		model->Init(m_Device.Get(), m_ModelCBBuffer);
		myModels.push_back(model);
	}
	else if (ImGuiMenu::ZeldaFBX)
	{
		shared_ptr<Model> model = ResourceManager::GetInstance()->CreateModel("../Resource/Texture/zeldaPosed001.fbx");
		model->Init(m_Device.Get(), m_ModelCBBuffer);
		myModels.push_back(model);
	}
	else if (ImGuiMenu::VampireFBX)
	{
		shared_ptr<Model> model = ResourceManager::GetInstance()->CreateModel("../Resource/Texture/Vampire.fbx");
		model->Init(m_Device.Get(), m_ModelCBBuffer);
		myModels.push_back(model);
	}
	else if (ImGuiMenu::cerberusFBX)
	{
		shared_ptr<Model> model = ResourceManager::GetInstance()->CreateModel("../Resource/Texture/cerberus_test.fbx");
		model->Init(m_Device.Get(), m_ModelCBBuffer);
		myModels.push_back(model);
	}

	// Camera
	{
		XMVECTOR Eye = XMVectorSet(ImGuiMenu::CameraPos.x, ImGuiMenu::CameraPos.y, ImGuiMenu::CameraPos.z, ImGuiMenu::CameraPos.w);
		XMVECTOR At = XMVectorSet(ImGuiMenu::CameraPos.x, ImGuiMenu::CameraPos.y + 1.f, 100.f, 0.0f);
		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		m_View = XMMatrixLookAtLH(Eye, At, Up);
		m_Projection = XMMatrixPerspectiveFovLH(ImGuiMenu::CameraFov, m_ClientWidth / (FLOAT)m_ClientHeight, ImGuiMenu::CameraNearFar[0], ImGuiMenu::CameraNearFar[1]);

		m_CBCamera.CameraPosition = Vector4{ ImGuiMenu::CameraPos.x, ImGuiMenu::CameraPos.y, ImGuiMenu::CameraPos.z, 0.f };
		m_CBCoordinateData.View = XMMatrixTranspose(m_View);
		m_CBCoordinateData.Projection = XMMatrixTranspose(m_Projection);
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
		m_CBNormalMap.UseEmissiveMap = ImGuiMenu::bIsEmissiveMap;
		m_CBNormalMap.UseGammaCorrection = ImGuiMenu::bIsGammaCorrection;
	}
}

void DemoApp::Render()
{
	renderBegin();

	{
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		static bool tem = false;

		if (GetAsyncKeyState('A') & 0x8000)
		{
			tem = !tem;
		}
		if (tem == true)
		{
			m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		}
		// VS

		m_DeviceContext->VSSetConstantBuffers(0, 1, m_pCBCoordinateData.GetAddressOf());
		m_DeviceContext->VSSetConstantBuffers(1, 1, m_pCBLight.GetAddressOf());
		m_DeviceContext->VSSetConstantBuffers(2, 1, m_pCBCamera.GetAddressOf());
		m_DeviceContext->VSSetConstantBuffers(3, 1, m_pCBUseTextureMap.GetAddressOf());

		// RAS
		m_DeviceContext->RSSetState(m_rasterizerState.Get());

		m_DeviceContext->UpdateSubresource(m_pCBCoordinateData.Get(), 0, nullptr, &m_CBCoordinateData, 0, 0);
		m_DeviceContext->UpdateSubresource(m_pCBLight.Get(), 0, nullptr, &m_CBLightData, 0, 0);
		m_DeviceContext->UpdateSubresource(m_pCBCamera.Get(), 0, nullptr, &m_CBCamera, 0, 0);
		m_DeviceContext->UpdateSubresource(m_pCBUseTextureMap.Get(), 0, nullptr, &m_CBNormalMap, 0, 0);

		// PS
		m_DeviceContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
		m_DeviceContext->PSSetSamplers(1, 1, m_BRDF_Sampler.GetAddressOf());
		m_DeviceContext->PSSetConstantBuffers(0, 1, m_pCBCoordinateData.GetAddressOf());
		m_DeviceContext->PSSetConstantBuffers(1, 1, m_pCBLight.GetAddressOf());
		m_DeviceContext->PSSetConstantBuffers(2, 1, m_pCBCamera.GetAddressOf());
		m_DeviceContext->PSSetConstantBuffers(3, 1, m_pCBUseTextureMap.GetAddressOf());

		// 모델 업데이트
		for (auto model : myModels)
			model->Update(m_DeviceContext.Get());
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

void DemoApp::createConstantBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	m_ModelCBBuffer = make_shared<ModelCBBuffer>();

	// 좌표(World, View, Projection) 데이터 Constant Buffer로 넘겨주기
	desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(CBCoordinateData);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = m_Device->CreateBuffer(&desc, nullptr, m_pCBCoordinateData.GetAddressOf());
	assert(SUCCEEDED(hr));

	// Light 데이터 Constant Buffer로 넘겨주기
	desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(CBLightData);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = m_Device->CreateBuffer(&desc, nullptr, m_pCBLight.GetAddressOf());
	assert(SUCCEEDED(hr));

	// Camera 데이터 Constant Buffer로 넘겨주기
	desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(CBCameraData);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = m_Device->CreateBuffer(&desc, nullptr, m_pCBCamera.GetAddressOf());
	assert(SUCCEEDED(hr));

	// TextureMap 데이터 Constant Buffer로 넘겨주기
	desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(CBUseTextureMap);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = m_Device->CreateBuffer(&desc, nullptr, m_pCBUseTextureMap.GetAddressOf());
	assert(SUCCEEDED(hr));

	// TextureMap있는지 체크하는 데이터 Constant Buffer로 넘겨주기
	desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(CBIsValidTextureMap);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = m_Device->CreateBuffer(&desc, nullptr, m_ModelCBBuffer->m_pCBbisTextureMap.GetAddressOf());
	assert(SUCCEEDED(hr));

	// Material 데이터 Constant Buffer로 넘겨주기
	desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(CBMaterial);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = m_Device->CreateBuffer(&desc, nullptr, m_ModelCBBuffer->m_pCBMaterialData.GetAddressOf());
	assert(SUCCEEDED(hr));

	// Model 데이터 Constant Buffer로 넘겨주기
	desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(CBNodeTransform);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = m_Device->CreateBuffer(&desc, nullptr, m_ModelCBBuffer->m_pCBNodelData.GetAddressOf());
	assert(SUCCEEDED(hr));

	// Mesh 데이터 Constant Buffer로 넘겨주기
	desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(CBMatrixPallete);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = m_Device->CreateBuffer(&desc, nullptr, m_ModelCBBuffer->m_pCBBoneTransformData.GetAddressOf());
	assert(SUCCEEDED(hr));
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

void DemoApp::createSamplerState()
{
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	desc.MaxLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr = m_Device->CreateSamplerState(&desc, m_samplerState.GetAddressOf());
	assert(SUCCEEDED(hr));

	desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	desc.MaxLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = m_Device->CreateSamplerState(&desc, m_BRDF_Sampler.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void DemoApp::createBlendState()
{
	D3D11_BLEND_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;

	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HRESULT hr = m_Device->CreateBlendState(&desc, m_blendState.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void DemoApp::setTransform()
{
	m_World = XMMatrixIdentity();
	m_DirectionLight = Vector4{ 1.f, 0.f, 0.f, 0.f };
}

void DemoApp::Release()
{
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