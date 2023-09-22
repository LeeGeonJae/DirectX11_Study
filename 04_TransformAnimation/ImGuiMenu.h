#pragma once

#include "../Engine/pch.h"
#include <Windows.h>

class DemoApp;

class ImGuiMenu
{
public:
	ImGuiMenu(DemoApp* owner);
	~ImGuiMenu();

public:
	void Init(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* devicecontext);
	void Update();
	void Render();
	void Release();

public:
	inline float* GetCameraPos();
	inline float GetCameraFov();
	inline float GetCameraNear();
	inline float GetCameraFar();
	inline Vector3 GetFirstCubePosition();
	inline Vector3 GetSecondCubePosition();
	inline Vector3 GetThirdCubePosition();

private:
	float m_CameraPos[4];
	float m_CameraFov;
	float m_CameraNearFar[2];

	Vector3 m_FirstCubePosition;
	Vector3 m_SecondCubePosition;
	Vector3 m_ThirdCubePosition;

private:
	DemoApp* m_Owner;
};

float* ImGuiMenu::GetCameraPos()
{
	return m_CameraPos;
}
float ImGuiMenu::GetCameraFov()
{
	return m_CameraFov / 180.f * 3.14;
}
float ImGuiMenu::GetCameraNear()
{
	return m_CameraNearFar[0];
}
float ImGuiMenu::GetCameraFar()
{
	return m_CameraNearFar[1];
}
Vector3 ImGuiMenu::GetFirstCubePosition()
{
	return m_FirstCubePosition;
}
Vector3 ImGuiMenu::GetSecondCubePosition()
{
	return m_SecondCubePosition;
}
Vector3 ImGuiMenu::GetThirdCubePosition()
{
	return m_ThirdCubePosition;
}