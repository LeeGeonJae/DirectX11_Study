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

	static Vector3 CubePosition;
	static Vector2 CubeRotation;
	static Vector3 CubeScale;

	static Vector4 DirectionLightDir;
	static Vector4 DirectionLightColor;
	static float SpecularPower;
	static Vector3 AmbientColor;

	static bool bIsDiffuseMap;
	static bool bIsNormalMap;
	static bool bIsSpecularMap;
	static bool bIsEmissiveMap;
	static bool bIsGammaCorrection;
	static float EmissivePower;
	static float OpacityValue;

	static float AnimationSpeed;

private:
	DemoApp* m_Owner;
};