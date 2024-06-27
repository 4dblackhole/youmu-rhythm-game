#pragma once
#include "framework.h"

#pragma region Effect
class Effect
{
public:
	Effect(ID3D11Device* device, const std::tstring& filename);
	virtual ~Effect();

private:
	Effect(const Effect& rhs);
	Effect& operator=(const Effect& rhs) = delete;

protected:
	ComPtr<ID3DX11Effect> mFX;
};
#pragma endregion

#pragma region SquareEffect
class SquareEffect : public Effect
{
public:
	SquareEffect(ID3D11Device* device, const std::tstring& filename);
	~SquareEffect();

	void SetWorld(CXMMATRIX M) const { mfxWorld->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProj(CXMMATRIX M) const { mfxWorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexture(ID3D11ShaderResourceView* tex) const { mfxTexture->SetResource(tex); }

	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorld;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;
	ID3DX11EffectShaderResourceVariable* mfxTexture;

};
#pragma endregion

#pragma region Effects
class Effects
{
public:
	static void Init(ID3D11Device* device);
	static void Release();

	static unique_ptr<SquareEffect> SquareFX;
};
#pragma endregion