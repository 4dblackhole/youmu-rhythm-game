#pragma once
#include "framework.h"
#include "System/AlignMode.h"
//#include "System/World3D.h"

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

	void SetLocalScale(const XMFLOAT2 s);
	void SetLocalRotation(const XMFLOAT3 s);
	void SetLocalPosition(const XMFLOAT3 s);
	void MoveLocalPosition(float x, float y, float z);

	void SetUvScale(const XMFLOAT2 s);
	void SetUvRotation(const XMFLOAT3 s);
	void SetUvPosition(const XMFLOAT2 s);

	void SetAlignX(AlignModeX m);
	void SetAlignY(AlignModeY m);

	const XMFLOAT2& GetObjectScale() const { return ObjectScale; }
	const XMFLOAT3& GetObjectPosition() const { return ObjectPosition; }

	const XMFLOAT2& GetScale() const { return LocalScale; }
	const XMFLOAT3& GetPosition() const { return LocalPosition; }

	const AlignModeX& GetAlignX() const { return alignX; }
	const AlignModeY& GetAlignY() const { return alignY; }

	XMFLOAT4 Diffuse;
	bool ColorMode = false;

	const XMFLOAT4X4& GetObjectWorld();
	const XMFLOAT4X4& GetLocalWorld();
	const XMFLOAT4X4& GetGlobalWorld();
	const XMFLOAT4X4& GetUvWorld();

private:
	void Init(float x, float y, float w, float h, const XMFLOAT4 diffuse, const bool colormode);

	//variables to describe the shape
	XMFLOAT2 ObjectScale;
	XMFLOAT3 ObjectRotation;
	XMFLOAT3 ObjectPosition;
	XMFLOAT4X4 mObjectWorld;

	//relative world to the parent world
	XMFLOAT2 LocalScale;
	XMFLOAT3 LocalRotation;
	XMFLOAT3 LocalPosition;
	XMFLOAT4X4 mLocalWorld;

	//total world
	XMFLOAT4X4 mGlobalWorld;

	XMFLOAT2 UvScale;
	XMFLOAT3 UvRotation;
	XMFLOAT2 UvPosition;
	XMFLOAT4X4 mUvWorld;

	void UpdateObjectWorld();
	void UpdateLocalWorld();
	void UpdateGlobalWorld();
	void UpdateUvWorld();

	bool mObjectWorldUpdateFlag = true;
	bool mLocalWorldUpdateFlag = true;
	bool mGlobalWorldUpdateFlag = true;
	bool mUvWorldUpdateFlag = true;

	const XMFLOAT4X4* mParentWorld = nullptr;

	AlignModeX alignX = AlignModeX::Mid;
	AlignModeY alignY = AlignModeY::Mid;

	ID3D11ShaderResourceView* textureSRV; //weak reference, never delete this pointer

	static ComPtr<ID3D11Buffer> mVB;
	static ComPtr<ID3D11Buffer> mIB;
	static ComPtr<ID3D11InputLayout> mInputLayout;

};

