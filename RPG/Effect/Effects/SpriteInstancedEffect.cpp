#include "framework.h"
#include "Effects.h"
#include "SpriteInstancedEffect.h"

SpriteInstancedEffect::SpriteInstancedEffect(ID3D11Device* device, const std::tstring& filename)
	: Effect(device, filename)
{
	InitFxVariables();
}

SpriteInstancedEffect::SpriteInstancedEffect(ID3D11Device* device, const int resourceId)
	: Effect(device, resourceId)
{
	InitFxVariables();
}

SpriteInstancedEffect::~SpriteInstancedEffect()
{
}

void SpriteInstancedEffect::InitFxVariables()
{
	mTechTexture = mFX->GetTechniqueByName("TechTexture");
	mTechColor = mFX->GetTechniqueByName("TechColor");

	mfxView = mFX->GetVariableByName("gView")->AsMatrix();
	mfxProj = mFX->GetVariableByName("gProj")->AsMatrix();
	mfxTexture = mFX->GetVariableByName("gTexture")->AsShaderResource();
}
