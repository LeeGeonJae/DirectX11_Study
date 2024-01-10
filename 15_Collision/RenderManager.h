#pragma once

#include <d3d11.h>
#include "../Engine/GameApp.h"
#include "../Engine/Header.h"
#include "BufferStruct.h"

class ImGuiMenu;
class RenderComponent;
class CameraComponent;
class BoxComponent;
class SphereComponent;
class OrientedBoxComponent;

enum class CB_DATA_TYPE
{
	COORDINATE,
	LIGHT,
	USE_TEXTURE_MAP,
};

class RenderManager
{
private:
	RenderManager() {}
public:
	static RenderManager* GetInstance()
	{
		static RenderManager renderManager;
		return &renderManager;
	}

public:
	void Init(HWND _hwnd, UINT _clientHeight, UINT _clientWidth);
	void Update();
	void Render();
	void Release();

public:
	inline void SetCamera(weak_ptr<CameraComponent> _cameraComponent);
	inline void SetRender(shared_ptr<RenderComponent> _renderComponent);
	inline void SetBox(BoxComponent* _boxComponent);
	inline void SetSphere(SphereComponent* _sphereComponent);
	inline void SetOriented(OrientedBoxComponent* _orientedComponent);

public:
	inline ComPtr<ID3D11Device> GetDevice();
	inline ComPtr<ID3D11DeviceContext> GetDeviceContext();
	inline ComPtr<ID3D11Buffer> GetCB(CB_DATA_TYPE _type);
	inline shared_ptr<ModelCBBuffer> GetModelCB();

private:
	void renderDebugDraw();
	void renderMesh();

private:
	void renderBegin();
	void renderEnd();
	void constantBufferSetting();

private:
	void createDeviceAndSwapChain();
	void createRenderTargetView();
	void setViewport();
	void createDeathStencilView();

	void createRasterizeState();
	void createBlendState();
	void createSamplerState();

	template <typename T>
	ComPtr<ID3D11Buffer> createConstantBuffer();

private:
	vector<shared_ptr<RenderComponent>> m_RenderVec;
	vector <BoxComponent*> m_BoxCollisionVec;
	vector <SphereComponent*> m_SphereCollisionVec;
	vector <OrientedBoxComponent*> m_OrientedCollisionVec;

	weak_ptr<CameraComponent> m_pCamera;
	DirectX::BoundingFrustum	m_Frustum;
	DirectX::SimpleMath::Matrix m_View;
	DirectX::SimpleMath::Matrix m_Projection;


private:
	ComPtr<ID3D11Device> m_Device = nullptr;						// 디바이스	
	ComPtr<ID3D11DeviceContext> m_DeviceContext = nullptr;			// 즉시 디바이스 컨텍스트
	ComPtr<IDXGISwapChain> m_SwapChain = nullptr;					// 스왑체인

	// RTV
	ComPtr<ID3D11RenderTargetView> m_RenderTargetView = nullptr;	// 렌더링 타겟뷰

	// Misc
	D3D11_VIEWPORT m_Viewport = { 0 };

	// RAS
	ComPtr<ID3D11RasterizerState> m_rasterizerState = nullptr;

	// RS
	ComPtr<ID3D11DepthStencilView> m_depthStancilView = nullptr;

private:
	ComPtr<ID3D11SamplerState> m_samplerState = nullptr;
	ComPtr<ID3D11SamplerState> m_BRDF_Sampler = nullptr;
	ComPtr<ID3D11BlendState> m_blendState = nullptr;
	// [ CPU <-> RAM ] [ GPU <-> VRAM ]

private:
	CBLightData m_CBLightData;
	CBUseTextureMap m_CBUseTextureMap;
	CBCoordinateData			m_CBCoordinateData;

	ComPtr<ID3D11Buffer>		m_pCBCoordinate;
	ComPtr<ID3D11Buffer>		m_pCBLight = nullptr;
	ComPtr<ID3D11Buffer>		m_pCBUseTextureMap = nullptr;
	shared_ptr<ModelCBBuffer>	m_pCBModel = nullptr;

	XMFLOAT4 m_DirectionLight;
	XMFLOAT4 m_LightColor;

private:
	HWND m_hWnd;
	UINT m_ClientHeight;
	UINT m_ClientWidth;

private:
	shared_ptr<ImGuiMenu> m_imgui;
};

ComPtr<ID3D11Device> RenderManager::GetDevice()
{
	return m_Device;
}
ComPtr<ID3D11DeviceContext> RenderManager::GetDeviceContext()
{
	return m_DeviceContext;
}

void RenderManager::SetCamera(weak_ptr<CameraComponent> _cameraComponent)
{
	m_pCamera = _cameraComponent;
}

void RenderManager::SetRender(shared_ptr<RenderComponent> _renderComponent)
{
	m_RenderVec.push_back(_renderComponent);
}

void RenderManager::SetBox(BoxComponent* _boxComponent)
{
	m_BoxCollisionVec.push_back(_boxComponent);
}
void RenderManager::SetSphere(SphereComponent* _sphereComponent)
{
	m_SphereCollisionVec.push_back(_sphereComponent);
}
void RenderManager::SetOriented(OrientedBoxComponent* _orientedComponent)
{
	m_OrientedCollisionVec.push_back(_orientedComponent);
}

ComPtr<ID3D11Buffer> RenderManager::GetCB(CB_DATA_TYPE _type)
{
	switch (_type)
	{
	case CB_DATA_TYPE::COORDINATE:
	{
		return m_pCBCoordinate;
	}
	case CB_DATA_TYPE::LIGHT:
	{
		return m_pCBLight;
	}
	case CB_DATA_TYPE::USE_TEXTURE_MAP:
	{
		return m_pCBUseTextureMap;
	}
	}
}

shared_ptr<ModelCBBuffer> RenderManager::GetModelCB()
{
	return m_pCBModel;
}

template <typename T>
ComPtr<ID3D11Buffer> RenderManager::createConstantBuffer()
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

#define DEVICE			RenderManager::GetInstance()->GetDevice()
#define DEVICE_CONTEXT	RenderManager::GetInstance()->GetDeviceContext()