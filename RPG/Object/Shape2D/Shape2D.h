#pragma once
#include <d2d1.h>
#include "System/World2D.h"

class Shape2D
{
public:
	Shape2D(bool isChild = false);
	virtual ~Shape2D();

	virtual void Draw();
	void Resize(float newW, float newH);

	World2D* const& GetWorld2dConst() const { return world2d; }
	World2D*& GetWorld2d() { return world2d; }

	D2D1::ColorF FillColor = { 1,1,1,1 };
	D2D1::ColorF BorderColor = { 1,1,1,1 };
	float BorderSize = 1.0f;

protected:
	void Reposition(float newW, float newH);
	World2D* world2d;

};