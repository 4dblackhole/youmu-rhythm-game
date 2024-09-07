#pragma once
#include "framework.h"
#include "System/AlignMode.h"

class World3D
{
public:
	World3D() : World3D({ 1.0f,1.0f }, {}, {}) {}
	World3D(XMFLOAT2 size, XMFLOAT3 rot, XMFLOAT3 position);
	constexpr World3D(const World3D& w); 
	constexpr World3D(World3D&& w) noexcept;
	World3D& operator=(const World3D& w);
	World3D& operator=(World3D&& w) noexcept;
	~World3D();

	void SetCenterPosition(const XMFLOAT3);

	void SetObjectScale(const XMFLOAT2 s);
	void SetObjectScale(const FLOAT s);
	void SetObjectRotation(const XMFLOAT3 s);
	void SetObjectPosition(const XMFLOAT3 s);

	void SetLocalScale(const XMFLOAT2 s);
	void SetLocalScale(const FLOAT s);
	void SetLocalRotation(const XMFLOAT3 s);
	void SetLocalPosition(const XMFLOAT3 s);
	void MoveLocalPosition(float x, float y, float z);

	void SetUvScale(const XMFLOAT2 s);
	void SetUvRotation(const XMFLOAT3 s);
	void SetUvPosition(const XMFLOAT2 s);

	void SetAlignX(AlignModeX m);
	void SetAlignY(AlignModeY m);

	const XMFLOAT3& GetCenterPosition() const { return CenterPosition; }

	const XMFLOAT2& GetObjectScale() const { return ObjectScale; }
	const XMFLOAT3& GetObjectPosition() const { return ObjectPosition; }

	const XMFLOAT2& GetLocalScale() const { return LocalScale; }
	const XMFLOAT3& GetLocalPosition() const { return LocalPosition; }

	const XMFLOAT2& GetUvScale() const { return UvScale; }
	const XMFLOAT2& GetUvPosition() const { return UvPosition; }

	const AlignModeX& GetAlignX() const { return alignX; }
	const AlignModeY& GetAlignY() const { return alignY; }

	const XMFLOAT4X4& GetObjectWorld();
	const XMFLOAT4X4& GetLocalWorld();
	const XMFLOAT4X4& GetGlobalWorld();
	const XMFLOAT4X4& GetTotalDrawWorld();
	const XMFLOAT4X4& GetUvWorld();

	inline World3D* GetParentWorld() const { return mParentWorld; }
	void SetParentWorld(World3D* p);
	void SetParentDrawWorld();
	void OnParentWorldUpdate();

private:
	XMFLOAT3 CenterPosition{};

	//variables to describe the shape
	XMFLOAT2 ObjectScale;
	XMFLOAT3 ObjectRotation{};
	XMFLOAT3 ObjectPosition{};
	XMFLOAT4X4 mObjectWorld;

	//relative world to the parent world
	XMFLOAT2 LocalScale{ 1.0f,1.0f };
	XMFLOAT3 LocalRotation{};
	XMFLOAT3 LocalPosition;
	XMFLOAT4X4 mLocalWorld;

	//total world
	XMFLOAT4X4 mGlobalWorld;
	XMFLOAT4X4 mTotalDrawWorld;

	XMFLOAT2 UvScale{ 1.0f,1.0f };
	XMFLOAT3 UvRotation{};
	XMFLOAT2 UvPosition{};
	XMFLOAT4X4 mUvWorld;

	void InitWorld();

	void UpdateObjectWorld();
	void UpdateLocalWorld();
	void UpdateGlobalWorld();
	void UpdateTotalDrawWorld();
	void UpdateUvWorld();

	bool mObjectWorldUpdateFlag = true;
	bool mLocalWorldUpdateFlag = true;
	bool mGlobalWorldUpdateFlag = true;
	bool mTotalDrawWorldUpdateFlag = true;
	bool mUvWorldUpdateFlag = true;

	World3D* mParentWorld = nullptr; //weak ptr, never delete this

	AlignModeX alignX = AlignModeX::Mid;
	AlignModeY alignY = AlignModeY::Mid;

};