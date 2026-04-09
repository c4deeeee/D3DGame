#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:
	Camera() = default;
	~Camera() = default;
	Camera(const Camera& rhs) = delete;
	Camera& operator=(const Camera& rhs) = delete;

	void Render();
	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	XMFLOAT3 GetPosition() const;
	XMFLOAT3 GetRotation() const;
	void GetViewMatrix(XMMATRIX&) const;

private:
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMMATRIX m_viewMatrix;

};