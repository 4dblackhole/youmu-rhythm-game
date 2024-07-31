#include "framework.h"
#include "World2D.h"

World2D::World2D(D2D1_POINT_2F size, FLOAT rot, D2D1_POINT_2F position) 
	: scale(size), rotation(rot), pos(position)
{
	Init();
}

World2D::~World2D()
{
}

void World2D::Init()
{
	Resize((float)App->GetWidth(), (float)App->GetHeight());
}

void World2D::Resize(float newW, float newH)
{
	Rescale(newW, newH);
	Reposition(newW, newH);

	UpdateWorld();
	UpdateGlobalWorld();
}

void World2D::Rescale(float newW, float newH)
{
	drawScale.x = scale.x*App->RateY();
	drawScale.y = scale.y*App->RateY();
}

void World2D::Reposition(float newW, float newH)
{
	switch (alignX)
	{
	case AlignModeX::Left:
		drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, pos.x, pos.y);
		break;
	case AlignModeX::Mid:
		drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, pos.x, pos.y, newW * 0.5f);
		break;
	case AlignModeX::Right:
		drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, pos.x, pos.y, newW);
		break;
	}
}

void World2D::SetScale(const D2D1_POINT_2F s)
{
	this->scale	 = s;
}

void World2D::SetScale(const FLOAT f)
{
	SetScale({ f,f });
}

void World2D::SetRotation(const FLOAT s)
{
	this->rotation = s;
}

void World2D::SetPosition(const D2D1_POINT_2F s)
{
	this->pos = s;
	Reposition((float)App->GetWidth(), (float)App->GetHeight());
}

/*
Update World 式式式成式> UpdateGlobalWorld 式式成式式> UpdateDrawWorld
				弛						弛
SetParentWorld 式戎						弛
										弛
Update Local World 式式式式式式式式式式式式式式式式式式式式式戎
*/

void World2D::SetParentWorld(const D2D1::Matrix3x2F* p) // MUST CALL UpdateGlobalWorld
{
	parentWorld = p;
}

void World2D::UpdateWorld() // MUST CALL UpdateGlobalWorld
{
	mLocalWorld = D2D1::Matrix3x2F::Scale({ drawScale.x,drawScale.y }) * D2D1::Matrix3x2F::Rotation(rotation) * D2D1::Matrix3x2F::Translation({ drawPos.x,drawPos.y });
}

void World2D::UpdateGlobalWorld()
{
	mGlobalWorld = 
		(parentWorld == nullptr) ? 
		mLocalWorld :
		mLocalWorld * (*parentWorld);

}