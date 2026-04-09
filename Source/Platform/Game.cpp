#include "Game.h"

bool Game::Initialize(HWND hwnd, int screenWidth, int screenHeight)
{
	m_renderer = std::make_unique<Renderer>();
	if (!m_renderer->Initialize(hwnd, screenWidth, screenHeight))
	{
		return false;
	}

	m_world = std::make_unique<World>();
	m_world->LoadWorld(m_renderer->GetDevice());

	m_camera = std::make_unique<Camera>();
	m_camera->SetPosition(0.0f, 100.0f, -200.0f);
	m_camera->SetRotation(0.0f, 0.0f, 0.0f);

	return true;
}

bool Game::Frame()
{
	if (!m_renderer || !m_world || !m_camera)
	{
		return false;
	}

	m_camera->Render();

	return m_renderer->Render(
		m_world->GetObjects(),
		m_world->GetObjectCount(),
		*m_camera);
}

