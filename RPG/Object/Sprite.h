#pragma once
#include "framework.h"

class Sprite
{
public:
	Sprite();
	Sprite(float w, float h);
	Sprite(float x, float y, float w, float h);
	Sprite(float x, float y, float w, float h, const XMFLOAT4 diffuse, const bool colormode);
	virtual ~Sprite();

	void SetTexture(ID3D11ShaderResourceView* ptr) { textureSRV = ptr; }
	void UpdateWorld();
	void Render(ID3D11DeviceContext*, const Camera&) const;

	void Resize(float w, float h);

	static void BulidBuffer(ID3D11Device*);
	static void BuildLayout(ID3D11Device*);

	XMFLOAT2 Scale;
	XMFLOAT3 Rotation;
	XMFLOAT3 Position;

	XMFLOAT4 Diffuse;

	bool ColorMode = false;

private:
	void Init(float x, float y, float w, float h, const XMFLOAT4 diffuse, const bool colormode);

	float x, y, w, h;

	XMFLOAT4X4 mWorld;
	
	ID3D11ShaderResourceView* textureSRV; //weak reference

	static ComPtr<ID3D11Buffer> mVB;
	static ComPtr<ID3D11Buffer> mIB;
	static ComPtr<ID3D11InputLayout> mInputLayout;

};

