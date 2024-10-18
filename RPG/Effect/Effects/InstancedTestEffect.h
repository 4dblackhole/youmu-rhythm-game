#pragma once
#include "framework.h"
#include "Effects.h"

class InstancedTestEffect : public Effect
{
public:
	InstancedTestEffect(ID3D11Device* device, const std::tstring& filename);
	virtual ~InstancedTestEffect();

	void SetWorld(DirectX::CXMMATRIX M) const { mfxWorld->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetView(DirectX::CXMMATRIX M) const { mfxView->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetProj(DirectX::CXMMATRIX M) const { mfxProj->SetMatrix(reinterpret_cast<const float*>(&M)); }

	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectTechnique* mTechInstanced;

	ID3DX11EffectMatrixVariable* mfxWorld;
	ID3DX11EffectMatrixVariable* mfxView;
	ID3DX11EffectMatrixVariable* mfxProj;
};
