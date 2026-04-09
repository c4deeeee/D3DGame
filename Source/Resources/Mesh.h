#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <string>
#include <vector>

using namespace DirectX;
using namespace Microsoft::WRL;

class Mesh
{
public:
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

	Mesh() = default;
	~Mesh() = default;
	Mesh(const Mesh& rhs) = delete;
	Mesh& operator=(const Mesh& rhs) = delete;

	bool Initialize(ID3D11Device* device);
	bool Initialize(
		ID3D11Device* device,
		const std::vector<Vertex>& vertices,
		const std::vector<unsigned long>& indices,
		const std::string& meshName);
	
	ID3D11Buffer* GetVertexBuffer() const;
	ID3D11Buffer* GetIndexBuffer() const;

	unsigned int GetVertexCount() const;
	unsigned int GetIndexCount() const;

	const std::string GetMeshName() const;

private:
	std::string m_meshName;

	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11Buffer> m_indexBuffer;

	unsigned int m_vertexCount;
	unsigned int m_indexCount;

};
