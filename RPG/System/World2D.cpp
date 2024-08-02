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

	//UpdateWorld();
	//UpdateGlobalWorld();
}

void World2D::Rescale(float newW, float newH)
{
	drawScale.x = App->RateY();
	drawScale.y = App->RateY();
	mDrawWorldUpdateFlag = true;
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
	mDrawWorldUpdateFlag = true;
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

const D2D1::Matrix3x2F& World2D::GetDrawWorld()
{
	UpdateDrawWorld();
	return mDrawWorld;
}

const D2D1::Matrix3x2F& World2D::GetTotalDrawWorld()
{
	UpdateTotalDrawWorld();
	return mTotalDrawWorld;
}

void World2D::SetParentWorld(const D2D1::Matrix3x2F* p)
{
	parentWorld = p;
	mGlobalWorldUpdateFlag = true;
}

void World2D::ParentWorldUpdated()
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

void World2D::UpdateDrawWorld()
{
	if (mDrawWorldUpdateFlag)
	{
		mDrawWorld = D2D1::Matrix3x2F::Scale({ drawScale.x,drawScale.y }) * D2D1::Matrix3x2F::Translation({ drawPos.x,drawPos.y });
		mTotalDrawWorldUpdateFlag = true;

		mDrawWorldUpdateFlag = false;
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
			mLocalWorld * (*parentWorld);
		mTotalDrawWorldUpdateFlag = true;
		mGlobalWorldUpdateFlag = false;
	}
}

void World2D::UpdateTotalDrawWorld()
{
	UpdateGlobalWorld();
	UpdateDrawWorld();
	if (mTotalDrawWorldUpdateFlag)
	{
		mTotalDrawWorld = mGlobalWorld * mDrawWorld;
		mTotalDrawWorldUpdateFlag = false;
	}
}

void World2D::SetAlignMode(AlignModeX x, AlignModeY y)
{
	SetAlignX(x);
	SetAlignY(y);
}

void World2D::SetAlignX(AlignModeX x)
{
	alignX = x;
	mDrawWorldUpdateFlag = true;
}

void World2D::SetAlignY(AlignModeY y)
{
	alignY = y;
	mDrawWorldUpdateFlag = true;
}
