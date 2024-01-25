#pragma once
#include "../Engine/GameApp.h"
#include "../Engine/pch.h"

#include <imgui.h>

class Object;

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

public:
	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	vector<shared_ptr<Object>> m_Objects;
};