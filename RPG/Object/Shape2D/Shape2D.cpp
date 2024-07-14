#include "framework.h"
#include "Shape2D.h"

using namespace D2D1;

Shape2D::Shape2D()
{
}

Shape2D::~Shape2D()
{
	ReleaseCOM(geometry);
}

void Shape2D::Draw()
{
	if (worldUpdateFlag == true)
	{
		UpdateWorld();
		worldUpdateFlag = false;
	}
	D2D.GetRenderTarget()->SetTransform(mWorld);
	/*
	D2D.GetRenderTarget()->DrawGeometry(geometry, D2D.D2D.GetSolidBrush(BorderColor), borderSize);
	D2D.GetRenderTarget()->FillGeometry(geometry, D2D.D2D.GetSolidBrush(FillColor));
	*/
}

void Shape2D::Repositioning(float newW, float newH)
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

void Shape2D::Resize(float newW, float newH)
{
	drawScale = newH / (float)StandardHeight;;

	Repositioning(newW, newH);
	worldUpdateFlag = true;
}

void Shape2D::SetCenter(const D2D1_POINT_2F s)
{
	memcpy_s(&this->Center, sizeof(D2D1_POINT_2F), &s, sizeof(D2D1_POINT_2F));
	worldUpdateFlag = true;
}

void Shape2D::SetScale(const D2D1_SIZE_F s)
{
	this->Scale = s;
	worldUpdateFlag = true;
}

void Shape2D::SetRotation(const FLOAT s)
{
	this->Rotation = s;
	worldUpdateFlag = true;
}

void Shape2D::SetPosition(const D2D1_SIZE_F s)
{
	this->Position = s;
	Repositioning((float)App->GetWidth(), (float)App->GetHeight());
	worldUpdateFlag = true;
}

void Shape2D::UpdateWorld()
{
	D2D1_SIZE_F totalScale = { Scale.width * drawScale, Scale.height * drawScale };
	mWorld = Matrix3x2F::Scale(totalScale, Center) * D2D1::Matrix3x2F::Rotation(Rotation, Center) * D2D1::Matrix3x2F::Translation({ drawPos.x,drawPos.y });
}
