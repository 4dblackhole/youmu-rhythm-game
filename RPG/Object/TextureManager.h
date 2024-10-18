#pragma once

class TextureManager
{
public:
	using ContainerType = map<const string, Texture>;

	TextureManager() {};
	~TextureManager() {};

	ContainerType::mapped_type& GetTexture(const ContainerType::key_type& str);
	void AddTexture(ID3D11Device* device, ContainerType::key_type& key, const wstring& file);
	void AddTexture(ContainerType::key_type& key, ContainerType::mapped_type& texture);

	bool AddTextureArr(const string& keyStr, const vector<LPCWSTR>& fileList);
	bool AddTextureArrIndexed(const string& keyStr, const wstring& fileName);

	void RemoveTexture(const ContainerType::key_type& key);
	void Clear();

	static ContainerType::mapped_type& GetNullTexture();
	static void ReleaseNullTexture();

private:
	void AddTextureArr_LoadTextureList(const vector<LPCWSTR>& fileList, vector<ID3D11Texture2D*>& textureList);
	bool AddTextureArr_MergeTextureList(const string& keyStr, const vector<ID3D11Texture2D*>& textureList);
	
	ContainerType textureList;
};
