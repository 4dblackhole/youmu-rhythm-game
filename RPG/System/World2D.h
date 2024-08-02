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

	void Resize(float newW, float newH);
	void Rescale(float newW, float newH);
	void Reposition(float newW, float newH);

	void SetScale(const D2D1_POINT_2F);
	void SetScale(const FLOAT);
	void SetRotation(const FLOAT);
	void SetPosition(const D2D1_POINT_2F);

	const D2D1_POINT_2F& GetScale() { return this->scale; }
	const FLOAT& GetRotation() { return this->rotation; }
	const D2D1_POINT_2F& GetPosition() { return this->pos; }

	const D2D1_POINT_2F& GetDrawPos() { return drawPos; }
	const D2D1_POINT_2F& GetDrawScale() { return drawScale; }

	const D2D1::Matrix3x2F& GetLocalWorld();
	const D2D1::Matrix3x2F& GetDrawWorld();
	const D2D1::Matrix3x2F& GetGlobalWorld();
	const D2D1::Matrix3x2F& GetTotalDrawWorld();
	
	inline const D2D1::Matrix3x2F* GetParentWorld() const { return parentWorld; }

	void SetParentWorld(const D2D1::Matrix3x2F* p);
	void ParentWorldUpdated();

	void SetAlignMode(AlignModeX, AlignModeY);
	void SetAlignX(AlignModeX);
	void SetAlignY(AlignModeY);
	const AlignModeX& GetAlignX() const { return alignX; }
	const AlignModeY& GetAlignY() const { return alignY; }

private:
	void UpdateLocalWorld();
	void UpdateDrawWorld();
	void UpdateGlobalWorld();
	void UpdateTotalDrawWorld();

	AlignModeX alignX = AlignModeX::Left;
	AlignModeY alignY = AlignModeY::Top;

	D2D1_POINT_2F scale;
	FLOAT rotation;
	D2D1_POINT_2F pos;

	D2D1_POINT_2F drawPos{ 0.0f,0.0f };
	D2D1_POINT_2F drawScale{ 1.0f,1.0f };

	D2D1::Matrix3x2F mLocalWorld{ D2D1::Matrix3x2F::Identity() };
	D2D1::Matrix3x2F mDrawWorld{ D2D1::Matrix3x2F::Identity() };
	D2D1::Matrix3x2F mGlobalWorld{ D2D1::Matrix3x2F::Identity() };
	D2D1::Matrix3x2F mTotalDrawWorld{ D2D1::Matrix3x2F::Identity() };

	const D2D1::Matrix3x2F* parentWorld = nullptr;

	bool parentWorldUpdateFlag = true;

	bool mLocalWorldUpdateFlag = true;
	bool mDrawWorldUpdateFlag = true;
	bool mGlobalWorldUpdateFlag = true;
	bool mTotalDrawWorldUpdateFlag = true;

};