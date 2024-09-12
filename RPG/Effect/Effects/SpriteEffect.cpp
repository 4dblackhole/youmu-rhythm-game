#include "framework.h"
#include "Effects.h"
#include "SpriteEffect.h"

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
	mTechTextureInstanced = mFX->GetTechniqueByName("TechTextureInstanced");

	mfxWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxUvWorld = mFX->GetVariableByName("gUvWorld")->AsMatrix();
	mfxView = mFX->GetVariableByName("gView")->AsMatrix();
	mfxProj = mFX->GetVariableByName("gProj")->AsMatrix();
	mfxTexture = mFX->GetVariableByName("gTexture")->AsShaderResource();
	mfxTextureDiffuse = mFX->GetVariableByName("gTextureDiffuse")->AsVector();
	mfxTextureID = mFX->GetVariableByName("gTextureID")->AsScalar();
}
