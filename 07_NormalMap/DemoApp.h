#pragma once
#include <d3d11.h>
#include "../Engine/GameApp.h"
#include "../Engine/pch.h"
#include "ImGuiMenu.h"
#include "BufferStruct.h"

#include <imgui.h>

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
	void createGeometry();
	void createInputLayout();
	void createConstantBuffer();
	void createVS();
	void createPS();

	void createRasterizeState();
	void createSamplerState();
	void createBlendState();
	void createSRV();

	void LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob);

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
	// Geometry
	vector<ShaderVertex> m_vertices;
	ComPtr<ID3D11Buffer> m_vertexBuffer = nullptr;
	vector<uint32> m_indices;
	ComPtr<ID3D11Buffer> m_indexBuffer = nullptr;
	ComPtr<ID3D11InputLayout> m_inputLayout = nullptr;

	// VS
	ComPtr<ID3D11VertexShader> m_vertexShader = nullptr;
	ComPtr<ID3DBlob> m_vsBlob = nullptr;

	// RAS
	ComPtr<ID3D11RasterizerState> m_rasterizerState = nullptr;

	// RS
	ComPtr<ID3D11DepthStencilView> m_depthStancilView = nullptr;

	// PS
	ComPtr<ID3D11PixelShader> m_pixelShader = nullptr;
	ComPtr<ID3DBlob> m_psBlob = nullptr;

	// SRV
	ComPtr<ID3D11ShaderResourceView> m_shaderResourceView1 = nullptr;
	ComPtr<ID3D11ShaderResourceView> m_shaderResourceView2 = nullptr;
	ComPtr<ID3D11ShaderResourceView> m_shaderResourceView3 = nullptr;

	ComPtr<ID3D11SamplerState> m_samplerState = nullptr;
	ComPtr<ID3D11BlendState> m_blendState = nullptr;
	// [ CPU <-> RAM ] [ GPU <-> VRAM ]

private:
	ImGuiMenu* m_imgui;

	CBCoordinateData	m_CBChangesEveryFrame;
	CBLightData			m_CBLightData;
	CBCameraData		m_CBCamera;
	CBNormalMap			m_CBNormalMap;

	ComPtr<ID3D11Buffer> m_pCBChangesEveryFrame = nullptr;
	ComPtr<ID3D11Buffer> m_pCBLight = nullptr;
	ComPtr<ID3D11Buffer> m_pCBCamera = nullptr;
	ComPtr<ID3D11Buffer> m_pCBNormalMap = nullptr;

	DirectX::SimpleMath::Matrix m_World;
	DirectX::SimpleMath::Matrix m_View;
	DirectX::SimpleMath::Matrix m_Projection;

private:
	XMFLOAT4 m_DirectionLight;
	XMFLOAT4 m_LightColor;
};