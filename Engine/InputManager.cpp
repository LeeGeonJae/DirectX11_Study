#include "pch.h"
#include "InputManager.h"

bool InputManager::Initalize(HWND hWnd)
{
	m_Keyboard = std::make_unique<Keyboard>();
	m_Mouse = std::make_unique<Mouse>();
	m_Mouse->SetWindow(hWnd);

    return true;
}

void InputManager::Update()
{
	m_MouseState = m_Mouse->GetState();
	m_MouseStateTracker.Update(m_MouseState);

	m_KeyboardState = m_Keyboard->GetState();
	m_KeyboardStateTracker.Update(m_KeyboardState);
}
