#pragma once
#include <Windowsx.h>

#include <string>
#include <d2d1_1.h>
#include <dwrite.h>

#include "Shape2D/AlignMode.h"

class LayoutDesc
{
public:
	LayoutDesc() : LayoutDesc(10.0f, D2D1::ColorF::White, {}) {}
	LayoutDesc(float size, D2D1::ColorF color, POINT pos) : Color(color), FontSize(size), Pos(pos) {}
	LayoutDesc(const LayoutDesc& l) : Color(l.Color), FontSize(l.FontSize), Pos(l.Pos), alignX(l.alignX)
	{
		TRACE(_T("asdf\n")); }
	LayoutDesc(LayoutDesc&& l) noexcept : Color(l.Color), FontSize(l.FontSize), Pos(l.Pos), alignX(l.alignX) {}

	LayoutDesc& operator=(const LayoutDesc& l);
	LayoutDesc& operator=(LayoutDesc&& l) noexcept;

	void Resize(const float rateX, const float rateY, IDWriteTextLayout* layout);

	D2D1::ColorF Color;
	POINT Pos; //middle align
	FLOAT maxW = 1000.0f, maxH = 0.0f;

	float FontSize;
	D2D1_POINT_2F DrawPos{};

private:
	inline D2D1_POINT_2F PtUintToFloat(POINT pt) { return { (float)pt.x, (float)pt.y }; }

public:
	AlignModeX alignX = AlignModeX::Mid;
};

class DwLayout
{
public:
	DwLayout() : DwLayout(LayoutDesc()) {}
	DwLayout(float size, D2D1::ColorF color, POINT pos) : DwLayout(LayoutDesc()) {}
	DwLayout(const LayoutDesc& l) : layout(nullptr), desc(l) {}
	DwLayout(LayoutDesc&& l) : layout(nullptr), desc(l) {}

	~DwLayout();

	HRESULT ChangeSize(FLOAT size, const DWRITE_TEXT_RANGE sizeRange = { 0, (UINT32)-1 });
	void CreateLayout(const std::wstring text, IDWriteTextFormat* textFormat);
	void Draw() const;
	void Resize(const float w, const float h);

	LayoutDesc desc;

private:
	IDWriteTextLayout* layout;


};