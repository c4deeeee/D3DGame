#pragma once

#include <windows.h>

class WinApp
{
public:
	WinApp();
	~WinApp();
	HRESULT Initialize(HINSTANCE hInstance);
	void RunMessageLoop();

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	HWND hwnd;

};