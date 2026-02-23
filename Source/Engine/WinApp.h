#pragma once

/*
* 사용하지 않는 API를 제외하여 빌드 시간을 단축시키는 매크로
*/
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "InputManager.h"
#include "Renderer.h"

class WinApp
{
public:
	WinApp();
	WinApp(const WinApp&);
	~WinApp();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputManager* m_inputManager;
	Renderer* m_renderer;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static WinApp* ApplicationHandle = nullptr;