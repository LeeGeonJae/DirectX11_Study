#pragma once

#include "TimeManager.h"

#include <wrl.h>

#define MAX_LOADSTRING 100

class GameApp
{
public:
	GameApp(HINSTANCE hInstance);
	virtual ~GameApp();

public:
	virtual void Initialize(UINT Width, UINT Height);
	bool Run();

protected:
	virtual void Update();
	virtual void Render();

protected:
	HINSTANCE m_hInstance;
	WNDCLASSEXW m_wcex;
	MSG m_msg;
	HWND m_hWnd;
	WCHAR m_szTitle[MAX_LOADSTRING];
	WCHAR m_szWindowClass[MAX_LOADSTRING];
	UINT m_ClientWidth;
	UINT m_ClientHeight;

	float m_previousTime;
	float m_currentTime;
	GameTimer m_Timer;

private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_device = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deivceContext = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain = nullptr;

public:
	static GameApp* m_pInstance;

public:
	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
