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

	RenderManager::GetInstance()->Init(m_hWnd, m_ClientHeight, m_ClientWidth);
	ResourceManager::GetInstance()->Init(DEVICE, DEVICE_CONTEXT);
	ModelLoadManager::GetInstance()->Init(DEVICE, DEVICE_CONTEXT);


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
		camera->Setting(cameraDesc);

		// 오브젝트에 컴포넌트 세팅
		pawn->SetComponent<CameraComponent>(camera);
		pawn->Init();

		m_Objects.push_back(pawn);

		// 폰 컨트롤러 세팅
		shared_ptr<PawnController> pawnController = make_shared<PawnController>();
		pawnController->SetPawn(pawn);

		RenderManager::GetInstance()->SetCamera(camera);
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
		objectDesc.m_Position = Vector3(rand() % 4000 - 2000, rand() % 4000 - 2000, rand() % 4000 - 2000);
		object->Setting(objectDesc);

		// 렌더 컴포넌트 세팅
		shared_ptr<RenderComponent> renderComponent = make_shared<RenderComponent>();
		RenderComponentDesc renderComponentDesc;
		renderComponentDesc.m_ComponentDesc.m_Name = "SkinningRenderComponent";
		renderComponentDesc.m_ModelBuffer = RenderManager::GetInstance()->GetModelCB();
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
		objectDesc.m_Position = Vector3(rand() % 4000 - 2000, rand() % 4000 - 2000, rand() % 4000 - 2000);
		object->Setting(objectDesc);

		// 렌더 컴포넌트 세팅
		shared_ptr<RenderComponent> renderComponent = make_shared<RenderComponent>();
		RenderComponentDesc renderComponentDesc;
		renderComponentDesc.m_ComponentDesc.m_Name = "DyingRenderComponent";
		renderComponentDesc.m_ModelBuffer = RenderManager::GetInstance()->GetModelCB();
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
		objectDesc.m_Position = Vector3(rand() % 4000 - 2000, rand() % 4000 - 2000, rand() % 4000 - 2000);
		object->Setting(objectDesc);

		shared_ptr<RenderComponent> renderComponent = make_shared<RenderComponent>();
		RenderComponentDesc renderComponentDesc;
		renderComponentDesc.m_ComponentDesc.m_Name = "ZeldaRenderComponent";
		renderComponentDesc.m_ModelBuffer = RenderManager::GetInstance()->GetModelCB();
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
		objectDesc.m_Position = Vector3(rand() % 4000 - 2000, rand() % 4000 - 2000, rand() % 4000 - 2000);
		object->Setting(objectDesc);

		shared_ptr<RenderComponent> renderComponent = make_shared<RenderComponent>();
		RenderComponentDesc renderComponentDesc;
		renderComponentDesc.m_ComponentDesc.m_Name = "VampireRenderComponent";
		renderComponentDesc.m_ModelBuffer = RenderManager::GetInstance()->GetModelCB();
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
		objectDesc.m_Position = Vector3(rand() % 4000 - 2000, rand() % 4000 - 2000, rand() % 4000 - 2000);
		object->Setting(objectDesc);

		shared_ptr<RenderComponent> renderComponent = make_shared<RenderComponent>();
		RenderComponentDesc renderComponentDesc;
		renderComponentDesc.m_ComponentDesc.m_Name = "cerberus_testRenderComponent";
		renderComponentDesc.m_ModelBuffer = RenderManager::GetInstance()->GetModelCB();
		renderComponentDesc.m_Path = "../Resource/Texture/cerberus_test.fbx";
		renderComponent->Setting(renderComponentDesc);

		// 오브젝트에 컴포넌트 세팅
		object->SetComponent<RenderComponent>(renderComponent);
		object->Init();

		m_Objects.push_back(object);
	}

	for (auto object : m_Objects)
	{
		object->Update();
		object->LateUpdate();
		object->Render();
	}

	RenderManager::GetInstance()->Update();
}

void DemoApp::Render()
{
	RenderManager::GetInstance()->Render();
}

void DemoApp::Release()
{
	RenderManager::GetInstance()->Release();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK DemoApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	return __super::WndProc(hWnd, message, wParam, lParam);
}