#include "CameraComponent.h"
#include "ImGuiMenu.h"
#include "../Engine/Values.h"

CameraComponent::CameraComponent()
{
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::Setting(CameraComponentDesc desc)
{
	__super::Setting(desc.m_ComponentDesc);

	m_DeviceContext = desc.m_DeviceContext;
	m_CBCoordinateData = desc.m_CBCoordinateData;
	m_CBCamera = desc.m_CBCamera;
	m_CBCoordinateBuffer = desc.m_pCBCoordinateData;
	m_CBCameraBuffer = desc.m_pCBCamera;
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
		XMMATRIX m_View = XMMatrixLookAtLH(eye, target, up);
		XMMATRIX m_Projection = XMMatrixPerspectiveFovLH(ImGuiMenu::CameraFov, GWinSizeX / (FLOAT)GWinSizeY, ImGuiMenu::CameraNearFar[0], ImGuiMenu::CameraNearFar[1]);

		m_CBCamera.CameraPosition = Vector4{ GetWorldTransform().Translation().x, GetWorldTransform().Translation().y, GetWorldTransform().Translation().z, 1.f};
		m_CBCoordinateData.View = XMMatrixTranspose(m_View);
		m_CBCoordinateData.Projection = XMMatrixTranspose(m_Projection);
	}

	m_DeviceContext->UpdateSubresource(m_CBCoordinateBuffer.Get(), 0, nullptr, &m_CBCoordinateData, 0, 0);
	m_DeviceContext->UpdateSubresource(m_CBCameraBuffer.Get(), 0, nullptr, &m_CBCamera, 0, 0);

	m_DeviceContext->VSSetConstantBuffers(0, 1, m_CBCoordinateBuffer.GetAddressOf());
	m_DeviceContext->VSSetConstantBuffers(2, 1, m_CBCameraBuffer.GetAddressOf());
	m_DeviceContext->PSSetConstantBuffers(0, 1, m_CBCoordinateBuffer.GetAddressOf());
	m_DeviceContext->PSSetConstantBuffers(2, 1, m_CBCameraBuffer.GetAddressOf());
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

void CameraComponent::GetViewMatrix(SimpleMath::Matrix& ViewMatrix)
{
	SimpleMath::Vector3 eye = m_World.Translation();
	SimpleMath::Vector3 target = m_World.Translation() + -m_World.Forward();
	SimpleMath::Vector3 up = m_World.Up();
	ViewMatrix = XMMatrixLookAtLH(eye, target, up);
}
