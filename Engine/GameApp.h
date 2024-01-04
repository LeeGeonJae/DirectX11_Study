#pragma once


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

public:
	static GameApp* m_pInstance;

public:
	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
