#pragma once
#include <d2d1.h>
#include "Shape2D.h"

class Rectangle2D : public Shape2D
{
public:
	static constexpr D2D1_RECT_F DefaultRect{ -0.5,-0.5f,0.5f,0.5f };

	Rectangle2D();
	Rectangle2D(const D2D1_RECT_F& pts);
	virtual ~Rectangle2D();

	virtual void Draw();

	const D2D1_RECT_F& GetArea() const { return rect.rect; }
	D2D1_ROUNDED_RECT& Rect() { return rect; }

	void SetRadius(FLOAT f);
	void SetRadius(FLOAT f1 , FLOAT f2);

	bool IsRound = false;
private:
	D2D1_ROUNDED_RECT rect;
};