#pragma once
#include "framework.h"

struct Texture
{
	ComPtr<ID3D11ShaderResourceView> textureSRV = nullptr;
	UINT width = 0;
	UINT height = 0;

};
