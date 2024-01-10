#include "CameraComponent.h"
#include "ImGuiMenu.h"
#include "../Engine/Values.h"

#include "RenderManager.h"

CameraComponent::CameraComponent()
{
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::Setting(CameraComponentInfo info)
{
	__super::Setting(info.m_ComponentInfo);
}

void CameraComponent::Init()
{
}

void CameraComponent::Update()
{
	__super::Update();

	// Camera
	{
		Vector3 eye = m_World.Translation();
		Vector3 target = m_World.Translation() + -m_World.Forward();
		Vector3 up = m_World.Up();
		m_View = XMMatrixLookAtLH(eye, target, up);
		m_Projection = XMMatrixPerspectiveFovLH(ImGuiMenu::CameraFov, GWinSizeX / (FLOAT)GWinSizeY, ImGuiMenu::CameraNearFar[0], ImGuiMenu::CameraNearFar[1]);
	}
}

void CameraComponent::LateUpdate()
{
}

void CameraComponent::FixedUpdate()
{
}

void CameraComponent::Render()
{
}