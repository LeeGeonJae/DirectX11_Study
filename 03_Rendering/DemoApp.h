#pragma once
#include <d3d11.h>
#include "../Engine/GameApp.h"
#include "../Engine/pch.h"

class DemoApp
	: public GameApp
{
public:
	DemoApp(HINSTANCE hInstance);
	~DemoApp();

public:
	virtual void Initialize(UINT Width, UINT Height);	// ������ ������ ���� ���� �ٸ��� �����Ƿ� ���,����,���̱⸸ �Ѵ�.

protected:
	virtual void Update();
	virtual void Render();

private:
	void renderBegin();
	void renderEnd();

private:
	void InitD3D();

	void createDeviceAndSwapChain();
	void createRenderTargetView();
	void setViewport();

private:
	void createGeometry();
	void createInputLayout();
	void createVS();
	void createPS();

	void createRasterizerState();
	void createSamplerState();
	void createBlendState();

	void createSRV();
	void createConstantBuffer();

	void LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob);


private:
	// ������ ������������ �����ϴ� �ʼ� ��ü�� �������̽� ( �X�� ���ٽ� �䵵 ������ ���� ������� �ʴ´�.)
	ComPtr<ID3D11Device> m_Device = nullptr;						// ����̽�	
	ComPtr<ID3D11DeviceContext> m_DeviceContext = nullptr;		// ��� ����̽� ���ؽ�Ʈ
	ComPtr<IDXGISwapChain> m_SwapChain = nullptr;					// ����ü��

	// RTV
	ComPtr<ID3D11RenderTargetView> m_RenderTargetView = nullptr;	// ������ Ÿ�ٺ�

	// Misc
	D3D11_VIEWPORT m_Viewport = { 0 };

private:
	// Geometry
	vector<Vertex> m_vertices;
	ComPtr<ID3D11Buffer> m_vertexBuffer = nullptr;
	vector<uint32> m_indices;
	ComPtr<ID3D11Buffer> m_indexBuffer = nullptr;
	ComPtr<ID3D11InputLayout> m_inputLayout = nullptr;

	// VS
	ComPtr<ID3D11VertexShader> m_vertexShader = nullptr;
	ComPtr<ID3DBlob> m_vsBlob = nullptr;

	// RAS
	ComPtr<ID3D11RasterizerState> m_rasterizerState = nullptr;

	// PS
	ComPtr<ID3D11PixelShader> m_pixelShader = nullptr;
	ComPtr<ID3DBlob> m_psBlob = nullptr;

	// SRV
	ComPtr<ID3D11ShaderResourceView> m_shaderResoureceView = nullptr;

	ComPtr<ID3D11SamplerState> m_samplerState = nullptr;
	ComPtr<ID3D11BlendState> m_blendState = nullptr;
	// [ CPU <-> RAM ] [ GPU <-> VRAM ]

private:
	TransformData m_transformData;
	ComPtr<ID3D11Buffer> m_constantBuffer;
};