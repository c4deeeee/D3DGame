#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class Direct3D
{
public:
	Direct3D();
	Direct3D(const Direct3D&);
	~Direct3D();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char*, int&);

	void SetBackBufferRenderTarget();
	void ResetViewport();

private:
	bool m_vsync_enabled;

	IDXGISwapChain* m_swapChain;
	ID3D11RenderTargetView* m_renderTargetView;

	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;

	ID3D11RasterizerState* m_rasterState;

	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthoMatrix;

	D3D11_VIEWPORT m_viewport;

};