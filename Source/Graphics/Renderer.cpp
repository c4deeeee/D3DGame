#include "Renderer.h"

#include <DirectXMath.h>
#include <vector>

#include "../Scene/Camera.h"
#include "../Scene/Object.h"
#include "../Resources/Model.h"
#include "../Resources/ModelPart.h"
#include "../Resources/Mesh.h"
#include "../Resources/Texture.h"

namespace
{
	constexpr float SCREEN_DEPTH = 1000.0f;
	constexpr float SCREEN_NEAR = 0.1f;
}

bool Renderer::Initialize(HWND hwnd, int screenWidth, int screenHeight)
{
	m_graphicsDevice = std::make_unique<GraphicsDevice>();
	if (!m_graphicsDevice->Initialize())
	{
		return false;
	}

	m_renderSurface = std::make_unique<RenderSurface>();
	if (!m_renderSurface->Initialize(
		m_graphicsDevice->GetDevice(),
		m_graphicsDevice->GetDeviceContext(),
		hwnd,
		screenWidth,
		screenHeight,
		VSYNC_ENABLED,
		FULL_SCREEN))
	{
		return false;
	}

	m_pipelineStates = std::make_unique<PipelineStates>();
	if (!m_pipelineStates->Initialize(m_graphicsDevice->GetDevice()))
	{
		return false;
	}

	m_shader = std::make_unique<Shader>();
	if (!m_shader->Initialize(m_graphicsDevice->GetDevice(), hwnd))
	{
		return false;
	}

	const float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
	m_projectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspectRatio, SCREEN_NEAR, SCREEN_DEPTH);

	return true;
}

bool Renderer::Render(Object* objects, size_t objectCount, Camera& camera)
{
	if (!m_graphicsDevice || !m_renderSurface || !m_pipelineStates || !m_shader)
	{
		return false;
	}

	m_renderSurface->BindRenderTarget(m_graphicsDevice->GetDeviceContext());
	m_renderSurface->BindViewport(m_graphicsDevice->GetDeviceContext());
	m_pipelineStates->BindDepthStencilState(m_graphicsDevice->GetDeviceContext());
	m_pipelineStates->BindRasterState(m_graphicsDevice->GetDeviceContext());

	m_renderSurface->BeginScene(m_graphicsDevice->GetDeviceContext(), 0.1f, 0.1f, 0.15f, 1.0f);

	XMMATRIX viewMatrix = XMMatrixIdentity();
	camera.GetViewMatrix(viewMatrix);

	ID3D11DeviceContext* context = m_graphicsDevice->GetDeviceContext();
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (size_t objectIndex = 0; objectIndex < objectCount; ++objectIndex)
	{
		Object& object = objects[objectIndex];
		Model* model = object.GetModel();
		if (!model)
		{
			continue;
		}

		const XMMATRIX worldMatrix = object.GetWorldMatrix();
		const std::vector<ModelPart>& parts = model->GetParts();

		for (const ModelPart& part : parts)
		{
			Mesh* mesh = part.GetMesh();
			Texture* texture = part.GetTexture();
			if (!mesh || !texture)
			{
				continue;
			}

			ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
			ID3D11Buffer* indexBuffer = mesh->GetIndexBuffer();
			if (!vertexBuffer || !indexBuffer)
			{
				continue;
			}

			const UINT stride = sizeof(Mesh::Vertex);
			const UINT offset = 0;
			context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

			if (!m_shader->Render(
				context,
				static_cast<int>(mesh->GetIndexCount()),
				worldMatrix,
				viewMatrix,
				m_projectionMatrix,
				texture->GetTexture()))
			{
				return false;
			}
		}
	}

	m_renderSurface->EndScene();
	return true;
}

ID3D11Device* Renderer::GetDevice() const
{
	return m_graphicsDevice->GetDevice();
}
