#include "WinApp.h"

WinApp::WinApp() :
	m_renderer(nullptr),
	m_inputManager(nullptr)
{

}


WinApp::WinApp(const WinApp& rhs)
{

}


WinApp::~WinApp()
{

}


bool WinApp::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	screenWidth = 0;
	screenHeight = 0;
	InitializeWindows(screenWidth, screenHeight);

	m_inputManager = new InputManager;
	m_inputManager->Initialize();

	m_renderer = new Renderer();
	result = m_renderer->Initialize(screenWidth, screenHeight, m_hwnd);

	return result;
}


void WinApp::Shutdown()
{
	if (m_renderer)
	{
		m_renderer->Shutdown();
		delete m_renderer;
		m_renderer = nullptr;
	}

	if (m_inputManager)
	{
		delete m_inputManager;
		m_inputManager = nullptr;
	}

	ShutdownWindows();
}


void WinApp::Run()
{
	MSG msg;
	bool isExit, result;

	ZeroMemory(&msg, sizeof(MSG));

	isExit = false;
	while (!isExit)
	{
		/*
		* WinAPI 메시지 루프에서 GetMessage와 PeekMessage의 차이점
		* GetMessage : 메시지를 받을때 까지 블로킹 하며, 메시지가 없을 경우 스레드를 슬립 시키고 대기
		* PeekMessage : 메시지가 없어도 즉시 반환하여 논블로킹 실시간 렌더링 가능
		*/
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			isExit = true;
		}
		else
		{
			result = Frame();
			if (!result)
			{
				isExit = true;
			}
		}
	}
}


LRESULT CALLBACK WinApp::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	case WM_KEYDOWN:
	{
		m_inputManager->KeyDown((unsigned int)wparam);
		return 0;
	}

	case WM_KEYUP:
	{
		m_inputManager->KeyUp((unsigned int)wparam);
		return 0;
	}

	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}


bool WinApp::Frame()
{
	bool result;

	if (m_inputManager->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	result = m_renderer->Frame();

	return result;

}


void WinApp::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	ApplicationHandle = this;
	m_hinstance = GetModuleHandle(nullptr);
	m_applicationName = L"Engine";

#pragma region Init Window Class

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&wc);

#pragma endregion

#pragma region Set Resolution

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (FULL_SCREEN)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		posX = posY = 0;
	}
	else
	{
		screenWidth = 800;
		screenHeight = 600;

		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

#pragma endregion

#pragma region Create Window

	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, nullptr, nullptr, m_hinstance, nullptr);

	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);
	ShowCursor(false);

#pragma endregion
}


void WinApp::ShutdownWindows()
{
	ShowCursor(true);

	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(nullptr, 0);
	}

	DestroyWindow(m_hwnd);
	m_hwnd = nullptr;

	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = nullptr;
	ApplicationHandle = nullptr;

	return;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}