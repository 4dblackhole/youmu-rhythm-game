#include "framework.h"

#include "System/D2Ddevice.h"
#include "Utility/ShortCut.h"
#include "DwLayout.h"

using namespace std;
using namespace D2D1;

DwLayout::~DwLayout()
{
	ReleaseCOM(this->layout);
}

void DwLayout::ChangeClipArea(FLOAT rate)
{
	layout->SetMaxWidth(desc.maxW * rate);
	layout->SetMaxHeight(desc.maxH * rate);
}

void DwLayout::ChangeSize(FLOAT size, const DWRITE_TEXT_RANGE sizeRange)
{
	layout->SetFontSize(size, sizeRange);
}

//auto Release
void DwLayout::SetLayout(const std::wstring text, IDWriteTextFormat* textFormat)
{
	ReleaseCOM(this->layout);
	HR(D2D.GetDwFactory()->CreateTextLayout(text.c_str(), (UINT32)text.length(), textFormat, desc.maxW, desc.maxH, &this->layout));
	//resize according to monitor dimensions
	Resize(App->GetWidth(), App->GetHeight());
}

void DwLayout::Draw() const
{
	D2D.DrawTextLayout(desc.DrawPos, layout, D2D.GetSolidBrush(desc.Color), D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

void DwLayout::Resize(float w, float h)
{
	const float rateY = h / (float)StandardHeight;
	ChangeSize(this->desc.FontSize * rateY);
	ChangeClipArea(rateY);
	desc.Resize(w, h, layout);
}

LayoutDesc& LayoutDesc::operator=(const LayoutDesc& l)
{
	Color = (l.Color);
	FontSize = l.FontSize;
	Pos = l.Pos;
	DrawPos = l.DrawPos;
	maxW = l.maxW;
	maxH = l.maxH;
	alignX = l.alignX;
	return *this;
}

LayoutDesc& LayoutDesc::operator=(LayoutDesc&& l) noexcept
{
	Color = (l.Color);
	FontSize = l.FontSize;
	Pos = l.Pos;
	DrawPos = l.DrawPos;
	maxW = l.maxW;
	maxH = l.maxH;
	alignX = l.alignX;
	return *this;
}

void LayoutDesc::Resize(const float w, const float h, IDWriteTextLayout* layout)
{
	switch (alignX)
	{
	case AlignModeX::Left:
	{
		DrawPos = ShortCut::Resize2DtoStandardCS(w, h, (float)Pos.x, (float)Pos.y);
	}	
	break;
	case AlignModeX::Mid:
	{
		DrawPos = ShortCut::Resize2DtoStandardCS(w, h, (float)Pos.x, (float)Pos.y, (float)App->GetWidth() * 0.5f);
	}
	break;
	case AlignModeX::Right:
	{
		DrawPos = ShortCut::Resize2DtoStandardCS(w, h, (float)Pos.x, (float)Pos.y, (float)App->GetWidth());
	}
	break;
	}
}
