#pragma once

#include <memory>
#include <DirectXMath.h>
#include "Resources/Model.h"

using namespace DirectX;

class Object
{
public:
	Object() = default;
	~Object() = default;
	Object(const Object& rhs) = delete;
	Object& operator=(const Object& rhs) = delete;
	Object(Object&& rhs) noexcept = default;
	Object& operator=(Object&& rhs) noexcept = default;

	const XMFLOAT3 GetLocation() const;
	Model* GetModel() const;
	const XMFLOAT3 GetRotation() const;
	const XMFLOAT3 GetScale() const;
	const XMMATRIX GetWorldMatrix() const;

	void SetLocation(XMFLOAT3 location);
	void SetModel(std::unique_ptr<Model> model);
	void SetRotation(XMFLOAT3 rotation);
	void SetScale(XMFLOAT3 scale);

private:
	std::unique_ptr<Model> m_model;
	XMFLOAT3 m_location;
	XMFLOAT3 m_rotation;
	XMFLOAT3 m_scale;
	
};
