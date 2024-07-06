#include "framework.h"

#include "Effects.h"

#pragma region Effect
Effect::Effect(ID3D11Device* device, const std::tstring& filename)
	: mFX(nullptr)
{
	std::ifstream fin(filename, std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();

	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size,
		0, device, &mFX));
}

Effect::~Effect()
{
}

#pragma endregion

#pragma region SpriteEffect
SpriteEffect::SpriteEffect(ID3D11Device* device, const std::tstring& filename)
	: Effect(device, filename)
{
	mTechTexture = mFX->GetTechniqueByName("TechTexture");
	mTechColor = mFX->GetTechniqueByName("TechColor");

	mfxWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	mfxTexture = mFX->GetVariableByName("gTexture")->AsShaderResource();
	mfxTextureDiffuse = mFX->GetVariableByName("gTextureDiffuse")->AsVector();
}

SpriteEffect::~SpriteEffect()
{
}

#pragma endregion

#pragma region Effects

std::unique_ptr<SpriteEffect> Effects::SpriteFX;

void Effects::Init(ID3D11Device* device)
{
	SpriteFX.reset(new SpriteEffect(device, L"Shader/Sprite.fxo"));
}

void Effects::Release()
{
}
#pragma endregion