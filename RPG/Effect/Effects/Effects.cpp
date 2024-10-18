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

	ReleaseCOM(mFX);
	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 0, device, &mFX));

}

Effect::~Effect()
{
	ReleaseCOM(mFX);
}


SpriteEffect* EffectList::SpriteFX = nullptr;
InstancedTestEffect* EffectList::InstancedTestFX = nullptr;

void EffectList::Init(ID3D11Device* device)
{
	//SpriteFX.reset(new SpriteEffect(device, IDR_SHADER_SPRITE));

	EffectList::Release();

	InstancedTestFX = (new InstancedTestEffect(device, L"Shader/InstancedTest.fxo"));
	SpriteFX = (new SpriteEffect(device, L"Shader/Sprite.fxo"));

}

void EffectList::Release()
{
	SafeDelete(InstancedTestFX);
	SafeDelete(SpriteFX);
}
