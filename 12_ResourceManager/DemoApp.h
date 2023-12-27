#pragma once
#include <d3d11.h>
#include "../Engine/GameApp.h"
#include "../Engine/pch.h"
#include "BufferStruct.h"
#include "Model.h"

#include <imgui.h>

class ImGuiMenu;
class ModelLoadManager;
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
	void createDeviceAndSwapChain();
	void createRenderTargetView();
	void setViewport();
	void createDeathStencilView();

private:
	void createConstantBuffer();

	void createRasterizeState();
	void createSamplerState();
	void createBlendState();

public:
	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	void setTransform();

private:
	// 렌더링 파이프라인을 구성하는 필수 객체의 인터페이스 ( 뎊스 스텐실 뷰도 있지만 아직 사용하지 않는다.)
	ComPtr<ID3D11Device> m_Device = nullptr;						// 디바이스	
	ComPtr<ID3D11DeviceContext> m_DeviceContext = nullptr;			// 즉시 디바이스 컨텍스트
	ComPtr<IDXGISwapChain> m_SwapChain = nullptr;					// 스왑체인

	// RTV
	ComPtr<ID3D11RenderTargetView> m_RenderTargetView = nullptr;	// 렌더링 타겟뷰

	// Misc
	D3D11_VIEWPORT m_Viewport = { 0 };

private:

	// RAS
	ComPtr<ID3D11RasterizerState> m_rasterizerState = nullptr;

	// RS
	ComPtr<ID3D11DepthStencilView> m_depthStancilView = nullptr;

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

	ComPtr<ID3D11Buffer> m_pCBCoordinateData = nullptr;
	ComPtr<ID3D11Buffer> m_pCBLight = nullptr;
	ComPtr<ID3D11Buffer> m_pCBCamera = nullptr;
	ComPtr<ID3D11Buffer> m_pCBUseTextureMap = nullptr;
	shared_ptr<ModelCBBuffer> m_ModelCBBuffer = nullptr;

	DirectX::SimpleMath::Matrix m_World;
	DirectX::SimpleMath::Matrix m_View;
	DirectX::SimpleMath::Matrix m_Projection;
	XMFLOAT4 m_DirectionLight;
	XMFLOAT4 m_LightColor;

	struct ModelInstance
	{
		Model* Model;
		DirectX::SimpleMath::Matrix World;
		float TimePos = 0;

		void Update(ComPtr<ID3D11DeviceContext> deviceContext)
		{
			TimePos += TimeManager::GetInstance()->GetfDT();
			Model->Update(deviceContext);
		}
	};

private:
	vector<shared_ptr<Model>> myModels;
};