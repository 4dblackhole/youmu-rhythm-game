#pragma once
#include "framework.h"
#include "AlignMode.h"
#include <D2D1.h>

class World2D
{
public:
	World2D() : World2D({ 1.0f,1.0f }, 0.0f, { 0.0f,0.0f }) {}
	World2D(D2D1_POINT_2F size, FLOAT rot, D2D1_POINT_2F position);
	~World2D();

	void SetScale(const D2D1_POINT_2F);
	void SetScale(const FLOAT);
	void SetRotation(const FLOAT);
	void SetPosition(const D2D1_POINT_2F);

	const D2D1_POINT_2F& GetScale() { return this->scale; }
	const FLOAT& GetRotation() { return this->rotation; }
	const D2D1_POINT_2F& GetPosition() { return this->pos; }

	const D2D1::Matrix3x2F& GetLocalWorld();
	const D2D1::Matrix3x2F& GetGlobalWorld();
	
	inline const D2D1::Matrix3x2F* GetParentWorld() const { return parentWorld; }

	void SetParentWorld(const D2D1::Matrix3x2F* p);
	void ParentWorldUpdate();

	void SetAlignX(AlignModeX);
	const AlignModeX& GetAlignX() const { return alignX; }

private:
	void UpdateLocalWorld();
	void UpdateGlobalWorld();

	AlignModeX alignX = AlignModeX::Left;

	D2D1_POINT_2F scale;
	FLOAT rotation;
	D2D1_POINT_2F pos;

	D2D1::Matrix3x2F mLocalWorld{ D2D1::Matrix3x2F::Identity() };
	D2D1::Matrix3x2F mGlobalWorld{ D2D1::Matrix3x2F::Identity() };

	bool mLocalWorldUpdateFlag = true;
	bool mGlobalWorldUpdateFlag = true;

	const D2D1::Matrix3x2F* parentWorld = nullptr;
	bool parentWorldUpdateFlag = true;

};