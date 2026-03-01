#include "Model.h"

Model::Model() :
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr)
{

}


Model::Model(const Model& rhs)
{

}


Model::~Model()
{

}


bool Model::Initialize(ID3D11Device* device)
{
	bool result;
	result = InitializeBuffers(device);
	return result;
}


void Model::Shutdown()
{
	ShutdownBuffers();
}


void Model::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}


int Model::GetIndexCount()
{
	return m_indexCount;
}


bool Model::InitializeBuffers(ID3D11Device* device)
{
	HRESULT result;

#pragma region Load Data

	Vertex* vertices;
	unsigned long* indices;

	/*
	* 정점, 인덱스 배열의 경우 3D모델 파일(.FBX 등)을 읽어와 동적으로 할당해야 함
	* 지금은 임시로 삼각형을 출력시키기 위해 하드코딩
	*/

	m_vertexCount = 4;
	m_indexCount = 6;

	vertices = new Vertex[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	vertices[0].position = XMFLOAT3(-1.0f, 1.0f, 0.0f); // 왼쪽 위
	vertices[0].color = XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(1.0f, 1.0f, 0.0f); // 오른쪽 위
	vertices[1].color = XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); // 왼쪽 아래
	vertices[2].color = XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f);

	vertices[3].position = XMFLOAT3(1.0f, -1.0f, 0.0f); // 오른쪽 아래
	vertices[3].color = XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	indices[3] = 2;
	indices[4] = 1;
	indices[5] = 3;

#pragma endregion


#pragma region Set Vertex Buffer
	
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

#pragma endregion


#pragma region Set Index Buffer

	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexData;

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0; 
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

#pragma endregion


#pragma region Delete Array

	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

#pragma endregion

	return true;
}


void Model::ShutdownBuffers()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	return;
}


void Model::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(Vertex);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}