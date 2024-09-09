#pragma once
#include "framework.h"
#include "System/AlignMode.h"
#include "System/World/World3D.h"

class SpriteDesc
{
public:
	SpriteDesc();
	~SpriteDesc();
	ID3D11ShaderResourceView* const GetTexture() const { return textureSRV; }
	void SetTexture(ID3D11ShaderResourceView* ptr) { textureSRV = ptr; }

	const World3D& GetWorld3dConst() const { return world3d; }
	World3D& GetWorld3d() { return world3d; }

	XMFLOAT4 Diffuse = { 1,1,1,1 };
	bool ColorMode = false;

private:
	World3D world3d;
	ID3D11ShaderResourceView* textureSRV = nullptr;

};

struct SpriteInstanceData
{
	XMFLOAT4X4 world;
	XMFLOAT4X4 uvworld;
	XMFLOAT4 Diffuse;
};

class Sprite
{
public:
	Sprite();
	Sprite(float w, float h);
	Sprite(float x, float y, float w, float h);
	Sprite(float x, float y, float w, float h, const XMFLOAT4 diffuse, const bool colormode);
	virtual ~Sprite();

	void SetTexture(ID3D11ShaderResourceView* ptr) { textureSRV = ptr; }
	void Render(ID3D11DeviceContext*, const Camera&);
	void Render(ID3D11DeviceContext*, const Camera&, SpriteDesc&);
	static void RenderInstanced(ID3D11DeviceContext*, const Camera&, const vector<SpriteInstanceData>&);
	void OnResize();

	void ChangeWidthToCurrentWidth(float w, float h);

	static void BulidBuffer(ID3D11Device*);
	static void BuildLayout(ID3D11Device*);
	static void BuildInstancedLayout(ID3D11Device*);

	const World3D& GetWorld3dConst() const { return world3d; }
	World3D& GetWorld3d() { return world3d; }

	void RepeatTexture(UINT imgWidth, UINT imgHeight, float imgRate = 1.0f);
	void RepeatTextureInExtraArea(UINT imgWidth, UINT imgHeight);

	XMFLOAT4 Diffuse;
	bool ColorMode = false;

private:
	void Init(float x, float y, float w, float h, const XMFLOAT4 diffuse, const bool colormode);

	World3D world3d;
	ID3D11ShaderResourceView* textureSRV; //weak reference, never delete this pointer

	void MakeCenterUV();

public:
	static ComPtr<ID3D11Buffer> mVB;
	static ComPtr<ID3D11Buffer> mIB;
	static ComPtr<ID3D11InputLayout> mInputLayout;
	static ComPtr<ID3D11InputLayout> mInstancedInputLayout;

};

