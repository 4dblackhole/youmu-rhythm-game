#include "framework.h"
#include "Effects.h"
#include "InstancedTestEffect.h"

InstancedTestEffect::InstancedTestEffect(ID3D11Device* device, const std::tstring& filename) : Effect(device, filename)
{
	mTech = mFX->GetTechniqueByName("Tech");
	mTechInstanced = mFX->GetTechniqueByName("TechInstanced");

	mfxWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxView = mFX->GetVariableByName("gView")->AsMatrix();
	mfxProj = mFX->GetVariableByName("gProj")->AsMatrix();
}

InstancedTestEffect::~InstancedTestEffect()
{
}
