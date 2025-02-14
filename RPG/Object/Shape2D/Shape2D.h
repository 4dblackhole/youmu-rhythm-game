#pragma once
#include <d2d1.h>
#include "System/World/World2D.h"

class Shape2D
{
public:
	Shape2D();
	virtual ~Shape2D();

	virtual void Draw();

	void OnResize();

	const World2D& GetWorld2dConst() const { return world2d; }
	World2D& GetWorld2d() { return world2d; }

	D2D1::ColorF FillColor{ D2D1::ColorF::Black };
	D2D1::ColorF BorderColor{ D2D1::ColorF::White };
	float BorderSize = 1.0f;
	bool isHollow = false;

protected:
	World2D world2d;

};