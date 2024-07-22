#include "framework.h"
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
	AddTexture(device, TextureDir+ L"TextureNotFound.png", TextureFindError);
	AddTexture(device, TextureDir+ L"tempTexture.png", "test");
	AddTexture(device, TextureDir+ L"myon.png", "myon");
	AddTexture(device, TextureDir+ L"Fmod/FMOD Logo White - Black Background.png", Name::fmodLogo);
	AddTexture(device, TextureDir+ L"SelectMusic/MusicScroll.png", Name::MusicScroll);
}

const ComPtr<ID3D11ShaderResourceView>& TextureManager::GetTexture(const string& str)
{
	// image search
	TextureList::iterator it = textureList.find(str);
	if (it != textureList.end()) return it->second;
	else return textureList[TextureFindError];
}
