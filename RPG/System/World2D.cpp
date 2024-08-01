#include "framework.h"
#include "World2D.h"

World2D::World2D(D2D1_POINT_2F size, FLOAT rot, D2D1_POINT_2F position) 
	: scale(size), rotation(rot), pos(position)
{
}

World2D::~World2D()
{
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
	drawScale.x = App->RateY();
	drawScale.y = App->RateY();
	UpdateDrawWorld();
}

void World2D::Reposition(float newW, float newH)
{
	switch (alignX)
	{
	case AlignModeX::Left:
		//drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, pos.x, pos.y);
		drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, 0, 0);
		break;
	case AlignModeX::Mid:
		//drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, pos.x, pos.y, newW * 0.5f);
		drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, 0, 0, newW * 0.5f);
		break;
	case AlignModeX::Right:
		//drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, pos.x, pos.y, newW);
		drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, 0, 0, newW);
		break;
	}
	UpdateDrawWorld();
	int asdf = 3;
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
	//Reposition((float)App->GetWidth(), (float)App->GetHeight());
}

void World2D::SetParentWorld(const D2D1::Matrix3x2F* p) // MUST CALL UpdateGlobalWorld
{
	parentWorld = p;
}

void World2D::UpdateWorld() // MUST CALL UpdateGlobalWorld
{
	mLocalWorld = D2D1::Matrix3x2F::Scale({ scale.x,scale.y }) * D2D1::Matrix3x2F::Rotation(rotation) * D2D1::Matrix3x2F::Translation({ pos.x,pos.y });
}

void World2D::UpdateDrawWorld()
{
	mDrawWorld = D2D1::Matrix3x2F::Scale({ drawScale.x,drawScale.y }) * D2D1::Matrix3x2F::Translation({ drawPos.x,drawPos.y });
	UpdateTotalDrawWorld();
}

void World2D::UpdateGlobalWorld()
{
	mGlobalWorld = 
		(parentWorld == nullptr) ? 
		mLocalWorld :
		mLocalWorld * (*parentWorld);

	UpdateTotalDrawWorld();

}

void World2D::UpdateTotalDrawWorld()
{
	mTotalDrawWorld = mGlobalWorld * mDrawWorld;
}
