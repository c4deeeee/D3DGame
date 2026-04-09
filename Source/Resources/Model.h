#pragma once

#include <vector>
#include "ModelPart.h"

class Model
{
public:
	Model() = default;
	~Model() = default;
	Model(const Model& rhs) = delete;
	Model& operator=(const Model& rhs) = delete;
	Model(Model&& rhs) noexcept = default;
	Model& operator=(Model&& rhs) noexcept = default;

	const std::vector<ModelPart>& GetParts() const;
	void AddParts(ModelPart&& part);

private:
	std::vector<ModelPart> m_parts;

};
