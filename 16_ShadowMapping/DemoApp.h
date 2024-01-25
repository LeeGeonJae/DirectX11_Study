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
	virtual void Initialize(UINT Width, UINT Height);	// ������ ������ ���� ���� �ٸ��� �����Ƿ� ���,����,���̱⸸ �Ѵ�.

protected:
	virtual void Update();
	virtual void Render();
	virtual void Release();

public:
	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	vector<shared_ptr<Object>> m_Objects;
};