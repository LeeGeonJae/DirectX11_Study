#include "../Engine/pch.h"
#include "../Engine/TimeManager.h"
#include "../Engine/Helper.h"

#include "DemoApp.h"
#include <DirectXMath.h>
#include <Directxtk/DDSTextureLoader.h>

#include "ImGuiMenu.h"
#include "ModelLoadManager.h"
#include "ResourceManager.h"
#include "RenderManager.h"
#include "../Engine/InputManager.h"
#include "Model.h"

#include "PawnController.h"
#include "Object.h"
#include "Pawn.h"
#include "RenderComponent.h"
#include "CameraComponent.h"

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
	m_imgui->Init(m_hWnd, DEVICE.Get(), DEVICE_CONTEXT.Get());
}

void DemoApp::initD3D()
{

	createDeathStencilView();
}

void DemoApp::initScene()
{
	ResourceManager::GetInstance()->Init(DEVICE, DEVICE_CONTEXT);
	ModelLoadManager::GetInstance()->Init(DEVICE, DEVICE_CONTEXT);

	createRasterizeState();
	createSamplerState();
	createBlendState();

	constantBufferSetting();

	// 카메라
	{
		// 폰 (오브젝트)
		shared_ptr<Pawn> pawn = make_shared<Pawn>();
		PawnDesc pawnDesc;
		pawnDesc.m_ObjectDesc.m_Name = "Pawn";
		pawnDesc.m_ObjectDesc.m_Position = Vector3(0.f, 40.f, -800.f);
		pawn->Setting(pawnDesc);

		// 카메라 컴포넌트
		shared_ptr<CameraComponent> camera = make_shared<CameraComponent>();
		CameraComponentDesc cameraDesc;
		cameraDesc.m_ComponentDesc.m_Name = "CameraComponent";
		cameraDesc.m_CBCamera = m_CBCamera;
		cameraDesc.m_CBCoordinateData = m_CBCoordinateData;
		cameraDesc.m_DeviceContext = DEVICE_CONTEXT;
		cameraDesc.m_pCBCamera = m_pCBCamera;
		cameraDesc.m_pCBCoordinateData = m_pCBCoordinateData;
		camera->Setting(cameraDesc);

		// 오브젝트에 컴포넌트 세팅
		pawn->SetComponent<CameraComponent>(camera);
		pawn->Init();

		m_Objects.push_back(pawn);

		// 폰 컨트롤러 세팅
		shared_ptr<PawnController> pawnController = make_shared<PawnController>();
		pawnController->SetPawn(pawn);
	}
}

void DemoApp::Update()
{
	__super::Update();

	if (ImGuiMenu::SkinningTestFBX)
	{
		// 오브젝트 세팅
		shared_ptr<Object> object = make_shared<Object>();
		ObjectDesc objectDesc;
		objectDesc.m_Name = "Skinning";
		objectDesc.m_Position = Vector3(rand() % 400 - 200, 0.f, 0.f);
		object->Setting(objectDesc);

		// 렌더 컴포넌트 세팅
		shared_ptr<RenderComponent> renderComponent = make_shared<RenderComponent>();
		RenderComponentDesc renderComponentDesc;
		renderComponentDesc.m_ComponentDesc.m_Name = "SkinningRenderComponent";
		renderComponentDesc.m_Device = DEVICE;
		renderComponentDesc.m_DeviceContext = DEVICE_CONTEXT;
		renderComponentDesc.m_ModelBuffer = m_ModelCBBuffer;
		renderComponentDesc.m_Path = "../Resource/Texture/SkinningTest.fbx";
		renderComponent->Setting(renderComponentDesc);

		// 오브젝트에 컴포넌트 세팅
		object->SetComponent<RenderComponent>(renderComponent);
		object->Init();

		m_Objects.push_back(object);
	}
	else if (ImGuiMenu::DyingAnimationFBX)
	{
		// 오브젝트 세팅
		shared_ptr<Object> object = make_shared<Object>();
		ObjectDesc objectDesc;
		objectDesc.m_Name = "Dying";
		objectDesc.m_Position = Vector3(rand() % 400 - 200, 0.f, 0.f);
		object->Setting(objectDesc);

		// 렌더 컴포넌트 세팅
		shared_ptr<RenderComponent> renderComponent = make_shared<RenderComponent>();
		RenderComponentDesc renderComponentDesc;
		renderComponentDesc.m_ComponentDesc.m_Name = "DyingRenderComponent";
		renderComponentDesc.m_Device = DEVICE;
		renderComponentDesc.m_DeviceContext = DEVICE_CONTEXT;
		renderComponentDesc.m_ModelBuffer = m_ModelCBBuffer;
		renderComponentDesc.m_Path = "../Resource/Texture/Dying.fbx";
		renderComponent->Setting(renderComponentDesc);

		// 오브젝트에 컴포넌트 세팅
		object->SetComponent<RenderComponent>(renderComponent);
		object->Init();

		m_Objects.push_back(object);
	}
	else if (ImGuiMenu::ZeldaFBX)
	{
		shared_ptr<Object> object = make_shared<Object>();
		ObjectDesc objectDesc;
		objectDesc.m_Name = "Zelda";
		objectDesc.m_Position = Vector3(rand() % 400 - 200, 0.f, 0.f);
		object->Setting(objectDesc);

		shared_ptr<RenderComponent> renderComponent = make_shared<RenderComponent>();
		RenderComponentDesc renderComponentDesc;
		renderComponentDesc.m_ComponentDesc.m_Name = "ZeldaRenderComponent";
		renderComponentDesc.m_Device = DEVICE;
		renderComponentDesc.m_DeviceContext = DEVICE_CONTEXT;
		renderComponentDesc.m_ModelBuffer = m_ModelCBBuffer;
		renderComponentDesc.m_Path = "../Resource/Texture/zeldaPosed001.fbx";
		renderComponent->Setting(renderComponentDesc);

		// 오브젝트에 컴포넌트 세팅
		object->SetComponent<RenderComponent>(renderComponent);
		object->Init();

		m_Objects.push_back(object);
	}
	else if (ImGuiMenu::VampireFBX)
	{
		shared_ptr<Object> object = make_shared<Object>();
		ObjectDesc objectDesc;
		objectDesc.m_Name = "Vampire";
		objectDesc.m_Position = Vector3(rand() % 400 - 200, 0.f, 0.f);
		object->Setting(objectDesc);

		shared_ptr<RenderComponent> renderComponent = make_shared<RenderComponent>();
		RenderComponentDesc renderComponentDesc;
		renderComponentDesc.m_ComponentDesc.m_Name = "VampireRenderComponent";
		renderComponentDesc.m_Device = DEVICE;
		renderComponentDesc.m_DeviceContext = DEVICE_CONTEXT;
		renderComponentDesc.m_ModelBuffer = m_ModelCBBuffer;
		renderComponentDesc.m_Path = "../Resource/Texture/Vampire.fbx";
		renderComponent->Setting(renderComponentDesc);

		// 오브젝트에 컴포넌트 세팅
		object->SetComponent<RenderComponent>(renderComponent);
		object->Init();

		m_Objects.push_back(object);
	}
	else if (ImGuiMenu::cerberusFBX)
	{
		shared_ptr<Object> object = make_shared<Object>();
		ObjectDesc objectDesc;
		objectDesc.m_Name = "Gun";
		objectDesc.m_Position = Vector3(rand() % 400 - 200, 0.f, 0.f);
		object->Setting(objectDesc);

		shared_ptr<RenderComponent> renderComponent = make_shared<RenderComponent>();
		RenderComponentDesc renderComponentDesc;
		renderComponentDesc.m_ComponentDesc.m_Name = "cerberus_testRenderComponent";
		renderComponentDesc.m_Device = DEVICE;
		renderComponentDesc.m_DeviceContext = DEVICE_CONTEXT;
		renderComponentDesc.m_ModelBuffer = m_ModelCBBuffer;
		renderComponentDesc.m_Path = "../Resource/Texture/cerberus_test.fbx";
		renderComponent->Setting(renderComponentDesc);

		// 오브젝트에 컴포넌트 세팅
		object->SetComponent<RenderComponent>(renderComponent);
		object->Init();

		m_Objects.push_back(object);
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
		if (InputManager::GetInstance()->GetKeyboardState().IsKeyDown(DirectX::Keyboard::F))
		{
			m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		}
		else
		{
			m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}

		// VS
		m_DeviceContext->VSSetConstantBuffers(1, 1, m_pCBLight.GetAddressOf());
		m_DeviceContext->VSSetConstantBuffers(3, 1, m_pCBUseTextureMap.GetAddressOf());

		// RAS
		m_DeviceContext->RSSetState(m_rasterizerState.Get());


		m_DeviceContext->UpdateSubresource(m_pCBLight.Get(), 0, nullptr, &m_CBLightData, 0, 0);
		m_DeviceContext->UpdateSubresource(m_pCBUseTextureMap.Get(), 0, nullptr, &m_CBNormalMap, 0, 0);

		// PS
		m_DeviceContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
		m_DeviceContext->PSSetSamplers(1, 1, m_BRDF_Sampler.GetAddressOf());

		m_DeviceContext->PSSetConstantBuffers(1, 1, m_pCBLight.GetAddressOf());
		m_DeviceContext->PSSetConstantBuffers(3, 1, m_pCBUseTextureMap.GetAddressOf());

		// 모델 업데이트
		for (auto object : m_Objects)
		{
			object->Update();
			object->LateUpdate();
			object->Render();
		}
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





void DemoApp::constantBufferSetting()
{
	m_ModelCBBuffer = make_shared<ModelCBBuffer>();

	m_pCBCoordinateData = createConstantBuffer<CBCoordinateData>();
	m_pCBLight = createConstantBuffer<CBLightData>();
	m_pCBCamera = createConstantBuffer<CBCameraData>();
	m_pCBUseTextureMap = createConstantBuffer<CBUseTextureMap>();
	m_ModelCBBuffer->m_pCBbisTextureMap = createConstantBuffer<CBIsValidTextureMap>();
	m_ModelCBBuffer->m_pCBBoneTransformData = createConstantBuffer<CBMatrixPallete>();
	m_ModelCBBuffer->m_pCBMaterialData = createConstantBuffer<CBMaterial>();
	m_ModelCBBuffer->m_pCBNodelData = createConstantBuffer<CBNodeTransform>();
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