#pragma once
#include "libraries.h"

struct NoteDrawDesc
{
public:
	XMFLOAT4 color;
	double diameter;
	UINT textureID;
	UINT textureOverlayID;
};
static constexpr NoteDrawDesc defaultNoteDrawDesc{ MyColor4::Blank, 0 };
