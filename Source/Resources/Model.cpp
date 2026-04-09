#include "Model.h"
#include <utility>

const std::vector<ModelPart>& Model::GetParts() const
{
	return m_parts;
}

void Model::AddParts(ModelPart&& part)
{
	m_parts.push_back(std::move(part));
}
