#include "Engine/Core/XboxController.hpp"

ButtonState::ButtonState()
{
	m_button = XBOX_A;
	m_justPressed = false;
	m_isDown = false;
}
XboxController::XboxController()
{
	m_controllerNumber = 0;
	m_controllerLastPressedButtons = 0;
	InitializeButtonArray();
}

XboxController::XboxController(int controllerNumber)
{
	m_controllerNumber = controllerNumber;
	m_controllerLastPressedButtons = 0;
	InitializeButtonArray();
}

void XboxController::Update()
{
	memset(&m_xboxControllerState, 0, sizeof(m_xboxControllerState));
	m_errorStatus = XInputGetState(m_controllerNumber, &m_xboxControllerState);

	for (int i = 0; i < NUM_BUTTONS; i++)
	{
		if (m_xboxControllerState.Gamepad.wButtons & m_buttons[i].m_button)
		{
			m_buttons[i].m_isDown = true;
			
			if ((m_xboxControllerState.Gamepad.wButtons & m_buttons[i].m_button) && !(m_controllerLastPressedButtons & m_buttons[i].m_button))
			{
				m_buttons[i].m_justPressed = true;
			}
			else
			{
				m_buttons[i].m_justPressed = false;
			}
		}
		else
		{
			m_buttons[i].m_isDown = false;
		}
		

	}

	UpdateButtonJustPressedState();
}

void XboxController::HandleStickDeadZone(Vector2& thumbStick, bool isRightStick)
{
	float x = thumbStick.x / 32768.f;
	float y = thumbStick.y / 32768.f;

	float r = (float)sqrt(x*x + y*y);
	float theta = (float) atan2(y, x);

	float lowRadiusNorm = 0.f;
	if (!isRightStick)
		lowRadiusNorm = (float)XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE / 32768.f;
	else
		lowRadiusNorm = (float)XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE / 32768.f;

	float rDeadZone = RangeMap(r, lowRadiusNorm, 0.98f, 0.f, 1.f);
	rDeadZone = ClampFloat(rDeadZone, 0.f, 1.f);

	thumbStick.SetXY(rDeadZone, theta);
}

void XboxController::HandleStickDeadZoneF(Vector2& thumbStick, bool isRightStick)
{
	float x = thumbStick.x / 32768.f;
	float y = thumbStick.y / 32768.f;

// 	float r = sqrt(x*x + y*y);
// 	float theta = atan2(y, x);

	float lowRadiusNorm = 0.f;
	if (!isRightStick)
		lowRadiusNorm = (float)XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE / 32768.f;
	else
		lowRadiusNorm = (float)XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE / 32768.f;

	/*
	if (x > m_bottomThresholdMultiplier)
		m_normalizedX = OtherRangeMap( x, m_bottomThresholdMultiplier, m_upperThresholdMultiplier, 0.f, 1.f );
	elseif (x < m_bottomThresholdMultiplier * -1.f)
		m_normalizedX = -1.f * OtherRangeMap( x * -1.f, m_bottomThresholdMultiplier, m_upperThresholdMultiplier, 0.f, 1.f );
	else
		m_normalizedX = 0.f;
	*/
	float deadZoneX;
	float deadZoneY;

	if (x > lowRadiusNorm)
	{
		deadZoneX = RangeMap(x, lowRadiusNorm, 0.98f, 0.f, 1.f);
	}
	else if (x < -lowRadiusNorm)
	{
		deadZoneX = -1.f *  RangeMap(-x, lowRadiusNorm, 0.98f, 0.f, 1.f);
	}
	else
	{
		deadZoneX = 0.f;
	}

	if (y > lowRadiusNorm)
	{
		deadZoneY = RangeMap(y, lowRadiusNorm, 0.98f, 0.f, 1.f);
	}
	else if (y < -lowRadiusNorm)
	{
		deadZoneY = -1.f *  RangeMap(-y, lowRadiusNorm, 0.98f, 0.f, 1.f);
	}
	else
	{
		deadZoneY = 0.f;
	}

// 	float rDeadZone = RangeMap(r, lowRadiusNorm, 0.98f, 0.f, 1.f);
	deadZoneX = ClampFloat(deadZoneX, -1.f, 1.f);
	deadZoneY = ClampFloat(deadZoneY, -1.f, 1.f);
// 
// 	float outX = rDeadZone * cos(theta);
// 	float outY = rDeadZone * sin(theta);


	thumbStick.SetXY(deadZoneX, deadZoneY);
}

Vector2 XboxController::GetLeftStick()
{
	float leftX = (float) m_xboxControllerState.Gamepad.sThumbLX;
	float leftY = (float) m_xboxControllerState.Gamepad.sThumbLY;
	Vector2 leftStick;
	leftStick.SetXY(leftX, leftY);
	HandleStickDeadZone(leftStick, false); //Convert to polar (r, theta)
	return leftStick;
}

Vector2 XboxController::GetRightStick()
{
	float rightX = (float)m_xboxControllerState.Gamepad.sThumbRX;
	float rightY = (float)m_xboxControllerState.Gamepad.sThumbRY;
	Vector2 rightStick;
	rightStick.SetXY(rightX, rightY);
	HandleStickDeadZone(rightStick, true); //Convert to polar (r, theta)
	return rightStick;
}

Vector2 XboxController::GetLeftStickF()
{
	float leftX = (float)m_xboxControllerState.Gamepad.sThumbLX;
	float leftY = (float)m_xboxControllerState.Gamepad.sThumbLY;
	Vector2 leftStick;
	leftStick.SetXY(leftX, leftY);
	HandleStickDeadZoneF(leftStick, false);
	return leftStick;
}

Vector2 XboxController::GetRightStickF()
{
	float rightX = (float)m_xboxControllerState.Gamepad.sThumbRX;
	float rightY = (float)m_xboxControllerState.Gamepad.sThumbRY;
	Vector2 rightStick;
	rightStick.SetXY(rightX, rightY);
	HandleStickDeadZoneF(rightStick, true);
	return rightStick;
}

float XboxController::GetLeftTrigger()
{
	float leftTrigger = (float)m_xboxControllerState.Gamepad.bLeftTrigger;
	leftTrigger = RangeMap(leftTrigger, (float)XINPUT_GAMEPAD_TRIGGER_THRESHOLD, 255.f, 0.f, 1.f);
	return leftTrigger;
}

float XboxController::GetRightTrigger()
{
	float rightTrigger = (float)m_xboxControllerState.Gamepad.bRightTrigger;
	rightTrigger = RangeMap(rightTrigger, (float)XINPUT_GAMEPAD_TRIGGER_THRESHOLD, 255.f, 0.f, 1.f);
	return rightTrigger;
}

bool XboxController::IsButtonDown(XBOX_BUTTONS button)
{
	ButtonState bs;
	for (int i = 0; i < NUM_BUTTONS; i++)
	{
		bs = m_buttons[i];
		if (bs.m_button == button)
		{
			if (bs.m_isDown)
				return true;
			return false;
		}
	}
	return false;
}

bool XboxController::IsButtonJustPressed(XBOX_BUTTONS button)
{
	ButtonState bs;
	for (int i = 0; i < NUM_BUTTONS; i++)
	{
		bs = m_buttons[i];
		if (bs.m_button == button)
		{
			if (bs.m_justPressed)
				return true;
			return false;
		}
	}
	return false;
}

bool XboxController::IsButtonPressedOnce(XBOX_BUTTONS button)
{
	return (IsButtonJustPressed(button) && IsButtonDown(button));

}

void XboxController::ClearButtonJustPressedState()
{
	m_controllerLastPressedButtons = 0;
}

void XboxController::UpdateButtonJustPressedState()
{
	m_controllerLastPressedButtons = m_xboxControllerState.Gamepad.wButtons;
}

void XboxController::SetControllerNumber(int controllerNumber)
{
	m_controllerNumber = controllerNumber;
}

void XboxController::InitializeButtonArray()
{
	m_buttons[0].m_button = XBOX_A,
	m_buttons[1].m_button = XBOX_B;
	m_buttons[2].m_button = XBOX_X;
	m_buttons[3].m_button = XBOX_Y;
	m_buttons[4].m_button = XBOX_LB;
	m_buttons[5].m_button = XBOX_RB;
	m_buttons[6].m_button = XBOX_LS;
	m_buttons[7].m_button = XBOX_RS;
	m_buttons[8].m_button = XBOX_BACK;
	m_buttons[9].m_button = XBOX_START;
	m_buttons[10].m_button = XBOX_DPAD_UP;
	m_buttons[11].m_button = XBOX_DPAD_UPLEFT;
	m_buttons[12].m_button = XBOX_DPAD_LEFT;
	m_buttons[13].m_button = XBOX_DPAD_DOWNLEFT;
	m_buttons[14].m_button = XBOX_DPAD_DOWN;
	m_buttons[15].m_button = XBOX_DPAD_DOWNRIGHT;
	m_buttons[16].m_button = XBOX_DPAD_RIGHT;
	m_buttons[17].m_button = XBOX_DPAD_UPRIGHT;
}




