#include "framework.h"
#include "Effects.h"

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

	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 0, device, &mFX));

}

Effect::~Effect()
{
}


std::unique_ptr<SpriteEffect> EffectList::SpriteFX;
std::unique_ptr<InstancedTestEffect> EffectList::InstancedTestFX;

void EffectList::Init(ID3D11Device* device)
{
	//SpriteFX.reset(new SpriteEffect(device, IDR_SHADER_SPRITE));

	InstancedTestFX.reset(new InstancedTestEffect(device, L"Shader/InstancedTest.fxo"));
	SpriteFX.reset(new SpriteEffect(device, L"Shader/Sprite.fxo"));

}

void EffectList::Release()
{
}
