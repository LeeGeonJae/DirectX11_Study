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
#include "ObjectManager.h"
#include "CollisionManager.h"
#include "../Engine/InputManager.h"
#include "Model.h"

#include "PawnController.h"
#include "Object.h"
#include "Pawn.h"
#include "TestBoxObject.h"

#include "RenderComponent.h"
#include "CameraComponent.h"
#include "CollisionComponent.h"
#include "BoxComponent.h"
#include "SphereComponent.h"
#include "OrientedBoxComponent.h"


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
	CollisionManager::GetInstance()->Init();

	// 카메라
	{
		// 폰 (오브젝트)
		shared_ptr<Pawn> pawn = ObjectManager::GetInstance()->CreateObject<Pawn>();
		PawnInfo pawnInfo;
		pawnInfo.m_ObjectDesc.m_Name = "Pawn";
		pawnInfo.m_ObjectDesc.m_Position = Vector3(0.f, 40.f, -800.f);
		pawn->Setting(pawnInfo);
		pawn->Init();

		m_Objects.push_back(pawn);
	}
}

void DemoApp::Update()
{
	__super::Update();

	if (ImGuiMenu::SkinningTestFBX)
	{
		// 오브젝트 세팅
		for (int i = 0; i < 2; i++)
		{
			shared_ptr<TestBoxObject> object = ObjectManager::GetInstance()->CreateObject<TestBoxObject>();
			TestBoxObjectInfo testObjectInfo;
			testObjectInfo.m_ObjectInfo.m_Name = "Rendering";
			testObjectInfo.m_ObjectInfo.m_Position = Vector3(rand() % 4000 - 2000, 0, rand() % 4000 - 2000);
			testObjectInfo.m_RenderName = "SkinningRenderComponent";
			testObjectInfo.m_RenderPath = "../Resource/Texture/SkinningTest.fbx";
			object->Setting(testObjectInfo);
			object->Init();

			m_Objects.push_back(object);
		}
	}
	else if (ImGuiMenu::DyingAnimationFBX)
	{
		// 오브젝트 세팅
		shared_ptr<TestBoxObject> object = ObjectManager::GetInstance()->CreateObject<TestBoxObject>();
		TestBoxObjectInfo testObjectInfo;
		testObjectInfo.m_ObjectInfo.m_Name = "Rendering";
		testObjectInfo.m_ObjectInfo.m_Position = Vector3(rand() % 4000 - 2000, 0.f, rand() % 4000 - 2000);
		testObjectInfo.m_RenderName = "DyingRenderComponent";
		testObjectInfo.m_RenderPath = "../Resource/Texture/Dying.fbx";
		object->Setting(testObjectInfo);
		object->Init();

		m_Objects.push_back(object);
	}
	else if (ImGuiMenu::ZeldaFBX)
	{
		// 오브젝트 세팅
		shared_ptr<TestBoxObject> object = ObjectManager::GetInstance()->CreateObject<TestBoxObject>();
		TestBoxObjectInfo testObjectInfo;
		testObjectInfo.m_ObjectInfo.m_Name = "Rendering";
		testObjectInfo.m_ObjectInfo.m_Position = Vector3(0.f, -100.f, 0.f);
		testObjectInfo.m_ObjectInfo.m_Scale = Vector3(100.f, 1.f, 100.f);
		testObjectInfo.m_RenderName = "BoxComponent";
		testObjectInfo.m_RenderPath = "../Resource/Texture/box.fbx";
		object->Setting(testObjectInfo);
		object->Init();

		m_Objects.push_back(object);
	}
	else if (ImGuiMenu::VampireFBX)
	{
		// 오브젝트 세팅
		shared_ptr<TestBoxObject> object = ObjectManager::GetInstance()->CreateObject<TestBoxObject>();
		TestBoxObjectInfo testObjectInfo;
		testObjectInfo.m_ObjectInfo.m_Name = "Rendering";
		testObjectInfo.m_ObjectInfo.m_Position = Vector3(rand() % 4000 - 2000, 0.f, rand() % 4000 - 2000);
		testObjectInfo.m_RenderName = "VampireRenderComponent";
		testObjectInfo.m_RenderPath = "../Resource/Texture/Vampire.fbx";
		object->Setting(testObjectInfo);
		object->Init();

		m_Objects.push_back(object);
	}
	else if (ImGuiMenu::cerberusFBX)
	{
		// 오브젝트 세팅
		shared_ptr<TestBoxObject> object = ObjectManager::GetInstance()->CreateObject<TestBoxObject>();
		TestBoxObjectInfo testObjectInfo;
		testObjectInfo.m_ObjectInfo.m_Name = "Rendering";
		testObjectInfo.m_ObjectInfo.m_Position = Vector3(rand() % 4000 - 2000, 0.f, rand() % 4000 - 2000);
		testObjectInfo.m_RenderName = "cerberus_testRenderComponent";
		testObjectInfo.m_RenderPath = "../Resource/Texture/cerberus_test.fbx";
		object->Setting(testObjectInfo);
		object->Init();

		m_Objects.push_back(object);
	}


	for (auto object : m_Objects)
	{
		object->Update();
		object->LateUpdate();
		object->Render();
	}

	static float durationTime = 0;
	durationTime += TimeManager::GetInstance()->GetfDT();

	if (durationTime >= 0.016667f)
	{
		durationTime = durationTime - 0.016667f;

		ImGuiMenu::FixedUpdateCount++;
	}

	CollisionManager::GetInstance()->FixedUpdate();
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


LRESULT CALLBACK DemoApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return __super::WndProc(hWnd, message, wParam, lParam);
}