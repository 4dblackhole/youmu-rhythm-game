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

HRESULT DwLayout::ChangeSize(FLOAT size, const DWRITE_TEXT_RANGE sizeRange)
{
	return layout->SetFontSize(size, sizeRange);
}

void DwLayout::CreateLayout(const std::wstring text, IDWriteTextFormat* textFormat)
{
	ReleaseCOM(this->layout);
	HR(D2D.GetDwFactory()->CreateTextLayout(text.c_str(), (UINT32)text.length(), textFormat,desc.maxW, desc.maxH, &this->layout));
	HR(ChangeSize(desc.FontSize));
}

void DwLayout::Draw() const
{
	D2D.DrawTextLayout(desc.DrawPos, layout, D2D.GetSolidBrush(desc.Color));
}

void DwLayout::Resize(const float w, const float h)
{
	const float rateY = h / (float)StandardHeight;
	ChangeSize(this->desc.FontSize * rateY);
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
		DWRITE_TEXT_METRICS mt;
		layout->GetMetrics(&mt);
		DrawPos = ShortCut::Resize2DtoStandardCS(w, h, (float)Pos.x, (float)Pos.y, (float)App->GetWidth() - mt.width);
	}
	break;
	}
}
