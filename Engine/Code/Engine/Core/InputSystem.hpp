#pragma once
#include "Engine/Core/XboxController.hpp"
#include "Engine/Math/IntVector2.hpp"

const int NUM_KEYS = 256;


struct KeyButtonState
{
	bool isKeyDown;
	bool didKeyJustChange;
	KeyButtonState::KeyButtonState();
};

class InputSystem
{
public:
	InputSystem();
	InputSystem(void* windowHandle);

	void Update();
	void SetKeyDownStatus(unsigned char windowsKeyCode, bool isNowDown);
	void UpdateKeyJustPressedState();
	bool IsKeyDown(unsigned char windowsKeyCode);
	bool IsKeyJustPressed(unsigned char windowsKeyCode);
	bool IsKeyPressed(unsigned char windowsKeyCode);
	void ShowMouse(bool mouseState);
	IntVector2 GetMousePos();
	void SetMousePos(int x, int y);
	void SetMouseWheelDirection(int dir);
	int GetMouseWheelDirection();
	

	XboxController m_controllers[4];
	void* m_window;
	bool m_isInFocus;
	bool m_isMouseShown;
	IntVector2 m_mousePos;
	IntVector2 m_mouseDelta;
	int m_mouseWheelDirection;

private:
	KeyButtonState m_keyState[NUM_KEYS];
};