#pragma once

#include <d3d11.h>
#include "../Engine/GameApp.h"
#include "../Engine/pch.h"
#include "BufferStruct.h"

#include <imgui.h>

class RenderManager
{
private:
	RenderManager();
public:
	static RenderManager* GetInstance()
	{
		static RenderManager renderManager;
		return &renderManager;
	}

public:
	void Init(HWND _hwnd, UINT _clientHeight, UINT _clientWidth);

public:
	inline ComPtr<ID3D11Device> GetDevice();
	inline ComPtr<ID3D11DeviceContext> GetDeviceContext();



private:
	void createDeviceAndSwapChain();
	void createRenderTargetView();
	void setViewport();
	void createDeathStencilView();

private:
	void createRasterizeState();


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
	HWND m_hWnd;
	UINT m_ClientHeight;
	UINT m_ClientWidth;
};

ComPtr<ID3D11Device> RenderManager::GetDevice()
{
	return m_Device;
}
ComPtr<ID3D11DeviceContext> RenderManager::GetDeviceContext()
{
	return m_DeviceContext;
}

#define DEVICE			RenderManager::GetInstance()->GetDevice()
#define DEVICE_CONTEXT	RenderManager::GetInstance()->GetDeviceContext()