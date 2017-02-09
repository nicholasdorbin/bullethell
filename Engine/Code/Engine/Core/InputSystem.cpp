#include "Engine/Core/InputSystem.hpp"


KeyButtonState::KeyButtonState()
{
	isKeyDown = false;
	didKeyJustChange = false;
}

InputSystem::InputSystem()
{
	m_window = nullptr;;

	for (int keyIndex = 0; keyIndex < NUM_KEYS; ++keyIndex)
	{
		m_keyState[keyIndex].isKeyDown = false;
	}
	m_controllers[0].SetControllerNumber(0);
	m_controllers[1].SetControllerNumber(1);
	m_controllers[2].SetControllerNumber(2);
	m_controllers[3].SetControllerNumber(3);

	//Update once to initialize controller state
	UpdateKeyJustPressedState();
	for (int i = 0; i < 4; i++)
	{
		m_controllers[i].Update();
	}
	m_isInFocus = false;
	m_isMouseShown = true;
	m_mousePos.SetXY(0, 0);
	m_mouseDelta.SetXY(0, 0);
	m_mouseWheelDirection = 0;
}

InputSystem::InputSystem(void* windowHandle)
: m_window(nullptr)
{
	m_window = windowHandle;

	for (int keyIndex = 0; keyIndex < NUM_KEYS; ++keyIndex)
	{
		m_keyState[keyIndex].isKeyDown = false;
	}
	m_controllers[0].SetControllerNumber(0);
	m_controllers[1].SetControllerNumber(1);
	m_controllers[2].SetControllerNumber(2);
	m_controllers[3].SetControllerNumber(3);

	//Update once to initialize controller state
	UpdateKeyJustPressedState();
	for (int i = 0; i < 4; i++)
	{
		m_controllers[i].Update();
	}
	m_isInFocus = false;
	m_isMouseShown = true;
	m_mousePos.SetXY(0, 0);
	m_mouseDelta.SetXY(0, 0);
	m_mouseWheelDirection = 0;
}

void InputSystem::Update()
{
	IntVector2 oldPos = m_mousePos;
	m_mousePos = GetMousePos();
	m_mouseDelta = m_mousePos - oldPos;
	//UpdateKeyJustPressedState();
	for (int i = 0; i < 4; i++)
	{
		m_controllers[i].Update();
	}

	HWND focusWindow = GetForegroundWindow();
	if (focusWindow == m_window)
	{
		m_isInFocus = true;
	}
	else
	{
		m_isInFocus = false;
	}

}

void InputSystem::SetKeyDownStatus(unsigned char windowsKeyCode, bool isNowDown)
{
	if (m_keyState[windowsKeyCode].isKeyDown != isNowDown)
		m_keyState[windowsKeyCode].didKeyJustChange = true;
	m_keyState[windowsKeyCode].isKeyDown = isNowDown;
}

void InputSystem::UpdateKeyJustPressedState()
{
	for (int keyIndex = 0; keyIndex < NUM_KEYS; ++keyIndex)
	{
		m_keyState[keyIndex].didKeyJustChange = false;
	}
	m_mouseWheelDirection = 0;
}

bool InputSystem::IsKeyDown(unsigned char windowsKeyCode)
{
	return m_keyState[windowsKeyCode].isKeyDown;
}

bool InputSystem::IsKeyJustPressed(unsigned char windowsKeyCode)
{
	return m_keyState[windowsKeyCode].didKeyJustChange;
}

bool InputSystem::IsKeyPressed(unsigned char windowsKeyCode)
{
	return (IsKeyDown(windowsKeyCode) && IsKeyJustPressed(windowsKeyCode));
}

void InputSystem::ShowMouse(bool mouseState)
{
	ShowCursor(mouseState);
	m_isMouseShown = mouseState;
}

IntVector2 InputSystem::GetMousePos()
{
	POINT p;
	GetCursorPos(&p);
	IntVector2 result = IntVector2((int)p.x, (int)p.y);
	return result;
}

void InputSystem::SetMousePos(int x, int y)
{
	SetCursorPos(x, y);
}

void InputSystem::SetMouseWheelDirection(int dir)
{
	m_mouseWheelDirection = dir;
}

int InputSystem::GetMouseWheelDirection()
{
	return m_mouseWheelDirection;
}

