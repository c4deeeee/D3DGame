#include "Mesh.h"

#include <vector>

bool Mesh::Initialize(ID3D11Device* device)
{
	/*
	* TODO: Replace this fallback quad with imported mesh data when available.
	*/
	// -------------------------------------------------------------------------------------- //

	std::vector<Vertex> vertices(4);
	std::vector<unsigned long> indices(6);

	vertices[0].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);
	vertices[1].position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertices[1].texture = XMFLOAT2(1.0f, 0.0f);
	vertices[2].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[3].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[3].texture = XMFLOAT2(1.0f, 1.0f);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 2;
	indices[4] = 1;
	indices[5] = 3;

	// -------------------------------------------------------------------------------------- //

	return Initialize(device, vertices, indices, "DefaultQuad");
}

bool Mesh::Initialize(
	ID3D11Device* device,
	const std::vector<Vertex>& vertices,
	const std::vector<unsigned long>& indices,
	const std::string& meshName)
{
	if (!device || vertices.empty() || indices.empty())
	{
		return false;
	}

	m_meshName = meshName;
	m_vertexCount = static_cast<unsigned int>(vertices.size());
	m_indexCount = static_cast<unsigned int>(indices.size());

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	D3D11_SUBRESOURCE_DATA vertexData = {};

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(
		&vertexBufferDesc,
		&vertexData,
		m_vertexBuffer.ReleaseAndGetAddressOf())))
	{
		return false;
	}

	D3D11_BUFFER_DESC indexBufferDesc = {};
	D3D11_SUBRESOURCE_DATA indexData = {};

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(
		&indexBufferDesc,
		&indexData,
		m_indexBuffer.ReleaseAndGetAddressOf())))
	{
		return false;
	}

	return true;
}

ID3D11Buffer* Mesh::GetVertexBuffer() const
{
	return m_vertexBuffer.Get();
}

ID3D11Buffer* Mesh::GetIndexBuffer() const
{
	return m_indexBuffer.Get();
}

unsigned int Mesh::GetVertexCount() const
{
	return m_vertexCount;
}

unsigned int Mesh::GetIndexCount() const
{
	return m_indexCount;
}

const std::string Mesh::GetMeshName() const
{
	return m_meshName;
}
