#pragma once

#include <directXTK/Mouse.h>
#include <directXTK/Keyboard.h>

class InputManager
{
private:
	InputManager() {}
public:
	static InputManager* GetInstance()
	{
		static InputManager instance;
		return &instance;
	}

public:
	bool Initalize(HWND hWnd);
	void Update();

public:
	inline DirectX::Keyboard* GetKeyboard();
	inline DirectX::Mouse* GetMouse();
	inline DirectX::Keyboard::State GetKeyboardState();
	inline DirectX::Mouse::State GetMouseState();
	inline DirectX::Keyboard::KeyboardStateTracker GetKeyboardStateTracker();
	inline DirectX::Mouse::ButtonStateTracker GetMouseStateTracker();

public:
	std::unique_ptr<DirectX::Keyboard>              m_Keyboard;
	std::unique_ptr<DirectX::Mouse>                 m_Mouse;

private:
	// input
	DirectX::Keyboard::KeyboardStateTracker         m_KeyboardStateTracker;
	DirectX::Mouse::ButtonStateTracker              m_MouseStateTracker;

	DirectX::Mouse::State                           m_MouseState;
	DirectX::Keyboard::State                        m_KeyboardState;
};

DirectX::Keyboard::State InputManager::GetKeyboardState()
{
	return m_KeyboardState;
}
DirectX::Mouse::State InputManager::GetMouseState()
{
	return m_MouseState;
}

DirectX::Keyboard::KeyboardStateTracker InputManager::GetKeyboardStateTracker()
{
	return m_KeyboardStateTracker;
}
DirectX::Mouse::ButtonStateTracker InputManager::GetMouseStateTracker()
{
	return m_MouseStateTracker;
}

DirectX::Keyboard* InputManager::GetKeyboard()
{
	return m_Keyboard.get();
}
DirectX::Mouse* InputManager::GetMouse()
{
	return m_Mouse.get();
}