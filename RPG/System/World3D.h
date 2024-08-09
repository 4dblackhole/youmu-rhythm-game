#pragma once
#include "framework.h"
#include "AlignMode.h"

class World3D
{
public:
	World3D() : World3D({ 1.0f,1.0f,1.0f }, {}, {}) {}
	World3D(XMFLOAT3 size, XMFLOAT3 rot, XMFLOAT3 position);
	~World3D();

	void SetScale(const XMFLOAT3);
	void SetScale(const FLOAT);
	void SetRotation(const XMFLOAT3);
	void SetPosition(const XMFLOAT3);

	const XMFLOAT3& GetScale() { return this->scale; }
	const XMFLOAT3& GetRotation() { return this->rotation; }
	const XMFLOAT3& GetPosition() { return this->pos; }

	const XMFLOAT4X4& GetLocalWorld();
	const XMFLOAT4X4& GetGlobalWorld();
	
	inline const XMFLOAT4X4* GetParentWorld() const { return parentWorld; }

	void SetParentWorld(const XMFLOAT4X4* p);
	void ParentWorldUpdate();

	void SetAlignX(AlignModeX);
	const AlignModeX& GetAlignX() const { return alignX; }

private:
	void UpdateLocalWorld();
	void UpdateGlobalWorld();

	AlignModeX alignX = AlignModeX::Mid;
	AlignModeY alignY = AlignModeY::Mid;

	XMFLOAT3 scale;
	XMFLOAT3 rotation;
	XMFLOAT3 pos;

	XMFLOAT4X4 mLocalWorld{ XmFloatT4X4Identity };
	XMFLOAT4X4 mGlobalWorld{ XmFloatT4X4Identity };

	bool mLocalWorldUpdateFlag = true;
	bool mGlobalWorldUpdateFlag = true;

	const XMFLOAT4X4* parentWorld = nullptr;
	bool parentWorldUpdateFlag = true;

};