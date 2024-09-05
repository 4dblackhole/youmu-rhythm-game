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

Effect::Effect(ID3D11Device* device, const int resourceId)
{
	HRSRC hRes = FindResource(nullptr, MAKEINTRESOURCE(resourceId), RT_RCDATA);
	assert(hRes != nullptr);

	HGLOBAL hResData = LoadResource(nullptr, hRes);
	assert(hResData != nullptr);

	void* pShaderData = LockResource(hResData);
	DWORD shaderSize = SizeofResource(nullptr, hRes);
}

Effect::~Effect()
{
}

#pragma endregion

#pragma region SpriteEffect
SpriteEffect::SpriteEffect(ID3D11Device* device, const std::tstring& filename)
	: Effect(device, filename)
{
	InitFxVariables();
}

SpriteEffect::SpriteEffect(ID3D11Device* device, const int resourceId)
	: Effect(device, resourceId)
{
	InitFxVariables();
}

SpriteEffect::~SpriteEffect()
{
}

void SpriteEffect::InitFxVariables()
{
	mTechTexture = mFX->GetTechniqueByName("TechTexture");
	mTechColor = mFX->GetTechniqueByName("TechColor");

	mfxWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxUvWorld = mFX->GetVariableByName("gUvWorld")->AsMatrix();
	mfxView= mFX->GetVariableByName("gView")->AsMatrix();
	mfxProj = mFX->GetVariableByName("gProj")->AsMatrix();
	mfxTexture = mFX->GetVariableByName("gTexture")->AsShaderResource();
	mfxTextureDiffuse = mFX->GetVariableByName("gTextureDiffuse")->AsVector();
}

#pragma endregion

#pragma region Effects

std::unique_ptr<SpriteEffect> EffectList::SpriteFX;

void EffectList::Init(ID3D11Device* device)
{
	SpriteFX.reset(new SpriteEffect(device, L"Shader/Sprite.fxo"));
	//SpriteFX.reset(new SpriteEffect(device, IDR_SHADER_SPRITE));
}

void EffectList::Release()
{
}
#pragma endregion