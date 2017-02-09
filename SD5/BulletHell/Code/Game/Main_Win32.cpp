#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include "Engine/Math/Vector2.hpp"
#include <time.h>
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/TheApp.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Core/Console.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Memory/MemoryAnalytics.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
#define UNUSED(x) (void)(x);


//-----------------------------------------------------------------------------------------------
const int OFFSET_FROM_WINDOWS_DESKTOP = 50;
const int WINDOW_PHYSICAL_WIDTH = 1600;
const int WINDOW_PHYSICAL_HEIGHT = 900;
const double VIEW_LEFT = 0.0;
const double VIEW_RIGHT = 16.0;
const double VIEW_BOTTOM = 0.0;
//const double VIEW_TOP = VIEW_RIGHT * static_cast< double >(WINDOW_PHYSICAL_HEIGHT) / static_cast< double >(WINDOW_PHYSICAL_WIDTH);
const double VIEW_TOP = 9.f;

HWND g_hWnd = nullptr;
HDC g_displayDeviceContext = nullptr;
HGLRC g_openGLRenderingContext = nullptr;
const char* APP_NAME = "Ultra Jet Force";


//-----------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
	unsigned char asKey = (unsigned char)wParam;

	if (wmMessageCode == WM_CHAR && g_theConsole->m_isOpen)
	{
		g_theConsole->HandleInput(asKey);
	}
	switch (wmMessageCode)
	{
	case WM_CLOSE:
	case WM_DESTROY:
	case WM_QUIT:
		g_isQuitting = true;
		return 0;

	case WM_KEYDOWN:
		g_theApp->m_inputSystem.SetKeyDownStatus(asKey, true);
		g_theConsole->m_inputSystem.SetKeyDownStatus(asKey, true);
		if (asKey == VK_ESCAPE && !g_theConsole->m_isOpen)
		{
			g_isQuitting = true;
			return 0;
		}
		//		g_keyDownArray[ asKey ] = true;
		break;

	case WM_KEYUP:
		g_theApp->m_inputSystem.SetKeyDownStatus(asKey, false);
		g_theConsole->m_inputSystem.SetKeyDownStatus(asKey, false);
		//		g_keyDownArray[ asKey ] = false;
		break;
	case WM_LBUTTONDOWN:
		g_theApp->m_inputSystem.SetKeyDownStatus(VK_LBUTTON, true);
		g_theConsole->m_inputSystem.SetKeyDownStatus(VK_LBUTTON, true);
		break;
	case WM_LBUTTONUP:
		g_theApp->m_inputSystem.SetKeyDownStatus(VK_LBUTTON, false);
		g_theConsole->m_inputSystem.SetKeyDownStatus(VK_LBUTTON, false);
		break;
	case WM_RBUTTONDOWN:
		g_theApp->m_inputSystem.SetKeyDownStatus(VK_RBUTTON, true);
		g_theConsole->m_inputSystem.SetKeyDownStatus(VK_RBUTTON, true);
		break;
	case WM_RBUTTONUP:
		g_theApp->m_inputSystem.SetKeyDownStatus(VK_RBUTTON, false);
		g_theConsole->m_inputSystem.SetKeyDownStatus(VK_RBUTTON, false);
		break;
	case WM_MOUSEWHEEL:
		short mouseWheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (mouseWheelDelta > 0)
		{
			g_theApp->m_inputSystem.SetMouseWheelDirection(1);
			g_theConsole->m_inputSystem.SetMouseWheelDirection(1);
		}
		else if (mouseWheelDelta < 0)
		{
			g_theApp->m_inputSystem.SetMouseWheelDirection(-1);
			g_theConsole->m_inputSystem.SetMouseWheelDirection(-1);
		}
		else
		{
			g_theApp->m_inputSystem.SetMouseWheelDirection(0);
			g_theConsole->m_inputSystem.SetMouseWheelDirection(0);
		}
	}

	return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}


//-----------------------------------------------------------------------------------------------
void CreateOpenGLWindow(HINSTANCE applicationInstanceHandle)
{
	// Define a window class
	WNDCLASSEX windowClassDescription;
	memset(&windowClassDescription, 0, sizeof(windowClassDescription));
	windowClassDescription.cbSize = sizeof(windowClassDescription);
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(WindowsMessageHandlingProcedure); // Assign a win32 message-handling function
	windowClassDescription.hInstance = GetModuleHandle(NULL);
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT("Simple Window Class");
	RegisterClassEx(&windowClassDescription);

	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle, &desktopRect);

	RECT windowRect = { OFFSET_FROM_WINDOWS_DESKTOP, OFFSET_FROM_WINDOWS_DESKTOP, OFFSET_FROM_WINDOWS_DESKTOP + WINDOW_PHYSICAL_WIDTH, OFFSET_FROM_WINDOWS_DESKTOP + WINDOW_PHYSICAL_HEIGHT };
	AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);

	WCHAR windowTitle[1024];
	MultiByteToWideChar(GetACP(), 0, APP_NAME, -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
	g_hWnd = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		applicationInstanceHandle,
		NULL);

	ShowWindow(g_hWnd, SW_SHOW);
	SetForegroundWindow(g_hWnd);
	SetFocus(g_hWnd);

	g_displayDeviceContext = GetDC(g_hWnd);

	HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
	SetCursor(cursor);

	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	memset(&pixelFormatDescriptor, 0, sizeof(pixelFormatDescriptor));
	pixelFormatDescriptor.nSize = sizeof(pixelFormatDescriptor);
	pixelFormatDescriptor.nVersion = 1;
	pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	pixelFormatDescriptor.cColorBits = 24;
	pixelFormatDescriptor.cDepthBits = 24;
	pixelFormatDescriptor.cAccumBits = 0;
	pixelFormatDescriptor.cStencilBits = 8;

	int pixelFormatCode = ChoosePixelFormat(g_displayDeviceContext, &pixelFormatDescriptor);
	SetPixelFormat(g_displayDeviceContext, pixelFormatCode, &pixelFormatDescriptor);
	g_openGLRenderingContext = wglCreateContext(g_displayDeviceContext);
	wglMakeCurrent(g_displayDeviceContext, g_openGLRenderingContext);

	//g_theRenderer->CreateScreen();

}


//-----------------------------------------------------------------------------------------------
void RunMessagePump()
{
	MSG queuedMessage;
	for (;;)
	{
		const BOOL wasMessagePresent = PeekMessage(&queuedMessage, NULL, 0, 0, PM_REMOVE);
		if (!wasMessagePresent)
		{
			break;
		}

		TranslateMessage(&queuedMessage);
		DispatchMessage(&queuedMessage);
	}
}


//-----------------------------------------------------------------------------------------------
void Update()
{
	g_theProfiler->StartProfilerSample("MainUpdate");
	static double s_timeLastFrameBegan = GetCurrentTimeSeconds(); // "static" local variables are initialized once, when first encountered, thus this function call only happens once
	double timeThisFrameBegan = GetCurrentTimeSeconds();
	float deltaSeconds = static_cast<float>(timeThisFrameBegan - s_timeLastFrameBegan);
	s_timeLastFrameBegan = timeThisFrameBegan;
	//g_systemClock->Update(deltaSeconds);
	TimeUpdate();
	g_theApp->Update();
	g_theGame->Update(deltaSeconds);
	if (g_theApp->m_canCloseGame)
	{
		g_isQuitting = true;
	}
	g_theProfiler->EndProfilerSample();
}

//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
void Render()
{
	g_theProfiler->StartProfilerSample("MainRender");
	//g_theRenderer->ClearScreen(0.f, 0.f, 0.f);
	//g_theRenderer->SetColor(1.f, 1.f, 1.f, 1.f);
	static double s_timeLastFrameBeganRender = GetCurrentTimeSeconds(); // "static" local variables are initialized once, when first encountered, thus this function call only happens once
	double timeThisFrameBegan = GetCurrentTimeSeconds();
	float deltaSeconds = static_cast<float>(timeThisFrameBegan - s_timeLastFrameBeganRender);
	s_timeLastFrameBeganRender = timeThisFrameBegan;


	g_theApp->Render(deltaSeconds);
	//g_theGame->Render(deltaSeconds);
	//g_theRenderer->DrawGrid(200, 200);
	SwapBuffers(g_displayDeviceContext);
	g_theProfiler->EndProfilerSample();
}

//-----------------------------------------------------------------------------------------------
void RunFrame()
{
	g_theProfiler->ProfileFrameMark();
	RunMessagePump();
	Update();
	Render();
}


//-----------------------------------------------------------------------------------------------
void Initialize(HINSTANCE applicationInstanceHandle)
{
	SetProcessDPIAware();
	CreateOpenGLWindow(applicationInstanceHandle);
	srand((int)time(nullptr));
	MemoryInit();
	

	g_theRenderer = new Renderer();
	g_theApp = new TheApp(WINDOW_PHYSICAL_WIDTH, WINDOW_PHYSICAL_HEIGHT, g_hWnd);
	g_theRenderer->CreateScreen();
	g_theGame = new TheGame();
	g_theConsole = new Console(WINDOW_PHYSICAL_HEIGHT, WINDOW_PHYSICAL_WIDTH);

	g_theProfiler->ProfilerSystemStartup();
}


//-----------------------------------------------------------------------------------------------
void Shutdown()
{
	
	MemoryShutdown();
	g_theProfiler->ProfilerSystemShutdown();

	delete g_theConsole;
	g_theConsole = nullptr;

	delete g_theGame;
	g_theGame = nullptr;

	delete g_theApp;
	g_theApp = nullptr;

	delete g_theRenderer;
	g_theRenderer = nullptr;
}


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int)
{
	UNUSED(commandLineString);
	Initialize(applicationInstanceHandle);

	while (!g_isQuitting)
	{
		RunFrame();
	}

	Shutdown();
	return 0;
}


