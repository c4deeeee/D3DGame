#include "WinApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (SUCCEEDED(CoInitialize(nullptr)))
	{
		{
			WinApp app;
			if (SUCCEEDED(app.Initialize(hInstance)))
			{
				app.RunMessageLoop();
			}
		}
		CoUninitialize();
	}
	return 0;
}

WinApp::WinApp() :
	hwnd(nullptr)
{

}

WinApp::~WinApp()
{

}

HRESULT WinApp::Initialize(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WinApp::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = hInstance;
	wcex.hbrBackground = nullptr;
	wcex.lpszMenuName = nullptr;
	wcex.hCursor = LoadCursor(nullptr, IDI_APPLICATION);
	wcex.lpszClassName = L"D3DGame";
	RegisterClassEx(&wcex);

	hwnd = CreateWindow(
		L"D3DGame", 
		L"D3DGame",
		WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT,
		1600, 
		900, 
		nullptr, 
		nullptr, 
		hInstance, 
		this
	);

	HRESULT hr = hwnd ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		ShowWindow(hwnd, SW_SHOWNORMAL);
		UpdateWindow(hwnd);
	}

	return hr;
}

void WinApp::RunMessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT CALLBACK WinApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		WinApp* pWinApp = (WinApp*)pcs->lpCreateParams;

		SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWinApp));

		result = 1;
	}
	else
	{
		WinApp* pWinApp = reinterpret_cast<WinApp*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
		bool wasHandled = false;

		if (pWinApp)
		{
			switch (message)
			{
			/*case WM_SIZE:
			{
				UINT width = LOWORD(lParam);
				UINT height = HIWORD(lParam);
				pWinApp->OnResize(width, height);
				result = 0;
				wasHandled = true;
				break;
			}*/
			case WM_DISPLAYCHANGE:
			{
				InvalidateRect(hWnd, NULL, FALSE);
				result = 0;
				wasHandled = true;
				break;
			}
			/*case WM_PAINT:
			{
				pWinApp->OnRender();
				ValidateRect(hWnd, NULL);
				result = 0;
				wasHandled = true;
				break;
			}*/
			case WM_DESTROY:
			{
				PostQuitMessage(0);
				result = 1;
				wasHandled = true;
				break;
			}
			}
		}

		if (!wasHandled)
		{
			result = DefWindowProc(hWnd, message, wParam, lParam);
		}
	}

	return result;
}