#include "RenderSurface.h"

#include <vector>

bool RenderSurface::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, HWND hwnd, int screenWidth, int screenHeight, bool vsync, bool fullscreen)
{
	// DXGI 팩토리 생성
	ComPtr<IDXGIDevice> dxgiDevice;
	device->QueryInterface(IID_PPV_ARGS(&dxgiDevice));

	ComPtr<IDXGIAdapter> adapter;
	dxgiDevice->GetAdapter(&adapter);

	ComPtr<IDXGIFactory> factory;
	adapter->GetParent(IID_PPV_ARGS(&factory));

	// 화면 주사율 탐색
	unsigned int numerator = 0, denominator = 1;
	if (!FindRefreshRate(adapter.Get(), &numerator, &denominator, screenWidth, screenHeight))
	{
		return false;
	}

	// 스왑 체인 초기화
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = fullscreen ? false : true;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	m_vsync = vsync;
	if (m_vsync)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	
	const HRESULT createSwapChainResult = factory.Get()->CreateSwapChain(device, &swapChainDesc, m_swapChain.GetAddressOf());
	if (FAILED(createSwapChainResult))
	{
		return false;
	}

	// 렌더 타겟 뷰 초기화
	ComPtr<ID3D11Texture2D> backBuffer;
	if (FAILED(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer)))
	{
		return false;
	}

	if (FAILED(device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_renderTargetView)))
	{
		return false;
	}

	// 깊이/스텐실 버퍼 초기화
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	if (FAILED(device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer)))
	{
		return false;
	}
	
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &depthStencilViewDesc, &m_depthStencilView)))
	{
		return false;
	}

	context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

	// 뷰포트 설정
	m_viewport.Width = (float)screenWidth;
	m_viewport.Height = (float)screenHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	context->RSSetViewports(1, &m_viewport);

    return true;
}

void RenderSurface::BeginScene(ID3D11DeviceContext* context, float red, float green, float blue, float alpha)
{
	float color[4];
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	context->ClearRenderTargetView(m_renderTargetView.Get(), color);
	context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.f, 0);
}

void RenderSurface::EndScene()
{
	if (m_vsync)
	{
		m_swapChain->Present(1, 0);
	}
	else
	{
		m_swapChain->Present(0, 0);
	}
}

void RenderSurface::BindRenderTarget(ID3D11DeviceContext* context)
{
	context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
}

void RenderSurface::BindViewport(ID3D11DeviceContext* context)
{
	context->RSSetViewports(1, &m_viewport);
}

bool RenderSurface::FindRefreshRate(IDXGIAdapter* adapter, unsigned int* numerator, unsigned int* denominator, unsigned int screenWidth, unsigned int screenHeight) const
{
	ComPtr<IDXGIOutput> adapterOutput;
	if (FAILED(adapter->EnumOutputs(0, &adapterOutput)))
	{
		return false;
	}

	unsigned int numModes = 0;
	if (FAILED(adapterOutput->GetDisplayModeList(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED,
		&numModes,
		nullptr)))
	{
		return false;
	}

	std::vector<DXGI_MODE_DESC> displayModeList(numModes);
	if (FAILED(adapterOutput->GetDisplayModeList(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED,
		&numModes,
		displayModeList.data())))
	{
		return false;
	}

	for (unsigned int i = 0; i < numModes; ++i)
	{
		if (displayModeList[i].Width == screenWidth)
		{
			if (displayModeList[i].Height == screenHeight)
			{
				*numerator = displayModeList[i].RefreshRate.Numerator;
				*denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	return true;
}