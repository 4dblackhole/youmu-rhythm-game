#include "framework.h"
#include "Shape2D.h"

using namespace D2D1;

Shape2D::Shape2D()
{
	mWorld = Matrix3x2F::Identity();
	drawPos = { (float)position.width,(float)position.height };
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
}

void Shape2D::Repositioning(float newW, float newH)
{
	const float rateY = newH / (float)StandardHeight;
	const float centerWidth = newW * 0.5f;
	switch (alignX)
	{
	case AlignModeX::Left:
		drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, position.width, position.height);
		break;
	case AlignModeX::Mid:
		drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, position.width, position.height, centerWidth);
		break;
	case AlignModeX::Right:
		drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, position.width, position.height, newW - scale.width * rateY);
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
	memcpy_s(&this->centerPos, sizeof(D2D1_POINT_2F), &s, sizeof(D2D1_POINT_2F));
	worldUpdateFlag = true;
}

void Shape2D::SetScale(const D2D1_SIZE_F s)
{
	this->scale = s;
	worldUpdateFlag = true;
}

void Shape2D::SetRotation(const FLOAT s)
{
	this->rotation = s;
	worldUpdateFlag = true;
}

void Shape2D::SetPosition(const D2D1_SIZE_F s)
{
	this->position = s;
	Repositioning((float)App->GetWidth(), (float)App->GetHeight());
	worldUpdateFlag = true;
}

void Shape2D::UpdateWorld()
{
	D2D1_SIZE_F totalScale = { scale.width * drawScale, scale.height * drawScale };
	mWorld = Matrix3x2F::Scale(totalScale, centerPos) * D2D1::Matrix3x2F::Rotation(rotation, centerPos) * D2D1::Matrix3x2F::Translation({ drawPos.x,drawPos.y });
}
