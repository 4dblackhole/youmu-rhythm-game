#pragma once
#include "framework.h"

class TextureManager
{
public:
	using ContainerType = map<const string, Texture>;

	TextureManager() {};
	~TextureManager() {};

	const ContainerType::mapped_type& GetTexture(ContainerType::key_type& str);
	void AddTexture(ID3D11Device* device, ContainerType::key_type& key, const wstring& file);
	void RemoveTexture(ContainerType::key_type & key);
	void Clear();

	ContainerType::mapped_type& GetNullTexture()
	{
		static ContainerType::mapped_type nullTexture(App->GetDevice(), TextureDir + L"TextureNotFound.png");
		return nullTexture;
	}

private:
	ContainerType textureList;
	/*
	class Name
	{
	public:
		DECLARE_VARIABLE_STRING(test);
		DECLARE_VARIABLE_STRING(myon);

		DECLARE_VARIABLE_STRING(Title);
		DECLARE_VARIABLE_STRING(MusicScroll);
	};
	void TextureManager::InitTextures(ID3D11Device* device)
	{
		AddTexture(device, TextureDir + L"TextureNotFound.png", Name::NullImg);
		AddTexture(device, TextureDir + L"tempTexture.png", Name::test);
		AddTexture(device, TextureDir + L"myon.png", Name::myon);
		AddTexture(device, TextureDir + L"SelectMusic/MusicScroll.png", Name::MusicScroll);
	}
	*/
};
