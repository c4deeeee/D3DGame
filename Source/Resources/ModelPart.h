#pragma once

#include <memory>
#include "Mesh.h"
#include "Texture.h"

class ModelPart
{
public:
	ModelPart() = default;
	~ModelPart() = default;
	ModelPart(const ModelPart& rhs) = delete;
	ModelPart& operator=(const ModelPart& rhs) = delete;
	ModelPart(ModelPart&& rhs) noexcept = default;
	ModelPart& operator=(ModelPart&& rhs) noexcept = default;

	Mesh* GetMesh() const;
	Texture* GetTexture() const;

	void SetMesh(Mesh* mesh);
	void SetTexture(Texture* texture);

private:
	std::unique_ptr<Mesh> m_mesh;
	std::unique_ptr<Texture> m_texture;

};
