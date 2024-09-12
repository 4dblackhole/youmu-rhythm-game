#pragma once
#include "framework.h"

class Texture
{
public:
	Texture() : width(0), height(0) {}
	~Texture() {}

	void CreateTexture(ID3D11Device* device3d, const wstring& file)
	{
		HR(D3DX11CreateShaderResourceViewFromFile(device3d,
			file.c_str(), 0, 0, textureSRV.GetAddressOf(), 0));

		GetDimensionFromSRV();

	}

	void GetDimensionFromSRV()
	{
		const D3D11_TEXTURE2D_DESC& desc = ShortCut::GetDescFromSRV(*textureSRV.GetAddressOf());
		width = desc.Width;
		height = desc.Height;
	}

	ComPtr<ID3D11ShaderResourceView> textureSRV;
	UINT width;
	UINT height;
};
