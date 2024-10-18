#pragma once
#include "framework.h"

class Texture
{
public:
	Texture() : width(0), height(0), arrSize(0) {}
	Texture(ID3D11ShaderResourceView*& s);
	Texture(ID3D11Device* device3d, const wstring& file);
	~Texture();

	void CreateTexture(ID3D11Device* device3d, const wstring& file);
	
	static void UpdateDescFromSRV(Texture& t);

	ID3D11ShaderResourceView*& GetRefSRV() { return srv; }
	ID3D11ShaderResourceView* const& GetRefSRVConst() const { return srv; }

public:

	UINT width;
	UINT height;
	UINT arrSize;

private:
	ID3D11ShaderResourceView* srv = nullptr;
};
