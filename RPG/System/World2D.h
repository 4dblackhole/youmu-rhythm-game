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
	void Reposition(float newW, float newH);

	void SetScale(const D2D1_POINT_2F);
	void SetScale(const FLOAT);
	void SetRotation(const FLOAT);
	void SetPosition(const D2D1_POINT_2F);


	const D2D1_POINT_2F& GetScale() const { return this->scale; }
	const FLOAT& GetRotation() const { return this->rotation; }
	const D2D1_POINT_2F& GetPosition() const { return this->pos; }

	const D2D1_POINT_2F& GetDrawPos() const { return drawPos; }
	const D2D1_POINT_2F& GetDrawScale() const { return drawScale; }

	const D2D1::Matrix3x2F& GetWorld() const { return mWorld; }
	const D2D1::Matrix3x2F& GetGlobalWorld() const { return mGlobalWorld; }
	const D2D1::Matrix3x2F& GetParentWorld() const { return parentWorld; }

	void SetParentWorld(const D2D1::Matrix3x2F& p);
	void UpdateWorld(); //MUST CALL THIS FUNCTION AFTER CHANGE the Center/S.R.T
	void UpdateGlobalWorld(); 
	void UpdateChildWorld();

	AlignModeX alignX = AlignModeX::Mid;
	AlignModeY alignY = AlignModeY::Mid;

private:
	D2D1_POINT_2F scale;
	FLOAT rotation;
	D2D1_POINT_2F pos;

	D2D1_POINT_2F drawPos{ 0.0f,0.0f };
	D2D1_POINT_2F drawScale{ 1.0f,1.0f };

	D2D1::Matrix3x2F mWorld{ D2D1::Matrix3x2F::Identity() };
	D2D1::Matrix3x2F mGlobalWorld{ D2D1::Matrix3x2F::Identity() };
	D2D1::Matrix3x2F parentWorld{ D2D1::Matrix3x2F::Identity() };
	bool worldUpdateFlag = false;
	bool globalWorldUpdateFlag = false;
public:
	vector<World2D*> childWorlds;
};