#include "TextureManager.h"

static constexpr LPCSTR TextureFindError = "NullImg";

void TextureManager::AddTexture(ID3D11Device* device, const wstring& file, string&& name)
{
	ID3D11ShaderResourceView* tempTexture;
	HR(D3DX11CreateShaderResourceViewFromFile(device,
		file.c_str(), 0, 0, &tempTexture, 0));
	textureList.insert(make_pair(name, tempTexture));
	ReleaseCOM(tempTexture);
}

void TextureManager::InitTextures(ID3D11Device* device)
{
	AddTexture(device, L"Textures/TextureNotFound.png", TextureFindError);
	AddTexture(device, L"Textures/tempTexture.png", "test");
	AddTexture(device, L"Textures/myon.png", "myon");
	AddTexture(device, L"Textures/Fmod/FMOD Logo White - Black Background.png", Name::fmodLogo);
}

const ComPtr<ID3D11ShaderResourceView>& TextureManager::GetTexture(const string& str)
{
	// image search
	if (textureList.find(str) != textureList.end()) return textureList[str];
	else return textureList[TextureFindError];
}
