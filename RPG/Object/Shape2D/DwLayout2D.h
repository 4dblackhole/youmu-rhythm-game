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

	void SetLayout(const std::wstring& text, IDWriteTextFormat* textFormat = nullptr);
	void SetLayoutRightAlign(const std::wstring& text, IDWriteTextFormat* textFormat = nullptr);
	static void GetLayoutMetrics(const std::wstring& text, IDWriteTextFormat* textFormat, DWRITE_TEXT_METRICS* out);

	void MoveLayoutForRightAlign(const std::wstring& text, IDWriteTextFormat* textFormat = nullptr);

	void SetText(const wstring& str);
	const wstring& GetText() { return text; }

	void SetTextFormat(IDWriteTextFormat* ptr);
	const IDWriteTextFormat* GetTextFormat() { return textFormat; }

	virtual void Draw();

	FLOAT maxW = 500.0f, maxH = 54.0f;
	D2D1::ColorF Color;

private:
	void UpdateLayout();

	IDWriteTextLayout* layout;
	bool textUpdateFlag = true;
	wstring text;
	IDWriteTextFormat* textFormat = nullptr;

	void DrawTextLayout(D2D1_POINT_2F origin,
		_In_ IDWriteTextLayout* textLayout,
		_In_ ID2D1Brush* defaultFillBrush,
		_In_opt_ D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NONE) noexcept
	{
		D2D.GetRenderTarget()->DrawTextLayout(origin, textLayout, defaultFillBrush, options);
	}
};
