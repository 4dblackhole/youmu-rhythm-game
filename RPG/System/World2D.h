#pragma once
#include "AlignMode.h"
#include <D2D1.h>

class World2D
{
public:
	World2D();
	~World2D();

	void Resize(float newW, float newH);

private:
	AlignModeX alignX = AlignModeX::Mid;
	AlignModeY alignY = AlignModeY::Mid;

	D2D1_POINT_2F drawPos{};
	D2D1_POINT_2F scale{ 1.0f,1.0f };
	D2D1_POINT_2F pos{ 0.0f, 0.0f };
};