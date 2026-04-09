#include "Texture.h"
#include "../Platform/Utility.h"

bool Texture::Initialize(ID3D11Device* device, const std::string filename)
{
	ScratchImage image;
	TexMetadata metadata;

	if (FAILED(LoadFromWICFile(
		Utf8ToWide(filename).c_str(),
		WIC_FLAGS_NONE,
		&metadata,
		image)))
	{
		return false;
	}

	ComPtr<ID3D11Resource> texture;
	if (FAILED(CreateTexture(
		device,
		image.GetImages(),
		image.GetImageCount(),
		metadata,
		texture.GetAddressOf())))
	{
		return false;
	}

	if (FAILED(device->CreateShaderResourceView(
		texture.Get(), 
		nullptr, 
		m_texture.GetAddressOf())))
	{
		return false;
	}

	m_textureName = filename;

	return true;
}

ID3D11ShaderResourceView* Texture::GetTexture()
{
	return m_texture.Get();
}

const std::string Texture::GetTextureName() const
{
	return m_textureName;
}
