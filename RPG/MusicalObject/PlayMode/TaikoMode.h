#pragma once
#include "libraries.h"
#include "PlayMode.h"

class TaikoMode : public PlayMode
{
public:
	TaikoMode();
	virtual ~TaikoMode();

	virtual void InitTextures();

private:
	DECLARE_VARIABLE_WSTRING(LeftD);
	DECLARE_VARIABLE_WSTRING(RightD);
	DECLARE_VARIABLE_WSTRING(LeftK);
	DECLARE_VARIABLE_WSTRING(RightK);
};