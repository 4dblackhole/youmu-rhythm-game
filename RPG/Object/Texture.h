#pragma once
#include "framework.h"

class Texture
{
public:
	Texture() : width(0), height(0),arrSize(0) {}
	Texture(ID3D11ShaderResourceView*& srv)
	{
		textureSRV = srv;
		UpdateDescFromSRV(*this);
	}

	~Texture() {}

	void CreateTexture(ID3D11Device* device3d, const wstring& file)
	{
		HR(D3DX11CreateShaderResourceViewFromFile(device3d,
			file.c_str(), 0, 0, textureSRV.GetAddressOf(), 0));

		UpdateDescFromSRV(*this);
	}

	static void UpdateDescFromSRV(Texture& t)
	{
		const D3D11_TEXTURE2D_DESC& desc = ShortCut::GetDescFromSRV(*t.textureSRV.GetAddressOf());
		t.width = desc.Width;
		t.height = desc.Height;
		t.arrSize = desc.ArraySize;
	}

public:
	ComPtr<ID3D11ShaderResourceView> textureSRV;
	UINT width;
	UINT height;
	UINT arrSize;
};
