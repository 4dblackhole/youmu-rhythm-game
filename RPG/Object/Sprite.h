#pragma once
#include "framework.h"
#include "System/AlignMode.h"

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
	void OnResize();

	void ChangeWidthToCurrentWidth(float w, float h);

	static void BulidBuffer(ID3D11Device*);
	static void BuildLayout(ID3D11Device*);

	inline const D2D1_RECT_F& GetDrawArea() const { return drawArea; }

	void SetScale(const XMFLOAT2 s);
	void SetRotation(const XMFLOAT3 s);
	void SetPosition(const XMFLOAT3 s);
	void MovePosition(float x, float y, float z);

	void SetUvScale(const XMFLOAT2 s);
	void SetUvRotation(const XMFLOAT3 s);
	void SetUvPosition(const XMFLOAT2 s);

	void SetAlignX(AlignModeX m);
	void SetAlignY(AlignModeY m);

	const XMFLOAT2& GetLocalScale() const { return LocalScale; }
	const XMFLOAT3& GetLocalPosition() const { return LocalPosition; }

	const XMFLOAT2& GetScale() const { return Scale; }
	const XMFLOAT3& GetPosition() const { return Position; }

	const AlignModeX& GetAlignX() const { return alignX; }
	const AlignModeY& GetAlignY() const { return alignY; }

	XMFLOAT4 Diffuse;

	bool ColorMode = false;

	void UpdateLocalWorld();
	void UpdateWorld();
	void UpdateGlobalWorld();
	void UpdateUvWorld();

private:
	void Init(float x, float y, float w, float h, const XMFLOAT4 diffuse, const bool colormode);

	//variables to describe the shape
	XMFLOAT2 LocalScale;
	XMFLOAT3 LocalRotation;
	XMFLOAT3 LocalPosition;
	XMFLOAT4X4 mLocalWorld;

	XMFLOAT2 Scale;
	XMFLOAT3 Rotation;
	XMFLOAT3 Position;
	XMFLOAT4X4 mWorld;

	XMFLOAT4X4 mParentWorld;
	XMFLOAT4X4 mGlobalWorld;

	bool updateUvWorldFlag = false;
	XMFLOAT2 UvScale;
	XMFLOAT3 UvRotation;
	XMFLOAT2 UvPosition;
	XMFLOAT4X4 mUvWorld;
	
	ID3D11ShaderResourceView* textureSRV; //weak reference, never delete this pointer

	void AdjustDrawPos();
	XMFLOAT3 DrawPos;
	AlignModeX alignX = AlignModeX::Mid;
	AlignModeY alignY = AlignModeY::Mid;

	void UpdateDrawArea();
	D2D1_RECT_F drawArea{};

	static ComPtr<ID3D11Buffer> mVB;
	static ComPtr<ID3D11Buffer> mIB;
	static ComPtr<ID3D11InputLayout> mInputLayout;

};

