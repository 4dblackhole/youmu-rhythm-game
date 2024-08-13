#include "framework.h"
#include "World2D.h"

World2D::World2D(D2D1_POINT_2F size, FLOAT rot, D2D1_POINT_2F position) 
	: scale(size), rotation(rot), pos(position)
{
}

World2D::~World2D()
{
}

void World2D::SetScale(const D2D1_POINT_2F s)
{
	this->scale	 = s;
	mLocalWorldUpdateFlag = true;
}

void World2D::SetScale(const FLOAT f)
{
	SetScale({ f,f });
}

void World2D::SetRotation(const FLOAT s)
{
	this->rotation = s;
	mLocalWorldUpdateFlag = true;
}

void World2D::SetPosition(const D2D1_POINT_2F s)
{
	this->pos = s;
	mLocalWorldUpdateFlag = true;
}

const D2D1::Matrix3x2F& World2D::GetGlobalWorld()
{
	UpdateGlobalWorld();
	return mGlobalWorld;
}

const D2D1::Matrix3x2F& World2D::GetLocalWorld()
{
	UpdateLocalWorld();
	return mLocalWorld;
}

void World2D::SetParentWorld(World2D* p)
{
	parentWorld = p;
	mGlobalWorldUpdateFlag = true;
}

void World2D::SetParentDrawWorld()
{
	SetParentWorld(&App->GetDrawWorld2D(GetAlignX()));
}

void World2D::OnParentWorldUpdate()
{
	mGlobalWorldUpdateFlag = true;
}

void World2D::UpdateLocalWorld()
{
	if (mLocalWorldUpdateFlag)
	{
		mLocalWorld = D2D1::Matrix3x2F::Scale({ scale.x,scale.y }) * D2D1::Matrix3x2F::Rotation(rotation) * D2D1::Matrix3x2F::Translation({ pos.x,pos.y });
		mGlobalWorldUpdateFlag = true;

		mLocalWorldUpdateFlag = false;
	}
}

void World2D::UpdateGlobalWorld()
{
	UpdateLocalWorld();
	if (mGlobalWorldUpdateFlag)
	{
		mGlobalWorld =
			(parentWorld == nullptr) ?
			mLocalWorld :
			mLocalWorld * (parentWorld->GetGlobalWorld());
		mGlobalWorldUpdateFlag = false;
	}
}

void World2D::SetAlignX(AlignModeX x)
{
	alignX = x;
	SetParentDrawWorld();
}