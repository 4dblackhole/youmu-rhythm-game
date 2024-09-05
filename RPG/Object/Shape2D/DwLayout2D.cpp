#include "framework.h"
#include "DwLayout2D.h"

DwLayout2D::DwLayout2D(float size, D2D1::ColorF color, D2D1_SIZE_F pos)
	: Shape2D(),
	Color(color), layout(nullptr)
{
	GetWorld2d().SetScale(size);
}

DwLayout2D::~DwLayout2D()
{
	ReleaseCOM(this->layout);
}

void DwLayout2D::SetLayout(const std::wstring text, IDWriteTextFormat* textFormat)
{
	ReleaseCOM(this->layout);
	HR(D2D.GetDwFactory()->CreateTextLayout(text.c_str(), (UINT32)text.length(), textFormat, maxW, maxH, &this->layout));

}

void DwLayout2D::SetLayoutRightAlign(const std::wstring text, IDWriteTextFormat* textFormat)
{
	//Get Total Width and Height
	DWRITE_TEXT_METRICS mt;
	GetLayoutMetrics(text, textFormat, &mt);

	float sizeRate = GetWorld2d().GetScale().x;
	GetWorld2d().SetPosition({ GetWorld2d().GetPosition().x - (int)(mt.width * sizeRate),GetWorld2d().GetPosition().y });

	SetLayout(text, textFormat);
}

void DwLayout2D::GetLayoutMetrics(const std::wstring text, IDWriteTextFormat* textFormat, DWRITE_TEXT_METRICS* out)
{
	IDWriteTextLayout* tempLayout = nullptr;
	HR(D2D.GetDwFactory()->CreateTextLayout(text.c_str(), (UINT32)text.length(), textFormat, 10000.0f, 0.0f, &tempLayout));
	tempLayout->GetMetrics(out);
	ReleaseCOM(tempLayout);


}

void DwLayout2D::Draw()
{
	__super::Draw();
	D2D.DrawTextLayout({ 0,0 }, layout, D2D.GetSolidBrush(Color), D2D1_DRAW_TEXT_OPTIONS_CLIP);
}
