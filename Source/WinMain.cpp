#include "Engine/WinApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WinApp* app = new WinApp;

	if (app->Initialize())
	{
		app->Run();
	}

	app->Shutdown();
	delete app;
	app = nullptr;

	return 0;
}