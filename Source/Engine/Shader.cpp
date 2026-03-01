#include "Shader.h"

Shader::Shader() :
	m_vertexShader(nullptr),
	m_pixelShader(nullptr),
	m_layout(nullptr),
	m_matrixBuffer(nullptr)
{

}


Shader::Shader(const Shader& rhs)
{

}


Shader::~Shader()
{

}


bool Shader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	int error;

	error = wcscpy_s(vsFilename, 128, L"Shader/color.vs.hlsl");
	if (error != 0)
	{
		return false;
	}

	error = wcscpy_s(psFilename, 128, L"Shader/color.ps.hlsl");
	if (error != 0)
	{
		return false;
	}

	result = InitializeShader(device, hwnd, vsFilename, psFilename);

	return result;
}


void Shader::Shutdown()
{
	ShutdownShader();
}


bool Shader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return result;
}


bool Shader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;

#pragma region Compile Shaders

	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;

	/*
	* 파일 상태의 HLSL 코드를 컴파일한 뒤 기계어 코드를 버퍼에 삽입
	*/
	result = D3DCompileFromFile(
		vsFilename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"ColorVertexShader", "vs_5_0", 
		D3D10_SHADER_ENABLE_STRICTNESS, 0, 
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		wchar_t msg[256];
		swprintf_s(msg, L"D3DCompileFromFile failed. hr=0x%08X", (unsigned)result);
		MessageBoxW(hwnd, msg, L"Compile failed", MB_OK);

		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else
		{
			MessageBox(hwnd, vsFilename, L"셰이더 파일 누락", MB_OK);
		}

		return false;
	}

	result = D3DCompileFromFile(
		psFilename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"ColorPixelShader", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		wchar_t msg[256];
		swprintf_s(msg, L"D3DCompileFromFile failed. hr=0x%08X", (unsigned)result);
		MessageBoxW(hwnd, msg, L"Compile failed", MB_OK);

		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		else
		{
			MessageBox(hwnd, psFilename, L"셰이더 파일 누락", MB_OK);
		}

		return false;
	}

	result = device->CreateVertexShader(
		vertexShaderBuffer->GetBufferPointer(), 
		vertexShaderBuffer->GetBufferSize(), 
		nullptr, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreatePixelShader(
		pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(),
		nullptr, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

#pragma endregion

#pragma region Create Input Layout

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

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	/*
	* D3D11_INPUT_ELEMENT_DESC 배열 원소 갯수
	*/
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	
	result = device->CreateInputLayout(
		polygonLayout, numElements, 
		vertexShaderBuffer->GetBufferPointer(), 
		vertexShaderBuffer->GetBufferSize(), 
		&m_layout);
	if (FAILED(result))
	{
		return false;
	}

	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

#pragma endregion

#pragma region Set Constant Buffer

	D3D11_BUFFER_DESC matrixBufferDesc;

	/*
	* 상수 버퍼: 매 정점 데이터 스트림마다 바인딩 되는 인풋 레이아웃과 달리 드로우 콜이 수행되는 모든 정점에 대한 공통 적용 데이터
	*/

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

#pragma endregion

	return true;
}


void Shader::ShutdownShader()
{
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = nullptr;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}
}


void Shader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize;
	std::ofstream fout;

	compileErrors = (char*)(errorMessage->GetBufferPointer());
	bufferSize = errorMessage->GetBufferSize();

	fout.open("shader-error.txt");
	for (unsigned int i = 0; i < bufferSize; ++i)
	{
		fout << compileErrors[i];
	}

	fout.close();

	errorMessage->Release();
	errorMessage = nullptr;

	MessageBox(hwnd, L"셰이더 컴파일 오류(shader-error.txt)", shaderFilename, MB_OK);
}


bool Shader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
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
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr = (MatrixBuffer*)mappedResource.pData;

	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	deviceContext->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}


void Shader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
