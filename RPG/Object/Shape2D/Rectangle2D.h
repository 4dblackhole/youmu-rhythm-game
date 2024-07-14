#pragma once
#include <d2d1.h>
#include "AlignMode.h"

class Rectangle2D
{
public:
	Rectangle2D();
	Rectangle2D(const D2D1_TRIANGLE& pts);
	~Rectangle2D();

	void Draw();
	void Resize(float newW, float newH);

	void SetCenter(const D2D1_POINT_2F);
	void SetScale(const D2D1_SIZE_F);
	void SetRotation(const FLOAT);
	void SetPosition(const D2D1_SIZE_F);

	const D2D1_POINT_2F& GetCenter() const { return this->Center; }
	const D2D1_SIZE_F& GetScale() const { return this->Scale; }
	const FLOAT& GetRotation() const { return this->Rotation; }
	const D2D1_SIZE_F& GetPosition() const { return this->Position; }

	D2D1::ColorF FillColor = { 1,1,1,1 };
	D2D1::ColorF BorderColor = { 1,1,1,1 };
	float borderSize = 0.5f;

private:
	void Repositioning(float newW, float newH);

	D2D1_POINT_2F Center = { 0.0f, 0.0f };
	D2D1_SIZE_F Scale = { 1.0f, 1.0f };
	FLOAT Rotation = 0.0f;
	D2D1_SIZE_F Position = { 0.0f, 0.0f };

	bool worldUpdateFlag = false;
	void UpdateWorld(); //MUST CALL THIS FUNCTION AFTER CHANGE the Center/S.R.T
	D2D1::Matrix3x2F mWorld;

	FLOAT drawScale = 1.0f;
	D2D1_POINT_2F drawPos = { 0.0f, 0.0f };

	ID2D1PathGeometry* geometry = nullptr;

	AlignModeX alignX = AlignModeX::Mid;
};