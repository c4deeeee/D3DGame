#pragma once

#include <Windows.h>
#include <memory>

#include "../Graphics/Renderer.h"
#include "../Scene/Camera.h"
#include "../Scene/World.h"

class Game
{
public:
	Game() = default;
	~Game() = default;
	Game(const Game& rhs) = delete;
	Game& operator=(const Game& rhs) = delete;

	bool Initialize(HWND hwnd, int screenWidth, int screenHeight);
	bool Frame();

private:
	std::unique_ptr<Camera> m_camera;
	std::unique_ptr<Renderer> m_renderer;
	std::unique_ptr<World> m_world;

};
