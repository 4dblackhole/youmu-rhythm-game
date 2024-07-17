#include "framework.h"
#include "World2D.h"

World2D::World2D(D2D1_POINT_2F size, FLOAT rot, D2D1_POINT_2F position) 
	: scale(size), rotation(rot), pos(position)
{
	Resize(App->GetWidth(), App->GetHeight());
}

World2D::~World2D()
{
}

void World2D::Resize(float newW, float newH)
{
	const float rateY = newH / (float)StandardHeight;
	drawScale.x = scale.x * rateY;
	drawScale.y = scale.y * rateY;

	Reposition(newW, newH);
	worldUpdateFlag = true;
}

void World2D::Reposition(float newW, float newH)
{
	switch (alignX)
	{
	case AlignModeX::Left:
		drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, pos.x, pos.y);
		break;
	case AlignModeX::Mid:
		drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, pos.x, pos.y, (float)App->GetWidth() * 0.5f);
		break;
	case AlignModeX::Right:
		drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, pos.x, pos.y, (float)App->GetWidth());
		break;
	}
}

void World2D::SetScale(const D2D1_POINT_2F s)
{
	this->scale = s;
	worldUpdateFlag = true;
}

void World2D::SetScale(const FLOAT f)
{
	SetScale({ f,f });
}

void World2D::SetRotation(const FLOAT s)
{
	this->rotation = s;
	worldUpdateFlag = true;
}

void World2D::SetPosition(const D2D1_POINT_2F s)
{
	this->pos = s;
	Reposition((float)App->GetWidth(), (float)App->GetHeight());
	worldUpdateFlag = true;
}

void World2D::UpdateWorld()
{
	if (!worldUpdateFlag) return;
	mWorld = D2D1::Matrix3x2F::Scale({ drawScale.x,drawScale.y }) * D2D1::Matrix3x2F::Rotation(rotation) * D2D1::Matrix3x2F::Translation({ drawPos.x,drawPos.y });
	worldUpdateFlag = false;
}
