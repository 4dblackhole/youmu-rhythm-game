#pragma once

class SpriteInstancedEffect : public Effect
{
public:
	SpriteInstancedEffect(ID3D11Device* device, const std::tstring& filename);
	SpriteInstancedEffect(ID3D11Device* device, const int resourceId);
	~SpriteInstancedEffect();

	void SetView(DirectX::CXMMATRIX M) const { mfxView->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetProj(DirectX::CXMMATRIX M) const { mfxProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexture(ID3D11ShaderResourceView* tex) const { mfxTexture->SetResource(tex); }

	void InitFxVariables();

	ID3DX11EffectTechnique* mTechTexture;
	ID3DX11EffectTechnique* mTechColor;

	ID3DX11EffectMatrixVariable* mfxView;
	ID3DX11EffectMatrixVariable* mfxProj;
	ID3DX11EffectShaderResourceVariable* mfxTexture;

};
