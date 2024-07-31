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
	//Resize((float)App->GetWidth(), (float)App->GetHeight());
}

void DwLayout::GetLayoutMetrics(const std::wstring text, IDWriteTextFormat* textFormat, DWRITE_TEXT_METRICS* out)
{
	IDWriteTextLayout* tempLayout = nullptr;
	HR(D2D.GetDwFactory()->CreateTextLayout(text.c_str(), (UINT32)text.length(), textFormat, 10000.0f, 0.0f, &tempLayout));
	tempLayout->GetMetrics(out);
	ReleaseCOM(tempLayout);
}

// move the base position to the right side of text layout
// for example, if you put 0 on pos.x and set alignX to right, the leftside of the text is located at the right side of window.
// so you should move the text left as the length of text to display it
void DwLayout::SetLayoutRightAlign(const std::wstring text, IDWriteTextFormat* textFormat)
{
	//Get Total Width and Height
	DWRITE_TEXT_METRICS mt;
	GetLayoutMetrics(text, textFormat, &mt);

	float sizeRate = desc.world2d.GetScale().x;
	desc.world2d.SetPosition({ desc.world2d.GetPosition().x - (int)(mt.width * sizeRate),desc.world2d.GetPosition().y });

	SetLayout(text, textFormat);

}

void DwLayout::Draw()
{
	World2D& myWorld = desc.world2d;
	D2D.GetRenderTarget()->SetTransform(myWorld.GetGlobalWorld());
	D2D.DrawTextLayout({0,0}, layout, D2D.GetSolidBrush(desc.Color), D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

void DwLayout::Resize(float w, float h)
{
	desc.Resize(w, h);
	const float rateY = h / (float)StandardHeight;
	//ChangeSize(this->desc.world2d.GetDrawScale().x);
	//ChangeClipArea(rateY);
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
}
