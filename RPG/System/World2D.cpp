#include "framework.h"
#include "World2D.h"

World2D::World2D(D2D1_POINT_2F size, FLOAT rot, D2D1_POINT_2F position) 
	: scale(size), rotation(rot), pos(position)
{
	Resize((float)App->GetWidth(), (float)App->GetHeight());
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
	UpdateWorld();
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

void World2D::SetParentWorld(const D2D1::Matrix3x2F& p)
{
	parentWorld = p;
	UpdateGlobalWorld();
}

/*

Update World 式式式式成式式式式> UpdateGlobalWorld -> UpdateChildWorld
				 弛
SetParentWorld 式式戎

*/

void World2D::UpdateWorld()
{
	mWorld = D2D1::Matrix3x2F::Scale({ drawScale.x,drawScale.y }) * D2D1::Matrix3x2F::Rotation(rotation) * D2D1::Matrix3x2F::Translation({ drawPos.x,drawPos.y });
	UpdateGlobalWorld();
}

void World2D::UpdateGlobalWorld()
{
	mGlobalWorld = mWorld * parentWorld;
	UpdateChildWorld();
}

void World2D::UpdateChildWorld()
{
	for (auto& childWorld : childWorlds) childWorld->SetParentWorld(mGlobalWorld);
	
}
