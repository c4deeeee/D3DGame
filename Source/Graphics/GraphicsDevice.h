#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <wrl.h>
#include <string>

using namespace Microsoft::WRL;

class GraphicsDevice
{
public:
	GraphicsDevice() = default;
	~GraphicsDevice() = default;
	GraphicsDevice(const GraphicsDevice& rhs) = delete;
	GraphicsDevice& operator=(const GraphicsDevice& rhs) = delete;

	bool Initialize();

	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetDeviceContext() const;
	const std::string& GetGpuDescription() const;
	int GetVideoMemoryMB() const;

private:
	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;

	std::string m_gpuDescription;
	int m_videoMemory = 0;
};
