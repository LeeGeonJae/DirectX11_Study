#pragma once
#include <d3d11.h>
#include "../Engine/GameApp.h"
#include "../Engine/pch.h"
#include "BufferStruct.h"

#include <imgui.h>

class ImGuiMenu;
class ModelLoadManager;
class Model;
class RenderComponent;
class Component;
class CameraComponent;
class Object;
class Model;

class DemoApp
	: public GameApp
{
public:
	DemoApp(HINSTANCE hInstance);
	~DemoApp();

public:
	virtual void Initialize(UINT Width, UINT Height);	// 윈도우 정보는 게임 마다 다를수 있으므로 등록,생성,보이기만 한다.

protected:
	virtual void Update();
	virtual void Render();
	virtual void Release();

private:
	void renderBegin();
	void renderEnd();

private:
	void initD3D();
	void initScene();

private:
	void createDeathStencilView();

private:
	template <typename T>
	ComPtr<ID3D11Buffer> createConstantBuffer();
	void constantBufferSetting();

	void createRasterizeState();
	void createSamplerState();
	void createBlendState();

public:
	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);



private:



	ComPtr<ID3D11SamplerState> m_samplerState = nullptr;
	ComPtr<ID3D11SamplerState> m_BRDF_Sampler = nullptr;
	ComPtr<ID3D11BlendState> m_blendState = nullptr;
	// [ CPU <-> RAM ] [ GPU <-> VRAM ]

private:
	shared_ptr<ImGuiMenu> m_imgui;

	CBCoordinateData		m_CBCoordinateData;
	CBLightData				m_CBLightData;
	CBCameraData			m_CBCamera;
	CBUseTextureMap			m_CBNormalMap;

	ComPtr<ID3D11Buffer>		m_pCBCoordinateData = nullptr;
	ComPtr<ID3D11Buffer>		m_pCBLight = nullptr;
	ComPtr<ID3D11Buffer>		m_pCBCamera = nullptr;
	ComPtr<ID3D11Buffer>		m_pCBUseTextureMap = nullptr;
	shared_ptr<ModelCBBuffer>	m_ModelCBBuffer = nullptr;

	XMFLOAT4 m_DirectionLight;
	XMFLOAT4 m_LightColor;

private:
	vector<shared_ptr<Object>> m_Objects;
};

template <typename T>
ComPtr<ID3D11Buffer> DemoApp::createConstantBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	ComPtr<ID3D11Buffer> buffer;

	desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(T);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = m_Device->CreateBuffer(&desc, nullptr, buffer.GetAddressOf());
	assert(SUCCEEDED(hr));
	
	return buffer;
}