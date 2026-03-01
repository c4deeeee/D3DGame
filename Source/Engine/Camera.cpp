#include "Camera.h"

Camera::Camera() :
	m_positionX(0.f), m_positionY(0.f), m_positionZ(0.f),
	m_rotationX(0.f), m_rotationY(0.f), m_rotationZ(0.f)
{

}


Camera::Camera(const Camera& rhs)
{

}


Camera::~Camera()
{

}


void Camera::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
}


void Camera::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}


XMFLOAT3 Camera::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}


XMFLOAT3 Camera::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}


void Camera::Render()
{

#pragma region Set Camera Status

	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;

	up.x = 0.f;
	up.y = 1.f;
	up.z = 0.f;
	upVector = XMLoadFloat3(&up);

	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;
	positionVector = XMLoadFloat3(&position);

	lookAt.x = 0.f;
	lookAt.y = 0.f;
	lookAt.z = 1.f;
	lookAtVector = XMLoadFloat3(&lookAt);

#pragma endregion


#pragma region Set Camera Rotation

	float pitch, yaw, roll;
	XMMATRIX rotationMatrix;

	/*
	* Degree -> Radian 변환
	* 0.0174532925는 1도에 해당하는 라디안 값
	*/
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

#pragma endregion


#pragma region Set View Matrix

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

#pragma endregion

}


void Camera::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}