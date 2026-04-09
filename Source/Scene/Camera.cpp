#include "Camera.h"

void Camera::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;

	up.x = 0.f;
	up.y = 1.f;
	up.z = 0.f;
	upVector = XMLoadFloat3(&up);

	position.x = m_position.x;
	position.y = m_position.y;
	position.z = m_position.z;
	positionVector = XMLoadFloat3(&position);

	lookAt.x = 0.f;
	lookAt.y = 0.f;
	lookAt.z = 1.f;
	lookAtVector = XMLoadFloat3(&lookAt);

	float pitch, yaw, roll;
	XMMATRIX rotationMatrix;

	/*
	* Degree -> Radian 변환
	* 0.0174532925는 1도에 해당하는 라디안 값
	*/
	pitch = m_rotation.x * 0.0174532925f;
	yaw = m_rotation.y * 0.0174532925f;
	roll = m_rotation.z * 0.0174532925f;
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	/*
	* 기본 상태의 lookAt, up 벡터에 회전 행렬을 곱해 카메라 회전값에 따른 올바른 방향을 보도록 설정
	*/
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	/*
	* lookAt 벡터는 원점에서 정면을 바라보고 있으므로, 실제 카메라 위치에서 보도록 카메라 위치값을 더함
	*/
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void Camera::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}

void Camera::SetRotation(float x, float y, float z)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
}

XMFLOAT3 Camera::GetPosition() const
{
	return m_position;
}

XMFLOAT3 Camera::GetRotation() const
{
	return m_rotation;
}

void Camera::GetViewMatrix(XMMATRIX& viewMatrix) const
{
	viewMatrix = m_viewMatrix;
	return;
}