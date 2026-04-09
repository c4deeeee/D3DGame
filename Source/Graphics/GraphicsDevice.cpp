#include "GraphicsDevice.h"
#include "../Platform/Utility.h"

bool GraphicsDevice::Initialize()
{
	UINT flags = 0;
#if defined(_DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL requestedFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL createdFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	const HRESULT createDeviceResult = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		flags,
		&requestedFeatureLevel,
		1,
		D3D11_SDK_VERSION,
		m_device.GetAddressOf(),
		&createdFeatureLevel,
		m_context.GetAddressOf());
	if (FAILED(createDeviceResult))
	{
		m_device.Reset();
		m_context.Reset();
		return false;
	}

	ComPtr<IDXGIDevice> dxgiDevice;
	if (FAILED(m_device.As(&dxgiDevice)))
	{
		return false;
	}

	ComPtr<IDXGIAdapter> adapter;
	if (FAILED(dxgiDevice->GetAdapter(adapter.GetAddressOf())))
	{
		return false;
	}

	DXGI_ADAPTER_DESC adapterDesc = {};
	if (FAILED(adapter->GetDesc(&adapterDesc)))
	{
		return false;
	}

	m_gpuDescription = WideToUtf8(adapterDesc.Description);
	m_videoMemory = static_cast<int>(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	return true;
}

ID3D11Device* GraphicsDevice::GetDevice() const
{
	return m_device.Get();
}

ID3D11DeviceContext* GraphicsDevice::GetDeviceContext() const
{
	return m_context.Get();
}

const std::string& GraphicsDevice::GetGpuDescription() const
{
	return m_gpuDescription;
}

int GraphicsDevice::GetVideoMemoryMB() const
{
	return m_videoMemory;
}
