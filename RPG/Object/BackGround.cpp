#include "BackGround.h"

BackGround::BackGround(float w, float h, const XMFLOAT4 color) : Sprite(w, h)
{
	Diffuse = color;
	colorMode = true;
}

BackGround::~BackGround()
{
}

void BackGround::Resize(float w, float h)
{
	Scale.x = ShortCut::GetOrthoWidth(w, h);
	UpdateWorld();
}
