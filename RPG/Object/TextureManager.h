#pragma once

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

	ContainerType::mapped_type& GetNullTexture();

private:
	ContainerType textureList;
};
