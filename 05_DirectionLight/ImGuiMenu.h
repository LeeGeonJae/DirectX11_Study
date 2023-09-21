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
	inline Vec3 GetFirstCubePosition();
	inline Vec3 GetSecondCubePosition();
	inline Vec3 GetThirdCubePosition();

private:
	float m_CameraPos[4];
	float m_CameraFov;
	float m_CameraNearFar[2];

	Vec3 m_FirstCubePosition;
	Vec3 m_SecondCubePosition;
	Vec3 m_ThirdCubePosition;

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
Vec3 ImGuiMenu::GetFirstCubePosition()
{
	return m_FirstCubePosition;
}
Vec3 ImGuiMenu::GetSecondCubePosition()
{
	return m_SecondCubePosition;
}
Vec3 ImGuiMenu::GetThirdCubePosition()
{
	return m_ThirdCubePosition;
}