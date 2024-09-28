#pragma once

#include <Windowsx.h>

#include <string>
#include <d2d1_1.h>
#include <dwrite.h>

#include "System/AlignMode.h"
#include "System/World/World2D.h"

#include "Shape2D.h"

class DwLayout2D : public Shape2D
{
public:
	DwLayout2D(float size = 1.0f, D2D1::ColorF color = D2D1::ColorF::White, D2D1_POINT_2F pos = {});
	virtual ~DwLayout2D();

	void SetLayout(const std::wstring text, IDWriteTextFormat* textFormat);
	void SetLayoutRightAlign(const std::wstring text, IDWriteTextFormat* textFormat);
	static void GetLayoutMetrics(const std::wstring text, IDWriteTextFormat* textFormat, DWRITE_TEXT_METRICS* out);

	virtual void Draw();

	FLOAT maxW = 500.0f, maxH = 54.0f;
	D2D1::ColorF Color;

	IDWriteTextLayout* layout;

};