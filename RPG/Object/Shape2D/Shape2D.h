#pragma once
#include <d2d1.h>
#include "System/AlignMode.h"

class Shape2D
{
public:
	Shape2D();
	virtual ~Shape2D();

	virtual void Draw();
	void Resize(float newW, float newH);

	void SetCenter(const D2D1_POINT_2F);
	void SetScale(const D2D1_SIZE_F);
	void SetRotation(const FLOAT);
	void SetPosition(const D2D1_SIZE_F);

	const D2D1_POINT_2F& GetCenter() const { return this->centerPos; }
	const D2D1_SIZE_F& GetScale() const { return this->scale; }
	const FLOAT& GetRotation() const { return this->rotation; }
	const D2D1_SIZE_F& GetPosition() const { return this->position; }

	D2D1::ColorF FillColor = { 1,1,1,1 };
	D2D1::ColorF BorderColor = { 1,1,1,1 };
	float BorderSize = 0.5f;

protected:
	void Repositioning(float newW, float newH);

	D2D1_POINT_2F centerPos = { 0.0f, 0.0f };
	D2D1_SIZE_F scale = { 1.0f, 1.0f };
	FLOAT rotation = 0.0f;
	D2D1_SIZE_F position = { 0.0f, 0.0f };

	bool worldUpdateFlag = false;
	void UpdateWorld(); //MUST CALL THIS FUNCTION AFTER CHANGE the Center/S.R.T
	D2D1::Matrix3x2F mWorld;

	FLOAT drawScale = 1.0f;
	D2D1_POINT_2F drawPos = { 0.0f, 0.0f };

	ID2D1PathGeometry* geometry = nullptr;

	AlignModeX alignX = AlignModeX::Mid;
};