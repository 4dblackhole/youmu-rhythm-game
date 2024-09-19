#include "framework.h"
#include "TextureManager.h"

void TextureManager::AddTexture(ID3D11Device* device, ContainerType::key_type& key, const wstring& file)
{
	Texture tempTexture;
	tempTexture.CreateTexture(device, file);
	textureList.emplace(make_pair(key, tempTexture));
}

void TextureManager::RemoveTexture(ContainerType::key_type& key)
{
	// image search
	ContainerType::iterator it = textureList.find(key);
	if (it != textureList.end()) textureList.erase(it);
}

void TextureManager::Clear()
{
	textureList.clear();
}

const TextureManager::ContainerType::mapped_type& TextureManager::GetTexture(ContainerType::key_type& key)
{
	// image search
	ContainerType::iterator it = textureList.find(key);
	if (it != textureList.end()) return it->second;
	else return GetNullTexture();
}

TextureManager::ContainerType::mapped_type& TextureManager::GetNullTexture()
{
	static ContainerType::mapped_type nullTexture(App->GetDevice(), TextureDir + L"TextureNotFound.png");
	return nullTexture;
}