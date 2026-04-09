#include "ModelPart.h"

Mesh* ModelPart::GetMesh() const
{
	return m_mesh.get();
}

Texture* ModelPart::GetTexture() const
{
	return m_texture.get();
}

void ModelPart::SetMesh(Mesh* mesh)
{
	m_mesh.reset(mesh);
}

void ModelPart::SetTexture(Texture* texture)
{
	m_texture.reset(texture);
}
