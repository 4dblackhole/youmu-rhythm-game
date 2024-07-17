#pragma once
#include <Windowsx.h>

#include <string>
#include <d2d1_1.h>
#include <dwrite.h>

#include "System/AlignMode.h"
#include "System/World2D.h"

class LayoutDesc
{
public:
	LayoutDesc() : LayoutDesc(10.0f, D2D1::ColorF::White, {}) {}
	LayoutDesc(float size, D2D1::ColorF color, D2D1_SIZE_F pos) : Color(color), world2d({ size,size }, 0.0f, { pos.width,pos.height }) {}

	LayoutDesc(const LayoutDesc& l)
		: Color(l.Color), world2d(l.world2d)
		, maxW(l.maxW), maxH(l.maxH) {}

	LayoutDesc(LayoutDesc&& l) noexcept 
		: Color(l.Color), world2d(l.world2d)
		, maxW(l.maxW), maxH(l.maxH) {}
	LayoutDesc& operator=(const LayoutDesc& l);
	LayoutDesc& operator=(LayoutDesc&& l) noexcept;

	void Resize(const float rateX, const float rateY);

	FLOAT maxW = 500.0f, maxH = 54.0f;
	D2D1::ColorF Color;

	World2D world2d;
	//POINT Pos; //middle align
	//float FontSize;
	//D2D1_POINT_2F DrawPos{};
	//AlignModeX alignX = AlignModeX::Mid;

private:
	inline D2D1_POINT_2F PtUintToFloat(POINT pt) { return { (float)pt.x, (float)pt.y }; }

};

class DwLayout
{
public:
	DwLayout() : DwLayout(LayoutDesc()) {}
	DwLayout(float size, D2D1::ColorF color, POINT pos) : DwLayout(LayoutDesc()) {}
	DwLayout(const LayoutDesc& l) : layout(nullptr), desc(l) {}
	DwLayout(LayoutDesc&& l) : layout(nullptr), desc(l) {}

	~DwLayout();

	void ChangeClipArea(FLOAT rate);
	void ChangeSize(FLOAT size, const DWRITE_TEXT_RANGE sizeRange = { 0, (UINT32)-1 });
	void SetLayout(const std::wstring text, IDWriteTextFormat* textFormat);
	void SetLayoutRightAlign(const std::wstring text, IDWriteTextFormat* textFormat);
	static void GetLayoutMetrics(const std::wstring text, IDWriteTextFormat* textFormat, DWRITE_TEXT_METRICS* out);

	void Draw() const;
	void Resize(float w, float h);

	LayoutDesc desc;

	IDWriteTextLayout* layout;

};