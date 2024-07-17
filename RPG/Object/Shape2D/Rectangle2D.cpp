#include "framework.h"
#include "Rectangle2D.h"

using namespace D2D1;

Rectangle2D::Rectangle2D()
	: Rectangle2D({ -0.5f,-0.5f,0.5f,0.5f })
{
}

Rectangle2D::Rectangle2D(const D2D1_RECT_F& pts)
	: Shape2D(), rect({ pts, 0, 0 })
{
}

Rectangle2D::~Rectangle2D()
{

}

/*
ComPtr<ID2D1StrokeStyle> CreateStrokeStyle()
{
	ComPtr<ID2D1StrokeStyle> strokeStyle;

	HR(D2Ddevice::GetInstance().GetD2DFactory()->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND,
			D2D1_LINE_JOIN_ROUND,
			0.0f,
			D2D1_DASH_STYLE_SOLID,
			0.0f),
		nullptr,
		0,
		strokeStyle.GetAddressOf()
	));

	return strokeStyle;
}
ComPtr<ID2D1StrokeStyle> stroke = CreateStrokeStyle();
*/

void Rectangle2D::Draw()
{
	__super::Draw();
	if(IsRound)D2D.GetRenderTarget()->DrawRoundedRectangle(rect, D2D.GetSolidBrush(BorderColor), BorderSize);
	else D2D.GetRenderTarget()->DrawRectangle(rect.rect, D2D.GetSolidBrush(BorderColor), BorderSize);
	//D2D.GetRenderTarget()->FillGeometry(geometry, D2D.D2D.GetSolidBrush(FillColor));
}

void Rectangle2D::SetRadius(FLOAT f)
{
	SetRadius(f, f);
}

void Rectangle2D::SetRadius(FLOAT f1, FLOAT f2)
{
	rect.radiusX = f1, rect.radiusY = f2;
}
