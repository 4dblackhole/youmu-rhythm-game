#include "framework.h"
#include "Triangle2D.h"

using namespace D2D1;

Triangle2D::Triangle2D() 
	: Triangle2D({
		{ 1.0f, 0.0f },
		{ -0.5f,  0.86602540378443864676372317075294f},
		{ -0.5f, -0.86602540378443864676372317075294f} })
{
}

Triangle2D::Triangle2D(const D2D1_TRIANGLE& pts)
{
	ID2D1GeometrySink* pSink = nullptr;
	mWorld = Matrix3x2F::Identity();

	D2D.GetD2DFactory()->CreatePathGeometry(&geometry);
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

	drawPos = { (float)Position.width,(float)Position.height };
}

Triangle2D::~Triangle2D()
{
	ReleaseCOM(geometry);
}

void Triangle2D::Draw()
{
	if (worldUpdateFlag == true)
	{
		UpdateWorld();
		worldUpdateFlag = false;
	}
	D2D.GetRenderTarget()->SetTransform(mWorld);
	D2D.GetRenderTarget()->DrawGeometry(geometry, D2D.D2D.GetSolidBrush(BorderColor), borderSize);
	D2D.GetRenderTarget()->FillGeometry(geometry, D2D.D2D.GetSolidBrush(FillColor));
}

void Triangle2D::Repositioning(float newW, float newH)
{
	const float rateY = newH / (float)StandardHeight;
	const float centerWidth = newW * 0.5f;
	switch (alignX)
	{
	case AlignModeX::Left:
		drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, Position.width, Position.height);
		break;
	case AlignModeX::Mid:
		drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, Position.width, Position.height, centerWidth);
		break;
	case AlignModeX::Right:
		drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, Position.width, Position.height, newW - Scale.width * rateY);
		break;
	}
}

void Triangle2D::Resize(float newW, float newH)
{
	drawScale = newH / (float)StandardHeight;;

	Repositioning(newW, newH);
	worldUpdateFlag = true;
}

void Triangle2D::SetCenter(const D2D1_POINT_2F s)
{
	memcpy_s(&this->Center, sizeof(D2D1_POINT_2F), &s, sizeof(D2D1_POINT_2F));
	worldUpdateFlag = true;
}

void Triangle2D::SetScale(const D2D1_SIZE_F s)
{
	this->Scale = s;
	worldUpdateFlag = true;
}

void Triangle2D::SetRotation(const FLOAT s)
{
	this->Rotation = s;
	worldUpdateFlag = true;
}

void Triangle2D::SetPosition(const D2D1_SIZE_F s)
{
	this->Position = s;
	Repositioning((float)App->GetWidth(), (float)App->GetHeight());
	worldUpdateFlag = true;
}

void Triangle2D::UpdateWorld()
{
	D2D1_SIZE_F totalScale = { Scale.width * drawScale, Scale.height * drawScale };
	mWorld = Matrix3x2F::Scale(totalScale, Center) * D2D1::Matrix3x2F::Rotation(Rotation, Center) * D2D1::Matrix3x2F::Translation({ drawPos.x,drawPos.y });
}
