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
	void Render();
	void Release();

public:
	static Vector4 CameraPos;
	static float CameraFov;
	static float CameraNearFar[2];

	static Vector3 FirstCubePosition;
	static Vector3 SecondCubePosition;
	static Vector3 ThirdCubePosition;

	static Vector4 DirectionLightDir1;
	static Vector4 DirectionLightColor1;
	static Vector4 DirectionLightDir2;
	static Vector4 DirectionLightColor2;

private:
	DemoApp* m_Owner;
};