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
	ImGuiMenu();
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

	static int ObjectCount;
	static int DrawComponentCount;
	static bool bIsFreezeCulling;

	static int FixedUpdateCount;
	static bool IsBlockNotOverlap;
	static int CameraBeginOverlapCount;
	static int CameraOverlapCount;
	static int CameraEndOverlapCount;

	static Vector3 CameraBeforePosition;
	static Vector3 CameraAfterPosition;

	static Vector3 ShadowDirection;
	static ID3D11ShaderResourceView* ShadowMap;

private:
	IDXGIFactory4* m_DXGIFactory;		// DXGI���丮
	IDXGIAdapter3* m_DXGIAdapter;		// ����ī�� ������ ���� ������ �������̽�
};