#include "framework.h"
#include "Triangle2D.h"

using namespace D2D1;

Triangle2D::Triangle2D() 
	: Triangle2D(DefaultTriangle)
{
}

Triangle2D::Triangle2D(const D2D1_TRIANGLE& pts) : Shape2D()
{
	D2D.GetD2DFactory()->CreatePathGeometry(&geometry);
	ID2D1GeometrySink* pSink = nullptr;
	geometry->Open(&pSink);

	pSink->BeginFigure(
		pts.point1,
		D2D1_FIGURE_BEGIN_FILLED
	);

	pSink->AddLine(pts.point2);
	pSink->AddLine(pts.point3);
	pSink->AddLine(pts.point1);
	pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

	pSink->Close();
	ReleaseCOM(pSink);
}

Triangle2D::~Triangle2D()
{
	ReleaseCOM(geometry);
}

void Triangle2D::Draw()
{
	__super::Draw();

	D2D.GetRenderTarget()->DrawGeometry(geometry, D2D.D2D.GetSolidBrush(BorderColor), BorderSize);
	D2D.GetRenderTarget()->FillGeometry(geometry, D2D.D2D.GetSolidBrush(FillColor));
}
