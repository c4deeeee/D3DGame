#include "Direct3D.h"

Direct3D::Direct3D() :
	m_swapChain(nullptr),
	m_device(nullptr),
	m_deviceContext(nullptr),
	m_renderTargetView(nullptr),
	m_depthStencilBuffer(nullptr),
	m_depthStencilState(nullptr),
	m_depthStencilView(nullptr),
	m_rasterState(nullptr)
{

}


Direct3D::Direct3D(const Direct3D&)
{

}


Direct3D::~Direct3D()
{

}


bool Direct3D::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;

	m_vsync_enabled = vsync;

#pragma region Get Refresh Rate

	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	DXGI_MODE_DESC* displayModeList;
	unsigned int numModes, numerator, denominator;

	/*
	* DXGI - DirectX Graphic Interface: GPU와 모니터를 관리하는 Direct3D 하부 레이어 인터페이스
	* DXGI Factory: DXGI 최상위 객체로 GPU 관리
	*/
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	/*
	* DXGI Adapter: DXGI 인터페이스에서 GPU를 어댑터라고 하며 EnumAdapters() 함수로 시스템이 사용중인 GPU 목록중 0번(기본 GPU)을 어댑터 객체로 복사
	*/
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	/*
	* DXGI Output: GPU에 연결된 모니터를 아웃풋이라고 하며 EnumOuputs() 함수의 첫번째 인자는 모니터 목록으로 0번이 주 모니터, 1 이상은 보조 모니터
	*/
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	/*
	* GetDisplayModeList(): 출력 모니터의 해상도와 주사율 목록을 반환하고 세번째 인자는 목록의 크기, 네번째 인자는 실제 목록을 저장할 배열
	*/
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	/*
	* 첫번째 GetDisplayModeList() 함수 호출로 해상도/주사율 목록의 갯수를 이용해 실제 정보를 저장할 배열을 생성하는데 사용
	*/
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	/*
	* 첫번째 GetDisplayModeList() 함수 호출에서 이전에 생성한 배열을 네번째 인자로 넘겨 실제 해상도/주사율 목록을 받음
	*/
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	/*
	* 윈도우와 일치하는 해상도를 찾아 해당 디스플레이 모드의 주사율(분자/분모)을 가져옴
	* 윈도우 해상도는 WinApp -> InitializeWindows() 함수에서 결정
	*/
	for (unsigned int i = 0; i < numModes; ++i)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

#pragma endregion


#pragma region Get VRAM

	unsigned long long stringLength;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;

	/*
	* 어댑터 디스크립션: 명칭, 제조사, VRAM등 정보를 담고있는 구조체
	*/
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	/*
	* 어뎁터 디스크립션 구조체의 DedicatedVideoMemory 필드는 VRAM을 바이트 단위로 저장, 이를 MiB 단위로 축약
	*/
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	/*
	* Direc3D 클래스의 멤버변수 m_videoCardDescription은 멀티바이트 문자열(char[], 널문자 '\0' 종료)로 선언되어 있고
	* 어댑터 디스크립션 구조체의 Description 필드는 와이드 문자열(wchar_t)이기 때문에 와이드 문자열 -> 멀티 바이트 문자열로 변환
	* wcstombs_s() 함수는 버퍼 사이즈 체크로 버퍼 오버플로우를 방지하는 보안 CRT(C-Runtime) 함수
	*/
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	/*
	* 주사율과 그래픽 카드 정보 추출에 사용된 인터페이스, 구조체에 대한 메모리 해제
	*/

	delete[] displayModeList;
	displayModeList = nullptr;

	adapterOutput->Release();
	adapterOutput = nullptr;

	adapter->Release();
	adapter = nullptr;

	factory->Release();
	factory = nullptr;

#pragma endregion


#pragma region Set Swap Chain

	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	/*
	* 스왑 체인: 두개의 버퍼를 이용해 출력과 그리기를 동시에 수행하는 Direct3D 렌더링 기술
	* 스왑 체인 파이프라인:
	* 1. 프론트 버퍼를 이용해 모니터에 픽셀 출력
	* 2. 백 버퍼는 다음 프레임에 출력될 픽셀에 대한 그리기 작업 수행
	* 3. 백 버퍼의 그리기 작업이 끝나면 두 버퍼 스왑 백->프론트, 프론트->백
	*/

	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	/*
	* 백 버퍼 주사율 설정(초당 몇 프레임을 모니터에 출력할지, 60hz = 초당 60번 모니터에 출력)
	* V-Sync가 켜져(true) 있다면 미리 구했던(줄 41 ~ 114) 모니터 주사율로 설정
	* 반대의 경우 모니터 주사율과 관계없이 무제한(하드웨어 성능에 따라) 출력
	* 
	* V-Sync를 사용하지 않는 경우 티어링 현상(화면 찢어짐)이 발생할 수 있으나
	* 사용하는 경우 인풋렉의 원인이 되어 반응속도가 중요한 게임에선 단점이 될 수 있음
	*/
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	/*
	* 백 버퍼의 사용 용도를 명시
	* 그래픽 카드는 백 버퍼를 리소스로 연산을 수행하게 되고, 버퍼의 용도에 대한 정보를 플래그 형태로 전달
	* DXGI_USAGE_RENDER_TARGET_OUTPUT 플래그는 해당 버퍼를 픽셀 셰이더를 그리는 용도를 전달
	*/
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.SampleDesc.Count = 1; // 멀티 샘플링 안티 엘리어싱(MSAA), 1 = 사용 안함
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = fullscreen ? false : true;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // 사용한 백 버퍼의 내용을 버림
	swapChainDesc.Flags = 0;

#pragma endregion


#pragma region Init Direct3D

	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;

	featureLevel = D3D_FEATURE_LEVEL_11_0; // 다이렉트 3D API 버전 명시

	/*
	* D3D11CreateDeviceAndSwapChain(): D3D 디바이스, 디바이스 컨텍스트, 스왑 체인을 생성해주는 함수
	* D3D 디바이스: 렌더링 자원(텍스쳐, 버퍼, 셰이더)를 생성하는 객체
	* D3D 디바이스 컨텍스트: 렌더링 명령을 GPU로 전달하는 인터페이스
	*/
	result = D3D11CreateDeviceAndSwapChain(
		nullptr, 
		D3D_DRIVER_TYPE_HARDWARE, 
		nullptr, 
		0, 
		&featureLevel, 
		1, 
		D3D11_SDK_VERSION, 
		&swapChainDesc, 
		&m_swapChain, 
		&m_device, 
		nullptr, 
		&m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	/*
	* 렌더 타겟 뷰를 생성하기 위한 백 버퍼 추출
	*/
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	/*
	* 백 버퍼 주소를 이용해 렌더 타겟 뷰 생성
	*/
	result = m_device->CreateRenderTargetView(backBufferPtr, nullptr, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	backBufferPtr->Release();
	backBufferPtr = nullptr;

#pragma endregion


#pragma region Set Depth Buffer

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

	/*
	* 깊이 테스트: 두 개체가 겹쳐 있을때(ex. 벽 앞에 서있는 사람) 각 픽셀에 Z값(깊이) 정보를 통해 카메라(화면)에
	* 가장 가까운 픽셀(사람 픽셀에 가려진 '부분'의 벽은 렌더링 안함)만 출력되게 함 깊이 버퍼는 GPU에서 해당 연산을 수행하기 위한 Z값 리소스
	*/

	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 깊이 버터 3바이트, 스텐실 버퍼 1바이트
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	result = m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	/*
	* 스텐실 테스트: 스텐실이란 어떤 픽셀에 대한 렌더링 여부를 개발자가 부여하는 속성으로 그림자, 거울 반사등에 사용
	* 이후 렌더링 파이프 라인에서 스텐실 값을 이용해 그림자나 거울에 반사된 씬을 렌더링 하는데 사용
	*/

	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xff;
	depthStencilDesc.StencilWriteMask = 0xff;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	
#pragma endregion


#pragma region Set Rasterizer State

	D3D11_RASTERIZER_DESC rasterDesc;

	/*
	* 래스터라이저: 정점(Vertex)으로 구성된 삼각형 폴리곤을 프래그먼트(픽셀이 되기 전 정보 묶음)으로 변환하는 렌더링 파이프라인의 한 단계
	* 래스터라이저 디스크립션 구조체는 삼각형을 어떤식으로 그릴지에 대한 설정 정보를 담음
	*/

	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK; // 백페이스 컬링
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true; // 근평면, 원평면 바깥의 정점 클리핑
	rasterDesc.FillMode = D3D11_FILL_SOLID; // 삼각형 면 전체를 채움, D3D11_FILL_WIREFRAME으로 설정할 경우 바깥 선만 렌더링
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}
	
	m_deviceContext->RSSetState(m_rasterState);

	/*
	* 뷰포트 생성
	*/

	m_viewport.Width = (float)screenWidth;
	m_viewport.Height = (float)screenHeight;
	m_viewport.MinDepth = 0.0f; 
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	m_deviceContext->RSSetViewports(1, &m_viewport);

#pragma endregion


#pragma region Create Projection Matrix

	float fieldOfView, screenAspect;

	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	/*
	* MVP 행렬: 3D 모델이 가진 로컬 공간을 월드->뷰->클립 공간으로 변환하기 위한 행렬
	* Model 행렬: 로컬 공간을 월드 공간상의 위치로 변환하는 행렬, 예를들어 물체가 원점(월드)으로부터 상대적인 위치, 회전, 크기를 적용
	* View 행렬: 화면을 비추는 카메라 위치에 따라 월드 공간상의 물체의 상대적 위치를 표현한 뷰 공간을 만드는 행렬
	* Projection 행렬: 인간의 시야가 감지하는 원근감을 표현하거나(원근투영), 상대적으로 더 멀리있는 물체도 가까운 물체와 가깝게 표현(직교투영)하여 클립 공간을 만드는 행렬
	*/

	m_worldMatrix = XMMatrixIdentity();

	/*
	* 원근 투영 행렬은 일반적으로 시야각(FOV), 화면비(aspect ratio), 원근평면(near, far)으로 구성할 수 있음
	*/
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	/*
	* 뷰 행렬은 카메라 객체에서 생성하고 관리
	*/

	/*
	* 직교 투영 행렬은 GUI등 2D 인터페이스를 렌더링 하는데 사용되며 이미지, 텍스트 처리에 사용
	*/
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

#pragma endregion

	return true;
}

