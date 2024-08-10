#include "framework.h"
#include "Rectangle2D.h"

using namespace D2D1;

Rectangle2D::Rectangle2D()
	: Rectangle2D(DefaultRect)
{
}

Rectangle2D::Rectangle2D(const D2D1_RECT_F& pts)
	: Shape2D(), rect({ pts, 0, 0 })
{
}

Rectangle2D::~Rectangle2D()
{

}

void Rectangle2D::Draw()
{
	__super::Draw();
	if(IsRound)D2D.GetRenderTarget()->DrawRoundedRectangle(rect, D2D.GetSolidBrush(BorderColor), BorderSize);
	else D2D.GetRenderTarget()->DrawRectangle(rect.rect, D2D.GetSolidBrush(BorderColor), BorderSize);
}

void Rectangle2D::SetRadius(FLOAT f)
{
	SetRadius(f, f);
}

void Rectangle2D::SetRadius(FLOAT f1, FLOAT f2)
{
	rect.radiusX = f1, rect.radiusY = f2;
}
