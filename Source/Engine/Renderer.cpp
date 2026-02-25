#include "Renderer.h"

Renderer::Renderer() :
	m_direct3d(nullptr)
{

}


Renderer::Renderer(const Renderer& other)
{

}


Renderer::~Renderer()
{

}


bool Renderer::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	m_direct3d = new Direct3D;
	result = m_direct3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Direct3D 초기화에 실패 했습니다.", L"Error", MB_OK);
	}

	return result;
}


void Renderer::Shutdown()
{
	if (m_direct3d)
	{
		m_direct3d->Shutdown();
		delete m_direct3d;
		m_direct3d = nullptr;
	}
}


bool Renderer::Frame()
{
	bool result;
	result = Render();

	return result;
}


bool Renderer::Render()
{
	m_direct3d->BeginScene(0.5f, 0.5f, 0.5f, 0.5f);

	m_direct3d->EndScene();

	return true;
}