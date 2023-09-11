#include "pch.h"

#include "GameApp.h"
#include <cassert>

// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
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

    // ���
    RegisterClassExW(&m_wcex);

    // ���ϴ� ũ�Ⱑ �����Ǿ� ����
    RECT rcClient = { 0, 0, (LONG)Width, (LONG)Height };
    AdjustWindowRect(&rcClient, WS_OVERLAPPEDWINDOW, FALSE);

    //����
    m_hWnd = CreateWindowW(m_szWindowClass, m_szTitle, WS_OVERLAPPEDWINDOW,
        100, 100,	// ���� ��ġ
        rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
        nullptr, nullptr, m_hInstance, nullptr);

    assert(m_hWnd);

    // ������ ���̱�
    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);

}

bool GameApp::Run()
{
    // PeekMessage �޼����� ������ true,������ false
    while (TRUE)
    {
        if (PeekMessage(&m_msg, NULL, 0, 0, PM_REMOVE))
        {
            if (m_msg.message == WM_QUIT)
                break;

            //������ �޽��� ó�� 
            TranslateMessage(&m_msg); // Ű�Է°��� �޽��� ��ȯ  WM_KEYDOWN -> WM_CHAR
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
}

void GameApp::Render()
{
}

void GameApp::init3D()
{
    createDeviceAndSwapChain();
}

void GameApp::createDeviceAndSwapChain()
{
    HRESULT hr;
    DXGI_SWAP_CHAIN_DESC swapdesc;
    ZeroMemory(&swapdesc, sizeof(swapdesc));        // swapdesc �� �ִ� ������ 0���� �ʱ�ȭ
    {
        swapdesc.BufferDesc.Width = GWinSizeX;
        swapdesc.BufferDesc.Height = GWinSizeY;
        swapdesc.BufferDesc.RefreshRate.Numerator = 60;         // ȭ�� �ֻ����� ���õ� �κ�
        swapdesc.BufferDesc.RefreshRate.Denominator = 1;
        swapdesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapdesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        swapdesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        swapdesc.SampleDesc.Count = 1;
        swapdesc.SampleDesc.Quality = 0;
        swapdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapdesc.OutputWindow = m_hWnd;
        swapdesc.Windowed = true;
        swapdesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    }

    hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &swapdesc,
        m_SwapChain.GetAddressOf(),
        m_device.GetAddressOf(),
        nullptr,
        m_deivceContext.GetAddressOf()
    );
    assert(SUCCEEDED(hr));

}

void GameApp::createRenderTargetView()
{
}

LRESULT GameApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
