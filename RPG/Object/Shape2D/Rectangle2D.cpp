#include "framework.h"
#include "Rectangle2D.h"

using namespace D2D1;

Rectangle2D::Rectangle2D()
	: Rectangle2D({ -0.5f,-0.5f,0.5f,0.5f })
{
}

Rectangle2D::Rectangle2D(const D2D1_RECT_F& pts) : Shape2D()
{
	D2D.GetD2DFactory()->CreatePathGeometry(&geometry);
	ID2D1GeometrySink* pSink = nullptr;
	geometry->Open(&pSink);

	const D2D1_POINT_2F ptLT{ pts.left,pts.top };
	const D2D1_POINT_2F ptRT{ pts.right,pts.top };
	const D2D1_POINT_2F ptLB{ pts.left,pts.bottom};
	const D2D1_POINT_2F ptRB{ pts.right,pts.bottom};

	pSink->BeginFigure(ptLT, D2D1_FIGURE_BEGIN_FILLED);

	pSink->AddLine(ptRT);
	pSink->AddLine(ptRB);
	pSink->AddLine(ptLB);
	pSink->AddLine(ptLT);

	pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

	pSink->Close();
	ReleaseCOM(pSink);
}

Rectangle2D::~Rectangle2D()
{

}

void Rectangle2D::Draw()
{
	__super::Draw();
	D2D.GetRenderTarget()->DrawGeometry(geometry, D2D.D2D.GetSolidBrush(BorderColor), BorderSize);
	//D2D.GetRenderTarget()->FillGeometry(geometry, D2D.D2D.GetSolidBrush(FillColor));
}