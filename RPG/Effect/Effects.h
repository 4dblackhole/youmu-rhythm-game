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

#pragma region Effect
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
#pragma endregion

#pragma region SpriteEffect
class SpriteEffect : public Effect
{
public:
	SpriteEffect(ID3D11Device* device, const std::tstring& filename);
	SpriteEffect(ID3D11Device* device, const int resourceId);
	~SpriteEffect();

	void SetWorld(DirectX::CXMMATRIX M) const { mfxWorld->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProj(DirectX::CXMMATRIX M) const { mfxWorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexture(ID3D11ShaderResourceView* tex) const { mfxTexture->SetResource(tex); }

	void InitFxVariables();

	ID3DX11EffectTechnique* mTechTexture;
	ID3DX11EffectTechnique* mTechColor;
	ID3DX11EffectMatrixVariable* mfxWorld;
	ID3DX11EffectMatrixVariable* mfxUvWorld;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;
	ID3DX11EffectShaderResourceVariable* mfxTexture;
	ID3DX11EffectVectorVariable* mfxTextureDiffuse;

};
#pragma endregion

#pragma region Effects
class EffectList
{
public:
	static void Init(ID3D11Device* device);
	static void Release();

	static std::unique_ptr<SpriteEffect> SpriteFX;
};
#pragma endregion