#include "pch.h"

#include <cassert>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <directxtk/Keyboard.h>
#include <directxtk/Mouse.h>

#include "GameApp.h"
#include "TimeManager.h"
#include "InputManager.h"

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
GameApp* GameApp::m_pInstance = nullptr;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


LRESULT CALLBACK DefaultWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return GameApp::m_pInstance->WndProc(hWnd, message, wParam, lParam);
}

GameApp::GameApp(HINSTANCE hInstance)
    :m_hInstance(hInstance), m_szWindowClass(L"DefaultWindowCalss"), m_szTitle(L"GameApp"), m_ClientWidth(1024), m_ClientHeight(768)
{
    GameApp::m_pInstance = this;
    m_wcex.hInstance = hInstance;
    m_wcex.cbSize = sizeof(WNDCLASSEX);
    m_wcex.style = CS_HREDRAW | CS_VREDRAW;
    m_wcex.lpfnWndProc = DefaultWndProc;
    m_wcex.cbClsExtra = 0;
    m_wcex.cbWndExtra = 0;
    m_wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    m_wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    m_wcex.lpszClassName = m_szWindowClass;
}

GameApp::~GameApp()
{
}

void GameApp::Initialize(UINT Width, UINT Height)
{
    m_ClientWidth = Width;
    m_ClientHeight = Height;

    // 등록
    RegisterClassExW(&m_wcex);

    // 원하는 크기가 조정되어 리턴
    RECT rcClient = { 0, 0, (LONG)Width, (LONG)Height };
    AdjustWindowRect(&rcClient, WS_OVERLAPPEDWINDOW, FALSE);

    //생성
    m_hWnd = CreateWindowW(m_szWindowClass, m_szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0,
        rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
        nullptr, nullptr, m_hInstance, nullptr);

    assert(m_hWnd);

    // 윈도우 보이기
    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);

    TimeManager::GetInstance()->Initialize();
    InputManager::GetInstance()->Initalize(m_hWnd);

    srand(static_cast<unsigned int>(time(NULL)));
}

bool GameApp::Run()
{
    // PeekMessage 메세지가 있으면 true,없으면 false
    while (TRUE)
    {
        if (PeekMessage(&m_msg, NULL, 0, 0, PM_REMOVE))
        {
            if (m_msg.message == WM_QUIT)
                break;

            //윈도우 메시지 처리 
            TranslateMessage(&m_msg); // 키입력관련 메시지 변환  WM_KEYDOWN -> WM_CHAR
            DispatchMessage(&m_msg);
        }
        else
        {
            Update();
            Render();
        }
    }
    return 0;
}

void GameApp::Update()
{
    TimeManager::GetInstance()->Update();
    InputManager::GetInstance()->Update();
}

void GameApp::Render()
{
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT GameApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_ACTIVATEAPP:
        DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
        DirectX::Mouse::ProcessMessage(message, wParam, lParam);
        break;
    case WM_INPUT:
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEWHEEL:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_MOUSEHOVER:
        Mouse::ProcessMessage(message, wParam, lParam);
        break;
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        Keyboard::ProcessMessage(message, wParam, lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
