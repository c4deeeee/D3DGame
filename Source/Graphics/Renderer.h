#pragma once

#include <Windows.h>
#include <memory>

#include "GraphicsDevice.h"
#include "RenderSurface.h"
#include "PipelineStates.h"
#include "../Resources/Shader.h"

class Camera;
class Object;

constexpr bool FULL_SCREEN = false;
constexpr bool VSYNC_ENABLED = true;

class Renderer
{
public:
	Renderer() = default;
	~Renderer() = default;
	Renderer(const Renderer& rhs) = delete;
	Renderer& operator=(const Renderer& rhs) = delete;

	bool Initialize(HWND hwnd, int screenWidth, int screenHeight);
	bool Render(Object* objects, size_t objectCount, Camera& camera);

	ID3D11Device* GetDevice() const;

private:
	std::unique_ptr<GraphicsDevice> m_graphicsDevice;
	std::unique_ptr<RenderSurface> m_renderSurface;
	std::unique_ptr<PipelineStates> m_pipelineStates;
	std::unique_ptr<Shader> m_shader;
	XMMATRIX m_projectionMatrix = XMMatrixIdentity();

};
