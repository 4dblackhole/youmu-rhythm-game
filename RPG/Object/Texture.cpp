#include "framework.h"
#include "Texture.h"

Texture::Texture(ID3D11ShaderResourceView*& s)
{
	srv = s;
	UpdateDescFromSRV(*this);
}

Texture::Texture(ID3D11Device* device3d, const wstring& file)
{
	this->CreateTexture(device3d, file);
}

Texture::~Texture()
{ 
	ReleaseCOM(srv); 
}

void Texture::CreateTexture(ID3D11Device* device3d, const wstring& file)
{
	ReleaseCOM(srv);
	HR(D3DX11CreateShaderResourceViewFromFile(device3d,
		file.c_str(), 0, 0, &srv, 0));

	UpdateDescFromSRV(*this);
}

void Texture::UpdateDescFromSRV(Texture& t)
{
	const D3D11_TEXTURE2D_DESC& desc = ShortCut::GetDescFromSRV(t.GetRefSRV());
	t.width = desc.Width;
	t.height = desc.Height;
	t.arrSize = desc.ArraySize;
}
