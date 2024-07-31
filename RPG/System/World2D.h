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

	void SetLocalScale(const D2D1_POINT_2F);
	void SetLocalScale(const FLOAT);
	void SetLocalRotation(const FLOAT);
	void SetLocalPosition(const D2D1_POINT_2F);

	void SetRelativeScale(const D2D1_POINT_2F);
	void SetRelativeScale(const FLOAT);
	void SetRelativeRotation(const FLOAT);
	void SetRelativePosition(const D2D1_POINT_2F);


	const D2D1_POINT_2F& GetScale() const { return this->relativeScale; }
	const FLOAT& GetRotation() const { return this->relativeRotation; }
	const D2D1_POINT_2F& GetPosition() const { return this->relativePos; }

	const D2D1_POINT_2F& GetLocalScale() const { return this->localScale; }
	const FLOAT& GetLocalRotation() const { return this->localRotation; }
	const D2D1_POINT_2F& GetLocalPosition() const { return this->localPos; }

	const D2D1_POINT_2F& GetDrawPos() const { return drawPos; }
	const D2D1_POINT_2F& GetDrawScale() const { return drawScale; }

	const D2D1::Matrix3x2F& GetWorld() const { return mRelativeWorld; }
	const D2D1::Matrix3x2F& GetGlobalWorld() const { return mGlobalWorld; }
	const D2D1::Matrix3x2F& GetDrawWorld() const { return mDrawWorld; }
	const D2D1::Matrix3x2F* GetParentWorld() const { return parentWorld; }

	void SetParentWorld(const D2D1::Matrix3x2F* p);
	void UpdateRelativeWorld(); //MUST CALL THIS FUNCTION AFTER CHANGE the Center/S.R.T
	void UpdateLocalWorld(); //MUST CALL THIS FUNCTION AFTER CHANGE the Center/S.R.T
	void UpdateGlobalWorld(); 
	void UpdateDrawWorld();

	AlignModeX alignX = AlignModeX::Mid;
	AlignModeY alignY = AlignModeY::Mid;

private:
	D2D1_POINT_2F localScale;
	FLOAT localRotation;
	D2D1_POINT_2F localPos;

	D2D1_POINT_2F relativeScale{ 1.0f,1.0f };
	FLOAT relativeRotation = 0.0f;
	D2D1_POINT_2F relativePos{0.0f,0.0f};

	D2D1_POINT_2F drawPos{ 0.0f,0.0f };
	D2D1_POINT_2F drawScale{ 1.0f,1.0f };

	D2D1::Matrix3x2F mRelativeWorld{ D2D1::Matrix3x2F::Identity() };
	D2D1::Matrix3x2F mLocalWorld{ D2D1::Matrix3x2F::Identity() };
	D2D1::Matrix3x2F mGlobalWorld{ D2D1::Matrix3x2F::Identity() };
	D2D1::Matrix3x2F mDrawWorld{ D2D1::Matrix3x2F::Identity() };

	const D2D1::Matrix3x2F* parentWorld = nullptr;
	bool worldUpdateFlag = false;
	bool globalWorldUpdateFlag = false;

};