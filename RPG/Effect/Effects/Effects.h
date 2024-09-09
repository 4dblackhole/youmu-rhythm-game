#pragma once
#include <Windowsx.h>
#include <wrl.h>

#include <D3D11.h>
#include <D3DX11.h>
#include <d3dcompiler.h>
#include <effects.h>
#include <dxgidebug.h>
#include <DxErr.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <d3dx11effect.h>

class Effect
{
public:
	Effect(ID3D11Device* device, const std::tstring& filename);
	Effect(ID3D11Device* device, const int resourceId);
	virtual ~Effect();

private:
	Effect(const Effect& rhs);
	Effect& operator=(const Effect& rhs) = delete;

protected:
	Microsoft::WRL::ComPtr<ID3DX11Effect> mFX;
};

#include "SpriteEffect.h"
#include "SpriteInstancedEffect.h"
#include "InstancedTestEffect.h"

class EffectList
{
public:
	static void Init(ID3D11Device* device);
	static void Release();

	static std::unique_ptr<SpriteEffect> SpriteFX;
	static std::unique_ptr<SpriteInstancedEffect> SpriteInstancedFX;
	static std::unique_ptr<InstancedTestEffect> InstancedTestFX;
};
