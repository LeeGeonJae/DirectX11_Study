#include "RenderManager.h"
#include "ImGuiMenu.h"
#include "../Engine/InputManager.h"

#include "Model.h"
#include "Node.h"
#include "Mesh.h"
#include "Material.h"
#include "RenderComponent.h"
#include "CameraComponent.h"
#include "BoxComponent.h"
#include "SphereComponent.h"
#include "OrientedBoxComponent.h"

#include "ResourceManager.h"

#include "DebugDraw.h"

const int SHADOWMAP_SIZE = 16384;

void RenderManager::Init(HWND _hwnd, UINT _clientHeight, UINT _clientWidth)
{
	m_hWnd = _hwnd;
	m_ClientHeight = _clientHeight;
	m_ClientWidth = _clientWidth;

	createDeviceAndSwapChain();
	createRenderTargetView();
	setViewport();
	createDeathStencilView();

	createRasterizeState();
	createSamplerState();
	//createBlendState();

	constantBufferSetting();

	m_imgui = make_shared<ImGuiMenu>();
	m_imgui->Init(m_hWnd, DEVICE.Get(), DEVICE_CONTEXT.Get());
	DebugDraw::Initialize(m_Device, m_DeviceContext);
}

void RenderManager::Update()
{
	// Lighting
	{
		XMFLOAT4 LightDirection = ImGuiMenu::DirectionLightDir;
		DirectX::SimpleMath::Color LightColor = ImGuiMenu::DirectionLightColor;
		Vector3 AmbientColor = ImGuiMenu::AmbientColor;
		float SpeclarPower = ImGuiMenu::SpecularPower;

		m_CBLightData.LightDir = LightDirection;
		m_CBLightData.LightColor = LightColor;
		m_CBLightData.AmbientColor = AmbientColor;
		m_CBLightData.SpecularPower = SpeclarPower;
	}

	// NormalMap
	{
		m_CBUseTextureMap.UseNormalMap = ImGuiMenu::bIsNormalMap;
		m_CBUseTextureMap.UseSpecularMap = ImGuiMenu::bIsSpecularMap;
		m_CBUseTextureMap.UseEmissiveMap = ImGuiMenu::bIsEmissiveMap;
		m_CBUseTextureMap.UseGammaCorrection = ImGuiMenu::bIsGammaCorrection;
	}

	// Camera
	if (m_pCamera.expired() == false)
	{
		auto camera = m_pCamera.lock();

		if (!ImGuiMenu::bIsFreezeCulling)
		{
			BoundingFrustum::CreateFromMatrix(m_Frustum, camera->GetProjectionMatrix());
			m_Frustum.Transform(m_Frustum, camera->GetWorldTransform());
		}

		m_View = camera->GetViewMatrix();
		m_Projection = camera->GetProjectionMatrix();

		m_CBCoordinateData.Projection = m_Projection.Transpose();
		m_CBCoordinateData.View = m_View.Transpose();
		m_CBCoordinateData.CameraPosition = camera->GetWorldTransform().Translation();
	}

	// Shadow
	{
		m_ShadowProjection = XMMatrixPerspectiveFovLH(XM_PIDIV4, SHADOWMAP_SIZE / SHADOWMAP_SIZE, 2000.f, 400000.f);
		m_ShadowLookAt = m_pCamera.lock()->GetWorldTransform().Translation() + m_pCamera.lock()->GetForward() * m_ShadowForwardistFromCamera;
		m_ShadowPos = m_ShadowLookAt + (-ImGuiMenu::DirectionLightDir * m_ShadowUpDistFromLookAt);
		m_ShadowView = XMMatrixLookAtLH(m_ShadowPos, m_ShadowLookAt, Vector3(0.f, 1.f, 0.f));

		ImGuiMenu::ShadowDirection = m_ShadowLookAt - m_ShadowPos;
		ImGuiMenu::ShadowDirection.Normalize();

		m_CBCoordinateData.ShadowView = m_ShadowView.Transpose();
		m_CBCoordinateData.ShadowProjection = m_ShadowProjection.Transpose();
	}
}

void RenderManager::Render()
{
	{
		m_DeviceContext->RSSetViewports(1, &m_ShadowViewport);
		m_DeviceContext->OMSetRenderTargets(0, NULL, m_pShadowMapDSV.Get());
		m_DeviceContext->ClearDepthStencilView(m_pShadowMapDSV.Get(), D3D11_CLEAR_DEPTH, 1.f, 0);
		m_DeviceContext->PSSetShader(NULL, NULL, 0);

		m_DeviceContext->VSSetConstantBuffers(0, 1, m_pCBCoordinate.GetAddressOf());
		m_DeviceContext->UpdateSubresource(m_pCBCoordinate.Get(), 0, nullptr, &m_CBCoordinateData, 0, 0);

		for (auto renderComponent : m_RenderVec)
		{
			renderComponent->SetIsCulled(true);

			renderComponent->LateUpdate();
		}
	}

	float clearColor[4] = { 0.0f, 0.5f, 0.5f, 1.f };

	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), clearColor);
	m_DeviceContext->ClearDepthStencilView(m_depthStancilView.Get(), D3D11_CLEAR_DEPTH, 1.f, 0);	// 뎁스버퍼 1.0f로 초기화
	m_DeviceContext->RSSetViewports(1, &m_Viewport);
	m_DeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_depthStancilView.Get());

	// 파이프라인 세팅
	{
		if (InputManager::GetInstance()->GetKeyboardState().IsKeyDown(DirectX::Keyboard::F))
		{
			m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		}
		else
		{
			m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}

		m_DeviceContext->PSSetShaderResources(8, 1, m_pShadowMapSRV.GetAddressOf());
		ImGuiMenu::ShadowMap = m_pShadowMapSRV.Get();

		// VS
		m_DeviceContext->VSSetConstantBuffers(0, 1, m_pCBCoordinate.GetAddressOf());
		m_DeviceContext->VSSetConstantBuffers(1, 1, m_pCBLight.GetAddressOf());
		m_DeviceContext->VSSetConstantBuffers(3, 1, m_pCBUseTextureMap.GetAddressOf());

		// RAS
		m_DeviceContext->RSSetState(m_rasterizerState.Get());

		m_DeviceContext->UpdateSubresource(m_pCBCoordinate.Get(), 0, nullptr, &m_CBCoordinateData, 0, 0);
		m_DeviceContext->UpdateSubresource(m_pCBLight.Get(), 0, nullptr, &m_CBLightData, 0, 0);
		m_DeviceContext->UpdateSubresource(m_pCBUseTextureMap.Get(), 0, nullptr, &m_CBUseTextureMap, 0, 0);

		// PS
		m_DeviceContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
		m_DeviceContext->PSSetSamplers(1, 1, m_BRDF_Sampler.GetAddressOf());

		m_DeviceContext->PSSetConstantBuffers(0, 1, m_pCBCoordinate.GetAddressOf());
		m_DeviceContext->PSSetConstantBuffers(1, 1, m_pCBLight.GetAddressOf());
		m_DeviceContext->PSSetConstantBuffers(3, 1, m_pCBUseTextureMap.GetAddressOf());
	}

	// Render
	{
		renderMesh();
		//renderDebugDraw();
		m_imgui->Render();
	}

	m_RenderVec.clear();

	HRESULT hr = m_SwapChain->Present(0, 0);
	assert(SUCCEEDED(hr));
}

void RenderManager::renderDebugDraw()
{
	DebugDraw::g_BatchEffect->Apply(m_DeviceContext.Get());
	DebugDraw::g_BatchEffect->SetView(m_View);
	DebugDraw::g_BatchEffect->SetProjection(m_Projection);

	DebugDraw::g_Batch->Begin();

	//DebugDraw::Draw(DebugDraw::g_Batch.get(), m_Frustum, Colors::Yellow);

	for (auto renderComponent : m_RenderVec)
	{
		if (renderComponent->GetIsCulled())
		{
			DebugDraw::Draw(DebugDraw::g_Batch.get(), renderComponent->GetBoundingBox(), Colors::Blue);
		}
		else
		{
			DebugDraw::Draw(DebugDraw::g_Batch.get(), renderComponent->GetBoundingBox(), Colors::Red);
		}
	}

	for (auto boxComponent : m_BoxCollisionVec)
	{
		DebugDraw::Draw(DebugDraw::g_Batch.get(), boxComponent->GetGeometry(), Colors::Yellow);
	}
	for (auto sphereComponent : m_SphereCollisionVec)
	{
		//DebugDraw::Draw(DebugDraw::g_Batch.get(), sphereComponent->GetGeometry(), Colors::Yellow);
	}
	for (auto orientedComponent : m_OrientedCollisionVec)
	{
		DebugDraw::Draw(DebugDraw::g_Batch.get(), orientedComponent->GetGeometry(), Colors::Yellow);
	}

	DebugDraw::g_Batch->End();
}

void RenderManager::renderMesh()
{
	ImGuiMenu::ObjectCount = m_RenderVec.size();

	for (auto renderComponent : m_RenderVec)
	{
		renderComponent->SetIsCulled(true);

		//if (m_Frustum.Intersects(renderComponent->GetBoundingBox()))
		//{
			renderComponent->Render();
		//	renderComponent->SetIsCulled(false);
		//	ImGuiMenu::DrawComponentCount++;
		//}
	}
}

void RenderManager::constantBufferSetting()
{
	m_pCBModel = make_shared<ModelCBBuffer>();

	m_pCBCoordinate = createConstantBuffer<CBCoordinateData>();
	m_pCBLight = createConstantBuffer<CBLightData>();
	m_pCBUseTextureMap = createConstantBuffer<CBUseTextureMap>();
	m_pCBModel->m_pCBbisTextureMap = createConstantBuffer<CBIsValidTextureMap>();
	m_pCBModel->m_pCBBoneTransformData = createConstantBuffer<CBMatrixPallete>();
	m_pCBModel->m_pCBMaterialData = createConstantBuffer<CBMaterial>();
	m_pCBModel->m_pCBNodelData = createConstantBuffer<CBNodeTransform>();
}

void RenderManager::createDeviceAndSwapChain()
{
	HRESULT hr = 0;

	//// 스왑체인 속성 설정 구조체를 초기화합니다.
	//DXGI_SWAP_CHAIN_DESC swapDesc;
	//ZeroMemory(&swapDesc, sizeof(swapDesc));
	//swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 백 버퍼의 픽셀 형식을 설정합니다.
	//swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	//swapDesc.BufferDesc.Width = m_ClientWidth;					// 백 버퍼의 너비를 설정합니다.
	//swapDesc.BufferDesc.Height = m_ClientHeight;				// 백 버퍼의 높이를 설정합니다.
	//swapDesc.BufferDesc.RefreshRate.Numerator = 60;				// 화면 주사율의 분자를 설정합니다.
	//swapDesc.BufferDesc.RefreshRate.Denominator = 1;			// 화면 주사율의 분모를 설정합니다.
	//swapDesc.SampleDesc.Count = 1;								// 멀티 샘플링 수를 설정합니다.
	//swapDesc.SampleDesc.Quality = 0;							// 멀티 샘플링 품질을 설정합니다.
	//swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//swapDesc.BufferCount = 1;
	//swapDesc.OutputWindow = m_hWnd;
	//swapDesc.Windowed = true;
	//swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// 스왑체인 속성 설정 구조체 생성.
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 1;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = m_hWnd;	// 스왑체인 출력할 창 핸들 값.
	swapDesc.Windowed = true;		// 창 모드 여부 설정.
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 백버퍼(텍스처)의 가로/세로 크기 설정.
	swapDesc.BufferDesc.Width = m_ClientWidth;
	swapDesc.BufferDesc.Height = m_ClientHeight;
	// 화면 주사율 설정.
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;

	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;


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

	m_ShadowViewport.TopLeftX = 0.f;
	m_ShadowViewport.TopLeftY = 0.f;
	m_ShadowViewport.Width = static_cast<float>(SHADOWMAP_SIZE);
	m_ShadowViewport.Height = static_cast<float>(SHADOWMAP_SIZE);
	m_ShadowViewport.MinDepth = 0.f;
	m_ShadowViewport.MaxDepth = 1.f;
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

	// 그림자용 ShadowMap Texture와 SRV, DSV 생성
	{
		// Create Shadow map texture desc
		D3D11_TEXTURE2D_DESC texDesc = {};
		texDesc.Width = static_cast<float>(SHADOWMAP_SIZE);
		texDesc.Height = static_cast<float>(SHADOWMAP_SIZE);
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;

		ID3D11Texture2D* pShadowMap = nullptr;
		hr = m_Device->CreateTexture2D(&texDesc, NULL, &pShadowMap);
		assert(SUCCEEDED(hr));

		// Create the depth stencil view desc
		D3D11_DEPTH_STENCIL_VIEW_DESC ShadowdescDSV = {};
		ShadowdescDSV.Format = DXGI_FORMAT_D32_FLOAT;
		ShadowdescDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		hr = m_Device->CreateDepthStencilView(pShadowMap, &ShadowdescDSV, m_pShadowMapDSV.GetAddressOf());
		assert(SUCCEEDED(hr));

		// Create the depth stencil view desc
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		hr = m_Device->CreateShaderResourceView(pShadowMap, &srvDesc, m_pShadowMapSRV.GetAddressOf());
		assert(SUCCEEDED(hr));

	}
}

void RenderManager::createRasterizeState()
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.DepthBias = 1000;
	desc.DepthBiasClamp = 0.f;
	desc.SlopeScaledDepthBias = 1.f;
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_NONE;
	desc.FrontCounterClockwise = false;		// 정점이 시계방향으로 돌면서 삼각형을 그리면 정면이 앞부분으로 간다. true면 정면이 반대방향을 향한다.

	HRESULT hr = m_Device->CreateRasterizerState(&desc, m_rasterizerState.GetAddressOf());
	assert(SUCCEEDED(hr));
}

//void RenderManager::createBlendState()
//{
//	D3D11_BLEND_DESC desc;
//	ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));
//	desc.AlphaToCoverageEnable = false;
//	desc.IndependentBlendEnable = false;
//	
//	desc.RenderTarget[0].BlendEnable = true;
//	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
//	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
//	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
//	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
//	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
//	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
//	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
//
//	HRESULT hr = m_Device->CreateBlendState(&desc, m_blendState.GetAddressOf());
//	assert(SUCCEEDED(hr));
//}

void RenderManager::createSamplerState()
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

void RenderManager::Release()
{
	DebugDraw::Uninitialize();

	m_RenderTargetView.ReleaseAndGetAddressOf();
	m_SwapChain.ReleaseAndGetAddressOf();
	m_DeviceContext.ReleaseAndGetAddressOf();
	m_Device.ReleaseAndGetAddressOf();
}

