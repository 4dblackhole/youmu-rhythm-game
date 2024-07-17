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
	Resize((float)App->GetWidth(), (float)App->GetHeight());
}

void DwLayout::GetLayoutMetrics(const std::wstring text, IDWriteTextFormat* textFormat, DWRITE_TEXT_METRICS* out)
{
	IDWriteTextLayout* tempLayout = nullptr;
	HR(D2D.GetDwFactory()->CreateTextLayout(text.c_str(), (UINT32)text.length(), textFormat, 10000.0f, 0.0f, &tempLayout));
	tempLayout->GetMetrics(out);
	ReleaseCOM(tempLayout);
}

void DwLayout::SetLayoutRightAlign(const std::wstring text, IDWriteTextFormat* textFormat)
{
	//Get Total Width and Height
	DWRITE_TEXT_METRICS mt;
	GetLayoutMetrics(text, textFormat, &mt);

	float sizeRate = desc.world2d.GetScale().x / textFormat->GetFontSize();
	desc.world2d.SetPosition({ desc.world2d.GetPosition().x - (int)(mt.width * sizeRate),desc.world2d.GetPosition().y });

	SetLayout(text, textFormat);

}

void DwLayout::Draw() const
{
	D2D.GetRenderTarget()->SetTransform(Matrix3x2F::Identity());
	D2D.DrawTextLayout(desc.world2d.GetDrawPos(), layout, D2D.GetSolidBrush(desc.Color), D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

void DwLayout::Resize(float w, float h)
{
	desc.Resize(w, h);
	const float rateY = h / (float)StandardHeight;
	ChangeSize(this->desc.world2d.GetDrawScale().x);
	ChangeClipArea(rateY);
}

LayoutDesc& LayoutDesc::operator=(const LayoutDesc& l)
{
	Color = (l.Color);
	world2d = l.world2d;
	maxW = l.maxW;
	maxH = l.maxH;
	return *this;
}

LayoutDesc& LayoutDesc::operator=(LayoutDesc&& l) noexcept
{
	Color = (l.Color); 
	world2d = l.world2d;
	maxW = l.maxW;
	maxH = l.maxH;
	return *this;
}

void LayoutDesc::Resize(const float w, const float h)
{
	world2d.Resize(w, h);
	/*
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
	*/
}
