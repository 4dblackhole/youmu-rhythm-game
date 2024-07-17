#pragma once
#include <d2d1.h>
#include "Shape2D.h"

class Triangle2D : public Shape2D
{
public:
	Triangle2D();
	Triangle2D(const D2D1_TRIANGLE& pts);
	virtual ~Triangle2D();

	virtual void Draw();

private:
	ID2D1PathGeometry* geometry = nullptr;
};