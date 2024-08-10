#pragma once
#include <d2d1.h>
#include "Shape2D.h"

class Triangle2D : public Shape2D
{
public:
	static constexpr D2D1_TRIANGLE DefaultTriangle{
		{ 1.0f, 0.0f },
		{ -0.5f,  0.86602540378443864676372317075294f},
		{ -0.5f, -0.86602540378443864676372317075294f} };

	Triangle2D();
	Triangle2D(const D2D1_TRIANGLE& pts);
	virtual ~Triangle2D();

	virtual void Draw();

private:
	ID2D1PathGeometry* geometry = nullptr;
};