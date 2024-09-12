#pragma once

class SpriteEffect : public Effect
{
public:
	SpriteEffect(ID3D11Device* device, const std::tstring& filename);
	SpriteEffect(ID3D11Device* device, const int resourceId);
	~SpriteEffect();

	void SetWorld(DirectX::CXMMATRIX& M) const { mfxWorld->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetView(DirectX::CXMMATRIX& M) const { mfxView->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetProj(DirectX::CXMMATRIX& M) const { mfxProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetUvWorld(DirectX::CXMMATRIX& M) const { mfxUvWorld->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexture(ID3D11ShaderResourceView* tex) const { mfxTexture->SetResource(tex); }

	void InitFxVariables();

	ID3DX11EffectTechnique* mTechTexture;
	ID3DX11EffectTechnique* mTechColor;
	ID3DX11EffectTechnique* mTechTextureInstanced;

	ID3DX11EffectMatrixVariable* mfxWorld;
	ID3DX11EffectMatrixVariable* mfxUvWorld;
	ID3DX11EffectMatrixVariable* mfxView;
	ID3DX11EffectMatrixVariable* mfxProj;
	ID3DX11EffectShaderResourceVariable* mfxTexture;
	ID3DX11EffectVectorVariable* mfxTextureDiffuse;

};
