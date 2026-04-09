#include "PipelineStates.h"

bool PipelineStates::Initialize(ID3D11Device* device)
{
	// 깊이/스탠실 상태 정보 초기화
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
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

	if (FAILED(device->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState.GetAddressOf())))
	{
		return false;
	}

	// 래스터라이저 상태 정보 초기화
	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(rasterDesc));

	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	if (FAILED(device->CreateRasterizerState(&rasterDesc, m_rasterState.GetAddressOf())))
	{
		return false;
	}

    return true;
}

ID3D11DepthStencilState* PipelineStates::GetDepthStencilState() const
{
	return m_depthStencilState.Get();
}

ID3D11RasterizerState* PipelineStates::GetRasterizerState() const
{
	return m_rasterState.Get();
}

void PipelineStates::BindDepthStencilState(ID3D11DeviceContext* context)
{
	context->OMSetDepthStencilState(m_depthStencilState.Get(), 1);
}

void PipelineStates::BindRasterState(ID3D11DeviceContext* context)
{
	context->RSSetState(m_rasterState.Get());
}
