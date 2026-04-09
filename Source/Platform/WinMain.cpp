#include "WinApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	std::unique_ptr<WinApp> app = std::make_unique<WinApp>();

	if (app->Initialize())
	{
		app->Run();
	}

	app->Shutdown();

	return 0;
}