#pragma once

#include <windows.h> 

#include "Direct3D.h"
#include "Camera.h"
#include "Model.h"
#include "Shader.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;

class Renderer
{
public:
	Renderer();
	Renderer(const Renderer&);
	~Renderer();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
	Direct3D* m_direct3d;
	Camera* m_camera;
	Model* m_model;
	Shader* m_shader;

};