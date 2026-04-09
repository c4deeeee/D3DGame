#pragma once

/*
* 사용하지 않는 API를 제외하여 빌드 시간을 단축시키는 매크로
*/
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <memory>

#include "InputManager.h"
#include "Game.h"

class WinApp
{
public:
	WinApp() = default;
	~WinApp() = default;
	WinApp(const WinApp& rhs) = delete;
	WinApp& operator=(const WinApp& rhs) = delete;

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

private:
	bool Frame();
	void InitializeWindows(int& screenWidth, int& screenHeight);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	std::unique_ptr<InputManager> m_inputManager;
	std::unique_ptr<Game> m_game;
};

static WinApp* ApplicationHandle = nullptr;
static LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);