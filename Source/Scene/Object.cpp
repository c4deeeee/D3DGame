#include "Object.h"
#include "../Resources/Model.h"

#include <utility>

const XMFLOAT3 Object::GetLocation() const
{
	return m_location;
}

Model* Object::GetModel() const
{
	return m_model.get();
}

const XMFLOAT3 Object::GetRotation() const
{
	return m_rotation;
}

const XMFLOAT3 Object::GetScale() const
{
	return m_scale;
}

const XMMATRIX Object::GetWorldMatrix() const
{
	XMMATRIX scaleMatrix = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_rotation.x),
		XMConvertToRadians(m_rotation.y),
		XMConvertToRadians(m_rotation.z));
	XMMATRIX translationMatrix = XMMatrixTranslation(m_location.x, m_location.y, m_location.z);

	return scaleMatrix * rotationMatrix * translationMatrix;
}

void Object::SetLocation(XMFLOAT3 location)
{
	m_location = location;
}

void Object::SetModel(std::unique_ptr<Model> model)
{
	m_model = std::move(model);
}

void Object::SetRotation(XMFLOAT3 rotation)
{
	m_rotation = rotation;
}

void Object::SetScale(XMFLOAT3 scale)
{
	m_scale = scale;
}
