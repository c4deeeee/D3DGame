#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <wrl.h>

using namespace Microsoft::WRL;

class RenderSurface
{
public:
	RenderSurface() = default;
	~RenderSurface() = default;
	RenderSurface(const RenderSurface& rhs) = delete;
	RenderSurface& operator=(const RenderSurface& rhs) = delete;

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context, HWND hwnd, int screenWidth, int screenHeight, bool vsync, bool fullscreen);

	void BeginScene(ID3D11DeviceContext* context, float red, float green, float blue, float alpha);
	void EndScene();

	void BindRenderTarget(ID3D11DeviceContext* context);
	void BindViewport(ID3D11DeviceContext* context);

private:
	bool FindRefreshRate(IDXGIAdapter* adapter, unsigned int* numerator, unsigned int* denominator, unsigned int screenWidth, unsigned int screenHeight) const;

private:
	ComPtr<IDXGISwapChain> m_swapChain;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;

	D3D11_VIEWPORT m_viewport = {};
	bool m_vsync = false;
};
