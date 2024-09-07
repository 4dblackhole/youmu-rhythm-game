#pragma once
#include "framework.h"

class Texture
{
public:
	Texture() : width(0), height(0) {}
	~Texture() {}

	void CreateTexture(ID3D11Device* device3d, D2Ddevice* device2d, const wstring& file)
	{
		HR(D3DX11CreateShaderResourceViewFromFile(device3d,
			file.c_str(), 0, 0, textureSRV.GetAddressOf(), 0));
		auto desc = ShortCut::GetDescFromSRV(*textureSRV.GetAddressOf());
		width = desc.Width;
		height = desc.Height;
	}

	ComPtr<ID3D11ShaderResourceView> textureSRV;
	UINT width;
	UINT height;
};
