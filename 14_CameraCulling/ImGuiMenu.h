#pragma once

#include "string"
#include "../Engine/Header.h"
#include "../Engine/Types.h"

#include <vector>
#include <directxtk/SimpleMath.h>
#include <dxgi1_4.h>

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

private:
	void getVideoMemoryInfo(std::string& out);
	void getSystemMemoryInfo(std::string& out);

public:
	static float CameraFov;
	static float CameraNearFar[2];

	static Vector3 ModelPosition;
	static Vector2 ModelRotation;
	static Vector3 ModelScale;
	static float ModelAnimationSpeed;

	static Vector4 DirectionLightDir;
	static Vector4 DirectionLightColor;
	static float SpecularPower;
	static Vector3 AmbientColor;

	static bool bIsNormalMap;
	static bool bIsSpecularMap;
	static bool bIsEmissiveMap;
	static bool bIsGammaCorrection;
	static float EmissivePower;
	static float OpacityValue;

	static bool DyingAnimationFBX;
	static bool SkinningTestFBX;
	static bool ZeldaFBX;
	static bool VampireFBX;
	static bool cerberusFBX;

private:
	DemoApp* m_Owner;

	IDXGIFactory4* m_DXGIFactory;		// DXGI팩토리
	IDXGIAdapter3* m_DXGIAdapter;		// 비디오카드 정보에 접근 가능한 인터페이스
};