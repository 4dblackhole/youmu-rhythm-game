#include "framework.h"
#include "ClipRect.h"

ClipRect::ClipRect() : ClipRect(0, 0, 1, 1) {}

constexpr ClipRect::ClipRect(float x, float y, float w, float h) noexcept :
	Pos({ x,y }), Scale({ w,h }), rc({ x - w * 0.5f, y - h * 0.5f, x + w * 0.5f, y + h * 0.5f }) {
	int asdf = 3;
}

void ClipRect::Resize()
{
	D2D1_POINT_2F tempPos{};
	const float localWidth = ShortCut::GetOrthoWidth((float)App->GetWidth(), (float)App->GetHeight());
	switch (alignX)
	{
	case AlignModeX::Left:
		tempPos.x = (Scale.x - localWidth) * 0.5f;
		break;
	case AlignModeX::Right:
		tempPos.x = (localWidth - Scale.x) * 0.5f;
		break;
	}
	switch (alignY)
	{
	case AlignModeY::Top:
		tempPos.y = ((float)StandardHeight - Scale.y) * 0.5f;
		break;
	case AlignModeY::Bottom:
		tempPos.y = (Scale.y - (float)StandardHeight) * 0.5f;
		break;
	}
	tempPos.x += Pos.x;
	tempPos.y += Pos.y;

	drawPos = tempPos;

	const float orthoWidth = ShortCut::GetOrthoWidth((float)App->GetWidth(), (float)App->GetHeight());
	const float centerW = App->GetWidth() * 0.5f;
	const float centerH = App->GetHeight() * 0.5f;
	const float rateX = App->GetWidth() / orthoWidth;
	const float rateY = App->GetHeight() / (float)StandardHeight;

	rc = {
		centerW + (drawPos.x - Scale.x * 0.5f) * rateX, centerH + (drawPos.y + Scale.y * 0.5f) * -rateY,
		centerW + (drawPos.x + Scale.x * 0.5f) * rateX, centerH + (drawPos.y - Scale.y * 0.5f) * -rateY };

}
