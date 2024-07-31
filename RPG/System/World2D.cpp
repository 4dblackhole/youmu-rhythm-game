#include "framework.h"
#include "World2D.h"

World2D::World2D(D2D1_POINT_2F size, FLOAT rot, D2D1_POINT_2F position) 
	: localScale(size), localRotation(rot), localPos(position)
{
	UpdateLocalWorld();
	Resize((float)App->GetWidth(), (float)App->GetHeight());
}

World2D::~World2D()
{
}

void World2D::Resize(float newW, float newH)
{
	Rescale(newW, newH);
	Reposition(newW, newH);

	UpdateRelativeWorld();
	UpdateGlobalWorld();
	UpdateDrawWorld();
}

void World2D::Rescale(float newW, float newH)
{
	drawScale.x = relativeScale.x * App->RateY();
	drawScale.y = relativeScale.y * App->RateY();
}

void World2D::Reposition(float newW, float newH)
{
	switch (alignX)
	{
	case AlignModeX::Left:
		drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, relativePos.x, relativePos.y);
		break;
	case AlignModeX::Mid:
		drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, relativePos.x, relativePos.y, newW * 0.5f);
		break;
	case AlignModeX::Right:
		drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, relativePos.x, relativePos.y, newW);
		break;
	}
}

void World2D::SetLocalScale(const D2D1_POINT_2F s)
{
	this->localScale = s;
}

void World2D::SetLocalScale(const FLOAT f)
{
	SetLocalScale({ f,f });
}

void World2D::SetLocalRotation(const FLOAT s)
{
	this->localRotation = s;
}

void World2D::SetLocalPosition(const D2D1_POINT_2F s)
{
	this->localPos = s;
}

void World2D::SetRelativeScale(const D2D1_POINT_2F s)
{
	this->relativeScale = s;
}

void World2D::SetRelativeScale(const FLOAT f)
{
	SetRelativeScale({ f,f });
}

void World2D::SetRelativeRotation(const FLOAT s)
{
	this->relativeRotation = s;
}

void World2D::SetRelativePosition(const D2D1_POINT_2F s)
{
	this->relativePos = s;
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

void World2D::UpdateRelativeWorld() // MUST CALL UpdateGlobalWorld
{
	mRelativeWorld = D2D1::Matrix3x2F::Scale({ drawScale.x,drawScale.y }) * D2D1::Matrix3x2F::Rotation(relativeRotation) * D2D1::Matrix3x2F::Translation({ drawPos.x,drawPos.y });
}

void World2D::UpdateLocalWorld()
{
	mLocalWorld = D2D1::Matrix3x2F::Scale({ localScale.x,localScale.y }) * D2D1::Matrix3x2F::Rotation(localRotation) * D2D1::Matrix3x2F::Translation({ localPos.x,localPos.y });
}

void World2D::UpdateGlobalWorld()
{
	mGlobalWorld = 
		(parentWorld == nullptr) ? 
		mRelativeWorld : 
		mRelativeWorld * (*parentWorld);

}

void World2D::UpdateDrawWorld()
{
	mDrawWorld = mLocalWorld * mGlobalWorld;
}
