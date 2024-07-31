#pragma once

#include <Windowsx.h>

#include <string>
#include <d2d1_1.h>
#include <dwrite.h>

#include "System/AlignMode.h"
#include "System/World2D.h"

#include "Shape2D.h"

class DwLayout2D : public Shape2D
{
public:
	DwLayout2D();
	virtual ~DwLayout2D();

	void SetLayout(const std::wstring text, IDWriteTextFormat* textFormat);
	void SetLayoutRightAlign(const std::wstring text, IDWriteTextFormat* textFormat);
	static void GetLayoutMetrics(const std::wstring text, IDWriteTextFormat* textFormat, DWRITE_TEXT_METRICS* out);

	void Draw();
	void Resize(float w, float h);

	FLOAT maxW = 500.0f, maxH = 54.0f;
	D2D1::ColorF Color;

	IDWriteTextLayout* layout;

};