#pragma once
#include "framework.h"
#include "SingletonBase.h"

class TextureManager : public SingletonBase<TextureManager>
{
public:
	TextureManager() {};
	~TextureManager() {};

	void InitTextures(ID3D11Device* device);
	const ComPtr<ID3D11ShaderResourceView>& GetTexture(const string& str);

	class Name
	{
	public:
		DECLARE_VARIABLE_STRING(NullImg);
		DECLARE_VARIABLE_STRING(test);
		DECLARE_VARIABLE_STRING(myon);

		DECLARE_VARIABLE_STRING(fmodLogo);
		DECLARE_VARIABLE_STRING(Title);
		DECLARE_VARIABLE_STRING(MusicScroll);
	};

private:
	using TextureList = map<const string, ComPtr<ID3D11ShaderResourceView>>;
	TextureList textureList;
	void AddTexture(ID3D11Device* device, const wstring& file, string&& name);
};

#define GETTEXTURE(str) TextureManager::GetInstance().GetTexture(str).Get()