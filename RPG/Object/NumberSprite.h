#pragma once
#include "libraries.h"
#include "System/World/World3D.h"

class NumberSprite
{
public:
	static constexpr char ValidCharList[] =
	{ '%', ',' , '.', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	static constexpr std::span validCharSpan = NumberSprite::ValidCharList;

public:
	NumberSprite();
	~NumberSprite();

	void OnResize();

	template <floating_point Flt>
	void UpdateText(const Flt& val)
	{

	}

	template <integral T>
	void UpdateText(const T& val)
	{

	}

	void UpdateText(string str);

	World3D& GetWorld3D() { return world; }
	const World3D& GetWorld3DConst() const { return world; }

private:

	string txt;
	Texture numImgs;

	World3D world;
	vector<Sprite> numSprites;
};