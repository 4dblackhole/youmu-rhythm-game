#pragma once

#include <D2D1_1.h>
#include <dwrite.h>
#include <D3D11.h>
#include <D3DX11.h>
#include <d3dcompiler.h>
#include <effects.h>
#include <dxgidebug.h>
#include <DxErr.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <d3dx11effect.h>

#include <map>
#include "App/MainApp.h"

class TextureManager
{
public:
	using ContainerType = map<const string, Texture>;

	TextureManager() {};
	~TextureManager() {};

	const ContainerType::mapped_type& GetTexture(const ContainerType::key_type& str);
	void AddTexture(ID3D11Device* device, ContainerType::key_type& key, const wstring& file);
	void RemoveTexture(const ContainerType::key_type& key);
	void Clear();

	ContainerType::mapped_type& GetNullTexture()
	{
		static ContainerType::mapped_type nullTexture(App->GetDevice(), TextureDir + L"TextureNotFound.png");
		return nullTexture;
	}

private:
	ContainerType textureList;
};
