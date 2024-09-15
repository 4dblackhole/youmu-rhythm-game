#pragma once
#include "framework.h"

class Texture
{
public:
	Texture() : width(0), height(0),arrSize(0) {}
	Texture(ID3D11ShaderResourceView*& s)
	{
		srv = s;
		UpdateDescFromSRV(*this);
	}
	Texture(ID3D11Device* device3d, const wstring& file)
	{
		this->CreateTexture(device3d, file);
	}

	~Texture() {}

	void CreateTexture(ID3D11Device* device3d, const wstring& file)
	{
		HR(D3DX11CreateShaderResourceViewFromFile(device3d,
			file.c_str(), 0, 0, &srv, 0));

		UpdateDescFromSRV(*this);
	}

	static void UpdateDescFromSRV(Texture& t)
	{
		const D3D11_TEXTURE2D_DESC& desc = ShortCut::GetDescFromSRV(t.GetRefSRV());
		t.width = desc.Width;
		t.height = desc.Height;
		t.arrSize = desc.ArraySize;
	}

public:
	ID3D11ShaderResourceView*& GetRefSRV() { return *(srv.GetAddressOf()); }

	ComPtr<ID3D11ShaderResourceView> srv = nullptr;
	UINT width;
	UINT height;
	UINT arrSize;
};
