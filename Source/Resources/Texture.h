#pragma once

#include <d3d11.h>
#include <DirectXTex.h>
#include <wrl.h>
#include <string>

using namespace DirectX;
using namespace Microsoft::WRL;

class Texture
{
public:
	Texture() = default;
	~Texture() = default;
	Texture(const Texture& rhs) = delete;
	Texture& operator=(const Texture& rhs) = delete;

	bool Initialize(ID3D11Device* device, const std::string filename);

	ID3D11ShaderResourceView* GetTexture();

	const std::string GetTextureName() const;

private:
	std::string m_textureName;

	ComPtr<ID3D11ShaderResourceView> m_texture;

};