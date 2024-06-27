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

#pragma region SquareEffect
SquareEffect::SquareEffect(ID3D11Device* device, const std::tstring& filename)
	: Effect(device, filename)
{
	mTech = mFX->GetTechniqueByName("SquareTech");

	mfxWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	mfxTexture = mFX->GetVariableByName("gTexture")->AsShaderResource();
}

SquareEffect::~SquareEffect()
{
}

#pragma endregion

#pragma region Effects

unique_ptr<SquareEffect> Effects::SquareFX;

void Effects::Init(ID3D11Device* device)
{
	SquareFX.reset(new SquareEffect(device, L"Object/Square.fxo"));
}

void Effects::Release()
{
}
#pragma endregion