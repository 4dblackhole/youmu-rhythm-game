#pragma once
#include "Sprite.h"

class BackGround : public Sprite
{
public:
	BackGround(float w = (float)StandardWidth , float h = (float)StandardHeight, const XMFLOAT4 color = {0.0f, 0.0f, 0.0f, 1.0f});
	virtual ~BackGround();

	void Resize(float w, float h);
};