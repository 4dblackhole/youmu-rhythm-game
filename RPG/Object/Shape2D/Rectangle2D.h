#pragma once
#include <d2d1.h>
#include "Shape2D.h"

class Rectangle2D : public Shape2D
{
public:
	Rectangle2D();
	Rectangle2D(const D2D1_RECT_F& pts);
	virtual ~Rectangle2D();

	virtual void Draw();
};