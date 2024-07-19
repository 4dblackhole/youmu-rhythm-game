#pragma once
#include "framework.h"

class ClipRect
{
public:
	ClipRect();
	constexpr ClipRect(float x, float y, float w, float h) noexcept;
	~ClipRect() {};

	void Resize();

	const D2D1_RECT_F& Get() const { return rc; }

	D2D1_POINT_2F Pos;
	D2D1_POINT_2F Scale;

	AlignModeX alignX = AlignModeX::Mid;
	AlignModeY alignY = AlignModeY::Mid;

private:

	D2D1_POINT_2F drawPos{ 0.0f,0.0f };
	D2D1_POINT_2F drawScale{ 1.0f,1.0f };

	D2D1_RECT_F rc;
};

