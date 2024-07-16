#include "framework.h"
#include "World2D.h"

World2D::World2D()
{
}

World2D::~World2D()
{
}

void World2D::Resize(float newW, float newH)
{
	const float rateY = newH / (float)StandardHeight;
	scale.x *= rateY;
	scale.y *= rateY;
	switch (alignX)
	{
	case AlignModeX::Left:
	{
		drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, pos.x, pos.y);
	}
	break;
	case AlignModeX::Mid:
	{
		drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, pos.x, pos.y, (float)App->GetWidth() * 0.5f);
	}
	break;
	case AlignModeX::Right:
	{
		drawPos = ShortCut::Resize2DtoStandardCS(newW, newH, pos.x, pos.y, (float)App->GetWidth());
	}
	break;
	}
}