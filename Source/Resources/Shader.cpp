#include "Shader.h"

bool Shader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	int error;

	error = wcscpy_s(vsFilename, 128, L"Resource/Shader/vertex.hlsl");
	if (error != 0)
	{
		return false;
	}

	error = wcscpy_s(psFilename, 128, L"Resource/Shader/pixel.hlsl");
	if (error != 0)
	{
		return false;
	}

	result = InitializeShader(device, hwnd, vsFilename, psFilename);

	return result;
}

bool Shader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return result;
}

bool Shader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	ComPtr<ID3D10Blob> errorMessage;
	ComPtr<ID3D10Blob> vertexShaderBuffer;
	ComPtr<ID3D10Blob> pixelShaderBuffer;

	/*
	* 파일 상태의 HLSL 코드를 컴파일한 뒤 기계어 코드를 버퍼에 삽입
	*/
	if (FAILED(D3DCompileFromFile(
		vsFilename,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", 
		"vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 
		0,
		vertexShaderBuffer.GetAddressOf(),
		errorMessage.GetAddressOf())))
	{
		if (errorMessage.Get())
		{
			OutputShaderErrorMessage(errorMessage.Get(), hwnd, vsFilename);
		}
		else
		{
			MessageBox(hwnd, vsFilename, L"셰이더 파일 누락", MB_OK);
		}

		return false;
	}

	if (FAILED(D3DCompileFromFile(
		psFilename,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		pixelShaderBuffer.GetAddressOf(),
		errorMessage.GetAddressOf())))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage.Get(), hwnd, psFilename);
		}
		else
		{
			MessageBox(hwnd, psFilename, L"셰이더 파일 누락", MB_OK);
		}

		return false;
	}

	if (FAILED(device->CreateVertexShader(
		vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(),
		nullptr,
		m_vertexShader.GetAddressOf())))
	{
		return false;
	}

	if (FAILED(device->CreatePixelShader(
		pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(),
		nullptr, 
		m_pixelShader.GetAddressOf())))
	{
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;

	/*
	* 인풋 레이아웃: Vertex Shader 단계에 입력되는 데이터, 매 정점 데이터 스트림마다 데이터 바인딩
	* 시멘틱: 정점 스트림 데이터를 셰이더 입력 변수와 매핑 시키기 위한 바인딩 태그
	*/

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	/*
	* D3D11_INPUT_ELEMENT_DESC 배열 원소 갯수
	*/
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	if (FAILED(device->CreateInputLayout(
		polygonLayout, numElements,
		vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(),
		m_layout.GetAddressOf())))
	{
		return false;
	}

	D3D11_BUFFER_DESC matrixBufferDesc = {};

	/*
	* 상수 버퍼: 매 정점 데이터 스트림마다 바인딩 되는 인풋 레이아웃과 달리 드로우 콜이 수행되는 모든 정점에 대한 공통 적용 데이터
	*/

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(
		&matrixBufferDesc, 
		nullptr, 
		m_matrixBuffer.GetAddressOf())))
	{
		return false;
	}

	D3D11_SAMPLER_DESC samplerDesc = {};

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	if (FAILED(device->CreateSamplerState(
		&samplerDesc, 
		m_sampleState.GetAddressOf())))
	{
		return false;
	}

	return true;
}

void Shader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize;
	std::ofstream fout;

	compileErrors = (char*)(errorMessage->GetBufferPointer());
	bufferSize = errorMessage->GetBufferSize();

	fout.open("Shader-error.txt");
	for (unsigned int i = 0; i < bufferSize; ++i)
	{
		fout << compileErrors[i];
	}

	fout.close();

	errorMessage->Release();
	errorMessage = nullptr;

	MessageBox(hwnd, L"셰이더 컴파일 오류(Shader-error.txt)", shaderFilename, MB_OK);
}

bool Shader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	if (!deviceContext || !m_matrixBuffer || !texture)
	{
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	MatrixBuffer* dataPtr;
	unsigned int bufferNumber;

	/*
	* 셰이더로 보내기 전 행렬 전치
	* Direct3D::XMMatrix는 행 우선 행렬이지만 HLSL의 matrix(GPU의 행렬 연산)는 열 우선으로 연산되기 때문에 전치 필요
	*/
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	/*
	* 상수 버퍼는 GPU 메모리 리소스로 CPU에서 직접 접근이 불가능
	* GPU 메모리에서 상수 버퍼를 읽어 D3D11_MAPPED_SUBRESOURCE 구조체로 감싸고 임시 포인터 필드를 통해 CPU에서 상수 버퍼 수정 가능
	* D3D11_MAP_WRITE_DISCARD 플래그는 GPU가 이미 사용중인 상수 버퍼가 아닌 새 버퍼를 할당한 다음 CPU가 새 버퍼를 참조하게 하고
	* 새 버퍼에 쓰기가 완료된 후 Unmap()을 호출하면 GPU는 기존 상수 버퍼가 아닌 새 상수 버퍼를 읽어 기존 버퍼를 버림(GPU/CPU간 데이터 경합 방지)
	*/
	if (FAILED(deviceContext->Map(
		m_matrixBuffer.Get(), 
		0, 
		D3D11_MAP_WRITE_DISCARD, 
		0, 
		&mappedResource)))
	{
		return false;
	}

	dataPtr = static_cast<MatrixBuffer*>(mappedResource.pData);
	XMStoreFloat4x4(&dataPtr->world, worldMatrix);
	XMStoreFloat4x4(&dataPtr->view, viewMatrix);
	XMStoreFloat4x4(&dataPtr->projection, projectionMatrix);

	deviceContext->Unmap(m_matrixBuffer.Get(), 0);

	bufferNumber = 0;
	ID3D11Buffer* matrixBuffer = m_matrixBuffer.Get();
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void Shader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	ID3D11SamplerState* samplerState = m_sampleState.Get();

	deviceContext->IASetInputLayout(m_layout.Get());
	deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, &samplerState);
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
