#pragma once

#include <d3d11.h>
#include <wrl.h>

using namespace Microsoft::WRL;

class PipelineStates
{
public:
	PipelineStates() = default;
	~PipelineStates() = default;
	PipelineStates(const PipelineStates& rhs) = delete;
	PipelineStates& operator=(const PipelineStates& rhs) = delete;

	bool Initialize(ID3D11Device* device);

	ID3D11DepthStencilState* GetDepthStencilState() const;
	ID3D11RasterizerState* GetRasterizerState() const;

	void BindDepthStencilState(ID3D11DeviceContext* context);
	void BindRasterState(ID3D11DeviceContext* context);

private:
	ComPtr<ID3D11DepthStencilState> m_depthStencilState;
	ComPtr<ID3D11RasterizerState> m_rasterState;

};