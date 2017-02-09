#pragma once
#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h> // include the Xinput API
#include "Engine/MathUtils.hpp"
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Explicitly use XInput v9_1_0, since v1_4 is not supported under Windows 7

const int NUM_BUTTONS = 32769;
enum XBOX_BUTTONS {
	XBOX_A = 4096,
	XBOX_B = 8192,
	XBOX_X = 16384,
	XBOX_Y = 32768,
	XBOX_LB = 256,
	XBOX_RB = 512,
	XBOX_LS = 64,
	XBOX_RS = 128,
	XBOX_BACK = 32,
	XBOX_START = 16,
	XBOX_DPAD_UP = 1,
	XBOX_DPAD_UPLEFT = 5,
	XBOX_DPAD_LEFT = 4,
	XBOX_DPAD_DOWNLEFT = 6,
	XBOX_DPAD_DOWN = 2,
	XBOX_DPAD_DOWNRIGHT = 10,
	XBOX_DPAD_RIGHT = 8,
	XBOX_DPAD_UPRIGHT = 9
};

struct ButtonState
{
	ButtonState::ButtonState();
	XBOX_BUTTONS m_button;
	bool m_isDown;
	bool m_justPressed;
};



class XboxController
{
public:
	XboxController::XboxController();
	XboxController::XboxController(int controllerNumber);

	void XboxController::Update();
	Vector2 XboxController::GetLeftStick();
	Vector2 XboxController::GetRightStick();
	Vector2 XboxController::GetLeftStickF();
	Vector2 XboxController::GetRightStickF();
	float GetLeftTrigger();
	float GetRightTrigger();
	bool IsButtonDown(XBOX_BUTTONS button);
	bool IsButtonJustPressed(XBOX_BUTTONS button);
	bool IsButtonPressedOnce(XBOX_BUTTONS button);
	void ClearButtonJustPressedState();
	void UpdateButtonJustPressedState();
	void SetControllerNumber(int controllerNumber);
private:
	XINPUT_STATE m_xboxControllerState;
	int m_controllerNumber;
	DWORD m_errorStatus;
	ButtonState m_buttons[NUM_BUTTONS];
	int m_controllerLastPressedButtons;
	void XboxController::HandleStickDeadZone(Vector2& thumbStick, bool isRightStick);	
	void XboxController::HandleStickDeadZoneF(Vector2& thumbStick, bool isRightStick);//Gets Cartesian values
	void XboxController::InitializeButtonArray();
};